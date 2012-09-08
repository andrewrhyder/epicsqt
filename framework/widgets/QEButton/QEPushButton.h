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

#ifndef QEPUSHBUTTON_H
#define QEPUSHBUTTON_H

#include <QPushButton>
#include <QCaWidget.h>
#include <QCaString.h>
#include <QCaStringFormatting.h>
#include <QCaPluginLibrary_global.h>
#include <QEForm.h>
#include <UserMessage.h>
#include <ContainerProfile.h>
#include <managePixmaps.h>
#include <QEGenericButton.h>
#include <QCaVariableNamePropertyManager.h>


class QCAPLUGINLIBRARYSHARED_EXPORT QEPushButton : public QPushButton, public QEGenericButton {
    Q_OBJECT

  public:
    QEPushButton( QWidget *parent = 0 );
    QEPushButton( const QString& variableName, QWidget *parent = 0 );

  private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo ) { QEGenericButton::connectionChanged( connectionInfo ); }
    void setButtonText( const QString& text, QCaAlarmInfo& alarmInfo, QCaDateTime& timestamp, const unsigned int& variableIndex ) { setGenericButtonText( text, alarmInfo, timestamp, variableIndex); }
    void userPressed() { QEGenericButton::userPressed(); }
    void userReleased() { QEGenericButton::userReleased(); }
    void userClicked( bool checked ) { QEGenericButton::userClicked( checked ); }

public slots:
    void launchGui( QString guiName, QEForm::creationOptions creationOption ){ QEGenericButton::launchGui( guiName, creationOption); }
    void requestEnabled( const bool& state ){ setApplicationEnabled( state ); } //!! move into Standard Properties section??
    void onGeneralMessage( QString message ){ QEGenericButton::onGeneralMessage( message ); }


  signals:
    void dbValueChanged( const QString& out );

    void newGui( QString guiName, QEForm::creationOptions creationOption );

    void requestResend();

  protected:

