/*  QEStripChartItem.cpp
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
 */

#include <alarm.h>

#include <QColor>
#include <QCaObject.h>
#include <QEArchiveInterface.h>
#include <QColorDialog>

#include <qwt_plot_curve.h>

#include <QECommon.h>

#include "QEStripChartItem.h"
#include "QEStripChartContextMenu.h"


#define DEBUG  qDebug () <<  "QEStripChartItem::" <<  __FUNCTION__  << ":" << __LINE__

// Standard Channel Access archiver only support 10K points
//
#define MAXIMUM_POINTS  10000

// Define colours: essentially RGB byte triplets
//
static const QColor item_colours [QEStripChart::NUMBER_OF_PVS] = {
    QColor (0xFF0000), QColor (0x0000FF), QColor (0x008000), QColor (0xFF8000),
    QColor (0x4080FF), QColor (0x800000), QColor (0x008080), QColor (0x808000),
    QColor (0x800080), QColor (0x00FF00), QColor (0x00FFFF), QColor (0xFFFF00)
};

// Can't declare black as QColor (0x000000)
//
static const QColor clBlack (0x00, 0x00, 0x00, 0xFF);
static const QColor clWhite (0xFF, 0xFF, 0xFF, 0xFF);

static const QString inuse  ("QWidget { background-color: #e0e0e0; }");
static const QString unused ("QWidget { background-color: #c0c0c0; }");

static const QString scaledTip ("Note: this PV has been re-scaled");
static const QString regularTip ("Use context menu to modify PV attributes or double click here.");


//==============================================================================
//
QEStripChartItem::QEStripChartItem (QEStripChart* chartIn,
                                    unsigned int slotIn,
                                    QWidget* parent) : QWidget (parent)
{
   QColor defaultColour;

   // Construct internal widgets for this chart item.
   //
   this->createInternalWidgets ();

   this->chart = chartIn;
   this->slot = slotIn;
   this->inUse = false;
   this->previousQcaItem = NULL;

   // Set geometry
   //
   this->pvName->setGeometry  (  0, 0, 344, 15);
   this->caLabel->setGeometry (348, 0, 128, 15);

   // Set up other properties.
   //
   this->pvName->setIndent (6);
   this->pvName->setToolTip (regularTip);
   this->pvName->setAcceptDrops (true);

   // Use the chart item object as the pvName event filter object.
   //
   this->pvName->installEventFilter (this);

   // Set up context menus.
   //
   this->pvName->setContextMenuPolicy (Qt::CustomContextMenu);

   // Setup QELabel properties.
   //
   this->caLabel->setIndent (6);
   this->caLabel->setAlignment (Qt::AlignRight);
   QFont font = this->caLabel->font ();
   font.setFamily ("Monospace");
   this->caLabel->setFont (font);

   if (slot < QEStripChart::NUMBER_OF_PVS) {
      defaultColour = item_colours [this->slot];
   } else {
      defaultColour = clBlack;
   }
   this->setColour (defaultColour);


   // Clear/initialise.
   //
   this->clear ();

   // Assign the chart widget message source id the the associated archive access object.
   //
   this->archiveAccess.setMessageSourceId (chartIn->getMessageSourceId ());

   // Set up a connection to recieve variable name property changes.  The variable
   // name property manager class only delivers an updated variable name after the
   // user has stopped typing.
   //
   this->pvNameProperyManager.setVariableIndex (0);
   QObject::connect (&this->pvNameProperyManager, SIGNAL (newVariableNameProperty (QString, QString, unsigned int)),
                     this,                        SLOT   (newVariableNameProperty (QString, QString, unsigned int)));


   // Set up connection to archive access mamanger.
   //
   QObject::connect (&this->archiveAccess, SIGNAL (setArchiveData (const QObject *, const bool, const QCaDataPointList &)),
                     this,                 SLOT   (setArchiveData (const QObject *, const bool, const QCaDataPointList &)));


   this->connect (this->pvName, SIGNAL (customContextMenuRequested (const QPoint &)),
                  this,         SLOT   (contextMenuRequested (const QPoint &)));

   // Connect the context menus
   //
   this->connect (this->inUseMenu, SIGNAL (contextMenuSelected (const QEStripChartNames::ContextMenuOptions)),
                  this,            SLOT   (contextMenuSelected (const QEStripChartNames::ContextMenuOptions)));

   this->connect (this->emptyMenu, SIGNAL (contextMenuSelected (const QEStripChartNames::ContextMenuOptions)),
                  this,            SLOT   (contextMenuSelected (const QEStripChartNames::ContextMenuOptions)));
}

