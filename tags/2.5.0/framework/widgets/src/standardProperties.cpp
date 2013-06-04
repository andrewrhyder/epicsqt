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
  This class adds common property support to all QE widgets where the support involves interaction with the QWidget.
  Any common property support that requires a greater scope than the QWidget is managed in QEWidget.
*/

#include <standardProperties.h>
#include <QEWidget.h>
//#include <QCoreApplication>
//#include <QFileInfo>


// Construction.
standardProperties::standardProperties( QWidget* ownerIn )
{
    // Sanity check.
    if( ownerIn == NULL )
    {
        qWarning( "standardProperties constructor called with a null 'owner'" );
        exit( EXIT_FAILURE );
    }

    // Keep a handle on the underlying QWidget of the QE widgets
    owner = ownerIn;

    visibilityLevel = userLevelTypes::USERLEVEL_USER;
    enabledLevel = userLevelTypes::USERLEVEL_USER;
    currentLevel = userLevelTypes::USERLEVEL_USER;

    dataDisabled = false;

    applicationEnabled = true;
    applicationVisibility = true;

    displayAlarmState = true;
}

// !!
userLevelTypes::userLevels standardProperties::getUserLevelVisibility()
{
    return visibilityLevel;
}

void standardProperties::setUserLevelVisibility( userLevelTypes::userLevels levelIn )
{
    visibilityLevel = levelIn;
    setSuperVisibility();
}

userLevelTypes::userLevels standardProperties::getUserLevelEnabled()
{
    return enabledLevel;
}

void standardProperties::setUserLevelEnabled( userLevelTypes::userLevels levelIn )
{
    enabledLevel = levelIn;
    setSuperEnabled();
}


// Return what the enabled state of widget would be if connected and the appropriate user level
bool standardProperties::getApplicationEnabled() const
{
    return applicationEnabled;
}

// Set what the enabled state of widget would be if connected and the appropriate user level
void standardProperties::setApplicationEnabled( bool state )
{
    applicationEnabled = state;
    setSuperEnabled();
}


// Set the enabled/disabled state of the widget.
// Generally it is enabled or disabled according to 'applicationEnabled', however this can be overridden
// and it is can be disabled if not connected or not the appropriate user level
void standardProperties::setSuperEnabled()
{
    owner->setEnabled( applicationEnabled && !dataDisabled && (currentLevel >= enabledLevel) );
}

// Set the visibility of the widget.
// Generally it is visible or not according to 'applicationVisibility', however this can be overridden
// and it is can be disabled if not at the appropriate user level, or enabled running within designer
void standardProperties::setSuperVisibility()
{
    ContainerProfile profile;

    // Note the desired visibility
    bool vis = applicationVisibility;

    // Make visible if running within designer
    if( QEWidget::inDesigner() )
    {
        vis = true;
    }

    // If not in designer, make invisible if not at a suitable user level
    // Note, in designer, the user level will never cause visibility to be false unless in  preview
    // and the user level is changed in the preview window
    else if( profile.getUserLevel() < visibilityLevel )
    {
        vis = false;
    }

    // Apply the result
    owner->setVisible( vis );
}

// Disable the widget if not connected
void standardProperties::setDataDisabled( bool disable )
{
    dataDisabled = disable;
    setSuperEnabled();
}

void standardProperties::checkVisibilityEnabledLevel( userLevelTypes::userLevels level )
{
    currentLevel = level;
    setSuperEnabled();
    setSuperVisibility();
}


// visible (widget is visible outside 'Designer')
void standardProperties::setRunVisible( bool visibleIn )
{
    applicationVisibility = visibleIn;
    setSuperVisibility();
}

bool standardProperties::getRunVisible()
{
    return applicationVisibility;
}

// displayAlarmState. If set (default) widget will indicate the alarm state of any variable data is displaying.
void standardProperties::setDisplayAlarmState( bool displayAlarmStateIn )
{
    displayAlarmState = displayAlarmStateIn;
}

bool standardProperties::getDisplayAlarmState()
{
    return displayAlarmState;
}
