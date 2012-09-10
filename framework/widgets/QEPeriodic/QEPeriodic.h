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
 *  Copyright (c) 2011
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef QEPeriodic_H
#define QEPeriodic_H

#include <QFrame>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QCaWidget.h>
#include <QCaFloating.h>
#include <QCaFloatingFormatting.h>
#include <QCaVariableNamePropertyManager.h>
#include <QCaPluginLibrary_global.h>

#define NUM_ELEMENTS 118

class QEPeriodicComponentData
{
public:

    QEPeriodicComponentData()
    {
        variableIndex1 = 0;
        lastData1 = 0.0;
        haveLastData1 = false;

        variableIndex2 = 0;
        lastData2 = 0.0;
        haveLastData2 = false;
    }

    unsigned int variableIndex1;
    double lastData1;
    bool haveLastData1;

    unsigned int variableIndex2;
    double lastData2;
    bool haveLastData2;
};

// Dynamic element information structure
class userInfoStruct
{
public:
    userInfoStruct() { enable = false; value1 = 0.0; value2 = 0.0; }
    bool    enable;         // True if element is available for the user to select
    double  value1;         // User value to be written to and compared against the first variable
    double  value2;         // User value to be written to and compared against the second variable
    QString elementText;    // User text associated with element (emitted on element change)
};

class QCAPLUGINLIBRARYSHARED_EXPORT QEPeriodic : public QFrame, public QCaWidget {
    Q_OBJECT

  public:
    // Static element information structure
    struct elementInfoStruct
    {
        unsigned int number;            // Atomic number:     1
        double       atomicWeight;      // Atomic weight:     1.0079
        QString      name;              // Element name:      Hydrogen
        QString      symbol;            // Element symbol:    H
        double       meltingPoint;      // Melting point:     -259 deg C
        double       boilingPoint;      // Boiling point:     -253 deg C
        double       density;           // Density:           0.09
        unsigned int group;             // Periodic group:    1
        double       ionizationEnergy;  // Ionization energy: 13.5984 eV
        unsigned int tableRow;          // Index into table row representing periodic table (related to user interface, not chemistry)
        unsigned int tableCol;          // Index into table column representing periodic table (related to user interface, not chemistry)
    };

    // Array of dynamic element information structure
    struct userInfoStructArray
    {
        userInfoStruct array[NUM_ELEMENTS];
    };

    QEPeriodic( QWidget *parent = 0 );
    QEPeriodic( const QString& variableName, QWidget *parent = 0 );

    static elementInfoStruct elementInfo[NUM_ELEMENTS];      // Array of static element information
    userInfoStruct userInfo[NUM_ELEMENTS];                   // Array of dynamic element information

    // Element information options
    enum variableTypes { VARIABLE_TYPE_NUMBER,
                         VARIABLE_TYPE_ATOMIC_WEIGHT,
                         VARIABLE_TYPE_MELTING_POINT,
                         VARIABLE_TYPE_BOILING_POINT,
                         VARIABLE_TYPE_DENSITY,
                         VARIABLE_TYPE_GROUP,
                         VARIABLE_TYPE_IONIZATION_ENERGY,
                         VARIABLE_TYPE_USER_VALUE_1,
                         VARIABLE_TYPE_USER_VALUE_2 };

    // Property convenience functions

    // subscribe
    void setSubscribe( bool subscribe );
    bool getSubscribe();

    // presentation options
    enum presentationOptions { PRESENTATION_BUTTON_AND_LABEL,
                               PRESENTATION_BUTTON_ONLY,
                               PRESENTATION_LABEL_ONLY };
    void setPresentationOption( presentationOptions presentationOptionIn );
    presentationOptions getPresentationOption();

    // variable 1 type
    void setVariableType1( variableTypes variableType1In );
    variableTypes getVariableType1();

    // variable 2 type
    void setVariableType2( variableTypes variableType2In );
    variableTypes getVariableType2();


    // variable 1 tolerance
    void setVariableTolerance1( double variableTolerance1In );
    double getVariableTolerance1();

    // variable 2 tolerance
    void setVariableTolerance2( double variableTolerance2In );
    double getVariableTolerance2();

    // user info
    void setUserInfo( QString userInfo );
    QString getUserInfo();

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setElement( const double& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& );
    void userClicked();

  public slots:
    void requestEnabled( const bool& state );

  signals:
    void dbValueChanged( const double& out );
    void dbElementChanged( const QString& out );
    void requestResend();

  protected:
    QCaFloatingFormatting floatingFormatting;
    bool localEnabled;
    bool allowDrop;

    variableTypes variableType1;
    variableTypes variableType2;

    double variableTolerance1;
    double variableTolerance2;


    void establishConnection( unsigned int variableIndex );

  private:
    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex  );
    void updateToolTip ( const QString & toolTip );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

    QEPeriodicComponentData writeButtonData;
    QEPeriodicComponentData readbackLabelData;
    QPushButton* writeButton;
    QLabel* readbackLabel;
    QHBoxLayout *layout;


    bool getElementTextForValue( const double& value, const unsigned int& variableIndex, QEPeriodicComponentData& componentData, const QString& currentText, QString& newText );

    presentationOptions presentationOption;
    void updatePresentationOptions();

    float elementMatch( int i, bool haveFirstVariable, double lastData1, bool haveSecondVariable, double lastData2 );

    // Drag and Drop
