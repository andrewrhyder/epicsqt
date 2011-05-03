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

#ifndef QCAPERIODIC_H
#define QCAPERIODIC_H

#include <QPushButton>
#include <QCaWidget.h>
#include <QCaFloating.h>
#include <QCaFloatingFormatting.h>
#include <QCaPluginLibrary_global.h>

class QCAPLUGINLIBRARYSHARED_EXPORT QCaPeriodic : public QPushButton, public QCaWidget {
    Q_OBJECT

  public:
    struct elementInfoStruct
    {
        unsigned int number;
        double atomicWeight;
        QString name;
        QString symbol;
        double meltingPoint;
        double boilingPoint;
        double density;
        unsigned int group;
        double ionizationEnergy;
        unsigned int tableRow;
        unsigned int tableCol;
    };
    struct userInfoStruct
    {
        bool enable;
        double value1;
        double value2;
    };

    struct userInfoStructArray
    {
        userInfoStruct array[113];
    };


    QCaPeriodic( QWidget *parent = 0 );
    QCaPeriodic( const QString& variableName, QWidget *parent = 0 );

    static elementInfoStruct elementInfo[113];
    userInfoStruct userInfo[113];

    enum variableTypes { VARIABLE_TYPE_NUMBER, VARIABLE_TYPE_ATOMIC_WEIGHT, VARIABLE_TYPE_MELTING_POINT, VARIABLE_TYPE_BOILING_POINT, VARIABLE_TYPE_DENSITY, VARIABLE_TYPE_GROUP, VARIABLE_TYPE_IONIZATION_ENERGY, VARIABLE_TYPE_USER_VALUE_1, VARIABLE_TYPE_USER_VALUE_2 };

    bool isEnabled() const;
    void setEnabled( const bool& state );

    // Property convenience functions

    // Variable Name and substitution
    void setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );

    // subscribe
    void setSubscribe( bool subscribe );
    bool getSubscribe();

    // variable as tool tip
    void setVariableAsToolTip( bool variableAsToolTip );
    bool getVariableAsToolTip();

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
    void setElement( const double& text, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& );
    void userClicked();

  public slots:
    void requestEnabled( const bool& state );

  signals:
    void dbValueChanged( const double& out );

  protected:
    QCaFloatingFormatting floatingFormatting;
    bool localEnabled;

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

    QString hideWS( QString in );
    QString restoreWS( QString in );

    double lastData1;
    bool haveLastData1;
    double lastData2;
    bool haveLastData2;

};

Q_DECLARE_METATYPE(QCaPeriodic::userInfoStructArray)

#endif /// QCAPERIODIC_H
