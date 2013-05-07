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
 * Refer to persistanceMAnager.cpp for further details
 */

#ifndef PERSISTANCEMANAGER_H
#define PERSISTANCEMANAGER_H

#include <QObject>
#include <QHash>
#include <QStringList>
#include <QXmlStreamWriter>
#include <QEPluginLibrary_global.h>
#include <QXmlDefaultHandler>
#include <QVariant>
#include <QDomDocument>



class PersistanceManager;
// Class used to generate signals that a save or restore is require.
// A single instance of this class is shared by all instances of
// the persistanceManager class.
// The persistanceManager class can't generate signals directly as it
// is not based on QObject and can't be as it is a base class for
// Widgets that may also be based on Qwidgets and only one base
// class can be based on a QObject.
class SaveRestoreSignal : public QObject
{
    Q_OBJECT

public:

    // Constructor, destructor
    SaveRestoreSignal(){ owner = NULL; }
    ~SaveRestoreSignal(){}
    void setOwner( PersistanceManager* ownerIn ){ owner = ownerIn; }


    void save();      // Save the current configuration
    void restore();   // Restore a configuration




enum saveRestoreOptions { SAVE, RESTORE };
  signals:
    // Internal use only. Send when a save or restore is needed.
    void saveRestore( SaveRestoreSignal::saveRestoreOptions option );   // Saving or restoring

  private:
    PersistanceManager* owner;
};

//// Class to save and return restore data
//class PMTag
//{
//public:
//    PMTag(){ isRoot = false; }
//    PMTag( const QString tagIn, const QString dataIn ){ tag = tagIn; data = dataIn; isRoot = false; }

////!!!private
//    QString tag;
//    QString data;
//    bool isRoot;        // If set, data is nested tag/data and needs to be further disassembled
//};

// Class to maintain a context while parsing XML save/restore data
class PMContext
{
public:
    PMContext(){ context = 0; }
private:
    int context;
};

class PMElement;

class QEPLUGINLIBRARYSHARED_EXPORT PMElementList: private QDomNodeList
{
public:
    PMElementList( PersistanceManager* ownerIn, QDomNodeList elementListIn );
    PMElement getElement( int i );
    int count(){ return ((QDomNodeList*)this)->count(); }

private:
    PersistanceManager* owner;
};

// Class to conceal QDomElement from users of the persistance manager (and make it easier to add stuff)
class QEPLUGINLIBRARYSHARED_EXPORT PMElement : private QDomElement
{
public:
    PMElement( PersistanceManager* ownerIn, QDomElement elementIn );
    PMElement addElement( QString name );                   // Add an element
    void addAttribute( QString name, int value );           // Add an integer attribute
    void addAttribute( QString name, QString value );       // Add a string attribute

    PMElement getElement( QString name );                   // Get a named element
    PMElement getElement( QString name, int i );            // Get one element from a named element list

    PMElementList getElementList( QString name );           // Get a named element list

    bool getElementValue( int& val );                       // Get the integer value for an element
    bool getElementValue( QString& val );                   // Get the string value for an element
    bool getElementAttribute( QString name, int& val );     // Get a named integer attribute from an element
    bool getElementAttribute( QString name, QString& val ); // Get a named string attribute from an element

    bool isNull(){ return QDomElement::isNull(); }          // Indicate if an element is empty

private:
    PersistanceManager* owner;                              // Persistance manager that supplied this PMElement instance
};

// Persistance manager
class QEPLUGINLIBRARYSHARED_EXPORT PersistanceManager : public QXmlDefaultHandler
{
public:

    friend class PMElement;

    PersistanceManager();


    void save( const QString fileName, const QString rootName, const QString configName );            // Save the current configuration
    void restore( const QString fileName, const QString rootName, const QString configName );         // Restore a configuration

    PMElement addElement( QString name );
    QDomElement addDomElement( QString name );

    QObject* getSaveRestoreObject();          // Get a reference to the object that will supply save and restore signals

    PMElement getMyData( QString name );
    QString  getMyDataString( QString name );
    QDomElement getMyDataDom( QString name );


private:
    PMElement addElement( QDomElement parent, QString name );
    void addAttribute( QDomElement element, QString name, int value );
    void addAttribute( QDomElement element, QString name, QString value );




    QDomElement getElement( QDomElement element, QString name );
    QDomElement getElement( QDomElement element, QString name, int i );
    QDomElement getElement( QDomNodeList nodeList, int i );

    QDomNodeList getElementList( QDomElement element, QString name );

    bool getElementValue( QDomElement element, int& val );
    bool getElementValue( QDomElement element, QString& val );
    bool getElementAttribute( QDomElement element, QString name, int& val );
    bool getElementAttribute( QDomElement element, QString name, QString& val );



    SaveRestoreSignal signal;           // Save/Restore signal object. One instance to signal all QE Widgets and applications

//    QDomNodeList itemList;              // List of saved items. Each object saving data will have an item with 'name' and 'data' element


    bool restoreInProgress;             // If true a restore has been started and is not yet complete.

    QDomDocument doc;                   // Save and restore xml document
    QDomElement config;                   // Current configuration

};

#endif // PERSISTANCEMANAGER_H
