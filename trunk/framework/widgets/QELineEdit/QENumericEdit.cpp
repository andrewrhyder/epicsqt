/*  QENumericEdit.cpp
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

#include <math.h>
#include <QDebug>
#include <QVariant>
#include <QEFloating.h>

#include <QECommon.h>
#include "QENumericEdit.h"

#define DEBUG  qDebug () << "QENumericEdit::" << __FUNCTION__ << __LINE__

#define MAX_SIGNIFICANCE   15
#define NUMBER_OF_RADICES  4

//==============================================================================
// Local function.
// Maybe they should be static functions of the QENumericEdit class
//==============================================================================
//
// Decimal = 0, Hexadecimal, Octal, Binary
//
const static int radix_value_list [NUMBER_OF_RADICES] = { 10, 16, 8, 2 };

// For decimal, this is about 48.9 bits, for the others 48 bits exactly.
const static int maximum_number_digits_list [NUMBER_OF_RADICES] = { 15, 12, 16, 48 };


//------------------------------------------------------------------------------
// Provides double to QString
//
static QString fixedRadixImage (const double value,
                                const QENumericEdit::Radicies radix,
                                const bool sign,
                                const int zeros,
                                const int precision,
                                const QENumericEdit::Separators separator)
{
   const char radixChars [] = "0123456789ABCDEF";
   const char separatorChars [] = " ,_ ";
   const int  separatorSizes [NUMBER_OF_RADICES] = { 3, 4, 3, 4 };
   const int r = radix_value_list [radix];

   QString result;
   double work;
   int mostSig;
   int s;
   double prs;
   int t;

   result = " ";

   // Do leading sign if needeed or requested.
   //
   if (value >= 0.0) {
      if (sign) {
          result.append ('+');
      }
   } else {
       result.append ('-');
   }

   work = ABS (value);
   mostSig = 0;
   while (pow (r, mostSig + 1) < work) {
      mostSig++;
   }

   /*
    for (t = r; t < work; t = t*r)
       mostSig++;
    */

   mostSig = MAX (mostSig, zeros - 1);

   // Round up by half the value of the least significant digit.
   //
   work = work + (pow ((1.0/r), precision) * 0.499999999);

   for (s = mostSig; s >= -precision; s--) {

      prs = pow (r, s);
      t = int (floor (work / prs));
      work = work - t*prs;

      result.append (radixChars [t]);

      // All done?
      //
      if (s <= -precision) break;

      if (s == 0) {
         result.append ('.');
      } else if (ABS (s) % separatorSizes [radix] == 0) {
         if (separator > QENumericEdit::None) {
            result.append (separatorChars [separator]);
         }
      }
   }

   return result;
}

//------------------------------------------------------------------------------
// Provides QString to double.
//
static bool fixedRadixValue (const QString & image,
                             const QENumericEdit::Radicies radix,
                             double & result)
{
   const int r = radix_value_list [radix];
   bool isNegative;
   bool isPoint;
   bool signIsAllowed;
   int scale;
   int j;
   char c;
   int d;

   result = 0.0;     // ensure not erroneous
   isNegative = false;
   isPoint = false;
   scale = 0;

   // We could, and prob should, be more strict with
   // the syntax checking.
   //
   signIsAllowed = true;

   for (j = 0; j < image.length(); j++) {
      c = image [j].toAscii ();
      d = 0;

      switch (c) {
         case ' ':
         case ',':
         case '_':
             // null
             break;

         case '+':
            if (!signIsAllowed) {
               return false;
            }
            signIsAllowed = false;
            break;

         case '-':
            if (!signIsAllowed) {
               return false;
            }
            isNegative = true;
            signIsAllowed = false;
            break;

         case '.':
          if (isPoint) {
             return false;
          }
          isPoint = true;
          break;

         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
            d = int (c) - int ('0');
            result = (result*r) + d;
            if (isPoint) scale--;
            break;

         case 'A':
         case 'B':
         case 'C':
         case 'D':
         case 'E':
         case 'F':
            d = int (c) - int ('A') + 10;
            result = (result*r) + d;
            if (isPoint) scale--;
            break;

         case 'a':
         case 'b':
         case 'c':
         case 'd':
         case 'e':
         case 'f':
            d = int (c) - int ('a') + 10;
            result = (result*r) + d;
            if (isPoint) scale--;
            break;

         default:
            return false;
            break;
      }
      if (d >= r) {
         return false;
      }
   }

   // Scale result.
   //
   result = result * pow (r, scale);

   // Apply sign
   //
   if (isNegative) result = -result;

   return true;
}

