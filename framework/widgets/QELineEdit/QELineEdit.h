/*  QELineEdit.h
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
 *  Copyright (c) 2009, 2010, 2012
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef QELINEEDIT_H
#define QELINEEDIT_H

#include <QLineEdit>
#include <QEWidget.h>
#include <QEString.h>
#include <QEStringFormatting.h>
#include <QEPluginLibrary_global.h>
#include <QEStringFormattingMethods.h>
#include <QCaVariableNamePropertyManager.h>

class QEPLUGINLIBRARYSHARED_EXPORT QELineEdit :
      public QLineEdit, public QEWidget, public QEStringFormattingMethods {

    Q_OBJECT

    //=================================================================================
    // Single Variable properties
    // These properties should be identical for every widget using a single variable.
    // WHEN MAKING CHANGES: search for SINGLEVARIABLEPROPERTIES and change all occurances.
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
    //=================================================================================

    //=================================================================================
    // Control widget properties
    // These properties should be similar for all widgets with control functionality (writing to variables, not just displaying).
    // WHEN MAKING CHANGES: search for CONTROLVARIABLEPROPERTIES and change all relevent occurances.
public:
    /// Sets if this widget subscribes for data updates and displays current data.
    /// Default is 'true' (subscribes for and displays data updates)
    Q_PROPERTY(bool subscribe READ getSubscribe WRITE setSubscribe)
    /// Sets if this widget automatically writes any changes when it loses focus.
    /// Default is 'false' (does not write any changes when it loses focus).
    Q_PROPERTY(bool writeOnLoseFocus READ getWriteOnLoseFocus WRITE setWriteOnLoseFocus)
    /// Sets if this widget writes any changes when the user presses 'enter'.
    /// Note, the current value will be written even if the user has not changed it.
    /// Default is 'true' (writes any changes when the user presses 'enter').
    Q_PROPERTY(bool writeOnEnter READ getWriteOnEnter WRITE setWriteOnEnter)
    /// Sets if this widget writes any changes when the user finished editing (the QLineEdit 'editingFinished' signal is emitted).
    /// No writing occurs if no changes were made.
    /// Default is 'true' (writes any changes when the QLineEdit 'editingFinished' signal is emitted).
    Q_PROPERTY(bool writeOnFinish READ getWriteOnFinish WRITE setWriteOnFinish)
    /// Sets if this widget will ask for confirmation (using a dialog box) prior to writing data.
    /// Default is 'false' (will not ask for confirmation (using a dialog box) prior to writing data).
    Q_PROPERTY(bool confirmWrite READ getConfirmWrite WRITE setConfirmWrite)
public:
    //=================================================================================

    //=================================================================================
    // Standard properties
    // These properties should be identical for every widget using them.
    // WHEN MAKING CHANGES: search for STANDARDPROPERTIES and change all occurances.
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
    /// User friendly enumerations for #userLevelVisibility and #userLevelEnabled properties - refer to #userLevelVisibility and #userLevelEnabled properties and ::userLevel enumeration for details.
    enum UserLevels { User      = USERLEVEL_USER,          ///< Refer to ::USERLEVEL_USER for details
                      Scientist = USERLEVEL_SCIENTIST,     ///< Refer to ::USERLEVEL_SCIENTIST for details
                      Engineer  = USERLEVEL_ENGINEER       ///< Refer to ::USERLEVEL_ENGINEER for details
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

public slots:
    /// Similar to standard setEnabled slot, but allows QE widget to determine if the widget remains disabled due to invalid data.
    /// If disabled due to invalid data, a request to enable the widget will be honoured when the data is no longer invalid.
    void requestEnabled( const bool& state ){ setApplicationEnabled( state ); }

public:
    //=================================================================================

    //=================================================================================
    // String formatting properties
    // These properties should be identical for every widget managing strings.
    // WHEN MAKING CHANGES: search for STRINGPROPERTIES and change all occurances.
public:
    /// Precision used when formatting floating point numbers. The default is 4.
    /// This is only used if useDbPrecision is false.
    Q_PROPERTY(int  precision READ getPrecision WRITE setPrecision)

    /// If true (default), format floating point numbers using the precision supplied with the data.
    /// If false, the precision property is used.
    Q_PROPERTY(bool useDbPrecision READ getUseDbPrecision WRITE setUseDbPrecision)

    /// If true (default), always add a leading zero when formatting numbers.
    ///
    Q_PROPERTY(bool leadingZero READ getLeadingZero WRITE setLeadingZero)

    /// If true (default), always remove any trailing zeros when formatting numbers.
    ///
    Q_PROPERTY(bool trailingZeros READ getTrailingZeros WRITE setTrailingZeros)

    /// If true (default), add engineering units supplied with the data.
    ///
    Q_PROPERTY(bool addUnits READ getAddUnits WRITE setAddUnits)

    /// An enumeration list used to data values. Used only when the formatting option is 'local enumeration'.
    /// Value is converted to an integer and used to select a string from this list.
    Q_PROPERTY(QString/*localEnumerationList*/ localEnumeration READ getLocalEnumeration WRITE setLocalEnumeration)

    /// \enum    Formats
    /// User friendly enumerations for format property - refer to QEStringFormatting::formats for details.
    enum Formats { Default          = QEStringFormatting::FORMAT_DEFAULT,            ///< Refer to QEStringFormatting::FORMAT_DEFAULT for details
                   Floating         = QEStringFormatting::FORMAT_FLOATING,           ///< Refer to QEStringFormatting::FORMAT_FLOATING for details
                   Integer          = QEStringFormatting::FORMAT_INTEGER,            ///< Refer to QEStringFormatting::FORMAT_INTEGER for details
                   UnsignedInteger  = QEStringFormatting::FORMAT_UNSIGNEDINTEGER,    ///< Refer to QEStringFormatting::FORMAT_UNSIGNEDINTEGER for details
                   Time             = QEStringFormatting::FORMAT_TIME,               ///< Refer to QEStringFormatting::FORMAT_TIME for details
                   LocalEnumeration = QEStringFormatting::FORMAT_LOCAL_ENUMERATE     ///< Refer to QEStringFormatting::FORMAT_LOCAL_ENUMERATE  for details (and the #localEnumeration property)
                };
    void setFormatProperty( Formats format ){ setFormat( (QEStringFormatting::formats)format ); }  ///< Access function for #format property - refer to #format property for details
    Formats getFormatProperty(){ return (Formats)getFormat(); }                                    ///< Access function for #format property - refer to #format property for details
    Q_ENUMS(Formats)

    /// Format to apply to data. Default is 'Default' in which case the data type supplied with the data determines how the data is formatted.
    /// For all other options, an attempt is made to format the data as requested (whatever its native form).
    Q_PROPERTY(Formats format READ getFormatProperty WRITE setFormatProperty)

    /// Base used for when formatting integers. Default is 10 (duh!)
    ///
    Q_PROPERTY(unsigned int radix READ getRadix WRITE setRadix)

    /// \enum Notations
    /// User friendly enumerations for notation property - refer to QEStringFormatting::notations for details.
    enum Notations { Fixed       = QEStringFormatting::NOTATION_FIXED,              ///< Refer to QEStringFormatting::NOTATION_FIXED for details
                     Scientific  = QEStringFormatting::NOTATION_SCIENTIFIC,         ///< Refer to QEStringFormatting::NOTATION_SCIENTIFIC for details
                     Automatic   = QEStringFormatting::NOTATION_AUTOMATIC           ///< Refer to QEStringFormatting::NOTATION_AUTOMATIC for details
                };
    void setNotationProperty( Notations notation ){ setNotation( (QEStringFormatting::notations)notation ); }  ///< Access function for #notation property - refer to #notation property for details
    Notations getNotationProperty(){ return (Notations)getNotation(); }                                        ///< Access function for #notation property - refer to #notation property for details
    Q_ENUMS(Notations)
    /// Notation used for numerical formatting. Default is fixed.
    ///
    Q_PROPERTY(Notations notation READ getNotationProperty WRITE setNotationProperty)

    /// \enum ArrayActions
    /// User friendly enumerations for arrayAction property - refer to QEStringFormatting::arrayActions for details.
    enum ArrayActions { Append = QEStringFormatting::APPEND,            ///< Refer to QEStringFormatting::APPEND for details
                        Ascii  = QEStringFormatting::ASCII,             ///< Refer to QEStringFormatting::ASCII for details
                        Index  = QEStringFormatting::INDEX              ///< Refer to QEStringFormatting::INDEX for details
                    };
    void setArrayActionProperty( ArrayActions arrayAction ){ setArrayAction( (QEStringFormatting::arrayActions)arrayAction ); }    ///< Access function for #arrayAction property - refer to #arrayAction property for details
    ArrayActions getArrayActionProperty(){ return (ArrayActions)getArrayAction(); }                                                ///< Access function for #arrayAction property - refer to #arrayAction property for details
    Q_ENUMS(ArrayActions)

    /// Text formatting option for array data. Default is ASCII. Options are:
    /// \li ASCII - treat array as a single text string. For example an array of three characters 'a' 'b' 'c' will be formatted as 'abc'.
    /// \li APPEND - treat array as an array of numbers and format a string containing them all with a space between each. For example, an array of three numbers 10, 11 and 12 will be formatted as '10 11 12'.
    /// \li INDEX - Extract a single item from the array. The item is then formatted as any other non array data would be. The item selected is determined by the arrayIndex property. For example, if arrayIndex property is 1, an array of three numbers 10, 11 and 12 will be formatted as '11'.
    Q_PROPERTY(ArrayActions arrayAction READ getArrayActionProperty WRITE setArrayActionProperty)

    /// Index used to select a single item of data for formatting from an array of data. Default is 0.
    /// Only used when the arrayAction property is INDEX. Refer to the arrayAction property for more details.
    Q_PROPERTY(unsigned int arrayIndex READ getArrayIndex WRITE setArrayIndex)
