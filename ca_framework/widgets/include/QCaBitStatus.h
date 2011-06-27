/* $File: //ASP/Personal/starritt/qt/QCaBitStatus.h $
 * $Revision: #1 $
 * $DateTime: 2011/06/11 21:36:00 $
 * Last checked in by: $Author: starritt $
*/

/*!
  \class QCaBitStatus
  \version $Revision: #1 $
  \date $DateTime: 2011/06/11 21:36:00 $
  \author andrew.starritt
  \brief CA Bit Status Widget.
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
 *  Copyright (c) 2011
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef QCABITSTATUS_H
#define QCABITSTATUS_H

#include <QString>
#include <QBitStatus.h>
#include <QCaWidget.h>
#include <QCaInteger.h>
#include <QCaIntegerFormatting.h>
#include <QCaVariableNamePropertyManager.h>
#include <QCaPluginLibrary_global.h>

class QCAPLUGINLIBRARYSHARED_EXPORT QCaBitStatus : public QBitStatus, public QCaWidget {
   Q_OBJECT

/// #ifdef PLUGIN_APP

   // Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
   // A property name ending with 'Name' results in some sort of string a variable being displayed,
   // but will only accept alphanumeric and won't generate callbacks on change.
   //
   Q_PROPERTY (QString variable
               READ  getVariableNameProperty
               WRITE setVariableNameProperty)

   Q_PROPERTY (QString variableSubstitutions
               READ  getVariableNameSubstitutionsProperty
               WRITE setVariableNameSubstitutionsProperty)

   Q_PROPERTY (bool variableAsToolTip
               READ  getVariableAsToolTip
               WRITE setVariableAsToolTip)

   Q_PROPERTY (bool enabled READ isEnabled WRITE setEnabled)

   // Display properties
   //
   Q_PROPERTY (bool visible READ getRunVisible WRITE setRunVisible)

/// #endif

public:
   QCaBitStatus (QWidget * parent = 0);
   QCaBitStatus (const QString & variableName, QWidget * parent = 0);

   bool isEnabled () const;
   void setEnabled (bool state);

   // Property functions
   //
   void setVariableNameProperty (QString variableName);
   QString getVariableNameProperty ();

   void setVariableNameSubstitutionsProperty (QString variableNameSubstitutions);
   QString getVariableNameSubstitutionsProperty ();

   // Variable Name and substitution
   //
   void setVariableNameAndSubstitutions (QString variableNameIn,
                                         QString variableNameSubstitutionsIn,
                                         unsigned int variableIndex);

   // variable as tool tip
   //
   void setVariableAsToolTip (bool variableAsToolTip);
   bool getVariableAsToolTip ();

   // Display properties
   // visible (widget is visible outside 'Designer')
   //
   void setRunVisible (bool visibleIn);
   bool getRunVisible ();

public slots:
   void requestEnabled (const bool & state);


protected:
   QCaIntegerFormatting integerFormatting;
   bool localEnabled;
   bool visible;      // Flag true if the widget should be visible outside 'Designer'

   void establishConnection (unsigned int variableIndex);

private:
   void setup ();

   qcaobject::QCaObject * createQcaItem (unsigned int variableIndex);
   void updateToolTip (const QString & tip);

   QCaVariableNamePropertyManager variableNamePropertyManager;

   QCAALARMINFO_SEVERITY lastSeverity;
   bool isConnected;

private slots:
   void connectionChanged (QCaConnectionInfo &
                           connectionInfo);

   void setBitStatusValue (const long &value, QCaAlarmInfo &,
                           QCaDateTime &, const unsigned int &);

   void useNewVariableNameProperty (QString variableNameIn,
                                    QString variableNameSubstitutionsIn,
                                    unsigned int variableIndex);

signals:
   void dbValueChanged (const long &out);
};

#endif                          /// QCABITSTATUS_H
