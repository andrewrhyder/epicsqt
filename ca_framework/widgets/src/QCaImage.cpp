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

/*!
  This class is a CA aware image widget based on the Qt frame widget.
  It is tighly integrated with the base class QCaWidget. Refer to QCaWidget.cpp for details.

  This class displays images from byte array (originating from a EPICS waveform record)
  It determines the width and height from other EPICS variables.
  The user can interact with the image.
  The image is managed by the VideoWidget class.
  User interaction and drawing markups over the image (such as selecting an area) is managed by the imageMarkup class.
 */

#include <QCaImage.h>
#include <QCaByteArray.h>
#include <QCaInteger.h>

/*!
    Constructor with no initialisation
*/
QCaImage::QCaImage( QWidget *parent ) : QFrame( parent ), QCaWidget( this ) {
    setup();
}

/*!
    Constructor with known variable
*/
QCaImage::QCaImage( const QString &variableNameIn, QWidget *parent ) : QFrame( parent ), QCaWidget( this )  {
    setup();
    setVariableName( variableNameIn, 0 );
}

/*!
    Setup common to all constructors
*/
void QCaImage::setup() {

    // Set up data
    // This control uses the following data sources:
    //  - image
    //  - width
    //  - height
    setNumVariables( QCAIMAGE_NUM_VARIABLES );

    // Set up default properties
    caEnabled = true;
    caVisible = true;
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
    pauseEnabled = false;

    displayPauseButton = false;
    displaySaveButton = false;
    displayZoomButton = false;
    displayRoiButton = false;

    haveVSliceX = false;
    haveHSliceY = false;
    haveProfileLine = false;
    haveSelectedArea = false;

//!!!all property variables initialised?

    // Set the initial state
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;


    imageDataSize = 0;

    // Use frame signals
    // --Currently none--

    // Create the video destination
    videoWidget = new VideoWidget;
    setMarkupColor(QColor(0, 255, 0));
    QObject::connect( videoWidget, SIGNAL( userSelection( imageMarkup::markupModes, QPoint, QPoint, QPoint, QPoint ) ),
                      this,        SLOT  ( userSelection( imageMarkup::markupModes, QPoint, QPoint, QPoint, QPoint )) );
    QObject::connect( videoWidget, SIGNAL( zoomInOut( int ) ),
                      this,        SLOT  ( zoomInOut( int ) ) );
    QObject::connect( videoWidget, SIGNAL( currentPixelInfo( QPoint ) ),
                      this,        SLOT  ( currentPixelInfo( QPoint ) ) );


    // Add the video destination to the widget
    scrollArea = new QScrollArea;
    scrollArea->setFrameStyle( QFrame::NoFrame );
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget( videoWidget );
    scrollArea->setEnabled( false );  // Reflects initial disconnected state

    // Add the pixel display labels
    currentCursorPixelLabel = new QLabel();
    currentVertPixelLabel = new QLabel();
    currentHozPixelLabel = new QLabel();
    currentLineLabel = new QLabel();
    currentAreaLabel = new QLabel();

    infoLayout = new QHBoxLayout();
    infoLayout->addWidget( currentCursorPixelLabel );
    infoLayout->addWidget( currentVertPixelLabel );
    infoLayout->addWidget( currentHozPixelLabel );
    infoLayout->addWidget( currentLineLabel );
    infoLayout->addWidget( currentAreaLabel, 1 );


    // Create vertical, horizontal, and general profile plots
    vSliceDisplay = new profilePlot();
    vSliceDisplay->setMinimumWidth( 100 );

    hSliceDisplay = new profilePlot();
    hSliceDisplay->setMinimumHeight( 100 );

    profileDisplay = new profilePlot();
    profileDisplay->setMinimumHeight( 100 );


    QGridLayout* graphicsLayout = new QGridLayout();
    graphicsLayout->addWidget( scrollArea, 0, 0 );
    graphicsLayout->addLayout( infoLayout, 1, 0 );
    graphicsLayout->addWidget( vSliceDisplay, 0, 1 );
    graphicsLayout->addWidget( hSliceDisplay, 2, 0 );
    graphicsLayout->addWidget( profileDisplay, 3, 0 );

    graphicsLayout->setColumnStretch( 0, 1 );  // display image to take all spare room
    graphicsLayout->setRowStretch( 0, 1 );  // display image to take all spare room

    // Create label group
    labelGroup = new QGroupBox();
    labelGroup->setTitle( "Details");
    QGridLayout* labelLayout = new QGridLayout();
    labelLayout->setMargin( 0 );
    labelGroup->setLayout( labelLayout);

    acquirePeriodQELabel = new QELabel( this );
    acquirePeriodLabel = new QLabel( this );
    acquirePeriodLabel->setText( "Acquire Period:" );

    exposureTimeQELabel = new QELabel( this );
    exposureTimeLabel = new QLabel( this );
    exposureTimeLabel->setText( "Exposure Time:" );

    labelLayout->addWidget( acquirePeriodLabel, 0, 0 );
    labelLayout->addWidget( acquirePeriodQELabel, 0, 1 );
    labelLayout->addWidget( exposureTimeLabel, 1, 0 );
    labelLayout->addWidget( exposureTimeQELabel, 1, 1 );

    labelLayout->setColumnStretch( 2, 1 );
    labelLayout->setRowStretch( 2, 1 );


    // Create region of interest group
    roiGroup = new QGroupBox();
    roiGroup->setTitle( "R.O.I.");
    QGridLayout* roiLayout = new QGridLayout();
    roiLayout->setMargin( 0 );
    roiGroup->setLayout( roiLayout);


    roiXQELabel = new QELabel( roiGroup );
    roiXLabel = new QLabel( roiGroup );
    roiXLabel->setText( "X:" );

    roiYQELabel = new QELabel( roiGroup );
    roiYLabel = new QLabel( roiGroup );
    roiYLabel->setText( "Y:" );

    roiWQELabel = new QELabel( roiGroup );
    roiWLabel = new QLabel( roiGroup );
    roiWLabel->setText( "Width:" );

    roiHQELabel = new QELabel( roiGroup );
    roiHLabel = new QLabel( roiGroup );
    roiHLabel->setText( "Height:" );


    roiLayout->addWidget( roiXLabel, 0, 0 );
    roiLayout->addWidget( roiXQELabel, 0, 1 );
    roiLayout->addWidget( roiYLabel, 1, 0 );
    roiLayout->addWidget( roiYQELabel, 1, 1 );
    roiLayout->addWidget( roiWLabel, 2, 0 );
    roiLayout->addWidget( roiWQELabel, 2, 1 );
    roiLayout->addWidget( roiHLabel, 3, 0 );
    roiLayout->addWidget( roiHQELabel, 3, 1 );

    roiLayout->setColumnStretch( 2, 1 );


    // Create button group
    buttonGroup = new QGroupBox();
    buttonGroup->setTitle( "Actions");
    QGridLayout* buttonLayout = new QGridLayout();
    buttonLayout->setMargin( 0 );
    buttonGroup->setLayout( buttonLayout);


    pauseButton= new QPushButton(buttonGroup);
    pauseButton->setText("Pause");
    pauseButton->setToolTip("Pause image display");
    QObject::connect(pauseButton, SIGNAL(clicked()), this, SLOT(pauseClicked()));

    saveButton = new QPushButton(buttonGroup);
    saveButton->setText("Save");
    saveButton->setToolTip("Save displayed image");
    QObject::connect(saveButton, SIGNAL(clicked()), this, SLOT(saveClicked()));

    roiButton = new QPushButton(buttonGroup);
    roiButton->setText("ROI");
    roiButton->setToolTip("Apply selected area to Region Of Interst");
    roiButton->setEnabled( false );
    QObject::connect(roiButton, SIGNAL(clicked()), this, SLOT(roiClicked()));

    zoomButton = new QPushButton(buttonGroup);
    zoomButton->setText("Zoom");
    zoomButton->setToolTip("Zoom to selected area");
    zoomButton->setEnabled( false );
    QObject::connect(zoomButton, SIGNAL(clicked()), this, SLOT(zoomClicked()));


    buttonLayout->addWidget(pauseButton, 0, 0);
    buttonLayout->addWidget(saveButton, 0, 1);
    buttonLayout->addWidget(roiButton, 1, 0);
    buttonLayout->addWidget(zoomButton, 1, 1);

    buttonLayout->setColumnStretch( 2, 1 );

    // Create area selection options
    areaSelectionGroup = new QGroupBox( "Selection", this );
    QVBoxLayout* areaSelectionLayout = new QVBoxLayout(); // !!! are layouts added to a widget deleted with the widget???
    areaSelectionLayout->setMargin( 0 );
    areaSelectionGroup->setLayout( areaSelectionLayout);

    vSliceSelectMode = new QRadioButton( "Vertical Slice", areaSelectionGroup );
    vSliceSelectMode->setToolTip("Allows user to select a vertical slice through the image");

    hSliceSelectMode = new QRadioButton( "Horizontal Slice", areaSelectionGroup );
    hSliceSelectMode->setToolTip("Allows user to select a horizontal slice across the image");

    areaSelectMode = new QRadioButton( "Area (ROI and Zoom)", areaSelectionGroup );
    areaSelectMode->setToolTip("Allows user to select an area of the image for Region Of Interest, of for local zoom");

    profileSelectMode = new QRadioButton( "Profile", areaSelectionGroup );
    profileSelectMode->setToolTip("Allows user to select a line within the image");


    QObject::connect(vSliceSelectMode, SIGNAL(clicked()), this, SLOT(vSliceSelectModeClicked()));
    QObject::connect(hSliceSelectMode, SIGNAL(clicked()), this, SLOT(hSliceSelectModeClicked()));
    QObject::connect(areaSelectMode, SIGNAL(clicked()), this, SLOT(areaSelectModeClicked()));
    QObject::connect(profileSelectMode, SIGNAL(clicked()), this, SLOT(profileSelectModeClicked()));




    areaSelectionLayout->addWidget(vSliceSelectMode, 0 );
    areaSelectionLayout->addWidget(hSliceSelectMode, 1 );
    areaSelectionLayout->addWidget(areaSelectMode, 2 );
    areaSelectionLayout->addWidget(profileSelectMode, 3 );

    manageSelectionOptions();



    // Create main layout containing image, label, and button layouts
    mainLayout = new QGridLayout;
    mainLayout->setMargin( 0 );

    mainLayout->addLayout( graphicsLayout, 0, 0, 1, 0 );
    mainLayout->addWidget( areaSelectionGroup, 1, 0  );
    mainLayout->addWidget( labelGroup, 1, 1 );
    mainLayout->addWidget( buttonGroup, 2, 0 );
    mainLayout->addWidget( roiGroup, 2, 1 );

    // Set graphics to take all spare room
    mainLayout->setColumnStretch( 1, 1 );
    mainLayout->setRowStretch( 0, 1 );

//    mainLayout->setStretch( 0, 1 );  // Graphics to take all spare room

    setLayout( mainLayout );

    // Set up labels as required by properties
    manageRoiLayout();

    manageAcquirePeriodLabel();
    manageExposureTimeLabel();

    managePauseButton();
    manageSaveButton();
    manageRoiButton();
    manageZoomButton();





    // Initially set the video widget to the size of the scroll bar
    // This will be resized when the image size is known
    videoWidget->resize( scrollArea->width(), scrollArea->height() );

    // Set image size to zero
    // Image will not be presented until size is available
    imageBuffWidth = 0;
    imageBuffHeight = 0;
}

