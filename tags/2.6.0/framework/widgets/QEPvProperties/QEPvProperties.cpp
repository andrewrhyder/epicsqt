/*  QEPvProperties.cpp
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#include <stdlib.h>

#include <QDebug>
#include <QTableWidgetItem>
#include <QComboBox>
#include <QFrame>
#include <QLabel>
#include <QMenu>
#include <QScrollArea>
#include <QVBoxLayout>

#include <QECommon.h>
#include <QELabel.h>
#include <QEStringFormatting.h>
#include <QEResizeableFrame.h>

#include "QEPvProperties.h"
#include "QEPvPropertiesUtilities.h"


#define DEBUG qDebug() << "QEPvProperties::" << __FUNCTION__ << ":" << __LINE__

//==============================================================================
// class wide data
//==============================================================================
//
static bool recordSpecsAreInitialised = false;       // setup housekeeping
static QERecordSpec *pDefaultRecordSpec = NULL;      // default for unknown record types
static QERecordSpecList recordSpecList;              // list of record type specs


//==============================================================================
// Utilities function
//==============================================================================
//
// This function is idempotent
//
static void initialiseRecordSpecs ()
{
   bool okay;

   // If already setup then exit.
   //
   if (recordSpecsAreInitialised) return;
   recordSpecsAreInitialised = true;

   recordSpecList.clear ();

   // Create a record spec to be used as default if we given an unknown record type.
   // All the common fields plus meta field RTYP plus VAL.
   //
   pDefaultRecordSpec = new QERecordSpec ("_default_");
   (*pDefaultRecordSpec)
         << "RTYP" << "NAME" << "DESC" << "ASG"  << "SCAN" << "PINI" << "PHAS"
         << "EVNT" << "TSE"  << "TSEL" << "DTYP" << "DISV" << "DISA" << "SDIS"
         << "DISP" << "PROC" << "STAT" << "SEVR" << "NSTA" << "NSEV" << "ACKS"
         << "ACKT" << "DISS" << "LCNT" << "PACT" << "PUTF" << "RPRO" << "PRIO"
         << "TPRO" << "UDF"  << "FLNK" << "VAL";

   okay = false;

   // First process the internal file list (from resource file).
   //
   okay |= recordSpecList.processRecordSpecFile (":/qe/pvproperties/record_field_list.txt");

   // Next agument from any file specified using the environment variable.
   //
   okay |= recordSpecList.processRecordSpecFile (getenv ("QE_RECORD_FIELD_LIST"));

   // Lastly augment used file in current (startup) directory.
   //
   okay |= recordSpecList.processRecordSpecFile ("./record_field_list.txt");

   if (okay == false) {
      DEBUG << "unable to read any record field files";
   }
}


//==============================================================================
// Tables columns
//
#define FIELD_COL                 0
#define VALUE_COL                 1
#define NUNBER_COLS               2
#define DEFAULT_SECTION_SIZE      22

#define WIDGET_MIN_WIDTH          340
#define WIDGET_MIN_HEIGHT         400

#define WIDGET_DEFAULT_WIDTH      448
#define WIDGET_DEFAULT_HEIGHT     448

#define ENUMERATIONS_MIN_HEIGHT   12
#define ENUMERATIONS_MAX_HEIGHT   100
#define NUMBER_OF_ENUMERATIONS    32


//==============================================================================
// WidgetHolder class functions
//==============================================================================
//
class QLabelList : public QList<QLabel *> {};

class QEPvProperties::OwnWidgets {
public:
   OwnWidgets (QEPvProperties *parent);
   ~OwnWidgets ();

   QFrame *topFrame;
   QLabel *label1;
   QLabel *label2;
   QLabel *label3;
   QLabel *label4;
   QLabel *label5;
   QLabel *label6;
   QComboBox *box;
   QELabel *valueLabel;
   QLabel *hostName;
   QLabel *fieldType;
   QLabel *timeStamp;
   QLabel *indexInfo;
   QVBoxLayout *topFrameVlayout;
   QHBoxLayout *hlayouts [6];

   QTableWidget *table;
   QMenu *tableContextMenu;
   QFrame *enumerationFrame;
   QLabelList enumerationLabelList;
   QScrollArea *enumerationScroll;
   QEResizeableFrame * enumerationResize;
   QVBoxLayout *vlayout;
};

//------------------------------------------------------------------------------
//
QEPvProperties::OwnWidgets::OwnWidgets (QEPvProperties * parent)
{
   const int label_height = 18;
   const int label_width = 48;

   int j;

   // Creates all the internal widgets including basic geometry.
   //
   this->topFrame = new QFrame (parent);
   this->topFrame->setFixedHeight (128);     // go on - do the sums...
   this->topFrame->setObjectName ("topFrame");

   this->topFrameVlayout = new QVBoxLayout (this->topFrame);
   this->topFrameVlayout->setContentsMargins (0, 2, 0, 4);  // l, t, r, b
   this->topFrameVlayout->setSpacing (6);
   this->topFrameVlayout->setObjectName ("topFrameVlayout");

   for (j = 1; j <= 5; j++) {
      this->hlayouts [j] = new QHBoxLayout ();
      this->hlayouts [j]->setContentsMargins (2, 0, 2, 0);  // l, t, r, b
      this->hlayouts [j]->setSpacing (6);

      this->topFrameVlayout->addLayout (this->hlayouts [j], 1);
   }

   this->label1 = new QLabel ("NAME", this->topFrame);
   this->label1->setFixedSize (QSize (label_width, label_height));
   this->box = new QComboBox (this->topFrame);
   this->box->setFixedHeight (label_height + 9);
   this->hlayouts [1]->addWidget (this->label1, 0, Qt::AlignVCenter);
   this->hlayouts [1]->addWidget (this->box, 0, Qt::AlignVCenter);

   this->label2 = new QLabel ("VAL", this->topFrame);
   this->label2->setFixedSize (QSize (label_width, label_height));
   this->valueLabel = new QELabel (this->topFrame);
   this->valueLabel->setFixedHeight (label_height);
   this->hlayouts [2]->addWidget (this->label2);
   this->hlayouts [2]->addWidget (this->valueLabel);

   this->label3 = new QLabel ("HOST", this->topFrame);
   this->label3->setFixedSize (QSize (label_width, label_height));
   this->hostName = new QLabel (this->topFrame);
   this->hostName->setFixedHeight (label_height);
   this->hlayouts [3]->addWidget (this->label3);
   this->hlayouts [3]->addWidget (this->hostName);

   this->label4 = new QLabel ("TIME", this->topFrame);
   this->label4->setFixedSize (QSize (label_width, label_height));
   this->timeStamp = new QLabel (this->topFrame);
   this->timeStamp->setFixedHeight (label_height);
   this->hlayouts [4]->addWidget (this->label4);
   this->hlayouts [4]->addWidget (this->timeStamp);

   this->label5 = new QLabel ("DBF", this->topFrame);
   this->label5->setFixedSize (QSize (label_width, label_height));
   this->fieldType = new QLabel (this->topFrame);
   this->fieldType->setFixedHeight (label_height);
   this->label6 = new QLabel ("INDEX", this->topFrame);
   this->label6->setFixedSize (QSize (label_width, label_height));
   this->indexInfo = new QLabel (this->topFrame);
   this->indexInfo->setFixedHeight (label_height);
   this->hlayouts [5]->addWidget (this->label5);
   this->hlayouts [5]->addWidget (this->fieldType);
   this->hlayouts [5]->addWidget (this->label6);
   this->hlayouts [5]->addWidget (this->indexInfo);

   this->enumerationFrame = new QFrame (NULL); // is re-pareneted by enumerationScroll
   for (j = 0; j < NUMBER_OF_ENUMERATIONS; j++) {
      QLabel * item;
      item = new QLabel (this->enumerationFrame);
      item->setGeometry (0, 0, 128, label_height);
      this->enumerationLabelList.append (item);
   }

   // Create scrolling area and add pv frame.
   //
   this->enumerationScroll = new QScrollArea ();          // this will become parented by enumerationResize
   this->enumerationScroll->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOn);
   this->enumerationScroll->setWidgetResizable (true);    // MOST IMPORTANT
   this->enumerationScroll->setWidget (this->enumerationFrame);

   // Create user controllable resize area
   //
   this->enumerationResize = new QEResizeableFrame (ENUMERATIONS_MIN_HEIGHT, ENUMERATIONS_MAX_HEIGHT, parent);
   this->enumerationResize->setFixedHeight (ENUMERATIONS_MIN_HEIGHT);
   this->enumerationResize->setFrameShape (QFrame::Panel);
   this->enumerationResize->setGrabberToolTip ("Re size enuerations");
   this->enumerationResize->setWidget (this->enumerationScroll);

   // We create this with 40 rows initially - this will get expanded if/when necessary.
   // Mainly want enough to make it look sensible in designer.
   //
   this->table = new QTableWidget (40, NUNBER_COLS, parent);
   this->tableContextMenu = new QMenu (parent);

   this->vlayout = new QVBoxLayout (parent);
   this->vlayout->setMargin (4);
   this->vlayout->setSpacing (4);
   this->vlayout->addWidget (this->topFrame);
   this->vlayout->addWidget (this->enumerationResize);
   this->vlayout->addWidget (this->table);
}

//------------------------------------------------------------------------------
//
QEPvProperties::OwnWidgets::~OwnWidgets ()
{
   // TODO: Find out if we need to explicitly delete QTableWidgetItems associated
   // with the table? or does that happen as part of the table delete?
}


//==============================================================================
// QEPvProperties class functions
//==============================================================================
//
QEPvProperties::QEPvProperties (QWidget * parent) :
      QFrame (parent),
      QEWidget (this)
{
   this->recordBaseName = "";
   this->common_setup ();
}

//------------------------------------------------------------------------------
//
QEPvProperties::QEPvProperties (const QString & variableName, QWidget * parent) :
      QFrame (parent),
      QEWidget (this)
{
   this->recordBaseName = QERecordFieldName::recordName (variableName);
   this->common_setup ();
   setVariableName (variableName, 0);
   this->ownWidgets->valueLabel->setVariableName (variableName, 0);
}

//------------------------------------------------------------------------------
// NB. Need to do a deep clear to avoid memory loss.
//
QEPvProperties::~QEPvProperties ()
{
   QEString *qca;

   // Free up all allocated QEString objects.
   //
   while (!this->fieldChannels.isEmpty ()) {
      qca = this->fieldChannels.takeFirst ();
      if (qca) {
         delete qca;
      }
   }

   if (this->ownWidgets) {
      delete this->ownWidgets;
   }
}

//------------------------------------------------------------------------------
//
QSize QEPvProperties::sizeHint () const {
   return QSize (WIDGET_DEFAULT_WIDTH, WIDGET_DEFAULT_HEIGHT);
}

//------------------------------------------------------------------------------
//
void QEPvProperties::common_setup ()
{
   OwnWidgets *own;
   QTableWidgetItem * item;
   QString style;
   int j;
   QLabel *enumLabel;

   this->m = 1;
   this->d = 1;

   // This function only perform required actions on first call.
   //
   initialiseRecordSpecs ();

   this->fieldChannels.clear ();

   // configure the panel and create contents
   //
   this->setFrameShape (Panel);

   // allocate and configure own widgets
   // ...and setup an alias
   //
   own = this->ownWidgets = new OwnWidgets (this);

   // Configure widgets
   //
   own->box->setEditable (true);
   own->box->setMaxCount (36);
   own->box->setMaxVisibleItems(20);
   own->box->setEnabled (true);
   // These two don't seem to enforce what one might sensibly expect.
   own->box->setInsertPolicy (QComboBox::InsertAtTop);
   own->box->setDuplicatesEnabled (false);

   // We use the activated signal (as opposed to currentIndexChanged) as it
   // is only emmited on User change.
   //
   QObject::connect (own->box, SIGNAL (activated              (int)),
                     this,     SLOT   (boxCurrentIndexChanged (int)));

   // We allow ourselves to select the index programatically.
   //
   QObject::connect (this,     SIGNAL (setCurrentBoxIndex (int)),
                     own->box, SLOT   (setCurrentIndex    (int)));

   style = "QWidget { background-color: #F0F0F0; }";

   own->valueLabel->setIndent (4);
   own->valueLabel->setStyleSheet (style);
   // We have to be general here
   own->valueLabel->setPrecision (9);
   own->valueLabel->setUseDbPrecision (false);
   own->valueLabel->setNotationProperty (QELabel::Automatic);

   own->hostName->setIndent (4);
   own->hostName->setStyleSheet (style);

   own->timeStamp->setIndent (4);
   own->timeStamp->setStyleSheet (style);

   own->fieldType->setAlignment(Qt::AlignHCenter);
   own->fieldType->setStyleSheet (style);

   own->indexInfo->setAlignment(Qt::AlignRight);
   own->indexInfo->setIndent (4);
   own->indexInfo->setStyleSheet (style);

   for (j = 0; j < own->enumerationLabelList.count (); j++) {
      enumLabel = own->enumerationLabelList.value (j);
      enumLabel->setIndent (4);
      enumLabel->setStyleSheet (style);
   }

   item = new QTableWidgetItem (" Field ");
   own->table->setHorizontalHeaderItem (FIELD_COL, item);

   item = new QTableWidgetItem (" Value ");
   own->table->setHorizontalHeaderItem (VALUE_COL, item);

   own->table->horizontalHeader()->setDefaultSectionSize (60);
   own->table->horizontalHeader()->setStretchLastSection (true);

   own->table->verticalHeader()->hide ();
   own->table->verticalHeader()->setDefaultSectionSize (DEFAULT_SECTION_SIZE);


   // Setup layout of widgets with the QEPvProperties QFrame
   //
   this->setMinimumWidth (WIDGET_MIN_WIDTH);
   this->setMinimumHeight(WIDGET_MIN_HEIGHT);

   this->fieldStringFormatting.setAddUnits (false);
   this->fieldStringFormatting.setUseDbPrecision (false);
   this->fieldStringFormatting.setPrecision (12);
   this->fieldStringFormatting.setNotation (QEStringFormatting::NOTATION_AUTOMATIC);

   // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   // Framework boiler-plate stuff.
   //
   // This control used a single PV via the framework.
   //
   setNumVariables (1);

   // Enable drag drop onto this widget by default.
   //
   this->setAllowDrop (true);

   // By default, the PV properties widget does not display the alarm state.
   // The interbal VALue widget does this on our behalf.
   //
   this->setDisplayAlarmState (false);

   // Use standard context menu for overall widget.
   //
   setupContextMenu (this);

   // Do special context for the table.
   //
   own->table->setContextMenuPolicy (Qt::CustomContextMenu);

   QObject::connect (own->table, SIGNAL (customContextMenuRequested (const QPoint &)),
                     this,       SLOT   (customContextMenuRequested (const QPoint &)));

   QObject::connect (own->tableContextMenu, SIGNAL (triggered             (QAction* )),
                     this,                  SLOT   (contextMenuTriggered  (QAction* )));


   // Set up a connection to recieve variable name property changes
   // The variable name property manager class only delivers an updated
   // variable name after the user has stopped typing.
   //
   QObject::connect (
         &variableNamePropertyManager, SIGNAL (newVariableNameProperty    (QString, QString, unsigned int)),
         this,                         SLOT   (useNewVariableNameProperty (QString, QString, unsigned int)));
}

//------------------------------------------------------------------------------
//
void QEPvProperties::scaleBy (const int mIn, const int dIn)
{
   // If sensible, save new scaling values..
   //
   if (m > 0 && d > 0) {
      this->m = mIn;
      this->d = dIn;
   }
}

//------------------------------------------------------------------------------
//
void  QEPvProperties::resizeEvent (QResizeEvent *)
{
   OwnWidgets *own = this->ownWidgets;

   QRect g;
   QLabel *enumLabel;
   int pw;
   int ew;
   int epr;  // enumerations per row.
   int gap;
   int lh;   // label height
   int lw;   // label width
   int j;

   // Have we been scaled ??
   //
   gap = (4 * this->m) / this->d;
   ew = (172 *this->m) / this->d;

   pw = own->enumerationFrame->width ();
   epr = MAX (1, (pw / ew));    // calc enumerations per row.
   lw = ((pw - gap)/ epr) - gap;
   lh = own->enumerationLabelList.value (0)->geometry().height();

   for (j = 0; j < own->enumerationLabelList.count (); j++) {
      enumLabel = own->enumerationLabelList.value (j);
      enumLabel->setGeometry (gap + (j%epr)*(lw + gap), gap + (j/epr)*(lh + gap), lw, lh);
   }
}

//------------------------------------------------------------------------------
//
void QEPvProperties::useNewVariableNameProperty (QString variableNameIn,
                                                 QString variableNameSubstitutionsIn,
                                                 unsigned int variableIndex )
{
   this->setVariableNameAndSubstitutions (variableNameIn, variableNameSubstitutionsIn, variableIndex);
}


//------------------------------------------------------------------------------
// NB. Need to do a deep clear to avoid memory loss.
//
void QEPvProperties::clearFieldChannels ()
{
   QTableWidget *table = this->ownWidgets->table;
   QEString *qca;
   QTableWidgetItem *item;
   QString gap ("           ");  // empirically found to be quivilent width of " DESC "
   int j;

   while (!this->fieldChannels.isEmpty ()) {
      qca = this->fieldChannels.takeFirst ();
      delete qca;
   }

   for (j = 0; j < table->rowCount (); j++) {
      item = table->verticalHeaderItem (j);
      if (item) {
         item->setText (gap);
      }

      item = table->item (j, 0);
      if (item) {
         item->setText ("");
      }
   }
}

//------------------------------------------------------------------------------
//
qcaobject::QCaObject* QEPvProperties::createQcaItem (unsigned int variableIndex)
{
   QComboBox *box = this->ownWidgets->box;
   QString substitutedPVName;
   int slot;

   if (variableIndex != 0) {
      DEBUG << "unexpected variableIndex" << variableIndex;
      return NULL;
   }

   substitutedPVName = this->getSubstitutedVariableName (0).trimmed ();
   this->recordBaseName = QERecordFieldName::recordName (substitutedPVName);

   // Clear associated data fields.
   //
   this->ownWidgets->label2->setText (QERecordFieldName::fieldName (substitutedPVName));
   this->ownWidgets->hostName->setText ("");
   this->ownWidgets->timeStamp->setText ("");
   this->ownWidgets->fieldType->setText ("");
   this->ownWidgets->indexInfo->setText ("");
   this->ownWidgets->valueLabel->setText ("");


   //-----start--do--this---------------------------------
   // Move to establishConnection??
   //-----------------------------------------------------
   // Clear any exiting field connections.
   //
   this->clearFieldChannels ();

   // Remove this name from mid-list if it exists and (re) insert at top of list.
   //
   for (slot = box->count() - 1; slot >= 0; slot--) {
      if (box->itemText (slot).trimmed () == substitutedPVName) {
         box->removeItem (slot);
      }
   }

   // Maksure at least 2 free slots - one for this PV and one
   // for the user to type.
   //
   while (box->count() >= box->maxCount () - 2) {
      box->removeItem (box->count () - 1);
   }

   box->insertItem (0, substitutedPVName, QVariant ());

   // Ensure CombBox consistent .
   //
   emit setCurrentBoxIndex (0);

   //-----end--do--this-----------------------------------


   // Regardless of the actual PV, we need to connect to the RTYP pseudo field
   // of the associated record.
   //
   return new QEString (QERecordFieldName::rtypePvName (substitutedPVName),
                        this, &stringFormatting, 0);
}

//------------------------------------------------------------------------------
//
void QEPvProperties::establishConnection (unsigned int variableIndex)
{
   qcaobject::QCaObject* qca;

   if (variableIndex != 0) {
      DEBUG << "unexpected variableIndex" << variableIndex;
      return;
   }

   // Create a connection.
   // If successfull, the QCaObject object that will supply data update signals will be returned
   // Note createConnection creates the connection and returns reference to existing QCaObject.
   //
   qca = createConnection (variableIndex);

   // If a QCaObject object is now available to supply data update signals, connect it to the appropriate slots
   //
   if (qca) {
      QObject::connect (qca,  SIGNAL (connectionChanged (QCaConnectionInfo&)),
                        this, SLOT (setRecordTypeConnection (QCaConnectionInfo& )));

      QObject::connect (qca, SIGNAL (stringChanged (const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& )),
                        this, SLOT (setRecordTypeValue (const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& )));
   }
}

//------------------------------------------------------------------------------
//
void QEPvProperties::setRecordTypeConnection (QCaConnectionInfo& connectionInfo)
{
   QString substitutedPVName;
   qcaobject::QCaObject *qca;
   QELabel *valueLabel = this->ownWidgets->valueLabel;   // brevity and SDK auto complete

   // Update tool tip, but leave the basic widget enabled.
   //
   updateToolTipConnection (connectionInfo.isChannelConnected ());

   if (connectionInfo.isChannelConnected ()) {

      // The pseudo RTYP field has connected - we are good to go...
      //
      substitutedPVName = this->getSubstitutedVariableName (0).trimmed ();;

      // Set PV name of internal QELabel.
      //
      valueLabel->setVariableNameAndSubstitutions (substitutedPVName, "", 0);

      // We know that QELabels use slot zero for the connection.
      //
      qca = valueLabel->getQcaItem (0);
      if (qca) {
         QObject::connect (qca, SIGNAL (connectionChanged  (QCaConnectionInfo&) ),
                           this,  SLOT (setValueConnection (QCaConnectionInfo&) ) );

         QObject::connect (qca, SIGNAL (stringChanged (const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                           this,  SLOT (setValueValue (const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );

      } else {
         qDebug () << __FUNCTION__ << " no qca object";
      }
   }
}


//------------------------------------------------------------------------------
// Called when notified of the (new) record type value.
//
void QEPvProperties::setRecordTypeValue (const QString & rtypeValue,
                                         QCaAlarmInfo&,
                                         QCaDateTime&,
                                         const unsigned int&)
{
   QTableWidget *table = this->ownWidgets->table;

   int j;
   QERecordSpec *pRecordSpec;
   int numberOfFields;
   QString fieldName;
   QTableWidgetItem *item;
   QString pvName;
   QEString *qca;

   // Look for the record spec for the given record type if it exists.
   //
   pRecordSpec = recordSpecList.find (rtypeValue);

   // If we didn't find the specific record type, use the default record spec.
   //
   if (!pRecordSpec) {
       pRecordSpec = pDefaultRecordSpec;
   }

   // If we didn't find the specific record type or the default record spec
   // not defined then quit.
   //
   if(!pRecordSpec) {
      // Output some error???
      return;
   }

   // It is possible that a record may change RTYP (e.g. calc to calcout while IOC is off line)
   // Cannot rely soley on the clear called in createQcaItem / establish connection.
   //
   this->clearFieldChannels ();

   numberOfFields = pRecordSpec->size ();

   table->setRowCount (numberOfFields);
   for (j = 0; j < numberOfFields; j++) {
      fieldName = pRecordSpec->getFieldName (j);

      // Ensure vertical header exists and set it.
      //
      item = table->item (j, FIELD_COL);
      if (!item) {
         // We need to allocate iteem and inset into the table.
         item = new QTableWidgetItem ();
         table->setItem (j, FIELD_COL, item);
      }
      item->setText  (" " + fieldName + " ");

      // Ensure table entry item exists.
      //
      item = table->item (j, VALUE_COL);
      if (!item) {
         // We need to allocate item and inset into the table.
         item = new QTableWidgetItem ();
         table->setItem (j, VALUE_COL, item);
      }

      // Form the required PV name.
      //
      pvName = this->recordBaseName + "." + fieldName;

      qca = new QEString (pvName, this, &this->fieldStringFormatting, j);

      QObject::connect (qca, SIGNAL (stringConnectionChanged (QCaConnectionInfo&, const unsigned int& )),
                        this,  SLOT (setFieldConnection (QCaConnectionInfo&, const unsigned int& )));

      QObject::connect (qca, SIGNAL (stringChanged (const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& )),
                        this,  SLOT (setFieldValue (const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& )));

      qca->subscribe();

      this->fieldChannels.append (qca);
   }

   // The alarmInfo not really applicabe to the RTYP field.
   // We pick up on the VAL field instead.
}

//------------------------------------------------------------------------------
//
void QEPvProperties::setValueConnection (QCaConnectionInfo& connectionInfo)
{
   qcaobject::QCaObject *qca;
   QString s;

   // These are not QELabels - so gotta do manually.
   //
   this->ownWidgets->hostName->setEnabled  (connectionInfo.isChannelConnected ());
   this->ownWidgets->timeStamp->setEnabled (connectionInfo.isChannelConnected ());
   this->ownWidgets->fieldType->setEnabled (connectionInfo.isChannelConnected ());
   this->ownWidgets->indexInfo->setEnabled (connectionInfo.isChannelConnected ());

   if (connectionInfo.isChannelConnected ()) {
      // We "know" that the only/main channel is the 1st (slot 0) channel.
      //
      qca = this->ownWidgets->valueLabel->getQcaItem (0);
      this->ownWidgets->hostName->setText (qca->getHostName());
      this->ownWidgets->fieldType->setText (qca->getFieldType());

      // Assume we are looking at 1st/only element for now.
      //
      s.sprintf ("%d / %ld", 1,  qca->getElementCount());
      this->ownWidgets->indexInfo->setText (s);
      this->isFirstUpdate = true;
   }
}

//------------------------------------------------------------------------------
//
void QEPvProperties::setValueValue (const QString &,
                                    QCaAlarmInfo& alarmInfo,
                                    QCaDateTime& dateTime,
                                    const unsigned int&)
{
   OwnWidgets *own = this->ownWidgets;

   qcaobject::QCaObject *qca;
   QStringList enumerations;
   QLabel *enumLabel;
   QLabel *enumLast;
   int n;
   int j;
   QRect g;
   int h;

   // NOTE: The value label updates itself.
   //
   own->timeStamp->setText (dateTime.text () + "  " + QEUtilities::getTimeZoneTLA (dateTime));

   if (this->isFirstUpdate) {

      // Ensure we do any required resizing.
      //
      this->resizeEvent (NULL);

      // Set up any enumeration values
      // We "know" that the only/main channel is the 1st (slot 0) channel.
      //
      qca = this->ownWidgets->valueLabel->getQcaItem (0);
      enumerations = qca->getEnumerations ();
      n = enumerations.count();

      enumLast = NULL;
      for (j = 0; j < this->ownWidgets->enumerationLabelList.count (); j++) {
         enumLabel = this->ownWidgets->enumerationLabelList.value (j);
         if (j < n) {
            // Value is specified.
            enumLabel->setText (enumerations.value (j));
            enumLast = enumLabel;
            enumLabel->setVisible (true);
         } else {
            enumLabel->clear ();
            enumLabel->setVisible (false);
         }
      }

      if (enumLast) {
         g = enumLast->geometry ();
         h = g.top() + g.height() + 4;
      } else {
         h = 0;
      }
      own->enumerationFrame->setFixedHeight (h);

      // Set and expand to new max height.
      own->enumerationResize->setAllowedMaximum (ENUMERATIONS_MIN_HEIGHT + h);
      own->enumerationResize->setFixedHeight (ENUMERATIONS_MIN_HEIGHT + h);

      this->isFirstUpdate = false;
   }

   // Invoke common alarm handling processing.
   //
   this->processAlarmInfo (alarmInfo);
}

//------------------------------------------------------------------------------
//
void QEPvProperties::setFieldConnection (QCaConnectionInfo& connectionInfo,
                                         const unsigned int &variableIndex)
{
   QTableWidget *table = this->ownWidgets->table;
   int numberOfRows;
   QTableWidgetItem *item;

   numberOfRows = table->rowCount ();
   if ((int) variableIndex < numberOfRows) {
      item = table->item (variableIndex, VALUE_COL);

      if (connectionInfo.isChannelConnected ()) {
         // connected
         item->setForeground (QColor (0, 0, 0));
         item->setText ("");
      } else {
         // disconnected - leave old text - readable but grayed out.
         item->setForeground (QColor (160, 160, 160));
      }
   } else {
      DEBUG << "variableIndex =" << variableIndex
             << ", out of range - must be <" << numberOfRows;
   }
}

//------------------------------------------------------------------------------
//
void QEPvProperties::setFieldValue (const QString &value,
                                    QCaAlarmInfo &,
                                    QCaDateTime &,
                                    const unsigned int & variableIndex)
{
   QTableWidget *table = this->ownWidgets->table;
   int numberOfRows;
   QTableWidgetItem *item;

   numberOfRows = table->rowCount ();
   if ((int) variableIndex < numberOfRows) {
      item = table->item (variableIndex, VALUE_COL);
      item->setText  (" " + value);
   } else {
      DEBUG << "variableIndex =" << variableIndex
            << ", out of range - must be <" << numberOfRows;
   }
}

//------------------------------------------------------------------------------
// Unlike most widgets, the frame is not disabled if/when PVs disconnect
// Normally, standardProperties::setApplicationEnabled() is called
// For this widget our own version which just calls the widget's setEnabled is called.
//
void QEPvProperties::setApplicationEnabled (const bool & state)
{
    QWidget::setEnabled (state);
}

//==============================================================================
// ComboBox
//
void QEPvProperties::boxCurrentIndexChanged (int index)
{
   QComboBox *box = this->ownWidgets->box;
   QString newPvName;
   QString oldPvName;

   if (index >= 0) {
      newPvName = box->itemText (index);
      oldPvName = getSubstitutedVariableName (0);

      // belts 'n' braces.
      //
      if (newPvName != oldPvName) {
         setVariableName (newPvName, 0);
         this->establishConnection (0);
      } else {
         DEBUG <<  index << oldPvName << newPvName;
      }
   }
}

//==============================================================================
// Conextext menu.
//
void QEPvProperties::customContextMenuRequested (const QPoint & posIn)
{
   QTableWidget *table = this->ownWidgets->table;
   QMenu *menu = this->ownWidgets->tableContextMenu;
   QTableWidgetItem * item;
   QString trimmed;
   QString newPV;
   QAction *action;
   QPoint pos = posIn;
   QPoint golbalPos;

   // Find the associated item
   //
   item = table->itemAt (posIn);
   if (!item) {
      return;  // just in case
   }

   trimmed = item->text ().trimmed();

   switch (item->column ()) {
      case FIELD_COL:
         newPV = QERecordFieldName::fieldPvName (this->recordBaseName, trimmed);
         break;

      case VALUE_COL:
         QERecordFieldName::extractPvName (trimmed, newPV);
         break;

      default:
         DEBUG << "unexpected column number:" << item->column () << trimmed;
         newPV = "";
         return;
   }

   action = new QAction ("Properties", menu);
   action->setCheckable (false);
   action->setData (QVariant (newPV));
   action->setEnabled (!newPV.isEmpty ());
   menu->clear ();
   menu->addAction (action);

   pos.setY (pos.y () + DEFAULT_SECTION_SIZE);  // A feature of QTableWiget (because header visible maybe?).
   golbalPos = table->mapToGlobal (pos);
   menu->exec (golbalPos, 0);
}

//------------------------------------------------------------------------------
//
void QEPvProperties::contextMenuTriggered (QAction* action)
{
   QString newPV;

   if (action) {
      newPV = action->data ().toString ();
      this->setVariableName (newPV , 0);
      this->establishConnection (0);
   }
}

//==============================================================================
// Drag / drop
//
void QEPvProperties::saveConfiguration (PersistanceManager* pm)
{
   const QString formName = this->persistantName ("QEPvProperties");
   PMElement formElement = pm->addNamedConfiguration (formName);

   // qDebug () << "\nQEPvProperties " << __FUNCTION__ << formName << "\n";

   // Note: we save the subsituted name (as opposed to template name and any macros).
   //
   formElement.addValue ("Name", this->getSubstitutedVariableName (0));

}

//------------------------------------------------------------------------------
//
void QEPvProperties::restoreConfiguration (PersistanceManager* pm, restorePhases restorePhase)
{
   if (restorePhase != FRAMEWORK) return;

   const QString formName = this->persistantName ("QEPvProperties");
   PMElement formElement = pm->getNamedConfiguration (formName);
   bool status;
   QString pvName;

   // qDebug () << "\nQEPvProperties " << __FUNCTION__ << formName <<  restorePhase << "\n";

   if ((restorePhase == FRAMEWORK) && !formElement.isNull ()) {
      status = formElement.getValue ("Name", pvName);
      if (status) {
         this->setPvName (pvName);
      }
   }
}

//==============================================================================
//
void QEPvProperties::setPvName (const QString& pvNameIn)
{
   this->setVariableName (pvNameIn, 0);
   this->establishConnection (0);
}


//==============================================================================
// Drag / drop
//
void QEPvProperties::setDrop (QVariant drop)
{
   this->setPvName (drop.toString ());
}

//------------------------------------------------------------------------------
//
QVariant QEPvProperties::getDrop ()
{
   if( isDraggingVariable() )
      return QVariant (this->copyVariable ());
   else
      return this->copyData();
}


//==============================================================================
// Copy / Paste
//
QString QEPvProperties::copyVariable ()
{
   return this->getSubstitutedVariableName (0);
}

//------------------------------------------------------------------------------
//
QVariant QEPvProperties::copyData ()
{
   QTableWidget* table = ownWidgets->table;
   QTableWidgetItem *f, *v;
   QString fieldList;

   // Create csv format.
   //
   fieldList.clear ();
   for (int i = 0; i < table->rowCount(); i++) {
      f = table->item (i, FIELD_COL);
      v = table->item (i, VALUE_COL);
      fieldList.append (f->text ()).append (",").append (v->text ().append ("\n"));
   }
   return QVariant (fieldList);
}

//------------------------------------------------------------------------------
//
void QEPvProperties::paste( QVariant v )
{
   if (this->getAllowDrop ()) {
      this->setPvName (v.toString ());
   }
}

// end