//------------------------------------------------------------------------------
//
QEStripChartItem::~QEStripChartItem ()
{
   // Place holder
}

//------------------------------------------------------------------------------
//
void QEStripChartItem::createInternalWidgets ()
{
   this->pvName = new QLabel (this);
   this->caLabel = new QELabel (this);
   this->colourDialog = new QColorDialog (this);
   this->inUseMenu = new QEStripChartContextMenu (true, this);
   this->emptyMenu = new QEStripChartContextMenu (false, this);
   this->pvNameSelectDialog = new QEPVNameSelectDialog (this);
   this->adjustPVDialog = new QEStripChartAdjustPVDialog (this);
}

//------------------------------------------------------------------------------
//
void QEStripChartItem::clear ()
{
   this->inUse = false;
   this->pvName->setText ("");
   this->caLabel->setVariableNameAndSubstitutions ("", "", 0);
   this->caLabel->setText ("-");
   this->caLabel->setStyleSheet (unused);
   this->previousQcaItem = NULL;

   this->displayedMinMax.clear ();
   this->historicalMinMax.clear ();
   this->realTimeMinMax.clear ();
   this->historicalTimeDataPoints.clear ();
   this->realTimeDataPoints.clear ();

   this->useReceiveTime = false;
   this->archiveReadHow = QEArchiveInterface::Linear;
   this->lineDrawMode = QEStripChartNames::ldmRegular;

   // Reset identity sclaing
   //
   this->scaling.reset();
}

//------------------------------------------------------------------------------
//
qcaobject::QCaObject* QEStripChartItem::getQcaItem ()
{
   // We "know" that a QELabel has only one PV.
   //
   return this->caLabel->getQcaItem (0);
}

//------------------------------------------------------------------------------
//
void QEStripChartItem::connectQcaSignals ()
{
   qcaobject::QCaObject *qca;

   // Set up connection if we can/if we need to.
   //
   qca = this->getQcaItem ();

   if (qca && (qca != this->previousQcaItem)) {
      this->previousQcaItem = qca;

      QObject::connect (qca, SIGNAL (connectionChanged (QCaConnectionInfo&) ),
                        this,  SLOT (setDataConnection (QCaConnectionInfo&) ) );

      QObject::connect (qca, SIGNAL (dataChanged  (const QVariant&, QCaAlarmInfo&, QCaDateTime&) ),
                        this,  SLOT (setDataValue (const QVariant&, QCaAlarmInfo&, QCaDateTime&) ) );
   }
}

//------------------------------------------------------------------------------
//
void QEStripChartItem::setPvName (QString pvName, QString substitutions)
{
   QString substitutedPVName;

   // Clear any existing data and reset defaults.
   //
   this->clear ();
   this->chart->evaluateAllowDrop ();   // move to strip chart proper??

   // We "know" that a QELabel has only one PV (index = 0).
   //
   caLabel->setVariableNameAndSubstitutions (pvName.trimmed (), substitutions, 0);
   substitutedPVName = caLabel->getSubstitutedVariableName (0);

   // Verify caller attempting add a potentially sensible PV?
   //
   if (substitutedPVName  == "") return;

   this->pvName->setText (substitutedPVName);
   this->caLabel->setStyleSheet (inuse);
   this->inUse = true;

   // Set up connections.
   //
   this->connectQcaSignals ();
}

//------------------------------------------------------------------------------
//
QString QEStripChartItem::getPvName ()
{
   return this->inUse ? this->pvName->text () : "";
}

//------------------------------------------------------------------------------
//
bool QEStripChartItem::isInUse ()
{
   return this->inUse;
}

