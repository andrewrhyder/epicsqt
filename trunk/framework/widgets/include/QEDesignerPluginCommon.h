/*  QEDesignerPluginCommon.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
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
 *  Copyright (c) 2014 Australian Synchrotron.
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef QE_DESIGNER_PLUGIN_COMMON_H
#define QE_DESIGNER_PLUGIN_COMMON_H

// Macro to do all the designer plugin plumbing for straight forward widgets.
// If plugin defines design time dialog (as per QEPeriodic) then this macro is
// not suitable.
//
// Macro formal parameters:
// widgetName    - class type name (no quotes). The manager class name must be the
//                 class name with Manager appened, and the include file name must
//                 be the class name with ".h" appended, e.g.:
//                 QENumericEdit, QENumericEditManager, "QENumericEdit.h"
// groupName     - string - typically "EPICS Widgets'
// iconFilepath  - string - typically a resoure file ":/qe/.../classname.png"
// containerFlag - true/false - typically false except for QEFrame, QEGroupBox and
//                 other container widgets.
//
#define QE_IMPLEMENT_PLUGIN(widgetName, groupName, iconFilepath, containerFlag)    \
                                                                                   \
widgetName##Manager::widgetName##Manager (QObject* parent) : QObject( parent ) {   \
   initialized = false;                                                            \
}                                                                                  \
                                                                                   \
void widgetName##Manager::initialize (QDesignerFormEditorInterface*)               \
{                                                                                  \
   if (initialized) {                                                              \
      return;                                                                      \
   }                                                                               \
   initialized = true;                                                             \
}                                                                                  \
                                                                                   \
bool widgetName##Manager::isInitialized () const {                                 \
   return initialized;                                                             \
}                                                                                  \
                                                                                   \
QWidget* widgetName##Manager::createWidget (QWidget* parent) {                     \
   return new widgetName (parent);                                                 \
}                                                                                  \
                                                                                   \
QString widgetName##Manager::name() const {                                        \
   return #widgetName;                                                             \
}                                                                                  \
                                                                                   \
QString widgetName##Manager::group() const {                                       \
   return groupName;                                                               \
}                                                                                  \
                                                                                   \
QIcon widgetName##Manager::icon() const {                                          \
   return QIcon (iconFilepath);                                                    \
}                                                                                  \
                                                                                   \
QString widgetName##Manager::toolTip() const {                                     \
   return #widgetName;                                                             \
}                                                                                  \
                                                                                   \
QString widgetName##Manager::whatsThis() const {                                   \
   return #widgetName;                                                             \
}                                                                                  \
                                                                                   \
bool widgetName##Manager::isContainer() const {                                    \
   return containerFlag;                                                           \
}                                                                                  \
                                                                                   \
QString widgetName##Manager::includeFile() const  {                                \
   return #widgetName".h";                                                         \
}


#endif  // QE_DESIGNER_PLUGIN_COMMON_H
