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
  This class manages the QEImage local brightness and contrast controls.
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

localBrightnessContrast::localBrightnessContrast()
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
    range = 255;

    // Initialise image stats
    bins = NULL;
    maxP = 0;
    minP = UINT_MAX;

    setFrameStyle( QFrame::StyledPanel|QFrame::Raised );

    QGridLayout* brightnessContrastMainLayout = new QGridLayout();
    brightnessContrastMainLayout->setSpacing( 10 );
    setLayout( brightnessContrastMainLayout );

    QHBoxLayout* brightnessContrastSub1Layout = new QHBoxLayout();
    QGridLayout* brightnessContrastSub2Layout = new QGridLayout();
    QHBoxLayout* brightnessContrastSub3Layout = new QHBoxLayout();

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

    histXLabel = new QLabel( hist );
    histXLabel->setAlignment( Qt::AlignRight );

    brightnessValidator = new QIntValidator( 0, 100, this );
    gradientValidator = new QIntValidator( 0, 1000, this );
    zeroValueValidator = new QIntValidator( 0, 254, this );
    fullValueValidator = new QIntValidator( 1, 255, this );

    brightnessLineEdit = new QLineEdit( this );
    brightnessLineEdit->setToolTip( "Brightness percentage (0 to 100)");
    brightnessLineEdit->setValidator( brightnessValidator );
    brightnessLineEdit->setText( QString( "%1" ).arg( brightnessSlider->value() ) );
    QObject::connect( brightnessLineEdit, SIGNAL( textChanged ( QString ) ), this,  SLOT  ( brightnessLineEditChanged( QString )) );

    brightnessLineEdit->setMinimumWidth( 100 ); // Set width for all readbacks

    gradientLineEdit = new QLineEdit( this );
    gradientLineEdit->setToolTip( "Gradient (0 to 1000)");
    gradientLineEdit->setValidator( gradientValidator );
    gradientLineEdit->setText( QString( "%1" ).arg( gradientSlider->value() ) );
    QObject::connect( gradientLineEdit, SIGNAL( textChanged ( QString ) ), this,  SLOT  ( gradientLineEditChanged( QString )) );

    zeroValueLineEdit = new QLineEdit( this );
    zeroValueLineEdit->setToolTip( "Pixel value at low end of brightness / colour scale (0 to range limited by bit depth)");
    zeroValueLineEdit->setValidator( zeroValueValidator );
    zeroValueLineEdit->setText( QString( "%1" ).arg( zeroValueSlider->value() ) );
    QObject::connect( zeroValueLineEdit, SIGNAL( textChanged ( QString ) ), this,  SLOT  ( minLineEditChanged( QString )) );

    fullValueLineEdit = new QLineEdit( this );
    fullValueLineEdit->setToolTip( "Pixel value at high end of brightness / colour scale (0 to range limited by bit depth)");
    fullValueLineEdit->setValidator( fullValueValidator );
    fullValueLineEdit->setText( QString( "%1" ).arg( fullValueSlider->value() ) );
    QObject::connect( fullValueLineEdit, SIGNAL( textChanged ( QString ) ), this,  SLOT  ( maxLineEditChanged( QString )) );

    contrastReversalCheckBox = new QCheckBox( "Contrast Reversal", this );
    contrastReversalCheckBox->setToolTip( "Reverse light for dark");
    QObject::connect( contrastReversalCheckBox, SIGNAL( toggled( bool ) ), this,  SLOT  ( contrastReversalToggled( bool )) );

    logCheckBox = new QCheckBox( "Log scale", this );
    logCheckBox->setToolTip( "Logarithmic brightness scale");
    QObject::connect( logCheckBox, SIGNAL( toggled( bool ) ), this,  SLOT  ( logToggled( bool )) );

    falseColourCheckBox = new QCheckBox( "False Colour", this );
    falseColourCheckBox->setToolTip( "Interpret intensitiy scale as a range of colours");
    QObject::connect( falseColourCheckBox, SIGNAL( toggled( bool ) ), this,  SLOT  ( falseColourToggled( bool )) );

    brightnessContrastSub1Layout->addWidget( autoBrightnessCheckBox, 0, Qt::AlignLeft );
    brightnessContrastSub1Layout->addWidget( autoImageButton, 0, Qt::AlignLeft );
    brightnessContrastSub1Layout->addWidget( resetButton, 1, Qt::AlignLeft );

    brightnessContrastSub2Layout->addWidget( brightnessLabel,   0, 0 );
    brightnessContrastSub2Layout->addWidget( brightnessSlider,  0, 1 );
    brightnessContrastSub2Layout->addWidget( brightnessLineEdit, 0, 2 );

    brightnessContrastSub2Layout->addWidget( gradientLabel,     1, 0 );
    brightnessContrastSub2Layout->addWidget( gradientSlider,    1, 1 );
    brightnessContrastSub2Layout->addWidget( gradientLineEdit,   1, 2 );

    brightnessContrastSub2Layout->addWidget( minLabel,          2, 0 );
    brightnessContrastSub2Layout->addWidget( zeroValueSlider,         2, 1 );
    brightnessContrastSub2Layout->addWidget( zeroValueLineEdit,        2, 2 );

    brightnessContrastSub2Layout->addWidget( maxLabel,          3, 0 );
    brightnessContrastSub2Layout->addWidget( fullValueSlider,         3, 1 );
    brightnessContrastSub2Layout->addWidget( fullValueLineEdit,        3, 2 );

    brightnessContrastSub2Layout->setColumnStretch( 1, 1 );  // Read back labels to take all spare room

    brightnessContrastSub3Layout->addWidget( contrastReversalCheckBox, 0, Qt::AlignLeft );
    brightnessContrastSub3Layout->addWidget( falseColourCheckBox,      0, Qt::AlignLeft );
    brightnessContrastSub3Layout->addWidget( logCheckBox,              1, Qt::AlignLeft );

    brightnessContrastMainLayout->addLayout( brightnessContrastSub1Layout, 0, 0 );
    brightnessContrastMainLayout->addLayout( brightnessContrastSub2Layout, 1, 0 );
    brightnessContrastMainLayout->addLayout( brightnessContrastSub3Layout, 2, 0 );

    brightnessContrastMainLayout->addWidget( hist, 0, 1, 3, 1 );

    range = 0;

    adjustSize();
}

