/*  QEScratchPadManager.cpp
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
 *  Copyright (c) 2013
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

// CA ScratchPad Widget Plugin Manager for designer.

#include <QEScratchPadManager.h>
#include <QEScratchPad.h>
#include <QtPlugin>

//---------------------------------------------------------------------------------
//
QEScratchPadManager::QEScratchPadManager( QObject *parent ) : QObject( parent ) {
   initialized = false;
}

//---------------------------------------------------------------------------------
//
void QEScratchPadManager::initialize( QDesignerFormEditorInterface * ) {
   if (initialized) {
      return;
   }
   initialized = true;
}

//---------------------------------------------------------------------------------
//
bool QEScratchPadManager::isInitialized() const {
   return initialized;
}

//---------------------------------------------------------------------------------
// Widget factory. Creates a QEScratchPad widget.
//
QWidget *QEScratchPadManager::createWidget ( QWidget *parent ) {
   return new QEScratchPad(parent);
}

//---------------------------------------------------------------------------------
// Name for widget. Used by Qt Designer in widget list.
//
QString QEScratchPadManager::name() const {
   return "QEScratchPad";
}

//---------------------------------------------------------------------------------
// Name of group Qt Designer will add widget to.
//
QString QEScratchPadManager::group() const {
   return "EPICS Widgets";
}

//---------------------------------------------------------------------------------
// Icon for widget. Used by Qt Designer in widget list.
//
QIcon QEScratchPadManager::icon() const {
   return QIcon(":/qe/scratchpad/QEScratchPad.png");
}

//---------------------------------------------------------------------------------
// Tool tip for widget. Used by Qt Designer in widget list.
//
QString QEScratchPadManager::toolTip() const {
   return "EPICS Scratch Pad";
}

//---------------------------------------------------------------------------------
//
QString QEScratchPadManager::whatsThis() const {
   return "EPICS Scratch Pad";
}

//---------------------------------------------------------------------------------
//
bool QEScratchPadManager::isContainer() const {
   return false;
}

//---------------------------------------------------------------------------------
//
/*QString QEScratchPadManager::domXml() const {
    return "<widget class=\"QEScratchPad\" name=\"qEScratchPad\">\n"
           " <property name=\"geometry\">\n"
           "  <rect>\n"
           "   <x>0</x>\n"
           "   <y>0</y>\n"
           "   <width>100</width>\n"
           "   <height>100</height>\n"
           "  </rect>\n"
           " </property>\n"
           " <property name=\"toolTip\" >\n"
           "  <string></string>\n"
           " </property>\n"
           " <property name=\"whatsThis\" >\n"
           "  <string> "
           ".</string>\n"
           " </property>\n"
           "</widget>\n";
}*/

//---------------------------------------------------------------------------------
//
QString QEScratchPadManager::includeFile() const {
   return "QEScratchPad.h";
}

// end
