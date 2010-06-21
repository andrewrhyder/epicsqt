/*! 
  \class QCaLineEditPlugin
  \version $Revision: #1 $
  \date $DateTime: 2009/07/14 15:59:56 $
  \author andrew.rhyder
  \brief CA Line Edit Widget Plugin.
 */
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

#include <QtCore>
#include <QTimer>
#include <QCaLineEditPlugin.h>

/*!
    ???
*/
QCaLineEditPlugin::QCaLineEditPlugin( QWidget *parent ) : QCaLineEdit( parent ) {
    /// Set up a connection to recieve variable name property changes
    /// The variable name property manager class only delivers an updated variable name after the user has stopped typing
    QObject::connect( &variableNamePropertyManager, SIGNAL( newVariableNameProperty( QString, QString, unsigned int ) ), this, SLOT( useNewVariableNameProperty( QString, QString, unsigned int ) ) );
}

/*!
    Slot to recieve variable name property changes.
*/
void QCaLineEditPlugin::useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex ) {
    /// Set a new variable name and substitutions
    setVariableNameSubstitutions( variableNameSubstitutionsIn );
    setVariableName( variableNameIn, variableIndex );
    establishConnection( variableIndex );
}
