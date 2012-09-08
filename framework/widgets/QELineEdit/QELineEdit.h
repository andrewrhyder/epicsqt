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
#include <QCaWidget.h>
#include <QCaString.h>
#include <QCaStringFormatting.h>
#include <QCaPluginLibrary_global.h>
#include <QCaStringFormattingMethods.h>
#include <QCaVariableNamePropertyManager.h>

class QCAPLUGINLIBRARYSHARED_EXPORT QELineEdit :
      public QLineEdit, public QCaWidget, public QCaStringFormattingMethods {

    Q_OBJECT

  public:
      //=================================================================================
      // Single Variable properties
      // These properties should be identical for every widget using a single variable.
      // WHEN MAKING CHANGES: search for SINGLEVARIABLEPROPERTIES and change all occurances.
      //
      // Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
      // A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
      Q_PROPERTY(QString variable READ getVariableNameProperty WRITE setVariableNameProperty);
      Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)

      void    setVariableNameProperty( QString variableName ){ variableNamePropertyManager.setVariableNameProperty( variableName ); }
      QString getVariableNameProperty(){ return variableNamePropertyManager.getVariableNameProperty(); }

      void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions ){ variableNamePropertyManager.setSubstitutionsProperty( variableNameSubstitutions ); }
      QString getVariableNameSubstitutionsProperty(){ return variableNamePropertyManager.getSubstitutionsProperty(); }

  private:
      QCaVariableNamePropertyManager variableNamePropertyManager;
  public:
      //=================================================================================

      Q_PROPERTY(bool subscribe READ getSubscribe WRITE setSubscribe)

    //=================================================================================
    // Standard properties
    // These properties should be identical for every widget using them.
    // WHEN MAKING CHANGES: search for STANDARDPROPERTIES and change all occurances.
      bool isEnabled() const { return getApplicationEnabled(); }
      void setEnabled( bool state ){ setApplicationEnabled( state ); }
    Q_PROPERTY(bool variableAsToolTip READ getVariableAsToolTip WRITE setVariableAsToolTip)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
    Q_PROPERTY(bool allowDrop READ getAllowDrop WRITE setAllowDrop)
    Q_PROPERTY(bool visible READ getRunVisible WRITE setRunVisible)
    Q_PROPERTY(unsigned int messageSourceId READ getMessageSourceId WRITE setMessageSourceId )
    Q_PROPERTY(QString userLevelUserStyle READ getStyleUser WRITE setStyleUser)
    Q_PROPERTY(QString userLevelScientistStyle READ getStyleScientist WRITE setStyleScientist)
    Q_PROPERTY(QString userLevelEngineerStyle READ getStyleEngineer WRITE setStyleEngineer)
    Q_ENUMS(UserLevels)
    Q_PROPERTY(UserLevels userLevelVisibility READ getUserLevelVisibilityProperty WRITE setUserLevelVisibilityProperty)
    Q_PROPERTY(UserLevels userLevelEnabled READ getUserLevelEnabledProperty WRITE setUserLevelEnabledProperty)
    //=================================================================================

    //=================================================================================
    // String formatting properties
    // These properties should be identical for every widget managing strings.
    // WHEN MAKING CHANGES: search for STRINGPROPERTIES and change all occurances.
    Q_PROPERTY(int  precision READ getPrecision WRITE setPrecision)
    Q_PROPERTY(bool useDbPrecision READ getUseDbPrecision WRITE setUseDbPrecision)
    Q_PROPERTY(bool leadingZero READ getLeadingZero WRITE setLeadingZero)
    Q_PROPERTY(bool trailingZeros READ getTrailingZeros WRITE setTrailingZeros)
    Q_PROPERTY(bool addUnits READ getAddUnits WRITE setAddUnits)
    Q_PROPERTY(QString/*localEnumerationList*/ localEnumeration READ getLocalEnumeration WRITE setLocalEnumeration)
    Q_ENUMS(Formats)
    Q_PROPERTY(Formats format READ getFormatProperty WRITE setFormatProperty)
    Q_PROPERTY(unsigned int radix READ getRadix WRITE setRadix)
    Q_ENUMS(Notations)
    Q_PROPERTY(Notations notation READ getNotationProperty WRITE setNotationProperty)
    Q_ENUMS(ArrayActions)
    Q_PROPERTY(ArrayActions arrayAction READ getArrayActionProperty WRITE setArrayActionProperty)
    Q_PROPERTY(unsigned int arrayIndex READ getArrayIndex WRITE setArrayIndex)
    //=================================================================================

    // widget specific properties
    //
    Q_PROPERTY(bool writeOnLoseFocus READ getWriteOnLoseFocus WRITE setWriteOnLoseFocus)
    Q_PROPERTY(bool writeOnEnter READ getWriteOnEnter WRITE setWriteOnEnter)
    Q_PROPERTY(bool writeOnFinish READ getWriteOnFinish WRITE setWriteOnFinish)
    Q_PROPERTY(bool confirmWrite READ getConfirmWrite WRITE setConfirmWrite)

public:
    QELineEdit( QWidget *parent = 0 );
    QELineEdit( const QString &variableName, QWidget *parent = 0 );

    // Property convenience functions

    // write on lose focus
    void setWriteOnLoseFocus( bool writeOnLoseFocus );
    bool getWriteOnLoseFocus();

    // write on enter
    void setWriteOnEnter( bool writeOnEnter );
    bool getWriteOnEnter();

    // write on finish
    void setWriteOnFinish( bool writeOnFinish );
    bool getWriteOnFinish();

    // subscribe
    void setSubscribe( bool subscribe );
    bool getSubscribe();

    // confirm write
    void setConfirmWrite( bool confirmWrite );
    bool getConfirmWrite();

protected:
    bool writeOnLoseFocus;        /// Write changed value to database when widget object loses focus (user moves from widget)
    bool writeOnEnter;            /// Write changed value to database when enter is pressed with focus on the widget
    bool writeOnFinish;           /// Write changed value to database when user finishes editing (leaves a widget)
    bool localEnabled;            /// Override the default widget setEnabled to allow alarm states to override current enabled state
    bool confirmWrite;            /// Request confirmation before writing a value
    bool allowDrop;

    void establishConnection( unsigned int variableIndex );

private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setTextIfNoFocus( const QString& value, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );  /// Update the text in the widget as long as the user is not entering data in it
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );
    void userReturnPressed();                       /// Act on the user pressing return in the widget
    void userEditingFinished();                     /// Act on the user signaling text editing is complete (pressing return)

public slots:
    void requestEnabled( const bool& state );
    void writeNow();

signals:
    void dbValueChanged( const QString& out );
    void userChange( const QString& oldValue, const QString& newValue, const QString& lastValue );    /// Signal a user attempt to change a value. Values are strings as the user sees them
    void requestResend();

private:
    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );
    void updateToolTip ( const QString & toolTip );
    void writeValue( QCaString *qca, QString newValue );
    QString lastValue;                      /// Last updated value (may have arrived while user is editing field)
    QString lastUserValue;                  /// Last updated value seen by the user (same as lastValue unless the user is editing the text)
    bool writeConfirmDialogPresent;         /// True if write confirm dialog box is present. When this dialog box is present the line edit no longer have focus, but the user is still effectivly interacting with it
    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

    void stringFormattingChange(){ requestResend(); }

    // Drag and Drop
protected:
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    // Don't drag from interactive widget void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDrop( QVariant drop );
    QVariant getDrop();
};

#endif /// QELINEEDIT_H
