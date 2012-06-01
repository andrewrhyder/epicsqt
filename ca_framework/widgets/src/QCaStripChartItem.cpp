/*  QCaStripChartItem.cpp
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
static const QColor clWhite (0xFF, 0xFF, 0xFF, 0xFF);


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
      // use single value to "stat things off".
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
   QColor defaultColor;

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
      defaultColor = item_colours [slot];
   } else {
      defaultColor = clBlack;
   }
   this->setColor (defaultColor);

   // Set up a connection to recieve variable name property changes.  The variable
   // name property manager class only delivers an updated variable name after the
   // user has stopped typing.
   //
   this->pvNameProperyManager.setVariableIndex (0);
   QObject::connect (&this->pvNameProperyManager, SIGNAL (newVariableNameProperty (QString, QString, unsigned int)),
                     this,                        SLOT   (newVariableNameProperty (QString, QString, unsigned int)) );
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

   this->displayedMinMax.clear ();
   this->bufferedMinMax.clear ();
   this->historicalTimeDataPoints.clear ();
   this->realTimeDataPoints.clear ();
}   // clear


//------------------------------------------------------------------------------
//
void QCaStripChartItem::setPvName (QString pvName, QString substitutions)
{
   qcaobject::QCaObject *qca;

   // Clear any existing data and reset defaults.
   //
   this->clear ();

   this->privateData->pvName->setText (pvName);
   this->privateData->caLabel->setVariableNameAndSubstitutions (pvName, substitutions, 0);

   // We know that QCaLabels use slot zero for the connection.
   //
   qca = this->privateData->caLabel->getQcaItem (0);
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
      qca = this->privateData->caLabel->getQcaItem (0);
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
   return this->bufferedMinMax;
}   // getBufferedMinMax


//------------------------------------------------------------------------------
//
QwtPlotCurve * QCaStripChartItem::allocateCurve ()
{
   QwtPlotCurve * result;
   // Curves are managed by the chart widget.
   //
   result = this->privateData->chart->allocateCurve ();

   // Set item Pen which include its colour.
   //
   result->setPen (this->getPen ());
   return result;
}   // allocateCurve


//------------------------------------------------------------------------------
//
void QCaStripChartItem::plotData ()
{
   const QDateTime start_time = this->privateData->chart->getStartTime ();
   const QDateTime end_time   = this->privateData->chart->getEndTime ();
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
   this->displayedMinMax.clear ();
   isFirstPoint = true;
   doesPreviousExist = false;

   count = this->realTimeDataPoints.count ();
   for (j = 0; j < count; j++) {
      point = this->realTimeDataPoints.value (j);

      if (point.datetime < start_time) {
         // Just save this point. Last time it is saved it will be the
         // pen-ultimate point before the chart start time.
         //
         previous = point;

         // Only "exists" if plottable.
         //
         doesPreviousExist = (previous.alarm.isInvalid () == false);
      }
      else if ((point.datetime >= start_time) &&
               (point.datetime <= end_time)) {

         // Is it a valid point - can we sensible plot it?
         //
         if (point.alarm.isInvalid () == false) {
            // Yes we can.
            //
            // start edge effect required?
            //
            if (isFirstPoint && doesPreviousExist) {
                t = -duration;
                tdata.append (t);
                ydata.append (previous.value);
                this->displayedMinMax.merge (previous.value);
            }

            // calculate relative time (in seconds) from the chart end time.
            //
            t = point.datetime.floating (end_time);

            // Do steps - do it like this as using qwt Step mode is not quite what I want.
            //
            if (ydata.count () >= 1) {
               tdata.append (t);
               ydata.append (ydata.last ());
            }

            tdata.append (t);
            ydata.append (point.value);
            this->displayedMinMax.merge (point.value);

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

         // We have processes at leat one point now.
         //
         isFirstPoint = false;
      }
   }

   // Start edge special required?
   //
   if (isFirstPoint && doesPreviousExist) {
       t = -duration;
       tdata.append (t);
       ydata.append (previous.value);
       this->displayedMinMax.merge (previous.value);
   }

   // End egde special required?
   //
   if (ydata.count () >= 1) {
      curve = this->allocateCurve ();

      // Replicate last value upto end of chart.
      //
      tdata.append (0.0);
      ydata.append (ydata.last ());
      this->displayedMinMax.merge (ydata.last ());

      curve->setSamples (tdata, ydata);
   }
}   // plotData


//------------------------------------------------------------------------------
//
void QCaStripChartItem::newVariableNameProperty (QString pvName, QString substitutions, unsigned int)
{
   this->setPvName (pvName, substitutions);

   // Re evaluate the chart drag drop allowed status.
   //
   this->privateData->chart->calcAllowDrop ();
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
      point.datetime = QDateTime::currentDateTime ();
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
      point.datetime = datetime;

      this->bufferedMinMax.merge (point.value);
   } else {
      // Could not convert to a double - mark as an invalid point.
      //
      point.value = 0.0;
      point.alarm = QCaAlarmInfo (NO_ALARM, INVALID_ALARM);
      point.datetime = datetime;
   }

   this->realTimeDataPoints.append (point);

   if (this->realTimeDataPoints.count () > MAXIMUM_POINTS) {
      this->realTimeDataPoints.remove (0);
   }
}   // setDataValue


//------------------------------------------------------------------------------
//
QString QCaStripChartItem::getStyle ()
{
   QString result;
   int r, g, b;
   bool white_text;

   r = this->color.red ();
   g = this->color.green ();
   b = this->color.blue ();

   // Weight sum of background colour to detrmine if white ot black text.
   //
   white_text = ((2*r +3*g + 2*b) <= (7*102));   // 2+3+2 == 7

   result.sprintf ("QWidget { background-color: #%02x%02x%02x; color: %s ; }",
                    r, g, b, white_text ? "white" : "black" );
   return result;
}   // getStyle


//------------------------------------------------------------------------------
//
QColor QCaStripChartItem::getColor ()
{
   return this->color;
}   // getColor

//------------------------------------------------------------------------------
//
void QCaStripChartItem::setColor (QColor colorIn)
{
   this->color = colorIn;
   this->privateData->pvName->setStyleSheet (this->getStyle ());
}

//------------------------------------------------------------------------------
//
QPen QCaStripChartItem::getPen ()
{
   QPen result (this->getColor ());

   result.setWidth (1);
   return result;
}   // getPen

// end
