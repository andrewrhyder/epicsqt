#include "contextMenu.h"
#include <QClipboard>
#include <QApplication>
#include <QDebug>

// Create a context menu
contextMenuObject::contextMenuObject( QWidget *parent, contextMenu* menuIn ) : QMenu(parent)
{
    menu = menuIn;
    owner = NULL;

    QAction* a;
    a = new QAction( "Copy variable name", this ); a->setCheckable( false ); a->setData( contextMenu::CM_COPY_VARIABLE );  addAction( a );
    a = new QAction( "Copy data",          this ); a->setCheckable( false ); a->setData( contextMenu::CM_COPY_DATA );      addAction( a );
    a = new QAction( "Paste",              this ); a->setCheckable( false ); a->setData( contextMenu::CM_PASTE );          addAction( a );
    addSeparator();
    a = new QAction( "Drag variable name", this ); a->setCheckable( true  ); a->setData( contextMenu::CM_DRAG_VARIABLE  ); addAction( a );
    dragVarAction = a;
    a = new QAction( "Drag data",          this ); a->setCheckable( true  ); a->setData( contextMenu::CM_DRAG_DATA  );     addAction( a );
    dragDataAction = a;

    dragVarAction->setChecked( true );
    dragDataAction->setChecked( false );

    setTitle( "Standard" );

    QObject::connect( this, SIGNAL( triggered ( QAction* ) ), this, SLOT( contextMenuTriggered( QAction* )) );

}

// An action was selected from this menu
void contextMenuObject::contextMenuTriggered( QAction* selectedItem )
{
    menu->triggered( (contextMenu::contextMenuOptions)(selectedItem->data().toInt()) );
}

// Slot to present this menu when it is the context menu for a widget
void contextMenuObject::showContextMenu( const QPoint& pos )
{
    QPoint globalPos = owner->mapToGlobal( pos );
    exec( globalPos );
}

// Add the context menu to the widget as the entire context menu
// Widgets may create their own context menu and add this menu as a sub menu
void contextMenuObject::addContextMenuToWidget( QWidget* w )
{
    owner = w;
    w->setContextMenuPolicy( Qt::CustomContextMenu );
    QObject::connect( w, SIGNAL( customContextMenuRequested( const QPoint& )), this, SLOT( showContextMenu( const QPoint& )));
}

void contextMenuObject::manageChecked( bool draggingVariable )
{
        dragVarAction->setChecked( draggingVariable );
        dragDataAction->setChecked( !draggingVariable );
}

//======================================================

contextMenu::contextMenu( QWidget *parent )
{
    object = new contextMenuObject( parent, this );
    draggingVariable = true;
}

contextMenu::~contextMenu()
{
    delete object;
}

void contextMenu::triggered( contextMenuOptions option )
{
    switch( option )
    {
        default:
        case contextMenu::CM_NONE:
            break;

        case contextMenu::CM_COPY_VARIABLE:
            doCopyVariable();
            break;

        case contextMenu::CM_COPY_DATA:
            doCopyData();
            break;

        case contextMenu::CM_PASTE:
            doPaste();
            break;

        case contextMenu::CM_DRAG_VARIABLE:
            draggingVariable = true;
            object->manageChecked( draggingVariable );
            break;

        case contextMenu::CM_DRAG_DATA:
            draggingVariable = false;
            object->manageChecked( draggingVariable );
            break;

    }
}

void contextMenu::doCopyVariable()
{
    qDebug() << "contextMenu::doCopyVariable()";
    QString s = copyVariable();
    QClipboard *cb = QApplication::clipboard();
    cb->setText( s );
}

void contextMenu::doCopyData()
{
    qDebug() << "contextMenu::doCopyData()";
    QClipboard *cb = QApplication::clipboard();
    QVariant v = copyData();
    switch( v.type() )
    {
        default:
        case QVariant::String:
            cb->setText( v.toString() );
            break;

        case QVariant::Image:
            cb->setImage( v.value<QImage>() );
            break;
    }
}

//void contextMenu::copyToClipboard( QString text )
//{
//    qDebug() << "contextMenu::copyToClipboard()" << text;
//    //!! copy text to clipboard
//}

void contextMenu::doPaste()
{
    qDebug() << "contextMenu::doPaste()";

    QVariant v;
    QClipboard *cb = QApplication::clipboard();
    if( !cb->text().isEmpty() )
    {
        v = QVariant( cb->text() );
    }
    else if( !cb->image().isNull() )
    {
        v = QVariant( cb->image() );
    }
    paste( v );
}

bool contextMenu::isDraggingVariable()
{
    return draggingVariable;
}

