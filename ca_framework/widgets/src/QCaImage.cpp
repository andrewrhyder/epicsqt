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
#include <QtMultimedia>
#include <QSpacerItem>


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

    // Set the initial state
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;

    // Use frame signals
    // --Currently none--

    // Create the video destination
    videoWidget = new VideoWidget;
    surface = videoWidget->videoSurface();

    // Add the video destination to the widget
    scrollArea = new QScrollArea;
    scrollArea->setFrameStyle( QFrame::NoFrame );
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget( videoWidget );
    scrollArea->setEnabled( false );  // Reflects initial disconnected state

    // Sum layout containing labels
    labelLayout = new QGridLayout;
    labelLayout->setMargin( 0 );

    acquirePeriodQCaLabel = new QCaLabel( this );
    acquirePeriodLabel = new QLabel( this );
    acquirePeriodLabel->setText( "Acquire Period:" );

    exposureTimeQCaLabel = new QCaLabel( this );
    exposureTimeLabel = new QLabel( this );
    exposureTimeLabel->setText( "Exposure Time:" );



    buttonLayout = new QGridLayout;
    buttonLayout->setMargin(0);

    qPushButtonPause = new QPushButton(this);
    qPushButtonPause->setText("Pause");
    qPushButtonPause->setToolTip("Pause image display");
    buttonLayout->addWidget(qPushButtonPause, 0, 0);
    QObject::connect(qPushButtonPause, SIGNAL(clicked()), this, SLOT(buttonPauseClicked()));

    qPushButtonSave = new QPushButton(this);
    qPushButtonSave->setText("Save");
    qPushButtonSave->setToolTip("Save displayed image");
    QObject::connect(qPushButtonSave, SIGNAL(clicked()), this, SLOT(buttonSaveClicked()));
    buttonLayout->addWidget(qPushButtonSave, 0, 1);





    // Main layout containing image and label layout
    mainLayout = new QVBoxLayout;
    mainLayout->setMargin( 0 );

    mainLayout->addWidget( scrollArea );
    mainLayout->addItem( labelLayout );
    mainLayout->addItem(buttonLayout);


    setLayout( mainLayout );

    // set up labels as required by properties
    manageAcquirePeriodLabel();
    manageExposureTimeLabel();

    // Initially set the video widget to the size of the scroll bar
    // This will be resized when the image size is known
    videoWidget->resize( scrollArea->width(), scrollArea->height() );

    // Populate color index table for grayscale
    // The following table is required if using Format_Indexed8 QImage with a grey scale lookup table
    //for( int i = 0; i < 256; i++ )
    //{
    //    greyscaleColors.append( (i<<16)+(i<<8)+i );
    //}

    // Set image size to zero
    // Image will not be presented until size is available
    imageBuffWidth = 0;
    imageBuffHeight = 0;

    // Set default image format
    formatOption = GREY8;


    setShowTimeColor(QColor(0, 255, 0));

    pauseEnabled = false;
    showTimeEnabled = false;


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


    if (pauseEnabled)
    {
        return;
    }



    /// Signal a database value change to any Link widgets
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
        scrollArea->horizontalScrollBar()->setValue( initialHozScrollPos );
        initScrollPosSet = true;
    }

