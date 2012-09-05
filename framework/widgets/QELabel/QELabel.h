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

#ifndef QELABEL_H
#define QELABEL_H

#include <QLabel>
#include <QCaWidget.h>
#include <QCaString.h>
#include <QCaStringFormatting.h>
#include <QCaPluginLibrary_global.h>
#include <managePixmaps.h>
#include <QCaStringFormattingMethods.h>
#include <QCaVariableNamePropertyManager.h>


class QCAPLUGINLIBRARYSHARED_EXPORT QELabel : public QLabel, public QCaWidget, public managePixmaps, public QCaStringFormattingMethods {
    Q_OBJECT

  public:
    enum updateOptions { UPDATE_TEXT, UPDATE_PIXMAP };

    QELabel( QWidget *parent = 0 );
    QELabel( const QString &variableName, QWidget *parent = 0 );

    // Property convenience functions

    // Update option (icon, text, or both)
    void setUpdateOption( updateOptions updateOptionIn );
    updateOptions getUpdateOption();

  protected:
    void establishConnection( unsigned int variableIndex );

    updateOptions updateOption;

private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setLabelText( const QString& text, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )//!! move into Standard Properties section??
    {
        setVariableNameAndSubstitutions(variableNameIn, variableNameSubstitutionsIn, variableIndex);
    }

public slots:
    void requestEnabled( const bool& state ){ setApplicationEnabled( state ); } //!! move into Standard Properties section??

  signals:
    void dbValueChanged( const QString& out );
    void requestResend();

  private:
    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );
    void updateToolTip( const QString& tip );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

    QString lastTextStyle;
    QString currentText;

    void stringFormattingChange(){ requestResend(); }



    // Drag and Drop
protected:
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDrop( QVariant drop );
    QVariant getDrop();

    // Copy paste
    QString copyVariable();
    QVariant copyData();
    void paste( QVariant s );


public:
    //=================================================================================
    // Single Variable properties
    // These properties should be identical for every widget using a single variable.
    // WHEN MAKING CHANGES: search for SINGLEVARIABLEPROPERTIES and change all occurances.
    //
    // Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
    // A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
    Q_PROPERTY(QString variable READ getVariableNameProperty WRITE setVariableNameProperty);
    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)

    void    setVariableNameProperty( QString variableName ){ variableNamePropertyManager.setVariableNameProperty( variableName ); }
    QString getVariableNameProperty(){ return variableNamePropertyManager.getVariableNameProperty(); }

    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions ){ variableNamePropertyManager.setSubstitutionsProperty( variableNameSubstitutions ); }
    QString getVariableNameSubstitutionsProperty(){ return variableNamePropertyManager.getSubstitutionsProperty(); }

private:
    QCaVariableNamePropertyManager variableNamePropertyManager;
public:
    //=================================================================================

    //=================================================================================
    // Standard properties
    // These properties should be identical for every widget using them.
    // WHEN MAKING CHANGES: search for STANDARDPROPERTIES and change all occurances.
    bool isEnabled() const { return getApplicationEnabled(); }
    void setEnabled( bool state ){ setApplicationEnabled( state ); }
    Q_PROPERTY(bool variableAsToolTip READ getVariableAsToolTip WRITE setVariableAsToolTip)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
    Q_PROPERTY(bool allowDrop READ getAllowDrop WRITE setAllowDrop)
    Q_PROPERTY(bool visible READ getRunVisible WRITE setRunVisible)
    Q_PROPERTY(unsigned int messageSourceId READ getMessageSourceId WRITE setMessageSourceId )
    Q_PROPERTY(QString userLevelUserStyle READ getStyleUser WRITE setStyleUser);
    Q_PROPERTY(QString userLevelScientistStyle READ getStyleScientist WRITE setStyleScientist);
    Q_PROPERTY(QString userLevelEngineerStyle READ getStyleEngineer WRITE setStyleEngineer);
    Q_ENUMS(UserLevels)
    Q_PROPERTY(UserLevels userLevelVisibility READ getUserLevelVisibilityProperty WRITE setUserLevelVisibilityProperty);
    Q_PROPERTY(UserLevels userLevelEnabled READ getUserLevelEnabledProperty WRITE setUserLevelEnabledProperty);
    //=================================================================================

    //=================================================================================
    // String formatting properties
    // These properties should be identical for every widget managing strings.
    // WHEN MAKING CHANGES: search for STRINGPROPERTIES and change all occurances.
    Q_PROPERTY(int  precision READ getPrecision WRITE setPrecision)
    Q_PROPERTY(bool useDbPrecision READ getUseDbPrecision WRITE setUseDbPrecision)
    Q_PROPERTY(bool leadingZero READ getLeadingZero WRITE setLeadingZero)
    Q_PROPERTY(bool trailingZeros READ getTrailingZeros WRITE setTrailingZeros)
    Q_PROPERTY(bool addUnits READ getAddUnits WRITE setAddUnits)
    Q_PROPERTY(QString/*localEnumerationList*/ localEnumeration READ getLocalEnumeration WRITE setLocalEnumeration)
    Q_ENUMS(Formats)
    Q_PROPERTY(Formats format READ getFormatProperty WRITE setFormatProperty)
    Q_PROPERTY(unsigned int radix READ getRadix WRITE setRadix)
    Q_ENUMS(Notations)
    Q_PROPERTY(Notations notation READ getNotationProperty WRITE setNotationProperty)
    Q_ENUMS(ArrayActions)
    Q_PROPERTY(ArrayActions arrayAction READ getArrayActionProperty WRITE setArrayActionProperty)
    Q_PROPERTY(unsigned int arrayIndex READ getArrayIndex WRITE setArrayIndex)
    //=================================================================================

