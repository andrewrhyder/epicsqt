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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef QEIMAGE_H
#define QEIMAGE_H

#include <QScrollArea>
#include <QEWidget.h>
#include <QEInteger.h>
#include <videowidget.h>
//#include <QELabel.h>
#include <QVBoxLayout>
#include <QGridLayout>
#include <profilePlot.h>
#include <zoomMenu.h>
#include <flipRotateMenu.h>
#include <selectMenu.h>
#include <QCaVariableNamePropertyManager.h>


#include <QEPluginLibrary_global.h>
#include <QEIntegerFormatting.h>


class QEPLUGINLIBRARYSHARED_EXPORT QEImage : public QFrame, public QEWidget {
    Q_OBJECT

  public:

    QEImage( QWidget *parent = 0 );
    QEImage( const QString &variableName, QWidget *parent = 0 );
    ~QEImage();

    enum selectOptions{ SO_NONE, SO_PANNING, SO_VSLICE, SO_HSLICE, SO_AREA1, SO_AREA2, SO_AREA3, SO_AREA4, SO_PROFILE, SO_TARGET, SO_BEAM };
    selectOptions getSelectionOption();

    // Property convenience functions

    // Allow user to set the video format
    enum formatOptions{ GREY8, GREY12, GREY16, RGB_888 };
    void setFormatOption( formatOptions formatOption );
    formatOptions getFormatOption();

    // Size options
    enum resizeOptions { RESIZE_OPTION_ZOOM, RESIZE_OPTION_FIT };
    void setResizeOption( resizeOptions resizeOptionIn );
    resizeOptions getResizeOption();

    // Zoom
    void setZoom( int zoomIn );
    int getZoom();

    // Rotation
    enum rotationOptions { ROTATION_0, ROTATION_90_RIGHT, ROTATION_90_LEFT, ROTATION_180 };
    void setRotation( rotationOptions rotationIn );
    rotationOptions getRotation();

    void setHorizontalFlip( bool flipHozIn );
    bool getHorizontalFlip();

    void setVerticalFlip( bool flipVertIn );
    bool getVerticalFlip();

    void setInitialHozScrollPos( int initialHosScrollPosIn );
    int getInitialHozScrollPos();

    void setInitialVertScrollPos( int initialVertScrollPosIn );
    int getInitialVertScrollPos();

    // Display the acquire period
    void setDisplayAcquirePeriod( bool displayAcquirePeriodIn );
    bool getDisplayAcquirePeriod();

    // Display the exposure time
    void setDisplayExposureTime( bool displayExposureTimeIn );
    bool getDisplayExposureTime();

    void setDisplayButtonBar( bool displayButtonBarIn );
    bool getDisplayButtonBar();

    void setShowTime(bool pValue);
    bool getShowTime();

    void setVertSliceMarkupColor(QColor pValue);
    QColor getVertSliceMarkupColor();

    void setHozSliceMarkupColor(QColor pValue);
    QColor getHozSliceMarkupColor();

    void setProfileMarkupColor(QColor pValue);
    QColor getProfileMarkupColor();

    void setAreaMarkupColor(QColor pValue);
    QColor getAreaMarkupColor();

    void setTargetMarkupColor(QColor pValue);
    QColor getTargetMarkupColor();

    void setBeamMarkupColor(QColor pValue);
    QColor getBeamMarkupColor();

    void setTimeMarkupColor(QColor pValue);
    QColor getTimeMarkupColor();

    void setDisplayCursorPixelInfo( bool displayCursorPixelInfoIn );
    bool getDisplayCursorPixelInfo();

    void setContrastReversal( bool contrastReversalIn );
    bool getContrastReversal();

    void setEnablePan( bool enablePanIn );
    bool getEnablePan();

    void setEnableVertSliceSelection( bool enableVSliceSelectionIn );
    bool getEnableVertSliceSelection();

    void setEnableHozSliceSelection( bool enableHSliceSelectionIn );
    bool getEnableHozSliceSelection();

