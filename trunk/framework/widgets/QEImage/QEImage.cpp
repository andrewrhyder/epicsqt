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

#include <QIcon>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include <QECommon.h>
#include <QEImage.h>
#include <QEByteArray.h>
#include <QEInteger.h>
#include <QEFloating.h>
#include <QEString.h>
#include <imageContextMenu.h>
#include <windowCustomisation.h>

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
    infoUpdateZoom( zoom );

    rotation = ROTATION_0;
    flipVert = false;
    flipHoz = false;

    initialHozScrollPos = 0;
    initialVertScrollPos = 0;
    initScrollPosSet = false;

    mFormatOption = imageDataFormats::MONO;
    bitDepth = 8;

    paused = false;
    infoUpdatePaused( paused );
    pauseExternalAction = NULL;

    vSliceThickness = 1;
    hSliceThickness = 1;
    profileThickness = 1;

    vSliceX = 0;
    hSliceY = 0;

    haveVSliceX = false;
    haveHSliceY = false;
    haveProfileLine = false;
    haveSelectedArea1 = false;
    haveSelectedArea2 = false;
    haveSelectedArea3 = false;
    haveSelectedArea4 = false;

    enableProfilePresentation = true;
    enableHozSlicePresentation = true;
    enableVertSlicePresentation = true;

    // Set the initial state
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;

    imageDataSize = 0;
    elementsPerPixel = 1;
    bytesPerPixel = 0;

    clippingOn = false;
    clippingLow = 0;
    clippingHigh = 0;

    pixelLookupValid = false;
    pixelLookup.resize( 256*sizeof(rgbPixel) );

    appHostsControls = false;
    hostingAppAvailable = false;

    receivedImageSize = 0;

    displayMarkups = false;

    fullScreen = false;
    fullScreenMainWindow = NULL;

    // With so many variables involved, don't bother alterning the presentation of the widget when any one variable goes into alarm
    setDisplayAlarmState( false );

    // Prepare to interact with whatever application is hosting this widget.
    // For example, the QEGui application can host docks and toolbars for QE widgets
    if( isProfileDefined() )
    {
        // Setup a signal to request component hosting.
        QObject* launcher = getGuiLaunchConsumer();
        if( launcher )
        {
            QObject::connect( this, SIGNAL( componentHostRequest( const QEActionRequests& ) ),
                              launcher, SLOT( requestAction( const QEActionRequests& ) ) );
            hostingAppAvailable = true;
        }
    }

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
    setEllipseMarkupColor(   QColor(255, 127, 255));

    QObject::connect( videoWidget, SIGNAL( userSelection( imageMarkup::markupIds, bool, bool, QPoint, QPoint, unsigned int ) ),
                      this,        SLOT  ( userSelection( imageMarkup::markupIds, bool, bool, QPoint, QPoint, unsigned int )) );
    QObject::connect( videoWidget, SIGNAL( zoomInOut( int ) ),
                      this,        SLOT  ( zoomInOut( int ) ) );
    QObject::connect( videoWidget, SIGNAL( currentPixelInfo( QPoint ) ),
                      this,        SLOT  ( currentPixelInfo( QPoint ) ) );
    QObject::connect( videoWidget, SIGNAL( pan( QPoint ) ),
                      this,        SLOT  ( pan( QPoint ) ) );
    QObject::connect( videoWidget, SIGNAL( redraw() ),
                      this,        SLOT  ( redraw() ) );




    // Create zoom sub menu
    zMenu = new zoomMenu();
    zMenu->enableAreaSelected( haveSelectedArea1 );
    QObject::connect( zMenu, SIGNAL( triggered ( QAction* ) ), this,  SLOT  ( zoomMenuTriggered( QAction* )) );

    // Create flip/rotate sub menu
    frMenu = new flipRotateMenu();
    frMenu->setChecked( rotation, flipHoz, flipVert );
    QObject::connect( frMenu, SIGNAL( triggered ( QAction* ) ), this,  SLOT  ( flipRotateMenuTriggered( QAction* )) );

    // Create and setup the select menu
    sMenu = new selectMenu();
    QObject::connect( sMenu, SIGNAL( triggered ( QAction* ) ), this,  SLOT  ( selectMenuTriggered( QAction* )) );

    // Add the video destination to the widget
    scrollArea = new QScrollArea;
    scrollArea->setFrameStyle( QFrame::NoFrame );
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget( videoWidget );

    // Local brightness and contrast controls
    localBC = new localBrightnessContrast;
    QObject::connect( localBC, SIGNAL( brightnessContrastChange() ),
                      this,    SLOT  ( brightnessContrastChanged()) );
    QObject::connect( localBC, SIGNAL( brightnessContrastAutoImage() ),
                      this,    SLOT  ( brightnessContrastAutoImageRequest() ) );
    QObject::connect(localBC, SIGNAL(destroyed(QObject*)), this, SLOT(localBCDestroyed(QObject*)));

    // Create vertical, horizontal, and general profile plots
    vSliceLabel = new QLabel( "Vertical Profile" );
    vSliceLabel->setVisible( false );
    vSliceDisplay = new profilePlot( profilePlot::PROFILEPLOT_BT );
    QObject::connect(vSliceDisplay, SIGNAL(destroyed(QObject*)), this, SLOT(vSliceDisplayDestroyed(QObject*)));
    vSliceDisplay->setVisible( false );

    hSliceLabel = new QLabel( "Horizontal Profile" );
    hSliceLabel->setVisible( false );
    hSliceDisplay = new profilePlot( profilePlot::PROFILEPLOT_LR );
    QObject::connect(hSliceDisplay, SIGNAL(destroyed(QObject*)), this, SLOT(hSliceDisplayDestroyed(QObject*)));
    hSliceDisplay->setVisible( false );

    profileLabel = new QLabel( "Arbitrary Line Profile" );
    profileLabel->setVisible( false );
    profileDisplay = new profilePlot( profilePlot::PROFILEPLOT_LR );
    QObject::connect(profileDisplay, SIGNAL(destroyed(QObject*)), this, SLOT(profileDisplayDestroyed(QObject*)));
    profileDisplay->setVisible( false );

    graphicsLayout = new QGridLayout();
    graphicsLayout->addWidget( scrollArea,      0, 0 );
    graphicsLayout->addLayout( getInfoWidget(), 1, 0 );
    graphicsLayout->addWidget( vSliceLabel,    1, 1 );
    graphicsLayout->addWidget( hSliceLabel,    2, 0 );
    graphicsLayout->addWidget( profileLabel,   4, 0 );

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
    graphicsLayout->addWidget( vSliceLabel,    1, 1 );
    graphicsLayout->addWidget( hSliceLabel,    2, 0 );
    graphicsLayout->addWidget( profileLabel,   4, 0 );

    mainLayout->addWidget( buttonGroup, 0, 0 );

    presentControls();

    graphicsLayout->setColumnStretch( 0, 1 );  // display image to take all spare room
    graphicsLayout->setRowStretch( 0, 1 );  // display image to take all spare room

    mainLayout->addLayout( graphicsLayout, 1, 0, 1, 0 );

    // Set graphics to take all spare room
    mainLayout->setColumnStretch( 1, 1 );
    mainLayout->setRowStretch( 1, 1 );

    setLayout( mainLayout );

    // Set up context sensitive menu (right click menu)
    setContextMenuPolicy( Qt::CustomContextMenu );
    connect( this, SIGNAL( customContextMenuRequested( const QPoint& )), this, SLOT( showImageContextMenu( const QPoint& )));
    fullContextMenu = true;

    // Create options dialog
    // This is done after all items manipulated by the options dialog have been built - such as the brightness/contrast controls
    // Also pareneted by this so will scaled automatically.
    optionsDialog = new QEImageOptionsDialog( this );
    QObject::connect( optionsDialog, SIGNAL( optionChange(  imageContextMenu::imageContextMenuOptions, bool )),
                      this,          SLOT  ( optionAction( imageContextMenu::imageContextMenuOptions, bool )) );
    optionsDialog->initialise();

    // Initially set the video widget to the size of the scroll bar
    // This will be resized when the image size is known
    videoWidget->resize( scrollArea->width(), scrollArea->height() );

    // Set image size to zero
    // Image will not be presented until size is available
    imageBuffWidth = 0;
    imageBuffHeight = 0;

    numDimensions = 0;
    imageDimension0 = 0;
    imageDimension1 = 0;
    imageDimension2 = 0;

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
    // Release components hosted by the application.
    // Note, the application may already have deleted them in which case we will
    // have recieved a destroyed signal and set the reference to the component to NULL.
    // An example of this scenario is if a QEGui main window is closed while a GUI is displayed.
    // Components not hosted by the application will be part of the widget hierarcy under this
    // widget and will not need explicit deletion.
    if( appHostsControls && hostingAppAvailable )
    {
        if( localBC )
        {
            QObject::disconnect( localBC, 0, this, 0);
            delete localBC;
        }

        if( vSliceDisplay )
        {
            QObject::disconnect( vSliceDisplay, 0, this, 0);
            delete vSliceDisplay;
        }

        if( hSliceDisplay )
        {
            QObject::disconnect( hSliceDisplay, 0, this, 0);
            delete hSliceDisplay;
        }

        if( profileDisplay )
        {
            QObject::disconnect( profileDisplay, 0, this, 0);
            delete profileDisplay;
        }
    }
    delete videoWidget;
}

// If an object handed over to the application (which we have a reference to) has been deleted, then clear the reference
void QEImage::localBCDestroyed( QObject* ){ localBC = NULL; }
void QEImage::vSliceDisplayDestroyed( QObject* ){ vSliceDisplay = NULL; }
void QEImage::hSliceDisplayDestroyed( QObject* ){ hSliceDisplay = NULL; }
void QEImage::profileDisplayDestroyed( QObject* ){ profileDisplay = NULL; }


// Put the controls where they should go.
// (within this widget, or hosted by the application containing this widget)
void QEImage::presentControls()
{
    // If components are being hosted by the application, hide any associated labels within the widget and
    // hand over the components to the application.
    // (Note, if components are not being hosted, they should always exist, but if something
    //  has gone wrong perhaps the appliction has deleted them, so don't assume they are present)
    if( appHostsControls && hostingAppAvailable )
    {
        QList<componentHostListItem> components;

        if( localBC )
        {
            mainLayout->removeWidget( localBC );
            components.append( componentHostListItem( localBC, QEActionRequests::OptionTopDockWindow, true, "Brightness / Contrast" ) );
        }

        vSliceLabel->setVisible( false );
        hSliceLabel->setVisible( false );
        profileLabel->setVisible( false );

        if( vSliceDisplay && enableVertSlicePresentation )
        {
            graphicsLayout->removeWidget( vSliceDisplay );
            components.append( componentHostListItem( vSliceDisplay,  QEActionRequests::OptionLeftDockWindow, true, "Vertical Slice Profile" ) );
        }

        if( hSliceDisplay && enableHozSlicePresentation )
        {
            graphicsLayout->removeWidget( hSliceDisplay );
            components.append( componentHostListItem( hSliceDisplay,  QEActionRequests::OptionTopDockWindow, true, "Horizontal Slice Profile" ) );
        }

        if( profileDisplay && enableProfilePresentation )
        {
            graphicsLayout->removeWidget( profileDisplay );
            components.append( componentHostListItem( profileDisplay, QEActionRequests::OptionTopDockWindow, true, "Arbitrary Profile" ) );
        }

        buttonGroup->hide();

        emitComponentHostRequest( QEActionRequests( components ) );

    }

    // If components are not being hosted by the application, present them within the widget.
    // (Note, if components are not being hosted, they should always exist, but if something
    //  has gone wrong perhaps the appliction has deleted them, so don't assume they are present)
    else
    {
        if( localBC )
        {
            mainLayout->addWidget( localBC, 0, 1 );
        }

        if( vSliceDisplay && enableVertSlicePresentation )
        {
            graphicsLayout->addWidget( vSliceDisplay,  0, 1 );
            vSliceLabel->setVisible( vSliceDisplay->isVisible() );
        }
        if( hSliceDisplay && enableHozSlicePresentation )
        {
            graphicsLayout->addWidget( hSliceDisplay,  3, 0 );
            hSliceLabel->setVisible( hSliceDisplay->isVisible() );
        }
        if( profileDisplay && enableProfilePresentation )
        {
            graphicsLayout->addWidget( profileDisplay, 5, 0 );
            profileLabel->setVisible( profileDisplay->isVisible() );
        }
    }
}

