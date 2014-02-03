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

#include <QTemporaryFile>
#include <QMessageBox>
#include "applicationLauncher.h"

#define FILE_KEYWORD "<FILENAME>"

applicationLauncher::applicationLauncher()
{
    programStartupOption = PSO_NONE;
}

// Desctuction
applicationLauncher::~applicationLauncher()
{
}

void applicationLauncher::launchImage( VariableNameManager* variableNameManager, QImage image )
{
    // Do nothing if no program to run
    if( program.isEmpty() )
    {
        return;
    }

    // Create a temporary file containing the image
    QTemporaryFile* tempFile = new QTemporaryFile;
    tempFile->open();
    if( !image.save( tempFile, "TIFF") )
    {
        QMessageBox msgBox;
        msgBox.setText("Can't start application. There is no image available.");
        msgBox.exec();
        return;
    }

    // Launch the program
    launchCommon( variableNameManager, tempFile );
}

void applicationLauncher::launch( VariableNameManager* variableNameManager, QObject* receiver )
{
    launchCommon( variableNameManager, NULL, receiver );
}

void applicationLauncher::launchCommon( VariableNameManager* variableNameManager, QTemporaryFile* tempFile, QObject* receiver )
{
    // Do nothing if no program to run
    if( program.isEmpty() )
    {
        return;
    }

    // Create a new process to run the program
    // (It will be up to the processManager to delete the temporary file if present)
    processManager* process = new processManager( programStartupOption == PSO_LOGOUTPUT, tempFile );

    // Connect to caller if a recipient has been provided
    if( receiver )
    {
        QObject::connect( process, SIGNAL( processCompleted() ), receiver, SLOT( programCompletedSlot() ) );
    }

    // Apply substitutions to the arguments
    QStringList substitutedArguments = arguments;
    for( int i = 0; i < substitutedArguments.size(); i++ )
    {
        substitutedArguments[i] = variableNameManager->substituteThis( substitutedArguments[i] );
    }

    // Apply substitutions to the program name
    QString prog = variableNameManager->substituteThis( program );

    // Build up a single string with the command and arguments and run the program
    bool foundFileKeyword = false;
    for( int i = 0; i < substitutedArguments.size(); i++)
    {
        // Get the next argument
        QString arg = substitutedArguments[i];

        // If the argument contains the filename keyword, and a temorary filename is
        // available, insert the filename into the argument
        if( !foundFileKeyword && tempFile && !tempFile->fileName().isEmpty() && arg.contains( FILE_KEYWORD ) )
        {
            arg.replace( FILE_KEYWORD, tempFile->fileName() );
            foundFileKeyword = true;
        }

        // Add the argument to the command line
        prog.append( " " );
        prog.append( arg );
    }

    // If the filename of the temporary file needs to be added, and has not been added
    // already (to replace a filename keyword in an existing argument), then add the
    // filename as a new argument
    if( !foundFileKeyword && tempFile && !tempFile->fileName().isEmpty() )
    {
        prog.append( " " );
        prog.append( tempFile->fileName() );
    }

    // Add apropriate terminal command if starting up within a terminal
    if( programStartupOption == PSO_TERMINAL )
    {
#ifdef WIN32
        prog.prepend( "cmd.exe /C start " );
#else
        prog.prepend( "xterm -hold -e " );// use $TERM ??
#endif
    }

    // Run the program
    message.sendMessage( QString( "Launching: " ).append( prog ), "Application launcher" );
    process->start( prog );

    // Alternate (and cleaner) way to run the program without building a string containing the program and arguments.
    // (This didn't seem to work when starting EDM with the '-one' switch, perhaps due to the
    //  way EDM checks all arguments are identical when the '-one' switch is present?)
    //process->start( substituteThis( program ), substitutedArguments );
}
