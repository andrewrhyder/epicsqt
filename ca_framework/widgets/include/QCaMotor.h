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
#include <QCaWidget.h>
#include <QCaLineEdit.h>
#include <list>

using namespace std;



enum motorConfiguration
{
    FROM_FILE,
    FROM_TEXT
};



// ============================================================
//  FIELD CLASS
// ============================================================
class _Field
{

    private:
        QString name;
        QString processVariable;
        QString mask;
        QString type;
        QString visible;
        QString editable;
        bool join;


    public:
        _Field();

        QString getName();
        void setName(QString pValue);

        QString getProcessVariable();
        void setProcessVariable(QString pValue);

        bool getJoin();
        void setJoin(bool pValue);

        QString getMask();
        void setMask(QString pValue);

        QString getType();
        void setType(QString pValue);

        QString getVisible();
        void setVisible(QString pValue);

        QString getEditable();
        void setEditable(QString pValue);

};




// ============================================================
//  GROUP CLASS
// ============================================================
class _Group
{

    private:
        QString name;
//        list <_Field *> fieldList;  // TODO: this attribute should be private

    public:
        _Group();

        QString getName();

        void setName(QString pValue);

        void addField(_Field &pField);

        list <_Field *> fieldList;  // TODO: this attribute should be private

//        list <_Field> getFieldList();


};




// ============================================================
//  MOTOR CLASS
// ============================================================
class _Motor
{

    private:
        QString name;
        QString substitution;
        QString visible;


    protected:


    public:
        _Motor();

        void setName(QString pValue);

        QString getName();

        void setSubstitution(QString pValue);

        QString getSubstitution();

        void setVisible(QString pValue);

        QString getVisible();

        void addGroup(_Group &pGroup);

        list <_Group *> groupList;  // TODO: this attribute should be private

};





// ============================================================
//  _QDIALOGMOTOR CLASS
// ============================================================
class _QDialogMotor:public QDialog
{

    Q_OBJECT

    private:
        QPushButton *qPushButtonClose;


    public:
        _QDialogMotor(QWidget * pParent = 0, int pCurrentUserType = 0, _Motor *pMotor = 0, _Group *pGroup = 0, Qt::WindowFlags pF = 0);


    private slots:
        void buttonCloseClicked();

};





// ============================================================
//  _QPUSHBUTTONGROUP CLASS
// ============================================================
class _QPushButtonGroup:public QPushButton
{

    Q_OBJECT


    private:


    public:
        _QPushButtonGroup(QWidget * pParent = 0);

        void mouseReleaseEvent(QMouseEvent *qMouseEvent);

        void keyPressEvent(QKeyEvent *pKeyEvent);

        void showDialogGroup();

        // TODO: these attributes should be private
        _Motor *motor;
        _Group *group;
        int currentUserType;


    public slots:
        void buttonGroupClicked();


};





// ============================================================
//  _QCALINEEDIT CLASS
// ============================================================
class _QCaLineEdit:public QCaLineEdit
{

    private:
        QString motorName;
        QString fieldName;


    public:
        _QCaLineEdit(QWidget * pParent = 0);

        void setMotorName(QString pMotorName);

        QString getMotorName();

        void setFieldName(QString pFieldName);

        QString getFieldName();

        void valueWritten(QString pNewValue, QString pOldValue);

};




// ============================================================
//  QCAMOTOR CLASS
// ============================================================
class QCAPLUGINLIBRARYSHARED_EXPORT QCaMotor:public QWidget, public QCaWidget
{

    Q_OBJECT

    private:
        void setMotorConfiguration(QString pValue);


    protected:
        list <_Motor *> motorList;
        QLabel *qLabel;
        QComboBox *qComboBoxMotor;
        QVBoxLayout *qVBoxLayoutFields;
        QScrollArea *qScrollArea;
        QString motorConfigurationFile;
        QString motorConfigurationText;
        int motorConfigurationType;
        int currentUserType;


    public:

        QCaMotor(QWidget *pParent = 0);
        virtual ~QCaMotor(){}

        void setMotorConfigurationType(int pValue);
        int getMotorConfigurationType();

        void setShowMotorList(bool pValue);
        bool getShowMotorList();

        void setMotorConfigurationFile(QString pValue);
        QString getMotorConfigurationFile();

        void setMotorConfigurationText(QString pValue);
        QString getMotorConfigurationText();

        void setCurrentUserType(int pValue);
        int getCurrentUserType();


        void refreshFields();

        void userLevelChanged(userLevels pValue);

        Q_PROPERTY(bool showMotorList READ getShowMotorList WRITE setShowMotorList)

        Q_ENUMS(motorConfigurationTypesProperty)
        Q_PROPERTY(motorConfigurationTypesProperty motorConfigurationType READ getMotorConfigurationTypeProperty WRITE setMotorConfigurationTypeProperty)
        enum motorConfigurationTypesProperty
        {
            File = FROM_FILE,
            Text = FROM_TEXT
        };

        void setMotorConfigurationTypeProperty(motorConfigurationTypesProperty pMotorConfigurationType)
        {
            setMotorConfigurationType((motorConfigurationTypesProperty) pMotorConfigurationType);
        }
        motorConfigurationTypesProperty getMotorConfigurationTypeProperty()
        {
            return (motorConfigurationTypesProperty) getMotorConfigurationType();
        }

        Q_PROPERTY(QString motorConfigurationFile READ getMotorConfigurationFile WRITE setMotorConfigurationFile)

        Q_PROPERTY(QString motorConfigurationText READ getMotorConfigurationText WRITE setMotorConfigurationText)

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



    private slots:
        void comboBoxMotorSelected(int);


};





#endif /// QCAMOTOR_H



