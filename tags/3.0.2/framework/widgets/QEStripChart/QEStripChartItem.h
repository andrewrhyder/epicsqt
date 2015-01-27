/*  QEStripChartItem.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2012 Australian Synchrotron
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 *
 */

#ifndef QSTRIPCHARTITEM_H
#define QSTRIPCHARTITEM_H

#include <QColor>
#include <QColorDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QObject>
#include <QPoint>
#include <QPushButton>
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
#include <QEWidget.h>
#include <QEActionRequests.h>
#include <QEExpressionEvaluation.h>
#include <QEDisplayRanges.h>

#include "QEStripChart.h"
#include "QEStripChartNames.h"
#include "QEStripChartAdjustPVDialog.h"
#include "QEStripChartContextMenu.h"
#include "QEStripChartUtilities.h"

//==============================================================================
// This is essentially a private classes used soley by the QEStripChart widget.
// We have to make is public so that it can be a pukka Q_OBJECT in order to
// receive signals. We also need to make it a QEWidget so that we can find the
// launch consumer.
//
class QEStripChartItem : public QWidget, private QEWidget {
   Q_OBJECT
public:
   explicit QEStripChartItem (QEStripChart* chart,
                              unsigned int slot,
                              QWidget* parent);
   virtual ~QEStripChartItem ();

   bool isInUse ();
   bool isCalculation ();

   void setPvName (QString pvName, QString substitutions);
   QString getPvName ();

   bool isScaled ();

   // Following used to extract status for the context menu object.
   //
   bool getUseReceiveTime () { return this->useReceiveTime; }
   QEArchiveInterface::How getArchiveReadHow () { return this->archiveReadHow; }
   QEStripChartNames::LineDrawModes getLineDrawMode () { return this->lineDrawMode; }

   // NOTE: Where ever possible I spell colour properly.
   //
   void setColour (const QColor&  colour);    // also used by colour dialog
   QColor getColour ();

   // For each of these three function whren the bool doScale parameter is true, the
   // return range item is scaled using the current PV scale. When false, the "raw"
   // value is returned.
   //
   QEDisplayRanges getLoprHopr (bool doScale);          // returns CA specified operating range
   QEDisplayRanges getDisplayedMinMax (bool doScale);   // returns range of values currently plotted
   QEDisplayRanges getBufferedMinMax (bool doScale);    // returns range of values that could be plotted
   QCaDataPointList determinePlotPoints ();

   void readArchive ();
   void normalise ();

   void plotData ();

   void saveConfiguration (PMElement & parentElement);
   void restoreConfiguration (PMElement & parentElement);

   QCaVariableNamePropertyManager pvNameProperyManager;

signals:
   void itemContextMenuRequested (const unsigned int, const QPoint &);
   void requestAction (const QEActionRequests&);

protected:
   bool eventFilter (QObject *obj, QEvent *event);

private:
   qcaobject::QCaObject* getQcaItem ();   // Return reference to QELabel used to stream CA updates
   void connectQcaSignals ();             // Performs qca connections if needs be.
   void setCaption ();
   void clear ();
   void highLight (bool isHigh);

   QPen getPen ();
   void plotDataPoints (const QCaDataPointList& dataPoints,
                        const bool isRealTime,
                        QEDisplayRanges& plottedTrackRange);

   // Perform a pvNameDropEvent 'drop'.
   //
   void pvNameDropEvent (QDropEvent *event);

   void writeTraceToFile ();
   void generateStatistics ();

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
   QEDisplayRanges historicalMinMax;
   QEDisplayRanges realTimeMinMax;

   bool firstPointIsDefined;
   QCaDataPoint firstPoint;
   QEDisplayRanges displayedMinMax;

   QEArchiveAccess archiveAccess;

   QEStripChartAdjustPVDialog *adjustPVDialog;

   enum DataChartKinds { NotInUse,          // blank  - not in use - no data - no plot
                         PVData,            // use specified PV to provide plot data
                         CalculationData }; // "= ..." - use given calculation for plot data

   DataChartKinds dataKind;
   QString expression;        // when dataKind is CalculationPlot
   bool expressionIsValid;
   QEExpressionEvaluation* calculator;

   // Internal widgets.
   //
   QEStripChart *chart;

   QHBoxLayout* layout;
   QPushButton *pvSlotLetter;
   QLabel *pvName;
   QELabel *caLabel;
   QColorDialog *colourDialog;
   QEStripChartContextMenu* inUseMenu;
   QEStripChartContextMenu* emptyMenu;
   qcaobject::QCaObject *previousQcaItem;
   bool hostSlotAvailable;

   void createInternalWidgets ();
   void runSelectNameDialog (QWidget* control);

private slots:
   void newVariableNameProperty (QString pvName, QString substitutions, unsigned int slot);

   void setDataConnection (QCaConnectionInfo& connectionInfo, const unsigned int& variableIndex);
   void setDataValue (const QVariant& value, QCaAlarmInfo& alarm, QCaDateTime& datetime, const unsigned int& variableIndex);

   void setArchiveData (const QObject *userData, const bool okay, const QCaDataPointList &archiveData);

   void letterButtonClicked (bool checked);
   void contextMenuRequested (const QPoint & pos);
   void contextMenuSelected  (const QEStripChartNames::ContextMenuOptions option);

};

#endif  // QSTRIPCHARTITEM_H
