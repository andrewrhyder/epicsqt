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
   This class inherits directly from QProgressBar and provides an analog view of
   the progress bar widget including analogous analog functions and properties:

       QProgressBar      QAnalogProgressBar

       setRange()        setAnalogRange()
       setMinimum()      setAnalogMinimum()
       getMinimum()      getAnalogMinimum()
       setMaximum()      setAnalogMaximum()
       getMaximum()      getAnalogMaximum()
       setValue()        setAnalogValue()
       getValue()        getAnalogValue()

       Minimum           analogMinimum
       Maximum           analogMaximum
       Value             analogValue

   The class sets the range of of the underlying QProgressBar to 0 .. 10000.
   It it considered unlikely, in practice, that  an AnalogProgressBar wll stretch
   more than 10000 pixels given that current screen technology is limited to 2000 pixels.

   This class inherited from rather than containing a QProgressBar, and as such cannot
   hide and does not override the integer related base class functions and properties.
   Therefore they should not be used.
 */


#ifndef QANALOGPROGRESSBAR_H
#define QANALOGPROGRESSBAR_H

#include <QProgressBar>
#include <QCaPluginLibrary_global.h>


class QCAPLUGINLIBRARYSHARED_EXPORT QAnalogProgressBar:public QProgressBar {
    Q_OBJECT
//#ifdef PLUGIN_APP
    Q_PROPERTY (double analogMinimum READ getAnalogMinimum WRITE setAnalogMinimum)
    Q_PROPERTY (double analogMaximum READ getAnalogMaximum WRITE setAnalogMaximum)
    Q_PROPERTY (double analogValue   READ getAnalogValue   WRITE setAnalogValue)
//#endif

private:
    double getScale ();
    void set_progress_bar ();

    // class member variable names start with m so as not to clash with
    // the propery names.
    //
    double mAnalogMinimum;
    double mAnalogMaximum;
    double mAnalogValue;

protected:
    // Returns the format parameter for a call to sprintf, used to set
    // underlying bar format property.
    // This may be overridden by a derived class.
    //
    virtual QString getSprintfFormat ();

public:
    /// Constructor
    QAnalogProgressBar (QWidget * parent = 0);
    virtual ~QAnalogProgressBar(){}

    double getAnalogMinimum ();
    double getAnalogMaximum ();
    double getAnalogValue   ();

public slots:
    void setAnalogRange   (const double analogMinimumIn, const double analogMaximumIn);
    void setAnalogMinimum (const double analogMinimumIn);
    void setAnalogMaximum (const double analogMaximumIn);
    void setAnalogValue   (const double analogValueIn);
};

#endif /// QANALOGPROGRESSBAR_H
