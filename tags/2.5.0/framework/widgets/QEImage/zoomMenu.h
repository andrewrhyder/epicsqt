#ifndef ZOOMMENU_H
#define ZOOMMENU_H

#include <QMenu>
#include <imageContextMenu.h>

class zoomMenu : public QMenu
{
    Q_OBJECT
public:

    explicit zoomMenu( QWidget *parent = 0 );

    void enableAreaSelected( bool enable );
    imageContextMenu::imageContextMenuOptions getZoom( const QPoint& pos );

signals:

public slots:

private:
    QAction* areaSelectedAction;

};

#endif // ZOOMMENU_H
