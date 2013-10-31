/* rad_control.h
 *
 * Copyright (c) 2013
 *
 * Author:
 *    Andrew Starritt
 * Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef RAD_CONTROL_H
#define RAD_CONTROL_H

#include <QObject>
#include <QString>
#include <QTimer>

#include <QCaDateTime.h>
#include <QCaDataPoint.h>
#include <QEArchiveInterface.h>
#include <QEArchiveManager.h>
#include <QEOptions.h>

class Rad_Control : QObject {
Q_OBJECT
public:
   Rad_Control ();
   ~Rad_Control ();

private:
   static const int MaximumPVNames = 20;

   struct PVData {
      QString pvName;
      bool isOkayStatus;
      int responseCount;
      QCaDataPointList archiveData;
   };

   enum States { setup,
                 waitArchiverReady,
                 initialiseRequest,
                 sendRequest,
                 waitResponse,
                 printAll,
                 allDone,
                 errorExit };

   PVData pvDataList [MaximumPVNames];
   int numberPVNames;

   Qt::TimeSpec useTimeZone;
   QEArchiveInterface::How how;
   bool useFixedTime;
   double fixedTime;

   QString outputFile;
   QCaDateTime startTime;
   QCaDateTime nextTime;
   QCaDateTime endTime;

   States state;
   int pvIndex;
   int timeout;

   QEOptions *options;
   QTimer* tickTimer;
   QEArchiveAccess * archiveAccess;

   void usage (const QString & message);
   void help ();

   void initialise ();
   void readArchive ();
   void postProcess (struct PVData* pvData);

   void putDatumSet (QTextStream& target, QCaDataPoint p [], const int j, const QCaDateTime & firstTime);
   void putArchiveData ();

   QDateTime value (const QString& s, bool& okay);

   void setTimeout (const double delay);

private slots:
   static void printFile (const QString&  filename,
                          std::ostream& stream);         // Print file to stream

   void tickTimeout ();
   void setArchiveData (const QObject* userData, const bool okay,
                        const QCaDataPointList& archiveData);

};

#endif  // RAD_CONTROL_H 
