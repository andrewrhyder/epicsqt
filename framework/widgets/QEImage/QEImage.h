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
#include <QCaWidget.h>
#include <QCaInteger.h>
#include <videowidget.h>
//#include <QELabel.h>
#include <QVBoxLayout>
#include <QGridLayout>
#include <profilePlot.h>
#include <zoomMenu.h>
#include <flipRotateMenu.h>
#include <selectMenu.h>
#include <QCaVariableNamePropertyManager.h>


#include <QCaPluginLibrary_global.h>
#include <QCaIntegerFormatting.h>


class QCAPLUGINLIBRARYSHARED_EXPORT QEImage : public QFrame, public QCaWidget {
    Q_OBJECT

  public:

    QEImage( QWidget *parent = 0 );
    QEImage( const QString &variableName, QWidget *parent = 0 );
    ~QEImage();

    enum selectOptions{ SO_NONE, SO_PANNING, SO_VSLICE, SO_HSLICE, SO_AREA, SO_PROFILE, SO_TARGET, SO_BEAM };
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
    QCaIntegerFormatting integerFormatting; // Integer formatting options.

    void establishConnection( unsigned int variableIndex );

    enum variableIndexes{ IMAGE_VARIABLE, WIDTH_VARIABLE, HEIGHT_VARIABLE, ROI_X_VARIABLE, ROI_Y_VARIABLE, ROI_W_VARIABLE, ROI_H_VARIABLE, TARGET_X_VARIABLE, TARGET_Y_VARIABLE, BEAM_X_VARIABLE, BEAM_Y_VARIABLE, TARGET_TRIGGER_VARIABLE, CLIPPING_ONOFF_VARIABLE, CLIPPING_LOW_VARIABLE, CLIPPING_HIGH_VARIABLE, QEIMAGE_NUM_VARIABLES };

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

    void vSliceSelectModeClicked();
    void hSliceSelectModeClicked();
    void areaSelectModeClicked();
    void profileSelectModeClicked();
    void targetSelectModeClicked();
    void beamSelectModeClicked();

    void panModeClicked();

