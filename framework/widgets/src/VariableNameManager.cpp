/*  VariableNameManager.cpp
 *
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

/*
 * Description:
 *
 * A class to manage changes to one or more variable names and the macro 
 * substitutions applying to the variable names.
 *
 */

#include <VariableNameManager.h>
#include <ContainerProfile.h>
#include <QStringList>
#include <QtDebug>

class sub
{
    QString key;
    QString value;
};

/*
    Assume one variable name.
*/
VariableNameManager::VariableNameManager() {

    // Assume one variable name.
    variableNameManagerInitialise( 1 );
}

/*
    Define the required number of variables to manage.
    The list of variable names is initially conatins a single variable name so this
    need only be called if more than one variable name is required.
*/
void VariableNameManager::variableNameManagerInitialise( unsigned int numVariables ) {

    // Sanity check. List must contain at least one variable name
    if( numVariables < 1 )
        return;

    // Clear out any existing variables
    variableNames.clear();

    // Create the required number of variables
    for( unsigned int i = 0; i < numVariables; i++ ) {
        variableNames.append( QString() );
    }
}

/*
    Get the current variable name.
    Supply a variable index if this class is managing more than one variable
    name.
*/
QString VariableNameManager::getOriginalVariableName( unsigned int variableIndex ) {

    // Sanity check
    if( variableIndex >= (unsigned int )variableNames.size() )
        return "";

    // Return the original variable name
    return variableNames[variableIndex];
}

/*
    Get the current variable name substitutions.
    Note the substitutions for the first variable are always returned as
    the same substitutions are used for every entry in the variableNames list.
*/
QString VariableNameManager::getVariableNameSubstitutions() {

    return macroSubstitutions;
}

/*
    Get the current variable name with substitutions applied.
*/
QString VariableNameManager::getSubstitutedVariableName( unsigned int variableIndex ) {

    // Sanity check
    if( variableIndex >= (unsigned int )variableNames.size() )
        return "";

    // Perform the substitution
    return doSubstitution( variableIndex );
}

/*
    Override variable name substitutions.
    This is called when any macro substitutions set by default are overridden by the creator.
*/
void VariableNameManager::setVariableNameSubstitutionsOverride( const QString& macroSubstitutionsOverrideIn ) {

    macroSubstitutionsOverride = macroSubstitutionsOverrideIn;
}

/*
    Set the variable name.
    Macro substitution will be performed.
    A new connection is established.
*/
void VariableNameManager::setVariableName( const QString& variableNameIn, unsigned int variableIndex ) {

    // Sanity check
    if( variableIndex >= (unsigned int )variableNames.size() )
        return;

    // Save the variable name and request the variableName data if updates are required
    variableNames[variableIndex] = variableNameIn;
}

/*
    Set the variable name substitutions.
    Note, if there is more than one variable name in the list, the same
    substitutions are used for every entry in the variableNames list.
    Macro substitution will be performed.
    A new connection is established.
*/
void VariableNameManager::setVariableNameSubstitutions( const QString& macroSubstitutionsIn ) {

    macroSubstitutions = macroSubstitutionsIn;
}

/*
    Perform a set of substitutions throughout a variable name.
    Replace $MACRO1 with VALUE1, $MACRO2 with VALUE2, etc wherever they appear in the string.
*/
QString VariableNameManager::doSubstitution( unsigned int variableIndex ) {

    // Sanity check
    if( variableIndex >= (unsigned int )variableNames.size() )
        return "";

    // Start with the initial string
    QString result = variableNames[variableIndex];

    // Perform the required substitutions on the variable name
    return substituteThis( result );
}

