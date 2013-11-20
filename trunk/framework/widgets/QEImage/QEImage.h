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
#include <QVBoxLayout>
#include <QGridLayout>
#include <profilePlot.h>
#include <zoomMenu.h>
#include <flipRotateMenu.h>
#include <selectMenu.h>
#include <QEImageOptionsDialog.h>
#include <QCaVariableNamePropertyManager.h>
#include <imageInfo.h>
#include <brightnessContrast.h>

#include <QEPluginLibrary_global.h>
#include <QEIntegerFormatting.h>

// Class to keep track of a rectangular area such as region of interest or profile line information
// As data arrives, this class is used to record it.
class areaInfo
{
    public:
        // Construction
        areaInfo() { haveX1 = false; haveY1 = false; haveX2 = false; haveY2 = false; }

        // Set elements
        void setX1( long x ) { p1.setX( x ); haveX1 = true; }
        void setY1( long y ) { p1.setY( y ); haveY1 = true; }
        void setX2( long x ) { p2.setX( x ); haveX2 = true; }
        void setY2( long y ) { p2.setY( y ); haveY2 = true; }

        void setX( long x ) { int w = p2.x()-p1.x(); p1.setX( x ); p2.setX( x+w ); haveX1 = true; }
        void setY( long y ) { int h = p2.y()-p1.y(); p1.setY( y ); p2.setY( y+h ); haveY1 = true; }
        void setW( long w ) { p2.setX( p1.x()+w ); haveX2 = true; }
        void setH( long h ) { p2.setY( p1.y()+h ); haveY2 = true; }

        void setPoint1( QPoint p1In ) { p1 = p1In; haveX1 = true; haveY1 = true; }
        void setPoint2( QPoint p2In ) { p2 = p2In; haveX2 = true; haveY2 = true; }

        // Clear elements (invalid data)
        void clearX1() { haveX1 = false; }
        void clearY1() { haveY1 = false; }
        void clearX2() { haveX2 = false; }
        void clearY2() { haveY2 = false; }

        void clearX() { clearX1(); }
        void clearY() { clearY1(); }
        void clearW() { clearX2(); }
        void clearH() { clearY2(); }

        // Get ROI info
        bool getStatus() { return haveX1 && haveY1 && haveX2 && haveY2; }
        QRect getArea() { return QRect( p1, p2 ); }
        QPoint getPoint1() { return p1; }
        QPoint getPoint2() { return p2; }

    private:
        QPoint p1;
        QPoint p2;
        bool haveX1;
        bool haveY1;
        bool haveX2;
        bool haveY2;
};


class QEPLUGINLIBRARYSHARED_EXPORT QEImage : public QFrame, public QEWidget, public imageInfo{
    Q_OBJECT

  public:

    /// Create without a variable.
    /// Use setVariableName'n'Property() - where 'n' is a number from 0 to 32 - and setSubstitutionsProperty() to define variables and, optionally, macro substitutions later.
    /// Note, each variable property is named by function (such as imageVariable and widthVariable) but given
    /// a numeric get and set property access function such as setVariableName22Property(). Refer to the
    /// property definitions to determine what 'set' and 'get' function is used for each varible, or use Qt library functions to set or get the variable names by name.
    QEImage( QWidget *parent = 0 );

    /// Create with a variable.
    /// A connection is automatically established.
    /// The variable is set up as the first variable.
    /// This is consistant with other widgets, but will not result in an updating image as the width and height variables are required as a minimum.
    QEImage( const QString &variableName, QWidget *parent = 0 );

    /// Destructor
    ~QEImage();

public:
    ///  \enum selectOptions.
    /// Internal use only. Selection options. What will happen when the user interacts with the image area
    enum selectOptions{ SO_NONE,                                ///< Do nothing
                        SO_PANNING,                             ///< User is panning
                        SO_VSLICE,                              ///< Select the vertical slice point
                        SO_HSLICE,                              ///< Select the horizontal slice point
                        SO_AREA1, SO_AREA2, SO_AREA3, SO_AREA4, ///< User is selecting an area (for region of interest)
                        SO_PROFILE,                             ///< Select an arbitrary line across the image (to determine a profile)
                        SO_TARGET,                              ///< Mark the target point
                        SO_BEAM                                 ///< Mark the current beam location
                       };

    selectOptions getSelectionOption();    // Get the current selection option

public:
    // Property convenience functions

    // Video format options
    /// \enum formatOptions
    /// Video format options
    enum formatOptions{ GREY8,          ///< 8 bit grey scale
                        GREY12,         ///< 12 bit grey scale
                        GREY16,         ///< 16 bit grey scale
                        RGB_888,         ///< 24 bit RGB
                        NUM_OPTIONS     // Must be last
                      };

    void setFormatOption( formatOptions formatOption );                 ///< Access function for #formatOption property - refer to #formatOption property for details
    formatOptions getFormatOption();                                    ///< Access function for #formatOption property - refer to #formatOption property for details

