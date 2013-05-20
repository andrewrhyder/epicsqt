#ifndef QEGUI_H
#define QEGUI_H

#include <QApplication>
#include <StartupParams.h>
#include <ContainerProfile.h>
#include <MainWindow.h>


class guiListItem
{
public:
//    guiListItem(){ form = NULL; mainWindow = NULL; }
    guiListItem( QEForm* formIn, MainWindow* mainWindowIn ){ form = formIn; mainWindow = mainWindowIn; }
    QEForm* getForm(){ return form; }
    MainWindow* getMainWindow(){ return mainWindow; }
    void setScroll( QPoint scrollIn ){ scroll = scrollIn; }
    QPoint getScroll(){ return scroll; }
//   =guiListItem(const guiListItem& other){ other.form = form; other.mainWindow = mainWindow; }
private:
    QEForm* form;
    MainWindow* mainWindow;
    QPoint scroll;
};

class QEGui : public QApplication, ContainerProfile
{
public:
    QEGui( int argc, char *argv[] );

    int run();

    startupParams* getParams();

    int getMainWindowCount();
    MainWindow* getMainWindow( int i );
    void addMainWindow( MainWindow* window );

    int getGuiCount();
    QEForm* getGuiForm( int i );
    MainWindow* getGuiMainWindow( int i );
    QPoint getGuiScroll( int i );
    void setGuiScroll( int i, QPoint scroll );
    void addGui( QEForm* gui, MainWindow* window );
    bool removeGui( QEForm* gui );
    int getMainWindowPosition( MainWindow* mw );
    void removeMainWindow( MainWindow* window );
    void removeMainWindow( int i );

signals:

public slots:

private:
    void printVersion ();
    void printUsage (std::ostream & stream);
    void printHelp ();

    startupParams params;
//    int count();                                     // Static function to report the number of main windows
    QList<guiListItem> guiList;                      // List of all forms being displayed in all main windows
    QList<MainWindow*> mainWindowList;               // List of all main windows
};

#endif // QEGUI_H
