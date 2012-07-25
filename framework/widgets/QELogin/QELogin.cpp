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

#include <QELogin.h>



// ============================================================
//  QELOGIN METHODS
// ============================================================
QELogin::QELogin(QWidget *pParent):QWidget(pParent), QCaWidget( this )
{

    qLabelUserType = new QLabel(this);
    qPushButtonLogin = new QPushButton(this);
    qPushButtonLogout = new QPushButton(this);

    qLabelUserType->setToolTip("Current user");

    qPushButtonLogin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    qPushButtonLogin->setText("Login");
    qPushButtonLogin->setToolTip("Change current user");
    QObject::connect(qPushButtonLogin, SIGNAL(clicked()), this, SLOT(buttonLoginClicked()));

    qPushButtonLogout->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    qPushButtonLogout->setText("Logout");
    qPushButtonLogout->setToolTip("Logout current user");
    qPushButtonLogout->setEnabled(false);
    QObject::connect(qPushButtonLogout, SIGNAL(clicked()), this, SLOT(buttonLogoutClicked()));

    currentUserType = USERLEVEL_USER;
    qLabelUserType->setText(getUserTypeName((userLevels) currentUserType));
    setUserLevel((userLevels) currentUserType);

    setUserPassword("");
    setScientistPassword("");
    setEngineerPassword("");
    setDetailsLayout(RIGHT);

}




void QELogin::setShowUserType(bool pValue)
{

    qLabelUserType->setVisible(pValue);

}



bool QELogin::getShowUserType()
{

    return qLabelUserType->isVisible();

}




void QELogin::setShowLogin(bool pValue)
{

    qPushButtonLogin->setVisible(pValue);

}



bool QELogin::getShowButtonLogin()
{

    return qPushButtonLogin->isVisible();

}




void QELogin::setShowLogout(bool pValue)
{

    qPushButtonLogout->setVisible(pValue);

}



bool QELogin::getShowButtonLogout()
{

    return qPushButtonLogout->isVisible();

}




void QELogin::setUserPassword(QString pValue)
{

    userPassword = pValue;

}



QString QELogin::getUserPassword()
{

    return userPassword;

}


void QELogin::setScientistPassword(QString pValue)
{

    scientistPassword = pValue;

}


QString QELogin::getScientistPassword()
{

    return scientistPassword;

}


void QELogin::setEngineerPassword(QString pValue)
{

    engineerPassword = pValue;

}



QString QELogin::getEngineerPassword()
{

    return engineerPassword;

}



void QELogin::setCurrentUserType(int pValue)
{

    switch(pValue)
    {
        case USERLEVEL_USER:
            sendMessage("The user type was changed from '" + getUserTypeName((userLevels) currentUserType) + "' to '" + getUserTypeName((userLevels) pValue) + "'");
            loginHistory.push(currentUserType);
            qPushButtonLogout->setEnabled(true);
            qLabelUserType->setText(getUserTypeName((userLevels) pValue));
            currentUserType = USERLEVEL_USER;
            setUserLevel((userLevels) currentUserType);
            break;

        case USERLEVEL_SCIENTIST:
            sendMessage("The user type was changed from '" + getUserTypeName((userLevels) currentUserType) + "' to '" + getUserTypeName((userLevels) pValue) + "'");
            loginHistory.push(currentUserType);
            qPushButtonLogout->setEnabled(true);
            qLabelUserType->setText(getUserTypeName((userLevels) pValue));
            currentUserType = USERLEVEL_SCIENTIST;
            setUserLevel((userLevels) currentUserType);
            break;

        case USERLEVEL_ENGINEER:
            sendMessage("The user type was changed from '" + getUserTypeName((userLevels) currentUserType) + "' to '" + getUserTypeName((userLevels) pValue) + "'");
            loginHistory.push(currentUserType);
            qPushButtonLogout->setEnabled(true);
            qLabelUserType->setText(getUserTypeName((userLevels) pValue));
            currentUserType = USERLEVEL_ENGINEER;
            setUserLevel((userLevels) currentUserType);
    }

}




