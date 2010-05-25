/*!
  \class ASguiFormPlugin
  \version $Revision: #1 $
  \date $DateTime: 2009/07/14 15:59:56 $
  \author andrew.rhyder
  \brief 
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
 * Description:
 *
 * Wrapper around QCaLabel to allow QCaLabel to be used within Designer
 *
 */
#include <ASguiFormPlugin.h>

#include <QtCore>


/// Constructor
ASguiFormPlugin::ASguiFormPlugin( QWidget* parent ) : ASguiForm( parent )
{
    /// Set up a connection to recieve variable name property changes (actually only substitutions are used)
    /// The variable name property manager class only delivers an updated variable name (or substition) after the user has stopped typing
    QObject::connect( &variableNamePropertyManager, SIGNAL( newVariableNameProperty( QString, QString, unsigned int ) ), this, SLOT( useNewVariableNameProperty( QString, QString, unsigned int ) ) );
}

/*!
    Slot to recieve macro substitutions property changes.
*/
void ASguiFormPlugin::useNewVariableNameProperty( QString, QString variableNameSubstitutionsIn, unsigned int ) {

    /// Set a new variable name substitutions
    /// (Ignore variable name - A form may supply macro substitutions to it's QCa widgets but does not use a variable name itself)
    setVariableNameSubstitutions( variableNameSubstitutionsIn );
}
