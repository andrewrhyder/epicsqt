#include <QtDebug>
//#include <QToolTip>
#include "mpeg.h"
//#include <QColorDialog>
//#include "colorMaps.h"
//#include <QX11Info>
//#include <assert.h>
//#include <QImage>
//#include <QPainter>

#include <QByteArray>

/* global switch for fallback mode */
int fallback = 0;

/* set this when the ffmpeg lib is initialised */
static int ffinit=0;

/* need this to protect certain ffmpeg functions */
static QMutex *ffmutex;

// An FFBuffer contains an AVFrame, a mutex for access and some data
FFBuffer::FFBuffer() {
    qDebug() << "FFBuffer::FFBuffer()";
    this->mutex = new QMutex();
    this->refs = 0;
    this->pFrame = avcodec_alloc_frame();
    this->mem = (unsigned char *) calloc(MAXWIDTH*MAXHEIGHT*3, sizeof(unsigned char));
}

FFBuffer::~FFBuffer() {
    qDebug() << "FFBuffer::~FFBuffer()";
    av_free(this->pFrame);
    free(this->mem);
}

bool FFBuffer::grabFree() {
    qDebug() << "FFBuffer::grabFree()";
    if (this->mutex->tryLock()) {
        if (this->refs == 0) {
            this->refs += 1;
            this->mutex->unlock();    
            return true;
        } else {
            this->mutex->unlock();    
            return false;
        }
    } else {
        return false;
    }
}

void FFBuffer::reserve() {
    qDebug() << "FFBuffer::reserve()";
    this->mutex->lock();
    this->refs += 1;
    this->mutex->unlock();    
}

void FFBuffer::release() {
    qDebug() << "FFBuffer::release()";
    this->mutex->lock();
    this->refs -= 1;
    this->mutex->unlock();    
}

// List of FFBuffers to use for raw frames
static FFBuffer rawbuffers[NBUFFERS];

// List of FFBuffers to use for uncompressed frames
static FFBuffer outbuffers[NBUFFERS];

// find a free FFBuffer
FFBuffer * findFreeBuffer(FFBuffer* source) {
    qDebug() << "findFreeBuffer";
    for (int i = 0; i < NBUFFERS; i++) {
        // if we can lock it and it has a 0 refcount, we can use it!
        if (source[i].mutex->tryLock()) {
            if (source[i].refs == 0) {
                source[i].refs += 1;
                source[i].mutex->unlock();    
                return &source[i];
            } else {
                source[i].mutex->unlock();    
            }
        }
    }
    return NULL;
}

/* thread that decodes frames from video stream and emits updateSignal when
 * each new frame is available
 */
FFThread::FFThread (const QString &url, QObject* parent)
    : QThread (parent)
{
    qDebug() << "FFThread::FFThread";
    // this is the url to read the stream from
    strcpy(this->url, url.toAscii().data());
    // set this to 1 to finish
    this->stopping = 0;
    // initialise the ffmpeg library once only
    if (ffinit==0) {
        ffinit = 1;
        // init mutext
        ffmutex = new QMutex();
        // only display errors
        av_log_set_level(AV_LOG_ERROR);
        // Register all formats and codecs
        av_register_all();
    }
}

// destroy widget
FFThread::~FFThread() {
    qDebug() << "FFThread::~FFThread()";
}

