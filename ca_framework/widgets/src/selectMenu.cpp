#include "selectMenu.h"
#include "QCaImage.h"
#include <QAction>

selectMenu::selectMenu( QWidget *parent) : QMenu(parent)
{
    //!! macro for this
    actionPan     = new QAction( "Pan",              this ); actionPan    ->setData( contextMenu::CM_SELECT_PAN     ); addAction( actionPan     );
    actionHSlice  = new QAction( "Horizontal slice", this ); actionHSlice ->setData( contextMenu::CM_SELECT_HSLICE  ); addAction( actionHSlice  );
    actionVSlice  = new QAction( "Vertical slice",   this ); actionVSlice ->setData( contextMenu::CM_SELECT_VSLICE  ); addAction( actionVSlice  );
    actionArea    = new QAction( "Area",             this ); actionArea   ->setData( contextMenu::CM_SELECT_AREA    ); addAction( actionArea    );
    actionProfile = new QAction( "Line profile",     this ); actionProfile->setData( contextMenu::CM_SELECT_PROFILE ); addAction( actionProfile );

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
    actionPan    ->setChecked( (QCaImage::selectOptions)(mode) == QCaImage::SO_PANNING );
    actionHSlice ->setChecked( (QCaImage::selectOptions)(mode) == QCaImage::SO_HSLICE );
    actionVSlice ->setChecked( (QCaImage::selectOptions)(mode) == QCaImage::SO_VSLICE );
    actionArea   ->setChecked( (QCaImage::selectOptions)(mode) == QCaImage::SO_AREA );
    actionProfile->setChecked( (QCaImage::selectOptions)(mode) == QCaImage::SO_PROFILE );
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


