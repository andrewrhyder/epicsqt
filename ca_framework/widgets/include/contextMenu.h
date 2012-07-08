#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QMenu>

class contextMenu : public QMenu
{
    Q_OBJECT
public:

    explicit contextMenu( QWidget *parent = 0 );


    enum contextMenuOptions{ CM_NONE,
                             CM_SAVE,
                             CM_PAUSE,
                             CM_ENABLE_TIME,
                             CM_ENABLE_CURSOR_PIXEL,
                             CM_ENABLE_PAN,
                             CM_ENABLE_VERT,
                             CM_ENABLE_HOZ,
                             CM_ENABLE_AREA,
                             CM_ENABLE_LINE,
                             CM_DISPLAY_BUTTON_BAR,
                             CM_DISPLAY_ROI_INFO,
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
                             CM_ZOOM_400,
                             CM_ROTATE_NONE,
                             CM_ROTATE_RIGHT,
                             CM_ROTATE_LEFT,
                             CM_ROTATE_180,
                             CM_FLIP_HORIZONTAL,
                             CM_FLIP_VERTICAL,
                             CM_SELECT_PAN,
                             CM_SELECT_HSLICE,
                             CM_SELECT_VSLICE,
                             CM_SELECT_AREA,
                             CM_SELECT_PROFILE
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
