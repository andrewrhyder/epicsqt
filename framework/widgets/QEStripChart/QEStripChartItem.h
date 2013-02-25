/*  QEStripChartItem.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
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
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 *
 */

#ifndef QSTRIPCHARTITEM_H
#define QSTRIPCHARTITEM_H

#include <QObject>
#include <QColor>
#include <QLabel>
#include <QString>

#include <qwt_plot_curve.h>

#include <QCaAlarmInfo.h>
#include <QCaConnectionInfo.h>
#include <QCaDataPoint.h>
#include <QCaDateTime.h>
#include <QELabel.h>
#include <QCaObject.h>
#include <QCaVariableNamePropertyManager.h>
#include <QEArchiveManager.h>
#include <QEStripChart.h>
#include <QEStripChartItemDialog.h>

// Provide log and exp 10 macro functions.
//
// Log is a safe log in that it avoids attempting to take the log of negative
// or zero values. The 1.0e-20 limit is somewhat arbitary, but in practice is
// good for most numbers encountered at the sychrotron.
//
// Not all platforms provide exp10.
//
#define LOG10(x)  ( (x) >=  1.0e-20 ? log10 (x) : -20.0 )
#define EXP10(x)  exp (2.302585092994046 * (x))

// Utility class - move to a separate unit??
// Tracks the minimum and maximum range of a value.
//
class TrackRange {
public:
   TrackRange ();
   void clear ();
   void merge (const double d);
   void merge (const TrackRange that);
   // returns true if range is defined together with min and max.
   //
   bool getMinMax (double & min, double& max);
private:
   double minimum;
   double maximum;
   bool isDefined;
};


// This is essentially a private classes used soley by the QEStripChart widget.
// We have to make is public so that it can be a pukka Q_OBJECT and as such receive
// signals.
//
class QEStripChartItem : public QObject {
   Q_OBJECT
public:
   explicit QEStripChartItem (QEStripChart *chart,
                              QLabel *pvName,
                              QELabel *caLabel,
                              unsigned int slot);
   virtual ~QEStripChartItem ();

   bool isInUse ();

   void setPvName (QString pvName, QString substitutions);
   QString getPvName ();

   // NOTE: Where ever possible I spell colour properly.
   //
   void setColour (QColor colourIn);
   QColor getColour ();

   TrackRange getLoprHopr ();                  // returns CA specified operating range
   TrackRange getDisplayedMinMax ();           // retruns range of values currently plotted
   TrackRange getBufferedMinMax ();            // retruns range of values that coulb be plotted

   void readArchive ();

   void plotData (const double timeScale,      // x scale modifier
                  const bool isLinearScale);   // y scale modifier

   QCaVariableNamePropertyManager pvNameProperyManager;

private:
   qcaobject::QCaObject* getQcaItem ();   // Return reference to QELabel used to stream CA updates
   void clear ();

   QPen getPen ();
   QwtPlotCurve *allocateCurve ();
   void plotDataPoints (const QCaDataPointList & dataPoints,
                        const double timeScale,
                        const bool isLinearScale,
                        const bool isRealTime,
                        TrackRange & plottedTrackRange);
   static bool isDisplayable (QCaDataPoint & point);

   // data members
   //
   bool isConnected;
   QColor colour;
   QCaDataPointList historicalTimeDataPoints;
   QCaDataPointList realTimeDataPoints;
   TrackRange historicalMinMax;
   TrackRange realTimeMinMax;

   TrackRange displayedMinMax;

   QEArchiveAccess archiveAccess;
   QEStripChartItemDialog dialog;

   // Return pv name label style, i.e. colour
   //
   QString getStyle ();

   // Intermal widgets references and associated support data.
   // If these items declared at class level, there is a run time exception.
   //
   class PrivateData;
   PrivateData *privateData;

private slots:
   void newVariableNameProperty (QString pvName, QString substitutions, unsigned int slot);
   void channelPropertiesClicked (bool checked = false);

   void setDataConnection (QCaConnectionInfo& connectionInfo);
   void setDataValue (const QVariant& value, QCaAlarmInfo& alarm, QCaDateTime& datetime);

   void setArchiveData (const QObject *userData, const bool okay, const QCaDataPointList &archiveData);
};

#endif  // QSTRIPCHARTITEM_H
