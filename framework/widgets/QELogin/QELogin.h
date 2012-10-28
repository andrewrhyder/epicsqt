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

#include <QCaWidget.h>


enum details
{
    TOP,
    BOTTOM,
    LEFT,
    RIGHT
};




// ============================================================
//  _QDIALOGLOGIN CLASS
// ============================================================
class _QDialogLogin:public QDialog
{

    Q_OBJECT

    private:


    protected:
        QGridLayout *qGridLayout;
        QVBoxLayout *qVBoxLayout;
        QGroupBox *qGroupBox;
        QRadioButton *qRadioButtonUser;
        QRadioButton *qRadioButtonScientist;
        QRadioButton *qRadioButtonEngineer;
        QLabel *qLabelType;
        QLineEdit *qLineEditPassword;
        QPushButton *qPushButtonOk;
        QPushButton *qPushButtonCancel;
        int userType;


    public:
        _QDialogLogin(QWidget * pParent = 0, int pUserType = -1, Qt::WindowFlags pF = 0);
        void setCurrentUserType(int pValue);
        void setPassword(QString pValue);


    private slots:
        void radioButtonClicked();

        void lineEditPasswordTextChanged(QString pValue);

        void buttonOkClicked();

        void buttonCancelClicked();

};





// ============================================================
//  QELOGIN CLASS
// ============================================================
class QCAPLUGINLIBRARYSHARED_EXPORT QELogin:public QWidget, public QCaWidget
{

    Q_OBJECT

    private:



    protected:
        QStack<int> loginHistory;
        QPushButton *qPushButtonLogin;
        QPushButton *qPushButtonLogout;
        QLabel *qLabelUserType;
        QString userPassword;
        QString scientistPassword;
        QString engineerPassword;
        int currentUserType;
        int detailsLayout;


    public:

        QELogin(QWidget *pParent = 0);
        virtual ~QELogin(){}

        void setShowUserType(bool pValue);
        bool getShowUserType();

        void setShowLogin(bool pValue);
        bool getShowButtonLogin();

        void setShowLogout(bool pValue);
        bool getShowButtonLogout();

        void setUserPassword(QString pValue);
        QString getUserPassword();

        void setScientistPassword(QString pValue);
        QString getScientistPassword();

        void setEngineerPassword(QString pValue);
        QString getEngineerPassword();

        void setCurrentUserType(int pValue);
        int getCurrentUserType();

        void setDetailsLayout(int pValue);
        int getDetailsLayout();

        QString getUserTypeName( userLevels type );

        void logoutCurrentUserType();

        Q_PROPERTY(bool showUserType READ getShowUserType WRITE setShowUserType)

        Q_PROPERTY(bool showLogin READ getShowButtonLogin WRITE setShowLogin)

        Q_PROPERTY(bool showLogout READ getShowButtonLogout WRITE setShowLogout)

        Q_PROPERTY(QString userPassword READ getUserPassword WRITE setUserPassword)

        Q_PROPERTY(QString scientistPassword READ getScientistPassword WRITE setScientistPassword)

        Q_PROPERTY(QString engineerPassword READ getEngineerPassword WRITE setEngineerPassword)

        Q_ENUMS(userTypesProperty)
        Q_PROPERTY(userTypesProperty currentUserType READ getCurrentUserTypeProperty WRITE setCurrentUserTypeProperty)
        enum userTypesProperty
        {
            User = USERLEVEL_USER,
            Scientist = USERLEVEL_SCIENTIST,
            Engineer = USERLEVEL_ENGINEER
        };

        void setCurrentUserTypeProperty(userTypesProperty pUserType)
        {
            setCurrentUserType((userTypesProperty) pUserType);
        }
        userTypesProperty getCurrentUserTypeProperty()
        {
            return (userTypesProperty) getCurrentUserType();
        }


        Q_ENUMS(detailsLayoutProperty)
        Q_PROPERTY(detailsLayoutProperty detailsLayout READ getDetailsLayoutProperty WRITE setDetailsLayoutProperty)
        enum detailsLayoutProperty
        {
            Top = TOP,
            Bottom = BOTTOM,
            Left = LEFT,
            Right = RIGHT
        };

        void setDetailsLayoutProperty(detailsLayoutProperty pDetailsLayout)
        {
            setDetailsLayout((detailsLayoutProperty) pDetailsLayout);
        }
        detailsLayoutProperty getDetailsLayoutProperty()
        {
            return (detailsLayoutProperty) getDetailsLayout();
        }



    private slots:
        void buttonLoginClicked();

        void buttonLogoutClicked();


};



#endif // QELOGIN_H