/*
    Perform a set of substitutions throughout a string.
    Replace $MACRO1 with VALUE1, $MACRO2 with VALUE2, etc wherever they appear in the string.
*/
QString VariableNameManager::substituteThis( const QString string ) {

    // Start with the initial string
    QString result = string;

    // Generate a list where each item in the list is a single substitution in the form MACRO1=VALUE1
    QString subs;
    subs.append( macroSubstitutionsOverride ).append( "," ).append( macroSubstitutions );

    //!!! for efficiency, should this be done when substitutions are added or removed?? Build a list of keys and values...
    // Process the substitutions.
    // This is implemented using a finite state table.
    // the states are defined in the enumeration list 'states'.
    // The case statement for each state is prefixed with a comment showing which parts of the format that case deals with.
    //
    // The substitutions format is:
    // [___]KEY[___]=[___][']VALUE['][___][,...]
    //
    // where:
    //    ___ = whitespace
    //    If optional ' is present before VALUE, a ' must be present after VALUE
    //    VALUE may include any character (including white space) except '
    //
    // Example:   AAA=123, BBB = 456, CCC = xx xx   ,  DDD= 'xx xx'
    // Note, in the above example, the values for CCC and DDD are both 'xx xx'

    int keyStart = 0;   // Index to first key character
    int keyEnd = 0;     // Index to character PAST last key character (key length = keyEnd - keyStart)
    int valueStart = 0; // Index to first value character
    int valueEnd = 0;   // Index to character PAST last value character (value length = valueEnd - valueStart)

    QString key;        // Key extracted from macro substitutions
    QString value;      // Value extracted from macro substitutions

    bool processingSpaces = false; // True if working through spaces that may be in the middle of a value

    enum states{ PRE_KEY, KEY, POST_KEY, EQUATE, VALUE_START_QUOTE, VALUE, VALUE_QUOTED, POST_VALUE, ERROR };
    states state = PRE_KEY;

    int i;
    for( i = 0; i < subs.length(); i++ )
    {
        // Get next character from the macro substitutions
        char nextChar = subs.at(i).toLatin1();

        // Finite state switch
        switch( state )
        {

        // Error parsing. Ignore rest of string.
        case ERROR:
            i = subs.length();
            break;

        // [___]KEY[___]=[___][']VALUE['][___][,...]
        //  ^^^ ^
        case PRE_KEY:
            switch( nextChar )
            {
            case '\t':
            case ' ':
            case ',':  // Handles case where macros were blindly added on to an empty macro string with a ',' in between (,KEY=VALUE_)
                break;

            case '=':
            case '\'':
                state = ERROR;
                break;

            default:
                keyStart = i;
                state = KEY;
                break;
            }
            break;

        // [___]KEY[___]=[___][']VALUE['][___][,...]
        //       ^^ ^   ^
        case KEY:
            switch( nextChar )
            {
            case '\t':
            case ' ':
                keyEnd = i;
                key = subs.mid( keyStart, keyEnd-keyStart );
                state = POST_KEY;
                break;

            case '=':
                keyEnd = i;
                key = subs.mid( keyStart, keyEnd-keyStart );
                state = EQUATE;
                break;

            default:
                break;
            }
            break;

        // [___]KEY[___]=[___][']VALUE['][___][,...]
        //           ^^ ^
        case POST_KEY:
            switch( nextChar )
            {
            case '\t':
            case ' ':
                break;

            case '=':
                state = EQUATE;
                break;

            default:
                state = ERROR;
                break;
            }
            break;

        // [___]KEY[___]=[___][']VALUE['][___][,...]
        //                ^^^  ^ ^
        case EQUATE:
            switch( nextChar )
            {
            case '\t':
            case ' ':
                break;

            case '\'':
                state = VALUE_START_QUOTE;
                break;

            default:
                valueStart = i;
                processingSpaces = false;

                state = VALUE;
                break;
            }
            break;

        // [___]KEY[___]=[___]'VALUE'[___][,...]
        //                     ^    ^
        case VALUE_START_QUOTE:
            switch( nextChar )
            {
            case '\'':
                value = QString();
                substituteKey( result, key, value );

                state = POST_VALUE;
                break;

            default:
                valueStart = i;

                state = VALUE_QUOTED;
                break;
            }
            break;

        // [___]KEY[___]=[___]VALUE[___][,...]
        //                     ^^^^ ^^^  ^
        case VALUE:
            switch( nextChar )
            {
            case '\t':
            case ' ':
                if( !processingSpaces )
                {
                    processingSpaces = true;  // Working through spaces that may be in the middle of the value
                    valueEnd = i;             // This will mark the value end if there are no more non white space value characters
                }
                break;

            case ',':
                if( !processingSpaces )
                {
                    valueEnd = i;
                }
                value = subs.mid( valueStart, valueEnd-valueStart );
                substituteKey( result, key, value );

                processingSpaces = false;
                state = PRE_KEY;
                break;

            default:
                processingSpaces = false;   // No longer working through spaces that may be in the middle of the value
                break;
            }
            break;

        // [___]KEY[___]=[___]'VALUE'[___][,...]
        //                      ^^^^^
        case VALUE_QUOTED:
            switch( nextChar )
            {
            case '\'':
                valueEnd = i;

                value = subs.mid( valueStart, valueEnd-valueStart );
                substituteKey( result, key, value );

                state = POST_VALUE;
                break;

            default:
                break;
            }
            break;

        // [___]KEY[___]=[___][']VALUE['][___][,...]
        //                                ^^^  ^
        case POST_VALUE:
            switch( nextChar )
            {
            case '\t':
            case ' ':
                break;

            case ',':
                state = PRE_KEY;
                break;

            default:
                state = ERROR;
                break;
            }
            break;
        }
    }

    // Use last value
    if( state == VALUE )
    {
        if( !processingSpaces )
        {
            valueEnd = i;
        }
        value = subs.mid( valueStart, valueEnd-valueStart );
        substituteKey( result, key, value );
    }

    // Return the string with substitutions applied
    return result;
}

// Replace occurances of a single key with a value
void VariableNameManager::substituteKey( QString& string, QString key, const QString value )
{
    key.prepend( "$(" );
    key.append( ")" );
    string.replace( key, value );
}
