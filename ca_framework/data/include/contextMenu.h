#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QMenu>

class contextMenu;

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

class contextMenu
{
public:

    explicit contextMenu( QWidget *parent = 0 );
    ~contextMenu();

    enum contextMenuOptions{ CM_NONE, CM_COPY_VARIABLE, CM_COPY_DATA, CM_PASTE, CM_DRAG_VARIABLE, CM_DRAG_DATA, CM_SPECIFIC_WIDGETS_START_HERE };

    void addContextMenuToWidget( QWidget* w ){ object->addContextMenuToWidget( w ); }
    bool isDraggingVariable();
    QMenu* getContextMenu(){ return (QMenu*)object; }
    void triggered( contextMenuOptions option );

    virtual QString copyVariable(){ return ""; }
    virtual QString copyData(){ return ""; }
    virtual void paste( QString ){}

private:
    contextMenuObject* object;
    void doCopyVariable();
    void doCopyData();
    void copyToClipboard( QString text );
    void doPaste();
    bool draggingVariable;


};

#endif // CONTEXTMENU_H
