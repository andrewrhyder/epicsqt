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
#include <QFile>
#include <QList>
#include <QStringList>
#include <QTableWidgetItem>
#include <QTextStream>
#include <QComboBox>
#include <QFrame>
#include <QLabel>
#include <QELabel.h>

#include <QEPvProperties.h>


//==============================================================================
// Local classes
//==============================================================================
// This class provides a named (by record type: ai, bo, calc etc) list of
// record field names.
// No other info about the field (DBF_INLINK, DBF_DOUBLE, DBF_MENU etc.) provided.
//
class RecordSpec : public QStringList {
private:
   QString recordType;

public:
   RecordSpec (const QString theRecordType)
   {
      this->recordType = theRecordType;
      this->clear ();
   }

   QString getRecordType ()
   {
      return this->recordType;
   }

   QString getFieldName (const int index)
   {
      if ((0 <= index) && (index < size ())) {
         return this->at (index);
      } else {
         return "";
      }
   }
};

//------------------------------------------------------------------------------
// This class provides a list of RecordSpec specs, with additional find functions.
//
class RecordSpecList : public QList<RecordSpec *> {
private:
   int findSlot (const QString recordType);

public:
    RecordSpec *find (const QString recordType);
    void appendOrReplace ( RecordSpec *pRecordSpec);
};


//------------------------------------------------------------------------------
//
int RecordSpecList::findSlot (const QString recordType) {
   int result = -1;
   RecordSpec * pCheckSpec;

   for (int j = 0; j < this->size (); j++) {

      pCheckSpec = this->at (j);
      if (pCheckSpec->getRecordType () == recordType) {
         // Found it ;-)
         //
         result = j;
         break;
      }
   }
   return result;
}

//------------------------------------------------------------------------------
//
RecordSpec * RecordSpecList::find (const QString recordType) {
   RecordSpec * result = NULL;
   int slot;

   slot = this->findSlot(recordType);
   if (slot >= 0) {
      result = this->at (slot);
   }
   return result;
}

//------------------------------------------------------------------------------
//
void RecordSpecList::appendOrReplace ( RecordSpec *pRecordSpec)
{
   int slot;

   if (pRecordSpec) {
      slot = this->findSlot (pRecordSpec->getRecordType());
      if (slot >= 0) {
         RecordSpec *pPrevious;

         pPrevious = this->at (slot);
         this->replace (slot, pRecordSpec);
         delete pPrevious;

      } else {

         // Just append this ite.
         //
         this->append (pRecordSpec);
      }
   }
}


//==============================================================================
// class wide data
//==============================================================================
//
static bool record_specs_are_initialised = false;  // setup housekeeping
static RecordSpec *pDefaultRecordSpec = NULL;      // default for unknown record types
static RecordSpecList recordSpecList;              // list of record type specs


//==============================================================================
// Utilities functions
//==============================================================================
//
static bool process_record_spec_file (const QString& filename)
{
   if (filename == "") {
      return false;
   }

   QFile record_field_file (filename);

   if (!record_field_file.open (QIODevice::ReadOnly | QIODevice::Text)) {
      return false;
   }

   QTextStream source (&record_field_file);
   RecordSpec *pRecordSpec;
   QString recordType;
   unsigned int lineno;

   pRecordSpec = NULL;
   lineno = 0;
   while (!source.atEnd()) {
      QString line = source.readLine ().trimmed ();

      lineno++;

      // Skip empty line and comment lines.
      //
      if (line.length () == 0) continue;
      if (line.left (1) == "#") continue;

      // record types designated by: <<name>>
      //
      if ((line.left (2) == "<<") && (line.right (2) == ">>")) {

         recordType = line.mid (2, line.length() - 4).trimmed ();
         pRecordSpec = new RecordSpec (recordType);
         recordSpecList.appendOrReplace (pRecordSpec);

      } else {
         // Just a regular field.
         //
         if (pRecordSpec) {
            pRecordSpec->append (line);
         } else {
            qDebug () << __FUNCTION__ << "field occured before first record type: " << filename << lineno << line;
         }
      }
   }

   return true;
}

