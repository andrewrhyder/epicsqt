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
  It is tighly integrated with the base class QCaWidget. Refer to QCaWidget.cpp for details
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
    sizeOption = SIZE_OPTION_ZOOM;
    zoom = 100;
    rotation = 0.0;
    initialHozScrollPos = 0;
    initialVertScrollPos = 0;
    initScrollPosSet = false;
    formatOption = GREY8;
    setShowTimeColor(QColor(0, 255, 0));
    pauseEnabled = false;

    displayPauseButton = false;
    displaySaveButton = false;
    displayZoomButton = false;
    displayRoiButton = false;


//!!!all property variables initialised?

    // Set the initial state
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;

    // Use frame signals
    // --Currently none--

    // Create the video destination
    videoWidget = new VideoWidget;

    // Add the video destination to the widget
    scrollArea = new QScrollArea;
    scrollArea->setFrameStyle( QFrame::NoFrame );
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget( videoWidget );
    scrollArea->setEnabled( false );  // Reflects initial disconnected state



    // Create label group
    labelGroup = new QGroupBox();
    labelGroup->setTitle( "Details");
    QGridLayout* labelLayout = new QGridLayout();
    labelLayout->setMargin( 0 );
    labelGroup->setLayout( labelLayout);

    acquirePeriodQCaLabel = new QCaLabel( this );
    acquirePeriodLabel = new QLabel( this );
    acquirePeriodLabel->setText( "Acquire Period:" );

    exposureTimeQCaLabel = new QCaLabel( this );
    exposureTimeLabel = new QLabel( this );
    exposureTimeLabel->setText( "Exposure Time:" );

    labelLayout->addWidget( acquirePeriodLabel, 0, 0 );
    labelLayout->addWidget( acquirePeriodQCaLabel, 0, 1 );
    labelLayout->addWidget( exposureTimeLabel, 1, 0 );
    labelLayout->addWidget( exposureTimeQCaLabel, 1, 1 );



    // Create region of interest group
    roiGroup = new QGroupBox();
    roiGroup->setTitle( "R.O.I.");
    QGridLayout* roiLayout = new QGridLayout();
    roiLayout->setMargin( 0 );
    roiGroup->setLayout( roiLayout);


    roiXQCaLabel = new QCaLabel( roiGroup );
    roiXLabel = new QLabel( roiGroup );
    roiXLabel->setText( "X:" );

    roiYQCaLabel = new QCaLabel( roiGroup );
    roiYLabel = new QLabel( roiGroup );
    roiYLabel->setText( "Y:" );

    roiWQCaLabel = new QCaLabel( roiGroup );
    roiWLabel = new QLabel( roiGroup );
    roiWLabel->setText( "Width:" );

    roiHQCaLabel = new QCaLabel( roiGroup );
    roiHLabel = new QLabel( roiGroup );
    roiHLabel->setText( "Height:" );


    roiLayout->addWidget( roiXLabel, 0, 0 );
    roiLayout->addWidget( roiXQCaLabel, 0, 1 );
    roiLayout->addWidget( roiYLabel, 1, 0 );
    roiLayout->addWidget( roiYQCaLabel, 1, 1 );
    roiLayout->addWidget( roiWLabel, 2, 0 );
    roiLayout->addWidget( roiWQCaLabel, 2, 1 );
    roiLayout->addWidget( roiHLabel, 3, 0 );
    roiLayout->addWidget( roiHQCaLabel, 3, 1 );


    // Create button group
    buttonGroup = new QGroupBox();
    buttonGroup->setTitle( "Actions");
    QGridLayout* buttonLayout = new QGridLayout();
    buttonLayout->setMargin( 0 );
    buttonGroup->setLayout( buttonLayout);


    pauseButton= new QPushButton(this);
    pauseButton->setText("Pause");
    pauseButton->setToolTip("Pause image display");
    QObject::connect(pauseButton, SIGNAL(clicked()), this, SLOT(pauseClicked()));

    saveButton = new QPushButton(this);
    saveButton->setText("Save");
    saveButton->setToolTip("Save displayed image");
    QObject::connect(saveButton, SIGNAL(clicked()), this, SLOT(saveClicked()));

    roiButton = new QPushButton(this);
    roiButton->setText("ROI");
    roiButton->setToolTip("Apply selected area to Region Of Interst");
    QObject::connect(roiButton, SIGNAL(clicked()), this, SLOT(roiClicked()));

    zoomButton = new QPushButton(this);
    zoomButton->setText("Zoom");
    zoomButton->setToolTip("Zoom to selected area");
    QObject::connect(zoomButton, SIGNAL(clicked()), this, SLOT(zoomClicked()));


    buttonLayout->addWidget(pauseButton, 0, 0);
    buttonLayout->addWidget(saveButton, 0, 1);
    buttonLayout->addWidget(roiButton, 0, 2);
    buttonLayout->addWidget(zoomButton, 0, 3);



    // Create main layout containing image, label, and button layouts
    mainLayout = new QVBoxLayout;
    mainLayout->setMargin( 0 );

    mainLayout->addWidget( scrollArea );
    mainLayout->addWidget( labelGroup );
    mainLayout->addWidget( roiGroup );
    mainLayout->addWidget(buttonGroup);

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

    // Set the initial selection mode of the setup widget
