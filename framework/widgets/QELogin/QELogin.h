/*  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
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

#ifndef QELOGIN_H
#define QELOGIN_H

#include <QEWidget.h>


class QELogin;

// ============================================================
//  loginWidget CLASS
// ============================================================
class loginWidget:public QFrame
{

    Q_OBJECT

    private:
        QELogin* owner;

    protected:
        QGridLayout *qGridLayout;
        QVBoxLayout *qVBoxLayout;
        QGroupBox *qGroupBox;
        QRadioButton *qRadioButtonUser;
        QRadioButton *qRadioButtonScientist;
        QRadioButton *qRadioButtonEngineer;
        QLineEdit *qLineEditPassword;
        userLevelTypes::userLevels userType;


    public:
        loginWidget(QELogin* ownerIn );
        void setCurrentUserType(userLevelTypes::userLevels pValue);
        void setPassword(QString pValue);
        userLevelTypes::userLevels getUserType();
        QString getPassword();
        void clearPassword();


    private slots:
        void radioButtonClicked();
};


// ============================================================
//  _QDIALOGLOGIN CLASS
// ============================================================
class _QDialogLogin:public QDialog
{

    Q_OBJECT

    private:
        QELogin* owner;


    protected:
        QGridLayout *qGridLayout;

        loginWidget *loginForm;
        QPushButton *qPushButtonOk;
        QPushButton *qPushButtonCancel;


    public:
        _QDialogLogin(QELogin* ownerIn, userLevelTypes::userLevels pUserType);
        void setCurrentUserType(userLevelTypes::userLevels pValue);
        void setPassword(QString pValue);


    private slots:
        void buttonOkClicked();

        void buttonCancelClicked();

};





// ============================================================
//  QELOGIN CLASS
// ============================================================
class QEPLUGINLIBRARYSHARED_EXPORT QELogin:public QFrame, public QEWidget
{

    Q_OBJECT

    private:
        void setCurrentLevelText();

    protected:
        QGridLayout *qGridLayout;
        QStack<userLevelTypes::userLevels> loginHistory;
        QPushButton *qPushButtonLogin;
        QPushButton *qPushButtonLogout;
        QLabel *qLabelUserType;
        loginWidget *loginForm;
        QString userPassword;
        QString scientistPassword;
        QString engineerPassword;
        userLevelTypes::userLevels currentUserType;
        bool compactStyle;

    public:

        void login( userLevelTypes::userLevels level, QString password );

        QString getPriorityUserPassword();
        QString getPriorityScientistPassword();
        QString getPriorityEngineerPassword();

        QELogin(QWidget *pParent = 0);
        virtual ~QELogin(){}

        void setUserPassword(QString pValue);
        QString getUserPassword();

        void setScientistPassword(QString pValue);
        QString getScientistPassword();

        void setEngineerPassword(QString pValue);
        QString getEngineerPassword();

        void setCurrentUserType(userLevelTypes::userLevels pValue);
        userLevelTypes::userLevels getCurrentUserType();

        void setCompactStyle(bool compactStyle );
        bool getCompactStyle();

        QString getUserTypeName( userLevelTypes::userLevels type );

        Q_PROPERTY( bool compactStyle READ getCompactStyle WRITE setCompactStyle )

        Q_PROPERTY(QString userPassword READ getUserPassword WRITE setUserPassword)

        Q_PROPERTY(QString scientistPassword READ getScientistPassword WRITE setScientistPassword)

        Q_PROPERTY(QString engineerPassword READ getEngineerPassword WRITE setEngineerPassword)

    private slots:
        void buttonLoginClicked();

        void buttonLogoutClicked();


};



#endif // QELOGIN_H