//------------------------------------------------------------------------------
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
   pDefaultRecordSpec = new RecordSpec ("_default_");
   (*pDefaultRecordSpec)
         << "RTYP" << "NAME" << "DESC" << "ASG"  << "SCAN" << "PINI" << "PHAS"
         << "EVNT" << "TSE"  << "TSEL" << "DTYP" << "DISV" << "DISA" << "SDIS"
         << "DISP" << "PROC" << "STAT" << "SEVR" << "NSTA" << "NSEV" << "ACKS"
         << "ACKT" << "DISS" << "LCNT" << "PACT" << "PUTF" << "RPRO" << "PRIO"
         << "TPRO" << "UDF"  << "FLNK" << "VAL";

   okay = false;

   // First process the internal file list (from resource file).
   //
   okay |= process_record_spec_file (":/qe/pvproperties/record_field_list.txt");

   // Next agument from any file specified using the environment variable.
   //
   okay |= process_record_spec_file (getenv ("QE_RECORD_FIELD_LIST"));

   // Lastly augment used file in current (startup) directory.
   //
   okay |= process_record_spec_file ("./record_field_list.txt");
   
   if (okay == false) {
      qDebug () << __FUNCTION__ << __LINE__ << "unable to read any record field files";
   }
}

//------------------------------------------------------------------------------
// Converts PV name to record name, e.g.:
//
// SR11BCM01:CURRENT_MONITOR.PREC => SR11BCM01:CURRENT_MONITOR
// SR11BCM01:CURRENT_MONITOR.VAL  => SR11BCM01:CURRENT_MONITOR
// SR11BCM01:CURRENT_MONITOR      => SR11BCM01:CURRENT_MONITOR
//
static QString record_name (const QString pvName)
{
   QString result;
   int dot_posn;

   result = pvName;
   dot_posn = result.indexOf (".", 0);
   if (dot_posn >= 0) {
      result.truncate (dot_posn);
   }
   return result;
}

//------------------------------------------------------------------------------
// Converts PV name to field name, e.g.:
//
// SR11BCM01:CURRENT_MONITOR.PREC => PREC
// SR11BCM01:CURRENT_MONITOR.VAL  => VAL
// SR11BCM01:CURRENT_MONITOR      => VAL (it's the default)
//
static QString field_name (const QString pvName)
{
   QString result = "VAL";
   int dot_posn;
   int fs;

   dot_posn = pvName.indexOf (".", 0);
   if (dot_posn >= 0) {
      fs = pvName.length() - dot_posn - 1;
      if (fs > 0) {
         result = pvName.right (fs);
      }
   }
   return result;
}

//------------------------------------------------------------------------------
// Form pseudo field record type PV name
//
static QString rtype_pv_name (const QString pvName)
{
   return record_name (pvName) + ".RTYP";
}


//==============================================================================
// WidgetHolder class functions
//==============================================================================
//
class QEPvProperties::PrivateWidgetHolder {
public:
   PrivateWidgetHolder (QWidget *parent = 0);
   ~PrivateWidgetHolder ();

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
};

//------------------------------------------------------------------------------
//
QEPvProperties::PrivateWidgetHolder::PrivateWidgetHolder (QWidget * parent)
{
   // Creates all the internal widgets, and apart from static labels, does no
   // other configuration or setup per se.
   //
   this->label1 = new QLabel ("NAME", parent);
   this->label2 = new QLabel ("VAL", parent);
   this->label3 = new QLabel ("HOST", parent);
   this->label4 = new QLabel ("TIME", parent);
   this->label5 = new QLabel ("DBF", parent);
   this->label6 = new QLabel ("INDEX", parent);

   this->box = new QComboBox (parent);
   this->value = new QELabel (parent);
   this->hostName = new QLabel (parent);
   this->fieldType = new QLabel (parent);
   this->timeStamp = new QLabel (parent);
   this->indexInfo = new QLabel (parent);
   this->table = new QTableWidget (40, 1, parent);
}


//------------------------------------------------------------------------------
//
QEPvProperties::PrivateWidgetHolder::~PrivateWidgetHolder ()
{
   // TODO: Find ou if we need to explicitly delete QTableWidgetItems associated
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
   this->recordBaseName = record_name (variableName);
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
      delete qca;
   }
}

//------------------------------------------------------------------------------
//
void QEPvProperties::setup ()
{
   // Not used but declared pure virtual in parent class.
}

//------------------------------------------------------------------------------
//
QSize QEPvProperties::sizeHint () const {
   return QSize (400, 290);
}

