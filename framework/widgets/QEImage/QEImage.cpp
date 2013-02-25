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

/*
  This class is a CA aware image widget based on the Qt frame widget.
  It is tighly integrated with the base class QEWidget. Refer to QEWidget.cpp for details.

  This class displays images from byte array (originating from a EPICS waveform record)
  It determines the width and height from other EPICS variables.
  The user can interact with the image.
  The image is managed by the VideoWidget class.
  User interaction and drawing markups over the image (such as selecting an area) is managed by the imageMarkup class.
 */

#include <QEImage.h>
#include <QEByteArray.h>
#include <QEInteger.h>
#include <imageContextMenu.h>
#include <QIcon>

/*
    Constructor with no initialisation
*/
QEImage::QEImage( QWidget *parent ) : QFrame( parent ), QEWidget( this ) {
    setup();
}

/*
    Constructor with known variable
*/
QEImage::QEImage( const QString &variableNameIn, QWidget *parent ) : QFrame( parent ), QEWidget( this )  {
    setup();
    setVariableName( variableNameIn, 0 );
}

/*
    Setup common to all constructors
*/
void QEImage::setup() {

    // Set up data
    // This control uses the following data sources:
    //  - image
    //  - width
    //  - height
    setNumVariables( QEIMAGE_NUM_VARIABLES );

    // Set up default properties
    setVariableAsToolTip( false );

    setAllowDrop( false );
    resizeOption = RESIZE_OPTION_ZOOM;
    zoom = 100;
    rotation = ROTATION_0;
    flipVert = false;
    flipHoz = false;
    initialHozScrollPos = 0;
    initialVertScrollPos = 0;
    initScrollPosSet = false;
    formatOption = GREY8;
    paused = false;

    displayButtonBar = false;

    haveVSliceX = false;
    haveHSliceY = false;
    haveProfileLine = false;
    haveSelectedArea1 = false;
    haveSelectedArea2 = false;
    haveSelectedArea3 = false;
    haveSelectedArea4 = false;

    showTimeEnabled = false;

    enableAreaSelection = true;
    enableVSliceSelection = false;
    enableHSliceSelection = false;
    enableProfileSelection = false;
    enableTargetSelection = false;

    displayCursorPixelInfo = false;
    contrastReversal = false;

    autoBrightnessContrast = false;
    enableBrightnessContrast = true;
    autoBrightnessContrast = false;

    // Set the initial state
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;


    imageDataSize = 0;

    clippingOn = false;
    clippingLow = 0;
    clippingHigh = 0;

    // Use frame signals
    // --Currently none--


    // Create the video destination
    videoWidget = new VideoWidget;
    setVertSliceMarkupColor( QColor(127, 255, 127));
    setHozSliceMarkupColor(  QColor(255, 100, 100));
    setProfileMarkupColor(   QColor(255, 255, 100));
    setAreaMarkupColor(      QColor(100, 100, 255));
    setBeamMarkupColor(      QColor(255,   0,   0));
    setTargetMarkupColor(    QColor(  0, 255,   0));
    setTimeMarkupColor(      QColor(255, 255, 255));

    QObject::connect( videoWidget, SIGNAL( userSelection( imageMarkup::markupIds, bool, bool, QPoint, QPoint ) ),
                      this,        SLOT  ( userSelection( imageMarkup::markupIds, bool, bool, QPoint, QPoint )) );
    QObject::connect( videoWidget, SIGNAL( zoomInOut( int ) ),
                      this,        SLOT  ( zoomInOut( int ) ) );
    QObject::connect( videoWidget, SIGNAL( currentPixelInfo( QPoint ) ),
                      this,        SLOT  ( currentPixelInfo( QPoint ) ) );
    QObject::connect( videoWidget, SIGNAL( pan( QPoint ) ),
                      this,        SLOT  ( pan( QPoint ) ) );


    // Create sub menus
    zMenu = new zoomMenu();
    zMenu->enableAreaSelected( haveSelectedArea1 );
    QObject::connect( zMenu, SIGNAL( triggered ( QAction* ) ), this,  SLOT  ( zoomMenuTriggered( QAction* )) );

    frMenu = new flipRotateMenu();
    frMenu->setChecked( rotation, flipHoz, flipVert );
    QObject::connect( frMenu, SIGNAL( triggered ( QAction* ) ), this,  SLOT  ( flipRotateMenuTriggered( QAction* )) );

    sMenu = new selectMenu();
    sMenu->setVSliceEnabled( enableVSliceSelection );
    sMenu->setHSlicetEnabled( enableHSliceSelection );
    sMenu->setAreaEnabled( enableAreaSelection );
    sMenu->setProfileEnabled( enableProfileSelection );
    sMenu->setTargetEnabled( enableTargetSelection );
    QObject::connect( sMenu, SIGNAL( triggered ( QAction* ) ), this,  SLOT  ( selectMenuTriggered( QAction* )) );

    // Add the video destination to the widget
    scrollArea = new QScrollArea;
    scrollArea->setFrameStyle( QFrame::NoFrame );
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget( videoWidget );

    // Add the pixel display labels
    currentCursorPixelLabel = new QLabel();
    currentVertPixelLabel = new QLabel();
    currentHozPixelLabel = new QLabel();
    currentLineLabel = new QLabel();
    currentArea1Label = new QLabel();
    currentArea2Label = new QLabel();
    currentArea3Label = new QLabel();
    currentArea4Label = new QLabel();
    currentTargetLabel = new QLabel();
    currentBeamLabel = new QLabel();

    infoLayout = new QGridLayout();
    infoLayout->addWidget( currentCursorPixelLabel, 0, 0 );
    infoLayout->addWidget( currentVertPixelLabel, 0, 1 );
    infoLayout->addWidget( currentHozPixelLabel, 0, 2 );
    infoLayout->addWidget( currentLineLabel, 0, 3 );
    infoLayout->addWidget( currentArea1Label, 1, 0 );
    infoLayout->addWidget( currentArea2Label, 1, 1 );
    infoLayout->addWidget( currentArea3Label, 1, 2 );
    infoLayout->addWidget( currentArea4Label, 1, 3 );
    infoLayout->addWidget( currentTargetLabel, 2, 0 );
    infoLayout->addWidget( currentBeamLabel, 2, 1 );

    // Local brightness and contrast controls
    brightnessContrastFrame = new QFrame;
    brightnessContrastFrame->setFrameStyle( QFrame::StyledPanel|QFrame::Raised );

    QGridLayout* brightnessContrastLayout = new QGridLayout();
    brightnessContrastLayout->setVerticalSpacing( 0 );
    brightnessContrastFrame->setLayout( brightnessContrastLayout );

    QLabel* brightnessLabel = new QLabel( "Brightnes:", brightnessContrastFrame );

    QLabel* contrastLabel = new QLabel( "Contrast:", brightnessContrastFrame );

    autoBrightnessCheckBox = new QCheckBox( "Auto Brightness and Contrast", brightnessContrastFrame );
    QObject::connect( brightnessSlider, SIGNAL( changeEvent ( QEvent * ) ), this,  SLOT  ( autoBrightnessCheckBoxChanged( QEvent * )) );

    brightnessSlider = new QSlider( Qt::Horizontal, brightnessContrastFrame );
    QObject::connect( brightnessSlider, SIGNAL( valueChanged ( int ) ), this,  SLOT  ( brightnessSliderValueChanged( int )) );

    contrastSlider = new QSlider( Qt::Horizontal, brightnessContrastFrame );
    QObject::connect( contrastSlider, SIGNAL( valueChanged ( int ) ), this,  SLOT  ( contrastSliderValueChanged( int )) );

    brightnessRBLabel = new QLabel( brightnessContrastFrame );

    contrastRBLabel = new QLabel( brightnessContrastFrame );


    brightnessContrastLayout->addWidget( autoBrightnessCheckBox, 0, 0 );

    brightnessContrastLayout->addWidget( brightnessLabel, 1, 0 );
    brightnessContrastLayout->addWidget( brightnessSlider, 1, 1 );
    brightnessContrastLayout->addWidget( brightnessRBLabel, 1, 2 );

    brightnessContrastLayout->addWidget( contrastLabel, 2, 0 );
    brightnessContrastLayout->addWidget( contrastSlider, 2, 1 );
    brightnessContrastLayout->addWidget( contrastRBLabel, 2, 2 );

    brightnessContrastLayout->setColumnStretch( 2, 1 );  // Read back labels to take all spare room

    localBrightness = 0.5;
    localContrast = 0.5;


    // Create vertical, horizontal, and general profile plots
    vSliceDisplay = new profilePlot( profilePlot::PROFILEPLOT_BT );
    vSliceDisplay->setMinimumWidth( 100 );
    vSliceDisplay->setVisible( false );

    hSliceDisplay = new profilePlot( profilePlot::PROFILEPLOT_LR );
    hSliceDisplay->setMinimumHeight( 100 );
    hSliceDisplay->setVisible( false );

    profileDisplay = new profilePlot( profilePlot::PROFILEPLOT_LR );
    profileDisplay->setMinimumHeight( 100 );
    profileDisplay->setVisible( false );


    QGridLayout* graphicsLayout = new QGridLayout();
    graphicsLayout->addWidget( scrollArea, 0, 0 );
    graphicsLayout->addLayout( infoLayout, 1, 0 );
    graphicsLayout->addWidget( vSliceDisplay, 0, 1 );
    graphicsLayout->addWidget( hSliceDisplay, 2, 0 );
    graphicsLayout->addWidget( profileDisplay, 3, 0 );

    graphicsLayout->setColumnStretch( 0, 1 );  // display image to take all spare room
    graphicsLayout->setRowStretch( 0, 1 );  // display image to take all spare room


    // Create button group
    int buttonWidth = 28;
    int buttonMenuWidth = 48;

    buttonGroup = new QFrame;
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setMargin( 0 );
    buttonGroup->setLayout( buttonLayout );

    pauseButton = new QPushButton( buttonGroup );
    pauseButton->setMinimumWidth( buttonWidth );
    pauseButtonIcon = new QIcon( ":/qe/image/pause.png" );
    playButtonIcon = new QIcon( ":/qe/image/play.png" );
    pauseButton->setIcon( *pauseButtonIcon );
    pauseButton->setToolTip("Pause image display");
    QObject::connect(pauseButton, SIGNAL(clicked()), this, SLOT(pauseClicked()));

    saveButton = new QPushButton(buttonGroup);
    saveButton->setMinimumWidth( buttonWidth );
    QIcon saveButtonIcon( ":/qe/image/save.png" );
    saveButton->setIcon( saveButtonIcon );
    saveButton->setToolTip("Save displayed image");
    QObject::connect(saveButton, SIGNAL(clicked()), this, SLOT(saveClicked()));

    targetButton = new QPushButton(buttonGroup);
    targetButton->setMinimumWidth( buttonWidth );
    QIcon targetButtonIcon( ":/qe/image/target.png" );
    targetButton->setIcon( targetButtonIcon );
    targetButton->setToolTip("Move target position into beam");
    QObject::connect(targetButton, SIGNAL(clicked()), this, SLOT(targetClicked()));

    selectModeButton = new QPushButton(buttonGroup);
    selectModeButton->setMinimumWidth( buttonMenuWidth );
    QIcon selectModeButtonIcon( ":/qe/image/select.png" );
    selectModeButton->setIcon( selectModeButtonIcon );
    selectModeButton->setToolTip("Choose selection and pan modes");
    selectModeButton->setMenu( sMenu );

    zoomButton = new QPushButton(buttonGroup);
    zoomButton->setMinimumWidth( buttonMenuWidth );
    QIcon zoomButtonIcon( ":/qe/image/zoom.png" );
    zoomButton->setIcon( zoomButtonIcon );
    zoomButton->setToolTip("Zoom options");
    zoomButton->setMenu( zMenu );

    flipRotateButton = new QPushButton(buttonGroup);
    flipRotateButton->setMinimumWidth( buttonMenuWidth );
    QIcon flipRotateButtonIcon( ":/qe/image/flipRotate.png" );
    flipRotateButton->setIcon( flipRotateButtonIcon );
    flipRotateButton->setToolTip("Flip and rotate options");
    flipRotateButton->setMenu( frMenu );


    buttonLayout->addWidget( pauseButton,      0);
    buttonLayout->addWidget( saveButton,       1);
    buttonLayout->addWidget( targetButton,     2);
    buttonLayout->addWidget( selectModeButton, 3);
    buttonLayout->addWidget( zoomButton,       4);
    buttonLayout->addWidget( flipRotateButton, 5);
    buttonLayout->addStretch();


    // Create main layout containing image, label, and button layouts
    mainLayout = new QGridLayout;
    mainLayout->setMargin( 0 );

    mainLayout->addWidget( buttonGroup, 0, 0 );
    mainLayout->addWidget( brightnessContrastFrame, 0, 1 );
    mainLayout->addLayout( graphicsLayout, 1, 0, 1, 0 );

    // Set graphics to take all spare room
    mainLayout->setColumnStretch( 1, 1 );
    mainLayout->setRowStretch( 1, 1 );

    setLayout( mainLayout );

    // Set up labels as required by properties
    manageButtonBar();
    manageInfoLayout();

    // Set up context sensitive menu (right click menu)
    setContextMenuPolicy( Qt::CustomContextMenu );
    connect( this, SIGNAL( customContextMenuRequested( const QPoint& )), this, SLOT( showContextMenu( const QPoint& )));

    // Initially set the video widget to the size of the scroll bar
    // This will be resized when the image size is known
    videoWidget->resize( scrollArea->width(), scrollArea->height() );

    // Set image size to zero
    // Image will not be presented until size is available
    imageBuffWidth = 0;
    imageBuffHeight = 0;


    // Act on default property values
    // Note, this resets them to their current value
    showTimeEnabled = false;

    setShowTime( showTimeEnabled );

    setEnableAreaSelection( enableAreaSelection );
    setEnableVertSliceSelection( enableVSliceSelection );
    setEnableHozSliceSelection( enableHSliceSelection );
    setEnableProfileSelection( enableProfileSelection );
    setEnableTargetSelection( enableTargetSelection );

    setDisplayCursorPixelInfo( displayCursorPixelInfo );

    // Simulate pan mode being selected
    panModeClicked();
    sMenu->setChecked( QEImage::SO_PANNING );

    // !! move this functionality into QEWidget???
    // !! needs one for single variables and one for multiple variables, or just the multiple variable one for all
    // for each variable name property manager, set up an index to identify it when it signals and
    // set up a connection to recieve variable name property changes.
    // The variable name property manager class only delivers an updated variable name after the user has stopped typing
    for( int i = 0; i < QEIMAGE_NUM_VARIABLES; i++ )
    {
        variableNamePropertyManagers[i].setVariableIndex( i );
        QObject::connect( &variableNamePropertyManagers[i], SIGNAL( newVariableNameProperty( QString, QString, unsigned int ) ), this, SLOT( useNewVariableNameProperty( QString, QString, unsigned int ) ) );
    }
}

