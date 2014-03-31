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

#include "recording.h"
#include "ui_recording.h"
#include <QDebug>

recording::recording( QObject* target, QWidget* parent ) :
    QWidget(parent),
    ui(new Ui::recording)
{
    ui->setupUi(this);

    QObject::connect( this, SIGNAL( byteArrayChanged( const QByteArray&, unsigned long, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                      target, SLOT( setImage( const QByteArray&, unsigned long, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );

    // Initialise
    reset();

}

recording::~recording()
{
    delete ui;
}

void recording::reset()
{
    ui->doubleSpinBoxPlaybackRate->setValue( 1.0 );
    ui->horizontalSliderPosition->setValue( 0 );
    setLimit( 20 );
    enableControls();
}

int recording::getLimit()
{
    return ui->spinBoxMaxImages->value();
}

void recording::setLimit( int limit )
{
    ui->spinBoxMaxImages->setValue( limit );
}

bool recording::isRecording()
{
    return ui->pushButtonRecord->isChecked();
}

void recording::recordImage( QByteArray image, unsigned long dataSize, QCaAlarmInfo& alarmInfo, QCaDateTime& time )
{
    if( history.count() >= getLimit() )
    {
        history.removeFirst();
    }

    history.append( historicImage( image, dataSize, alarmInfo, time ) );

    ui->horizontalSliderPosition->setMaximum( history.count() );
    ui->horizontalSliderPosition->setValue( history.count() );
    qDebug() << "added image" << history.count();
}

void recording::on_pushButtonRecord_toggled( bool checked )
{
    if( checked )
    {
//        ui->pushButtonRecord->setIcon( stopIcon );
        ui->pushButtonRecord->setText( "Stop" );
    }
    else
    {
//        ui->pushButtonRecord->setIcon( recordIcon );
        ui->pushButtonRecord->setText( "Record" );
    }

    enableControls();
}

void recording::enableControls()
{
    // playback controls should be enabled if not recording, and if history is available
    bool playback = ( !ui->pushButtonRecord->isChecked() && history.count() );

    ui->pushButtonPlay            ->setEnabled( playback );
    ui->doubleSpinBoxPlaybackRate ->setEnabled( playback );
    ui->pushButtonLoop            ->setEnabled( playback );
    ui->pushButtonFirstImage      ->setEnabled( playback );
    ui->pushButtonPreviousImage   ->setEnabled( playback );
    ui->pushButtonNextImage       ->setEnabled( playback );
    ui->pushButtonLastImage       ->setEnabled( playback );
    ui->horizontalSliderPosition  ->setEnabled( playback );
}

void recording::on_pushButtonPlay_toggled(bool checked)
{
    if( checked )
    {
//        ui->pushButtonPlay->setIcon( pauseIcon );
        ui->pushButtonPlay->setText( "Pause" );
        startPlaying();
    }
    else
    {
//        ui->pushButtonPlay->setIcon( playIcon );
        ui->pushButtonPlay->setText( "Play" );
        stopPlaying();
    }
}

void recording::startPlaying()
{
    if( ui->horizontalSliderPosition->value() == ui->horizontalSliderPosition->maximum() )
    {
        ui->horizontalSliderPosition->setValue( 0 );
    }
    timer.start( 0 );
}

void recording::stopPlaying()
{
    timer.stop();
}

void playbackTimer::timerEvent( QTimerEvent* )
{
    recorder->nextFrameDue();
}

void recording::nextFrameDue()
{
    // Get and display the frame
    int currentFrame = ui->horizontalSliderPosition->value();
    historicImage frame = history.at( currentFrame );

    emit byteArrayChanged( frame.image, frame.dataSize, frame.alarmInfo, frame.time, 0 );


    // If done all frames, loop if looping, otherwise stop
    if( ui->horizontalSliderPosition->value() == ui->horizontalSliderPosition->maximum() )
    {
        if( ui->pushButtonLoop->isChecked() )
        {
            ui->horizontalSliderPosition->setValue( 0 );
        }
        else
        {
            stopPlaying();
        }
    }

    // If not done all frames, step on to the next
    else
    {
        ui->horizontalSliderPosition->setValue( currentFrame+1 );
    }

    // Set the due time for the next frame
    timer.start( ui->doubleSpinBoxPlaybackRate->value() * 1000 );
}
