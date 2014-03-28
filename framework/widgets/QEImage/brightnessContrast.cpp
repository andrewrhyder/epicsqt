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
 *  Copyright (c) 2013
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

/*
  This class manages the QEImage image display properties controls such as brightness and contrast controls.
 */

#include <brightnessContrast.h>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPainter>
#include <math.h>

#define SCALE_HEIGHT 20

// Gradient scale is the tangent of the gradient.
//
// Internal gradient range is from pi/4 (all pixel values in range)
// to pi/2 one pixel in range (vertical).
// Vertical can never be reached (min and max pixels are always kept at
// least one apart) so depending on the bit depth, maximum is near pi/2.
//
// User gradient range is from 0 to 1000. So scale factor is 1000/(pi/4) = 1273.239 and offset is pi/4 = 0.78539;
#define GRADIENT_USER_SCALE_FACTOR 1273.239
#define GRADIENT_BASE 0.78539

imageDisplayProperties::imageDisplayProperties()
{
    nonInteractive = false;

    inBrightnessSliderCallback = false;
    inGradientSliderCallback = false;
    inZeroValueSliderCallback = false;
    inFullValueSliderCallback = false;

    inBrightnessEditCallback = false;
    inGradientEditCallback = false;
    inZeroValueEditCallback = false;
    inFullValueEditCallback = false;

    zeroValue = 0;
    fullValue = 255;

    // Note the full value is only a default. It will be set when the first set of statistics arrive to the real full range,
    defaultFullValue = true;

    range = 255;

    // Initialise image stats
    bins = NULL;
    maxP = 0;
    minP = UINT_MAX;

    setFrameStyle( QFrame::StyledPanel|QFrame::Raised );

    QGridLayout* imageDisplayPropertiesMainLayout = new QGridLayout();
    imageDisplayPropertiesMainLayout->setSpacing( 10 );
    setLayout( imageDisplayPropertiesMainLayout );

    QHBoxLayout* imageDisplayPropertiesSub1Layout = new QHBoxLayout();
    QGridLayout* imageDisplayPropertiesSub2Layout = new QGridLayout();
    QHBoxLayout* imageDisplayPropertiesSub3Layout = new QHBoxLayout();

    QLabel* brightnessLabel = new QLabel( "Brightness:", this );
    QLabel* gradientLabel = new QLabel( "Gradient:\n(Contrast)", this );
    QLabel* minLabel = new QLabel( "Minimum:", this );
    QLabel* maxLabel = new QLabel( "Maximum:", this );

    autoBrightnessCheckBox = new QCheckBox( "Auto Brightness and Contrast", this );
    autoBrightnessCheckBox->setToolTip( "Set brightness and contrast to use the full dynamic range of an area when an area is selected");

    QPushButton* autoImageButton = new QPushButton( "Auto all", this );
    autoImageButton->setToolTip( "Set brightness and contrast to use the full dynamic range for the entire image");
    QObject::connect( autoImageButton, SIGNAL( clicked ( bool ) ), this,  SLOT  ( brightnessContrastAutoImageClicked()) );

    QPushButton* resetButton = new QPushButton( "Reset", this );
    resetButton->setToolTip( "Reset brightness and contrast");
    QObject::connect( resetButton, SIGNAL( clicked ( bool ) ), this,  SLOT  ( brightnessContrastResetClicked( bool )) );

    brightnessSlider = new QSlider( Qt::Horizontal, this );
    brightnessSlider->setMinimum( 0 );
    brightnessSlider->setMaximum( 100 );
    brightnessSlider->setValue( 50 );
    QObject::connect( brightnessSlider, SIGNAL( valueChanged ( int ) ), this,  SLOT  ( brightnessSliderValueChanged( int )) );

    gradientSlider = new QSlider( Qt::Horizontal, this );
    gradientSlider->setMinimum( 0 );
    gradientSlider->setMaximum( 1000 );
    gradientSlider->setValue( 0 );
    QObject::connect( gradientSlider, SIGNAL( valueChanged ( int ) ), this,  SLOT  ( gradientSliderValueChanged( int )) );

    zeroValueSlider = new QSlider( Qt::Horizontal, this );
    zeroValueSlider->setMinimum( 0 );
    zeroValueSlider->setMaximum( 254 );
    zeroValueSlider->setValue( 0 );
    QObject::connect( zeroValueSlider, SIGNAL( valueChanged ( int ) ), this,  SLOT  ( minSliderValueChanged( int )) );

    fullValueSlider = new QSlider( Qt::Horizontal, this );
    fullValueSlider->setMinimum( 1 );
    fullValueSlider->setMaximum( 255 );
    fullValueSlider->setValue( 255 );
    QObject::connect( fullValueSlider, SIGNAL( valueChanged ( int ) ), this,  SLOT  ( maxSliderValueChanged( int )) );

    hist = new histogram( this, this );

    histScroll = new histogramScroll( this, this );
    histScroll->setMinimumWidth( 256 );
    histScroll->setMinimumHeight(200 );
    histScroll->setWidget( hist );

    histZoom = new QSlider( Qt::Vertical, this );
    histZoom->setMinimum( 100 );
    histZoom->setMaximum( 1000 );
    histZoom->setValue( 100 );
    histZoom->setToolTip( "Zoom histogram");
    QObject::connect( histZoom, SIGNAL( valueChanged ( int ) ), this,  SLOT  ( histZoomSliderValueChanged( int )) );

    histXLabel = new QLabel( hist );
    histXLabel->setAlignment( Qt::AlignRight );

    brightnessSpinBox = new QSpinBox( this );
    brightnessSpinBox->setToolTip( "Brightness percentage (0 to 100)");
    brightnessSpinBox->setMinimum( 0 );
    brightnessSpinBox->setMaximum( 100 );
    brightnessSpinBox->setValue( brightnessSlider->value() );
    QObject::connect( brightnessSpinBox, SIGNAL( valueChanged ( int ) ), this,  SLOT  ( brightnessSpinBoxChanged( int )) );

    brightnessSpinBox->setMinimumWidth( 60 ); // Set width for all

    gradientSpinBox = new QSpinBox( this );
    gradientSpinBox->setToolTip( "Gradient (0 to 1000)");
    gradientSpinBox->setMinimum( 0 );
    gradientSpinBox->setMaximum( 1000 );
    gradientSpinBox->setValue( gradientSlider->value() );
    QObject::connect( gradientSpinBox, SIGNAL( valueChanged ( int ) ), this,  SLOT  ( gradientSpinBoxChanged( int )) );

    zeroValueSpinBox = new QSpinBox( this );
    zeroValueSpinBox->setToolTip( "Pixel value at low end of brightness / colour scale (0 to range limited by bit depth)");
    zeroValueSpinBox->setMinimum( 0 );
    zeroValueSpinBox->setMaximum( 254 );
    zeroValueSpinBox->setValue( zeroValueSlider->value() );
    QObject::connect( zeroValueSpinBox, SIGNAL( valueChanged ( int ) ), this,  SLOT  ( minSpinBoxChanged( int )) );

    fullValueSpinBox = new QSpinBox( this );
    fullValueSpinBox->setToolTip( "Pixel value at high end of brightness / colour scale (0 to range limited by bit depth)");
    fullValueSpinBox->setMinimum( 1 );
    fullValueSpinBox->setMaximum( 255 );
    fullValueSpinBox->setValue( fullValueSlider->value() );
    QObject::connect( fullValueSpinBox, SIGNAL( valueChanged ( int ) ), this,  SLOT  ( maxSpinBoxChanged( int )) );

    contrastReversalCheckBox = new QCheckBox( "Contrast Reversal", this );
    contrastReversalCheckBox->setToolTip( "Reverse light for dark");
    QObject::connect( contrastReversalCheckBox, SIGNAL( toggled( bool ) ), this,  SLOT  ( contrastReversalToggled( bool )) );

    logCheckBox = new QCheckBox( "Log scale", this );
    logCheckBox->setToolTip( "Logarithmic brightness scale");
    QObject::connect( logCheckBox, SIGNAL( toggled( bool ) ), this,  SLOT  ( logToggled( bool )) );

    falseColourCheckBox = new QCheckBox( "False Colour", this );
    falseColourCheckBox->setToolTip( "Interpret intensitiy scale as a range of colours");
    QObject::connect( falseColourCheckBox, SIGNAL( toggled( bool ) ), this,  SLOT  ( falseColourToggled( bool )) );

    imageDisplayPropertiesSub1Layout->addWidget( autoBrightnessCheckBox, 0, Qt::AlignLeft );
    imageDisplayPropertiesSub1Layout->addWidget( autoImageButton,        0, Qt::AlignLeft );
    imageDisplayPropertiesSub1Layout->addWidget( resetButton,            1, Qt::AlignLeft );

    imageDisplayPropertiesSub2Layout->addWidget( brightnessLabel,   0, 0 );
    imageDisplayPropertiesSub2Layout->addWidget( brightnessSlider,  0, 1 );
    imageDisplayPropertiesSub2Layout->addWidget( brightnessSpinBox, 0, 2 );

    imageDisplayPropertiesSub2Layout->addWidget( gradientLabel,     1, 0 );
    imageDisplayPropertiesSub2Layout->addWidget( gradientSlider,    1, 1 );
    imageDisplayPropertiesSub2Layout->addWidget( gradientSpinBox,   1, 2 );

    imageDisplayPropertiesSub2Layout->addWidget( minLabel,          2, 0 );
    imageDisplayPropertiesSub2Layout->addWidget( zeroValueSlider,   2, 1 );
    imageDisplayPropertiesSub2Layout->addWidget( zeroValueSpinBox,  2, 2 );

    imageDisplayPropertiesSub2Layout->addWidget( maxLabel,          3, 0 );
    imageDisplayPropertiesSub2Layout->addWidget( fullValueSlider,   3, 1 );
    imageDisplayPropertiesSub2Layout->addWidget( fullValueSpinBox,  3, 2 );

    imageDisplayPropertiesSub2Layout->setColumnStretch( 1, 1 );  // Sliders to take all spare room

    imageDisplayPropertiesSub3Layout->addWidget( contrastReversalCheckBox, 0, Qt::AlignLeft );
    imageDisplayPropertiesSub3Layout->addWidget( falseColourCheckBox,      0, Qt::AlignLeft );
    imageDisplayPropertiesSub3Layout->addWidget( logCheckBox,              1, Qt::AlignLeft );

    imageDisplayPropertiesMainLayout->addLayout( imageDisplayPropertiesSub1Layout, 0, 0 );
    imageDisplayPropertiesMainLayout->addLayout( imageDisplayPropertiesSub2Layout, 1, 0 );
    imageDisplayPropertiesMainLayout->addLayout( imageDisplayPropertiesSub3Layout, 2, 0 );

    imageDisplayPropertiesMainLayout->addWidget( histZoom, 0, 1, 3, 1 );
    imageDisplayPropertiesMainLayout->addWidget( histScroll, 0, 2, 3, 1 );
    imageDisplayPropertiesMainLayout->setColumnStretch( 1, 2 );  // Histogram to take all spare room

    range = 0;

    adjustSize();
}