QCaImage::~QCaImage()
{
    delete videoWidget;

}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
*/
qcaobject::QCaObject* QCaImage::createQcaItem( unsigned int variableIndex ) {

    switch( variableIndex )
    {
        // Create the image item as a QCaByteArray
        case IMAGE_VARIABLE:
            return new QCaByteArray( getSubstitutedVariableName( variableIndex ), this, variableIndex );

        // Create the width item as a QCaInteger
        case WIDTH_VARIABLE:
            return new QCaInteger( getSubstitutedVariableName( variableIndex ), this, &integerFormatting, variableIndex );

        // Create the heigh item as a QCaInteger
        case HEIGHT_VARIABLE:
            return new QCaInteger( getSubstitutedVariableName( variableIndex ), this, &integerFormatting, variableIndex );

        // Don't create anything - just pass on the variable name and substitutions on to the acquire period QELabel
        case ACQUIREPERIOD_VARIABLE:
            acquirePeriodQELabel->setVariableNameAndSubstitutions( getOriginalVariableName( ACQUIREPERIOD_VARIABLE ), getVariableNameSubstitutions(), 0 );
            return NULL;

        // Don't create anything - just pass on the variable name and substitutions on to the exposure time QELabel
        case EXPOSURETIME_VARIABLE:
            exposureTimeQELabel->setVariableNameAndSubstitutions( getOriginalVariableName( EXPOSURETIME_VARIABLE ), getVariableNameSubstitutions(), 0 );
            return NULL;

        // Pass on the variable name and substitutions on to the region of interext X QELabel, then create the roi X as a QCaInteger
        case ROI_X_VARIABLE:
            roiXQELabel->setVariableNameAndSubstitutions( getOriginalVariableName( ROI_X_VARIABLE ), getVariableNameSubstitutions(), 0 );
            return new QCaInteger( getSubstitutedVariableName( variableIndex ), this, &integerFormatting, variableIndex );

        // Pass on the variable name and substitutions on to the region of interext Y QELabel, then create the roi Y as a QCaInteger
        case ROI_Y_VARIABLE:
            roiYQELabel->setVariableNameAndSubstitutions( getOriginalVariableName( ROI_Y_VARIABLE ), getVariableNameSubstitutions(), 0 );
            return new QCaInteger( getSubstitutedVariableName( variableIndex ), this, &integerFormatting, variableIndex );

        // Pass on the variable name and substitutions on to the region of interext Width QELabel, then create the roi width as a QCaInteger
        case ROI_W_VARIABLE:
            roiWQELabel->setVariableNameAndSubstitutions( getOriginalVariableName( ROI_W_VARIABLE ), getVariableNameSubstitutions(), 0 );
            return new QCaInteger( getSubstitutedVariableName( variableIndex ), this, &integerFormatting, variableIndex );

        // Pass on the variable name and substitutions on to the region of interext Height QELabel, then create the roi height as a QCaInteger
        case ROI_H_VARIABLE:
            roiHQELabel->setVariableNameAndSubstitutions( getOriginalVariableName( ROI_H_VARIABLE ), getVariableNameSubstitutions(), 0 );
            return new QCaInteger( getSubstitutedVariableName( variableIndex ), this, &integerFormatting, variableIndex );

        default:
            return NULL;
    }
}

