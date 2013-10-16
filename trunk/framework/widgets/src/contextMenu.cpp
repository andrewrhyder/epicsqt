/*  contextMenu.cpp
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

/* Description:
 *
 * A class to manage the QEGui context menu.
 *
 * This class creates a menu that can be added as the right click menu for QE widgets.
 * The contextMenu class cannot be based on QObject so it creates and owns an instance
 * of a small class (contextMenyObject) that is based on QObject that can manage
 * signals and slots.
 * The contextMenu class can't be based on QObject as only one base class of an object
 * can be based on QObjects and for all QE widgets that class is typically a standard Qt widget.
 *
 */

#include <contextMenu.h>
#include <QClipboard>
#include <QApplication>
#include <QDebug>
#include <QEWidget.h>
#include <QEScaling.h>

// Flag common to all context menus.
// true if 'dragging the variable
// false if dragging the data
bool contextMenu::draggingVariable = true;

//======================================================
// Methods for QObject based contextMenuObject class
void contextMenuObject::contextMenuTriggeredSlot( QAction* selectedItem ){ menu->contextMenuTriggered( selectedItem->data().toInt() ); }
void contextMenuObject::showContextMenuSlot( const QPoint& pos ){ menu->showContextMenu( pos ); }

//======================================================

// Create a class to manage the QE context menu
contextMenu::contextMenu( QEWidget* qewIn )
{
    hasConsumer = false;
    qew = qewIn;
    object = new contextMenuObject( this );
}

contextMenu::~contextMenu()
{
}

// Build the QE generic context menu
QMenu* contextMenu::buildContextMenu()
{
    // Create the menu
    QMenu* menu = new QMenu( );//qew->getQWidget() );
    menu->setStyle( QApplication::style() );

    // Get Qt widget standard menu if any
    QMenu* defaultMenu = qew->getDefaultContextMenu();

    if( defaultMenu )
    {
        defaultMenu->setStyle( QApplication::style() );

        // Apply current scaling if any to new default menu.
        //
        QEScaling::applyToWidget (defaultMenu, 10);
        menu->addMenu( defaultMenu );
        menu->addSeparator();
    }

    // Add QE context menu
    QAction* a;

    // Add menu options that require the application to provide support such as launch a strip chart.
    if( hasConsumer )
    {
        a = new QAction( "Examine Properties",     menu ); a->setCheckable( false ); a->setData( CM_SHOW_PV_PROPERTIES ); menu->addAction( a );
        a = new QAction( "Plot in StripChart",     menu ); a->setCheckable( false ); a->setData( CM_ADD_TO_STRIPCHART );  menu->addAction( a );
        a = new QAction( "Show in Scratch Pad",    menu ); a->setCheckable( false ); a->setData( CM_ADD_TO_SCRATCH_PAD ); menu->addAction( a );
        menu->addSeparator();
    }

    // Add menu options that don't require the application to provide support such as launch a strip chart.
    a = new QAction( "Copy variable name",     menu ); a->setCheckable( false ); a->setData( CM_COPY_VARIABLE );      menu->addAction( a );
    a = new QAction( "Copy data",              menu ); a->setCheckable( false ); a->setData( CM_COPY_DATA );          menu->addAction( a );
    a = new QAction( "Paste to variable name", menu ); a->setCheckable( false ); a->setData( CM_PASTE );              menu->addAction( a );
    QClipboard *cb = QApplication::clipboard();
    a->setEnabled( qew->getAllowDrop() && !cb->text().isEmpty() );
    menu->addSeparator();

    a = new QAction( "Drag variable name",     menu ); a->setCheckable( true );  a->setData( CM_DRAG_VARIABLE );      menu->addAction( a );
    a->setChecked( draggingVariable );
    a = new QAction( "Drag data",              menu ); a->setCheckable( true );  a->setData( CM_DRAG_DATA );          menu->addAction( a );
    a->setChecked( !draggingVariable );

    menu->setTitle( "Use..." );

    QObject::connect( menu, SIGNAL( triggered ( QAction* ) ), object, SLOT( contextMenuTriggeredSlot( QAction* )) );

    // This object is created dynamically as opposed to at overall contruction time,
    // so need to apply current scalling, if any to the new menu.
    QEScaling::applyToWidget (menu, 10);

    return menu;
}

