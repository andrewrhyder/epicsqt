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

localBrightnessContrast::localBrightnessContrast()
{
    nonInteractive = false;

    inBrightnessCallback = false;
    inGradientCallback = false;
    inZeroValueCallback = false;
    inFullValueCallback = false;

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
    gradientSlider->setMinimum( 1 );
    gradientSlider->setMaximum( 255 );
    gradientSlider->setValue( 1 );
    QObject::connect( gradientSlider, SIGNAL( valueChanged ( int ) ), this,  SLOT  ( gradientSliderValueChanged( int )) );

    minSlider = new QSlider( Qt::Horizontal, this );
    minSlider->setMinimum( 0 );
    minSlider->setMaximum( 255 );
    minSlider->setValue( 0 );
    QObject::connect( minSlider, SIGNAL( valueChanged ( int ) ), this,  SLOT  ( minSliderValueChanged( int )) );

    maxSlider = new QSlider( Qt::Horizontal, this );
    maxSlider->setMinimum( 0 );
    maxSlider->setMaximum( 255 );
    maxSlider->setValue( 255 );
    QObject::connect( maxSlider, SIGNAL( valueChanged ( int ) ), this,  SLOT  ( maxSliderValueChanged( int )) );

    hist = new histogram( this, this );

    brightnessRBLabel = new QLabel( this );
    brightnessRBLabel->setText( QString( "%1%" ).arg( brightnessSlider->value() ) );

    brightnessRBLabel->setMinimumWidth( 100 ); // Set width for all readbacks

    gradientRBLabel = new QLabel( this );
    gradientRBLabel->setText( QString( "%1" ).arg( gradientSlider->value() ) );

    minRBLabel = new QLabel( this );
    minRBLabel->setText( QString( "%1" ).arg( minSlider->value() ) );

    maxRBLabel = new QLabel( this );
    maxRBLabel->setText( QString( "%1" ).arg( maxSlider->value() ) );

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
    brightnessContrastSub2Layout->addWidget( brightnessRBLabel, 0, 2 );

    brightnessContrastSub2Layout->addWidget( gradientLabel,     1, 0 );
    brightnessContrastSub2Layout->addWidget( gradientSlider,    1, 1 );
    brightnessContrastSub2Layout->addWidget( gradientRBLabel,   1, 2 );

    brightnessContrastSub2Layout->addWidget( minLabel,          2, 0 );
    brightnessContrastSub2Layout->addWidget( minSlider,         2, 1 );
    brightnessContrastSub2Layout->addWidget( minRBLabel,        2, 2 );

    brightnessContrastSub2Layout->addWidget( maxLabel,          3, 0 );
    brightnessContrastSub2Layout->addWidget( maxSlider,         3, 1 );
    brightnessContrastSub2Layout->addWidget( maxRBLabel,        3, 2 );

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
    delete minSlider;
    delete maxSlider;
    delete gradientSlider;
    delete brightnessRBLabel;
    delete minRBLabel;
    delete maxRBLabel;
    delete gradientRBLabel;
    delete contrastReversalCheckBox;
    delete logCheckBox;
    delete falseColourCheckBox;

    delete hist;
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
    qDebug() << range;
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
    logCheckBox->setChecked( falseColour );
}

//==========================================================

// The local brightness slider has been moved
void localBrightnessContrast::brightnessSliderValueChanged( int localBrightnessIn )
{
    if( nonInteractive )
    {
        return;
    }

    inBrightnessCallback = true;
    updateBrightness( (double)localBrightnessIn/100.0 );
    inBrightnessCallback = false;

    emit brightnessContrastChange();
}

// The gradient slider has been moved
void localBrightnessContrast::gradientSliderValueChanged( int value )
{
    if( nonInteractive )
    {
        return;
    }

    inGradientCallback = true;
    updateGradient( (double)value );
    inGradientCallback = false;

    emit brightnessContrastChange();
}

// The minimum slider has been moved
void localBrightnessContrast::minSliderValueChanged( int value )
{
    if( nonInteractive )
    {
        return;
    }

    inZeroValueCallback = true;
    updateZeroValue( value );
    inZeroValueCallback = false;

    emit brightnessContrastChange();
}

// The maximum slider has been moved
void localBrightnessContrast::maxSliderValueChanged( int value )
{
    if( nonInteractive )
    {
        return;
    }

    inFullValueCallback = true;
    updateFullValue( value );
    inFullValueCallback = false;

    emit brightnessContrastChange();
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
    val = tan( val/1000 );
    qDebug() << "before" << "val"<<val << "zeroValue"<<zeroValue<<"fullValue"<<fullValue;
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

    qDebug() << "after" << "val"<<val << "zeroValue"<<zeroValue<<"fullValue"<<fullValue<<"mid"<<mid<<"span"<<span;

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

    brightnessRBLabel->setText( QString( "%1" ).arg( (int)(brightness*100) ));
    if( !inBrightnessCallback )
    {
        nonInteractive = true;
        brightnessSlider->setValue( brightness*100 );
        nonInteractive = false;
    }
}

void localBrightnessContrast::updateGradientInterface()
{
    gradientSlider->setMinimum( 785 );  // 1000*pi/4
    gradientSlider->setMaximum( 1570 ); // 1000*pi/2
    double gradient = atan((double)range/(double)(fullValue-zeroValue))*1000;

    gradientRBLabel->setText( QString( "%1" ).arg( ((int)(gradient)-785)*1000/785 ));
    if( !inGradientCallback )
    {
        nonInteractive = true;
        gradientSlider->setValue( gradient );
        nonInteractive = false;
    }
}

void localBrightnessContrast::updateZeroValueInterface()
{
    minRBLabel->setText( QString( "%1" ).arg( zeroValue ));
    if( !inZeroValueCallback )
    {
        nonInteractive = true;
        minSlider->setValue( zeroValue );
        nonInteractive = false;
    }
}

void localBrightnessContrast::updateFullValueInterface()
{
    maxRBLabel->setText( QString( "%1" ).arg( fullValue ));
    if( !inFullValueCallback )
    {
        nonInteractive = true;
        maxSlider->setValue( fullValue );
        nonInteractive = false;
    }
}

//=========================================================

void localBrightnessContrast::setStatistics( unsigned int minPIn, unsigned int maxPIn, unsigned int bitDepth, unsigned int binsIn[HISTOGRAM_BINS] )
{
    // Update image statistics
    minP = minPIn;
    maxP = maxPIn;
    depth = bitDepth;
    bins = binsIn;

    // Recalculate dependand variables
    range = (1<<depth)-1;

    // Apply changes
    minSlider->setMaximum( range );
    maxSlider->setMaximum( range );

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
    if( binRange == 0 )
    {
        return;
    }
    int h = height()-1;
    QPoint pnt1(0,h - lbc->bins[0]*h/binRange);
    QPoint pnt2;
    QPainter p( this );
    p.setPen(Qt::red);
    for( int i = 1; i < HISTOGRAM_BINS-1; i++ )
    {
        pnt2.setX(i);
        pnt2.setY(h - lbc->bins[i]*h/binRange );
        p.drawLine(pnt1,pnt2);
        pnt1=pnt2;
    }
    unsigned int bitsPerBin = (lbc->depth<=8)?1:lbc->depth-8;
    unsigned int minBin = lbc->zeroValue>>(bitsPerBin-1);
    unsigned int maxBin = lbc->fullValue>>(bitsPerBin-1);

    p.setPen(Qt::blue);

    p.drawLine( minBin,0,minBin,h);
    p.drawLine( maxBin,0,maxBin,h);
    p.drawLine( minBin,h,maxBin,0);
}
