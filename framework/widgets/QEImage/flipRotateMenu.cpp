#include "flipRotateMenu.h"
#include "QEImage.h"

flipRotateMenu::flipRotateMenu( QWidget *parent) : QMenu(parent)
{
    // Macro to create the buttons
#define NEW_FLIP_ROTATE_MENU_BUTTON( TITLE, ID, ACTION ) \
    ACTION = new QAction( TITLE, this );                 \
    ACTION->setData( imageContextMenu::ID );                  \
    ACTION->setCheckable( true );                        \
    addAction( ACTION );

    // Create the buttons
    NEW_FLIP_ROTATE_MENU_BUTTON( "No rotation",             ICM_ROTATE_NONE,     rotationNoneAction )
    NEW_FLIP_ROTATE_MENU_BUTTON( "Rotate 90 Clockwise",     ICM_ROTATE_RIGHT,    rotation90RAction )
    NEW_FLIP_ROTATE_MENU_BUTTON( "Rotate 90 Anticlockwise", ICM_ROTATE_LEFT,     rotation90LAction )
    NEW_FLIP_ROTATE_MENU_BUTTON( "Rotate 180",              ICM_ROTATE_180,      rotation180Action )
    NEW_FLIP_ROTATE_MENU_BUTTON( "Flip Horizontal",         ICM_FLIP_HORIZONTAL, flipHAction )
    NEW_FLIP_ROTATE_MENU_BUTTON( "Flip Vertical",           ICM_FLIP_VERTICAL,   flipVAction )

    // Group the rotation buttons (they are mutually exclusive)
    QActionGroup* rotationGroup = new QActionGroup(this);
    rotationGroup->addAction( rotationNoneAction );
    rotationGroup->addAction( rotation90RAction );
    rotationGroup->addAction( rotation90LAction );
    rotationGroup->addAction( rotation180Action );

    // Set the title
    setTitle( "Flip / Rotate" );
}

// Get a flip/rotate option from the user.
// Used when this menu is used independantly of the main context menu
imageContextMenu::imageContextMenuOptions flipRotateMenu::getFlipRotate( const QPoint& pos )
{
    QAction* selectedItem = exec( pos );
    if( selectedItem )
    {
        //!!! Doesn't return fliped state
        qDebug() << (imageContextMenu::imageContextMenuOptions)(selectedItem->data().toInt());
        return (imageContextMenu::imageContextMenuOptions)(selectedItem->data().toInt());
    }
    else
    {
        return imageContextMenu::ICM_NONE;
    }
}

void flipRotateMenu::setChecked( const int rotation, const bool flipH, const bool flipV )
{
    // Check the appropriate rotation option
    switch( (QEImage::rotationOptions)(rotation) )
    {
        case QEImage::ROTATION_0:        rotationNoneAction->setChecked( true ); break;
        case QEImage::ROTATION_90_RIGHT: rotation90RAction ->setChecked( true ); break;
        case QEImage::ROTATION_90_LEFT:  rotation90LAction ->setChecked( true ); break;
        case QEImage::ROTATION_180:      rotation180Action ->setChecked( true ); break;
    }

    // Check the appropriate flip options
    flipHAction->setChecked( flipH );
    flipVAction->setChecked( flipV );
}
