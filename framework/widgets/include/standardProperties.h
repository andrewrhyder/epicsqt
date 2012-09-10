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

#ifndef STANDARDPROPERTIES_H
#define STANDARDPROPERTIES_H

#include <QWidget>
#include <ContainerProfile.h>

class standardProperties
{

public:
    standardProperties( QWidget* ownerIn );
    virtual ~standardProperties(){}

protected:
    userLevels getUserLevelVisibility();
    void setUserLevelVisibility( userLevels level );

    userLevels getUserLevelEnabled();
    void setUserLevelEnabled( userLevels level );

    bool getApplicationEnabled() const;
    void setApplicationEnabled( bool state );

    void setDataDisabled( bool disable );

    // visible (widget is visible outside 'Designer')
    void setRunVisible( bool visibleIn );
    bool getRunVisible();

    void checkVisibilityEnabledLevel( userLevels level );

private:
    QWidget* owner;

    userLevels currentLevel;

    void setSuperEnabled();
    void setSuperVisibility();

    userLevels visibilityLevel; // User level below which the widget will be made invisible
    userLevels enabledLevel;    // User level below which the widget will be made disabled

    bool applicationVisibility; // The 'visibility' state the widget would be in if it wasn't being held invisible due to inapropriate user level
    bool applicationEnabled;    // The 'enabled/disabled' state the widget would be in if it wasn't being held disabled by inapropriate user level or disconnected status
    bool dataDisabled;          // Flag the widget should be disabled due to disconnected status
};

#endif // STANDARDPROPERTIES_H
