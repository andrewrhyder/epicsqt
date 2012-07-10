#include "selectMenu.h"
#include "QEImage.h"
#include <QAction>

selectMenu::selectMenu( QWidget *parent) : QMenu(parent)
{
#define NEW_SELECT_MENU_BUTTON( TITLE, ID, ACTION ) \
    ACTION = new QAction( TITLE, this );            \
    ACTION->setCheckable( true );                   \
    ACTION ->setData( contextMenu::ID );            \
    addAction( ACTION );

    NEW_SELECT_MENU_BUTTON( "Pan",              CM_SELECT_PAN,     actionPan     )
    NEW_SELECT_MENU_BUTTON( "Horizontal slice", CM_SELECT_HSLICE,  actionHSlice  )
    NEW_SELECT_MENU_BUTTON( "Vertical slice",   CM_SELECT_VSLICE,  actionVSlice  )
    NEW_SELECT_MENU_BUTTON( "Area",             CM_SELECT_AREA,    actionArea    )
    NEW_SELECT_MENU_BUTTON( "Line profile",     CM_SELECT_PROFILE, actionProfile )

    setTitle( "Select" );
}

// Get a selection option from the user.
// Used when this menu is used independantly of the main context menu
contextMenu::contextMenuOptions selectMenu::getSelectOption( const QPoint& pos )
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

void selectMenu::setChecked( const int mode )
{
    actionPan    ->setChecked( (QEImage::selectOptions)(mode) == QEImage::SO_PANNING );
    actionHSlice ->setChecked( (QEImage::selectOptions)(mode) == QEImage::SO_HSLICE );
    actionVSlice ->setChecked( (QEImage::selectOptions)(mode) == QEImage::SO_VSLICE );
    actionArea   ->setChecked( (QEImage::selectOptions)(mode) == QEImage::SO_AREA );
    actionProfile->setChecked( (QEImage::selectOptions)(mode) == QEImage::SO_PROFILE );
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


