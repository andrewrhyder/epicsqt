/*! 
  \class QCaLabel
  \version $Revision: #13 $
  \date $DateTime: 2010/09/06 13:16:04 $
  \author andrew.rhyder
  \brief Manage updating tool tip with variable name, alarm state and connected state
 */
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

#ifndef QCALABEL_H
#define QCALABEL_H

#include <QLabel>
#include <QCaWidget.h>
#include <QCaString.h>
#include <QCaStringFormatting.h>
#include <QCaPluginLibrary_global.h>
#include <managePixmaps.h>
#include <QCaStringFormattingMethods.h>

class QCAPLUGINLIBRARYSHARED_EXPORT QCaLabel : public QLabel, public QCaWidget, public managePixmaps, public QCaStringFormattingMethods {
    Q_OBJECT

  public:
    enum updateOptions { UPDATE_TEXT, UPDATE_PIXMAP };

    QCaLabel( QWidget *parent = 0 );
    QCaLabel( const QString &variableName, QWidget *parent = 0 );

    bool isEnabled() const;
    void setEnabled( bool state );

    // Property convenience functions

    // Update option (icon, text, or both)
    void setUpdateOption( updateOptions updateOptionIn );
    updateOptions getUpdateOption();

    // Variable Name and substitution
    void setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );

    // variable as tool tip
    void setVariableAsToolTip( bool variableAsToolTip );
    bool getVariableAsToolTip();

    // visible (widget is visible outside 'Designer')
    void setRunVisible( bool visibleIn );
    bool getRunVisible();



  protected:
    bool caEnabled;

    void establishConnection( unsigned int variableIndex );

    bool caVisible;               // Flag true if the widget should be visible outside 'Designer'

    updateOptions updateOption;

private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setLabelText( const QString& text, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );

  public slots:
    void requestEnabled( const bool& state );

  signals:
    void dbValueChanged( const QString& out );
    void requestResend();

  private:
    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );
    void updateToolTip( const QString& tip );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

    QString lastColor;

    QString defaultStyleSheet;
    QString alarmStyleSheet;
    QString colorStyleSheet;
    void updateStyleSheet();

    void stringFormattingChange(){ requestResend(); }
};

#endif /// QCALABEL_H