//------------------------------------------------------------------------------
//
TrackRange QEStripChartItem::getLoprHopr (bool doScale)
{
   TrackRange result;
   qcaobject::QCaObject *qca;
   double lopr;
   double hopr;

   result.clear ();

   if (this->isInUse ()) {
      qca = this->getQcaItem ();
      if (qca) {
         lopr = qca->getDisplayLimitLower ();
         hopr = qca->getDisplayLimitUpper ();
      } else {
         lopr = hopr = 0.0;
      }

      // If either HOPR or LOPR are non zero - then range is deemed defined.
      //
      if ((lopr != 0.0) || (hopr != 0.0)) {
         result.merge (lopr);
         result.merge (hopr);
      }
   }
   if (doScale) {
       result = this->scaling.value (result);
   }
   return result;
}


//------------------------------------------------------------------------------
//
TrackRange QEStripChartItem::getDisplayedMinMax (bool doScale)
{
   TrackRange result;

   result = this->displayedMinMax;
   if (doScale) {
       result = this->scaling.value (result);
   }
   return result;
}


//------------------------------------------------------------------------------
//
TrackRange QEStripChartItem::getBufferedMinMax (bool doScale)
{
   TrackRange result;

   result = this->historicalMinMax;
   result.merge (this->realTimeMinMax);

   if (doScale) {
       result = this->scaling.value (result);
   }
   return result;
}


//------------------------------------------------------------------------------
//
QwtPlotCurve * QEStripChartItem::allocateCurve ()
{
   QwtPlotCurve * result;

   // Curves are managed by the chart widget.
   //
   result = this->chart->allocateCurve ();

   // Set curve propeties plus item Pen which include its colour.
   //
   result->setRenderHint (QwtPlotItem::RenderAntialiased);
   result->setStyle (QwtPlotCurve::Lines);
   result->setPen (this->getPen ());

   return result;
}


//------------------------------------------------------------------------------
//
void QEStripChartItem::plotDataPoints (const QCaDataPointList & dataPoints,
                                       const double timeScale,
                                       const QEStripChartNames::YScaleModes yScaleMode,
                                       const bool isRealTime,
                                       TrackRange & plottedTrackRange)
{

// macro functions to convert real-world values to a plot values, doing safe log conversion if required.
//
#define PLOT_T(t) ((t) / timeScale)
#define PLOT_Y(y) ((yScaleMode == QEStripChartNames::linear) ? this->scaling.value (y) : LOG10 (this->scaling.value (y)))

   const QDateTime end_time = this->chart->getEndDateTime ();
   const double duration = this->chart->getDuration ();

   QVector<double> tdata;
   QVector<double> ydata;
   int count;
   int j;
   QCaDataPoint point;
   QCaDataPoint previous;
   bool doesPreviousExist;
   bool isFirstPoint;
   double t;
   QwtPlotCurve *curve;

   // Both values zero is deemed to be undefined.
   //
   plottedTrackRange.clear ();
   isFirstPoint = true;
   doesPreviousExist = false;

   count = dataPoints.count ();
   for (j = 0; j < count; j++) {
      point = dataPoints.value (j);

      // Calculate the time of this point (in seconds) relative to the end of the chart.
      //
      t = point.datetime.floating (end_time);

      if (t < -duration) {
         // Point time is before current time range of the chart.
         //
         // Just save this point. Last time it is saved it will be the
         // pen-ultimate point before the chart start time.
         //
         previous = point;

         // Only "exists" if plottable.
         //
         doesPreviousExist = point.isDisplayable ();  // (previous.alarm.isInvalid () == false);

      }
      else if ((t >= -duration) && (t <= 0.0)) {
         // Point time is within current time range of the chart.
         //
         // Is it a valid point - can we sensible plot it?
         //
         if (point.isDisplayable ()) {
            if (!this->firstPointIsDefined) {
               this->firstPointIsDefined = true;
               this->firstPoint = point;
            }
            // Yes we can.
            //
            // start edge effect required?
            //
            if (isFirstPoint && doesPreviousExist) {
                tdata.append (PLOT_T (-duration));
                ydata.append (PLOT_Y (previous.value));
                plottedTrackRange.merge (previous.value);
            }

            // Do steps - do it like this as using qwt Step mode is not quite what I want.
            //
            if (ydata.count () >= 1) {
               tdata.append (PLOT_T (t));
               ydata.append (ydata.last ());   // copy don't need PLOT_Y
            }

            tdata.append (PLOT_T (t));
            ydata.append (PLOT_Y (point.value));
            plottedTrackRange.merge (point.value);

         } else {
            // plot what we have so far (need at least 2 points).
            //
            if (tdata.count () >= 1) {
               // The current pont is unplotable (invalid/disconneted).
               // Create  a valid stopper point consisting of prev. point value and this point time.
               //
               tdata.append (PLOT_T (t));
               ydata.append (ydata.last ());   // is a copy - no PLOT_Y required.
               curve = this->allocateCurve ();
#if QWT_VERSION >= 0x060000
               curve->setSamples (tdata, ydata);
#else
               curve->setData (tdata, ydata);
#endif
               tdata.clear ();
               ydata.clear ();
            }
         }

         // We have processed at least one point now.
         //
         isFirstPoint = false;

      } else {
         // Point time is after current plot time of the chart.
         // Move along - nothing more to see here.
         //
         break;
      }
   }

   // Start edge special required?
   //
   if (isFirstPoint && doesPreviousExist) {
       tdata.append (PLOT_T (-duration));
       ydata.append (PLOT_Y (previous.value));
       plottedTrackRange.merge (previous.value);
   }

   // Plot what we have accumulated.
   //
   if (ydata.count () >= 1) {
      // Real time extention to time now required?
      //
      if (isRealTime) {
         // Replicate last value upto end of chart.
         //
         tdata.append (PLOT_T (0.0));
         ydata.append (ydata.last ());   // is a copy - no PLOT_Y required.
      }
      curve = this->allocateCurve ();
#if QWT_VERSION >= 0x060000
      curve->setSamples (tdata, ydata);
#else
      curve->setData (tdata, ydata);
#endif
   }

#undef PLOT_T
#undef PLOT_Y
}


