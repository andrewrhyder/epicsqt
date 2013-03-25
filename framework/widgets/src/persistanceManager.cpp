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
 *  Copyright (c) 2013
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

/*
 * Description:
 *
 * This class provides a mechanism to manage application persistance
 * Any object - typically an application creating QE widgets can use this manager to save and restore
 * a configuration.
 *
 * - QE widgets that have persistant data and the application creating them such as QEGui
 *   can connect to the persistance manager and be notified by a signal that they should save
 *   or recover data.
 *
 * - The application (typically) can ask the persistance manager to save the current state or
 *   restore a previous named state.
 *
 * - When notified to save or restore, a signal is sent to all interested objects to save or restore data.
 *   The persistance manager provides methods to help build and read XML containing the data.
 *
 * - Each object dealing with the persistance manager uses a unique name to identify itself.
 *   For QE widgets this name is built from its name and the name of all it's ancestors. As these
 *   names are stored in the .ui file, these names will be the same each time the .ui file is loaded.
 *
 *
 * Typical scenarios:
 *
 *  Initial startup:
 *  ================
 *
 *   - QEGui starts, connects to the persistance manager's save and restore signals.
 *
 *   - If required, QEGui asks the persistance manager to restore (startup parameters
 *     control if this is done, and what named set is to be restored)
 *
 *   - QEGui gets a signal to restore. (Yes, it know it's restoring - it just asked!)
 *
 *   - QEGui asks for its data.
 *
 *   - QGui uses the persistance manager to parse the XML restore data it gets and acts on
 *     each item it understands. Typically the data will contain GUI names and positions.
 *
 *   - QEGui creates GUIs as its part of restoring a configuration.
 *
 *   - As GUIs are loaded QE widgets are created. If they have persistance requirements
 *     they will connect to the persistance manager signals. They have missed the
 *     initial restore signal, but that is OK as they ask for restoration data on creation.
 *     If a restoration is in progress they will get their restoration data.
 *
 *   - When QEGui has finished creating all the GUIs required it tells the persistance
 *     manager that the restoration is complete.
 *
 *
 *  Saving: (instigated by the user or when QEGui exits)
 *  =======
 *
 *   - QEGui asks the persistance manager to save, providing a name to tag the
 *     saved data with.
 *
 *   - The persistance manager sends a 'save' signal.
 *
 *   - All objects connected to the signal (QEGui itself and any interested QE
 *     widgets) use the persistance manager services to build an XML packet and
 *     then give that packet to the persistance manager.
 *
 *   - The persistance manager collects all packets togeather and saves the data
 *
 *
 *  Restoring:
 *  ==========
 *
 *   - This is similar to initial startup, but many of the objects being
 *     restored will already exist.
 *
 *   - QEGui requests the persistance manager to restore.
 *
 *   - QEGui recieves a restore signal.
 *
 *   - QEGui asks for its data.
 *
 *   - QEGui uses the persistance manager to parse the XML data it gets acts on
 *     each item it understands. Typically the data will contain GUI names and positions.
 *
 *   - QEGui deletes GUIs that are not required, repositions and resizes GUIS
 *     that already exist and creates GUIs not present.
 *
 *   - For New GUIs the behaviour is identical to initial startup - Each interested
 *     QE widget on creation connects to the restore signal and asks for acts on
 *     restoration data.
 *
 *   - For existing GUI each interested QE widget receives a restore signal and
 *     asks for acts on restoration data.
 */
#include <QDebug>
#include <persistanceManager.h>
#include <QDomDocument>

PersistanceManager::PersistanceManager()
{
    xmlWriter = NULL;
    restoreInProgress = false;
}

// Called after restoring is complete.
//   After this is called newly created QE widgets asking for persistance
//   data will not receive any data
void PersistanceManager::restoreComplete()
{
    restoreInProgress = false;
}


QString PersistanceManager::getItem( const QString name )
{
    return restoreData[name];
}

void PersistanceManager::startElement( const QString name )
{
    qDebug() << "PersistanceManager::setItemStart() 1" << name;

    if( !xmlWriter )
        return;

    qDebug() << "PersistanceManager::setItemStart() 2" << name;

    xmlWriter->writeStartElement( name );

}

void PersistanceManager::textElement( const QString name, const QString data )
{
    qDebug() << "PersistanceManager::textElement() 1" << name << data;

    if( !xmlWriter )
        return;

    qDebug() << "PersistanceManager::textElement() 2" << name << data;

    xmlWriter->writeTextElement( name, data );
}

void PersistanceManager::endElement()
{
    qDebug() << "PersistanceManager::setItemEnd() 1";

    if( !xmlWriter )
        return;

    qDebug() << "PersistanceManager::setItemEnd() 2";

    xmlWriter->writeEndElement();
}

// Get a reference to the object that will supply save and restore signals
QObject* PersistanceManager::getSaveRestoreObject()
{
    return &signal;
}


// Save the current configuration
void PersistanceManager::save( const QString name )
{
    // Create an XML writter to generate all save xml data
    xmlWriter = new QXmlStreamWriter( &xml );
    xmlWriter->setAutoFormatting( true );

    // Start the save document
    xmlWriter->writeStartDocument();

    // Top element
    xmlWriter->writeStartElement( "QEConfiguration" );

    xmlWriter->writeTextElement( "Name", name );

    // Notify any interested objects to contribute their persistant data
    signal.save();

    // Close the top element
    xmlWriter->writeEndElement();

    qDebug() << xml;

    // Destroy XML writter used generate all save xml data
    delete xmlWriter;
    xmlWriter = NULL;


}

// Restore a configuration
void PersistanceManager::restore( const QString name )
{
    // Restore the data
    //!! build hash table (populate restoreData)
    qDebug() << "restoring..." << name;


    // Notify any interested objects to collect their persistant data
    signal.restore();

}






void SaveRestoreSignal::save()
{
    // Ask all interested components to add their persistant data
    //!!! signal must be blocking
    emit saveRestore( SAVE );

}

void SaveRestoreSignal::restore()
{
    // Ask  all interested components to collect their persistant data
    //!!! signal must be blocking
    emit saveRestore( RESTORE );
}
