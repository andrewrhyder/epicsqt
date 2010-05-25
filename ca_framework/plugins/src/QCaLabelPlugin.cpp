/*! 
  \class QCaLabelPlugin
  \version $Revision: #3 $
  \date $DateTime: 2009/11/18 10:21:48 $
  \author andrew.rhyder
  \brief CA Label Widget Plugin for designer.
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

#include <QtCore>
#include <QTimer>
#include <QCaLabelPlugin.h>

/*!
    ???
*/
QCaLabelPlugin::QCaLabelPlugin( QWidget* parent ) : QCaLabel( parent ) {





/*
    int id = qRegisterMetaType<localEnumerationList>();
    qDebug() << "qRegisterMetaType ID: " << id;
    qRegisterMetaTypeStreamOperators<localEnumerationList>("localEnumerationList");

//    QVariant v;
*/





 // Set some default text to give the label visibility and size as the default label has no border and the background colour is the same as the form
    setText( "QCaLabel" );

    /// Set up a connection to recieve variable name property changes
    /// The variable name property manager class only delivers an updated variable name after the user has stopped typing
    QObject::connect( &variableNamePropertyManager, SIGNAL( newVariableNameProperty( QString, QString, unsigned int ) ), this, SLOT( useNewVariableNameProperty( QString, QString, unsigned int) ) );
}

/*!
    Slot to recieve variable name property changes.
*/
void QCaLabelPlugin::useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex ) {

    /// Set a new variable name and substitutions
    setVariableNameSubstitutions( variableNameSubstitutionsIn );
    setVariableName( variableNameIn, variableIndex );
    establishConnection( variableIndex );
}
