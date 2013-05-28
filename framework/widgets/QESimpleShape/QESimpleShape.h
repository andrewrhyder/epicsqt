/*  QESimpleShape.h
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

#ifndef QESIMPLESHAPE_H
#define QESIMPLESHAPE_H

#include <QString>
#include <QVector>
#include <QCaObject.h>
#include <QEWidget.h>
#include <QEInteger.h>
#include <QEIntegerFormatting.h>
#include <QCaVariableNamePropertyManager.h>
#include <QEPluginLibrary_global.h>
#include <QEStringFormattingMethods.h>

/*!
  This class is an EPICS aware simple shape.
  The shape is selected at design time using the shape property - refer to #Shapes for details, or may be set by the setShape function.
  The colour is based on either the value of or the alarm state of a single PV.
  When the (standard) displayAlarmState property is false, the PV value (modulo 16) is used to select one of the colours defined by the colour0 to colour15 properties.
  When the displayAlarmState property is true, the colour is based on the alarm state, i.e. green when no alarm, yellow for minor alarm etc.
*/
class QEPLUGINLIBRARYSHARED_EXPORT QESimpleShape :
      public QWidget, public QEWidget, public QEStringFormattingMethods  {

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


    // QESimpleShape specific properties ==============================================
    //
public:
    //----------------------------------------------------------------------------------
    //
    enum Shapes { circle, ellipse, rectangle, roundRectangle, roundSquare, square,
                  triangleUp, triangleDown, triangleLeft, triangleRight,
                  triangleTopRight, triangleBottomRight, triangleBottomLeft, triangleTopLeft,
                  diamond, equalDiamond };
    Q_ENUMS (Shapes)

    /// Nominated shape
    ///
    Q_PROPERTY (Shapes shape READ getShape WRITE setShape)

    void setShape (Shapes value);   ///< Access function for #shape property - refer to #Shapes  property for details
    Shapes getShape ();             ///< Access function for #shape property - refer to #Shapes  property for details


    //----------------------------------------------------------------------------------
    //
    enum TextFormats { FixedText,           ///< Use user specified fixed text (default)
                       PvText,              ///< Use EPICS value agumented with units if selected.
                       LocalEnumeration };  ///< Use specied enumeration values, PV value (modulo 16) used to select item
    Q_ENUMS (TextFormats)

    /// Nominated text format
    ///
    Q_PROPERTY (TextFormats format READ getTextFormat WRITE setTextFormat)

    void setTextFormat (TextFormats value);   ///< Access function for #textMode property - refer to #TextFormats  property for details
    TextFormats getTextFormat ();             ///< Access function for #textMode property - refer to #TextFormats  property for details


    //----------------------------------------------------------------------------------
    /// User specified text - defaults to null string.
    Q_PROPERTY (QString fixedText READ getFixedText WRITE setFixedText)

    void setFixedText (QString value);
    QString getFixedText ();


    //----------------------------------------------------------------------------------
    /// If true (default), add engineering units supplied with the data.
    ///
    Q_PROPERTY(bool addUnits READ getAddUnits WRITE setAddUnits)


    //----------------------------------------------------------------------------------
    // NOTE, keep in sync. The documentation below is repeated in QEStringFormatting::setLocalEnumeration() (in QEStringformatting.cpp)
    /// An enumeration list used to data values. Used only when the formatting option is 'local enumeration'.
    /// Value is converted to an integer and used to select a string from this list.
    ///
    /// Format is:
    ///
    ///   [[<|<=|=|!=|>=|>]value1|*] : string1 , [[<|<=|=|!=|>=|>]value2|*] : string2 , [[<|<=|=|!=|>=|>]value3|*] : string3 , ...
    ///
    /// Where:
    ///   <  Less than
    ///   <= Less than or equal
    ///   =  Equal (default if no operator specified)
    ///   >= Greather than or equal
    ///   >  Greater than
    ///   *  Always match (used to specify default text)
    ///
    /// Values may be numeric or textual
    /// Values do not have to be in any order, but first match wins
    /// Values may be quoted
    /// Strings may be quoted
    /// Consecutive values do not have to be present.
    /// Operator is assumed to be equality if not present.
    /// White space is ignored except within quoted strings.
    /// \n may be included in a string to indicate a line break
    ///
    /// Examples are:
    ///
    /// 0:Off,1:On
    /// 0 : "Pump Running", 1 : "Pump not running"
    /// 0:"", 1:"Warning!\nAlarm"
    /// <2:"Value is less than two", =2:"Value is equal to two", >2:"Value is grater than 2"
    /// 3:"Beamline Available", *:""
    /// "Pump Off":"OH NO!, the pump is OFF!","Pump On":"It's OK, the pump is on"
    ///
    /// The data value is converted to a string if no enumeration for that value is available.
    /// For example, if the local enumeration is '0:off,1:on', and a value of 10 is processed, the text generated is '10'.
    /// If a blank string is required, this should be explicit. for example, '0:off,1:on,10:""'
    ///
    /// A range of numbers can be covered by a pair of values as in the following example: >=4:"Between 4 and 8",<=8:"Between 4 and 8"
    Q_PROPERTY(QString/*localEnumerationList*/ localEnumeration READ getLocalEnumeration WRITE setLocalEnumeration)


    //----------------------------------------------------------------------------------
    /// When displayAlarmState is true then widget colour refect PV alarm state.
    /// When displayAlarmState is false, PV value (modulo 16) used to select colour
    ///
    Q_PROPERTY (QColor  colour0    READ getColour0Property        WRITE setColour0Property)
    Q_PROPERTY (QColor  colour1    READ getColour1Property        WRITE setColour1Property)
    Q_PROPERTY (QColor  colour2    READ getColour2Property        WRITE setColour2Property)
    Q_PROPERTY (QColor  colour3    READ getColour3Property        WRITE setColour3Property)
    Q_PROPERTY (QColor  colour4    READ getColour4Property        WRITE setColour4Property)
    Q_PROPERTY (QColor  colour5    READ getColour5Property        WRITE setColour5Property)
    Q_PROPERTY (QColor  colour6    READ getColour6Property        WRITE setColour6Property)
    Q_PROPERTY (QColor  colour7    READ getColour7Property        WRITE setColour7Property)
    Q_PROPERTY (QColor  colour8    READ getColour8Property        WRITE setColour8Property)
    Q_PROPERTY (QColor  colour9    READ getColour9Property        WRITE setColour9Property)
    Q_PROPERTY (QColor  colour10   READ getColour10Property       WRITE setColour10Property)
    Q_PROPERTY (QColor  colour11   READ getColour11Property       WRITE setColour11Property)
    Q_PROPERTY (QColor  colour12   READ getColour12Property       WRITE setColour12Property)
    Q_PROPERTY (QColor  colour13   READ getColour13Property       WRITE setColour13Property)
    Q_PROPERTY (QColor  colour14   READ getColour14Property       WRITE setColour14Property)
    Q_PROPERTY (QColor  colour15   READ getColour15Property       WRITE setColour15Property)

    // Property access READ and WRITE functions.
    // We can define the access functions using a macro.
    // Alas, due to SDK limitation, we cannot embedded the property definition itself in a macro.
    //
    #define PROPERTY_ACCESS(slot)                                                                              \
       void   setColour##slot##Property (QColor colour) { this->setColourProperty (slot, colour); }        \
       QColor getColour##slot##Property ()       { return this->getColourProperty (slot); }

    PROPERTY_ACCESS  (0)
    PROPERTY_ACCESS  (1)
    PROPERTY_ACCESS  (2)
    PROPERTY_ACCESS  (3)
    PROPERTY_ACCESS  (4)
    PROPERTY_ACCESS  (5)
    PROPERTY_ACCESS  (6)
    PROPERTY_ACCESS  (7)
    PROPERTY_ACCESS  (8)
    PROPERTY_ACCESS  (9)
    PROPERTY_ACCESS  (10)
    PROPERTY_ACCESS  (11)
    PROPERTY_ACCESS  (12)
    PROPERTY_ACCESS  (13)
    PROPERTY_ACCESS  (14)
    PROPERTY_ACCESS  (15)

   #undef PROPERTY_ACCESS
   //
   // End of QESimpleShape specific properties =====================================

public:
    /// Create without a variable.
    /// Use setVariableNameProperty() and setSubstitutionsProperty() to define a variable and, optionally, macro substitutions later.
    ///
    QESimpleShape( QWidget *parent = 0 );

    /// Create with a variable.
    /// A connection is automatically established.
    /// If macro substitutions are required, create without a variable and set the variable and macro substitutions after creation.
    ///
    QESimpleShape( const QString &variableName, QWidget *parent = 0 );

    /// Destruction
    virtual ~QESimpleShape(){}

    /// Extract the stores value.
    ///
    int getValue ();

    /// Extract the stored value which is PV value modulo 16.
    ///
    int getModuloValue ();

protected:
    QEIntegerFormatting integerFormatting;

    void establishConnection( unsigned int variableIndex );
    void stringFormattingChange() { this->update (); }

private:
    Shapes shape;
    TextFormats textFormat;
    QString fixedText;

    QString textImage;
    void setTextImage ();
    QString getTextImage ();   // text image to be used.

    QColor colourList [16];
    int value;
    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;
    bool isFirstUpdate;

    void setup();
    qcaobject::QCaObject* createQcaItem (unsigned int variableIndex);

    void equaliseRect (QRect & rect);

    // Like painter.drawText, but centred on textCentre.
    // (painter.drawText aligns bottom left corner on given point).
    //
    void drawText (QPainter & painter, QPoint & textCentre, QString & text);
    void paintEvent (QPaintEvent *event);

    void   setColourProperty (int slot, QColor color);
    QColor getColourProperty (int slot);

private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );

    void setShapeValue( const long& value, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );

    // Just in case hooked up to an array PV.
    //
    void setShapeValues ( const QVector<long>& values, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );

    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );  // !! move into Standard Properties section??

signals:
    // Note, the following signals are common to many QE widgets,
    // if changing the doxygen comments, ensure relevent changes are migrated to all instances
    /// Sent when the widget is updated following a data change
    /// Can be used to pass on EPICS data (as presented in this widget) to other widgets.
    /// For example a QList widget could log updates from this widget.
    void dbValueChanged( const long& out );

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

#endif // QESIMPLESHAPE_H