QEImage::~QEImage()
{
    delete videoWidget;

}

/*
    Implementation of QEWidget's virtual funtion to create the specific type of QCaObject required.
*/
qcaobject::QCaObject* QEImage::createQcaItem( unsigned int variableIndex ) {

    switch( variableIndex )
    {
        // Create the image item as a QEByteArray
        case IMAGE_VARIABLE:
            return new QEByteArray( getSubstitutedVariableName( variableIndex ), this, variableIndex );

        // Create the width, height, target and beam, and clipping items as a QEInteger
        case WIDTH_VARIABLE:
        case HEIGHT_VARIABLE:

        case ROI1_X_VARIABLE:
        case ROI1_Y_VARIABLE:
        case ROI1_W_VARIABLE:
        case ROI1_H_VARIABLE:

        case ROI2_X_VARIABLE:
        case ROI2_Y_VARIABLE:
        case ROI2_W_VARIABLE:
        case ROI2_H_VARIABLE:

        case ROI3_X_VARIABLE:
        case ROI3_Y_VARIABLE:
        case ROI3_W_VARIABLE:
        case ROI3_H_VARIABLE:

        case ROI4_X_VARIABLE:
        case ROI4_Y_VARIABLE:
        case ROI4_W_VARIABLE:
        case ROI4_H_VARIABLE:

        case TARGET_X_VARIABLE:
        case TARGET_Y_VARIABLE:

        case BEAM_X_VARIABLE:
        case BEAM_Y_VARIABLE:
        case TARGET_TRIGGER_VARIABLE:

        case CLIPPING_ONOFF_VARIABLE:
        case CLIPPING_LOW_VARIABLE:
        case CLIPPING_HIGH_VARIABLE:
            return new QEInteger( getSubstitutedVariableName( variableIndex ), this, &integerFormatting, variableIndex );

        default:
            return NULL;
    }
}