    void setEnableAreaSelection( bool enableAreaSelectionIn );
    bool getEnableAreaSelection();

    void setEnableProfileSelection( bool enableProfileSelectionIn );
    bool getEnableProfileSelection();

    void setEnableTargetSelection( bool enableTargetSelectionIn );
    bool getEnableTargetSelection();


  protected:
    QEIntegerFormatting integerFormatting; // Integer formatting options.

    void establishConnection( unsigned int variableIndex );

    enum variableIndexes{ IMAGE_VARIABLE,
                          WIDTH_VARIABLE, HEIGHT_VARIABLE,
                          ROI1_X_VARIABLE, ROI1_Y_VARIABLE, ROI1_W_VARIABLE, ROI1_H_VARIABLE,
                          ROI2_X_VARIABLE, ROI2_Y_VARIABLE, ROI2_W_VARIABLE, ROI2_H_VARIABLE,
                          ROI3_X_VARIABLE, ROI3_Y_VARIABLE, ROI3_W_VARIABLE, ROI3_H_VARIABLE,
                          ROI4_X_VARIABLE, ROI4_Y_VARIABLE, ROI4_W_VARIABLE, ROI4_H_VARIABLE,
                          TARGET_X_VARIABLE, TARGET_Y_VARIABLE,
                          BEAM_X_VARIABLE, BEAM_Y_VARIABLE,
                          TARGET_TRIGGER_VARIABLE,
                          CLIPPING_ONOFF_VARIABLE, CLIPPING_LOW_VARIABLE, CLIPPING_HIGH_VARIABLE,
                          QEIMAGE_NUM_VARIABLES /*Must be last*/ };

    resizeOptions resizeOption;
    int zoom;
    rotationOptions rotation;
    bool flipVert;
    bool flipHoz;

    int initialHozScrollPos;
    int initialVertScrollPos;

    bool displayButtonBar;

private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setImage( const QByteArray& image, unsigned long dataSize, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );
    void setDimension( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex);
    void setClipping( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex);
    void setROI( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex);

    void vSliceSelectModeClicked();
    void hSliceSelectModeClicked();
    void area1SelectModeClicked();
    void area2SelectModeClicked();
    void area3SelectModeClicked();
    void area4SelectModeClicked();
    void profileSelectModeClicked();
    void targetSelectModeClicked();
    void beamSelectModeClicked();

    void panModeClicked();

    // !! move this functionality into QEWidget???
    // !! needs one for single variables and one for multiple variables, or just the multiple variable one for all
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )// !! move into Standard Properties section??
    {
        setVariableNameAndSubstitutions(variableNameIn, variableNameSubstitutionsIn, variableIndex);
    }

    void userSelection( imageMarkup::markupIds mode, bool complete, bool clearing, QPoint point1, QPoint point2 );
    void zoomInOut( int zoomAmount );
    void currentPixelInfo( QPoint pos );
    void pan( QPoint pos );
    void showContextMenu( const QPoint& );
    void selectMenuTriggered( QAction* selectedItem );
    void zoomMenuTriggered( QAction* selectedItem );
    void flipRotateMenuTriggered( QAction* selectedItem );
    void setImageFile( QString name );

