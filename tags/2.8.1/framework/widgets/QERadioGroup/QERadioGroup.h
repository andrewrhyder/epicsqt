/*  QERadioGroup.h
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
 *  Copyright (c) 2013
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef QERADIOGROUP_H
#define QERADIOGROUP_H

#include <QGridLayout>
#include <QList>
#include <QString>
#include <QVector>
#include <QAbstractButton>
#include <QSize>

#include <QCaObject.h>
#include <QEWidget.h>
#include <QEInteger.h>
#include <QEIntegerFormatting.h>
#include <QELocalEnumeration.h>
#include <QCaVariableNamePropertyManager.h>
#include <QEPluginLibrary_global.h>
#include <QEGroupBox.h>
#include <QEOneToOne.h>

class QEPLUGINLIBRARYSHARED_EXPORT QERadioGroup : public QEGroupBox {

   Q_OBJECT
   // QERadioGroup specific properties ===============================================
   // Like most Single Variable properties, but not quite because of two substitution properties.
   // These properties should be identical for every widget using a single variable.
   // WHEN MAKING CHANGES: Use the update_widget_properties script in the
   // resources directory.
   //
   // Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
   // A property name ending with 'Name' results in some sort of string a variable being displayed,
   // but will only accept alphanumeric and won't generate callbacks on change.
public:
   /// EPICS variable name (CA PV)
   ///
   Q_PROPERTY(QString variable READ getVariableNameProperty WRITE setVariableNameProperty)

   /// Macro substitutions. The default is no substitutions. The format is NAME1=VALUE1[,] NAME2=VALUE2...
   /// Values may be quoted strings. For example, 'PUMP=PMP3, NAME = "My Pump"'
   /// These substitutions are applied to variable names for all QE widgets. In some widgets are are also used for other purposes.
   Q_PROPERTY(QString variableSubstitutions READ getSubstitutionsProperty WRITE setSubstitutionsProperty)

private:
   void    setVariableNameProperty( QString variableName ){ variableNamePropertyManager.setVariableNameProperty( variableName ); }
   QString getVariableNameProperty(){ return variableNamePropertyManager.getVariableNameProperty(); }

   // Overide QEGroupBox setSubstitutionsProperty, use getSubstitutionsProperty as is.
   void setSubstitutionsProperty (QString macroSubstitutions);

   QCaVariableNamePropertyManager variableNamePropertyManager;

public:
   /// Number of colums - defaults to two.
   ///
   Q_PROPERTY (int columns READ getColumns WRITE setColumns)

   void setColumns (int columns);
   int getColumns ();

   /// Use database enumerations - defaults to true.
   ///
   Q_PROPERTY (bool useDbEnumerations READ getUseDbEnumerations WRITE setUseDbEnumerations)

   void setUseDbEnumerations (bool useDbEnumerations);
   bool getUseDbEnumerations ();

   /// Enumrations values used when useDbEnumerations is false.
   ///
   Q_PROPERTY (QString localEnumeration READ getLocalEnumerations  WRITE setLocalEnumerations)

   void setLocalEnumerations (const QString& localEnumerations);
   QString getLocalEnumerations ();

   /// Enumrations values used to select the button style.
   /// Whereas check box buttons can/do work, this option not provided as check
   /// boxes are not assoicated with the radio button, i.e. one and only one
   /// selected, paradigm.
   //
   enum ButtonStyles { Radio,      ///< Use radio buttons - the default
                       Push };     ///< Use push buttons.

   Q_ENUMS (ButtonStyles)
   Q_PROPERTY (ButtonStyles buttonStyle READ getButtonStyle  WRITE setButtonStyle)

   void setButtonStyle (const ButtonStyles & buttonStyle);
   ButtonStyles getButtonStyle ();
   //
   // End of QERadioGroup specific properties =========================================

public:
   /// Create without a variable.
   /// Use setVariableNameProperty() and setSubstitutionsProperty() to define a
   /// variable and, optionally, macro substitutions later.
   ///
   explicit QERadioGroup (QWidget* parent = 0);

   /// Create with a variable.
   /// A connection is automatically established.
   /// If macro substitutions are required, create without a variable and set the
   /// variable and macro substitutions after creation.
   ///
   explicit QERadioGroup (const QString& variableName, QWidget* parent = 0);

   /// Create with a group title and a variable.
   /// A connection is automatically established.
   /// If macro substitutions are required, create without a variable and set the
   /// variable and macro substitutions after creation.
   ///
   explicit QERadioGroup (const QString& title, const QString& variableName, QWidget* parent = 0);

   /// Destruction
   virtual ~QERadioGroup(){}

   int getCurrentIndex ();
protected:
   QSize sizeHint () const;

   // override QEWidget fnctions.
   //
   void establishConnection (unsigned int variableIndex);
   qcaobject::QCaObject* createQcaItem (unsigned int variableIndex);

   void setCurrentIndex (int index);

private:
   typedef QList<QAbstractButton*> QAbstractButtonList;
   typedef QEOneToOne<int, QAbstractButton*> ValueButtonAssoications;

   QEIntegerFormatting integerFormatting;
   QELocalEnumeration localEnumerations;

   // Use of the local enumerations means that we could have sparce mapping,
   // e.g.: 1 => Red, 5 => Blue, 63 => Green.  Therefore we need to create
   // and maintain a value to button mapping.
   //
   ValueButtonAssoications valueToButton;
   QGridLayout* buttonLayout;
   QAbstractButtonList buttonList;
   QAbstractButton *noSelectionButton;

   bool useDbEnumerations;
   int currentIndex;
   int number;    // number of displayed buttons.
   int rows;
   int cols;
   ButtonStyles buttonStyle;
   bool isConnected;
   bool isFirstUpdate;

   QAbstractButton* createButton (QWidget* parent);
   void reCreateAllButtons ();
   void commonSetup ();
   void setButtonText ();
   void setButtonLayout ();

private slots:
   void connectionChanged (QCaConnectionInfo& connectionInfo);
   void valueUpdate (const long& value, QCaAlarmInfo&, QCaDateTime&, const unsigned int&);

   void useNewVariableNameProperty (QString variableNameIn,
                                    QString variableNameSubstitutionsIn,
                                    unsigned int variableIndex );

   void buttonClicked (bool checked);

signals:
   // Note, the following signals are common to many QE widgets,
   // if changing the doxygen comments, ensure relevent changes are migrated to all instances
   /// Sent when the widget is updated following a data change
   /// Can be used to pass on EPICS data (as presented in this widget) to other widgets.
   /// For example a QList widget could log updates from this widget.
   /// Note: this widget emits the numeric enumeration value as opposed to the associated text.
   void dbValueChanged (const long& out);

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
   void paste( QVariant s );
};

#endif // QERADIOGROUP_H
