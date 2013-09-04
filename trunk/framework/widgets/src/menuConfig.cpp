#include "menuConfig.h"

menuConfigItem::menuConfigItem( QString titleIn ) : QAction( titleIn, this )
{
    title = titleIn;

    connect(this, SIGNAL(triggered()), this, SLOT(itemAction()));
}

void menuConfigItem::itemAction()
{

}


menuConfig::menuConfig()
{
}