public slots:
    // Slots to allow external setting of selection menu options
    void setSelectPanMode()     { sMenu->setChecked(  QEImage::SO_PANNING ); panModeClicked(); }
    void setSelectVSliceMode()  { sMenu->setChecked(  QEImage::SO_VSLICE );  vSliceSelectModeClicked(); }
    void setSelectHSliceMode()  { sMenu->setChecked(  QEImage::SO_HSLICE );  hSliceSelectModeClicked(); }
    void setSelectArea1Mode()   { sMenu->setChecked(  QEImage::SO_AREA1 );   area1SelectModeClicked(); }
    void setSelectArea2Mode()   { sMenu->setChecked(  QEImage::SO_AREA2 );   area2SelectModeClicked(); }
    void setSelectArea3Mode()   { sMenu->setChecked(  QEImage::SO_AREA3 );   area3SelectModeClicked(); }
    void setSelectArea4Mode()   { sMenu->setChecked(  QEImage::SO_AREA4 );   area4SelectModeClicked(); }
    void setSelectProfileMode() { sMenu->setChecked(  QEImage::SO_PROFILE ); profileSelectModeClicked(); }
    void setSelectTargetMode()  { sMenu->setChecked(  QEImage::SO_TARGET );  targetSelectModeClicked(); }
    void setSelectBeamMode()    { sMenu->setChecked(  QEImage::SO_BEAM );    beamSelectModeClicked(); }

    // Slots to allow external operation of control buttons
    void pauseClicked();

    void saveClicked();

    void roi1Changed();
    void roi2Changed();
    void roi3Changed();
    void roi4Changed();

    void targetClicked();


  signals:
    // Note, the following signals are common to many QE widgets,
    // if changing the doxygen comments, ensure relevent changes are migrated to all instances
    /// Sent when the widget is updated following a data change
    /// Can be used to pass on EPICS data (as presented in this widget) to other widgets.
    /// For example a QList widget could log updates from this widget.
    void dbValueChanged( const QString& out );
    /// Internal use only. Used when changing a property value to force a re-display to reflect the new property value.
    void requestResend();

  private:
    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

    void setImageBuff();

    QGridLayout *mainLayout;

    QFrame *buttonGroup;

    QScrollArea* scrollArea;
    bool initScrollPosSet;

    VideoWidget* videoWidget;

    QGridLayout* infoLayout;
    QLabel* currentCursorPixelLabel;
    QLabel* currentVertPixelLabel;
    QLabel* currentHozPixelLabel;
    QLabel* currentLineLabel;
    QLabel* currentArea1Label;
    QLabel* currentArea2Label;
    QLabel* currentArea3Label;
    QLabel* currentArea4Label;
    QLabel* currentTargetLabel;
    QLabel* currentBeamLabel;


    QPushButton* pauseButton;
    QPushButton* saveButton;
    QPushButton* targetButton;
    QPushButton* selectModeButton;
    QPushButton* zoomButton;
    QPushButton* flipRotateButton;

    profilePlot* vSliceDisplay;
    profilePlot* hSliceDisplay;
    profilePlot* profileDisplay;


    zoomMenu*       zMenu;
    flipRotateMenu* frMenu;
    selectMenu*     sMenu;

    bool paused;
    bool showTimeEnabled;

    bool enableAreaSelection;
    bool enableVSliceSelection;
    bool enableHSliceSelection;
    bool enableProfileSelection;
    bool enableTargetSelection;

    bool displayCursorPixelInfo;
    bool contrastReversal;


    void manageButtonBar();
    void manageInfoLayout();

    QCaDateTime imageTime;
    unsigned long imageDataSize;
    QByteArray image;       // Buffer to hold original image data.
    QByteArray imageBuff;   // Buffer to hold data converted to format for generating QImage.
