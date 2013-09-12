/*  QEPvLoadSaveModel.cpp
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
#include <QList>
#include <QVariant>

#include "QEPvLoadSave.h"
#include "QEPvLoadSaveItem.h"
#include "QEPvLoadSaveModel.h"

#define DEBUG  qDebug () << "QEPvLoadSaveModel::" << __FUNCTION__ << ":" << __LINE__

//-----------------------------------------------------------------------------
//
QEPvLoadSaveModel::QEPvLoadSaveModel (QEPvLoadSave* parent) : QAbstractItemModel (parent)
{
   // Save calling parameter
   //
   this->requestedInsertItem = NULL;

   // The core item is a QTreeView/QAbstractItemModel artefact
   // Note this item does not/must not have a parent.
   // It is a place holder - not visible per se.
   // Note to be confused withe the user ROOT item.
   //
   this->coreItem = new QEPvLoadSaveItem ("Core", false, QVariant (QVariant::Invalid), NULL);
   this->heading = "";
}

//-----------------------------------------------------------------------------
//
QEPvLoadSaveModel::~QEPvLoadSaveModel ()
{
   if (this->coreItem) delete this->coreItem;
}

//-----------------------------------------------------------------------------
//
void QEPvLoadSaveModel::setupModelData (QEPvLoadSaveItem* rootItem, const QString& headingIn)
{
   // Removed exisiting user root item (not model core item).
   //
   QEPvLoadSaveItem* oldRootItem = this->getRootItem ();
   if (oldRootItem) {
      this->removeItemFromModel (oldRootItem);
      oldRootItem = NULL;
      this->heading = "";
   }

   if (rootItem) {
      this->heading = headingIn;
      this->addItemToModel (rootItem, this->coreItem);

      // rootItem calls this resursively down the QEPvLoadSaveItem tree.
      //
      rootItem->actionConnect (this, SLOT (acceptActionComplete (const QEPvLoadSaveItem*, QEPvLoadSaveCommon::ActionKinds, bool)));
      this->modelUpdated ();
   }
}

//-----------------------------------------------------------------------------
//
void QEPvLoadSaveModel::modelUpdated ()
{
   emit this->layoutChanged ();
}

//-----------------------------------------------------------------------------
//
bool QEPvLoadSaveModel::addItemToModel (QEPvLoadSaveItem* item, QEPvLoadSaveItem* parentItem)
{
   bool result = false;

   // sanity checks: item must exist and specified parent.
   //
   if (item && parentItem) {
      QModelIndex parentIndex= this->getIndex (parentItem);
      int number = parentItem->childCount ();

      // Save reference item - we use this in insertRows.
      //
      this->requestedInsertItem = item;

      // We always append items.
      //
      result = this->insertRow (number, parentIndex);

      this->requestedInsertItem = NULL;   // remove dangling reference.

   }
   return result;
}

//-----------------------------------------------------------------------------
//
bool QEPvLoadSaveModel::removeItemFromModel (QEPvLoadSaveItem* item)
{
   bool result = false;

   // sanity checks: item must exist and specified parent.
   //
   if (item) {
      QEPvLoadSaveItem* parentItem = item->getParent ();

      if (parentItem) {
         QModelIndex pi = this->getIndex (parentItem);
         int row = item->childPosition ();

         if (row >= 0) {
            result = this->removeRow (row, pi);
         } else {
            DEBUG << "fail  row" << row;
         }
      } else {
         DEBUG << "fail  no parent";
      }
   }
   return result;
}

//-----------------------------------------------------------------------------
//
void QEPvLoadSaveModel::extractPVData ()
{
   // core always exists, and it will find root if it exists.
   //
   this->coreItem->extractPVData ();
}

//-----------------------------------------------------------------------------
//
void QEPvLoadSaveModel::applyPVData ()
{
   this->coreItem->applyPVData ();
}

//-----------------------------------------------------------------------------
//
void QEPvLoadSaveModel::readArchiveData (const QCaDateTime& dateTime)
{
   this->coreItem->readArchiveData (dateTime);
}

//-----------------------------------------------------------------------------
//
int QEPvLoadSaveModel::leafCount ()
{
   return this->coreItem->leafCount ();
}

//-----------------------------------------------------------------------------
//
QEPvLoadSaveItem* QEPvLoadSaveModel::getRootItem ()
{
   return this->coreItem->getChild (0);
}

//-----------------------------------------------------------------------------
//
void QEPvLoadSaveModel::acceptActionComplete (const QEPvLoadSaveItem* item, QEPvLoadSaveCommon::ActionKinds action, bool actionSuccessful)
{
   QModelIndex index = this->getIndex (item);

   switch (action) {
      case QEPvLoadSaveCommon::Extract:
      case QEPvLoadSaveCommon::ReadArchive:
         emit this->dataChanged (index, index);  // this causes tree view to update
         break;

      case QEPvLoadSaveCommon::Apply:
         // no change per se - do nothing.
         break;
   }

   emit this->reportActionComplete (action, actionSuccessful);
}

//=============================================================================
// Overriden model functions
//=============================================================================
//
QVariant QEPvLoadSaveModel::data (const QModelIndex& index, int role) const
{
   if (!index.isValid ()) {
      return QVariant ();
   }

   if (role != Qt::DisplayRole && role != Qt::EditRole) {
      return QVariant ();
   }

   QEPvLoadSaveItem *item = this->getItem (index);

   return item->getData (index.column ());
}

//-----------------------------------------------------------------------------
//
bool QEPvLoadSaveModel::setData (const QModelIndex&, const QVariant&, int)
{
   DEBUG << " ==========  UNEXPECTED  ===========";
   return false;
}


//-----------------------------------------------------------------------------
//
Qt::ItemFlags QEPvLoadSaveModel::flags (const QModelIndex & index) const
{
   if (!index.isValid ()) {
      return 0;
   } else {
      return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
   }
}

//-----------------------------------------------------------------------------
//
QVariant QEPvLoadSaveModel::headerData (int section, Qt::Orientation orientation, int role) const
{
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
       switch (section) {
       case 0:  return QVariant (this->heading);
       default: return QVariant ("");
       }
   }

   return QVariant ();
}

//-----------------------------------------------------------------------------
//

bool QEPvLoadSaveModel::setHeaderData (int, Qt::Orientation, const QVariant&, int)
{
   DEBUG << " ==========  UNEXPECTED  ===========";
   return false;
}

//-----------------------------------------------------------------------------
//
QModelIndex QEPvLoadSaveModel::index (int row, int column, const QModelIndex& parent) const
{
   if (parent.isValid() && parent.column() != 0) {
        return QModelIndex();
   }

   QEPvLoadSaveItem *parentItem = this->getItem (parent);
   QEPvLoadSaveItem *childItem = parentItem->getChild (row);

   if (childItem) {
      return this->createIndex (row, column, childItem);
   } else {
      return QModelIndex ();
   }
}

//-----------------------------------------------------------------------------
//
QModelIndex QEPvLoadSaveModel::parent (const QModelIndex & child) const
{
   if (!child.isValid ()) {
      return QModelIndex ();
   }

   QEPvLoadSaveItem *childItem = this->getItem (child);
   if (!childItem) {
      DEBUG << "null childItem " << child;
   }
   QEPvLoadSaveItem *parentItem = childItem->getParent ();

   if (parentItem == this->coreItem) {
      return QModelIndex ();
   }

   if (parentItem) {
      int row = parentItem->childPosition ();
      if (row >= 0) {
         return this->createIndex (row, 0, parentItem);
      }
   }

   DEBUG << " ========== UNEXPECTED ===========";
   return QModelIndex ();
}

//-----------------------------------------------------------------------------
//
int QEPvLoadSaveModel::rowCount (const QModelIndex & parent) const
{
   QEPvLoadSaveItem* parentItem = this->getItem (parent);

   if (parentItem) return  parentItem->childCount();

   DEBUG << "========== no parent";
   return 0;
}

//-----------------------------------------------------------------------------
//
int QEPvLoadSaveModel::columnCount (const QModelIndex& /* parent */ ) const
{
   return this->coreItem->columnCount ();
}

