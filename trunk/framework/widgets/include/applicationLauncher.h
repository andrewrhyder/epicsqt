/*
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

/*
 This class manages applications launched from the QEImage widget to present a
 snapshot of the current image in another application.
 */

#ifndef APPLICATIONLAUNCHER_H
#define APPLICATIONLAUNCHER_H

#include <QImage>
#include <QString>
#include <QProcess>
#include <QTemporaryFile>
#include <VariableNameManager.h>
#include <UserMessage.h>


// Class to manage a process started by a QE button
class processManager : public QProcess
{
    Q_OBJECT

public:
    processManager( bool logOutput, QTemporaryFile* tempFileIn )
    {
        tempFile = tempFileIn;

        // Catch when the process can be deleted
        QObject::connect( this, SIGNAL( finished(int, QProcess::ExitStatus) ), this, SLOT( doFinished(int, QProcess::ExitStatus) ) );

        // Catch output if required
        if( logOutput )
        {
            QObject::connect( this, SIGNAL( readyReadStandardOutput() ), this, SLOT( doRead() ) );
            QObject::connect( this, SIGNAL( readyReadStandardError() ), this, SLOT( doRead() ) );
        }
    }
    virtual ~processManager()
    {
        qDebug() << "processManager destructor called";
        if( tempFile )
        {
            delete tempFile;
        }

    }

signals:
    void processCompleted();

public slots:
    void doRead()
    {
        message.sendMessage( readAll() );
    }
    void doFinished( int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/ )
    {
        emit processCompleted();
        deleteLater();
    }

private:
    UserMessage message;
    QTemporaryFile* tempFile;
    QObject* receiver;
};


class applicationLauncher
{
public:

    applicationLauncher();
    ~applicationLauncher();

    void launchImage( VariableNameManager* variableNameManager, QImage image );
    void launch( VariableNameManager* variableNameManager, QObject* receiver );
    void launchCommon( VariableNameManager* variableNameManager, QTemporaryFile* tempFile = NULL, QObject* receiver = NULL );

    // Program String
    void setProgram( QString programIn ){ program = programIn; }
    QString getProgram(){ return program; }

    // Arguments String
    void setArguments( QStringList argumentsIn ){ arguments = argumentsIn; }
    QStringList getArguments(){ return  arguments; }

    // Startup option
    enum programStartupOptions { PSO_NONE,         // Just run the program
                                 PSO_TERMINAL,     // Run the program in a termainal
                                 PSO_LOGOUTPUT };  // Run the program, and log the output in the QE message system
    void setProgramStartupOption( programStartupOptions programStartupOptionIn ){ programStartupOption = programStartupOptionIn; }
    programStartupOptions getProgramStartupOption(){ return programStartupOption; }


protected:

private slots:

private:
    QString program;        // Program to run
    QStringList arguments;  // Program arguments
    programStartupOptions programStartupOption; // Startup option (in a terminal, log output, or just start it and forget it)
    UserMessage message;
};

#endif // APPLICATIONLAUNCHER_H