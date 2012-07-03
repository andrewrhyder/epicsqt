#include "flipRotateMenu.h"

flipRotateMenu::flipRotateMenu( const QCaImage::rotationOptions rotation, const bool flipH, const bool flipV, QWidget *parent) : QMenu(parent)
{
    QAction* a;

    a = new QAction( "No rotation",             this ); a->setData( contextMenu::CM_ROTATE_NONE     );
    a->setCheckable( true ); a->setChecked( rotation == QCaImage::ROTATION_0 ), addAction( a );

    a = new QAction( "Rotate 90 Clockwise",     this ); a->setData( contextMenu::CM_ROTATE_RIGHT    );
    a->setCheckable( true ); a->setChecked( rotation == QCaImage::ROTATION_90_RIGHT ), addAction( a );

    a = new QAction( "Rotate 90 Anticlockwise", this ); a->setData( contextMenu::CM_ROTATE_LEFT     );
    a->setCheckable( true ); a->setChecked( rotation == QCaImage::ROTATION_90_LEFT ), addAction( a );

    a = new QAction( "Rotate 180",              this ); a->setData( contextMenu::CM_ROTATE_180      );
    a->setCheckable( true ); a->setChecked( rotation == QCaImage::ROTATION_180 ), addAction( a );

    a = new QAction( "Flip Horizontal",         this ); a->setData( contextMenu::CM_FLIP_HORIZONTAL );
    a->setCheckable( true ); a->setChecked( flipH ), addAction( a );

    a = new QAction( "Flip Vertical",           this ); a->setData( contextMenu::CM_FLIP_VERTICAL   );
    a->setCheckable( true ); a->setChecked( flipV ), addAction( a );

    setTitle( "Flip / Rotate" );
}

// Get a flip/rotate option from the user.
// Used when this menu is used independantly of the main context menu
contextMenu::contextMenuOptions flipRotateMenu::getFlipRotate( const QPoint& pos )
{
    QAction* selectedItem = exec( pos );
    if( selectedItem )
    {
        //!!! Doesn't return fliped state
        return (contextMenu::contextMenuOptions)(selectedItem->data().toInt());
    }
    else
    {
        return contextMenu::CM_NONE;
    }
}