localBrightnessContrast::~localBrightnessContrast()
{
    delete autoBrightnessCheckBox;
    delete brightnessSlider;
    delete zeroValueSlider;
    delete fullValueSlider;
    delete gradientSlider;
    delete brightnessLineEdit;
    delete zeroValueLineEdit;
    delete fullValueLineEdit;
    delete gradientLineEdit;
    delete contrastReversalCheckBox;
    delete logCheckBox;
    delete falseColourCheckBox;

    delete hist;

    delete brightnessValidator;
    delete gradientValidator;
    delete zeroValueValidator;
    delete fullValueValidator;
}

int localBrightnessContrast::getLowPixel()
{
    return zeroValue;
}

int localBrightnessContrast::getHighPixel()
{
    return fullValue;
}

bool localBrightnessContrast::getAutoBrightnessContrast()
{
    return autoBrightnessCheckBox->isChecked();
}

bool localBrightnessContrast::getContrastReversal()
{
    return contrastReversalCheckBox->isChecked();
}

bool localBrightnessContrast::getLog()
{
    return logCheckBox->isChecked();
}

bool localBrightnessContrast::getFalseColour()
{
    return falseColourCheckBox->isChecked();
}

// Reset the brightness and contrast to normal
void localBrightnessContrast::brightnessContrastResetClicked( bool )
{
    zeroValue = 0;
    fullValue = range;

    updateBrightnessInterface();
    updateGradientInterface();
    updateZeroValueInterface();
    updateFullValueInterface();

    emit brightnessContrastChange();
}

// Auto brightness and contrast check box has been checked or unchecked
void localBrightnessContrast::brightnessContrastAutoImageClicked()
{
    emit brightnessContrastAutoImage();
}

// Contrast reversal check box has been checked or unchecked
void localBrightnessContrast::contrastReversalToggled( bool )
{
    emit brightnessContrastChange();
}

// Log brightness check box has been checked or unchecked
void localBrightnessContrast::logToggled( bool )
{
    emit brightnessContrastChange();
}

// False colour check box has been checked or unchecked
void localBrightnessContrast::falseColourToggled( bool )
{
    emit brightnessContrastChange();
}

//=============================================


// Set brightness and contrast based on a values for black and white
void localBrightnessContrast::setBrightnessContrast( const unsigned int max, const unsigned int min )
{
    updateZeroValueFullValue( min, max );
    emit brightnessContrastChange();
}

void localBrightnessContrast::setAutoBrightnessContrast( bool autoBrightnessContrast )
{
    autoBrightnessCheckBox->setChecked( autoBrightnessContrast );
}

