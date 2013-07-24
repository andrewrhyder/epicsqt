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
class QEWidget;

// QObject based context menu
// Instance of this created and owned by contextMenu which itself cannot be based on a QObject
class contextMenuObject : public QObject
{
    Q_OBJECT
public:
    contextMenuObject( contextMenu* menuIn ){ menu = menuIn; }          // Construction
    void sendRequestGui( const QEGuiLaunchRequests& request){ emit requestGui( request ); } // Emit a GUI launch request

signals:
    void requestGui( const QEGuiLaunchRequests& );                      // Signal 'launch a GUI'

public slots:
    void contextMenuTriggeredSlot( QAction* selectedItem );             // Slot - an item has been selected from the context menu
    void showContextMenuSlot( const QPoint& pos );                      // Slot - a widget has requested the QE sutom context menu be shown

private:
    contextMenu* menu;                                                  // contextMenu class owning this class
};

// Manage QE widget context menu
class contextMenu
{
public:
    friend class contextMenuObject;

    explicit contextMenu( QEWidget* qewIn );
    virtual ~contextMenu();


    void setConsumer (QObject *consumer);               // Set the consumer of the signal generted by this object

    enum contextMenuOptions{ CM_NONE,                   // Menu options
                             CM_COPY_VARIABLE, CM_COPY_DATA, CM_PASTE,
                             CM_DRAG_VARIABLE, CM_DRAG_DATA,
                             CM_SHOW_PV_PROPERTIES,
                             CM_ADD_TO_STRIPCHART,
                             CM_SPECIFIC_WIDGETS_START_HERE };
    void setupContextMenu();                            // Set up the standard QE context menu for a QE widget (conextMenu class is a base class for all QE widgets, but a menu is only available to users if this is called)
    bool isDraggingVariable();                          // Return the global 'is dragging variable' flag (Dragging variable is true, draging data if false)

    QMenu* buildContextMenu();                             // Build the QE generic context menu
    void contextMenuTriggered( int selectedItemNum );   // An action was selected from the context menu

    virtual QString copyVariable(){ return ""; }        // Function a widget may implement to perform a 'copy variable' operation
    virtual QVariant copyData(){ return ""; }           // Function a widget may implement to perform a 'copy data' operation
    virtual void paste( QVariant ){}                    // Function a widget may implement to perform a 'paste' operation

private:
    contextMenuObject* object;                          // Our own QObject based class to managing signals and slots
    void doCopyVariable();                              // 'Copy Variable' was selected from the menu
    void doCopyData();                                  // 'Copy Data' was selected from the menu
    void doPaste();                                     // 'Paste' was selected from the menu
    void doShowPvProperties();                          // 'Show Properties' was selected from the menu
    void doAddToStripChart();                           // 'Add to strip chart' wasselected from the menu
    void showContextMenu( const QPoint& pos );          // Present the context menu
    static bool draggingVariable;                       // Global 'dragging variable' flag (dragging data if false)
    QEWidget* qew;                                      // QEWidget associated with this instance
};


#endif // CONTEXTMENU_H
