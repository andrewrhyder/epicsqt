/*  QEScratchPad.cpp
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
#include <QColor>

#include <QECommon.h>
#include <QEPvPropertiesUtilities.h>

#include "QEScratchPad.h"

#define DEBUG qDebug() << "QEScratchPad::" << __FUNCTION__ << ":" << __LINE__

static const QColor clHighLight (0xFFFFFF);
static const QColor clInUse     (0xE8E8E8);
static const QColor clNotInUse  (0xC8C8C8);
static const QColor clSelected  (0x7090FF);

static const int NULL_SELECTION = -1;


//=================================================================================
// QEScratchPad
//=================================================================================
//
void QEScratchPad::createInternalWidgets ()
{
   const int frameHeight = 19;
   const int horMargin = 2;    // 19 - 2 - 2 => widget height is 15
   const int horSpacing = 12;
   const int indent = 6;

   int slot;

   // Main layout.
   //
   this->vLayout = new QVBoxLayout (this);
   this->vLayout->setMargin (2);
   this->vLayout->setSpacing (1);

   this->titleFrame = new QFrame (this);
   this->titleFrame->setFixedHeight (frameHeight);

   this->titlePvName = new QLabel ("PV Name", this->titleFrame );
   this->titlePvName->setIndent (indent);

   this->titleDescription = new QLabel ("Description", this->titleFrame );
   this->titleDescription->setIndent (indent);

   this->titleValue = new QLabel ("Value", this->titleFrame );
   this->titleValue->setIndent (indent);

   this->titleLayout = new QHBoxLayout (this->titleFrame);
   this->titleLayout->setMargin (horMargin);
   this->titleLayout->setSpacing (horSpacing);

   this->titleLayout->addWidget (this->titlePvName);
   this->titleLayout->addWidget (this->titleDescription);
   this->titleLayout->addWidget (this->titleValue);
   this->vLayout->addWidget (this->titleFrame);

   for (slot = 0; slot < ARRAY_LENGTH (this->items); slot++) {

      DataSets* ds = &(this->items [slot]);

      ds->menu = new QEScratchPadMenu (slot, this);

      ds->frame = new QFrame (this);
      ds->frame->setFixedHeight (frameHeight);
      ds->frame->setAcceptDrops (true);
      ds->frame->installEventFilter (this);
      ds->frame->setContextMenuPolicy (Qt::CustomContextMenu);

      ds->pvName = new QLabel (ds->frame);
      ds->pvName->installEventFilter (this);
      ds->pvName->setText ("");
      ds->pvName->setIndent (indent);
      ds->pvName->setStyleSheet (QEUtilities::colourToStyle (clNotInUse));

      ds->description = new QELabel (ds->frame);
      ds->description->setDisplayAlarmState (false);
      ds->description->setText ("");
      ds->description->setIndent (indent);
      ds->description->setStyleSheet (QEUtilities::colourToStyle (clNotInUse));

      ds->value = new QELabel (ds->frame);
      ds->value->setDisplayAlarmState (true);
      ds->value->setText ("");
      ds->value->setIndent (indent);
      ds->value->setStyleSheet (QEUtilities::colourToStyle (clNotInUse));

      // Set up layout - paramers must be same as titlelayout
      //
      ds->hLayout = new QHBoxLayout (ds->frame);
      ds->hLayout->setMargin (horMargin);
      ds->hLayout->setSpacing (horSpacing);

      // Add to layouts
      //
      ds->hLayout->addWidget (ds->pvName);
      ds->hLayout->addWidget (ds->description);
      ds->hLayout->addWidget (ds->value);

      this->vLayout->addWidget (ds->frame);

      QObject::connect (ds->frame, SIGNAL (customContextMenuRequested (const QPoint &)),
                        this,      SLOT   (contextMenuRequested (const QPoint &)));

      QObject::connect (ds->menu, SIGNAL (contextMenuSelected (const int, const QEScratchPadMenu::ContextMenuOptions)),
                        this,     SLOT   (contextMenuSelected (const int, const QEScratchPadMenu::ContextMenuOptions)));
   }

   this->vLayout->addStretch ();

   this->dataDialog = new QEScratchPadItemDialog (this);
}


//=================================================================================
// DataSets
//=================================================================================
//
QEScratchPad::DataSets::DataSets () {
   this->thePvName = "";
   this->isHighLighted = false;
}

//---------------------------------------------------------------------------------
//
void QEScratchPad::DataSets::setHighLighted (const bool isHighLightedIn)
{
   QString styleSheet;

   // Can only set/remove high ligt if not in use.
   //
   if (!this->isInUse()) {
      this->isHighLighted = isHighLightedIn;
      if (this->isHighLighted) {
         styleSheet = QEUtilities::colourToStyle (clHighLight);
      } else {
         styleSheet = QEUtilities::colourToStyle (clNotInUse);
      }

      this->pvName->setStyleSheet (styleSheet);
      this->description->setStyleSheet (styleSheet);
      this->value->setStyleSheet (styleSheet);
   }
}

//=================================================================================
// QEScratchPad
//=================================================================================
//
QEScratchPad::QEScratchPad (QWidget* parent) : QEFrame (parent)
{
   this->createInternalWidgets ();

   this->setNumVariables (0);

   // Configure the panel.
   //
   this->setFrameShape (QFrame::StyledPanel);
   this->setFrameShadow (QFrame::Raised);

   this->setMinimumWidth (800);
   this->calcMinimumHeight ();

   this->selectedItem = NULL_SELECTION;

   this->setAllowDrop (false);
   this->setDisplayAlarmState (false);
}

//---------------------------------------------------------------------------------
//
QEScratchPad::~QEScratchPad ()
{
   // place holder
}

//------------------------------------------------------------------------------
//
QSize QEScratchPad::sizeHint () const {
   return QSize (800, 50);
}

//---------------------------------------------------------------------------------
// Slot range checking macro function.
// Set default to nil for void functions.
//
#define SLOT_CHECK(slot, default) {                                   \
   if ((slot < 0) || (slot >= ARRAY_LENGTH (this->items))) {          \
      DEBUG << "slot out of range: " << slot;                         \
      return default;                                                 \
   }                                                                  \
}

//---------------------------------------------------------------------------------
//
int QEScratchPad::findSlot (QObject *obj)
{
   int result = -1;
   int slot;

   for (slot = 0 ; slot < ARRAY_LENGTH (this->items); slot++) {
      if ((obj == this->items [slot].frame) ||
          (obj == this->items [slot].pvName)) {
         // found it.
         //
         result = slot;
         break;
      }
   }

   return result;
}

//---------------------------------------------------------------------------------
//
void QEScratchPad::calcMinimumHeight ()
{
   int slot;
   int last;
   int count;
   int delta_top;

   // Find last used item.
   //
   last = -1;
   for (slot = 0; slot < ARRAY_LENGTH (this->items); slot++) {
      if (this->items [slot].isInUse ()) {
         last = slot;
      }
   }

   // Allow one spare at end of widget if there is room.
   //
   if (last  < ARRAY_LENGTH (this->items) - 1) last++;

   // Set visibility accordingly
   //
   for (slot = 0; slot < ARRAY_LENGTH (this->items); slot++) {
      this->items [slot].frame->setVisible (slot <= last);
      if (slot <= last) {
      }
   }

   // Allow +1 for titles and +1 for zero indexed nature of slot and last.
   //
   count = 2 + last;

   delta_top = 20;
   this->setMinimumHeight ((delta_top * count) + 10);
}


//--------::-------------------------------------------------------------------------
//
void QEScratchPad::selectItem (const int slot, const bool toggle)
{
   const int previousSelection = this->selectedItem;

   if (slot != NULL_SELECTION) SLOT_CHECK (slot,);

   if (toggle) {
      if (this->selectedItem == slot) {
         this->selectedItem = NULL_SELECTION;
      } else {
         this->selectedItem = slot;
      }
   } else {
      this->selectedItem = slot;
   }

   if (this->selectedItem != previousSelection) {
      if (previousSelection != NULL_SELECTION) {
         DataSets* ds = &(this->items [previousSelection]);
         ds->frame->setStyleSheet ("");
      }

      if (this->selectedItem != NULL_SELECTION) {
         DataSets* ds = &(this->items [this->selectedItem]);
         QString styleSheet = QEUtilities::colourToStyle (clSelected);
         ds->frame->setStyleSheet (styleSheet);
      }
   }
}

//---------------------------------------------------------------------------------
//
void QEScratchPad::contextMenuRequested (const QPoint& pos)
{
   QObject *obj = this->sender();   // who sent the signal.
   const int slot = this->findSlot (obj);
   SLOT_CHECK (slot,);


   QWidget* w = dynamic_cast<QWidget*> (obj);
   if (w) {
      DataSets* ds = &(this->items [slot]);
      QPoint golbalPos = w->mapToGlobal(pos);
      ds->menu->setIsInUse (ds->isInUse ());
      ds->menu->exec (golbalPos, 0);
   }
}

//---------------------------------------------------------------------------------
//
void QEScratchPad::contextMenuSelected (const int slot, const QEScratchPadMenu::ContextMenuOptions option)
{
   SLOT_CHECK (slot,);
   int n;

   switch (option) {
      case QEScratchPadMenu::SCRATCHPAD_PASTE_PV_NAME:
         {
            QClipboard* cb = QApplication::clipboard ();
            QString pasteText = cb->text().trimmed();

            if (! pasteText.isEmpty()) {
               this->setPvName (slot, pasteText);
            }
         }
         break;

      case QEScratchPadMenu::SCRATCHPAD_ADD_PV_NAME:
      case QEScratchPadMenu::SCRATCHPAD_EDIT_PV_NAME:
         this->dataDialog->setFieldInformation (this->getPvName (slot));
         n = this->dataDialog->exec ();
         if (n == 1) {
            QString newData;
            this->dataDialog->getFieldInformation (newData);
            this->setPvName (slot, newData);
         }
         break;

      case QEScratchPadMenu::SCRATCHPAD_DATA_CLEAR:
         this->setPvName (slot, "");
         break;

      default:
         DEBUG << slot <<  option;
         break;
   }
}

//---------------------------------------------------------------------------------
//
void QEScratchPad::pvNameDropEvent (const int slot, QDropEvent *event)
{
   SLOT_CHECK (slot,);

   // If no text available, do nothing
   //
   if (!event->mimeData()->hasText ()){
      event->ignore ();
      return;
   }

   // Get the drop data
   //
   const QMimeData *mime = event->mimeData ();

   // If there is any text, drop the text

   if (!mime->text().isEmpty ()) {
      // Get the component textual parts
      //
      QStringList pieces = mime->text ().split (QRegExp ("\\s+"),
                                                QString::SkipEmptyParts);

      // Carry out the drop action
      //
      this->setPvName (slot, pieces.value (0, ""));
   }

   // Tell the dropee that the drop has been acted on
   //
   if (event->source() == this) {
      event->setDropAction(Qt::CopyAction);
      event->accept();
   } else {
      event->acceptProposedAction ();
   }
}

//---------------------------------------------------------------------------------
//
bool QEScratchPad::eventFilter (QObject *obj, QEvent *event)
{
   const QEvent::Type type = event->type ();
   QMouseEvent* mouseEvent = NULL;
   int slot;

   switch (type) {
      case QEvent::MouseButtonPress:
         mouseEvent = static_cast<QMouseEvent *> (event);
         slot = this->findSlot (obj);
         if (slot >= 0 && (mouseEvent->button () ==  Qt::LeftButton)) {
            this->selectItem (slot, true);
            return true;  // we have handled this mouse press
         }
         break;

      case QEvent::MouseButtonDblClick:
         slot = this->findSlot (obj);
         if (slot >= 0) {
            // Leverage of menu handler
            this->selectItem (slot, false);
            this->contextMenuSelected (slot, QEScratchPadMenu::SCRATCHPAD_ADD_PV_NAME);
            return true;  // we have handled double click
         }
         break;

      case QEvent::DragEnter:
         slot = this->findSlot (obj);
         if (slot >= 0) {
            QDragEnterEvent* dragEnterEvent = static_cast<QDragEnterEvent*> (event);

            // Can only drop if text and not in use.
            //
            if ((dragEnterEvent->mimeData()->hasText ()) &&
                (!this->items [slot].isInUse ())) {
               dragEnterEvent->setDropAction (Qt::CopyAction);
               dragEnterEvent->accept ();
               this->items [slot].setHighLighted (true);
            } else {
               dragEnterEvent->ignore ();
               this->items [slot].setHighLighted (false);
            }
            return true;
         }
         break;

      case QEvent::DragLeave:
         slot = this->findSlot (obj);
         if (slot >= 0) {
            this->items [slot].setHighLighted (false);
            return true;
         }
         break;


      case QEvent::Drop:
         slot = this->findSlot (obj);
         if (slot >= 0) {
            QDropEvent* dropEvent = static_cast<QDropEvent*> (event);
            this->pvNameDropEvent (slot, dropEvent);
            this->items [slot].setHighLighted (false);
            return true;
         }
         break;

      default:
         // Just fall through
         break;
   }

   return false;
}


//---------------------------------------------------------------------------------
//
void QEScratchPad::setPvName (const int slot, const QString& pvName)
{
   SLOT_CHECK (slot,);
   QString descPv;

   DataSets* ds = &(this->items [slot]);

   ds->thePvName = pvName.trimmed ();
   ds->pvName->setText (ds->thePvName);

   // New PV name or clear - clear current text values.
   ds->description->setText ("");
   ds->value->setText ("");

   if (ds->isInUse()) {
      descPv = QERecordFieldName::fieldPvName (ds->thePvName, "DESC");
      ds->description->setVariableNameAndSubstitutions (descPv, "", 0);
      ds->value->setVariableNameAndSubstitutions (ds->thePvName, "", 0);

      ds->pvName->setStyleSheet (QEUtilities::colourToStyle (clInUse));
      ds->description->setStyleSheet (QEUtilities::colourToStyle (clInUse));
      ds->value->setStyleSheet (QEUtilities::colourToStyle (clInUse));
   } else {
      ds->description->setVariableNameAndSubstitutions ("", "", 0);
      ds->value->setVariableNameAndSubstitutions ("", "", 0);

      ds->pvName->setStyleSheet (QEUtilities::colourToStyle (clNotInUse));
      ds->description->setStyleSheet (QEUtilities::colourToStyle (clNotInUse));
      ds->value->setStyleSheet (QEUtilities::colourToStyle (clNotInUse));
   }

   this->calcMinimumHeight ();
}


//---------------------------------------------------------------------------------
//
QString QEScratchPad::getPvName (const int slot)
{
   SLOT_CHECK (slot, "");
   return this->items [slot].thePvName;
}

// end
