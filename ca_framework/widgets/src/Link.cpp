/*! 
  \class Link
  \version $Revision: #4 $
  \date $DateTime: 2010/02/18 15:15:02 $
  \author andrew.rhyder
  \brief Link Widget.
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

#include <Link.h>
#include <ContainerProfile.h>
#include <QVariant>
#include <QString>
#include <QDebug>

/*!
    Constructor with no initialisation
*/
Link::Link( QWidget *parent ) : QLabel( parent ) {

    // If a container profile has been defined, then this widget is being used in a real GUI and
    // should be visible or not according to the visible property. (While in Designer it can always be displayed)
    ContainerProfile profile;
    if( profile.isProfileDefined() )
        setVisible( visibleProperty );

    // Set default properties
    setText( "Link" );

    signalFalseProperty = true;
    signalTrueProperty = true;

    visibleProperty = false;

    conditionProperty = CONDITION_EQ;
}

// Common comparison. Macro to evaluate the 'in' signal value.
// Determine if the 'in' signal value matches the condition
// If match and signaling on a match, then send a signal
// If not a match and signaling on no match, then send a signal
#define EVAL_CONDITION                                              \
                                                                    \
    bool match = false;                                             \
    switch( conditionProperty )                                     \
    {                                                               \
        case CONDITION_EQ: if( in == val ) match = true; break;     \
        case CONDITION_NE: if( in != val ) match = true; break;     \
        case CONDITION_GT: if( in >  val ) match = true; break;     \
        case CONDITION_GE: if( in >= val ) match = true; break;     \
        case CONDITION_LT: if( in <  val ) match = true; break;     \
        case CONDITION_LE: if( in <= val ) match = true; break;     \
    }                                                               \
                                                                    \
    sendValue( match );

// Slot to perform a comparison on a bool
void Link::in( const bool& in )
{
    bool val = comparisonValueProperty.toBool();
    EVAL_CONDITION;
}

// Slot to perform a comparison on an integer
void Link::in( const qlonglong& in )
{
    qlonglong val = comparisonValueProperty.toLongLong();
    EVAL_CONDITION;
}

// Slot to perform a comparison on a floating point number
void Link::in( const double& in )
{
    double val = comparisonValueProperty.toDouble();
    EVAL_CONDITION;
}

// Slot to perform a comparison on a string
void Link::in( const QString& in )
{
    bool stringIsNum = false;

    // If the string is a valid number, compare it as a number

    QStringList inList = in.split(" ", QString::SkipEmptyParts);
    if( inList.size() )
    {
        double inDouble = inList[0].toDouble( &stringIsNum );
        if( stringIsNum )
        {
            this->in( inDouble );
        }
    }

    // If the string is not a valid number, do a string comparison
    if( !stringIsNum )
    {
        QString val = comparisonValueProperty.toString();
        EVAL_CONDITION;
    }
}

// Generate appropriate signals following a comparison of an input value
void Link::sendValue( bool match )
{
    // If input comparison matched, emit the appropriate value if required
    if( match )
    {
        if( signalTrueProperty )
            emitValue( outTrueValueProperty );
    }

    // If input comparison did not match, emit the appropriate value if required
    else
    {
        if( signalFalseProperty )
            emitValue( outFalseValueProperty );
    }
}

// Emit signals required when input value matches or fails to match
void Link::emitValue( QVariant value )
{
    emit out( value.toBool() );
    emit out( value.toLongLong() );
    emit out( value.toDouble() );
    emit out( value.toString() );
}

// Slot to allow signal/slot manipulation of the auto fill background attribute of the base label class
void Link::autoFillBackground( const bool& enable )
{
    setAutoFillBackground( enable );
}

// Manage property to set widget visible or not
void Link::setVisibleProperty( bool visiblePropertyIn )
{
    // Update the property
    visibleProperty = visiblePropertyIn;

    // If a container profile has been defined, then this widget is being used in a real GUI and
    // should be visible or not according to the visible property. (While in Designer it can always be displayed)
    ContainerProfile profile;
    if( profile.isProfileDefined() )
    {
        setVisible( visibleProperty );
    }

}
