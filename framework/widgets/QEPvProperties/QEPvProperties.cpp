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
#include <QVBoxLayout>

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
static bool record_specs_are_initialised = false;    // setup housekeeping
static QERecordSpec *pDefaultRecordSpec = NULL;      // default for unknown record types
static QERecordSpecList recordSpecList;              // list of record type specs


//==============================================================================
// Utilities function
//==============================================================================
//
// This function is idempotent
//
static void initialise_record_specs ()
{
   bool okay;

   // If already setup then exit.
   //
   if (record_specs_are_initialised) return;
   record_specs_are_initialised = true;

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
   okay |= recordSpecList.process_record_spec_file (":/qe/pvproperties/record_field_list.txt");

   // Next agument from any file specified using the environment variable.
   //
   okay |= recordSpecList.process_record_spec_file (getenv ("QE_RECORD_FIELD_LIST"));

   // Lastly augment used file in current (startup) directory.
   //
   okay |= recordSpecList.process_record_spec_file ("./record_field_list.txt");
   
   if (okay == false) {
      qDebug () << __FUNCTION__ << __LINE__ << "unable to read any record field files";
   }
}


//==============================================================================
// Tables columns
//
#define FIELD_COL              0
#define VALUE_COL              1
#define NUNBER_COLS            2

#define WIDGET_MIN_WIDTH       340
#define WIDGET_MIN_HEIGHT      246

#define WIDGET_DEFAULT_WIDTH   448
#define WIDGET_DEFAULT_HEIGHT  290


//==============================================================================
// WidgetHolder class functions
//==============================================================================
//
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
   QELabel *value;
   QLabel *hostName;
   QLabel *fieldType;
   QLabel *timeStamp;
   QLabel *indexInfo;
   QTableWidget *table;
   QMenu *tableContextMenu;
   QFrame *enumerations;
   QEResizeableFrame * enumerationResize;
   QVBoxLayout *layout;
};

//------------------------------------------------------------------------------
//
QEPvProperties::OwnWidgets::OwnWidgets (QEPvProperties * parent)
{
   // Creates all the internal widgets, and apart from static labels, does no
   // other configuration or setup per se.
   //
   this->topFrame = new QFrame (parent);
   this->topFrame->setFixedHeight (120);   // a sort of gues - this is recalulated later

   this->label1 = new QLabel ("NAME", this->topFrame);
   this->label2 = new QLabel ("VAL", this->topFrame);
   this->label3 = new QLabel ("HOST", this->topFrame);
   this->label4 = new QLabel ("TIME", this->topFrame);
   this->label5 = new QLabel ("DBF", this->topFrame);
   this->label6 = new QLabel ("INDEX", this->topFrame);

   this->box = new QComboBox (this->topFrame);
   this->value = new QELabel (this->topFrame);
   this->hostName = new QLabel (this->topFrame);
   this->fieldType = new QLabel (this->topFrame);
   this->timeStamp = new QLabel (this->topFrame);
   this->indexInfo = new QLabel (this->topFrame);


   this->enumerations = new QFrame (NULL); // is re-pareneted
   this->enumerationResize = new QEResizeableFrame (10, 80, parent);
   this->enumerationResize->setFixedHeight (10);
   this->enumerationResize->setFrameShape (QFrame::Panel);
   this->enumerationResize->setGrabberToolTip ("Re size enuerations");
   this->enumerationResize->setWidget (this->enumerations);

   // We create this with 40 rows initially - this will get expanded if/when necessary.
   // Mainly want enough to make it look sensible in designer.
   //
   this->table = new QTableWidget (40, NUNBER_COLS, parent);
   this->tableContextMenu = new QMenu (parent);

   this->layout = new QVBoxLayout (parent);
   this->layout->setMargin (4);
   this->layout->setSpacing (4);
   this->layout->addWidget (this->topFrame);
   this->layout->addWidget (this->enumerationResize);
   this->layout->addWidget (this->table);
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
   this->ownWidgets->value->setVariableName (variableName, 0);
}

