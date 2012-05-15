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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef QCAIMAGEPLUGIN_H
#define QCAIMAGEPLUGIN_H

#include <QTimer>
#include <QString>
#include <QCaImage.h>
#include <QCaVariableNamePropertyManager.h>
#include <QCaStringFormatting.h>

class QCaImagePlugin : public QCaImage {
    Q_OBJECT

  public:
    /// Constructors
    QCaImagePlugin( QWidget *parent = 0 );
    QCaImagePlugin( QString variableName, QWidget *parent = 0 );

    /// Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
    /// A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
    Q_PROPERTY(QString imageVariable READ getVariableName1Property WRITE setVariableName1Property)
    void    setVariableName1Property( QString variable1Name ){ variableNamePropertyManagers[0].setVariableNameProperty( variable1Name ); }
    QString getVariableName1Property(){ return variableNamePropertyManagers[0].getVariableNameProperty(); }

    Q_PROPERTY(QString widthVariable READ getVariableName2Property WRITE setVariableName2Property)
    void    setVariableName2Property( QString variable2Name ){ variableNamePropertyManagers[1].setVariableNameProperty( variable2Name ); }
    QString getVariableName2Property(){ return variableNamePropertyManagers[1].getVariableNameProperty(); }

    Q_PROPERTY(QString heightVariable READ getVariableName3Property WRITE setVariableName3Property)
    void    setVariableName3Property( QString variable3Name ){ variableNamePropertyManagers[2].setVariableNameProperty( variable3Name ); }
    QString getVariableName3Property(){ return variableNamePropertyManagers[2].getVariableNameProperty(); }

    Q_PROPERTY(QString acquirePeriodVariable READ getVariableName4Property WRITE setVariableName4Property)
    void    setVariableName4Property( QString variable4Name ){ variableNamePropertyManagers[3].setVariableNameProperty( variable4Name ); }
    QString getVariableName4Property(){ return variableNamePropertyManagers[3].getVariableNameProperty(); }

    Q_PROPERTY(QString exposureTimeVariable READ getVariableName5Property WRITE setVariableName5Property)
    void    setVariableName5Property( QString variable5Name ){ variableNamePropertyManagers[4].setVariableNameProperty( variable5Name ); }
    QString getVariableName5Property(){ return variableNamePropertyManagers[4].getVariableNameProperty(); }

    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)
    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions ){ for( int i = 0; i < QCAIMAGE_NUM_VARIABLES; i++ ) variableNamePropertyManagers[i].setSubstitutionsProperty( variableNameSubstitutions ); }
    QString getVariableNameSubstitutionsProperty(){ return variableNamePropertyManagers[0].getSubstitutionsProperty(); }


    Q_PROPERTY(bool variableAsToolTip READ getVariableAsToolTip WRITE setVariableAsToolTip)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
    Q_PROPERTY(bool allowDrop READ getAllowDrop WRITE setAllowDrop)

    Q_PROPERTY(bool visible READ getRunVisible WRITE setRunVisible)

    /// Format options (8 bit grey scale, 32 bit color, etc)
    Q_ENUMS(FormatOptions)
    Q_PROPERTY(FormatOptions formatOption READ getFormatOptionProperty WRITE setFormatOptionProperty)

    enum FormatOptions { Grey_8   = QCaImage::GREY8,
                         Grey_12  = QCaImage::GREY12,
                         RGB_888  = QCaImage::RGB_888 };
    void setFormatOptionProperty( FormatOptions formatOption ){ setFormatOption( (QCaImage::formatOptions)formatOption ); }
    FormatOptions getFormatOptionProperty(){ return (FormatOptions)getFormatOption(); }

    Q_PROPERTY(bool displayAcquirePeriod READ getDisplayAcquirePeriod WRITE setDisplayAcquirePeriod)
    Q_PROPERTY(bool displayExposureTime READ getDisplayExposureTime WRITE setDisplayExposureTime)

    Q_PROPERTY(int zoom READ getZoom WRITE setZoom)

    Q_PROPERTY(int initialHosScrollPos READ getInitialHozScrollPos WRITE setInitialHozScrollPos)
    Q_PROPERTY(int initialVertScrollPos READ getInitialVertScrollPos WRITE setInitialVertScrollPos)

  private:
    QCaVariableNamePropertyManager variableNamePropertyManagers[QCAIMAGE_NUM_VARIABLES];

  private slots:
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );
};

#endif /// QCAIMAGEPLUGIN_H
