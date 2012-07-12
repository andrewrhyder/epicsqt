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

#ifndef QECONFIGUREDLAYOUT_H
#define QECONFIGUREDLAYOUT_H

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
#include <QCaComboBox.h>
#include <QCaSpinBox.h>
#include <list>

using namespace std;



enum configuration
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
//  CONFIGUREDLAYOUT CLASS
// ============================================================
class _Item
{

    private:
        QString name;
        QString substitution;
        QString visible;


    protected:


    public:
        _Item();

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
//  _QDIALOGITEM CLASS
// ============================================================
class _QDialogItem:public QDialog
{

    Q_OBJECT

    private:
        QPushButton *qPushButtonClose;


    public:
        _QDialogItem(QWidget * pParent = 0, int pCurrentUserType = 0, _Item *pItem = 0, _Group *pGroup = 0, Qt::WindowFlags pF = 0);


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
        _Item *item;
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
        QString itemName;
        QString fieldName;


    public:
        _QCaLineEdit(QWidget * pParent = 0);

        void setItemName(QString pItemName);

        QString getItemName();

        void setFieldName(QString pFieldName);

        QString getFieldName();

        void valueWritten(QString pNewValue, QString pOldValue);

};




// ============================================================
//  _QCACOMBOBOX CLASS
// ============================================================
class _QCaComboBox:public QCaComboBox
{

    private:
        QString itemName;
        QString fieldName;


    public:
        _QCaComboBox(QWidget * pParent = 0);

        void setItemName(QString pItemName);

        QString getItemName();

        void setFieldName(QString pFieldName);

        QString getFieldName();

        void valueWritten(QString pNewValue, QString pOldValue);

};





// ============================================================
//  _QCASPINBOX CLASS
// ============================================================
class _QCaSpinBox:public QCaSpinBox
{

    private:
        QString itemName;
        QString fieldName;


    public:
        _QCaSpinBox(QWidget * pParent = 0);

        void setItemName(QString pItemName);

        QString getItemName();

        void setFieldName(QString pFieldName);

        QString getFieldName();

        void valueWritten(QString pNewValue, QString pOldValue);

};






// ============================================================
//  QECONFIGUREDLAYOUT CLASS
// ============================================================
class QCAPLUGINLIBRARYSHARED_EXPORT QEConfiguredLayout:public QWidget, public QCaWidget
{

    Q_OBJECT

    private:
        void setConfiguration(QString pValue);


    protected:
        list <_Item *> itemList;
        QLabel *qLabelDescriptionList;
        QComboBox *qComboBoxItemList;
        QVBoxLayout *qVBoxLayoutFields;
        QScrollArea *qScrollArea;
        QString configurationFile;
        QString configurationText;
        //QString descriptionList;
        int configurationType;
        int currentUserType;


    public:

        QEConfiguredLayout(QWidget *pParent = 0);
        virtual ~QEConfiguredLayout(){}

        void setConfigurationType(int pValue);
        int getConfigurationType();

        void setShowItemList(bool pValue);
        bool getShowItemList();

        void setDescriptionList(QString pValue);
        QString getDescriptionList();

        void setConfigurationFile(QString pValue);
        QString getConfigurationFile();

        void setConfigurationText(QString pValue);
        QString getConfigurationText();

        void setCurrentUserType(int pValue);
        int getCurrentUserType();


        void refreshFields();

        void userLevelChanged(userLevels pValue);

        Q_PROPERTY(bool showItemList READ getShowItemList WRITE setShowItemList)

        Q_PROPERTY(QString descriptionList READ getDescriptionList WRITE setDescriptionList)


        Q_ENUMS(configurationTypesProperty)
        Q_PROPERTY(configurationTypesProperty configurationType READ getConfigurationTypeProperty WRITE setConfigurationTypeProperty)
        enum configurationTypesProperty
        {
            File = FROM_FILE,
            Text = FROM_TEXT
        };

        void setConfigurationTypeProperty(configurationTypesProperty pConfigurationType)
        {
            setConfigurationType((configurationTypesProperty) pConfigurationType);
        }
        configurationTypesProperty getConfigurationTypeProperty()
        {
            return (configurationTypesProperty) getConfigurationType();
        }

        Q_PROPERTY(QString configurationFile READ getConfigurationFile WRITE setConfigurationFile)

        Q_PROPERTY(QString configurationText READ getConfigurationText WRITE setConfigurationText)

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
        void comboBoxItemSelected(int);


};





#endif /// QCACONFIGUREDLAYOUT_H