#define IMAGEBUFF_BYTES_PER_PIXEL 4   // 4 bytes for Format_RGB32
    unsigned long imageBuffWidth;   // Original image width
    unsigned long imageBuffHeight;  // Original image height

    formatOptions formatOption;

    int vSliceX;
    int hSliceY;
    QPoint profileLineStart;
    QPoint profileLineEnd;
    QPoint selectedArea1Point1;
    QPoint selectedArea1Point2;
    QPoint selectedArea2Point1;
    QPoint selectedArea2Point2;
    QPoint selectedArea3Point1;
    QPoint selectedArea3Point2;
    QPoint selectedArea4Point1;
    QPoint selectedArea4Point2;
    QPoint target;
    QPoint beam;

    bool haveVSliceX;
    bool haveHSliceY;
    bool haveProfileLine;
    bool haveSelectedArea1;
    bool haveSelectedArea2;
    bool haveSelectedArea3;
    bool haveSelectedArea4;
    bool haveTarget; // !!! not used??
    bool haveBeam; // !!! not used??

    void generateVSlice( int x );
    void generateHSlice( int y );
    void generateProfile( QPoint point1, QPoint point2 );
    void displaySelectedArea1Info( QPoint point1, QPoint point2 );
    void displaySelectedArea2Info( QPoint point1, QPoint point2 );
    void displaySelectedArea3Info( QPoint point1, QPoint point2 );
    void displaySelectedArea4Info( QPoint point1, QPoint point2 );

    void updateMarkups();

    QVector<QPointF> vSliceData;
    QVector<QPointF> hSliceData;
    QVector<QPointF> profileData;

    int getPixelValueFromData( const unsigned char* ptr );
    double getFloatingPixelValueFromData( const unsigned char* ptr );


    unsigned int rotatedImageBuffWidth();
    unsigned int rotatedImageBuffHeight();


    void displayImage();
    int getScanOption();
    QPoint rotateFLipPoint( QPoint& pos );
    const unsigned char* getImageDataPtr( QPoint& pos );


    void zoomToArea();
    void setResizeOptionAndZoom( int zoomIn );

    double maxPixelValue();

    QIcon* pauseButtonIcon;
    QIcon* playButtonIcon;


    bool clippingOn;
    unsigned int clippingLow;
    unsigned int clippingHigh;


    // Drag and Drop
protected:
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    // Don't drag from interactive widget void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDrop( QVariant drop );
    QVariant getDrop();
    QString copyVariable();
    QVariant copyData();
    void paste( QVariant v );


    void resizeEvent(QResizeEvent* );

    //=================================================================================
    // Multiple Variable properties
    // These properties should be similar for every widget using multiple variables (The number of variables may vary).
    // WHEN MAKING CHANGES: search for MULTIPLEVARIABLEPROPERTIESBASE and change all occurances.
    private:
        QCaVariableNamePropertyManager variableNamePropertyManagers[QEIMAGE_NUM_VARIABLES];
    public:

    // Define a variable
    // Note, the QPROPERTY declaration itself can't be in this macro
