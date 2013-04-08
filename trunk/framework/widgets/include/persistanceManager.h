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

// Persistance manager
class PersistanceManager
{
public:
    PersistanceManager();


    void save( const QString name );            // Save the current configuration
    void restore( const QString name );         // Restore a configuration
    void restoreComplete();                     // Called after restoring is complete.
                                                //   After this is called newly created QE widgets asking for persistance
                                                //   data will not receive any data

    QString getItem( const QString name );
    void startElement( const QString name );
    void textElement( const QString name, const QString itemData );
    void endElement();

    QObject* getSaveRestoreObject();          // Get a reference to the object that will supply save and restore signals

//    PMTag getNextTag( const QString data, PMContext& context );
//    void setNextTag( QString& XML, const QString tag, const QString data );



private:
    SaveRestoreSignal signal;           // Save/Restore signal object. One instance to signal all QE Widgets and applications

    QHash<QString, QString> restoreData;// Restore data
//    QList<PMTag> saveData;              // Save data

    QString xml;    // Save data

    QXmlStreamWriter* xmlWriter;



    bool restoreInProgress;             // If true a restore has been started and is not yet complete.

};

#endif // PERSISTANCEMANAGER_H
