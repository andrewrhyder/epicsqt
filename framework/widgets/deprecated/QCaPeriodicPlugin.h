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

#include <QEPeriodic.h>


/// QCaPeriodicPlugin IS deprecated - use QEPeriodic.
//
class QCaPeriodicPlugin : public QEPeriodic {
    Q_OBJECT

  public:
    QCaPeriodicPlugin( QWidget *parent = 0 ) : QEPeriodic( parent ) {}

// This deprecated widget is now just a shell around its replacement.
// Its replacement does not recognise any properties defined in using a Q_ENUM in the original widget,
// so these properties must still be implemented in the deprecated widget
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

};

#endif /// QCAPERIODICPLUGIN_H
