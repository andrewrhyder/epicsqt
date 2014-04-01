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

recording::recording( QWidget* parent ) :
    QWidget(parent),
    ui(new Ui::recording)
{
    ui->setupUi(this);

    timer = new playbackTimer( this );

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
    ui->doubleSpinBoxPlaybackRate->setMaximum( 10.0 );
    ui->doubleSpinBoxPlaybackRate->setMinimum( 0.02 );
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

    ui->horizontalSliderPosition->setMaximum( history.count()-1 );
    ui->horizontalSliderPosition->setValue( history.count()-1 );
    ui->labelImageCount->setText( QString( "%1" ).arg( history.count() ) );
    ui->pushButtonClear->setEnabled( history.count() );
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
    ui->pushButtonFirstImage      ->setEnabled( playback );
    ui->pushButtonPreviousImage   ->setEnabled( playback );
    ui->pushButtonNextImage       ->setEnabled( playback );
    ui->pushButtonLastImage       ->setEnabled( playback );
    ui->pushButtonPause           ->setEnabled( playback && ui->pushButtonPlay->isChecked() );
    ui->pushButtonRecord          ->setEnabled( !playback || !ui->pushButtonPlay->isChecked() );
    ui->horizontalSliderPosition  ->setEnabled( playback );

    ui->pushButtonClear->setEnabled( history.count() );

    if( !ui->pushButtonPause->isEnabled() )
    {
        ui->pushButtonPause->setChecked( false );
    }
}

void recording::on_pushButtonPlay_toggled(bool checked)
{
    if( checked )
    {
        startPlaying();
    }
    else
    {
        stopPlaying();
    }
    enableControls();
}

void recording::startPlaying()
{
    emit playingBack( true );
    if( ui->horizontalSliderPosition->value() == ui->horizontalSliderPosition->maximum() )
    {
        ui->horizontalSliderPosition->setValue( 0 );
    }
    timer->start( 0 );

}

void recording::stopPlaying()
{
    ui->pushButtonPlay->setChecked( false );
    timer->stop();
    emit playingBack( false ); //!!! this is wrong. Should allow updates when 'live' button is pressed

    enableControls();
}

void playbackTimer::timerEvent( QTimerEvent* )
{
    recorder->nextFrameDue();
}

void recording::showRecordedFrame( int currentFrame )
{
    // Get and display the frame
    if( currentFrame < history.count() )
    {
        ui->labelImageCount->setText( QString( "%1/%2" ).arg( currentFrame ).arg( ui->horizontalSliderPosition->maximum() ) );
        historicImage frame = history.at( currentFrame );
        emit byteArrayChanged( frame.image, frame.dataSize, frame.alarmInfo, frame.time, 0 );
    }
}

void recording::nextFrameDue()
{
    int currentFrame = ui->horizontalSliderPosition->value();
    showRecordedFrame( currentFrame );

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
    timer->setInterval( ui->doubleSpinBoxPlaybackRate->value() * 1000 );
}

void recording::on_pushButtonClear_clicked()
{
    history.clear();
    ui->labelImageCount->setText( "0" );
    ui->horizontalSliderPosition->setMaximum( 0 );
    ui->horizontalSliderPosition->setValue( 0 );
    enableControls();
}

void recording::on_pushButtonPause_toggled(bool checked)
{
    if( checked )
    {
        timer->stop();
    }
    else
    {
        if( history.count() )
        {
            timer->start();
        }
    }
}

void recording::on_pushButtonPreviousImage_clicked()
{
    int currentFrame = ui->horizontalSliderPosition->value();
    if( currentFrame > 0 )
    {
        currentFrame--;
        ui->horizontalSliderPosition->setValue( currentFrame );
        showRecordedFrame( currentFrame );
    }
}

void recording::on_pushButtonFirstImage_clicked()
{
    ui->horizontalSliderPosition->setValue( 0 );
    showRecordedFrame( 0 );
}

void recording::on_pushButtonNextImage_clicked()
{
    int currentFrame = ui->horizontalSliderPosition->value();
    if( currentFrame < ui->horizontalSliderPosition->maximum() )
    {
        currentFrame++;
        ui->horizontalSliderPosition->setValue( currentFrame );
        showRecordedFrame( currentFrame );
    }
}

void recording::on_pushButtonLastImage_clicked()
{
    int currentFrame = ui->horizontalSliderPosition->maximum();
    ui->horizontalSliderPosition->setValue( currentFrame );
    showRecordedFrame( currentFrame );
}

void recording::on_horizontalSliderPosition_valueChanged(int value)
{
    showRecordedFrame( value );
}
