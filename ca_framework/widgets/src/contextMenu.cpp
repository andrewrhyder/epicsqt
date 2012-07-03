#include "contextMenu.h"

contextMenu::contextMenu( QWidget *parent) : QMenu(parent)
{
    optionMenu = NULL;
    setTitle( "Image options" );
}

void contextMenu::getContextMenuOption( const QPoint& pos, contextMenuOptions* option, bool* checked )
{
    QAction* selectedItem = exec( pos );
    if( selectedItem )
    {
        *option = (contextMenuOptions)(selectedItem->data().toInt());
        *checked = selectedItem->isChecked();
    }
    else
    {
        *option = CM_NONE;
        *checked = false;
    }
}

void contextMenu::addMenuItem( const QString& title, const bool checkable, const bool checked, const contextMenuOptions option )
{
    addMenuItemGeneric( this, title, checkable, checked, option);
}

void contextMenu::addOptionMenuItem( const QString& title, const bool checkable, const bool checked, const contextMenuOptions option )
{
    if( !optionMenu )
    {
        optionMenu = new QMenu( "Options" );
        addMenu( optionMenu );
    }

    addMenuItemGeneric( optionMenu, title, checkable, checked, option);
}

void contextMenu::addMenuItemGeneric( QMenu* menu, const QString& title, const bool checkable, const bool checked, const contextMenuOptions option )
{
    QAction* a = new QAction( title, this );
    a->setCheckable( checkable );
    if( checkable )
    {
        a->setChecked( checked );
    }
    a->setData( option );
    menu->addAction( a );
}

