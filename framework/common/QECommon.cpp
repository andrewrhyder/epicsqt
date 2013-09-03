/*  QECommon.cpp
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
 *
 */

#include <QColor>
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QHeaderView>
#include <QMetaObject>
#include <QSize>
#include <QTableWidget>
#include <QWidget>

#include <QEResizeableFrame.h>
#include <QEWidget.h>

#include "QECommon.h"

//------------------------------------------------------------------------------
//
QColor QEUtilities::fontColour (const QColor & backgroundColour)
{
   QColor result;
   int r, g, b, a;
   int wc;
   bool isDark = false;

   // Split colour into components.
   //
   backgroundColour.getRgb (&r, &g, &b, &a);

   // Counting the perceptive luminance - human eye favors green color...
   // Form weighted component wc. Weights based on algorithm:
   // http://stackoverflow.com/questions/1855884/determine-font-color-based-on-background-color
   //
   wc = ((299 * r) + (587 * g) + (114 * b)) / 1000;   // 299 + 587 + 114 = 1000

   // Dark or bright background colour ?
   //
   isDark = (wc < 112);
   if (isDark) {
      result = QColor (255, 255, 255, 255);    // white font
   } else {
      result = QColor (0, 0, 0, 255);          // black font
   }

   return result;
}

//------------------------------------------------------------------------------
//
QString QEUtilities::colourToStyle (const QColor backgroundColour) {
   QString result;
   QColor foregroundColour;
   int br, bg, bb, ba;
   int fr, fg, fb, fa;

   // Choose high contrast font/foreground colour.
   //
   foregroundColour = QEUtilities::fontColour (backgroundColour);

   // Split colours into components and aggragate into a style sheet.
   //
   backgroundColour.getRgb (&br, &bg, &bb, &ba);
   foregroundColour.getRgb (&fr, &fg, &fb, &fa);

   result.sprintf ("QWidget { background-color: #%02x%02x%02x; color: #%02x%02x%02x; }",
                   br, bg, bb, fr, fg, fb );
   return result;
}

//------------------------------------------------------------------------------
//
int QEUtilities::getTimeZoneOffset (const QDateTime & atTime)
{
   QDateTime local = atTime.toLocalTime ();

   // This changes the time, e.g. from "2013-02-24 11:37:19 EST" to "2013-02-24 11:37:19 UTC" which
   // has same numbers but is not the same time in an absolute sense (with apologies to Einstein).
   //
   local.setTimeSpec (Qt::UTC);

   // The "same" time in different time zones is a different time.
   // Use that difference to determine the local time offset from UTC.
   //
   return atTime.secsTo (local);
}

//------------------------------------------------------------------------------
//
QString QEUtilities::getTimeZoneTLA (const Qt::TimeSpec timeSpec, const QDateTime & atTime)
{
   QString result;
   int actualOffset;

   switch (timeSpec) {
      case Qt::UTC:
         result = "UTC";
         break;

      case Qt::LocalTime:
         // Get offset and convert to hours
         //
         actualOffset = QEUtilities::getTimeZoneOffset (atTime);

#ifdef _XOPEN_SOURCE
#include <time.h>
         // Ensure zone information initialised.
         //
         tzset ();

         // timezone is seconds West of GMT, whereas actualOffset is seconds East,
         // hence the negation in the equality test.
         //
         if ((actualOffset == -timezone) || (daylight == 0) ) {
            result = tzname [0];
         } else {
            // offsets not equal and daylight available - use it.
            //
            result = tzname [1];
         }
#else
         // Not sure what Windows has to offer (yet).
         //
         result = "???";
#endif
         break;

      case Qt::OffsetFromUTC:    // Don't know what to do here,
      default:                   // or here - both unexpected;
         result = "ERR";
         break;
   }
   return result;
}

//------------------------------------------------------------------------------
//
QString QEUtilities::getTimeZoneTLA (const QDateTime & atTime)
{
   return QEUtilities::getTimeZoneTLA (atTime.timeSpec(), atTime);
}


//------------------------------------------------------------------------------
//
QString QEUtilities::enumToString (const QObject& object,
                                   const QString& enumTypeName,
                                   const int enumValue)
{
   const QMetaObject *mo =  object.metaObject();
   QString result;

   for (int e = 0; e < mo->enumeratorCount(); e++) {
      QMetaEnum metaEnum = mo->enumerator(e);
      if (metaEnum.isValid () && metaEnum.name () == enumTypeName) {
         // found it.
         //
         result = metaEnum.valueToKey (enumValue);
         break;
      }
   }

   return result;
}

//------------------------------------------------------------------------------
//
int QEUtilities::stringToEnum (const QObject& object,
                               const QString& enumTypeName,
                               const QString& enumImage,
                               bool* ok)
{
   const QMetaObject *mo =  object.metaObject();
   int result = -1;
   bool okay = false;

   for (int e = 0; e < mo->enumeratorCount(); e++) {
      QMetaEnum metaEnum = mo->enumerator(e);
      if (metaEnum.isValid () && metaEnum.name () == enumTypeName) {
         // found it.
         //
         result = metaEnum.keyToValue (enumImage.trimmed ().toAscii ().data ());
         if (result != (-1)) {
            // This is a good value.
            okay = true;
         } else {
            okay = false;  // hypothosize not okay
            for (int i = 0; i < metaEnum.keyCount (); i++) {
               if (result == metaEnum.value (i)) {
                  // This was a valid value after all.
                  //
                  okay = true;
                  break;
               }
            }
         }
         break;
      }
   }

   if (ok) *ok = okay;
   return result;
}

//------------------------------------------------------------------------------
//
QWidget* QEUtilities::findWidget (QWidget* parent, const QString& className)
{
   const QMetaObject* meta = parent->metaObject ();

   QWidget* result = NULL;
   int j, n;
   QObjectList childList;
   QObject *child = NULL;
   QWidget *childWidget = NULL;

   if (meta->className () == className) {
      // found it - return immediatley
      return parent;
   }

   childList = parent->children ();
   n = childList.count();
   for (j = 0; j < n; j++) {
      child = childList.value (j);
      // We need only tree walk widgets. All widget parents are themselves widgets.
      //
      childWidget = dynamic_cast <QWidget *>(child);
      if (childWidget) {
         // Recursive call.
         //
         result = QEUtilities::findWidget (childWidget, className);
         if (result) break;
      }
   }

   return result;
}

// end
