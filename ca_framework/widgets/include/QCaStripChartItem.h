/*  QCaStripChartItem.h
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
#include <QCaLabel.h>
#include <QCaObject.h>
#include <QCaVariableNamePropertyManager.h>

#include <QCaStripChart.h>


// Utility class - move to a separate unit.
//
class TrackRange {
public:
   TrackRange ();
   void clear ();
   void merge (const double d);
   void merge (const TrackRange that);
   /* returns true if range is defined together with min and max.
    */
   bool getMinMax (double & min, double& max);
private:
   double minimum;
   double maximum;
   bool isDefined;
};


// This is essentially a private classes used soley by the QCaStripChart widget.
// We have to make is public so that it can be pukka Q_OBJECT and as such receive
// signals.
//
class QCaStripChartItem : public QObject {
   Q_OBJECT

private:
   friend class QCaStripChart;

   QCaStripChartItem (QCaStripChart *chart,
                      QLabel *pvName,
                      QCaLabel *caLabel,
                      unsigned int slot);
   ~QCaStripChartItem ();

   QCaVariableNamePropertyManager pvNameProperyManager;
   void setPvName (QString pvName, QString substitutions);
   QString getPvName ();
   void clear ();
   bool isInUse ();
   void plotData ();
   void setColor (QColor colorIn);
   QColor getColor ();
   QPen getPen ();
   QwtPlotCurve *allocateCurve ();

   // data memebers
   //
   TrackRange getLoprHopr ();
   TrackRange getDisplayedMinMax ();
   TrackRange getBufferedMinMax ();

   bool isConnected;
   QColor color;
   QCaDataPointList historicalTimeDataPoints;
   QCaDataPointList realTimeDataPoints;

   TrackRange displayedMinMax;
   TrackRange bufferedMinMax;

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
};

#endif  // QSTRIPCHARTITEM_H
