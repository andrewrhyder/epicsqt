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

#ifndef QCAIMAGE_H
#define QCAIMAGE_H

#include <QScrollArea>
#include <QCaWidget.h>
#include <QCaInteger.h>
#include <videowidget.h>
#include <QELabel.h>
#include <QVBoxLayout>
#include <QGridLayout>
#include <profilePlot.h>
#include <zoomMenu.h>
#include <flipRotateMenu.h>
#include <selectMenu.h>


#include <QCaPluginLibrary_global.h>
#include <QCaIntegerFormatting.h>


class QCAPLUGINLIBRARYSHARED_EXPORT QCaImage : public QFrame, public QCaWidget {
    Q_OBJECT

  public:

    QCaImage( QWidget *parent = 0 );
    QCaImage( const QString &variableName, QWidget *parent = 0 );
    ~QCaImage();

    bool isEnabled() const;
    void setEnabled( bool state );

    enum selectOptions{ SO_NONE, SO_PANNING, SO_VSLICE, SO_HSLICE, SO_AREA, SO_PROFILE };
    selectOptions getSelectionOption();

    // Property convenience functions

    // Variable Name and substitution
    void setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );

    // variable as tool tip
    void setVariableAsToolTip( bool variableAsToolTip );
    bool getVariableAsToolTip();

    // visible (widget is visible outside 'Designer')
    void setRunVisible( bool visibleIn );
    bool getRunVisible();

    // Allow user to drop new PVs into this widget
    void setAllowDrop( bool allowDropIn );
    bool getAllowDrop();

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

    void setDisplayRegionOfInterest( bool displayRoiLayoutIn );
    bool getDisplayRegionOfInterest();

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

    void setMarkupColor(QColor pValue);
    QColor getMarkupColor();

    void setDisplayCursorPixelInfo( bool displayCursorPixelInfoIn );
    bool getDisplayCursorPixelInfo();

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



  protected:
    QCaIntegerFormatting integerFormatting; // Integer formatting options.
    bool caEnabled;

    void establishConnection( unsigned int variableIndex );

    bool caVisible;               // Flag true if the widget should be visible outside 'Designer'

    bool allowDrop;

    enum variableIndexes{ IMAGE_VARIABLE, WIDTH_VARIABLE, HEIGHT_VARIABLE, ROI_X_VARIABLE, ROI_Y_VARIABLE, ROI_W_VARIABLE, ROI_H_VARIABLE, QCAIMAGE_NUM_VARIABLES };

    resizeOptions resizeOption;
    int zoom;
    rotationOptions rotation;
    bool flipVert;
    bool flipHoz;

    int initialHozScrollPos;
    int initialVertScrollPos;
    bool displayRoiLayout;

    bool displayButtonBar;

private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setImage( const QByteArray& image, unsigned long dataSize, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );
    void setDimension( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex);

    void pauseClicked();

    void saveClicked();

    void roiClicked();
    void resetRoiClicked();

    void vSliceSelectModeClicked();
    void hSliceSelectModeClicked();
    void areaSelectModeClicked();
    void profileSelectModeClicked();

    void panModeClicked();




  public slots:
    void requestEnabled( const bool& state );
    void userSelection( imageMarkup::markupModes mode, QPoint point1, QPoint point2 );
    void zoomInOut( int zoomAmount );
    void currentPixelInfo( QPoint pos );
    void pan( QPoint pos );
    void ShowContextMenu( const QPoint& );
    void selectMenuTriggered( QAction* selectedItem );
    void zoomMenuTriggered( QAction* selectedItem );
    void flipRotateMenuTriggered( QAction* selectedItem );
    void setImageFile( QString name );

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

    QGroupBox *roiGroup;
    QFrame *buttonGroup;

    QELabel* roiXQELabel;
    QLabel* roiXLabel;

    QELabel* roiYQELabel;
    QLabel* roiYLabel;

    QELabel* roiWQELabel;
    QLabel* roiWLabel;

    QELabel* roiHQELabel;
    QLabel* roiHLabel;

    QScrollArea* scrollArea;
    bool initScrollPosSet;

    VideoWidget* videoWidget;

    QHBoxLayout* infoLayout;
    QLabel* currentCursorPixelLabel;
    QLabel* currentVertPixelLabel;
    QLabel* currentHozPixelLabel;
    QLabel* currentLineLabel;
    QLabel* currentAreaLabel;


    QPushButton *pauseButton;
    QPushButton *saveButton;
    QPushButton *roiButton;
    QPushButton *resetRoiButton;
    QPushButton *selectModeButton;
    QPushButton *zoomButton;
    QPushButton *flipRotateButton;

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

    bool displayCursorPixelInfo;



    void manageButtonBar();
    void manageInfoLayout();
    void manageRoiLayout();

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

    bool haveVSliceX;
    bool haveHSliceY;
    bool haveProfileLine;
    bool haveSelectedArea;

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

    // Drag and Drop
protected:
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    // Don't drag from interactive widget void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDropText( QString text );
    QString getDropText();




    void resizeEvent(QResizeEvent* );

};

#endif // QCAIMAGE_H