imageDisplayProperties::~imageDisplayProperties()
{

    // !!!What needs to be deleted since they are all children of the form?
    delete autoBrightnessCheckBox;
    delete brightnessSlider;
    delete zeroValueSlider;
    delete fullValueSlider;
    delete gradientSlider;
    delete brightnessSpinBox;
    delete zeroValueSpinBox;
    delete fullValueSpinBox;
    delete gradientSpinBox;
    delete contrastReversalCheckBox;
    delete logCheckBox;
    delete falseColourCheckBox;

    delete histScroll;
    delete histZoom;
}

int imageDisplayProperties::getLowPixel()
{
    return zeroValue;
}

int imageDisplayProperties::getHighPixel()
{
    return fullValue;
}

bool imageDisplayProperties::getAutoBrightnessContrast()
{
    return autoBrightnessCheckBox->isChecked();
}

bool imageDisplayProperties::getContrastReversal()
{
    return contrastReversalCheckBox->isChecked();
}

bool imageDisplayProperties::getLog()
{
    return logCheckBox->isChecked();
}

bool imageDisplayProperties::getFalseColour()
{
    return falseColourCheckBox->isChecked();
}

// Reset the brightness and contrast to normal
void imageDisplayProperties::brightnessContrastResetClicked( bool )
{
    zeroValue = 0;
    fullValue = range;

    updateBrightnessInterface();
    updateGradientInterface();
    updateZeroValueInterface();
    updateFullValueInterface();

    emit imageDisplayPropertiesChange();
}

