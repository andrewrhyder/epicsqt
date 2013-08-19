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

// Most buttons are both sides -= some are obe or the other.
#define BOTH 0
#define LHS  1
#define RHS  2

#define NL                (-9)        // new line gap
#define ICW                44         // icon width
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
// Called by establishConnection
//
void QEPvLoadSave::Halves::open (const QString& configurationFileIn)
{
   QFile* file = NULL;
   QEPvLoadSaveItem* topItem = NULL;
   QModelIndex topIndex;

   this->configurationFile = configurationFileIn;

   // Empty model.
   //
   if (this->model) {
      delete this->model;
      this->model = NULL;
   }

   this->model = new QEPvLoadSaveModel (this->owner);
   this->tree->setModel (this->model);

   QObject::connect (this->model, SIGNAL (reportActionComplete (QEPvLoadSaveCommon::ActionKinds, bool)),
                     this->owner, SLOT   (acceptActionComplete (QEPvLoadSaveCommon::ActionKinds, bool)));

   // No file - skip it.
   //
   if (this->configurationFile == "") {
      return;
   }

   file = this->owner->openQEFile (this->configurationFile, QFile::ReadOnly);
   if (!file) {
      DEBUG << "file open fail: " << this->configurationFile;
      return;
   }

   topItem = QEPvLoadSaveUtilities::readTree (file);
   file->close ();

   if (!topItem) {
       DEBUG << "file read fail";
       return;
   }

   this->model->setupModelData (topItem, this->configurationFile);

   // Ensure top level is expanded.
   //
   topIndex = this->model->index (0, 0, this->model->getRootIndex ());
   this->tree->expand (topIndex);
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
   this->setMinimumSize (916, 400);

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


   this->setAllowDrop (false);

   // Set the initial state
   //
   QWidget::setEnabled (true);

   // Use default context menu.
   //
   this->setupContextMenu ();
}

//-----------------------------------------------------------------------------
//
QEPvLoadSave::~QEPvLoadSave ()
{
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::resizeEvent (QResizeEvent* )
{
   int w = this->geometry ().width ();

   this->progressBar->setGeometry (12, 28, w - (WCW + 40), 26);
   this->abortButton->setGeometry (w - (WCW + 20), 28, WCW, 26);
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
   int v;

   if (okay) {
      v = this->progressBar->value ();
      this->progressBar->setValue (v + 1);
   }
}

//==============================================================================
// Button signal functions
//
void QEPvLoadSave::writeAllClicked (bool)
{
   Sides side = this->sideOfSender (this->sender());
   QEPvLoadSaveModel* model = this->half [side]->model;

   this->progressBar->setMaximum (model->leafCount ());
   this->progressBar->setValue (0);
   model->applyPVData ();
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::readAllClicked (bool)
{
   Sides side = this->sideOfSender (this->sender());
   QEPvLoadSaveModel* model = this->half [side]->model;

   this->progressBar->setMaximum (model->leafCount ());
   this->progressBar->setValue (0);
   model->extractPVData ();
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::writeSubsetClicked (bool)
{
   Sides side = this->sideOfSender (this->sender());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::readSubsetClicked (bool)
{
   Sides side = this->sideOfSender (this->sender());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::archiveTimeClicked (bool)
{
   Sides side = this->sideOfSender (this->sender());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::copyAllClicked (bool)
{
   Sides side = this->sideOfSender (this->sender());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::copySubsetClicked (bool)
{
   Sides side = this->sideOfSender (this->sender());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::loadClicked (bool)
{
   Sides side = this->sideOfSender (this->sender());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::saveClicked (bool)
{
   Sides side = this->sideOfSender (this->sender());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::deleteClicked (bool)
{
   Sides side = this->sideOfSender (this->sender());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::editClicked (bool)
{
   Sides side = this->sideOfSender (this->sender());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::sortClicked (bool)
{
   Sides side = this->sideOfSender (this->sender());
   DEBUG << side;
}

//------------------------------------------------------------------------------
//
void QEPvLoadSave::compareClicked (bool)
{
   Sides side = this->sideOfSender (this->sender());
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
