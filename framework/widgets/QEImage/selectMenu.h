#ifndef SELECTMENU_H
#define SELECTMENU_H

#include <QMenu>
#include <imageContextMenu.h>

class selectMenu : public QMenu
{
    Q_OBJECT
public:

    explicit selectMenu( QWidget *parent = 0 );

    imageContextMenu::imageContextMenuOptions getSelectOption( const QPoint& pos );

    void setChecked( const int mode );

    void setPanEnabled( bool enablePan );
    void setVSliceEnabled( bool enableVSliceSelection );
    void setHSlicetEnabled( bool enableHSliceSelection );
    void setAreaEnabled( bool enableAreaSelection );
    void setProfileEnabled( bool enableProfileSelection );
    void setTargetEnabled( bool enableTargetSelection );

signals:

public slots:

private:
    QAction* actionPan;
    QAction* actionHSlice;
    QAction* actionVSlice;
    QAction* actionArea;
    QAction* actionProfile;
    QAction* actionTarget;
    QAction* actionBeam;
};

#endif // SELECTMENU_H
