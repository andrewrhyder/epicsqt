#ifndef ZOOMMENU_H
#define ZOOMMENU_H

#include <QMenu>
#include <contextMenu.h>

class zoomMenu : public QMenu
{
    Q_OBJECT
public:

    explicit zoomMenu( const bool areaSelected, QWidget *parent = 0 );

    contextMenu::contextMenuOptions getZoom( const QPoint& pos );

signals:

public slots:

private:
};

#endif // ZOOMMENU_H
