/*  QEScratchPadMenu.cpp
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
#include <QEScratchPadMenu.h>
#include <QECommon.h>

#define DEBUG  qDebug () << "QEScratchPadMenu::" <<  __FUNCTION__  << ":" << __LINE__


//------------------------------------------------------------------------------
//
QEScratchPadMenu::QEScratchPadMenu (const int slotIn, QWidget* parent) : QMenu (parent)
{
   this->slot = slotIn;

   this->setTitle ("ScratchPad Item");

   this->make (this, "Add PV Name...",   false, SCRATCHPAD_ADD_PV_NAME);
   this->make (this, "Paste PV Name",    false, SCRATCHPAD_PASTE_PV_NAME);
   this->make (this, "Edit PV Name...",  false, SCRATCHPAD_EDIT_PV_NAME);
   this->make (this, "Clear",            false, SCRATCHPAD_DATA_CLEAR);


   QObject::connect (this, SIGNAL (triggered             (QAction* ) ),
                     this, SLOT   (contextMenuTriggered  (QAction* )));
}

//------------------------------------------------------------------------------
//
QEScratchPadMenu::~QEScratchPadMenu ()
{
}

//------------------------------------------------------------------------------
//
void QEScratchPadMenu::setIsInUse (const bool isInUse)
{
   this->setActionVisible (SCRATCHPAD_ADD_PV_NAME,   !isInUse);
   this->setActionVisible (SCRATCHPAD_PASTE_PV_NAME, !isInUse);
   this->setActionVisible (SCRATCHPAD_EDIT_PV_NAME,  isInUse);
   this->setActionVisible (SCRATCHPAD_DATA_CLEAR,    isInUse);
}

//------------------------------------------------------------------------------
//
void QEScratchPadMenu::setActionEnabled (const ContextMenuOptions option,
                                         const bool visible)
{
   const int t =  option - ContextMenuItemFirst;
   if (t >= 0 && t < ARRAY_LENGTH (this->actionList)) {
      this->actionList [t]->setEnabled (visible);
   }
}

//------------------------------------------------------------------------------
//
void QEScratchPadMenu::setActionVisible (const ContextMenuOptions option,
                                         const bool visible)
{
   const int t =  option - ContextMenuItemFirst;
   if (t >= 0 && t < ARRAY_LENGTH (this->actionList)) {
      this->actionList [t]->setVisible (visible);
   }
}

//------------------------------------------------------------------------------
//
QAction* QEScratchPadMenu::make (QMenu* parent,
                                 const QString& caption,
                                 const bool checkable,
                                 const QEScratchPadMenu::ContextMenuOptions option)
{
   QAction* action;

   action = new QAction (caption, parent);
   action->setCheckable (checkable);
   action->setData (QVariant (int (option)));
   parent->addAction (action);

   this->actionList [option - ContextMenuItemFirst] = action;
   return action;
}

//------------------------------------------------------------------------------
//
void QEScratchPadMenu::contextMenuTriggered (QAction* selectedItem)
{
   bool okay;
   QEScratchPadMenu::ContextMenuOptions option;

   option = QEScratchPadMenu::ContextMenuOptions (selectedItem->data ().toInt (&okay));

   if (okay &&
       (option >= QEScratchPadMenu::ContextMenuItemFirst) &&
       (option <= QEScratchPadMenu::ContextMenuItemLast)) {
      emit this->contextMenuSelected (this->slot, option);
   }
}

// end
