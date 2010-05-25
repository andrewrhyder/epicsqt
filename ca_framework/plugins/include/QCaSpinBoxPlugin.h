/*! 
  \class QCaSpinBoxPlugin
  \version $Revision: #3 $
  \date $DateTime: 2010/02/01 15:54:01 $
  \author andrew.rhyder
  \brief CA Spinbox Widget Plugin.
 */
/*
 *  This file is part of the EPICS QT Framework.
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
 *    andrew@rhyder.org
 */

#ifndef QCASPINBOXPLUGIN_H
#define QCASPINBOXPLUGIN_H

#include <QTimer>
#include <QString>
#include <QCaSpinBox.h>
#include <QCaVariableNamePropertyManager.h>
#include <QCaStringFormatting.h>

class QCaSpinBoxPlugin : public QCaSpinBox {
    Q_OBJECT

  public:
    QCaSpinBoxPlugin( QWidget *parent = 0 );
    QCaSpinBoxPlugin( QString variableNameProperty = "", QWidget *parent = 0 );

    /// Qt Designer Properties - Variable Name
    /// Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
    /// A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
    Q_PROPERTY(QString variable READ getVariableNameProperty WRITE setVariableNameProperty);
    void    setVariableNameProperty( QString variableName ){ variableNamePropertyManager.setVariableNameProperty( variableName ); }
    QString getVariableNameProperty(){ return variableNamePropertyManager.getVariableNameProperty(); }

    /// Qt Designer Properties - variable substitutions Example: $SECTOR=01 will result in any occurance of $SECTOR in variable name being replaced with 01.
    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)
    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions ){ variableNamePropertyManager.setSubstitutionsProperty( variableNameSubstitutions ); }
    QString getVariableNameSubstitutionsProperty(){ return variableNamePropertyManager.getSubstitutionsProperty(); }

    /// Qt Designer Properties - subscribe
    Q_PROPERTY(bool subscribe READ getSubscribeProperty WRITE setSubscribeProperty)
    void setSubscribeProperty( bool subscribe ){ subscribeProperty = subscribe; }
    bool getSubscribeProperty(){ return subscribeProperty; }

    /// Qt Designer Properties - variable as tool tip
    Q_PROPERTY(bool variableAsToolTip READ getVariableAsToolTipProperty WRITE setVariableAsToolTipProperty)
    void setVariableAsToolTipProperty( bool variableAsToolTip ){ variableAsToolTipProperty = variableAsToolTip; }
    bool getVariableAsToolTipProperty(){ return variableAsToolTipProperty; }

    /// Qt Designer Properties - enabled (override of widget enabled)
    Q_PROPERTY(bool enabled READ getEnabledProperty WRITE setEnabledProperty)
    void setEnabledProperty( bool enabled ){ setEnabled( enabled ); }
    bool getEnabledProperty(){ return enabledProperty; }

  private:
    QCaVariableNamePropertyManager variableNamePropertyManager;

  private slots:
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );

};

#endif /// QCASPINBOXPLUGIN_H