// run the FFThread
void FFThread::run()
{
    qDebug() << "FFThread::run()";
    AVFormatContext     *pFormatCtx;
    int                 videoStream;
    AVCodecContext      *pCodecCtx;
    AVCodec             *pCodec;
    AVPacket            packet;
    int                 frameFinished, len;

    // Open video file
    if (av_open_input_file(&pFormatCtx, this->url, NULL, 0, NULL)!=0) {
        printf("Opening input '%s' failed\n", this->url);
        return;
    }

    // Find the first video stream
    videoStream=-1;
    for (unsigned int i=0; i<pFormatCtx->nb_streams; i++) {
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
            videoStream=i;
            break;
        }
    }
    if( videoStream==-1) {
        printf("Finding video stream in '%s' failed\n", this->url);
        return;
    }

    // Get a pointer to the codec context for the video stream
    pCodecCtx=pFormatCtx->streams[videoStream]->codec;

    // Find the decoder for the video stream
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL) {
        printf("Could not find decoder for '%s'\n", this->url);
        return;
    }

    // Open codec
    ffmutex->lock();
    if(avcodec_open(pCodecCtx, pCodec)<0) {
        printf("Could not open codec for '%s'\n", this->url);
        return;
    }
    ffmutex->unlock();

    // read frames into the packets
    while (stopping !=1 && av_read_frame(pFormatCtx, &packet) >= 0) {

        // Is this a packet from the video stream?
        if (packet.stream_index!=videoStream) {
            // Free the packet if not
            printf("Non video packet. Shouldn't see this...\n");
            av_free_packet(&packet);
            continue;
        }

        // grab a buffer to decode into
        FFBuffer *raw = findFreeBuffer(rawbuffers);        
        if (raw == NULL) {
            printf("Couldn't get a free buffer, skipping packet\n");
            av_free_packet(&packet);
            continue;
        }
        
        // Tell the codec to use this bit of memory
        pCodecCtx->internal_buffer = raw->mem;

        // Decode video frame
        len = avcodec_decode_video2(pCodecCtx, raw->pFrame, &frameFinished,
            &packet);
        if (!frameFinished) {
            printf("Frame not finished. Shouldn't see this...\n");
            av_free_packet(&packet);
            raw->release();
            continue;
        }
        
        // Set the internal buffer back to null so that we don't accidentally free it
        pCodecCtx->internal_buffer = NULL;
        
        // Fill in the output buffer
        raw->pix_fmt = pCodecCtx->pix_fmt;         
        raw->height = pCodecCtx->height;
        raw->width = pCodecCtx->width;                

        // Emit and free
        emit updateSignal(raw);
        av_free_packet(&packet);
    }
    // tidy up
    ffmutex->lock();
    avcodec_close(pCodecCtx);
    av_close_input_file(pFormatCtx);
    pCodecCtx = NULL;
    pFormatCtx = NULL;
    ffmutex->unlock();
}







mpegSourceObject::mpegSourceObject( mpegSource* msIn )
{
    qDebug() << "mpegSourceObject::mpegSourceObject(";
    ms = msIn;
}

mpegSourceObject::~mpegSourceObject()
{
    qDebug() << "mpegSourceObject::~mpegSourceObject()";

}

void mpegSourceObject::sentAboutToQuit()
{
    qDebug() << "mpegSourceObject::sentAboutToQuit()";
    emit aboutToQuit();
}



mpegSource::mpegSource()
{
    qDebug() << "mpegSource::mpegSource()";
    ff = NULL;
    rawbuf = NULL;
    fullbuf = NULL;
    ctx = NULL;
    mso = new mpegSourceObject( this );


}

mpegSource::~mpegSource()
{
    qDebug() << "mpegSource::~mpegSource()";
}

QString mpegSource::getURL()
{
    qDebug() << "mpegSource::getURL()";
    return url;
}

void mpegSource::setURL( QString urlIn )
{
    ffQuit();

    qDebug() << "mpegSource::setURL";
    url = urlIn;

    /* create the ffmpeg thread */
    ff = new FFThread( url, mso );

    QObject::connect( ff, SIGNAL(updateSignal(FFBuffer *)),
                      mso, SLOT(updateImage(FFBuffer *)) );
    QObject::connect( mso, SIGNAL(aboutToQuit()),
                      ff, SLOT(stopGracefully()) );

    ff->start();
}

void mpegSource::ffQuit() {
    qDebug() << "mpegSource::ffQuit()";
    // Tell the ff thread to stop
    if (ff==NULL) return;
    mso->sentAboutToQuit();
    if (!ff->wait(500)) {
        // thread won't stop, kill it
        ff->terminate();
        ff->wait(100);
    }
    delete ff;
    ff = NULL;
}

