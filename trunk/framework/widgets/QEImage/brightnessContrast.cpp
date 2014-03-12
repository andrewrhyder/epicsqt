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

localBrightnessContrast::localBrightnessContrast()
{
    nonInteractive = false;

    brightness = 100;
    contrast = 100;
    gradient = 255;
    zeroValue = 0;
    fullValue = 255;
    range = 255;


    // Initialise image stats
    bins.resize(256);
    bins.clear();
    maxP = 0;
    minP = UINT_MAX;

    setFrameStyle( QFrame::StyledPanel|QFrame::Raised );

    QGridLayout* brightnessContrastMainLayout = new QGridLayout();
    brightnessContrastMainLayout->setSpacing( 10 );
    setLayout( brightnessContrastMainLayout );

    QHBoxLayout* brightnessContrastSub1Layout = new QHBoxLayout();
    QGridLayout* brightnessContrastSub2Layout = new QGridLayout();

    QLabel* brightnessLabel = new QLabel( "Brightness:", this );
    QLabel* contrastLabel = new QLabel( "Contrast:", this );
    QLabel* minLabel = new QLabel( "Minimum:", this );
    QLabel* maxLabel = new QLabel( "Maximum:", this );
    QLabel* gradientLabel = new QLabel( "Gradient:", this );

    autoBrightnessCheckBox = new QCheckBox( "Auto Brightness and Contrast", this );
    autoBrightnessCheckBox->setToolTip( "Set brightness and contrast to use the full dynamic range of an area when an area is selected");

    QPushButton* autoImageButton = new QPushButton( "Auto all", this );
    autoImageButton->setToolTip( "Set brightness and contrast to use the full dynamic range for the entire image");
    QObject::connect( autoImageButton, SIGNAL( clicked ( bool ) ), this,  SLOT  ( brightnessContrastAutoImageClicked()) );

    QPushButton* resetButton = new QPushButton( "Reset", this );
    resetButton->setToolTip( "Reset brightness and contrast");
    QObject::connect( resetButton, SIGNAL( clicked ( bool ) ), this,  SLOT  ( brightnessContrastResetClicked( bool )) );

    brightnessSlider = new QSlider( Qt::Horizontal, this );
    brightnessSlider->setMinimum( -100 );
    brightnessSlider->setMaximum( 100 );
    brightnessSlider->setValue( 0 );
    QObject::connect( brightnessSlider, SIGNAL( valueChanged ( int ) ), this,  SLOT  ( brightnessSliderValueChanged( int )) );

    contrastSlider = new QSlider( Qt::Horizontal, this );
    contrastSlider->setMinimum( 0 );
    contrastSlider->setMaximum( 1000 );
    contrastSlider->setValue( 100 );
    QObject::connect( contrastSlider, SIGNAL( valueChanged ( int ) ), this,  SLOT  ( contrastSliderValueChanged( int )) );

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

    gradientSlider = new QSlider( Qt::Horizontal, this );
    gradientSlider->setMinimum( 0 );
    gradientSlider->setMaximum( 1000 );
    gradientSlider->setValue( 1000 );
    QObject::connect( gradientSlider, SIGNAL( valueChanged ( int ) ), this,  SLOT  ( gradientSliderValueChanged( int )) );

    hist = new histogram( this, this );

    brightnessRBLabel = new QLabel( this );
    brightnessRBLabel->setText( QString( "%1%" ).arg( brightnessSlider->value() ) );

    contrastRBLabel = new QLabel( this );
    contrastRBLabel->setText( QString( "%1%" ).arg( contrastSlider->value() ) );

    minRBLabel = new QLabel( this );
    minRBLabel->setText( QString( "%1" ).arg( minSlider->value() ) );

    maxRBLabel = new QLabel( this );
    maxRBLabel->setText( QString( "%1" ).arg( maxSlider->value() ) );

    gradientRBLabel = new QLabel( this );
    gradientRBLabel->setText( QString( "%1" ).arg( gradientSlider->value() ) );

    contrastReversalCheckBox = new QCheckBox( "Contrast Reversal", this );
    contrastReversalCheckBox->setToolTip( "Reverse light for dark");
    QObject::connect( contrastReversalCheckBox, SIGNAL( toggled( bool ) ), this,  SLOT  ( contrastReversalToggled( bool )) );

    brightnessContrastSub1Layout->addWidget( autoBrightnessCheckBox, 0, Qt::AlignLeft );
    brightnessContrastSub1Layout->addWidget( autoImageButton, 0, Qt::AlignLeft );
    brightnessContrastSub1Layout->addWidget( resetButton, 1, Qt::AlignLeft );

    brightnessContrastSub2Layout->addWidget( brightnessLabel, 0, 0 );
    brightnessContrastSub2Layout->addWidget( brightnessSlider, 0, 1 );
    brightnessContrastSub2Layout->addWidget( brightnessRBLabel, 0, 2 );

    brightnessContrastSub2Layout->addWidget( contrastLabel, 1, 0 );
    brightnessContrastSub2Layout->addWidget( contrastSlider, 1, 1 );
    brightnessContrastSub2Layout->addWidget( contrastRBLabel, 1, 2 );

    brightnessContrastSub2Layout->addWidget( minLabel, 2, 0 );
    brightnessContrastSub2Layout->addWidget( minSlider, 2, 1 );
    brightnessContrastSub2Layout->addWidget( minRBLabel, 2, 2 );

    brightnessContrastSub2Layout->addWidget( maxLabel, 3, 0 );
    brightnessContrastSub2Layout->addWidget( maxSlider, 3, 1 );
    brightnessContrastSub2Layout->addWidget( maxRBLabel, 3, 2 );

    brightnessContrastSub2Layout->addWidget( gradientLabel, 4, 0 );
    brightnessContrastSub2Layout->addWidget( gradientSlider, 4, 1 );
    brightnessContrastSub2Layout->addWidget( gradientRBLabel, 4, 2 );

    brightnessContrastSub2Layout->setColumnStretch( 1, 1 );  // Read back labels to take all spare room

    brightnessContrastMainLayout->addLayout( brightnessContrastSub1Layout, 0, 0 );
    brightnessContrastMainLayout->addLayout( brightnessContrastSub2Layout, 1, 0 );

    brightnessContrastMainLayout->addWidget( contrastReversalCheckBox, 2, 0 );
    brightnessContrastMainLayout->addWidget( hist, 0, 1, 3, 1 );

    brightnessSlider->setValue( 0 );    // Range -100% (black) to +100% (white)
    contrastSlider->setValue( 100 );    // Range 0% (no difference in any pixels) to 1000% (10 times normal contrast)

    bins.resize(256);
    range = 0;

    adjustSize();
}

