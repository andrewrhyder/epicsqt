/*  QEDevicePadManager.cpp
 */

#include <QEDevicePadManager.h>
#include <QEDevicePad.h>
#include <QtPlugin>

//---------------------------------------------------------------------------------
//
QEDevicePadManager::QEDevicePadManager( QObject *parent ) : QObject( parent ) {
   initialized = false;
}

//---------------------------------------------------------------------------------
//
void QEDevicePadManager::initialize( QDesignerFormEditorInterface * ) {
   if (initialized) {
      return;
   }
   initialized = true;
}

//---------------------------------------------------------------------------------
//
bool QEDevicePadManager::isInitialized() const {
   return initialized;
}

//---------------------------------------------------------------------------------
// Widget factory. Creates a QEDevicePad widget.
//
QWidget *QEDevicePadManager::createWidget ( QWidget *parent ) {
   return new QEDevicePad(parent);
}

//---------------------------------------------------------------------------------
// Name for widget. Used by Qt Designer in widget list.
//
QString QEDevicePadManager::name() const {
   return "QEDevicePad";
}

//---------------------------------------------------------------------------------
// Name of group Qt Designer will add widget to.
//
QString QEDevicePadManager::group() const {
   return "EPICS Porject-based Widgets";
}

//---------------------------------------------------------------------------------
// Icon for widget. Used by Qt Designer in widget list.
//
QIcon QEDevicePadManager::icon() const {
   return QIcon(":/qe/project_based/QEDevicePad.png");
}

//---------------------------------------------------------------------------------
// Tool tip for widget. Used by Qt Designer in widget list.
//
QString QEDevicePadManager::toolTip() const {
   return "EPICS Device Control";
}

//---------------------------------------------------------------------------------
//
QString QEDevicePadManager::whatsThis() const {
   return "EPICS Device Pad";
}

//---------------------------------------------------------------------------------
//
bool QEDevicePadManager::isContainer() const {
   return false;
}

//---------------------------------------------------------------------------------
//
/*QString QEDevicePadManager::domXml() const {
    return "<widget class=\"QEDevicePad\" name=\"qEDevicePad\">\n"
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
QString QEDevicePadManager::includeFile() const {
   return "QEDevicePad.h";
}

// end
