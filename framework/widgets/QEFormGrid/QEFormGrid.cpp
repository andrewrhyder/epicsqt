/*  QEFormGrid.cpp
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
#include <QECommon.h>

#include "QEFormGrid.h"

#define DEBUG qDebug () << "QEFormGrid" << __FUNCTION__ << __LINE__

const int QEFormGrid::MaximumForms = 210;    // 2*3*5*7
const int QEFormGrid::MaximumColumns = 42;   // 2*3*7


//=============================================================================
// MacroData functions
//=============================================================================
//
QEFormGrid::MacroData::MacroData (const QString& prefixIn, QEFormGrid* formGridIn)
{
   this->prefix = prefixIn;
   this->formGrid = formGridIn;
   this->offset = 1;
   this->numberWidth = 2;
}

//---------------------------------------------------------------------------------
//
void QEFormGrid::MacroData::setOffset (const int offsetIn)
{
   this->offset = offsetIn;
   this->formGrid->reCreateAllForms ();
}

//---------------------------------------------------------------------------------
//
int QEFormGrid::MacroData::getOffset ()
{
   return this->offset;
}

//---------------------------------------------------------------------------------
//
void QEFormGrid::MacroData::setNumberWidth (const int numberWidthIn)
{
   this->numberWidth = LIMIT (numberWidthIn, 1, 6);
   this->formGrid->reCreateAllForms ();
}

//---------------------------------------------------------------------------------
//
int QEFormGrid::MacroData::getNumberWidth ()
{
   return this->numberWidth;
}

//---------------------------------------------------------------------------------
//
void QEFormGrid::MacroData::setStrings (const QStringList& stringsIn)
{
   this->strings = stringsIn;
   this->formGrid->reCreateAllForms ();
}

//---------------------------------------------------------------------------------
//
QStringList QEFormGrid::MacroData::getStrings ()
{
   return this->strings;
}

//---------------------------------------------------------------------------------
//
QString QEFormGrid::MacroData::genSubsitutions (const int n)
{
   QString subs;

   subs = "";

   // E.g.  ROWNAME=Fred where prefix provides ROW or COL
   //
   subs.append (this->prefix).append ("NAME=").append (this->strings.value (n, ""));

   subs.append (", ");

   // E.g.  ROW=09
   //
   subs.append (this->prefix).append ("=");

   // Pad number with '0' to required width.
   //
   subs.append (QString ("%1").arg (n + this->offset, this->numberWidth, 10, QChar ('0')));
   return subs;
}


//=============================================================================
// QEFormGrid functions
//=============================================================================
// Constructor with no initialisation
//
QEFormGrid::QEFormGrid (QWidget* parent) : QEFrame (parent)
{
   this->commonSetup ("", 4, 1);
}

//---------------------------------------------------------------------------------
//
QEFormGrid::QEFormGrid (const QString& uiFileIn, const int numberIn,
                        const int colsIn, QWidget* parent) : QEFrame (parent)
{
   this->commonSetup (uiFileIn, numberIn, colsIn);
}

//---------------------------------------------------------------------------------
//
void QEFormGrid::commonSetup (const QString& uiFileIn, const int numberIn, const int colsIn)
{
   // Save input parameters.
   //
   this->uiFile = uiFileIn;
   this->number = LIMIT (numberIn, 1, QEFormGrid::MaximumForms);
   this->columns = LIMIT (colsIn, 1, QEFormGrid::MaximumColumns);

   // Set up the number of variables managed by the variable name manager.
   // NOTE: there is no data associated with this widget, but it uses the same mechanism
   // as other data widgets to manage the UI filename and macro substitutions. The standard
   // variable name and macros mechanism is used by QEFormGrid for UI file name and marcos
   //
   this->setNumVariables (1);

   // Configure the panel.
   //
   this->setFrameStyle (QFrame::Sunken);
   this->setFrameShape (QFrame::Box);

   this->gridOrder = RowMajor;

   // Set up macro formal name prefixes.
   // Maybe theses could be defined by properties.
   //
   this->rowMacroData = new MacroData ("ROW", this);
   this->colMacroData = new MacroData ("COL", this);
   this->slotMacroData = new MacroData ("SLOT", this);

   this->layout = new QGridLayout (this);
   this->layout->setMargin (2);
   this->layout->setSpacing (2);

   // Create initial number of sub-forms. .
   //
   for (int j = 0; j < this->number; j++) {
      this->addSubForm ();
   }

   // Note: unlike QEFile, the grid does not need to recieve notification that
   // the ui file being displayed has changed. The QEForm does that anyways.
   //
   // Set up a connection to recieve variable name property changes (Actually
   // only interested in substitution changes.
   //
   this->variableNamePropertyManager.setVariableIndex (0);

   QObject::connect (&this->variableNamePropertyManager,
                     SIGNAL (newVariableNameProperty (QString, QString, unsigned int) ),
                     this, SLOT        (setNewUiFile (QString, QString, unsigned int) ) );
}

//---------------------------------------------------------------------------------
//
QEFormGrid::~QEFormGrid ()
{
   delete this->rowMacroData;
   delete this->colMacroData;
   delete this->slotMacroData;
}

//---------------------------------------------------------------------------------
//
void QEFormGrid::setNewUiFile (QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex)
{
    this->setVariableNameAndSubstitutions (variableNameIn, variableNameSubstitutionsIn, variableIndex);
}

//---------------------------------------------------------------------------------
//
void QEFormGrid::establishConnection (unsigned int variableIndex)
{
   if (variableIndex != 0) {
      DEBUG << "Unexpected variableIndex: " << variableIndex;
      return;
   }

   // Get the fully substituted variable name.
   //
   this->uiFile = this->getSubstitutedVariableName (variableIndex);

   for (int j = 0; j < this->formsList.count (); j++) {
      QEForm* form = this->formsList.value (j, NULL);
      if (form) {
         form->setUiFileNameProperty (this->uiFile);
      }
   }
}


//---------------------------------------------------------------------------------
//
QString QEFormGrid::getPrioritySubstitutions (const int slot)
{
   QString result = "";
   int row;
   int col;

   // Get row and col number from the slot number.
   //
   this->splitSlot (slot, row, col);

   result.append (this->slotMacroData->genSubsitutions (slot));
   result.append (", ");
   result.append (this->rowMacroData->genSubsitutions (row));
   result.append (", ");
   result.append (this->colMacroData->genSubsitutions (col));

   return result;
}

//---------------------------------------------------------------------------------
//
QEForm* QEFormGrid::createQEForm (const int slot)
{
   QString psubs;
   QEForm* form = NULL;

   // Publish the profile this button recieved.
   //
   this->publishOwnProfile ();

   // Extend any variable name substitutions with this form grid's substitutions
   // Like most other macro substitutions, the substitutions already present take precedence.
   //
   this->addMacroSubstitutions (this->getGridVariableSubstitutions ());

   // Extend any variable name substitutions with this grid's priority substitutions
   // Unlike most other macro substitutions, these macro substitutions take precedence over
   // substitutions already present.
   //
   psubs = this->getPrioritySubstitutions (slot);
   this->addPriorityMacroSubstitutions (psubs);

   form = new QEForm (this);
   form->setUiFileNameProperty (this->uiFile);

   // Remove this grid's priority macro substitutions now all its children are created
   //
   this->removePriorityMacroSubstitutions ();

   // Remove this grid's normal macro substitutions now all its children are created
   //
   this->removeMacroSubstitutions ();

   // Release the profile now all QE widgets have been created.
   //
   this->releaseProfile ();

   return form;
}

//---------------------------------------------------------------------------------
//
void QEFormGrid::addSubForm ()
{
   QEForm* form;
   int slot;
   int row, col;

   slot = this->formsList.count ();
   if (slot < QEFormGrid::MaximumForms) {

      form = this->createQEForm (slot);
      this->formsList.append (form);

      // Get row and col number.
      //
      this->splitSlot (slot, row, col);
      this->layout->addWidget (form, row, col);
   }
}

//---------------------------------------------------------------------------------
//
void QEFormGrid::reCreateAllForms ()
{
   while (this->formsList.count () > 0) {
      QEForm* form = this->formsList.value (0);
      this->formsList.removeFirst ();
      this->layout->removeWidget (form);
      delete form;
   }

   for (int slot = 0; slot < this->number; slot++) {
      this->addSubForm ();
   }
}

//---------------------------------------------------------------------------------
//
int QEFormGrid::slotOf (const int row, const int col)
{
   int slot = 0;

   switch (this->gridOrder) {
      case RowMajor:
         slot = (row * this->getColumns ()) + col;
         break;

      case ColMajor:
         slot = (col * this->getRows ()) + row;
         break;

      default:
         DEBUG << "Unexpected gridOrder: " << this->gridOrder;
         slot = 0;
         break;
   }
   return slot;
}

//---------------------------------------------------------------------------------
//
void QEFormGrid::splitSlot (const int slot, int& row, int& col)
{
   switch (this->gridOrder) {
      case RowMajor:
         row = slot / this->getColumns ();
         col = slot % this->getColumns ();
         break;
      case ColMajor:
         row = slot % this->getRows ();
         col = slot / this->getRows ();
         break;
      default:
         DEBUG << "Unexpected gridOrder: " << this->gridOrder;
         break;
   }
}

//---------------------------------------------------------------------------------
//
QSize QEFormGrid::sizeHint () const
{
   return QSize (160, 200);
}


//=================================================================================
// Property access.
//=================================================================================
//
// We use the variableNamePropertyManager to manage the uiFile and local subsitutions.
//
void QEFormGrid::setUiFile (QString uiFileIn)
{
   this->variableNamePropertyManager.setVariableNameProperty (uiFileIn);
}

//---------------------------------------------------------------------------------
//
QString QEFormGrid::getUiFile ()
{
   return this->variableNamePropertyManager.getVariableNameProperty ();
}

//---------------------------------------------------------------------------------
//
void  QEFormGrid::setGridVariableSubstitutions (QString variableSubstitutionsIn)
{
   this->variableNamePropertyManager.setSubstitutionsProperty (variableSubstitutionsIn);
}

//---------------------------------------------------------------------------------
//
QString QEFormGrid::getGridVariableSubstitutions ()
{
   return  this->variableNamePropertyManager.getSubstitutionsProperty ();
}

//---------------------------------------------------------------------------------
//
void QEFormGrid::setNumber (int number)
{
   const int currentNumber = this->getNumber ();

   this->number = LIMIT (number, 1, QEFormGrid::MaximumForms);

   if (this->gridOrder == RowMajor) {
      // Existing row and col numbers remain unchanged.
      //
      if (this->number > currentNumber) {
         for (int j = currentNumber; j < this->number; j++) {
            this->addSubForm ();
         }
      } else if (this->number < currentNumber) {
         for (int j = currentNumber - 1; j >= this->number; j--) {
            QEForm* form =  this->formsList.value (j, NULL);
            this->formsList.removeLast ();
            delete form;
         }
      }
   } else {
      // Existing row and col numbers may change.
      //
      while (this->formsList.count () > 0) {
         QEForm* form =  this->formsList.value (0);
         this->formsList.removeFirst ();
         this->layout->removeWidget (form);
         delete form;
      }

      for (int slot = 0; slot < this->number; slot++) {
         this->addSubForm ();
      }
   }
}

//---------------------------------------------------------------------------------
//
int QEFormGrid::getNumber ()
{
   return this->number;
}

//---------------------------------------------------------------------------------
//
void QEFormGrid::setColumns (int number)
{
   this->columns = LIMIT (number, 1, QEFormGrid::MaximumColumns);
   // May get smarter and only re-create those we really need to do.
   // E.g. slot 0 is _always_ row 0, col 0.
   //
   this->reCreateAllForms ();
}

//---------------------------------------------------------------------------------
//
int QEFormGrid::getColumns ()
{
   return this->columns;
}

//---------------------------------------------------------------------------------
//
int QEFormGrid::getRows ()
{
   int r = (this->getNumber () + this->columns - 1) / this->columns;
   return MAX (1, r);  // always at least 1 row.
}

//---------------------------------------------------------------------------------
//
void QEFormGrid::setGridOrder (GridOrders gridOrderIn)
{
   this->gridOrder = gridOrderIn;
   this->reCreateAllForms ();
}

//---------------------------------------------------------------------------------
//
QEFormGrid::GridOrders QEFormGrid::getGridOrder ()
{
   return this->gridOrder;
}

//---------------------------------------------------------------------------------
//
void  QEFormGrid::setMargin (int margin) {
   this->layout->setMargin (margin);
}

//---------------------------------------------------------------------------------
//
int  QEFormGrid::getMargin () {
   return this->layout->margin ();
}

//---------------------------------------------------------------------------------
//
void  QEFormGrid::setSpacing (int spacing)
{
   this->layout->setSpacing (spacing);
}

//---------------------------------------------------------------------------------
//
int  QEFormGrid::getSpacing () {
   return this->layout->spacing ();
}

// end