//------------------------------------------------------------------------------
// Example: leading zeros = 2, precision = 1, radix = 10, then max
// value is 99.9 =  10**2 - 10**(-1)
//
static double calcUpper (const QENumericEdit::Radicies radix,
                         const int leadingZeros,
                         const int precison)
{
   const double r = (double) radix_value_list [radix];

   double a, b;

   a = pow (r, leadingZeros);
   b = pow (r, -precison);

   return a - b;
}

//------------------------------------------------------------------------------
//
static double calcLower (const QENumericEdit::Radicies radix,
                         const int leadingZeros,
                         const int precison)
{
   return -calcUpper (radix, leadingZeros, precison);
}


//==============================================================================
// QENumericEdit
//==============================================================================
//
QENumericEdit::QENumericEdit (QWidget * parent) : QEGenericEdit (parent)
{
   this->commonConstructor ();
}

//------------------------------------------------------------------------------
//
QENumericEdit::QENumericEdit (const QString & variableNameIn, QWidget * parent): QEGenericEdit (variableNameIn, parent)
{
   this->commonConstructor ();
}

//------------------------------------------------------------------------------
//
void QENumericEdit::commonConstructor ()
{
   this->mAutoScale = true;
   this->mCursor = 1;

   this->addUnits = true;
   this->mRadix = Decimal;
   this->mSeparator = None;

   this->propertyPrecision = 4;
   this->propertyLeadingZeros = 3;
   this->propertyMinimum = calcLower (this->mRadix, 3, 4);
   this->propertyMaximum = calcUpper (this->mRadix, 3, 4);

   // Ensure sensible auto values
   //
   this->autoPrecision = this->propertyPrecision;
   this->autoLeadingZeros = this->propertyLeadingZeros;
   this->autoMinimum = this->propertyMinimum;
   this->autoMaximum = this->propertyMaximum;

   // force setNumericValue to process.
   //
   this->mValue = 1.0;
   this->setNumericValue (0.0);
}

//------------------------------------------------------------------------------
//
QENumericEdit::~QENumericEdit ()
{
   // place holder
}

//------------------------------------------------------------------------------
// Implementation of QEWidget's virtual funtion to create the specific type of
// QCaObject required. For a numeric edit, a QCaObject that streams floating
// point (double) numbers is required.
//
qcaobject::QCaObject * QENumericEdit::createQcaItem (unsigned int variableIndex)
{
   // Sanity check ...
   //
   if (variableIndex != 0) {
      QString message;

      message.sprintf ("QENumericEdit::createQcaItem - invalid variable index %d", variableIndex);
      this->sendMessage (message, message_types (MESSAGE_TYPE_ERROR));
      return NULL;
   }

   return new QEFloating (this->getSubstitutedVariableName (variableIndex),
                          this,    // eventObject
                          &this->floatingFormatting,
                          variableIndex);
}


