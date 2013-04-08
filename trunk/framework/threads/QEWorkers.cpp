/*  QEWorkers.cpp
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

#include <QEWorkers.h>
#include <QThread>
#include <QDebug>

#include "QEWorkers.h"


#define DEBUG  qDebug () << __FILE__ << "::" << __FUNCTION__  << ":" << __LINE__ 


//==============================================================================
//
WorkerThread::WorkerThread
   (const QEWorkerManager::Counts instanceIn,
    QEWorkerManager * managerIn) : QThread (managerIn)
{
   this->instance = instanceIn;
   this->manager = managerIn;
}

//------------------------------------------------------------------------------
//
WorkerThread::~WorkerThread ()
{
   // nothing required - just a place holder.
}

//------------------------------------------------------------------------------
//
void WorkerThread::run ()
{
   // Cross connect manager and worker threads.
   //
   connect (this->manager, SIGNAL (readyToGo (const QEWorkerManager::SequenceNumbers)),
            this, SLOT            (readyToGo (const QEWorkerManager::SequenceNumbers)));

   connect (this, SIGNAL        (finished (const QEWorkerManager::Counts, const QEWorkerManager::SequenceNumbers)),
            this->manager, SLOT (finished (const QEWorkerManager::Counts, const QEWorkerManager::SequenceNumbers)));

   // And start event loop for this thread.
   //
   this->exec ();
}

//------------------------------------------------------------------------------
//
void WorkerThread::process ()
{
   if (this->manager) {
      // This is a dispatching call/hook function.
      //
      this->manager->processSubset (this->instance,
                                    this->manager->getNumber ());
   }
}

//------------------------------------------------------------------------------
//
void WorkerThread::readyToGo (const QEWorkerManager::SequenceNumbers sequenceNumberIn)
{
   this->sequenceNumber = sequenceNumberIn;
   this->process ();
   emit this->finished (this->instance, this->sequenceNumber);
}


//==============================================================================
//
class QEWorkerManager::ReallyPrivate {
public:
   ReallyPrivate (const QEWorkerManager::Counts number, QEWorkerManager * manager)
   {
      int j;

      for (j = 0; j < number; j++) {
         this->threadList[j] = new WorkerThread (j, manager);
         this->threadList[j]->start ();

      } for (j = number; j < MAXIMUM_THREADS; j++) {
         this->threadList[j] = NULL;
      }
   }

private:
   // NOTE: These objects are owned/parented by the QEWorkerManager.
   //
   WorkerThread * threadList[MAXIMUM_THREADS];
};


//==============================================================================
//
QEWorkerManager::QEWorkerManager (const Counts numberIn,
                                  QObject * parent) : QObject (parent)
{
   this->number = numberIn;

   // Ensure this is sensible.
   if (this->number > MAXIMUM_THREADS) {
      this->number = MAXIMUM_THREADS;
   }

   if (this->number < 1) {
      this->number = 1;
   }

   this->sequenceNumber = 0;
   this->privateData = new ReallyPrivate (this->number, this);
}

//------------------------------------------------------------------------------
//
QEWorkerManager::~QEWorkerManager ()
{
   // The WorkerThread are owned by this so will be automatically deleted.

   delete this->privateData;
}

//------------------------------------------------------------------------------
//
QEWorkerManager::Counts QEWorkerManager::getNumber ()
{
   return this->number;
}

//------------------------------------------------------------------------------
//
void QEWorkerManager::finished (const Counts instance,
                                const QEWorkerManager::SequenceNumbers workerSequenceNumber)
{
   if (workerSequenceNumber == this->sequenceNumber) {
      this->workerComplete[instance] = true;
      // All done??
      //
      if (this->isComplete()) {
         emit this->complete();
      }
   } else {
      DEBUG << "sequenceNumber mismatch, "
            << "instance"   << instance
            << "returned"   << workerSequenceNumber
            << ", expected" <<this->sequenceNumber;
      // otherwise ignore for now.
   }
}

//------------------------------------------------------------------------------
//
void QEWorkerManager::start ()
{
   int j;

   this->sequenceNumber++;

   for (j = 0; j < this->getNumber (); j++) {
      this->workerComplete[j] = false;
   }

   emit this->readyToGo (this->sequenceNumber);
}


//------------------------------------------------------------------------------
//
void QEWorkerManager::startAndWait (const double timeout, bool & complete)
{
   const unsigned long delay = 10;   // mSecs

   unsigned long j;
   unsigned long k;

   complete = false;
   this->start ();
   k = (unsigned long) ((1000.0 * timeout) / delay);
   if (k < 1) k = 1;
   for (j = 0; (j < k) && (!complete); j++) {
      // A bit of a cheat - as the manager class is a friend class of the worker
      // it can see the inherited protected msleep function.
      //
      WorkerThread::msleep (delay);
      complete = this->isComplete ();
   }
}

//------------------------------------------------------------------------------
//
bool QEWorkerManager::isComplete ()
{
   bool result;
   int j;

   result = true;               //hypothesize okay.
   for (j = 0; j < this->getNumber (); j++) {
      if (!this->workerComplete[j]) {
         result = false;
         break;
      }
   }

   return result;
}

//------------------------------------------------------------------------------
//
void QEWorkerManager::processSubset (const Counts i, const Counts  n)
{
   DEBUG <<  "(" << i << "," << n << ") - this should be overriden";
}

// end