public:
    //=================================================================================

public:
    /// Create without a variable.
    /// Use setVariableNameProperty() and setSubstitutionsProperty() to define a variable and, optionally, macro substitutions later.
    QELineEdit( QWidget *parent = 0 );

    /// Create with a variable.
    /// A connection is automatically established.
    /// If macro substitutions are required, create without a variable and set the variable and macro substitutions after creation.
    QELineEdit( const QString &variableName, QWidget *parent = 0 );

    // Property convenience functions

    // write on lose focus
    /// Sets if this widget automatically writes any changes when it loses focus.
    /// Default is 'false' (does not write any changes when it loses focus).
    void setWriteOnLoseFocus( bool writeOnLoseFocus );
    /// Returns 'true' if this widget automatically writes any changes when it loses focus.
    ///
    bool getWriteOnLoseFocus();

    // write on enter
    /// Sets if this widget writes any changes when the user presses 'enter'.
    /// Note, the current value will be written even if the user has not changed it.
    /// Default is 'true' (writes any changes when the user presses 'enter').
    void setWriteOnEnter( bool writeOnEnter );
    /// Returns 'true' if this widget writes any changes when the user presses 'enter'.
    ///
    bool getWriteOnEnter();

    // write on finish
    /// Sets if this widget writes any changes when the user finished editing (the QLineEdit 'editingFinished' signal is emitted).
    /// No writing occurs if no changes were made.
    /// Default is 'true' (writes any changes when the QLineEdit 'editingFinished' signal is emitted).
    void setWriteOnFinish( bool writeOnFinish );
    /// Returns 'true' if this widget writes any changes when the user finished editing (the QLineEdit 'editingFinished' signal is emitted).
    ///
    bool getWriteOnFinish();

    // confirm write
    /// Sets if this widget will ask for confirmation (using a dialog box) prior to writing data.
    /// Default is 'false' (will not ask for confirmation (using a dialog box) prior to writing data).
    void setConfirmWrite( bool confirmWrite );
    /// Returns 'true' if this widget will ask for confirmation (using a dialog box) prior to writing data.
    ///
    bool getConfirmWrite();

    // subscribe
    /// Sets if this widget subscribes for data updates and displays current data.
    /// Default is 'true' (subscribes for and displays data updates)
    void setSubscribe( bool subscribe );
    /// Returns 'true' if this widget subscribes for data updates and displays current data.
    ///
    bool getSubscribe();

