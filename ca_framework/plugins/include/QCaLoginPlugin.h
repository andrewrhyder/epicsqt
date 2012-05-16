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
 *    Ricardo Fernandes
 *  Contact details:
 *    ricardo.fernandes@synchrotron.org.au
 */

#ifndef QCALOGINPLUGIN_H
#define QCALOGINPLUGIN_H

#include <QString>
#include <QCaLogin.h>

class QCaLoginPlugin:public QCaLogin
{

    Q_OBJECT

    public:
        QCaLoginPlugin( QWidget *parent = 0 );

        Q_PROPERTY(bool showButtonLogin READ getShowButtonLogin WRITE setShowButtonLogin)

        Q_PROPERTY(QString userPassword READ getUserPassword WRITE setUserPassword)

        Q_PROPERTY(QString scientistPassword READ getScientistPassword WRITE setScientistPassword)

        Q_PROPERTY(QString engineerPassword READ getEngineerPassword WRITE setEngineerPassword)

        Q_ENUMS(userTypes)
        Q_PROPERTY(userTypes currentUserType READ getCurrentUserTypeProperty WRITE setCurrentUserTypeProperty)
        enum userTypes
        {
            User = QCaLogin::USER_TYPE,
            Scientist = QCaLogin::SCIENTIST_TYPE,
            Engineer = QCaLogin::ENGINEER_TYPE
        };
        void setCurrentUserTypeProperty(userTypes pUserType)
        {
            setCurrentUserType((userTypes) pUserType);
        }
        userTypes getCurrentUserTypeProperty()
        {
            return (userTypes) getCurrentUserType();
        }

};

#endif /// QCALOGINPLUGIN_H