//------------------------------------------------------------------------------
//
void QEStripChartItem::plotData (const double timeScale,
                                 const QEStripChartNames::YScaleModes yScaleMode)
{
   TrackRange temp;

   this->displayedMinMax.clear ();
   this->firstPointIsDefined = false;

   if (this->lineDrawMode != QEStripChartNames::ldmHide) {

      this->plotDataPoints (this->historicalTimeDataPoints, timeScale, yScaleMode, false, temp);
      this->displayedMinMax.merge (temp);

      this->plotDataPoints (this->realTimeDataPoints,timeScale,  yScaleMode, true, temp);
      this->displayedMinMax.merge (temp);
   }

   // Sometimes the qca Item first used is not the qca Item we end up with, due the
   // vagaries of loading ui files and the framework start up. As plot data called
   // on a regular basis this is a convient place to recall connectQca.
   // Note: connectQcaSignals only does anything if underlying qca item has changed.
   //
   this->connectQcaSignals ();
}

//------------------------------------------------------------------------------
//
void QEStripChartItem::newVariableNameProperty (QString pvName, QString substitutions, unsigned int)
{
   this->setPvName (pvName, substitutions);

   // Re evaluate the chart drag drop allowed status.
   //
   this->chart->evaluateAllowDrop ();
}


//------------------------------------------------------------------------------
//
void QEStripChartItem::setDataConnection (QCaConnectionInfo& connectionInfo)
{
   QCaDataPoint point;

   this->isConnected = connectionInfo.isChannelConnected ();
   if ((this->isConnected == false) && (this->realTimeDataPoints.count () >= 1)) {
      // We have a channel disconnect.
      //
      // create a dummy point with last value and time now.
      //
      point = this->realTimeDataPoints.last ();
      point.datetime = QDateTime::currentDateTime ().toUTC ();
      this->realTimeDataPoints.append (point);
      if (this->realTimeDataPoints.count () > MAXIMUM_POINTS) {
         this->realTimeDataPoints.removeFirst ();
      }

      // create a dummy point with same time but marked invalid.
      //
      point.alarm = QCaAlarmInfo (NO_ALARM, INVALID_ALARM);
      this->realTimeDataPoints.append (point);
      if (this->realTimeDataPoints.count () > MAXIMUM_POINTS) {
         this->realTimeDataPoints.removeFirst ();
      }
   }
}

