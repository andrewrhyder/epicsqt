#ifndef IMAGECONTEXTMENU_H
#define IMAGECONTEXTMENU_H

#include <contextMenu.h>
#include <QMenu>

class imageContextMenu : public QMenu
{
    Q_OBJECT
public:

    explicit imageContextMenu( QWidget *parent = 0 );


    enum imageContextMenuOptions{ ICM_NONE = contextMenu::CM_SPECIFIC_WIDGETS_START_HERE,
                                  ICM_SAVE,
                                  ICM_PAUSE,
                                  ICM_ENABLE_TIME,
                                  ICM_ENABLE_CURSOR_PIXEL,
                                  ICM_ENABLE_PAN,
                                  ICM_ENABLE_VERT,
                                  ICM_ENABLE_HOZ,
                                  ICM_ENABLE_AREA,
                                  ICM_ENABLE_LINE,
                                  ICM_ENABLE_TARGET,
                                  ICM_DISPLAY_BUTTON_BAR,
                                  ICM_ZOOM_SELECTED,
                                  ICM_ZOOM_FIT,
                                  ICM_ZOOM_10,
                                  ICM_ZOOM_25,
                                  ICM_ZOOM_50,
                                  ICM_ZOOM_75,
                                  ICM_ZOOM_100,
                                  ICM_ZOOM_150,
                                  ICM_ZOOM_200,
                                  ICM_ZOOM_300,
                                  ICM_ZOOM_400,
                                  ICM_ROTATE_NONE,
                                  ICM_ROTATE_RIGHT,
                                  ICM_ROTATE_LEFT,
                                  ICM_ROTATE_180,
                                  ICM_FLIP_HORIZONTAL,
                                  ICM_FLIP_VERTICAL,
                                  ICM_SELECT_PAN,
                                  ICM_SELECT_HSLICE,
                                  ICM_SELECT_VSLICE,
                                  ICM_SELECT_AREA,
                                  ICM_SELECT_PROFILE,
                                  ICM_SELECT_TARGET,
                                  ICM_SELECT_BEAM
                                };


    void getContextMenuOption( const QPoint&, imageContextMenuOptions* option, bool* checked );
    void addMenuItem( const QString& title, const bool checkable, const bool checked, const imageContextMenuOptions option );
    void addOptionMenuItem( const QString& title, const bool checkable, const bool checked, const imageContextMenuOptions option );

signals:

public slots:

private:
    void addMenuItemGeneric( QMenu* menu, const QString& title, const bool checkable, const bool checked, const imageContextMenuOptions option );

    QMenu* optionMenu;
};

#endif // IMAGECONTEXTMENU_H
