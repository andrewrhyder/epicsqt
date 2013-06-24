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
 *  Copyright (c) 2012, 2013
 *
 *  Author:
 *    Ricardo Fernandes, Andrew Rhyder
 *  Contact details:
 *    ricardo.fernandes@synchrotron.org.au
 */

#include <QELogin.h>



// ============================================================
//  QELOGIN METHODS
// ============================================================
QELogin::QELogin(QWidget *pParent):QFrame(pParent), QEWidget( this )
{
    setFrameStyle( QFrame::StyledPanel );
    setFrameShadow( QFrame::Raised );

    qLabelUserType = new QLabel(this);
    loginForm = new loginWidget( this );

    setCompactStyle( true );

    qPushButtonLogin = new QPushButton(this);
    qPushButtonLogout = new QPushButton(this);


    qGridLayout = new QGridLayout(this);
    qGridLayout->addWidget( qLabelUserType, 0, 0, 1, 2 );
    qGridLayout->addWidget( loginForm, 1, 0, 1, 2 );
    qGridLayout->addWidget( qPushButtonLogin, 2, 0);
    qGridLayout->addWidget( qPushButtonLogout, 2, 1);


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

    currentUserType = getUserLevel();
    setCurrentLevelText();
    loginForm->setCurrentUserType( currentUserType );
}

void QELogin::setCurrentLevelText()
{
    qLabelUserType->setText(QString( "Current Level: " ).append( getUserTypeName( currentUserType) ));
}

// Set if the widget is small and pops up a dialog to allow login, or larger and displays the login all the time
void QELogin::setCompactStyle(bool compactStyleIn )
{
    compactStyle = compactStyleIn;

    loginForm->setHidden( compactStyle );
}