    // Size options
    /// \enum resizeOptions
    /// Image resize options
    enum resizeOptions { RESIZE_OPTION_ZOOM,    ///< Zoom to selected percentage
                         RESIZE_OPTION_FIT      ///< Zoom to fit the current window size
                       };
    void setResizeOption( resizeOptions resizeOptionIn );               ///< Access function for #resizeOption property - refer to #resizeOption property for details
    resizeOptions getResizeOption();                                    ///< Access function for #resizeOption property - refer to #resizeOption property for details

    // Zoom
    void setZoom( int zoomIn );                                         ///< Access function for #zoom property - refer to #zoom property for details
    int getZoom();                                                      ///< Access function for #zoom property - refer to #zoom property for details

    // Rotation
    /// \enum rotationOptions
    /// Image rotation options
    enum rotationOptions { ROTATION_0,          ///< No image rotation
                           ROTATION_90_RIGHT,   ///< Rotate image 90 degrees clockwise
                           ROTATION_90_LEFT,    ///< Rotate image 90 degrees anticlockwise
                           ROTATION_180         ///< Rotate image 180 degrees
                         };
    void setRotation( rotationOptions rotationIn );                     ///< Access function for #rotation property - refer to #rotation property for details
    rotationOptions getRotation();                                      ///< Access function for #rotation property - refer to #rotation property for details

    void setHorizontalFlip( bool flipHozIn );                           ///< Access function for #horizontalFlip property - refer to #horizontalFlip property for details
    bool getHorizontalFlip();                                           ///< Access function for #horizontalFlip property - refer to #horizontalFlip property for details

    void setVerticalFlip( bool flipVertIn );                            ///< Access function for #verticalFlip property - refer to #verticalFlip property for details
    bool getVerticalFlip();                                             ///< Access function for #verticalFlip property - refer to #verticalFlip property for details

    void setInitialHozScrollPos( int initialHosScrollPosIn );           ///< Access function for #initialHosScrollPos property - refer to #initialHosScrollPos property for details
    int getInitialHozScrollPos();                                       ///< Access function for #initialHosScrollPos property - refer to #initialHosScrollPos property for details

    void setInitialVertScrollPos( int initialVertScrollPosIn );         ///< Access function for #initialVertScrollPos property - refer to #initialVertScrollPos property for details
    int getInitialVertScrollPos();                                      ///< Access function for #initialVertScrollPos property - refer to #initialVertScrollPos property for details

    void setDisplayButtonBar( bool displayButtonBarIn );                ///< Access function for #displayButtonBar property - refer to #displayButtonBar property for details
    bool getDisplayButtonBar();                                         ///< Access function for #displayButtonBar property - refer to #displayButtonBar property for details

    void setShowTime(bool pValue);                                      ///< Access function for #showTime property - refer to #showTime property for details
    bool getShowTime();                                                 ///< Access function for #showTime property - refer to #showTime property for details

    void setVertSliceMarkupColor(QColor pValue);                        ///< Access function for #vertSliceColor property - refer to #vertSliceColor property for details
    QColor getVertSliceMarkupColor();                                   ///< Access function for #vertSliceColor property - refer to #vertSliceColor property for details

    void setHozSliceMarkupColor(QColor pValue);                         ///< Access function for #hozSliceColor property - refer to #hozSliceColor property for details
    QColor getHozSliceMarkupColor();                                    ///< Access function for #hozSliceColor property - refer to #hozSliceColor property for details

    void setProfileMarkupColor(QColor pValue);                          ///< Access function for #profileColor property - refer to #profileColor property for details
    QColor getProfileMarkupColor();                                     ///< Access function for #profileColor property - refer to #profileColor property for details

    void setAreaMarkupColor(QColor pValue);                             ///< Access function for #areaColor property - refer to #areaColor property for details
    QColor getAreaMarkupColor();                                        ///< Access function for #areaColor property - refer to #areaColor property for details

    void setTargetMarkupColor(QColor pValue);                           ///< Access function for #targetColor property - refer to #targetColor property for details
    QColor getTargetMarkupColor();                                      ///< Access function for #targetColor property - refer to #targetColor property for details

    void setBeamMarkupColor(QColor pValue);                             ///< Access function for #beamColor property - refer to #beamColor property for details
    QColor getBeamMarkupColor();                                        ///< Access function for #beamColor property - refer to #beamColor property for details

    void setTimeMarkupColor(QColor pValue);                             ///< Access function for #timeColor property - refer to #timeColor property for details
    QColor getTimeMarkupColor();                                        ///< Access function for #timeColor property - refer to #timeColor property for details

    void setDisplayCursorPixelInfo( bool displayCursorPixelInfo );      ///< Access function for #displayCursorPixelInfo property - refer to #displayCursorPixelInfo property for details
    bool getDisplayCursorPixelInfo();                                   ///< Access function for #displayCursorPixelInfo property - refer to #displayCursorPixelInfo property for details

