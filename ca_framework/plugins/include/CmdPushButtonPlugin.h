/*!
  \class CmdPushButtonPlugin
  \version $Revision: #3 $
  \date $DateTime: 2010/02/01 15:54:01 $
  \author Last checked in by: $Author: rhydera $
  \brief Command Push Button Widget Plugin.
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

#ifndef CMDPUSHBUTTONPLUGIN_H
#define CMDPUSHBUTTONPLUGIN_H

#include <QString>
#include <CmdPushButton.h>

class CmdPushButtonPlugin : public CmdPushButton {
    Q_OBJECT

  public:
    /// Constructors
    CmdPushButtonPlugin( QWidget *parent = 0 );

    /// Qt Designer Properties - Program String
    Q_PROPERTY(QString program READ getProgram WRITE setProgram)
    void setProgram( QString program ){ CmdPushButton::program = program; }
    QString getProgram(){ return CmdPushButton::program; }

    /// Qt Designer Properties - Arguments String
    Q_PROPERTY(QStringList arguments READ getArguments WRITE setArguments)
    void setArguments( QStringList arguments ){ CmdPushButton::arguments = arguments; }
    QStringList getArguments(){ return CmdPushButton::arguments; }

    /// Qt Designer Properties - variable substitutions Example: $SECTOR=01 will result in any occurance of $SECTOR in variable name being replaced with 01.
    /// Note, unlike most objects with a variable name substitions, changes to this property do not take immediate effect, so the normal code which waits
    /// for the user to finish typing, then propogates the substitutions is not required. the substitutions are simply used next time the button is pushed.
    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)
    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions ){ CmdPushButton::variableNameSubstitutions = variableNameSubstitutions; }
    QString getVariableNameSubstitutionsProperty(){ return CmdPushButton::variableNameSubstitutions; }
};

#endif /// CMDPUSHBUTTONPLUGIN_H