// Auto brightness and contrast check box has been checked or unchecked
void imageDisplayProperties::brightnessContrastAutoImageClicked()
{
    emit brightnessContrastAutoImage();
}

// Contrast reversal check box has been checked or unchecked
void imageDisplayProperties::contrastReversalToggled( bool )
{
    emit imageDisplayPropertiesChange();
}

// Log brightness check box has been checked or unchecked
void imageDisplayProperties::logToggled( bool )
{
    emit imageDisplayPropertiesChange();
}

// False colour check box has been checked or unchecked
void imageDisplayProperties::falseColourToggled( bool )
{
    emit imageDisplayPropertiesChange();
}

//=============================================


// Set brightness and contrast based on a values for black and white
void imageDisplayProperties::setBrightnessContrast( const unsigned int max, const unsigned int min )
{
    updateZeroValueFullValue( min, max );
    emit imageDisplayPropertiesChange();
}

void imageDisplayProperties::setAutoBrightnessContrast( bool autoBrightnessContrast )
{
    autoBrightnessCheckBox->setChecked( autoBrightnessContrast );
}

void imageDisplayProperties::setContrastReversal( bool contrastReversal )
{
    contrastReversalCheckBox->setChecked( contrastReversal );
}

void imageDisplayProperties::setLog( bool log )
{
    logCheckBox->setChecked( log );
}

