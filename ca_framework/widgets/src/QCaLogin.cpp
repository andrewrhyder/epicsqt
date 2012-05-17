/*  This file is part of the EPICS QT Framework, initially developed at
 *  the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
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

#include <QMessageBox>
#include <QDialog>
#include <QCaLogin.h>


QCaLogin::QCaLogin(QWidget *pParent):QWidget(pParent)
{

    qCaLoginDialog = new QCaLoginDialog(this);
    qGridLayout = new QGridLayout(this);
    qLabelUserType = new QLabel(this);
    qPushButtonLogin = new QPushButton(this);

    qLabelUserType->setToolTip("Current user");

    qPushButtonLogin->setText("Login");
    qPushButtonLogin->setToolTip("Change user");
    QObject::connect(qPushButtonLogin, SIGNAL(clicked()), this, SLOT(buttonLoginClicked()));


    qGridLayout->addWidget(qLabelUserType, 0, 0);
    qGridLayout->addWidget(qPushButtonLogin, 0, 1);

    setUserPassword("");
    setScientistPassword("");
    setEngineerPassword("");
    setCurrentUserType(USER_TYPE);

}




void QCaLogin::setShowButtonLogin(bool pValue)
{

    qPushButtonLogin->setVisible(pValue);

}



bool QCaLogin::getShowButtonLogin()
{

    return qPushButtonLogin->isVisible();

}



void QCaLogin::setUserPassword(QString pValue)
{

    userPassword = pValue;

}



QString QCaLogin::getUserPassword()
{

    return userPassword;

}


void QCaLogin::setScientistPassword(QString pValue)
{

    scientistPassword = pValue;

}


QString QCaLogin::getScientistPassword()
{

    return scientistPassword;

}


void QCaLogin::setEngineerPassword(QString pValue)
{

    engineerPassword = pValue;

}



QString QCaLogin::getEngineerPassword()
{

    return engineerPassword;

}



void QCaLogin::setCurrentUserType(int pValue)
{

    switch(pValue)
    {
        case USER_TYPE:
            currentUserType = USER_TYPE;
            qLabelUserType->setText("User");
            // should update slot to notify other widgets that the user type has changed
            break;

        case SCIENTIST_TYPE:
            currentUserType = SCIENTIST_TYPE;
            qLabelUserType->setText("Scientist");
            // should update slot to notify other widgets that the user type has changed
            break;

        case ENGINEER_TYPE:
            currentUserType = ENGINEER_TYPE;
            qLabelUserType->setText("Engineer");
            // should update slot to notify other widgets that the user type has changed
    }

}



int QCaLogin::getCurrentUserType()
{

    return currentUserType;

}



void QCaLogin::buttonLoginClicked()
{

    qCaLoginDialog->show();
    qCaLoginDialog->activateWindow();

}





QCaLoginDialog::QCaLoginDialog(QWidget * pParent, Qt::WindowFlags pF):QDialog(pParent, pF)
{

//    QLabel *qLabelUserType;

//    qLabelUserType = new QLabel(this);

}