void mpegSourceObject::updateImage(FFBuffer *newbuf)
{
    qDebug() << "mpegSourceObject::updateImage";
    ms->updateImage( newbuf );
}

void mpegSource::updateImage(FFBuffer *newbuf) {
    qDebug() << "mpegSource::updateImage";

    // store the buffer
    if (this->rawbuf) this->rawbuf->release();
    this->rawbuf = newbuf;

    // make the frame the right format
    makeFullFrame();

    if ( fullbuf )
    {
        QByteArray ba;

        newbuf->reserve();
//for( int i = 0; i < 100; i++ )
//{
//    qDebug() << newbuf->pFrame->data[0][i];
//    qDebug() << newbuf->pFrame->data[1][i];
//    qDebug() << newbuf->pFrame->data[2][i];
//}
#if QT_VERSION >= 0x040700
        ba.setRawData( (const char*)(newbuf->pFrame->data[0]), fullbuf->width * fullbuf->height );
#else
        ba = QByteArray::fromRawData( fullbuf->pFrame->data[0], fullbuf->width * fullbuf->height );
#endif
qDebug() <<  newbuf->width << newbuf->height;
        setImage( ba, 1, newbuf->width, newbuf->height );
        newbuf->release();
    }
}

void mpegSource::makeFullFrame() {
    qDebug() << "mpegSource::makeFullFrame()";
    PixelFormat pix_fmt;

    // make sure we have a raw buffer
    if (this->rawbuf == NULL || this->rawbuf->width <= 0 || this->rawbuf->height <= 0) {
        return;
    }

    // if we have a raw buffer then decompress it
    this->rawbuf->reserve();

    // if we've got an image that's too big, force RGB
//    if (this->ff_fmt == PIX_FMT_YUVJ420P && (this->rawbuf->width > maxW || this->rawbuf->height > maxH)) {
//        printf("Image too big, using QImage fallback mode\n");
        pix_fmt = PIX_FMT_RGB24;
//    } else {
//        pix_fmt = this->ff_fmt;
//    }

    // release any full frame we might have
    if (this->fullbuf) this->fullbuf->release();

//    // Format the decoded frame as we've been asked
//    if (_fcol) {
//        // make it false colour
//        this->fullbuf = this->falseFrame(this->rawbuf, pix_fmt);
//    } else {
//        // pass out frame through sw_scale
        this->fullbuf = this->formatFrame(this->rawbuf, pix_fmt);
//    }
    this->rawbuf->release();

    // Check we got a buffer
    if (this->fullbuf == NULL) {
        printf("Couldn't get a free buffer, skipping frame\n");
        return;
    }
}


// take a buffer and swscale it to the requested dimensions
FFBuffer * mpegSource::formatFrame(FFBuffer *src, PixelFormat pix_fmt) {
    qDebug() << "mpegSource::formatFrame";
    FFBuffer *dest = findFreeBuffer(outbuffers);
    // make sure we got a buffer
    if (dest == NULL) return NULL;
    // fill in multiples of 8 that we can cope with
    dest->width = src->width - src->width % 8;
    dest->height = src->height - src->height % 2;
    dest->pix_fmt = pix_fmt;
    // see if we have a suitable cached context
    // note that we use the original values of width and height
    this->ctx = sws_getCachedContext(this->ctx,
        dest->width, dest->height, src->pix_fmt,
        dest->width, dest->height, dest->pix_fmt,
        SWS_BICUBIC, NULL, NULL, NULL);
    // Assign appropriate parts of buffer->mem to planes in buffer->pFrame
    qDebug() << "size of image data" << avpicture_fill((AVPicture *) dest->pFrame, dest->mem,
        dest->pix_fmt, dest->width, dest->height);

    // do the software scale
    qDebug() << "height of slice" << sws_scale(this->ctx, src->pFrame->data, src->pFrame->linesize, 0,
        src->height, dest->pFrame->data, dest->pFrame->linesize);
    return dest;
}

