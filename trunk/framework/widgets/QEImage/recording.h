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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

/*
 This class manages image recording and playback for the QEImage widget
 */

#ifndef RECORDING_H
#define RECORDING_H

#include <QWidget>
#include <QList>
#include <QTimer>

#include <QByteArray>
#include <QCaAlarmInfo.h>
#include <QCaDateTime.h>
//#include <QDebug>

class historicImage
{
public:
    historicImage( QByteArray image, unsigned long dataSize, QCaAlarmInfo& alarmInfo, QCaDateTime& time );

//    historicImage(historicImage* other ){ qDebug() << "historicImage(historicImage* other )"; image = other->image;
//                                          dataSize = other->dataSize;
//                                          alarmInfo = other->alarmInfo;
//                                          time = other->time; }
    QByteArray image;
    unsigned long dataSize;
    QCaAlarmInfo alarmInfo;
    QCaDateTime time;
};

namespace Ui {
    class recording;
}

class recording;

class playbackTimer : public QTimer
{
    Q_OBJECT
public:
    playbackTimer( recording* recorderIn ){ recorder = recorderIn; }
    recording* recorder;
    void timerEvent( QTimerEvent * event );
};

class recording : public QWidget
{
    Q_OBJECT

public:
    explicit recording( QWidget *parent = 0 );
    ~recording();

    int getLimit();
    void setLimit( int limit );

    void reset();

    bool isRecording();
    void recordImage( QByteArray image, unsigned long dataSize, QCaAlarmInfo& alarmInfo, QCaDateTime& time );

    void nextFrameDue();

private:

    void startPlaying();
    void stopPlaying();
    void showRecordedFrame( int currentFrame );

    void stopRecording();


    playbackTimer* timer;

    Ui::recording *ui;

    QList<historicImage> history;

signals:
  void byteArrayChanged( const QByteArray& value, unsigned long dataSize, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp, const unsigned int& variableIndex );
  void playingBack( bool playing );

private slots:
    void on_pushButtonPlay_toggled(bool checked);
    void on_pushButtonRecord_toggled(bool checked);
    void on_pushButtonClear_clicked();

//    void on_pushButtonPause_toggled(bool checked);

    void on_pushButtonLastImage_clicked();
    void on_pushButtonNextImage_clicked();
    void on_pushButtonFirstImage_clicked();
    void on_pushButtonPreviousImage_clicked();
    void on_horizontalSliderPosition_valueChanged(int value);
    void on_radioButtonLive_toggled(bool checked);
};

#endif // RECORDING_H