//------------------------------------------------------------------------------
// Start updating.
// Implementation of VariableNameManager's virtual funtion to establish a
// connection to a PV as the variable name has changed.
// This function may also be used to initiate updates when loaded as a plugin.
//
void QENumericEdit::establishConnection (unsigned int variableIndex)
{
   // Sanity check ...
   //
   if (variableIndex != 0) {
      QString message;

      message.sprintf ("QENumericEdit::establishConnection - invalid variable index %d", variableIndex);
      this->sendMessage (message, message_types (MESSAGE_TYPE_ERROR));
      return;
   }

   // Create a connection.
   // If successfull, the QCaObject object that will supply data update signals will be returned
   // Note createConnection creates the connection and returns reference to existing QCaObject.
   //
   qcaobject::QCaObject * qca = createConnection (variableIndex);

   // If a QCaObject object is now available to supply data update signals, connect it to the appropriate slots.
   //
   if ((qca) && (variableIndex == 0)) {
      QObject::connect (qca, SIGNAL (floatingChanged (const double &, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)),
                        this, SLOT  (setDoubleValue  (const double &, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)));

      QObject::connect (qca, SIGNAL (connectionChanged (QCaConnectionInfo &)),
                        this, SLOT  (connectionChanged (QCaConnectionInfo &)));
   }
}

//------------------------------------------------------------------------------
//
void QENumericEdit::setDoubleValue (const double& valueIn, QCaAlarmInfo& alarmInfo,
                                    QCaDateTime& dateTime, const unsigned int& variableIndex)
{
   qcaobject::QCaObject * qca = this->getQcaItem (0);

   // Sanity check ...
   //
   if (variableIndex != 0) {
      QString message;

      message.sprintf ("QENumericEdit::setDoubleValue - invalid variable index %d", variableIndex);
      this->sendMessage (message, message_types (MESSAGE_TYPE_ERROR));
      return;
   }

   // Check first update.
   //
   if (this->testAndClearIsFirstUpdate ()) {

      // Check for auto scale and avoid the segment fault.
      //
      if (this->getAutoScale () && (qca)) {
         // Do the auto scale calculations.
         //
         double ctrlLow;
         double ctrlUpp;
         enum Priority priority;

         // Check that sensible limits have been defined and not just left
         // at the default (i.e. zero) values by a lazy database creator.
         // Otherwise, leave as design time limits.
         //
         ctrlLow = qca->getControlLimitLower ();
         ctrlUpp = qca->getControlLimitUpper ();
         if ((ctrlLow != 0.0) || (ctrlUpp != 0.0)) {
            // At least one of these limits is non-zero - assume database creator
            // has thought about this.
            //
            this->autoMinimum = ctrlLow;
            this->autoMaximum = ctrlUpp;
            priority = ABS (ctrlLow) > ABS (ctrlUpp) ? min : max;
         } else {
            // Just go with design property values.
            this->autoMinimum = this->getPropertyMinimum ();
            this->autoMaximum = this->getPropertyMaximum ();
            this->autoLeadingZeros = this->getPropertyLeadingZeros ();
            priority = prec;
         }
         this->autoPrecision = qca->getPrecision ();

         this->rationalise (priority, this->autoLeadingZeros, this->autoPrecision,
                            this->autoMinimum, this->autoMaximum);

      }  // else no auto scaling, just go with design property values.
         // Note: this have already been pre rationalised
   }

   // Get on with update proper.  Do generic update processing.
   //
   this->setDataIfNoFocus (QVariant (valueIn), alarmInfo, dateTime);

   // Signal a database value change to any Link widgets
   //
   emit dbValueChanged (valueIn);
}

//------------------------------------------------------------------------------
//
int QENumericEdit::getRadixValue ()
{
   return radix_value_list [this->mRadix];
}

//------------------------------------------------------------------------------
//
int QENumericEdit::maximumSignificance ()
{
   return maximum_number_digits_list [this->mRadix];
}