    void setContrastReversal( bool contrastReversalIn );                ///< Access function for #contrastReversal property - refer to #contrastReversal property for details
    bool getContrastReversal();                                         ///< Access function for #contrastReversal property - refer to #contrastReversal property for details

    void setEnableVertSliceSelection( bool enableVSliceSelection );     ///< Access function for #enableVertSliceSelection property - refer to #enableVertSliceSelection property for details
    bool getEnableVertSliceSelection();                                 ///< Access function for #enableVertSliceSelection property - refer to #enableVertSliceSelection property for details

    void setEnableHozSliceSelection( bool enableHSliceSelection );      ///< Access function for #enableHozSliceSelection property - refer to #enableHozSliceSelection property for details
    bool getEnableHozSliceSelection();                                  ///< Access function for #enableHozSliceSelection property - refer to #enableHozSliceSelection property for details

    void setEnableAreaSelection( bool enableAreaSelectionIn );          ///< Access function for #enableAreaSelection property - refer to #enableAreaSelection property for details
    bool getEnableAreaSelection();                                      ///< Access function for #enableAreaSelection property - refer to #enableAreaSelection property for details

    void setEnableProfileSelection( bool enableProfileSelectionIn );    ///< Access function for #enableProfileSelection property - refer to #enableProfileSelection property for details
    bool getEnableProfileSelection();                                   ///< Access function for #enableProfileSelection property - refer to #enableProfileSelection property for details

    void setEnableTargetSelection( bool enableTargetSelectionIn );      ///< Access function for #enableTargetSelection property - refer to #enableTargetSelection property for details
    bool getEnableTargetSelection();                                    ///< Access function for #enableTargetSelection property - refer to #enableTargetSelection property for details

    void setEnableBrightnessContrast( bool enableBrightnessContrastIn );///< Access function for #enableBrightnessContrast property - refer to #enableBrightnessContrast property for details
    bool getEnableBrightnessContrast();                                 ///< Access function for #enableBrightnessContrast property - refer to #enableBrightnessContrast property for details

    void setAutoBrightnessContrast( bool autoBrightnessContrastIn );    ///< Access function for #autoBrightnessContrast property - refer to #autoBrightnessContrast property for details
    bool getAutoBrightnessContrast();                                   ///< Access function for #autoBrightnessContrast property - refer to #autoBrightnessContrast property for details

    void setExternalControls( bool externalControlsIn );                ///< Access function for #externalControls property - refer to #externalControls property for details
    bool getExternalControls();                                         ///< Access function for #externalControls property - refer to #externalControls property for details

    void setFullContextMenu( bool fullContextMenuIn );                  ///< Access function for #fullContextMenu property - refer to #fullContextMenu property for details
    bool getFullContextMenu();                                          ///< Access function for #fullContextMenu property - refer to #fullContextMenu property for details

    void setEnableProfilePresentation( bool enableProfilePresentationIn );     ///< Access function for #fullContextMenu property - refer to #enableProfilePresentation property for details
    bool getEnableProfilePresentation();                                       ///< Access function for #fullContextMenu property - refer to #enableProfilePresentation property for details

    void setEnableHozSlicePresentation( bool enableHozSlicePresentationIn );   ///< Access function for #fullContextMenu property - refer to #enableHozSlicePresentation property for details
    bool getEnableHozSlicePresentation();                                      ///< Access function for #fullContextMenu property - refer to #enableHozSlicePresentation property for details

    void setEnableVertSlicePresentation( bool enableVertSlicePresentationIn ); ///< Access function for #fullContextMenu property - refer to #enableVertSlicePresentation property for details
    bool getEnableVertSlicePresentation();                                     ///< Access function for #fullContextMenu property - refer to #enableVertSlicePresentation property for details

  protected:
    QEIntegerFormatting integerFormatting; // Integer formatting options.

    void establishConnection( unsigned int variableIndex );

    // Index for access to registered variables
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
                          PROFILE_H_VARIABLE, PROFILE_V_VARIABLE,
                          LINE_PROFILE_X1_VARIABLE, LINE_PROFILE_Y1_VARIABLE, LINE_PROFILE_X2_VARIABLE, LINE_PROFILE_Y2_VARIABLE,
                          QEIMAGE_NUM_VARIABLES /*Must be last*/ };

    resizeOptions resizeOption; // Resize option. (zoom or fit)
    int zoom;                   // Zoom percentage
    rotationOptions rotation;   // Rotation option
    bool flipVert;              // True if vertical flip option set
    bool flipHoz;               // True if horizontal flip option set

    int initialHozScrollPos;    // Initial horizontal scroll bar position (for when starting zoomed)
    int initialVertScrollPos;   // Initial vertical scroll bar position (for when starting zoomed)

    bool displayButtonBar;      // True if button bar should be displayed

