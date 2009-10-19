/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/data/include/QCaWidget.h $
 * $Revision: #5 $
 * $DateTime: 2009/07/31 14:45:31 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class QCaWidget
  \version $Revision: #5 $
  \date $DateTime: 2009/07/31 14:45:31 $
  \author anthony.owen
  \brief Template for Qt-CA aware widgets.
 */

/* Copyright (c) 2009 Australian Synchrotron
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * Licence as published by the Free Software Foundation; either
 * version 2.1 of the Licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public Licence for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * Licence along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Contact details:
 * anthony.owen@synchrotron.org.au
 * 800 Blackburn Road, Clayton, Victoria 3168, Australia.
 *
 */

#ifndef QCAWIDGET_H
#define QCAWIDGET_H

#include <QObject>
#include <QCaObject.h>
#include <VariableNameManager.h>
#include <UserMessage.h>
#include <ContainerProfile.h>
#include <QCaToolTip.h>

class QCaWidget : public VariableNameManager, public QCaToolTip, public ContainerProfile {

public:
    QCaWidget();                            /// Constructor
    ~QCaWidget();                           /// Destructor
    UserMessage userMessage;                /// Manager of messages to the user
    void activate();                        /// Initiate updates.

protected:
    void setNumVariables( unsigned int numVariablesIn );    /// Set the number of variables that will stream data updates to the widget. Default of 1 if not called.

    bool subscribeProperty;                 /// Flag if data updates should be requested
    bool variableAsToolTipProperty;         /// Flag the tool tip should be set to the variable name

    bool createConnection( unsigned int variableIndex );    /// Create a CA connection
    void deleteQcaItem( unsigned int variableIndex );       /// Delete a stream of CA updates

    virtual void setup() = 0;                               ///???
    virtual void createQcaItem( unsigned int variableIndex ) = 0; /// Function to create a appropriate superclass of QCaObject to stream data updates

    void setQcaItem( qcaobject::QCaObject*, unsigned int variableIndex );   /// Save a reference to a recently created QCaObject based object
    qcaobject::QCaObject* getQcaItem( unsigned int variableIndex );         /// Return a reference to one of the qCaObjects used to stream CA updates

  private:
    unsigned int numVariables;              /// The number of process variables that will be managed for the QCa widget.
    qcaobject::QCaObject** qcaItem;          /// CA access - provides a stream of updates. One for each variable name used by the QCa widget

};

#endif // QCAWIDGET_H
