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

#ifndef QESPINBOX_H
#define QESPINBOX_H

#include <QDoubleSpinBox>
#include <QCaWidget.h>
#include <QCaFloating.h>
#include <QCaFloatingFormatting.h>
#include <QCaPluginLibrary_global.h>
#include <QCaVariableNamePropertyManager.h>

class QCAPLUGINLIBRARYSHARED_EXPORT QESpinBox : public QDoubleSpinBox, public QCaWidget {
    Q_OBJECT

  public:
    QESpinBox( QWidget *parent = 0 );
    QESpinBox( const QString& variableName, QWidget *parent = 0 );

    // Property convenience functions

    // write on change
    void setWriteOnChange( bool writeOnChangeIn );
    bool getWriteOnChange();

    // subscribe
    void setSubscribe( bool subscribe );
    bool getSubscribe();

    // Add units (as suffix)
    void setAddUnitsAsSuffix( bool addUnitsAsSuffixIn );
    bool getAddUnitsAsSuffix();

    // useDbPrecision (as spinbox 'decimals')
    void setUseDbPrecisionForDecimals( bool useDbPrecisionForDecimalIn );
    bool getUseDbPrecisionForDecimals();


  protected:
    QCaFloatingFormatting floatingFormatting;
    bool writeOnChange;                     // Write changed value to database when user changes a value
    bool addUnitsAsSuffix;
    bool useDbPrecisionForDecimal;

    void establishConnection( unsigned int variableIndex );

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setValueIfNoFocus( const double& value, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );
    void userValueChanged( double value );
  private slots:
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )// !! move into Standard Properties section??
    {
        setVariableNameAndSubstitutions(variableNameIn, variableNameSubstitutionsIn, variableIndex);
    }


  public slots:
    void requestEnabled( const bool& state ){ setApplicationEnabled( state ); } // !! move into Standard Properties section??
    void writeNow();

  signals:
    void dbValueChanged( const double& out );
    void userChange( const QString& oldValue, const QString& newValue, const QString& lastValue );    // Signal a user attempt to change a value. Values are strings as the user sees them

private:
    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );
    void updateToolTip ( const QString & toolTip );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

    bool programaticValueChange;   // Flag set while the spin box value is being changed programatically (not by the user)
    double lastValue;
    QString lastUserValue;

    bool ignoreSingleShotRead;

    void setSuffixEgu( qcaobject::QCaObject* qca );
    void setDecimalsFromPrecision( qcaobject::QCaObject* qca );



    // Drag and Drop
protected:
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    // Don't drag from interactive widget void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDrop( QVariant drop );
    QVariant getDrop();


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
    enum UserLevels { User      = USERLEVEL_USER,
                      Scientist = USERLEVEL_SCIENTIST,
                      Engineer  = USERLEVEL_ENGINEER };
    UserLevels getUserLevelVisibilityProperty() { return (UserLevels)getUserLevelVisibility(); }
    void setUserLevelVisibilityProperty( UserLevels level ) { setUserLevelVisibility( (userLevels)level ); }

    UserLevels getUserLevelEnabledProperty() { return (UserLevels)getUserLevelEnabled(); }
    void setUserLevelEnabledProperty( UserLevels level ) { setUserLevelEnabled( (userLevels)level ); }
    Q_ENUMS(UserLevels)
    Q_PROPERTY(UserLevels userLevelVisibility READ getUserLevelVisibilityProperty WRITE setUserLevelVisibilityProperty)
    Q_PROPERTY(UserLevels userLevelEnabled READ getUserLevelEnabledProperty WRITE setUserLevelEnabledProperty)
    //=================================================================================


    Q_PROPERTY(bool writeOnChange READ getWriteOnChange WRITE setWriteOnChange)
    Q_PROPERTY(bool subscribe READ getSubscribe WRITE setSubscribe)

    // Note, this useDbPrecision property is normally part of the standard 'string properties' set.
    //       The normal get and set methods are QCaStringFormatting::getUseDbPrecision() and QCaStringFormatting::setUseDbPrecision().
    //       In this case, the flag is used to determine the QSpinBox 'decimals' property.
    Q_PROPERTY(bool useDbPrecision READ getUseDbPrecisionForDecimals WRITE setUseDbPrecisionForDecimals)

    // Note, this addUnits property is normally part of the standard 'string properties' set.
    //       The normal get and set methods are QCaStringFormatting::getAddUnits() and QCaStringFormatting::setAddUnits().
    //       In this case, the units are added as the QSpinBox suffix, and not as part of a string.
    Q_PROPERTY(bool addUnits READ getAddUnitsAsSuffix WRITE setAddUnitsAsSuffix)

};

#endif //QESPINBOX_H
