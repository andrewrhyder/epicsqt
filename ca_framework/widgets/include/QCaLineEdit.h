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
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef QCALINEEDIT_H
#define QCALINEEDIT_H

#include <QLineEdit>
#include <QCaWidget.h>
#include <QCaString.h>
#include <QCaStringFormatting.h>
#include <QCaPluginLibrary_global.h>
#include <QCaStringFormattingMethods.h>

class QCAPLUGINLIBRARYSHARED_EXPORT QCaLineEdit : public QLineEdit, public QCaWidget, public QCaStringFormattingMethods {
    Q_OBJECT

  public:
    QCaLineEdit( QWidget *parent = 0 );
    QCaLineEdit( const QString &variableName, QWidget *parent = 0 );

    bool isEnabled() const;
    void setEnabled( bool state );

    // Property convenience functions

    // Variable Name and substitution
    void setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );

    // write on lose focus
    void setWriteOnLoseFocus( bool writeOnLoseFocus );
    bool getWriteOnLoseFocus();

    // write on enter
    void setWriteOnEnter( bool writeOnEnter );
    bool getWriteOnEnter();

    // subscribe
    void setSubscribe( bool subscribe );
    bool getSubscribe();

    // variable as tool tip
    void setVariableAsToolTip( bool variableAsToolTip );
    bool getVariableAsToolTip();

    // Allow user to drop new PVs into this widget
    void setAllowDrop( bool allowDropIn );
    bool getAllowDrop();

    // confirm write
    void setConfirmWrite( bool confirmWrite );
    bool getConfirmWrite();

protected:
    bool writeOnLoseFocus;                  /// Write changed value to database when widget object loses focus (user moves from widget)
    bool writeOnEnter;                      /// Write changed value to database when enter is pressed with focus on the widget
    bool localEnabled;                      /// Override the default widget setEnabled to allow alarm states to override current enabled state
    bool confirmWrite;                      /// Request confirmation before writing a value
    bool allowDrop;

    void establishConnection( unsigned int variableIndex );

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setTextIfNoFocus( const QString& value, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );  /// Update the text in the widget as long as the user is not entering data in it
    void userReturnPressed();                       /// Act on the user pressing return in the widget
    void userEditingFinished();                     /// Act on the user signaling text editing is complete (pressing return)

  public slots:
    void requestEnabled( const bool& state );

  signals:
    void dbValueChanged( const QString& out );
    void requestResend();

  private:
    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );
    void updateToolTip ( const QString & toolTip );
    void writeValue( QCaString *qca, QString newValue );
    QString lastValue;                      /// Last updated value (may have arrived while user is editing field)

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

    void stringFormattingChange(){ requestResend(); }

    // Drag and Drop
protected:
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    // Don't drag from interactive widget void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDropText( QString text );
    QString getDropText();
};

#endif /// QCALINEEDIT_H