private:
    void setup();
    void updateToolTip ( const QString & toolTip );

    void setButtonState( bool checked ){ QPushButton::setChecked( checked ); }
    void setButtonText( QString text ){ QPushButton::setText( text ); }
    QString getButtonText(){ return text(); }
    void setButtonIcon( QIcon& icon ) {setIcon( icon ); }

    void setButtonStyleSheet( QString style ){ setStyleSheet( style ); }

    void emitDbValueChanged( QString text ){ emit dbValueChanged( text ); }

    void emitNewGui( QString guiName, QEForm::creationOptions creationOption  ){ emit newGui( guiName, creationOption); }

    void connectButtonDataChange( qcaobject::QCaObject* qca )
    {
                QObject::connect( qca,  SIGNAL( stringChanged( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                                  this, SLOT( setButtonText( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
                QObject::connect( this, SIGNAL( requestResend() ),
                                  qca, SLOT( resendLastData() ) );
    }

    QObject* getButtonQObject(){ return this; }

    void stringFormattingChange(){ requestResend(); }

    QEGenericButton::updateOptions getDefaultUpdateOption() { return QEGenericButton::UPDATE_TEXT; }

    // Drag and Drop
protected:
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    // Don't drag from interactive widget void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDrop( QVariant drop );
    QVariant getDrop();


public:

    //=================================================================================
    // Multiple Variable properties
    // These properties should be identical for every widget using multiple variables (The number of variables may vary).
    // WHEN MAKING CHANGES: search for MULTIPLEVARIABLEPROPERTIESBASE and change all occurances.
    private:
        QCaVariableNamePropertyManager variableNamePropertyManagers[QEGENERICBUTTON_NUM_VARIABLES];
    public:

    // Define a variable
    // Note, the QPROPERTY declaration itself can't be in this macro
#define VARIABLE_PROPERTY_ACCESS(VAR_INDEX) \
    void    setVariableName##VAR_INDEX##Property( QString variableName ){ variableNamePropertyManagers[VAR_INDEX].setVariableNameProperty( variableName ); } \
    QString getVariableName##VAR_INDEX##Property(){ return variableNamePropertyManagers[VAR_INDEX].getVariableNameProperty(); }

    VARIABLE_PROPERTY_ACCESS(0)
    Q_PROPERTY(QString variable READ getVariableName0Property WRITE setVariableName0Property)

    VARIABLE_PROPERTY_ACCESS(1)
    Q_PROPERTY(QString altReadbackVariable READ getVariableName1Property WRITE setVariableName1Property)

    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)
    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions )
    {
        for( int i = 0; i < QEGENERICBUTTON_NUM_VARIABLES; i++ )
        {
            variableNamePropertyManagers[i].setSubstitutionsProperty( variableNameSubstitutions );
        }
    }
    QString getVariableNameSubstitutionsProperty()
    {
        return variableNamePropertyManagers[0].getSubstitutionsProperty();
    }

private slots:
  void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )
  {
      setVariableNameAndSubstitutions(variableNameIn, variableNameSubstitutionsIn, variableIndex);
  }

public:
    //=================================================================================

    Q_PROPERTY(bool subscribe READ getSubscribe WRITE setSubscribe)

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
    Q_PROPERTY(QString userLevelUserStyle READ getStyleUser WRITE setStyleUser)
    Q_PROPERTY(QString userLevelScientistStyle READ getStyleScientist WRITE setStyleScientist)
    Q_PROPERTY(QString userLevelEngineerStyle READ getStyleEngineer WRITE setStyleEngineer)
    Q_ENUMS(UserLevels)
    Q_PROPERTY(UserLevels userLevelVisibility READ getUserLevelVisibilityProperty WRITE setUserLevelVisibilityProperty)
    Q_PROPERTY(UserLevels userLevelEnabled READ getUserLevelEnabledProperty WRITE setUserLevelEnabledProperty)
    //=================================================================================


    // Widget specific properties

    /// Update options (text, pixmap, or both)
    Q_ENUMS(UpdateOptions)
    Q_PROPERTY(UpdateOptions updateOption READ getUpdateOptionProperty WRITE setUpdateOptionProperty)
    enum UpdateOptions { Text        = QEPushButton::UPDATE_TEXT,
                         Icon        = QEPushButton::UPDATE_ICON,
                         TextAndIcon = QEPushButton::UPDATE_TEXT_AND_ICON,
                         State       = QEPushButton::UPDATE_STATE };
    void setUpdateOptionProperty( UpdateOptions updateOption ){ setUpdateOption( (QEPushButton::updateOptions)updateOption ); }
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

    /// String formatting properties
    Q_PROPERTY(unsigned int precision READ getPrecision WRITE setPrecision)
    Q_PROPERTY(bool useDbPrecision READ getUseDbPrecision WRITE setUseDbPrecision)
    Q_PROPERTY(bool leadingZero READ getLeadingZero WRITE setLeadingZero)
    Q_PROPERTY(bool trailingZeros READ getTrailingZeros WRITE setTrailingZeros)
    Q_PROPERTY(bool addUnits READ getAddUnits WRITE setAddUnits)
    Q_PROPERTY(QString/*localEnumerationList*/ localEnumeration READ getLocalEnumeration WRITE setLocalEnumeration)
    Q_PROPERTY(Qt::Alignment alignment READ getTextAlignment WRITE setTextAlignment )

    Q_ENUMS(Formats)
    Q_PROPERTY(Formats format READ getFormatProperty WRITE setFormatProperty)
    enum Formats { Default         = QCaStringFormatting::FORMAT_DEFAULT,
                   Floating        = QCaStringFormatting::FORMAT_FLOATING,
                   Integer         = QCaStringFormatting::FORMAT_INTEGER,
                   UnsignedInteger = QCaStringFormatting::FORMAT_UNSIGNEDINTEGER,
                   Time            = QCaStringFormatting::FORMAT_TIME,
                   LocalEnumeration = QCaStringFormatting::FORMAT_LOCAL_ENUMERATE };
    void setFormatProperty( Formats format ){ setFormat( (QCaStringFormatting::formats)format ); }
    Formats getFormatProperty(){ return (Formats)getFormat(); }

    Q_ENUMS(Notations)
    Q_PROPERTY(Notations notation READ getNotationProperty WRITE setNotationProperty)
    enum Notations { Fixed      = QCaStringFormatting::NOTATION_FIXED,
                     Scientific = QCaStringFormatting::NOTATION_SCIENTIFIC,
                     Automatic  = QCaStringFormatting::NOTATION_AUTOMATIC };
    void setNotationProperty( Notations notation ){ setNotation( (QCaStringFormatting::notations)notation ); }
    Notations getNotationProperty(){ return (Notations)getNotation(); }

    Q_PROPERTY(QString password READ getPassword WRITE setPassword)

    Q_PROPERTY(bool writeOnPress READ getWriteOnPress WRITE setWriteOnPress)
    Q_PROPERTY(bool writeOnRelease READ getWriteOnRelease WRITE setWriteOnRelease)
    Q_PROPERTY(bool writeOnClick READ getWriteOnClick WRITE setWriteOnClick)

    Q_PROPERTY(QString pressText READ getPressText WRITE setPressText)
    Q_PROPERTY(QString releaseText READ getReleaseText WRITE setReleaseText)
    Q_PROPERTY(QString clickText READ getClickText WRITE setClickText)
    Q_PROPERTY(QString clickCheckedText READ getClickCheckedText WRITE setClickCheckedText)

    Q_PROPERTY(QString labelText READ getLabelTextProperty WRITE setLabelTextProperty);

    Q_PROPERTY(QString program READ getProgram WRITE setProgram)
    Q_PROPERTY(QStringList arguments READ getArguments WRITE setArguments)



    // Note, a property macro in the form 'Q_PROPERTY(QString guiName READ ...' doesn't work.
    // A property name ending with 'Name' results in some sort of string variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
    Q_PROPERTY(QString guiFile READ getGuiName WRITE setGuiName)

    // Creation options
    Q_ENUMS(CreationOptionNames)
    Q_PROPERTY(CreationOptionNames creationOption READ getCreationOptionProperty WRITE setCreationOptionProperty)
    enum CreationOptionNames { Open = QEForm::CREATION_OPTION_OPEN,
                               NewTab = QEForm::CREATION_OPTION_NEW_TAB,
                               NewWindow = QEForm::CREATION_OPTION_NEW_WINDOW };
    void setCreationOptionProperty( CreationOptionNames creationOptionIn ){ setCreationOption( (QEForm::creationOptions)creationOptionIn ); }
    CreationOptionNames getCreationOptionProperty(){ return (CreationOptionNames)getCreationOption(); }

    Q_PROPERTY(unsigned int messageSourceId READ getMessageSourceId WRITE setMessageSourceId )

};

#endif // QEPUSHBUTTON_H
