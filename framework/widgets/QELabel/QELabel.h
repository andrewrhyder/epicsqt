/*
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
 *  Copyright (c) 2009, 2010
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef QELABEL_H
#define QELABEL_H

#include <QLabel>
#include <QCaWidget.h>
#include <QCaString.h>
#include <QCaStringFormatting.h>
#include <QCaPluginLibrary_global.h>
#include <managePixmaps.h>
#include <QCaStringFormattingMethods.h>
#include <QCaVariableNamePropertyManager.h>

/*!
  This class is a EPICS aware label widget based on the Qt label widget.
  When a variable is defined, the label text (or optionally the background pixmap) will be updated. The label will be disabled if the variable is invalid.
  It is tighly integrated with the base class QCaWidget which provides generic support such as macro substitutions, drag/drop, and standard properties.
 */
class QCAPLUGINLIBRARYSHARED_EXPORT QELabel : public QLabel, public QCaWidget, private managePixmaps, protected QCaStringFormattingMethods {
    Q_OBJECT

  public:
    /// Options for updating the label.
    /// The formatted text is used to update the label text, or select a background pixmap.
    enum updateOptions { UPDATE_TEXT,    ///< Update the label text
                         UPDATE_PIXMAP   ///< Update the label background pixmap
                       };

    /// Create without a variable.
    /// Use setVariableNameProperty() and setSubstitutionsProperty() to define a variable and, optionally, macro substitutions later.
    QELabel( QWidget *parent = 0 );

    /// Create with a variable.
    /// A connection is automatically established.
    /// If macro substitutions are required, create without a variable and set the variable and macro substitutions after creation.
    QELabel( const QString &variableName, QWidget *parent = 0 );

  private:
    void establishConnection( unsigned int variableIndex );

    updateOptions updateOption;

    // Property convenience functions - Update option (icon, text, or both)
    void setUpdateOption( updateOptions updateOptionIn );
    updateOptions getUpdateOption();

private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setLabelText( const QString& text, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )// !! move into Standard Properties section??
    {
        setVariableNameAndSubstitutions(variableNameIn, variableNameSubstitutionsIn, variableIndex);
    }

  signals:
    /// Signal a data update has occured and supply the new formatted text.
    /// Note, the formatted text is still signaled even when it is being used to select a pixmap for the label.
    void dbValueChanged( const QString& out );

    /// Internal use only - Used when a property changes to force a redisplay of the current data.
    void requestResend();

  private:
    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );
    void updateToolTip( const QString& tip );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

    QString lastTextStyle;
    QString currentText;

    void stringFormattingChange(){ requestResend(); }



    // Drag and Drop
