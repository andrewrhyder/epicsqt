/* QEPvProperties.h
 *
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
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

/* The QEPvProperties class allows user to view all the displayalbe fields
     of the associated IOC record.
*/

#ifndef QEPVPROPERTIES_H
#define QEPVPROPERTIES_H

#include <QAction>
#include <QList>
#include <QString>
#include <QTableWidget>
#include <QWidget>
#include <QPoint>

#include <persistanceManager.h>
#include <QCaAlarmInfo.h>
#include <QEDragDrop.h>
#include <QCaObject.h>
#include <QEPluginLibrary_global.h>
#include <QEFrame.h>
#include <QEString.h>
#include <QEStringFormatting.h>
#include <QCaVariableNamePropertyManager.h>
#include <QEWidget.h>

class QEPLUGINLIBRARYSHARED_EXPORT QEPvProperties : public QEFrame {
Q_OBJECT

    // BEGIN-SINGLE-VARIABLE-PROPERTIES ===============================================
    // Single Variable properties
    // These properties should be identical for every widget using a single variable.
    // WHEN MAKING CHANGES: Use the update_widget_properties script in the
    // resources directory.
    //
    // Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
    // A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
public:
    /// EPICS variable name (CA PV)
    ///
    Q_PROPERTY(QString variable READ getVariableNameProperty WRITE setVariableNameProperty)
    /// Macro substitutions. The default is no substitutions. The format is NAME1=VALUE1[,] NAME2=VALUE2... Values may be quoted strings. For example, 'PUMP=PMP3, NAME = "My Pump"'
    /// These substitutions are applied to variable names for all QE widgets. In some widgets are are also used for other purposes.
    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)

private:
    void    setVariableNameProperty( QString variableName ){ variableNamePropertyManager.setVariableNameProperty( variableName ); }
    QString getVariableNameProperty(){ return variableNamePropertyManager.getVariableNameProperty(); }

    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions ){ variableNamePropertyManager.setSubstitutionsProperty( variableNameSubstitutions ); }
    QString getVariableNameSubstitutionsProperty(){ return variableNamePropertyManager.getSubstitutionsProperty(); }

    QCaVariableNamePropertyManager variableNamePropertyManager;
public:
    // END-SINGLE-VARIABLE-PROPERTIES =================================================


private:
   QEStringFormatting stringFormatting;
   bool isFirstUpdate;

   // If widgets are declared at class level, there is a run time exception,
   // so we place then in separate and private class.
   //
   class OwnWidgets;
   OwnWidgets *ownWidgets;

   QString recordBaseName;
   QEStringFormatting fieldStringFormatting;

   QEString* standardRecordType;
   QEString* alternateRecordType;

   QList<QEString *> fieldChannels;

   int m, d;   // scaling.

   // common constructor function.
   void common_setup ();
   void clearFieldChannels ();

   void setUpLabelChannel ();
   void setUpRecordTypeChannels (QEString* &qca, const bool useCharArray);

   // Override standardProperties::setApplicationEnabled()
   void setApplicationEnabled (const bool & state);

   // Set pvName.
   //
   void setPvName (const QString& pvName);

private slots:
   void useNewVariableNameProperty (QString variableNameIn,
                                    QString variableNameSubstitutionsIn,
                                    unsigned int variableIndex);

   // Basic widgit PV related slots (used for RTYP pseudo field).
   //
   void setRecordTypeConnection (QCaConnectionInfo& connectionInfo, const unsigned int &variableIndex);
   void setRecordTypeValue (const QString & rtypeValue, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );

   // The value item slots.
   //
   void setValueConnection (QCaConnectionInfo& connectionInfo);
   void setValueValue (const QString & rtypeValue, QCaAlarmInfo&, QCaDateTime&, const unsigned int&);

   // Field related slots
   //
   void setFieldConnection (QCaConnectionInfo& connectionInfo,
                            const unsigned int &variableIndex);

   void setFieldValue (const QString &value,
                       QCaAlarmInfo & alarmInfo,
                       QCaDateTime & dateTime,
                       const unsigned int & variableIndex);

   // from own combo box.
   //
   void boxCurrentIndexChanged (int index);

   // For the table.
   //
   void customContextMenuRequested (const QPoint & pos);
   void contextMenuTriggered (QAction* selectedItem);

signals:
   void setCurrentBoxIndex (int index);

protected:
   void resizeEvent ( QResizeEvent * event );
   void establishConnection (unsigned int variableIndex);
   void scaleBy (const int m, const int d);

   // Override QCaObject/QEWidget functions.
   //
   qcaobject::QCaObject* createQcaItem (unsigned int variableIndex);

   // Drop only. Dragging is from individual embedded QEWidgets.
   //
   // Override QEDragDrop functions.
   //
   void dragEnterEvent (QDragEnterEvent *event) { qcaDragEnterEvent (event); }
   void dropEvent (QDropEvent *event)           { qcaDropEvent(event); }
   // void mousePressEvent (QMouseEvent *event)    { qcaMousePressEvent (event); }
   void setDrop (QVariant drop);

   QVariant getDrop ();

   void saveConfiguration (PersistanceManager* pm);
   void restoreConfiguration (PersistanceManager* pm, restorePhases restorePhase);

   // Copy paste
   //
   QString copyVariable();
   QVariant copyData();
   void paste (QVariant s);
   bool canPaste(){ return getAllowDrop(); }

public:
   // Constructors
   //
   QEPvProperties (QWidget * parent = 0);
   QEPvProperties (const QString & variableName, QWidget * parent = 0);
   ~QEPvProperties ();

   QSize sizeHint () const;
};

# endif  // QEPVPROPERTIES_H
