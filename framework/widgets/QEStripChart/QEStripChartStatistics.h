/*  QEStripChartStatistics.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
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
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 *
 */

#ifndef QESTRIPCHARTSTATISTICS_H
#define QESTRIPCHARTSTATISTICS_H

#include <QString>
#include <QWidget>
#include <QCaDataPoint.h>

namespace Ui {
class QEStripChartStatistics;
}

class QEStripChartStatistics : public QWidget
{
   Q_OBJECT
   
public:
   explicit QEStripChartStatistics (const QString& pvName,
                                    const QString& egu,
                                    const QCaDataPointList& dataList,
                                    QWidget *parent = 0);
   ~QEStripChartStatistics();
   
public slots:
   void processDataList (const QCaDataPointList& dataList);

private:
   void clearLabels ();
   Ui::QEStripChartStatistics *ui;
   QString pvName;
   QString egu;
};

#endif // QESTRIPCHARTSTATISTICS_H
