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

#ifndef QCAPERIODICPLUGIN_H
#define QCAPERIODICPLUGIN_H

#include <QTimer>
#include <QString>
#include <QCaPeriodic.h>
#include <QCaVariableNamePropertyManager.h>


//!!! ???
#define VARIABLE_NAME_PROPERTY_MANAGERS_SIZE 4

class QCaPeriodicPlugin : public QCaPeriodic {
    Q_OBJECT

  public:
    /// Constructors
    QCaPeriodicPlugin( QWidget *parent = 0 );
    QCaPeriodicPlugin( QString variableName, QWidget *parent = 0 );

    /// Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
    /// A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
    Q_PROPERTY(QString writeButtonVariable1 READ getVariableName1Property WRITE setVariableName1Property)
    void    setVariableName1Property( QString variable1Name ){ variableNamePropertyManagers[0].setVariableNameProperty( variable1Name ); }
    QString getVariableName1Property(){ return variableNamePropertyManagers[0].getVariableNameProperty(); }

    Q_PROPERTY(QString writeButtonVariableSubstitutions1 READ getVariableNameSubstitutions1Property WRITE setVariableNameSubstitutions1Property)
    void    setVariableNameSubstitutions1Property( QString variableNameSubstitutions1 ){ variableNamePropertyManagers[0].setSubstitutionsProperty( variableNameSubstitutions1 ); }
    QString getVariableNameSubstitutions1Property(){ return variableNamePropertyManagers[0].getSubstitutionsProperty(); }

    Q_PROPERTY(QString writeButtonVariable2 READ getVariableName2Property WRITE setVariableName2Property)
    void    setVariableName2Property( QString variable2Name ){ variableNamePropertyManagers[1].setVariableNameProperty( variable2Name ); }
    QString getVariableName2Property(){ return variableNamePropertyManagers[1].getVariableNameProperty(); }

    Q_PROPERTY(QString writeButtonVariableSubstitutions2 READ getVariableNameSubstitutions2Property WRITE setVariableNameSubstitutions2Property)
    void    setVariableNameSubstitutions2Property( QString variableNameSubstitutions2 ){ variableNamePropertyManagers[1].setSubstitutionsProperty( variableNameSubstitutions2 ); }
    QString getVariableNameSubstitutions2Property(){ return variableNamePropertyManagers[1].getSubstitutionsProperty(); }

    Q_PROPERTY(QString readbackLabelVariable1 READ getVariableName3Property WRITE setVariableName3Property)
    void    setVariableName3Property( QString variable3Name ){ variableNamePropertyManagers[2].setVariableNameProperty( variable3Name ); }
    QString getVariableName3Property(){ return variableNamePropertyManagers[2].getVariableNameProperty(); }

    Q_PROPERTY(QString readbackLabelVariableSubstitutions1 READ getVariableNameSubstitutions3Property WRITE setVariableNameSubstitutions3Property)
    void    setVariableNameSubstitutions3Property( QString variableNameSubstitutions3 ){ variableNamePropertyManagers[2].setSubstitutionsProperty( variableNameSubstitutions3 ); }
    QString getVariableNameSubstitutions3Property(){ return variableNamePropertyManagers[2].getSubstitutionsProperty(); }

    Q_PROPERTY(QString readbackLabelVariable2 READ getVariableName4Property WRITE setVariableName4Property)
    void    setVariableName4Property( QString variable4Name ){ variableNamePropertyManagers[3].setVariableNameProperty( variable4Name ); }
    QString getVariableName4Property(){ return variableNamePropertyManagers[3].getVariableNameProperty(); }

    Q_PROPERTY(QString readbackLabelVariableSubstitutions2 READ getVariableNameSubstitutions4Property WRITE setVariableNameSubstitutions4Property)
    void    setVariableNameSubstitutions4Property( QString variableNameSubstitutions4 ){ variableNamePropertyManagers[3].setSubstitutionsProperty( variableNameSubstitutions4 ); }
    QString getVariableNameSubstitutions4Property(){ return variableNamePropertyManagers[3].getSubstitutionsProperty(); }

    Q_PROPERTY(bool subscribe READ getSubscribe WRITE setSubscribe)
    Q_PROPERTY(bool variableAsToolTip READ getVariableAsToolTip WRITE setVariableAsToolTip)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
    Q_PROPERTY(bool allowDrop READ getAllowDrop WRITE setAllowDrop)


    Q_ENUMS(PresentationOptions)
    Q_PROPERTY(PresentationOptions presentationOption READ getPresentationOptionProperty WRITE setPresentationOptionProperty)
    enum PresentationOptions { buttonAndLabel = QCaPeriodic::PRESENTATION_BUTTON_AND_LABEL,
                               buttonOnly     = QCaPeriodic::PRESENTATION_BUTTON_ONLY,
                               labelOnly      = QCaPeriodic::PRESENTATION_LABEL_ONLY };
    void setPresentationOptionProperty( PresentationOptions presentationOption ){ setPresentationOption( (QCaPeriodic::presentationOptions)presentationOption ); }
    PresentationOptions getPresentationOptionProperty(){ return (PresentationOptions)getPresentationOption(); }



    Q_ENUMS(VariableTypes)
    Q_PROPERTY(VariableTypes variableType1 READ getVariableType1Property WRITE setVariableType1Property)
    Q_PROPERTY(VariableTypes variableType2 READ getVariableType2Property WRITE setVariableType2Property)
    enum VariableTypes { Number           = QCaPeriodic::VARIABLE_TYPE_NUMBER,
                         atomicWeight     = QCaPeriodic::VARIABLE_TYPE_ATOMIC_WEIGHT,
                         meltingPoint     = QCaPeriodic::VARIABLE_TYPE_MELTING_POINT,
                         boilingPoint     = QCaPeriodic::VARIABLE_TYPE_BOILING_POINT,
                         density          = QCaPeriodic::VARIABLE_TYPE_DENSITY,
                         group            = QCaPeriodic::VARIABLE_TYPE_GROUP,
                         ionizationEnergy = QCaPeriodic::VARIABLE_TYPE_IONIZATION_ENERGY,
                         userValue1       = QCaPeriodic::VARIABLE_TYPE_USER_VALUE_1,
                         userValue2       = QCaPeriodic::VARIABLE_TYPE_USER_VALUE_2 };
    void setVariableType1Property( VariableTypes variableType ){ setVariableType1( (QCaPeriodic::variableTypes)variableType ); }
    void setVariableType2Property( VariableTypes variableType ){ setVariableType2( (QCaPeriodic::variableTypes)variableType ); }
    VariableTypes getVariableType1Property(){ return (VariableTypes)getVariableType1(); }
    VariableTypes getVariableType2Property(){ return (VariableTypes)getVariableType2(); }


    Q_PROPERTY(double variableTolerance1 READ getVariableTolerance1 WRITE setVariableTolerance1)
    Q_PROPERTY(double variableTolerance2 READ getVariableTolerance2 WRITE setVariableTolerance2)

    Q_PROPERTY(QString userInfo READ getUserInfo WRITE setUserInfo)

  private:
    QCaVariableNamePropertyManager variableNamePropertyManagers[VARIABLE_NAME_PROPERTY_MANAGERS_SIZE];

  private slots:
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );
};

#endif /// QCAPERIODICPLUGIN_H
