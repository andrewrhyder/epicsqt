/*  QEPvLoadSaveItem.cpp
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

#include <QDebug>
#include <QFrame>
#include <QModelIndex>
#include <QPushButton>
#include <QWidget>

#include "QEPvLoadSave.h"
#include "QEPvLoadSaveItem.h"

#define DEBUG  qDebug () << "QEPvLoadSaveItem::" << __FUNCTION__ << ":" << __LINE__

//=============================================================================
//
QEPvLoadSaveItem::QEPvLoadSaveItem (const QString & nodeNameIn,
                                    const bool isPVIn,
                                    const QVariant& valueIn,
                                    QEPvLoadSaveItem *parent) : QObject (parent)
{
   this->nodeName = nodeNameIn;
   this->isPV = isPVIn;
   this->value = valueIn;

   this->index = QModelIndex ();

   this->updateItemData ();

   if (parent) {
      parent->appendChild (this);
   }

   this->isConnected = false;
   this->qca = NULL;
   this->archiveAccess = NULL;

   // Is this a leaf node, i.e. a PV ?
   // Note: we do not enforce rule that leaf and only leaf items are PVs.
   //
   if (isPV) {
      // Interpret node name as a PV name.
      //
      this->qca = new qcaobject::QCaObject (this->nodeName, this);

      // QCaObject does not do this auotmatically. Maybe it should?.
       //
      qca->setParent (this);

      this->connect (this->qca, SIGNAL (connectionChanged (QCaConnectionInfo& )),
                     this,      SLOT   (connectionChanged (QCaConnectionInfo& )));

      this->connect (this->qca, SIGNAL (dataChanged (const QVariant&, QCaAlarmInfo& , QCaDateTime& )),
                     this,      SLOT   (dataChanged (const QVariant&, QCaAlarmInfo& , QCaDateTime& )));


      this->archiveAccess = new QEArchiveAccess (this);

      this->connect (this->archiveAccess, SIGNAL (setArchiveData ( const QObject*, const bool, const QCaDataPointList&)),
                     this,                SLOT   (setArchiveData ( const QObject*, const bool, const QCaDataPointList&)));

   }
}

//-----------------------------------------------------------------------------
//
QEPvLoadSaveItem::~QEPvLoadSaveItem ()
{
}

//-----------------------------------------------------------------------------
//
void QEPvLoadSaveItem::setModelIndex (const QModelIndex& indexIn)
{
   this->index = indexIn;
}

//-----------------------------------------------------------------------------
//
QString QEPvLoadSaveItem::getNodeName ()
{
   return this->nodeName;
}

//-----------------------------------------------------------------------------
//
void QEPvLoadSaveItem::updateItemData ()
{
   QString valueImage;

   if (this->isPV) {
      valueImage = " = ";
      if (this->value.type() == QVariant::List) {
         QVariantList vl = this->value.toList ();
         valueImage.append (QString (" << %1 element array >>").arg (vl.size ()));
      } else {
         valueImage.append (this->value.toString ());
      }
   } else {
      valueImage = "";
   }

   this->itemData.clear();
   this->itemData << (this->nodeName + valueImage);
}

//-----------------------------------------------------------------------------
//
void QEPvLoadSaveItem::actionConnect (QObject* actionCompleteObject, const char* actionCompleteSlot)
{
   if (this->isPV) {
      QObject::connect (this, SIGNAL (reportActionComplete (const QModelIndex&, QEPvLoadSaveCommon::ActionKinds, bool)),
                        actionCompleteObject, actionCompleteSlot);
   } else {
      for (int j = 0; j < this->childItems.count(); j++) {
          QEPvLoadSaveItem* item = this->child (j);
          if (item) item->actionConnect (actionCompleteObject, actionCompleteSlot);
      }
   }
}

//-----------------------------------------------------------------------------
//
void QEPvLoadSaveItem::extractPVData ()
{
    if (this->isPV) {
       if (this->qca && isConnected)  {
          bool status;
          status = qca->singleShotRead ();
          if (!status) {
             emit this->reportActionComplete (this->index, QEPvLoadSaveCommon::Extract, false);
          }
       } else {
          emit this->reportActionComplete (this->index, QEPvLoadSaveCommon::Extract, false);
       }

    } else {
        for (int j = 0; j < this->childItems.count(); j++) {
            QEPvLoadSaveItem* item = this->child (j);
            if (item) item->extractPVData ();
        }
    }
}

//-----------------------------------------------------------------------------
//
void QEPvLoadSaveItem::applyPVData ()
{
   if (this->isPV) {
      if (this->qca && isConnected)  {
         bool status;
         status = qca->writeData (this->value);
         emit this->reportActionComplete (this->index, QEPvLoadSaveCommon::Apply, status);
      } else {
         emit this->reportActionComplete (this->index, QEPvLoadSaveCommon::Apply, false);
      }

   } else {
       for (int j = 0; j < this->childItems.count(); j++) {
           QEPvLoadSaveItem* item = this->child (j);
           if (item) item->applyPVData ();
       }
   }
}

//-----------------------------------------------------------------------------
//
void QEPvLoadSaveItem::readArchiveData (const QCaDateTime& dateTime)
{
   if (this->isPV) {
      if (this->archiveAccess) {
         this->archiveAccess->readArchive (this, this->getNodeName (),
                                           dateTime, dateTime, 1,
                                           QEArchiveInterface::Linear, 0);
      } else {
         //
      }
   } else {
       for (int j = 0; j < this->childItems.count(); j++) {
           QEPvLoadSaveItem* item = this->child (j);
           if (item) item->readArchiveData (dateTime);
       }
   }
}

//-----------------------------------------------------------------------------
//
int QEPvLoadSaveItem::leafCount ()
{
   int result;

   if (this->isPV) {
      result = 1;
   } else {
       result = 0;
       for (int j = 0; j < this->childItems.count(); j++) {
           QEPvLoadSaveItem* item = this->child (j);
           if (item) result += item->leafCount ();
       }
   }
   return result;
}

//-----------------------------------------------------------------------------
//
void QEPvLoadSaveItem::connectionChanged (QCaConnectionInfo& connectionInfo)
{
   this->isConnected = connectionInfo.isChannelConnected ();
}

//-----------------------------------------------------------------------------
//
void  QEPvLoadSaveItem::dataChanged (const QVariant& valueIn, QCaAlarmInfo& alarmInfoIn, QCaDateTime&)
{
   //
   this->value = valueIn;
   this->alarmInfo = alarmInfoIn;
   this->updateItemData ();
   emit this->reportActionComplete (this->index, QEPvLoadSaveCommon::Extract, true);
}

//-----------------------------------------------------------------------------
//
void QEPvLoadSaveItem::setArchiveData (const QObject*, const bool okay, const QCaDataPointList & dataPointList)
{
   if (okay && dataPointList.count() > 0) {
      QCaDataPoint item = dataPointList.value (0);

      this->value = QVariant (item.value);
      this->alarmInfo = item.alarm;
      this->updateItemData ();

      emit this->reportActionComplete (this->index, QEPvLoadSaveCommon::ReadArchive, true);
   } else {
      emit this->reportActionComplete (this->index, QEPvLoadSaveCommon::ReadArchive, false);
   }
}

//-----------------------------------------------------------------------------
//
void QEPvLoadSaveItem::appendChild (QEPvLoadSaveItem *child)
{
   // Check for duplicates???
   //
   this->childItems.append (child);

   // Ensure consistency.
   //
   if (child->parent() != this) {
       child->setParent (this);
   }
}

//-----------------------------------------------------------------------------
// Each item is able to return any of its child items when given a suitable row
// number.
//
QEPvLoadSaveItem *QEPvLoadSaveItem::child (int row)
{
   return this->childItems.value (row);
}

//-----------------------------------------------------------------------------
//
int QEPvLoadSaveItem::childCount () const
{
   return this->childItems.count ();
}

//-----------------------------------------------------------------------------
//
int QEPvLoadSaveItem::columnCount () const
{
   return this->itemData.count ();
}

//-----------------------------------------------------------------------------
//
QVariant QEPvLoadSaveItem::getData (int column) const
{
   return this->itemData.value (column);
}

//-----------------------------------------------------------------------------
// The TreeModel uses this function to determine the number of rows that exist
// for a given parent item. The row() function reports the item's location
// within its parent's list of items:
//
int QEPvLoadSaveItem::row () const
{
   const QEPvLoadSaveItem * parentItem = this->getParent ();

   if (parentItem) {
      // avoid error: invalid conversion from 'const QEPvLoadSaveItem* const' to 'QEPvLoadSaveItem*'
      //
      return parentItem->childItems.indexOf (const_cast <QEPvLoadSaveItem *>(this));
   }
   
   // Note that, although the root item (with no parent item) is automatically
   // assigned a row number of 0, this information is never used by the model.
   //
   return 0;
}

//-----------------------------------------------------------------------------
//
QEPvLoadSaveItem * QEPvLoadSaveItem::getParent () const
{
   return (QEPvLoadSaveItem *) this->parent ();
}

// end
