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
 *    Anthony Owen
 *  Contact details:
 *    anthony.owen@gmail.com
 */

#ifndef QEWIDGET_H
#define QEWIDGET_H

#include <QObject>
#include <QCaObject.h>
#include <VariableNameManager.h>
#include <UserMessage.h>
#include <ContainerProfile.h>
#include <QEToolTip.h>
#include <QEPluginLibrary_global.h>
#include <QEDragDrop.h>
#include <styleManager.h>
#include <contextMenu.h>
#include <standardProperties.h>

/**
  This class is used as a base for all CA aware wigets, such as QELabel, QESpinBox, etc.
  It manages common issues including creating a source of CA data updates, handling error,
  warning and status messages, and setting tool tips based on variable names.

  Note, there is tight integration between the CA aware widget classes, this class, and its
  base classes, especially VariableNameManager and QEToolTip.

  In particular, this class manages QCaObject classes that stream updates to the
  CA aware widget class. But this class, however, doesn't know how to format the data,
  or how the updates will be used.
  To resolve this, this class asks its parent class (such as QELabel) to create the
  QCaObject class in what ever flavour it wants, by calling the virtual function createQcaItem.
  A QELabel, for example, wants string updates so it creates a QEString which is based on a
  QCaObject class and formats all updates as strings.

  The CA aware parent class (such as QELabel) defines a variable by calling VariableNameManager::setVariableName().
  The VariableNamePropertyManager class calls the establishConnection function of the CA aware parent class, such as QELabel
  when it has a new variable name.

  This class uses its base QEToolTip class to format tool tips. that class in turn calls the CA aware parent class
  (such as QELabel) directly to make use of a new tool tip.


  After construction, a CA aware widget is activated (starts updating) by calling it's
  establishConnection() function in one of two ways:

   1) The variable name or variable name substitutions is changed by calling setVariableName
      or setVariableNameSubstitutions respectively. These functions are in the VariableNameManager class.
      The VariableNamePropertyManager calls a virtual function establishConnection() which is implemented by the CA aware widget.
      This is how a CA aware widget is activated in 'designer'. It occurs when 'designer' updates the
      variable name property or variable name substitution property.

   2) When an QEForm widget is created, resulting in a set of CA aware widgets being created by loading a UI file
      contining plugin definitions.
      After loading the plugin widgets, code in the QEForm class calls the activate() function in this class (QEWiget).
      the activate() function calls  establishConnection() in the CA aware widget for each variable. This simulates
      what the VariableNamePropertyManager does as each variable name is entered (see 1, above, for details)

  No matter which way a CA aware widget is activated, the establishConnection() function in the CA aware widget is called
  for each variable. The establishConnection() function asks this QEWidget base class, by calling the createConnection()
  function, to perform the tasks common to all CA aware widgets for establishing a stream of CA data.

  The createConnection() function sets up the widget 'tool tip', then immedietly calls the CA aware widget back asking it to create
  an object based on QCaObject. This object will supply a stream of CA update signals to the CA aware object in a form that
  it needs. For example a QELabel creates a QEString object. The QEString class is based on the QCaObject class and converts
  all update data to a strings which is required for updating a Qt label widget. This class stores the QCaObject based class.

  After the establishConnection() function in the CA aware widget has called createConnection(), the remaining task of the
  establishConnection() function is to connect the signals of the newly created QCaObject based classes to its own slots
  so that data updates can be used. For example, a QELabel connects the 'stringChanged' signal
  fromthe QEString object to its setLabelText slot.
 */

class QEWidget : public VariableNameManager,
                 public QEToolTip,
                 public ContainerProfile,
                 public QEDragDrop,
                 public styleManager,
                 public UserMessage,
                 public contextMenu,
                 public standardProperties
{
public:
    /// Constructor
    QEWidget( QWidget* ownerIn );

    /// Destructor
    virtual ~QEWidget();

    /// Initiate updates.
    /// Called after all configuration is complete.
    void activate();

    /// Get the message source ID.
    /// The message source ID is used as part of the system where QE widgets can emit
    /// a message and have the right QE widget in the right form catch the message.
    /// Refer to the UserMessage class for further details.
    unsigned int getMessageSourceId(){ return getSourceId(); }

    /// Set the message source ID.
    /// The message source ID is used as part of the system where QE widgets can emit
    /// a message and have the right QE widget in the right form catch the message.
    /// Refer to the UserMessage class for further details.
    void setMessageSourceId( unsigned int messageSourceId ){ setSourceId( messageSourceId ); }

    /// Return a reference to one of the qCaObjects used to stream CA updates
    ///
    qcaobject::QCaObject* getQcaItem( unsigned int variableIndex );

    /// Take a menu widgt and add it as the context menu for this widget
    ///
    void setupContextMenu( QWidget* w );

    /// Return a colour to update the widget's look to reflect the current alarm state
    /// Note, the color is determined by the alarmInfo class, but since that class is used in non
    /// gui applications, it can't return a QColor
    QColor getColor( QCaAlarmInfo& alarmInfo, const int saturation );

    /// Perform a single shot read on all variables (Usefull when not subscribing by default)
    ///
    void readNow();

    /// (Control widgets only - such as QELineEdit)
    /// Write the value now. Used when writeOnChange, writeOnEnter, etc are all false
    virtual void writeNow(){ qDebug()<<"default writeNow"; }

    /// Virtual function that may be implimented by users of QEWidget to update variable names and macro substitutions.
    /// A default is provided that is suitible in most cases.
    virtual void setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex ); // Generally don't need to override

    /// Looks for a file in a standard set of locations (and opens the file)
    ///
    QFile* openQEFile( QString name, QFile::OpenModeFlag mode );

    /// Static method that looks for a file in a standard set of locations
    /// Returns a pointer to a QFile which is the caller's responsibility to delete, or NULL if the file was not found.
    static QFile* findQEFile( QString name, ContainerProfile* profile );

    /// Returns the default location to create files.
    /// Use this to create files in a consistant location
    QString defaultFileLocation();

    /// Returns the QE framework that built this instance of the widget.
    /// On windows, the QEFramework DLL may be loaded twice with potentially different versions of it.
    QString getFrameworkVersion();

protected:
    void setNumVariables( unsigned int numVariablesIn );    // Set the number of variables that will stream data updates to the widget. Default of 1 if not called.

    bool subscribe;                 // Flag if data updates should be requested

    qcaobject::QCaObject* createConnection( unsigned int variableIndex );    // Create a CA connection. Return a QCaObject if successfull

    virtual qcaobject::QCaObject* createQcaItem( unsigned int variableIndex ); // Function to create a appropriate superclass of QCaObject to stream data updates
    virtual void establishConnection( unsigned int variableIndex );     // Create a CA connection and initiates updates if required

private:
    void deleteQcaItem( unsigned int variableIndex );       // Delete a stream of CA updates
    unsigned int numVariables;              // The number of process variables that will be managed for the QE widgets.
    qcaobject::QCaObject** qcaItem;          // CA access - provides a stream of updates. One for each variable name used by the QE widgets

    void userLevelChanged( userLevels level );
    void setToolTipFromVariableNames();

public:
    static bool inDesigner();
};

#endif // QEWIDGET_H
