#include "selectMenu.h"
#include "QEImage.h"
#include <QAction>

selectMenu::selectMenu( QWidget *parent) : QMenu(parent)
{
    // All selection modes are mutually exclusive, so add to a group
    QActionGroup* selectionGroup = new QActionGroup(this);

    // Macro to create the buttons
#define NEW_SELECT_MENU_BUTTON( TITLE, ID, ACTION ) \
    ACTION = new QAction( TITLE, this );            \
    ACTION->setCheckable( true );                   \
    ACTION ->setData( imageContextMenu::ID );       \
    selectionGroup->addAction( ACTION );            \
    addAction( ACTION );

    // Create the buttons
    NEW_SELECT_MENU_BUTTON( "Pan",                      ICM_SELECT_PAN,     actionPan     )
    NEW_SELECT_MENU_BUTTON( "Horizontal Slice Profile", ICM_SELECT_HSLICE,  actionHSlice  )
    NEW_SELECT_MENU_BUTTON( "Vertical Slice Profile",   ICM_SELECT_VSLICE,  actionVSlice  )
    NEW_SELECT_MENU_BUTTON( "Line Profile",             ICM_SELECT_PROFILE, actionProfile )
    NEW_SELECT_MENU_BUTTON( "Select an Area",           ICM_SELECT_AREA,    actionArea    )
    NEW_SELECT_MENU_BUTTON( "Mark Target",              ICM_SELECT_TARGET,  actionTarget  )
    NEW_SELECT_MENU_BUTTON( "Mark Beam",                ICM_SELECT_BEAM,    actionBeam    )

    // Set the title
    setTitle( "Mode" );
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
    // Check the appropriate selection option
    switch( (QEImage::selectOptions)(mode) )
    {
        case QEImage::SO_PANNING : actionPan    ->setChecked( true ); break;
        case QEImage::SO_HSLICE:   actionHSlice ->setChecked( true ); break;
        case QEImage::SO_VSLICE:   actionVSlice ->setChecked( true ); break;
        case QEImage::SO_AREA:     actionArea   ->setChecked( true ); break;
        case QEImage::SO_PROFILE:  actionProfile->setChecked( true ); break;
        case QEImage::SO_TARGET:   actionTarget ->setChecked( true ); break;
        case QEImage::SO_BEAM:     actionTarget ->setChecked( true ); break;
        default:  break;
    }
}

void selectMenu::setPanEnabled( bool enablePan )
{
    actionPan->setVisible( enablePan );
}

void selectMenu::setVSliceEnabled( bool enableVSliceSelection )
{
    actionVSlice->setVisible( enableVSliceSelection );
}

void selectMenu::setHSlicetEnabled( bool enableHSliceSelection )
{
    actionHSlice->setVisible( enableHSliceSelection );
}

void selectMenu::setAreaEnabled( bool enableAreaSelection )
{
    actionArea->setVisible( enableAreaSelection );
}

void selectMenu::setProfileEnabled( bool enableProfileSelection )
{
    actionProfile->setVisible( enableProfileSelection );
}

void selectMenu::setTargetEnabled( bool enableTargetSelection )
{
    actionTarget->setVisible( enableTargetSelection );
    actionBeam->setVisible( enableTargetSelection );
}

