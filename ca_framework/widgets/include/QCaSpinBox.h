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

#ifndef QCASPINBOX_H
#define QCASPINBOX_H

#include <QDoubleSpinBox>
#include <QCaWidget.h>
#include <QCaFloating.h>
#include <QCaFloatingFormatting.h>
#include <QCaPluginLibrary_global.h>

class QCAPLUGINLIBRARYSHARED_EXPORT QCaSpinBox : public QDoubleSpinBox, public QCaWidget {
    Q_OBJECT

  public:
    QCaSpinBox( QWidget *parent = 0 );
    QCaSpinBox( const QString& variableName, QWidget *parent = 0 );

    bool isEnabled() const;
    void setEnabled( bool state );

    // Property convenience functions

    // Variable name and substitutions
    void setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );

    // subscribe
    void setSubscribe( bool subscribe );
    bool getSubscribe();

    // variable as tool tip
    void setVariableAsToolTip( bool variableAsToolTip );
    bool getVariableAsToolTip();

    // Allow user to drop new PVs into this widget
    void setAllowDrop( bool allowDropIn );
    bool getAllowDrop();

  protected:
    QCaFloatingFormatting floatingFormatting;
    bool localEnabled;
    bool allowDrop;

    void establishConnection( unsigned int variableIndex );

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setValueIfNoFocus( const double& value, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );
    void userValueChanged( double value );
    void userChange( const QString& oldValue, const QString& newValue, const QString& lastValue );    // Signal a user attempt to change a value. Values are strings as the user sees them

  public slots:
    void requestEnabled( const bool& state );

  signals:
    void dbValueChanged( const double& out );

private:
    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );
    void updateToolTip ( const QString & toolTip );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

    bool programaticValueChange;   // Flag set while the spin box value is being changed programatically (not by the user)
    double lastValue;
    QString lastUserValue;

    // Drag and Drop
protected:
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    // Don't drag from interactive widget void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDrop( QVariant drop );
    QVariant getDrop();
};

#endif ///QCASPINBOX_H