void localBrightnessContrast::setContrastReversal( bool contrastReversal )
{
    contrastReversalCheckBox->setChecked( contrastReversal );
}

void localBrightnessContrast::setLog( bool log )
{
    logCheckBox->setChecked( log );
}

void localBrightnessContrast::setFalseColour( bool falseColour )
{
    falseColourCheckBox->setChecked( falseColour );
}

//==========================================================

// The local brightness slider has been moved
void localBrightnessContrast::brightnessSliderValueChanged( int localBrightnessIn )
{
    if( nonInteractive )
    {
        return;
    }

    inBrightnessSliderCallback = true;
    updateBrightness( (double)(localBrightnessIn)/100.0 );
    inBrightnessSliderCallback = false;

    emit brightnessContrastChange();
}

void localBrightnessContrast::brightnessLineEditChanged( QString text )
{
    if( nonInteractive )
    {
        return;
    }

    bool ok;
    double value = text.toDouble( &ok );
    if( !ok )
    {
        return;
    }

    inBrightnessEditCallback = true;
    updateBrightness( value/100.0 );
    inBrightnessEditCallback = false;

}

// The gradient slider has been moved
void localBrightnessContrast::gradientSliderValueChanged( int value )
{
    if( nonInteractive )
    {
        return;
    }

    inGradientSliderCallback = true;
    updateGradient( (double)(value)/GRADIENT_USER_SCALE_FACTOR+GRADIENT_BASE );
    inGradientSliderCallback = false;

    emit brightnessContrastChange();
}

void localBrightnessContrast::gradientLineEditChanged( QString text )
{
    if( nonInteractive )
    {
        return;
    }

    bool ok;
    double value = text.toDouble( &ok );
    if( !ok )
    {
        return;
    }

    inGradientEditCallback = true;
    updateGradient( value/GRADIENT_USER_SCALE_FACTOR+GRADIENT_BASE );
    inGradientEditCallback = false;

}

// The minimum slider has been moved
void localBrightnessContrast::minSliderValueChanged( int value )
{
    if( nonInteractive )
    {
        return;
    }

    inZeroValueSliderCallback = true;
    updateZeroValue( value );
    inZeroValueSliderCallback = false;

    emit brightnessContrastChange();
}

void localBrightnessContrast::minLineEditChanged( QString text )
{
    if( nonInteractive )
    {
        return;
    }

    bool ok;
    double value = text.toDouble( &ok );
    if( !ok )
    {
        return;
    }

    inZeroValueEditCallback = true;
    updateZeroValue( value );
    inZeroValueEditCallback = false;

}

// The maximum slider has been moved
void localBrightnessContrast::maxSliderValueChanged( int value )
{
    if( nonInteractive )
    {
        return;
    }

    inFullValueSliderCallback = true;
    updateFullValue( value );
    inFullValueSliderCallback = false;

    emit brightnessContrastChange();
}

void localBrightnessContrast::maxLineEditChanged( QString text )
{
    if( nonInteractive )
    {
        return;
    }

    bool ok;
    double value = text.toDouble( &ok );
    if( !ok )
    {
        return;
    }

    inFullValueEditCallback = true;
    updateFullValue( value );
    inFullValueEditCallback = false;

}

//=========================================================

void localBrightnessContrast::updateBrightness( double val )
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

void localBrightnessContrast::updateGradient( double val )
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

void localBrightnessContrast::updateZeroValue( unsigned int val )
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

void localBrightnessContrast::updateFullValue( unsigned int val )
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

void localBrightnessContrast::updateZeroValueFullValue( unsigned int min, unsigned int max )
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

void localBrightnessContrast::updateBrightnessInterface()
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
        brightnessLineEdit->setText( QString( "%1" ).arg( (int)(brightness*100) ));
    }

    if( !inBrightnessSliderCallback )
    {
        brightnessSlider->setValue( brightness*100 );
    }

    nonInteractive = false;
}

void localBrightnessContrast::updateGradientInterface()
{
    // Calculate gradient (derived)
    double gradient = (atan((double)range/(double)(fullValue-zeroValue))-GRADIENT_BASE)*GRADIENT_USER_SCALE_FACTOR;

    // Update interface
    nonInteractive = true;

    if( !inGradientEditCallback )
    {
        gradientLineEdit->setText( QString( "%1" ).arg( (int)(gradient) ));
    }

    if( !inGradientSliderCallback )
    {
        gradientSlider->setValue( gradient );
    }

    nonInteractive = false;
}

