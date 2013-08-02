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

#include <stdio.h>

#include <QDebug>
#include <QECommon.h>

#include "QEFormGrid.h"

#define DEBUG qDebug () << "QEFormGrid" << __FUNCTION__ << __LINE__


//=============================================================================
//
//=============================================================================
//
QEFormGrid::MacroData::MacroData ()
{
   this->offset = 1;
   this->numberWidth = 2;
}

//---------------------------------------------------------------------------------
//
QString QEFormGrid::MacroData::genSubsitutions (const int n)
{
   QString subs;
   char format [20];
   QString t;

   subs = "";

   // E.g.  ROWNAME=Fred
   //
   subs.append (this->prefix).append("NAME=").append (this->strings.value (n, ""));

   subs.append (", ");

   // E.g.  ROW=09
   //
   subs.append (this->prefix).append("=");
   snprintf (format, sizeof (format), "%%0%dd", this->numberWidth);
   subs.append (t.sprintf (format, n + this->offset));

   return subs;
}

//=============================================================================
//
//=============================================================================
// Constructor with no initialisation
//
QEFormGrid::QEFormGrid (QWidget* parent) : QEFrame (parent)
{
   this->layout = new QGridLayout (this);
   this->layout->setMargin (2);
   this->layout->setSpacing (2);

   // Configure the panel.
   //
   this->setFrameStyle (QFrame::Sunken);
   this->setFrameShape (QFrame::Box);

   this->setNumVariables (0);

   this->rowMacroData.prefix = "ROW";
   this->colMacroData.prefix = "COL";
   this->slotMacroData.prefix = "SLOT";

   this->uiFileName = "";
   this->columns = 1;
   this->gridOrder = RowMajor;

   // Create defult number of sub-forms. This "sets" the number attribute.
   //
   for (int j = 0; j < 4; j++) {
      this->createSubForm ();
   }
}

//---------------------------------------------------------------------------------
//
QSize QEFormGrid::sizeHint () const
{
   return QSize (40, 80);
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
   }
   return slot;
}

//---------------------------------------------------------------------------------
//
void QEFormGrid::splitSlot (const int slot, int& row, int& col)
{
   switch (this->gridOrder) {
      case RowMajor:
         row = slot /  this->getColumns ();
         col = slot %  this->getColumns ();
         break;
      case ColMajor:
         row = slot %  this->getRows ();
         col = slot /  this->getRows ();
         break;
   }
}

//---------------------------------------------------------------------------------
//
void QEFormGrid::createSubForm ()
{
   QEForm* form;
   int slot;
   int row, col;

   slot = this->formsList.count ();
   if (slot < QEFormGrid::MaximumForms) {
      form = new QEForm (this);
      this->formsList.append (form);

      this->setFormSubstitutions (slot);
      form->setUiFileNameProperty (uiFileName);

      // Get row and col number.
      //
      this->splitSlot (slot, row, col);
      this->layout->addWidget (form, row, col);
   }
}

//---------------------------------------------------------------------------------
//
void QEFormGrid::reLayoutForms ()
{
   int row, col;

   // Re jig layouts
   // First remove all items from the layout.
   //
   for (int slot = 0 ; slot < this->formsList.count(); slot++) {
      QEForm* form = this->formsList.value (slot, NULL);
      this->layout->removeWidget (form);
   }

   // Now re-add to the layout.
   //
   for (int slot = 0; slot < this->formsList.count(); slot++) {
      QEForm* form = this->formsList.value (slot, NULL);

      // Get row and col number.
      //
      this->splitSlot (slot, row, col);
      this->layout->addWidget (form, row, col);
   }

   // Lastly reset the substitutions.
   //
   this->setSubstitutions ();
}

//---------------------------------------------------------------------------------
//
void QEFormGrid::setFormSubstitutions (const int slot)
{
   QEForm* form;
   int row;
   int col;
   QString subs;

   form = this->formsList.value (slot, NULL);
   if (!form) return;

   // Get row and col number.
   //
   this->splitSlot (slot, row, col);

   subs= "";
   subs.append (this->slotMacroData.genSubsitutions (slot));
   subs.append (", ");
   subs.append (this->rowMacroData.genSubsitutions (row));
   subs.append (", ");
   subs.append (this->colMacroData.genSubsitutions (col));

   form->setVariableNameSubstitutionsProperty (subs);

   // Force reload using new macro definitions.
   // QUERY: is this the best/only way? Talk to AR.
   //
   form->setUiFileNameProperty (":/no/file");
   form->setUiFileNameProperty (this->uiFileName);
}

