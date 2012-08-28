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

/*!
  This class adds common style support to all QCa widgets if required.

  Standard properties for all QCa widgets specify a style to be applied for user, scientist, and engineer mode.
  Also QCa widgets can specify data or status related style changes.

  All QCa widgets (eg, QELabel, QELineEdit) have an instance of this class as they based on
  QCaWidget which itself uses this class as a base class.

  To enable user level related properties to affect style, the QCa widget must include the following:
        !!! document

  To use this class to manage style changes related to data and status the QCa widget must include the following:
        !!! document

  Note, this class notes the initial style when instantiated and uses that style as the base style for
  all style changes. This means any style changes not performed through this class will be lost the next
  time this class changes the style.

*/

#include <styleManager.h>

// Construction.
styleManager::styleManager( QWidget* ownerIn )
{
    // Keep a handle on the underlying QWidget of the QCa widget
    owner = ownerIn;
    defaultStyleSheet = owner->styleSheet();
    level = USERLEVEL_USER;
}


//!!
void styleManager::setStyleUser( QString style )
{
    userUserStyle = style;

}

//!!
QString styleManager::getStyleUser()
{
    return userUserStyle;
}

//!!
void styleManager::setStyleScientist( QString style )
{
    userScientistStyle = style;
}

//!!
QString styleManager::getStyleScientist()
{
    return userScientistStyle;
}

//!!
void styleManager::setStyleEngineer( QString style )
{
    userEngineerStyle = style;
}

//!!
QString styleManager::getStyleEngineer()
{
    return userEngineerStyle;
}

//!!
void styleManager::updateDataStyle( QString style )
{
    dataStyleSheet = style;
    updateStyleSheet();
}

//!!
void styleManager::updateStatusStyle( QString style )
{
    statusStyleSheet = style;
    updateStyleSheet();
}


//!!! Update the style sheet with the various style sheet components used to modify the label style (alarm info, enumeration color)
void styleManager::updateStyleSheet()
{
    QString levelStyle;
    switch( level )
    {
        case USERLEVEL_USER:
            levelStyle = userUserStyle;
            break;

        case USERLEVEL_SCIENTIST:
            levelStyle = userScientistStyle;
            break;

        case USERLEVEL_ENGINEER:
            levelStyle = userEngineerStyle;
            break;
    }

    QString newStyleSheet;
    newStyleSheet.append( defaultStyleSheet ).append( statusStyleSheet ).append( dataStyleSheet ).append(levelStyle);
    owner->setStyleSheet( newStyleSheet );
}

//!!
void styleManager::styleUserLevelChanged( userLevels levelIn )
{
    bool newLevel = level != levelIn;
    level = levelIn;
    if( newLevel )
    {
        updateStyleSheet();
    }
}