/*
    Implementation of QEWidget's virtual funtion to create the specific types of QCaObject required.
*/
qcaobject::QCaObject* QEImage::createQcaItem( unsigned int variableIndex ) {
    switch( variableIndex )
    {
        // Create the image item as a QEByteArray
        case IMAGE_VARIABLE:
            {
                // Create the image item
                QEByteArray* qca = new QEByteArray( getSubstitutedVariableName( variableIndex ), this, variableIndex );

                // If we already have the image dimensions (and the elements per pixel if required), update the image
                // size we need here before the subscription.
                // (we should have image dimensions as a connection is only established once these have been read)
                if( imageBuffWidth && imageBuffHeight && ( numDimensions !=3 || imageDimension0))
                {
                    // element count is at least width x height
                    unsigned int elementCount = imageBuffWidth * imageBuffHeight;

                    // Regardless of the souce of the width and height (either from width and height variables or from
                    // the appropriate area detector dimension variables), if the number of area detector dimensions
                    // is 3, then the first dimension is the number or elements per pixel so the element count needs to
                    // be multiplied by the first area detector dimension.

                    // It is possible for the image dimensions to change dynamically. For example to change from
                    // 3 dimensions to 2. In this example, the first dimension may change from being the data elements
                    // per pixel to being the image width before the 'number of dimensions' variable changes. This results
                    // in a window where the first dimension is assumed to be the data elements per pixel (num dimensions is 3)
                    // but it is actually the image width (much larger) this can result in crashes where a huge number of bytes
                    // per pixel is assumed and data arrays are overrun. If the dimensions appear odd, 32 was chosen as being large enough to cater for the
                    // largest number of elements per pixel. It is reasonable for image widths to be less than 32, so code must
                    // still handle invalid bytes per pixel calculations.
                    if( numDimensions == 3 && imageDimension0 && imageDimension0 <= 32 )
                    {
                        elementCount = elementCount * elementsPerPixel;
                    }

                    qca->setRequestedElementCount( elementCount );
                }
                return qca;
            }

        // Create the image format, image dimensions, target and beam, regions and profile and clipping items as a QEString
        case FORMAT_VARIABLE:
            return new QEString( getSubstitutedVariableName( variableIndex ), this, &stringFormatting, variableIndex );

        // Create the image dimensions, target and beam, regions and profile, clipping items and other variables as a QEInteger
        case BIT_DEPTH_VARIABLE:

        case NUM_DIMENSIONS_VARIABLE:
        case DIMENSION_0_VARIABLE:
        case DIMENSION_1_VARIABLE:
        case DIMENSION_2_VARIABLE:

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

        case PROFILE_H_VARIABLE:
        case PROFILE_H_THICKNESS_VARIABLE:
        case PROFILE_V_VARIABLE:
        case PROFILE_V_THICKNESS_VARIABLE:
        case LINE_PROFILE_X1_VARIABLE:
        case LINE_PROFILE_Y1_VARIABLE:
        case LINE_PROFILE_X2_VARIABLE:
        case LINE_PROFILE_Y2_VARIABLE:
        case LINE_PROFILE_THICKNESS_VARIABLE:

        case ELLIPSE_X1_VARIABLE:
        case ELLIPSE_Y1_VARIABLE:
        case ELLIPSE_X2_VARIABLE:
        case ELLIPSE_Y2_VARIABLE:

            return new QEInteger( getSubstitutedVariableName( variableIndex ), this, &integerFormatting, variableIndex );

        case PROFILE_H_ARRAY:
        case PROFILE_V_ARRAY:
        case PROFILE_LINE_ARRAY:

            return new QEFloating( getSubstitutedVariableName( variableIndex ), this, &floatingFormatting, variableIndex );

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

    // Do nothing regarding the image until the width and height are available
    if( variableIndex == IMAGE_VARIABLE && imageBuffWidth == 0 && imageBuffHeight == 0 )
       return;

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

        case FORMAT_VARIABLE:
            if(  qca )
            {
                QObject::connect( qca,  SIGNAL( stringChanged( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                                  this, SLOT( setFormat( const QString&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
                QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                                  this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
                QObject::connect( this, SIGNAL( requestResend() ),
                                  qca, SLOT( resendLastData() ) );
            }
            break;

        // Connect the bit depth variable
        case BIT_DEPTH_VARIABLE:
            if(  qca )
            {
                QObject::connect( qca,  SIGNAL( integerChanged( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                                  this, SLOT( setBitDepth( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
                QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                                  this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
                QObject::connect( this, SIGNAL( requestResend() ),
                                  qca, SLOT( resendLastData() ) );
            }
            break;

        // Connect the image dimension variables
        case WIDTH_VARIABLE:
        case HEIGHT_VARIABLE:
        case NUM_DIMENSIONS_VARIABLE:
        case DIMENSION_0_VARIABLE:
        case DIMENSION_1_VARIABLE:
        case DIMENSION_2_VARIABLE:
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

        // Connect to line profile variables
        case PROFILE_H_VARIABLE:
        case PROFILE_H_THICKNESS_VARIABLE:
        case PROFILE_V_VARIABLE:
        case PROFILE_V_THICKNESS_VARIABLE:
        case LINE_PROFILE_X1_VARIABLE:
        case LINE_PROFILE_Y1_VARIABLE:
        case LINE_PROFILE_X2_VARIABLE:
        case LINE_PROFILE_Y2_VARIABLE:
        case LINE_PROFILE_THICKNESS_VARIABLE:

            if(  qca )
            {
                QObject::connect( qca,  SIGNAL( integerChanged( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                                  this, SLOT( setProfile( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
                QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                                  this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
                QObject::connect( this, SIGNAL( requestResend() ),
                                  qca, SLOT( resendLastData() ) );
            }
            break;

        // Connect to targeting variables
        case TARGET_X_VARIABLE:
        case TARGET_Y_VARIABLE:

        case BEAM_X_VARIABLE:
        case BEAM_Y_VARIABLE:

            if(  qca )
            {
                QObject::connect( qca,  SIGNAL( integerChanged( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                                  this, SLOT( setTargeting( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
                QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                                  this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
                QObject::connect( this, SIGNAL( requestResend() ),
                                  qca, SLOT( resendLastData() ) );
            }
            break;
        // QCa creation occured, but no connection for display is required here.
        case PROFILE_H_ARRAY:
        case PROFILE_V_ARRAY:
        case PROFILE_LINE_ARRAY:

            break;

        // Connect to ellipse variables
        case ELLIPSE_X1_VARIABLE:
        case ELLIPSE_Y1_VARIABLE:
        case ELLIPSE_X2_VARIABLE:
        case ELLIPSE_Y2_VARIABLE:
            if(  qca )
            {
                QObject::connect( qca,  SIGNAL( integerChanged( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                                  this, SLOT( setEllipse( const long&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
                QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                                  this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
                QObject::connect( this, SIGNAL( requestResend() ),
                                  qca, SLOT( resendLastData() ) );
            }
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
    // Note the connected state
    isConnected = connectionInfo.isChannelConnected();

// Don't perform standard connection action (grey out widget and all its dialogs, and place disconnected in tooltip)
// If
//    // Display the connected state
//    updateToolTipConnection( isConnected );
//    updateConnectionStyle( isConnected );

// Instead just log the disconnected variables.
    if( !isConnected )
    {
        QString messageText;
        messageText.append( "Disconnected variable: " ).append( connectionInfo.variable() );
        sendMessage( messageText, "QEImage" );
    }

}

// Update the image dimensions (width and height) from the area detector dimension variables.
// If an area detector dimension is available, then set up the width and height from the
// appropriate area detector dimension variables if available.
// This function is called when any area detector dimension related variable changes
// Width and height will not be touched untill the number of dimensions is avaliable, and
// will only be altered if there is a valid dimension.
void QEImage::setWidthHeightFromDimensions()
{
    switch( numDimensions )
    {
        // 2 dimensions - one data element per pixel, dimensions are width x height
        case 2:
            if( imageDimension0 )
            {
                imageBuffWidth = imageDimension0;
            }
            if( imageDimension1 )
            {
                imageBuffHeight = imageDimension1;
            }
            break;

        // 3 dimensions - multiple data elements per pixel, dimensions are pixel x width x height
        case 3:
            if( imageDimension1 )
            {
                imageBuffWidth = imageDimension1;
            }
            if( imageDimension2 )
            {
                imageBuffHeight = imageDimension2;
            }
            break;
    }
}

/*
    Update the image format from a variable.
    This tends to take precedence over the format property simply as variable data arrives after all properties are set.
    If the 'format' property is set later, then it be used.

    This is the slot used to recieve data updates from a QCaObject based class.
 */
void QEImage::setFormat( const QString& text, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& )
{
    imageDataFormats::formatOptions previousFormatOption = mFormatOption;

    // Update image format
    // Area detector formats
    if     ( !text.compare( "Mono" ) )         mFormatOption = imageDataFormats::MONO;
    else if( !text.compare( "Bayer" ) )        mFormatOption = imageDataFormats::BAYER;
    else if( !text.compare( "RGB1" ) )         mFormatOption = imageDataFormats::RGB1;
    else if( !text.compare( "RGB2" ) )         mFormatOption = imageDataFormats::RGB2;
    else if( !text.compare( "RGB3" ) )         mFormatOption = imageDataFormats::RGB3;
    else if( !text.compare( "YUV444" ) )       mFormatOption = imageDataFormats::YUV444;
    else if( !text.compare( "YUV422" ) )       mFormatOption = imageDataFormats::YUV422;
    else if( !text.compare( "YUV421" ) )       mFormatOption = imageDataFormats::YUV421;
    else
    {
        // !!! warn unexpected format
    }

    // Do nothing if no format change
    if( previousFormatOption == mFormatOption)
    {
        return;
    }

    // Update the image.
    // This is required if image data arrived before the format.
    // The image data will be present, but will not have been used to update the image if the
    // width and height and format were not available at the time of the image update.
    displayImage();

    // Display invalid if invalid
    if( alarmInfo.isInvalid() )
    {
        //setImageInvalid()
        // !!! not done
    }
}

/*
    Update the image dimensions
    This is the slot used to recieve data updates from a QCaObject based class.
 */
void QEImage::setDimension( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex)
{
    bool dimensionChange = false;

    // C/C++ not so smart assiging signed long value to an
    // unsigned long value - ensure sensible.
    unsigned long uValue = MAX( 0, value );

    // Update image size variable
    switch( variableIndex )
    {
        case WIDTH_VARIABLE:
            if( imageBuffWidth != uValue )
            {
                dimensionChange = true;
                imageBuffWidth = uValue;
            }
            break;

        case HEIGHT_VARIABLE:
            if( imageBuffHeight != uValue )
            {
                dimensionChange = true;
                imageBuffHeight = uValue;
            }
            break;

        case NUM_DIMENSIONS_VARIABLE:
            if( numDimensions != uValue )
            {
                dimensionChange = true;
                switch( uValue )
                {
                    case 0:
                        numDimensions = uValue;
                        break;

                    case 2:
                    case 3:
                        numDimensions = uValue;
                        setWidthHeightFromDimensions();
                        break;
                }
            }
            break;

        case DIMENSION_0_VARIABLE:
            if( imageDimension0 != uValue )
            {
                dimensionChange = true;
                imageDimension0 = uValue;
                setWidthHeightFromDimensions();
            }
            break;

        case DIMENSION_1_VARIABLE:
            if( imageDimension1 != uValue )
            {
                dimensionChange = true;
                imageDimension1 = uValue;
                setWidthHeightFromDimensions();
            }
            break;

        case DIMENSION_2_VARIABLE:
            if( imageDimension2 != uValue )
            {
                dimensionChange = true;
                imageDimension2 = uValue;
                setWidthHeightFromDimensions();
            }
            break;
    }

    // Update the image buffer according to the new size
    // This will do nothing unless both width and height are available
    // Note, this is also called in displayImage() next, but only if the image buffer empty.
    setImageBuff();

    // Update the image.
    // This is required if image data for an enlarged image arrived before the width and height.
    // The image data will be present, but will not have been used to update the image if the
    // width and height were not suitable at the time of the image update
    displayImage();

    // If the image size or data array dimensions has changed and we have good dimensions, update the image
    // variable connection to reflect the elements we now need.
    // If an image dimensions change dynamically we may pass through a period where a set of dimensions that are nonsense. For example,
    // if the number of dimensions is changing from 3 to 2, this means the first dimension will change from being the data elements
    // per pixel to the image width. If the update for the first dimension arrives first, the number of dimensions will still be 3 (implying the
    // first dimension is the number of data elements per pixel, but the the first dimension will be the image width.
    // If the dimensions appear nonsense, then don't force an image update. Note, this won't stop an image update from occuring, so
    // the image update must cope with odd dimensions, but just no point forcing it here.
    // The test for good dimensions is to check if a width and height is present, and (if the first dimension is expected to be the number
    // of data elements per pixel, then is is less than 32. 32 was chosen as being large enough for any pixel format (for example 32 bits
    // per color for 4 Bayer RGBG colours) but less than most image widths. This test doesn't have to be perfect since the image update must
    // be able to cope with an invalid set of dimensions as mentioned above.
    unsigned long pixelCount = imageBuffWidth * imageBuffHeight;
    if( pixelCount && dimensionChange && (( numDimensions != 3 ) || ( imageDimension0 < 32 ) ) )
    {
        if( numDimensions == 3 )
        {
            elementsPerPixel = imageDimension0;
        }
        else
        {
            elementsPerPixel = 1;
        }

        // Clear any current image as the data in it is not our own and will be lost when the connection is re-established
        image.clear();

        // Re-establish the image connection. This will set request the appropriate array size.
        establishConnection( IMAGE_VARIABLE );
    }

    // Display invalid if invalid
    if( alarmInfo.isInvalid() )
    {
        //setImageInvalid()
        // !!! not done
    }
}

/*
    Update the image dimensions
    This is the slot used to recieve data updates from a QCaObject based class.
 */
void QEImage::setBitDepth( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex)
{
    // Sanity check - Only deal with bit depth
    if( variableIndex != BIT_DEPTH_VARIABLE)
    {
        return;
    }

    // Update the depth
    setBitDepth( value );

    // Update the image.
    // This is required if image data for an enlarged image arrived before the width and height.
    // The image data will be present, but will not have been used to update the image if the
    // width and height were not suitable at the time of the image update
    displayImage();

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
            {
                bool newClippingOn = (value>0)?true:false;
                if( clippingOn != newClippingOn )
                {
                    pixelLookupValid = false;
                }
                clippingOn = newClippingOn;
                break;
            }
        case CLIPPING_LOW_VARIABLE:
            if( clippingLow != (unsigned int)value )
            {
                pixelLookupValid = false;
            }
            clippingLow = value;
            break;

        case CLIPPING_HIGH_VARIABLE:
            if( clippingHigh != (unsigned int)value )
            {
                pixelLookupValid = false;
            }
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
        // Save the tageting data
        switch( variableIndex )
        {
            case ROI1_X_VARIABLE:  roiInfo[0].setX( value ); break;
            case ROI1_Y_VARIABLE:  roiInfo[0].setY( value ); break;
            case ROI1_W_VARIABLE:  roiInfo[0].setW( value ); break;
            case ROI1_H_VARIABLE:  roiInfo[0].setH( value ); break;
            case ROI2_X_VARIABLE:  roiInfo[1].setX( value ); break;
            case ROI2_Y_VARIABLE:  roiInfo[1].setY( value ); break;
            case ROI2_W_VARIABLE:  roiInfo[1].setW( value ); break;
            case ROI2_H_VARIABLE:  roiInfo[1].setH( value ); break;
            case ROI3_X_VARIABLE:  roiInfo[2].setX( value ); break;
            case ROI3_Y_VARIABLE:  roiInfo[2].setY( value ); break;
            case ROI3_W_VARIABLE:  roiInfo[2].setW( value ); break;
            case ROI3_H_VARIABLE:  roiInfo[2].setH( value ); break;
            case ROI4_X_VARIABLE:  roiInfo[3].setX( value ); break;
            case ROI4_Y_VARIABLE:  roiInfo[3].setY( value ); break;
            case ROI4_W_VARIABLE:  roiInfo[3].setW( value ); break;
            case ROI4_H_VARIABLE:  roiInfo[3].setH( value ); break;
        }

        // If there is an image, present the ROI data
        // (if there is no image, the ROI data will be used when one arrives)
        if( videoWidget->hasCurrentImage() )
        {
            useROIData( variableIndex );
        }
    }
}

// Apply the ROI data.
// This can be done once all ROI data is available and an image is available
// (the image is needed to determine scaling)
void QEImage::useROIData( const unsigned int& variableIndex )
{
#define USE_ROI_DATA( N )                                                                         \
    if( sMenu->getAreaEnabled() && roiInfo[N].getStatus() )                                       \
    {                                                                                             \
        QRect scaledArea = roiInfo[N].getArea();                                                  \
        scaledArea.setTopLeft( videoWidget->scaleImagePoint( scaledArea.topLeft() ) );            \
        scaledArea.setBottomRight( videoWidget->scaleImagePoint( scaledArea.bottomRight() ) );    \
        videoWidget->markupRegionValueChange( N, scaledArea, displayMarkups );                    \
    }                                                                                             \
    break;

    switch( variableIndex )
    {
        case ROI1_X_VARIABLE:
        case ROI1_Y_VARIABLE:
        case ROI1_W_VARIABLE:
        case ROI1_H_VARIABLE:
            USE_ROI_DATA( 0 )

        case ROI2_X_VARIABLE:
        case ROI2_Y_VARIABLE:
        case ROI2_W_VARIABLE:
        case ROI2_H_VARIABLE:
            USE_ROI_DATA( 1 )

        case ROI3_X_VARIABLE:
        case ROI3_Y_VARIABLE:
        case ROI3_W_VARIABLE:
        case ROI3_H_VARIABLE:
            USE_ROI_DATA( 2 )

        case ROI4_X_VARIABLE:
        case ROI4_Y_VARIABLE:
        case ROI4_W_VARIABLE:
        case ROI4_H_VARIABLE:
            USE_ROI_DATA( 3 )
    }
}

/*
    Update the Profile displays if any
    This is the slot used to recieve data updates from a QCaObject based class.
 */
void QEImage::setProfile( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex)
{
    // If invalid, mark the appropriate profile info as not present
    if( alarmInfo.isInvalid() )
    {
        switch( variableIndex )
        {
            case PROFILE_H_VARIABLE:              hSliceY = 0;               break;
            case PROFILE_V_VARIABLE:              vSliceX = 0;               break;
            case PROFILE_H_THICKNESS_VARIABLE:    hSliceThickness = 1;       break;
            case PROFILE_V_THICKNESS_VARIABLE:    vSliceThickness = 1;       break;
            case LINE_PROFILE_X1_VARIABLE:        lineProfileInfo.clearX1(); break;
            case LINE_PROFILE_Y1_VARIABLE:        lineProfileInfo.clearY1(); break;
            case LINE_PROFILE_X2_VARIABLE:        lineProfileInfo.clearX2(); break;
            case LINE_PROFILE_Y2_VARIABLE:        lineProfileInfo.clearY2(); break;
            case LINE_PROFILE_THICKNESS_VARIABLE: profileThickness = 1;      break;
        }
    }

    // Good data. Save the profile data (and note it is present) then if the
    // markup is visible, update it
    else
    {
        // Save the tageting data
        switch( variableIndex )
        {
            // !!! these next two lines are broken - when called on the initial updates of the scalar values the scaling is not available as there is no image.
            //     change hSliceY and vSliceX (and infact all other similar variables for regions, targeting, etc) to be ordinates in raw image
            case PROFILE_H_VARIABLE:              hSliceY = videoWidget->scaleImageOrdinate( value ); break;
            case PROFILE_V_VARIABLE:              vSliceX = videoWidget->scaleImageOrdinate( value ); break;
            case PROFILE_H_THICKNESS_VARIABLE:    hSliceThickness = value;                            break;
            case PROFILE_V_THICKNESS_VARIABLE:    vSliceThickness = value;                            break;
            case LINE_PROFILE_X1_VARIABLE:        lineProfileInfo.setX1( value );                     break;
            case LINE_PROFILE_Y1_VARIABLE:        lineProfileInfo.setY1( value );                     break;
            case LINE_PROFILE_X2_VARIABLE:        lineProfileInfo.setX2( value );                     break;
            case LINE_PROFILE_Y2_VARIABLE:        lineProfileInfo.setY2( value );                     break;
            case LINE_PROFILE_THICKNESS_VARIABLE: profileThickness = 1;                               break;
        }

        // If there is an image, present the profile data
        // (if there is no image, the profile data will be used when one arrives)
        if( videoWidget->hasCurrentImage() )
        {
            useProfileData( variableIndex );
        }
    }
}

// Apply the profile data.
// This can be done once all profile data is available and an image is available
// (the image is needed to determine scaling)
void QEImage::useProfileData( const unsigned int& variableIndex )
{
    switch( variableIndex )
    {
        case PROFILE_H_VARIABLE:
            if( sMenu->getHSliceEnabled() )
            {
                videoWidget->markupHProfileChange(  videoWidget->scaleOrdinate( hSliceY ), displayMarkups );
            }
            break;

        case PROFILE_V_VARIABLE:
            if( sMenu->getVSliceEnabled() )
            {
                videoWidget->markupVProfileChange(  videoWidget->scaleOrdinate( vSliceX ), displayMarkups );
            }
            break;

        case LINE_PROFILE_X1_VARIABLE:
        case LINE_PROFILE_Y1_VARIABLE:
        case LINE_PROFILE_X2_VARIABLE:
        case LINE_PROFILE_Y2_VARIABLE:
            if( sMenu->getProfileEnabled() && lineProfileInfo.getStatus() )
            {
                QRect scaledArea = lineProfileInfo.getArea();
                scaledArea.setTopLeft( videoWidget->scaleImagePoint( scaledArea.topLeft() ) );
                scaledArea.setBottomRight( videoWidget->scaleImagePoint( scaledArea.bottomRight() ) );
                videoWidget->markupLineProfileChange( scaledArea.topLeft(), scaledArea.bottomRight(), displayMarkups );
            }
            break;
    }
}

/*
    Update the Ellipse displays if any
    This is the slot used to recieve data updates from a QCaObject based class.
 */
void QEImage::setEllipse( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex)
{
    // If invalid, mark the appropriate profile info as not present
    if( alarmInfo.isInvalid() )
    {
        switch( variableIndex )
        {
            case ELLIPSE_X1_VARIABLE: ellipseInfo.clearX1(); break;
            case ELLIPSE_Y1_VARIABLE: ellipseInfo.clearY1(); break;
            case ELLIPSE_X2_VARIABLE: ellipseInfo.clearX2(); break;
            case ELLIPSE_Y2_VARIABLE: ellipseInfo.clearY2(); break;
        }
    }

    // Good data. Save the ellipse data (and note it is present) then if the
    // markup is visible, update it
    else
    {
        // Save the ellipse data
        switch( variableIndex )
        {
            case ELLIPSE_X1_VARIABLE: ellipseInfo.setX1( value ); break;
            case ELLIPSE_Y1_VARIABLE: ellipseInfo.setY1( value ); break;
            case ELLIPSE_X2_VARIABLE: ellipseInfo.setX2( value ); break;
            case ELLIPSE_Y2_VARIABLE: ellipseInfo.setY2( value ); break;
        }

        // If there is an image, present the ellipse data
        // (if there is no image, the profile data will be used when one arrives)
        if( videoWidget->hasCurrentImage() )
        {
            useEllipseData();
        }
    }
}

// Apply the ellipse data.
// This can be done once all ellipse data is available and an image is available
// (the image is needed to determine scaling)
void QEImage::useEllipseData()
{
    if( ellipseInfo.getStatus() )
    {
        QRect scaledArea = ellipseInfo.getArea();
        scaledArea.setTopLeft( videoWidget->scaleImagePoint( scaledArea.topLeft() ) );
        scaledArea.setBottomRight( videoWidget->scaleImagePoint( scaledArea.bottomRight() ) );
        videoWidget->markupEllipseValueChange( scaledArea.topLeft(), scaledArea.bottomRight(), displayMarkups );
    }
}

/*
    Update the target and beam position markers if any.
    This is the slot used to recieve data updates from a QCaObject based class.
 */
void QEImage::setTargeting( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex)
{
    // If invalid, mark the appropriate profile info as not present
    if( alarmInfo.isInvalid() )
    {
        switch( variableIndex )
        {
            case TARGET_X_VARIABLE: targetInfo.clearX();  break;
            case TARGET_Y_VARIABLE: targetInfo.clearY();  break;
            case BEAM_X_VARIABLE:   beamInfo.clearX();    break;
            case BEAM_Y_VARIABLE:   beamInfo.clearX();    break;
        }
    }

    // Good data. Save the target and beam data (and note it is present) then if the
    // markup is visible, update it
    else
    {
        // Save the tageting data
        switch( variableIndex )
        {
            case TARGET_X_VARIABLE:  targetInfo.setX( value ); break;
            case TARGET_Y_VARIABLE:  targetInfo.setY( value ); break;
            case BEAM_X_VARIABLE:    beamInfo.setX( value );   break;
            case BEAM_Y_VARIABLE:    beamInfo.setY( value );   break;
        }

        // If there is an image, present the targeting data
        // (if there is no image, the targeting data will be used when one arrives)
        if( videoWidget->hasCurrentImage() )
        {
            useTargetingData();
        }
    }
}

// Apply the targeting data.
// This can be done once all targeting data is available and an image is available
// (the image is needed to determine scaling)
void QEImage::useTargetingData()
{
    if( sMenu->getTargetEnabled() && targetInfo.getStatus() && beamInfo.getStatus() )
    {
        QPoint scaledPoint;

        scaledPoint = videoWidget->scaleImagePoint( targetInfo.getPoint() );
        videoWidget->markupTargetValueChange( scaledPoint, displayMarkups );

        scaledPoint = videoWidget->scaleImagePoint( beamInfo.getPoint() );
        videoWidget->markupBeamValueChange( scaledPoint, displayMarkups );
    }
}

// Display all markup data
// Used When the first image update occurs to display any
// markups for which data has arrived, but could not be presented
// until an image was available to determine scaling
void QEImage::useAllMarkupData()
{
    useROIData( ROI1_X_VARIABLE );
    useROIData( ROI1_Y_VARIABLE );
    useROIData( ROI1_W_VARIABLE );
    useROIData( ROI1_H_VARIABLE );
    useROIData( ROI2_X_VARIABLE );
    useROIData( ROI2_Y_VARIABLE );
    useROIData( ROI2_W_VARIABLE );
    useROIData( ROI2_H_VARIABLE );
    useROIData( ROI3_X_VARIABLE );
    useROIData( ROI3_Y_VARIABLE );
    useROIData( ROI3_W_VARIABLE );
    useROIData( ROI3_H_VARIABLE );
    useROIData( ROI4_X_VARIABLE );
    useROIData( ROI4_Y_VARIABLE );
    useROIData( ROI4_W_VARIABLE );
    useROIData( ROI4_H_VARIABLE );

    useProfileData( PROFILE_H_VARIABLE);
    useProfileData( PROFILE_V_VARIABLE );
    useProfileData( LINE_PROFILE_X1_VARIABLE ); //!!! all 4 of these requried???
    useProfileData( LINE_PROFILE_Y1_VARIABLE );
    useProfileData( LINE_PROFILE_X2_VARIABLE );
    useProfileData( LINE_PROFILE_Y2_VARIABLE );

    useTargetingData(); //!!! change this to use each of the targeting

    useEllipseData();
}

// Update image from non CA souce (no associated CA timestamp or alarm info available)
void QEImage::setImage( const QByteArray& imageIn, unsigned long dataSize, unsigned long elements, unsigned long width, unsigned long height, imageDataFormats::formatOptions format, unsigned int depth )
{
    //!!! Should the format, bit depth, width and height be clobered like this? (especially where we are altering properties, like bitDepth)
    //!!! Perhaps CA delivered and MPEG delivered images should maintain their own attributes?

    // set the format
    setFormatOption( format );

    //!!! should also set format as delivered with image from mpeg source???

    // Set the image bit depth
    bitDepth = depth;

    elementsPerPixel = elements;

    // Set the image dimensions to match the image size
    imageBuffWidth = width;
    imageBuffHeight = height;

    // Update the image buffer according to the new size.
    setImageBuff();

    QCaAlarmInfo alarmInfo;
    QCaDateTime dateTime = QCaDateTime( QDateTime::currentDateTime() );
    setImage( imageIn, dataSize, alarmInfo, dateTime, 0 );
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
    receivedImageSize = (unsigned long) image.size ();
    imageDataSize = dataSize;

    // Calculate the number of bytes per pixel.
    // If the number of elements per pixel is known (derived from the image dimension zero if there are three dimensions)
    // then it is the image data element size * the number of elements per pixel
    // If the number of elements per pixel is not known (number of dimensions is not know or not three or dimension zero is not present)
    // then the elements per pixel will default to 1.
    bytesPerPixel = imageDataSize * elementsPerPixel;

    // Note the time of this image
    imageTime = time;

    // Note if the widget already had an image
    // (Used below to determine if markups data should now be applied)
    bool hasImage = videoWidget->hasCurrentImage();

    // Present the new image
    displayImage();

    // If this is the first image update, use any markup data that may have already arrived
    // (markup data can't be used until there is an image to determine the current scaling from)
    // Set of as a timer only to ensure it occurs after the initial paint already queued by displayImage() above.
    if( !hasImage )
    {
        QTimer::singleShot( 0, this, SLOT(useAllMarkupData() ) );
    }

    // Indicate another image has arrived
    freshImage();

    // Display invalid if invalid
    if( alarmInfo.isInvalid() )
    {
        //setImageInvalid()
        // !!! not done
    }
}

// Generate a lookup table to convert the top 8 bits of raw pixel values to display pixel values taking into
// account, clipping, contrast reversal, and local brightness and contrast.
// Only the top 8 bits are used as all presentation is performed using 8 bit.
// Note, the table will be used to translate each colour in an RGB format.
//
// The following example assumes:
//  - a user set contrast of 150%
//  - a user set brightness of -15%
//
//       (A)    (B)    (C)    (D)    (E)
//
//   382         *
//               *
//               *
//               *      *
//               *      *
//               *      *      *
//   255  *      *      *      *-
//        *      *      *      * -
//        *      *      *      *  -
//        *      *      *      *   -
//        *    --*--    *      *    --*-----
//        *  --  *  --  *      *      *
//      --*--    *    --*--    *      *
//        *      *      *  --  *      *
//        *      *      *    --*      *
//        *      *      *      *      *
//        *      *      *      *      *
//     0  *------*------*------*------*------
//                      *      *
//                      *      *
//                      *      *
//                             *
//                             *
//
// (A) Top 8 bits in each pixel in the image has a range of values from 0 to 255.
//     Note central mid grey is marked.
//
// (B) Range of values is extended by contrast.
//     In this example, 150% contrast results in 4096 values ranging from 0 to 382.
//     If left like this the top 127 values would be lost in the white.
//
// (C) Range of values is offset so half of the extended range is lost in the white and half in the black.
//     The values now range from -64 to 318.
//
// (D) Range of values is offset by a user set brightness.
//     The brightness range of -100% to +100% is meant to bring the highest value down to 0 (black) or the lowest value up to white.
//     The offset applied by the user brightness value must take into account the varying range of values caused by contrast changes.
//     In the example, the brightness is lowered by 15%. -100% would bring 318 down to 0. +100% would take -64 up to 255.
//
// (E) Values matching the original range of values are selected from the translated table.
//
const QEImage::rgbPixel* QEImage::getPixelTranslation()
{
    // If the table is already set up, return it.
    if( pixelLookupValid )
    {
        return (rgbPixel*)(pixelLookup.constData());
    }

// Maximum pixel value for 8 bit
#define MAX_VALUE 255

    // View the table as an array or rgb pixels
    rgbPixel* lookupTable = (rgbPixel*)(pixelLookup.constData());

    // Determine if local contrast and brightness applies
    // Even if local brightness and contrast is enabled, no need to go through the pain if they are both neutral
    bool doLCB = false;
    int localContrast = 100;
    int localBrightness = 0;
    if( localBC )
    {
        localContrast = localBC->getContrast();
        localBrightness = localBC->getBrightness();
        doLCB = localContrast!=100 || localBrightness != 0;
    }

    // Range of values with contrast applied
    int range = MAX_VALUE*localContrast/100;

    // Offset to set black level with new range of values
    int offset = (range-MAX_VALUE)/2;

    // Loop populating table with pixel translations for every pixel value
    unsigned int value = 0;
    for( value = 0; value <= MAX_VALUE; value++ )
    {
        // Alpha always 100%
        lookupTable[value].p[3] = 0xff;

        // Assume no clipping
        bool clipped = false;
        if( clippingOn && (clippingHigh > 0 || clippingLow > 0 ))
        {
            // If clipping high, set pixel to solid 'clip high' color
            if( clippingHigh > 0 && value >= clippingHigh )
            {
                lookupTable[value].p[0] = 0x80;
                lookupTable[value].p[1] = 0x80;
                lookupTable[value].p[2] = 0xff;
                clipped = true;
            }
            // If clipping low, set pixel to solid 'clip low' color
            else if( clippingLow > 0 && value <= clippingLow )
            {
                lookupTable[value].p[0] = 0xff;
                lookupTable[value].p[1] = 0x80;
                lookupTable[value].p[2] = 0x80;
                clipped = true;
            }
        }

        // Translate pixel value if not clipped
        if( !clipped )
        {
            // Start with initial pixel value
            int translatedValue = value;

            // Reverse contrast if required
            if( localBC->getContrastReversal() )
            {
                translatedValue = MAX_VALUE - translatedValue;
            }

            // Apply local brightness and contrast if required
            if( doLCB )
            {
                translatedValue = (translatedValue*localContrast/100)-offset+((range-offset)*localBrightness/100);
                if( translatedValue < 0 )
                {
                    translatedValue = 0;
                }
                else if ( translatedValue > MAX_VALUE )
                {
                    translatedValue = MAX_VALUE;
                }
            }

            // Save translated pixel
            lookupTable[value].p[0] = (unsigned char)translatedValue;
            lookupTable[value].p[1] = (unsigned char)translatedValue;
            lookupTable[value].p[2] = (unsigned char)translatedValue;
        }
    }

    pixelLookupValid = true;
    return lookupTable;
}

// Get a false color representation for an entry from the color lookup table
//???!!! not used yet
QEImage::rgbPixel QEImage::getFalseColor (const unsigned char value) {
    rgbPixel result;
    int h, l;
    QColor c;

    // Hue goes 320 (purple) down to 0 (red) as monochrome value goes 0 to 255.
    //
    h = (320 * (0xFF - value)) / 0xFF;

    // Intesity ramps up to a max of 128
    //
    l = MIN (4*value, 128);

    c.setHsl (h, 0xFF, l);

    result.p[0] = (unsigned char) c.blue();
    result.p[1] = (unsigned char) c.green();
    result.p[2] = (unsigned char) c.red();
    result.p[3] = (unsigned char) 0xFF; // Alpha always 100%

    return result;
}


// Display a new image.
void QEImage::displayImage()
{

    // Do nothing if there is no image, or are no image dimensions yet
    if( image.isEmpty() || !imageBuffWidth || !imageBuffHeight )
        return;

    // Do we have enough (or any) data
    //
    const unsigned long required_size = imageBuffWidth * imageBuffHeight * bytesPerPixel;
    if( required_size > (unsigned int)(image.size()) )
    {
        // Do nothing if no image data.
        //
        if( receivedImageSize == 0 ) {
            return;
        }

        QString messageText;

        messageText = QString( "Image too small (")
                .append( QString( "available image size: %1, " )    .arg( receivedImageSize ))
                .append( QString( "required size: %1, " )           .arg( required_size ))
                .append( QString( "width: %1, " )                   .arg( imageBuffWidth ))
                .append( QString( "height: %1, " )                  .arg( imageBuffHeight ))
                .append( QString( "data element size: %1, " )       .arg( imageDataSize ))
                .append( QString( "data elements per pixel: %1, " ) .arg( elementsPerPixel ))
                .append( QString( "bytes per pixel: %1)" )          .arg( bytesPerPixel ));

        // Skip if messageText same as last message.
        if (messageText != previousMessageText) {
            sendMessage( messageText, "QEImage" );
            previousMessageText = messageText;
        }

        // If not enough image data for the expected size then zero extend.
        // Part image better than no image at all.
        int extra = (int)required_size - image.size();
        QByteArray zero_extend ( extra, '\0' );
        image.append( zero_extend );
    }

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
    rgbPixel* dataOut = (rgbPixel*)(imageBuff.data());
    unsigned long buffIndex = 0;
    unsigned long dataIndex = 0;

    // Determine the number of pixels to process
    // If something is wrong, do nothing
    unsigned long pixelCount = imageBuffWidth*imageBuffHeight;
    if(( pixelCount * bytesPerPixel > (unsigned long)image.size() ) ||
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
    // Depending on the scan option, however, the outer may be height or width.
    // The input buffer is read consecutively from first pixel to last and written to the
    // output buffer, which is moved to the next pixel by both the inner and outer
    // loops to where ever that next pixel is according to the rotation and flipping.
    dataIndex = start;

    // Get the pixel lookup table to convert raw pixel values to display pixel values taking into
    // account input pixel size, clipping, contrast reversal, and local brightness and contrast.
    const rgbPixel* pixelLookup = getPixelTranslation();


// For speed, the format switch statement is outside the pixel loop.
// An identical(ish) loop is used for each format
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

    // Format each pixel ready for use in an RGB32 QImage.
    // Note, for speed, the switch on format is outside the loop. The loop is duplicated in each case using macros which.
    switch( mFormatOption )
    {
        case imageDataFormats::MONO:
        {
            switch( bitDepth )
            {
                default:
                // Pixel data is 1 to 8 bits wide. Extract the fist byte. For less than 8 bits assume rest of byte is zero.
                // (Assumtion is safe as even if incorrect range will still be 0-255 which is OK as an index into the pixelLookup table)
                case 1:
                case 2:
                case 4:
                case 6:
                case 8:
                {
                    LOOP_START
                        unsigned char inPixel = *(unsigned char*)(&dataIn[dataIndex*bytesPerPixel]);
                        dataOut[buffIndex] = pixelLookup[inPixel];
                    LOOP_END
                    break;
                }

                // Pixel data is 10 bits wide - extract as 16 bit and use the top 8 bits of the first 10 bits
                // (Zero top bits to ensure range is safe as an index into the pixelLookup table)
                case 10:
                {
                    LOOP_START
                        unsigned short inPixel = *(unsigned short*)(&dataIn[dataIndex*bytesPerPixel]);
                        dataOut[buffIndex] = pixelLookup[(inPixel&0x03ff)>>2];
                    LOOP_END
                    break;
                }

                // Pixel data is 12 bits wide - extract as 16 bit and use the top 8 bits of the first 12 bits
                // (Zero top bits to ensure range is safe as an index into the pixelLookup table)
                case 12:
                {
                    LOOP_START
                        unsigned short inPixel = *(unsigned short*)(&dataIn[dataIndex*bytesPerPixel]);
                        dataOut[buffIndex] = pixelLookup[(inPixel&0x0fff)>>4];
                    LOOP_END
                    break;
                }

                // Pixel data is 14 bits wide - extract as 16 bit and use the top 8 bits of the first 14 bits
                // (Zero top bits to ensure range is safe as an index into the pixelLookup table)
                case 14:
                {
                    LOOP_START
                        unsigned short inPixel = *(unsigned short*)(&dataIn[dataIndex*bytesPerPixel]);
                        dataOut[buffIndex] = pixelLookup[(inPixel&0x03fff)>>6];
                    LOOP_END
                    break;
                }

                // Pixel data is 16 bits wide - use the top byte
                case 16:
                {
                    LOOP_START
                        unsigned char inPixel = *(unsigned char*)(&dataIn[dataIndex*bytesPerPixel+1]);
                        dataOut[buffIndex] = pixelLookup[inPixel];
                    LOOP_END
                    break;
                }

                // Pixel data is 18 bits wide - extract as 32 bit and use the top 8 bits of the first 18 bits
                // (Zero top bits to ensure range is safe as an index into the pixelLookup table)
                case 18:
                    {
                        LOOP_START
                            // Pixel data is 18 bits wide - use the top 8 bits
                            quint32 inPixel = *(quint32*)(&dataIn[dataIndex*bytesPerPixel]);
                            dataOut[buffIndex] = pixelLookup[(inPixel&0x03ffff)>>10];
                        LOOP_END
                        break;
                    }

                // Pixel data is 20 bits wide - extract as 32 bit and use the top 8 bits of the first 20 bits
                // (Zero top bits to ensure range is safe as an index into the pixelLookup table)
                case 20:
                    {
                        LOOP_START
                            quint32 inPixel = *(quint32*)(&dataIn[dataIndex*bytesPerPixel]);
                            dataOut[buffIndex] = pixelLookup[(inPixel&0x0fffff)>>12];
                        LOOP_END
                        break;
                    }

                // Pixel data is 22 bits wide - extract as 32 bit and use the top 8 bits of the first 22 bits
                // (Zero top bits to ensure range is safe as an index into the pixelLookup table)
                case 22:
                    {
                        LOOP_START
                            // Pixel data is 22 bits wide - use the top 8 bits
                            quint32 inPixel = *(quint32*)(&dataIn[dataIndex*bytesPerPixel]);
                            dataOut[buffIndex] = pixelLookup[(inPixel&0x03fffff)>>14];
                        LOOP_END
                        break;
                    }

                // Pixel data is 24 bits wide - use the top byte
                case 24:
                    {
                        LOOP_START
                            unsigned char inPixel = *(unsigned char*)(&dataIn[dataIndex*bytesPerPixel+2]);
                            dataOut[buffIndex] = pixelLookup[inPixel];
                        LOOP_END
                        break;
                    }

                // Pixel data is 26 bits wide - extract as 32 bit and use the top 8 bits of the first 26 bits
                // (Zero top bits to ensure range is safe as an index into the pixelLookup table)
                case 26:
                    {
                        LOOP_START
                            unsigned long inPixel = *(unsigned long*)(&dataIn[dataIndex*bytesPerPixel]);
                            dataOut[buffIndex] = pixelLookup[(inPixel&0x03ffffff)>>18];
                        LOOP_END
                        break;
                    }

                // Pixel data is 28 bits wide - extract as 32 bit and use the top 8 bits of the first 28 bits
                // (Zero top bits to ensure range is safe as an index into the pixelLookup table)
                case 28:
                    {
                        LOOP_START
                            unsigned long inPixel = *(unsigned long*)(&dataIn[dataIndex*bytesPerPixel]);
                            dataOut[buffIndex] = pixelLookup[(inPixel&0x0fffffff)>>20];
                        LOOP_END
                        break;
                    }

                // Pixel data is 30 bits wide - extract as 32 bit and use the top 8 bits of the first 30 bits
                // (Zero top bits to ensure range is safe as an index into the pixelLookup table)
                case 30:
                    {
                        LOOP_START
                            unsigned long inPixel = *(unsigned long*)(&dataIn[dataIndex*bytesPerPixel]);
                            dataOut[buffIndex] = pixelLookup[(inPixel&0x03fffffff)>>22];
                        LOOP_END
                        break;
                    }

                // Pixel data is 32 bits wide - use the top byte
                case 32:
                    {
                        LOOP_START
                            // Pixel data is 32 bits wide - use the top 8 bits
                            unsigned char inPixel = *(unsigned char*)(&dataIn[dataIndex*bytesPerPixel+3]);
                            dataOut[buffIndex] = pixelLookup[inPixel];
                        LOOP_END
                        break;
                    }
            }
            break;
        }

        case imageDataFormats::BAYER:
        {
            int TLOffset = (-(int)(imageBuffWidth)-1)*(int)(bytesPerPixel);
            int  TOffset = -(int)(imageBuffWidth)*(int)(bytesPerPixel);
            int TROffset = (-(int)(imageBuffWidth)+1)*(int)(bytesPerPixel);
            int  LOffset = -(int)(bytesPerPixel);
            int  ROffset = (int)(bytesPerPixel);
            int BLOffset = ((int)(imageBuffWidth)-1)*(int)(bytesPerPixel);
            int  BOffset = imageBuffWidth*(int)(bytesPerPixel);
            int BROffset = ((int)(imageBuffWidth)+1)*(int)(bytesPerPixel);

            enum regions {REG_TL, REG_T, REG_TR, REG_L, REG_C, REG_R, REG_BL, REG_B, REG_BR};

            quint32 r1;
            quint32 r2;
            quint32 r3;
            quint32 r4;
            quint32 g1;
            quint32 g2;
            quint32 g3;
            quint32 g4;
            quint32 b1;
            quint32 b2;
            quint32 b3;
            quint32 b4;



            int outLast = outCount-1;
            int inLast = inCount-1;


            unsigned int TLPixel = 0;
            unsigned int TRPixel = imageBuffWidth-1;
            unsigned int BLPixel = (imageBuffHeight-1)*imageBuffWidth;
            unsigned int BRPixel = (imageBuffHeight*imageBuffWidth)-1;


            regions region;

            int shift = (bitDepth<=8)?0:bitDepth-8;
            quint32 mask = (1<<bitDepth)-1;

            LOOP_START
                unsigned char* inPixel  = (unsigned char*)(&dataIn[dataIndex*bytesPerPixel]);
                unsigned int color = (dataIndex&1)|(((dataIndex/imageBuffWidth)&1)<<1);

                // Assume Central region
                region = REG_C;

                // If on an edge...
                if( i == 0 || j == 0 || i == outLast || j == inLast )
                {
                    // Determine where on edge
                    // (this will be simpler if we loop through source data rather than output image)

                    // If on top edge...
                    if( dataIndex < imageBuffWidth )
                    {
                        if     ( dataIndex == TLPixel ) region = REG_TL;
                        else if( dataIndex == TRPixel ) region = REG_TR;
                        else                            region = REG_T;
                    }

                    // If on bottom edge...
                    else if( dataIndex >= BLPixel)
                    {
                        if     ( dataIndex == BLPixel ) region = REG_BL;
                        else if( dataIndex == BRPixel ) region = REG_BR;
                        else                            region = REG_B;
                    }

                    // if on left or right edge...
                    else if( !(dataIndex % imageBuffWidth) ) region = REG_L;
                    else                                     region = REG_R;
                }



                switch( color )
                {
                    case 0: // red
                        r1 = (*((quint32*)inPixel))&mask;
                        switch( region )
                        {
                            case REG_C:
                                g1 = (*((quint32*)(&inPixel[TOffset])))&mask;
                                g2 = (*((quint32*)(&inPixel[BOffset])))&mask;
                                g3 = (*((quint32*)(&inPixel[LOffset])))&mask;
                                g4 = (*((quint32*)(&inPixel[ROffset])))&mask;
                                b1 = (*((quint32*)(&inPixel[TLOffset])))&mask;
                                b2 = (*((quint32*)(&inPixel[TROffset])))&mask;
                                b3 = (*((quint32*)(&inPixel[BLOffset])))&mask;
                                b4 = (*((quint32*)(&inPixel[BROffset])))&mask;
                                break;

                            case REG_TL:
                                g2 = (*((quint32*)(&inPixel[BOffset])))&mask;
                                g4 = (*((quint32*)(&inPixel[ROffset])))&mask;
                                g1 = g2;
                                g3 = g4;
                                b4 = (*((quint32*)(&inPixel[BROffset])))&mask;
                                b1 = b4;
                                b2 = b4;
                                b3 = b4;
                                break;

                            case REG_T:
                                g2 = (*((quint32*)(&inPixel[BOffset])))&mask;
                                g3 = (*((quint32*)(&inPixel[LOffset])))&mask;
                                g4 = (*((quint32*)(&inPixel[ROffset])))&mask;
                                g1 = (g2+g3+g4)/3;
                                b3 = (*((quint32*)(&inPixel[BLOffset])))&mask;
                                b4 = (*((quint32*)(&inPixel[BROffset])))&mask;
                                b1 = b3;
                                b2 = b4;
                                break;

                            case REG_TR:
                                g2 = (*((quint32*)(&inPixel[BOffset])))&mask;
                                g3 = (*((quint32*)(&inPixel[LOffset])))&mask;
                                g1 = g2;
                                g4 = g3;
                                b3 = (*((quint32*)(&inPixel[BLOffset])))&mask;
                                b1 = b3;
                                b2 = b3;
                                b4 = b3;
                                break;

                            case REG_L:
                                g1 = (*((quint32*)(&inPixel[TOffset])))&mask;
                                g2 = (*((quint32*)(&inPixel[BOffset])))&mask;
                                g4 = (*((quint32*)(&inPixel[ROffset])))&mask;
                                g3 =
                                b1 =
                                b2 = (*((quint32*)(&inPixel[TROffset])))&mask;
                                b3 =
                                b4 = (*((quint32*)(&inPixel[BROffset])))&mask;
                                break;

                            case REG_R:
                                g1 = (*((quint32*)(&inPixel[TOffset])))&mask;
                                g2 = (*((quint32*)(&inPixel[BOffset])))&mask;
                                g3 = (*((quint32*)(&inPixel[LOffset])))&mask;
                                g4 = (g1+g2+g3)/3;
                                b1 = (*((quint32*)(&inPixel[TLOffset])))&mask;
                                b3 = (*((quint32*)(&inPixel[BLOffset])))&mask;
                                b2 = b1;
                                b4 = b3;
                                break;

                            case REG_BL:
                                g1 = (*((quint32*)(&inPixel[TOffset])))&mask;
                                g4 = (*((quint32*)(&inPixel[ROffset])))&mask;
                                g2 = g1;
                                g3 = g4;
                                b2 = (*((quint32*)(&inPixel[TROffset])))&mask;
                                b1 = b2;
                                b3 = b2;
                                b4 = b2;
                                break;

                            case REG_B:
                                g1 = (*((quint32*)(&inPixel[TOffset])))&mask;
                                g3 = (*((quint32*)(&inPixel[LOffset])))&mask;
                                g4 = (*((quint32*)(&inPixel[ROffset])))&mask;
                                g2 = (g1+g3+g4)/3;
                                b1 = (*((quint32*)(&inPixel[TLOffset])))&mask;
                                b2 = (*((quint32*)(&inPixel[TROffset])))&mask;
                                b3 = b1;
                                b4 = b2;
                                break;

                            case REG_BR:
                                g1 = (*((quint32*)(&inPixel[TOffset])))&mask;
                                g3 = (*((quint32*)(&inPixel[LOffset])))&mask;
                                g2 = g1;
                                g4 = g3;
                                b1 = (*((quint32*)(&inPixel[TLOffset])))&mask;
                                b2 = b1;
                                b3 = b1;
                                b4 = b1;
                                break;

                        }

                        dataOut[buffIndex].p[2] = pixelLookup[r1>>shift].p[0];                  // red
                        dataOut[buffIndex].p[1] = pixelLookup[(g1+g2+g3+g4)>>(shift+2)].p[0];   // green
                        dataOut[buffIndex].p[0] = pixelLookup[(b1+b2+b3+b4)>>(shift+2)].p[0];   // blue

                        break;

                    case 1: // green 1
                        g1 = (*((quint32*)inPixel))&mask;
                        switch( region )
                        {
                            case REG_C:
                                r1 = (*((quint32*)(&inPixel[LOffset])))&mask;
                                r2 = (*((quint32*)(&inPixel[ROffset])))&mask;
                                b1 = (*((quint32*)(&inPixel[TOffset])))&mask;
                                b2 = (*((quint32*)(&inPixel[BOffset])))&mask;
                                break;

                            case REG_T:
                                r1 = (*((quint32*)(&inPixel[LOffset])))&mask;
                                r2 = (*((quint32*)(&inPixel[ROffset])))&mask;
                                b2 = (*((quint32*)(&inPixel[BOffset])))&mask;
                                b1 = b2;
                                break;

                            case REG_TR:
                                r1 = (*((quint32*)(&inPixel[LOffset])))&mask;
                                r2 = r1;
                                b2 = (*((quint32*)(&inPixel[BOffset])))&mask;
                                b1 = b2;
                                break;

                            case REG_R:
                                r1 = (*((quint32*)(&inPixel[LOffset])))&mask;
                                r2 = r1;
                                b1 = (*((quint32*)(&inPixel[TOffset])))&mask;
                                b2 = (*((quint32*)(&inPixel[BOffset])))&mask;
                                break;

                            case REG_B:
                                r1 = (*((quint32*)(&inPixel[LOffset])))&mask;
                                r2 = (*((quint32*)(&inPixel[ROffset])))&mask;
                                b1 = (*((quint32*)(&inPixel[TOffset])))&mask;
                                b2 = b1;
                                break;

                            case REG_BR:
                                r1 = (*((quint32*)(&inPixel[LOffset])))&mask;
                                r2 = r1;
                                b1 = (*((quint32*)(&inPixel[TOffset])))&mask;
                                b2 = b1;
                                break;

                            // Top left, Left, Bottom-Left is never green 1
                            case REG_TL:
                            case REG_L:
                            case REG_BL:
                                r1 = 0;
                                r2 = 0;
                                b1 = 0;
                                b2 = 0;
                                break;

                        }

                        dataOut[buffIndex].p[2] = pixelLookup[(r1+r2)>>(shift+1)].p[0]; // red
                        dataOut[buffIndex].p[1] = pixelLookup[g1>>shift].p[0];          // green
                        dataOut[buffIndex].p[0] = pixelLookup[(b1+b2)>>(shift+1)].p[0]; // blue

                        break;

                    case 2: // green 2
                        g2 = (*((quint32*)inPixel))&mask;
                        switch( region )
                        {
                            case REG_C:
                                r1 = (*((quint32*)(&inPixel[TOffset])))&mask;
                                r2 = (*((quint32*)(&inPixel[BOffset])))&mask;
                                b1 = (*((quint32*)(&inPixel[LOffset])))&mask;
                                b2 = (*((quint32*)(&inPixel[ROffset])))&mask;
                                break;

                            case REG_L:
                                r1 = (*((quint32*)(&inPixel[TOffset])))&mask;
                                r2 = (*((quint32*)(&inPixel[BOffset])))&mask;
                                b2 = (*((quint32*)(&inPixel[ROffset])))&mask;
                                b1 = b2;
                                break;

                            case REG_R:
                                r1 = (*((quint32*)(&inPixel[TOffset])))&mask;
                                r2 = (*((quint32*)(&inPixel[BOffset])))&mask;
                                b1 = (*((quint32*)(&inPixel[LOffset])))&mask;
                                b2 = b1;
                                break;

                            case REG_BL:
                                r1 = (*((quint32*)(&inPixel[TOffset])))&mask;
                                r2 = r1;
                                b2 = (*((quint32*)(&inPixel[ROffset])))&mask;
                                b1 = b2;
                                break;

                            case REG_B:
                                r1 = (*((quint32*)(&inPixel[TOffset])))&mask;
                                r2 = r1;
                                b1 = (*((quint32*)(&inPixel[LOffset])))&mask;
                                b2 = (*((quint32*)(&inPixel[ROffset])))&mask;
                                break;

                            case REG_BR:
                                r1 = (*((quint32*)(&inPixel[TOffset])))&mask;
                                r2 = r1;
                                b1 = (*((quint32*)(&inPixel[LOffset])))&mask;
                                b2 = b1;
                                break;

                            // Top left, Top, Top-Right is never green 2
                            case REG_TL:
                            case REG_T:
                            case REG_TR:
                                r1 = 0;
                                r2 = 0;
                                b1 = 0;
                                b2 = 0;
                                break;
                        }

                        dataOut[buffIndex].p[2] = pixelLookup[(r1+r2)>>(shift+1)].p[0]; // red
                        dataOut[buffIndex].p[1] = pixelLookup[g2>>shift].p[0];          // green
                        dataOut[buffIndex].p[0] = pixelLookup[(b1+b2)>>(shift+1)].p[0]; // blue

                        break;

                    case 3: // blue
                        b1 = (*((quint32*)inPixel))&mask;
                        switch( region )
                        {
                            case REG_C:
                                r1 = (*((quint32*)(&inPixel[TLOffset])))&mask;
                                r2 = (*((quint32*)(&inPixel[TROffset])))&mask;
                                r3 = (*((quint32*)(&inPixel[BLOffset])))&mask;
                                r4 = (*((quint32*)(&inPixel[BROffset])))&mask;
                                g1 = (*((quint32*)(&inPixel[TOffset])))&mask;
                                g2 = (*((quint32*)(&inPixel[BOffset])))&mask;
                                g3 = (*((quint32*)(&inPixel[LOffset])))&mask;
                                g4 = (*((quint32*)(&inPixel[ROffset])))&mask;
                                break;

                            // Top-left, Top, Top-right, Left, Bottom-Left is never blue
                            case REG_T:
                            case REG_TL:
                            case REG_TR:
                            case REG_L:
                            case REG_BL:
                                r1 = 0;
                                r2 = 0;
                                r3 = 0;
                                r4 = 0;
                                g1 = 0;
                                g2 = 0;
                                g3 = 0;
                                g4 = 0;
                                break;

                            case REG_R:
                                r1 = (*((quint32*)(&inPixel[TLOffset])))&mask;
                                r3 = (*((quint32*)(&inPixel[BLOffset])))&mask;
                                r2 = r1;
                                r4 = r3;
                                g1 = (*((quint32*)(&inPixel[TOffset])))&mask;
                                g2 = (*((quint32*)(&inPixel[BOffset])))&mask;
                                g3 = (*((quint32*)(&inPixel[LOffset])))&mask;
                                g4 = (g1+g2+g3)/3;
                                break;

                            case REG_B:
                                r1 = (*((quint32*)(&inPixel[TLOffset])))&mask;
                                r2 = (*((quint32*)(&inPixel[TROffset])))&mask;
                                r3 = r1;
                                r4 = r2;
                                g1 = (*((quint32*)(&inPixel[TOffset])))&mask;
                                g3 = (*((quint32*)(&inPixel[LOffset])))&mask;
                                g4 = (*((quint32*)(&inPixel[ROffset])))&mask;
                                g2 = (r1+r3+r4)/3;
                                break;

                            case REG_BR:
                                r1 = (*((quint32*)(&inPixel[TLOffset])))&mask;
                                r2 = r1;
                                r3 = r1;
                                r4 = r1;
                                g1 = (*((quint32*)(&inPixel[TOffset])))&mask;
                                g3 = (*((quint32*)(&inPixel[LOffset])))&mask;
                                g2 = g1;
                                g4 = g3;
                                break;

                        }

                        dataOut[buffIndex].p[2] = pixelLookup[(r1+r2+r3+r4)>>(shift+2)].p[0];   // red
                        dataOut[buffIndex].p[1] = pixelLookup[(g1+g2+g3+g4)>>(shift+2)].p[0];   // green
                        dataOut[buffIndex].p[0] = pixelLookup[b1>>shift].p[0];                  // blue

                        break;
                }


            LOOP_END
            break;
        }

        case imageDataFormats::RGB1:
        {
            //unsigned int rOffset = 0*imageDataSize;
            unsigned int gOffset = imageDataSize;
            unsigned int bOffset = 2*imageDataSize;
            int temp = 0;
            LOOP_START
                unsigned char* inPixel  = (unsigned char*)(&dataIn[dataIndex*bytesPerPixel]);
                dataOut[buffIndex].p[0] = pixelLookup[inPixel[bOffset]].p[0];
                dataOut[buffIndex].p[1] = pixelLookup[inPixel[gOffset]].p[0];
                dataOut[buffIndex].p[2] = pixelLookup[*inPixel].p[0];
                dataOut[buffIndex].p[3] = 0xff;
            LOOP_END
            break;
        }

        case imageDataFormats::RGB2:
        {
            //!!! not done yet - this is a copy of RGB1
            //unsigned int rOffset = 0*imageDataSize;
            unsigned int gOffset = imageDataSize;
            unsigned int bOffset = 2*imageDataSize;
            LOOP_START
                unsigned char* inPixel  = (unsigned char*)(&dataIn[dataIndex*bytesPerPixel]);
                dataOut[buffIndex].p[0] = pixelLookup[inPixel[bOffset]].p[0];
                dataOut[buffIndex].p[1] = pixelLookup[inPixel[gOffset]].p[0];
                dataOut[buffIndex].p[2] = pixelLookup[*inPixel].p[0];
                dataOut[buffIndex].p[3] = 0xff;
            LOOP_END
            break;
        }

        case imageDataFormats::RGB3:
        {
            //!!! not done yet - this is a copy of RGB1
            //unsigned int rOffset = 0*imageDataSize;
            unsigned int gOffset = imageDataSize;
            unsigned int bOffset = 2*imageDataSize;
            LOOP_START
                unsigned char* inPixel  = (unsigned char*)(&dataIn[dataIndex*bytesPerPixel]);
                dataOut[buffIndex].p[0] = pixelLookup[inPixel[bOffset]].p[0];
                dataOut[buffIndex].p[1] = pixelLookup[inPixel[gOffset]].p[0];
                dataOut[buffIndex].p[2] = pixelLookup[*inPixel].p[0];
                dataOut[buffIndex].p[3] = 0xff;
            LOOP_END
            break;
        }

        case imageDataFormats::YUV444:
        {
            //!!! not done yet - this is a copy of RGB1
            //unsigned int rOffset = 0*imageDataSize;
            unsigned int gOffset = imageDataSize;
            unsigned int bOffset = 2*imageDataSize;
            LOOP_START
                unsigned char* inPixel  = (unsigned char*)(&dataIn[dataIndex*bytesPerPixel]);
                dataOut[buffIndex].p[0] = pixelLookup[inPixel[bOffset]].p[0];
                dataOut[buffIndex].p[1] = pixelLookup[inPixel[gOffset]].p[0];
                dataOut[buffIndex].p[2] = pixelLookup[*inPixel].p[0];
                dataOut[buffIndex].p[3] = 0xff;
            LOOP_END
            break;
        }

        case imageDataFormats::YUV422:
        {
            //!!! not done yet - this is a copy of RGB1
            //unsigned int rOffset = 0*imageDataSize;
            unsigned int gOffset = imageDataSize;
            unsigned int bOffset = 2*imageDataSize;
            LOOP_START
                unsigned char* inPixel  = (unsigned char*)(&dataIn[dataIndex*bytesPerPixel]);
                dataOut[buffIndex].p[0] = pixelLookup[inPixel[bOffset]].p[0];
                dataOut[buffIndex].p[1] = pixelLookup[inPixel[gOffset]].p[0];
                dataOut[buffIndex].p[2] = pixelLookup[*inPixel].p[0];
                dataOut[buffIndex].p[3] = 0xff;
            LOOP_END
            break;
        }

        case imageDataFormats::YUV421:
        {
            //!!! not done yet - this is a copy of RGB1
            //unsigned int rOffset = 0*imageDataSize;
            unsigned int gOffset = imageDataSize;
            unsigned int bOffset = 2*imageDataSize;
            LOOP_START
                unsigned char* inPixel  = (unsigned char*)(&dataIn[dataIndex*bytesPerPixel]);
                dataOut[buffIndex].p[0] = pixelLookup[inPixel[bOffset]].p[0];
                dataOut[buffIndex].p[1] = pixelLookup[inPixel[gOffset]].p[0];
                dataOut[buffIndex].p[2] = pixelLookup[*inPixel].p[0];
                dataOut[buffIndex].p[3] = 0xff;
            LOOP_END
            break;
        }
    }

    // Generate a frame from the data
    QImage frameImage( (uchar*)(imageBuff.constData()), rotatedImageBuffWidth(), rotatedImageBuffHeight(), QImage::Format_RGB32 );

    // Display the new image
    videoWidget->setNewImage( frameImage, imageTime );

    // Update markups if required
    updateMarkupData();
}

// Return the size of the widget where the image will be presented
// It will be presented in the QEImage's main window used for full screen view,
// or in QEImage's scroll area
QSize QEImage::getVedioDestinationSize()
{
    // If full screen, return the size of the main window used for this
    // (sanity check, only do this if the full screen widget is present - it always should be in full screen)
    if( fullScreen && fullScreenMainWindow )
    {
        return fullScreenMainWindow->size();
    }
    // Not in full screen, the destination is the scroll area widget
    else
    {
        return scrollArea->size();
    }
}

// Set the image buffer used for generating images so it will be large enough to hold the processed image.
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
            QSize destSize = getVedioDestinationSize();
            double vScale = (double)(destSize.height()) / (double)(rotatedImageBuffHeight());
            double hScale = (double)(destSize.width()) / (double)(rotatedImageBuffWidth());
            double scale = (hScale<vScale)?hScale:vScale;

            videoWidget->resize( (int)((double)rotatedImageBuffWidth() * scale),
                                 (int)((double)rotatedImageBuffHeight() * scale) );
            zoom = scale * 100;

            // Update the info area
            infoUpdateZoom( zoom );

            break;
    }

    // Determine buffer size
    unsigned long buffSize = IMAGEBUFF_BYTES_PER_PIXEL * imageBuffWidth * imageBuffHeight;

    // Resize buffer
    imageBuff.resize( buffSize );
}

//=================================================================================================

// Allow a signal to supply a filename of an image that will be used instead of a live image
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
    setFormatOption( imageDataFormats::RGB1 );
    bitDepth = 8;
    setImageBuff();

    // Use the image data just like it came from a waveform variable
    setImage( baData, 4, alarmInfo, time, 0 );
}

//=================================================================================================

// Update data related to markups if required.
// This is called after displaying the image.
void QEImage::updateMarkupData()
{
    if( haveVSliceX )
    {
        generateVSliceUnscaled( vSliceX, vSliceThickness );
    }
    if( haveHSliceY )
    {
        generateHSliceUnscaled( hSliceY, hSliceThickness );
    }
    if( haveProfileLine )
    {
        generateProfileUnscaled( profileLineStart, profileLineEnd, profileThickness );
    }
    if( haveSelectedArea1 )
    {
        displaySelectedAreaInfo( 1, selectedArea1Point1, selectedArea1Point2 );
    }
    if( haveSelectedArea2 )
    {
        displaySelectedAreaInfo( 2, selectedArea2Point1, selectedArea2Point2 );
    }
    if( haveSelectedArea3 )
    {
        displaySelectedAreaInfo( 3, selectedArea3Point1, selectedArea3Point2 );
    }
    if( haveSelectedArea4 )
    {
        displaySelectedAreaInfo( 4, selectedArea4Point1, selectedArea4Point2 );
    }
}

//=================================================================================================

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

    // Update the info area
    infoUpdateZoom( zoom );
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

    return;
}

// Arbitrary line profile  changed
void QEImage::lineProfileChanged()
{
    // Write the arbitrary line profile variables.
    QEInteger *qca;
    qca = (QEInteger*)getQcaItem( LINE_PROFILE_X1_VARIABLE );
    if( qca ) qca->writeInteger( videoWidget->scaleOrdinate( profileLineStart.x() ));

    qca = (QEInteger*)getQcaItem( LINE_PROFILE_Y1_VARIABLE );
    if( qca ) qca->writeInteger(  videoWidget->scaleOrdinate( profileLineStart.y() ));

    qca = (QEInteger*)getQcaItem( LINE_PROFILE_X2_VARIABLE );
    if( qca ) qca->writeInteger( videoWidget->scaleOrdinate( profileLineEnd.x() ));

    qca = (QEInteger*)getQcaItem( LINE_PROFILE_Y2_VARIABLE );
    if( qca ) qca->writeInteger( videoWidget->scaleOrdinate( profileLineEnd.y() ));

    qca = (QEInteger*)getQcaItem( LINE_PROFILE_THICKNESS_VARIABLE );
    if( qca ) qca->writeInteger( profileThickness );

    return;
}

// Horizontal line profile changed
void QEImage::hozProfileChanged()
{
    // Write the horizontal line profile variable.
    QEInteger *qca;
    qca = (QEInteger*)getQcaItem( PROFILE_H_VARIABLE );
    if( qca ) qca->writeInteger( videoWidget->scaleOrdinate( hSliceY ));

    qca = (QEInteger*)getQcaItem( PROFILE_H_THICKNESS_VARIABLE );
    if( qca ) qca->writeInteger( hSliceThickness );

    return;
}

// Vertical line profile changed
void QEImage::vertProfileChanged()
{
    // Write the horizontal line profile variable.
    QEInteger *qca;
    qca = (QEInteger*)getQcaItem( PROFILE_V_VARIABLE );
    if( qca ) qca->writeInteger( videoWidget->scaleOrdinate( vSliceX ));

    qca = (QEInteger*)getQcaItem( PROFILE_V_THICKNESS_VARIABLE );
    if( qca ) qca->writeInteger( vSliceThickness );

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
    // If paused, resume
    if (paused)
    {
        // Resume display of most recent image
        pauseButton->setIcon( *pauseButtonIcon );
        pauseButton->setToolTip("Pause image display");
        paused = false;
    }

    // Not paused, so pause
    else
    {
        // Force a deep copy of the image data as it will be used while paused if the user
        // interacts with the image - even by just hovering over it and causing pixel information to be displayed.
        // For efficiency, the image data was generated from the original update data using QByteArray::fromRawData().
        // The data system keeps the raw data until the next update so QByteArray instances like 'image' will remain vaild.
        // The data system discards it and replaces it will the latest data after each update. So if not using the latest
        // data (such as when paused) the data in 'image' would become stale after an update.
        image.resize( image.count()+1);

        // Pause the display
        pauseButton->setIcon( *playButtonIcon );
        pauseButton->setToolTip("Resume image display");
        paused = true;
    }

    // Ensure the checked state of the internal pause buttons is set correctly.
    // (A change in pause state may have originated from the external QAction)
    pauseButton->setChecked( paused );

    // If there is an external QAction that can pause display, set its checked state correctly.
    // (A change in pause state may have originated from the interbal button )
    if( pauseExternalAction )
    {
        pauseExternalAction->setChecked( paused );
    }

    // Update the info area
    infoUpdatePaused( paused );

}

// Save button pressed
void QEImage::saveClicked()
{
    QFileDialog *qFileDialog;
    QStringList filterList;
    QString filename;
    bool result;

    qFileDialog = new QFileDialog(this, "Save displayed image", QDir::currentPath().append( QDir::separator() ).append("image.png") );
    filterList << "Tagged Image File Format (*.tiff)" << "Portable Network Graphics (*.png)" << "Windows Bitmap (*.bmp)" << "Joint Photographics Experts Group (*.jpg)";
    qFileDialog->setNameFilters(filterList);
    qFileDialog->setAcceptMode(QFileDialog::AcceptSave);

// Don't set default suffix since the filename as entered is checked for existance (and
// replacement confirmed with the user), then the filename with suffix is returned!
// this means a file may be overwritten without warning, or warning may be given,
// then a different file created
//    qFileDialog->setDefaultSuffix( "png" );

// Don't avoid native dialog as they are much richer.
//    qFileDialog->setOption ( QFileDialog::DontUseNativeDialog, true );

    if (qFileDialog->exec())
    {
        QImage qImage = copyImage();
        filename = qFileDialog->selectedFiles().at(0);

        if (qFileDialog->selectedNameFilter() == filterList.at(0))
        {
            result = qImage.save(filename, "TIFF");
        }
        else if (qFileDialog->selectedNameFilter() == filterList.at(1))
        {
            result = qImage.save(filename, "PNG");
        }
        else if (qFileDialog->selectedNameFilter() == filterList.at(2))
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

// Return a QImage based on the current image
QImage QEImage::copyImage()
{
    return QImage((uchar*) imageBuff.constData(), rotatedImageBuffWidth(), rotatedImageBuffHeight(), QImage::Format_RGB32);
}


// Update the video widget if the QEImage has changed
void QEImage::resizeEvent(QResizeEvent* )
{
    setImageBuff();
}


//==============================================================================

// Manage local brightness and contrast controls
void QEImage::doEnableBrightnessContrast( bool enableBrightnessContrast )
{
    if( !localBC )
    {
        return;
    }

    localBC->setVisible( enableBrightnessContrast );
}

// Manage contrast reversal
void QEImage::doContrastReversal( bool /*contrastReversal*/ )
{
    // Flag color lookup table is invalid
    pixelLookupValid = false;

    // Redraw the current image (don't wait for next update (image may be stalled)
    redraw();
}

// Manage vertical slice selection
void QEImage::doEnableVertSliceSelection( bool enableVSliceSelection )
{
    sMenu->setVSliceEnabled( enableVSliceSelection );

    // If disabling, and it is the current mode, then default to panning
    if( !enableVSliceSelection )
    {
        if( getSelectionOption() == SO_VSLICE )
        {
            sMenu->setChecked( QEImage::SO_PANNING );
            panModeClicked();
        }
        videoWidget->clearMarkup( imageMarkup::MARKUP_ID_V_SLICE );
    }
    else
    {
        videoWidget->showMarkup( imageMarkup::MARKUP_ID_V_SLICE );
    }
}

// Enable horizontal slice selection
void QEImage::doEnableHozSliceSelection( bool enableHSliceSelection )
{
    sMenu->setHSlicetEnabled( enableHSliceSelection );

    // If disabling, and it is the current mode, then default to panning
    if( !enableHSliceSelection )
    {
        if( getSelectionOption() == SO_HSLICE )
        {
            sMenu->setChecked( QEImage::SO_PANNING );
            panModeClicked();
        }
        videoWidget->clearMarkup( imageMarkup::MARKUP_ID_H_SLICE );
    }
    else
    {
        videoWidget->showMarkup( imageMarkup::MARKUP_ID_H_SLICE );
    }
}

// Enable area selection (used for ROI and zoom)
void QEImage::doEnableAreaSelection( bool enableAreaSelection )
{
    sMenu->setAreaEnabled( enableAreaSelection );

    // If disabling, and it is the current mode, then default to panning
    if( !enableAreaSelection )
    {
        if( ( ( getSelectionOption() == SO_AREA1 ) ||
            ( getSelectionOption() == SO_AREA2 ) ||
            ( getSelectionOption() == SO_AREA3 ) ||
            ( getSelectionOption() == SO_AREA4 )))
        {
            sMenu->setChecked( QEImage::SO_PANNING );
            panModeClicked();
        }
        videoWidget->clearMarkup( imageMarkup::MARKUP_ID_REGION1 );
        videoWidget->clearMarkup( imageMarkup::MARKUP_ID_REGION2 );
        videoWidget->clearMarkup( imageMarkup::MARKUP_ID_REGION3 );
        videoWidget->clearMarkup( imageMarkup::MARKUP_ID_REGION4 );
    }
    else
    {
        videoWidget->showMarkup( imageMarkup::MARKUP_ID_REGION1 );
        videoWidget->showMarkup( imageMarkup::MARKUP_ID_REGION2 );
        videoWidget->showMarkup( imageMarkup::MARKUP_ID_REGION3 );
        videoWidget->showMarkup( imageMarkup::MARKUP_ID_REGION4 );
    }
}

// Manage profile selection
void QEImage::doEnableProfileSelection( bool enableProfileSelection )
{
    sMenu->setProfileEnabled( enableProfileSelection );

    // If disabling, and it is the current mode, then default to panning
    if( !enableProfileSelection )
    {
        if( getSelectionOption() == SO_PROFILE )
        {
            sMenu->setChecked( QEImage::SO_PANNING );
            panModeClicked();
        }
        videoWidget->clearMarkup( imageMarkup::MARKUP_ID_LINE );
    }
    else
    {
        videoWidget->showMarkup( imageMarkup::MARKUP_ID_LINE );
    }
}

// Manage target selection
void QEImage::doEnableTargetSelection( bool enableTargetSelection )
{
    sMenu->setTargetEnabled( enableTargetSelection );
    targetButton->setVisible( enableTargetSelection );

    // If disabling, and it is the current mode, then default to panning
    if( !enableTargetSelection )
    {
        if( ( getSelectionOption() == SO_TARGET || getSelectionOption() == SO_BEAM ))
        {
            sMenu->setChecked( QEImage::SO_PANNING );
            panModeClicked();
        }
        videoWidget->clearMarkup( imageMarkup::MARKUP_ID_TARGET );
        videoWidget->clearMarkup( imageMarkup::MARKUP_ID_BEAM );
    }
    else
    {
        videoWidget->showMarkup( imageMarkup::MARKUP_ID_TARGET );
        videoWidget->showMarkup( imageMarkup::MARKUP_ID_BEAM );
    }
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
void QEImage::setFormatOption( imageDataFormats::formatOptions formatOptionIn )
{
    if( mFormatOption != formatOptionIn )
    {
        pixelLookupValid = false;
    }

    // Save the option
    mFormatOption = formatOptionIn;
}

imageDataFormats::formatOptions QEImage::getFormatOption()
{
    return mFormatOption;
}

// Allow user to set the bit depth for Mono video format
void QEImage::setBitDepth( unsigned int bitDepthIn )
{
    // Ensure bit depth is reasonable
    unsigned int sanitiedBitDepth = bitDepthIn;
    if( sanitiedBitDepth == 0 )
    {
        sanitiedBitDepth = 1;
    }
    else if( sanitiedBitDepth > 32 )
    {
        sanitiedBitDepth = 32;
    }

    // Invalidate pixel look up table if bit depth changes (it will be regenerated with the new depth when next needed)
    if( bitDepth != sanitiedBitDepth )
    {
        pixelLookupValid = false;
    }

    // Save the option
    bitDepth = sanitiedBitDepth;
}

unsigned int QEImage::getBitDepth()
{
    return bitDepth;
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

    // Update the info area
    infoUpdateZoom( zoom );

}

int QEImage::getZoom()
{
    return zoom;
}

// Rotation
void QEImage::setRotation( rotationOptions rotationIn )
{
    // Save the rotation requested
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

// Automatic setting of brightness and contrast on region selection
void QEImage::setAutoBrightnessContrast( bool autoBrightnessContrastIn )
{
    if( !localBC )
    {
        return;
    }

    localBC->setAutoBrightnessContrast( autoBrightnessContrastIn );
}

bool QEImage::getAutoBrightnessContrast()
{
    if( !localBC )
    {
        return false;
    }
    return localBC->getAutoBrightnessContrast();
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

// Show time
void QEImage::setShowTime(bool value)
{
    optionsDialog->optionSet( imageContextMenu::ICM_ENABLE_TIME, value );
}

bool QEImage::getShowTime()
{
    return optionsDialog->optionGet( imageContextMenu::ICM_ENABLE_TIME );
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

// Ellipse markup colour
void QEImage::setEllipseMarkupColor(QColor markupColor )
{
    videoWidget->setMarkupColor( imageMarkup::MARKUP_ID_ELLIPSE, markupColor );
}

QColor QEImage::getEllipseMarkupColor()
{
    return videoWidget->getMarkupColor( imageMarkup::MARKUP_ID_ELLIPSE );
}

// Display the button bar
void QEImage::setDisplayButtonBar( bool displayButtonBar )
{
    optionsDialog->optionSet( imageContextMenu::ICM_DISPLAY_BUTTON_BAR, displayButtonBar );
}

bool QEImage::getDisplayButtonBar()
{
    return optionsDialog->optionGet( imageContextMenu::ICM_DISPLAY_BUTTON_BAR );
}

// Show cursor pixel
void QEImage::setDisplayCursorPixelInfo( bool displayCursorPixelInfo )
{
    optionsDialog->optionSet( imageContextMenu::ICM_ENABLE_CURSOR_PIXEL, displayCursorPixelInfo );
}

bool QEImage::getDisplayCursorPixelInfo()
{
    return optionsDialog->optionGet( imageContextMenu::ICM_ENABLE_CURSOR_PIXEL );
}

// Show contrast reversal
void QEImage::setContrastReversal( bool contrastReversal )
{
    localBC->setContrastReversal( contrastReversal );
}

bool QEImage::getContrastReversal()
{
    return localBC->getContrastReversal();
}

// Enable vertical slice selection
void QEImage::setEnableVertSliceSelection( bool enableVSliceSelection )
{
    optionsDialog->optionSet( imageContextMenu::ICM_ENABLE_VERT, enableVSliceSelection );
}

bool QEImage::getEnableVertSliceSelection()
{
    return optionsDialog->optionGet( imageContextMenu::ICM_ENABLE_VERT );
}

// Enable horizontal slice selection
void QEImage::setEnableHozSliceSelection( bool enableHSliceSelection )
{
    optionsDialog->optionSet( imageContextMenu::ICM_ENABLE_HOZ, enableHSliceSelection );
}

bool QEImage::getEnableHozSliceSelection()
{
    return optionsDialog->optionGet( imageContextMenu::ICM_ENABLE_HOZ );
}

// Enable area selection (used for ROI and zoom)
void QEImage::setEnableAreaSelection( bool enableAreaSelection )
{
    optionsDialog->optionSet( imageContextMenu::ICM_ENABLE_AREA, enableAreaSelection );
}

bool QEImage::getEnableAreaSelection()
{
    return optionsDialog->optionGet( imageContextMenu::ICM_ENABLE_AREA );
}

// Enable profile selection
void QEImage::setEnableProfileSelection( bool enableProfileSelection )
{
    optionsDialog->optionSet( imageContextMenu::ICM_ENABLE_LINE, enableProfileSelection );
}

bool QEImage::getEnableProfileSelection()
{
    return optionsDialog->optionGet( imageContextMenu::ICM_ENABLE_LINE );
}

// Enable profile presentation
void QEImage::setEnableProfilePresentation( bool enableProfilePresentationIn )
{
    enableProfilePresentation = enableProfilePresentationIn;
}

bool QEImage::getEnableProfilePresentation()
{
    return enableProfilePresentation;
}

// Enable horizontal profile presentation
void QEImage::setEnableHozSlicePresentation( bool enableHozSlicePresentationIn )
{
    enableHozSlicePresentation = enableHozSlicePresentationIn;
}

bool QEImage::getEnableHozSlicePresentation()
{
    return enableHozSlicePresentation;
}

// Enable vertical profile presentation
void QEImage::setEnableVertSlicePresentation( bool enableVertSlicePresentationIn )
{
    enableVertSlicePresentation = enableVertSlicePresentationIn;
}

bool QEImage::getEnableVertSlicePresentation()
{
    return enableVertSlicePresentation;
}

// Enable target selection
void QEImage::setEnableTargetSelection( bool enableTargetSelection )
{
    optionsDialog->optionSet( imageContextMenu::ICM_ENABLE_TARGET, enableTargetSelection );
}

bool QEImage::getEnableTargetSelection()
{
    return optionsDialog->optionGet( imageContextMenu::ICM_ENABLE_TARGET );
}

// Enable local brightness and contrast controls if required
void QEImage::setEnableBrightnessContrast( bool enableBrightnessContrast )
{
    optionsDialog->optionSet( imageContextMenu::ICM_DISPLAY_BRIGHTNESS_CONTRAST, enableBrightnessContrast );
}

bool QEImage::getEnableBrightnessContrast()
{
    return optionsDialog->optionGet( imageContextMenu::ICM_DISPLAY_BRIGHTNESS_CONTRAST );
}

// Request the application host controls such as toolbars and profile views for this widget
void QEImage::setExternalControls( bool externalControlsIn )
{
    appHostsControls = externalControlsIn;
    presentControls();
}

bool QEImage::getExternalControls()
{
    return appHostsControls;
}

// Determine if a full context menu allowing manipulation of the image is available or not
void QEImage::setFullContextMenu( bool fullContextMenuIn )
{
    fullContextMenu = fullContextMenuIn;
    return;
}

bool QEImage::getFullContextMenu()
{
    return fullContextMenu;
}

// Display all markups for which there is data available.
void QEImage::setDisplayMarkups( bool displayMarkupsIn )
{
    displayMarkups = displayMarkupsIn;
}

bool QEImage::getDisplayMarkups()
{
    return displayMarkups;
}

// Application launching
// Program String
void QEImage::setProgram1( QString program ){ programLauncher1.setProgram( program ); }
QString QEImage::getProgram1(){ return programLauncher1.getProgram(); }
void QEImage::setProgram2( QString program ){ programLauncher2.setProgram( program ); }
QString QEImage::getProgram2(){ return programLauncher2.getProgram(); }

// Arguments String
void QEImage::setArguments1( QStringList arguments ){ programLauncher1.setArguments( arguments ); }
QStringList QEImage::getArguments1(){ return  programLauncher1.getArguments(); }
void QEImage::setArguments2( QStringList arguments ){ programLauncher2.setArguments( arguments ); }
QStringList QEImage::getArguments2(){ return  programLauncher2.getArguments(); }

// Startup option
void QEImage::setProgramStartupOption1( applicationLauncher::programStartupOptions programStartupOption ){ programLauncher1.setProgramStartupOption( programStartupOption ); }
applicationLauncher::programStartupOptions QEImage::getProgramStartupOption1(){ return programLauncher1.getProgramStartupOption(); }
void QEImage::setProgramStartupOption2( applicationLauncher::programStartupOptions programStartupOption ){ programLauncher2.setProgramStartupOption( programStartupOption ); }
applicationLauncher::programStartupOptions QEImage::getProgramStartupOption2(){ return programLauncher2.getProgramStartupOption(); }

// Legends
QString QEImage::getHozSliceLegend()                     { return videoWidget->getMarkupLegend( imageMarkup::MARKUP_ID_H_SLICE );        }
void    QEImage::setHozSliceLegend      ( QString legend ){        videoWidget->setMarkupLegend( imageMarkup::MARKUP_ID_H_SLICE, legend ); }
QString QEImage::getVertSliceLegend()                    { return videoWidget->getMarkupLegend( imageMarkup::MARKUP_ID_V_SLICE );        }
void    QEImage::setVertSliceLegend     ( QString legend ){        videoWidget->setMarkupLegend( imageMarkup::MARKUP_ID_V_SLICE, legend ); }
QString QEImage::getprofileLegend()                      { return videoWidget->getMarkupLegend( imageMarkup::MARKUP_ID_LINE );           }
void    QEImage::setProfileLegend       ( QString legend ){        videoWidget->setMarkupLegend( imageMarkup::MARKUP_ID_LINE,    legend ); }
QString QEImage::getAreaSelection1Legend()               { return videoWidget->getMarkupLegend( imageMarkup::MARKUP_ID_REGION1 );        }
void    QEImage::setAreaSelection1Legend( QString legend ){        videoWidget->setMarkupLegend( imageMarkup::MARKUP_ID_REGION1, legend ); }
QString QEImage::getAreaSelection2Legend()               { return videoWidget->getMarkupLegend( imageMarkup::MARKUP_ID_REGION2 );        }
void    QEImage::setAreaSelection2Legend( QString legend ){        videoWidget->setMarkupLegend( imageMarkup::MARKUP_ID_REGION2, legend ); }
QString QEImage::getAreaSelection3Legend()               { return videoWidget->getMarkupLegend( imageMarkup::MARKUP_ID_REGION3 );        }
void    QEImage::setAreaSelection3Legend( QString legend ){        videoWidget->setMarkupLegend( imageMarkup::MARKUP_ID_REGION3, legend ); }
QString QEImage::getAreaSelection4Legend()               { return videoWidget->getMarkupLegend( imageMarkup::MARKUP_ID_REGION4 );        }
void    QEImage::setAreaSelection4Legend( QString legend ){        videoWidget->setMarkupLegend( imageMarkup::MARKUP_ID_REGION4, legend ); }
QString QEImage::getTargetLegend()                       { return videoWidget->getMarkupLegend( imageMarkup::MARKUP_ID_TARGET );         }
void    QEImage::setTargetLegend        ( QString legend ){        videoWidget->setMarkupLegend( imageMarkup::MARKUP_ID_TARGET,  legend ); }
QString QEImage::getBeamLegend()                         { return videoWidget->getMarkupLegend( imageMarkup::MARKUP_ID_BEAM );           }
void    QEImage::setBeamLegend          ( QString legend ){        videoWidget->setMarkupLegend( imageMarkup::MARKUP_ID_BEAM,    legend ); }
QString QEImage::getEllipseLegend()                       { return videoWidget->getMarkupLegend( imageMarkup::MARKUP_ID_ELLIPSE );       }
void    QEImage::setEllipseLegend       ( QString legend ){        videoWidget->setMarkupLegend( imageMarkup::MARKUP_ID_ELLIPSE, legend ); }

// Full Screen property set/get
bool QEImage::getFullScreen()
{
    return fullScreen;
}

void QEImage::setFullScreen( bool fullScreenIn )
{
    // Save the current full screen state
    fullScreen = fullScreenIn;

    // Enter full screen
    if( fullScreen )
    {
        // Only do anything if not in designer, and no full screen window has been created already
        if( !inDesigner() && !fullScreenMainWindow )
        {
            // Create full screen window
            // (and set up context sensitive menu (right click menu))
            fullScreenMainWindow = new fullScreenWindow( this );
            fullScreenMainWindow->setContextMenuPolicy( Qt::CustomContextMenu );
            connect( fullScreenMainWindow, SIGNAL( customContextMenuRequested( const QPoint& )), this, SLOT( showImageContextMenuFullScreen( const QPoint& )));
            connect( fullScreenMainWindow, SIGNAL( fullScreenResize()), this, SLOT( fullScreenResize()));

            // Move the video widget into the full screen window and present it in full screen
            QWidget* w = scrollArea->takeWidget();
            fullScreenMainWindow->setCentralWidget( w );
            fullScreenMainWindow->showFullScreen();

            // Raise in front of whatever application the QEImage widget is in, and resize it
            // This is only required when the QEWidget is created before being loaded into
            // some other application widget hierarchy.
            // For example, when QEGui opens a .ui file containing a QEImage widget:
            //    - The QEImage is created when the .ui file is loaded (and on creation creates and uses the full screen widget here)
            //    - QEGui inserts the widgets created from the .ui file and presents it's main window (over the top of the QEImage's full screen window)
            // Note, a timer event is used to to wait for any particular elapsed time,
            //       but to ensure raising the full screen window occurs after an application creating
            //       this QEImage widget has finished doing whatever it is doing (which may include
            //       showing itself over the top of the full screen window.
            QTimer::singleShot( 0, this, SLOT(raiseFullScreen() ) );
        }
    }

    // Leave full screen
    else
    {
        // Only do anything if already presenting in full screen
        if( fullScreenMainWindow )
        {
            // Move the video widget back into the scroll area within the QEImage
            QWidget* w = fullScreenMainWindow->centralWidget();
            scrollArea->setWidget( w );

            // Destroy the fullscreen main window
            delete fullScreenMainWindow;
            fullScreenMainWindow = NULL;
        }
    }
}

// Ensure the full screen main window is in front of the application that created the QEImage widget,
// and resized to fit the screen.
// This is called as a timer event, not to create a delay (time is zero) but to ensure it is called after back in event loop
void QEImage::raiseFullScreen()
{
    if( fullScreenMainWindow )
    {
        fullScreenMainWindow->activateWindow();
        fullScreenMainWindow->raise();
        fullScreenMainWindow->setFocus();
    }
}

// Resize full screen once it has been managed
void QEImage::resizeFullScreen()
{
    setResizeOption( RESIZE_OPTION_FIT );
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
void QEImage::userSelection( imageMarkup::markupIds mode, bool complete, bool clearing, QPoint point1, QPoint point2, unsigned int thickness )
{
    // If creating or moving a markup...
    if( !clearing )
    {
        switch( mode )
        {
            case imageMarkup::MARKUP_ID_V_SLICE:
                vSliceX = point1.x();
                vSliceThickness = thickness;
                haveVSliceX = true;
                if( enableVertSlicePresentation )
                {
                    QTimer::singleShot( 0, this, SLOT(setVSliceControlsVisible() ) );
                    generateVSliceUnscaled(  vSliceX, vSliceThickness );
                }
                vertProfileChanged();
                break;

            case imageMarkup::MARKUP_ID_H_SLICE:
                hSliceY = point1.y();
                hSliceThickness = thickness;
                haveHSliceY = true;
                if( enableHozSlicePresentation )
                {
                    QTimer::singleShot( 0, this, SLOT(setHSliceControlsVisible() ) );
                    generateHSliceUnscaled( hSliceY, hSliceThickness );
                }
                hozProfileChanged();
                break;

            case imageMarkup::MARKUP_ID_REGION1:
                selectedArea1Point1 = point1;
                selectedArea1Point2 = point2;
                haveSelectedArea1 = true;

                zMenu->enableAreaSelected( haveSelectedArea1 );

                displaySelectedAreaInfo( 1, point1, point2 );
                if( localBC && localBC->getAutoBrightnessContrast() )
                {
                    setRegionAutoBrightnessContrast( point1, point2 );
                }

                if( complete )
                {
                    roi1Changed();
                }
                break;

            case imageMarkup::MARKUP_ID_REGION2:
                selectedArea2Point1 = point1;
                selectedArea2Point2 = point2;
                haveSelectedArea2 = true;

                displaySelectedAreaInfo( 2, point1, point2 );
                if( localBC && localBC->getAutoBrightnessContrast() )
                {
                    setRegionAutoBrightnessContrast( point1, point2 );
                }

                if( complete )
                {
                    roi2Changed();
                }
                break;

            case imageMarkup::MARKUP_ID_REGION3:
                selectedArea3Point1 = point1;
                selectedArea3Point2 = point2;
                haveSelectedArea3 = true;

                displaySelectedAreaInfo( 3, point1, point2 );
                if( localBC && localBC->getAutoBrightnessContrast() )
                {
                    setRegionAutoBrightnessContrast( point1, point2 );
                }

                if( complete )
                {
                    roi3Changed();
                }
                break;

            case imageMarkup::MARKUP_ID_REGION4:
                selectedArea4Point1 = point1;
                selectedArea4Point2 = point2;
                haveSelectedArea4 = true;

                displaySelectedAreaInfo( 4, point1, point2 );
                if( localBC && localBC->getAutoBrightnessContrast() )
                {
                    setRegionAutoBrightnessContrast( point1, point2 );
                }

                if( complete )
                {
                    roi4Changed();
                }
                break;

            case imageMarkup::MARKUP_ID_LINE:
                profileLineStart = point1;
                profileLineEnd = point2;
                profileThickness = thickness;
                haveProfileLine = true;
                if( enableProfilePresentation )
                {
                    QTimer::singleShot( 0, this, SLOT(setLineProfileControlsVisible() ) );
                    generateProfileUnscaled( profileLineStart, profileLineEnd, profileThickness );
                }

                lineProfileChanged();
                break;

            case imageMarkup::MARKUP_ID_TARGET:
                {
                    targetInfo.setPoint( videoWidget->scalePoint( point1 ) );

                    // Write the target variables.
                    QEInteger *qca;
                    qca = (QEInteger*)getQcaItem( TARGET_X_VARIABLE );
                    if( qca ) qca->writeInteger( targetInfo.getPoint().x() );

                    qca = (QEInteger*)getQcaItem( TARGET_Y_VARIABLE );
                    if( qca ) qca->writeInteger( targetInfo.getPoint().y() );

                    // Display textual info
                    infoUpdateTarget( targetInfo.getPoint().x(), targetInfo.getPoint().y() );
                }
                break;

            case imageMarkup::MARKUP_ID_BEAM:
                {
                    beamInfo.setPoint( videoWidget->scalePoint( point1 ) );

                    // Write the beam variables.
                    QEInteger *qca;
                    qca = (QEInteger*)getQcaItem( BEAM_X_VARIABLE );
                    if( qca ) qca->writeInteger( beamInfo.getPoint().x() );

                    qca = (QEInteger*)getQcaItem( BEAM_Y_VARIABLE );
                    if( qca ) qca->writeInteger( beamInfo.getPoint().y() );

                    // Display textual info
                    infoUpdateBeam( beamInfo.getPoint().x(), beamInfo.getPoint().y() );
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
                QTimer::singleShot( 0, this, SLOT(setVSliceControlsNotVisible() ) );
                infoUpdateVertProfile();
                break;

            case imageMarkup::MARKUP_ID_H_SLICE:
                hSliceY = 0;
                haveHSliceY = false;
                QTimer::singleShot( 0, this, SLOT(setHSliceControlsNotVisible() ) );
                infoUpdateHozProfile();
                break;

            case imageMarkup::MARKUP_ID_REGION1:
                selectedArea1Point1 = QPoint();
                selectedArea1Point2 = QPoint();
                haveSelectedArea1 = false;
                infoUpdateRegion( 1 );

                zMenu->enableAreaSelected( haveSelectedArea1 );
                break;

            case imageMarkup::MARKUP_ID_REGION2:
                selectedArea2Point1 = QPoint();
                selectedArea2Point2 = QPoint();
                haveSelectedArea2 = false;
                infoUpdateRegion( 2 );
                break;

            case imageMarkup::MARKUP_ID_REGION3:
                selectedArea3Point1 = QPoint();
                selectedArea3Point2 = QPoint();
                haveSelectedArea3 = false;
                infoUpdateRegion( 3 );
                break;

            case imageMarkup::MARKUP_ID_REGION4:
                selectedArea4Point1 = QPoint();
                selectedArea4Point2 = QPoint();
                haveSelectedArea4 = false;
                infoUpdateRegion( 4 );
                break;

            case imageMarkup::MARKUP_ID_LINE:
                profileLineStart = QPoint();
                profileLineEnd = QPoint();
                haveProfileLine = false;

                QTimer::singleShot( 0, this, SLOT(setLineProfileControlsNotVisible() ) );
                infoUpdateProfile();
                break;

            case imageMarkup::MARKUP_ID_TARGET:
                infoUpdateTarget();
                break;

            case imageMarkup::MARKUP_ID_BEAM:
                infoUpdateBeam();
                break;

            default:
                break;

        }
    }
}

//==================================================
// Slots to make profile plots appear or disappear
// They are used as timer events to ensure resize events (that happen as the controls are inserted or deleted)
// don't cause a redraw of markups while handling a markup draw event

// Slot to make vertical slice profile plot appear
void QEImage::setVSliceControlsVisible()
{
    if( !appHostsControls )
    {
        vSliceLabel->setVisible( true );
    }
    if( vSliceDisplay )
    {
        vSliceDisplay->setVisible( true );
    }
}

// Slot to make vertical slice profile plot disapear
void QEImage::setVSliceControlsNotVisible()
{
    vSliceLabel->setVisible( false );
    if( vSliceDisplay )
    {
        vSliceDisplay->setVisible( false );
    }
}

// Slot to make horizontal slice profile plot appear
void QEImage::setHSliceControlsVisible()
{
    if( !appHostsControls )
    {
        hSliceLabel->setVisible( true );
    }
    if( hSliceDisplay )
    {
        hSliceDisplay->setVisible( true );
    }
}

// Slot to make horizontal profile plot disapear
void QEImage::setHSliceControlsNotVisible()
{
    hSliceLabel->setVisible( false );
    if( hSliceDisplay )
    {
        hSliceDisplay->setVisible( false );
    }
}

// Slot to make arbitrary line profile plot appear
void QEImage::setLineProfileControlsVisible()
{
    if( !appHostsControls )
    {
        profileLabel->setVisible( true );
    }
    if( profileDisplay )
    {
        profileDisplay->setVisible( true );
    }
}

// Slot to make arbitrary line profile plot disapear
void QEImage::setLineProfileControlsNotVisible()
{
    profileLabel->setVisible( false );
    if( profileDisplay )
    {
        profileDisplay->setVisible( false );
    }
}

//==================================================

// Determine the maximum pixel value for the current format
double QEImage::maxPixelValue()
{
    double result = 0;

    switch( mFormatOption )
    {
        case imageDataFormats::BAYER:
        case imageDataFormats::MONO:
            result = (1<<bitDepth)-1;
            break;

        case imageDataFormats::RGB1:
        case imageDataFormats::RGB2:
        case imageDataFormats::RGB3:
            result = (1<<8)-1; //???!!! not done yet probably correct
            break;

        case imageDataFormats::YUV444:
        case imageDataFormats::YUV422:
        case imageDataFormats::YUV421:
            result = (1<<8)-1; //???!!! not done yet probably correct
            break;
    }

    return result;
}

// Return a pointer to pixel data in the original image data.
// The position parameter is scaled to the original image size but reflects
// the displayed rotation and flip options, so it must be transformed first.
// Return NULL, if there is no image data, or point is beyond end of image data
const unsigned char* QEImage::getImageDataPtr( QPoint& pos )
{
    QPoint posTr;

    // Transform the position to reflect the original unrotated or flipped data
    posTr = rotateFlipPoint( pos );

    // Set up reference to start of the data, and the index to the required pixel
    const unsigned char* data = (unsigned char*)image.constData();
    int index = (posTr.x()+posTr.y()*imageBuffWidth)*bytesPerPixel;

    // Return a pointer to the pixel data if possible
    if( !image.isEmpty() && index < image.size() )
    {
        return &(data[index]);
    }
    else
    {
        return NULL;
    }
}

// Display textual info about a selected area
void QEImage::displaySelectedAreaInfo( int region, QPoint point1, QPoint point2 )
{
    infoUpdateRegion( region, videoWidget->scaleOrdinate( point1.x() ),
                              videoWidget->scaleOrdinate( point1.y() ),
                              videoWidget->scaleOrdinate( point2.x() ),
                              videoWidget->scaleOrdinate( point2.y() ));
}

// Update the brightness and contrast, if in auto, to match the recently selected region
void QEImage::setRegionAutoBrightnessContrast( QPoint point1, QPoint point2 )
{
    // Get the area corners scaled to match the original image data
    QPoint corner1( videoWidget->scaleOrdinate( point1.x() ), videoWidget->scaleOrdinate( point1.y() ) );
    QPoint corner2( videoWidget->scaleOrdinate( point2.x() ), videoWidget->scaleOrdinate( point2.y() ) );

    // Translate the corners to match the current flip and roate options
    QRect area = rotateFlipRectangle( corner1, corner2 );

    // Determine the range of pixel values in the selected area
    unsigned int min, max;
    getPixelRange( area, &min, &max );

    // Range of pixel values in area
    int range = (max>min)?max-min:1;

    // Calculate the contrast that will set the dynamic range
    // to match the range of pixels in the area.
    double newContrastDouble = (double)(maxPixelValue())/(double)(range);

    // Calculate the brightness that will set the dynamic range
    // to match the range of pixels in the area.
    // Offset from mid pixel value of range to mid pixel value in area (in original pixel scale) scaled for new contrast...
    double midOffset = (((double)(maxPixelValue())/2)-((double)(min+max)/2))*newContrastDouble;

    // Calculate brightness that will offset pixel values in the selected area to use full range.
    // Note, when used, the brightness will be multiplied by (the new pixel range - an offset used to center the new pixel range )
    double newBrightnessDouble = midOffset/(maxPixelValue()*(newContrastDouble-(newContrastDouble-1)/2));

    // Update the local brightness and contrast
    if( localBC )
    {
        localBC->setBrightnessContrast( newBrightnessDouble*100.0, newContrastDouble*100.0);
    }
}

// Determine the range of pixel values an area of the image
void QEImage::getPixelRange( const QRect& area, unsigned int* min, unsigned int* max )
{
    // If the area selected was the the entire image, and the image was not presented at 100%, rounding areas while scaling
    // may result in area dimensions outside than the actual image by a pixel or so, so limit the area to within the image.
    unsigned int areaX = (area.topLeft().x()>=0)?area.topLeft().x():0;
    unsigned int areaY = (area.topLeft().y()>=0)?area.topLeft().y():0;
    unsigned int areaW = (area.width()<=(int)rotatedImageBuffWidth())?area.width():rotatedImageBuffWidth();
    unsigned int areaH = (area.height()<=(int)rotatedImageBuffHeight())?area.height():rotatedImageBuffHeight();

    // Set up to step pixel by pixel through the area
    const unsigned char* data = (unsigned char*)image.constData();
    unsigned int index = (areaY*rotatedImageBuffWidth()+areaX)*bytesPerPixel;

    // This function is called as the user drags region handles around the
    // screen. Recalculating min and max pixels for large areas
    // for each mouse movement event needs to be efficient so speed loop by
    // extracting width and height. (Compiler can't assume QRect width
    // and height stays constant so it is evaluated each iteration of for
    // loop if it was in the form   'for( int i = 0; i < area.height(); i++ )'
    unsigned int stepW = bytesPerPixel;

    // Calculate the step to the start of the next row in the area selected.
    unsigned int stepH = (rotatedImageBuffWidth()-areaW)*bytesPerPixel;

    unsigned int maxP = 0;
    unsigned int minP = UINT_MAX;

    // Determine the maximum and minimum pixel values in the area
    for( unsigned int i = 0; i < areaH; i++ )
    {
        for( unsigned int j = 0; j < areaW; j++ )
        {
            unsigned int p = getPixelValueFromData( &(data[index]) );
            if( p < minP ) minP = p;
            if( p > maxP ) maxP = p;

            index += stepW;
        }
        index += stepH;
    }

    // Return results
    *min = minP;
    *max = maxP;
}

//=====================================================================
// Slots to use signals from the Brightness/contrast control

// The brightness or contrast or contrast reversal has changed
void QEImage::brightnessContrastChanged()
{
    // Flag that the current pixel lookup table needs recalculating
    pixelLookupValid = false;

    // Present the updated image
    displayImage();
}

// A request has been made to set the brightness and contrast to suit the current image
void QEImage::brightnessContrastAutoImageRequest()
{
    setRegionAutoBrightnessContrast( QPoint( 0, 0), QPoint( videoWidget->width(), videoWidget->height()) );
}

//=====================================================================

// Generate a profile along a line down an image at a given X position
// Input ordinates are at the resolution of the displayed image (not scaled to the source image data)
// The profile contains values for each pixel intersected by the line.
// See generateVSlice() below for further details
void QEImage::generateVSliceUnscaled( int xUnscaled, unsigned int thicknessUnscaled )
{
    if( !vSliceDisplay )
    {
        return;
    }

    // Scale the ordinate to the original image data
    int x = videoWidget->scaleOrdinate( xUnscaled );

    // Scale the thickness to the original image data. (thickness of 1 pixel is not scaled, 1 is the minimum)
    // Note, thickness is not an ordinate, but scaleOrdinate
    unsigned int thickness = (thicknessUnscaled>1)?std::max(1,videoWidget->scaleOrdinate( thicknessUnscaled )):1;

    // Generate the profile
    generateVSlice( x, thickness );
}

// Generate a profile along a line down an image at a given X position
// Input ordinates are scaled to the source image data.
// The profile contains values for each pixel intersected by the line.
void QEImage::generateVSlice( int x, unsigned int thickness )
{
    if( !vSliceDisplay )
    {
        return;
    }

    // Display textual info
    infoUpdateVertProfile( x, thickness );

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
    const unsigned char* dataPtr = &(data[x*bytesPerPixel]);
    int dataPtrStep = rotatedImageBuffWidth()*bytesPerPixel;

    // Set up to step through the line thickness
    unsigned int halfThickness = thickness/2;
    int xMin = x-halfThickness;
    if( xMin < 0 ) xMin = 0;
    int xMax =  xMin+thickness;
    if( xMax >= (int)rotatedImageBuffWidth() ) xMax = rotatedImageBuffWidth();

    // Accumulate data for each pixel in the thickness
    bool firstPass = true;
    for( int nextX = xMin; nextX < xMax; nextX++ )
    {
        // Accumulate the image data value at each pixel.
        // The buffer is filled backwards so the plot, which sits on its side beside the image is drawn correctly
        QPoint pos;
        pos.setX( nextX );
        for( int i = rotatedImageBuffHeight()-1; i >= 0; i-- )
        {
            pos.setY( i );
            QPointF* dataPoint = &vSliceData[i];
            double value = getFloatingPixelValueFromData( getImageDataPtr( pos ) );

            // On first pass, set up X and Y
            if( firstPass )
            {
                dataPoint->setY( i );
                dataPoint->setX( value );
            }

            // On subsequent passes (when thickness is greater than 1), accumulate X
            else
            {
                dataPoint->setX( dataPoint->x() + value );
            }

            dataPtr += dataPtrStep;
        }

        firstPass = false;
    }

    // Calculate average pixel values if more than one pixel thick
    if( thickness > 1 )
    {
        for( int i = rotatedImageBuffHeight()-1; i >= 0; i-- )
        {
            QPointF* dataPoint = &vSliceData[i];
            dataPoint->setX( dataPoint->x()/thickness );
        }
    }

    // Write the profile data
    QEFloating *qca;
    qca = (QEFloating*)getQcaItem( PROFILE_V_ARRAY );
    if( qca )
    {
        int arraySize = vSliceData.size();
        QVector<double> waveform(arraySize);
        for( int i = 0; i < arraySize; i++ )
        {
            waveform[i] = vSliceData[i].x();
        }
        qca->writeFloating( waveform );
    }

    // Display the profile
    QDateTime dt = QDateTime::currentDateTime();
    QString title = QString( "Vertical profile - " ).append( getSubstitutedVariableName( IMAGE_VARIABLE ) ).append( dt.toString(" - dd.MM.yyyy HH:mm:ss.zzz") );
    vSliceDisplay->setProfile( &vSliceData, maxPixelValue(), 0.0, (double)(vSliceData.size()), 0.0, title, QPoint( x, 0 ), QPoint( x, rotatedImageBuffHeight()-1 ), thickness );
}

// Generate a profile along a line across an image at a given Y position
// Input ordinates are at the resolution of the displayed image (not scaled to the source image data)
// The profile contains values for each pixel intersected by the line.
// See generateHSlice() below for further details
void QEImage::generateHSliceUnscaled( int yUnscaled, unsigned int thicknessUnscaled )
{
    if( !hSliceDisplay )
    {
        return;
    }

    // Scale the ordinate to the original image data
    int y = videoWidget->scaleOrdinate( yUnscaled );

    // Scale the thickness to the original image data. (thickness of 1 pixel is not scaled, 1 is the minimum)
    // Note, thickness is not an ordinate, but scaleOrdinate
    unsigned int thickness = (thicknessUnscaled>1)?std::max(1,videoWidget->scaleOrdinate( thicknessUnscaled )):1;

    // Generate the profile
    generateHSlice( y, thickness );
}

// Generate a profile along a line across an image at a given Y position
// Input ordinates are at the resolution of the source image data
// The profile contains values for each pixel intersected by the line.
void QEImage::generateHSlice( int y, unsigned int thickness )
{
    if( !hSliceDisplay )
    {
        return;
    }

    // Display textual info
    infoUpdateHozProfile( y, thickness );

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
    const unsigned char* dataPtr = &(data[y*rotatedImageBuffWidth()*bytesPerPixel]);
    int dataPtrStep = bytesPerPixel;

    // Set up to step through the line thickness
    unsigned int halfThickness = thickness/2;
    int yMin = y-halfThickness;
    if( yMin < 0 ) yMin = 0;
    int yMax =  yMin+thickness;
    if( yMax >= (int)rotatedImageBuffHeight() ) yMax = rotatedImageBuffHeight();

    // Accumulate data for each pixel in the thickness
    bool firstPass = true;
    for( int nextY = yMin; nextY < yMax; nextY++ )
    {
        // Accumulate the image data value at each pixel.
        QPoint pos;
        pos.setY( nextY );
        for( unsigned int i = 0; i < rotatedImageBuffWidth(); i++ )
        {
            pos.setX( i );
            QPointF* dataPoint = &hSliceData[i];
            double value = getFloatingPixelValueFromData( getImageDataPtr( pos ) );

            // On first pass, set up X and Y
            if( firstPass )
            {
                dataPoint->setX( i );
                dataPoint->setY( value );
            }

            // On subsequent passes (when thickness is greater than 1), accumulate X
            else
            {
                dataPoint->setY( dataPoint->y() + value );
            }

            dataPtr += dataPtrStep;
        }

        firstPass = false;
    }


    // Calculate average pixel values if more than one pixel thick
    if( thickness > 1 )
    {
        for( unsigned int i = 0; i < rotatedImageBuffWidth(); i++ )
        {
            QPointF* dataPoint = &hSliceData[i];
            dataPoint->setY( dataPoint->y()/thickness );
        }
    }

    // Write the profile data
    QEFloating *qca;
    qca = (QEFloating*)getQcaItem( PROFILE_H_ARRAY );
    if( qca )
    {
        int arraySize = hSliceData.size();
        QVector<double> waveform(arraySize);
        for( int i = 0; i < arraySize; i++ )
        {
            waveform[i] = hSliceData[i].y();
        }
        qca->writeFloating( waveform );
    }

    // Display the profile
    QDateTime dt = QDateTime::currentDateTime();
    QString title = QString( "Horizontal profile - " ).append( getSubstitutedVariableName( IMAGE_VARIABLE ) ).append( dt.toString(" - dd.MM.yyyy HH:mm:ss.zzz") );
    hSliceDisplay->setProfile( &hSliceData, 0.0, (double)(hSliceData.size()), 0.0,  maxPixelValue(), title, QPoint( y, 0 ), QPoint( y, rotatedImageBuffWidth()-1 ), thickness );
}

// Generate a profile along an arbitrary line through an image.
// Input ordinates are at the resolution of the displayed image (not scaled to the source image data)
// See generateProfile() below for further details
void QEImage::generateProfileUnscaled( QPoint point1Unscaled, QPoint point2Unscaled, unsigned int thicknessUnscaled )
{
    if( !profileDisplay )
    {
        return;
    }

    // Scale the coordinates to the original image data
    QPoint point1 = videoWidget->scalePoint( point1Unscaled );
    QPoint point2 = videoWidget->scalePoint( point2Unscaled );

    // Scale the thickness to the original image data. (thickness of 1 pixel is not scaled, 1 is the minimum)
    // Note, thickness is not an ordinate, but scaleOrdinate
    unsigned int thickness = (thicknessUnscaled>1)?std::max(1,videoWidget->scaleOrdinate( thicknessUnscaled )):1;

    // Generate the profile
    generateProfile( point1, point2, thickness );
}

// Generate a profile along an arbitrary line through an image.
// Input ordinates are scaled to the source image data.
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
// The line has a notional thickness. The above processing for a single
// pixel width is repeated with the start and end points moved at right
// angles to the line by a 'pixel' distance up to the line thickness.
// The results are then averaged.
//
void QEImage::generateProfile( QPoint point1, QPoint point2, unsigned int thickness )
{
    if( !profileDisplay )
    {
        return;
    }

    // Display textual information
    infoUpdateProfile( point1, point2, thickness );

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

    // Step on each axis to move one 'pixel' length
    double xStep = dX/len;
    double yStep = dY/len;

    // Starting point in center of start pixel
    double initX = point1.x()+0.5;
    double initY = point1.y()+0.5;

    // Ensure output buffer is the correct size
    if( profileData.size() != len )
    {
       profileData.resize( int( len ) );
    }

    // Integer pixel length
    int intLen = (int)len;

    // Parrallel passes will be made one 'pixel' away from each other up to the thickness required.
    // Determine the offset for the first pass.
    // Note, this will not add an offset for a thickness of 1 pixel
    initX -= yStep * (double)(thickness-1) / 2;
    initY += xStep * (double)(thickness-1) / 2;

    // Accumulate a set of values for each pixel width up to the thickness required
    bool firstPass = true;
    for( unsigned int j = 0; j < thickness; j++ )
    {
        // Starting point for this pass
        double x = initX;
        double y = initY;

        // Calculate a value for each pixel length along the selected line
        for( int i = 0; i < intLen; i++ )
        {
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
                // The larger the proportion of the real pixel overlayed, the greated the weight.
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


                // Calculate the weighted value
                value = value / pixelsInValue * 4;

                // Move on to the next 'point'
                x+=xStep;
                y+=yStep;
            }

            // Use a value of zero if the point is not within the image (user can drag outside the image)
            else
            {
                value = 0.0;
            }

            // Get a reference to the current data point
            QPointF* data = &profileData[i];

            // If the first pass, set the X axis and the initial data value
            if( firstPass )
            {
                data->setX( i );
                data->setY( value );
            }

            // On consequent passes, accumulate the data value
            else
            {
                data->setY( data->y() + value );
            }
        }

        initX += yStep;
        initY -= xStep;

        firstPass = false;

    }

    // Average the values
    for( int i = 0; i < intLen; i++ )
    {
        QPointF* data = &profileData[i];
        data->setY( data->y() / thickness );
    }

    // Write the profile data
    QEFloating *qca;
    qca = (QEFloating*)getQcaItem( PROFILE_LINE_ARRAY );
    if( qca )
    {
        int arraySize = profileData.size();
        QVector<double> waveform(arraySize);
        for( int i = 0; i < arraySize; i++ )
        {
            waveform[i] = profileData[i].y();
        }
        qca->writeFloating( waveform );
    }

    // Update the profile display
    QDateTime dt = QDateTime::currentDateTime();
    QString title = QString( "Line profile - " ).append( getSubstitutedVariableName( IMAGE_VARIABLE ) ).append( dt.toString(" - dd.MM.yyyy HH:mm:ss.zzz") );
    profileDisplay->setProfile( &profileData, 0.0, (double)(profileData.size()), 0.0,  maxPixelValue(), title, point1, point2, thickness );
}

//=================================================================================================


// Return a number representing a pixel intensity given a pointer into an image data buffer.
// Note, the pointer is indexed according to the pixel data size which will be at least
// big enough for the data format.
int QEImage::getPixelValueFromData( const unsigned char* ptr )
{
    // Sanity check
    if( !ptr )
        return 0;

    // Case the data to the correct size, then return the data as a floating point number.
    switch( mFormatOption )
    {
        case imageDataFormats::BAYER:
        case imageDataFormats::MONO:
            {
                unsigned int usableDepth = bitDepth;
                if( bitDepth > (imageDataSize*8) )
                {
                    usableDepth = imageDataSize*8;
                }

                quint32 mask = (1<<usableDepth)-1;

                return (*((quint32*)ptr))&mask;
            }

        case imageDataFormats::RGB1:
            {
                // for RGB, average all colors
                unsigned int pixel = *(unsigned int*)ptr;
                return ((pixel&0xff0000>>16) + (pixel&0x00ff00>>8) + (pixel&0x0000ff)) / 3;
            }

        case imageDataFormats::RGB2:
            //!!! not done - copy of RGB1
            {
                // for RGB, average all colors
                unsigned int pixel = *(unsigned int*)ptr;
                return ((pixel&0xff0000>>16) + (pixel&0x00ff00>>8) + (pixel&0x0000ff)) / 3;
            }

        case imageDataFormats::RGB3:
            //!!! not done - copy of RGB1
            {
                // for RGB, average all colors
                unsigned int pixel = *(unsigned int*)ptr;
                return ((pixel&0xff0000>>16) + (pixel&0x00ff00>>8) + (pixel&0x0000ff)) / 3;
            }

        case imageDataFormats::YUV444:
            //!!! not done - copy of RGB1
            {
                // for RGB, average all colors
                unsigned int pixel = *(unsigned int*)ptr;
                return ((pixel&0xff0000>>16) + (pixel&0x00ff00>>8) + (pixel&0x0000ff)) / 3;
            }

        case imageDataFormats::YUV422:
            //!!! not done - copy of RGB1
            {
                // for RGB, average all colors
                unsigned int pixel = *(unsigned int*)ptr;
                return ((pixel&0xff0000>>16) + (pixel&0x00ff00>>8) + (pixel&0x0000ff)) / 3;
            }

        case imageDataFormats::YUV421:
            //!!! not done - copy of RGB1
            {
                // for RGB, average all colors
                unsigned int pixel = *(unsigned int*)ptr;
                return ((pixel&0xff0000>>16) + (pixel&0x00ff00>>8) + (pixel&0x0000ff)) / 3;
            }
    }

    // Avoid compilation warning (not sure why this is required as all cases are handled in switch statements.
    return *ptr;
}

// Return a floating point number representing a pixel intensity given a pointer into an image data buffer.
double QEImage::getFloatingPixelValueFromData( const unsigned char* ptr )
{
    return getPixelValueFromData( ptr );
}

// Transform a rectangle (defined by two points) according to current rotation and flip options.
QRect QEImage::rotateFlipRectangle( QPoint& pos1, QPoint& pos2 )
{
    QPoint trPos1 = rotateFlipPoint( pos1 );
    QPoint trPos2 = rotateFlipPoint( pos2 );

    QRect trRect( trPos1, trPos2 );
    trRect = trRect.normalized();

    return trRect;
}

// Transform the point according to current rotation and flip options.
QPoint QEImage::rotateFlipPoint( QPoint& pos )
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
    // Don't do anything if no image data yet
    if( image.isEmpty() )
    {
        return;
    }

    // If the pixel is not within the image, display nothing
    QString s;
    if( pos.x() < 0 || pos.y() < 0 || pos.x() >= (int)rotatedImageBuffWidth() || pos.y() >= (int)rotatedImageBuffHeight() )
    {
        infoUpdatePixel();
    }

    // If the pixel is within the image, display the pixel position and value
    else
    {
        // Extract the pixel data from the original image data
        int value = getPixelValueFromData( getImageDataPtr( pos ) );
        infoUpdatePixel( pos, value );
    }
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

//=================================================================================================
// Slot to redraw the current image.
// Required when properties change, such as contrast reversal, or when the video widget changes, such as a resize
void QEImage::redraw()
{
    qcaobject::QCaObject* qca = getQcaItem( IMAGE_VARIABLE );
    if( qca )
    {
        qca->resendLastData();
    }
}

//=================================================================================================
// Present the context menu
// (When in full screen)
void QEImage::showImageContextMenuFullScreen( const QPoint& pos )
{
    QPoint globalPos = fullScreenMainWindow->mapToGlobal( pos );
    showImageContextMenuCommon( pos, globalPos );
}

// Present the context menu
// (When not in full screen)
void QEImage::showImageContextMenu( const QPoint& pos )
{
    QPoint globalPos = mapToGlobal( pos );
    showImageContextMenuCommon( pos, globalPos );
}

// Present the context menu
// (full screen and not full screen)
void QEImage::showImageContextMenuCommon( const QPoint& pos, const QPoint& globalPos )
{
    // If the markup system wants to put up a menu, let it do so
    // For example, if the user has clicked over a markup, it may offer the user a menu
    if( videoWidget->showMarkupMenu( videoWidget->mapFrom( this, pos ), globalPos ) )
    {
        return;
    }

    if( fullContextMenu )
    {
        // Create the standard context menu
        QMenu* cm = buildContextMenu();

        // Add the Selection menu
        cm->addSeparator();
        sMenu->setChecked( getSelectionOption() );
        cm->addMenu( sMenu );

        // Add menu items

        //                    Title                            checkable  checked                     option
        addMenuItem( cm,      "Save...",                       false,     false,                      imageContextMenu::ICM_SAVE                     );
        addMenuItem( cm,      paused?"Resume":"Pause",         true,      paused,                     imageContextMenu::ICM_PAUSE                    );

        addMenuItem( cm,      "About image...",                false,     false,                      imageContextMenu::ICM_ABOUT_IMAGE              );

        // Add the zoom menu
        zMenu->enableAreaSelected( haveSelectedArea1 );
        cm->addMenu( zMenu );

        // Add the flip/rotate menu
        frMenu->setChecked( rotation, flipHoz, flipVert );
        cm->addMenu( frMenu );

        // Add 'full scree' item
        addMenuItem( cm,      "Full Screen",                   true,      fullScreen,                 imageContextMenu::ICM_FULL_SCREEN              );

        // Add option... dialog
        addMenuItem( cm,      "Options...",                    false,     false,                      imageContextMenu::ICM_OPTIONS                  );

        // Present the menu
        imageContextMenu::imageContextMenuOptions option;
        bool checked;
        QAction* selectedItem = showContextMenuGlobal( cm, globalPos );
        if( selectedItem )
        {
            option = (imageContextMenu::imageContextMenuOptions)(selectedItem->data().toInt());
            checked = selectedItem->isChecked();
        }
        else
        {
            option = imageContextMenu::ICM_NONE;
            checked = false;
        }

        // Act on the selected option.
        // (there won't be one if a standard context menu option was selected)
        optionAction( option, checked );
    }
    else
    {
        showContextMenuGlobal( globalPos );
    }

}

// Act on a selection from the option menu or dialog
void QEImage::optionAction( imageContextMenu::imageContextMenuOptions option, bool checked )
{
    // Act on the menu selection
    switch( option )
    {
        default:
        case imageContextMenu::ICM_NONE: break;

        case imageContextMenu::ICM_SAVE:                        saveClicked();                         break;
        case imageContextMenu::ICM_PAUSE:                       pauseClicked();                        break;
        case imageContextMenu::ICM_ENABLE_CURSOR_PIXEL:         showInfo                  ( checked ); break;
        case imageContextMenu::ICM_ABOUT_IMAGE:                 showImageAboutDialog();                break;
        case imageContextMenu::ICM_ENABLE_TIME:                 videoWidget->setShowTime  ( checked ); break;
        case imageContextMenu::ICM_ENABLE_VERT:                 doEnableVertSliceSelection( checked ); break;
        case imageContextMenu::ICM_ENABLE_HOZ:                  doEnableHozSliceSelection ( checked ); break;
        case imageContextMenu::ICM_ENABLE_AREA:                 doEnableAreaSelection     ( checked ); break;
        case imageContextMenu::ICM_ENABLE_LINE:                 doEnableProfileSelection  ( checked ); break;
        case imageContextMenu::ICM_ENABLE_TARGET:               doEnableTargetSelection   ( checked ); break;
        case imageContextMenu::ICM_DISPLAY_BUTTON_BAR:          buttonGroup->setVisible   ( checked ); break;
        case imageContextMenu::ICM_DISPLAY_BRIGHTNESS_CONTRAST: doEnableBrightnessContrast( checked ); break;
        case imageContextMenu::ICM_FULL_SCREEN:                 setFullScreen             ( checked ); break;
        case imageContextMenu::ICM_OPTIONS:                     optionsDialog->exec( this );           break;

        // Note, zoom options caught by zoom menu signal
        // Note, rotate and flip options caught by flip rotate menu signal
    }
}

// Act on a selection from the zoom menu
void QEImage::zoomMenuTriggered( QAction* selectedItem )
{
    switch( (imageContextMenu::imageContextMenuOptions)(selectedItem->data().toInt()) )
    {
        default:
        case imageContextMenu::ICM_NONE: break;

        case imageContextMenu::ICM_ZOOM_SELECTED:       zoomToArea();                           break;
        case imageContextMenu::ICM_ZOOM_FIT:            setResizeOption( RESIZE_OPTION_FIT );   break;
        case imageContextMenu::ICM_ZOOM_PLUS:           zoomInOut( 10 );                        break;
        case imageContextMenu::ICM_ZOOM_MINUS:          zoomInOut( -10 );                       break;
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

// Act on a selection from the flip/rotate menu
void QEImage::flipRotateMenuTriggered( QAction* selectedItem )
{
    switch( (imageContextMenu::imageContextMenuOptions)(selectedItem->data().toInt()) )
    {
        default:
        case imageContextMenu::ICM_NONE: break;

        case imageContextMenu::ICM_ROTATE_RIGHT:        setRotation( selectedItem->isChecked()?ROTATION_90_RIGHT:ROTATION_0 );               break;
        case imageContextMenu::ICM_ROTATE_LEFT:         setRotation( selectedItem->isChecked()?ROTATION_90_LEFT:ROTATION_0 );                break;
        case imageContextMenu::ICM_ROTATE_180:          setRotation( selectedItem->isChecked()?ROTATION_180:ROTATION_0 );                    break;

        case imageContextMenu::ICM_FLIP_HORIZONTAL:     setHorizontalFlip( selectedItem->isChecked() ); break;
        case imageContextMenu::ICM_FLIP_VERTICAL:       setVerticalFlip  ( selectedItem->isChecked() ); break;
    }

    // Update the checked state of the buttons now the user has selected an option.
    // Note, this is also called before displaying the menu to reflect any property
    // changes from other sources
    frMenu->setChecked( rotation, flipHoz, flipVert );
}

// Act on a selection from the select menu
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

//=================================================================================================
// Present information about the image.
// This is usefull when trying to determine why an image is not displaying well.
void QEImage::showImageAboutDialog()
{
    // Build the image information string
    QString about = QString ("QEImage image information:\n");

    about.append( QString( "\nSize (bytes) of CA data array: %1" ).arg( image.count() ));
    about.append( QString( "\nSize (bytes) of CA data elements: %1" ).arg( imageDataSize ));
    about.append( QString( "\nWidth (pixels) taken from dimension variables or width variable: %1" ).arg( imageBuffWidth ));
    about.append( QString( "\nHeight (pixels) taken from dimension variables or height variable: %1" ).arg( imageBuffHeight ));
    about.append( QString( "\nPixel depth taken from bit depth variable or bit depth property: %1" ).arg( bitDepth ));

    QString name;
    switch( mFormatOption )
    {
        case imageDataFormats::MONO:        name = "Monochrome";    break;
        case imageDataFormats::BAYER:       name = "Bayer";         break;
        case imageDataFormats::RGB1:        name = "8 bit RGB";     break;
        case imageDataFormats::RGB2:        name = "RGB2???";       break;
        case imageDataFormats::RGB3:        name = "RGB3???";       break;
        case imageDataFormats::YUV444:      name = "???bit YUV444"; break;
        case imageDataFormats::YUV422:      name = "???bit YUV422"; break;
        case imageDataFormats::YUV421:      name = "???bit YUV421"; break;
    }

    about.append( QString( "\nExpected format: " ).append( name ));

    about.append( "\n\nFirst bytes of raw image data:\n   ");
    if( image.isEmpty() )
    {
        about.append( "No data yet." );
    }
    else
    {
        int count = 20;
        if( image.count() < count )
        {
            count = image.count() ;
        }
        for( int i = 0; i < count; i++ )
        {
            about.append( QString( " %1" ).arg( (unsigned char)(image[i]) ));
        }
    }

    about.append( "\n\nFirst pixels of 32 bit RGBA image data (after flipping, rotating and clipping:");
    if( imageBuff.isEmpty() )
    {
        about.append( "\n   No data yet." );
    }
    else
    {
        int count = 20;
        if( imageBuff.count() < count )
        {
            count = imageBuff.count() ;
        }
        for( int i = 0; i < count; i += 4 )
        {
            about.append( QString( "\n   [%1, %2, %3, %4]" ).arg( (unsigned char)(imageBuff[i+0]) )
                                                            .arg( (unsigned char)(imageBuff[i+1]) )
                                                            .arg( (unsigned char)(imageBuff[i+2]) )
                                                            .arg( (unsigned char)(imageBuff[i+3]) ));
        }
    }

// Note if mpeg stuff if included.
// To include mpeg stuff, don't define QE_USE_MPEG directly, define environment variable
// QE_FFMPEG to be processed by framework.pro
#ifdef QE_USE_MPEG
    about.append( "\n\nImage MPEG URL: " ).append( (!getURL().isEmpty())?getURL():"No URL" );
#else
    about.append( "\n\nImage MPEG URL: ---MPEG source not enabled in this build---" );
#endif // QE_USE_MPEG

    qcaobject::QCaObject *qca;

    qca = getQcaItem( IMAGE_VARIABLE );
    about.append( "\n\nImage data variable: " ).append( (qca!=0)?qca->getRecordName():"No variable" );

    qca = getQcaItem( FORMAT_VARIABLE );
    about.append( "\n\nImage format variable: " ).append( (qca!=0)?qca->getRecordName():"No variable" );

    qca = getQcaItem( BIT_DEPTH_VARIABLE );
    about.append( "\n\nBit depth variable: " ).append( (qca!=0)?qca->getRecordName():"No variable" );

    qca = getQcaItem( WIDTH_VARIABLE );
    about.append( "\nImage width variable: " ).append( (qca!=0)?qca->getRecordName():"No variable" );

    qca = getQcaItem( HEIGHT_VARIABLE );
    about.append( "\nImage height variable: " ).append( (qca!=0)?qca->getRecordName():"No variable" );

    qca = getQcaItem( NUM_DIMENSIONS_VARIABLE );
    about.append( "\n\nImage data dimensions variable: " ).append( (qca!=0)?qca->getRecordName():"No variable" );

    qca = getQcaItem( DIMENSION_0_VARIABLE );
    about.append( "\n\nImage dimension 1 variable: " ).append( (qca!=0)?qca->getRecordName():"No variable" );

    qca = getQcaItem( DIMENSION_1_VARIABLE );
    about.append( "\n\nImage dimension 2 variable: " ).append( (qca!=0)?qca->getRecordName():"No variable" );

    qca = getQcaItem( DIMENSION_2_VARIABLE );
    about.append( "\n\nImage dimension 3 variable: " ).append( (qca!=0)?qca->getRecordName():"No variable" );

    // Display the 'about' text
    QMessageBox::about(this, "About Image", about );
}

// Perform a named action.
// A call to this method originates from the application that created this widget and arrives with the
// support of the windowCustomisation class.
// The 'originator' QAction reference parameter is the QAction from the application's menu item or button
// generating this action request.
//
// For each action:
//
//     If initialising:
//         - If this widget will need to manipulate the application's menu item or button generating this
//           action request (such as disable it, or set its checked state), then save the 'originator' QAction
//           reference for future use.
//         - If the 'triggered' signal from the 'originator' QAction can be used directly, then connect to it.
//
//     If not initialising:
//         - Nothing may be required if already connected to the 'triggered' signal from the 'originator' QAction.
//         or
//         - Perform the action required.
//         - Optionally manipulate the 'originator' QAction. For example, enable it, disable it, modify its
//           label, or set its checked state as required. Note, if manipulating the 'originator' QAction
//           from within this function, the originator reference does not need to be saved when initialising
//           as it is passed in on each call.
void QEImage::actionRequest( QString action, QStringList /*arguments*/, bool initialise, QAction* originator )
{

    // Save button
    if( action == "Save...")
    {
        if( initialise )
        {
            QObject::connect(originator, SIGNAL(triggered()), this, SLOT(saveClicked()));
        }
    }

    // Pause button
    else if( action == "Pause")
    {
        if( initialise )
        {
            pauseExternalAction = originator;
            pauseExternalAction->setCheckable( true );
            QObject::connect(pauseExternalAction, SIGNAL(triggered()), this, SLOT(pauseClicked()));
        }
    }

    // Positioning button
    else if( action == "Move target position into beam")
    {
        if( initialise )
        {
            QObject::connect(originator, SIGNAL(triggered()), this, SLOT(targetClicked()));
        }
    }

    // About image button
    else if( action == "About image..." )
    {
        if( !initialise )
        {
            showImageAboutDialog();
        }
    }

    // Zoom menu
    else if( action == "Zoom" )
    {
        if( initialise )
        {
            originator->setMenu( zMenu );
        }
    }

    // Flip/Rotate menu
    else if( action == "Flip/Rotate" )
    {
        if( initialise )
        {
            originator->setMenu( frMenu );
        }
    }

    // Mode menu
    else if( action == "Mode" )
    {
        if( initialise )
        {
            originator->setMenu( sMenu );
        }
    }

    // Options dialog
    else if( action == "Options..." )
    {
        if( !initialise )
        {
            optionsDialog->exec( this );
        }
    }

    // Copy Image
    else if( action == "Copy" )
    {
        if( !initialise )
        {
            contextMenuTriggered( CM_COPY_DATA );
        }
    }

    // Launch Application 1
    else if( action == "LaunchApplication1" )
    {
        if( !initialise )
        {
            programLauncher1.launchImage( this, copyImage() );
        }
    }

    // Launch Application 2
    else if( action == "LaunchApplication2" )
    {
        if( !initialise )
        {
            programLauncher2.launchImage( this, copyImage() );
        }
    }

    // Show in fullscreen mode
    else if( action == "Full Screen" )
    {
        if( !initialise )
        {
            setFullScreen( true );
        }
    }

    // Unimplemented action
    else
    {
        sendMessage( QString( "QEImage widget has recieved the following unimplemented action request: ").append( action ));
    }

}