void localBrightnessContrast::updateZeroValueInterface()
{
    // Update interface
    nonInteractive = true;

    if( !inZeroValueEditCallback )
    {
        zeroValueLineEdit->setText( QString( "%1" ).arg( zeroValue ));
    }

    if( !inZeroValueSliderCallback )
    {
        zeroValueSlider->setValue( zeroValue );
    }

    nonInteractive = false;
}

void localBrightnessContrast::updateFullValueInterface()
{
    // Update interface
    nonInteractive = true;

    if( !inFullValueEditCallback )
    {
        fullValueLineEdit->setText( QString( "%1" ).arg( fullValue ));
    }

    if( !inFullValueSliderCallback )
    {
        fullValueSlider->setValue( fullValue );
    }

    nonInteractive = false;
}

//=========================================================

void localBrightnessContrast::setStatistics( unsigned int minPIn, unsigned int maxPIn, unsigned int bitDepth, unsigned int binsIn[HISTOGRAM_BINS], rgbPixel pixelLookupIn[256] )
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

    zeroValueValidator->setTop( range-1 );
    fullValueValidator->setTop( range );

    histXLabel->setText( QString( "%1" ).arg( range ) );

    hist->update();
}

histogram::histogram( QWidget* parent, localBrightnessContrast* lbcIn )
    : QFrame( parent )
{
    setFrameStyle( QFrame::Panel );
    setMinimumWidth(256 );
    setMinimumHeight(200 );

    lbc = lbcIn;
}

// Histogram resize event
void histogram::resizeEvent( QResizeEvent* )
{
    // Keep the X asis label in the bottom right of the histogram
    lbc->histXLabel->setGeometry( width()-lbc->histXLabel->width()-2,
                                  height()-lbc->histXLabel->height()-10, //SCALE_HEIGHT,
                                  lbc->histXLabel->width(),
                                  lbc->histXLabel->height());
}


void histogram::paintEvent(QPaintEvent* )
{

    // Do nothing if no image info yet
    if( lbc->bins == NULL )
    {
        return;
    }

    // Determine range (ignore counts in first and last buckets as it is common for huge counts in one or both ends)
    unsigned int binRange = 0;
    for( int i = 1; i < HISTOGRAM_BINS-1; i++ )
    {
        if( lbc->bins[i] > binRange )
        {
            binRange = lbc->bins[i];
        }
    }

    // Do nothing if no data present
    if( binRange == 0 )
    {
        return;
    }


    QPainter p( this );
    QPoint pnt1;
    QPoint pnt2;

    // Draw the histogram
    int h = height()-1-SCALE_HEIGHT;
    pnt1 = QPoint( 0, h - lbc->bins[0]*h/binRange );

    p.setPen( Qt::red );
    for( int i = 1; i < HISTOGRAM_BINS-1; i++ )
    {
        pnt2.setX( i );
        pnt2.setY( h - lbc->bins[i]*h/binRange );
        p.drawLine( pnt1, pnt2 );
        pnt1 = pnt2;
    }

    // Prepare to draw the bounds an gradient
    unsigned int bitsPerBin = (lbc->depth<=8)?1:lbc->depth-8;
    unsigned int minBin = lbc->zeroValue>>(bitsPerBin-1);
    unsigned int maxBin = lbc->fullValue>>(bitsPerBin-1);

    QPen pen( Qt::blue );
    p.setPen( pen );

    // Draw max and min
    pen.setStyle( Qt::DashLine );
    p.drawLine( minBin,0,minBin,h);
    p.drawLine( maxBin,0,maxBin,h);

    // Draw gradient
    pen.setStyle( Qt::SolidLine );
    p.setPen( pen );

    p.drawLine( minBin,h,maxBin,0);

    // Draw the intensity / color bar
    pnt1.setY( h+2 );
    pnt2.setY( h + SCALE_HEIGHT -4 );
    for( unsigned int i = 1; i < HISTOGRAM_BINS-1; i++ )
    {
        // Set X to the next point in the histogram
        pnt1.setX( i );
        pnt2.setX( i );

        // Select the lookup index
        unsigned char index;
        if( i < minBin )
        {
            index = 0;
        }
        else if( i > maxBin )
        {
            index = 255;
        }
        else
        {
            index = (i-minBin)*256/(maxBin-minBin);
        }

        // draw the next line in the scale
        localBrightnessContrast::rgbPixel* col = &(lbc->pixelLookup[index] );
        p.setPen( QColor( col->p[2], col->p[1], col->p[0] ) );
        p.drawLine( pnt1, pnt2 );
    }
}