void imageDisplayProperties::setFalseColour( bool falseColour )
{
    falseColourCheckBox->setChecked( falseColour );
}

//==========================================================

// The local brightness slider has been moved
void imageDisplayProperties::brightnessSliderValueChanged( int localBrightnessIn )
{
    if( nonInteractive )
    {
        return;
    }

    inBrightnessSliderCallback = true;
    updateBrightness( (double)(localBrightnessIn)/100.0 );
    inBrightnessSliderCallback = false;

    emit imageDisplayPropertiesChange();
}

void imageDisplayProperties::brightnessSpinBoxChanged( int value )
{
    if( nonInteractive )
    {
        return;
    }

    inBrightnessEditCallback = true;
    updateBrightness( value/100.0 );
    inBrightnessEditCallback = false;

}

// The gradient slider has been moved
void imageDisplayProperties::gradientSliderValueChanged( int value )
{
    if( nonInteractive )
    {
        return;
    }

    inGradientSliderCallback = true;
    updateGradient( (double)(value)/GRADIENT_USER_SCALE_FACTOR+GRADIENT_BASE );
    inGradientSliderCallback = false;

    emit imageDisplayPropertiesChange();
}

void imageDisplayProperties::gradientSpinBoxChanged( int value )
{
    if( nonInteractive )
    {
        return;
    }

    inGradientEditCallback = true;
    updateGradient( (double)(value)/GRADIENT_USER_SCALE_FACTOR+GRADIENT_BASE );
    inGradientEditCallback = false;

    emit imageDisplayPropertiesChange();
}

// The minimum slider has been moved
void imageDisplayProperties::minSliderValueChanged( int value )
{
    if( nonInteractive )
    {
        return;
    }

    inZeroValueSliderCallback = true;
    updateZeroValue( value );
    inZeroValueSliderCallback = false;

    emit imageDisplayPropertiesChange();
}

void imageDisplayProperties::minSpinBoxChanged( int value )
{
    if( nonInteractive )
    {
        return;
    }

    inZeroValueEditCallback = true;
    updateZeroValue( value );
    inZeroValueEditCallback = false;

    emit imageDisplayPropertiesChange();
}