/* The following code was used to generate a grey scale QImage (Format_Indexed8 using a grey scale lookup table)
   Since the data is one byte per pixel, it needed to ensure each line was 32bit aligned
   NOTE!!   This code had the advantage of not having to do ANY processing for 8 bit grey scale
            images that had 32 bit aligned lines.

    // If supplied data is already 4 byte aligned, copy it straight in
    if( imageBuffWidth%4 == 0 )
    {
        imageBuff = imageIn;
    }

    // If supplied data is not 4 byte aligned, then align it
    else
    {
        const char* dataIn = imageIn.data();
        char* dataOut = imageBuff.data();
        unsigned long buffIndex = 0;
        unsigned long dataIndex = 0;
        unsigned long pad = 4-(imageBuffWidth%4);
        for( unsigned long y = 0; y < imageBuffHeight; y++ )
        {
            for( unsigned long x = 0; x < imageBuffWidth; x++ )
            {
                // Don't overrun byte array buffer
                if( buffIndex >= (unsigned long)imageBuff.size() )
                    break;

                // Copy pixel
                dataOut[buffIndex] = dataIn[dataIndex];
                dataIndex++;
                buffIndex++;
            }
            buffIndex += pad;
        }
    }

    // Generate a frame from the data
    QImage image( (uchar*)(imageBuff.data()), imageBuffWidth, imageBuffHeight, QImage::Format_Indexed8 );
    image.setColorTable( greyscaleColors );
    QImage frameImage = image.convertToFormat( QImage::Format_RGB888 );
*/

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

    unsigned char* temp = (unsigned char*)(imageBuff.data());
    int buffSize = imageBuff.size();
    for( int i = 0; i < buffSize; i++ )
    {
        temp[i] = 0;
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

            case GREY12:
            {
                // Duplicate top 8 bits of the grey scale into each color
                unsigned long inPixel = *(unsigned short*)(&dataIn[dataIndex]);
                inPixel = inPixel>>4;
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



    if (showTimeEnabled)
    {
        QPainter qPainter(&frameImage);
        qPainter.setFont(QFont("Courier", 9 * ((float) imageBuffWidth / (float) imageBuffHeight)));
        qPainter.setPen(qColorShowTime);
        qPainter.drawText(5, 15, QDateTime().currentDateTime().toString("yyyy/MM/dd - hh:mm:ss"));
    }



    QVideoFrame frame( frameImage );
    if( !frame.isValid() )
    {
        qDebug() << "Bad Frame";
        return;
    }


    // Format and present the frame
    QVideoSurfaceFormat currentFormat = surface->surfaceFormat();


    if( frame.pixelFormat() != currentFormat.pixelFormat() ||
        frame.size() != currentFormat.frameSize() )
    {
        QVideoSurfaceFormat format( frame.size(), frame.pixelFormat() );

        if( !surface->start(format) )
        {
            qDebug() << "Surface start failed";
            return;
        }
    }

    if( !surface->present(frame) )
    {
        surface->stop();
        qDebug() << "Surface could not present frame";
        return;
    }

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

// Add or remove the acquire period label
void QCaImage::manageAcquirePeriodLabel()
{
    if( displayAcquirePeriod )
    {
        labelLayout->addWidget( acquirePeriodLabel, 0, 0 );
        labelLayout->addWidget( acquirePeriodQCaLabel, 0, 1 );
        labelLayout->setColumnStretch( 1, 1 );
        acquirePeriodLabel->show();
        acquirePeriodQCaLabel->show();
    }
    else
    {
        labelLayout->removeWidget( acquirePeriodLabel );
        labelLayout->removeWidget( acquirePeriodQCaLabel );
        acquirePeriodLabel->hide();
        acquirePeriodQCaLabel->hide();
    }
}

// Add or remove the exposure time label
void QCaImage::manageExposureTimeLabel()
{
    if( displayExposureTime )
    {
        labelLayout->addWidget( exposureTimeLabel, 1, 0 );
        labelLayout->addWidget( exposureTimeQCaLabel, 1, 1 );
        labelLayout->setColumnStretch( 1, 1 );
        exposureTimeLabel->show();
        exposureTimeQCaLabel->show();
    }
    else
    {
        labelLayout->removeWidget( exposureTimeLabel );
        labelLayout->removeWidget( exposureTimeQCaLabel );
        exposureTimeLabel->hide();
        exposureTimeQCaLabel->hide();
    }
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

void QCaImage::setZoom( int zoomIn )
{

    // Save the zoom
    if( zoom < 10 )
        zoom = 10;
    else if( zoom > 400 )
        zoom = 400;
    else
        zoom = zoomIn;

    // Resize and rescale
    setImageBuff();
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

// Zoom level
int QCaImage::getZoom()
{
    return zoom;
}
void QCaImage::setInitialHozScrollPos( int initialHozScrollPosIn )
{
    initialHozScrollPos = initialHozScrollPosIn;
    scrollArea->horizontalScrollBar()->setValue( initialHozScrollPos );
}

int QCaImage::getInitialHozScrollPos()
{
    return initialHozScrollPos;
}

void QCaImage::setInitialVertScrollPos( int initialVertScrollPosIn )
{
    initialVertScrollPos = initialVertScrollPosIn;
    scrollArea->verticalScrollBar()->setValue( initialVertScrollPos );
}

int QCaImage::getInitialVertScrollPos()
{
    return initialVertScrollPos;
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






void QCaImage::setShowButtonPause(bool pValue)
{

    qPushButtonPause->setVisible(pValue);

}



bool QCaImage::getShowButtonPause()
{

    return qPushButtonPause->isVisible();

}




void QCaImage::buttonPauseClicked()
{

    if (pauseEnabled)
    {
        qPushButtonPause->setText("Pause");
        qPushButtonPause->setToolTip("Pause image display");
        pauseEnabled = false;
    }
    else
    {
        qPushButtonPause->setText("Resume");
        qPushButtonPause->setToolTip("Resume image display");
        pauseEnabled = true;
    }

}



void QCaImage::setShowButtonSave(bool pValue)
{

    qPushButtonSave->setVisible(pValue);

}



bool QCaImage::getShowButtonSave()
{

    return qPushButtonSave->isVisible();

}



void QCaImage::buttonSaveClicked()
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





void QCaImage::setShowTime(bool pValue)
{

    showTimeEnabled = pValue;

}



bool QCaImage::getShowTime()
{

    return showTimeEnabled;

}





void QCaImage::setShowTimeColor(QColor pValue)
{

    qColorShowTime = pValue;

}



QColor QCaImage::getShowTimeColor()
{

    return qColorShowTime;

}

