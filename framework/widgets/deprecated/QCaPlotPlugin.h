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

#include <QTimer>
#include <QString>
#include <QCaPlot.h>
#include <QCaStringFormatting.h>

class QCaPlotPlugin : public QCaPlot {
    Q_OBJECT

  public:
    /// Constructors
    QCaPlotPlugin( QWidget *parent = 0 );
    QCaPlotPlugin( QString variableName, QWidget *parent = 0 );

    // Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
    // A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
    Q_PROPERTY(QString variable1 READ getVariableName1Property WRITE setVariableName1Property)
    void    setVariableName1Property( QString variable1Name ){ variableNamePropertyManagers[0].setVariableNameProperty( variable1Name ); }
    QString getVariableName1Property(){ return variableNamePropertyManagers[0].getVariableNameProperty(); }

    Q_PROPERTY(QString variable2 READ getVariableName2Property WRITE setVariableName2Property)
    void    setVariableName2Property( QString variable2Name ){ variableNamePropertyManagers[1].setVariableNameProperty( variable2Name ); }
    QString getVariableName2Property(){ return variableNamePropertyManagers[1].getVariableNameProperty(); }

    Q_PROPERTY(QString variable3 READ getVariableName3Property WRITE setVariableName3Property)
    void    setVariableName3Property( QString variable3Name ){ variableNamePropertyManagers[2].setVariableNameProperty( variable3Name ); }
    QString getVariableName3Property(){ return variableNamePropertyManagers[2].getVariableNameProperty(); }

    Q_PROPERTY(QString variable4 READ getVariableName4Property WRITE setVariableName4Property)
    void    setVariableName4Property( QString variable4Name ){ variableNamePropertyManagers[3].setVariableNameProperty( variable4Name ); }
    QString getVariableName4Property(){ return variableNamePropertyManagers[3].getVariableNameProperty(); }

    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)
    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions ){ for( int i = 0; i < QCAPLOT_NUM_VARIABLES; i++ ) variableNamePropertyManagers[i].setSubstitutionsProperty( variableNameSubstitutions ); }
    QString getVariableNameSubstitutionsProperty(){ return variableNamePropertyManagers[0].getSubstitutionsProperty(); }


    Q_PROPERTY(bool variableAsToolTip READ getVariableAsToolTip WRITE setVariableAsToolTip)
    Q_PROPERTY(bool allowDrop READ getAllowDrop WRITE setAllowDrop)

    Q_PROPERTY(QColor traceColor1 READ getTraceColor1 WRITE setTraceColor1)
    Q_PROPERTY(QColor traceColor2 READ getTraceColor2 WRITE setTraceColor2)
    Q_PROPERTY(QColor traceColor3 READ getTraceColor3 WRITE setTraceColor3)
    Q_PROPERTY(QColor traceColor4 READ getTraceColor4 WRITE setTraceColor4)

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


    Q_PROPERTY(QString traceLegend1 READ getTraceLegend1 WRITE setTraceLegend1)
    Q_PROPERTY(QString traceLegend2 READ getTraceLegend2 WRITE setTraceLegend2)
    Q_PROPERTY(QString traceLegend3 READ getTraceLegend3 WRITE setTraceLegend3)
    Q_PROPERTY(QString traceLegend4 READ getTraceLegend4 WRITE setTraceLegend4)

    Q_PROPERTY(double yMin READ getYMin WRITE setYMin)
    Q_PROPERTY(double yMax READ getYMax WRITE setYMax)

    Q_PROPERTY(bool autoScale READ getAutoScale WRITE setAutoScale)

    Q_PROPERTY(bool axisEnableX READ getAxisEnableX WRITE setAxisEnableX)
    Q_PROPERTY(bool axisEnableY READ getAxisEnableY WRITE setAxisEnableY)

    Q_PROPERTY(bool gridEnableMajorX READ getGridEnableMajorX WRITE setGridEnableMajorX)
    Q_PROPERTY(bool gridEnableMajorY READ getGridEnableMajorY WRITE setGridEnableMajorY)

    Q_PROPERTY(bool gridEnableMinorX READ getGridEnableMinorX WRITE setGridEnableMinorX)
    Q_PROPERTY(bool gridEnableMinorY READ getGridEnableMinorY WRITE setGridEnableMinorY)

    Q_PROPERTY(QColor gridMajorColor READ getGridMajorColor WRITE setGridMajorColor)
    Q_PROPERTY(QColor gridMinorColor READ getGridMinorColor WRITE setGridMinorColor)

    Q_PROPERTY(QString title READ getTitle WRITE setTitle)
    Q_PROPERTY(QColor backgroundColor READ getBackgroundColor WRITE setBackgroundColor)
    Q_PROPERTY(QString xUnit READ getXUnit WRITE setXUnit)
    Q_PROPERTY(QString yUnit READ getYUnit WRITE setYUnit)
    Q_PROPERTY(double xStart READ getXStart WRITE setXStart)
    Q_PROPERTY(double xIncrement READ getXIncrement WRITE setXIncrement)
    Q_PROPERTY(unsigned int timeSpan READ getTimeSpan WRITE setTimeSpan)
    Q_PROPERTY(unsigned int tickRate READ getTickRate WRITE setTickRate)

  private:
    QCaVariableNamePropertyManager variableNamePropertyManagers[QCAPLOT_NUM_VARIABLES];

private slots:
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );

};

#endif /// QCAPLOTPLUGIN_H
