/*  QEDevicePad.cpp
 */

#include <QDebug>
#include <QColor>
#include <QECommon.h>
#include <QEPvPropertiesUtilities.h>

#include "QEDevicePad.h"

#define DEBUG qDebug() << "QEDevicePad::" << __FUNCTION__ << ":" << __LINE__

static const QColor clHighLight (0xFFFFFF);
static const QColor clInUse     (0xE8E8E8);
static const QColor clNotInUse  (0xC8C8C8);
static const QColor clSelected  (0x7090FF);

static const int NULL_SELECTION = -1;
static const int COPY_PV_NAMES  =  1;
static const int PASTE_PV_NAMES =  2;

//---------------------------------------------------------------------------------
//
void QEDevicePad::DeviceDataSets::setHighLighted (const bool isHighLightedIn)
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
      this->form->setStyleSheet (styleSheet);
   }
}


//=================================================================================
// QEDevicePad
//=================================================================================
//
void QEDevicePad::createInternalWidgets ()
{
   const int frameHeight = 34;
   const int horMargin = 2;    // 19 - 2 - 2 => widget height is 30
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

   this->titlePvName = new QLabel ("Device Name", this->titleFrame );
   this->titlePvName->setIndent (indent);
   this->titlePvName->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);

   this->titleValue = new QLabel ("Control", this->titleFrame );
   this->titleValue->setIndent (indent);
   this->titleValue->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);

   this->titleLayout = new QHBoxLayout (this->titleFrame);
   this->titleLayout->setMargin (horMargin);
   this->titleLayout->setSpacing (horSpacing);

   this->titleLayout->addWidget (this->titlePvName);
   this->titleLayout->addWidget (this->titleValue);
   // set stretch
   this->titleLayout->setStretch(0, 1);
   this->titleLayout->setStretch(1, 5);
   this->vLayout->addWidget (this->titleFrame);

   for (int slot = 0; slot < ARRAY_LENGTH (this->items); slot++) {

      DeviceDataSets* item = (DeviceDataSets*)this->items[slot];

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


      item->form = new QEForm (item->frame);
      item->form->setStyleSheet (QEUtilities::colourToStyle (clNotInUse));


      // Set up layout - paramers must be same as titlelayout
      //
      item->hLayout = new QHBoxLayout (item->frame);
      item->hLayout->setMargin (horMargin);
      item->hLayout->setSpacing (horSpacing);

      // Add to layouts
      //
      item->hLayout->addWidget (item->pvName);
      item->hLayout->addWidget (item->form);
      // set stretch
      item->hLayout->setStretch(0, 1);
      item->hLayout->setStretch(1, 5);

      this->vLayout->addWidget (item->frame);

      QObject::connect (item->frame, SIGNAL (customContextMenuRequested (const QPoint &)),
                        this,        SLOT   (contextMenuRequested (const QPoint &)));

      QObject::connect (item->menu, SIGNAL (contextMenuSelected (const int, const QEScratchPadMenu::ContextMenuOptions)),
                        this,       SLOT   (contextMenuSelected (const int, const QEScratchPadMenu::ContextMenuOptions)));
   }

   this->vLayout->addStretch ();

//   this->pvNameSelectDialog = new QEPVNameSelectDialog (QEPVNameSelectDialog::File, deviceListFileName, "Device Name Selection", this);

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
// QEDevicePad
//=================================================================================
//
QEDevicePad::QEDevicePad (QWidget* parent, QString devCtrlUiFile) : QEScratchPad (true, parent)
{
    // create Data sets
    for ( int i = 0; i < NUMBER_OF_ITEMS; i++ ){
        this->items[i] = new DeviceDataSets();
    }

   uiFileName = devCtrlUiFile;
   pvNameSelectDialog = 0;

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
QEDevicePad::~QEDevicePad ()
{
   // place holder
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
void QEDevicePad::contextMenuSelected (const int slot, const QEScratchPadMenu::ContextMenuOptions option)
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
         n = this->pvNameSelectDialog->exec (this->items[slot]->pvName);
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
void QEDevicePad::setPvName (const int slot, const QString& pvName)
{
   SLOT_CHECK (slot,);

   DeviceDataSets* item = (DeviceDataSets*)this->items [slot];

   item->thePvName = pvName.trimmed ();
   item->pvName->setText (item->thePvName);

   // New PV name or clear - clear current text values.
   if (!pvName.isEmpty()){
       // create QEForm
       item->form->setVariableNameSubstitutionsProperty("MOTOR=" + item->thePvName);
       item->form->setUiFileNameProperty (uiFileName);
       item->form->readUiFile();
   }
   else{
       // reset QEForm
       item->form->removeUi();
       qDebug() << "reset QEForm";
   }

   if (item->isInUse()) {
      item->pvName->setStyleSheet (QEUtilities::colourToStyle (clInUse));
      item->form->setStyleSheet (QEUtilities::colourToStyle (clInUse));
   } else {
      item->pvName->setStyleSheet (QEUtilities::colourToStyle (clNotInUse));
      item->form->setStyleSheet (QEUtilities::colourToStyle (clNotInUse));
   }

   this->calcMinimumHeight ();
}

//---------------------------------------------------------------------------------
//
void QEDevicePad::calcMinimumHeight ()
{
   int last;
   int count;
   int delta_top;

   // Find last used item.
   //
   last = -1;
   for (int slot = 0; slot < ARRAY_LENGTH (this->items); slot++) {
      if (this->items [slot]->isInUse ()) {
         last = slot;
      }
   }

   // Allow one spare at end of widget if there is room.
   //
   if (last  < ARRAY_LENGTH (this->items) - 1) last++;

   // Set visibility accordingly
   //
   for (int slot = 0; slot < ARRAY_LENGTH (this->items); slot++) {
      this->items [slot]->frame->setVisible (slot <= last);
      if (slot <= last) {
      }
   }

   // Allow +1 for titles and +1 for zero indexed nature of slot and last.
   //
   count = 2 + last;

   delta_top = 30;
   this->setMinimumHeight ((delta_top * count) + 20);
}

void QEDevicePad::setListFileName( QString listFileName ){
    deviceListFileName = listFileName;
    if (!this->pvNameSelectDialog){
        this->pvNameSelectDialog = new QEPVNameSelectDialog (QEPVNameSelectDialog::File, deviceListFileName, "Device Name Selection", this);
//        this->pvNameSelectDialog = new QEPVNameSelectDialog (QEPVNameSelectDialog::Database, deviceListFileName, "Device Name Selection", this);
    }
}

//---------------------------------------------------------------------------------
//
void QEDevicePad::saveConfiguration (PersistanceManager* pm)
{
   const QString formName = this->persistantName ("QEDevicePad");

   PMElement formElement = pm->addNamedConfiguration (formName);

   // Save each active PV.
   //
   PMElement pvListElement = formElement.addElement ("PV_List");

   for (int slot = 0; slot < ARRAY_LENGTH (this->items); slot++) {
      BaseDataSets* item = this->items [slot];
      if (item->isInUse()) {
         PMElement pvElement = pvListElement.addElement ("PV");
         pvElement.addAttribute ("id", slot);
         pvElement.addValue ("Name", this->getPvName(slot));
      }
   }
}

//---------------------------------------------------------------------------------
//
void QEDevicePad::restoreConfiguration (PersistanceManager* pm, restorePhases restorePhase)
{
   if (restorePhase != FRAMEWORK) return;

   const QString formName = this->persistantName ("QEDevicePad");

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