#define VARIABLE_PROPERTY_ACCESS(VAR_INDEX) \
    void    setVariableName##VAR_INDEX##Property( QString variableName ){ variableNamePropertyManagers[VAR_INDEX].setVariableNameProperty( variableName ); } \
    QString getVariableName##VAR_INDEX##Property(){ return variableNamePropertyManagers[VAR_INDEX].getVariableNameProperty(); }

    VARIABLE_PROPERTY_ACCESS(0)
    /// EPICS variable name (CA PV).
    /// This variable is used as the source the image waveform.
    Q_PROPERTY(QString imageVariable READ getVariableName0Property WRITE setVariableName0Property)

    VARIABLE_PROPERTY_ACCESS(1)
    /// EPICS variable name (CA PV).
    /// This variable is used to read the width of the image.
    Q_PROPERTY(QString widthVariable READ getVariableName1Property WRITE setVariableName1Property)

    VARIABLE_PROPERTY_ACCESS(2)
    /// EPICS variable name (CA PV).
    /// This variable is used to read the height of the image.
    Q_PROPERTY(QString heightVariable READ getVariableName2Property WRITE setVariableName2Property)

    VARIABLE_PROPERTY_ACCESS(3)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the first region of interest X position.
    Q_PROPERTY(QString regionOfInterest1XVariable READ getVariableName3Property WRITE setVariableName3Property)

    VARIABLE_PROPERTY_ACCESS(4)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the first region of interest Y position.
    Q_PROPERTY(QString regionOfInterest1YVariable READ getVariableName4Property WRITE setVariableName4Property)

    VARIABLE_PROPERTY_ACCESS(5)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the first region of interest width.
    Q_PROPERTY(QString regionOfInterest1WVariable READ getVariableName5Property WRITE setVariableName5Property)

    VARIABLE_PROPERTY_ACCESS(6)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the first region of interest height.
    Q_PROPERTY(QString regionOfInterest1HVariable READ getVariableName6Property WRITE setVariableName6Property)

    VARIABLE_PROPERTY_ACCESS(7)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the second region of interest X position.
    Q_PROPERTY(QString regionOfInterest2XVariable READ getVariableName7Property WRITE setVariableName7Property)

    VARIABLE_PROPERTY_ACCESS(8)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the second region of interest Y position.
    Q_PROPERTY(QString regionOfInterest2YVariable READ getVariableName8Property WRITE setVariableName8Property)

    VARIABLE_PROPERTY_ACCESS(9)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the second region of interest width.
    Q_PROPERTY(QString regionOfInterest2WVariable READ getVariableName9Property WRITE setVariableName9Property)

    VARIABLE_PROPERTY_ACCESS(10)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the second region of interest height.
    Q_PROPERTY(QString regionOfInterest2HVariable READ getVariableName10Property WRITE setVariableName10Property)

    VARIABLE_PROPERTY_ACCESS(11)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the third region of interest X position.
    Q_PROPERTY(QString regionOfInterest3XVariable READ getVariableName11Property WRITE setVariableName11Property)

    VARIABLE_PROPERTY_ACCESS(12)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the third region of interest Y position.
    Q_PROPERTY(QString regionOfInterest3YVariable READ getVariableName12Property WRITE setVariableName12Property)

    VARIABLE_PROPERTY_ACCESS(13)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the third region of interest width.
    Q_PROPERTY(QString regionOfInterest3WVariable READ getVariableName13Property WRITE setVariableName13Property)

    VARIABLE_PROPERTY_ACCESS(14)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the third region of interest height.
    Q_PROPERTY(QString regionOfInterest3HVariable READ getVariableName14Property WRITE setVariableName14Property)

    VARIABLE_PROPERTY_ACCESS(15)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the fourth region of interest X position.
    Q_PROPERTY(QString regionOfInterest4XVariable READ getVariableName15Property WRITE setVariableName15Property)

    VARIABLE_PROPERTY_ACCESS(16)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the fourth region of interest Y position.
    Q_PROPERTY(QString regionOfInterest4YVariable READ getVariableName16Property WRITE setVariableName16Property)

    VARIABLE_PROPERTY_ACCESS(17)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the fourth region of interest width.
    Q_PROPERTY(QString regionOfInterest4WVariable READ getVariableName17Property WRITE setVariableName17Property)

    VARIABLE_PROPERTY_ACCESS(18)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the fourth region of interest height.
    Q_PROPERTY(QString regionOfInterest4HVariable READ getVariableName18Property WRITE setVariableName18Property)

    VARIABLE_PROPERTY_ACCESS(19)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the selected target X position.
    Q_PROPERTY(QString targetXVariable READ getVariableName19Property WRITE setVariableName19Property)

    VARIABLE_PROPERTY_ACCESS(20)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the selected target Y position.
    Q_PROPERTY(QString targetYVariable READ getVariableName20Property WRITE setVariableName20Property)

    VARIABLE_PROPERTY_ACCESS(21)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the selected beam X position.
    Q_PROPERTY(QString beamXVariable READ getVariableName21Property WRITE setVariableName21Property)

    VARIABLE_PROPERTY_ACCESS(22)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the selected beam Y position.
    Q_PROPERTY(QString beamYVariable READ getVariableName22Property WRITE setVariableName22Property)

    VARIABLE_PROPERTY_ACCESS(23)
    /// EPICS variable name (CA PV).
    /// This variable is used to write a 'trigger' to initiate movement of the target into the beam as defined by the target and beam X and Y positions.
    Q_PROPERTY(QString targetTriggerVariable READ getVariableName23Property WRITE setVariableName23Property)

    VARIABLE_PROPERTY_ACCESS(24)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the areadetector clipping on/off command.
    Q_PROPERTY(QString clippingOnOffVariable READ getVariableName24Property WRITE setVariableName24Property)

    VARIABLE_PROPERTY_ACCESS(25)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the areadetector clipping low level.
    Q_PROPERTY(QString clippingLowVariable READ getVariableName25Property WRITE setVariableName25Property)

    VARIABLE_PROPERTY_ACCESS(26)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the areadetector clipping high level.
    Q_PROPERTY(QString clippingHighVariable READ getVariableName26Property WRITE setVariableName26Property)

    /// Macro substitutions. The default is no substitutions. The format is NAME1=VALUE1[,] NAME2=VALUE2... Values may be quoted strings. For example, 'CAM=1, NAME = "Image 1"'
    /// These substitutions are applied to all the variable names.
    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)