localBrightnessContrast::~localBrightnessContrast()
{
    delete autoBrightnessCheckBox;
    delete brightnessSlider;
    delete contrastSlider;
    delete minSlider;
    delete maxSlider;
    delete gradientSlider;
    delete brightnessRBLabel;
    delete contrastRBLabel;
    delete minRBLabel;
    delete maxRBLabel;
    delete gradientRBLabel;
    delete contrastReversalCheckBox;
    delete hist;
}

int localBrightnessContrast::getBrightness()
{
    return brightnessSlider->value();

}

int localBrightnessContrast::getContrast()
{
    return contrastSlider->value();
}

bool localBrightnessContrast::getAutoBrightnessContrast()
{
    return autoBrightnessCheckBox->isChecked();
}

bool localBrightnessContrast::getContrastReversal()
{
    return contrastReversalCheckBox->isChecked();
}

// Reset the brightness and contrast to normal
void localBrightnessContrast::brightnessContrastResetClicked( bool )
{
    // Reset brightness and contrast
    brightnessSlider->setValue( 0 );
    contrastSlider->setValue( 100 );
}

// Auto brightness and contrast check box has ben checked or unchecked
void localBrightnessContrast::brightnessContrastAutoImageClicked()
{
    emit brightnessContrastAutoImage();
}

// Contrast reversal check box has ben checked or unchecked
void localBrightnessContrast::contrastReversalToggled( bool )
{
    emit brightnessContrastChange();
}

// The local brightness slider has been moved
void localBrightnessContrast::brightnessSliderValueChanged( int localBrightnessIn )
{
    // Update the brightness
    brightnessRBLabel->setText( QString( "%1%" ).arg( localBrightnessIn ));

    if( nonInteractive )
    {
        return;
    }

    emit brightnessContrastChange();

}

// The local contrast slider has been moved
void localBrightnessContrast::contrastSliderValueChanged( int localContrastIn )
{
    // Update the contrast
    contrastRBLabel->setText( QString( "%1%" ).arg( localContrastIn ));

    if( nonInteractive )
    {
        return;
    }

    emit brightnessContrastChange();

}

// Set brightness and contrast based on a values for black and white
void localBrightnessContrast::setBrightnessContrast( const unsigned int max, const unsigned int min, const unsigned int highest, const QVector<unsigned int>& bins )
{
    // Range of pixel values in area
    int range = (max>min)?max-min:1;

    // Calculate the contrast that will set the dynamic range
    // to match the range of pixels in the area.
    double newContrastDouble = (double)(highest)/(double)(range);

    // Calculate the brightness that will set the dynamic range
    // to match the range of pixels in the area.
    // Offset from mid pixel value of range to mid pixel value in area (in original pixel scale) scaled for new contrast...
    double midOffset = (((double)(highest)/2)-((double)(min+max)/2))*newContrastDouble;

    // Calculate brightness that will offset pixel values in the selected area to use full range.
    // Note, when used, the brightness will be multiplied by (the new pixel range - an offset used to center the new pixel range )
    double newBrightnessDouble = midOffset/(highest*(newContrastDouble-(newContrastDouble-1)/2));

    // Set the brightness and constrast sliders
    setBrightnessContrast( newBrightnessDouble*100.0, newContrastDouble*100.0);
}

