/*  $Id: QCaStripChart.h $
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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef QCASTRIPCHART_H
#define QCASTRIPCHART_H

#include <QObject>
#include <QColor>
#include <QFrame>
#include <QSize>
#include <QDateTime>
#include <QTimer>
#include <QVariant>
#include <QAction>

#include <qwt_plot_curve.h>

#include <QCaAlarmInfo.h>
#include <QCaPluginLibrary_global.h>
#include <QCaDateTime.h>
#include <QCaConnectionInfo.h>
#include <QCaDragDrop.h>
#include <QCaWidget.h>

class QCAPLUGINLIBRARYSHARED_EXPORT QCaStripChart : public QFrame, public QCaWidget {
   Q_OBJECT


   Q_PROPERTY (int     duration   READ getDuration               WRITE setDuration)
   Q_PROPERTY (double  yMinimum   READ getYMinimum               WRITE setYMinimum)
   Q_PROPERTY (double  yMaximum   READ getYMaximum               WRITE setYMaximum)


   // Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
   // A property name ending with 'Name' results in some sort of string a variable being displayed,
   // but will only accept alphanumeric and won't generate callbacks on change.
   //
   Q_PROPERTY (QString variable1  READ getPropertyVariableName1  WRITE setPropertyVariableName1)
   Q_PROPERTY (QString variable2  READ getPropertyVariableName2  WRITE setPropertyVariableName2)
   Q_PROPERTY (QString variable3  READ getPropertyVariableName3  WRITE setPropertyVariableName3)
   Q_PROPERTY (QString variable4  READ getPropertyVariableName4  WRITE setPropertyVariableName4)
   Q_PROPERTY (QString variable5  READ getPropertyVariableName5  WRITE setPropertyVariableName5)
   Q_PROPERTY (QString variable6  READ getPropertyVariableName6  WRITE setPropertyVariableName6)
   Q_PROPERTY (QString variable7  READ getPropertyVariableName7  WRITE setPropertyVariableName7)
   Q_PROPERTY (QString variable8  READ getPropertyVariableName8  WRITE setPropertyVariableName8)
   Q_PROPERTY (QString variable9  READ getPropertyVariableName9  WRITE setPropertyVariableName9)
   Q_PROPERTY (QString variable10 READ getPropertyVariableName10 WRITE setPropertyVariableName10)
   Q_PROPERTY (QString variable11 READ getPropertyVariableName11 WRITE setPropertyVariableName11)
   Q_PROPERTY (QString variable12 READ getPropertyVariableName12 WRITE setPropertyVariableName12)

   Q_PROPERTY (QColor  color1     READ getColor1Property         WRITE setColor1Property)
   Q_PROPERTY (QColor  color2     READ getColor2Property         WRITE setColor2Property)
   Q_PROPERTY (QColor  color3     READ getColor3Property         WRITE setColor3Property)
   Q_PROPERTY (QColor  color4     READ getColor4Property         WRITE setColor4Property)
   Q_PROPERTY (QColor  color5     READ getColor5Property         WRITE setColor5Property)
   Q_PROPERTY (QColor  color6     READ getColor6Property         WRITE setColor6Property)
   Q_PROPERTY (QColor  color7     READ getColor7Property         WRITE setColor7Property)
   Q_PROPERTY (QColor  color8     READ getColor8Property         WRITE setColor8Property)
   Q_PROPERTY (QColor  color9     READ getColor9Property         WRITE setColor9Property)
   Q_PROPERTY (QColor  color10    READ getColor10Property        WRITE setColor10Property)
   Q_PROPERTY (QColor  color11    READ getColor11Property        WRITE setColor11Property)
   Q_PROPERTY (QColor  color12    READ getColor12Property        WRITE setColor12Property)


public:
   enum Constants {
      NUMBER_OF_PVS = 12
   };

   /// Constructors
   //
   QCaStripChart (QWidget * parent = 0);
   ~QCaStripChart ();

   QSize sizeHint () const;

   // Start time, end time, and duration (seconds).
   // Three values, but only two are indpendent.
   //
   // Setting end time keeps duration fixed and adjusts start time.
   // Setting duration keeps end time fixed and adjusts start time.
   // Setting a duration value < 1 limited to 1 second.
   //
   QDateTime getStartTime ();

   QDateTime getEndTime ();
   void setEndTime (QDateTime endTimeIn);

   int getDuration ();
   void setDuration (int durationIn);

   double getYMinimum ();
   void setYMinimum (double yMinimumIn);

   double getYMaximum ();
   void setYMaximum (double yMaximumIn);

protected:
   /// Drag and Drop
   //
   // Override QWidget functions - call up standard handlers defined in QCaDragDrop.
   //
   void dragEnterEvent (QDragEnterEvent *event) { qcaDragEnterEvent (event); }
   void dropEvent (QDropEvent *event)           { qcaDropEvent (event); }

   // Don't drag from this widget.
   // void mousePressEvent (QMouseEvent *event)    { qcaMousePressEvent (event); }

   // Override QCaDragDrop functions.
   //
   void setDropText (QString text);

   // override pure virtual functions
   //
   void setup ();
   qcaobject::QCaObject* createQcaItem (unsigned int variableIndex);
   void establishConnection (unsigned int variableIndex);
   void updateToolTip (const QString& tip);

private:
   // Intermal widgets and associated support data.
   // If these items declared at class level, there is a run time exception.
   // Also allows, what is essentially private, to be actually private, well
   // at least only declared in QCaStripChart.cpp
   //
   class PrivateData;
   PrivateData *privateData;
   friend class PrivateData;

   // Timer to keep strip chart scrolling
   //
   QTimer* tickTimer;

   // Chart time range in seconds.
   //
   int duration;
   QDateTime endTime;

   // Chart lower/upper range
   //
   double yMinimum;
   double yMaximum;


   void addPvName (QString pvName);  // make public ??

   // Used by QCaStripChartItem
   //
   friend class QCaStripChartItem;
   void evaluateAllowDrop ();
   QwtPlotCurve *allocateCurve ();


   // Property access support functions.
   //
   void    setVariableNameProperty (unsigned int slot, QString pvName);
   QString getVariableNameProperty (unsigned int slot);

   void   setColorProperty (unsigned int slot, QColor color);
   QColor getColorProperty (unsigned int slot);

   // Property access READ and WRITE functions.
   // We can define the access functions using a macro.
   // Alas we cannot embedded the property definition itself in a macro.
   //
   #define PROPERTY_ACCESS(slot)  \
      void    setPropertyVariableName##slot (QString name) { this->setVariableNameProperty (slot - 1, name); }   \
      QString getPropertyVariableName##slot ()      { return this->getVariableNameProperty (slot - 1); }         \
                                                                                                                 \
      void   setColor##slot##Property (QColor color) { this->setColorProperty (slot - 1, color); }               \
      QColor getColor##slot##Property ()      { return this->getColorProperty (slot - 1); }

   PROPERTY_ACCESS  (1)
   PROPERTY_ACCESS  (2)
   PROPERTY_ACCESS  (3)
   PROPERTY_ACCESS  (4)
   PROPERTY_ACCESS  (5)
   PROPERTY_ACCESS  (6)
   PROPERTY_ACCESS  (7)
   PROPERTY_ACCESS  (8)
   PROPERTY_ACCESS  (9)
   PROPERTY_ACCESS  (10)
   PROPERTY_ACCESS  (11)
   PROPERTY_ACCESS  (12)

#undef PROPERTY_ACCESS

private slots:
   void tickTimeout ();
   void menuSetYScale (QAction *action);
   void menuSetDuration (QAction *action);
   void menuSetTimeMode (QAction *action);
   void readArchiveClicked (bool checked = false);
};

# endif  // QCASTRIPCHART_H
