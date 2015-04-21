/*  QEWaveformHistogram.h
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
 *  Copyright (c) 2014 Australian Synchrotron.
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef QE_WAVEFORM_HISTOGRAM_H
#define QE_WAVEFORM_HISTOGRAM_H

#include <QLayout>
#include <QString>
#include <QVector>

#include <QECommon.h>
#include <QEHistogram.h>
#include <QEFrame.h>
#include <QEWidget.h>
#include <QEFloating.h>
#include <QEFloatingFormatting.h>
#include <QCaVariableNamePropertyManager.h>
#include <QEPluginLibrary_global.h>

/// The QEWaveformHistogram class is a EPICS aware histogram widget.
/// The value of, i.e. the length of each bar of the histogram is controlled by
/// the corresponding element of an array, e.g. waveform, process variable.
/// When the variable is defined (connected), the bar lengths are updated, and
/// optionally the bar colours set to reflect the variable's severity status.
/// The bar is 'grayed-out' when the variable is disconnected (although the bars
/// retains their last known values/lengths).
/// The histogram nature of the this widget is provided by a QEHistogram widget.
/// The QEWaveformHistogram widget is tighly integrated with the base class QEWidget,
/// via QEFrame, which provides generic support such as macro substitutions,
/// drag/drop, and standard properties.
///
class QEPLUGINLIBRARYSHARED_EXPORT QEWaveformHistogram:public QEFrame {
   Q_OBJECT

   // All standard properties are inherited from QEFrame.
   //
   Q_PROPERTY (QString variable READ getVariableNameProperty WRITE setVariableNameProperty)
   Q_PROPERTY (QString variableSubstitutions READ getPvNameSubstitutions WRITE
               setPvNameSubstitutions)

public:
   /// \enum ScaleModes
   enum ScaleModes {
      Manual,              ///< Use property minimum/maximum to scale histogram
      Auto,                ///< Dynamically scale based on minimum/maximum displayed value
      OperationalRange     ///< Use process variable operational range (LOPR/HOPR).
   };

   Q_ENUMS (ScaleModes)

   // Histogram properties
   //
   Q_PROPERTY (bool   autoBarGapWidths READ getAutoBarGapWidths WRITE setAutoBarGapWidths)
   Q_PROPERTY (int    barWidth         READ getBarWidth         WRITE setBarWidth)
   Q_PROPERTY (int    gap              READ getGap              WRITE setGap)
   Q_PROPERTY (ScaleModes scaleMode    READ getScaleMode        WRITE setScaleMode)
   Q_PROPERTY (double minimum          READ getMinimum          WRITE setMinimum)
   Q_PROPERTY (double maximum          READ getMaximum          WRITE setMaximum)
   Q_PROPERTY (double baseLine         READ getBaseLine         WRITE setBaseLine)
   Q_PROPERTY (bool   logScale         READ getLogScale         WRITE setLogScale)
   // When dislayAlarmState set ture, this property value effectively ignored.
   Q_PROPERTY (QColor barColour        READ getBarColour        WRITE setBarColour)
   Q_PROPERTY (bool   drawBorder       READ getDrawBorder       WRITE setDrawBorder)
   Q_PROPERTY (Qt::Orientation orientation READ getOrientation  WRITE setOrientation)

public:
   explicit QEWaveformHistogram (QWidget* parent = 0);
   ~QEWaveformHistogram () { }

   void setScaleMode (const ScaleModes scaleMode);
   ScaleModes getScaleMode () const;

   // Expose access to the internal widget's set/get functions.
   //
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (histogram, double, setMinimum, getMinimum)
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (histogram, double, setMaximum, getMaximum)
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (histogram, double, setBaseLine,  getBaseLine)
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (histogram, int,    setGap, getGap)
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (histogram, int,    setBarWidth, getBarWidth)
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (histogram, bool,   setAutoBarGapWidths, getAutoBarGapWidths)
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (histogram, bool,   setLogScale, getLogScale)
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (histogram, bool,   setDrawBorder, getDrawBorder)
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (histogram, QColor, setBarColour, getBarColour)
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (histogram, Qt::Orientation, setOrientation, getOrientation)

protected:
   qcaobject::QCaObject* createQcaItem (unsigned int variableIndex);
   void establishConnection (unsigned int variableIndex);
   bool eventFilter (QObject* obj, QEvent* event);

   // Drag and Drop
   void dragEnterEvent (QDragEnterEvent* event) { qcaDragEnterEvent (event);  }
   void dropEvent (QDropEvent* event)           { qcaDropEvent (event);       }
   void mousePressEvent (QMouseEvent* event)    { qcaMousePressEvent (event); }
   void setDrop (QVariant drop);
   QVariant getDrop ();

   // Copy paste
   QString copyVariable ();
   QVariant copyData ();

private:
   void setVariableNameProperty (const QString& variableName);
   QString getVariableNameProperty ();
   void setPvNameSubstitutions (const QString& subs);
   QString getPvNameSubstitutions () const;
   void updateHistogramScale ();
   void setReadOut (const QString& text);
   void genReadOut (const int index);

   QEHistogram* histogram;
   QHBoxLayout* layout;         // holds the histogram - any layout type will do
   QCaVariableNamePropertyManager vnpm;
   QEFloatingFormatting floatingFormatting;
   int selectedChannel;         //
   ScaleModes mScaleMode;
   bool isFirstUpdate;

private slots:
   void newVariableNameProperty (QString pvName, QString subs, unsigned int variableIndex);

   void connectionChanged (QCaConnectionInfo& connectionInfo, const unsigned int&variableIndex);

   void setChannelArrayValue (const QVector <double>& value, QCaAlarmInfo&,
                              QCaDateTime&, const unsigned int&);
};

#endif                          // QE_WAVEFORM_HISTOGRAM_H