//------------------------------------------------------------------------------
//
void QEStripChartItem::setDataValue (const QVariant& value, QCaAlarmInfo& alarm, QCaDateTime& datetime)
{
   double y;
   bool okay;
   QCaDataPoint point;

   y = value.toDouble (&okay);
   if (okay) {
      // Conversion went okay - use this point.
      //
      point.value = y;
      point.alarm = alarm;
   } else {
      // Could not convert to a double - mark as an invalid point.
      //
      point.value = 0.0;
      point.alarm = QCaAlarmInfo (NO_ALARM, INVALID_ALARM);
   }

   // Some records, e.g. the motor record, post RBV updated without updating
   // the the process tiime until the end of the move. Sometimes the server and/or
   // client time is just wrong. In these cases it is better to plot using the
   // receive time.
   //
   if (this->useReceiveTime) {
      point.datetime = QDateTime::currentDateTime ().toUTC ();
   } else {
      point.datetime = datetime;
   }

   if (point.isDisplayable ()) {
      this->realTimeMinMax.merge (point.value);
   }
   this->realTimeDataPoints.append (point);

   if (this->realTimeDataPoints.count () > MAXIMUM_POINTS) {
      this->realTimeDataPoints.removeFirst ();
   }
}

//------------------------------------------------------------------------------
//
void QEStripChartItem::setArchiveData (const QObject *userData, const bool okay,
                                       const QCaDataPointList & archiveData)
{
   QCaDateTime firstRealTime;
   QCaDateTime pointTime;
   int count;
   int j, last;
   QCaDataPoint point;

   if ((userData == this) && (okay)) {

      // Clear any existing data and save new data
      // Maybe would could/should do some stiching together
      //
      this->historicalTimeDataPoints.clear ();
      this->historicalTimeDataPoints = archiveData;


      // Have any data points been returned?
      //
      count = this->historicalTimeDataPoints.count ();
      if (count > 0) {

         // Now throw away any historical data that overlaps with the real time data,
         // there is no need for two copies. We keep the real time data as it is of
         // a better quality.
         //
         // Find trucate time
         //
         if (this->realTimeDataPoints.count () > 0) {
            firstRealTime = this->realTimeDataPoints.value (0).datetime;
         } else {
            firstRealTime = QDateTime::currentDateTime ().toUTC ();
         }

         // Purge all points with a time >= firstRealTime, except for the
         // the very first point after first time.
         //
         last = count - 1;
         for (j = last - 1; j >= 0; j--) {
            point = this->historicalTimeDataPoints.value (j);
            pointTime = point.datetime;
            if (pointTime >= firstRealTime) {
               this->historicalTimeDataPoints.removeLast ();  // i.e. j+1
            } else {
               // purge complete
               break;
            }
         }

         // Tuncate the time of the last point left in historicalTimeDataPoints
         // to firstTime if needs be.
         //
         last = this->historicalTimeDataPoints.count () - 1;
         if (last >= 0) {
            point = this->historicalTimeDataPoints.value (last);
            if (point.datetime > firstRealTime) {
                point.datetime = firstRealTime;
                this->historicalTimeDataPoints.replace (last, point);
            }
         }

         // Now determine the min and max values of the remaining data points.
         //
         this->historicalMinMax.clear ();
         count = this->historicalTimeDataPoints.count ();
         for (j = 0; j < count; j++) {
            point = this->historicalTimeDataPoints.value (j);
            if (point.isDisplayable ()) {
               this->historicalMinMax.merge (point.value);
            }
         }
      }

      // and replot the data
      //
      this->chart->plotData ();

   } else {
      DEBUG << "wrong item and/or data response not okay";
   }
}

//------------------------------------------------------------------------------
//
void QEStripChartItem::readArchive ()
{
   const QDateTime startDateTime = this->chart->getStartDateTime ();
   const QDateTime endDateTime   = this->chart->getEndDateTime ();

   // Assign the chart widget message source id the the associated archive access object.
   // We re-assign just before each read in case it has changed.
   //
   this->archiveAccess.setMessageSourceId (this->chart->getMessageSourceId ());

   this->archiveAccess.readArchive
         (this, this->getPvName (),  startDateTime, endDateTime,
          4000, this->archiveReadHow, 0);
}

//------------------------------------------------------------------------------
//
void QEStripChartItem:: normalise () {
   // Just leverage off the context menu handler.
   //
   this->contextMenuSelected (QEStripChartNames::SCCM_SCALE_PV_AUTO);
}

