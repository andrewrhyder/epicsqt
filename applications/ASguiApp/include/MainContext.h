#ifndef MAINCONTEXT_H
#define MAINCONTEXT_H

#include <QObject>
#include <StartupParams.h>
#include <MainWindow.h>
#include <qtsingleapplication.h>

// Class to manage multiple instances of this application
// This class holds application startup parameters, recieves signals from other instances of this application
class mainContext : public QObject
{
    Q_OBJECT

  public:
    mainContext( int argc, char *argv[], QWidget* parent = 0 );
    bool handball();                // Pass start up details to an earlier instance of this application if present
    void newWindow();               // Create a new window
    int exec();                     // Start event processing in this application instance

  public slots:
    void newAppRequest( const QString& message);    // Slot to recieve notifications from aonther application instance

  private:
    startupParams params;           // Start up parameters parsed from command line arguments or from serialised arguments passed in shared memory
    QSharedMemory share;            // Shared memory used to pass start up arguments from another application instance
    QtSingleApplication* instance;  // Application instance manger

};

#endif // MAINCONTEXT_H