//------------------------------------------------------------------------------
//
void QENumericEdit::keyPressEvent (QKeyEvent * event)
{
   const int key = event->key ();

   int index;
   int significance;
   QChar qc;
   QChar qk;
   int j;
   double delta;
   double newval;
   bool invokeInherited;

   invokeInherited = false;
   switch (key) {

      case Qt::Key_Up:
         index = this->getCursor ();
         qc = this->text () [index];

         // Is this a digit charcter?
         //
         if (this->isRadixDigit(qc)) {
            significance = -this->getPrecision ();
            for (j = index + 1; j <= this->lengthOfImageValue(); j++) {
               qc = this->text () [j];
               if (this->isRadixDigit(qc)) {
                  significance++;
               }
            }
            delta = pow (this->getRadixValue (), significance);
            this->setNumericValue (this->getNumericValue () + delta);
         } else if (this->cursorOverSign ()) {
            this->setNumericValue (+fabs (this->getNumericValue ()));
         }
         break;


      case Qt::Key_Down:
         index = this->getCursor ();
         qc = this->text () [index];

         // Is this a digit charcter?
         //
         if (this->isRadixDigit(qc)) {
            significance = -this->getPrecision ();
            for (j = index + 1; j <= this->lengthOfImageValue(); j++) {
               qc = this->text () [j];
               if (this->isRadixDigit(qc)) {
                  significance++;
               }
            }
            delta = pow (this->getRadixValue (), significance);
            this->setNumericValue (this->getNumericValue () - delta);
         } else if (this->cursorOverSign ()) {
            this->setNumericValue (-fabs (this->getNumericValue ()));
         }
         break;


      case Qt::Key_Left:
         this->setCursor (this->getCursor () - 1);

         // If we have moved onto a filler character, then move again.
         //
         qc = this->text () [this->getCursor ()];
         if (!this->isSignOrDigit (qc)) {
            this->setCursor (this->getCursor () - 1);
         }
         break;


      case Qt::Key_Right:
         this->setCursor (this->getCursor () + 1);

         // If we have moved onto a filler character, then move again.
         //
         qc = this->text () [this->getCursor ()];
         if (!this->isSignOrDigit (qc)) {
            this->setCursor (this->getCursor () + 1);
         }
         break;

      case Qt::Key_Plus:
      case Qt::Key_Minus:
         if (this->cursorOverSign ()) {
            if (key == Qt::Key_Plus) {
               this->setNumericValue (+fabs (this->getNumericValue ()));
            } else {
               this->setNumericValue (-fabs (this->getNumericValue ()));
            }
            this->setCursor (this->getCursor () + 1);
         }
         break;

      case Qt::Key_0:
      case Qt::Key_1:
      case Qt::Key_2:
      case Qt::Key_3:
      case Qt::Key_4:
      case Qt::Key_5:
      case Qt::Key_6:
      case Qt::Key_7:
      case Qt::Key_8:
      case Qt::Key_9:
      // we do not care about shift here, allow 'A' or 'a' etc.
      case Qt::Key_A:
      case Qt::Key_B:
      case Qt::Key_C:
      case Qt::Key_D:
      case Qt::Key_E:
      case Qt::Key_F:
         qk = QChar (key);
         index = this->getCursor ();
         qc = this->text () [index];

         // Both the new char and the existing char must both be radix digits.
         //
         if (this->isRadixDigit (qk) && this->isRadixDigit (qc)) {
            QString tryThis = this->text ();
            tryThis [index] = QChar (key);

            newval = this->valueOfImage (tryThis);
            this->setNumericValue (newval);
            this->setCursor (this->getCursor () + 1);

            // If we have moved onto a filler character, then move again.
            //
            qc = this->text () [this->getCursor ()];
            if (!this->isSignOrDigit (qc)) {
               this->setCursor (this->getCursor () + 1);
            }
         }
         break;

      case Qt::Key_Space:
         break;

      default:
         // Only reprocess enter, return etc.
         invokeInherited = (key >= 256);
         break;
   }

   if (invokeInherited) {
      this->QEGenericEdit::keyPressEvent (event);
   }
}

//------------------------------------------------------------------------------
//
void QENumericEdit::focusInEvent (QFocusEvent * event)
{
   if (event->gotFocus () != true) {
      // This is kind of unexpected.
      //
      DEBUG << this->objectName () << "focusInEvent  got: "
            << event->gotFocus () << "   lost" << event->lostFocus ();
   }
   // Call parent function.
   //
   this->QEGenericEdit::focusInEvent (event);
   this->setDigitSelection ();
}

