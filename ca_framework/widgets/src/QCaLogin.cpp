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
#include <QGroupBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QCaLogin.h>
#include <QDebug>


QCaLogin::QCaLogin(QWidget *pParent):QWidget(pParent)
{

    qCaLoginDialog = new QCaLoginDialog(this);
    qGridLayout = new QGridLayout(this);
    qLabelUserType = new QLabel(this);
    qPushButtonLogin = new QPushButton(this);

    qCaLoginDialog->setWindowTitle("Login");

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

    qDebug() << "setCurrentUserType() = " << currentUserType;

}



int QCaLogin::getCurrentUserType()
{

    qDebug() << "getCurrentUserType() = " << currentUserType;

    return currentUserType;

}



void QCaLogin::buttonLoginClicked()
{

    if (qCaLoginDialog->isVisible() == false)
    {
        qCaLoginDialog->setCurrentUserType(getCurrentUserType());
        qCaLoginDialog->setPassword("");
    }
    qCaLoginDialog->show();
    qCaLoginDialog->activateWindow();

}





QCaLoginDialog::QCaLoginDialog(QWidget *pParent, Qt::WindowFlags pF):QDialog(pParent, pF)
{

    qGridLayout = new QGridLayout(this);
    qGroupBox = new QGroupBox(this);
    qVBoxLayout = new QVBoxLayout();
    qRadioButtonUser = new QRadioButton();
    qRadioButtonScientist = new QRadioButton(this);
    qRadioButtonEngineer = new QRadioButton(this);
    qLabelType = new QLabel(this);
    qLineEditPassword = new QLineEdit(this);
    qPushButtonOk = new QPushButton(this);
    qPushButtonCancel = new QPushButton(this);

    qLabelType->setText("Type:");

    qRadioButtonUser->setText("User");
    QObject::connect(qRadioButtonUser, SIGNAL(clicked()), this, SLOT(radioButtonClicked()));

    qRadioButtonScientist->setText("Scientist");
    QObject::connect(qRadioButtonScientist, SIGNAL(clicked()), this, SLOT(radioButtonClicked()));

    qRadioButtonEngineer->setText("Engineer");
    QObject::connect(qRadioButtonEngineer, SIGNAL(clicked()), this, SLOT(radioButtonClicked()));


    qVBoxLayout->addWidget(qRadioButtonUser);
    qVBoxLayout->addWidget(qRadioButtonScientist);
    qVBoxLayout->addWidget(qRadioButtonEngineer);
    qGroupBox->setLayout(qVBoxLayout);

    qLineEditPassword->setEchoMode(QLineEdit::Password);
    qLineEditPassword->setToolTip("Password for the selected type");

    qPushButtonOk->setText("Ok");
    qPushButtonOk->setToolTip("Perform login");
    QObject::connect(qPushButtonOk, SIGNAL(clicked()), this, SLOT(buttonOkClicked()));

    qPushButtonCancel->setText("Cancel");
    qPushButtonCancel->setToolTip("Cancel login");
    QObject::connect(qPushButtonCancel, SIGNAL(clicked()), this, SLOT(buttonCancelClicked()));

    qGridLayout->addWidget(qLabelType, 0, 0);
    qGridLayout->addWidget(qGroupBox, 0, 1);
    qGridLayout->addWidget(qLineEditPassword, 1, 0, 1, 2);
    qGridLayout->addWidget(qPushButtonCancel, 2, 0);
    qGridLayout->addWidget(qPushButtonOk, 2, 1);

}




void QCaLoginDialog::setCurrentUserType(int pValue)
{

    switch(pValue)
    {
        case QCaLogin::USER_TYPE:
            qRadioButtonUser->setChecked(true);
            break;

        case QCaLogin::SCIENTIST_TYPE:
            qRadioButtonScientist->setChecked(true);
            break;

        default:
            qRadioButtonEngineer->setChecked(true);
    }

}




void QCaLoginDialog::setPassword(QString pValue)
{

    qLineEditPassword->setText(pValue);

}




void QCaLoginDialog::radioButtonClicked()
{

    QCaLogin *parent;

    parent = (QCaLogin *) this->parent();

    if (qRadioButtonUser->isChecked())
    {
        qLineEditPassword->setEnabled(parent->getUserPassword() != "");
    }
    else
    {
        if (qRadioButtonScientist->isChecked())
        {
            qLineEditPassword->setEnabled(parent->getScientistPassword() != "");
        }
        else
        {
            qLineEditPassword->setEnabled(parent->getEngineerPassword() != "");
        }
    }


}



void QCaLoginDialog::buttonOkClicked()
{

    QCaLogin *parent;
    int type;


    parent = (QCaLogin *) this->parent();
    type = -1;

    if (qRadioButtonUser->isChecked())
    {
        if (parent->getUserPassword() == qLineEditPassword->text())
        {
            type = QCaLogin::USER_TYPE;
        }
    }
    else
    {
        if (qRadioButtonScientist->isChecked())
        {
            if (parent->getScientistPassword() == qLineEditPassword->text())
            {
                type = QCaLogin::SCIENTIST_TYPE;
            }
        }
        else
        {
            if (parent->getEngineerPassword() == qLineEditPassword->text())
            {
                type = QCaLogin::ENGINEER_TYPE;
            }
        }
    }


    if (type == -1)
    {
        QMessageBox::critical(this, "Error", "The password is invalid. Please try again!");
    }
    else
    {
        parent->setCurrentUserType(type);
        this->hide();
    }

}



void QCaLoginDialog::buttonCancelClicked()
{

    this->hide();

}

