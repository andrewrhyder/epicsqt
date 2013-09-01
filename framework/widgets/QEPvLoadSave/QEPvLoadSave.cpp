/*  QEPvLoadSave.cpp
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
#include <QModelIndex>

#include <QECommon.h>
#include "QEPvLoadSave.h"
#include "QEPvLoadSaveItem.h"
#include "QEPvLoadSaveModel.h"
#include "QEPvLoadSaveUtilities.h"

#define DEBUG  qDebug () << "QEPvLoadSave::" << __FUNCTION__ << ":" << __LINE__

// Compare with QEStripChartToolBar
//
struct PushButtonSpecifications {
   int side;
   int gap;
   int width;
   bool isIcon;  // when false is caption
   const QString captionOrIcon;
   const QString toolTip;
   const char * member;
};

// Most buttons are on both sides - some are on one or the other.
//
#define BOTH 0
#define LHS  1
#define RHS  2

#define NL               (-99)        // new line gap
#define ICW                40         // icon width
#define NCW                68         // normal caption width
#define WCW                80         // wide caption width

static const struct PushButtonSpecifications buttonSpecs [QEPvLoadSave::NumberOfButtons] = {
   { BOTH, 0,   ICW, true,  QString ("write_all.png"),     QString ("Write all PV values to the system"),          SLOT (writeAllClicked (bool))    },
   { BOTH, 0,   ICW, true,  QString ("read_all.png"),      QString ("Read all PV values from the system"),         SLOT (readAllClicked (bool))     },
   { BOTH, 12,  ICW, true,  QString ("write_subset.png"),  QString ("Write selected PV values to the system"),     SLOT (writeSubsetClicked (bool)) },
   { BOTH, 0,   ICW, true,  QString ("read_subset.png"),   QString ("Read selected PV values from the system"),    SLOT (readSubsetClicked (bool))  },

   { BOTH, 12,  ICW, true,  QString ("archive_time.png"),  QString ("Extract all PV values from the archiver for the specified time"),
                                                                                                                   SLOT (archiveTimeClicked (bool)) },
   { LHS,  12,  ICW, true,  QString ("copy_all.png"),      QString ("Copy all PV values to other workspace"),      SLOT (copyAllClicked (bool))     },
   { LHS,  0,   ICW, true,  QString ("copy_subset.png"),   QString ("Copy selected PV values to other workspace"), SLOT (copySubsetClicked (bool))  },
   { RHS,  12,  ICW, true,  QString ("ypoc_all.png"),      QString ("Copy all PV values to other workspace"),      SLOT (copyAllClicked (bool))     },
   { RHS,  0,   ICW, true,  QString ("ypoc_subset.png"),   QString ("Copy selected PV values to other workspace"), SLOT (copySubsetClicked (bool))  },

   { BOTH, NL,  NCW, false, QString ("Load..."),           QString ("Load node tree from file"),                   SLOT (loadClicked (bool))        },
   { BOTH, 2,   NCW, false, QString ("Save..."),           QString ("Save node tree to file"),                     SLOT (saveClicked (bool))        },
   { BOTH, 2,   NCW, false, QString ("Delete"),            QString ("Delete node"),                                SLOT (deleteClicked (bool))      },
   { BOTH, 2,   NCW, false, QString ("Edit..."),           QString ("Edit node"),                                  SLOT (editClicked (bool))        },
   { BOTH, 2,   NCW, false, QString ("Sort"),              QString ("Sort node tree by PV name"),                  SLOT (sortClicked (bool))        },
   { BOTH, 2,   WCW, false, QString ("Compare"),           QString ("Compare workspaces"),                         SLOT (compareClicked (bool))     }
};


//=============================================================================
// Halves
//=============================================================================
//
QEPvLoadSave::Halves::Halves (const Sides sideIn, QEPvLoadSave* ownerIn, QBoxLayout* layout)
{
   int j;
   int left;
   int top;
   int gap;
   QString iconPathName;
   QPushButton* button;

   this->side = sideIn;
   this->owner = ownerIn;

   this->container = new QFrame ();
   this->container->setFrameShape (QFrame::Panel);
   this->container->setFrameShadow (QFrame::Plain);
   // This re-parents container.
   layout->addWidget (container);

   this->halfLayout = new QVBoxLayout (this->container);
   this->halfLayout->setMargin (2);
   this->halfLayout->setSpacing (2);

   this->header = new QFrame ();
   this->header->setFrameShape (QFrame::NoFrame);
   this->header->setFrameShadow (QFrame::Plain);
   this->header->setFixedHeight (68);
   this->halfLayout->addWidget (this->header);

   // Create add header buttons.
   //
   left = 4;
   top = 2;
   for (j = 0 ; j < ARRAY_LENGTH (buttonSpecs); j++) {

      this->headerPushButtons [j] = NULL;

      // Some buttons are only on one side.
      //
      if ((this->side == LeftSide)  && (buttonSpecs[j].side == RHS)) continue;
      if ((this->side == RightSide) && (buttonSpecs[j].side == LHS)) continue;

      button = new QPushButton (this->header);

      // Set up icon or caption text.
      //
      if (buttonSpecs[j].isIcon) {
         iconPathName = ":/qe/pvloadsave/";
         iconPathName.append (buttonSpecs[j].captionOrIcon);
         button->setIcon (QIcon (iconPathName));
      } else {
         button->setText (buttonSpecs[j].captionOrIcon);
      }

      button->setToolTip (buttonSpecs[j].toolTip);
      gap = buttonSpecs[j].gap;

      if (gap == NL) {
         // There is only one "newline".
         //
         if (this->side == LeftSide) {
            this->checkBox = new QCheckBox ("Show 2nd tree", this->header);
            this->checkBox->setGeometry (left + 2, top, 120, 26);
            this->checkBox->setChecked (true);

            QObject::connect (this->checkBox, SIGNAL (stateChanged (int)),
                              ownerIn,        SLOT   (checkBoxStateChanged (int)));
         } else {
            this->checkBox = NULL;
         }

         left = 4;
         top += 32;
         gap = 0;
      }

      button->setGeometry (left + gap, top, buttonSpecs[j].width, 26);
      left += gap + buttonSpecs[j].width + 2;
      if (buttonSpecs[j].member != NULL) {
         QObject::connect (button, SIGNAL (clicked (bool)), ownerIn, buttonSpecs[j].member);
      }

      if (j == 14) {
         button->setStyleSheet (QEUtilities::colourToStyle (QColor (155, 205, 255)));
      }
      this->headerPushButtons [j] = button;
   }

   this->tree = new QTreeView ();
   this->halfLayout->addWidget (this->tree);
   this->tree->setContextMenuPolicy (Qt::CustomContextMenu);

   QObject::connect (this->tree,  SIGNAL (customContextMenuRequested (const QPoint &)),
                     this->owner, SLOT   (treeMenuRequested          (const QPoint &)));

   this->footer = new QFrame ();
   this->footer->setFrameShape (QFrame::NoFrame);
   this->footer->setFrameShadow (QFrame::Plain);
   this->footer->setFixedHeight (40);
   this->halfLayout->addWidget (this->footer);


   // Configure basic tree setup.
   //
   // this->tree->setAllowDrop (false);
   this->tree->setIndentation (10);
   this->tree->setItemsExpandable (true);
   this->tree->setUniformRowHeights (true);
   this->tree->setRootIsDecorated (true);
   this->tree->setAlternatingRowColors (true);

   // Create an essentially empty model.
   //
   this->model = new QEPvLoadSaveModel (this->owner);  // not a widget

   QObject::connect (this->model, SIGNAL (reportActionComplete (QEPvLoadSaveCommon::ActionKinds, bool)),
                     this->owner, SLOT   (acceptActionComplete (QEPvLoadSaveCommon::ActionKinds, bool)));

   this->tree->setModel (this->model);                 // is a widget

   // Create an essentially empty model.
   //
   if (this->tree->header()) {
      this->tree->header()->setStretchLastSection (true);
      this->tree->header()->resizeSection (0, 240);
   }

   this->vnpm.setVariableIndex ((int) this->side);

   // Set up a connection to recieve configuration file name property changes.
   // The variable name property manager class only delivers an updated
   // variable name after the user has stopped typing.
   //
   QObject::connect (&this->vnpm, SIGNAL (newVariableNameProperty (QString, QString, unsigned int)),
                     this->owner, SLOT   (useNewConfigurationFileProperty (QString, QString, unsigned int)));
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::Halves::setTop (QEPvLoadSaveItem* topItem, const QString& heading)
{
   QModelIndex topIndex;

   this->model->setupModelData (topItem, heading);

   // Ensure top level is expanded.
   // Get first/only child of root index.
   //
   topIndex = this->model->index (0, 0, this->model->getRootIndex ());
   this->tree->expand (topIndex);
}

//------------------------------------------------------------------------------
// Called by establishConnection
//
void QEPvLoadSave::Halves::open (const QString& configurationFileIn)
{
   QEPvLoadSaveItem* topItem = NULL;

   this->configurationFile = configurationFileIn;
   if (this->configurationFile == "") {
      return;
   }

   topItem = QEPvLoadSaveUtilities::readTree (this->configurationFile);
   if (!topItem) {
       DEBUG << "file read fail " << this->configurationFile;
       return;
   }

   this->setTop (topItem, this->configurationFile);
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::Halves::setConfigurationFile  (const QString& configurationFile)
{
   this->vnpm.setVariableNameProperty (configurationFile);
}

//------------------------------------------------------------------------------
//
QString QEPvLoadSave::Halves::getConfigurationFile ()
{
   return this->vnpm.getVariableNameProperty ();
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::Halves::setConfigurationSubstitutions (const QString& substitutions)
{
   this->vnpm.setSubstitutionsProperty (substitutions);
}

//------------------------------------------------------------------------------
//
QString QEPvLoadSave::Halves::getConfigurationSubstitutions ()
{
   return this->vnpm.getSubstitutionsProperty ();
}


//=============================================================================
// QEPvLoadSave functions
//=============================================================================
// Constructor with no initialisation
//
QEPvLoadSave::QEPvLoadSave (QWidget * parent) : QEFrame (parent)
{
   this->setNumVariables (ARRAY_LENGTH (this->half));

   // configure the panel and create contents
   //
   this->setFrameShape (QFrame::Panel);
   this->setFrameShadow (QFrame::Plain);
   this->setMinimumSize (932, 400);

   // Create internal widgets.
   //
   this->overallLayout = new QVBoxLayout (this);
   this->overallLayout->setMargin (2);
   this->overallLayout->setSpacing (2);

   this->sidesFrame = new QFrame ();    // Note addwidget sets parent
   this->sidesFrame->setFrameShape (QFrame::NoFrame);
   this->sidesFrame->setFrameShadow (QFrame::Plain);
   this->overallLayout->addWidget (sidesFrame);

   this->sideBySidelayout = new QHBoxLayout (this->sidesFrame);
   this->sideBySidelayout->setMargin (0);
   this->sideBySidelayout->setSpacing (2);

   for (int t = 0; t < ARRAY_LENGTH (this->half); t++) {
      this->half [t] = new Halves ((Sides) t, this, this->sideBySidelayout);
   }

   this->loadSaveStatus = new QFrame ();
   this->loadSaveStatus->setFrameShape (QFrame::Panel);
   this->loadSaveStatus->setFrameShadow (QFrame::Plain);
   this->loadSaveStatus->setFixedHeight (68);
   this->overallLayout->addWidget (this->loadSaveStatus);

   this->loadSaveTitle = new QLabel ("Transfer to/from System or from Archive", this->loadSaveStatus);
   this->loadSaveTitle->setGeometry (12, 4, 400, 18);

   this->progressBar = new QProgressBar (this->loadSaveStatus);
   this->progressBar->setGeometry (12, 28, 400, 26);

   this->abortButton = new QPushButton (this->loadSaveStatus);
   this->abortButton->setText ("Abort");
   this->abortButton->setGeometry (424, 28, 80, 26);
   this->abortButton->setStyleSheet (QEUtilities::colourToStyle (QColor (155, 205, 255)));
   this->abortButton->setEnabled (false);
   QObject::connect (this->abortButton, SIGNAL (clicked (bool)), this, SLOT (abortClicked (bool)));

   // Create dialogs.
   //
   this->groupNameDialog = new QEPvLoadSaveGroupNameDialog (this);
   this->pvNameSelectDialog = new QEPVNameSelectDialog (this);

   this->setAllowDrop (false);

   // Set the initial state
   //
   QWidget::setEnabled (true);

   // Use widget specific context menu.
   //
   for (int j = 0; j < ARRAY_LENGTH (this->actionList); j++) {
      this->actionList [j] = NULL;
   }

   this->treeContextMenu = new QMenu (this);
   this->createAction (this->treeContextMenu, "Create Root",          false, TCM_CREATE_ROOT);
   this->createAction (this->treeContextMenu, "Add Group...",         false, TCM_ADD_GROUP);
   this->createAction (this->treeContextMenu, "Rename Group...",      false, TCM_RENAME_GROUP);
   this->createAction (this->treeContextMenu, "Add PV...",            false, TCM_ADD_PV);
   this->createAction (this->treeContextMenu, "Examine Properties",   false, TCM_SHOW_PV_PROPERTIES);
   this->createAction (this->treeContextMenu, "Plot in StripChart",   false, TCM_ADD_TO_STRIPCHART);
   this->createAction (this->treeContextMenu, "Show in Scatch Pad",   false, TCM_ADD_TO_SCRATCH_PAD);
   this->createAction (this->treeContextMenu, "Edit PV Name...",      false, TCM_EDIT_PV_NAME);
   this->createAction (this->treeContextMenu, "Edit PV Value...",     false, TCM_EDIT_PV_VALUE);
   this->createAction (this->treeContextMenu, "Copy variable name",   false, TCM_COPY_VARIABLE);
   this->createAction (this->treeContextMenu, "Copy data",            false, TCM_COPY_DATA);

   QObject::connect (this->treeContextMenu, SIGNAL (triggered        (QAction*)),
                     this,                  SLOT   (treeMenuSelected (QAction*)));


   // Gui requests.
   //
   QObject* consumer = this->getGuiLaunchConsumer ();
   QObject::connect (this,     SIGNAL (requestGui (const QEGuiLaunchRequests& )),
                     consumer, SLOT   (requestGui (const QEGuiLaunchRequests& )));
}

//-----------------------------------------------------------------------------
//
QEPvLoadSave::~QEPvLoadSave ()
{
}

//------------------------------------------------------------------------------
//
QAction* QEPvLoadSave::createAction (QMenu *parent,
                                     const QString &caption,
                                     const bool checkable,
                                     const TreeContextMenuActions treeAction)
{
   QAction* action = NULL;

   action = new QAction (caption + " ", parent);
   action->setCheckable (checkable);
   action->setData (QVariant (int (treeAction)));
   parent->addAction (action);

   this->actionList [treeAction] = action;
   return action;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::resizeEvent (QResizeEvent* )
{
   int fw = this->geometry ().width ();
   QRect pg = this->progressBar->geometry ();
   QRect ag = this->abortButton->geometry ();

   int m;
   int d;
   QEUtilities::getCurrentScaling (m, d);

   // Need to take scaling into account.
   //
   int margin = QEUtilities::scaleBy (20, m, d);
   int space =  QEUtilities::scaleBy (12, m, d);

   int dx = (fw - (margin + ag.width ()) - ag.x ());
   ag.translate (dx, 0);
   pg.setWidth (ag.x() - pg.x() - space);

   this->progressBar->setGeometry (pg);
   this->abortButton->setGeometry (ag);
}

//------------------------------------------------------------------------------
//
QSize QEPvLoadSave::sizeHint () const
{
    return QSize (1000, 500);
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::useNewConfigurationFileProperty (QString configurationFileIn,
                                                    QString configurationFileSubstitutionsIn,
                                                    unsigned int variableIndex)
{
   this->setVariableNameAndSubstitutions (configurationFileIn, configurationFileSubstitutionsIn, variableIndex);
}

//------------------------------------------------------------------------------
//
qcaobject::QCaObject* QEPvLoadSave::createQcaItem (unsigned int variableIndex)
{
   DEBUG << "unexpected - variableIndex =" << variableIndex;
   return NULL;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::establishConnection (unsigned int variableIndex)
{
   QString configurationFile;

   if (variableIndex >= ARRAY_LENGTH (this->half)) {
      DEBUG << "unexpected - variableIndex =" << variableIndex;
      return;
   }

   configurationFile = this->getSubstitutedVariableName (variableIndex);
   this->half [variableIndex]->open (configurationFile);
}

//------------------------------------------------------------------------------
//
QEPvLoadSave::Sides QEPvLoadSave::sideOfSender (QObject *sentBy)
{
   int s;
   int j;

   for (s = 0; s < ARRAY_LENGTH (this->half); s++) {
      if (sentBy == this->half [s]->tree) {
         // found a match.
         return Sides (s);
      }

      // Check push buttons.
      //
      for (j = 0; j < ARRAY_LENGTH (this->half [s]->headerPushButtons); j++) {
         if (sentBy == this->half [s]->headerPushButtons [j]) {
            // found a match.
            return Sides (s);
         }
      }
   }

   DEBUG  << "no match found";
   return ErrorSide;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::acceptActionComplete (QEPvLoadSaveCommon::ActionKinds, bool okay)
{
   if (okay) {
      int v = this->progressBar->value ();
      this->progressBar->setValue (v + 1);
   }
}

//==============================================================================
// Menu request/select
//
void QEPvLoadSave::treeMenuRequested (const QPoint& pos)
{
   this->selectedHalf = NULL;
   this->selectedItem = NULL;

   Sides side = this->sideOfSender (this->sender ());
   if (side == ErrorSide)  return;

   this->selectedHalf = this->half [side];
   QTreeView* tree = this->selectedHalf->tree;
   QEPvLoadSaveModel* model = this->selectedHalf->model;
   QModelIndex index;
   int j;

   // Get the model index 'address' of the item at this positon,
   // and then get the underlying load save item.
   //
   index = tree->indexAt (pos);
   this->selectedItem = model->indexToItem (index);

   // Make all actions invisible, then set visible required actions.
   //
   for (j = 0; j < ARRAY_LENGTH (this->actionList); j++) {
      this->actionList [j]->setVisible (false);
   }

   // Does item even exit at this position.
   //
   if (this->selectedItem) {
      // Is is a leaf/PV node or a gruop node?
      //
      if (this->selectedItem->getIsPV ()) {
         for (j = TCM_COPY_VARIABLE; j <= TCM_EDIT_PV_VALUE; j++) {
            this->actionList [j]->setVisible (true);
         }
      } else {
         this->actionList [TCM_ADD_GROUP]->setVisible (true);
         if (this->selectedItem != model->getTopItem ()) {
            // Renaming the 'ROOT' node prohibited.
            this->actionList [TCM_RENAME_GROUP]->setVisible (true);
         }
         this->actionList [TCM_ADD_PV]->setVisible (true);
      }

   } else
   // no item selected - is there a root item??
   if (!model->getTopItem ()) {
      // No "ROOT", i.e. top item.
      //
      this->actionList [TCM_CREATE_ROOT]->setVisible (true);

   } else {
      return;  // forget it.
   }

   QPoint golbalPos = tree->mapToGlobal (pos);

   // NOTE: We want access to the tree's rowHeight function as we need this as a
   // position offset (I suspect to account for the header). But the rowHeight ()
   // function IS protected. So we get around this by deriving our own tree view
   // class that can see the protected rowHeight () function and expose this as a
   // public function.
   //
   class ExposedTreeView : QTreeView {
   public:
       int getRowHeight (const QModelIndex &index) const { return this->rowHeight (index); }
   };

   ExposedTreeView* exposedTree = (ExposedTreeView*) tree;
   int rowHeight = exposedTree->getRowHeight (index);

   if (rowHeight == 0) rowHeight = 20;  // scale??

   golbalPos.setY (golbalPos.y () + rowHeight);
   this->treeContextMenu->exec (golbalPos, 0);
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::treeMenuSelected (QAction* action)
{
   if (!this->selectedHalf) return;   // sanity check

   QTreeView* tree = this->selectedHalf->tree;
   QEPvLoadSaveModel* model = this->selectedHalf->model;

   bool okay;
   int intAction;
   TreeContextMenuActions menuAction;
   QVariant nilValue (QVariant::Invalid);
   QEPvLoadSaveItem* item;
   QString nodeName = "";
   QVariant nodeValue;
   int n;

   intAction = action->data ().toInt (&okay);
   if (!okay) {
      return;
   }
   menuAction = (TreeContextMenuActions) intAction;

   // Extract current node name - need it is most case options.
   //
   if (this->selectedItem) {
      nodeName = this->selectedItem->getNodeName ();
   }

   switch (menuAction) {

      case TCM_CREATE_ROOT:
         item = new QEPvLoadSaveItem ("ROOT", false, nilValue, false);
         this->selectedHalf->setTop (item, "");
         break;

      case TCM_ADD_GROUP:
         /// TODO - create group name dialog - re-purposing pvNameSelectDialog here for now
         //
         this->groupNameDialog->setWindowTitle ("QEPvLoadSave - Add Group");
         this->groupNameDialog->setGroupName ("");
         n = this->groupNameDialog->exec (tree);
         if (n == 1) {
            item = new QEPvLoadSaveItem (this->groupNameDialog->getGroupName (), false, nilValue, this->selectedItem);
            model->modelUpdated ();
         }
         break;

      case TCM_RENAME_GROUP:
         /// TODO - create group name dialog - re-purposing pvNameSelectDialog here for now
         this->groupNameDialog->setWindowTitle ("QEPvLoadSave - Rename Group");
         this->groupNameDialog->setGroupName (nodeName);
         n = this->groupNameDialog->exec (tree);
         if (n == 1) {
            this->selectedItem->setNodeName (this->groupNameDialog->getGroupName ());
            model->modelUpdated ();
         }
         break;

      case TCM_ADD_PV:
         this->pvNameSelectDialog->setWindowTitle ("QEPvLoadSave - Add PV");
         this->pvNameSelectDialog->setPvName ("");
         n = this->pvNameSelectDialog->exec (tree);
         if (n == 1) {
            item = new QEPvLoadSaveItem (this->pvNameSelectDialog->getPvName (), true, nilValue, this->selectedItem);
            model->modelUpdated ();
         }
         break;

      case TCM_EDIT_PV_NAME:
         this->pvNameSelectDialog->setWindowTitle ("QEPvLoadSave - edit PV");
         this->pvNameSelectDialog->setPvName (nodeName);
         n = this->pvNameSelectDialog->exec (tree);
         if (n == 1) {
            this->selectedItem->setNodeName (this->pvNameSelectDialog->getPvName ());
            model->modelUpdated ();
         }
         break;

      case TCM_EDIT_PV_VALUE:
         break;

      case TCM_COPY_VARIABLE:
         QApplication::clipboard ()->setText (nodeName);
         break;

      case TCM_COPY_DATA:
         nodeValue = this->selectedItem->getNodeValue ();

         // Need be aware of lists.
         if (nodeValue.type() == QVariant::List) {
            QStringList sl = nodeValue.toStringList ();
            QString text = "( ";

            for (int j = 0; j < sl.size (); j++) {
               if (j > 0) text.append(", ");
               text.append (sl.value(j));
            }
            text.append(" )");
            QApplication::clipboard ()->setText (text);
         } else {
            // Not a list - easy.
            QApplication::clipboard ()->setText (nodeValue.toString ());
         }
         break;

      case TCM_SHOW_PV_PROPERTIES:
         emit this->requestGui (QEGuiLaunchRequests (QEGuiLaunchRequests::KindPvProperties, nodeName));
         break;

      case TCM_ADD_TO_STRIPCHART:
         emit this->requestGui (QEGuiLaunchRequests (QEGuiLaunchRequests::KindStripChart, nodeName));
         break;

      case TCM_ADD_TO_SCRATCH_PAD:
         emit this->requestGui (QEGuiLaunchRequests (QEGuiLaunchRequests::KindScratchPad, nodeName));
         break;

      default:
         DEBUG << "Unexpected action: " << menuAction;
         break;
   }
}

//==============================================================================
// Button and box signal functions
//
void QEPvLoadSave::checkBoxStateChanged (int state)
{
   const bool selected = (state == Qt::Checked);
   this->half [RightSide]->container->setVisible (selected);

   // Increase/Decrease minimum width
   int mw = this->minimumWidth ();
   this->setMinimumWidth (selected ? (mw*2) : (mw/2));
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::writeAllClicked (bool)
{
   Sides side = this->sideOfSender (this->sender ());
   QEPvLoadSaveModel* model = this->half [side]->model;

   this->progressBar->setMaximum (MAX (1, model->leafCount ()));
   this->progressBar->setValue (0);
   model->applyPVData ();
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::readAllClicked (bool)
{
   Sides side = this->sideOfSender (this->sender ());
   QEPvLoadSaveModel* model = this->half [side]->model;

   this->progressBar->setMaximum (MAX (1, model->leafCount ()));
   this->progressBar->setValue (0);
   model->extractPVData ();
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::writeSubsetClicked (bool)
{
   Sides side = this->sideOfSender (this->sender ());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::readSubsetClicked (bool)
{
   Sides side = this->sideOfSender (this->sender ());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::archiveTimeClicked (bool)
{
   Sides side = this->sideOfSender (this->sender ());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::copyAllClicked (bool)
{
   Sides side = this->sideOfSender (this->sender ());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::copySubsetClicked (bool)
{
   Sides side = this->sideOfSender (this->sender ());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::loadClicked (bool)
{
   Sides side = this->sideOfSender (this->sender ());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::saveClicked (bool)
{
   Sides side = this->sideOfSender (this->sender ());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::deleteClicked (bool)
{
   Sides side = this->sideOfSender (this->sender ());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::editClicked (bool)
{
   Sides side = this->sideOfSender (this->sender ());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::sortClicked (bool)
{
   Sides side = this->sideOfSender (this->sender ());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::compareClicked (bool)
{
   Sides side = this->sideOfSender (this->sender ());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::abortClicked (bool)
{
   DEBUG;
}


//==============================================================================
// Property functions
//
void QEPvLoadSave::setSubstitutions (QString configurationFileSubstitutions)
{
   // Use same substituitions for both halves.
   //
   this->half [0]->setConfigurationSubstitutions (configurationFileSubstitutions);
   this->half [1]->setConfigurationSubstitutions (configurationFileSubstitutions);
}

//------------------------------------------------------------------------------
//
QString QEPvLoadSave::getSubstitutions ()
{
   return this->half [0]->getConfigurationSubstitutions ();
}

//==============================================================================
// Copy (no paste)
//

// end
