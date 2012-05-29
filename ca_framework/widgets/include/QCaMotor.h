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

#ifndef QCAMOTOR_H
#define QCAMOTOR_H

#include <QWidget>
#include <QDialog>
#include <QGroupBox>
#include <ContainerProfile.h>
#include <QLineEdit>
#include <QRadioButton>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QCaPluginLibrary_global.h>
#include <list>

using namespace std;


enum details
{
    TOP,
    BOTTOM,
    LEFT,
    RIGHT
};



class _Field
{

    private:
        QString name;
        QString processVariable;
        QString mask;
        bool visible;

    public:
        _Field();

        QString getName();
        void setName(QString pValue);

        QString getProcessVariable();
        void setProcessVariable(QString pValue);

        QString getDescription();
        void setDescription(QString pValue);

        QString getVisible();
        void setVisible(bool pValue);

        QString getEditable();
        void setEditable(QString pValue);

        QString getMask();
        void setMask(QString pValue);

};




class _Group
{

    private:
        list <_Field> fieldList;

    public:
        void addField(QString pName, QString pProcessVariable, QString pMask, bool pVisible);

        list <_Field> getFieldList();


};




class _QDialogMotor:public QDialog
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



    public:
        _QDialogMotor(QWidget * pParent = 0, Qt::WindowFlags pF = 0);
        void setCurrentUserType(int pValue);
        void setPassword(QString pValue);


    private slots:
        void radioButtonClicked();

        void lineEditPasswordTextChanged(QString pValue);

        void buttonOkClicked();

        void buttonCancelClicked();

};





class QCAPLUGINLIBRARYSHARED_EXPORT QCaMotor:public QWidget, public ContainerProfile
{

    Q_OBJECT

    private:



    protected:
        _QDialogMotor *qCaMotorDialog;
        QLayout *qLayout;
        QPushButton *qPushButtonLogin;
        QLabel *qLabelUserType;
        QString motorConfiguration;
        int currentUserType;
        int detailsLayout;


    public:

        QCaMotor(QWidget *pParent = 0);
        virtual ~QCaMotor(){}

        void setMotorConfiguration(QString pValue);
        QString getMotorConfiguration();

        void setCurrentUserType(int pValue);
        int getCurrentUserType();

        void setDetailsLayout(int pValue);
        int getDetailsLayout();


        Q_PROPERTY(QString motorConfiguration READ getMotorConfiguration WRITE setMotorConfiguration)

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


};





#endif /// QCAMOTOR_H



