/*!

  \class CmdPushButtonPlugin

  \version $Revision: #2 $
  \date $DateTime: 2009/07/17 10:02:42 $
  \author Last checked in by: $Author: rhydera $

  \brief Command Push Button Widget Plugin.

  Copyright (c) 2009 Australian Synchrotron

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  Licence as published by the Free Software Foundation; either
  version 2.1 of the Licence, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public Licence for more details.

  You should have received a copy of the GNU Lesser General Public
  Licence along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.

  Contact details:
  andrew.rhyder@synchrotron.org.au
  800 Blackburn Road, Clayton, Victoria 3168, Australia.

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