//------------------------------------------------------------------------------
//
QColor QEStripChartItem::getColour ()
{
   return this->colour;
}

//------------------------------------------------------------------------------
//
void QEStripChartItem::setColour (const QColor & colourIn)
{
   QString styleSheet;

   this->colour = colourIn;
   styleSheet =  QEUtilities::colourToStyle (this->colour);
   this->pvName->setStyleSheet (styleSheet);
}

//------------------------------------------------------------------------------
//
void QEStripChartItem::highLight (bool isHigh)
{
   QString styleSheet;

   if (isHigh) {
      styleSheet =  QEUtilities::colourToStyle (clWhite);
   } else {
      styleSheet =  QEUtilities::colourToStyle (this->colour);
   }

   this->pvName->setStyleSheet (styleSheet);
}

//------------------------------------------------------------------------------
//
QPen QEStripChartItem::getPen ()
{
   QPen result (this->getColour ());

   switch (this->lineDrawMode) {
      case QEStripChartNames::ldmHide:
         result.setWidth (0);
         break;

      case QEStripChartNames::ldmRegular:
         result.setWidth (1);
         break;

      case QEStripChartNames::ldmBold:
         result.setWidth (2);
         break;

      default:
         result.setWidth (1);
         break;
   }
   return result;
}

//------------------------------------------------------------------------------
//
void QEStripChartItem::pvNameDropEvent (QDropEvent *event)
{
   // If no text available, do nothing
   //
   if (!event->mimeData()->hasText ()){
      event->ignore ();
      return;
   }

   // Get the drop data
   //
   const QMimeData *mime = event->mimeData ();

   // If there is any text, drop the text

   if (!mime->text().isEmpty ()) {
      // Get the component textual parts
      //
      QStringList pieces = mime->text ().split (QRegExp ("\\s+"),
                                                QString::SkipEmptyParts);

      // Carry out the drop action
      // Assume only the first text part is of interest
      //
      this->setPvName (pieces [0], "");
   }

   // Tell the dropee that the drop has been acted on
   //
   if (event->source() == this) {
      event->setDropAction(Qt::CopyAction);
      event->accept();
   } else {
      event->acceptProposedAction ();
   }
}

//------------------------------------------------------------------------------
//
bool QEStripChartItem::eventFilter (QObject *obj, QEvent *event)
{
   const QEvent::Type type = event->type ();

   switch (type) {

      case QEvent::MouseButtonDblClick:
         if (obj == this->pvName) {
            // Leverage of existing context menu handler.
            //
            this->contextMenuSelected (QEStripChartNames::SCCM_PV_EDIT_NAME);
            return true;  // we have handled double click
         }
         break;

      case QEvent::DragEnter:
         if (obj == this->pvName) {
            QDragEnterEvent* dragEnterEvent = static_cast<QDragEnterEvent*> (event);

            // Can only drop if text and not in use.
            //
            if (dragEnterEvent->mimeData()->hasText () && !this->isInUse()) {
               dragEnterEvent->setDropAction (Qt::CopyAction);
               dragEnterEvent->accept ();
               this->highLight (true);
            } else {
               dragEnterEvent->ignore ();
               this->highLight (false);
            }
            return true;
         }
         break;

      case QEvent::DragLeave:
         if (obj == this->pvName) {
            this->highLight (false);
            return true;
         }
         break;


      case QEvent::Drop:
         if (obj == this->pvName) {
            QDropEvent* dropEvent = static_cast<QDropEvent*> (event);
            this->pvNameDropEvent (dropEvent);
            this->highLight (false);
            return true;
         }
         break;

      default:
         // Just fall through
         break;
   }

   return false;
}

//------------------------------------------------------------------------------
//
void QEStripChartItem::contextMenuRequested (const QPoint & pos)
{
   QPoint tempPos;
   QPoint golbalPos;

   tempPos = pos;
   tempPos.setY (2);   // align with top of label
   golbalPos = this->mapToGlobal (tempPos);

   if (this->isInUse()) {
      this->inUseMenu->setUseReceiveTime (this->getUseReceiveTime ());
      this->inUseMenu->setArchiveReadHow (this->getArchiveReadHow ());
      this->inUseMenu->setLineDrawMode (this->getLineDrawMode ());
      this->inUseMenu->exec (golbalPos, 0);
   } else {
      this->emptyMenu->setPredefinedNames (chart->getPredefinedPVNameList ());
      this->emptyMenu->exec (golbalPos, 0);
   }
}


