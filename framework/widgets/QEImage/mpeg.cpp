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
        qDebug() << QString( "Opening input '%1' failed" ).arg( url );
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
        qDebug() << QString( "Finding video stream in '%1' failed" ).arg( url );
        return;
    }

    // Get a pointer to the codec context for the video stream
    pCodecCtx=pFormatCtx->streams[videoStream]->codec;

    // Find the decoder for the video stream
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL) {
        qDebug() << QString( "Could not find decoder for '%1'" ).arg( url );
        return;
    }

    // Open codec
    ffmutex->lock();
    if(avcodec_open2(pCodecCtx, pCodec, NULL)<0) {
        qDebug() << QString( "Could not open codec for '%1'" ).arg( url );
        return;
    }
    ffmutex->unlock();

    // read frames into the packets
    while (stopping !=1 && av_read_frame(pFormatCtx, &packet) >= 0) {

        // Is this a packet from the video stream?
        if (packet.stream_index!=videoStream) {
            // Free the packet if not
            qDebug() << "Non video packet. Shouldn't see this...";
            av_free_packet(&packet);
            continue;
        }

        // grab a buffer to decode into
        FFBuffer *raw = findFreeBuffer(rawbuffers);        
        if (raw == NULL) {
            qDebug() << "Couldn't get a free buffer, skipping packet";
            av_free_packet(&packet);
            continue;
        }
        
        // Decode video frame
        len = avcodec_decode_video2(pCodecCtx, raw->pFrame, &frameFinished,
            &packet);
        if (!frameFinished) {
            qDebug() << "Frame not finished. Shouldn't see this...";
            av_free_packet(&packet);
            raw->release();
            continue;
        }
        
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
    newbuf->release();
}

void mpegSource::updateImage(FFBuffer *newbuf) {

    newbuf->reserve();

    // Ensure an adequate buffer to hold the image data with no line gaps is allocated.
    // (re)allocate if not present of not the right size
    int newBuffSize = newbuf->width * newbuf->height;
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
    for( int i = 0; i < newbuf->height; i++ )
    {
        memcpy( buffPtr, linePtr, newbuf->width );
        buffPtr += newbuf->width;
        linePtr += newbuf->pFrame->linesize[0];// !!! Why is fullbuf->pFrame->linesize[0] not the same as newbuf->pFrame->linesize[0]???
    }

    // Deliver image update
    QByteArray ba;
#if QT_VERSION >= 0x040700
    ba.setRawData( (const char*)(buff), buffSize );
#else
    ba = QByteArray::fromRawData( buff, buffSize );
#endif
    setImage( ba, 1, newbuf->width, newbuf->height );

    // Unlock buffer
    newbuf->release();
}