//-----------------------------------------------------------------------------
//
bool QEPvLoadSaveModel::insertRows (int position, int rows, const QModelIndex& parent)
{
   QEPvLoadSaveItem *parentItem = this->getItem (parent);
   bool success = false;

   // insertRows - ensure we can deal with this request.
   //
   if (this->requestedInsertItem && rows == 1) {
      this->beginInsertRows (parent, position, position);
      success = parentItem->insertChild (position, this->requestedInsertItem);
      this->endInsertRows ();
   }
   return success;
}

//-----------------------------------------------------------------------------
//
bool QEPvLoadSaveModel::removeRows (int position, int rows, const QModelIndex& parent)
{
   QEPvLoadSaveItem *parentItem = this->getItem (parent);
   bool success = true;
   int last = position + rows - 1;

   this->beginRemoveRows (parent, position, last);
   success = parentItem->removeChildren (position, rows);
   this->endRemoveRows ();

   return success;
}

//=============================================================================
// Utility function to hide the nasty static cast and stuff.
//
QEPvLoadSaveItem* QEPvLoadSaveModel::indexToItem (const QModelIndex& index) const
{
   QEPvLoadSaveItem* result = NULL;

   if (index.isValid ()) {
      result = static_cast <QEPvLoadSaveItem *>(index.internalPointer ());
   }
   return result;
}

//-----------------------------------------------------------------------------
// Utility function to hide the nasty static cast and stuff.
//
QEPvLoadSaveItem *QEPvLoadSaveModel::getItem (const QModelIndex &index) const
{
   QEPvLoadSaveItem* result = this->coreItem;

   if (index.isValid ()) {
       QEPvLoadSaveItem* temp;
       temp = static_cast <QEPvLoadSaveItem *>(index.internalPointer ());
      if (temp) result = temp;
   }
   // If not set then still the coreItem.
   return result;
}

//-----------------------------------------------------------------------------
//
QModelIndex QEPvLoadSaveModel::getIndex (const QEPvLoadSaveItem* item)
{
   QModelIndex result;  // invalid by default (which is really getCoreIndex)

   if (item == this->coreItem) {
      result = this->getCoreIndex ();
   } else
   if (item) {
      int row = item->childPosition ();
      if (row >= 0) {
         result = this->createIndex (row, 0, (QEPvLoadSaveItem*)item);
      }
   }
   return result;
}

// end