//------------------------------------------------------------------------------
//
void QENumericEdit::mouseReleaseEvent (QMouseEvent * event)
{
   int posn;

   // Call parent function.
   //
   this->QEGenericEdit::mouseReleaseEvent (event);

   posn = LIMIT (this->cursorPosition (), 1, this->lengthOfImageValue () - 1);

   this->setCursor (posn);
   this->setDigitSelection ();
}


//------------------------------------------------------------------------------
//
void QENumericEdit::setDigitSelection ()
{
   int posn;

   // Only set/update selection if/when the widget has focus.
   //
   if (this->hasFocus()) {
      posn = this->getCursor ();
      this->setSelection (posn, 1);
   }
}


//------------------------------------------------------------------------------
//
QString QENumericEdit::imageOfValue ()
{
   return fixedRadixImage (this->mValue, this->mRadix, this->showSign (),
                           this->getLeadingZeros(), this->getPrecision (),
                           this->mSeparator);
}

//------------------------------------------------------------------------------
//
int QENumericEdit::lengthOfImageValue ()
{
   // maybe calc once each time critical parameters are set?
   // or attempt to calculate.
   return this->imageOfValue ().length ();
}

//------------------------------------------------------------------------------
//
double QENumericEdit::valueOfImage (const QString & image)
{
   bool okay;
   double result;

   okay = fixedRadixValue (image.mid (0, this->lengthOfImageValue ()), this->mRadix, result);

   if (!okay) {
      result = this->mValue;
   }
   return result;
}

//------------------------------------------------------------------------------
//
void QENumericEdit::setNumericText ()
{
   QString image = this->imageOfValue ();
   qcaobject::QCaObject* qca;
   QString units;

   if (this->getAddUnits ()) {
      qca = this->getQcaItem (0);
      if (qca) {
         units = qca->getEgu();
         if (!units.isEmpty ()) {
               image.append (" ").append (units);
         }
      }
   }

   // Note: this is an intended side effect.
   // TODO: Explain this more !!!
   //
   this->setMaxLength (image.length ());

   this->setText (image);
   this->setDigitSelection ();
}

//------------------------------------------------------------------------------
//
void QENumericEdit::setCursor (const int value)
{
   // cursor postion is zero based - skip leading zero.
   //
   this->mCursor = LIMIT (value, 1, this->lengthOfImageValue () - 1);
   this->setDigitSelection ();
}

//------------------------------------------------------------------------------
//
int QENumericEdit::getCursor ()
{
   return this->mCursor;
}

//------------------------------------------------------------------------------
//
bool QENumericEdit::isRadixDigit (QChar qc)
{
   bool result;
   char c = qc.toAscii ();

   switch (this->getRadix()) {
      case Decimal:
         result = ((c >= '0') && (c <= '9'));
         break;

      case Hexadecimal:
         result = ((c >= '0') && (c <= '9')) ||
                  ((c >= 'A') && (c <= 'F')) ||
                  ((c >= 'a') && (c <= 'f'));
         break;

      case Octal:
         result = ((c >= '0') && (c <= '7'));
         break;

      case Binary:
         result = ((c >= '0') && (c <= '1'));
         break;

      default:
         result = false;
         break;
   }
   return result;
}

//------------------------------------------------------------------------------
//
bool QENumericEdit::isSign (QChar qc)
{
   char c = qc.toAscii ();

   return ((c == '+') || (c == '-'));
}

//------------------------------------------------------------------------------
//
bool QENumericEdit::isSignOrDigit (QChar qc)
{
   return (this->isSign (qc) || this->isRadixDigit (qc));
}

//------------------------------------------------------------------------------
//
void QENumericEdit::setNumericValue (const double value)
{
   double limited_value;

   limited_value = LIMIT (value, this->getMinimum (), this->getMaximum ());

   if (this->mValue != limited_value) {
      this->mValue = limited_value;
      this->setNumericText ();
   }
}