void localBrightnessContrast::setBrightnessContrast( int brightness, int contrast )
{
    brightnessSlider->setValue( brightness );
    contrastSlider->setValue( contrast );
}

void localBrightnessContrast::setAutoBrightnessContrast( bool autoBrightnessContrast )
{
    autoBrightnessCheckBox->setChecked( autoBrightnessContrast );
}

void localBrightnessContrast::setContrastReversal( bool contrastReversal )
{
    contrastReversalCheckBox->setChecked( contrastReversal );
}

// The minimum slider has been moved
void localBrightnessContrast::minSliderValueChanged( int value )
{
    if( nonInteractive )
    {
        return;
    }

    // Update the minimum
    zeroValue = value*range/minSlider->maximum();
    minRBLabel->setText( QString( "%1" ).arg( zeroValue ));

    if( range && (fullValue <= zeroValue) )
    {
        fullValue = zeroValue+1;
        maxRBLabel->setText( QString( "%1" ).arg( fullValue ));
        nonInteractive = true;
        maxSlider->setValue(fullValue*maxSlider->maximum()/range);
        nonInteractive = false;
    }

    if( range )
    {
        gradient = (fullValue-zeroValue)*gradientSlider->maximum()/range;
        gradientRBLabel->setText( QString( "%1" ).arg( gradient ));
        nonInteractive = true;
        gradientSlider->setValue( gradient );
        nonInteractive = false;
    }

    if( range )
    {
        contrast = (fullValue-zeroValue)/range;
    }
    hist->update();
}

// The maximum slider has been moved
void localBrightnessContrast::maxSliderValueChanged( int value )
{
    if( nonInteractive )
    {
        return;
    }

    // Update the maximum
    fullValue = value*range/maxSlider->maximum();
    maxRBLabel->setText( QString( "%1" ).arg( fullValue ));
    if( range && ( zeroValue >= fullValue ))
    {
        zeroValue = fullValue-1;
        minRBLabel->setText( QString( "%1" ).arg( zeroValue ));
        nonInteractive = true;
        minSlider->setValue(zeroValue*minSlider->maximum()/range);
        nonInteractive = false;
    }

    if( range )
    {
        gradient = (fullValue-zeroValue)*gradientSlider->maximum()/range;
        gradientRBLabel->setText( QString( "%1" ).arg( gradient ));
        nonInteractive = true;
        gradientSlider->setValue( gradient );
        nonInteractive = false;
    }

    hist->update();
}

// The gradient slider has been moved
void localBrightnessContrast::gradientSliderValueChanged( int value )
{
    if( nonInteractive )
    {
        return;
    }

    // Update the gradient
    gradient = value;
    gradientRBLabel->setText( QString( "%1" ).arg( gradient ));
    hist->update();

    unsigned int midValue = (fullValue+zeroValue)/2;
    unsigned int span = range*value/gradientSlider->maximum();
    zeroValue = midValue-span/2;
    fullValue = zeroValue+span;

    minRBLabel->setText( QString( "%1" ).arg( zeroValue ));
    maxRBLabel->setText( QString( "%1" ).arg( fullValue ));

    nonInteractive = true;
    minSlider->setValue( zeroValue );
    maxSlider->setValue( fullValue );

    nonInteractive = false;
}

void localBrightnessContrast::setStatistics( unsigned int minPIn, unsigned int maxPIn, unsigned int bitDepth, QVector<unsigned int> binsIn )
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
    // Determine range (ignore counts in first and last buckets as it is common for huge counts in one or both ends)
    int binRange = 0;
    int count = lbc->bins.size();
    for( int i = 1; i < count-1; i++ )
    {
        if( lbc->bins.at(i) > binRange )
        {
            binRange = lbc->bins.at(i);
        }
    }
    if( binRange == 0 )
    {
        return;
    }
    int h = height()-1;
    QPoint pnt1(0,h - lbc->bins.at(0)*h/binRange);
    QPoint pnt2;
    QPainter p( this );
    p.setPen(Qt::red);
    for( int i = 1; i < count; i++ )
    {
        pnt2.setX(i);
        pnt2.setY(h - lbc->bins.at(i)*h/binRange );
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
