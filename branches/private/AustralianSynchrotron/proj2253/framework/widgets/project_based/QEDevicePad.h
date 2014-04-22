/*  QEDeviceControl.h
 */

#ifndef QEDEVICEPAD_H
#define QEDEVICEPAD_H

#include "QEScratchPad.h"
#include <QEForm.h>

class QEPLUGINLIBRARYSHARED_EXPORT QEDevicePad : public QEScratchPad {
   Q_OBJECT
public:
    explicit QEDevicePad (QWidget *parent = 0, QString devCtrlUiFile = QString());
   ~QEDevicePad();
   void    setPvName (const int slot, const QString& pvName);

   Q_PROPERTY(QString guiFile READ getGuiName WRITE setGuiName)
   // GUI name
   void setGuiName( QString guiName ){ uiFileName = guiName; }
   QString getGuiName(){ return uiFileName; }

   Q_PROPERTY(QString nameListFile READ getListFileName WRITE setListFileName)
   // GUI name
   void setListFileName( QString listFileName );
   QString getListFileName(){ return deviceListFileName; }

protected:
   class DeviceDataSets : public QEScratchPad::BaseDataSets {
   public:
       explicit DeviceDataSets (): QEScratchPad::BaseDataSets() {}

      void setHighLighted (const bool isHigh);

      QEForm* form;
   };

   // Internal widgets.
   QString uiFileName;
   QString deviceListFileName;

   void createInternalWidgets ();
   void calcMinimumHeight ();

   void saveConfiguration (PersistanceManager* pm);
   void restoreConfiguration (PersistanceManager* pm, restorePhases restorePhase);

private slots:
   void contextMenuSelected  (const int slot, const QEScratchPadMenu::ContextMenuOptions option);

};

#endif // QEDEVICEPAD_H
