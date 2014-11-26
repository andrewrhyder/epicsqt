/*  QESimpleShape.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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
 *  Copyright (c) 2013 Australian Synchrotron
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef QE_SIMPLE_SHAPE_H
#define QE_SIMPLE_SHAPE_H

#include <QString>

#include <QCaObject.h>
#include <QCaVariableNamePropertyManager.h>
#include <QEFrame.h>
#include <QEInteger.h>
#include <QEIntegerFormatting.h>
#include <QEPluginLibrary_global.h>
#include <QEStringFormattingMethods.h>
#include <QEScanTimers.h>

/*!
  This class is an EPICS aware simple shape.
  The shape is selected at design time using the shape property - refer to #Shapes for details, or may be set by the setShape function.
  The colour is based on either the value of or the alarm state of a single PV.
  When the (standard) displayAlarmState property is false, the PV value (modulo 16) is used to select one of the colours defined by the colour0 to colour15 properties.
  When the displayAlarmState property is true, the colour is based on the alarm state, i.e. green when no alarm, yellow for minor alarm etc.
*/
class QEPLUGINLIBRARYSHARED_EXPORT QESimpleShape :
      public QEFrame, public QEStringFormattingMethods  {

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

    /// Property access function for #variable property. This has special behaviour to work well within designer.
    void    setVariableNameProperty( QString variableName ){ variableNamePropertyManager.setVariableNameProperty( variableName ); }
    /// Property access function for #variable property. This has special behaviour to work well within designer.
    QString getVariableNameProperty(){ return variableNamePropertyManager.getVariableNameProperty(); }

    /// Property access function for #variableSubstitutions property. This has special behaviour to work well within designer.
    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions ){ variableNamePropertyManager.setSubstitutionsProperty( variableNameSubstitutions ); }
    /// Property access function for #variableSubstitutions property. This has special behaviour to work well within designer.
    QString getVariableNameSubstitutionsProperty(){ return variableNamePropertyManager.getSubstitutionsProperty(); }

private:
    QCaVariableNamePropertyManager variableNamePropertyManager;
public:
    // END-SINGLE-VARIABLE-PROPERTIES =================================================

    // QESimpleShape specific properties ==============================================
    //
