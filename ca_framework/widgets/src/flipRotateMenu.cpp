#include "flipRotateMenu.h"
#include "QCaImage.h"

flipRotateMenu::flipRotateMenu( QWidget *parent) : QMenu(parent)
{
    QAction* a;

    //!!! Set up a macro to do this table
    a = new QAction( "No rotation",             this ); a->setData( contextMenu::CM_ROTATE_NONE     );
    a->setCheckable( true ); addAction( a );        rotationNoneAction = a;

    a = new QAction( "Rotate 90 Clockwise",     this ); a->setData( contextMenu::CM_ROTATE_RIGHT    );
    a->setCheckable( true ); addAction( a ); rotation90RAction = a;

    a = new QAction( "Rotate 90 Anticlockwise", this ); a->setData( contextMenu::CM_ROTATE_LEFT     );
    a->setCheckable( true );addAction( a );  rotation90LAction = a;

    a = new QAction( "Rotate 180",              this ); a->setData( contextMenu::CM_ROTATE_180      );
    a->setCheckable( true ); addAction( a );      rotation180Action = a;



    a = new QAction( "Flip Horizontal",         this ); a->setData( contextMenu::CM_FLIP_HORIZONTAL );
    a->setCheckable( true ); addAction( a ); flipHAction = a;

    a = new QAction( "Flip Vertical",           this ); a->setData( contextMenu::CM_FLIP_VERTICAL   );
    a->setCheckable( true ); addAction( a ); flipVAction = a;

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

void flipRotateMenu::setChecked( const int rotation, const bool flipH, const bool flipV )
{
    rotationNoneAction->setChecked( (QCaImage::rotationOptions)(rotation) == QCaImage::ROTATION_0        );
    rotation90RAction->setChecked ( (QCaImage::rotationOptions)(rotation) == QCaImage::ROTATION_90_RIGHT );
    rotation90LAction->setChecked ( (QCaImage::rotationOptions)(rotation) == QCaImage::ROTATION_90_LEFT  );
    rotation180Action->setChecked ( (QCaImage::rotationOptions)(rotation) == QCaImage::ROTATION_180      );
    flipHAction->setChecked( flipH );
    flipVAction->setChecked( flipV );
}
