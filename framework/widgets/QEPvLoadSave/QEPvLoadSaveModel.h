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
 *  Copyright (c) 2013
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

// This class is an almost direct crib of example specified in:
// http://qt-project.org/doc/qt-4.8/itemviews-simpletreemodel.html
//
class QEPvLoadSaveModel : public QAbstractItemModel {

Q_OBJECT

public:
   explicit QEPvLoadSaveModel (QEPvLoadSave* parent);
   virtual ~QEPvLoadSaveModel ();

   void setupModelData (QEPvLoadSaveItem* topItem, const QString& heading);
   void modelUpdated ();

   // Request each item to perform read, write or access archive.
   //
   void extractPVData ();
   void applyPVData ();
   void readArchiveData (const QCaDateTime& dateTime);
   int leafCount ();


   // Override (pure abstract) virtual functions.
   //
   QVariant data       (const QModelIndex &index, int role = Qt::DisplayRole) const;
   Qt::ItemFlags flags (const QModelIndex &index) const;
   QVariant headerData (int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
   QModelIndex index   (int row, int column,
                        const QModelIndex &parent = QModelIndex()) const;
   QModelIndex parent  (const QModelIndex &child) const;
   int rowCount        (const QModelIndex &parent = QModelIndex()) const;
   int columnCount     (const QModelIndex &parent = QModelIndex()) const;


   QModelIndex getRootIndex () { return this->rootIndex; }
   QEPvLoadSaveItem* getTopItem () { return this->topItem; }

   // If index is invalid, then returns null.
   //
   QEPvLoadSaveItem* indexToItem (const QModelIndex& index) const;

signals:
   void reportActionComplete (QEPvLoadSaveCommon::ActionKinds, bool);

private:
   // Like indexToItem but returns rootItem if index is invalid.
   //
   QEPvLoadSaveItem* getItem (const QModelIndex &index) const;

   QModelIndex rootIndex;       // is an invlaid index
   QEPvLoadSaveItem *rootItem;  // the tree view root - must exist - provides headings
   QEPvLoadSaveItem *topItem;   // top (or data root) item, could be null but unlikely.
                                // (we could get first child index of rootIndex and then extract item)
   QString heading;

   void setModelIndex (QEPvLoadSaveItem* item, int row, const QModelIndex &parentIndex);

private slots:
   void acceptActionComplete (const QModelIndex&, QEPvLoadSaveCommon::ActionKinds, bool);
};

#endif   // QEPVLOADSAVEMODEL_H
