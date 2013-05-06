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
 * This class provides a mechanism to manage application persistance.
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
 * The PMElement class is used by users of the PersistanceManager class to return XML components from the XML data.
 * The PMElement class simply hides QDom functionality.
 *
 *
 * Typical scenarios:
 *
 *  Initial startup or restoring:
 *  =============================
 *
 *   - QEGui starts, connects to the persistance manager's save and restore signals.
 *
 *   - If specified in the startup parameters, or if the user requests a restore,
 *     QEGui asks the persistance manager to restore.
 *     (In the case of a user request, QEGui closes existing windows returning to a
 *     state very similar to initial startup)
 *
 *   - QEGui gets a signal to restore. (Yes, it know it's restoring - it just asked!)
 *
 *   - QEGui asks for its data.
 *
 *   - QGui uses the persistance manager to help parse the XML restore data it gets.
 *     QEGui then and acts on the recovered data. Typically the data will contain Main Window
 *     and GUI information such as positions, sizes, filenames.
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
 *  Saving: (instigated by the user or optionaly when QEGui exits)
 *  =======
 *
 *   - QEGui asks the persistance manager to save, providing a name to tag the
 *     saved data with.
 *
 *   - The persistance manager sends a 'save' signal.
 *
 *   - All objects connected to the signal (QEGui itself and any interested QE
 *     widgets) use the persistance manager services to build XML data and
 *     then give that data to the persistance manager to save.
 *
 *   - The persistance manager saves all data presented to it.
 */

#include <QDebug>
#include <persistanceManager.h>
#include <QFile>
#include <QByteArray>
#include <QBuffer>

// Construction
PersistanceManager::PersistanceManager()
{
    // Initialise
    restoreInProgress = false;
    doc = QDomDocument( "QEConfig" );
}

// Get a named set of configuration data.
PMElement PersistanceManager::getMyData( QString name )
{
    // ???
    if( restoreInProgress )
        return PMElement( this, config.namedItem( name ).toElement() );
    else
        return PMElement( this, QDomElement() );
}

// Get a node list by name
QDomNodeList PersistanceManager::getElementList( QDomElement element, QString name )
{
    if( element.isNull() || !element.isElement() )
        return QDomNodeList();

    return element.elementsByTagName( name );
}

QDomElement PersistanceManager::getElement( QDomElement element, QString name )
{
    if( element.isNull() || !element.isElement() )
        return QDomElement();

    return element.elementsByTagName( name ).at(0).toElement();
}

QDomElement PersistanceManager::getElement( QDomNodeList nodeList, int i )
{
    if( nodeList.isEmpty() || nodeList.count() <= i )
        return QDomElement();

    return nodeList.at( i ).toElement();
}

QDomElement PersistanceManager::getElement( QDomElement element, QString name, int i )
{
    if( element.isNull() || !element.isElement() )
        return QDomElement();

    return element.elementsByTagName( name ).at(i).toElement();
}

bool PersistanceManager::getElementValue( QDomElement element, int& val )
{
    if( element.isNull() || !element.isElement() )
    {
        val = 0;
        return false;
    }

    val = element.nodeValue().toInt();
    return true;
}

bool PersistanceManager::getElementValue( QDomElement element, QString& val )
{
    if( element.isNull() || !element.isElement() )
    {
        val = QString();
        return false;
    }

    val = element.nodeValue();
    return true;
}

bool PersistanceManager::getElementAttribute( QDomElement element, QString name, int& val )
{
    if( element.isNull() || !element.isElement() )
    {
        val = 0;
        return false;
    }

    QDomNode node = element.attributes().namedItem( name );
    if( node.isNull() )
    {
        val = 0;
        return false;
    }

    val = node.nodeValue().toInt();
    return true;
}

bool PersistanceManager::getElementAttribute( QDomElement element, QString name, QString& val )
{
    if( element.isNull() || !element.isElement() )
    {
        val = QString();
        return false;
    }

    QDomNode node = element.attributes().namedItem( name );
    if( node.isNull() )
    {
        val = QString();
        return false;
    }

    val = node.nodeValue();
    return true;
}


PMElement PersistanceManager::addElement( QString name )
{
    QDomElement element = doc.createElement( name );
    config.appendChild( element );
    return PMElement( this, element );
}

PMElement PersistanceManager::addElement( QDomElement parent, QString name )
{
    QDomElement element = doc.createElement( name );
    parent.appendChild( element );
    return PMElement( this, element );
}

void PersistanceManager::addAttribute( QDomElement element, QString name, int value )
{
    element.setAttribute( name, value );
}

void PersistanceManager::addAttribute( QDomElement element, QString name, QString value )
{
    element.setAttribute( name, value );
}

// Get a reference to the object that will supply save and restore signals
QObject* PersistanceManager::getSaveRestoreObject()
{
    return &signal;
}


// Save the current configuration
void PersistanceManager::save( const QString fileName, const QString rootName, const QString configName )
{ 
    doc.clear();


    QDomElement root = doc.createElement( rootName );
    doc.appendChild( root );

    config = doc.createElement( configName );
    root.appendChild( config );

    // Notify any interested objects to contribute their persistant data
    signal.save();

    QFile file( fileName );
    if ( file.open( QIODevice::WriteOnly ) )
    {
        QTextStream ts( &file );
        ts << doc.toString() ;
        file.close();
    }
    else
    {
        qDebug() << "Could not save configuration";
    }
}

// Restore a configuration
void PersistanceManager::restore( const QString fileName, const QString rootName, const QString configName  )
{
    QFile file( fileName );
    if (!file.open(QIODevice::ReadOnly))
             return;

    if ( !doc.setContent( &file ) )
    {
        file.close();
        return;
    }
    file.close();

    QDomElement docElem = doc.documentElement();

    if( docElem.nodeName().compare( rootName ) )
    {
        qDebug() << "Expected configuration root element (" << rootName << ") not found in config file (" << fileName << ")";
        return;
    }

    config = docElem.namedItem( configName ).toElement();

    // Notify any interested objects to collect their persistant data
    restoreInProgress = true;
    signal.restore();
    restoreInProgress = false;

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

//=================================================================================================
// PMElement class methods

PMElement::PMElement( PersistanceManager* ownerIn, QDomElement elementIn ) : QDomElement( elementIn )
{
    owner = ownerIn;
}

PMElement PMElement::addElement( QString name )
{
    return owner->addElement( *this, name );
}

void PMElement::addAttribute( QString name, int value )
{
    owner->addAttribute( *this, name, value );
}
void PMElement::addAttribute( QString name, QString value )
{
    owner->addAttribute( *this, name, value );
}

bool PMElement::getElementValue( int& val )
{
    return owner->getElementValue( *this, val );
}

bool PMElement::getElementValue( QString& val )
{
    return owner->getElementValue( *this, val );
}

bool PMElement::getElementAttribute( QString name, int& val )
{
    return owner->getElementAttribute( *this, name, val );
}

bool PMElement::getElementAttribute( QString name, QString& val )
{
    return owner->getElementAttribute( *this, name, val );
}

PMElement PMElement::getElement( QString name )
{
    return PMElement( owner, owner->getElement( *this, name ) );
}

PMElement PMElement::getElement( QString name, int i )
{
    return PMElement( owner, owner->getElement( *this, name, i ) );
}

PMElementList PMElement::getElementList( QString name )
{
    return PMElementList( owner, owner->getElementList( *this, name ) );
}

PMElementList::PMElementList( PersistanceManager* ownerIn, QDomNodeList elementListIn ) : QDomNodeList( elementListIn )
{
    owner = ownerIn;
}

PMElement PMElementList::getElement( int i )
{
    //!!! check range of i
    return PMElement( owner, this->at( i ).toElement() );
}

