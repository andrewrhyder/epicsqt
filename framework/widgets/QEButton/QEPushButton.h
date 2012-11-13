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

#ifndef QEPUSHBUTTON_H
#define QEPUSHBUTTON_H

#include <QPushButton>
#include <QEWidget.h>
#include <QEString.h>
#include <QEStringFormatting.h>
#include <QEPluginLibrary_global.h>
#include <QEForm.h>
#include <UserMessage.h>
#include <ContainerProfile.h>
#include <managePixmaps.h>
#include <QEGenericButton.h>
#include <QCaVariableNamePropertyManager.h>


class QEPLUGINLIBRARYSHARED_EXPORT QEPushButton : public QPushButton, public QEGenericButton {
    Q_OBJECT

  public:
    QEPushButton( QWidget *parent = 0 );
    QEPushButton( const QString& variableName, QWidget *parent = 0 );

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo ) { QEGenericButton::connectionChanged( connectionInfo ); }
    void setButtonText( const QString& text, QCaAlarmInfo& alarmInfo, QCaDateTime& timestamp, const unsigned int& variableIndex ) { setGenericButtonText( text, alarmInfo, timestamp, variableIndex); }
    void userPressed() { QEGenericButton::userPressed(); }
    void userReleased() { QEGenericButton::userReleased(); }
    void userClicked( bool checked ) { QEGenericButton::userClicked( checked ); }

public slots:
    void launchGui( QString guiName, QEForm::creationOptions creationOption ){ QEGenericButton::launchGui( guiName, creationOption); }
    void onGeneralMessage( QString message ){ QEGenericButton::onGeneralMessage( message ); }


  signals:
    // Note, the following signals are common to many QE widgets,
    // if changing the doxygen comments, ensure relevent changes are migrated to all instances
    /// Sent when the widget is updated following a data change
    /// Can be used to pass on EPICS data (as presented in this widget) to other widgets.
    /// For example a QList widget could log updates from this widget.
    void dbValueChanged( const QString& out );
    /// Internal use only. Used when changing a property value to force a re-display to reflect the new property value.
    void requestResend();

    /// Internal use only. Request a new GUI is created. Typically, this is caught by the QEGui application.
    void newGui( QString guiName, QEForm::creationOptions creationOption );

  protected:

private:
    void setup();
    void updateToolTip ( const QString & toolTip );

    void setButtonState( bool checked ){ QPushButton::setChecked( checked ); }
    void setButtonText( QString text ){ QPushButton::setText( text ); }
    QString getButtonText(){ return text(); }
    void setButtonIcon( QIcon& icon ) {setIcon( icon ); }

    void setButtonStyleSheet( QString style ){ setStyleSheet( style ); }

    void emitDbValueChanged( QString text ){ emit dbValueChanged( text ); }

    void emitNewGui( QString guiName, QEForm::creationOptions creationOption  ){ emit newGui( guiName, creationOption); }

    void connectButtonDataChange( qcaobject::QCaObject* qca )
    {
                QObject::connect( qca,  SIGNAL( stringChanged( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                                  this, SLOT( setButtonText( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
                QObject::connect( this, SIGNAL( requestResend() ),
                                  qca, SLOT( resendLastData() ) );
    }

    QObject* getButtonQObject(){ return this; }

    void stringFormattingChange(){ requestResend(); }

    QEGenericButton::updateOptions getDefaultUpdateOption() { return QEGenericButton::UPDATE_TEXT; }

    // Drag and Drop
protected:
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    // Don't drag from interactive widget void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDrop( QVariant drop );
    QVariant getDrop();


public:

    //=================================================================================
    // Multiple Variable properties
    // These properties should be similar for every widget using multiple variables (The number of variables may vary).
    // WHEN MAKING CHANGES: search for MULTIPLEVARIABLEPROPERTIESBASE and change all occurances.
    private:
        QCaVariableNamePropertyManager variableNamePropertyManagers[QEGENERICBUTTON_NUM_VARIABLES];
    public:

    // Define a variable
    // Note, the QPROPERTY declaration itself can't be in this macro
#define VARIABLE_PROPERTY_ACCESS(VAR_INDEX) \
    void    setVariableName##VAR_INDEX##Property( QString variableName ){ variableNamePropertyManagers[VAR_INDEX].setVariableNameProperty( variableName ); } \
    QString getVariableName##VAR_INDEX##Property(){ return variableNamePropertyManagers[VAR_INDEX].getVariableNameProperty(); }

    VARIABLE_PROPERTY_ACCESS(0)
    /// EPICS variable name (CA PV).
    /// This variable is used for both writing (on button press), and reading if subscribed and no alternate readback variable is provided.
    Q_PROPERTY(QString variable READ getVariableName0Property WRITE setVariableName0Property)

    VARIABLE_PROPERTY_ACCESS(1)
    /// EPICS variable name (CA PV).
    /// This variable is used to provide a readback value when different to the variable written to by a button press.
    Q_PROPERTY(QString altReadbackVariable READ getVariableName1Property WRITE setVariableName1Property)

    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)
    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions )
    {
        for( int i = 0; i < QEGENERICBUTTON_NUM_VARIABLES; i++ )
        {
            variableNamePropertyManagers[i].setSubstitutionsProperty( variableNameSubstitutions );
        }
    }
    QString getVariableNameSubstitutionsProperty()
    {
        return variableNamePropertyManagers[0].getSubstitutionsProperty();
    }

private slots:
  void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )
  {
      setVariableNameAndSubstitutions(variableNameIn, variableNameSubstitutionsIn, variableIndex);
  }

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


    // Widget specific properties

    // Update options (text, pixmap, or both)
    Q_ENUMS(UpdateOptions)
    Q_PROPERTY(UpdateOptions updateOption READ getUpdateOptionProperty WRITE setUpdateOptionProperty)
    enum UpdateOptions { Text        = QEPushButton::UPDATE_TEXT,
                         Icon        = QEPushButton::UPDATE_ICON,
                         TextAndIcon = QEPushButton::UPDATE_TEXT_AND_ICON,
                         State       = QEPushButton::UPDATE_STATE };
    void setUpdateOptionProperty( UpdateOptions updateOption ){ setUpdateOption( (QEPushButton::updateOptions)updateOption ); }
    UpdateOptions getUpdateOptionProperty(){ return (UpdateOptions)getUpdateOption(); }

    // Pixmaps
    Q_PROPERTY(QPixmap pixmap0 READ getPixmap0Property WRITE setPixmap0Property)
    Q_PROPERTY(QPixmap pixmap1 READ getPixmap1Property WRITE setPixmap1Property)
    Q_PROPERTY(QPixmap pixmap2 READ getPixmap2Property WRITE setPixmap2Property)
    Q_PROPERTY(QPixmap pixmap3 READ getPixmap3Property WRITE setPixmap3Property)
    Q_PROPERTY(QPixmap pixmap4 READ getPixmap4Property WRITE setPixmap4Property)
    Q_PROPERTY(QPixmap pixmap5 READ getPixmap5Property WRITE setPixmap5Property)
    Q_PROPERTY(QPixmap pixmap6 READ getPixmap6Property WRITE setPixmap6Property)
    Q_PROPERTY(QPixmap pixmap7 READ getPixmap7Property WRITE setPixmap7Property)

    void setPixmap0Property( QPixmap pixmap ){ setDataPixmap( pixmap, 0 ); }
    void setPixmap1Property( QPixmap pixmap ){ setDataPixmap( pixmap, 1 ); }
    void setPixmap2Property( QPixmap pixmap ){ setDataPixmap( pixmap, 2 ); }
    void setPixmap3Property( QPixmap pixmap ){ setDataPixmap( pixmap, 3 ); }
    void setPixmap4Property( QPixmap pixmap ){ setDataPixmap( pixmap, 4 ); }
    void setPixmap5Property( QPixmap pixmap ){ setDataPixmap( pixmap, 5 ); }
    void setPixmap6Property( QPixmap pixmap ){ setDataPixmap( pixmap, 6 ); }
    void setPixmap7Property( QPixmap pixmap ){ setDataPixmap( pixmap, 7 ); }

    QPixmap getPixmap0Property(){ return getDataPixmap( 0 ); }
    QPixmap getPixmap1Property(){ return getDataPixmap( 1 ); }
    QPixmap getPixmap2Property(){ return getDataPixmap( 2 ); }
    QPixmap getPixmap3Property(){ return getDataPixmap( 3 ); }
    QPixmap getPixmap4Property(){ return getDataPixmap( 4 ); }
    QPixmap getPixmap5Property(){ return getDataPixmap( 5 ); }
    QPixmap getPixmap6Property(){ return getDataPixmap( 6 ); }
    QPixmap getPixmap7Property(){ return getDataPixmap( 7 ); }

    // String formatting properties
    Q_PROPERTY(unsigned int precision READ getPrecision WRITE setPrecision)
    Q_PROPERTY(bool useDbPrecision READ getUseDbPrecision WRITE setUseDbPrecision)
    Q_PROPERTY(bool leadingZero READ getLeadingZero WRITE setLeadingZero)
    Q_PROPERTY(bool trailingZeros READ getTrailingZeros WRITE setTrailingZeros)
    Q_PROPERTY(bool addUnits READ getAddUnits WRITE setAddUnits)
    Q_PROPERTY(QString/*localEnumerationList*/ localEnumeration READ getLocalEnumeration WRITE setLocalEnumeration)
    Q_PROPERTY(Qt::Alignment alignment READ getTextAlignment WRITE setTextAlignment )

    Q_ENUMS(Formats)
    Q_PROPERTY(Formats format READ getFormatProperty WRITE setFormatProperty)
    enum Formats { Default         = QEStringFormatting::FORMAT_DEFAULT,
                   Floating        = QEStringFormatting::FORMAT_FLOATING,
                   Integer         = QEStringFormatting::FORMAT_INTEGER,
                   UnsignedInteger = QEStringFormatting::FORMAT_UNSIGNEDINTEGER,
                   Time            = QEStringFormatting::FORMAT_TIME,
                   LocalEnumeration = QEStringFormatting::FORMAT_LOCAL_ENUMERATE };
    void setFormatProperty( Formats format ){ setFormat( (QEStringFormatting::formats)format ); }
    Formats getFormatProperty(){ return (Formats)getFormat(); }

    Q_ENUMS(Notations)
    Q_PROPERTY(Notations notation READ getNotationProperty WRITE setNotationProperty)
    enum Notations { Fixed      = QEStringFormatting::NOTATION_FIXED,
                     Scientific = QEStringFormatting::NOTATION_SCIENTIFIC,
                     Automatic  = QEStringFormatting::NOTATION_AUTOMATIC };
    void setNotationProperty( Notations notation ){ setNotation( (QEStringFormatting::notations)notation ); }
    Notations getNotationProperty(){ return (Notations)getNotation(); }

    Q_PROPERTY(QString password READ getPassword WRITE setPassword)

    Q_PROPERTY(bool writeOnPress READ getWriteOnPress WRITE setWriteOnPress)
    Q_PROPERTY(bool writeOnRelease READ getWriteOnRelease WRITE setWriteOnRelease)
    Q_PROPERTY(bool writeOnClick READ getWriteOnClick WRITE setWriteOnClick)

    Q_PROPERTY(QString pressText READ getPressText WRITE setPressText)
    Q_PROPERTY(QString releaseText READ getReleaseText WRITE setReleaseText)
    Q_PROPERTY(QString clickText READ getClickText WRITE setClickText)
    Q_PROPERTY(QString clickCheckedText READ getClickCheckedText WRITE setClickCheckedText)

    Q_PROPERTY(QString labelText READ getLabelTextProperty WRITE setLabelTextProperty);

    Q_PROPERTY(QString program READ getProgram WRITE setProgram)
    Q_PROPERTY(QStringList arguments READ getArguments WRITE setArguments)



    // Note, a property macro in the form 'Q_PROPERTY(QString guiName READ ...' doesn't work.
    // A property name ending with 'Name' results in some sort of string variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
    Q_PROPERTY(QString guiFile READ getGuiName WRITE setGuiName)

    // Creation options
    Q_ENUMS(CreationOptionNames)
    Q_PROPERTY(CreationOptionNames creationOption READ getCreationOptionProperty WRITE setCreationOptionProperty)
    enum CreationOptionNames { Open = QEForm::CREATION_OPTION_OPEN,
                               NewTab = QEForm::CREATION_OPTION_NEW_TAB,
                               NewWindow = QEForm::CREATION_OPTION_NEW_WINDOW };
    void setCreationOptionProperty( CreationOptionNames creationOptionIn ){ setCreationOption( (QEForm::creationOptions)creationOptionIn ); }
    CreationOptionNames getCreationOptionProperty(){ return (CreationOptionNames)getCreationOption(); }

    Q_PROPERTY(unsigned int messageSourceId READ getMessageSourceId WRITE setMessageSourceId )

};

#endif // QEPUSHBUTTON_H
