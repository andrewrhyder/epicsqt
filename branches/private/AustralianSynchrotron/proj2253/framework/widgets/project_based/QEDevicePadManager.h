/*  QEDevicePadManager.h
 *
 */

// CA ScratchPad Widget Plugin Manager for designer.

#ifndef QEDEVICEPAD_MANAGER_H
#define QEDEVICEPAD_MANAGER_H

#include <QDesignerCustomWidgetInterface>
#include <QEPluginLibrary_global.h>

class QEPLUGINLIBRARYSHARED_EXPORT QEDevicePadManager : public QObject, public QDesignerCustomWidgetInterface {
   Q_OBJECT
   Q_INTERFACES (QDesignerCustomWidgetInterface)

public:
   QEDevicePadManager (QObject *parent = 0);

   bool isContainer () const;
   bool isInitialized () const;
   QIcon icon () const;
   //QString domXml() const;
   QString group () const;
   QString includeFile () const;
   QString name () const;
   QString toolTip () const;
   QString whatsThis () const;
   QWidget *createWidget (QWidget *parent);
   void initialize (QDesignerFormEditorInterface *core);

private:
   bool initialized;
};

#endif // QEDEVICEPAD_MANAGER_H
