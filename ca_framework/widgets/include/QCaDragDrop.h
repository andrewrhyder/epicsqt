/*
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
 *  Copyright (c) 2009, 2010
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef QCADRAGDROP_H
#define QCADRAGDROP_H

#include <QtGui>
#include <QDragEnterEvent>
#include <QDropEvent>

class QCaDragDrop {

public:
    QCaDragDrop( QWidget* ownerIn );
    virtual ~QCaDragDrop(){}
  protected:

    // Drag and Drop

    // Called by the QCa widget in the QCa widget's drag/drop implementation
    void qcaDragEnterEvent(QDragEnterEvent *event);
    void qcaDropEvent(QDropEvent *event);
    void qcaMousePressEvent(QMouseEvent *event);

    // Virtual functions to allow this class to get and set the QCa widgets drag/drop text
    // They are not defined as pure virtual as the QCa widget does not have to use this class's drag drop.
    virtual void setDropText( QString ) {}
    virtual QString getDropText() { return ""; }

private:
    QWidget* owner;
};

#endif // QCADRAGDROP_H
