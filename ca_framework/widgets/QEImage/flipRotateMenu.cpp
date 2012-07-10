#include "flipRotateMenu.h"
#include "QCaImage.h"

flipRotateMenu::flipRotateMenu( QWidget *parent) : QMenu(parent)
{

#define NEW_FLIP_ROTATE_MENU_BUTTON( TITLE, ID, ACTION ) \
    ACTION = new QAction( TITLE, this );                 \
    ACTION->setData( contextMenu::ID );                  \
    ACTION->setCheckable( true );                        \
    addAction( ACTION );

    NEW_FLIP_ROTATE_MENU_BUTTON( "No rotation",             CM_ROTATE_NONE,     rotationNoneAction )
    NEW_FLIP_ROTATE_MENU_BUTTON( "Rotate 90 Clockwise",     CM_ROTATE_RIGHT,    rotation90RAction )
    NEW_FLIP_ROTATE_MENU_BUTTON( "Rotate 90 Anticlockwise", CM_ROTATE_LEFT,     rotation90LAction )
    NEW_FLIP_ROTATE_MENU_BUTTON( "Rotate 180",              CM_ROTATE_180,      rotation180Action )
    NEW_FLIP_ROTATE_MENU_BUTTON( "Flip Horizontal",         CM_FLIP_HORIZONTAL, flipHAction )
    NEW_FLIP_ROTATE_MENU_BUTTON( "Flip Vertical",           CM_FLIP_VERTICAL,   flipVAction )

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
        qDebug() << (contextMenu::contextMenuOptions)(selectedItem->data().toInt());
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