    //!! move this functionality into QCaWidget???
    //!! needs one for single variables and one for multiple variables, or just the multiple variable one for all
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )//!! move into Standard Properties section??
    {
        setVariableNameAndSubstitutions(variableNameIn, variableNameSubstitutionsIn, variableIndex);
    }

    void requestEnabled( const bool& state ){ setApplicationEnabled( state ); } //!! move into Standard Properties section??
    void userSelection( imageMarkup::markupIds mode, QPoint point1, QPoint point2 );
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
    void setSelectAreaMode()    { sMenu->setChecked(  QEImage::SO_AREA );    areaSelectModeClicked(); }
    void setSelectProfileMode() { sMenu->setChecked(  QEImage::SO_PROFILE ); profileSelectModeClicked(); }
    void setSelectTargetMode()  { sMenu->setChecked(  QEImage::SO_TARGET );  targetSelectModeClicked(); }
    void setSelectBeamMode()    { sMenu->setChecked(  QEImage::SO_BEAM );    beamSelectModeClicked(); }

    // Slots to allow external operation of control buttons
    void pauseClicked();

    void saveClicked();

    void roiClicked();
    void resetRoiClicked();

    void targetClicked();


  signals:
    void dbValueChanged( const QString& out );
    void requestResend();

  private:
    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );
    void updateToolTip( const QString& tip );


    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

    void setImageBuff();

    QGridLayout *mainLayout;

    QFrame *buttonGroup;

    QScrollArea* scrollArea;
    bool initScrollPosSet;

    VideoWidget* videoWidget;

    QHBoxLayout* infoLayout;
    QLabel* currentCursorPixelLabel;
    QLabel* currentVertPixelLabel;
    QLabel* currentHozPixelLabel;
    QLabel* currentLineLabel;
    QLabel* currentAreaLabel;
    QLabel* currentTargetLabel;
    QLabel* currentBeamLabel;


    QPushButton* pauseButton;
    QPushButton* saveButton;
    QPushButton* roiButton;
    QPushButton* resetRoiButton;
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

    bool enablePan;
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
    QPoint selectedAreaPoint1;
    QPoint selectedAreaPoint2;
    QPoint target;
    QPoint beam;

    bool haveVSliceX;
    bool haveHSliceY;
    bool haveProfileLine;
    bool haveSelectedArea;
    bool haveTarget; //!!! not used??
    bool haveBeam; //!!! not used??

    void generateVSlice( int x );
    void generateHSlice( int y );
    void generateProfile( QPoint point1, QPoint point2 );
    void displaySelectedAreaInfo( QPoint point1, QPoint point2 );

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
    // These properties should be identical for every widget using multiple variables (The number of variables may vary).
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
    Q_PROPERTY(QString imageVariable READ getVariableName0Property WRITE setVariableName0Property)

    VARIABLE_PROPERTY_ACCESS(1)
    Q_PROPERTY(QString widthVariable READ getVariableName1Property WRITE setVariableName1Property)

    VARIABLE_PROPERTY_ACCESS(2)
    Q_PROPERTY(QString heightVariable READ getVariableName2Property WRITE setVariableName2Property)

    VARIABLE_PROPERTY_ACCESS(3)
    Q_PROPERTY(QString regionOfInterestXVariable READ getVariableName3Property WRITE setVariableName3Property)

    VARIABLE_PROPERTY_ACCESS(4)
    Q_PROPERTY(QString regionOfInterestYVariable READ getVariableName4Property WRITE setVariableName4Property)

    VARIABLE_PROPERTY_ACCESS(5)
    Q_PROPERTY(QString regionOfInterestWVariable READ getVariableName5Property WRITE setVariableName5Property)

    VARIABLE_PROPERTY_ACCESS(6)
    Q_PROPERTY(QString regionOfInterestHVariable READ getVariableName6Property WRITE setVariableName6Property)

    VARIABLE_PROPERTY_ACCESS(7)
    Q_PROPERTY(QString targetXVariable READ getVariableName7Property WRITE setVariableName7Property)

    VARIABLE_PROPERTY_ACCESS(8)
    Q_PROPERTY(QString targetYVariable READ getVariableName8Property WRITE setVariableName8Property)

    VARIABLE_PROPERTY_ACCESS(9)
    Q_PROPERTY(QString beamXVariable READ getVariableName9Property WRITE setVariableName9Property)

    VARIABLE_PROPERTY_ACCESS(10)
    Q_PROPERTY(QString beamYVariable READ getVariableName10Property WRITE setVariableName10Property)

    VARIABLE_PROPERTY_ACCESS(11)
    Q_PROPERTY(QString targetTriggerVariable READ getVariableName11Property WRITE setVariableName11Property)

    VARIABLE_PROPERTY_ACCESS(12)
    Q_PROPERTY(QString clippingOnOffVariable READ getVariableName12Property WRITE setVariableName12Property)

    VARIABLE_PROPERTY_ACCESS(13)
    Q_PROPERTY(QString clippingLowVariable READ getVariableName13Property WRITE setVariableName13Property)

    VARIABLE_PROPERTY_ACCESS(14)
    Q_PROPERTY(QString clippingHighVariable READ getVariableName14Property WRITE setVariableName14Property)


    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)
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
    Q_PROPERTY(QString userLevelUserStyle READ getStyleUser WRITE setStyleUser)
    Q_PROPERTY(QString userLevelScientistStyle READ getStyleScientist WRITE setStyleScientist)
    Q_PROPERTY(QString userLevelEngineerStyle READ getStyleEngineer WRITE setStyleEngineer)
    enum UserLevels { User      = USERLEVEL_USER,
                      Scientist = USERLEVEL_SCIENTIST,
                      Engineer  = USERLEVEL_ENGINEER };
    UserLevels getUserLevelVisibilityProperty() { return (UserLevels)getUserLevelVisibility(); }
    void setUserLevelVisibilityProperty( UserLevels level ) { setUserLevelVisibility( (userLevels)level ); }

    UserLevels getUserLevelEnabledProperty() { return (UserLevels)getUserLevelEnabled(); }
    void setUserLevelEnabledProperty( UserLevels level ) { setUserLevelEnabled( (userLevels)level ); }
    Q_ENUMS(UserLevels)
    Q_PROPERTY(UserLevels userLevelVisibility READ getUserLevelVisibilityProperty WRITE setUserLevelVisibilityProperty)
    Q_PROPERTY(UserLevels userLevelEnabled READ getUserLevelEnabledProperty WRITE setUserLevelEnabledProperty)
    //=================================================================================

//==========================================================================
// Widget specific properties
public:

    /// Format options (8 bit grey scale, 32 bit color, etc)
    Q_ENUMS(FormatOptions)
    Q_PROPERTY(FormatOptions formatOption READ getFormatOptionProperty WRITE setFormatOptionProperty)
    enum FormatOptions { Grey_8   = QEImage::GREY8,
                         Grey_12  = QEImage::GREY12,
                         Grey_16  = QEImage::GREY16,
                         RGB      = QEImage::RGB_888 };
    void setFormatOptionProperty( FormatOptions formatOption ){ setFormatOption( (QEImage::formatOptions)formatOption ); }
    FormatOptions getFormatOptionProperty(){ return (FormatOptions)getFormatOption(); }

    Q_PROPERTY(bool enablePan READ getEnablePan WRITE setEnablePan)

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
