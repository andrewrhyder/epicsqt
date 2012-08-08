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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

/*
 * USAGE:
 * Include this file in a plugin widget definition prior to declaring variables.
 * NUM_VARIABLES should be declared prior to including this file, then
 * variables should be declared after including this file.
 * Finally, multipleVariablePropertiesTail.inc should be included.
 * for example:

        //==========================================================================
        // Standard properties

        #define NUM_VARIABLES QEIMAGE_NUM_VARIABLES
        #include <multipleVariablePropertiesBase.inc>

            VARIABLE_PROPERTY_ACCESS(0)
            Q_PROPERTY(QString imageVariable READ getVariableName0Property WRITE setVariableName0Property)

            VARIABLE_PROPERTY_ACCESS(1)
            Q_PROPERTY(QString widthVariable READ getVariableName1Property WRITE setVariableName1Property)

            VARIABLE_PROPERTY_ACCESS(2)
            Q_PROPERTY(QString heightVariable READ getVariableName2Property WRITE setVariableName2Property)

            VARIABLE_PROPERTY_ACCESS(3)
            Q_PROPERTY(QString regionOfInterestXVariable READ getVariableName3Property WRITE setVariableName3Property)

            VARIABLE_PROPERTY_ACCESS(4)
            Q_PROPERTY(QString regionOfInterestYVariable READ getVariableName4Property WRITE setVariableName4Property)

            VARIABLE_PROPERTY_ACCESS(5)
            Q_PROPERTY(QString regionOfInterestWVariable READ getVariableName5Property WRITE setVariableName5Property)

            VARIABLE_PROPERTY_ACCESS(6)
            Q_PROPERTY(QString regionOfInterestHVariable READ getVariableName6Property WRITE setVariableName6Property)

        #include <multipleVariablePropertiesTail.inc>

* NOTE: a slot is also required in the plugin widget definition which should be in the following form

        private slots:
            void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )
            {
                setVariableNameAndSubstitutions(variableNameIn, variableNameSubstitutionsIn, variableIndex);
            }

*/

#ifndef MULTIPLEVARIABLEPROPERTIESBASE_INC
#define MULTIPLEVARIABLEPROPERTIESBASE_INC

  private:
    QCaVariableNamePropertyManager variableNamePropertyManagers[NUM_VARIABLES];
  public:

// Define a variable
// Note, the QPROPERTY declaration itself can't be in this macro
#define VARIABLE_PROPERTY_ACCESS(VAR_INDEX) \
    void    setVariableName##VAR_INDEX##Property( QString variableName ){ variableNamePropertyManagers[VAR_INDEX].setVariableNameProperty( variableName ); } \
    QString getVariableName##VAR_INDEX##Property(){ return variableNamePropertyManagers[VAR_INDEX].getVariableNameProperty(); }

#endif // MULTIPLEVARIABLEPROPERTIESBASE_INC
