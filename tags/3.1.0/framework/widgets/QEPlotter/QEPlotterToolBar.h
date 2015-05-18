/*  QEPlotterToolBar.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2013 Australian Synchrotron.
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 *
 */

#ifndef QEPLOTTERTOOLBAR_H
#define QEPLOTTERTOOLBAR_H

#include <QFrame>
#include <QPushButton>
#include <QHash>
#include <QPushButton>
#include <QWidget>

#include "QEPlotterNames.h"

/// This class holds all the QEPlotter tool bar widget.
///
class QEPlotterToolBar : public QFrame {
Q_OBJECT
public:  
   explicit QEPlotterToolBar (QWidget *parent = 0);
   virtual ~QEPlotterToolBar ();

   static const int designHeight = 32;

   void setEnabled (const QEPlotterNames::MenuActions action, const bool enabled);

signals:
   // Note: this is the same signature as the QEPlotterMenu signal.
   // For the toolbar slot is always 0.
   //
   void selected (const QEPlotterNames::MenuActions action, const int slot);

protected:
   void resizeEvent (QResizeEvent * event);

private:
   // Map actions <==>  QPushButtons
   //
   QHash <QPushButton*, QEPlotterNames::MenuActions> buttonToActionMap;
   QHash <QEPlotterNames::MenuActions, QPushButton*> actionToButtonMap;

private slots:
   // Send from the various buttons on the toolbar.
   //
   void buttonClicked (bool checked = false);
};

#endif  // QEPLOTTERTOOLBAR_H
