/*  QEScratchPad.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2013 Australian Synchrotron
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef QE_SCRATCH_PAD_H
#define QE_SCRATCH_PAD_H

#include <QMenu>
#include <QObject>
#include <QTimer>
#include <QVariant>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>

#include <QEFrame.h>
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
   ~QEScratchPad ();
   QSize sizeHint () const;

   static const int NUMBER_OF_ITEMS = 48;

   // Set (and clear if pvName is null).
   //
   void    setPvName (const int slot, const QString& pvName);
   QString getPvName (const int slot) const;


   // Selects/highlights row.
   //
public slots:
   void setSelection (int value);

public:
   int getSelection () const;

signals:
   void selectionChanged (int value);

   // Set, get and emit set of active PV names.
   //
public slots:
   void setPvNameSet (const QStringList& pvNameSet);

public:
   QStringList getPvNameSet () const;

signals:
   void pvNameSetChanged (const QStringList& nameSet);

protected:
   // Overtide super class functions.
   //
   void activated ();

   // Drag and Drop - no drop to self.
   //
   void mousePressEvent (QMouseEvent *event)    { qcaMousePressEvent (event); }
   void dragEnterEvent (QDragEnterEvent *event) { qcaDragEnterEvent (event, false); }
   void dragMoveEvent (QDragMoveEvent *event);  // we do a special to avoid self drop.
   void dropEvent (QDropEvent *event)           { qcaDropEvent (event, true); }
   // This widget uses the setDrop/getDrop defined in QEWidget.

   // Copy/Paste
   //
   QString copyVariable ();
   QVariant copyData ();
   void paste (QVariant s);

   // override pure virtual functions
   //
   void saveConfiguration (PersistanceManager* pm);
   void restoreConfiguration (PersistanceManager* pm, restorePhases restorePhase);

   void showEvent (QShowEvent* event);
   void keyPressEvent (QKeyEvent* event);
   bool eventFilter (QObject* obj, QEvent* event);

   int findSlot (QObject *obj) const;

private:
   // Internal widgets.
   //
   QVBoxLayout* vLayout;
   QFrame* titleFrame;
   QLabel* titlePvName;
   QLabel* titleDescription;
   QLabel* titleValue;
   QLabel* titleSpacer;    // corresponds to the vertical scroll bar

   QHBoxLayout* titleLayout;
   QScrollArea* scrollArea;
   QWidget*     scrollContents;
   QVBoxLayout* scrollLayout;   // manages BaseDataSets::frame items in scrollContents

   QEPVNameSelectDialog* pvNameSelectDialog;

   int selectedItem;
   bool emitSelectionChangeInhibited;
   bool emitPvNameSetChangeInhibited;

   class DataSets {
   public:
      explicit DataSets ();

      bool isInUse () const { return !(this->thePvName.isEmpty ()); }
      void setHighLighted (const bool isHigh);

      QString thePvName;
      bool isHighLighted;

      // Widgets.
      //
      QHBoxLayout* hLayout;
      QFrame* frame;
      QLabel* pvName;
      QELabel* description;
      QELabel* value;
      QEScratchPadMenu* menu;
   };

   DataSets items [NUMBER_OF_ITEMS];

   void createInternalWidgets ();
   void setSelectItem (const int slot, const bool toggle);
   int numberSlotsUsed () const;  // includes cleared slots
   void calcMinimumHeight ();

   // Perform a pvNameDropEvent 'drop' when dropped onto internall widget.
   //
   void pvNameDropEvent (const int slot, QDropEvent* event);
   void addPvName (const QString& pvName);

private slots:
   void contextMenuRequested (const QPoint& pos);
   void contextMenuSelected  (const int slot, const QEScratchPadMenu::ContextMenuOptions option);
};

#endif // QE_SCRATCH_PAD_H
