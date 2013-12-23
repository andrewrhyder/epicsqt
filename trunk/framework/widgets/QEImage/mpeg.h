#ifndef MPEG_H
#define MPEG_H

#include <QThread>
#include <QWidget>
#include <QMutex>
//#include <QTime>
//#include <QTimer>
//#include <X11/Xlib.h>
//#include <X11/extensions/Xvlib.h>

/* ffmpeg includes */
extern "C" {
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/avutil.h"
}

// max width of any input image
#define MAXWIDTH 4000
// max height of any input image
#define MAXHEIGHT 3000
// number of MAXWIDTH*MAXHEIGHT*3 buffers to create
#define NBUFFERS 20
// number of frames to calc fps from
#define MAXTICKS 10
// size of URL string
#define MAXSTRING 1024

class FFBuffer
{
public:
    FFBuffer ();
    ~FFBuffer ();
	void reserve();
	void release();
	bool grabFree();
    QMutex *mutex;
    unsigned char *mem;
    AVFrame *pFrame;
    PixelFormat pix_fmt;
    int width;
    int height;
    int refs;
};

class FFThread : public QThread
{
    Q_OBJECT

public:
    FFThread (const QString &url, QObject* parent);
    ~FFThread ();
    void run();

public slots:
    void stopGracefully() { stopping = 1; }

signals:
    void updateSignal(FFBuffer * buf);

private:
    char url[MAXSTRING];
    int stopping;
};

class mpegSource;

class mpegSourceObject : public QObject
{
    Q_OBJECT
public:
    mpegSourceObject( mpegSource* msIn );
    ~mpegSourceObject();

    void sentAboutToQuit();

public slots:
    void updateImage(FFBuffer *buf);

signals:
    void aboutToQuit();

private:
    mpegSource* ms;
};

class mpegSource
{
public:
    mpegSource();
    ~mpegSource();

    void updateImage(FFBuffer *buf);

protected:
    QString getURL();
    void setURL( QString urlIn );

private:
    void ffQuit();

    mpegSourceObject* mso;
    QString url;
    FFThread* ff;
    virtual void setImage( const QByteArray& imageIn, unsigned long dataSize, unsigned long width, unsigned long height ) = 0;

    FFBuffer* rawbuf;
    FFBuffer* fullbuf;
    void makeFullFrame();
    FFBuffer* formatFrame(FFBuffer *src, PixelFormat pix_fmt);
    struct SwsContext *ctx;

};

#endif // MPEG_H
