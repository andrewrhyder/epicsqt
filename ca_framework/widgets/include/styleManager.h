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
 *  Copyright (c) 2009, 2010
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QtGui>
#include <ContainerProfile.h>

#define STYLE_MANAGER_PROPERTIES \
                Q_PROPERTY(QString userLevelUserStyle READ getStyleUser WRITE setStyleUser); \
                Q_PROPERTY(QString userLevelScientistStyle READ getStyleScientist WRITE setStyleScientist); \
                Q_PROPERTY(QString userLevelEngineerStyle READ getStyleEngineer WRITE setStyleEngineer);


class styleManager {

public:
    styleManager( QWidget* ownerIn );
    virtual ~styleManager(){}

protected:
    void setStyleUser( QString style );
    QString getStyleUser();
    void setStyleScientist( QString style );
    QString getStyleScientist();
    void setStyleEngineer( QString style );
    QString getStyleEngineer();

    void updateDataStyle( QString style );
    void updateStatusStyle( QString style );

    void styleUserLevelChanged( userLevels levelIn );

private:
    QWidget* owner;


    QString userUserStyle;      // Style to apply to widget when current user is a 'user'
    QString userScientistStyle; // Style to apply to widget when current user is a 'scientist'
    QString userEngineerStyle;  // Style to apply to widget when current user is a 'engineer'

    QString defaultStyleSheet;
    QString statusStyleSheet;
    QString dataStyleSheet;

    QString lastDataStyle;

    void updateStyleSheet();

    userLevels level;
};

#endif // STYLEMANAGER_H
