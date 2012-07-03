#ifndef FLIPROTATEMENU_H
#define FLIPROTATEMENU_H

#include <QMenu>
#include "contextMenu.h"
#include "QCaImage.h"

class flipRotateMenu : public QMenu
{
    Q_OBJECT
public:

    explicit flipRotateMenu( const QCaImage::rotationOptions rotation, const bool flipH, const bool flipV, QWidget *parent = 0 );

    contextMenu::contextMenuOptions getFlipRotate( const QPoint& pos );

signals:

public slots:

private:
};

#endif // FLIPROTATEMENU_H
