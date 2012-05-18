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
#include <QCaLog.h>
#include <ContainerProfile.h>
#include <QDebug>


QCaLog::QCaLog(QWidget *pParent):QWidget(pParent)
{

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
    setCurrentUserType(USERLEVEL_USER);

}




void QCaLog::setShowButtonLogin(bool pValue)
{

    qPushButtonLogin->setVisible(pValue);

}



bool QCaLog::getShowButtonLogin()
{

    return qPushButtonLogin->isVisible();

}



void QCaLog::setUserPassword(QString pValue)
{

    userPassword = pValue;

}



QString QCaLog::getUserPassword()
{

    return userPassword;

}


void QCaLog::setScientistPassword(QString pValue)
{

    scientistPassword = pValue;

}


QString QCaLog::getScientistPassword()
{

    return scientistPassword;

}


void QCaLog::setEngineerPassword(QString pValue)
{

    engineerPassword = pValue;

}



QString QCaLog::getEngineerPassword()
{

    return engineerPassword;

}



void QCaLog::setCurrentUserType(int pValue)
{

    switch(pValue)
    {
        case USERLEVEL_USER:
            qLabelUserType->setText("User");
            currentUserType = USERLEVEL_USER;
            setUserLevel((userLevels) currentUserType);
            break;

        case USERLEVEL_SCIENTIST:
            qLabelUserType->setText("Scientist");
            currentUserType = USERLEVEL_SCIENTIST;
            setUserLevel((userLevels) currentUserType);
            break;

        case USERLEVEL_ENGINEER:
            qLabelUserType->setText("Engineer");
            currentUserType = USERLEVEL_ENGINEER;
            setUserLevel((userLevels) currentUserType);
    }

//    qDebug() << "setCurrentUserType() = " << currentUserType;

}



int QCaLog::getCurrentUserType()
{

//    qDebug() << "getCurrentUserType() = " << currentUserType;

    return currentUserType;

}



void QCaLog::buttonLoginClicked()
{

    if (qCaLogDialog)
    {
        if (qCaLogDialog->isVisible())
        {
            qCaLogDialog->activateWindow();
        }
        else
        {
            qCaLogDialog = new QCaLogDialog(this);
        }
    }
    else
    {
        qCaLogDialog = new QCaLogDialog(this);
    }
    qCaLogDialog->show();

}





QCaLogDialog::QCaLogDialog(QWidget *pParent, Qt::WindowFlags pF):QDialog(pParent, pF)
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

    setWindowTitle("Login");

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


    switch(((QCaLog *) this->parent())->getCurrentUserType())
    {
        case USERLEVEL_USER:
            qRadioButtonUser->setFocus();
            qRadioButtonUser->setChecked(true);
            radioButtonClicked();
            break;

        case USERLEVEL_SCIENTIST:
            qRadioButtonScientist->setFocus();
            qRadioButtonScientist->setChecked(true);
            radioButtonClicked();
            break;

        case USERLEVEL_ENGINEER:
            qRadioButtonEngineer->setFocus();
            qRadioButtonEngineer->setChecked(true);
            radioButtonClicked();
    }


}




void QCaLogDialog::setCurrentUserType(int pValue)
{

    switch(pValue)
    {
        case USERLEVEL_USER:
            qRadioButtonUser->setChecked(true);
            break;

        case USERLEVEL_SCIENTIST:
            qRadioButtonScientist->setChecked(true);
            break;

        default:
            qRadioButtonEngineer->setChecked(true);
    }

}




void QCaLogDialog::setPassword(QString pValue)
{

    qLineEditPassword->setText(pValue);

}




void QCaLogDialog::radioButtonClicked()
{

    QCaLog *parent;

    parent = (QCaLog *) this->parent();

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



void QCaLogDialog::buttonOkClicked()
{

    QCaLog *parent;
    int type;


    parent = (QCaLog *) this->parent();
    type = -1;

    if (qRadioButtonUser->isChecked())
    {
        if (parent->getUserPassword() == qLineEditPassword->text())
        {
            type = USERLEVEL_USER;
        }
    }
    else
    {
        if (qRadioButtonScientist->isChecked())
        {
            if (parent->getScientistPassword() == qLineEditPassword->text())
            {
                type = USERLEVEL_SCIENTIST;
            }
        }
        else
        {
            if (parent->getEngineerPassword() == qLineEditPassword->text())
            {
                type = USERLEVEL_ENGINEER;
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
        this->close();
    }

}



void QCaLogDialog::buttonCancelClicked()
{

    this->close();

}