private:
    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions )
    {
        for( int i = 0; i < QEIMAGE_NUM_VARIABLES; i++ )
        {
            variableNamePropertyManagers[i].setSubstitutionsProperty( variableNameSubstitutions );
        }
    }
    QString getVariableNameSubstitutionsProperty()
    {
        return variableNamePropertyManagers[0].getSubstitutionsProperty();
    }
public:
    //=================================================================================


    //=================================================================================
    // Standard properties
    // These properties should be identical for every widget using them.
    // WHEN MAKING CHANGES: search for STANDARDPROPERTIES and change all occurances.
public:
    /// Use the variable as the tool tip. Default is true. Tool tip property will be overwritten by the variable name.
    ///
    Q_PROPERTY(bool variableAsToolTip READ getVariableAsToolTip WRITE setVariableAsToolTip)

    /// Set the prefered 'enabled' state. Default is true.
    /// This property is copied to the standard Qt 'enabled' property if the data being displayed is valid.
    /// If the data being displayed is invalid the standard Qt 'enabled' property will always be set to false to indicate invalid data.
    /// The value of this property will only be copied to the standard Qt 'enabled' property once data is valid.
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)

    /// Access function for #enabled property - refer to #enabled property for details
    bool isEnabled() const { return getApplicationEnabled(); }

    /// Access function for #enabled property - refer to #enabled property for details
    void setEnabled( bool state ){ setApplicationEnabled( state ); }

    /// Allow drag/drops operations to this widget. Default is false. Any dropped text will be used as a new variable name.
    ///
    Q_PROPERTY(bool allowDrop READ getAllowDrop WRITE setAllowDrop)

    /// Display the widget. Default is true.
    /// Setting this property false is usefull if widget is only used to provide a signal - for example, when supplying data to a QELink widget.
    /// Note, when false the widget will still be visible in Qt Designer.
    Q_PROPERTY(bool visible READ getRunVisible WRITE setRunVisible)

    /// Set the ID used by the message filtering system. Default is zero.
    /// Widgets or applications that use messages from the framework have the option of filtering on this ID.
    /// For example, by using a unique message source ID a QELog widget may be set up to only log messages from a select set of widgets.
    Q_PROPERTY(unsigned int messageSourceId READ getMessageSourceId WRITE setMessageSourceId )

    /// Style Sheet string to be applied when the widget is displayed in 'User' mode. Default is an empty string.
    /// The syntax is the standard Qt Style Sheet syntax. For example, 'background-color: red'
    /// This Style Sheet string will be applied by the styleManager class.
    /// Refer to the styleManager class for details about how this Style Sheet string will be merged with any pre-existing Style Sheet string
    /// and any Style Sheet strings generated during the display of data.
    Q_PROPERTY(QString userLevelUserStyle READ getStyleUser WRITE setStyleUser)

    /// Style Sheet string to be applied when the widget is displayed in 'Scientist' mode. Default is an empty string.
    /// The syntax is the standard Qt Style Sheet syntax. For example, 'background-color: red'
    /// This Style Sheet string will be applied by the styleManager class.
    /// Refer to the styleManager class for details about how this Style Sheet string will be merged with any pre-existing Style Sheet string
    /// and any Style Sheet strings generated during the display of data.
    Q_PROPERTY(QString userLevelScientistStyle READ getStyleScientist WRITE setStyleScientist)

    /// Style Sheet string to be applied when the widget is displayed in 'Engineer' mode. Default is an empty string.
    /// The syntax is the standard Qt Style Sheet syntax. For example, 'background-color: red'
    /// This Style Sheet string will be applied by the styleManager class.
    /// Refer to the styleManager class for details about how this Style Sheet string will be merged with any pre-existing Style Sheet string
    /// and any Style Sheet strings generated during the display of data.
    Q_PROPERTY(QString userLevelEngineerStyle READ getStyleEngineer WRITE setStyleEngineer)

    /// \enum UserLevels
    /// User friendly enumerations for #userLevelVisibility and #userLevelEnabled properties - refer to #userLevelVisibility and #userLevelEnabled properties and userLevel enumeration for details.
    enum UserLevels { User      = USERLEVEL_USER,          ///< Refer to USERLEVEL_USER for details
                      Scientist = USERLEVEL_SCIENTIST,     ///< Refer to USERLEVEL_SCIENTIST for details
                      Engineer  = USERLEVEL_ENGINEER       ///< Refer to USERLEVEL_ENGINEER for details
                              };
    Q_ENUMS(UserLevels)

    /// Lowest user level at which the widget is visible. Default is 'User'.
    /// Used when designing GUIs that display more and more detail according to the user mode.
    /// The user mode is set application wide through the QELogin widget, or programatically through setUserLevel()
    /// Widgets that are always visible should be visible at 'User'.
    /// Widgets that are only used by scientists managing the facility should be visible at 'Scientist'.
    /// Widgets that are only used by engineers maintaining the facility should be visible at 'Engineer'.
    Q_PROPERTY(UserLevels userLevelVisibility READ getUserLevelVisibilityProperty WRITE setUserLevelVisibilityProperty)

    /// Lowest user level at which the widget is enabled. Default is 'User'.
    /// Used when designing GUIs that allow access to more and more detail according to the user mode.
    /// The user mode is set application wide through the QELogin widget, or programatically through setUserLevel()
    /// Widgets that are always accessable should be visible at 'User'.
    /// Widgets that are only accessable to scientists managing the facility should be visible at 'Scientist'.
    /// Widgets that are only accessable to engineers maintaining the facility should be visible at 'Engineer'.
    Q_PROPERTY(UserLevels userLevelEnabled READ getUserLevelEnabledProperty WRITE setUserLevelEnabledProperty)

    UserLevels getUserLevelVisibilityProperty() { return (UserLevels)getUserLevelVisibility(); }            ///< Access function for #userLevelVisibility property - refer to #userLevelVisibility property for details
    void setUserLevelVisibilityProperty( UserLevels level ) { setUserLevelVisibility( (userLevels)level ); }///< Access function for #userLevelVisibility property - refer to #userLevelVisibility property for details
    UserLevels getUserLevelEnabledProperty() { return (UserLevels)getUserLevelEnabled(); }                  ///< Access function for #userLevelEnabled property - refer to #userLevelEnabled property for details
    void setUserLevelEnabledProperty( UserLevels level ) { setUserLevelEnabled( (userLevels)level ); }      ///< Access function for #userLevelEnabled property - refer to #userLevelEnabled property for details

