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
 *  Copyright (c) 2011
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef QEBITSTATUS_H
#define QEBITSTATUS_H

#include <QString>
#include <QBitStatus.h>
#include <QCaWidget.h>
#include <QCaInteger.h>
#include <QCaIntegerFormatting.h>
#include <QCaVariableNamePropertyManager.h>
#include <QCaPluginLibrary_global.h>

class QCAPLUGINLIBRARYSHARED_EXPORT QEBitStatus : public QBitStatus, public QCaWidget {
   Q_OBJECT

/// #ifdef PLUGIN_APP

public:
    //=================================================================================
    // Single Variable properties
    // These properties should be identical for every widget using a single variable.
    // WHEN MAKING CHANGES: search for SINGLEVARIABLEPROPERTIES and change all occurances.
    //
    // Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
    // A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
    Q_PROPERTY(QString variable READ getVariableNameProperty WRITE setVariableNameProperty);
    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)

    void    setVariableNameProperty( QString variableName ){ variableNamePropertyManager.setVariableNameProperty( variableName ); }
    QString getVariableNameProperty(){ return variableNamePropertyManager.getVariableNameProperty(); }

    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions ){ variableNamePropertyManager.setSubstitutionsProperty( variableNameSubstitutions ); }
    QString getVariableNameSubstitutionsProperty(){ return variableNamePropertyManager.getSubstitutionsProperty(); }

private:
    QCaVariableNamePropertyManager variableNamePropertyManager;
public:
    //=================================================================================

    //=================================================================================
    // Standard properties
    // These properties should be identical for every widget using them.
    // WHEN MAKING CHANGES: search for STANDARDPROPERTIES and change all occurances.
    // Override the default widget isEnabled and setEnabled to allow alarm states to override current enabled state
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
    Q_ENUMS(UserLevels)
    Q_PROPERTY(UserLevels userLevelVisibility READ getUserLevelVisibilityProperty WRITE setUserLevelVisibilityProperty)
    Q_PROPERTY(UserLevels userLevelEnabled READ getUserLevelEnabledProperty WRITE setUserLevelEnabledProperty)
    //=================================================================================

/// #endif

public:
   QEBitStatus (QWidget * parent = 0);
   QEBitStatus (const QString & variableName, QWidget * parent = 0);

   // Variable Name and substitution
   //
   void setVariableNameAndSubstitutions (QString variableNameIn,
                                         QString variableNameSubstitutionsIn,
                                         unsigned int variableIndex);

public slots:
   void requestEnabled( const bool& state ){ setApplicationEnabled( state ); }  //!! move into Standard Properties section??


protected:
   QCaIntegerFormatting integerFormatting;

   void establishConnection (unsigned int variableIndex);

private:
   void setup ();

   qcaobject::QCaObject * createQcaItem (unsigned int variableIndex);
   void updateToolTip (const QString & tip);

   QCAALARMINFO_SEVERITY lastSeverity;
   bool isConnected;

private slots:
   void connectionChanged (QCaConnectionInfo &
                           connectionInfo);

   void setBitStatusValue (const long &value, QCaAlarmInfo &,
                           QCaDateTime &, const unsigned int &);

   void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex ) //!! move into Standard Properties section??
   {
       setVariableNameAndSubstitutions(variableNameIn, variableNameSubstitutionsIn, variableIndex);
   }

signals:
   void dbValueChanged (const long &out);

protected:
   // Drag and Drop
   void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
   void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
   void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
   void setDrop( QVariant drop );
   QVariant getDrop();

   // Copy paste
   QString copyVariable();
   QVariant copyData();

};

#endif                          /// QEBITSTATUS_H