private slots:
    // QCa data update slots
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setImage( const QByteArray& image, unsigned long dataSize, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );
    void setDimension( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex);
    void setClipping( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex);
    void setROI( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex);
    void setProfile( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex);

    // Menu choice slots
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

    void brightnessContrastChanged();
    void brightnessContrastAutoImageRequest();
    void brightnessContrastReversalRequest( bool state );

    // !! move this functionality into QEWidget???
    // !! needs one for single variables and one for multiple variables, or just the multiple variable one for all
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )// !! move into Standard Properties section??
    {
        setVariableNameAndSubstitutions(variableNameIn, variableNameSubstitutionsIn, variableIndex);
    }

    void userSelection( imageMarkup::markupIds mode, bool complete, bool clearing, QPoint point1, QPoint point2, unsigned int thickness );
    void zoomInOut( int zoomAmount );
    void currentPixelInfo( QPoint pos );
    void pan( QPoint pos );
    void showImageContextMenu( const QPoint& );
    void selectMenuTriggered( QAction* selectedItem );
    void zoomMenuTriggered( QAction* selectedItem );
    void flipRotateMenuTriggered( QAction* selectedItem );
    void showImageAboutDialog();
    void optionAction( imageContextMenu::imageContextMenuOptions option, bool checked );

public slots:
    void setImageFile( QString name );
    // Slots to allow external setting of selection menu options
    void setSelectPanMode()     { sMenu->setChecked(  QEImage::SO_PANNING ); panModeClicked(); }            ///< Framework use only. Slot to allow external setting of selection menu options
    void setSelectVSliceMode()  { sMenu->setChecked(  QEImage::SO_VSLICE );  vSliceSelectModeClicked(); }   ///< Framework use only. Slot to allow external setting of selection menu options
    void setSelectHSliceMode()  { sMenu->setChecked(  QEImage::SO_HSLICE );  hSliceSelectModeClicked(); }   ///< Framework use only. Slot to allow external setting of selection menu options
    void setSelectArea1Mode()   { sMenu->setChecked(  QEImage::SO_AREA1 );   area1SelectModeClicked(); }    ///< Framework use only. Slot to allow external setting of selection menu options
    void setSelectArea2Mode()   { sMenu->setChecked(  QEImage::SO_AREA2 );   area2SelectModeClicked(); }    ///< Framework use only. Slot to allow external setting of selection menu options
    void setSelectArea3Mode()   { sMenu->setChecked(  QEImage::SO_AREA3 );   area3SelectModeClicked(); }    ///< Framework use only. Slot to allow external setting of selection menu options
    void setSelectArea4Mode()   { sMenu->setChecked(  QEImage::SO_AREA4 );   area4SelectModeClicked(); }    ///< Framework use only. Slot to allow external setting of selection menu options
    void setSelectProfileMode() { sMenu->setChecked(  QEImage::SO_PROFILE ); profileSelectModeClicked(); }  ///< Framework use only. Slot to allow external setting of selection menu options
    void setSelectTargetMode()  { sMenu->setChecked(  QEImage::SO_TARGET );  targetSelectModeClicked(); }   ///< Framework use only. Slot to allow external setting of selection menu options
    void setSelectBeamMode()    { sMenu->setChecked(  QEImage::SO_BEAM );    beamSelectModeClicked(); }     ///< Framework use only. Slot to allow external setting of selection menu options

    // Slots to allow external operation of control buttons
    void pauseClicked();           ///< Framework use only. Slot to allow external setting of selection menu options

    void saveClicked();            ///< Framework use only. Slot to allow external setting of selection menu options

    void targetClicked();      ///< Framework use only. Slot to allow external setting of selection menu options

    void localBCDestroyed( QObject* ); ///< Framework use only. Slot to catch deletion of components (such as profile plots) that have been passed to the application for presentation
    void vSliceDisplayDestroyed( QObject* ); ///< Framework use only. Slot to catch deletion of components (such as profile plots) that have been passed to the application for presentation
    void hSliceDisplayDestroyed( QObject* ); ///< Framework use only. Slot to catch deletion of components (such as profile plots) that have been passed to the application for presentation
    void profileDisplayDestroyed( QObject* ); ///< Framework use only. Slot to catch deletion of components (such as profile plots) that have been passed to the application for presentation

  signals:
    // Note, the following signals are common to many QE widgets,
    // if changing the doxygen comments, ensure relevent changes are migrated to all instances
    /// Sent when the widget is updated following a data change
    /// Can be used to pass on EPICS data (as presented in this widget) to other widgets.
    /// For example a QList widget could log updates from this widget.
    void dbValueChanged( const QString& out );
    /// Internal use only. Used when changing a property value to force a re-display to reflect the new property value.
    void requestResend();

    void componentHostRequest( const QEActionRequests& request );


  private:
    void roi1Changed();        ///< Framework use only. Slot to allow external setting of selection menu options
    void roi2Changed();        ///< Framework use only. Slot to allow external setting of selection menu options
    void roi3Changed();        ///< Framework use only. Slot to allow external setting of selection menu options
    void roi4Changed();        ///< Framework use only. Slot to allow external setting of selection menu options

    void lineProfileChanged();  ///< Framework use only. Slot to allow external setting of selection menu options
    void hozProfileChanged();   ///< Framework use only. Slot to allow external setting of selection menu options
    void vertProfileChanged();  ///< Framework use only. Slot to allow external setting of selection menu options


    void actionRequest( QString action, QStringList arguments, bool initialise, QAction* originator ); // Perform a named action


    void emitComponentHostRequest( const QEActionRequests& request ){ emit componentHostRequest( request ); }

    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );

    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

    void setImageBuff();

    QGridLayout* mainLayout;
    QGridLayout* graphicsLayout;

    QFrame *buttonGroup;
    QToolBar* toolBar;

    QScrollArea* scrollArea;
    bool initScrollPosSet;

    VideoWidget* videoWidget;

    bool appHostsControls;
    bool hostingAppAvailable;

    bool fullContextMenu;

    void presentControls();

    // Button widgets
    QPushButton* pauseButton;
    QPushButton* saveButton;
    QPushButton* targetButton;
    QPushButton* zoomButton;
    QPushButton* selectModeButton;
    QPushButton* flipRotateButton;

    // External actions optionally provided by the application creating this widget
    QAction* pauseExternalAction;

    // Profile graphic widgets
    QLabel* vSliceLabel;
    QLabel* hSliceLabel;
    QLabel* profileLabel;
    profilePlot* vSliceDisplay;
    profilePlot* hSliceDisplay;
    profilePlot* profileDisplay;

    localBrightnessContrast* localBC;

    // Menus
    zoomMenu*       zMenu;
    flipRotateMenu* frMenu;
    selectMenu*     sMenu;

    QEImageOptionsDialog* optionsDialog;

    // Presentation
    bool paused;

    bool enableHozSlicePresentation;
    bool enableVertSlicePresentation;
    bool enableProfilePresentation;

    // Options
    formatOptions formatOption;

    // Image and related information
    QCaDateTime imageTime;
    unsigned long imageDataSize;    // Size of elements in image data (originating from CA data type
    unsigned long elementsPerPixel; // Number of data elements per pixel.
    unsigned long bytesPerPixel;    // Bytes in input data per pixel (imageDataSize * elementsPerPixel)
    QByteArray image;       // Buffer to hold original image data. WARNING To avoid expensive memory copies, data is generated using QByteArray::fromRawData(), where the raw data is the original CA update data. This data is held until a subsequent update. The data in this QByteArray will be valid only until the next update arrives.
    QByteArray imageBuff;   // Buffer to hold data converted to format for generating QImage.
