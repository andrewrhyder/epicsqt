/*
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2014
 *
 *  Author:
 *    Initial code copied by Andrew Rhyder from parts of ffmpegWidget.cpp (Author anonymous, part of EPICS area detector ffmpegViwer project)
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */



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
    this->mutex = new QMutex();
    this->refs = 0;
    this->pFrame = avcodec_alloc_frame();
    this->mem = (unsigned char *) calloc(MAXWIDTH*MAXHEIGHT*3, sizeof(unsigned char));
}

FFBuffer::~FFBuffer() {
    av_free(this->pFrame);
    free(this->mem);
}

bool FFBuffer::grabFree() {
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
    this->mutex->lock();
    this->refs += 1;
    this->mutex->unlock();    
}

void FFBuffer::release() {
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
    // this is the url to read the stream from
    strcpy(this->url, url.toLatin1().data());
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
}

// run the FFThread
void FFThread::run()
{
    AVFormatContext     *pFormatCtx = NULL;
    int                 videoStream;
    AVCodecContext      *pCodecCtx;
    AVCodec             *pCodec;
    AVPacket            packet;
    int                 frameFinished, len;

    // Open video file
    if (avformat_open_input(&pFormatCtx, this->url, NULL, NULL)!=0) {
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
    if(avcodec_open2(pCodecCtx, pCodec, NULL)<0) {
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
//        pCodecCtx->internal_buffer = raw->mem;

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
//        pCodecCtx->internal_buffer = NULL;
        
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
    ms = msIn;
}

mpegSourceObject::~mpegSourceObject()
{

}

void mpegSourceObject::sentAboutToQuit()
{
    emit aboutToQuit();
}



mpegSource::mpegSource()
{
    ff = NULL;
    rawbuf = NULL;
    fullbuf = NULL;
    ctx = NULL;
    mso = new mpegSourceObject( this );

    buff = NULL;
    buffSize=0;
}

mpegSource::~mpegSource()
{
    // Ensure the thread is dead
    ffQuit();
}

QString mpegSource::getURL()
{
    return url;
}

void mpegSource::setURL( QString urlIn )
{
    // don't do anything if URL is not changing
    if( urlIn == url )
    {
        return;
    }
    ffQuit();

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
    ms->updateImage( newbuf );
}

void mpegSource::updateImage(FFBuffer *newbuf) {

    // store the buffer
    if (this->rawbuf) this->rawbuf->release();
    this->rawbuf = newbuf;

    // make the frame the right format
    makeFullFrame();

    if ( fullbuf )
    {
        newbuf->reserve();

        // Ensure an adequate buffer to hold the image data with no line gaps is allocated.
        // (re)allocate if not present of not the right size
        int newBuffSize = fullbuf->width * fullbuf->height;
        if( buffSize != newBuffSize )
        {
            // Free last buffer, if re-allocating
            if( buff )
            {
                free( buff );
            }

            // Allocate buffer
            buffSize = newBuffSize;
            buff = (char*)malloc( newBuffSize );
        }

        // Populate buffer with no line gaps
        // (Each horizontal line of pixels in in a larger horizontal line of storage.
        //  Observed example: each line was 1624 pixels stored in 1664 bytes with
        //  trailing 40 bytes of value 128 before start of pixel on next line)
        char* buffPtr = buff;
        const char* linePtr = (const char*)(newbuf->pFrame->data[0]);
        for( int i = 0; i < fullbuf->height; i++ )
        {
            memcpy( buffPtr, linePtr, fullbuf->width );
            buffPtr += fullbuf->width;
            linePtr += newbuf->pFrame->linesize[0];// !!! Why is fullbuf->pFrame->linesize[0] not the same as newbuf->pFrame->linesize[0]???
        }

        // Deliver image update
        QByteArray ba;
#if QT_VERSION >= 0x040700
        ba.setRawData( (const char*)(buff), buffSize );
#else
        ba = QByteArray::fromRawData( buff, buffSize );
#endif
        //!!!!!! Can still crash here on deletionof QEImage
        setImage( ba, 1, newbuf->width, newbuf->height );

        // Unlock buffer
        newbuf->release();
    }
}

void mpegSource::makeFullFrame() {
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
    FFBuffer *dest = findFreeBuffer(outbuffers);
    // make sure we got a buffer
    if (dest == NULL) return NULL;

    // fill in multiples of 8 that we can cope with
    dest->width = src->width - src->width % 8;
    dest->height = src->height - src->height % 2;
    dest->pix_fmt = pix_fmt;
    // see if we have a suitable cached context
    // note that we use the original values of width and height
//    this->ctx = sws_getCachedContext(this->ctx,
//        dest->width, dest->height, src->pix_fmt,
//        dest->width, dest->height, dest->pix_fmt,
//        SWS_BICUBIC, NULL, NULL, NULL);
    // Assign appropriate parts of buffer->mem to planes in buffer->pFrame
//    avpicture_fill((AVPicture *) dest->pFrame, dest->mem,
//        dest->pix_fmt, dest->width, dest->height);
    // do the software scale
//    sws_scale(this->ctx, src->pFrame->data, src->pFrame->linesize, 0,
//        src->height, dest->pFrame->data, dest->pFrame->linesize);
    return dest;
}