/*!
    Start updating.
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
    This function may also be used to initiate updates when loaded as a plugin.
*/
void QCaImage::establishConnection( unsigned int variableIndex ) {

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

        // QCa creation for these variables was handballed to the embedded QELabel widgets. Do nothing here
        case ACQUIREPERIOD_VARIABLE:
        case EXPOSURETIME_VARIABLE:
            break;

        // QCa creation occured, but no connection for display is required here.
        // (Display was handballed to the embedded QELabel widgets which will manage their own connections)
        case ROI_X_VARIABLE:
        case ROI_Y_VARIABLE:
        case ROI_W_VARIABLE:
        case ROI_H_VARIABLE:
            break;
     }
}

/*!
    Update the tool tip as requested by QCaToolTip.
*/
void QCaImage::updateToolTip( const QString& tip )
{
    setToolTip( tip );
}

/*!
    Act on a connection change.
    Change how the label looks and change the tool tip
    This is the slot used to recieve connection updates from a QCaObject based class.
 */
void QCaImage::connectionChanged( QCaConnectionInfo& connectionInfo )
{
    /// If connected, enable the widget if the QCa enabled property is true
    if( connectionInfo.isChannelConnected() )
    {
        isConnected = true;
        updateToolTipConnection( isConnected );

        if( caEnabled )
            setEnabled( true );
    }

    /// If disconnected always disable the widget.
    else
    {
        isConnected = false;
        updateToolTipConnection( isConnected );

        scrollArea->setEnabled( false );
    }
}

/*!
    Update the image dimensions
    This is the slot used to recieve data updates from a QCaObject based class.
 */
void QCaImage::setDimension( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex)
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
        //!!! not done
    }
}

/*!
    Update the image
    This is the slot used to recieve data updates from a QCaObject based class.
    Note the following comments from the Qt help:
        Note: Drawing into a QImage with QImage::Format_Indexed8 is not supported.
        Note: Do not render into ARGB32 images using QPainter. Using QImage::Format_ARGB32_Premultiplied is significantly faster.
 */
void QCaImage::setImage( const QByteArray& imageIn, unsigned long dataSize, QCaAlarmInfo& alarmInfo, QCaDateTime& time, const unsigned int& )
{
    // If the display is paused, do nothing
    if (pauseEnabled)
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
        //!!! not done
    }
}