bool QELogin::getCompactStyle()
{
    return compactStyle;
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

// Return a name for each user type
QString QELogin::getUserTypeName(userLevelTypes::userLevels type)
{
    switch( type )
    {
        case userLevelTypes::USERLEVEL_USER:      return "User";
        case userLevelTypes::USERLEVEL_SCIENTIST: return "Scientist";
        case userLevelTypes::USERLEVEL_ENGINEER:  return "Engineer";
        default:                                  return "Unknown";
    }
}

void QELogin::buttonLoginClicked()
{
    if( compactStyle )
    {
        _QDialogLogin qELoginDialog( this, currentUserType );
        qELoginDialog.exec();
    }
    else
    {
        login( loginForm->getUserType(), loginForm->getPassword() );
        loginForm->clearPassword();
    }
}

void QELogin::login( userLevelTypes::userLevels level, QString password )
{
    QString requiredPassword;
    switch( level )
    {
        case userLevelTypes::USERLEVEL_USER:      requiredPassword = getPriorityUserPassword();      break;
        case userLevelTypes::USERLEVEL_SCIENTIST: requiredPassword = getPriorityScientistPassword(); break;
        case userLevelTypes::USERLEVEL_ENGINEER:  requiredPassword = getPriorityEngineerPassword();  break;
    }

    if( requiredPassword.isEmpty() || password == requiredPassword )
    {
        if( level != currentUserType )
        {
            loginHistory.push(currentUserType);
            currentUserType = level;
            setCurrentLevelText();
            qPushButtonLogout->setEnabled( true );
            setUserLevel( currentUserType);
        }
    }
    else
    {
        QMessageBox::critical(this, "Error", "The password is invalid. Please try again!");
    }
}

// Logout to the last user type logged into by THIS widget
void QELogin::buttonLogoutClicked()
{
    if( loginHistory.count() )
    {
        sendMessage("The user type was changed from '" + getUserTypeName( currentUserType) + "' to '" + getUserTypeName( loginHistory.top()) + "'");
        currentUserType = loginHistory.pop();
        setCurrentLevelText();
        if( loginHistory.count() == 0 )
        {
            qPushButtonLogout->setEnabled( false );
        }
        setUserLevel( currentUserType);
    }
}

// Return the user level password from the profile if available, otherwise use the local user level password property
QString QELogin::getPriorityUserPassword()
{
    ContainerProfile profile;
    if( profile.areUserLevelPasswordsSet() )
    {
        return profile.getUserLevelPassword( userLevelTypes::USERLEVEL_USER );
    }
    else
    {
        return userPassword;
    }
}

// Return the scientist level password from the profile if available, otherwise use the local scientist level password property
QString QELogin::getPriorityScientistPassword()
{
    ContainerProfile profile;
    if( profile.areUserLevelPasswordsSet() )
    {
        return profile.getUserLevelPassword( userLevelTypes::USERLEVEL_SCIENTIST );
    }
    else
    {
        return scientistPassword;
    }
}

// Return the engineer level password from the profile if available, otherwise use the local engineer level password property
QString QELogin::getPriorityEngineerPassword()
{
    ContainerProfile profile;
    if( profile.areUserLevelPasswordsSet() )
    {
        return profile.getUserLevelPassword( userLevelTypes::USERLEVEL_ENGINEER );
    }
    else
    {
        return engineerPassword;
    }
}


// ============================================================
// loginWidget
// This widget is used in the dialog launched by the compact form of QELogin, and by the larger form of QELogin directly
// ============================================================
loginWidget::loginWidget( QELogin* ownerIn )
{
    owner = ownerIn;

    qGridLayout = new QGridLayout(this);
    qGroupBox = new QGroupBox(this);
    qVBoxLayout = new QVBoxLayout();
    qRadioButtonUser = new QRadioButton();
    qRadioButtonScientist = new QRadioButton(this);
    qRadioButtonEngineer = new QRadioButton(this);
    qLineEditPassword = new QLineEdit(this);

    qGroupBox->setTitle( "Login as:");

    qRadioButtonUser->setText(owner->getUserTypeName(userLevelTypes::USERLEVEL_USER));
    QObject::connect(qRadioButtonUser, SIGNAL(clicked()), this, SLOT(radioButtonClicked()));

    qRadioButtonScientist->setText(owner->getUserTypeName(userLevelTypes::USERLEVEL_SCIENTIST));
    QObject::connect(qRadioButtonScientist, SIGNAL(clicked()), this, SLOT(radioButtonClicked()));

    qRadioButtonEngineer->setText(owner->getUserTypeName(userLevelTypes::USERLEVEL_ENGINEER));
    QObject::connect(qRadioButtonEngineer, SIGNAL(clicked()), this, SLOT(radioButtonClicked()));

    qVBoxLayout->addWidget(qRadioButtonUser);
    qVBoxLayout->addWidget(qRadioButtonScientist);
    qVBoxLayout->addWidget(qRadioButtonEngineer);
    qGroupBox->setLayout(qVBoxLayout);

    qLineEditPassword->setEchoMode(QLineEdit::Password);
    qLineEditPassword->setToolTip("Password for the selected type");

    qGridLayout->addWidget(qGroupBox, 0, 0);
    qGridLayout->addWidget(qLineEditPassword, 1, 0);

    userType = userLevelTypes::USERLEVEL_USER;
    qRadioButtonUser->setChecked(true);
    radioButtonClicked();
}

void loginWidget::setCurrentUserType(userLevelTypes::userLevels pValue)
{
    userType = pValue;
    switch( userType )
    {
        case userLevelTypes::USERLEVEL_USER:      qRadioButtonUser     ->setChecked(true); break;
        case userLevelTypes::USERLEVEL_SCIENTIST: qRadioButtonScientist->setChecked(true); break;
        case userLevelTypes::USERLEVEL_ENGINEER:  qRadioButtonScientist->setChecked(true); break;
    }

}

void loginWidget::setPassword(QString pValue)
{
    qLineEditPassword->setText(pValue);
}

void loginWidget::radioButtonClicked()
{
    // Enable password entry if a password is required
    if (qRadioButtonUser->isChecked())
    {
        qLineEditPassword->setEnabled(owner->getPriorityUserPassword().isEmpty() == false);
    }
    else if (qRadioButtonScientist->isChecked())
    {
        qLineEditPassword->setEnabled(owner->getPriorityScientistPassword().isEmpty() == false);
    }
    else
    {
        qLineEditPassword->setEnabled(owner->getPriorityEngineerPassword().isEmpty() == false);
    }
}

userLevelTypes::userLevels loginWidget::getUserType()
{
    if( qRadioButtonUser->isChecked() )
        return userLevelTypes::USERLEVEL_USER;

    if( qRadioButtonScientist->isChecked() )
        return userLevelTypes::USERLEVEL_SCIENTIST;

    if( qRadioButtonEngineer->isChecked() )
        return userLevelTypes::USERLEVEL_ENGINEER;

    // Default
    return userLevelTypes::USERLEVEL_USER;
}

QString loginWidget::getPassword()
{
    return qLineEditPassword->text();
}

void loginWidget::clearPassword()
{
    return qLineEditPassword->clear();
}

// ============================================================
//  _QDIALOGLOGIN METHODS
// ============================================================
_QDialogLogin::_QDialogLogin(QELogin* ownerIn, userLevelTypes::userLevels pUserType )
{
    owner = ownerIn;
    setWindowTitle("Login/Logout");

    loginForm = new loginWidget(owner);

    qGridLayout = new QGridLayout(this);

    qPushButtonOk = new QPushButton(this);
    qPushButtonCancel = new QPushButton(this);

    qPushButtonOk->setText("Ok");
    qPushButtonOk->setToolTip("Perform login");
    QObject::connect(qPushButtonOk, SIGNAL(clicked()), this, SLOT(buttonOkClicked()));

    qPushButtonCancel->setText("Cancel");
    qPushButtonCancel->setToolTip("Cancel login");
    QObject::connect(qPushButtonCancel, SIGNAL(clicked()), this, SLOT(buttonCancelClicked()));

    qGridLayout->addWidget( loginForm, 0, 0, 1, 2 );
    qGridLayout->addWidget( qPushButtonCancel, 1, 0);
    qGridLayout->addWidget( qPushButtonOk, 1, 1);

    loginForm->setCurrentUserType( pUserType );
}

void _QDialogLogin::setCurrentUserType(userLevelTypes::userLevels pValue)
{
    loginForm->setCurrentUserType( pValue );
}

void _QDialogLogin::setPassword(QString pValue)
{
    loginForm->setPassword( pValue );
}

void _QDialogLogin::buttonOkClicked()
{
    owner->login( loginForm->getUserType(), loginForm->getPassword() );
    close();
}

void _QDialogLogin::buttonCancelClicked()
{
    close();
}