public:
    /// Array Index element to display if variable is a waveform. Defaults to 0.
    ///
    Q_PROPERTY (int arrayIndex   READ getArrayIndex   WRITE setArrayIndex )

    void setArrayIndex (const int arrayIndexIn);
    int getArrayIndex () const;

    //----------------------------------------------------------------------------------
    //
    enum Shapes { circle, ellipse, rectangle, roundRectangle, roundSquare, square,
                  triangleUp, triangleDown, triangleLeft, triangleRight,
                  triangleTopRight, triangleBottomRight, triangleBottomLeft, triangleTopLeft,
                  diamond, equalDiamond, arrowUp, arrowDown, arrowLeft, arrowRight };
    Q_ENUMS (Shapes)

    /// Nominated shape
    ///
    Q_PROPERTY (Shapes shape READ getShape WRITE setShape)

    void setShape (Shapes value);   ///< Access function for #shape property - refer to #Shapes  property for details
    Shapes getShape () const;       ///< Access function for #shape property - refer to #Shapes  property for details


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
    TextFormats getTextFormat () const;       ///< Access function for #textMode property - refer to #TextFormats  property for details


    //----------------------------------------------------------------------------------
    /// User specified text - defaults to null string.
    Q_PROPERTY (QString fixedText READ getFixedText WRITE setFixedText)

    void setFixedText (QString value);
    QString getFixedText () const;


    //----------------------------------------------------------------------------------
    /// If true (default), add engineering units supplied with the data.
    ///
    Q_PROPERTY(bool addUnits READ getAddUnits WRITE setAddUnits)


    //----------------------------------------------------------------------------------
    /// Flash rate.
    /// The default value is Medium, i.e. 1Hz flash rate.
    Q_PROPERTY(QEScanTimers::ScanRates flashRate READ getFlashRate WRITE setFlashRate)

    void setFlashRate (QEScanTimers::ScanRates);
    QEScanTimers::ScanRates getFlashRate () const;


    //----------------------------------------------------------------------------------
    /// Flash offColour - default is clear.
    Q_PROPERTY(QColor flashOffColour READ getFlashOffColour WRITE setFlashOffColour)

    void setFlashOffColour (const QColor& colour);
    QColor getFlashOffColour () const;


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

    //----------------------------------------------------------------------------------
    /// When the widget's state coresponds to N and flashN is set true, the widget will
    /// alternate its normal colour with the specified flashOffColour.
    /// This applies irrespective displayAlarmColors value.
    ///
    Q_PROPERTY (bool    flash0     READ getFlash0Property         WRITE setFlash0Property)
    Q_PROPERTY (bool    flash1     READ getFlash1Property         WRITE setFlash1Property)
    Q_PROPERTY (bool    flash2     READ getFlash2Property         WRITE setFlash2Property)
    Q_PROPERTY (bool    flash3     READ getFlash3Property         WRITE setFlash3Property)
    Q_PROPERTY (bool    flash4     READ getFlash4Property         WRITE setFlash4Property)
    Q_PROPERTY (bool    flash5     READ getFlash5Property         WRITE setFlash5Property)
    Q_PROPERTY (bool    flash6     READ getFlash6Property         WRITE setFlash6Property)
    Q_PROPERTY (bool    flash7     READ getFlash7Property         WRITE setFlash7Property)
    Q_PROPERTY (bool    flash8     READ getFlash8Property         WRITE setFlash8Property)
    Q_PROPERTY (bool    flash9     READ getFlash9Property         WRITE setFlash9Property)
    Q_PROPERTY (bool    flash10    READ getFlash10Property        WRITE setFlash10Property)
    Q_PROPERTY (bool    flash11    READ getFlash11Property        WRITE setFlash11Property)
    Q_PROPERTY (bool    flash12    READ getFlash12Property        WRITE setFlash12Property)
    Q_PROPERTY (bool    flash13    READ getFlash13Property        WRITE setFlash13Property)
    Q_PROPERTY (bool    flash14    READ getFlash14Property        WRITE setFlash14Property)
    Q_PROPERTY (bool    flash15    READ getFlash15Property        WRITE setFlash15Property)

    // Property access READ and WRITE functions.
    // We can define the access functions using a macro.
    // Alas, due to SDK limitation, we cannot embedded the property definition itself in a macro.
    //
    #define PROPERTY_ACCESS(slot)                                                                          \
       void   setColour##slot##Property (QColor colour) { this->setColourProperty (slot, colour); }        \
       QColor getColour##slot##Property () const { return this->getColourProperty (slot); }                \
                                                                                                           \
       void  setFlash##slot##Property (bool flash)    { this->setFlashProperty (slot, flash); }            \
       bool  getFlash##slot##Property () const { return this->getFlashProperty (slot); }

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
    QESimpleShape (QWidget *parent = 0);

    /// Create with a variable.
    /// A connection is automatically established.
    /// If macro substitutions are required, create without a variable and set the variable and macro substitutions after creation.
    ///
    QESimpleShape (const QString &variableName, QWidget *parent = 0);

    /// Destruction
    virtual ~QESimpleShape () {}

    /// Extract the stores value.
    ///
    int getValue () const;

    /// Extract the stored value which is PV value modulo 16.
    ///
    int getModuloValue () const;

protected:
    QEIntegerFormatting integerFormatting;

    void establishConnection (unsigned int variableIndex );
    void stringFormattingChange() { this->update (); }

private:
    Shapes shape;
    TextFormats textFormat;
    QString fixedText;
    QColor flashOffColour;
    QEScanTimers::ScanRates flashRate;
    bool flashStateIsOn;

    QString textImage;
    void setTextImage ();
    QString getTextImage ();   // text image to be used.

    QColor colourList [16];
    bool   flashList [16];
    bool isStaticValue;   // as opposed to is PV value.
    int arrayIndex;
    int value;
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
    QColor getColourProperty (int slot) const;

    void  setFlashProperty (int slot, bool color);
    bool  getFlashProperty (int slot) const;

private slots:
    void connectionChanged (QCaConnectionInfo& connectionInfo,
                            const unsigned int& variableIndex);

    void setShapeValue (const long& value, QCaAlarmInfo&, QCaDateTime&, const unsigned int&);

    void useNewVariableNameProperty (QString variableNameIn,
                                     QString variableNameSubstitutionsIn,
                                     unsigned int variableIndex);

    void flashTimeout (const bool);

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
    // Use default QEWidget drag/drop actions.

    // Copy paste
    QString copyVariable();
    QVariant copyData();
    void paste( QVariant s );
};

#endif // QE_SIMPLE_SHAPE_H
