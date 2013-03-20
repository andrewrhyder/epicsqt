/*  QEResizeableFrame.h
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

#ifndef QERESIZEABLEFRAME_H
#define QERESIZEABLEFRAME_H

#include <QEvent>
#include <QFrame>
#include <QObject>
#include <QWidget>
#include <QVBoxLayout>

// The QEResizeableFrame provides a frame capable of holding another widget
// to gether with a grabber widget that allows the the resizable frame, and
// hence contained widget to be resized. The class currently only supports
// vertical resizing. It could be extended to support horizontal scale.
//
// NOTE: this is not an plugin class avaiable in dsigner nor is it dervoied
//       from QEWidget as originally developed as an internal support widget
//       for the QEStripChart class and as such created programatically.
//
class QEResizeableFrame : public QFrame {
public:
   QEResizeableFrame (QWidget *parent = 0);

   /// Construct widget specifying min and max allowed heights.
   //
   QEResizeableFrame (int minimum, int maximum, QWidget *parent = 0);
   virtual ~QEResizeableFrame ();

   // This modelled on QScrollArea
   //
   /// Returns a ref to the resizeable frame's widget, or 0 if there is none.
   QWidget *widget() const;

   /// Sets the resizeable frame's widget.
   /// The widget becomes a child of the resizeable frame, and will be destroyed when
   /// the resizeable frame is deleted or when a new widget is set.
   /// Any existing widget is deleted - use takeWidget if needs be.
   void setWidget (QWidget *widget);

   /// Removes the resizeable frame's widget, and passes ownership management of the widget to the caller.
   QWidget *takeWidget();

   /// Set the toop tip for the internal grabber object.
   void setGrabberToolTip (const QString & tip);

protected:
   bool eventFilter (QObject *obj, QEvent *event);

private:
   void setup (int minimum, int maximum);

   bool isActive;
   // No slots or signals - this should be safe.
   QVBoxLayout *layout;
   QWidget *userWidget;
   QWidget *grabber;

   // We cant use widget min/maximumHeight values to store theseare we call setFixedHeight to the frames height.
   //
   int allowedMin;
   int allowedMax;
};

#endif  // QERESIZEABLEFRAME_H