//------------------------------------------------------------------------------
//
double QENumericEdit::getNumericValue ()
{
   return this->mValue;
}

//------------------------------------------------------------------------------
//
int QENumericEdit::getPrecision ()
{
   return (this->getIsConnected () && this->getAutoScale ()) ? this->autoPrecision : this->propertyPrecision;
}

//------------------------------------------------------------------------------
//
int QENumericEdit::getLeadingZeros ()
{
   return (this->getIsConnected () && this->getAutoScale ()) ? this->autoLeadingZeros : this->propertyLeadingZeros;
}

//------------------------------------------------------------------------------
//
double QENumericEdit::getMinimum ()
{
    return (this->getIsConnected () && this->getAutoScale ()) ? this->autoMinimum : this->propertyMinimum;
}

//------------------------------------------------------------------------------
//
double QENumericEdit::getMaximum ()
{
    return (this->getIsConnected () && this->getAutoScale ()) ? this->autoMaximum : this->propertyMaximum;
}

//------------------------------------------------------------------------------
//
void QENumericEdit::setAutoScale (const bool value)
{
   if (this->mAutoScale != value) {
      this->mAutoScale = value;
      this->setNumericText ();
   }
}

//------------------------------------------------------------------------------
//
bool QENumericEdit::getAutoScale ()
{
   return this->mAutoScale;
}

//------------------------------------------------------------------------------
//
void QENumericEdit::setPropertyPrecision (const int value)
{
   this->propertyPrecision = value;
   this->rationalise (prec, this->propertyLeadingZeros, this->propertyPrecision, this->propertyMinimum, this->propertyMaximum);
   if (!this->getIsConnected ()) {

   }
   this->setNumericText ();
}

//------------------------------------------------------------------------------
//
int QENumericEdit::getPropertyPrecision ()
{
   return this->propertyPrecision;
}

//------------------------------------------------------------------------------
//
void QENumericEdit::setPropertyLeadingZeros (const int value)
{
   this->propertyLeadingZeros = value;
   this->rationalise (zeros, this->propertyLeadingZeros, this->propertyPrecision, this->propertyMinimum, this->propertyMaximum);
   this->setNumericText ();
}

//------------------------------------------------------------------------------
//
int QENumericEdit::getPropertyLeadingZeros ()
{
   return this->propertyLeadingZeros;
}

//------------------------------------------------------------------------------
//
void QENumericEdit::setPropertyMinimum (const double value)
{
   this->propertyMinimum = value;
   this->rationalise (min, this->propertyLeadingZeros, this->propertyPrecision, this->propertyMinimum, this->propertyMaximum);
   this->setNumericText ();
}

//------------------------------------------------------------------------------
//
double QENumericEdit::getPropertyMinimum ()
{
   return this->propertyMinimum;
}

//------------------------------------------------------------------------------
//
void QENumericEdit::setPropertyMaximum (const double value)
{
   this->propertyMaximum = value;
   this->rationalise (max, this->propertyLeadingZeros, this->propertyPrecision, this->propertyMinimum, this->propertyMaximum);
   this->setNumericText ();
}

//------------------------------------------------------------------------------
//
double QENumericEdit::getPropertyMaximum ()
{
   return this->propertyMaximum;
}

//------------------------------------------------------------------------------
//
void QENumericEdit::setAddUnits (bool addUnitsIn)
{
   if (this->addUnits != addUnitsIn) {
      this->addUnits = addUnitsIn;
      this->setNumericText ();      // Redisplay text.
   }
}

//------------------------------------------------------------------------------
//
bool QENumericEdit::getAddUnits ()
{
   return this->addUnits;
}

//------------------------------------------------------------------------------
//
void QENumericEdit::setRadix (const Radicies value)
{
   if (this->mRadix != value) {
      this->mRadix = value;
      this->setNumericText ();      // Redisplay text.
   }
}

