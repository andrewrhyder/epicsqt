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
 *  Copyright (c) 2009, 2010
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */
/* This class is used to manage saving and restoring for QEGui.
   The main item saved is the number of main windows saved so the restore will know how many main windows to restore.
   Each main window will save it's own info.
*/

#include <saveRestoreManager.h>
#include <MainWindow.h>

#define SAVERESTORE_NAME "QEGui"

// Construction
saveRestoreManager::saveRestoreManager( startupParams* paramsIn )
{
    params = paramsIn;

    // Setup to respond to requests to save or restore persistant data
    PersistanceManager* persistanceManager = profile.getPersistanceManager();
    QObject::connect( persistanceManager->getSaveRestoreObject(), SIGNAL( saveRestore( SaveRestoreSignal::saveRestoreOptions ) ), this, SLOT( saveRestore( SaveRestoreSignal::saveRestoreOptions ) ), Qt::DirectConnection );
}

// Destruction
saveRestoreManager::~saveRestoreManager()
{
}

// A save or restore has been requested (Probably by QEGui itself)
void saveRestoreManager::saveRestore( SaveRestoreSignal::saveRestoreOptions option )
{
    PersistanceManager* pm = profile.getPersistanceManager();

    switch( option )
    {
    case SaveRestoreSignal::SAVE:
        {
            // Start with the top level element - the QEGui application
            PMElement appElement =  pm->addElement( SAVERESTORE_NAME );

            appElement.addAttribute( "MainWindows", MainWindow::count() );
        }
        break;


    case SaveRestoreSignal::RESTORE:
        {
            // Get the data for this application
            PMElement QEGuiData = pm->getMyData( SAVERESTORE_NAME );

            // If none, do nothing
            if( QEGuiData.isNull() )
            {
                return;
            }

            // Get the number of expected main windows
            int numMainWindows = 0;
            QEGuiData.getElementAttribute( "MainWindows", numMainWindows );

            // Create the main windows. They will restore themselves
            ContainerProfile profile;
            profile.setupProfile( NULL, params->pathList, "", params->substitutions, params->userLevelPassword, params->scientistLevelPassword, params->engineerLevelPassword );
            for( int i = 0; i < numMainWindows; i++ )
            {
                MainWindow* mw = new MainWindow( "", false, params->enableEdit, params->disableMenu );
                mw->show();
            }

            profile.releaseProfile();
        }
        break;
    }

}