public slots:
    /// Similar to standard setEnabled slot, but allows QE widget to determine if the widget remains disabled due to invalid data.
    /// If disabled due to invalid data, a request to enable the widget will be honoured when the data is no longer invalid.
    void requestEnabled( const bool& state ){ setApplicationEnabled( state ); }

public:
    //=================================================================================

//==========================================================================
// Widget specific properties
public:

    // Format options (8 bit grey scale, 32 bit color, etc)
    Q_ENUMS(FormatOptions)
    Q_PROPERTY(FormatOptions formatOption READ getFormatOptionProperty WRITE setFormatOptionProperty)
    enum FormatOptions { Grey_8   = QEImage::GREY8,
                         Grey_12  = QEImage::GREY12,
                         Grey_16  = QEImage::GREY16,
                         RGB      = QEImage::RGB_888 };
    void setFormatOptionProperty( FormatOptions formatOption ){ setFormatOption( (QEImage::formatOptions)formatOption ); }
    FormatOptions getFormatOptionProperty(){ return (FormatOptions)getFormatOption(); }

    Q_PROPERTY(bool enableVertSliceSelection READ getEnableVertSliceSelection WRITE setEnableVertSliceSelection)
    Q_PROPERTY(bool enableHozSliceSelection READ getEnableHozSliceSelection WRITE setEnableHozSliceSelection)
    Q_PROPERTY(bool enableProfileSelection READ getEnableProfileSelection WRITE setEnableProfileSelection)

    Q_PROPERTY(bool enableAreaSelection READ getEnableAreaSelection WRITE setEnableAreaSelection)
    Q_PROPERTY(bool enableTargetSelection READ getEnableTargetSelection WRITE setEnableTargetSelection)

    Q_PROPERTY(bool displayCursorPixelInfo READ getDisplayCursorPixelInfo WRITE setDisplayCursorPixelInfo)
    Q_PROPERTY(bool contrastReversal READ getContrastReversal WRITE setContrastReversal)

    Q_PROPERTY(bool displayButtonBar READ getDisplayButtonBar WRITE setDisplayButtonBar)

    Q_PROPERTY(bool showTime READ getShowTime WRITE setShowTime)

    Q_PROPERTY(QColor vertSliceColor READ getVertSliceMarkupColor WRITE setVertSliceMarkupColor)
    Q_PROPERTY(QColor hozSliceColor READ getHozSliceMarkupColor WRITE setHozSliceMarkupColor)
    Q_PROPERTY(QColor profileColor READ getProfileMarkupColor WRITE setProfileMarkupColor)
    Q_PROPERTY(QColor areaColor READ getAreaMarkupColor WRITE setAreaMarkupColor)
    Q_PROPERTY(QColor beamColor READ getBeamMarkupColor WRITE setBeamMarkupColor)
    Q_PROPERTY(QColor targetColor READ getTargetMarkupColor WRITE setTargetMarkupColor)
    Q_PROPERTY(QColor timeColor READ getTimeMarkupColor WRITE setTimeMarkupColor)


    Q_ENUMS(ResizeOptions)
    Q_PROPERTY(ResizeOptions resizeOption READ getResizeOptionProperty WRITE setResizeOptionProperty)
    enum ResizeOptions { Zoom   = QEImage::RESIZE_OPTION_ZOOM,
                         Fit    = QEImage::RESIZE_OPTION_FIT };
    void setResizeOptionProperty( ResizeOptions resizeOption ){ setResizeOption( (QEImage::resizeOptions)resizeOption ); }
    ResizeOptions getResizeOptionProperty(){ return (ResizeOptions)getResizeOption(); }

    Q_PROPERTY(int zoom READ getZoom WRITE setZoom)


    Q_ENUMS(RotationOptions)
    Q_PROPERTY(RotationOptions rotation READ getRotationProperty WRITE setRotationProperty)
    enum RotationOptions { NoRotation    = QEImage::ROTATION_0,
                           Rotate90Right = QEImage::ROTATION_90_RIGHT,
                           Rotate90Left  = QEImage::ROTATION_90_LEFT,
                           Rotate180     = QEImage::ROTATION_180 };
    void setRotationProperty( RotationOptions rotation ){ setRotation( (QEImage::rotationOptions)rotation ); }
    RotationOptions getRotationProperty(){ return (RotationOptions)getRotation(); }

    Q_PROPERTY(bool verticalFlip READ getVerticalFlip WRITE setVerticalFlip)
    Q_PROPERTY(bool horizontalFlip READ getHorizontalFlip WRITE setHorizontalFlip)

    Q_PROPERTY(int initialHosScrollPos READ getInitialHozScrollPos WRITE setInitialHozScrollPos)
    Q_PROPERTY(int initialVertScrollPos READ getInitialVertScrollPos WRITE setInitialVertScrollPos)


};

#endif // QEIMAGE_H
