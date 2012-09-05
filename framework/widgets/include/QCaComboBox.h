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

#ifndef QCAComboBOX_H
#define QCAComboBOX_H

#include <QComboBox>
#include <QCaWidget.h>
#include <QCaInteger.h>
#include <QCaIntegerFormatting.h>
#include <QCaConnectionInfo.h>
#include <QCaPluginLibrary_global.h>

class QCAPLUGINLIBRARYSHARED_EXPORT QCaComboBox : public QComboBox, public QCaWidget {
    Q_OBJECT

  public:
    QCaComboBox( QWidget *parent = 0 );
    QCaComboBox( const QString& variableName, QWidget *parent = 0 );

    // write on change
    void setWriteOnChange( bool writeOnChangeIn );
    bool getWriteOnChange();

    // subscribe
    void setSubscribe( bool subscribe );
    bool getSubscribe();

    // use database enumerations
    void setUseDbEnumerations( bool useDbEnumerations );
    bool getUseDbEnumerations();


  protected:
    QCaIntegerFormatting integerFormatting;
    bool useDbEnumerations;
    bool writeOnChange;                     // Write changed value to database when user changes a value

    void establishConnection( unsigned int variableIndex );

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setValueIfNoFocus( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& );
    void userValueChanged( int value );

  public slots:
    void requestEnabled( const bool& state ){ setApplicationEnabled( state ); } //!! move into Standard Properties section??
    void writeNow();

signals:
    void dbValueChanged( const qlonglong& out );
    void userChange( const QString& oldValue, const QString& newValue, const QString& lastValue );    // Signal a user attempt to change a value. Values are strings as the user sees them

  private:
    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );
    void updateToolTip ( const QString & toolTip );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

    long lastValue;
    QString lastUserValue;

    bool ignoreSingleShotRead;

    // Drag and Drop
protected:
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    // Don't drag from interactive widget void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDrop( QVariant drop );
    QVariant getDrop();

};

#endif ///QCAComboBOX_H
