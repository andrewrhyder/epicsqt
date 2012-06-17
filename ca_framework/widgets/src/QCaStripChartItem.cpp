/*  $Id: QCaStripChartItem.cpp $
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

#include <QColor>
#include <alarm.h>
#include <qwt_plot_curve.h>
#include <QCaObject.h>
#include <QCaArchiveInterface.h>

#include <QCaStripChartItem.h>

#define DEBUG  qDebug () << __FILE__  << ":" << __LINE__ << "(" << __FUNCTION__ << ")"

#define MAX(a, b)           ((a) >= (b) ? (a) : (b))
#define MIN(a, b)           ((a) <= (b) ? (a) : (b))

#define MAXIMUM_POINTS  10000


// Define colours: essentially RGB byte triplets
//
static const QColor item_colours [QCaStripChart::NUMBER_OF_PVS] = {
    QColor (0xFF0000), QColor (0x0000FF), QColor (0x008000), QColor (0xFF8000),
    QColor (0x4080FF), QColor (0x800000), QColor (0x008080), QColor (0x808000),
    QColor (0x800080), QColor (0x00FF00), QColor (0x00FFFF), QColor (0xFFFF00)
};


static const QColor clBlack (0x00, 0x00, 0x00, 0xFF);


//==============================================================================
//
TrackRange::TrackRange ()
{
   this->clear ();
}   // TrackRange


//------------------------------------------------------------------------------
//
void TrackRange::clear ()
{
   this->isDefined = false;
   this->minimum = 0.0;
   this->maximum = 0.0;
}   // clear


//------------------------------------------------------------------------------
//
void TrackRange::merge (const double d)
{
   if (this->isDefined) {
      // already have at least one value
      //
      this->minimum = MIN (this->minimum, d);
      this->maximum = MAX (this->maximum, d);
   } else {
      // use single value to "start things off".
      this->minimum = d;
      this->maximum = d;
      this->isDefined = true;
   }
}   // merge


//------------------------------------------------------------------------------
//
void TrackRange::merge (const TrackRange that)
{
   if ((this->isDefined) && (that.isDefined)) {
      this->minimum = MIN (this->minimum, that.minimum);
      this->maximum = MAX (this->maximum, that.maximum);
   } else {
      // only this or that or neither can be defined.
      if (that.isDefined) {
         this->isDefined = true;
         this->minimum = that.minimum;
         this->maximum = that.maximum;
      }
   }
}   // merge


//------------------------------------------------------------------------------
//
bool TrackRange::getMinMax (double & min, double& max) {
   min = this->minimum;
   max = this->maximum;
   return this->isDefined;
}   // getMinMax


//==============================================================================
//
class QCaStripChartItem::PrivateData {
public:
   PrivateData ();
   QCaStripChart *chart;
   QLabel *pvName;
   QCaLabel *caLabel;
};

QCaStripChartItem::PrivateData::PrivateData ()
{
   this->chart = NULL;
   this->pvName = NULL;
   this->caLabel = NULL;
}

//==============================================================================
//
QCaStripChartItem::QCaStripChartItem (QCaStripChart *chart,
                                      QLabel *pvName,
                                      QCaLabel *caLabel,
                                      unsigned int slot) : QObject (chart)
{
   QColor defaultColour;

   // construct private data for this chart.
   //
   this->privateData = new QCaStripChartItem::PrivateData ();

   // Store references to to widgets in private data class.
   //
   this->privateData->chart = chart;
   this->privateData->pvName = pvName;
   this->privateData->caLabel = caLabel;

   pvName->setText ("");
   pvName->setIndent (6);

   caLabel->setText ("-");
   caLabel->setIndent (6);

   if (slot < QCaStripChart::NUMBER_OF_PVS) {
      defaultColour = item_colours [slot];
   } else {
      defaultColour = clBlack;
   }
   this->setColour (defaultColour);

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

}   //QCaStripChartItem


//------------------------------------------------------------------------------
//
QCaStripChartItem::~QCaStripChartItem ()
{
   delete this->privateData;
}   //


//------------------------------------------------------------------------------
//
void QCaStripChartItem::clear ()
{
   this->privateData->pvName->setText ("");
   this->privateData->caLabel->setVariableNameAndSubstitutions ("", "", 0);
   this->privateData->caLabel->setText ("-");

   this->displayedMinMax.clear ();
   this->historicalMinMax.clear ();
   this->realTimeMinMax.clear ();
   this->historicalTimeDataPoints.clear ();
   this->realTimeDataPoints.clear ();

   this->privateData->chart->evaluateAllowDrop ();
}   // clear


//------------------------------------------------------------------------------
//
qcaobject::QCaObject* QCaStripChartItem::getQcaItem ()
{
   // We "know" that a QCaLabel has only one PV.
   //
   return this->privateData->caLabel->getQcaItem (0);
}

//------------------------------------------------------------------------------
//
void QCaStripChartItem::setPvName (QString pvName, QString substitutions)
{
   qcaobject::QCaObject *qca;

   // Clear any existing data and reset defaults.
   //
   this->clear ();

   // Verify caller attempting add a potentially sensible PV?
   //
   pvName = pvName.trimmed ();
   if (pvName == "") return;

   this->privateData->pvName->setText (pvName);
   this->privateData->caLabel->setVariableNameAndSubstitutions (pvName, substitutions, 0);

   // We know that QCaLabels use slot zero for the connection.
   //
   qca = this->getQcaItem ();
   if (qca) {
      QObject::connect (qca, SIGNAL (connectionChanged (QCaConnectionInfo&) ),
                        this,  SLOT (setDataConnection (QCaConnectionInfo&) ) );

      QObject::connect (qca, SIGNAL (dataChanged  (const QVariant&, QCaAlarmInfo&, QCaDateTime&) ),
                        this,  SLOT (setDataValue (const QVariant&, QCaAlarmInfo&, QCaDateTime&) ) );

   } else {
      DEBUG << "no qca object";
   }
}   // setPvName


//------------------------------------------------------------------------------
//
QString QCaStripChartItem::getPvName ()
{
   return this->privateData->pvName->text ();
}   // getPvName


//------------------------------------------------------------------------------
//
bool QCaStripChartItem::isInUse ()
{
   qcaobject::QCaObject *qca;

   qca = this->privateData->caLabel->getQcaItem (0);
   return (qca != NULL);
}   // isInUse


//------------------------------------------------------------------------------
//
TrackRange QCaStripChartItem::getLoprHopr ()
{
   TrackRange result;
   qcaobject::QCaObject *qca;
   double lopr;
   double hopr;

   result.clear ();

   if (this->isInUse ()) {
      qca = this->getQcaItem ();
      lopr = qca->getDisplayLimitLower ();
      hopr = qca->getDisplayLimitUpper ();

      // If either HOPR or LOPR are non zero - then range is deemed defined.
      //
      if ((lopr != 0.0) || (hopr != 0.0)) {
         result.merge (lopr);
         result.merge (hopr);
      }
   }
   return result;
}   // getLoprHopr


//------------------------------------------------------------------------------
//
TrackRange QCaStripChartItem::getDisplayedMinMax ()
{
   return this->displayedMinMax;
}   // getDisplayedMinMax


//------------------------------------------------------------------------------
//
TrackRange QCaStripChartItem::getBufferedMinMax ()
{
   TrackRange temp;

   temp = this->historicalMinMax;
   temp.merge (this->realTimeMinMax);

   return temp;
}   // getBufferedMinMax


//------------------------------------------------------------------------------
//
QwtPlotCurve * QCaStripChartItem::allocateCurve ()
{
   QwtPlotCurve * result;

   // Curves are managed by the chart widget.
   //
   result = this->privateData->chart->allocateCurve ();

   // Set curvepropeties plus item Pen which include its colour.
   //
   result->setRenderHint (QwtPlotItem::RenderAntialiased);
   result->setStyle (QwtPlotCurve::Lines);
   result->setPen (this->getPen ());

   return result;
}   // allocateCurve


//------------------------------------------------------------------------------
//
bool QCaStripChartItem::isDisplayable (QCaDataPoint & point)
{
   // The archive severity encompasses the normal EPICS severity.
   //
   QCaArchiveInterface::archiveAlarmSeverity severity = (QCaArchiveInterface::archiveAlarmSeverity) point.alarm.getSeverity ();
   bool result;

   switch (severity) {
   case QCaArchiveInterface::archSevNone:
   case QCaArchiveInterface::archSevMinor:
   case QCaArchiveInterface::archSevMajor:
   case QCaArchiveInterface::archSevEstRepeat:
   case QCaArchiveInterface::archSevRepeat:
      result = true;
      break;

   case QCaArchiveInterface::archSevInvalid:
   case QCaArchiveInterface::archSevDisconnect:
   case QCaArchiveInterface::archSevStopped:
   case QCaArchiveInterface::archSevDisabled:
      result = false;
      break;

   default:
      result = false;
      break;
   }

   return result;
}

//------------------------------------------------------------------------------
//
void QCaStripChartItem::plotDataPoints (const QCaDataPointList & dataPoints,
                                        const bool isRealTime,
                                        TrackRange & plottedTrackRange)
{
   const QDateTime end_time   = this->privateData->chart->getEndDateTime ();
   const double duration = this->privateData->chart->getDuration ();

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
         // Just save this point. Last time it is saved it will be the
         // pen-ultimate point before the chart start time.
         //
         previous = point;

         // Only "exists" if plottable.
         //
         doesPreviousExist = (previous.alarm.isInvalid () == false);
      }
      else if ((t >= -duration) && (t <= 0.0)) {

         // Is it a valid point - can we sensible plot it?
         //
         if (this->isDisplayable (point)) {
            // Yes we can.
            //
            // start edge effect required?
            //
            if (isFirstPoint && doesPreviousExist) {
                t = -duration;
                tdata.append (t);
                ydata.append (previous.value);
                plottedTrackRange.merge (previous.value);
            }

            // Do steps - do it like this as using qwt Step mode is not quite what I want.
            //
            if (ydata.count () >= 1) {
               tdata.append (t);
               ydata.append (ydata.last ());
            }

            tdata.append (t);
            ydata.append (point.value);
            plottedTrackRange.merge (point.value);

         } else {

            // plot what we have so far (need at least 2 points).
            //
            if (tdata.count () >= 1) {
               if (tdata.count () >= 2) {
                  curve = this->allocateCurve ();
                  curve->setSamples (tdata, ydata);
               }
               tdata.clear ();
               ydata.clear ();
            }
         }

         // We have processed at least one point now.
         //
         isFirstPoint = false;
      } else {
         // t > 0 - beyond the end time of the chart.
         // Nothing more to see here.
         break;
      }
   }

   // Start edge special required?
   //
   if (isFirstPoint && doesPreviousExist) {
       t = -duration;
       tdata.append (t);
       ydata.append (previous.value);
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
         tdata.append (0.0);
         ydata.append (ydata.last ());
         plottedTrackRange.merge (ydata.last ());
      }
      curve = this->allocateCurve ();
      curve->setSamples (tdata, ydata);
   }
}   // plotDataPoints


//------------------------------------------------------------------------------
//
void QCaStripChartItem::plotData ()
{
   TrackRange temp;

   this->displayedMinMax.clear ();

   this->plotDataPoints (this->historicalTimeDataPoints, false, temp);
   this->displayedMinMax.merge (temp);

   this->plotDataPoints (this->realTimeDataPoints, true, temp);
   this->displayedMinMax.merge (temp);
}

//------------------------------------------------------------------------------
//
void QCaStripChartItem::newVariableNameProperty (QString pvName, QString substitutions, unsigned int)
{
   this->setPvName (pvName, substitutions);

   // Re evaluate the chart drag drop allowed status.
   //
   this->privateData->chart->evaluateAllowDrop ();
}


//------------------------------------------------------------------------------
//
void QCaStripChartItem::setDataConnection (QCaConnectionInfo& connectionInfo)
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
         this->realTimeDataPoints.remove (0);
      }

      // create a dummy point with same time but marked invalid.
      //
      point.alarm = QCaAlarmInfo (NO_ALARM, INVALID_ALARM);
      this->realTimeDataPoints.append (point);
      if (this->realTimeDataPoints.count () > MAXIMUM_POINTS) {
         this->realTimeDataPoints.remove (0);
      }
   }
}   // setDataConnection


//------------------------------------------------------------------------------
//
void QCaStripChartItem::setDataValue (const QVariant& value, QCaAlarmInfo& alarm, QCaDateTime& datetime)
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

   point.datetime = datetime.toUTC ();
   point.datetime.nSec = datetime.nSec;

   if (this->isDisplayable (point)){
      this->realTimeMinMax.merge (point.value);
   }
   this->realTimeDataPoints.append (point);

   if (this->realTimeDataPoints.count () > MAXIMUM_POINTS) {
      this->realTimeDataPoints.remove (0);
   }
}   // setDataValue


//------------------------------------------------------------------------------
//
void QCaStripChartItem::setArchiveData (const QObject *userData, const bool okay,
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

         // Purge
         //
         last = count - 1;
         for (j = count - 1; j >= 0; j--) {
            point = this->historicalTimeDataPoints.value (j);
            pointTime = point.datetime;
            if (pointTime < firstRealTime) {
               last = j;
               break;
            }
         }

         // Keep points 0 to last, and modify modify (last + 1) if exists.
         //
         if (last < (count - 1)) {
            last++;
            point = this->historicalTimeDataPoints.value (last);
            point.datetime = firstRealTime;
            this->historicalTimeDataPoints.replace (last, point);
            this->historicalTimeDataPoints.remove (last + 1, count - last - 1);
         }

         this->historicalMinMax.clear ();
         count = this->historicalTimeDataPoints.count ();
         for (j = 0; j < count; j++) {
            point = this->historicalTimeDataPoints.value (j);
            if (this->isDisplayable(point)) {
               this->historicalMinMax.merge (point.value);
            }
         }
      }

   } else {
      DEBUG << "wrong item and/or data response not okay";
   }
}

//------------------------------------------------------------------------------
//
void QCaStripChartItem::readArchive ()
{
   const QDateTime startDateTime = this->privateData->chart->getStartDateTime ();
   const QDateTime endDateTime   = this->privateData->chart->getEndDateTime ();

   this->archiveAccess.readArchive
         (this, this->getPvName (),  startDateTime, endDateTime,  4000,
          QCaArchiveInterface::Linear,  0);
}

//------------------------------------------------------------------------------
//
void QCaStripChartItem::channelPropertiesClicked (bool)
{
   int n;

   this->dialog.setPvName (this->getPvName ());
   this->dialog.setColour  (this->getColour ());

   n = this->dialog.exec ();
   if (n == 1) {
      // User has selected okay.
      if (this->getPvName () != this->dialog.getPvName ()) {
         this->setPvName (this->dialog.getPvName (), "");
      }
      this->setColour (this->dialog.getColour ());
   }
}


//------------------------------------------------------------------------------
//
QString QCaStripChartItem::getStyle ()
{
   QString result;
   int r, g, b;
   bool white_text;

   r = this->colour.red ();
   g = this->colour.green ();
   b = this->colour.blue ();

   // Weight sum of background colour to detrmine if white ot black text.
   //
   white_text = ((2*r +3*g + 2*b) <= (7*102));   // 2+3+2 == 7

   result.sprintf ("QWidget { background-color: #%02x%02x%02x; color: %s ; }",
                    r, g, b, white_text ? "white" : "black" );
   return result;
}   // getStyle


//------------------------------------------------------------------------------
//
QColor QCaStripChartItem::getColour ()
{
   return this->colour;
}   // getColour

//------------------------------------------------------------------------------
//
void QCaStripChartItem::setColour (QColor colourIn)
{
   this->colour = colourIn;
   this->privateData->pvName->setStyleSheet (this->getStyle ());
}

//------------------------------------------------------------------------------
//
QPen QCaStripChartItem::getPen ()
{
   QPen result (this->getColour ());

   result.setWidth (1);
   return result;
}   // getPen

// end
