
#include "profilePlot.h"


profilePlot::profilePlot(QWidget *parent)
    : QwtPlot(parent)
{
    enableAxis( xBottom, false );
    enableAxis( yLeft, false );

    curve = new QwtPlotCurve();
    curve->setRenderHint( QwtPlotItem::RenderAntialiased );
    curve->attach(this);

}

profilePlot::~profilePlot()
{
    delete curve;
}

void profilePlot::setScale( int scaleIn )
{
    scale = scaleIn;
}

void profilePlot::setProfile( QVector<QPointF>& profile, double maxX, double maxY )
{
    // Set the curve data
#if QWT_VERSION >= 0x060000
    curve->setSamples( profile );
#else
    curve->setData( profile );
#endif
    setAxisScale( xBottom, 0, maxX );
    setAxisScale( yLeft, 0, maxY );

    // Update the plot
    replot();
}

void profilePlot::setCursor( int cursorIn )
{
    cursor = cursorIn;
}

