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
#include <ContainerProfile.h>
#include <QDebug>


QCaLogin::QCaLogin(QWidget *pParent):QWidget(pParent)
{

    qCaLoginDialog = NULL;
    qLayout = NULL;

    qLabelUserType = new QLabel(this);
    qPushButtonLogin = new QPushButton(this);

    qLabelUserType->setToolTip("Current user");

    qPushButtonLogin->setText("Login");
    qPushButtonLogin->setToolTip("Change user");
    QObject::connect(qPushButtonLogin, SIGNAL(clicked()), this, SLOT(buttonLoginClicked()));

    setUserPassword("");
    setScientistPassword("");
    setEngineerPassword("");
    setCurrentUserType(USERLEVEL_USER);
    setDetailsLayout(RIGHT);

}




void QCaLogin::setShowLabelUserType(bool pValue)
{

    qLabelUserType->setVisible(pValue);

}



bool QCaLogin::getShowLabelUserType()
{

    return qLabelUserType->isVisible();

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



int QCaLogin::getCurrentUserType()
{

//    qDebug() << "getCurrentUserType() = " << currentUserType;

    return currentUserType;

}




void QCaLogin::setDetailsLayout(int pValue)
{

    switch(pValue)
    {
        case TOP:
            if (qLayout)
            {
                delete qLayout;
            }
            detailsLayout = TOP;
            qLayout = new QVBoxLayout(this);
            qLayout->setAlignment(Qt::AlignCenter);
            qLayout->addWidget(qPushButtonLogin);
            qLayout->addWidget(qLabelUserType);
            break;

        case BOTTOM:
            if (qLayout)
            {
                delete qLayout;
            }
            detailsLayout = BOTTOM;
            qLayout = new QVBoxLayout(this);
            qLayout->setAlignment(Qt::AlignCenter);
            qLayout->addWidget(qLabelUserType);
            qLayout->addWidget(qPushButtonLogin);
            break;

        case LEFT:
            if (qLayout)
            {
                delete qLayout;
            }
            detailsLayout = LEFT;
            qLayout = new QHBoxLayout(this);
            qLayout->setAlignment(Qt::AlignCenter);
            qLayout->addWidget(qPushButtonLogin);
            qLayout->addWidget(qLabelUserType);
            break;

        case RIGHT:
            if (qLayout)
            {
                delete qLayout;
            }
            detailsLayout = RIGHT;
            qLayout = new QHBoxLayout(this);
            qLayout->setAlignment(Qt::AlignCenter);
            qLayout->addWidget(qLabelUserType);
            qLayout->addWidget(qPushButtonLogin);
    }

//    qDebug() << "setDetailsLayout() = " << detailsLayout;

}



int QCaLogin::getDetailsLayout()
{

//    qDebug() << "getDetailsLayout() = " << detailsLayout;

    return detailsLayout;

}




void QCaLogin::buttonLoginClicked()
{

    if (qCaLoginDialog)
    {
        if (qCaLoginDialog->isVisible())
        {
            qCaLoginDialog->activateWindow();
        }
        else
        {
            qCaLoginDialog = new QCaLoginDialog(this);
        }
    }
    else
    {
        qCaLoginDialog = new QCaLoginDialog(this);
    }
    qCaLoginDialog->show();

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
    QObject::connect(qLineEditPassword, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditPasswordTextChanged(QString)));

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


    switch(((QCaLogin *) this->parent())->getCurrentUserType())
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




void QCaLoginDialog::setCurrentUserType(int pValue)
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
        qLineEditPassword->setEnabled(parent->getUserPassword().isEmpty() == false);
    }
    else
    {
        if (qRadioButtonScientist->isChecked())
        {
            qLineEditPassword->setEnabled(parent->getScientistPassword().isEmpty() == false);
        }
        else
        {
            qLineEditPassword->setEnabled(parent->getEngineerPassword().isEmpty() == false);
        }
    }
    qPushButtonOk->setEnabled(qLineEditPassword->isEnabled() == false || qLineEditPassword->text().isEmpty() == false);

}





void QCaLoginDialog::lineEditPasswordTextChanged(QString pValue)
{

//    qDebug() << "inside lineEditTextChanged";

    qPushButtonOk->setEnabled(pValue.isEmpty() == false);

}






void QCaLoginDialog::buttonOkClicked()
{

    QCaLogin *parent;
    int type;


    parent = (QCaLogin *) this->parent();

    type = -1;

    if (qRadioButtonUser->isChecked())
    {
        if (qLineEditPassword->isEnabled() == false || parent->getUserPassword() == qLineEditPassword->text())
        {
            type = USERLEVEL_USER;
        }
    }
    else
    {
        if (qRadioButtonScientist->isChecked())
        {
            if (qLineEditPassword->isEnabled() == false || parent->getScientistPassword() == qLineEditPassword->text())
            {
                type = USERLEVEL_SCIENTIST;
            }
        }
        else
        {
            if (qLineEditPassword->isEnabled() == false || parent->getEngineerPassword() == qLineEditPassword->text())
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



void QCaLoginDialog::buttonCancelClicked()
{

    this->close();

}

