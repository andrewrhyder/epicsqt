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
#include <QPoint>
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

#include "QEStripChart.h"
#include "QEStripChartNames.h"
#include "QEStripChartItemDialog.h"
#include "QEStripChartAdjustPVDialog.h"
#include "QEStripChartContextMenu.h"
#include "QEStripChartUtilities.h"


// Provide log and exp 10 macro functions.
//
// Log is a safe log in that it avoids attempting to take the log of negative
// or zero values. The 1.0e-20 limit is somewhat arbitary, but in practice is
// good for most numbers encountered at the synchrotron.
//
// Not all platforms provide exp10. What about pow () ??
//
#define LOG10(x)  ( (x) >=  1.0e-20 ? log10 (x) : -20.0 )
#define EXP10(x)  exp (2.302585092994046 * (x))


//==============================================================================
// This is essentially a private classes used soley by the QEStripChart widget.
// We have to make is public so that it can be a pukka Q_OBJECT in order to
// receive signals.
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

   void setScaling (const double d, const double m, const double c);
   void getScaling (double & d, double & m, double & c);
   bool isScaled ();

   // NOTE: Where ever possible I spell colour properly.
   //
public slots:
   void setColour (const QColor &  colour);    // also used by colour dialog
public:
   QColor getColour ();

   // For each of these three function whren the bool doScale parameter is true, the
   // return range item is scaled using the current PV scale. When false, the "raw"
   // value is returned.
   //
   TrackRange getLoprHopr (bool doScale);          // returns CA specified operating range
   TrackRange getDisplayedMinMax (bool doScale);   // returns range of values currently plotted
   TrackRange getBufferedMinMax (bool doScale);    // returns range of values that could be plotted

   void readArchive ();
   void normalise ();

   void plotData (const double timeScale,                             // x scale modifier
                  const QEStripChartNames::YScaleModes yScaleMode);   // y scale modifier

   void contextMenuSelected (const QEStripChartContextMenu::Options option);

   QCaVariableNamePropertyManager pvNameProperyManager;

public: signals:
   void customContextMenuRequested (const unsigned int, const QPoint &);

private:
   qcaobject::QCaObject* getQcaItem ();   // Return reference to QELabel used to stream CA updates
   void clear ();

   QPen getPen ();
   QwtPlotCurve *allocateCurve ();
   void plotDataPoints (const QCaDataPointList & dataPoints,
                        const double timeScale,
                        const QEStripChartNames::YScaleModes yScaleMode,
                        const bool isRealTime,
                        TrackRange & plottedTrackRange);
   static bool isDisplayable (QCaDataPoint & point);

   // data members
   //
   unsigned int slot;
   bool isConnected;
   QColor colour;
   ValueScaling scaling;

   QCaDataPointList historicalTimeDataPoints;
   QCaDataPointList realTimeDataPoints;
   TrackRange historicalMinMax;
   TrackRange realTimeMinMax;

   bool firstPointIsDefined;
   QCaDataPoint firstPoint;
   TrackRange displayedMinMax;

   QEArchiveAccess archiveAccess;

   QEStripChartItemDialog pvNameEditDialog;
   QEStripChartAdjustPVDialog adjustPVDialog;

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

   void setDataConnection (QCaConnectionInfo& connectionInfo);
   void setDataValue (const QVariant& value, QCaAlarmInfo& alarm, QCaDateTime& datetime);

   void setArchiveData (const QObject *userData, const bool okay, const QCaDataPointList &archiveData);

   void customContextMenuRequested (const QPoint & pos);
};

#endif  // QSTRIPCHARTITEM_H