private:
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDrop( QVariant drop );
    QVariant getDrop();

    // Copy paste
    QString copyVariable();
    QVariant copyData();
    void paste( QVariant s );

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
    // Standard properties
    // These properties should be identical for every widget using them.
    // WHEN MAKING CHANGES: search for STANDARDPROPERTIES and change all occurances.
    /// Use the variable as the tool tip. Default is true. Tool tip property will be overwritten by the variable name.
    ///
    Q_PROPERTY(bool variableAsToolTip READ getVariableAsToolTip WRITE setVariableAsToolTip)

    /// Set the prefered 'enabled' state. Default is true.
    /// This property is copied to the standard Qt 'enabled' property if the data being displayed is valid.
    /// If the data being displayed is invalid the standard Qt 'enabled' property will always be set to false to indicate invalid data.
    /// The value of this property will only be copied to the standard Qt 'enabled' property once data is valid.
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)

    /// Access function for 'enabled' property - refer to 'enabled' property for details
    bool isEnabled() const { return getApplicationEnabled(); }

    /// Access function for 'enabled' property - refer to 'enabled' property for details
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

    /// User friendly enumerations for userLevelVisibility and userLevelEnabled properties - refer to userLevelVisibility and userLevelEnabled properties and userLevel enumeration for details.
    enum UserLevels { User      = USERLEVEL_USER,
                      Scientist = USERLEVEL_SCIENTIST,
                      Engineer  = USERLEVEL_ENGINEER };
    Q_ENUMS(UserLevels)

    /// Lowest user level at which the widget is visible. Default is 'User'.
    /// Used when designing GUIs that display more and more detail according to the user mode.
    /// The user mode is set application through the QELogin widget, or programatically through setUserLevel()
    /// Widgets that are always visible should be visible at 'User'.
    /// Widgets that are only used by scientists managing the facility should be visible at 'Scientist'.
    /// Widgets that are only used by engineers maintaining the facility should be visible at 'Engineer'.
    Q_PROPERTY(UserLevels userLevelVisibility READ getUserLevelVisibilityProperty WRITE setUserLevelVisibilityProperty)

    /// Lowest user level at which the widget is enabled. Default is 'User'.
    /// Used when designing GUIs that allow access to more and more detail according to the user mode.
    /// The user mode is set application through the QELogin widget, or programatically through setUserLevel()
    /// Widgets that are always accessable should be visible at 'User'.
    /// Widgets that are only accessable to scientists managing the facility should be visible at 'Scientist'.
    /// Widgets that are only accessable to engineers maintaining the facility should be visible at 'Engineer'.
    Q_PROPERTY(UserLevels userLevelEnabled READ getUserLevelEnabledProperty WRITE setUserLevelEnabledProperty)

    UserLevels getUserLevelVisibilityProperty() { return (UserLevels)getUserLevelVisibility(); }            ///< Access function for 'userLevelVisibility' property - refer to 'userLevelVisibility' property for details
    void setUserLevelVisibilityProperty( UserLevels level ) { setUserLevelVisibility( (userLevels)level ); }///< Access function for 'userLevelVisibility' property - refer to 'userLevelVisibility' property for details
    UserLevels getUserLevelEnabledProperty() { return (UserLevels)getUserLevelEnabled(); }                  ///< Access function for 'userLevelEnabled' property - refer to 'userLevelEnabled' property for details
    void setUserLevelEnabledProperty( UserLevels level ) { setUserLevelEnabled( (userLevels)level ); }      ///< Access function for 'userLevelEnabled' property - refer to 'userLevelEnabled' property for details

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

    /// User friendly enumerations for format property - refer to QCaStringFormatting::formats for details.
    enum Formats { Default          = QCaStringFormatting::FORMAT_DEFAULT,
                   Floating         = QCaStringFormatting::FORMAT_FLOATING,
                   Integer          = QCaStringFormatting::FORMAT_INTEGER,
                   UnsignedInteger  = QCaStringFormatting::FORMAT_UNSIGNEDINTEGER,
                   Time             = QCaStringFormatting::FORMAT_TIME,
                   LocalEnumeration = QCaStringFormatting::FORMAT_LOCAL_ENUMERATE };
    void setFormatProperty( Formats format ){ setFormat( (QCaStringFormatting::formats)format ); }  ///< Access function for 'format' property - refer to 'format' property for details
    Formats getFormatProperty(){ return (Formats)getFormat(); }                                     ///< Access function for 'format' property - refer to 'format' property for details
    Q_ENUMS(Formats)

    /// Format to apply to data. Default is 'Default' in which case the data type supplied with the data determines how the data is formatted.
    /// For all other options, an attempt is made to format the data as requested (whatever its native form).
    Q_PROPERTY(Formats format READ getFormatProperty WRITE setFormatProperty)

    /// Base used for when formatting integers. Default is 10 (duh!)
    ///
    Q_PROPERTY(unsigned int radix READ getRadix WRITE setRadix)

    /// User friendly enumerations for notation property - refer to QCaStringFormatting::notations for details.
    enum Notations { Fixed = QCaStringFormatting::NOTATION_FIXED,
                     Scientific   = QCaStringFormatting::NOTATION_SCIENTIFIC,
                     Automatic      = QCaStringFormatting::NOTATION_AUTOMATIC };
    void setNotationProperty( Notations notation ){ setNotation( (QCaStringFormatting::notations)notation ); }  ///< Access function for 'notation' property - refer to 'notation' property for details
    Notations getNotationProperty(){ return (Notations)getNotation(); }                                         ///< Access function for 'notation' property - refer to 'notation' property for details
    Q_ENUMS(Notations)
    /// Notation used for numerical formatting. Default is fixed.
    ///
    Q_PROPERTY(Notations notation READ getNotationProperty WRITE setNotationProperty)

    /// User friendly enumerations for arrayAction property - refer to QCaStringFormatting::arrayActions for details.
    enum ArrayActions { Append = QCaStringFormatting::APPEND,
                        Ascii  = QCaStringFormatting::ASCII,
                        Index  = QCaStringFormatting::INDEX };
    void setArrayActionProperty( ArrayActions arrayAction ){ setArrayAction( (QCaStringFormatting::arrayActions)arrayAction ); }    ///< Access function for 'arrayAction' property - refer to 'arrayAction' property for details
    ArrayActions getArrayActionProperty(){ return (ArrayActions)getArrayAction(); }                                                 ///< Access function for 'arrayAction' property - refer to 'arrayAction' property for details
    Q_ENUMS(ArrayActions)

    /// Text formatting option for array data. Default is ASCII. Options are:
    /// \li ASCII - treat array as a single text string. For example an array of three characters 'a' 'b' 'c' will be formatted as 'abc'.
    /// \li APPEND - treat array as an array of numbers and format a string containing them all with a space between each. For example, an array of three numbers 10, 11 and 12 will be formatted as '10 11 12'.
    /// \li INDEX - Extract a single item from the array. The item is then formatted as any other non array data would be. The item selected is determined by the arrayIndex property. For example, if arrayIndex property is 1, an array of three numbers 10, 11 and 12 will be formatted as '11'.
    Q_PROPERTY(ArrayActions arrayAction READ getArrayActionProperty WRITE setArrayActionProperty)

    /// Index used to select a single item of data for formatting from an array of data. Default is 0.
    /// Only used when the arrayAction property is INDEX. Refer to the arrayAction property for more details.
    Q_PROPERTY(unsigned int arrayIndex READ getArrayIndex WRITE setArrayIndex)
    //=================================================================================

    // Widget specific properties

