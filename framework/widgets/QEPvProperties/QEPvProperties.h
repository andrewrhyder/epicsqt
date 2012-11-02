/* $Id: QEPvProperties.h$
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

/*  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
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

#ifndef QEPVPROPERTIES_H
#define QEPVPROPERTIES_H

#include <QComboBox>
#include <QFrame>
#include <QLabel>
#include <QList>
#include <QString>
#include <QTableWidget>
#include <QWidget>

#include <QCaAlarmInfo.h>
#include <QCaDragDrop.h>
#include <QCaObject.h>
#include <QCaPluginLibrary_global.h>
#include <QCaString.h>
#include <QCaStringFormatting.h>
#include <QCaVariableNamePropertyManager.h>
#include <QCaWidget.h>


class QCAPLUGINLIBRARYSHARED_EXPORT QEPvProperties : public QFrame,
                                                     public QCaWidget {
   Q_OBJECT

    //=================================================================================
    // Single Variable properties
    // These properties should be identical for every widget using a single variable.
    // WHEN MAKING CHANGES: search for SINGLEVARIABLEPROPERTIES and change all occurances.
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
    //=================================================================================

    //=================================================================================
    // Standard properties
    // These properties should be identical for every widget using them.
    // WHEN MAKING CHANGES: search for STANDARDPROPERTIES and change all occurances.
    bool isEnabled() const { return getApplicationEnabled(); }
    void setEnabled( bool state ){ setApplicationEnabled( state ); }
    Q_PROPERTY(bool variableAsToolTip READ getVariableAsToolTip WRITE setVariableAsToolTip)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
    Q_PROPERTY(bool allowDrop READ getAllowDrop WRITE setAllowDrop)
    Q_PROPERTY(bool visible READ getRunVisible WRITE setRunVisible)
    Q_PROPERTY(unsigned int messageSourceId READ getMessageSourceId WRITE setMessageSourceId )
    Q_PROPERTY(QString userLevelUserStyle READ getStyleUser WRITE setStyleUser)
    Q_PROPERTY(QString userLevelScientistStyle READ getStyleScientist WRITE setStyleScientist)
    Q_PROPERTY(QString userLevelEngineerStyle READ getStyleEngineer WRITE setStyleEngineer)
    enum UserLevels { User      = USERLEVEL_USER,
                      Scientist = USERLEVEL_SCIENTIST,
                      Engineer  = USERLEVEL_ENGINEER };
    UserLevels getUserLevelVisibilityProperty() { return (UserLevels)getUserLevelVisibility(); }
    void setUserLevelVisibilityProperty( UserLevels level ) { setUserLevelVisibility( (userLevels)level ); }

    UserLevels getUserLevelEnabledProperty() { return (UserLevels)getUserLevelEnabled(); }
    void setUserLevelEnabledProperty( UserLevels level ) { setUserLevelEnabled( (userLevels)level ); }
    Q_ENUMS(UserLevels)
    Q_PROPERTY(UserLevels userLevelVisibility READ getUserLevelVisibilityProperty WRITE setUserLevelVisibilityProperty)
    Q_PROPERTY(UserLevels userLevelEnabled READ getUserLevelEnabledProperty WRITE setUserLevelEnabledProperty)
    //=================================================================================

private:
   QCAALARMINFO_SEVERITY lastSeverity;
   QCaStringFormatting stringFormatting;
   bool isFirstUpdate;
   int valFieldIndex;

   // If these items declared at class level, there is a run time exception.
   //
   struct WidgetHolder {
     QVBoxLayout *layout;
     QComboBox *box;
     QLabel *timeStamp;
     QTableWidget *table;
   };
   struct WidgetHolder ownWidgets;

   QString recordBaseName;
   QCaStringFormatting fieldStringFormatting;
   QList<QCaString *> fieldChannels;

   // common constructor function.
   void common_setup ();
   void clearFieldChannels ();

private slots:
   void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )
   {
       setVariableNameAndSubstitutions(variableNameIn, variableNameSubstitutionsIn, variableIndex);
   }

   // Basic PV relates slots (used for RTYP pseudo field).
   //
   void setRecordTypeConnection (QCaConnectionInfo& connectionInfo);
   void setRecordTypeValue (const QString & rtypeValue, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );

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

signals:
   void setCurrentBoxIndex (int index);

protected:
   // Override QCaObject/QCaWidget functions.
   //
   void setup();
   qcaobject::QCaObject* createQcaItem (unsigned int variableIndex);

   // Drag and Drop
   //
   // Override QWidget functions - call up standard handlers.
   //
   void dragEnterEvent (QDragEnterEvent *event) { qcaDragEnterEvent (event); }
   void dropEvent (QDropEvent *event)           { qcaDropEvent(event); }
   void mousePressEvent (QMouseEvent *event)    { qcaMousePressEvent (event); }

   // Copy paste
   QString copyVariable();
   QVariant copyData();
   void paste( QVariant s );


   // Override QCaDragDrop functions.
   //
   void setDrop (QVariant drop);
   QVariant getDrop ();

public:
   // Constructors
   //
   QEPvProperties (QWidget * parent = 0);
   QEPvProperties (const QString & variableName, QWidget * parent = 0);
   ~QEPvProperties ();

   QSize sizeHint () const;

   void establishConnection (unsigned int variableIndex);
   void updateToolTip (const QString& tip);

   // "Property" access functions.
   //

public slots:
   void requestEnabled (const bool & state);
};

# endif  // QEPVPROPERTIES_H