protected:
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    // Don't drag from interactive widget void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDrop( QVariant drop );
    QVariant getDrop();


#define QEPERIODIC_NUM_VARIABLES 4

    //=================================================================================
    // Multiple Variable properties
    // These properties should be identical for every widget using multiple variables (The number of variables may vary).
    // WHEN MAKING CHANGES: search for MULTIPLEVARIABLEPROPERTIESBASE and change all occurances.
    private:
        QCaVariableNamePropertyManager variableNamePropertyManagers[QEPERIODIC_NUM_VARIABLES];
    public:

    // Define a variable
    // Note, the QPROPERTY declaration itself can't be in this macro
#define VARIABLE_PROPERTY_ACCESS(VAR_INDEX) \
    void    setVariableName##VAR_INDEX##Property( QString variableName ){ variableNamePropertyManagers[VAR_INDEX].setVariableNameProperty( variableName ); } \
    QString getVariableName##VAR_INDEX##Property(){ return variableNamePropertyManagers[VAR_INDEX].getVariableNameProperty(); }

    VARIABLE_PROPERTY_ACCESS(0)
    Q_PROPERTY(QString writeButtonVariable1 READ getVariableName0Property WRITE setVariableName0Property)

    VARIABLE_PROPERTY_ACCESS(1)
    Q_PROPERTY(QString writeButtonVariable2 READ getVariableName1Property WRITE setVariableName1Property)

    VARIABLE_PROPERTY_ACCESS(2)
    Q_PROPERTY(QString readbackLabelVariable1 READ getVariableName2Property WRITE setVariableName2Property)

    VARIABLE_PROPERTY_ACCESS(3)
    Q_PROPERTY(QString readbackLabelVariable2 READ getVariableName3Property WRITE setVariableName3Property)


    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)
    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions )
    {
        for( int i = 0; i < QEPERIODIC_NUM_VARIABLES; i++ )
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


    // Widget specific properties

    Q_ENUMS(PresentationOptions)
    Q_PROPERTY(PresentationOptions presentationOption READ getPresentationOptionProperty WRITE setPresentationOptionProperty)
    enum PresentationOptions { buttonAndLabel = QEPeriodic::PRESENTATION_BUTTON_AND_LABEL,
                               buttonOnly     = QEPeriodic::PRESENTATION_BUTTON_ONLY,
                               labelOnly      = QEPeriodic::PRESENTATION_LABEL_ONLY };
    void setPresentationOptionProperty( PresentationOptions presentationOption ){ setPresentationOption( (QEPeriodic::presentationOptions)presentationOption ); }
    PresentationOptions getPresentationOptionProperty(){ return (PresentationOptions)getPresentationOption(); }



    Q_ENUMS(VariableTypes)
    Q_PROPERTY(VariableTypes variableType1 READ getVariableType1Property WRITE setVariableType1Property)
    Q_PROPERTY(VariableTypes variableType2 READ getVariableType2Property WRITE setVariableType2Property)
    enum VariableTypes { Number           = QEPeriodic::VARIABLE_TYPE_NUMBER,
                         atomicWeight     = QEPeriodic::VARIABLE_TYPE_ATOMIC_WEIGHT,
                         meltingPoint     = QEPeriodic::VARIABLE_TYPE_MELTING_POINT,
                         boilingPoint     = QEPeriodic::VARIABLE_TYPE_BOILING_POINT,
                         density          = QEPeriodic::VARIABLE_TYPE_DENSITY,
                         group            = QEPeriodic::VARIABLE_TYPE_GROUP,
                         ionizationEnergy = QEPeriodic::VARIABLE_TYPE_IONIZATION_ENERGY,
                         userValue1       = QEPeriodic::VARIABLE_TYPE_USER_VALUE_1,
                         userValue2       = QEPeriodic::VARIABLE_TYPE_USER_VALUE_2 };
    void setVariableType1Property( VariableTypes variableType ){ setVariableType1( (QEPeriodic::variableTypes)variableType ); }
    void setVariableType2Property( VariableTypes variableType ){ setVariableType2( (QEPeriodic::variableTypes)variableType ); }
    VariableTypes getVariableType1Property(){ return (VariableTypes)getVariableType1(); }
    VariableTypes getVariableType2Property(){ return (VariableTypes)getVariableType2(); }


    Q_PROPERTY(double variableTolerance1 READ getVariableTolerance1 WRITE setVariableTolerance1)
    Q_PROPERTY(double variableTolerance2 READ getVariableTolerance2 WRITE setVariableTolerance2)

    Q_PROPERTY(QString userInfo READ getUserInfo WRITE setUserInfo)

};

Q_DECLARE_METATYPE(QEPeriodic::userInfoStructArray)

#endif /// QEPeriodic_H