/*
    Start updating.
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
    This function may also be used to initiate updates when loaded as a plugin.
*/
void QEImage::establishConnection( unsigned int variableIndex ) {

    // Create a connection.
    // If successfull, the QCaObject object that will supply data update signals will be returned
    qcaobject::QCaObject* qca = createConnection( variableIndex );

    switch( variableIndex )
    {
        // Connect the image waveform record to the display image
        case IMAGE_VARIABLE:
            if(  qca )
            {
                QObject::connect( qca,  SIGNAL( byteArrayChanged( const QByteArray&, unsigned long, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                                  this, SLOT( setImage( const QByteArray&, unsigned long, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
                QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                                  this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
                QObject::connect( this, SIGNAL( requestResend() ),
                                  qca, SLOT( resendLastData() ) );
            }
            break;

        // Connect the image dimension variables
        case WIDTH_VARIABLE:
        case HEIGHT_VARIABLE:
            if(  qca )
            {
                QObject::connect( qca,  SIGNAL( integerChanged( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                                  this, SLOT( setDimension( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
                QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                                  this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
                QObject::connect( this, SIGNAL( requestResend() ),
                                  qca, SLOT( resendLastData() ) );
            }
            break;

        // Connect the clipping variables
        case CLIPPING_ONOFF_VARIABLE:
        case CLIPPING_LOW_VARIABLE:
        case CLIPPING_HIGH_VARIABLE:
            if(  qca )
            {
                QObject::connect( qca,  SIGNAL( integerChanged( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                                  this, SLOT( setClipping( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
                QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                                  this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
                QObject::connect( this, SIGNAL( requestResend() ),
                                  qca, SLOT( resendLastData() ) );
            }
            break;

        // Conntect to the ROI variables
        case ROI1_X_VARIABLE:
        case ROI1_Y_VARIABLE:
        case ROI1_W_VARIABLE:
        case ROI1_H_VARIABLE:

        case ROI2_X_VARIABLE:
        case ROI2_Y_VARIABLE:
        case ROI2_W_VARIABLE:
        case ROI2_H_VARIABLE:

        case ROI3_X_VARIABLE:
        case ROI3_Y_VARIABLE:
        case ROI3_W_VARIABLE:
        case ROI3_H_VARIABLE:

        case ROI4_X_VARIABLE:
        case ROI4_Y_VARIABLE:
        case ROI4_W_VARIABLE:
        case ROI4_H_VARIABLE:

            if(  qca )
            {
                QObject::connect( qca,  SIGNAL( integerChanged( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                                  this, SLOT( setROI( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
                QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                                  this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
                QObject::connect( this, SIGNAL( requestResend() ),
                                  qca, SLOT( resendLastData() ) );
            }
            break;

        // QCa creation occured, but no connection for display is required here.
        case TARGET_X_VARIABLE:
        case TARGET_Y_VARIABLE:

        case BEAM_X_VARIABLE:
        case BEAM_Y_VARIABLE:
            break;
     }
}

/*
    Act on a connection change.
    Change how the label looks and change the tool tip
    This is the slot used to recieve connection updates from a QCaObject based class.
 */
void QEImage::connectionChanged( QCaConnectionInfo& connectionInfo )
{
    // If connected, enable the widget if the QE enabled property is true
    if( connectionInfo.isChannelConnected() )
    {
        isConnected = true;
        updateToolTipConnection( isConnected );

        setDataDisabled( false );
    }

    // If disconnected always disable the widget.
    else
    {
        isConnected = false;
        updateToolTipConnection( isConnected );

        setDataDisabled( true );
    }
}

/*
    Update the image dimensions
    This is the slot used to recieve data updates from a QCaObject based class.
 */
void QEImage::setDimension( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex)
{
    // Update image size variable
    switch( variableIndex )
    {
        case WIDTH_VARIABLE:
            imageBuffWidth = value;
            break;

        case HEIGHT_VARIABLE:
            imageBuffHeight = value;
            break;
    }

    // Update the image buffer according to the new size
    setImageBuff();

    // Display invalid if invalid
    if( alarmInfo.isInvalid() )
    {
        //setImageInvalid()
        // !!! not done
    }
}

/*
    Update the clipping info
    This is the slot used to recieve data updates from a QCaObject based class.
 */
void QEImage::setClipping( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex)
{
    // Update image size variable
    switch( variableIndex )
    {
        case CLIPPING_ONOFF_VARIABLE:
            clippingOn = (value>0)?true:false;
            break;

        case CLIPPING_LOW_VARIABLE:
            clippingLow = value;
            break;

        case CLIPPING_HIGH_VARIABLE:
            clippingHigh = value;
            break;
        }

    // Update the image buffer according to the new size
    setImageBuff();

    // Display invalid if invalid
    if( alarmInfo.isInvalid() )
    {
        //setImageInvalid()
        // !!! not done
    }
}

/*
    Update the ROI displays if any
    This is the slot used to recieve data updates from a QCaObject based class.
 */
void QEImage::setROI( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex)
{
    // If invalid, mark the appropriate ROI info as not present
    if( alarmInfo.isInvalid() )
    {
        switch( variableIndex )
        {
            case ROI1_X_VARIABLE: roiInfo[0].clearX(); break;
            case ROI1_Y_VARIABLE: roiInfo[0].clearY(); break;
            case ROI1_W_VARIABLE: roiInfo[0].clearW(); break;
            case ROI1_H_VARIABLE: roiInfo[0].clearH(); break;
            case ROI2_X_VARIABLE: roiInfo[1].clearX(); break;
            case ROI2_Y_VARIABLE: roiInfo[1].clearY(); break;
            case ROI2_W_VARIABLE: roiInfo[1].clearW(); break;
            case ROI2_H_VARIABLE: roiInfo[1].clearH(); break;
            case ROI3_X_VARIABLE: roiInfo[2].clearX(); break;
            case ROI3_Y_VARIABLE: roiInfo[2].clearY(); break;
            case ROI3_W_VARIABLE: roiInfo[2].clearW(); break;
            case ROI3_H_VARIABLE: roiInfo[2].clearH(); break;
            case ROI4_X_VARIABLE: roiInfo[3].clearX(); break;
            case ROI4_Y_VARIABLE: roiInfo[3].clearY(); break;
            case ROI4_W_VARIABLE: roiInfo[3].clearW(); break;
            case ROI4_H_VARIABLE: roiInfo[3].clearH(); break;
            }
    }

    // Good data. Save the ROI data (and note it is present) then if the
    // markup is visible, update it
    else
    {
#define USE_ROI_DATA( N, SET_NAME ) roiInfo[N].SET_NAME( value ); if( roiInfo[N].getStatus() ) videoWidget->markupRegionValueChange( N, roiInfo[N].getArea() ); break;
        switch( variableIndex )
        {
            case ROI1_X_VARIABLE:  USE_ROI_DATA( 0, setX )
            case ROI1_Y_VARIABLE:  USE_ROI_DATA( 0, setY )
            case ROI1_W_VARIABLE:  USE_ROI_DATA( 0, setW )
            case ROI1_H_VARIABLE:  USE_ROI_DATA( 0, setH )
            case ROI2_X_VARIABLE:  USE_ROI_DATA( 1, setX )
            case ROI2_Y_VARIABLE:  USE_ROI_DATA( 1, setY )
            case ROI2_W_VARIABLE:  USE_ROI_DATA( 1, setW )
            case ROI2_H_VARIABLE:  USE_ROI_DATA( 1, setH )
            case ROI3_X_VARIABLE:  USE_ROI_DATA( 2, setX )
            case ROI3_Y_VARIABLE:  USE_ROI_DATA( 2, setY )
            case ROI3_W_VARIABLE:  USE_ROI_DATA( 2, setW )
            case ROI3_H_VARIABLE:  USE_ROI_DATA( 2, setH )
            case ROI4_X_VARIABLE:  USE_ROI_DATA( 3, setX )
            case ROI4_Y_VARIABLE:  USE_ROI_DATA( 3, setY )
            case ROI4_W_VARIABLE:  USE_ROI_DATA( 3, setW )
            case ROI4_H_VARIABLE:  USE_ROI_DATA( 3, setH )
        }
    }
}

/*
    Update the image
    This is the slot used to recieve data updates from a QCaObject based class.
    Note the following comments from the Qt help:
        Note: Drawing into a QImage with QImage::Format_Indexed8 is not supported.
        Note: Do not render into ARGB32 images using QPainter. Using QImage::Format_ARGB32_Premultiplied is significantly faster.
 */
void QEImage::setImage( const QByteArray& imageIn, unsigned long dataSize, QCaAlarmInfo& alarmInfo, QCaDateTime& time, const unsigned int& )
{
    // If the display is paused, do nothing
    if (paused)
    {
        return;
    }

    // Signal a database value change to any Link widgets
    emit dbValueChanged( "image" );

    // Save the image data for analysis and redisplay
    image = imageIn;
    imageDataSize = dataSize;
    imageTime = time;

    // Present the new image
    displayImage();

    // Display invalid if invalid
    if( alarmInfo.isInvalid() )
    {
        //setImageInvalid()
        // !!! not done
    }
}

// Display a new image.
void QEImage::displayImage()
{
    // Do nothing if there are no image dimensions yet
    if( !imageBuffWidth || !imageBuffHeight )
        return;

    // Set up the image buffer if not done already
    if( imageBuff.isEmpty() )
        setImageBuff();

    // Now an image can be displayed, set the initial scroll bar positions if not set before
    if( initScrollPosSet == false )
    {
        scrollArea->verticalScrollBar()->setValue( initialVertScrollPos );
        scrollArea->horizontalScrollBar()->setValue( 0 );// !!!initialHozScrollPos );
        initScrollPosSet = true;
    }

    // Set up input and output pointers and counters ready to process each pixel
    const unsigned char* dataIn = (unsigned char*)image.constData();
    unsigned int* dataOut = (unsigned int*)(imageBuff.data());
    unsigned long buffIndex = 0;
    unsigned long dataIndex = 0;

    // Determine the number of pixels to process
    // If something is wrong, do nothing
    unsigned long pixelCount = imageBuffHeight*imageBuffWidth;
    if(( pixelCount * imageDataSize > (unsigned long)image.size() ) ||
       ( pixelCount * IMAGEBUFF_BYTES_PER_PIXEL > (unsigned long)imageBuff.size() ))
    {
        return;  // !!! should clear the image
    }

    // Depending on the flipping and rotating options pixel drawing can start in any of
    // the four corners and start scanning either vertically or horizontally.
    // See getScanOption() comments for more details on how the rotate and flip
    // options are used to generate one of 8 scan options.
    // The 8 scanning options are shown numbered here:
    //
    //    o----->1         2<-----o
    //    |                       |
    //    |                       |
    //    |                       |
    //    v                       v
    //    5                       6
    //
    //
    //
    //    7                       8
    //    ^                       ^
    //    |                       |
    //    |                       |
    //    |                       |
    //    o----->3         4<-----o
    //
    int scanOption = getScanOption();

    // Drawing is performed in two nested loops, one for height and one for width.
    // Depending on the scan option, however, the outer may be height or width.
    // The input buffer is read consecutivly from first pixel to last and written to the
    // output buffer, which is moved to the next pixel by both the inner and outer
    // loops to where ever that next pixel is according to the rotation and flipping.
    // The following defines parameters driving the loops:
    //
    // opt      = scan option
    // outCount = outer loop count (width or height);
    // inCount  = inner loop count (height or width)
    // start    = output buffer start pixel (one of the four corners)
    // outInc   = outer loop increment to output buffer
    // inInc    = inner loop increment to output buffer
    // w        = image width
    // h        = image height
    //
    // opt outCount inCount start    outInc   inInc
    //  1      h       w      0         0       1
    //  2      h       w      w-1       w      -1
    //  3      h       w    w*(h-1)    -2*w     1
    //  4      h       w    (w*h)-1     0      -1
    //  5      w       h      0     -w*(h-1)+1   w
    //  6      w       h      w-1   -w*(h-1)-1   w
    //  7      w       h    w*(h-1)  w*(h-1)+1  -w
    //  8      w       h    (w*h)-1  w*(h-1)-1  -w


    int outCount;   // Outer loop count (width or height);
    int inCount;    // Inner loop count (height or width)
    int start;      // Output buffer start pixel (one of the four corners)
    int outInc;     // Outer loop increment to output buffer
    int inInc;      // Inner loop increment to output buffer
    int h = imageBuffHeight;
    int w = imageBuffWidth;

    // Set the loop parameters according to the scan option
    switch( scanOption )
    {
        default:  // Sanity check. default to 1
        case 1: outCount = h; inCount = w; start = 0;       outInc =  0;     inInc =  1; break;
        case 2: outCount = h; inCount = w; start = w-1;     outInc =  2*w;   inInc = -1; break;
        case 3: outCount = h; inCount = w; start = w*(h-1); outInc = -2*w;   inInc =  1; break;
        case 4: outCount = h; inCount = w; start = (w*h)-1; outInc =  0;     inInc = -1; break;
        case 5: outCount = w; inCount = h; start = 0;       outInc = -w*h+1; inInc =  w; break;
        case 6: outCount = w; inCount = h; start = w-1;     outInc = -w*h-1; inInc =  w; break;
        case 7: outCount = w; inCount = h; start = w*(h-1); outInc =  w*h+1; inInc = -w; break;
        case 8: outCount = w; inCount = h; start = (w*h)-1; outInc =  w*h-1; inInc = -w; break;
    }

    // Draw the input pixels into the image buffer.
    // Drawing is performed in two nested loops, one for height and one for width.
    // Depenting on the scan option, however, the outer may be height or width.
    // The input buffer is read consecutivly from first pixel to last and written to the
    // output buffer, which is moved to the next pixel by both the inner and outer
    // loops to where ever that next pixel is according to the rotation and flipping.
    dataIndex = start;

// For speed, the format switch statement is outside the pixel loop.
// An identical loop is used for each format
#define LOOP_START                          \
    for( int i = 0; i < outCount; i++ )     \
    {                                       \
        for( int j = 0; j < inCount; j++ )  \
        {

#define LOOP_END                            \
            dataIndex += inInc;             \
            buffIndex++;                    \
        }                                   \
        dataIndex += outInc;                \
    }

// Define the clipping logic
#define CLIPPING(PIXEL)                                     \
    if( clippingHigh > 0 && (unsigned int)(PIXEL) >= clippingHigh )         \
    {                                                       \
        dataOut[buffIndex] = 0xffff8080;                    \
    }                                                       \
    else if( clippingLow > 0 && (unsigned int)(PIXEL) <= clippingLow )      \
    {                                                       \
        dataOut[buffIndex] = 0xff8080ff;                    \
    }                                                       \
    else

// Define the input pixel selection for each format
#define PIXEL_GREY8   unsigned long inPixel = dataIn[dataIndex*imageDataSize];
#define PIXEL_GREY16  unsigned long inPixel = *(unsigned short*)(&dataIn[dataIndex*imageDataSize]);
#define PIXEL_GREY12  unsigned long inPixel = *(unsigned short*)(&dataIn[dataIndex*imageDataSize]);
#define PIXEL_RGB_888 unsigned char* inPixel  = (unsigned char*)(&dataIn[dataIndex*imageDataSize]);


// Define the conversion to RGB for each format (Keep each in braces so each macro can be used as an 'else' statement)

// Duplicate 8 bits of the grey scale into each color
#define FORMAT_GREY8                                                        \
    {                                                                       \
        dataOut[buffIndex] = 0xff000000+(inPixel<<16)+(inPixel<<8)+inPixel; \
    }

// Duplicate top 8 bits of the grey scale into each color
#define FORMAT_GREY16                                                                   \
    {                                                                                   \
        inPixel = inPixel>>8;                                                           \
        dataOut[buffIndex] = 0xff000000+(inPixel<<16)+(inPixel<<8)+inPixel;             \
    }


// Duplicate top 8 bits of the grey scale into each color
#define FORMAT_GREY12                                                                   \
    {                                                                                   \
        inPixel = (inPixel>>4)&0xff;                                                    \
        dataOut[buffIndex] = 0xff000000+(inPixel<<16)+(inPixel<<8)+inPixel;             \
    }

// Copy RGB values
#define FORMAT_RGB_888                                                                  \
    {                                                                                   \
        dataOut[buffIndex] = 0xff000000+(inPixel[2]<<16)+(inPixel[1]<<8)+inPixel[0];    \
    }

#define CLIPPING_TEST ( clippingOn && (clippingHigh > 0 || clippingLow > 0 ))

#define CONTRAST_REVERSAL(PIXEL)                                                        \
    {                                                                                   \
        if( contrastReversal )                                                          \
        {                                                                               \
            *PIXEL=255-*PIXEL;                                                          \
        }                                                                               \
    }

    // Format each pixel ready for use in an RGB32 QImage
    // Note, for speed, the conditional code related to clipping has been extracted from the pixel loop
    // Macros have been used to ensure the same code is used within the clipping and non clipping loops.
    switch( formatOption )
    {
        case GREY8:
        {
            if( CLIPPING_TEST )
            {
                LOOP_START
                PIXEL_GREY8
                CONTRAST_REVERSAL(&inPixel)
                CLIPPING(inPixel)
                // else
                FORMAT_GREY8
                LOOP_END
            }
            else
            {
                LOOP_START
                PIXEL_GREY8
                CONTRAST_REVERSAL(&inPixel)
                FORMAT_GREY8
                LOOP_END
            }
            break;
        }

        case GREY16:
        {
            if( CLIPPING_TEST )
            {
                LOOP_START
                PIXEL_GREY16
                CONTRAST_REVERSAL(&inPixel)
                CLIPPING(inPixel)
                // else
                FORMAT_GREY16
                LOOP_END
            }
            else
            {
                LOOP_START
                PIXEL_GREY16
                CONTRAST_REVERSAL(&inPixel)
                FORMAT_GREY16
                LOOP_END
            }
            break;
        }

        case GREY12:
        {
            if( CLIPPING_TEST )
            {
                LOOP_START
                PIXEL_GREY12
                CONTRAST_REVERSAL(&inPixel)
                CLIPPING(inPixel)
                // else
                FORMAT_GREY12
                LOOP_END
            }
            else
            {
                LOOP_START
                PIXEL_GREY12
                CONTRAST_REVERSAL(&inPixel)
                FORMAT_GREY12
                LOOP_END
            }
            break;
        }

        case RGB_888:
        {
            if( CLIPPING_TEST )
            {
                LOOP_START
                PIXEL_RGB_888
                CONTRAST_REVERSAL(inPixel)
                CLIPPING(((inPixel[0]+inPixel[1]+inPixel[2])/3))
                // else
                FORMAT_RGB_888
                LOOP_END
            }
            else
            {
                LOOP_START
                PIXEL_RGB_888
                CONTRAST_REVERSAL(inPixel)
                FORMAT_RGB_888
                LOOP_END
            }
            break;
        }
    }

    // Generate a frame from the data
    // !!! don't create new image???
    QImage frameImage( (uchar*)(imageBuff.constData()), rotatedImageBuffWidth(), rotatedImageBuffHeight(), QImage::Format_RGB32 );

    // Display the new image
    videoWidget->setNewImage( frameImage, imageTime );

    // Update markups if required
    updateMarkups();
}

void QEImage::setImageFile( QString name )
{
    // Generate an image given the filename
    QImage image( name );

    // Generate an array of image data so the mechanisms that normally work
    // on the raw image waveform data have data to work on
    QImage stdImage = image.convertToFormat( QImage::Format_RGB32 );
#if QT_VERSION >= 0x040700
    const uchar* iDataPtr = stdImage.constBits();
#else
    const uchar* iDataPtr = stdImage.bits();
#endif
    int iDataSize = stdImage.byteCount();

    QByteArray baData;
    baData.resize( iDataSize );
    char* baDataPtr = baData.data();
    for( int i = 0; i < iDataSize; i++ )
    {
        baDataPtr[i] = iDataPtr[i];
    }

    // Generate information normally associated with an image waveform
    QCaAlarmInfo alarmInfo;

    QFileInfo fi( name );
    QCaDateTime time = fi.lastModified();

    // Setup the widget in the same way receiving valid image, width and height data would
    // !! make this common to the actual data update functions
    scrollArea->setEnabled( true );
    imageBuffWidth = stdImage.width();
    imageBuffHeight = stdImage.height();
    setFormatOption( RGB_888 );
    setImageBuff();

    // Use the image data just like it came from a waveform variable
    setImage( baData, 4, alarmInfo, time, 0 );
}


// Update markups if required.
// This is called after displaying the image.
void QEImage::updateMarkups()
{
    if( haveVSliceX )
    {
        generateVSlice( vSliceX );
    }
    if( haveHSliceY )
    {
        generateHSlice( hSliceY );
    }
    if( haveProfileLine )
    {
        generateProfile( profileLineStart, profileLineEnd );
    }
    if( haveSelectedArea1 )
    {
        displaySelectedArea1Info( selectedArea1Point1, selectedArea1Point2 );
    }
    if( haveSelectedArea2 )
    {
        displaySelectedArea2Info( selectedArea2Point1, selectedArea2Point2 );
    }
    if( haveSelectedArea3 )
    {
        displaySelectedArea3Info( selectedArea3Point1, selectedArea3Point2 );
    }
    if( haveSelectedArea4 )
    {
        displaySelectedArea4Info( selectedArea4Point1, selectedArea4Point2 );
    }
}

// Set the image buffer used for generate images will be large enough to hold the processed image
void QEImage::setImageBuff()
{
    // Do nothing if there are no image dimensions yet
    if( !imageBuffWidth || !imageBuffHeight )
        return;

    // Size the image
    switch( resizeOption )
    {
        // Zoom the image
        case RESIZE_OPTION_ZOOM:
            videoWidget->resize( rotatedImageBuffWidth() * zoom / 100, rotatedImageBuffHeight() * zoom / 100 );
            break;

        // Resize the image to fit exactly within the QCaItem
        case RESIZE_OPTION_FIT:
            double vScale = (double)(scrollArea->size().height()) / (double)(rotatedImageBuffHeight());
            double hScale = (double)(scrollArea->size().width()) / (double)(rotatedImageBuffWidth());
            double scale = (hScale<vScale)?hScale:vScale;


            videoWidget->resize( (int)((double)rotatedImageBuffWidth() * scale),
                                 (int)((double)rotatedImageBuffHeight() * scale) );
            break;

//        // Resize the QCaItem to exactly fit the image
//        case SIZE_OPTION_RESIZE:
//            // The top level QFrame of the QEImage needs to be resized, so rather
//            // than determine how large it needs to be to fit the image and whatever
//            // extra is taken up with borders and info widgets, just calculate how
//            // much bigger or smaller the current scroll area widget neesds to be
//            // and increase the Qframe by that much
//            resize( size().width()+rotatedImageBuffWidth()-scrollArea->size().width(),
//                    size().height()+rotatedImageBuffHeight()-scrollArea->size().height() );
//            videoWidget->resize( rotatedImageBuffWidth(), rotatedImageBuffHeight() );
//            break;
    }


    // Determine buffer size
    unsigned long buffSize = IMAGEBUFF_BYTES_PER_PIXEL * imageBuffWidth * imageBuffHeight;

    // Resize buffer
    imageBuff.resize( buffSize );
}

//=================================================================================================

// Add or remove the button bar
void QEImage::manageButtonBar()
{
    if( displayButtonBar )
    {
        buttonGroup->show();
    }
    else
    {
        buttonGroup->hide();
    }
}

// Add or remove the pixel information layout
void QEImage::manageInfoLayout()
{
    if( displayCursorPixelInfo )
    {
        currentCursorPixelLabel->show();
        currentVertPixelLabel->show();
        currentHozPixelLabel->show();
        currentLineLabel->show();
        currentArea1Label->show();
        currentArea2Label->show();
        currentArea3Label->show();
        currentArea4Label->show();
        currentTargetLabel->show();
        currentBeamLabel->show();
    }
    else
    {
        currentCursorPixelLabel->hide();
        currentVertPixelLabel->hide();
        currentHozPixelLabel->hide();
        currentLineLabel->hide();
        currentArea1Label->hide();
        currentArea2Label->hide();
        currentArea3Label->hide();
        currentArea4Label->hide();
        currentTargetLabel->hide();
        currentBeamLabel->hide();
    }
}

// Zoom to the area selected on the image
void QEImage::zoomToArea()
{
    // Determine the x and y zoom factors for the selected area
    // (the user is most likely to have selected an area with an
    // aspect ratio that does not match the current viewport)
    // Note, these zoom factors are the multiple the current zoom
    // must be changed by, not the actual zoom required
    int sizeX = selectedArea1Point2.x()-selectedArea1Point1.x();
    int sizeY = selectedArea1Point2.y()-selectedArea1Point1.y();
    double zoomFactorX = (double)(scrollArea->viewport()->width()) / (double)sizeX;
    double zoomFactorY = (double)(scrollArea->viewport()->height()) / (double)sizeY;

    // Determine which of the zoom factors will display all the selected area
    double zoomFactor = std::min( zoomFactorX, zoomFactorY );

    //Determine the new zoom
    double newZoom = zoomFactor * (double)(videoWidget->width()) / (double)(imageBuffWidth);

    // Ensure the zoom factor will not generate an image that is too large
    double maxDim = 5000;
    if( ((double)(imageBuffWidth) * newZoom ) > maxDim )
    {
        newZoom = (double)maxDim / (double)videoWidget->width();
    }
    if( ((double)(videoWidget->height()) * newZoom ) > maxDim )
    {
        newZoom = (double)maxDim / (double)videoWidget->height();
    }

    // Note the pixel position of the top left of the selected area in the original image
    // This will be the position that should be at the top left in the scroll area.
    QPoint newOrigin = videoWidget->scalePoint( selectedArea1Point1 );

    // Resize the display widget
    int newSizeX = int( (double)(imageBuffWidth) * newZoom );
    int newSizeY = int( (double)(imageBuffHeight) * newZoom );
    videoWidget->resize( newSizeX, newSizeY );

    // Reposition the display widget
    newOrigin.setX( int( -newOrigin.x()*newZoom ) );
    newOrigin.setY( int( -newOrigin.y()*newZoom ) );
    pan( newOrigin );

    // Set current zoom percentage
    zoom = int( newZoom*100.0 );
}

// ROI area 1 changed
void QEImage::roi1Changed()
{
    // Write the ROI variables.
    QEInteger *qca;
    qca = (QEInteger*)getQcaItem( ROI1_X_VARIABLE );
    if( qca ) qca->writeInteger( videoWidget->scaleOrdinate( selectedArea1Point1.x() ));

    qca = (QEInteger*)getQcaItem( ROI1_Y_VARIABLE );
    if( qca ) qca->writeInteger(  videoWidget->scaleOrdinate( selectedArea1Point1.y() ));

    qca = (QEInteger*)getQcaItem( ROI1_W_VARIABLE );
    if( qca ) qca->writeInteger( videoWidget->scaleOrdinate( selectedArea1Point2.x() ) - videoWidget->scaleOrdinate( selectedArea1Point1.x() ));

    qca = (QEInteger*)getQcaItem( ROI1_H_VARIABLE );
    if( qca ) qca->writeInteger( videoWidget->scaleOrdinate( selectedArea1Point2.y() ) - videoWidget->scaleOrdinate( selectedArea1Point1.y() ));

    // !!! should do the above whenever ROI changes?. This function should write to a trigger PV?

    return;
}

// ROI area 2 changed
void QEImage::roi2Changed()
{
    // Write the ROI variables.
    QEInteger *qca;
    qca = (QEInteger*)getQcaItem( ROI2_X_VARIABLE );
    if( qca ) qca->writeInteger( videoWidget->scaleOrdinate( selectedArea2Point1.x() ));

    qca = (QEInteger*)getQcaItem( ROI2_Y_VARIABLE );
    if( qca ) qca->writeInteger(  videoWidget->scaleOrdinate( selectedArea2Point1.y() ));

    qca = (QEInteger*)getQcaItem( ROI2_W_VARIABLE );
    if( qca ) qca->writeInteger( videoWidget->scaleOrdinate( selectedArea2Point2.x() ) - videoWidget->scaleOrdinate( selectedArea2Point1.x() ));

    qca = (QEInteger*)getQcaItem( ROI2_H_VARIABLE );
    if( qca ) qca->writeInteger( videoWidget->scaleOrdinate( selectedArea2Point2.y() ) - videoWidget->scaleOrdinate( selectedArea2Point1.y() ));

    // !!! should do the above whenever ROI changes?. This function should write to a trigger PV?

    return;
}

// ROI area 3 changed
void QEImage::roi3Changed()
{
    // Write the ROI variables.
    QEInteger *qca;
    qca = (QEInteger*)getQcaItem( ROI3_X_VARIABLE );
    if( qca ) qca->writeInteger( videoWidget->scaleOrdinate( selectedArea3Point1.x() ));

    qca = (QEInteger*)getQcaItem( ROI3_Y_VARIABLE );
    if( qca ) qca->writeInteger(  videoWidget->scaleOrdinate( selectedArea3Point1.y() ));

    qca = (QEInteger*)getQcaItem( ROI3_W_VARIABLE );
    if( qca ) qca->writeInteger( videoWidget->scaleOrdinate( selectedArea3Point2.x() ) - videoWidget->scaleOrdinate( selectedArea3Point1.x() ));

    qca = (QEInteger*)getQcaItem( ROI3_H_VARIABLE );
    if( qca ) qca->writeInteger( videoWidget->scaleOrdinate( selectedArea3Point2.y() ) - videoWidget->scaleOrdinate( selectedArea3Point1.y() ));

    // !!! should do the above whenever ROI changes?. This function should write to a trigger PV?

    return;
}

// ROI area 4 changed
void QEImage::roi4Changed()
{
    // Write the ROI variables.
    QEInteger *qca;
    qca = (QEInteger*)getQcaItem( ROI4_X_VARIABLE );
    if( qca ) qca->writeInteger( videoWidget->scaleOrdinate( selectedArea4Point1.x() ));

    qca = (QEInteger*)getQcaItem( ROI4_Y_VARIABLE );
    if( qca ) qca->writeInteger(  videoWidget->scaleOrdinate( selectedArea4Point1.y() ));

    qca = (QEInteger*)getQcaItem( ROI4_W_VARIABLE );
    if( qca ) qca->writeInteger( videoWidget->scaleOrdinate( selectedArea4Point2.x() ) - videoWidget->scaleOrdinate( selectedArea4Point1.x() ));

    qca = (QEInteger*)getQcaItem( ROI4_H_VARIABLE );
    if( qca ) qca->writeInteger( videoWidget->scaleOrdinate( selectedArea4Point2.y() ) - videoWidget->scaleOrdinate( selectedArea4Point1.y() ));

    // !!! should do the above whenever ROI changes?. This function should write to a trigger PV?

    return;
}

// Move target into beam button pressed
void QEImage::targetClicked()
{
    // Write to the target trigger variable.
    QEInteger *qca;
    qca = (QEInteger*)getQcaItem( TARGET_TRIGGER_VARIABLE );
    if( qca ) qca->writeInteger( 1 );

    return;
}

// Pause button pressed
void QEImage::pauseClicked()
{
    if (paused)
    {
        pauseButton->setIcon( *pauseButtonIcon );
        pauseButton->setToolTip("Pause image display");
        paused = false;
    }
    else
    {
        pauseButton->setIcon( *playButtonIcon );
        pauseButton->setToolTip("Resume image display");
        paused = true;
    }
}

// Save button pressed
void QEImage::saveClicked()
{
    QFileDialog *qFileDialog;
    QStringList filterList;
    QString filename;
    bool result;

    qFileDialog = new QFileDialog(this, "Save displayed image", QString());
    filterList << "Tagged Image File Format (*.tiff)" << "Portable Network Graphics (*.png)" << "Windows Bitmap (*.bmp)" << "Joint Photographics Experts Group (*.jpg)";
    qFileDialog->setFilters(filterList);
    qFileDialog->setAcceptMode(QFileDialog::AcceptSave);

    if (qFileDialog->exec())
    {

        QImage qImage((uchar*) imageBuff.constData(), rotatedImageBuffWidth(), rotatedImageBuffHeight(), QImage::Format_RGB32);
        filename = qFileDialog->selectedFiles().at(0);

        if (qFileDialog->selectedNameFilter() == filterList.at(0))
        {
            result = qImage.save(filename, "TIFF");
        }
        else if (qFileDialog->selectedNameFilter() == filterList.at(0))
        {
            result = qImage.save(filename, "PNG");
        }
        else if (qFileDialog->selectedNameFilter() == filterList.at(1))
        {
            result = qImage.save(filename, "BMP");
        }
        else
        {
            result = qImage.save(filename, "JPG");
        }

        if (result)
        {
            QMessageBox::information(this, "Info", "The displayed image was successfully saved in file '" + filename + "'!");
        }
        else
        {
            QMessageBox::critical(this, "Error", "Unable to save displayed image in file '" + filename + "'!");
        }
    }

}

// Update the video widget if the QEImage has changed
void QEImage::resizeEvent(QResizeEvent* )
{
    setImageBuff();
}

//==============================================================================
// Drag drop
void QEImage::setDrop( QVariant drop )
{
    setVariableName( drop.toString(), 0 );
    establishConnection( 0 );
}

QVariant QEImage::getDrop()
{
    if( isDraggingVariable() )
        return QVariant( copyVariable() );
    else
        return copyData();
}

//==============================================================================
// Copy / Paste
QString QEImage::copyVariable()
{
    return getSubstitutedVariableName(0);
}

QVariant QEImage::copyData()
{
    return QVariant( videoWidget->getImage() );
}

void QEImage::paste( QVariant v )
{
    if( getAllowDrop() )
    {
        setDrop( v );
    }
}

//==============================================================================
// Property convenience functions


// Allow user to set the video format
void QEImage::setFormatOption( formatOptions formatOptionIn )
{
    // Save the option
    formatOption = formatOptionIn;

    // Resize and rescale
    // !!! why is this needed??? is formatOption used by the video widget?
    setImageBuff();
}

QEImage::formatOptions QEImage::getFormatOption()
{
    return formatOption;
}

// Set the zoom percentage (and force zoom mode)
void QEImage::setResizeOptionAndZoom( int zoomIn )
{

    // !!! do each of the following two lines call setImageBuff()???
    setResizeOption( RESIZE_OPTION_ZOOM );
    setZoom( zoomIn );
}

// Zoom level
void QEImage::setZoom( int zoomIn )
{
    // Save the zoom
    // (Limit to 10 - 400 %)
    if( zoomIn < 10 )
        zoom = 10;
    else if( zoomIn > 400 )
        zoom = 400;
    else
        zoom = zoomIn;

    // Resize and rescale
    setImageBuff();
}

int QEImage::getZoom()
{
    return zoom;
}

// Rotation
void QEImage::setRotation( rotationOptions rotationIn )
{
    // save the rotation requested
    rotation = rotationIn;

    // Adjust the size of the image to maintain aspect ratio if required
    setImageBuff();

    // Present the updated image
    displayImage();
}

QEImage::rotationOptions QEImage::getRotation()
{
    return rotation;
}

// Horizontal flip
void QEImage::setHorizontalFlip( bool flipHozIn )
{
    flipHoz = flipHozIn;

    // Present the updated image
    displayImage();
}

bool QEImage::getHorizontalFlip()
{
    return flipHoz;
}

// Vertical flip
void QEImage::setVerticalFlip( bool flipVertIn )
{
    flipVert = flipVertIn;

    // Present the updated image
    displayImage();
}

bool QEImage::getVerticalFlip()
{
    return flipVert;
}

// True if local brightness and contrast controls are presented
void QEImage::setEnableBrightnessContrast( bool enableBrightnessContrastIn )
{
    enableBrightnessContrast = enableBrightnessContrastIn;
    brightnessContrastFrame->setVisible( enableBrightnessContrast );
}

bool QEImage::getEnableBrightnessContrast()
{
    return enableBrightnessContrast;
}

// Automatic setting of brightness and contrast on region selection
void QEImage::setAutoBrightnessContrast( bool autoBrightnessContrastIn )
{
    autoBrightnessContrast = autoBrightnessContrastIn;
}

bool QEImage::getAutoBrightnessContrast()
{
    return autoBrightnessContrast;
}

// Resize options
void QEImage::setResizeOption( resizeOptions resizeOptionIn )
{
    // Save the resize option
    resizeOption = resizeOptionIn;

    // Resize and rescale
    setImageBuff();

    // Present the updated image
    displayImage();
}

QEImage::resizeOptions QEImage::getResizeOption()
{
    return resizeOption;
}

// Initial vorizontal scroll position
void QEImage::setInitialHozScrollPos( int initialHozScrollPosIn )
{
    initialHozScrollPos = initialHozScrollPosIn;
    scrollArea->horizontalScrollBar()->setValue( initialHozScrollPos );
}

int QEImage::getInitialHozScrollPos()
{
    return initialHozScrollPos;
}

// Initial vertical scroll position
void QEImage::setInitialVertScrollPos( int initialVertScrollPosIn )
{
    initialVertScrollPos = initialVertScrollPosIn;
    scrollArea->verticalScrollBar()->setValue( initialVertScrollPos );
}

int QEImage::getInitialVertScrollPos()
{
    return initialVertScrollPos;
}

// Display the button bar
void QEImage::setDisplayButtonBar( bool displayButtonBarIn )
{
    displayButtonBar = displayButtonBarIn;
    manageButtonBar();
}

bool QEImage::getDisplayButtonBar()
{
    return displayButtonBar;
}

// Show time
void QEImage::setShowTime(bool value)
{
    videoWidget->setShowTime( value );
}

bool QEImage::getShowTime()
{
    return videoWidget->getShowTime();
}

// Vertical slice markup colour
void QEImage::setVertSliceMarkupColor(QColor markupColor )
{
    videoWidget->setMarkupColor( imageMarkup::MARKUP_ID_V_SLICE, markupColor );
}

QColor QEImage::getVertSliceMarkupColor()
{
    return videoWidget->getMarkupColor( imageMarkup::MARKUP_ID_V_SLICE );
}

// Horizontal slice markup colour
void QEImage::setHozSliceMarkupColor(QColor markupColor )
{
    videoWidget->setMarkupColor( imageMarkup::MARKUP_ID_H_SLICE, markupColor );
}

QColor QEImage::getHozSliceMarkupColor()
{
    return videoWidget->getMarkupColor( imageMarkup::MARKUP_ID_H_SLICE );
}

// Profile markup colour
void QEImage::setProfileMarkupColor(QColor markupColor )
{
    videoWidget->setMarkupColor( imageMarkup::MARKUP_ID_LINE, markupColor );
}

QColor QEImage::getProfileMarkupColor()
{
    return videoWidget->getMarkupColor( imageMarkup::MARKUP_ID_LINE );
}

// Area markup colour
void QEImage::setAreaMarkupColor(QColor markupColor )
{
    videoWidget->setMarkupColor( imageMarkup::MARKUP_ID_REGION1, markupColor );
    videoWidget->setMarkupColor( imageMarkup::MARKUP_ID_REGION2, markupColor );
    videoWidget->setMarkupColor( imageMarkup::MARKUP_ID_REGION3, markupColor );
    videoWidget->setMarkupColor( imageMarkup::MARKUP_ID_REGION4, markupColor );
}

QColor QEImage::getAreaMarkupColor()
{
    return videoWidget->getMarkupColor( imageMarkup::MARKUP_ID_REGION1 ); // same as MARKUP_ID_REGION2, MARKUP_ID_REGION3 and MARKUP_ID_REGION4
}

// Area markup colour
void QEImage::setTimeMarkupColor(QColor markupColor )
{
    videoWidget->setMarkupColor( imageMarkup::MARKUP_ID_TIMESTAMP, markupColor );
}

QColor QEImage::getTimeMarkupColor()
{
    return videoWidget->getMarkupColor( imageMarkup::MARKUP_ID_TIMESTAMP );
}

// Target slice markup colour
void QEImage::setTargetMarkupColor(QColor markupColor )
{
    videoWidget->setMarkupColor( imageMarkup::MARKUP_ID_TARGET, markupColor );
}

QColor QEImage::getTargetMarkupColor()
{
    return videoWidget->getMarkupColor( imageMarkup::MARKUP_ID_TARGET );
}

// Beam slice markup colour
void QEImage::setBeamMarkupColor(QColor markupColor )
{
    videoWidget->setMarkupColor( imageMarkup::MARKUP_ID_BEAM, markupColor );
}

QColor QEImage::getBeamMarkupColor()
{
    return videoWidget->getMarkupColor( imageMarkup::MARKUP_ID_BEAM );
}

// Show cursor pixel
void QEImage::setDisplayCursorPixelInfo( bool displayCursorPixelInfoIn )
{
    displayCursorPixelInfo = displayCursorPixelInfoIn;
    manageInfoLayout();
}

bool QEImage::getDisplayCursorPixelInfo(){
    return displayCursorPixelInfo;
}

// Show contrast reversal
void QEImage::setContrastReversal( bool contrastReversalIn )
{
    contrastReversal = contrastReversalIn;

    qcaobject::QCaObject* qca = getQcaItem( IMAGE_VARIABLE );
    if( qca )
    {
        qca->resendLastData();
    }
}

bool QEImage::getContrastReversal(){
    return contrastReversal;
}

// Enable vertical slice selection
void QEImage::setEnableVertSliceSelection( bool enableVSliceSelectionIn )
{
    enableVSliceSelection = enableVSliceSelectionIn;
    sMenu->setVSliceEnabled( enableVSliceSelection );

    // If disabling, and it is the current mode, then default to panning
    if( !enableVSliceSelection && getSelectionOption() == SO_VSLICE )
    {
        sMenu->setChecked( QEImage::SO_PANNING );
        panModeClicked();
    }
}

bool QEImage::getEnableVertSliceSelection()
{
    return enableVSliceSelection;
}

// Enable horizontal slice selection
void QEImage::setEnableHozSliceSelection( bool enableHSliceSelectionIn )
{
    enableHSliceSelection = enableHSliceSelectionIn;
    sMenu->setHSlicetEnabled( enableHSliceSelection );

    // If disabling, and it is the current mode, then default to panning
    if( !enableHSliceSelection && getSelectionOption() == SO_HSLICE )
    {
        sMenu->setChecked( QEImage::SO_PANNING );
        panModeClicked();
    }
}

bool QEImage::getEnableHozSliceSelection()
{
    return enableHSliceSelection;
}

// Enable area selection (used for ROI and zoom)
void QEImage::setEnableAreaSelection( bool enableAreaSelectionIn )
{
    enableAreaSelection = enableAreaSelectionIn;
    sMenu->setAreaEnabled( enableAreaSelection );

    // If disabling, and it is the current mode, then default to panning
    if( !enableAreaSelection &&
        ( ( getSelectionOption() == SO_AREA1 ) ||
          ( getSelectionOption() == SO_AREA2 ) ||
          ( getSelectionOption() == SO_AREA3 ) ||
          ( getSelectionOption() == SO_AREA4 )))
    {
        sMenu->setChecked( QEImage::SO_PANNING );
        panModeClicked();
    }
}

bool QEImage::getEnableAreaSelection()
{
    return enableAreaSelection;
}

// Enable profile selection
void QEImage::setEnableProfileSelection( bool enableProfileSelectionIn )
{
    enableProfileSelection = enableProfileSelectionIn;
    sMenu->setProfileEnabled( enableProfileSelection );

    // If disabling, and it is the current mode, then default to panning
    if( !enableProfileSelection && getSelectionOption() == SO_PROFILE )
    {
        sMenu->setChecked( QEImage::SO_PANNING );
        panModeClicked();
    }
}

bool QEImage::getEnableProfileSelection()
{
    return enableProfileSelection;
}

// Enable target selection
void QEImage::setEnableTargetSelection( bool enableTargetSelectionIn )
{
    enableTargetSelection = enableTargetSelectionIn;
    sMenu->setTargetEnabled( enableTargetSelection );
    targetButton->setVisible( enableTargetSelection );

    // If disabling, and it is the current mode, then default to panning
    if( !enableTargetSelection && ( getSelectionOption() == SO_TARGET || getSelectionOption() == SO_BEAM ))
    {
        sMenu->setChecked( QEImage::SO_PANNING );
        panModeClicked();
    }
}

bool QEImage::getEnableTargetSelection()
{
    return enableTargetSelection;
}

//=================================================================================================


void QEImage::panModeClicked()
{
    videoWidget->setMode(  imageMarkup::MARKUP_ID_NONE );
    videoWidget->setPanning( true );
}

void QEImage::vSliceSelectModeClicked()
{
    videoWidget->setPanning( false );
    videoWidget->setMode(  imageMarkup::MARKUP_ID_V_SLICE );
}

void QEImage::hSliceSelectModeClicked()
{
    videoWidget->setPanning( false );
    videoWidget->setMode(  imageMarkup::MARKUP_ID_H_SLICE );
}

void QEImage::area1SelectModeClicked()
{
    videoWidget->setPanning( false );
    videoWidget->setMode(  imageMarkup::MARKUP_ID_REGION1 );
}

void QEImage::area2SelectModeClicked()
{
    videoWidget->setPanning( false );
    videoWidget->setMode(  imageMarkup::MARKUP_ID_REGION2 );
}

void QEImage::area3SelectModeClicked()
{
    videoWidget->setPanning( false );
    videoWidget->setMode(  imageMarkup::MARKUP_ID_REGION3 );
}

void QEImage::area4SelectModeClicked()
{
    videoWidget->setPanning( false );
    videoWidget->setMode(  imageMarkup::MARKUP_ID_REGION4 );
}

void QEImage::profileSelectModeClicked()
{
    videoWidget->setPanning( false );
    videoWidget->setMode(  imageMarkup::MARKUP_ID_LINE );
}

void QEImage::targetSelectModeClicked()
{
    videoWidget->setPanning( false );
    videoWidget->setMode(  imageMarkup::MARKUP_ID_TARGET );
}

void QEImage::beamSelectModeClicked()
{
    videoWidget->setPanning( false );
    videoWidget->setMode(  imageMarkup::MARKUP_ID_BEAM);
}
//=================================================================================================

void QEImage::zoomInOut( int zoomAmount )
{
    setResizeOption( RESIZE_OPTION_ZOOM );
    double oldZoom = zoom;
    double newZoom = zoom + zoomAmount;
    setZoom( int( newZoom ) );

    double currentScrollPosX = scrollArea->horizontalScrollBar()->value();
    double currentScrollPosY = scrollArea->verticalScrollBar()->value();
    double newScrollPosX = currentScrollPosX *newZoom / oldZoom;
    double newScrollPosY = currentScrollPosY *newZoom / oldZoom;

    scrollArea->horizontalScrollBar()->setValue( int( newScrollPosX ) );
    scrollArea->verticalScrollBar()->setValue( int( newScrollPosY ) );
}

// The user has made (or is making) a selection in the displayed image.
// Act on the selelection
void QEImage::userSelection( imageMarkup::markupIds mode, bool complete, bool clearing, QPoint point1, QPoint point2 )
{
    // If creating or moving a markup...
    if( !clearing )
    {
        switch( mode )
        {
            case imageMarkup::MARKUP_ID_V_SLICE:
                vSliceX = point1.x();
                haveVSliceX = true;
                vSliceDisplay->setVisible( true );
                generateVSlice(  vSliceX );
                break;

            case imageMarkup::MARKUP_ID_H_SLICE:
                hSliceY = point1.y();
                haveHSliceY = true;
                hSliceDisplay->setVisible( true );
                generateHSlice( hSliceY );
                break;

            case imageMarkup::MARKUP_ID_REGION1:
                selectedArea1Point1 = point1;
                selectedArea1Point2 = point2;
                haveSelectedArea1 = true;

                zMenu->enableAreaSelected( haveSelectedArea1 );

                displaySelectedArea1Info( point1, point2 );

                if( complete )
                {
                    roi1Changed();
                }
                break;

            case imageMarkup::MARKUP_ID_REGION2:
                selectedArea2Point1 = point1;
                selectedArea2Point2 = point2;
                haveSelectedArea2 = true;

                displaySelectedArea2Info( point1, point2 );

                if( complete )
                {
                    roi2Changed();
                }
                break;

            case imageMarkup::MARKUP_ID_REGION3:
                selectedArea3Point1 = point1;
                selectedArea3Point2 = point2;
                haveSelectedArea3 = true;

                displaySelectedArea3Info( point1, point2 );

                if( complete )
                {
                    roi3Changed();
                }
                break;

            case imageMarkup::MARKUP_ID_REGION4:
                selectedArea4Point1 = point1;
                selectedArea4Point2 = point2;
                haveSelectedArea4 = true;

                displaySelectedArea4Info( point1, point2 );

                if( complete )
                {
                    roi4Changed();
                }
                break;

            case imageMarkup::MARKUP_ID_LINE:
                profileLineStart = point1;
                profileLineEnd = point2;
                haveProfileLine = true;
                profileDisplay->setVisible( true );
                generateProfile( profileLineStart, profileLineEnd );
                break;

            case imageMarkup::MARKUP_ID_TARGET:
                {
                    target = point1;

                    // Write the target variables.
                    QEInteger *qca;
                    qca = (QEInteger*)getQcaItem( TARGET_X_VARIABLE );
                    if( qca ) qca->writeInteger( videoWidget->scaleOrdinate( target.x() ));

                    qca = (QEInteger*)getQcaItem( TARGET_Y_VARIABLE );
                    if( qca ) qca->writeInteger(  videoWidget->scaleOrdinate( target.y() ));

                    // Display textual info
                    QString s;
                    s.sprintf( "T: (%d,%d)", target.x(), target.y() );
                    currentTargetLabel->setText( s );

                }
                break;

            case imageMarkup::MARKUP_ID_BEAM:
                {
                    beam = point1;

                    // Write the beam variables.
                    QEInteger *qca;
                    qca = (QEInteger*)getQcaItem( BEAM_X_VARIABLE );
                    if( qca ) qca->writeInteger( videoWidget->scaleOrdinate( beam.x() ));

                    qca = (QEInteger*)getQcaItem( BEAM_Y_VARIABLE );
                    if( qca ) qca->writeInteger(  videoWidget->scaleOrdinate( beam.y() ));

                    // Display textual info
                    QString s;
                    s.sprintf( "B: (%d,%d)", beam.x(), beam.y() );
                    currentBeamLabel->setText( s );
                }
                break;

            default:
                break;

        }
    }

    // If clearing a markup...
    else
    {
        switch( mode )
        {
            case imageMarkup::MARKUP_ID_V_SLICE:
                vSliceX = 0;
                haveVSliceX = false;
                vSliceDisplay->setVisible( false );
                currentVertPixelLabel->clear();
                break;

            case imageMarkup::MARKUP_ID_H_SLICE:
                hSliceY = 0;
                haveHSliceY = false;
                hSliceDisplay->setVisible( false );
                currentHozPixelLabel->clear();
                break;

            case imageMarkup::MARKUP_ID_REGION1:
                selectedArea1Point1 = QPoint();
                selectedArea1Point2 = QPoint();
                haveSelectedArea1 = false;
                currentArea1Label->clear();

                zMenu->enableAreaSelected( haveSelectedArea1 );
                break;

            case imageMarkup::MARKUP_ID_REGION2:
                selectedArea2Point1 = QPoint();
                selectedArea2Point2 = QPoint();
                haveSelectedArea2 = false;
                currentArea2Label->clear();
                break;

            case imageMarkup::MARKUP_ID_REGION3:
                selectedArea3Point1 = QPoint();
                selectedArea3Point2 = QPoint();
                haveSelectedArea3 = false;
                currentArea3Label->clear();
                break;

            case imageMarkup::MARKUP_ID_REGION4:
                selectedArea4Point1 = QPoint();
                selectedArea4Point2 = QPoint();
                haveSelectedArea4 = false;
                currentArea4Label->clear();
                break;

            case imageMarkup::MARKUP_ID_LINE:
                profileLineStart = QPoint();
                profileLineEnd = QPoint();
                haveProfileLine = false;
                profileDisplay->setVisible( false );
                currentLineLabel->clear();
                break;

            case imageMarkup::MARKUP_ID_TARGET:
                currentTargetLabel->clear();
                break;

            case imageMarkup::MARKUP_ID_BEAM:
                currentBeamLabel->clear();
                break;

            default:
                break;

        }
    }
}

// Generate a profile along a line down an image at a given X position
// The profile contains values for each pixel intersected by the line.
void QEImage::generateVSlice( int xUnscaled )
{
    // Scale the ordinate to the original image data
    int x = videoWidget->scaleOrdinate( xUnscaled );

    // Display textual info
    QString s;
    s.sprintf( "V: %d", x );
    currentVertPixelLabel->setText( s );

    // If not over the image, remove the profile
    if( x < 0 || x >= (int)rotatedImageBuffWidth() )
    {
        vSliceDisplay->clearProfile();
        return;
    }

    // Ensure the buffer is the correct size
    if( vSliceData.size() != (int)rotatedImageBuffHeight() )
        vSliceData.resize( rotatedImageBuffHeight() );

    // Set up to step pixel by pixel through the image data along the line
    const unsigned char* data = (unsigned char*)image.constData();
    const unsigned char* dataPtr = &(data[x*imageDataSize]);
    int dataPtrStep = rotatedImageBuffWidth()*imageDataSize;

    // Determine the image data value at each pixel
    // The buffer is filled backwards so the plot, which sits on its side beside the image is drawn correctly
    QPoint pos;
    pos.setX( x );
    for( int i = rotatedImageBuffHeight()-1; i >= 0; i-- )
    {
        pos.setY( i );
        vSliceData[i].setY( i );
        vSliceData[i].setX( getFloatingPixelValueFromData( getImageDataPtr( pos ) ) );
        dataPtr += dataPtrStep;
    }

    // Display the profile
    QDateTime dt = QDateTime::currentDateTime();
    QString title = QString( "Vertical profile - " ).append( getSubstitutedVariableName( IMAGE_VARIABLE ) ).append( dt.toString(" - dd.MM.yyyy HH:mm:ss.zzz") );
    vSliceDisplay->setProfile( &vSliceData, maxPixelValue(), 0.0, (double)(vSliceData.size()), 0.0, title, QPoint( x, 0 ), QPoint( x, rotatedImageBuffHeight()-1 ) );
}

// Determine the maximum pixel value for the current format
double QEImage::maxPixelValue()
{
    switch( formatOption )
    {
        default:
        case GREY8:  return (1<<8)-1;
        case GREY16: return (1<<16)-1;
        case GREY12: return (1<<12)-2;
        case RGB_888:return (1<<8)-1;
    }
}

// Return a pointer to pixel data in the original image data.
// The position parameter is scaled to the original image size but reflects
// the displayed rotation and flip options, so it must be transformed first.
const unsigned char* QEImage::getImageDataPtr( QPoint& pos )
{
    QPoint posTr;

    // Transform the position to reflect the original unrotated or flipped data
    posTr = rotateFLipPoint( pos );

    const unsigned char* data = (unsigned char*)image.constData();
    return &(data[(posTr.x()+posTr.y()*imageBuffWidth)*imageDataSize]);
}

// Display textual info about the first selected area
void QEImage::displaySelectedArea1Info( QPoint point1, QPoint point2 )
{
    // Display textual info
    QString s;
    s.sprintf( "R1: (%d,%d)(%d,%d)", videoWidget->scaleOrdinate( point1.x() ),
                                     videoWidget->scaleOrdinate( point1.y() ),
                                     videoWidget->scaleOrdinate( point2.x() ),
                                     videoWidget->scaleOrdinate( point2.y() ));
    currentArea1Label->setText( s );
}

// Display textual info about the second selected area
void QEImage::displaySelectedArea2Info( QPoint point1, QPoint point2 )
{
    // Display textual info
    QString s;
    s.sprintf( "R2: (%d,%d)(%d,%d)", videoWidget->scaleOrdinate( point1.x() ),
                                     videoWidget->scaleOrdinate( point1.y() ),
                                     videoWidget->scaleOrdinate( point2.x() ),
                                     videoWidget->scaleOrdinate( point2.y() ));
    currentArea2Label->setText( s );
}

// Display textual info about the third selected area
void QEImage::displaySelectedArea3Info( QPoint point1, QPoint point2 )
{
    // Display textual info
    QString s;
    s.sprintf( "R3: (%d,%d)(%d,%d)", videoWidget->scaleOrdinate( point1.x() ),
                                     videoWidget->scaleOrdinate( point1.y() ),
                                     videoWidget->scaleOrdinate( point2.x() ),
                                     videoWidget->scaleOrdinate( point2.y() ));
    currentArea3Label->setText( s );
}

// Display textual info about the fourth selected area
void QEImage::displaySelectedArea4Info( QPoint point1, QPoint point2 )
{
    // Display textual info
    QString s;
    s.sprintf( "R4: (%d,%d)(%d,%d)", videoWidget->scaleOrdinate( point1.x() ),
                                     videoWidget->scaleOrdinate( point1.y() ),
                                     videoWidget->scaleOrdinate( point2.x() ),
                                     videoWidget->scaleOrdinate( point2.y() ));
    currentArea4Label->setText( s );
}

void QEImage::autoBrightnessCheckBoxChanged( QEvent * )
{

//    autoBrightnessContrast = get from event
    qDebug() << "autoBrightnessCheckBoxChanged()";
}

// The local brightness slider has been moved
void QEImage::brightnessSliderValueChanged( int localBrightnessIn )
{
    qDebug() << "brightnessSliderValueChanged()";
    localBrightness = (double)(localBrightnessIn)/100.0;
}

// The local contrast slider has been moved
void QEImage::contrastSliderValueChanged( int localContrastIn )
{
    qDebug() << "contrastSliderValueChanged()";
    localContrast =  (double)(localContrastIn)/100.0;
}

// Generate a profile along a line across an image at a given Y position
// The profile contains values for each pixel intersected by the line.
void QEImage::generateHSlice( int yUnscaled )
{
    // Scale the ordinate to the original image data
    int y = videoWidget->scaleOrdinate( yUnscaled );

    // Display textual info
    QString s;
    s.sprintf( "H: %d", y );
    currentHozPixelLabel->setText( s );

    // If not over the image, remove the profile
    if( y < 0 || y >= (int)rotatedImageBuffHeight() )
    {
        hSliceDisplay->clearProfile();
        return;
    }

    // Ensure the buffer is the correct size
    if( hSliceData.size() != (int)rotatedImageBuffWidth() )
        hSliceData.resize( rotatedImageBuffWidth() );

    // Set up to step pixel by pixel through the image data along the line
    const unsigned char* data = (unsigned char*)image.constData();
    const unsigned char* dataPtr = &(data[y*rotatedImageBuffWidth()*imageDataSize]);
    int dataPtrStep = imageDataSize;

    // Determine the image data value at each pixel
    QPoint pos;
    pos.setY( y );
    for( unsigned int i = 0; i < rotatedImageBuffWidth(); i++ )
    {
        pos.setX( i );
        hSliceData[i].setX( i );
        hSliceData[i].setY( getFloatingPixelValueFromData( getImageDataPtr( pos ) ) );
        dataPtr += dataPtrStep;
    }

    // Display the profile
    QDateTime dt = QDateTime::currentDateTime();
    QString title = QString( "Horizontal profile - " ).append( getSubstitutedVariableName( IMAGE_VARIABLE ) ).append( dt.toString(" - dd.MM.yyyy HH:mm:ss.zzz") );
    hSliceDisplay->setProfile( &hSliceData, 0.0, (double)(hSliceData.size()), 0.0,  maxPixelValue(), title, QPoint( y, 0 ), QPoint( y, rotatedImageBuffWidth()-1 ) );
}

// Generate a profile along an arbitrary line through an image.
// The profile contains values one pixel length along the line.
// Except where the line is vertical or horizontal points one pixel
// length along the line will not line up with actual pixels.
// The values returned are a weighted average of the four actual pixels
// containing a notional pixel drawn around the each point on the line.
//
// In the example below, a line was drawn from pixels (1,1) to (3,3).
//
// The starting and ending points are the center of the start and end
// pixels: (1.5,1.5)  (3.5,3.5)
//
// The points along the line one pixel length apart are roughly at points
// (1.5,1.5) (2.2,2.2) (2.9,2.9) (3.6,3.6)
//
// The points are marked in the example with an 'x'.
// The second point has a notional pixel drawn around it like so:
//      .........
//      .       .
//      .       .
//      .   x   .
//      .       .
//      .........
//
// This notional pixel overlaps pixels (1,1) (1,2) (2,1) and (2,2).
//
// The notional pixel overlaps about 10% of pixel (1,1),
// 20% of pixels (1,2) and (2,1) and 50% of pixel (2,2).
//
// A value for the second point will be the sum of the four pixels
// overlayed by the notional pixel weighted by these values.
//
//     0       1       2       3       4
//   +-------+-------+-------+-------+-------+
//   |       |       |       |       |       |
// 0 |       |       |       |       |       |
//   |       |       |       |       |       |
//   +-------+-------+-------+-------+-------+
//   |       |       |       |       |       |
// 1 |       |   x ......... |       |       |
//   |       |     . |     . |       |       |
//   +-------+-----.-+-----.-+-------+-------+
//   |       |     . | x   . |       |       |
// 2 |       |     . |     . |       |       |
//   |       |     .........x|       |       |
//   +-------+-------+-------+-------+-------+
//   |       |       |       |       |       |
// 3 |       |       |       |   x   |       |
//   |       |       |       |       |       |
//   +-------+-------+-------+-------+-------+
//   |       |       |       |       |       |
// 4 |       |       |       |       |       |
//   |       |       |       |       |       |
//   +-------+-------+-------+-------+-------+
//
//  Formulas for x and y values along the selected line:
//
//  d = line length
//  x = x component
//  y = y component
//  s = slope
//
//  s = y / x
//  y = sx

//  d^2 = x^2 + y^2
//  d^2 = x^2 + (sx)^2
//  d^2 = x^2 + s^2 + x^2
//  d^2 = x^2(1 + s^2)
//  d^2 / (1 + s^2) = x^2
//  sqrt( d^2 / (1 + s^2) ) = x
//
void QEImage::generateProfile( QPoint point1Unscaled, QPoint point2Unscaled )
{
    // Scale the coordinates to the original image data
    QPoint point1 = videoWidget->scalePoint( point1Unscaled );
    QPoint point2 = videoWidget->scalePoint( point2Unscaled );

    // Display textual information
    QString s;
    s.sprintf( "L: (%d,%d)(%d,%d)", point1.x(), point1.y(), point2.x(), point2.y() );
    currentLineLabel->setText( s );

    // X and Y components of line drawn
    double dX = point2.x()-point1.x();
    double dY = point2.y()-point1.y();

    // Do nothing if no line
    if( dX == 0 && dY == 0 )
    {
        profileDisplay->clearProfile();
        return;
    }

    // Line length
    double len = sqrt( dX*dX+dY*dY );

    // Line slope
    // To handle infinite slope, switch slope between x/y or y/x as appropriate
    // This is more accurate for otherwise very steep slopes, and does not need a
    // special case for infinite slopes
    double slopeY = 0.0; // Y/X
    double slopeX = 0.0; // X/Y
    bool useSlopeY;
    if( dX != 0 )
    {
        slopeY = dY/dX;
        useSlopeY = true;
    }
    else
    {
        slopeX = dX/dY;
        useSlopeY = false;
    }

    // Determine the direction of the line relative the the start point
    double dirX, dirY;
    (dX<0)?dirX=-1.0:dirX=+1.0;
    (dY<0)?dirY=-1.0:dirY=+1.0;

    // Starting point in center of start pixel
    double initX = point1.x()+0.5;
    double initY = point1.y()+0.5;

    // Ensure output buffer is the correct size
    if( profileData.size() != len ) {
       profileData.resize( int( len ) );
    }

    // Calculate a value for each pixel length along the selected line
    for( int i = 0; i < (int) len; i++ )
    {
        // Determine the next 'point' on the line
        // Each point is one pixel length from the last.
        double x, y;
        if( useSlopeY)
        {
            x = initX + dirX * sqrt((double)(i*i) / (1 + slopeY*slopeY));
            y = initY + (x-initX) * slopeY;
        }
        else
        {
            y = initY + dirY * sqrt((double)(i*i) / (1 + slopeX*slopeX));
            x = initX + (y-initY) * slopeX;
        }

        // Calculate the value if the point is within the image (user can drag outside the image)
        double value;
        if( x >= 0 && x < rotatedImageBuffWidth() && y >= 0 && y < rotatedImageBuffHeight() )
        {

            // Determine the top left of the notional pixel that will be measured
            // The notional pixel is one pixel length both dimensions and will not
            // nessesarily overlay a single real pixel
            double xTL = x-0.5;
            double yTL = y-0.5;

            // Determine the top left actual pixel of the four actual pixels that
            // the notional pixel overlays, and the fractional part of a pixel that
            // the notional pixel is offset by.
            double xTLi, xTLf; // i = integer part, f = fractional part
            double yTLi, yTLf; // i = integer part, f = fractional part

            xTLf = modf( xTL, & xTLi );
            yTLf = modf( yTL, & yTLi );

            // For each of the four actual pixels that the notional pixel overlays,
            // determine the proportion of the actual pixel covered by the notional pixel
            double propTL = (1.0-xTLf)*(1-yTLf);
            double propTR = (xTLf)*(1-yTLf);
            double propBL = (1.0-xTLf)*(yTLf);
            double propBR = (xTLf)*(yTLf);

            // Determine a pointer into the image data for each of the four actual pixels overlayed by the notional pixel
            int actualXTL = (int)xTLi;
            int actualYTL = (int)yTLi;
            QPoint posTL( actualXTL,   actualYTL );
            QPoint posTR( actualXTL+1, actualYTL );
            QPoint posBL( actualXTL,   actualYTL+1 );
            QPoint posBR( actualXTL+1, actualYTL+1 );

            const unsigned char* dataPtrTL = getImageDataPtr( posTL );
            const unsigned char* dataPtrTR = getImageDataPtr( posTR );
            const unsigned char* dataPtrBL = getImageDataPtr( posBL );
            const unsigned char* dataPtrBR = getImageDataPtr( posBR );

            // Determine the value of the notional pixel from a weighted average of the four real pixels it overlays.
            // The larger the proportion of the real picture overlayed, the greated the weight.
            // (Ignore pixels outside the image)
            int pixelsInValue = 0;
            value = 0;
            if( xTLi >= 0 && yTLi >= 0 )
            {
                value += propTL * getFloatingPixelValueFromData( dataPtrTL );
                pixelsInValue++;
            }

            if( xTLi+1 < rotatedImageBuffWidth() && yTLi >= 0 )
            {
                value += propTR * getFloatingPixelValueFromData( dataPtrTR );
                pixelsInValue++;
            }

            if( xTLi >= 0 && yTLi+1 < rotatedImageBuffHeight() )
            {

                value += propBL * getFloatingPixelValueFromData( dataPtrBL );
                pixelsInValue++;
            }
            if( xTLi+1 < rotatedImageBuffWidth() && yTLi+1 < rotatedImageBuffHeight() )
            {
                value += propBR * getFloatingPixelValueFromData( dataPtrBR );
                pixelsInValue++;
            }

            value = value / pixelsInValue * 4;
        }

        // Use a value of zero if the point is not within the image (user can drag outside the image)
        else
        {
            value = 0.0;
        }

        // Set the data value
        profileData[i].setX( i );
        profileData[i].setY( value );
    }

    // Update the profile display
    QDateTime dt = QDateTime::currentDateTime();
    QString title = QString( "Line profile - " ).append( getSubstitutedVariableName( IMAGE_VARIABLE ) ).append( dt.toString(" - dd.MM.yyyy HH:mm:ss.zzz") );
    profileDisplay->setProfile( &profileData, 0.0, (double)(profileData.size()), 0.0,  maxPixelValue(), title, point1, point2  );
}

//=================================================================================================


// Return a floating point number given a pointer into an image data buffer.
// Note, the pointer is indexed according to the pixel data size which will be at least
// big enough for the data format.
int QEImage::getPixelValueFromData( const unsigned char* ptr )
{
    // Case the data to the correct size, then return the data as a floating point number.
    switch( formatOption )
    {
        default:
        case GREY8:
            return *ptr;

        case GREY16:
            return *(unsigned short*)ptr;

        case GREY12:
            return *(unsigned short*)ptr;

        case RGB_888:
            unsigned int pixel = *(unsigned int*)ptr;
            return ((pixel&0xff0000>>16) + (pixel&0x00ff00>>8) + (pixel&0x0000ff)) / 3;
    }
}

// Return a floating point number given a pointer to a value of an arbitrary size in a char* buffer.
double QEImage::getFloatingPixelValueFromData( const unsigned char* ptr )
{
    return getPixelValueFromData( ptr );
}

// Transform the point according to current rotation and flip options.
QPoint QEImage::rotateFLipPoint( QPoint& pos )
{
    // Transform the point according to current rotation and flip options.
    // Depending on the flipping and rotating options pixel drawing can start in any of
    // the four corners and start scanning either vertically or horizontally.
    // The 8 scanning options are shown numbered here:
    //
    //    o----->1         2<-----o
    //    |                       |
    //    |                       |
    //    |                       |
    //    v                       v
    //    5                       6
    //
    //
    //
    //    7                       8
    //    ^                       ^
    //    |                       |
    //    |                       |
    //    |                       |
    //    o----->3         4<-----o
    //
    // A point from a rotated and fliped image needs to be transformed to be able to
    // reference pixel data in the original data buffer.
    // Base the transformation on the scanning option used when building the image
    int w = (int)imageBuffWidth-1;
    int h = (int)imageBuffHeight-1;
    QPoint posTr;
    int scanOption = getScanOption();
    switch( scanOption )
    {
        default:
        case 1: posTr = pos;                                      break;
        case 2: posTr.setX( w-pos.x() ); posTr.setY( pos.y() );   break;
        case 3: posTr.setX( pos.x() );   posTr.setY( h-pos.y() ); break;
        case 4: posTr.setX( w-pos.x() ); posTr.setY( h-pos.y() ); break;
        case 5: posTr.setX( pos.y() );   posTr.setY( pos.x() );   break;
        case 6: posTr.setX( w-pos.y() ); posTr.setY( pos.x() );   break;
        case 7: posTr.setX( pos.y() );   posTr.setY( h-pos.x() ); break;
        case 8: posTr.setX( w-pos.y() ); posTr.setY( h-pos.x() ); break;
    }

    return posTr;
}

//=================================================================================================
// Display a pixel value.
void QEImage::currentPixelInfo( QPoint pos )
{
    // If the pixel is not within the image, display nothing
    QString s;
    if( pos.x() < 0 || pos.y() < 0 || pos.x() >= (int)rotatedImageBuffWidth() || pos.y() >= (int)rotatedImageBuffHeight() )
    {
        s = "";
    }

    // If the pixel is within the image, display the pixel position and value
    else
    {
        // Extract the pixel data from the original image data
        int value = getPixelValueFromData( getImageDataPtr( pos ) );
        s.sprintf( "(%d,%d)=%d", pos.x(), pos.y(), value );
    }
    currentCursorPixelLabel->setText( s );
}

// Return the image width following any rotation
unsigned int QEImage::rotatedImageBuffWidth()
{
    switch( rotation)
    {
        default:
        case ROTATION_0:
        case ROTATION_180:
            return imageBuffWidth;

        case ROTATION_90_RIGHT:
        case ROTATION_90_LEFT:
            return imageBuffHeight;
    }
}

// Return the image height following any rotation
unsigned int QEImage::rotatedImageBuffHeight()
{
    switch( rotation)
    {
        default:
        case ROTATION_0:
        case ROTATION_180:
            return imageBuffHeight;

        case ROTATION_90_RIGHT:
        case ROTATION_90_LEFT:
            return imageBuffWidth;
    }
}


// Determine the way the input pixel data must be scanned to accommodate the required
// rotate and flip options. This is used when generating the image data, and also when
// transforming points in the image back to references in the original pixel data.
int QEImage::getScanOption()
{
    // Depending on the flipping and rotating options pixel drawing can start in any of
    // the four corners and start scanning either vertically or horizontally.
    // The 8 scanning options are shown numbered here:
    //
    //    o----->1         2<-----o
    //    |                       |
    //    |                       |
    //    |                       |
    //    v                       v
    //    5                       6
    //
    //
    //
    //    7                       8
    //    ^                       ^
    //    |                       |
    //    |                       |
    //    |                       |
    //    o----->3         4<-----o
    //
    //
    // The rotation and flip properties can be set in 16 combinations, but these 16
    // options can only specify the 8 possible scan options as follows:
    // (for example rotating 180 degrees, then flipping both vertically and horizontally
    // is the same as doing no rotation or flipping at all - scan option 1)
    //
    //  rot vflip hflip scan_option
    //    0   0     0      1
    //    0   0     1      2
    //    0   1     0      3
    //    0   1     1      4
    //  R90   0     0      7
    //  R90   0     1      5
    //  R90   1     0      8
    //  R90   1     1      6
    //  L90   0     0      6
    //  L90   0     1      8
    //  L90   1     0      5
    //  L90   1     1      7
    //  180   0     0      4
    //  180   0     1      3
    //  180   1     0      2
    //  180   1     1      1
    //
    // Determine the scan option as shown in the above diagram
    switch( rotation )
    {                                               // vh v!h     !vh !v!h
        case ROTATION_0:        return flipVert?flipHoz?4:3:flipHoz?2:1;
        case ROTATION_90_RIGHT: return flipVert?flipHoz?6:8:flipHoz?5:7;
        case ROTATION_90_LEFT:  return flipVert?flipHoz?7:5:flipHoz?8:6;
        case ROTATION_180:      return flipVert?flipHoz?1:2:flipHoz?3:4;
        default:                return 1; // Sanity check
    }
}

// Pan the image.
// This is used when:
//   - Zooming to a selected area (zoom to the right level, then call this
//     method to move the selected area into view).
//   - Resetting the scroll bars after the user has panned by dragging the image.
//
// Note: when the user is panning by dragging the image, this method is only used to tidy
// up the scroll bars at the end of the pan.
// Panning has been done by moving the VideoWidget in the viewport directly (not via the
// scroll bars) as the VideoWidget can be moved directly more smoothly to pixel resolution,
// whereas the VideoWidget can only be moved by the resolution of a scrollbar step when moved
// by setting the scroll bar values.
// A consequence of this is, however, the scroll bars are left where ever they were
// when panning started. This function will set the scroll bars to match the new
// VideoWidget position. Note, if the scroll bar values are changed here, this will itself
// cause the VideoWidget to pan, but only from the pixel accurate position set by the
// direct scan to a close pixel determined by the scroll bar pixel resolution.
// Note, the VideoWidget can be panned with the mouse beyond the scroll range. If either
// scroll bar value is changed here the VideoWidget will be pulled back within the scroll
// bar range. If neither scroll bar value changes here, the VideoWidget is left panned
// beyond the scroll bar range. To demonstrate this, set both scroll bars to zero,
// then pan the viewport down and to the right with the mouse.
void QEImage::pan( QPoint origin )
{
    // Determine the proportion of the scroll bar maximums to set the scroll bar to.
    // The scroll bar will be zero when the VideoWidget origin is zero, and maximum when the
    // part of the VideoWidget past the origin equals the viewport size.
    QSize vpSize = scrollArea->viewport()->size();

    double xProportion = (double)-origin.x()/(double)(videoWidget->width()-vpSize.width());
    double yProportion = (double)-origin.y()/(double)(videoWidget->height()-vpSize.height());

    xProportion = (xProportion<0.0)?0.0:xProportion;
    yProportion = (yProportion<0.0)?0.0:yProportion;

    xProportion = (xProportion>1.0)?1.0:xProportion;
    yProportion = (yProportion>1.0)?1.0:yProportion;

    // Update the scroll bars to match the panning
    scrollArea->horizontalScrollBar()->setValue( int( scrollArea->horizontalScrollBar()->maximum() * xProportion ) );
    scrollArea->verticalScrollBar()->setValue( int( scrollArea->verticalScrollBar()->maximum() * yProportion ) );
}

void QEImage::showContextMenu( const QPoint& pos )
{
    // Get the overall position on the display
    QPoint globalPos = mapToGlobal( pos );

    // If the markup system wants to put up a menu, let it do so
    // For example, if the user has clicked over a markup, it may offer the user a menu
    if( videoWidget->showMarkupMenu( videoWidget->mapFrom( this, pos ), globalPos ) )
    {
        return;
    }

    // Create the image context menu
    imageContextMenu menu;

    // add the standard context menu as a sub menu
    menu.addMenu( getContextMenu() );

    // Add the Selection menu
    sMenu->setChecked( getSelectionOption() );
    menu.addMenu( sMenu );

    // Add menu items

    //                      Title                            checkable  checked                 option
    menu.addMenuItem(       "Save...",                       false,     false,                  imageContextMenu::ICM_SAVE                     );
    menu.addMenuItem(       paused?"Resume":"Pause",         true,      paused,                 imageContextMenu::ICM_PAUSE                    );
    menu.addMenuItem(       "Show time",                     true,      showTimeEnabled,        imageContextMenu::ICM_ENABLE_TIME              );
    menu.addMenuItem(       "Show cursor pixel info",        true,      displayCursorPixelInfo, imageContextMenu::ICM_ENABLE_CURSOR_PIXEL      );
    menu.addMenuItem(       "Contrast reversal",             true,      contrastReversal,       imageContextMenu::ICM_ENABLE_CONTRAST_REVERSAL );

    // Add the zoom menu
    zMenu->enableAreaSelected( haveSelectedArea1 );
    menu.addMenu( zMenu );

    // Add the flip/rotate menu
    frMenu->setChecked( rotation, flipHoz, flipVert );
    menu.addMenu( frMenu );

    // Add option menu items
    menu.addOptionMenuItem( "Enable vertical profile selection",                      true,      enableVSliceSelection,    imageContextMenu::ICM_ENABLE_VERT                 );
    menu.addOptionMenuItem( "Enable horizontal profile selection",                    true,      enableHSliceSelection,    imageContextMenu::ICM_ENABLE_HOZ                  );
    menu.addOptionMenuItem( "Enable region selection (required for 'zoom to area'')", true,      enableAreaSelection,      imageContextMenu::ICM_ENABLE_AREA                 );
    menu.addOptionMenuItem( "Enable arbitrary profile selection",                     true,      enableProfileSelection,   imageContextMenu::ICM_ENABLE_LINE                 );
    menu.addOptionMenuItem( "Enable target selection",                                true,      enableTargetSelection,    imageContextMenu::ICM_ENABLE_TARGET               );
    menu.addOptionMenuItem( "Display button bar",                                     true,      displayButtonBar,         imageContextMenu::ICM_DISPLAY_BUTTON_BAR          );
    menu.addOptionMenuItem( "Display local brightness and contrast controls",         true,      enableBrightnessContrast, imageContextMenu::ICM_DISPLAY_BRIGHTNESS_CONTRAST );

    // Present the menu
    imageContextMenu::imageContextMenuOptions option;
    bool checked;
    menu.getContextMenuOption( globalPos, &option, &checked );

    // Act on the menu selection
    switch( option )
    {
        default:
        case imageContextMenu::ICM_NONE: break;

        case imageContextMenu::ICM_SAVE:                        saveClicked();                          break;
        case imageContextMenu::ICM_PAUSE:                       pauseClicked();                         break;
        case imageContextMenu::ICM_ENABLE_CURSOR_PIXEL:         setDisplayCursorPixelInfo  ( checked ); break;
        case imageContextMenu::ICM_ENABLE_CONTRAST_REVERSAL:    setContrastReversal        ( checked ); break;
        case imageContextMenu::ICM_ENABLE_TIME:                 setShowTime                ( checked ); break;
        case imageContextMenu::ICM_ENABLE_VERT:                 setEnableVertSliceSelection( checked ); break;
        case imageContextMenu::ICM_ENABLE_HOZ:                  setEnableHozSliceSelection ( checked ); break;
        case imageContextMenu::ICM_ENABLE_AREA:                 setEnableAreaSelection     ( checked ); break;
        case imageContextMenu::ICM_ENABLE_LINE:                 setEnableProfileSelection  ( checked ); break;
        case imageContextMenu::ICM_ENABLE_TARGET:               setEnableTargetSelection   ( checked ); break;
        case imageContextMenu::ICM_DISPLAY_BUTTON_BAR:          setDisplayButtonBar        ( checked ); break;
        case imageContextMenu::ICM_DISPLAY_BRIGHTNESS_CONTRAST: setEnableBrightnessContrast( checked ); break;

        // Note, zoom options caught by zoom menu signal
        // Note, rotate and flip options caught by flip rotate menu signal
    }
}

void QEImage::zoomMenuTriggered( QAction* selectedItem )
{
    switch( (imageContextMenu::imageContextMenuOptions)(selectedItem->data().toInt()) )
    {
        default:
        case imageContextMenu::ICM_NONE: break;

        case imageContextMenu::ICM_ZOOM_SELECTED:       zoomToArea();                           break;
        case imageContextMenu::ICM_ZOOM_FIT:            setResizeOption( RESIZE_OPTION_FIT );   break;
        case imageContextMenu::ICM_ZOOM_10:             setResizeOptionAndZoom(  10 );          break;
        case imageContextMenu::ICM_ZOOM_25:             setResizeOptionAndZoom(  25 );          break;
        case imageContextMenu::ICM_ZOOM_50:             setResizeOptionAndZoom(  50 );          break;
        case imageContextMenu::ICM_ZOOM_75:             setResizeOptionAndZoom(  75 );          break;
        case imageContextMenu::ICM_ZOOM_100:            setResizeOptionAndZoom( 100 );          break;
        case imageContextMenu::ICM_ZOOM_150:            setResizeOptionAndZoom( 150 );          break;
        case imageContextMenu::ICM_ZOOM_200:            setResizeOptionAndZoom( 200 );          break;
        case imageContextMenu::ICM_ZOOM_300:            setResizeOptionAndZoom( 300 );          break;
        case imageContextMenu::ICM_ZOOM_400:            setResizeOptionAndZoom( 400 );          break;
    }
}

void QEImage::flipRotateMenuTriggered( QAction* selectedItem )
{
    switch( (imageContextMenu::imageContextMenuOptions)(selectedItem->data().toInt()) )
    {
        default:
        case imageContextMenu::ICM_NONE: break;

        case imageContextMenu::ICM_ROTATE_NONE:         setRotation( ROTATION_0 );                      break;
        case imageContextMenu::ICM_ROTATE_RIGHT:        setRotation( ROTATION_90_RIGHT );               break;
        case imageContextMenu::ICM_ROTATE_LEFT:         setRotation( ROTATION_90_LEFT );                break;
        case imageContextMenu::ICM_ROTATE_180:          setRotation( ROTATION_180 );                    break;
        case imageContextMenu::ICM_FLIP_HORIZONTAL:     setHorizontalFlip( selectedItem->isChecked() ); break;
        case imageContextMenu::ICM_FLIP_VERTICAL:       setVerticalFlip  ( selectedItem->isChecked() ); break;
    }

    // Update the checked state of the buttons now the user has selected an option.
    // Note, this is also called before displaying the menu to reflect any property
    // changes from other sources
    frMenu->setChecked( rotation, flipHoz, flipVert );
}

void QEImage::selectMenuTriggered( QAction* selectedItem )
{
    switch( (imageContextMenu::imageContextMenuOptions)(selectedItem->data().toInt()) )
    {
        default:
        case imageContextMenu::ICM_NONE: break;

        case imageContextMenu::ICM_SELECT_PAN:          panModeClicked();           break;
        case imageContextMenu::ICM_SELECT_VSLICE:       vSliceSelectModeClicked();  break;
        case imageContextMenu::ICM_SELECT_HSLICE:       hSliceSelectModeClicked();  break;
        case imageContextMenu::ICM_SELECT_AREA1:        area1SelectModeClicked();    break;
        case imageContextMenu::ICM_SELECT_AREA2:        area2SelectModeClicked();    break;
        case imageContextMenu::ICM_SELECT_AREA3:        area3SelectModeClicked();    break;
        case imageContextMenu::ICM_SELECT_AREA4:        area4SelectModeClicked();    break;
        case imageContextMenu::ICM_SELECT_PROFILE:      profileSelectModeClicked(); break;
        case imageContextMenu::ICM_SELECT_TARGET:       targetSelectModeClicked();  break;
        case imageContextMenu::ICM_SELECT_BEAM:         beamSelectModeClicked();    break;
    }
}

// Get the current selection option
QEImage::selectOptions QEImage::getSelectionOption()
{
    if( videoWidget->getPanning() )
    {
        return SO_PANNING;
    }
    else
    {
        switch( videoWidget->getMode() )
        {
        case imageMarkup::MARKUP_ID_V_SLICE:  return SO_VSLICE;
        case imageMarkup::MARKUP_ID_H_SLICE:  return SO_HSLICE;
        case imageMarkup::MARKUP_ID_REGION1:  return SO_AREA1;
        case imageMarkup::MARKUP_ID_REGION2:  return SO_AREA2;
        case imageMarkup::MARKUP_ID_REGION3:  return SO_AREA3;
        case imageMarkup::MARKUP_ID_REGION4:  return SO_AREA4;
        case imageMarkup::MARKUP_ID_LINE:     return SO_PROFILE;
        case imageMarkup::MARKUP_ID_TARGET:   return SO_TARGET;
        case imageMarkup::MARKUP_ID_BEAM:     return SO_BEAM;

        default:
        case imageMarkup::MARKUP_ID_NONE:    return SO_NONE;

        }
    }
}
