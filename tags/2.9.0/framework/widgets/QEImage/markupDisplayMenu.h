#ifndef MARKUPDISPLAYMENU_H
#define MARKUPDISPLAYMENU_H

#include <QMenu>
#include "imageContextMenu.h"

class markupDisplayMenu : public QMenu
{
    Q_OBJECT
public:

    explicit markupDisplayMenu( QWidget *parent = 0 );

    void setDisplayed( imageContextMenu::imageContextMenuOptions option, bool state );
    void setItemText( imageContextMenu::imageContextMenuOptions option, QString title );
    bool isDisplayed( imageContextMenu::imageContextMenuOptions option );
    void enable( imageContextMenu::imageContextMenuOptions option, bool state );

signals:

private:

    QAction* getAction( imageContextMenu::imageContextMenuOptions option );

    QAction* hSliceAction;
    QAction* vSliceAction;
    QAction* area1Action;
    QAction* area2Action;
    QAction* area3Action;
    QAction* area4Action;
    QAction* profileAction;
    QAction* targetAction;
    QAction* beamAction;
    QAction* timestampAction;
    QAction* ellipseAction;
};

#endif // MARKUPDISPLAYMENU_H