private:
    bool writeOnLoseFocus;        // Write changed value to database when widget object loses focus (user moves from widget)
    bool writeOnEnter;            // Write changed value to database when enter is pressed with focus on the widget
    bool writeOnFinish;           // Write changed value to database when user finishes editing (leaves a widget)
    bool localEnabled;            // Override the default widget setEnabled to allow alarm states to override current enabled state
    bool confirmWrite;            // Request confirmation before writing a value
    bool allowDrop;

    void establishConnection( unsigned int variableIndex );

private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setTextIfNoFocus( const QString& value, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );  // Update the text in the widget as long as the user is not entering data in it
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );
    void userReturnPressed();                       // Act on the user pressing return in the widget
    void userEditingFinished();                     // Act on the user signaling text editing is complete (pressing return)

signals:
    // Note, the following signals are common to many QE widgets,
    // if changing the doxygen comments, ensure relevent changes are migrated to all instances
    /// Sent when the widget is updated following a data change
    /// Can be used to pass on EPICS data (as presented in this widget) to other widgets.
    /// For example a QList widget could log updates from this widget.
    void dbValueChanged( const QString& out );
    /// Internal use only. Used by QEConfiguredLayout to be notified when one of its widgets has written something
    void userChange( const QString& oldValue, const QString& newValue, const QString& lastValue );    // Signal a user attempt to change a value. Values are strings as the user sees them
    /// Internal use only. Used when changing a property value to force a re-display to reflect the new property value.
    void requestResend();

private:
    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );
    void writeValue( QEString *qca, QString newValue );
    QString lastValue;                      // Last updated value (may have arrived while user is editing field)
    QString lastUserValue;                  // Last updated value seen by the user (same as lastValue unless the user is editing the text)
    bool messageDialogPresent;              // True if dialog box for the widget is present. When this dialog box is present the line edit no longer have focus, but the user is still effectivly interacting with it
    bool writeFailMessageDialogPresent;     // True if the write faile message dialog has been created. Cleared in spurious 'editing finished' callback caused by the 'write failed' dialog
    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

    void stringFormattingChange(){ requestResend(); }

    void writeNow();

    // Drag and Drop
private:
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    // Don't drag from interactive widget void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDrop( QVariant drop );
    QVariant getDrop();
};

#endif // QELINEEDIT_H
