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
   this->topItem = NULL;

   // The root item is a QTreeView/QAbstractItemModel artefact
   // Note this item does not/must not have a parent.
   // It is a place holder - not visible per se.
   // Note to be confused withe the user ROOT, aka topItem.
   //
   this->rootItem = new QEPvLoadSaveItem ("Origin", false, QVariant (QVariant::Invalid), NULL);
   this->heading = "";
}

//-----------------------------------------------------------------------------
//
QEPvLoadSaveModel::~QEPvLoadSaveModel ()
{
   if (this->rootItem) delete this->rootItem;
}

//-----------------------------------------------------------------------------
//
void QEPvLoadSaveModel::setupModelData (QEPvLoadSaveItem* topItemIn, const QString& headingIn)
{
   // Delete rootItem (which deletes any existing children) and then re-construct
   // the root - all easier than purging any children.
   //
   if (this->rootItem) {
      delete this->rootItem;
      this->rootItem = NULL;
      this->heading = "";
   }
   this->rootItem = new QEPvLoadSaveItem ("Origin", false, QVariant (QVariant::Invalid), NULL);

   this->topItem = topItemIn;
   if (this->topItem) {
      this->rootItem->appendChild (this->topItem);
      this->heading = headingIn;

      // topItem calls this resursively down the QEPvLoadSaveItem tree.
      //
      this->topItem->actionConnect (this, SLOT (acceptActionComplete (const QModelIndex&, QEPvLoadSaveCommon::ActionKinds, bool)));

      // The model calls this resursively down the index tree.
      //
      this->setModelIndex (this->topItem, 0, this->rootIndex);

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
void QEPvLoadSaveModel::setModelIndex (QEPvLoadSaveItem* item, int row, const QModelIndex &parentIndex)
{
   QModelIndex ownIndex;
   int childRow;
   QEPvLoadSaveItem* child;

   if (!item) {
      return;   // avoid any chance of a segmentation fault.
   }

   // Use createIndex directly (as opposed to index)???
   //
   ownIndex = this->index (row, 0, parentIndex);
   item->setModelIndex (ownIndex);

   // Now do chidren.
   //
   for (childRow = 0; childRow < item->childCount (); childRow++) {
       child = item->child (childRow);
       // Note: this is a recursive function call.
       this->setModelIndex (child, childRow, ownIndex);
   }
}

//-----------------------------------------------------------------------------
//
void QEPvLoadSaveModel::extractPVData ()
{
   if (this->topItem) this->topItem->extractPVData ();
}

//-----------------------------------------------------------------------------
//
void QEPvLoadSaveModel::applyPVData ()
{
   if (this->topItem) this->topItem->applyPVData ();
}

//-----------------------------------------------------------------------------
//
void QEPvLoadSaveModel::readArchiveData (const QCaDateTime& dateTime)
{
   if (this->topItem) this->topItem->readArchiveData (dateTime);
}

//-----------------------------------------------------------------------------
//
int QEPvLoadSaveModel::leafCount ()
{
   int result;
   if (this->topItem) {
      result = this->topItem->leafCount ();
   } else {
      result = 0;
   }

   return result;
}

//-----------------------------------------------------------------------------
//
void QEPvLoadSaveModel::acceptActionComplete (const QModelIndex& index, QEPvLoadSaveCommon::ActionKinds action, bool actionSuccessful)
{
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

//-----------------------------------------------------------------------------
//
QVariant QEPvLoadSaveModel::data (const QModelIndex& index, int role) const
{
   if (!index.isValid ()) {
      return QVariant ();
   }

   if (role != Qt::DisplayRole) {
      return QVariant ();
   }

   QEPvLoadSaveItem *item = this->getItem (index);

   return item->getData (index.column ());
}

//-----------------------------------------------------------------------------
//
Qt::ItemFlags QEPvLoadSaveModel::flags (const QModelIndex & index) const
{
   if (!index.isValid ()) {
      return 0;
   } else {
      return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
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
QModelIndex QEPvLoadSaveModel::index (int row, int column, const QModelIndex& parent) const
{
   if (!this->hasIndex (row, column, parent)) {
      return QModelIndex ();
   }

   QEPvLoadSaveItem *parentItem = this->getItem (parent);
   QEPvLoadSaveItem *childItem = parentItem->child (row);

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
   QEPvLoadSaveItem *parentItem = childItem->getParent ();

   if (parentItem == this->rootItem) {
      return QModelIndex ();
   }

   int row = parentItem->row ();
   return this->createIndex (row, 0, parentItem);
}

//-----------------------------------------------------------------------------
//
int QEPvLoadSaveModel::rowCount (const QModelIndex & parent) const
{
   if (parent.column () > 0) {
      return 0;
   }
   return this->getItem (parent)->childCount ();
}

//-----------------------------------------------------------------------------
//
int QEPvLoadSaveModel::columnCount (const QModelIndex & parent) const
{
   return this->getItem (parent)->columnCount ();
}

//-----------------------------------------------------------------------------
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
   QEPvLoadSaveItem* result;

   if (index.isValid()) {
      result = static_cast <QEPvLoadSaveItem *>(index.internalPointer ());
   } else {
      // invalid - must be the root item.
      result = this->rootItem;
   }
   return result;
}

// end
