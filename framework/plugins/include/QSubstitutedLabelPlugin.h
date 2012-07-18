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

#ifndef QSUBSTITUTEDLABELPLUGIN_H
#define QSUBSTITUTEDLABELPLUGIN_H

#include <QTimer>
#include <QString>
#include <QSubstitutedLabel.h>
#include <QCaVariableNamePropertyManager.h>

class QSubstitutedLabelPlugin : public QSubstitutedLabel {
    Q_OBJECT

  public:
    /// Constructors
    QSubstitutedLabelPlugin( QWidget *parent = 0 );

    /// Label text to be substituted.
    Q_PROPERTY(QString labelText READ getLabelTextPropertyFormat WRITE setLabelTextPropertyFormat);

    /// Text substitutions.
    /// NOTE, this plugin uses the variable-name-and-substitutions mechanism used to manage variable names to manage text displayed in the label.
    Q_PROPERTY(QString textSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)
    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions ){ variableNamePropertyManager.setSubstitutionsProperty( variableNameSubstitutions ); }
    QString getVariableNameSubstitutionsProperty(){ return variableNamePropertyManager.getSubstitutionsProperty(); }


  private:
    QCaVariableNamePropertyManager variableNamePropertyManager;

  private slots:
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );
};

#endif /// QSUBSTITUTEDLABELPLUGIN_H