//    videoWidget->setMode(  imageMarkup::MARKUP_MODE_NONE );
//    videoWidget->setMode(  imageMarkup::MARKUP_MODE_H_LINE ); //!!! testing only MARKUP_MODE_NONE );
//    videoWidget->setMode(  imageMarkup::MARKUP_MODE_V_LINE ); //!!! testing only MARKUP_MODE_NONE );
//    videoWidget->setMode(  imageMarkup::MARKUP_MODE_LINE ); //!!! testing only MARKUP_MODE_NONE );
    videoWidget->setMode(  imageMarkup::MARKUP_MODE_AREA ); //!!! testing only MARKUP_MODE_NONE );

    // Set image size to zero
    // Image will not be presented until size is available
    imageBuffWidth = 0;
    imageBuffHeight = 0;
}

QCaImage::~QCaImage()
{
// need to delete anything added to widget hierarchy?
//    delete videoWidget;
}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
    For a label a QCaObject that streams strings is required.
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

        // Don't create anything - just pass on the variable name and substitutions on to the acquire period QCaLabel
        case ACQUIREPERIOD_VARIABLE:
            acquirePeriodQCaLabel->setVariableNameAndSubstitutions( getOriginalVariableName( ACQUIREPERIOD_VARIABLE ), getVariableNameSubstitutions(), 0 );
            return NULL;

        // Don't create anything - just pass on the variable name and substitutions on to the exposure time QCaLabel
        case EXPOSURETIME_VARIABLE:
            exposureTimeQCaLabel->setVariableNameAndSubstitutions( getOriginalVariableName( EXPOSURETIME_VARIABLE ), getVariableNameSubstitutions(), 0 );
            return NULL;

        // Pass on the variable name and substitutions on to the region of interext X QCaLabel, then create the roi X as a QCaInteger
        case ROI_X_VARIABLE:
            roiXQCaLabel->setVariableNameAndSubstitutions( getOriginalVariableName( ROI_X_VARIABLE ), getVariableNameSubstitutions(), 0 );
            return new QCaInteger( getSubstitutedVariableName( variableIndex ), this, &integerFormatting, variableIndex );

        // Pass on the variable name and substitutions on to the region of interext Y QCaLabel, then create the roi Y as a QCaInteger
        case ROI_Y_VARIABLE:
            roiYQCaLabel->setVariableNameAndSubstitutions( getOriginalVariableName( ROI_Y_VARIABLE ), getVariableNameSubstitutions(), 0 );
            return new QCaInteger( getSubstitutedVariableName( variableIndex ), this, &integerFormatting, variableIndex );

        // Pass on the variable name and substitutions on to the region of interext Width QCaLabel, then create the roi width as a QCaInteger
        case ROI_W_VARIABLE:
            roiWQCaLabel->setVariableNameAndSubstitutions( getOriginalVariableName( ROI_W_VARIABLE ), getVariableNameSubstitutions(), 0 );
            return new QCaInteger( getSubstitutedVariableName( variableIndex ), this, &integerFormatting, variableIndex );

        // Pass on the variable name and substitutions on to the region of interext Height QCaLabel, then create the roi height as a QCaInteger
        case ROI_H_VARIABLE:
            roiHQCaLabel->setVariableNameAndSubstitutions( getOriginalVariableName( ROI_H_VARIABLE ), getVariableNameSubstitutions(), 0 );
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

        // Create the width item as a QCaInteger
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

        case ACQUIREPERIOD_VARIABLE:
        case EXPOSURETIME_VARIABLE:
            // QCa creation for these variables was handballed to the embedded QCaLabel widgets. Do nothing here
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
void QCaImage::setImage( const QByteArray& imageIn, unsigned long dataSize, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& )
{
    // If the display is paused, do nothing
    if (pauseEnabled)
    {
        return;
    }

    // Signal a database value change to any Link widgets
    emit dbValueChanged( "image" );

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
    const unsigned char* dataIn = (unsigned char*)imageIn.data();
    unsigned int* dataOut = (unsigned int*)(imageBuff.data());
    unsigned long buffIndex = 0;
    unsigned long dataIndex = 0;

    // Determine the number of pixels to process
    // If something is wrong, reduce the number of pixels to ensure neither
    // input or output image buffers overrun
    unsigned long pixelCount = imageBuffHeight*imageBuffWidth;
    if( pixelCount * dataSize > (unsigned long)imageIn.size() )
    {
        pixelCount = imageIn.size() / dataSize;
    }

    if( pixelCount * IMAGEBUFF_BYTES_PER_PIXEL > (unsigned long)imageBuff.size() )
    {
        pixelCount = imageBuff.size() / IMAGEBUFF_BYTES_PER_PIXEL;
    }

    // Process all pixels.
    // Note, a two dimensional loop will be required if using a non 32 bit
    // aligned output buffer which requires padding every line.
    for( unsigned long i = 0; i < pixelCount; i++ )
    {
        // Format the pixel ready for use in an RGB32 QImage
        switch( formatOption )
        {
            case GREY8:
            {
                // Duplicate 8 bits of the grey scale into each color
                unsigned long inPixel = dataIn[dataIndex];
                dataOut[buffIndex] = 0xff000000+(inPixel<<16)+(inPixel<<8)+inPixel;
                break;
            }

            case GREY16:
            {
                // Duplicate top 8 bits of the grey scale into each color
                unsigned long inPixel = *(unsigned short*)(&dataIn[dataIndex]);
//                qDebug() << ((inPixel>>8)&0xff) << (inPixel&0xff);
                inPixel = inPixel>>8;
                dataOut[buffIndex] = 0xff000000+(inPixel<<16)+(inPixel<<8)+inPixel;
                break;
            }

            case GREY12:
            {
                // Duplicate top 8 bits of the grey scale into each color
                unsigned long inPixel = *(unsigned short*)(&dataIn[dataIndex]);
                inPixel = (inPixel>>4)&0xff;
                dataOut[buffIndex] = 0xff000000+(inPixel<<16)+(inPixel<<8)+inPixel;
                break;
            }

            case RGB_888:
            {
                unsigned char* inPixel  = (unsigned char*)(&dataIn[dataIndex]);
                dataOut[buffIndex] = 0xff000000+(inPixel[2]<<16)+(inPixel[1]<<8)+inPixel[0];
                break;
            }
        }

        // Step on to the next pixel
        buffIndex++;
        dataIndex+=dataSize;
    }

    // Generate a frame from the data
    QImage frameImage( (uchar*)(imageBuff.data()), imageBuffWidth, imageBuffHeight, QImage::Format_RGB32 );

    // Display the new image
    videoWidget->setNewImage( frameImage );

    // Display invalid if invalid
    if( alarmInfo.isInvalid() )
    {
        //setImageInvalid()
        //!!! not done
    }
}

