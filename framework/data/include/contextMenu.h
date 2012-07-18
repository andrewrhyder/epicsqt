#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QMenu>

class contextMenuObject;

class contextMenu
{
public:
    friend class contextMenuObject;

    explicit contextMenu( QWidget *parent = 0 );
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
    contextMenuObject* object;
    void doCopyVariable();
    void doCopyData();
    void doPaste();
    bool draggingVariable;
};

class contextMenuObject : public QMenu
{
    Q_OBJECT
public:

    contextMenuObject( QWidget *parent, contextMenu* menuIn );

    void addContextMenuToWidget( QWidget* w );

    void manageChecked( bool draggingVariable );

signals:

public slots:
    void contextMenuTriggered( QAction* selectedItem );
    void showContextMenu( const QPoint& pos );

private:
    QAction* dragVarAction;
    QAction* dragDataAction;
    QWidget* owner;
    contextMenu* menu;
};

#endif // CONTEXTMENU_H
