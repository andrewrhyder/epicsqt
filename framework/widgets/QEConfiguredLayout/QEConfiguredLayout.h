/*  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2012 Australian Synchrotron
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

        QEWidget *qeWidget;  // TODO: this attribute should be private

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

        /// Change the order of the widgets. Valid orders are: TOP, BOTTOM, LEFT and RIG
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



        /*

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
        */



    // BEGIN-STANDARD-PROPERTIES ======================================================
    // Standard properties
    // These properties should be identical for every widget using them.
    // WHEN MAKING CHANGES: Use the update_widget_properties script in the
    // resources directory.
public:
    /// Use the variable as the tool tip. Default is true. Tool tip property will be overwritten by the variable name.
    ///
    Q_PROPERTY(bool variableAsToolTip READ getVariableAsToolTip WRITE setVariableAsToolTip)

    /// Allow drag/drops operations to this widget. Default is false. Any dropped text will be used as a new variable name.
    ///
    Q_PROPERTY(bool allowDrop READ getAllowDrop WRITE setAllowDrop)

    /// Display the widget. Default is true.
    /// Setting this property false is usefull if widget is only used to provide a signal - for example, when supplying data to a QELink widget.
    /// Note, when false the widget will still be visible in Qt Designer.
    Q_PROPERTY(bool visible READ getRunVisible WRITE setRunVisible)

    /// Set the ID used by the message filtering system. Default is zero.
    /// Widgets or applications that use messages from the framework have the option of filtering on this ID.
    /// For example, by using a unique message source ID a QELog widget may be set up to only log messages from a select set of widgets.
    Q_PROPERTY(unsigned int messageSourceId READ getMessageSourceId WRITE setMessageSourceId )

    /// Style Sheet string to be applied when the widget is displayed in 'User' mode. Default is an empty string.
    /// The syntax is the standard Qt Style Sheet syntax. For example, 'background-color: red'
    /// This Style Sheet string will be applied by the styleManager class.
    /// Refer to the styleManager class for details about how this Style Sheet string will be merged with any pre-existing Style Sheet string
    /// and any Style Sheet strings generated during the display of data.
    Q_PROPERTY(QString userLevelUserStyle READ getStyleUser WRITE setStyleUser)

    /// Style Sheet string to be applied when the widget is displayed in 'Scientist' mode. Default is an empty string.
    /// The syntax is the standard Qt Style Sheet syntax. For example, 'background-color: red'
    /// This Style Sheet string will be applied by the styleManager class.
    /// Refer to the styleManager class for details about how this Style Sheet string will be merged with any pre-existing Style Sheet string
    /// and any Style Sheet strings generated during the display of data.
    Q_PROPERTY(QString userLevelScientistStyle READ getStyleScientist WRITE setStyleScientist)

    /// Style Sheet string to be applied when the widget is displayed in 'Engineer' mode. Default is an empty string.
    /// The syntax is the standard Qt Style Sheet syntax. For example, 'background-color: red'
    /// This Style Sheet string will be applied by the styleManager class.
    /// Refer to the styleManager class for details about how this Style Sheet string will be merged with any pre-existing Style Sheet string
    /// and any Style Sheet strings generated during the display of data.
    Q_PROPERTY(QString userLevelEngineerStyle READ getStyleEngineer WRITE setStyleEngineer)

    /// \enum UserLevels
    /// User friendly enumerations for #userLevelVisibility and #userLevelEnabled properties - refer to #userLevelVisibility and #userLevelEnabled properties and userLevel enumeration for details.
    enum UserLevels { User      = userLevelTypes::USERLEVEL_USER,          ///< Refer to USERLEVEL_USER for details
                      Scientist = userLevelTypes::USERLEVEL_SCIENTIST,     ///< Refer to USERLEVEL_SCIENTIST for details
                      Engineer  = userLevelTypes::USERLEVEL_ENGINEER       ///< Refer to USERLEVEL_ENGINEER for details
                              };
    Q_ENUMS(UserLevels)

    /// Lowest user level at which the widget is visible. Default is 'User'.
    /// Used when designing GUIs that display more and more detail according to the user mode.
    /// The user mode is set application wide through the QELogin widget, or programatically through setUserLevel()
    /// Widgets that are always visible should be visible at 'User'.
    /// Widgets that are only used by scientists managing the facility should be visible at 'Scientist'.
    /// Widgets that are only used by engineers maintaining the facility should be visible at 'Engineer'.
    Q_PROPERTY(UserLevels userLevelVisibility READ getUserLevelVisibilityProperty WRITE setUserLevelVisibilityProperty)

    /// Lowest user level at which the widget is enabled. Default is 'User'.
    /// Used when designing GUIs that allow access to more and more detail according to the user mode.
    /// The user mode is set application wide through the QELogin widget, or programatically through setUserLevel()
    /// Widgets that are always accessable should be visible at 'User'.
    /// Widgets that are only accessable to scientists managing the facility should be visible at 'Scientist'.
    /// Widgets that are only accessable to engineers maintaining the facility should be visible at 'Engineer'.
    Q_PROPERTY(UserLevels userLevelEnabled READ getUserLevelEnabledProperty WRITE setUserLevelEnabledProperty)

    UserLevels getUserLevelVisibilityProperty() { return (UserLevels)getUserLevelVisibility(); }            ///< Access function for #userLevelVisibility property - refer to #userLevelVisibility property for details
    void setUserLevelVisibilityProperty( UserLevels level ) { setUserLevelVisibility( (userLevelTypes::userLevels)level ); }///< Access function for #userLevelVisibility property - refer to #userLevelVisibility property for details
    UserLevels getUserLevelEnabledProperty() { return (UserLevels)getUserLevelEnabled(); }                  ///< Access function for #userLevelEnabled property - refer to #userLevelEnabled property for details
    void setUserLevelEnabledProperty( UserLevels level ) { setUserLevelEnabled( (userLevelTypes::userLevels)level ); }      ///< Access function for #userLevelEnabled property - refer to #userLevelEnabled property for details

    /// DEPRECATED. USE displayAlarmStateOption INSTEAD.
    /// If set (default) widget will indicate the alarm state of any variable data it is displaying.
    /// If clear widget will never indicate the alarm state of any variable data it is displaying.
    /// Typically the background colour is set to indicate the alarm state.
    /// Note, this property is included in the set of standard properties as it applies to most widgets. It
    /// will do nothing for widgets that don't display data.
    Q_PROPERTY(bool displayAlarmState READ getDisplayAlarmState WRITE setDisplayAlarmState DESIGNABLE false)

    /// \enum DisplayAlarmStateOptions
    /// User friendly enumerations for #displayAlarmStateOption property - refer to #displayAlarmStateOption property and displayAlarmStateOptions enumeration for details.
    enum DisplayAlarmStateOptions { Never       = displayAlarmStateOptions::DISPLAY_ALARM_STATE_NEVER,          ///< Refer to DISPLAY_ALARM_STATE_NEVER for details
                                    Always      = displayAlarmStateOptions::DISPLAY_ALARM_STATE_ALWAYS,         ///< Refer to DISPLAY_ALARM_STATE_ALWAYS for details
                                    WhenInAlarm = displayAlarmStateOptions::DISPLAY_ALARM_STATE_WHEN_IN_ALARM   ///< Refer to DISPLAY_ALARM_STATE_WHEN_IN_ALARM for details
                              };
    Q_ENUMS(DisplayAlarmStateOptions)
    /// If 'Always' (default) widget will indicate the alarm state of any variable data it is displaying, including 'No Alarm'
    /// If 'Never' widget will never indicate the alarm state of any variable data it is displaying.
    /// If 'WhenInAlarm' widget only indicate the alarm state of any variable data it is displaying if it is 'in alarm'.
    /// Typically the background colour is set to indicate the alarm state.
    /// Note, this property is included in the set of standard properties as it applies to most widgets. It
    /// will do nothing for widgets that don't display data.
    Q_PROPERTY(DisplayAlarmStateOptions displayAlarmStateOption READ getDisplayAlarmStateOptionProperty WRITE setDisplayAlarmStateOptionProperty)

    DisplayAlarmStateOptions getDisplayAlarmStateOptionProperty() { return (DisplayAlarmStateOptions)getDisplayAlarmStateOption(); }            ///< Access function for #displayAlarmStateOption property - refer to #displayAlarmStateOption property for details
    void setDisplayAlarmStateOptionProperty( DisplayAlarmStateOptions option ) { setDisplayAlarmStateOption( (displayAlarmStateOptions)option ); }///< Access function for #displayAlarmStateOption property - refer to #displayAlarmStateOption property for details

public:
    // END-STANDARD-PROPERTIES ========================================================



    private slots:
        void comboBoxItemSelected(int);

        void valueWritten(const QString &pNewValue, const QString &pOldValue, const QString&);

};



#endif // QECONFIGUREDLAYOUT_H
