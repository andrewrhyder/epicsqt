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
 *  Copyright (c) 2011
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

/*!
  This class is a analog version of the Qt progress bar widget.
 */

#include <QAnalogProgressBar.h>
#include <math.h>


#define PROGRESS_BAR_SIZE     10000
#define MINIMUM_SPAN          0.000001

#define MAX(a, b)           ((a) >= (b) ? (a) : (b))
#define MIN(a, b)           ((a) <= (b) ? (a) : (b))
#define LIMIT(x,low,high)   (MAX(low, MIN(x, high)))


//------------------------------------------------------------------------------
// Constructor with no initialisation
//
QAnalogProgressBar::QAnalogProgressBar( QWidget *parent ) : QProgressBar( parent ) {
    mAnalogMinimum = 0.0;
    mAnalogMaximum = 10.0;
    setAnalogValue (0.0);
}


//------------------------------------------------------------------------------
//
void QAnalogProgressBar::set_progress_bar ()
{
    double fraction;

    // Calculate the fractional scale and constrain to be in range.
    //
    fraction = (mAnalogValue - mAnalogMinimum) / (mAnalogMaximum - mAnalogMinimum);
    fraction = LIMIT (fraction, 0.0, 1.0);

    // Ensure underlying range is still what we expect.
    //
    QProgressBar::setRange (0, (int) PROGRESS_BAR_SIZE);
    QProgressBar::setValue ((int) (fraction * PROGRESS_BAR_SIZE));
}


//------------------------------------------------------------------------------
//
void QAnalogProgressBar::setAnalogRange(const double analogMinimumIn,
                                        const double analogMaximumIn)
{
    mAnalogMinimum = analogMinimumIn;

    // Ensure  max - min >= miinimum span
    mAnalogMaximum = MAX (analogMaximumIn, mAnalogMinimum + MINIMUM_SPAN);
    set_progress_bar ();
}


//------------------------------------------------------------------------------
// Set analog minimum value
//
void QAnalogProgressBar::setAnalogMinimum (const double analogMinimumIn)
{
    mAnalogMinimum = analogMinimumIn;

    // Ensure  max - min >= miinimum span
    mAnalogMaximum = MAX (mAnalogMaximum, mAnalogMinimum + MINIMUM_SPAN);
    set_progress_bar ();
}


//------------------------------------------------------------------------------
//
double  QAnalogProgressBar::getAnalogMinimum ()
{
    return mAnalogMinimum;
}


//------------------------------------------------------------------------------
// Set analog maximum value
//
void QAnalogProgressBar::setAnalogMaximum( const double analogMaximumIn )
{
    mAnalogMaximum = analogMaximumIn;

    // Ensure  max - min >= miinimum span
    mAnalogMinimum = MIN (mAnalogMinimum, mAnalogMaximum - MINIMUM_SPAN);
    set_progress_bar ();
}


//------------------------------------------------------------------------------
//
double  QAnalogProgressBar::getAnalogMaximum ()
{
    return mAnalogMaximum;
}


//------------------------------------------------------------------------------
//
QString QAnalogProgressBar::getSprintfFormat ()
{
    return QString ("%+0.7g");
}


//------------------------------------------------------------------------------
//
void QAnalogProgressBar::setAnalogValue( const double analogValueIn )
{
    QString sprintfFormat;
    QString barFormat;

    mAnalogValue = analogValueIn;
    set_progress_bar ();

    // This is a dispatching call.
    //
    sprintfFormat = getSprintfFormat ();

    barFormat.sprintf(sprintfFormat.toAscii().data (), mAnalogValue);

    QProgressBar::setFormat (barFormat);
}


//------------------------------------------------------------------------------
//
double  QAnalogProgressBar::getAnalogValue()
{
    return mAnalogValue;
}

// end
