#ifndef ZOOMMENU_H
#define ZOOMMENU_H

#include <QMenu>
#include <contextMenu.h>

class zoomMenu : public QMenu
{
    Q_OBJECT
public:

    explicit zoomMenu( QWidget *parent = 0 );

    void enableAreaSelected( bool enable );
    contextMenu::contextMenuOptions getZoom( const QPoint& pos );

signals:

public slots:

private:
    QAction* areaSelectedAction;

};

#endif // ZOOMMENU_H