// Present the context menu
void contextMenu::showContextMenu( const QPoint& pos )
{

    QPoint globalPos = qew->getQWidget()->mapToGlobal( pos );

    QMenu* menu = buildContextMenu();
    menu->exec( globalPos );
    delete menu;
}

// Return the global 'is dragging variable' flag.
// (Dragging variable is true, draging data if false)
bool contextMenu::isDraggingVariable()
{
    return draggingVariable;
}

// Set the consumer of the signal generted by this object
// (send via the associated contextMenuObject object).
void contextMenu::setConsumer (QObject* consumer)
{
    if (consumer)
    {
        hasConsumer = true;
        QObject::connect(object, SIGNAL (requestAction( const QEActionRequests& )),
                         consumer,  SLOT (requestAction( const QEActionRequests& )));
    }
}

// Connect the supplied QE widget to a slot that will present out own context menu when requested
void contextMenu::setupContextMenu()
{
    QWidget* qw = qew->getQWidget();
    qw->setContextMenuPolicy( Qt::CustomContextMenu );
    QObject::connect( qw, SIGNAL( customContextMenuRequested( const QPoint& )), object, SLOT( showContextMenuSlot( const QPoint& )));
}

// An action was selected from the context menu
void contextMenu::contextMenuTriggered( int optionNum )
{
    switch( (contextMenuOptions)(optionNum) )
    {
        default:
        case contextMenu::CM_NONE:
            break;

        case contextMenu::CM_COPY_VARIABLE:
            doCopyVariable();
            break;

        case contextMenu::CM_COPY_DATA:
            doCopyData();
            break;

        case contextMenu::CM_PASTE:
            doPaste();
            break;

        case contextMenu::CM_DRAG_VARIABLE:
            draggingVariable = true;
            break;

        case contextMenu::CM_DRAG_DATA:
            draggingVariable = false;
            break;

        case contextMenu::CM_SHOW_PV_PROPERTIES:
            doShowPvProperties();
            break;

        case contextMenu::CM_ADD_TO_STRIPCHART:
            doAddToStripChart();
            break;

        case  contextMenu::CM_ADD_TO_SCRATCH_PAD:
            doAddToScratchPad();
            break;
    }
}

// 'Copy Variable' was selected from the menu
void contextMenu::doCopyVariable()
{
    QString s = copyVariable();
    QClipboard *cb = QApplication::clipboard();
    cb->setText( s );
}

// 'Copy Data' was selected from the menu
void contextMenu::doCopyData()
{
    QClipboard *cb = QApplication::clipboard();
    QVariant v = copyData();
    switch( v.type() )
    {
        default:
        case QVariant::String:
            cb->setText( v.toString() );
            break;

        case QVariant::Image:
            cb->setImage( v.value<QImage>() );
            break;
    }
}

// 'Paste' was selected from the menu
void contextMenu::doPaste()
{
    QVariant v;
    QClipboard *cb = QApplication::clipboard();
    if( !cb->text().isEmpty() )
    {
        v = QVariant( cb->text() );
    }
    else if( !cb->image().isNull() )
    {
        v = QVariant( cb->image() );
    }
    paste( v );
}

// 'Show Properties' was selected from the menu
void contextMenu::doShowPvProperties ()
{
    QString pvName = copyVariable();
    QEActionRequests request( "PV Properties...", pvName );
    object->sendRequestAction( request );
}

// 'Add to strip chart' wasselected from the menu
void contextMenu::doAddToStripChart ()
{
    QString pvName = copyVariable();
    QEActionRequests request( "Strip Chart...", pvName );
    object->sendRequestAction( request );
}

// 'Add to scratch pad' was selected from the menu
void contextMenu::doAddToScratchPad()
{
   QString pvName = copyVariable();
   QEActionRequests request( "Scratch Pad...", pvName );
   object->sendRequestAction( request );
}

// end