QString QELogin::getUserTypeName(userLevels type)
{
    switch( type )
    {
        case USERLEVEL_USER:      return "User";
        case USERLEVEL_SCIENTIST: return "Scientist";
        case USERLEVEL_ENGINEER:  return "Engineer";
        default:                  return "Unknown";
    }
}



int QELogin::getCurrentUserType()
{

    return currentUserType;

}




void QELogin::setDetailsLayout(int pValue)
{

    QLayout *qLayoutMain;


    delete layout();

    switch(pValue)
    {
        case TOP:
            detailsLayout = TOP;
            qLayoutMain = new QVBoxLayout(this);
            qLayoutMain->setAlignment(Qt::AlignCenter);
            qLayoutMain->addWidget(qPushButtonLogin);
            qLayoutMain->addWidget(qPushButtonLogout);
            qLayoutMain->addWidget(qLabelUserType);
            break;

        case BOTTOM:
            detailsLayout = BOTTOM;
            qLayoutMain = new QVBoxLayout(this);
            qLayoutMain->setAlignment(Qt::AlignCenter);
            qLayoutMain->addWidget(qLabelUserType);
            qLayoutMain->addWidget(qPushButtonLogin);
            qLayoutMain->addWidget(qPushButtonLogout);
            break;

        case LEFT:
            detailsLayout = LEFT;
            qLayoutMain = new QHBoxLayout(this);
            qLayoutMain->setAlignment(Qt::AlignCenter);
            qLayoutMain->addWidget(qPushButtonLogin);
            qLayoutMain->addWidget(qPushButtonLogout);
            qLayoutMain->addWidget(qLabelUserType);
            break;

        case RIGHT:
            detailsLayout = RIGHT;
            qLayoutMain = new QHBoxLayout(this);
            qLayoutMain->setAlignment(Qt::AlignCenter);
            qLayoutMain->addWidget(qLabelUserType);
            qLayoutMain->addWidget(qPushButtonLogin);
            qLayoutMain->addWidget(qPushButtonLogout);
    }


}



int QELogin::getDetailsLayout()
{

    return detailsLayout;

}




void QELogin::buttonLoginClicked()
{

    _QDialogLogin *qELoginDialog;

    qELoginDialog = new _QDialogLogin(this);
    qELoginDialog->exec();

}




void QELogin::buttonLogoutClicked()
{

    _QDialogLogin *qELoginDialog;
    int logoutUserType;


    logoutUserType = loginHistory.top();

    if (logoutUserType > currentUserType)
    {
        if (logoutUserType == USERLEVEL_USER)
        {
            if (userPassword.isEmpty())
            {
                logoutCurrentUserType();
            }
            else
            {
                qELoginDialog = new _QDialogLogin(this, USERLEVEL_USER);
                qELoginDialog->exec();
            }
        }
        else if (logoutUserType == USERLEVEL_SCIENTIST)
        {
            if (scientistPassword.isEmpty())
            {
                logoutCurrentUserType();
            }
            else
            {
                qELoginDialog = new _QDialogLogin(this, USERLEVEL_SCIENTIST);
                qELoginDialog->exec();
            }
        }
        else
        {
            if (engineerPassword.isEmpty())
            {
                logoutCurrentUserType();
            }
            else
            {
                qELoginDialog = new _QDialogLogin(this, USERLEVEL_ENGINEER);
                qELoginDialog->exec();
            }
        }

    }
    else
    {
        logoutCurrentUserType();
    }

}


void QELogin::logoutCurrentUserType()
{

    sendMessage("The user type was changed from '" + getUserTypeName((userLevels) currentUserType) + "' to '" + getUserTypeName((userLevels) loginHistory.top()) + "'");

    currentUserType = (userLevels) loginHistory.top();
    qLabelUserType->setText(getUserTypeName((userLevels) currentUserType));
    setUserLevel((userLevels) currentUserType);

    loginHistory.pop();
    qPushButtonLogout->setEnabled(loginHistory.empty() == false);

}





