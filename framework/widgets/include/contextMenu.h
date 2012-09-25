#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QMenu>

class contextMenu;

class contextMenuObject : public QMenu
{
    Q_OBJECT
public:

    contextMenuObject();

    void addContextMenuToWidget( QWidget* w );

    void manageChecked( bool draggingVariable );

    void setMenu( contextMenu* menuIn );
    bool isDraggingVariable();

signals:

public slots:
    void contextMenuTriggered( QAction* selectedItem );
    void showContextMenu( const QPoint& pos );
    void setChecked();

private:
    static bool draggingVariable;
    QAction* dragVarAction;
    QAction* dragDataAction;
    QWidget* owner;
    contextMenu* menu;
};

class contextMenu
{
public:
    friend class contextMenuObject;

    explicit contextMenu();
    virtual ~contextMenu();

    enum contextMenuOptions{ CM_NONE, CM_COPY_VARIABLE, CM_COPY_DATA, CM_PASTE, CM_DRAG_VARIABLE, CM_DRAG_DATA, CM_SPECIFIC_WIDGETS_START_HERE };

    void addContextMenuToWidget( QWidget* w );
    bool isDraggingVariable();
    QMenu* getContextMenu();

    virtual QString copyVariable(){ return ""; }
    virtual QVariant copyData(){ return ""; }
    virtual void paste( QVariant ){}

private:
    void triggered( contextMenuOptions option );
    contextMenuObject object;
    void doCopyVariable();
    void doCopyData();
    void doPaste();
};


#endif // CONTEXTMENU_H