public:
    // Update options (text, pixmap, or both)
    Q_ENUMS(UpdateOptions)
    /// Determines if data updates the label text, or the label pixmap.
    /// For both options all normal string formatting is applied. If Text, the formatted text is simply presented as the label text.
    /// If Picture, the FORMATTED text is then interpreted as an integer and used to select one of the pixmaps specified by properties pixmap0 through to pixmap7.
    Q_PROPERTY(UpdateOptions updateOption READ getUpdateOptionProperty WRITE setUpdateOptionProperty)
    /// User friendly enumerations for updateOption property - refer to QELabel::updateOptions for details.
    enum UpdateOptions { Text     = QELabel::UPDATE_TEXT,
                         Picture  = QELabel::UPDATE_PIXMAP };
    void setUpdateOptionProperty( UpdateOptions updateOption ){ setUpdateOption( (QELabel::updateOptions)updateOption ); }  ///< Access function for 'updateOption' property - refer to 'updateOption' property for details
    UpdateOptions getUpdateOptionProperty(){ return (UpdateOptions)getUpdateOption(); }                                     ///< Access function for 'updateOption' property - refer to 'updateOption' property for details

    // Pixmaps
    /// Pixmap displayed when updateOption property is 'Picture' and data is interpreted as 0.
    ///
    Q_PROPERTY(QPixmap pixmap0 READ getPixmap0Property WRITE setPixmap0Property)
    /// Pixmap displayed when updateOption property is 'Picture' and data is interpreted as 1.
    ///
    Q_PROPERTY(QPixmap pixmap1 READ getPixmap1Property WRITE setPixmap1Property)
    /// Pixmap displayed when updateOption property is 'Picture' and data is interpreted as 2.
    ///
    Q_PROPERTY(QPixmap pixmap2 READ getPixmap2Property WRITE setPixmap2Property)
    /// Pixmap displayed when updateOption property is 'Picture' and data is interpreted as 3.
    ///
    Q_PROPERTY(QPixmap pixmap3 READ getPixmap3Property WRITE setPixmap3Property)
    /// Pixmap displayed when updateOption property is 'Picture' and data is interpreted as 4.
    ///
    Q_PROPERTY(QPixmap pixmap4 READ getPixmap4Property WRITE setPixmap4Property)
    /// Pixmap displayed when updateOption property is 'Picture' and data is interpreted as 5.
    ///
    Q_PROPERTY(QPixmap pixmap5 READ getPixmap5Property WRITE setPixmap5Property)
    /// Pixmap displayed when updateOption property is 'Picture' and data is interpreted as 6.
    ///
    Q_PROPERTY(QPixmap pixmap6 READ getPixmap6Property WRITE setPixmap6Property)
    /// Pixmap displayed when updateOption property is 'Picture' and data is interpreted as 7.
    ///
    Q_PROPERTY(QPixmap pixmap7 READ getPixmap7Property WRITE setPixmap7Property)

    void setPixmap0Property( QPixmap pixmap ){ setDataPixmap( pixmap, 0 ); }     ///< Access function for 'pixmap0' property - refer to 'pixmap0' property for details
    void setPixmap1Property( QPixmap pixmap ){ setDataPixmap( pixmap, 1 ); }     ///< Access function for 'pixmap1' property - refer to 'pixmap1' property for details
    void setPixmap2Property( QPixmap pixmap ){ setDataPixmap( pixmap, 2 ); }     ///< Access function for 'pixmap2' property - refer to 'pixmap2' property for details
    void setPixmap3Property( QPixmap pixmap ){ setDataPixmap( pixmap, 3 ); }     ///< Access function for 'pixmap3' property - refer to 'pixmap3' property for details
    void setPixmap4Property( QPixmap pixmap ){ setDataPixmap( pixmap, 4 ); }     ///< Access function for 'pixmap4' property - refer to 'pixmap4' property for details
    void setPixmap5Property( QPixmap pixmap ){ setDataPixmap( pixmap, 5 ); }     ///< Access function for 'pixmap5' property - refer to 'pixmap5' property for details
    void setPixmap6Property( QPixmap pixmap ){ setDataPixmap( pixmap, 6 ); }     ///< Access function for 'pixmap6' property - refer to 'pixmap6' property for details
    void setPixmap7Property( QPixmap pixmap ){ setDataPixmap( pixmap, 7 ); }     ///< Access function for 'pixmap7' property - refer to 'pixmap7' property for details

    QPixmap getPixmap0Property(){ return getDataPixmap( 0 ); }     ///< Access function for 'pixmap0' property - refer to 'pixmap0' property for details
    QPixmap getPixmap1Property(){ return getDataPixmap( 1 ); }     ///< Access function for 'pixmap1' property - refer to 'pixmap1' property for details
    QPixmap getPixmap2Property(){ return getDataPixmap( 2 ); }     ///< Access function for 'pixmap2' property - refer to 'pixmap2' property for details
    QPixmap getPixmap3Property(){ return getDataPixmap( 3 ); }     ///< Access function for 'pixmap3' property - refer to 'pixmap3' property for details
    QPixmap getPixmap4Property(){ return getDataPixmap( 4 ); }     ///< Access function for 'pixmap4' property - refer to 'pixmap4' property for details
    QPixmap getPixmap5Property(){ return getDataPixmap( 5 ); }     ///< Access function for 'pixmap5' property - refer to 'pixmap5' property for details
    QPixmap getPixmap6Property(){ return getDataPixmap( 6 ); }     ///< Access function for 'pixmap6' property - refer to 'pixmap6' property for details
    QPixmap getPixmap7Property(){ return getDataPixmap( 7 ); }     ///< Access function for 'pixmap7' property - refer to 'pixmap7' property for details


};

#endif // QELABEL_H
