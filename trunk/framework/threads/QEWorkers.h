/*  QEWorkers.h
 * 
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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
 *  Copyright (c) 2013
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef QEWORKERS_H
#define QEWORKERS_H

#include <QObject>
#include <QThread>

/*!
 * Proof of concept class - NOT currently in use.
 */

#define MAXIMUM_THREADS   16

class QEWorkerManager : public QObject {
   Q_OBJECT
public:
   typedef unsigned short Counts;
   typedef unsigned int SequenceNumbers;

   explicit QEWorkerManager (const Counts number, QObject *parant = 0);
   virtual ~QEWorkerManager ();

   void start ();   // no wait per se - use complete signal.

   void startAndWait (const double timeout, bool & complete);  // or timeout
   bool isComplete ();

   Counts getNumber();

signals:
   void complete ();

protected:
   // Derived classes should override this function.  The function in the derived
   // class must do the i-th part of the n-part total work where i is 0 to (n-1).
   //
   // Each call to processSubset is in the context of a separate thread.
   //
   // It is the responsibilty of the derived class's processSubset function to
   // ensure that all the instances of execution do not step on each others toes
   // (that's a technical expression ;-), and that any interaction between each
   // instance and the rest of the system is thread safe.
   //
   virtual void processSubset (const Counts i, const Counts n);

private:
   Counts number;
   SequenceNumbers sequenceNumber;

   // Internally we declare a number of WorkerThread pointer item.
   // Declareing these in the header of a Q Bject seems to make the Qt SDK
   // get very upset and confused, so need to hide this a little
   //
   class ReallyPrivate;
   ReallyPrivate * privateData;

   bool workerComplete [MAXIMUM_THREADS];

signals:
   void readyToGo (const QEWorkerManager::SequenceNumbers sequenceNumber);

private slots:
   void finished (const QEWorkerManager::Counts instance,
                  const QEWorkerManager::SequenceNumbers sequenceNumber);

   friend class WorkerThread;
};


//==============================================================================
//
class WorkerThread : public QThread
{
   Q_OBJECT

public:
   WorkerThread (const QEWorkerManager::Counts instanceIn, QEWorkerManager *managerIn);
   ~WorkerThread ();

   void run ();

private:
   QEWorkerManager::Counts instance;
   QEWorkerManager *manager;
   QEWorkerManager::SequenceNumbers sequenceNumber;

   void process ();

signals:
   void finished (const QEWorkerManager::Counts instance,
                  const QEWorkerManager::SequenceNumbers sequenceNumber);

private slots:
   void readyToGo (const QEWorkerManager::SequenceNumbers sequenceNumber);

   friend class QEWorkerManager;
};

#endif  // QEWORKERS_H