void QCaImage::displayImage()
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
        scrollArea->horizontalScrollBar()->setValue( 0 );//initialHozScrollPos );
        initScrollPosSet = true;
    }

    // Set up input and output pointers and counters ready to process each pixel
    const unsigned char* dataIn = (unsigned char*)image.data();
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
    // Depenting on the scan option, however, the outer may be height or width.
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

    // Draw the input pixels into the image buffer .
    // Drawing is performed in two nested loops, one for height and one for width.
    // Depenting on the scan option, however, the outer may be height or width.
    // The input buffer is read consecutivly from first pixel to last and written to the
    // output buffer, which is moved to the next pixel by both the inner and outer
    // loops to where ever that next pixel is according to the rotation and flipping.
    dataIndex = start;
    //!!! this code needs to go FAST. Put the switch in the inner loop outside both and reproduce both loops within each case
    unsigned char aa=0;
    for( int i = 0; i < outCount; i++ )
    {
        for( int j = 0; j < inCount; j++ )
        {
            // Format the pixel ready for use in an RGB32 QImage
            switch( formatOption )
            {
                case GREY8:
                {
                    // Duplicate 8 bits of the grey scale into each color
                    unsigned long inPixel = dataIn[dataIndex*imageDataSize];
                    dataOut[buffIndex] = 0xff000000+(inPixel<<16)+(inPixel<<8)+inPixel;
                    break;
                }

                case GREY16:
                {
                    // Duplicate top 8 bits of the grey scale into each color
                    unsigned long inPixel = *(unsigned short*)(&dataIn[dataIndex*imageDataSize]);
                    inPixel = inPixel>>8;
                    dataOut[buffIndex] = 0xff000000+(inPixel<<16)+(inPixel<<8)+inPixel;
                    break;
                }

                case GREY12:
                {
                    // Duplicate top 8 bits of the grey scale into each color
                    unsigned long inPixel = *(unsigned short*)(&dataIn[dataIndex*imageDataSize]);
                    inPixel = (inPixel>>4)&0xff;
                    dataOut[buffIndex] = 0xff000000+(inPixel<<16)+(inPixel<<8)+inPixel;
                    break;
                }

                case RGB_888:
                {
                    unsigned char* inPixel  = (unsigned char*)(&dataIn[dataIndex*imageDataSize]);
                    dataOut[buffIndex] = 0xff000000+(inPixel[2]<<16)+(inPixel[1]<<8)+inPixel[0];
                    break;
                }
            }

            // Step on to the next pixel
            dataIndex += inInc;
            buffIndex++;
        }
        dataIndex += outInc;
    }

    // Generate a frame from the data
    QImage frameImage( (uchar*)(imageBuff.data()), rotatedImageBuffWidth(), rotatedImageBuffHeight(), QImage::Format_RGB32 );

    // Display the new image
    videoWidget->setNewImage( frameImage, imageTime );

    // Update markups if required
    updateMarkups();
}

// Update markups if required
void QCaImage::updateMarkups()
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
    // Nothing to regenerate for area yet
    //if( haveSelectedArea )
    //{
    //}
}