//------------------------------------------------------------------------------
//
void QEPvProperties::common_setup ()
{
   PrivateWidgetHolder *own;
   QTableWidgetItem * item;
   QString style;
   int pw;
   int ph;
   int y;
   int wh;
   int lw;
   int fw;
   int x;

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
   own = this->ownWidgets = new PrivateWidgetHolder (this);

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

   own->hostName->setIndent (4);
   own->hostName->setStyleSheet (style);

   own->timeStamp->setIndent (4);
   own->timeStamp->setStyleSheet (style);

   own->fieldType->setAlignment(Qt::AlignHCenter);
   own->fieldType->setStyleSheet (style);

   own->indexInfo->setAlignment(Qt::AlignRight);
   own->indexInfo->setIndent (4);
   own->indexInfo->setStyleSheet (style);

   item = new QTableWidgetItem (" Value ");
   own->table->setHorizontalHeaderItem (0, item);
   own->table->horizontalHeader ()->setStretchLastSection (true);
   own->table->verticalHeader ()->setDefaultSectionSize (22);

   // Setup layout of widgets with the QEPvProperties QFrame
   //
   pw = 400;
   ph = 290;

   this->setMinimumWidth (pw);
   this->setMinimumHeight(246);
   this->setMaximumWidth (pw);

   lw = 44;
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
   fw = 132;
   own->label5->setGeometry    (x, y, lw, wh); x += lw + 6;
   own->fieldType->setGeometry (x, y, fw, wh); x += fw + 24;
   own->label6->setGeometry    (x, y, lw, wh); x += lw + 6;
   own->indexInfo->setGeometry (x, y, fw, wh); y += 22;
   x = 6;
   own->table->setGeometry     (x, y, pw - 12, ph - y - 6);

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

   // Use standard context menu
   setupContextMenu( this );

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
void  QEPvProperties::resizeEvent ( QResizeEvent * )
{
   QTableWidget *table = this->ownWidgets->table;
   QRect tg;

   tg = table->geometry ();
   tg.setHeight (this->height () - tg.top () -6);
   table->setGeometry  (tg);
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
      qDebug () << __FUNCTION__ << __LINE__ << "unexpected variableIndex" << variableIndex;
      return NULL;
   }

   pv_name = getSubstitutedVariableName (0);
   pv_name = pv_name.trimmed ();
   this->recordBaseName = record_name (pv_name);

   // Clear associated data fields.
   //
   this->ownWidgets->label2->setText (field_name (pv_name));
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
   return new QEString (rtype_pv_name (pv_name), this, &stringFormatting, 0);
}

//------------------------------------------------------------------------------
//
void QEPvProperties::establishConnection (unsigned int variableIndex)
{
   qcaobject::QCaObject* qca;

   if (variableIndex != 0) {
      qDebug () << __FUNCTION__ << __LINE__ << "unexpected variableIndex" << variableIndex;
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
   RecordSpec *pRecordSpec;
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
      item = table->verticalHeaderItem (j);
      if (!item) {
         // We need to allocate iteem and inset into the table.
         item = new QTableWidgetItem ();
         table->setVerticalHeaderItem (j, item);
      }
      item->setText  (" " + fieldName + " ");

      // Ensure table entry item exists.
      //
      item = table->item (j, 0);
      if (!item) {
         // We need to allocate item and inset into the table.
         item = new QTableWidgetItem ();
         table->setItem (j, 0, item);
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
      item = table->item (variableIndex, 0);

      if (connectionInfo.isChannelConnected ()) {
         // connected
         item->setForeground (QColor (0, 0, 0));
         item->setText ("");
      } else {
         // disconnected - leave old text - readable but grayed out.
         item->setForeground (QColor (160, 160, 160));
      }
   } else {
      qDebug ()  << __FUNCTION__ << __LINE__ << "variableIndex" << variableIndex
                 << "out of range - must be <" << numberOfRows;
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
      item = table->item (variableIndex, 0);

      item->setText  (" " + value);

   } else {
      qDebug () << __FUNCTION__ << __LINE__ << "variableIndex" << variableIndex
                << "out of range - must be <" << numberOfRows;
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
   for( int i = 0; i < table->rowCount(); i++ )
    {
        QTableWidgetItem *item = table->item( i, 0 );
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
    if( getAllowDrop() )
    {
        setDrop( v );
    }
}

// end
