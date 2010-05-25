/*!
  \class VariableNameManager
  \version $Revision: #3 $
  \date $DateTime: 2009/07/22 13:02:25 $
  \author andrew.rhyder
  \brief Variable name management.
 */

/* Copyright (c) 2009 Australian Synchrotron
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * Licence as published by the Free Software Foundation; either
 * version 2.1 of the Licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public Licence for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * Licence along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Contact details:
 * andrew.rhyder@synchrotron.org.au
 * 800 Blackburn Road, Clayton, Victoria 3168, Australia.
 *
 */

#ifndef VARIABLENAMEMANAGER_H
#define VARIABLENAMEMANAGER_H

#include <QString>
#include <QList>

class VariableNameManager {

public:
    VariableNameManager();
    //virtual ~VariableNameManager();

    void variableNameManagerInitialise( unsigned int numVariables ) ;                     /// Used to set up the number of variables required (defaults to 1 if this is not called)

    QString getOriginalVariableName( unsigned int variableIndex );          /// Return variable name prior to any macro substitutions. (eg, SR$SECTOR$V )
    QString getVariableNameSubstitutions();                                 /// Return macro substitutions used for variable names (eg, SECTOR=01,V=PRESURE)
    QString getSubstitutedVariableName(unsigned int variableIndex );        /// Return variable name after macro substitutions

    void initialiseVariableNameSubstitutions( const QString& substitutions );   /// Initialise macro substitutions. May be used on creation.

    virtual void establishConnection( unsigned int variableIndex ) = 0;     /// Create a CA connection and initiates updates if required

    void setVariableNameSubstitutionsOverride( const QString& substitutions );

    void setVariableName( const QString& variableName, unsigned int variableIndex );    /// Accept a new variable name which may include substitution keys preceeded by $
    void setVariableNameSubstitutions( const QString& substitutions );                  /// Accept a new set of macro substitutions in the form KEY1=VALUE1,KEY2=VALUE2

  private:
    QString doSubstitution( unsigned int variableIndex );
    QString standardizeSubs( const QString &subsIn );

    QString macroSubstitutions;
    QString macroSubstitutionsOverride;

    QList<QString> variableNames;
};

#endif // VARIABLENAMEMANAGER_H