// The maximum slider has been moved
void imageDisplayProperties::maxSliderValueChanged( int value )
{
    if( nonInteractive )
    {
        return;
    }

    inFullValueSliderCallback = true;
    updateFullValue( value );
    inFullValueSliderCallback = false;

    emit imageDisplayPropertiesChange();
}

void imageDisplayProperties::maxSpinBoxChanged( int value )
{
    if( nonInteractive )
    {
        return;
    }

    inFullValueEditCallback = true;
    updateFullValue( value );
    inFullValueEditCallback = false;


    emit imageDisplayPropertiesChange();
}

//=========================================================

void imageDisplayProperties::updateBrightness( double val )
{
    // Brightness ranges from 0.0 (0%) to 1.0 (100%)
    // Validate brightness
    if( val < 0.0 )
    {
        val = 0.0;
    } else if( val > 1.0 )
    {
        val = 1.0;
    }

    // Update brightness contrast values according to new brightness
    // Note, this never alters the span, so gradient never changes
    double span = fullValue - zeroValue;
    zeroValue = (range-span)*(1.0-val);
    fullValue = zeroValue+span;

    // Update interface
    updateZeroValueInterface();
    updateFullValueInterface();
    updateBrightnessInterface();
    updateGradientInterface();

    hist->update();
}

void imageDisplayProperties::updateGradient( double val )
{
    // Gradient is range / span
    // With zeroValue at most one less than full value, gradient can go from 1 to range
    // validate gradient
    val = tan( val );
    if( val < 1.0 )
    {
        val = 1.0;
    } else if( val > range )
    {
        val = range;
    }

    double mid = (double)(fullValue+zeroValue)/2;
    double span = (double)range/(double)val;

    double low =  mid-(span/2);
    if( low < 0.0 )
    {
        low = 0.0;
    }

    zeroValue = floor( low + 0.5 );        // Note, round() not in windows math.h. Using floor+0.5 instead
    fullValue = floor( low + span + 0.5 ); // Note, round() not in windows math.h. Using floor+0.5 instead

    updateZeroValueInterface();
    updateFullValueInterface();
    updateBrightnessInterface();
    updateGradientInterface();

    hist->update();
}

void imageDisplayProperties::updateZeroValue( unsigned int val )
{
    if( val >= range )
    {
        val = range-1;
    }

    zeroValue = val;
    if( zeroValue >= fullValue )
    {
        fullValue = zeroValue+1;
    }

    updateZeroValueInterface();
    updateFullValueInterface();
    updateBrightnessInterface();
    updateGradientInterface();

    hist->update();
}

void imageDisplayProperties::updateFullValue( unsigned int val )
{
    if( val < 1 )
    {
        val = 1;
    }
    else if( val > range )
    {
        val = range;
    }


    fullValue = val;
    if( fullValue <= zeroValue )
    {
        zeroValue = fullValue-1;
    }


    updateZeroValueInterface();
    updateFullValueInterface();
    updateBrightnessInterface();
    updateGradientInterface();

    hist->update();
}

void imageDisplayProperties::updateZeroValueFullValue( unsigned int min, unsigned int max )
{
    if( min >= range )
    {
        min = range-1;
    }

    zeroValue = min;

    if( max > range )
    {
        max = range;
    }

    fullValue = max;

    if( zeroValue >= fullValue )
    {
        fullValue = zeroValue+1;
    }

    updateZeroValueInterface();
    updateFullValueInterface();
    updateBrightnessInterface();
    updateGradientInterface();

    hist->update();
}

//=========================================================

void imageDisplayProperties::updateBrightnessInterface()
{
    // Calculate brightness (derived)
    unsigned int span = fullValue-zeroValue;
    unsigned int brightnessScale = range-span;
    double brightness;
    if( brightnessScale )
    {
        brightness = 1.0-((double)zeroValue/(double)brightnessScale);
    }
    else
    {
        brightness = 0.5;
    }

    // Update interface
    nonInteractive = true;

    if( !inBrightnessEditCallback )
    {
        brightnessSpinBox->setValue( (int)(brightness*100) );
    }

    if( !inBrightnessSliderCallback )
    {
        brightnessSlider->setValue( brightness*100 );
    }

    nonInteractive = false;
}

