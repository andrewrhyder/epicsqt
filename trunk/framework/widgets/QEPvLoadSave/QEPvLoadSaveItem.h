/*  QEPvLoadSaveItem.h
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

#ifndef QEPVLOADSAVEITEM_H
#define QEPVLOADSAVEITEM_H

#include <QList>
#include <QModelIndex>
#include <QObject>
#include <QString>
#include <QTreeView>
#include <QVariant>
#include <QVariantList>
#include <QModelIndex>

#include <QCaObject.h>
#include <QCaDataPoint.h>
#include <QEArchiveManager.h>
#include <QEPvLoadSaveCommon.h>

// This class is based on the of example specified in:
// http://qt-project.org/doc/qt-4.8/itemviews-simpletreemodel.html
//
// A major difference is that it is derived from QObject because each leaf item
// is associated with a PV and needs a slot to recieve value data.
// A consequence of this is that this class must be exposed in a header file even though
// it is essentially a QEPvLoadSave private class.
//
// Also the constructor automatically makes itself a modelchild QEPvLoadSaveItem
// of its parent. This augments the normal QObject parent child linking.
//
class QEPvLoadSaveItem : public QObject {
Q_OBJECT
public:
   explicit QEPvLoadSaveItem (const QString& nodeName,
                              const bool isPV,   // as opposed to isGroup
                              const QVariant& value,
                              QEPvLoadSaveItem *parent);
   virtual ~QEPvLoadSaveItem ();

   // Set own model index - used for data changed signals.
   //
   void setModelIndex (const QModelIndex& index);

   void actionConnect (QObject* actionCompleteObject, const char* actionCompleteSlot);

   // no set node name - it's fixed by the contructor.
    //
   QString getNodeName ();

   void appendChild (QEPvLoadSaveItem *child);
   QEPvLoadSaveItem *child (int row);
   int childCount () const;
   int columnCount () const;
   QVariant getData (int column) const;
   int row () const;
   QEPvLoadSaveItem *getParent () const;

   // If this is a leaf (PV) item then performs action on associated qca channel.
   // If this a group item then command is re-issued to each child.
   //
   void extractPVData ();
   void applyPVData ();
   void readArchiveData (const QCaDateTime& dateTime);

   // Count of number of PV leaf items at or below this node.
   // (As opposed to childCount which is number of direct children).
   //
   int leafCount ();

signals:
   void reportActionComplete (const QModelIndex& index,
                              QEPvLoadSaveCommon::ActionKinds action,
                              bool actionSuccessful);

private:
   QModelIndex index;    // own index
   QString nodeName;     // alias for first item in itemData
   bool isPV;

   QVariant value;
   QCaAlarmInfo alarmInfo;

   QList<QVariant> itemData;

   // We keep and maintain a separate list of QEPvLoadSaveItem children, as
   // opposed to using the QObject children mechanism, as QEPvLoadSaveItem
   // objects have other children such as a qcaobject::QCaObject.
   //
   QList<QEPvLoadSaveItem*> childItems;

   bool isConnected;
   qcaobject::QCaObject *qca;
   QEArchiveAccess * archiveAccess;

   void updateItemData ();

private slots:
   void connectionChanged (QCaConnectionInfo& connectionInfo);
   void dataChanged (const QVariant& value, QCaAlarmInfo& alarmInfo, QCaDateTime& timeStamp);

   void setArchiveData (const QObject* userData, const bool okay, const QCaDataPointList& archiveData);
};

#endif    // QEPVLOADSAVEITEM_H
