/*  QEPvLoadSaveModel.h
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
 *  Copyright (c) Australian Synchrotron 2013
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef QEPVLOADSAVEMODEL_H
#define QEPVLOADSAVEMODEL_H

#include <QAbstractItemModel>
#include <QObject>

#include <QCaDateTime.h>
#include <QEPvLoadSaveCommon.h>

// Differed declaration - avoids mutual header inclusions.
//
class QEPvLoadSave;
class QEPvLoadSaveItem;

/// This class is based on the TreeModel example specified in:
/// http://qt-project.org/doc/qt-4.8/itemviews-editabletreemodel.html
///
///  Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
///  Contact: http://www.qt-project.org/legal
///
/// Note on naming: the example's root item that provide header info
/// is refered to the core item. It only ever has one child which is the
/// visible (on the QTreeView) root item, named "ROOT".
///
/// Note: we only re-size the number of rows (children). The
/// number of columns is fixed.
///
class QEPvLoadSaveModel : public QAbstractItemModel {
Q_OBJECT
public:
   explicit QEPvLoadSaveModel (QEPvLoadSave* parent = 0);
   virtual ~QEPvLoadSaveModel ();

   // Override (pure abstract) virtual functions.
   //
   QVariant data       (const QModelIndex& index, int role) const;
   QVariant headerData (int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

   QModelIndex index   (int row, int column,
                        const QModelIndex& parent = QModelIndex()) const;
   QModelIndex parent  (const QModelIndex& child) const;

   int rowCount        (const QModelIndex& parent = QModelIndex()) const;
   int columnCount     (const QModelIndex& parent = QModelIndex()) const;

   // Read-only tree models only need to provide the above functions.
   // The following public functions provide support for editing and resizing.
   //
   Qt::ItemFlags flags (const QModelIndex &index) const;

   bool setData  (const QModelIndex &index, const QVariant &value,
                  int role = Qt::EditRole);
   bool setHeaderData  (int section, Qt::Orientation orientation,
                        const QVariant &value, int role = Qt::EditRole);
   bool insertRows     (int position, int rows,
                        const QModelIndex& parent = QModelIndex());
   bool removeRows     (int position, int rows,
                        const QModelIndex& parent = QModelIndex());

   // We do not override insertColumns/removeColumns - this is fixed

   //----------------------------------------------------------------------------------
   // Own functions, i.e. not from the example.
   //
   void setupModelData (QEPvLoadSaveItem* rootItem, const QString& heading);
   void modelUpdated ();

   bool addItemToModel (QEPvLoadSaveItem* item, QEPvLoadSaveItem* parentItem);
   bool removeItemFromModel (QEPvLoadSaveItem* item);

   // Request each item to perform read, write or access archive.
   //
   void extractPVData ();
   void applyPVData ();
   void readArchiveData (const QCaDateTime& dateTime);
   int leafCount ();
   QEPvLoadSaveItem* getRootItem ();

   QModelIndex getRootIndex ()      { return this->index (0, 0, this->getCoreIndex ()); }

   // If index is invalid, then returns null.
   // This is like getItem but for external use.
   //
   QEPvLoadSaveItem* indexToItem (const QModelIndex& index) const;

signals:
   void reportActionComplete (QEPvLoadSaveCommon::ActionKinds, bool);

private:
   // The model index associated with the core item is an invalid index.
   // (as per the URL ref).
   //
   QModelIndex getCoreIndex ()      { return QModelIndex (); }

   // Like indexToItem but returns coreItem if index is invalid.
   //
   QEPvLoadSaveItem* getItem (const QModelIndex &index) const;

   QModelIndex getIndex (const QEPvLoadSaveItem* item);

   QEPvLoadSaveItem *coreItem;  // the tree view root (as opposed to user root) - must exist - provides headings
   QString heading;             // heading text.

   QEPvLoadSaveItem *requestedInsertItem;   //

private slots:
   void acceptActionComplete (const QEPvLoadSaveItem*, QEPvLoadSaveCommon::ActionKinds, bool);
};

#endif   // QEPVLOADSAVEMODEL_H
