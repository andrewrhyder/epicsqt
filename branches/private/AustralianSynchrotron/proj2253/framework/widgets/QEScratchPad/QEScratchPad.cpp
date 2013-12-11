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
static const int COPY_PV_NAMES  =  1;
static const int PASTE_PV_NAMES =  2;


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

   QAction* action;

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

   for (int slot = 0; slot < ARRAY_LENGTH (this->items); slot++) {

      DataSets* item = &(this->items [slot]);

      item->menu = new QEScratchPadMenu (slot, this);

      item->frame = new QFrame (this);
      item->frame->setFixedHeight (frameHeight);
      item->frame->setAcceptDrops (true);
      item->frame->installEventFilter (this);
      item->frame->setContextMenuPolicy (Qt::CustomContextMenu);

      item->pvName = new QLabel (item->frame);
      item->pvName->installEventFilter (this);
      item->pvName->setText ("");
      item->pvName->setIndent (indent);
      item->pvName->setStyleSheet (QEUtilities::colourToStyle (clNotInUse));

      item->description = new QELabel (item->frame);
      item->description->setDisplayAlarmState (false);
      item->description->setText ("");
      item->description->setIndent (indent);
      item->description->setStyleSheet (QEUtilities::colourToStyle (clNotInUse));

      item->value = new QELabel (item->frame);
      item->value->setDisplayAlarmState (true);
      item->value->setText ("");
      item->value->setIndent (indent);
      item->value->setStyleSheet (QEUtilities::colourToStyle (clNotInUse));

      // Set up layout - paramers must be same as titlelayout
      //
      item->hLayout = new QHBoxLayout (item->frame);
      item->hLayout->setMargin (horMargin);
      item->hLayout->setSpacing (horSpacing);

      // Add to layouts
      //
      item->hLayout->addWidget (item->pvName);
      item->hLayout->addWidget (item->description);
      item->hLayout->addWidget (item->value);

      this->vLayout->addWidget (item->frame);

      QObject::connect (item->frame, SIGNAL (customContextMenuRequested (const QPoint &)),
                        this,        SLOT   (contextMenuRequested (const QPoint &)));

      QObject::connect (item->menu, SIGNAL (contextMenuSelected (const int, const QEScratchPadMenu::ContextMenuOptions)),
                        this,       SLOT   (contextMenuSelected (const int, const QEScratchPadMenu::ContextMenuOptions)));
   }

   this->vLayout->addStretch ();

   this->pvNameSelectDialog = new QEPVNameSelectDialog (this);

   this->widgetContextMenu = new QMenu (this);
   action = new QAction ("Copy All PV Names", this->widgetContextMenu);
   action->setCheckable (false);
   action->setData (QVariant (COPY_PV_NAMES));
   this->widgetContextMenu->addAction (action);

   action = new QAction ("Paste All PV Names", this->widgetContextMenu);
   action->setCheckable (false);
   action->setData (QVariant (PASTE_PV_NAMES));
   this->widgetContextMenu->addAction (action);
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

   // Connect menu itself to menu handler.
   //
   this->setContextMenuPolicy (Qt::CustomContextMenu);

   QObject::connect (this, SIGNAL (customContextMenuRequested (const QPoint &)),
                     this, SLOT   (widgetMenuRequested        (const QPoint &)));

   QObject::connect (this->widgetContextMenu, SIGNAL (triggered          (QAction*)),
                     this,                    SLOT   (widgetMenuSelected (QAction*)));

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

   for (int slot = 0 ; slot < ARRAY_LENGTH (this->items); slot++) {
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
   int last;
   int count;
   int delta_top;

   // Find last used item.
   //
   last = -1;
   for (int slot = 0; slot < ARRAY_LENGTH (this->items); slot++) {
      if (this->items [slot].isInUse ()) {
         last = slot;
      }
   }

   // Allow one spare at end of widget if there is room.
   //
   if (last  < ARRAY_LENGTH (this->items) - 1) last++;

   // Set visibility accordingly
   //
   for (int slot = 0; slot < ARRAY_LENGTH (this->items); slot++) {
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
         DataSets* item = &(this->items [previousSelection]);
         item->frame->setStyleSheet ("");
      }

      if (this->selectedItem != NULL_SELECTION) {
         DataSets* item = &(this->items [this->selectedItem]);
         QString styleSheet = QEUtilities::colourToStyle (clSelected);
         item->frame->setStyleSheet (styleSheet);
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
      DataSets* item = &(this->items [slot]);
      QPoint golbalPos = w->mapToGlobal(pos);
      item->menu->setIsInUse (item->isInUse ());
      item->menu->exec (golbalPos, 0);
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
         this->pvNameSelectDialog->setPvName (this->getPvName (slot));
         n = this->pvNameSelectDialog->exec (this);
         if (n == 1) {
            this->setPvName (slot, this->pvNameSelectDialog->getPvName ());
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
void QEScratchPad::widgetMenuRequested (const QPoint& pos)
{
   QObject *obj = this->sender();   // who sent the signal.
   QWidget* w = dynamic_cast<QWidget*> (obj);
   QPoint golbalPos = w->mapToGlobal (pos);

   this->widgetContextMenu->exec (golbalPos, 0);
}

//---------------------------------------------------------------------------------
//
void QEScratchPad::widgetMenuSelected  (QAction* action)
{
   bool okay;
   int option;
   QClipboard* cb = NULL;
   QString text;

   option = action->data ().toInt (&okay);
   if (!okay) {
      return;
   }

   switch (option) {

      case COPY_PV_NAMES:
         // Create space delimited set of PV names.
         //
         text = "";
         for (int slot = 0; slot < ARRAY_LENGTH (this->items); slot++) {
            DataSets* item = &(this->items [slot]);

            if (item->isInUse()) {
               if (!text.isEmpty()) {
                  text.append (" ");
               };
               text.append (this->getPvName (slot));
            }
         }
         cb = QApplication::clipboard ();
         cb->setText (text);
         break;

      case PASTE_PV_NAMES:
         cb = QApplication::clipboard ();
         text = cb->text().trimmed();
         if (!text.isEmpty()) {
            this->addPvNameSet (text);
         }
         break;

      default:
         // do nothing
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
void QEScratchPad::addPvName (const QString& pvName)
{
   for (int slot = 0; slot < ARRAY_LENGTH (this->items); slot++) {
      DataSets* item = &(this->items [slot]);
      if (item->isInUse() == false) {
         // Found an empty slot.
         //
         this->setPvName (slot, pvName);
         break;
      }
   }
}

//---------------------------------------------------------------------------------
//
void QEScratchPad::addPvNameSet (const QString& pvNameSet)
{
   QStringList pvNameList;
   int j;

   // Split input string using white space as delimiter.
   //
   pvNameList = pvNameSet.split (QRegExp ("\\s+"), QString::SkipEmptyParts);
   for (j = 0; j < pvNameList.count (); j++) {
      this->addPvName (pvNameList.value (j));
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

   DataSets* item = &(this->items [slot]);

   item->thePvName = pvName.trimmed ();
   item->pvName->setText (item->thePvName);

   // New PV name or clear - clear current text values.
   item->description->setText ("");
   item->value->setText ("");

   if (item->isInUse()) {
      descPv = QERecordFieldName::fieldPvName (item->thePvName, "DESC");
      item->description->setVariableNameAndSubstitutions (descPv, "", 0);
      item->value->setVariableNameAndSubstitutions (item->thePvName, "", 0);

      item->pvName->setStyleSheet (QEUtilities::colourToStyle (clInUse));
      item->description->setStyleSheet (QEUtilities::colourToStyle (clInUse));
      item->value->setStyleSheet (QEUtilities::colourToStyle (clInUse));
   } else {
      item->description->setVariableNameAndSubstitutions ("", "", 0);
      item->value->setVariableNameAndSubstitutions ("", "", 0);

      item->pvName->setStyleSheet (QEUtilities::colourToStyle (clNotInUse));
      item->description->setStyleSheet (QEUtilities::colourToStyle (clNotInUse));
      item->value->setStyleSheet (QEUtilities::colourToStyle (clNotInUse));
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

//---------------------------------------------------------------------------------
//
void QEScratchPad::paste (QVariant s)
{
   QStringList list;
   QString pvNameSet;

   // s.toSring is a bit limiting when s is a StringList or a List of String.
   // We don't worry about List 0f StringList or List of List of String etc.
   //
   pvNameSet = "";
   list = s.toStringList ();
   for (int j = 0 ; j < list.count(); j++) {
      pvNameSet.append(" ").append (list.value (j));
   }

   // Use pasted text to add a PV(s) to the chart.
   //
   this->addPvNameSet (pvNameSet);
}

//---------------------------------------------------------------------------------
//
void QEScratchPad::saveConfiguration (PersistanceManager* pm)
{
   const QString formName = this->persistantName ("QEScratchPad");

   PMElement formElement = pm->addNamedConfiguration (formName);

   // Save each active PV.
   //
   PMElement pvListElement = formElement.addElement ("PV_List");

   for (int slot = 0; slot < ARRAY_LENGTH (this->items); slot++) {
      DataSets* item = &(this->items [slot]);
      if (item->isInUse()) {
         PMElement pvElement = pvListElement.addElement ("PV");
         pvElement.addAttribute ("id", slot);
         pvElement.addValue ("Name", this->getPvName(slot));
      }
   }
}

//---------------------------------------------------------------------------------
//
void QEScratchPad::restoreConfiguration (PersistanceManager* pm, restorePhases restorePhase)
{
   if (restorePhase != FRAMEWORK) return;

   const QString formName = this->persistantName ("QEScratchPad");

   PMElement formElement = pm->getNamedConfiguration (formName);

   // Restore each PV.
   //
   PMElement pvListElement = formElement.getElement ("PV_List");

   for (int slot = 0; slot < ARRAY_LENGTH (this->items); slot++) {
      PMElement pvElement = pvListElement.getElement ("PV", "id", slot);
      QString pvName;
      bool status;

      if (pvElement.isNull ()) continue;

      // Attempt to extract a PV name
      //
      status = pvElement.getValue ("Name", pvName);
      if (status) {
         this->setPvName (slot, pvName);
      }
   }
}

// end
