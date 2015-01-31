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

#include <QDialog>
#include <QDomDocument>
#include <QVBoxLayout>
#include <QELabel.h>
#include <QEBitStatus.h>
#include <QEPushButton.h>
#include <QELineEdit.h>
#include <QEComboBox.h>
#include <QESpinBox.h>



enum details
{
    TOP,
    BOTTOM,
    LEFT,
    RIGHT
};



enum configuration
{
    FROM_FILE,
    FROM_TEXT
};




enum types
{
    LABEL,
    LINEEDIT,
    COMBOBOX,
    SPINBOX,
    BUTTON,
    BITSTATUS
};





// ============================================================
//  FIELD CLASS
// ============================================================
class _Field
{

    private:
        QString name;
        QString processVariable;
        QString group;
        QString visible;
        QString editable;
        bool visibility;
        bool join;
        int type;


    public:
        _Field();

        QEWidget *getWidget();
        void setWidget(QString *pValue);

        QString getName();
        void setName(QString pValue);

        QString getProcessVariable();
        void setProcessVariable(QString pValue);

        void setJoin(bool pValue);
        bool getJoin();

        int getType();
        void setType(int pValue);

        QString getGroup();
        void setGroup(QString pValue);

        QString getVisible();
        void setVisible(QString pValue);

        QString getEditable();
        void setEditable(QString pValue);

        bool getVisibility();
        void setVisibility(bool pValue);

        QEWidget *qCaWidget;  // TODO: this attribute should be private

};





// ============================================================
//  _ITEM CLASS
// ============================================================
class _Item
{

    private:
        QString name;
        QString substitution;
        QString visible;


    public:
        _Item();

        void setName(QString pValue);
        QString getName();

        void setSubstitution(QString pValue);
        QString getSubstitution();

        void setVisible(QString pValue);
        QString getVisible();

        QList <_Field *> fieldList;  // TODO: this attribute should be private

};






// ============================================================
//  _QPUSHBUTTONGROUP CLASS
// ============================================================
class _QPushButtonGroup:public QPushButton
{

    Q_OBJECT


    private:
        QList <_Field *> *currentFieldList;
        QString itemName;
        QString groupName;


    public:
        _QPushButtonGroup(QWidget * pParent = 0, QString pItemName = "", QString pGroupName = "", QList <_Field *> *pCurrentFieldList = 0);

        void mouseReleaseEvent(QMouseEvent *qMouseEvent);

        void keyPressEvent(QKeyEvent *pKeyEvent);

        void showDialogGroup();



    public slots:
        void buttonGroupClicked();


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
        _QDialogItem(QWidget *pParent = 0, QString pItemName = "", QString pGroupName = "", QList <_Field *> *pCurrentFieldList = 0, Qt::WindowFlags pF = 0);


    private slots:
        void buttonCloseClicked();

};






// ============================================================
//  QECONFIGUREDLAYOUT CLASS
// ============================================================
class QEPLUGINLIBRARYSHARED_EXPORT QEConfiguredLayout:public QWidget, public QEWidget
{

    Q_OBJECT

    private:
        void setConfiguration(QString pValue);


    protected:
        QLabel *qLabelItemDescription;
        QComboBox *qComboBoxItemList;
        QVBoxLayout *qVBoxLayoutFields;
        QScrollArea *qScrollArea;
        QString configurationFile;
        QString configurationText;
        int configurationType;
        int optionsLayout;
        int currentUserType;
        bool subscription;


    public:
        QEConfiguredLayout(QWidget *pParent = 0, bool pSubscription = true);
        virtual ~QEConfiguredLayout(){}

        void setItemDescription(QString pValue);
        QString getItemDescription();

        void setShowItemList(bool pValue);
        bool getShowItemList();

        void setConfigurationType(int pValue);
        int getConfigurationType();

        void setConfigurationFile(QString pValue);
        QString getConfigurationFile();

        void setConfigurationText(QString pValue);
        QString getConfigurationText();

        void setOptionsLayout(int pValue);
        int getOptionsLayout();

        void setCurrentUserType(int pValue);
        int getCurrentUserType();


        void refreshFields();

        void userLevelChanged( userLevelTypes::userLevels pValue );

        QList <_Item *> itemList;    // TODO: this attribute shoule be private

        QList <_Field *> currentFieldList;   //TODO: this attribute should be private

        Q_PROPERTY(QString itemDescription READ getItemDescription WRITE setItemDescription)

        Q_PROPERTY(bool showItemList READ getShowItemList WRITE setShowItemList)

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


        Q_ENUMS(optionsLayoutProperty)
        Q_PROPERTY(optionsLayoutProperty optionsLayout READ getOptionsLayoutProperty WRITE setOptionsLayoutProperty)
        enum optionsLayoutProperty
        {
            Top = TOP,
            Bottom = BOTTOM,
            Left = LEFT,
            Right = RIGHT
        };

        void setOptionsLayoutProperty(optionsLayoutProperty pOptionsLayout)
        {
            setOptionsLayout((optionsLayoutProperty) pOptionsLayout);
        }

        optionsLayoutProperty getOptionsLayoutProperty()
        {
            return (optionsLayoutProperty) getOptionsLayout();
        }


        Q_ENUMS(userTypesProperty)
        Q_PROPERTY(userTypesProperty currentUserType READ getCurrentUserTypeProperty WRITE setCurrentUserTypeProperty)
        enum userTypesProperty
        {
            User =      userLevelTypes::USERLEVEL_USER,
            Scientist = userLevelTypes::USERLEVEL_SCIENTIST,
            Engineer =  userLevelTypes::USERLEVEL_ENGINEER
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

        void valueWritten(const QString &pNewValue, const QString &pOldValue, const QString&);

};



#endif // QECONFIGUREDLAYOUT_H
