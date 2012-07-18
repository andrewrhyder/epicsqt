#include "imageContextMenu.h"

imageContextMenu::imageContextMenu( QWidget *parent) : QMenu(parent)
{
    optionMenu = NULL;
    setTitle( "Image options" );
}

void imageContextMenu::getContextMenuOption( const QPoint& pos, imageContextMenuOptions* option, bool* checked )
{
    QAction* selectedItem = exec( pos );
    if( selectedItem )
    {
        *option = (imageContextMenuOptions)(selectedItem->data().toInt());
        *checked = selectedItem->isChecked();
    }
    else
    {
        *option = ICM_NONE;
        *checked = false;
    }
}

void imageContextMenu::addMenuItem( const QString& title, const bool checkable, const bool checked, const imageContextMenuOptions option )
{
    addMenuItemGeneric( this, title, checkable, checked, option);
}

void imageContextMenu::addOptionMenuItem( const QString& title, const bool checkable, const bool checked, const imageContextMenuOptions option )
{
    if( !optionMenu )
    {
        optionMenu = new QMenu( "Options" );
        addMenu( optionMenu );
    }

    addMenuItemGeneric( optionMenu, title, checkable, checked, option);
}

void imageContextMenu::addMenuItemGeneric( QMenu* menu, const QString& title, const bool checkable, const bool checked, const imageContextMenuOptions option )
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