void imageDisplayProperties::updateGradientInterface()
{
    // Calculate gradient (derived)
    double gradient = (atan((double)range/(double)(fullValue-zeroValue))-GRADIENT_BASE)*GRADIENT_USER_SCALE_FACTOR;

    // Update interface
    nonInteractive = true;

    if( !inGradientEditCallback )
    {
        gradientSpinBox->setValue( gradient );
    }

    if( !inGradientSliderCallback )
    {
        gradientSlider->setValue( gradient );
    }

    nonInteractive = false;
}

void imageDisplayProperties::updateZeroValueInterface()
{
    // Update interface
    nonInteractive = true;

    if( !inZeroValueEditCallback )
    {
        zeroValueSpinBox->setValue( zeroValue );
    }

    if( !inZeroValueSliderCallback )
    {
        zeroValueSlider->setValue( zeroValue );
    }

    nonInteractive = false;
}

void imageDisplayProperties::updateFullValueInterface()
{
    // Update interface
    nonInteractive = true;

    if( !inFullValueEditCallback )
    {
        fullValueSpinBox->setValue( fullValue );
    }

    if( !inFullValueSliderCallback )
    {
        fullValueSlider->setValue( fullValue );
    }

    nonInteractive = false;
}

//=========================================================

void imageDisplayProperties::setStatistics( unsigned int minPIn, unsigned int maxPIn, unsigned int bitDepth, unsigned int binsIn[HISTOGRAM_BINS], rgbPixel pixelLookupIn[256] )
{
    // Update image statistics
    minP = minPIn;
    maxP = maxPIn;
    depth = bitDepth;
    bins = binsIn;
    pixelLookup = pixelLookupIn;

    // Recalculate dependand variables
    range = (1<<depth)-1;

    // Apply changes
    zeroValueSlider->setMaximum( range-1 );
    fullValueSlider->setMaximum( range );

    zeroValueSpinBox->setMaximum( range-1 );
    fullValueSpinBox->setMaximum( range );

    if( defaultFullValue )
    {
        defaultFullValue = false;

        fullValue = range;
        updateFullValueInterface();
    }

    histXLabel->setText( QString( "%1" ).arg( range ) );

    hist->update();
}

// The histogram zoom slider has been moved
void imageDisplayProperties::histZoomSliderValueChanged( int value )
{
    setHistZoom( value );
}

// The histogram zoom slider has been moved
void imageDisplayProperties::setHistZoom( int value )
{
    // Determine the width and height that will just fit without scroll bars
    double fitWidth  = (double)(histScroll->width()  - histScroll->contentsMargins().left()*2 );//  - histScroll->frameWidth()*2);
    double fitHeight = (double)(histScroll->height() - histScroll->contentsMargins().top()*2 );//  - histScroll->frameWidth()*2);

    // Set the new zoomed size
    QRect currentGeom = hist->geometry();
    hist->setGeometry( currentGeom.x(), currentGeom.y(), (double)(value)/100*fitWidth, (double)(value)/100*fitHeight );
}

// Get the current histogram zoom percentage
int imageDisplayProperties::getHistZoom()
{
    return histZoom->value();
}

// Construct the histogram
histogramScroll::histogramScroll( QWidget* parent, imageDisplayProperties* idpIn )
    : QScrollArea( parent )
{
    idp = idpIn;
}

// Histogram resize event
void histogramScroll::resizeEvent( QResizeEvent* )
{
    idp->setHistZoom( idp->getHistZoom() );
}


// Construct the histogram
histogram::histogram( QWidget* parent, imageDisplayProperties* idpIn )
    : QFrame( parent )
{
    setFrameStyle( QFrame::Panel );
    idp = idpIn;
}

// Histogram resize event
void histogram::resizeEvent( QResizeEvent* )
{
    // Keep the X asis label in the bottom right of the histogram
    idp->histXLabel->setGeometry( width()-idp->histXLabel->width()-2,
                                  height()-idp->histXLabel->height()-10, //SCALE_HEIGHT,
                                  idp->histXLabel->width(),
                                  idp->histXLabel->height());
}