// ============================================================
//  _QDIALOGLOGIN METHODS
// ============================================================
_QDialogLogin::_QDialogLogin(QWidget *pParent, int pUserType, Qt::WindowFlags pF):QDialog(pParent, pF)
{

    QELogin *parent;


    userType = pUserType;
    parent = (QELogin *) this->parent();

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

    qRadioButtonUser->setText(parent->getUserTypeName(USERLEVEL_USER));
    QObject::connect(qRadioButtonUser, SIGNAL(clicked()), this, SLOT(radioButtonClicked()));

    qRadioButtonScientist->setText(parent->getUserTypeName(USERLEVEL_SCIENTIST));
    QObject::connect(qRadioButtonScientist, SIGNAL(clicked()), this, SLOT(radioButtonClicked()));

    qRadioButtonEngineer->setText(parent->getUserTypeName(USERLEVEL_ENGINEER));
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

    if (userType == -1)
    {
        setWindowTitle("Login");
        qRadioButtonUser->setFocus();
        qRadioButtonUser->setChecked(true);
    }
    else
    {
        setWindowTitle("Logout");
        switch(userType)
        {
            case USERLEVEL_USER:
                qRadioButtonUser->setChecked(true);
                qRadioButtonScientist->setEnabled(false);
                qRadioButtonEngineer->setEnabled(false);
                break;

            case USERLEVEL_SCIENTIST:
                qRadioButtonScientist->setChecked(true);
                qRadioButtonUser->setEnabled(false);
                qRadioButtonEngineer->setEnabled(false);
                break;

            case USERLEVEL_ENGINEER:
                qRadioButtonEngineer->setChecked(true);
                qRadioButtonUser->setEnabled(false);
                qRadioButtonScientist->setEnabled(false);
        }
    }
    radioButtonClicked();

}




void _QDialogLogin::setCurrentUserType(int pValue)
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




void _QDialogLogin::setPassword(QString pValue)
{

    qLineEditPassword->setText(pValue);

}




void _QDialogLogin::radioButtonClicked()
{

    QELogin *parent;

    parent = (QELogin *) this->parent();

    if (qRadioButtonUser->isChecked())
    {
        qLineEditPassword->setEnabled(parent->getUserPassword().isEmpty() == false);
    }
    else if (qRadioButtonScientist->isChecked())
    {
        qLineEditPassword->setEnabled(parent->getScientistPassword().isEmpty() == false);
    }
    else
    {
        qLineEditPassword->setEnabled(parent->getEngineerPassword().isEmpty() == false);
    }

    qPushButtonOk->setEnabled(qLineEditPassword->isEnabled() == false || qLineEditPassword->text().isEmpty() == false);

}





void _QDialogLogin::lineEditPasswordTextChanged(QString pValue)
{

    qPushButtonOk->setEnabled(pValue.isEmpty() == false);

}






void _QDialogLogin::buttonOkClicked()
{

    QELogin *parent;
    int type;


    parent = (QELogin *) this->parent();

    type = -1;

    if (qRadioButtonUser->isChecked())
    {
        if (qLineEditPassword->isEnabled() == false || parent->getUserPassword() == qLineEditPassword->text())
        {
            type = USERLEVEL_USER;
        }
    }
    else if (qRadioButtonScientist->isChecked())
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

    if (type == -1)
    {
        QMessageBox::critical(this, "Error", "The password is invalid. Please try again!");
    }
    else
    {
        if (userType == -1)
        {
            parent->setCurrentUserType(type);
        }
        else
        {
            parent->logoutCurrentUserType();
        }
        this->close();
    }

}



void _QDialogLogin::buttonCancelClicked()
{

    this->close();

}



