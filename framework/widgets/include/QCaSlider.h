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

#ifndef QCASLIDER_H
#define QCASLIDER_H

#include <QSlider>
#include <QCaWidget.h>
#include <QCaFloating.h>
#include <QCaFloatingFormatting.h>
#include <QCaPluginLibrary_global.h>

class QCAPLUGINLIBRARYSHARED_EXPORT QCaSlider : public QSlider, public QCaWidget {
    Q_OBJECT

  public:
    QCaSlider( QWidget *parent = 0 );
    QCaSlider( const QString& variableName, QWidget *parent = 0 );

    // Property convenience functions

    // write on change
    void setWriteOnChange( bool writeOnChange );
    bool getWriteOnChange();

    // subscribe
    void setSubscribe( bool subscribe );
    bool getSubscribe();

    // Set scale and offset (used to scale data when inteter scale bar min and max are not suitable)
    void setScale( double scaleIn );
    double getScale();
    void setOffset( double offsetIn );
    double getOffset();

  protected:
    QCaFloatingFormatting floatingFormatting; /// Floating formatting options.
    bool writeOnChange;             /// Write changed value to database when ever the position changes.

    void establishConnection( unsigned int variableIndex );

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setValueIfNoFocus( const double& value, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );
    void userValueChanged( const int& newValue );

  public slots:
    void requestEnabled( const bool& state ){ setApplicationEnabled( state ); } //!! with the MOC mind if this is moved into standardProperties.inc
    void writeNow();

  signals:
    void dbValueChanged( const qlonglong& out );

  private:
    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );
    void updateToolTip ( const QString & toolTip );

    bool updateInProgress;                  /// Ignore changes during updates, it isn't the user changing the slider.

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

    bool ignoreSingleShotRead;

    double scale;
    double offset;

    // Drag and Drop
protected:
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    // Don't drag from interactive widget void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDrop( QVariant drop );
    QVariant getDrop();
};

#endif /// QCASLIDER_H
