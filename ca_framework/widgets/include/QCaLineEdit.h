/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/widgets/include/QCaLineEdit.h $
 * $Revision: #6 $
 * $DateTime: 2009/07/31 15:55:17 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class QCaLineEdit
  \version $Revision: #6 $
  \date $DateTime: 2009/07/31 15:55:17 $
  \author andrew.rhyder
  \brief CA Line Edit Widget.
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
 * andrew.rhyder@synchrotron.org.au
 * 800 Blackburn Road, Clayton, Victoria 3168, Australia.
 *
 */

#ifndef QCALINEEDIT_H
#define QCALINEEDIT_H

#include <QLineEdit>
#include <QCaWidget.h>
#include <QCaString.h>
#include <QCaStringFormatting.h>

class QCaLineEdit : public QLineEdit, public QCaWidget {
    Q_OBJECT

  public:
    QCaLineEdit( QWidget *parent = 0 );
    QCaLineEdit( const QString &variableName = "", QWidget *parent = 0 );

    bool isEnabled() const;

  protected:
    QCaStringFormatting stringFormatting;   /// String formatting options
    bool writeOnLoseFocusProperty;          /// Write changed value to database when widget object loses focus (user moves from widget)
    bool writeOnEnterProperty;              /// Write changed value to database when enter is pressed with focus on the widget
    bool writeOnChangeProperty;             /// Write changed value to database when ever the text changes
    bool enabledProperty;

    void establishConnection( unsigned int variableIndex );

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setTextIfNoFocus( const QString& value, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );  /// Update the text in the widget as long as the user is not entering data in it
    void userReturnPressed();                       /// Act on the user pressing return in the widget
    void userEditingFinished();                     /// Act on the user signaling text editing is complete (pressing return)
    void userTextEdited ( const QString &text );    /// Act on the user changing the text

  public slots:
    void setEnabled( bool state );

  private:
    void setup();
    void createQcaItem( unsigned int variableIndex );
    void updateToolTip ( const QString & toolTip );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;
};

#endif /// QCALINEEDIT_H
