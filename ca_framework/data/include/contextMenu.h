#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QMenu>

class contextMenu : public QMenu
{
    Q_OBJECT
public:

    explicit contextMenu( QWidget *parent = 0 );

    enum contextMenuOptions{ CM_NONE, CM_COPY_VARIABLE, CM_COPY_DATA, CM_DRAG_VARIABLE, CM_DRAG_DATA };
    void getContextMenuOption( const QPoint& pos, contextMenuOptions* option, bool* checked );

signals:

public slots:
    void contextMenuTriggered( QAction* selectedItem );
    void showContextMenu( const QPoint& pos );

private:

};

#endif // CONTEXTMENU_H