// Set the image buffer used for generate images will be large enough to hold the processed image
void QCaImage::setImageBuff()
{
    // Size the image
    switch( sizeOption )
    {
        // Zoom the image
        case SIZE_OPTION_ZOOM:
            videoWidget->resize( imageBuffWidth * zoom / 100, imageBuffHeight * zoom / 100 );
            break;

        // Resize the image to fit exactly within the QCaItem
        case SIZE_OPTION_FIT:
            videoWidget->resize( scrollArea->size() );
            break;

        // Resize the QCaItem to exactly fit the image
        case SIZE_OPTION_RESIZE:
            // The top level QFrame of the QCaImage needs to be resized, so rather
            // than determine how large it needs to be to fit the image and whatever
            // extra is taken up with borders and info widgets, just calculate how
            // much bigger or smaller the current scroll area widget neesds to be
            // and increase the Qframe by that much
            resize( size().width()+imageBuffWidth-scrollArea->size().width(),
                    size().height()+imageBuffHeight-scrollArea->size().height() );
            videoWidget->resize( imageBuffWidth, imageBuffHeight );
            break;
    }


    // Determine buffer size
    unsigned long buffSize = IMAGEBUFF_BYTES_PER_PIXEL * imageBuffWidth * imageBuffHeight;

    // Resize buffer
    imageBuff.resize( buffSize );

}

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
    acquirePeriodQCaLabel->setVisible( displayAcquirePeriod );
    manageLabelGroup();
}

// Add or remove the exposure time label
void QCaImage::manageExposureTimeLabel()
{
    exposureTimeLabel->setVisible( displayExposureTime );
    exposureTimeQCaLabel->setVisible( displayExposureTime );
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
    qDebug() << "zoom clicked";
}

// ROI apply button pressed
void QCaImage::roiClicked()
{
    qDebug() << "roi clicked";
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

        QImage qImage((uchar*) imageBuff.data(), imageBuffWidth, imageBuffHeight, QImage::Format_RGB32);
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
    if( zoom < 10 )
        zoom = 10;
    else if( zoom > 400 )
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
void QCaImage::setRotation( double rotationIn )
{
    rotation = rotationIn;
    videoWidget->setRotation( rotation );
}

double QCaImage::getRotation()
{
    return rotation;
}

// Size options
void QCaImage::setSizeOption( sizeOptions sizeOptionIn )
{
    // Save the size option
    sizeOption = sizeOptionIn;

    // Resize and rescale
    setImageBuff();
}

QCaImage::sizeOptions QCaImage::getSizeOption()
{
    return sizeOption;
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

// Show time colour
void QCaImage::setShowTimeColor(QColor pValue)
{
    qColorShowTime = pValue;
}

QColor QCaImage::getShowTimeColor()
{
    return qColorShowTime;
}