//------------------------------------------------------------------------------
//
void QEStripChartItem::contextMenuSelected (const QEStripChartNames::ContextMenuOptions option)
{
   TrackRange range;
   double min, max;
   double midway;
   bool status;
   int n;

   switch (option) {

      case QEStripChartNames::SCCM_READ_ARCHIVE:
         this->readArchive();
         break;

      case  QEStripChartNames::SCCM_SCALE_CHART_AUTO:
         range = this->getLoprHopr (true);
         status = range.getMinMax(min, max);
         if (status) {
            this->chart->setYRange (min, max);
         }
         break;

      case QEStripChartNames::SCCM_SCALE_CHART_PLOTTED:
         range = this->getDisplayedMinMax (true);
         status = range.getMinMax(min, max);
         if (status) {
            this->chart->setYRange (min, max);
         }
         break;

      case QEStripChartNames::SCCM_SCALE_CHART_BUFFERED:
         range = this->getBufferedMinMax (true);
         status = range.getMinMax(min, max);
         if (status) {
            this->chart->setYRange (min, max);
         }
         break;


      case QEStripChartNames::SCCM_SCALE_PV_RESET:
         this->scaling.reset();
         this->chart->plotData ();
         break;


      case QEStripChartNames::SCCM_SCALE_PV_GENERAL:
         this->adjustPVDialog->setSupport (this->chart->getYMinimum (),
                                           this->chart->getYMaximum (),
                                           this->getLoprHopr(false),
                                           this->getDisplayedMinMax(false),
                                           this->getBufferedMinMax(false));

         this->adjustPVDialog->setValueScaling (this->scaling);
         n = this->adjustPVDialog->exec (this);
         if (n == 1) {
             // User has selected okay.
             this->scaling.assign (this->adjustPVDialog->getValueScaling ());
             this->chart->plotData ();
         }
         break;

      case QEStripChartNames::SCCM_SCALE_PV_AUTO:
         range = this->getLoprHopr (false);
         status = range.getMinMax (min, max);
         if (status) {
            this->scaling.map (min, max, this->chart->getYMinimum (), this->chart->getYMaximum ());
            this->chart->plotData ();
         }
         break;

      case QEStripChartNames::SCCM_SCALE_PV_PLOTTED:
         range = this->getDisplayedMinMax (false);
         status = range.getMinMax (min, max);
         if (status) {
            this->scaling.map (min, max, this->chart->getYMinimum (), this->chart->getYMaximum ());
            this->chart->plotData ();
         }
         break;

      case QEStripChartNames::SCCM_SCALE_PV_BUFFERED:
         range = this->getBufferedMinMax (false);
         status = range.getMinMax (min, max);
         if (status) {
            this->scaling.map (min, max, this->chart->getYMinimum (), this->chart->getYMaximum ());
            this->chart->plotData ();
         }
         break;

      case QEStripChartNames::SCCM_SCALE_PV_CENTRE:
         if (this->firstPointIsDefined) {
            midway = (chart->getYMinimum () + this->chart->getYMaximum () ) / 2.0;
            this->scaling.set (this->firstPoint.value, 1.0, midway);
         }
         break;

      case QEStripChartNames::SCCM_LINE_COLOUR:
         this->colourDialog->setCurrentColor (this->getColour ());
         this->colourDialog->open (this, SLOT (setColour (const QColor &)));
         break;

      case QEStripChartNames::SCCM_PV_ADD_NAME:
      case QEStripChartNames::SCCM_PV_EDIT_NAME:
         this->pvNameSelectDialog->setPvName (this->getPvName ());
         n = this->pvNameSelectDialog->exec (this);
         if (n == 1) {
            // User has selected okay.
            //
            if (this->getPvName () != this->pvNameSelectDialog->getPvName ()) {
               this->setPvName (this->pvNameSelectDialog->getPvName (), "");
            }
            // and replot the data
            //
            this->chart->plotData ();
         }
         break;

      case QEStripChartNames::SCCM_PV_PASTE_NAME:
         {
            QClipboard *cb = QApplication::clipboard ();
            QString pasteText = cb->text().trimmed();

            if (! pasteText.isEmpty()) {
               this->setPvName (pasteText, "");
            }
         }
         break;

      case QEStripChartNames::SCCM_ADD_TO_PREDEFINED:
         this->chart->addToPredefinedList (this->getPvName ());
         break;

      case QEStripChartNames::SCCM_PREDEFINED_01:
      case QEStripChartNames::SCCM_PREDEFINED_02:
      case QEStripChartNames::SCCM_PREDEFINED_03:
      case QEStripChartNames::SCCM_PREDEFINED_04:
      case QEStripChartNames::SCCM_PREDEFINED_05:
      case QEStripChartNames::SCCM_PREDEFINED_06:
      case QEStripChartNames::SCCM_PREDEFINED_07:
      case QEStripChartNames::SCCM_PREDEFINED_08:
      case QEStripChartNames::SCCM_PREDEFINED_09:
      case QEStripChartNames::SCCM_PREDEFINED_10:
         n = option - QEStripChartNames::SCCM_PREDEFINED_01;
         this->setPvName (chart->getPredefinedItem (n), "");
         break;

      case QEStripChartNames::SCCM_PV_CLEAR:
         this->clear ();
         this->chart->evaluateAllowDrop ();   // move to strip chart proper??
         break;

      case QEStripChartNames::SCCM_PLOT_SERVER_TIME:
         this->useReceiveTime = false;
         break;

      case QEStripChartNames::SCCM_PLOT_CLIENT_TIME:
         this->useReceiveTime = true;
         break;

      case QEStripChartNames::SCCM_ARCH_LINEAR:
         this->archiveReadHow = QEArchiveInterface::Linear;
         break;

      case QEStripChartNames::SCCM_ARCH_PLOTBIN:
         this->archiveReadHow = QEArchiveInterface::PlotBinning;
         break;

      case QEStripChartNames::SCCM_ARCH_RAW:
         this->archiveReadHow = QEArchiveInterface::Raw;
         break;

      case QEStripChartNames::SCCM_ARCH_SHEET:
         this->archiveReadHow = QEArchiveInterface::SpreadSheet;
         break;

      case QEStripChartNames::SCCM_ARCH_AVERAGED:
         this->archiveReadHow = QEArchiveInterface::Averaged;
         break;

      case QEStripChartNames::SCCM_LINE_HIDE:
         this->lineDrawMode = QEStripChartNames::ldmHide;
         this->chart->plotData ();
         break;

      case QEStripChartNames::SCCM_LINE_REGULAR:
         this->lineDrawMode = QEStripChartNames::ldmRegular;
         this->chart->plotData ();
         break;

      case QEStripChartNames::SCCM_LINE_BOLD:
         this->lineDrawMode = QEStripChartNames::ldmBold;
         this->chart->plotData ();
         break;


      default:
         DEBUG << int (option) << this->pvName->text () << "tbd";
   }

   // Set tool top depending on current scaling.
   //
   if (this->scaling.isScaled()) {
      this->pvName->setToolTip (scaledTip);
   } else {
      this->pvName->setToolTip (regularTip);
   }
}

//------------------------------------------------------------------------------
//
void QEStripChartItem::saveConfiguration (PMElement & parentElement)
{
   // Any config data to save?
   //
   if (this->isInUse ()) {
      PMElement pvElement = parentElement.addElement ("PV");
      pvElement.addAttribute ("slot", (int) this->slot);

      // Note: we save the actual, i.e. substituted, PV name.
      //
      pvElement.addValue ("Name", this->getPvName ());

      // Save any scaling.
      //
      this->scaling.saveConfiguration (pvElement);
   }
}

//------------------------------------------------------------------------------
//
void QEStripChartItem::restoreConfiguration (PMElement & parentElement)
{
   QString pvName;
   bool status;

   PMElement pvElement = parentElement.getElement ("PV", "slot", (int) this->slot);

   if (pvElement.isNull ()) return;

   // Attempt to extract a PV name
   //
   status = pvElement.getValue ("Name", pvName);
   if (status) {
      this->setPvName (pvName, "");
      this->scaling.restoreConfiguration (pvElement);
   }
}

// end