//------------------------------------------------------------------------------
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

   // This control uses a single data source (from a designer point of view)
   //
   this->setNumVariables (1);

   // This function only perform required actions on first call.
   //
   initialise_record_specs ();

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

   own->value->setIndent (4);
   own->value->setStyleSheet (style);
   // We have to be general here
   own->value->setPrecision (9);
   own->value->setUseDbPrecision (false);
   own->value->setNotationProperty (QELabel::Automatic);

   own->hostName->setIndent (4);
   own->hostName->setStyleSheet (style);

   own->timeStamp->setIndent (4);
   own->timeStamp->setStyleSheet (style);

   own->fieldType->setAlignment(Qt::AlignHCenter);
   own->fieldType->setStyleSheet (style);

   own->indexInfo->setAlignment(Qt::AlignRight);
   own->indexInfo->setIndent (4);
   own->indexInfo->setStyleSheet (style);

   item = new QTableWidgetItem (" Field ");
   own->table->setHorizontalHeaderItem (FIELD_COL, item);

   item = new QTableWidgetItem (" Value ");
   own->table->setHorizontalHeaderItem (VALUE_COL, item);

   own->table->horizontalHeader()->setDefaultSectionSize (60);
   own->table->horizontalHeader()->setStretchLastSection (true);

   own->table->verticalHeader()->hide ();
   own->table->verticalHeader()->setDefaultSectionSize (22);


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

   // Set the initial state
   //
   lastSeverity = QCaAlarmInfo::getInvalidSeverity ();

   // Unlike most CA aware widgets the PV Properties panel enabled-ness does
   // not reflect the (initial) disconnected state.
   //
   QWidget::setEnabled (true);

   // Use standard context menu for overall widget.
   //
   setupContextMenu (this);

   // Do special for the table.
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
// NB. Need to do a deep clear to avoid memory loss.
//
void  QEPvProperties::resizeEvent (QResizeEvent *)
{
   OwnWidgets *own = this->ownWidgets;
   int pw, ph;   //
   int x, y;
   int wh;       // widget height
   int lw;       // label width
   int fw;       // field width

   // Get current width and height.
   //
   pw = own->topFrame->width ();
   ph = own->topFrame->height ();

   lw = 48;
   fw = pw - lw - 18;
   wh = 18;
   x = 6;
   y = 4;
   own->label1->setGeometry    (x,    y + 6, lw, wh);
   own->box->setGeometry       (lw + 12,  y, fw, 27); y += 30;
   own->label2->setGeometry    (x,        y, lw, wh);
   own->value->setGeometry     (lw + 12,  y, fw, wh); y += 22;
   own->label3->setGeometry    (x,        y, lw, wh);
   own->hostName->setGeometry  (lw + 12,  y, fw, wh); y += 22;
   own->label4->setGeometry    (x,        y, lw, wh);
   own->timeStamp->setGeometry (lw + 12,  y, fw, wh); y += 22;

   fw = (pw - (48 + 2*lw)) / 2;
   own->label5->setGeometry    (x, y, lw, wh); x += lw + 6;
   own->fieldType->setGeometry (x, y, fw, wh); x += fw + 24;
   own->label6->setGeometry    (x, y, lw, wh); x += lw + 6;
   own->indexInfo->setGeometry (x, y, fw, wh); y += 22;

   own->topFrame->setFixedHeight (y);
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
   qcaobject::QCaObject *qca;
   QComboBox *box = this->ownWidgets->box;
   QString pv_name;
   int slot;

   if (variableIndex != 0) {
      DEBUG << "unexpected variableIndex" << variableIndex;
      return NULL;
   }

   pv_name = getSubstitutedVariableName (0);
   pv_name = pv_name.trimmed ();
   this->recordBaseName = QERecordFieldName::recordName (pv_name);

   // Clear associated data fields.
   //
   this->ownWidgets->label2->setText (QERecordFieldName::fieldName (pv_name));
   this->ownWidgets->hostName->setText ("");
   this->ownWidgets->timeStamp->setText ("");
   this->ownWidgets->fieldType->setText ("");
   this->ownWidgets->indexInfo->setText ("");
   this->ownWidgets->value->setText("");

   // Set PV name of internal QELabel.
   //
   this->ownWidgets->value->setVariableNameAndSubstitutions (pv_name, "", 0);

   // We know that QELabels use slot zero for the connection.
   //
   qca = this->ownWidgets->value->getQcaItem (0);
   if (qca) {
      QObject::connect (qca, SIGNAL (connectionChanged  (QCaConnectionInfo&) ),
                        this,  SLOT (setValueConnection (QCaConnectionInfo&) ) );

      QObject::connect (qca, SIGNAL (stringChanged (const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                        this,  SLOT (setValueValue (const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );

   } else {
      qDebug () << __FUNCTION__ << " no qca object";
   }


   //-----start--do--this---------------------------------
   // Move to establishConnection??
   //-----------------------------------------------------
   // Clear any exiting field connections.
   //
   this->clearFieldChannels ();

   // Remove this name from mid-list if it exists and (re) insert at top of list.
   //
   for (slot = box->count() - 1; slot >= 0; slot--) {
      if (box->itemText (slot).trimmed () == pv_name) {
         box->removeItem (slot);
      }
   }

   // Maksure at least 2 free slots - one for this PV and one
   // for the user to type.
   //
   while (box->count() >= box->maxCount () - 2) {
      box->removeItem (box->count () - 1);
   }

   box->insertItem (0, pv_name, QVariant ());

   // Ensure CombBox consistent .
   //
   emit setCurrentBoxIndex (0);

   //-----end--do--this-----------------------------------


   // Regardless of the actual PV, we need to connect to the RTYP pseudo field
   // of the associated record.
   //
   return new QEString (QERecordFieldName::rtypePvName (pv_name), this, &stringFormatting, 0);
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
   // Update tool tip, but leave the basic widget enabled.
   //
   updateToolTipConnection (connectionInfo.isChannelConnected ());
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
      qca = this->ownWidgets->value->getQcaItem (0);
      this->ownWidgets->hostName->setText (qca->getHostName());
      this->ownWidgets->fieldType->setText (qca->getFieldType());

      // Assume we are looking at 1st/only element for now.
      //
      s.sprintf ("%d / %ld", 1,  qca->getElementCount());
      this->ownWidgets->indexInfo->setText (s);
   }
}

//------------------------------------------------------------------------------
//
void QEPvProperties::setValueValue (const QString &,
                                    QCaAlarmInfo& alarmInfo,
                                    QCaDateTime& dateTime,
                                    const unsigned int&)
{
    this->ownWidgets->timeStamp->setText (dateTime.text ());

    if (this->lastSeverity != alarmInfo.getSeverity ()) {
       this->lastSeverity = alarmInfo.getSeverity ();
       this->updateToolTipAlarm (alarmInfo.severityName ());
       // setStyleSheet (alarmInfo.style ());
    }
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
          qDebug () << __FUNCTION__ << __LINE__ << "BnB" <<  index << oldPvName << newPvName;
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
         DEBUG << "unexpected colum number:" << item->column () << trimmed;
         newPV = "";
         return;
   }

   menu->clear ();
   action = new QAction ("Properties", menu);
   action->setCheckable (false);
   action->setData (QVariant (newPV));
   action->setEnabled (!newPV.isEmpty ());
   menu->addAction (action);

   pos.setY (pos.y () + 22);  // A feature of QTableWiget or header vizible.
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
// Drag drop
//
void QEPvProperties::setDrop (QVariant drop)
{
   setVariableName( drop.toString(), 0);
   this->establishConnection (0);
}

//------------------------------------------------------------------------------
//
QVariant QEPvProperties::getDrop ()
{
   if( isDraggingVariable() )
      return QVariant( copyVariable() );
   else
      return copyData();
}


//==============================================================================
// Copy / Paste
QString QEPvProperties::copyVariable ()
{
    // Note: we return the record name, as opposed to the selected PV name.
    //
    return recordBaseName;
}

//------------------------------------------------------------------------------
//
QVariant QEPvProperties::copyData ()
{
   QTableWidget *table = ownWidgets->table;

   for (int i = 0; i < table->rowCount(); i++) {
        QTableWidgetItem *item = table->item (i, 0);
        qDebug() << item->text();

    }

    // !! built a list of field/values
    QString fieldValues;
    return QVariant( fieldValues );
}

//------------------------------------------------------------------------------
//
void QEPvProperties::paste( QVariant v )
{
    if (getAllowDrop ()) {
       setDrop (v);
    }
}

// end