// Set the image buffer used for generate images will be large enough to hold the processed image
void QCaImage::setImageBuff()
{
    // Size the image
    switch( resizeOption )
    {
        // Zoom the image
        case RESIZE_OPTION_ZOOM:
            videoWidget->resize( rotatedImageBuffWidth() * zoom / 100, rotatedImageBuffHeight() * zoom / 100 );
            break;

        // Resize the image to fit exactly within the QCaItem
        case RESIZE_OPTION_FIT:
            videoWidget->resize( scrollArea->size() );
            break;

//        // Resize the QCaItem to exactly fit the image
//        case SIZE_OPTION_RESIZE:
//            // The top level QFrame of the QCaImage needs to be resized, so rather
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
// Manage the enabled state
//=================================================================================================

/*!
   Override the default widget isEnabled to allow alarm states to override current enabled state
 */
bool QCaImage::isEnabled() const
{
    /// Return what the state of widget would be if connected.
    return caEnabled;
}

/*!
   Override the default widget setEnabled to allow alarm states to override current enabled state
 */
void QCaImage::setEnabled( bool state )
{
    /// Note the new 'enabled' state
    caEnabled = state;

    /// Set the enabled state of the widget only if connected
    if( isConnected )
        scrollArea->setEnabled( caEnabled );
}

/*!
   Slot similar to default widget setEnabled, but will use our own setEnabled which will allow alarm states to override current enabled state
 */
void QCaImage::requestEnabled( const bool& state )
{
    setEnabled(state);
}

//=================================================================================================

// Add or remove the region of interest layout
void QCaImage::manageRoiLayout()
{
    if( displayRoiLayout )
    {
        roiGroup->show();
    }
    else
    {
        roiGroup->hide();
    }
}

// Add or remove the pause button
void QCaImage::managePauseButton()
{
    pauseButton->setVisible( displayPauseButton );
    manageButtonGroup();
}

// Add or remove the save button
void QCaImage::manageSaveButton()
{
    saveButton->setVisible( displaySaveButton );
    manageButtonGroup();
}

// Add or remove the ROI apply button
void QCaImage::manageRoiButton()
{
    roiButton->setVisible( displayRoiButton );
    manageButtonGroup();
}

// Add or remove the zoom button
void QCaImage::manageZoomButton()
{
    zoomButton->setVisible( displayZoomButton );
    manageButtonGroup();
}

void QCaImage::manageButtonGroup()
{
    buttonGroup->setVisible( displayPauseButton ||
                             displaySaveButton  ||
                             displayRoiButton   ||
                             displayZoomButton );
}

// Add or remove the acquire period label
void QCaImage::manageAcquirePeriodLabel()
{
    acquirePeriodLabel->setVisible( displayAcquirePeriod );
    acquirePeriodQELabel->setVisible( displayAcquirePeriod );
    manageLabelGroup();
}

// Add or remove the exposure time label
void QCaImage::manageExposureTimeLabel()
{
    exposureTimeLabel->setVisible( displayExposureTime );
    exposureTimeQELabel->setVisible( displayExposureTime );
    manageLabelGroup();
}

void QCaImage::manageLabelGroup()
{
    labelGroup->setVisible( displayExposureTime ||
                            displayAcquirePeriod );
}

// Zoom button pressed
void QCaImage::zoomClicked()
{
    // Disable the zoom button now the area has been applied
    zoomButton->setEnabled( false );

    // Determine the zoom factors
    int sizeX = selectedAreaPoint2.x()-selectedAreaPoint1.x();
    int sizeY = selectedAreaPoint2.y()-selectedAreaPoint1.y();
    double zoomFactorX = (double)(videoWidget->width()) / (double)sizeX;
    double zoomFactorY = (double)(videoWidget->height()) / (double)sizeY;

    // Resize the display widget
    int newSizeX = (double)(videoWidget->width()) * zoomFactorX;
    if( newSizeX > 5000 )
    {
        newSizeX = 5000;
        zoomFactorX = (double)newSizeX / (double)videoWidget->width();
    }

    int newSizeY = (double)(videoWidget->height()) * zoomFactorY;
    if( newSizeY > 5000 )
    {
        newSizeY = 5000;
        zoomFactorY = (double)newSizeY / (double)videoWidget->height();
    }

    videoWidget->resize( newSizeX, newSizeY );

    // Reposition the display widget
    scrollArea->horizontalScrollBar()->setValue( (double)(selectedAreaPoint1.x()) * zoomFactorX );
    scrollArea->verticalScrollBar()->setValue( (double)(selectedAreaPoint1.y()) * zoomFactorY );
}

// ROI apply button pressed
void QCaImage::roiClicked()
{
    // Disable the ROI button now it has been applied
    // !!! should be disabled when the area stops being selected. It's OK for the zoom to be disabled when pressed, but there is no reason why the current ROI can't be reapplied
    roiButton->setEnabled( false );

    // Write the ROI variables.
    QCaInteger *qca;
    qca = (QCaInteger*)getQcaItem( ROI_X_VARIABLE );
    if( qca ) qca->writeInteger( selectedAreaScaledPoint1.x() );

    qca = (QCaInteger*)getQcaItem( ROI_Y_VARIABLE );
    if( qca ) qca->writeInteger(  selectedAreaScaledPoint1.y() );

    qca = (QCaInteger*)getQcaItem( ROI_W_VARIABLE );
    if( qca ) qca->writeInteger( selectedAreaScaledPoint2.x()-selectedAreaScaledPoint1.x() );

    qca = (QCaInteger*)getQcaItem( ROI_H_VARIABLE );
    if( qca ) qca->writeInteger( selectedAreaScaledPoint2.y()-selectedAreaScaledPoint1.y() );

    return;
}

// Pause button pressed
void QCaImage::pauseClicked()
{
    if (pauseEnabled)
    {
        pauseButton->setText("Pause");
        pauseButton->setToolTip("Pause image display");
        pauseEnabled = false;
    }
    else
    {
        pauseButton->setText("Resume");
        pauseButton->setToolTip("Resume image display");
        pauseEnabled = true;
    }
}

// Save button pressed
void QCaImage::saveClicked()
{
    QFileDialog *qFileDialog;
    QStringList filterList;
    QString filename;
    bool result;

    qFileDialog = new QFileDialog(this, "Save displayed image", QString());
    filterList << "Portable Network Graphics (*.png)" << "Windows Bitmap (*.bmp)" << "Joint Photographics Experts Group (*.jpg)";
    qFileDialog->setFilters(filterList);
    qFileDialog->setAcceptMode(QFileDialog::AcceptSave);

    if (qFileDialog->exec())
    {

        QImage qImage((uchar*) imageBuff.data(), rotatedImageBuffWidth(), rotatedImageBuffHeight(), QImage::Format_RGB32);
        filename = qFileDialog->selectedFiles().at(0);

        if (qFileDialog->selectedNameFilter() == filterList.at(0))
        {
            result = qImage.save(filename, "PNG");
        }
        else
        {
            if (qFileDialog->selectedNameFilter() == filterList.at(1))
            {
                result = qImage.save(filename, "BMP");
            }
            else
            {
                result = qImage.save(filename, "JPG");
            }
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

// Update the video widget if the QCaImage has changed
void QCaImage::resizeEvent(QResizeEvent* )
{
    setImageBuff();
}

//==============================================================================
// Drag drop
void QCaImage::setDropText( QString text )
{
    setVariableName( text, 0 );
    establishConnection( 0 );
}

QString QCaImage::getDropText()
{
    return getSubstitutedVariableName(0);
}

//==============================================================================
// Property convenience functions


// Access functions for variableName and variableNameSubstitutions
// variable substitutions Example: SECTOR=01 will result in any occurance of $SECTOR in variable name being replaced with 01.
void QCaImage::setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex ) {
    setVariableNameSubstitutions( variableNameSubstitutionsIn );
    setVariableName( variableNameIn, variableIndex );
    establishConnection( variableIndex );
}

// variable as tool tip
void QCaImage::setVariableAsToolTip( bool variableAsToolTipIn )
{
    variableAsToolTip = variableAsToolTipIn;
}
bool QCaImage::getVariableAsToolTip()
{
    return variableAsToolTip;
}

// visible (widget is visible outside 'Designer')
void QCaImage::setRunVisible( bool visibleIn )
{
    // Update the property
    caVisible = visibleIn;

    // If a container profile has been defined, then this widget is being used in a real GUI and
    // should be visible or not according to the visible property. (While in Designer it can always be displayed)
    ContainerProfile profile;
    if( profile.isProfileDefined() )
    {
        QWidget::setVisible( caVisible );
    }

}
bool QCaImage::getRunVisible()
{
    return caVisible;
}

// allow drop (Enable/disable as a drop site for drag and drop)
void QCaImage::setAllowDrop( bool allowDropIn )
{
    allowDrop = allowDropIn;
    setAcceptDrops( allowDrop );
}

bool QCaImage::getAllowDrop()
{
    return allowDrop;
}

// Allow user to set the video format
void QCaImage::setFormatOption( formatOptions formatOptionIn )
{
    // Save the option
    formatOption = formatOptionIn;

    // Resize and rescale
    setImageBuff();
}

QCaImage::formatOptions QCaImage::getFormatOption()
{
    return formatOption;
}

// Zoom level
void QCaImage::setZoom( int zoomIn )
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

int QCaImage::getZoom()
{
    return zoom;
}

// Rotation
void QCaImage::setRotation( rotationOptions rotationIn )
{
    rotation = rotationIn;

    // Present the updated image
    displayImage();
}

QCaImage::rotationOptions QCaImage::getRotation()
{
    return rotation;
}

// Horizontal flip
void QCaImage::setHorizontalFlip( bool flipHozIn )
{
    flipHoz = flipHozIn;

    // Present the updated image
    displayImage();
}

bool QCaImage::getHorizontalFlip()
{
    return flipHoz;
}

// Vertical flip
void QCaImage::setVerticalFlip( bool flipVertIn )
{
    flipVert = flipVertIn;

    // Present the updated image
    displayImage();
}

bool QCaImage::getVerticalFlip()
{
    return flipVert;
}



// Resize options
void QCaImage::setResizeOption( resizeOptions resizeOptionIn )
{
    // Save the resize option
    resizeOption = resizeOptionIn;

    // Resize and rescale
    setImageBuff();

    // Present the updated image
    displayImage();
}

QCaImage::resizeOptions QCaImage::getResizeOption()
{
    return resizeOption;
}

// Initial vorizontal scroll position
void QCaImage::setInitialHozScrollPos( int initialHozScrollPosIn )
{
    initialHozScrollPos = initialHozScrollPosIn;
    scrollArea->horizontalScrollBar()->setValue( initialHozScrollPos );
}

int QCaImage::getInitialHozScrollPos()
{
    return initialHozScrollPos;
}

// Initial vertical scroll position
void QCaImage::setInitialVertScrollPos( int initialVertScrollPosIn )
{
    initialVertScrollPos = initialVertScrollPosIn;
    scrollArea->verticalScrollBar()->setValue( initialVertScrollPos );
}

int QCaImage::getInitialVertScrollPos()
{
    return initialVertScrollPos;
}

// Display the region of interest values
void QCaImage::setDisplayRegionOfInterest( bool displayRoiLayoutIn )
{
    displayRoiLayout = displayRoiLayoutIn;
    manageRoiLayout();
}

bool QCaImage::getDisplayRegionOfInterest()
{
    return displayRoiLayout;
}

// Display the acquire period
void QCaImage::setDisplayAcquirePeriod( bool displayAcquirePeriodIn )
{
    displayAcquirePeriod = displayAcquirePeriodIn;
    manageAcquirePeriodLabel();
}

bool QCaImage::getDisplayAcquirePeriod()
{
    return displayAcquirePeriod;
}

// Display the exposure time
void QCaImage::setDisplayExposureTime( bool displayExposureTimeIn )
{
    displayExposureTime = displayExposureTimeIn;
    manageExposureTimeLabel();
}

bool QCaImage::getDisplayExposureTime()
{
    return displayExposureTime;
}

// Show pause button
void QCaImage::setShowPauseButton( bool displayPauseButtonIn )
{
    displayPauseButton = displayPauseButtonIn;
    managePauseButton();
}

bool QCaImage::getShowPauseButton()
{
    return displayPauseButton;
}

// Show save button
void QCaImage::setShowSaveButton( bool displaySaveButtonIn )
{
    displaySaveButton = displaySaveButtonIn;
    manageSaveButton();
}

bool QCaImage::getShowSaveButton()
{
    return displaySaveButton;
}

// Show ROI apply button
void QCaImage::setShowRoiButton( bool displayRoiButtonIn )
{
    displayRoiButton = displayRoiButtonIn;
    manageRoiButton();
}

bool QCaImage::getShowRoiButton()
{
    return displayRoiButton;
}

// Show zoom button
void QCaImage::setShowZoomButton( bool displayZoomButtonIn )
{
    displayZoomButton = displayZoomButtonIn;
    manageZoomButton();
}

bool QCaImage::getShowZoomButton()
{
    return displayZoomButton;
}

// Show time
void QCaImage::setShowTime(bool pValue)
{
    videoWidget->setShowTime( pValue );
}

bool QCaImage::getShowTime()
{
    return videoWidget->getShowTime();
}

// Markup colour
void QCaImage::setMarkupColor(QColor markupColor )
{
    videoWidget->setMarkupColor( markupColor );
}

QColor QCaImage::getMarkupColor()
{
    return videoWidget->getMarkupColor();
}

// Show cursor pixel
void QCaImage::setDisplayCursorPixelInfo( bool displayCursorPixelInfoIn )
{
    displayCursorPixelInfo = displayCursorPixelInfoIn;
}

bool QCaImage::getDisplayCursorPixelInfo(){
    return displayCursorPixelInfo;
}

// Enable vertical slice selection
void QCaImage::setEnableVertSliceSelection( bool enableVSliceSelectionIn )
{
    enableVSliceSelection = enableVSliceSelectionIn;
    manageSelectionOptions();
}

bool QCaImage::getEnableVertSliceSelection()
{
    return enableVSliceSelection;
}

// Enable horizontal slice selection
void QCaImage::setEnableHozSliceSelection( bool enableHSliceSelectionIn )
{
    enableHSliceSelection = enableHSliceSelectionIn;
    manageSelectionOptions();
}

bool QCaImage::getEnableHozSliceSelection()
{
    return enableHSliceSelection;
}

// Enable area selection (used for ROI and zoom)
void QCaImage::setEnableAreaSelection( bool enableAreaSelectionIn )
{
    enableAreaSelection = enableAreaSelectionIn;
    manageSelectionOptions();
}

bool QCaImage::getEnableAreaSelection()
{
    return enableAreaSelection;
}

// Enable profile selection
void QCaImage::setEnableProfileSelection( bool enableProfileSelectionIn )
{
    enableProfileSelection = enableProfileSelectionIn;
    manageSelectionOptions();
}

bool QCaImage::getEnableProfileSelection()
{
    return enableProfileSelection;
}

//=================================================================================================

void QCaImage::manageSelectionOptions()
{
    // If more than one buton is required, then make the appropriate ones visible
    // (If only one selection option is required, no point having a radio button for it)
    int count = 0;
    if( enableAreaSelection ) count++;
    if( enableVSliceSelection ) count++;
    if( enableHSliceSelection ) count++;
    if( enableProfileSelection ) count++;

    if( count >= 2 )
    {
        areaSelectionGroup->setVisible( true );

        // For each button, make it visible if it is enabled.
        areaSelectMode->setVisible( enableAreaSelection );

        vSliceSelectMode->setVisible( enableVSliceSelection );

        hSliceSelectMode->setVisible( enableHSliceSelection );

        profileSelectMode->setVisible( enableProfileSelection );

        // If no buttons are checked, check the first visible button
        if( !areaSelectMode->isChecked() &&
            !areaSelectMode->isChecked() &&
            !areaSelectMode->isChecked() &&
            !areaSelectMode->isChecked() )
        {
            if( enableAreaSelection ) {
                areaSelectMode->setChecked( true );
                areaSelectModeClicked();
            }
            else if( enableVSliceSelection ){
                vSliceSelectMode->setChecked( true );
                vSliceSelectModeClicked();
            }
            else if( enableHSliceSelection ){
                hSliceSelectMode->setChecked( true );
                hSliceSelectModeClicked();
            }
            else if( enableProfileSelection ){
                profileSelectMode->setChecked( true );
                profileSelectModeClicked();
            }
        }


    }
    // If no buttons are required, hide the entire group
    else
    {
        areaSelectionGroup->setVisible( false );

    }

    // Display, or don't display, the profile plots as required
    profileDisplay->setVisible( enableProfileSelection );
    hSliceDisplay->setVisible(enableHSliceSelection );
    vSliceDisplay->setVisible( enableVSliceSelection );

}

//=================================================================================================


void QCaImage::vSliceSelectModeClicked()
{
    videoWidget->setMode(  imageMarkup::MARKUP_MODE_V_LINE );
}

void QCaImage::hSliceSelectModeClicked()
{
    videoWidget->setMode(  imageMarkup::MARKUP_MODE_H_LINE );
}

void QCaImage::areaSelectModeClicked()
{
    videoWidget->setMode(  imageMarkup::MARKUP_MODE_AREA );
}

void QCaImage::profileSelectModeClicked()
{
    videoWidget->setMode(  imageMarkup::MARKUP_MODE_LINE );
}

//=================================================================================================

void QCaImage::zoomInOut( int zoomAmount )
{
    setResizeOption( RESIZE_OPTION_ZOOM );
    double oldZoom = zoom;
    double newZoom = zoom + zoomAmount;
    setZoom( newZoom);

    double currentScrollPosX = scrollArea->horizontalScrollBar()->value();
    double currentScrollPosY = scrollArea->verticalScrollBar()->value();
    double newScrollPosX = currentScrollPosX *newZoom / oldZoom;
    double newScrollPosY = currentScrollPosY *newZoom / oldZoom;

    scrollArea->horizontalScrollBar()->setValue( newScrollPosX );
    scrollArea->verticalScrollBar()->setValue( newScrollPosY );

}

// The user has made (or is making) a selection in the displayed image.
// Act on the selelection
void QCaImage::userSelection( imageMarkup::markupModes mode, QPoint point1, QPoint point2, QPoint scaledPoint1, QPoint scaledPoint2 )
{
    QString s;
    switch( mode )
    {
        case imageMarkup::MARKUP_MODE_V_LINE:
            vSliceX = scaledPoint1.x();
            haveVSliceX = true;
            generateVSlice( vSliceX );
            s.sprintf( "V: %d", scaledPoint1.x() );
            currentVertPixelLabel->setText( s );
            break;

        case imageMarkup::MARKUP_MODE_H_LINE:
            hSliceY = scaledPoint1.y();
            haveHSliceY = true;
            generateHSlice( hSliceY );
            s.sprintf( "H: %d", scaledPoint1.y() );
            currentHozPixelLabel->setText( s );
            break;

        case imageMarkup::MARKUP_MODE_AREA:
            selectedAreaPoint1 = point1;
            selectedAreaPoint2 = point2;
            selectedAreaScaledPoint1 = scaledPoint1;
            selectedAreaScaledPoint2 = scaledPoint2;
            haveSelectedArea = true;

            roiButton->setEnabled( true );
            zoomButton->setEnabled( true );

            s.sprintf( "A: (%d,%d)(%d,%d)", scaledPoint1.x(), scaledPoint1.y(), scaledPoint2.x(), scaledPoint2.y() );
            currentAreaLabel->setText( s );
            break;

        case imageMarkup::MARKUP_MODE_LINE:
            profileLineStart = scaledPoint1;
            profileLineEnd = scaledPoint2;
            haveProfileLine = true;
            generateProfile( profileLineStart, profileLineEnd );
            s.sprintf( "L: (%d,%d)(%d,%d)", scaledPoint1.x(), scaledPoint1.y(), scaledPoint2.x(), scaledPoint2.y() );
            currentLineLabel->setText( s );
            break;

        case imageMarkup::MARKUP_MODE_NONE:
            break;

    }

}

// Generate a profile along a line down an image at a given X position
// The profile contains values for each pixel intersected by the line.
void QCaImage::generateVSlice( int x )
{
    // If not over the image, remove the profile
    if( x < 0 || x >= (int)rotatedImageBuffWidth() )
    {
        QVector<QPointF> empty;
        vSliceDisplay->setProfile( empty, 1, 1 );
        return;
    }

    // Ensure the buffer is the correct size
    if( vSliceData.size() != (int)rotatedImageBuffHeight() )
        vSliceData.resize( rotatedImageBuffHeight() );

    // Set up to step pixel by pixel through the image data along the line
    const unsigned char* data = (unsigned char*)image.data();
    const unsigned char* dataPtr = &(data[x*imageDataSize]);
    int dataPtrStep = rotatedImageBuffWidth()*imageDataSize;

    // Determine the image data value at each pixel
    // The buffer is filled backwards so the plot, which sits on its side beside the image is drawn correctly
    QPoint pos;
    pos.setX( x );
    for( int i = rotatedImageBuffHeight()-1; i >= 0; i-- )
    {
        // Determine the next position
        pos.setY( i );

        vSliceData[i].setY( i );
        vSliceData[i].setX( getFloatingPixelValueFromData( getImageDataPtr( pos ), imageDataSize ) );
        dataPtr += dataPtrStep;
    }

    // Display the profile
    vSliceDisplay->setProfile( vSliceData, 1<<(imageDataSize*8), vSliceData.size() );
}

// Return a pointer to pixel data in the original image data.
// The position parameter is scaled to the original image size but reflects
// the displayed rotation and flip options, so it must be transformed first.
const unsigned char* QCaImage::getImageDataPtr( QPoint& pos )
{
    QPoint posTr;

    // Transform the position to reflect the original unrotated or flipped data
    posTr = rotateFLipPoint( pos );

    const unsigned char* data = (unsigned char*)image.data();
    return &(data[(posTr.x()+posTr.y()*imageBuffWidth)*imageDataSize]);
}

// Generate a profile along a line across an image at a given Y position
// The profile contains values for each pixel intersected by the line.
void QCaImage::generateHSlice( int y )
{
    // If not over the image, remove the profile
    if( y < 0 || y >= (int)rotatedImageBuffHeight() )
    {
        QVector<QPointF> empty;
        hSliceDisplay->setProfile( empty, 1, 1 );
        return;
    }

    // Ensure the buffer is the correct size
    if( hSliceData.size() != (int)rotatedImageBuffWidth() )
        hSliceData.resize( rotatedImageBuffWidth() );

    // Set up to step pixel by pixel through the image data along the line
    const unsigned char* data = (unsigned char*)image.data();
    const unsigned char* dataPtr = &(data[y*rotatedImageBuffWidth()*imageDataSize]);
    int dataPtrStep = imageDataSize;

    // Determine the image data value at each pixel
    for( unsigned int i = 0; i < rotatedImageBuffWidth(); i++ )
    {
        hSliceData[i].setX( i );
        hSliceData[i].setY( getFloatingPixelValueFromData( dataPtr, imageDataSize ) );
        dataPtr += dataPtrStep;
    }

    // Display the profile
    hSliceDisplay->setProfile( hSliceData, hSliceData.size(), 1<<(imageDataSize*8) );
}

// Generate a profile along an arbitrary line through an image
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
void QCaImage::generateProfile( QPoint point1, QPoint point2 )
{
    // X and Y components of line drawn
    double dX = point2.x()-point1.x();
    double dY = point2.y()-point1.y();

    // Do nothing if no line
    if( dX == 0 && dY == 0 )
    {
        QVector<QPointF> empty;
        profileDisplay->setProfile( empty, 1, 1 );
        return;
    }

    // Line length
    double len = sqrt( dX*dX+dY*dY );

    // Line slope
    // To handle infinite slope, switch slope between x/y or y/x as appropriate
    // This is more accurate for otherwise very steep slopes, and does not need a
    // special case for infinite slopes
    double slopeY; // Y/X
    double slopeX; // X/Y
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
    if( profileData.size() != len )
        profileData.resize( len );

    // Get reference to image data
    const unsigned char* data = (unsigned char*)image.data();

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
            x = initY + (y-initY) * slopeX;
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
            const unsigned char* dataPtrTL = &(data[((int)xTLi+(int)yTLi*rotatedImageBuffWidth())*imageDataSize]);
            const unsigned char* dataPtrTR = &(dataPtrTL[imageDataSize]);
            const unsigned char* dataPtrBL = &(dataPtrTL[rotatedImageBuffWidth()*imageDataSize]);
            const unsigned char* dataPtrBR = &(dataPtrBL[imageDataSize]);

            // Determine the value of the notional pixel from a weighted average of the four real pixels it overlays.
            // The larger the proportion of the real picture overlayed, the greated the weight.
            // (Ignore pixels outside the image)
            int pixelsInValue = 0;
            if( xTLi >= 0 && yTLi >= 0 )
            {
                value = propTL * getFloatingPixelValueFromData( dataPtrTL, imageDataSize );
                pixelsInValue++;
            }

            if( xTLi+1 < rotatedImageBuffWidth() && yTLi >= 0 )
            {
                value += propTR * getFloatingPixelValueFromData( dataPtrTR, imageDataSize );
                pixelsInValue++;
            }

            if( xTLi >= 0 && yTLi+1 < rotatedImageBuffHeight() )
            {

                value += propBL * getFloatingPixelValueFromData( dataPtrBL, imageDataSize );
                pixelsInValue++;
            }
            if( xTLi+1 < rotatedImageBuffWidth() && yTLi+1 < rotatedImageBuffHeight() )
            {
                value += propBR * getFloatingPixelValueFromData( dataPtrBR, imageDataSize );
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
    profileDisplay->setProfile( profileData, profileData.size(), 1<<(imageDataSize*8) );
}
//=================================================================================================


// Return a floating point number given a pointer to a value of an arbitrary size in a char* buffer.
int QCaImage::getPixelValueFromData( const unsigned char* ptr, unsigned long dataSize )
{
    // Case the data to the correct size, then return the data as a floating point number.
    switch( dataSize )
    {
        default:
        case 1: return *ptr;
        case 2: return *(unsigned short*)ptr;
        case 4: return *(unsigned int*)ptr;
    }
}

// Return a floating point number given a pointer to a value of an arbitrary size in a char* buffer.
double QCaImage::getFloatingPixelValueFromData( const unsigned char* ptr, unsigned long dataSize )
{
    return getPixelValueFromData( ptr, dataSize );
}

QPoint QCaImage::rotateFLipPoint( QPoint& pos )
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
    int w = (int)imageBuffWidth;
    int h = (int)imageBuffHeight;
    QPoint posTr;
    switch( getScanOption() )
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
qDebug() << pos <<  posTr;
    return posTr;
}

//=================================================================================================
// Display a pixel value.
void QCaImage::currentPixelInfo( QPoint pos )
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
//        const unsigned char* data = (unsigned char*)image.data();
//        const unsigned char* dataPtr = &(data[(posTr.x()+ posTr.y()*w)*imageDataSize]);
        int value = getPixelValueFromData( getImageDataPtr( pos ), imageDataSize );
        s.sprintf( "(%d,%d)=%d", pos.x(), pos.y(), value );
    }
    currentCursorPixelLabel->setText( s );
}

// Return the image width following any rotation
unsigned int QCaImage::rotatedImageBuffWidth()
{
    switch( rotation)
    {
        case ROTATION_0:
        case ROTATION_180:
            return imageBuffWidth;

        case ROTATION_90_RIGHT:
        case ROTATION_90_LEFT:
            return imageBuffHeight;
    }
}

// Return the image height following any rotation
unsigned int QCaImage::rotatedImageBuffHeight()
{
    switch( rotation)
    {
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
int QCaImage::getScanOption()
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
