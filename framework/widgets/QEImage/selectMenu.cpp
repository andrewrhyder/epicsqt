#include "selectMenu.h"
#include "QEImage.h"
#include <QAction>

selectMenu::selectMenu( QWidget *parent) : QMenu(parent)
{
#define NEW_SELECT_MENU_BUTTON( TITLE, ID, ACTION ) \
    ACTION = new QAction( TITLE, this );            \
    ACTION->setCheckable( true );                   \
    ACTION ->setData( imageContextMenu::ID );            \
    addAction( ACTION );

    NEW_SELECT_MENU_BUTTON( "Pan",              ICM_SELECT_PAN,     actionPan     )
    NEW_SELECT_MENU_BUTTON( "Horizontal slice", ICM_SELECT_HSLICE,  actionHSlice  )
    NEW_SELECT_MENU_BUTTON( "Vertical slice",   ICM_SELECT_VSLICE,  actionVSlice  )
    NEW_SELECT_MENU_BUTTON( "Area",             ICM_SELECT_AREA,    actionArea    )
    NEW_SELECT_MENU_BUTTON( "Line profile",     ICM_SELECT_PROFILE, actionProfile )
    NEW_SELECT_MENU_BUTTON( "Target",           ICM_SELECT_TARGET,  actionTarget  )

    setTitle( "Select" );
}

// Get a selection option from the user.
// Used when this menu is used independantly of the main context menu
imageContextMenu::imageContextMenuOptions selectMenu::getSelectOption( const QPoint& pos )
{
    QAction* selectedItem = exec( pos );
    if( selectedItem )
    {
        return (imageContextMenu::imageContextMenuOptions)(selectedItem->data().toInt());
    }
    else
    {
        return imageContextMenu::ICM_NONE;
    }
}

void selectMenu::setChecked( const int mode )
{
    actionPan    ->setChecked( (QEImage::selectOptions)(mode) == QEImage::SO_PANNING );
    actionHSlice ->setChecked( (QEImage::selectOptions)(mode) == QEImage::SO_HSLICE );
    actionVSlice ->setChecked( (QEImage::selectOptions)(mode) == QEImage::SO_VSLICE );
    actionArea   ->setChecked( (QEImage::selectOptions)(mode) == QEImage::SO_AREA );
    actionProfile->setChecked( (QEImage::selectOptions)(mode) == QEImage::SO_PROFILE );
    actionTarget ->setChecked( (QEImage::selectOptions)(mode) == QEImage::SO_TARGET );
}

void selectMenu::setPanEnabled( bool enablePan )
{
    actionPan->setEnabled( enablePan );
}

void selectMenu::setVSliceEnabled( bool enableVSliceSelection )
{
    actionVSlice->setEnabled( enableVSliceSelection );
}

void selectMenu::setHSlicetEnabled( bool enableHSliceSelection )
{
    actionHSlice->setEnabled( enableHSliceSelection );
}

void selectMenu::setAreaEnabled( bool enableAreaSelection )
{
    actionArea->setEnabled( enableAreaSelection );
}

void selectMenu::setProfileEnabled( bool enableProfileSelection )
{
    actionProfile->setEnabled( enableProfileSelection );
}

void selectMenu::setTargetEnabled( bool enableTargetSelection )
{
    actionProfile->setEnabled( enableTargetSelection );
}

