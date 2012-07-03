#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QMenu>

class contextMenu : public QMenu
{
    Q_OBJECT
public:

    explicit contextMenu( QWidget *parent = 0 );


    enum contextMenuOptions{ CM_NONE,
                             CM_PAN,
                             CM_VERT,
                             CM_HOZ,
                             CM_AREA,
                             CM_LINE,
                             CM_SAVE,
                             CM_PAUSE,
                             CM_ENABLE_TIME,
                             CM_ENABLE_CURSOR_PIXEL,
                             CM_ENABLE_PAN,
                             CM_ENABLE_VERT,
                             CM_ENABLE_HOZ,
                             CM_ENABLE_AREA,
                             CM_ENABLE_LINE,
                             CM_DISPLAY_ROI_INFO,
                             CM_DISPLAY_ACQ,
                             CM_DISPLAY_EXP,
                             CM_DISPLAY_SAVE,
                             CM_DISPLAY_PAUSE,
                             CM_DISPLAY_ROI_BUTTON,
                             CM_DISPLAY_ZOOM,
                             CM_ZOOM_SELECTED,
                             CM_ZOOM_FIT,
                             CM_ZOOM_10,
                             CM_ZOOM_25,
                             CM_ZOOM_50,
                             CM_ZOOM_75,
                             CM_ZOOM_100,
                             CM_ZOOM_150,
                             CM_ZOOM_200,
                             CM_ZOOM_300,
                             CM_ZOOM_400
                            };


    void getContextMenuOption( const QPoint&, contextMenuOptions* option, bool* checked );
    void addMenuItem( const QString& title, const bool checkable, const bool checked, const contextMenuOptions option );
    void addOptionMenuItem( const QString& title, const bool checkable, const bool checked, const contextMenuOptions option );

signals:

public slots:

private:
    void addMenuItemGeneric( QMenu* menu, const QString& title, const bool checkable, const bool checked, const contextMenuOptions option );

    QMenu* optionMenu;
};

#endif // CONTEXTMENU_H
