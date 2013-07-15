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
#include <QWidget>

#include <QCaAlarmInfo.h>
#include <QCaConnectionInfo.h>
#include <QCaDataPoint.h>
#include <QCaDateTime.h>
#include <QELabel.h>
#include <QCaObject.h>
#include <QCaVariableNamePropertyManager.h>
#include <QEArchiveManager.h>
#include <persistanceManager.h>

#include "QEStripChart.h"
#include "QEStripChartNames.h"
#include "QEStripChartItemDialog.h"
#include "QEStripChartAdjustPVDialog.h"
#include "QEStripChartContextMenu.h"
#include "QEStripChartUtilities.h"

// Defered declaration - exists in qwt_plot_curve.h - but
// we don't need to expose that.
//
class QwtPlotCurve;

//==============================================================================
// This is essentially a private classes used soley by the QEStripChart widget.
// We have to make is public so that it can be a pukka Q_OBJECT in order to
// receive signals.
//
class QEStripChartItem : public QWidget {
   Q_OBJECT
public:
   explicit QEStripChartItem (QEStripChart* chart,
                              unsigned int slot,
                              QWidget* parent);
   virtual ~QEStripChartItem ();

   bool isInUse ();

   void setPvName (QString pvName, QString substitutions);
   QString getPvName ();

   void setScaling (const double d, const double m, const double c);
   void getScaling (double & d, double & m, double & c);
   bool isScaled ();

   // Following used to extract status for the context menu object.
   //
   bool getUseReceiveTime () { return this->useReceiveTime; }
   QEArchiveInterface::How getArchiveReadHow () { return this->archiveReadHow; }
   QEStripChartNames::LineDrawModes getLineDrawMode () { return this->lineDrawMode; }

   // NOTE: Where ever possible I spell colour properly.
   //
public slots:
   void setColour (const QColor&  colour);    // also used by colour dialog
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

   void saveConfiguration (PMElement & parentElement);
   void restoreConfiguration (PMElement & parentElement);

   QCaVariableNamePropertyManager pvNameProperyManager;

public: signals:
   void itemContextMenuRequested (const unsigned int, const QPoint &);

protected:
   bool eventFilter (QObject *obj, QEvent *event);

private:
   qcaobject::QCaObject* getQcaItem ();   // Return reference to QELabel used to stream CA updates
   void connectQcaSignals ();             // Performs qca connections if needs be.
   void clear ();
   void highLight (bool isHigh);

   QPen getPen ();
   QwtPlotCurve *allocateCurve ();
   void plotDataPoints (const QCaDataPointList & dataPoints,
                        const double timeScale,
                        const QEStripChartNames::YScaleModes yScaleMode,
                        const bool isRealTime,
                        TrackRange & plottedTrackRange);

   // Perform a pvNameDropEvent 'drop'
   //
   void pvNameDropEvent (QDropEvent *event);

   // data members
   //
   unsigned int slot;
   bool isConnected;
   bool useReceiveTime;
   QEArchiveInterface::How archiveReadHow;
   QEStripChartNames::LineDrawModes lineDrawMode;

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

   QEStripChartItemDialog *pvNameEditDialog;
   QEStripChartAdjustPVDialog *adjustPVDialog;

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

   void contextMenuRequested (const QPoint & pos);
   void contextMenuSelected  (const QEStripChartNames::ContextMenuOptions option);
};

#endif  // QSTRIPCHARTITEM_H
