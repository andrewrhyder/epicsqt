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
    QCaSpinBoxPlugin( QString variableName, QWidget *parent = 0 );

    /// Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
    /// A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
    Q_PROPERTY(QString variable READ getVariableNameProperty WRITE setVariableNameProperty);
    void    setVariableNameProperty( QString variableName ){ variableNamePropertyManager.setVariableNameProperty( variableName ); }
    QString getVariableNameProperty(){ return variableNamePropertyManager.getVariableNameProperty(); }

    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)
    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions ){ variableNamePropertyManager.setSubstitutionsProperty( variableNameSubstitutions ); }
    QString getVariableNameSubstitutionsProperty(){ return variableNamePropertyManager.getSubstitutionsProperty(); }

    Q_PROPERTY(bool writeOnChange READ getWriteOnChange WRITE setWriteOnChange)
    Q_PROPERTY(bool subscribe READ getSubscribe WRITE setSubscribe)
    Q_PROPERTY(bool variableAsToolTip READ getVariableAsToolTip WRITE setVariableAsToolTip)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
    Q_PROPERTY(bool allowDrop READ getAllowDrop WRITE setAllowDrop)

    // Note, this addUnits property is normally part of the standard 'string properties' set.
    //       The normal get and set methods are QCaStringFormatting::getAddUnits() and QCaStringFormatting::setAddUnits().
    //       In this case, the units are added as the QSpinBox suffix, and not as part of a string.
    Q_PROPERTY(bool addUnits READ getAddUnitsAsSuffix WRITE setAddUnitsAsSuffix)

  private:
    QCaVariableNamePropertyManager variableNamePropertyManager;

  private slots:
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );

};

#endif /// QCASPINBOXPLUGIN_H
