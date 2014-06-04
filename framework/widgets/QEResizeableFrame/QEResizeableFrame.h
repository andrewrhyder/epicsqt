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
// together with a grabber widget that allows the frame to be re-sized, and
// hence contained widget to be resized. The class currently only supports
// vertical and horizontal resizing.
//
// NOTE: this is not a class available in designer as a plugin nor is it derived
//       from QEWidget nor has it any properties. It was originally developed
//       as an internal support widget for the QEStripChart class and as such
//       created and configured programatically.
//
class QEResizeableFrame : public QFrame {
public:
   enum GrabbingEdges { TopEdge, LeftEdge, BottomEdge, RightEdge };

   QEResizeableFrame (GrabbingEdges grabbingEdge, QWidget *parent = 0);

   /// Construct widget specifying min and max allowed heights.
   QEResizeableFrame (GrabbingEdges grabbingEdge, int minimum, int maximum, QWidget *parent = 0);
   virtual ~QEResizeableFrame ();

   // This modelled on QScrollArea
   //
   /// Returns a ref to the resizeable frame's widget, or 0 if there is none.
   QWidget *widget() const;

   /// Sets the resizeable frame's widget.
   /// The widget becomes a child of the resizeable frame, and will be destroyed when
   /// the resizeable frame is deleted or when a new widget is set.
   /// Any existing widget is deleted - use takeWidget first if needs be.
   void setWidget (QWidget *widget);

   /// Removes the resizeable frame's widget, and passes ownership management of the widget to the caller.
   QWidget *takeWidget();

   /// Set the tool tip for the internal grabber object.
   void setGrabberToolTip (const QString & tip);

   /// (Re)set allowed limits.
   void setAllowedMinimum (const int minimum);
   int getAllowedMinimum () const;

   void setAllowedMaximum (const int maximum);
   int getAllowedMaximum () const;

protected:
   bool eventFilter (QObject *obj, QEvent *event);

private:
   void applyLimits ();
   void setup (GrabbingEdges grabbingEdge, int minimum, int maximum);
   bool isVertical ();
   void processMouseMove (const int x, const int y);
   bool isActive;
   bool noMoreDebug;

   // No slots or signals - this should be safe, as in moc won't get confused.
   //
   QBoxLayout* layout;
   QWidget* userWidget;
   QWidget* grabber;
   QWidget* defaultWidget;

   GrabbingEdges grabbingEdge;
   // We can't use widget's min/maximumHeight values to store these as we call setFixedHeight
   // to the frame height.
   //
   int allowedMin;
   int allowedMax;
};

#endif  // QERESIZEABLEFRAME_H
