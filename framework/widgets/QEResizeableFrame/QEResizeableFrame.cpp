/*  QEResizeableFrame.cpp
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
 *  Copyright (c) 2013
 */

#include <QDebug>
#include <QMouseEvent>
#include <QRect>
#include <QString>

#include "QEResizeableFrame.h"


#define MAX(a, b)           ((a) >= (b) ? (a) : (b))
#define MIN(a, b)           ((a) <= (b) ? (a) : (b))
#define LIMIT(x,low,high)   (MAX(low, MIN(x, high)))


static const QString passive ("QWidget { background-color: #a0c0e0; }");
static const QString active  ("QWidget { background-color: #e0e0e0; }");

//------------------------------------------------------------------------------
//
QEResizeableFrame::QEResizeableFrame (QWidget *parent) : QFrame (parent)
{
   this->setup (10, 100);
}

//------------------------------------------------------------------------------
//
QEResizeableFrame::QEResizeableFrame (int minimumIn, int maximumIn, QWidget *parent) : QFrame (parent)
{
   this->setup (minimumIn, maximumIn);
}


//------------------------------------------------------------------------------
//
QEResizeableFrame::~QEResizeableFrame ()
{
   // place holder
}

//------------------------------------------------------------------------------
//
void QEResizeableFrame::setup (int minimumIn, int maximumIn)
{
   this->allowedMin = MAX (minimumIn, 8);
   this->allowedMax = MAX (maximumIn, this->allowedMin);
   this->isActive = false;

   this->userWidget = NULL;

   this->grabber = new QWidget (this);
   this->grabber->setCursor (QCursor (Qt::SizeVerCursor));
   this->grabber->setStyleSheet (passive);
   this->grabber->setFixedHeight (4);
   this->grabber->setMouseTracking (true);
   this->grabber->installEventFilter (this);   // Use self as the event filter object.
   this->grabber->setToolTip ("");

   this->layout = new QVBoxLayout (this);
   this->layout->setMargin (1);
   this->layout->setSpacing (1);

   this->layout->addWidget (this->grabber);
}

//------------------------------------------------------------------------------
//
QWidget *QEResizeableFrame::widget() const {
   return this->userWidget;
}

//------------------------------------------------------------------------------
//
void QEResizeableFrame::setWidget (QWidget *widgetIn)
{
   // There should usually be one of these, but just in case ...
   //
   if (this->userWidget) {
      this->layout->removeWidget (this->userWidget);
      delete this->userWidget;
      this->userWidget = NULL;
   }

   // Did user specify a widget.
   //
   this->userWidget = widgetIn;
   if (widgetIn) {
      this->userWidget->setParent (this);
      // Ensure user widget resizeable.
      //
      this->userWidget->setMinimumHeight (0);
      this->userWidget->setMaximumHeight (8000);   // designer deafults to 16777215
   }

   // Remove grabber (now empty) and append widgets in desired order.
   //
   this->layout->removeWidget (this->grabber);

   if (this->userWidget) {
      this->layout->addWidget (this->userWidget);
   }
   this->layout->addWidget (this->grabber);
}

//------------------------------------------------------------------------------
//

QWidget *QEResizeableFrame::takeWidget ()
{
   QWidget *result = this->userWidget;

   if (result) {
      this->setWidget (NULL);
      result->setParent (NULL);
   }
   return result;
}

//------------------------------------------------------------------------------
//
void QEResizeableFrame::setGrabberToolTip (const QString & tip)
{
   this->grabber->setToolTip (tip);
}

//------------------------------------------------------------------------------
//
bool QEResizeableFrame::eventFilter (QObject *obj, QEvent *event)
{
   QMouseEvent * mouseEvent = NULL;
   int y;
   int dy;
   int h;
   int newh;

   // case on type first else we get a seg fault.
   //
   switch (event->type ()) {

      case QEvent::MouseButtonPress:
         if (obj == this->grabber) {
            this->isActive = true;
            this->grabber->setStyleSheet (active);
            return true;
         }
         break;

      case QEvent::MouseButtonRelease:
         if (obj == this->grabber) {
            this->isActive = false;
            this->grabber->setStyleSheet (passive);
            return true;
         }
         break;


      case QEvent::MouseMove:
         mouseEvent = static_cast<QMouseEvent *> (event);

         if (obj == this->grabber) {
            // if Actived then stay Activated otherwise ...
            if (this->isActive) {
               y = mouseEvent->y ();
               dy = y - 2;
               h = this->geometry().height ();
               newh = LIMIT (h + dy, this->allowedMin, this->allowedMax);
               this->setFixedHeight (newh);
            }
            return true;
         }
         break;

      default:
         // Just fall through
         break;
   }

   // standard event processing
   //
   return QObject::eventFilter (obj, event);
}

// end