//------------------------------------------------------------------------------
//
QENumericEdit::Radicies QENumericEdit::getRadix ()
{
   return this->mRadix;
}

//------------------------------------------------------------------------------
//
void QENumericEdit::setSeparator (const Separators value)
{
   if (this->mSeparator != value) {
      this->mSeparator = value;
      this->setNumericText ();      // Redisplay text.
   }
}

//------------------------------------------------------------------------------
//
QENumericEdit::Separators QENumericEdit::getSeparator ()
{
   return this->mSeparator;
}


//------------------------------------------------------------------------------
//
bool QENumericEdit::showSign ()
{
   // Only force '+' if sign can be -ve.
   //
   return (this->getMinimum () < 0.0);
}

//------------------------------------------------------------------------------
//
bool QENumericEdit::cursorOverSign ()
{
   return (this->showSign () && (this->getCursor () == 1));
}

//------------------------------------------------------------------------------
//
void QENumericEdit::rationalise (const enum Priority priority,
                                 int & leadingZerosInOut,
                                 int & precisionInOut,
                                 double & minimumInOut,
                                 double & maximumInOut)
{
   double temp;

   switch (priority) {

      case zeros:
         leadingZerosInOut = LIMIT (leadingZerosInOut, 1, MAX_SIGNIFICANCE);
         // Reduce precision so as not to exceed max significance if required.
         //
         precisionInOut = MIN (precisionInOut, MAX_SIGNIFICANCE - leadingZerosInOut);
         break;

      case prec:
         precisionInOut = LIMIT (precisionInOut, 0, MAX_SIGNIFICANCE);

         // Reduce (effective) number of leading zeros so as not to exceed max
         // significance if required.
         //
         leadingZerosInOut = MIN (leadingZerosInOut, MAX_SIGNIFICANCE - precisionInOut);
         break;

      case min:
         temp = MAX (1.0, ABS (minimumInOut));

         // the (int) cast truncates to 0
         leadingZerosInOut = 1 + (int) (log10 (temp));

         leadingZerosInOut = LIMIT (leadingZerosInOut, 1, MAX_SIGNIFICANCE);

         // Reduce precision so as not to exceed max significance if required.
         //
         precisionInOut = MIN (precisionInOut, MAX_SIGNIFICANCE - leadingZerosInOut);
         break;

      case max:
         temp = MAX (1.0, ABS (maximumInOut));

         // (int) cast truncates to 0
         leadingZerosInOut = 1 + (int) (log10 (temp));

         leadingZerosInOut = LIMIT (leadingZerosInOut, 1, MAX_SIGNIFICANCE);

         // Reduce precision so as not to exceed max significance if required.
         //
         precisionInOut = MIN (precisionInOut, MAX_SIGNIFICANCE - leadingZerosInOut);
         break;
   }

   // Re-caluclate the min/max values and constrain values if needs be.
   //
   minimumInOut = MAX (minimumInOut, calcLower (this->getRadix (), leadingZerosInOut, precisionInOut));
   maximumInOut = MIN (maximumInOut, calcUpper (this->getRadix (), leadingZerosInOut, precisionInOut));
}

\
//==============================================================================
// Set widget to the given value
//
void QENumericEdit::setValue (const QVariant & value)
{
   double d;
   bool ok;

   d = value.toDouble (&ok);
   if (ok) {
       this->setNumericValue (d);
   }
}

//------------------------------------------------------------------------------
//
QVariant QENumericEdit::getValue()
{
   return QVariant (this->getNumericValue ());
}

//------------------------------------------------------------------------------
// Write the given value to the associated channel.
//
bool QENumericEdit::writeData (const QVariant & value, QString& message)
{
   QEFloating *qca = dynamic_cast <QEFloating*> ( this->getQcaItem(0) );
   double d;
   bool ok;

   d = value.toDouble (&ok);
   if (qca && ok) {
      qca->writeFloating (d);
      return true;
   } else {
      message = "null qca object and/or value is not a double";
      return false;
   }
}

// end
