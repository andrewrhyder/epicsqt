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
 *  Copyright (c) 2009, 2010
 *
 *  Author:
 *    Glenn Jackson
 *  Contact details:
 *    glenn.jackson@synchrotron.org.au
 */

#ifndef QCAPLOTPLUGIN_H
#define QCAPLOTPLUGIN_H

#include <QEPlot.h>

/// QCaPlotPlugin IS deprecated - use QEPlot.
class QCaPlotPlugin : public QEPlot {
    Q_OBJECT

  public:
    QCaPlotPlugin( QWidget *parent = 0 ) : QEPlot( parent ) {}

    // This deprecated widget is now just a shell around its replacement.
    // Its replacement does not recognise any properties defined in using a Q_ENUM in the original widget,
    // so these properties must still be implemented in the deprecated widget
    Q_ENUMS(TraceStyles)
    Q_PROPERTY(TraceStyles traceStyle1 READ getTraceStyle1 WRITE setTraceStyle1)
    Q_PROPERTY(TraceStyles traceStyle2 READ getTraceStyle2 WRITE setTraceStyle2)
    Q_PROPERTY(TraceStyles traceStyle3 READ getTraceStyle3 WRITE setTraceStyle3)
    Q_PROPERTY(TraceStyles traceStyle4 READ getTraceStyle4 WRITE setTraceStyle4)
    enum TraceStyles { Lines  = QwtPlotCurve::Lines,
                       Sticks = QwtPlotCurve::Sticks,
                       Steps  = QwtPlotCurve::Steps,
                       Dots   = QwtPlotCurve::Dots };
    void setTraceStyle1( TraceStyles traceStyle ){ setTraceStyle( (QwtPlotCurve::CurveStyle)traceStyle, 0 ); }
    void setTraceStyle2( TraceStyles traceStyle ){ setTraceStyle( (QwtPlotCurve::CurveStyle)traceStyle, 1 ); }
    void setTraceStyle3( TraceStyles traceStyle ){ setTraceStyle( (QwtPlotCurve::CurveStyle)traceStyle, 2 ); }
    void setTraceStyle4( TraceStyles traceStyle ){ setTraceStyle( (QwtPlotCurve::CurveStyle)traceStyle, 3 ); }

    TraceStyles getTraceStyle1(){ return (TraceStyles)(getTraceStyle( 0 )); }
    TraceStyles getTraceStyle2(){ return (TraceStyles)(getTraceStyle( 1 )); }
    TraceStyles getTraceStyle3(){ return (TraceStyles)(getTraceStyle( 2 )); }
    TraceStyles getTraceStyle4(){ return (TraceStyles)(getTraceStyle( 3 )); }

};

#endif // QCAPLOTPLUGIN_H
