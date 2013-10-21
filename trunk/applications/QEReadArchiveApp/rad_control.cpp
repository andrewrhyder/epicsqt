/*  rad_control.cpp
 *
 *  Copyright (c) 2013
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#include <stdlib.h>

#include <iostream>

#include <QDebug>
#include <QDateTime>

#include <QECommon.h>
#include <QEArchiveInterface.h>
#include <QEFrameworkVersion.h>

#include <QEAdaptationParameters.h>
#include <QESettings.h>

#include "rad_control.h"

#define DEBUG qDebug () << "rad_control::" << __FUNCTION__ << __LINE__

namespace colour {
const static char* red    = "\033[31;1m";
const static char* yellow = "\033[33;1m";
const static char* reset  = "\033[00m";
}

static const QString stdFormat = "dd/MMM/yyyy HH:mm:ss";

//------------------------------------------------------------------------------
//
Rad_Control::Rad_Control () : QObject (NULL)
{
   this->options = new QEOptions ();

   this->useTimeZone = Qt::LocalTime;
   this->state = setup;
   this->timeout = 0;
   this->numberPVNames = 0;
   this->pvIndex = 0;

   this->tickTimer = new QTimer (this);
   QObject::connect (this->tickTimer, SIGNAL (timeout ()), this, SLOT (tickTimeout ()));

   this->tickTimer->start (50);  // mSes
}

//------------------------------------------------------------------------------
//
Rad_Control::~Rad_Control ()
{
   delete this->options;
}

//------------------------------------------------------------------------------
//
void Rad_Control::setTimeout (const double delay)
{
   double n = this->tickTimer->interval ();

   this->timeout = (int) ((1000.0 * delay + (n - 1.0)) / n);
   if (this->timeout < 1) this->timeout = 1;
}

//------------------------------------------------------------------------------
// A sort of state machine.
//
void Rad_Control::tickTimeout ()
{

//   qDebug () << this->state << this->timeout;
   switch (this->state) {

      case setup:
         this->state = errorExit;  // default next state
         this->initialise ();
         this->setTimeout (30.0);
         break;

      case watingReady:
         if (this->archiveAccess->isReady ()) {
            std::cout << "Archiver interface initialised\n";
            this->state = sendRequest;  // default next state
         } else {
            this->timeout--;
            if (this->timeout <= 0) {
               std::cerr << "Archiver interface initialise timeout\n";
               exit (1);
            }
         }
         break;

      case sendRequest:
         this->readArchive ();
         break;

      case waitResponse:
         this->timeout--;
         if (this->timeout <= 0) {
            std::cerr << "archive read timeout\n";
            exit (1);
         }
         break;

      case printAll:
         this->putArchiveData();
         this->state = allDone;
         break;

      case allDone:
         std::cout << "qerad complete\n";
         exit (0);
         break;

      case errorExit:
         std::cout << "qerad terminated\n";
         exit (1);
         break;

      default:
         std::cerr << "bad state:" << this->state << "\n";
         exit (4);
         break;
   }
}


//------------------------------------------------------------------------------
//
void Rad_Control::usage (const QString & message)
{
   std::cerr << message.toAscii().data() << "\n";
   Rad_Control::printFile (":/help/help_usage.txt", std::cerr);
   this->state = errorExit;
}


//------------------------------------------------------------------------------
//
void  Rad_Control::help ()
{
   Rad_Control::printFile (":/help/help_usage.txt",   std::cout);
   Rad_Control::printFile (":/help/help_general.txt", std::cout);
}

//------------------------------------------------------------------------------
//
QDateTime Rad_Control::value (const QString& timeImage, bool& okay)
{
   const QString formats [] = { "dd/MMM/yyyy HH:mm:ss",
                                "dd/MM/yyyy HH:mm:ss",
                                "dd/MMM/yyyy HH:mm",
                                "dd/MM/yyyy HH:mm",
                                "dd/MMM/yyyy HH",
                                "dd/MM/yyyy HH",
                                "dd/MMM/yyyy",
                                "dd/MM/yyyy" };

   QDateTime result;
   int j;
   QString image;

   okay = false;
   for (j = 0; j < ARRAY_LENGTH (formats); j++) {
      result = QCaDateTime::fromString (timeImage, formats [j]);
      image = result.toString (formats [0]);

//      qDebug () << j << timeImage << result << image << formats [j] ;

      if (!image.isEmpty()) {
         okay = true;
         break;
      }
   }
   result.setTimeSpec (this->useTimeZone);
   return result;
}


//------------------------------------------------------------------------------
//
void Rad_Control::initialise ()
{
   const QString format = "dd/MMM/yyyy HH:mm:ss";

   QString timeImage;
   bool okay;
   int j;
   QString pattern;
   QString pv;
   QString line;

   if (this->options->getBool ("help", 'h')) {
      this->help ();
      this->state = allDone;
      return;
   }

   if (this->options->getBool ("utc")) {
      this->useTimeZone = Qt::UTC;
   } else {
      this->useTimeZone = Qt::LocalTime;
   }

   if (this->options->getBool ("raw")) {
      this->how = QEArchiveInterface::Raw;
   } else {
      this->how = QEArchiveInterface::Linear;
   }


   this->useFixedTime = false;
   if (this->options->isSpecified ("fixed")) {
      // If the default value is returned assume error.
      //
      this->fixedTime = this->options->getFloat ("fixed", -99.0);
      if (this->fixedTime == -99.0) {
         std::cerr << colour::red << "error: fixed time has invalid format." << colour::reset << "\n";
         this->state = errorExit;
         return;
      } else {
         this->useFixedTime = true;
         if (this->fixedTime < 0.25) {
            this->fixedTime = 0.25;
            std::cout  << colour::yellow << "warning: fixed time limited to no less than 0.25 seconds" << colour::reset << "\n";
         }
      }
   }

   this->outputFile = this->options->getParameter (0);
   if (this->outputFile.isEmpty()) {
      this->usage ("missing output file");
      return;
   }

   timeImage = this->options->getParameter (1);
   this->startTime = this->value (timeImage, okay);
   if (!okay) {
      this->usage ("Invalid start time format. Valid example is 31/02/2013 16:30:00");
      return;
   }

   timeImage = this->options->getParameter (2);
   this->endTime = this->value (timeImage, okay);
   if (!okay) {
      this->usage ("Invalid end time format. Valid example is 27/May/2013 16:30:00");
      return;
   }


   pv = this->options->getParameter (3);
   if (pv.isEmpty()) {
      this->usage ("missing pv name");
      return;
   }

   this->pvDataList [0].pvName = pv;
   this->numberPVNames = 1;

   // Make a regular expression - exact match
   //
   pattern = "^" + pv + "$";

   for (j = 1; j < MaximumPVNames; j++) {
      pv = this->options->getParameter (j + 3);
      if (pv.isEmpty()) {
         break;
      }

      if (!this->useFixedTime) {
         // Multiple PVs - must use fiexd time.
         //
         this->useFixedTime = true;
         this->fixedTime = 1.0;
         std::cout  << colour::yellow << "warning: multiple PVs - auto selecting fixed time of 1.0 s" << colour::reset << "\n";
       }

      this->pvDataList [j].pvName = pv;
      this->numberPVNames = j + 1;

      pattern.append("|^").append (pv).append ("$");
   }

   line = "start time: ";
   line.append (this->startTime.toString(format));
   line.append (" ");
   line.append (QEUtilities::getTimeZoneTLA (this->startTime));
   std::cout << line.toAscii().data() << "\n";

   line = "end time:   ";
   line.append (this->endTime.toString(format));
   line.append (" ");
   line.append (QEUtilities::getTimeZoneTLA (this->endTime));
   std::cout << line.toAscii().data() << "\n";

   line = "pattern:   ";
   line.append (pattern);
   std::cout << line.toAscii().data() << "\n";


   QEArchiveAccess::initialise ("CR01ARC01:80/cgi-bin/ArchiveDataServer.cgi CR01ARC02:80/cgi-bin/ArchiveDataServer.cgi",
                                pattern);

   this->archiveAccess = new QEArchiveAccess ();

   QObject::connect (this->archiveAccess, SIGNAL (setArchiveData (const QObject *, const bool , const QCaDataPointList &) ),
                     this,                SLOT   (setArchiveData (const QObject *, const bool , const QCaDataPointList &)));

   this->state = watingReady;
   this->pvIndex = 0;
}


//------------------------------------------------------------------------------
//
void Rad_Control::readArchive ()
{
   QString pvName = this->pvDataList [this->pvIndex].pvName;


   this->archiveAccess->readArchive (this, pvName,
                                     this->startTime, this->endTime,
                                     20000,
                                     this->how, 0);

   std::cout << "\nArchiver request issued: "
             << pvName.toAscii ().data () << "\n";

   this->state = waitResponse;
   this->setTimeout (10.0);
}


//------------------------------------------------------------------------------
//
void Rad_Control::setArchiveData (const QObject *, const bool okay, const QCaDataPointList &archiveData)
{
   QString pvName = this->pvDataList [this->pvIndex].pvName;
   int number;


   number = archiveData.count ();

   std::cout << "Archiver response received: "
             << pvName.toAscii ().data ()
             << " status: " << (okay ? "okay" : "failed")
             <<  ", number of points: " << number   << "\n";

   this->pvDataList [this->pvIndex].isOkayStatus = okay;

   if (okay && this->useFixedTime) {
      std::cout << "resampling ...";

      if (this->numberPVNames == 1) {
         // Just do a simple resample.
         this->pvDataList [this->pvIndex].archiveData.resample (archiveData, this->fixedTime, this->endTime);
      } else {
         // All sets must start at the same time.
         //
         QCaDataPointList working;
         QCaDataPoint nullPoint;

         nullPoint.alarm = QCaAlarmInfo (0, (int) QEArchiveInterface::archSevInvalid);
         nullPoint.datetime = this->startTime;
         nullPoint.value = 0;

         working.clear ();
         working.append (nullPoint);
         working.append (archiveData);
         this->pvDataList [this->pvIndex].archiveData.resample (working, this->fixedTime, this->endTime);
      }

      number = this->pvDataList [this->pvIndex].archiveData.count ();
      std::cout << " resampled to " << number << " points.\n";
   } else {
      // Just copy
      this->pvDataList [this->pvIndex].archiveData = archiveData;
   }

   // Move onto next PV (if defined).
   //
   this->pvIndex++;

   if (pvIndex < this->numberPVNames) {
      this->state = sendRequest;  // fo next request
   } else {
      this->state =  printAll;
   }

}

//------------------------------------------------------------------------------
//
void Rad_Control::putDatumSet (QTextStream& target, QCaDataPoint p [], const int j, const QCaDateTime & firstTime)
{
   double relative;
   QCaDateTime time;
   QString zone;
   QString line;
   int n;
   bool valid;

   // Calculate the relative time from start.
   //
   relative = p [0].datetime.floating (firstTime);

   // Copy and covert to required time zone.
   //
   time = p [0].datetime;

   // Now set to the required time zone.
   //
   time = time.toTimeSpec (this->useTimeZone);

   zone = QEUtilities::getTimeZoneTLA (time);

   line = QString ("%1   %2 %3 %4 ")
         .arg (j, 6)
         .arg (time.toString (stdFormat), 20)
         .arg (zone)
         .arg (relative, 12, 'f', 3);

   for (n = 0; n < this->numberPVNames; n++) {
      valid = p [n].isDisplayable ();
      // f, 8   => 1.12345678e+00 = 8 + 6 => 14, so allow a couple spare.
      if (valid) {
         line.append (QString (" %1").arg (p [n].value, 16, 'e', 8));
      } else {
         line.append (QString (" %1").arg ("nil", 16));
      }
   }

   target << line  << "\n";
}

//------------------------------------------------------------------------------
//
void Rad_Control::putArchiveData ()
{
   QFile target_file (this->outputFile);

   int pv;
   int number;
   QCaDateTime firstTime;
   int j;
   QCaDataPoint point;

   std::cout << "\nOutputing data to file: " << this->outputFile.toAscii ().data () << "\n";

   if (!target_file.open (QIODevice::WriteOnly | QIODevice::Text)) {
      std::cerr << "open file failed\n";
      this->state = errorExit;
      return;
   }

   QTextStream target (&target_file);

   if (this->numberPVNames == 1) {

      QCaDataPointList* archiveData = &this->pvDataList [0].archiveData;

      number = archiveData->count ();
      if (number > 0 ) {

         firstTime = archiveData->value (0).datetime;

         target << "\n";
         target << "#   No  Time                          Relative Time             Value      Valid     Severity    Status\n";

         archiveData->toStream (target, true, true);
      }

   } else {
      // multiple PV outputFile
      //
      QCaDataPoint p_set [MaximumPVNames];
      QCaDataPoint nullPoint;

      nullPoint.alarm = QCaAlarmInfo (0, (int) QEArchiveInterface::archSevInvalid);

      firstTime = this->startTime;

      // Because of the way we re-sample the data - the number of points in
      // each data set should be the same, but just in case ....
      //
      number = 0;
      for (pv = 0 ; pv < this->numberPVNames; pv++) {
         QCaDataPointList* archiveData = &this->pvDataList [pv].archiveData;
         if (this->pvDataList [pv].isOkayStatus) {
            number = MAX (number, archiveData->count ());
         }
      }

      for (pv = 0 ; pv < this->numberPVNames; pv++) {
         target << QString ("# %1 %2").arg (pv,3).arg (this->pvDataList [pv].pvName) << "\n";
      }
      target << "\n";
      target << "#   No   Time                        Rel. Time    Values...\n";

      for (j = 0; j < number; j++) {
         for (pv = 0 ; pv < this->numberPVNames; pv++) {
            QCaDataPointList* archiveData = &this->pvDataList [pv].archiveData;

            if (this->pvDataList [pv].isOkayStatus) {
               if (j < archiveData->count ()) {
                  p_set [pv] = archiveData->value (j);
               } else {
                  p_set [pv] = nullPoint;
               }
            } else {
               p_set [pv] = nullPoint;
            }
         }
         this->putDatumSet (target, p_set, j, firstTime);
      }
   }

   target << "\n";
   target << "# end\n";

   target_file.close ();
}


//------------------------------------------------------------------------------
//
void Rad_Control::printFile (const QString& filename,
                             std::ostream& stream)
{
   QFile textFile (filename);

   if (!textFile.open (QIODevice::ReadOnly | QIODevice::Text)) {
      return;
   }

   QTextStream textStream( &textFile );
   QString text = textStream.readAll();
   textFile.close();

   stream << text.toAscii().data();
}



// end
