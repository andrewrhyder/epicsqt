/*  QERadioGroup.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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
 *  Copyright (c) 2013
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef QERADIOGROUP_H
#define QERADIOGROUP_H

#include <QGroupBox>
#include <QList>
#include <QString>
#include <QVector>
#include <QRadioButton>
#include <QSize>
#include <QMap>

#include <QCaObject.h>
#include <QEWidget.h>
#include <QEInteger.h>
#include <QEIntegerFormatting.h>
#include <QELocalEnumeration.h>
#include <QCaVariableNamePropertyManager.h>
#include <QEPluginLibrary_global.h>


class QEPLUGINLIBRARYSHARED_EXPORT QERadioGroup :
      public QGroupBox, public QEWidget  {

Q_OBJECT
    // BEGIN-SINGLE-VARIABLE-PROPERTIES ===============================================
    // Single Variable properties
    // These properties should be identical for every widget using a single variable.
    // WHEN MAKING CHANGES: Use the update_widget_properties script in the
    // resources directory.
    //
    // Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
    // A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
public:
    /// EPICS variable name (CA PV)
    ///
    Q_PROPERTY(QString variable READ getVariableNameProperty WRITE setVariableNameProperty)
    /// Macro substitutions. The default is no substitutions. The format is NAME1=VALUE1[,] NAME2=VALUE2... Values may be quoted strings. For example, 'PUMP=PMP3, NAME = "My Pump"'
    /// These substitutions are applied to variable names for all QE widgets. In some widgets are are also used for other purposes.
    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)

private:
    void    setVariableNameProperty( QString variableName ){ variableNamePropertyManager.setVariableNameProperty( variableName ); }
    QString getVariableNameProperty(){ return variableNamePropertyManager.getVariableNameProperty(); }

    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions ){ variableNamePropertyManager.setSubstitutionsProperty( variableNameSubstitutions ); }
    QString getVariableNameSubstitutionsProperty(){ return variableNamePropertyManager.getSubstitutionsProperty(); }

    QCaVariableNamePropertyManager variableNamePropertyManager;
public:
    // END-SINGLE-VARIABLE-PROPERTIES =================================================

    // BEGIN-STANDARD-PROPERTIES ======================================================
    // Standard properties
    // These properties should be identical for every widget using them.
    // WHEN MAKING CHANGES: Use the update_widget_properties script in the
    // resources directory.
public:
    /// Use the variable as the tool tip. Default is true. Tool tip property will be overwritten by the variable name.
    ///
    Q_PROPERTY(bool variableAsToolTip READ getVariableAsToolTip WRITE setVariableAsToolTip)

    /// Set the prefered 'enabled' state. Default is true.
    /// This property is copied to the standard Qt 'enabled' property if the data being displayed is valid.
    /// If the data being displayed is invalid the standard Qt 'enabled' property will always be set to false to indicate invalid data.
    /// The value of this property will only be copied to the standard Qt 'enabled' property once data is valid.
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)

    /// Access function for #enabled property - refer to #enabled property for details
    bool isEnabled() const { return getApplicationEnabled(); }

    /// Access function for #enabled property - refer to #enabled property for details
    void setEnabled( bool state ){ setApplicationEnabled( state ); }

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
    enum UserLevels { User      = USERLEVEL_USER,          ///< Refer to USERLEVEL_USER for details
                      Scientist = USERLEVEL_SCIENTIST,     ///< Refer to USERLEVEL_SCIENTIST for details
                      Engineer  = USERLEVEL_ENGINEER       ///< Refer to USERLEVEL_ENGINEER for details
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
    void setUserLevelVisibilityProperty( UserLevels level ) { setUserLevelVisibility( (userLevels)level ); }///< Access function for #userLevelVisibility property - refer to #userLevelVisibility property for details
    UserLevels getUserLevelEnabledProperty() { return (UserLevels)getUserLevelEnabled(); }                  ///< Access function for #userLevelEnabled property - refer to #userLevelEnabled property for details
    void setUserLevelEnabledProperty( UserLevels level ) { setUserLevelEnabled( (userLevels)level ); }      ///< Access function for #userLevelEnabled property - refer to #userLevelEnabled property for details

    /// If set (default) widget will indicate the alarm state of any variable data is displaying.
    /// Typically the background colour is set to indicate the alarm state.
    /// Note, this property is included in the set of standard properties as it applies to most widgets. It
    /// will do nothing for widgets that don't display data.
    Q_PROPERTY(bool displayAlarmState READ getDisplayAlarmState WRITE setDisplayAlarmState)

public slots:
    /// Similar to standard setEnabled slot, but allows QE widget to determine if the widget remains disabled due to invalid data.
    /// If disabled due to invalid data, a request to enable the widget will be honoured when the data is no longer invalid.
    void requestEnabled( const bool& state ){ setApplicationEnabled( state ); }

public:
    // END-STANDARD-PROPERTIES ========================================================


    // QERadioGroup specific properties ===============================================
    //
public:
    Q_PROPERTY (bool useDbEnumerations READ getUseDbEnumerations WRITE setUseDbEnumerations)

    /// Use database enumerations - defaults to true
    ///
    void setUseDbEnumerations (bool useDbEnumerations);
    bool getUseDbEnumerations ();

    /// Enumrations values used when useDbEnumerations is false.
    ///
    Q_PROPERTY (QString localEnumerations READ getLocalEnumerations  WRITE setLocalEnumerations)

    void setLocalEnumerations (const QString & localEnumerations);
    QString getLocalEnumerations ();

    /// Number of colums - defaults to two.
    ///
    Q_PROPERTY (int columns READ getColumns WRITE setColumns)

    void setColumns (int columns);
    int getColumns ();
    //
    // End of QERadioGroup specific properties =========================================

public:
    /// Create without a variable.
    /// Use setVariableNameProperty() and setSubstitutionsProperty() to define a
    /// variable and, optionally, macro substitutions later.
    ///
    explicit QERadioGroup (QWidget *parent = 0);

    /// Create with a variable.
    /// A connection is automatically established.
    /// If macro substitutions are required, create without a variable and set the
    /// variable and macro substitutions after creation.
    ///
    explicit QERadioGroup (const QString &variableName, QWidget *parent = 0);

    /// Create with a group title and a variable.
    /// A connection is automatically established.
    /// If macro substitutions are required, create without a variable and set the
    /// variable and macro substitutions after creation.
    ///
    explicit QERadioGroup (const QString &title, const QString &variableName, QWidget* parent = 0);

    /// Destruction
    virtual ~QERadioGroup(){}

    int getCurrentIndex ();
protected:
    QSize sizeHint () const;
    void resizeEvent (QResizeEvent * event);

    // override QEWidget fnctions.
    //
    void establishConnection (unsigned int variableIndex);
    qcaobject::QCaObject* createQcaItem (unsigned int variableIndex);

    void setCurrentIndex (int index);

private:
    typedef QList<QRadioButton *> QRadioButtonList;
    typedef QMap<int, int> QIntTintMap;


    QEIntegerFormatting integerFormatting;
    QELocalEnumeration localEnumerations;

    // Use of the local enumerations means thay we could have spare mapping,
    // e.g.: 1 => Red, 5 => Blue, 63 => Green.   Therefore we need to create
    // and maintain a value to button and button to value maps.
    //
    QIntTintMap valueToButtonMap;
    QIntTintMap buttonToValueMap;
    QRadioButtonList radioButtonList;

    bool useDbEnumerations;
    int currentIndex;
    int number;    // number of displayed buttons.
    int rows;
    int cols;
    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;
    bool isFirstUpdate;

    void commonSetup ();
    void setButtonText ();
    void setButtonGeometry ();

private slots:
    void connectionChanged (QCaConnectionInfo& connectionInfo);
    void valueUpdate (const long& value, QCaAlarmInfo&, QCaDateTime&, const unsigned int&);

    // Just in case hooked up to an array PV.
    //
    void arrayValueUpdate (const QVector<long>& values, QCaAlarmInfo&, QCaDateTime&, const unsigned int&);

    void useNewVariableNameProperty (QString variableNameIn,
                                     QString variableNameSubstitutionsIn,
                                     unsigned int variableIndex );

    void buttonClicked (bool checked);

signals:
    // Note, the following signals are common to many QE widgets,
    // if changing the doxygen comments, ensure relevent changes are migrated to all instances
    /// Sent when the widget is updated following a data change
    /// Can be used to pass on EPICS data (as presented in this widget) to other widgets.
    /// For example a QList widget could log updates from this widget.
    void dbValueChanged (const long& out);

protected:
    // Drag and Drop
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDrop( QVariant drop );
    QVariant getDrop();

    // Copy paste
    QString copyVariable();
    QVariant copyData();
};

#endif // QERADIOGROUP_H