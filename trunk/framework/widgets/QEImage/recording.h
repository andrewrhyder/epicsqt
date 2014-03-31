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

class historicImage
{
public:
    historicImage( QByteArray image, unsigned long dataSize, QCaAlarmInfo& alarmInfo, QCaDateTime& time );

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
    recording* recorder;
    void timerEvent( QTimerEvent * event );
};

class recording : public QWidget
{
    Q_OBJECT

public:
    explicit recording( QObject* target, QWidget *parent = 0 );
    ~recording();

    int getLimit();
    void setLimit( int limit );

    void reset();

    bool isRecording();
    void recordImage( QByteArray image, unsigned long dataSize, QCaAlarmInfo& alarmInfo, QCaDateTime& time );

    void nextFrameDue();

private:

    void enableControls();
    void startPlaying();
    void stopPlaying();

    playbackTimer timer;

    Ui::recording *ui;

    QList<historicImage> history;

signals:
  void byteArrayChanged( const QByteArray& value, unsigned long dataSize, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp, const unsigned int& variableIndex );

private slots:
    void on_pushButtonPlay_toggled(bool checked);
    void on_pushButtonRecord_toggled(bool checked);
};

#endif // RECORDING_H