#define IMAGEBUFF_BYTES_PER_PIXEL 4   // 4 bytes for Format_RGB32
    unsigned long imageBuffWidth;   // Original image width
    unsigned long imageBuffHeight;  // Original image height

    // Region of interest information
    areaInfo roiInfo[4];

    // User selected information
    int vSliceX;
    unsigned int vSliceThickness;

    int hSliceY;
    unsigned int hSliceThickness;

    areaInfo lineProfileInfo;
    QPoint profileLineStart;
    QPoint profileLineEnd;
    unsigned int profileThickness;

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


    // Private methods
    void generateVSlice( int x, unsigned int thickness );                           // Generate a profile along a line down an image at a given X position
    void generateHSlice( int y, unsigned int thickness );                           // Generate a profile along a line across an image at a given Y position
    void generateProfile( QPoint point1, QPoint point2, unsigned int thickness );   // Generate a profile along an arbitrary line through an image.
    void displaySelectedAreaInfo( const int region, const QPoint point1, const QPoint point2 );  // Display textual info about a selected area

    void updateMarkupData();                                               // Update markups if required. (For example, after image update)

    int getPixelValueFromData( const unsigned char* ptr );              // Return a floating point number given a pointer into an image data buffer.
    double getFloatingPixelValueFromData( const unsigned char* ptr );   // Return a floating point number given a pointer to a value of an arbitrary size in a char* buffer.

    unsigned int rotatedImageBuffWidth();                   // Return the image width following any rotation
    unsigned int rotatedImageBuffHeight();                  // Return the image height following any rotation

    void displayImage();                                    // Display a new image.
    int getScanOption();                                    // Determine the way the input pixel data must be scanned to accommodate the required rotate and flip options.
    QPoint rotateFlipPoint( QPoint& pos );                  // Transform the point according to current rotation and flip options.
    QRect rotateFlipRectangle( QPoint& pos1, QPoint& pos2 );// Transform the rectangle according to current rotation and flip options

    const unsigned char* getImageDataPtr( QPoint& pos );    // Return a pointer to pixel data in the original image data.

    void zoomToArea();                                      // Zoom to the area selected on the image
    void setResizeOptionAndZoom( int zoomIn );              // Set the zoom percentage (and force zoom mode)

    double maxPixelValue();                                 // Determine the maximum pixel value for the current format

    // Data generated from pixel profiles
    QVector<QPointF> vSliceData;
    QVector<QPointF> hSliceData;
    QVector<QPointF> profileData;

    // Icons
    QIcon* pauseButtonIcon;
    QIcon* playButtonIcon;

    // Clipping info (determined from cliping variable data)
    bool clippingOn;
    unsigned int clippingLow;
    unsigned int clippingHigh;

    struct rgbPixel
    {
        unsigned char p[4]; // Alpha/R/G/B
    };
    const rgbPixel* getPixelTranslation();    // Get a table of translated pixel values (from pixelLookup) creating it first if required

    bool pixelLookupValid;  // pixelLookup is valid. It is invalid if anything that affects the translation changes, such as pixel format, local brigtness, etc

    QByteArray pixelLookup; // Table of translated pixel values (includig contrast reversal, local brightness and contrast, and clipping)

    void setRegionAutoBrightnessContrast( QPoint point1, QPoint point2 );    // Update the brightness and contrast, if in auto, to match the recently selected region
    void getPixelRange( const QRect& area, unsigned int* min, unsigned int* max ); // Determine the range of pixel values an area of the image

    void doEnableBrightnessContrast( bool enableBrightnessContrast );
    void doContrastReversal( bool contrastReversal );
    void doEnableVertSliceSelection( bool enableVSliceSelection );
    void doEnableHozSliceSelection( bool enableHSliceSelection );
    void doEnableAreaSelection( bool enableAreaSelection );
    void doEnableProfileSelection( bool enableProfileSelection );
    void doEnableTargetSelection( bool enableTargetSelection );

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

    VARIABLE_PROPERTY_ACCESS(27)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the areadetector horizontal profile.
    Q_PROPERTY(QString profileHozVariable READ getVariableName27Property WRITE setVariableName27Property)

    VARIABLE_PROPERTY_ACCESS(28)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the areadetector vertical profile.
    Q_PROPERTY(QString profileVertVariable READ getVariableName28Property WRITE setVariableName28Property)

    VARIABLE_PROPERTY_ACCESS(29)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the areadetector arbitrary line profile start X.
    Q_PROPERTY(QString lineProfileX1Variable READ getVariableName29Property WRITE setVariableName29Property)

    VARIABLE_PROPERTY_ACCESS(30)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the areadetector arbitrary line profile start Y.
    Q_PROPERTY(QString lineProfileY1Variable READ getVariableName30Property WRITE setVariableName30Property)

    VARIABLE_PROPERTY_ACCESS(31)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the areadetector arbitrary line profile end X.
    Q_PROPERTY(QString lineProfileX2Variable READ getVariableName31Property WRITE setVariableName31Property)

    VARIABLE_PROPERTY_ACCESS(32)
    /// EPICS variable name (CA PV).
    /// This variable is used to write the areadetector arbitrary line profile end Y.
    Q_PROPERTY(QString lineProfileY2Variable READ getVariableName32Property WRITE setVariableName32Property)

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
    enum UserLevels { User      = userLevelTypes::USERLEVEL_USER,          ///< Refer to USERLEVEL_USER for details
                      Scientist = userLevelTypes::USERLEVEL_SCIENTIST,     ///< Refer to USERLEVEL_SCIENTIST for details
                      Engineer  = userLevelTypes::USERLEVEL_ENGINEER       ///< Refer to USERLEVEL_ENGINEER for details
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
    void setUserLevelVisibilityProperty( UserLevels level ) { setUserLevelVisibility( (userLevelTypes::userLevels)level ); }///< Access function for #userLevelVisibility property - refer to #userLevelVisibility property for details
    UserLevels getUserLevelEnabledProperty() { return (UserLevels)getUserLevelEnabled(); }                  ///< Access function for #userLevelEnabled property - refer to #userLevelEnabled property for details
    void setUserLevelEnabledProperty( UserLevels level ) { setUserLevelEnabled( (userLevelTypes::userLevels)level ); }      ///< Access function for #userLevelEnabled property - refer to #userLevelEnabled property for details

    /// If set (default) widget will indicate the alarm state of any variable data is displaying.
    /// Typically the background colour is set to indicate the alarm state.
    /// Note, this property is included in the set of standard properties as it applies to most widgets. It
    /// will do nothing for widgets that don't display data.
    Q_PROPERTY(bool displayAlarmState READ getDisplayAlarmState WRITE setDisplayAlarmState)

public:
    // END-STANDARD-PROPERTIES ========================================================

//==========================================================================
// Widget specific properties
public:

    // Format options (8 bit grey scale, 32 bit color, etc)
    Q_ENUMS(FormatOptions)
    /// Video format.
    /// EPICS data type size will typically be adequate for the number of bits required (one byte for 8 bits, 2 bytes for 12 and 16 bits), but can be larger (4 bytes for 24 bits.)
    Q_PROPERTY(FormatOptions formatOption READ getFormatOptionProperty WRITE setFormatOptionProperty)

    /// \enum FormatOptions
    /// User friendly enumerations for #formatOption property - refer to #formatOption property and #formatOptions enumeration for details.
    enum FormatOptions { Grey_8   = QEImage::GREY8,     ///< 8 bit grey scale
                         Grey_12  = QEImage::GREY12,    ///< 12 bit grey scale
                         Grey_16  = QEImage::GREY16,    ///< 16 bit grey scale
                         RGB      = QEImage::RGB_888 }; ///< 24 bit RGB

    void setFormatOptionProperty( FormatOptions formatOption ){ setFormatOption( (QEImage::formatOptions)formatOption ); }  ///< Access function for #formatOption property - refer to #formatOption property for details
    FormatOptions getFormatOptionProperty(){ return (FormatOptions)getFormatOption(); }                                     ///< Access function for #formatOption property - refer to #formatOption property for details

    /// If true, the option to select a vertical slice through the image will be available to the user.
    /// This will be used to generate a horizontal pixel profile, and write the position of the slice to the optional variable specified by the #profileVertVariable property.
    /// The profile will only be presented to the user if #enableVertSlicePresentation property is true.
    Q_PROPERTY(bool enableVertSliceSelection READ getEnableVertSliceSelection WRITE setEnableVertSliceSelection)

    /// If true, the option to select a horizontal slice through the image will be available to the user.
    /// This will be used to generate a horizontal pixel profile, and write the position of the slice to the optional variable specified by the #profileHozVariable property.
    /// The profile will only be presented to the user if #enableHozSlicePresentation property is true.
    Q_PROPERTY(bool enableHozSliceSelection READ getEnableHozSliceSelection WRITE setEnableHozSliceSelection)

    /// If true, the option to select an arbitrary line through any part of the image will be available to the user.
    /// This will be used to generate a pixel profile.
    Q_PROPERTY(bool enableProfileSelection READ getEnableProfileSelection WRITE setEnableProfileSelection)

    //=========

    /// If true, the horizontal pixel profile plot will be presented to the user when a horizontal slice is selected.
    /// If false, the profile plot will not be presented to the user. False is used when a variable has been specified
    /// in the #profileHozVariable property and the presentation or use of the slice information is being managed elsewhere.
    Q_PROPERTY(bool enableHozSlicePresentation READ getEnableHozSlicePresentation WRITE setEnableHozSlicePresentation)

    /// If true, the vertical pixel profile plot will be presented to the user when a vertical slice is selected.
    /// If false, the profile plot will not be presented to the user. False is used when a variable has been specified
    /// in the #profileVertVariable property and the presentation or use of the slice information is being managed elsewhere.
    Q_PROPERTY(bool enableVertSlicePresentation READ getEnableVertSlicePresentation WRITE setEnableVertSlicePresentation)

    /// If true, the vertical pixel profile plot will be presented to the user when a vertical slice is selected.
    /// If false, the profile plot will not be presented to the user. False is used when a variable has been specified
    /// in the #profileVertVariable property and the presentation or use of the slice information is being managed elsewhere.
    Q_PROPERTY(bool enableProfilePresentation READ getEnableProfilePresentation WRITE setEnableProfilePresentation)

    //=========

    /// If true, the user will be able to select areas. These are used for selection of Region of Interests,
    /// and for zooming to a particular area
    Q_PROPERTY(bool enableAreaSelection READ getEnableAreaSelection WRITE setEnableAreaSelection)

    /// If true, the user will be able to select points on the image to mark a target position and the
    /// current beam position. This can be used for automatic beam positioning.
    Q_PROPERTY(bool enableTargetSelection READ getEnableTargetSelection WRITE setEnableTargetSelection)

    /// If true, an area will be presented under the image with textual information about the pixel under
    /// the cursor, and for other selections such as selected areas.
    Q_PROPERTY(bool displayCursorPixelInfo READ getDisplayCursorPixelInfo WRITE setDisplayCursorPixelInfo)

    /// If true, the image will undergo contrast reversal.
    ///
    Q_PROPERTY(bool contrastReversal READ getContrastReversal WRITE setContrastReversal)

    /// If true, a button bar will be displayed above the image.
    /// If not displayed, all buttons in the button bar are still available in the right click menu.
    Q_PROPERTY(bool displayButtonBar READ getDisplayButtonBar WRITE setDisplayButtonBar)

    /// If true, the image timestamp will be written in the top left of the image.
    ///
    Q_PROPERTY(bool showTime READ getShowTime WRITE setShowTime)

    /// Used to select the color of the vertical slice markup.
    ///
    Q_PROPERTY(QColor vertSliceColor READ getVertSliceMarkupColor WRITE setVertSliceMarkupColor)

    /// Used to select the color of the horizontal slice markup.
    ///
    Q_PROPERTY(QColor hozSliceColor READ getHozSliceMarkupColor WRITE setHozSliceMarkupColor)

    /// Used to select the color of the arbitrarty profile line markup.
    ///
    Q_PROPERTY(QColor profileColor READ getProfileMarkupColor WRITE setProfileMarkupColor)

    /// Used to select the color of the area selection markups.
    ///
    Q_PROPERTY(QColor areaColor READ getAreaMarkupColor WRITE setAreaMarkupColor)

    /// Used to select the color of the beam marker.
    ///
    Q_PROPERTY(QColor beamColor READ getBeamMarkupColor WRITE setBeamMarkupColor)

    /// Used to select the color of the target marker.
    ///
    Q_PROPERTY(QColor targetColor READ getTargetMarkupColor WRITE setTargetMarkupColor)

    /// Used to select the color of the timestamp.
    ///
    Q_PROPERTY(QColor timeColor READ getTimeMarkupColor WRITE setTimeMarkupColor)


    Q_ENUMS(ResizeOptions)
    /// Resize option. Zoom to zoom to the percentage given by the #zoom property, or fit to the window size.
    ///
    Q_PROPERTY(ResizeOptions resizeOption READ getResizeOptionProperty WRITE setResizeOptionProperty)
    /// \enum ResizeOptions
    /// User friendly enumerations for #resizeOption property
    enum ResizeOptions { Zoom   = QEImage::RESIZE_OPTION_ZOOM,  ///< Zoom to selected percentage
                         Fit    = QEImage::RESIZE_OPTION_FIT    ///< Zoom to fit the current window size
                              };
    void setResizeOptionProperty( ResizeOptions resizeOption ){ setResizeOption( (QEImage::resizeOptions)resizeOption ); }  ///< Access function for #resizeOption property - refer to #resizeOption property for details
    ResizeOptions getResizeOptionProperty(){ return (ResizeOptions)getResizeOption(); }                                     ///< Access function for #resizeOption property - refer to #resizeOption property for details

    /// Zoom percentage. Used when #resizeOption is #Zoom
    Q_PROPERTY(int zoom READ getZoom WRITE setZoom)

    Q_ENUMS(RotationOptions)

    /// Image rotation option.
    ///
    Q_PROPERTY(RotationOptions rotation READ getRotationProperty WRITE setRotationProperty)
    /// \enum RotationOptions
    /// User friendly enumerations for #rotation property
    enum RotationOptions { NoRotation    = QEImage::ROTATION_0,         ///< No image rotation
                           Rotate90Right = QEImage::ROTATION_90_RIGHT,  ///< Rotate image 90 degrees clockwise
                           Rotate90Left  = QEImage::ROTATION_90_LEFT,   ///< Rotate image 90 degrees anticlockwise
                           Rotate180     = QEImage::ROTATION_180        ///< Rotate image 180 degrees
                          };
    void setRotationProperty( RotationOptions rotation ){ setRotation( (QEImage::rotationOptions)rotation ); }          ///< Access function for #rotation property - refer to #rotation property for details
    RotationOptions getRotationProperty(){ return (RotationOptions)getRotation(); }                                     ///< Access function for #rotation property - refer to #rotation property for details

    /// If true, flip image vertically.
    ///
    Q_PROPERTY(bool verticalFlip READ getVerticalFlip WRITE setVerticalFlip)

    /// If true, flip image horizontally.
    ///
    Q_PROPERTY(bool horizontalFlip READ getHorizontalFlip WRITE setHorizontalFlip)

    /// Sets the initial position of the horizontal scroll bar, if present.
    /// Used to set up an initial view when zoomed in.
    Q_PROPERTY(int initialHosScrollPos READ getInitialHozScrollPos WRITE setInitialHozScrollPos)

    /// Sets the initial position of the vertical scroll bar, if present.
    /// Used to set up an initial view when zoomed in.
    Q_PROPERTY(int initialVertScrollPos READ getInitialVertScrollPos WRITE setInitialVertScrollPos)

    /// If true, auto set local brightness and contrast when any area is selected.
    /// The brightness and contrast is set to use the full range of pixels in the selected area.
    Q_PROPERTY(bool enableBrightnessContrast READ getEnableBrightnessContrast WRITE setEnableBrightnessContrast)

    /// If true, local brightness and contrast controls are displayed.
    /// The brightness and contrast is set to use the full range of pixels in the selected area.
    Q_PROPERTY(bool autoBrightnessContrast READ getAutoBrightnessContrast WRITE setAutoBrightnessContrast)

    /// If true, image controls and views such as brightness controls and profile plots are hosted by the application as dock windows, toolbars, etc.
    /// Refer to the #ContainerProfile class and the #windowCustomisation class to see how this class asks an application to act as a host.
    Q_PROPERTY(bool externalControls READ getExternalControls WRITE setExternalControls)

    /// If true, a full context menu allowing manipulation of the image is available. If false, a simpler context menu containing common context menu options such as 'copy' is presented.
    ///
    Q_PROPERTY(bool fullContextMenu READ getFullContextMenu WRITE setFullContextMenu)
};

#endif // QEIMAGE_H
