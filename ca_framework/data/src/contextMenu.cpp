#include "contextMenu.h"
#include <QDebug>

contextMenu::contextMenu( QWidget *parent) : QMenu(parent)
{
    QAction* a;
    a = new QAction( "Copy variable name", this ); a->setData( CM_COPY_VARIABLE );  addAction( a );
    a = new QAction( "Copy data",          this ); a->setData( CM_COPY_DATA );      addAction( a );
    a = new QAction( "Drag variable name", this ); a->setData( CM_DRAG_VARIABLE  ); addAction( a );
    a = new QAction( "Drag data",          this ); a->setData( CM_DRAG_DATA  );     addAction( a );

    setTitle( "Standard" );

    QObject::connect( this, SIGNAL( triggered ( QAction* ) ), this, SLOT( contextMenuTriggered( QAction* )) );

}


void contextMenu::getContextMenuOption( const QPoint& pos, contextMenuOptions* option, bool* checked )
{
    QAction* selectedItem = exec( pos );
    if( selectedItem )
    {
        *option = (contextMenuOptions)(selectedItem->data().toInt());
        *checked = selectedItem->isChecked();
    }
    else
    {
        *option = CM_NONE;
        *checked = false;
    }
}



void contextMenu::contextMenuTriggered( QAction* selectedItem )
{
    switch( (contextMenu::contextMenuOptions)(selectedItem->data().toInt()) )
    {
        default:
        case contextMenu::CM_NONE:
            break;

        case contextMenu::CM_COPY_VARIABLE:
             break;
        case contextMenu::CM_COPY_DATA:
             break;
        case contextMenu::CM_DRAG_VARIABLE:
             break;
        case contextMenu::CM_DRAG_DATA:
             break;

    }
}


void contextMenu::showContextMenu( const QPoint& pos )
{
    // for most widgets
    QPoint globalPos = mapToGlobal( pos );
    // for QAbstractScrollArea and derived classes you would use:
    // QPoint globalPos = myWidget->viewport()->mapToGlobal(pos);

    contextMenuOptions option;
    bool checked;
    getContextMenuOption( pos, &option, &checked );
    qDebug() << option << checked;

}

