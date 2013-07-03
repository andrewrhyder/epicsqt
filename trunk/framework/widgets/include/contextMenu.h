/*  contextMenu.h
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
 *  Copyright (c) 2011
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QMenu>
#include <QEGuiLaunchRequests.h>

class contextMenu;

class contextMenuObject : public QMenu
{
    Q_OBJECT
public:

    contextMenuObject();

    void addContextMenuToWidget( QWidget* w );

    void manageChecked( bool draggingVariable );

    void setMenu( contextMenu* menuIn );
    bool isDraggingVariable();

signals:
    void requestGui( const QEGuiLaunchRequests& );

public slots:
    void contextMenuTriggered( QAction* selectedItem );
    void showContextMenu( const QPoint& pos );
    void setChecked();

private:
    static bool draggingVariable;
    QAction* dragVarAction;
    QAction* dragDataAction;
    QWidget* owner;
    contextMenu* menu;

    // We use this object as a convenience to send these signals.
    // This object is a QObject, contextMenu is not a QObject.
    void sendRequestGui( const QEGuiLaunchRequests& request);

    friend class contextMenu;
};

class contextMenu
{
public:
    friend class contextMenuObject;

    explicit contextMenu();
    virtual ~contextMenu();

    // Set the consumer of the signal generted by this object
    // (send via the associated contextMenuObject object).
    //
    void setConsumer (QObject *consumer);

    enum contextMenuOptions{ CM_NONE,
                             CM_COPY_VARIABLE, CM_COPY_DATA, CM_PASTE,
                             CM_DRAG_VARIABLE, CM_DRAG_DATA,
                             CM_SHOW_PV_PROPERTIES,
                             CM_ADD_TO_STRIPCHART,
                             CM_SPECIFIC_WIDGETS_START_HERE };

    void addContextMenuToWidget( QWidget* w );
    bool isDraggingVariable();
    QMenu* getContextMenu();

    virtual QString copyVariable(){ return ""; }
    virtual QVariant copyData(){ return ""; }
    virtual void paste( QVariant ){}

private:
    void triggered( contextMenuOptions option );
    contextMenuObject object;
    void doCopyVariable();
    void doCopyData();
    void doPaste();
    void doShowPvProperties();
    void doAddToStripChart();
};


#endif // CONTEXTMENU_H