//==========================================================================
// Widget specific properties

public:
    /// Update options (text, pixmap, or both)
    Q_ENUMS(UpdateOptions)
    Q_PROPERTY(UpdateOptions updateOption READ getUpdateOptionProperty WRITE setUpdateOptionProperty)
    enum UpdateOptions { Text     = QELabel::UPDATE_TEXT,
                         Picture  = QELabel::UPDATE_PIXMAP };
    void setUpdateOptionProperty( UpdateOptions updateOption ){ setUpdateOption( (QELabel::updateOptions)updateOption ); }
    UpdateOptions getUpdateOptionProperty(){ return (UpdateOptions)getUpdateOption(); }

    /// Pixmaps
    Q_PROPERTY(QPixmap pixmap0 READ getPixmap0Property WRITE setPixmap0Property)
    Q_PROPERTY(QPixmap pixmap1 READ getPixmap1Property WRITE setPixmap1Property)
    Q_PROPERTY(QPixmap pixmap2 READ getPixmap2Property WRITE setPixmap2Property)
    Q_PROPERTY(QPixmap pixmap3 READ getPixmap3Property WRITE setPixmap3Property)
    Q_PROPERTY(QPixmap pixmap4 READ getPixmap4Property WRITE setPixmap4Property)
    Q_PROPERTY(QPixmap pixmap5 READ getPixmap5Property WRITE setPixmap5Property)
    Q_PROPERTY(QPixmap pixmap6 READ getPixmap6Property WRITE setPixmap6Property)
    Q_PROPERTY(QPixmap pixmap7 READ getPixmap7Property WRITE setPixmap7Property)

    void setPixmap0Property( QPixmap pixmap ){ setDataPixmap( pixmap, 0 ); }
    void setPixmap1Property( QPixmap pixmap ){ setDataPixmap( pixmap, 1 ); }
    void setPixmap2Property( QPixmap pixmap ){ setDataPixmap( pixmap, 2 ); }
    void setPixmap3Property( QPixmap pixmap ){ setDataPixmap( pixmap, 3 ); }
    void setPixmap4Property( QPixmap pixmap ){ setDataPixmap( pixmap, 4 ); }
    void setPixmap5Property( QPixmap pixmap ){ setDataPixmap( pixmap, 5 ); }
    void setPixmap6Property( QPixmap pixmap ){ setDataPixmap( pixmap, 6 ); }
    void setPixmap7Property( QPixmap pixmap ){ setDataPixmap( pixmap, 7 ); }

    QPixmap getPixmap0Property(){ return getDataPixmap( 0 ); }
    QPixmap getPixmap1Property(){ return getDataPixmap( 1 ); }
    QPixmap getPixmap2Property(){ return getDataPixmap( 2 ); }
    QPixmap getPixmap3Property(){ return getDataPixmap( 3 ); }
    QPixmap getPixmap4Property(){ return getDataPixmap( 4 ); }
    QPixmap getPixmap5Property(){ return getDataPixmap( 5 ); }
    QPixmap getPixmap6Property(){ return getDataPixmap( 6 ); }
    QPixmap getPixmap7Property(){ return getDataPixmap( 7 ); }


};

#endif /// QELABEL_H
