/*
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

/*
 This class manages the selection mode menu for the QEImage widget
 */

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
    NEW_SELECT_MENU_BUTTON( "Select Area 1",            ICM_SELECT_AREA1,   actionArea1    )
    NEW_SELECT_MENU_BUTTON( "Select Area 2",            ICM_SELECT_AREA2,   actionArea2    )
    NEW_SELECT_MENU_BUTTON( "Select Area 3",            ICM_SELECT_AREA3,   actionArea3    )
    NEW_SELECT_MENU_BUTTON( "Select Area 4",            ICM_SELECT_AREA4,   actionArea4    )
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
        case QEImage::SO_AREA1:    actionArea1  ->setChecked( true ); break;
        case QEImage::SO_AREA2:    actionArea2  ->setChecked( true ); break;
        case QEImage::SO_AREA3:    actionArea3  ->setChecked( true ); break;
        case QEImage::SO_AREA4:    actionArea4  ->setChecked( true ); break;
        case QEImage::SO_PROFILE:  actionProfile->setChecked( true ); break;
        case QEImage::SO_TARGET:   actionTarget ->setChecked( true ); break;
        case QEImage::SO_BEAM:     actionBeam   ->setChecked( true ); break;
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
    actionArea1->setVisible( enableAreaSelection );
    actionArea2->setVisible( enableAreaSelection );
    actionArea3->setVisible( enableAreaSelection );
    actionArea4->setVisible( enableAreaSelection );
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

bool selectMenu::getPanEnabled()
{
    return actionPan->isVisible();
}

bool selectMenu::getVSliceEnabled()
{
    return actionVSlice->isVisible();
}

bool selectMenu::getHSliceEnabled()
{
    return actionHSlice->isVisible();
}

bool selectMenu::getAreaEnabled()
{
    return actionArea1->isVisible();    // Assumes actionArea1-4 are all the same
}

bool selectMenu::getProfileEnabled()
{
    return actionProfile->isVisible();
}

bool selectMenu::getTargetEnabled()
{
    return actionTarget->isVisible();   // Assumes actionTarget and actionBeam are both the same
}
