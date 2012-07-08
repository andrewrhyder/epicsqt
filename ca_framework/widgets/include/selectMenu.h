#ifndef SELECTMENU_H
#define SELECTMENU_H

#include <QMenu>
#include <contextMenu.h>

class selectMenu : public QMenu
{
    Q_OBJECT
public:

    explicit selectMenu( QWidget *parent = 0 );

    contextMenu::contextMenuOptions getSelectOption( const QPoint& pos );

    void setChecked( const int mode );

    void setPanEnabled( bool enablePan );
    void setVSliceEnabled( bool enableVSliceSelection );
    void setHSlicetEnabled( bool enableHSliceSelection );
    void setAreaEnabled( bool enableAreaSelection );
    void setProfileEnabled( bool enableProfileSelection );

signals:

public slots:

private:
    QAction* actionPan;
    QAction* actionHSlice;
    QAction* actionVSlice;
    QAction* actionArea;
    QAction* actionProfile;

};

#endif // SELECTMENU_H
