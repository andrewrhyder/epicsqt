/*  QEScratchPad.h
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

#ifndef QESCRATCHPAD_H
#define QESCRATCHPAD_H

#include <QMenu>
#include <QObject>
#include <QTimer>
#include <QVariant>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QEFrame.h>
#include <QEForm.h>
#include <QELabel.h>
#include <QCaVariableNamePropertyManager.h>
#include <persistanceManager.h>

#include <QEPVNameSelectDialog.h>
#include "QEScratchPadMenu.h"

/// This class provides a flexible scratch pad form, to which any Process Variable
/// may be added. It displays the PV Name, the Description, i.e. the content of
/// the DESC field together with the value of PV itself.
///
/// This class is a direct re-implementation of TScratch_Pad_Form out of the
/// Delphi OPI framework.
///
class QEPLUGINLIBRARYSHARED_EXPORT QEScratchPad : public QEFrame {
   Q_OBJECT
public:
    explicit QEScratchPad (QWidget *parent = 0);
    explicit QEScratchPad (bool isDefault, QWidget *parent = 0) : QEFrame (parent){ Q_UNUSED(isDefault);}
   ~QEScratchPad();
   QSize sizeHint () const;

   static const int NUMBER_OF_ITEMS = 48;

   // Set (and clear is pvName is null).
   //
   virtual void    setPvName (const int slot, const QString& pvName);
   QString getPvName (const int slot);

protected:
   // Overtide super class functions.
   //
   // Paste only
   //
   void paste (QVariant s);

   // override pure virtual functions
   //
   void saveConfiguration (PersistanceManager* pm);
   void restoreConfiguration (PersistanceManager* pm, restorePhases restorePhase);

   bool eventFilter (QObject *obj, QEvent *event);
   int findSlot (QObject *obj);

   // Internal widgets.
   //
   QVBoxLayout* vLayout;
   QFrame* titleFrame;
   QLabel* titlePvName;
   QLabel* titleDescription;
   QLabel* titleValue;
   QHBoxLayout* titleLayout;

   QEPVNameSelectDialog* pvNameSelectDialog;
   QMenu* widgetContextMenu;

   int selectedItem;

   class BaseDataSets : QObject {
   public:
       explicit BaseDataSets (): QObject() {}

       bool isInUse () { return !(this->thePvName.isEmpty ()); }
       virtual void setHighLighted (const bool isHigh) = 0;

       QString thePvName;
       bool isHighLighted;
       QHBoxLayout* hLayout;
       QFrame* frame;
       QLabel* pvName;
       QEScratchPadMenu* menu;
   };

   class DataSets : public BaseDataSets {
   public:
      explicit DataSets ();

      void setHighLighted (const bool isHigh);

      bool isHighLighted;

      // Widgets.
      QELabel* description;
      QELabel* value;
   };

   // Slot 0 used for X data - some redundancy (e.g. colour)
   //
   BaseDataSets *items [NUMBER_OF_ITEMS];

   virtual void createInternalWidgets ();
   void selectItem (const int slot, const bool toggle);
   virtual void calcMinimumHeight ();

   // Perform a pvNameDropEvent 'drop'
   //
   void pvNameDropEvent (const int slot, QDropEvent *event);

   void addPvName (const QString& pvName);
   void addPvNameSet (const QString& pvNameSet);

protected slots:
   void contextMenuRequested (const QPoint& pos);
   virtual void contextMenuSelected  (const int slot, const QEScratchPadMenu::ContextMenuOptions option);

   void widgetMenuRequested (const QPoint& pos);
   void widgetMenuSelected  (QAction* action);

};

#endif // QESCRATCHPAD_H