//---------------------------------------------------------------------------------
//
void QEFormGrid::setSubstitutions ()
{
   for (int slot = 0; slot < this->formsList.count(); slot++) {
      this->setFormSubstitutions (slot);
   }
}

//=================================================================================
// Property access.
//=================================================================================
//
void QEFormGrid::setUiFile (QString uiFileNameIn)
{
   this->uiFileName = uiFileNameIn;

   for (int j = 0; j < this->formsList.count (); j++) {
      QEForm* form = this->formsList.value (j, NULL);
      if (form) {
         form->setUiFileNameProperty (uiFileName);
      }
   }
}

//---------------------------------------------------------------------------------
//
QString QEFormGrid::getUiFile ()
{
   return this->uiFileName;
}

//---------------------------------------------------------------------------------
//
void QEFormGrid::setNumber (int number)
{
   const int newNumber = LIMIT (number, 1, QEFormGrid::MaximumForms);
   const int currentNumber = this->getNumber ();

   if (newNumber > currentNumber) {
      for (int j = currentNumber; j < newNumber; j++) {
         this->createSubForm ();
      }
   } else
   if (newNumber < currentNumber) {
      for (int j = currentNumber - 1; j >= newNumber; j--) {
         QEForm* form =  this->formsList.value (j, NULL);
         this->formsList.removeLast ();
         delete form;
      }
   }
   this->reLayoutForms ();
}

//---------------------------------------------------------------------------------
//
int QEFormGrid::getNumber ()
{
   return this->formsList.count ();
}

//---------------------------------------------------------------------------------
//
void QEFormGrid::setColumns (int number)
{
   this->columns = LIMIT (number, 1, QEFormGrid::MaximumColumns);
   this->reLayoutForms ();
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
   return (this->getNumber () + this->columns - 1) / this->columns;
}

//---------------------------------------------------------------------------------
//
void QEFormGrid::setGridOrder (GridOrders gridOrderIn)
{
   this->gridOrder = gridOrderIn;
   this->reLayoutForms ();
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
void  QEFormGrid::setSpacing (int spacing) {
   DEBUG;
   this->layout->setSpacing (spacing);
}

//---------------------------------------------------------------------------------
//
int  QEFormGrid::getSpacing () {
   return this->layout->spacing ();
}

//---------------------------------------------------------------------------------
//
#define SET_GET_ATTRIBUTES_FUNCTIONS(Attr, attr)             \
void QEFormGrid::set##Attr##Offset (int n)                   \
{                                                            \
   this->attr##MacroData.offset = n;                         \
   this->setSubstitutions ();                                \
}                                                            \
                                                             \
int QEFormGrid::get##Attr##Offset ()                         \
{                                                            \
   return this->attr##MacroData.offset;                      \
}                                                            \
                                                             \
void QEFormGrid::set##Attr##NumberWidth (int n)              \
{                                                            \
   this->attr##MacroData.numberWidth = LIMIT (n, 1, 6);      \
   this->setSubstitutions ();                                \
}                                                            \
                                                             \
int QEFormGrid::get##Attr##NumberWidth ()                    \
{                                                            \
   return this->attr##MacroData.numberWidth;                 \
}                                                            \
                                                             \
void QEFormGrid::set##Attr##Strings (QStringList& strings)   \
{                                                            \
   this->attr##MacroData.strings = strings;                  \
   this->setSubstitutions ();                                \
}                                                            \
                                                             \
QStringList QEFormGrid::get##Attr##Strings ()                \
{                                                            \
   return this->attr##MacroData.strings;                     \
}

SET_GET_ATTRIBUTES_FUNCTIONS (Row, row)
SET_GET_ATTRIBUTES_FUNCTIONS (Col, col)
SET_GET_ATTRIBUTES_FUNCTIONS (Slot, slot)

#undef SET_GET_ATTRIBUTES

// end
