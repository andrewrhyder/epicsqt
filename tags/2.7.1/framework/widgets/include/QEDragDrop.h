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

#ifndef QEDragDrop_H
#define QEDragDrop_H

#include <QtGui>
#include <QDragEnterEvent>
#include <QDropEvent>

class QEDragDrop {

public:
    QEDragDrop( QWidget* ownerIn );
    virtual ~QEDragDrop(){}
    bool getAllowDrop();
  protected:

    // Drag and Drop

    // Called by the QE widgets in the QE widgets's drag/drop implementation
    void qcaDragEnterEvent(QDragEnterEvent *event);
    void qcaDropEvent(QDropEvent *event);
    void qcaMousePressEvent(QMouseEvent *event);

    // Virtual functions to allow this class to get and set the QE widgets drag/drop text
    // They are not defined as pure virtual as the QE widgets does not have to use this class's drag drop.
    virtual void setDrop( QVariant ) {}
    virtual QVariant getDrop() { return QVariant(); }

    void setAllowDrop( bool allowDropIn );

private:
    QWidget* owner;
    bool allowDrop;

};

#endif // QEDragDrop_H