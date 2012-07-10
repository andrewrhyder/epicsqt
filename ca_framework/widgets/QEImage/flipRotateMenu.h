#ifndef FLIPROTATEMENU_H
#define FLIPROTATEMENU_H

#include <QMenu>
#include "contextMenu.h"

class flipRotateMenu : public QMenu
{
    Q_OBJECT
public:

    explicit flipRotateMenu( QWidget *parent = 0 );

    contextMenu::contextMenuOptions getFlipRotate( const QPoint& pos );
    void setChecked( const int rotation, const bool flipH, const bool flipV );

signals:

public slots:

private:
    QAction* rotationNoneAction;
    QAction* rotation90RAction;
    QAction* rotation90LAction;
    QAction* rotation180Action;
    QAction* flipHAction;
    QAction* flipVAction;
};

#endif // FLIPROTATEMENU_H
