#include "zoomMenu.h"

zoomMenu::zoomMenu( const bool areaSelected, QWidget *parent) : QMenu(parent)
{
    QAction* a;

    if( areaSelected )
    {
        a = new QAction( "Selected area", this );
        a->setData( contextMenu::CM_ZOOM_SELECTED );
        addAction( a );
    }

    a = new QAction( "Fit",  this ); a->setData( contextMenu::CM_ZOOM_FIT ); addAction( a );
    a = new QAction( "10%",  this ); a->setData( contextMenu::CM_ZOOM_10  ); addAction( a );
    a = new QAction( "25%",  this ); a->setData( contextMenu::CM_ZOOM_25  ); addAction( a );
    a = new QAction( "50%",  this ); a->setData( contextMenu::CM_ZOOM_50  ); addAction( a );
    a = new QAction( "75%",  this ); a->setData( contextMenu::CM_ZOOM_75  ); addAction( a );
    a = new QAction( "100%", this ); a->setData( contextMenu::CM_ZOOM_100 ); addAction( a );
    a = new QAction( "150%", this ); a->setData( contextMenu::CM_ZOOM_150 ); addAction( a );
    a = new QAction( "200%", this ); a->setData( contextMenu::CM_ZOOM_200 ); addAction( a );
    a = new QAction( "300%", this ); a->setData( contextMenu::CM_ZOOM_300 ); addAction( a );
    a = new QAction( "400%", this ); a->setData( contextMenu::CM_ZOOM_400 ); addAction( a );

    setTitle( "Zoom" );
}

// Get a zoom factor from the user.
// Used when this menu is used independantly of the main context menu
contextMenu::contextMenuOptions zoomMenu::getZoom( const QPoint& pos )
{
    QAction* selectedItem = exec( pos );
    if( selectedItem )
    {
        return (contextMenu::contextMenuOptions)(selectedItem->data().toInt());
    }
    else
    {
        return contextMenu::CM_NONE;
    }
}