// Histogram repaint event
void histogram::paintEvent(QPaintEvent* )
{

    // Do nothing if no image info yet
    if( idp->bins == NULL )
    {
        return;
    }

    // Determine range (ignore counts in first and last buckets as it is common for huge counts in one or both ends)
    unsigned int binRange = 0;
    for( int i = 1; i < HISTOGRAM_BINS-1; i++ )
    {
        if( idp->bins[i] > binRange )
        {
            binRange = idp->bins[i];
        }
    }

    // Do nothing if no data present
    if( binRange == 0 )
    {
        return;
    }

    // Determine bin stuff
    unsigned int bitsPerBin;
    unsigned int minBin;
    unsigned int maxBin;

    if( idp->depth<=8)
    {
        bitsPerBin = 1;
        minBin = idp->zeroValue;
        maxBin = idp->fullValue;
    }
    else
    {
        // Determine bins for maximum and minimum values
        bitsPerBin = idp->depth-8;

        minBin = idp->zeroValue>>bitsPerBin;
        if( minBin > 254 )
        {
            minBin = 254;
        }

        maxBin = idp->fullValue>>bitsPerBin;
        if( maxBin <= minBin )
        {
            maxBin = minBin+1;
        }
    }

     QPainter p( this );

    // Draw the histogram with scale...

    // Determine overall size
     int h = height()-1-SCALE_HEIGHT;
     double w = width();

    // Initialise rectangle used for both histogram and scale
    QRectF barRect;

    barRect.setBottom( h+1 );

    barRect.setLeft( 0 );
    barRect.setWidth( w/HISTOGRAM_BINS );

    // Draw histogram bins
    for( unsigned int i = 0; i < HISTOGRAM_BINS; i++ )
    {
        // Draw histogram bar
        barRect.setTop( h - (double)(idp->bins[i])*h/binRange );
        p.fillRect( barRect, Qt::red );

        // Move on to the next bar
        barRect.moveLeft( barRect.right() );
    }

    // Draw scale bar
    int minX = idp->zeroValue*w/idp->range;
    int maxX = idp->fullValue*w/idp->range;
    imageDisplayProperties::rgbPixel* col;

    int scaleTop = h+3;
    int scaleHeight = SCALE_HEIGHT-4;

    // Draw the first colour in the lookup table for the entire area to the left of the minimum value
    QRect scaleRect = QRect( 0, scaleTop, minX, scaleHeight );
    col = &(idp->pixelLookup[0] );
    p.fillRect( scaleRect, QColor( col->p[2], col->p[1], col->p[0] ) );

    // Draw the last colour in the lookup table for the entire area to the right of the maximum value
    scaleRect = QRect( maxX, scaleTop, w-maxX, scaleHeight );
    col = &(idp->pixelLookup[255] );
    p.fillRect( scaleRect, QColor( col->p[2], col->p[1], col->p[0] ) );

    // Display all colors in the lookup table
    QRectF colourRect;

    colourRect.setTop( scaleTop );
    colourRect.setBottom( scaleTop + scaleHeight );

    colourRect.setLeft( minX );
    colourRect.setWidth( (double)(maxX-minX)/255 );

    for( unsigned int i = 0; i < 256; i++ )
    {
        // Draw the color rectangle
        imageDisplayProperties::rgbPixel* col = &(idp->pixelLookup[i] );
        p.fillRect( colourRect, QColor( col->p[2], col->p[1], col->p[0] ) );

        // Move on to the next colour
        colourRect.moveLeft( colourRect.right() );
    }

    // Prepare to draw the bounds an gradient

    QPen pen( Qt::blue );
    p.setPen( pen );

    // Draw max and min
    pen.setStyle( Qt::DashLine );
    double minScaled = (double)(minBin)*w/HISTOGRAM_BINS;
    double maxScaled = (double)(maxBin)*w/HISTOGRAM_BINS;
    p.drawLine( minScaled,0,minScaled,h);
    p.drawLine( maxScaled,0,maxScaled,h);

    // Draw gradient
    pen.setStyle( Qt::SolidLine );
    p.setPen( pen );

    p.drawLine( minScaled,h,maxScaled,0);
}
