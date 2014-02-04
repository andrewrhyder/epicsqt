/*  QEStripChart.h
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

#ifndef QESTRIPCHART_H
#define QESTRIPCHART_H

#include <QAction>
#include <QBoxLayout>
#include <QColor>
#include <QDateTime>
#include <QGridLayout>
#include <QMenu>
#include <QMouseEvent>
#include <QObject>
#include <QPointF>
#include <QSize>
#include <QTimer>
#include <QVariant>
#include <QScrollArea>

#include <QCaAlarmInfo.h>
#include <QEPluginLibrary_global.h>
#include <QCaDateTime.h>
#include <QCaConnectionInfo.h>
#include <QEDragDrop.h>
#include <QEFrame.h>
#include <QEWidget.h>
#include <persistanceManager.h>
#include <QEResizeableFrame.h>
#include <QEPVNameSelectDialog.h>

#include "QEStripChartNames.h"
#include "QEStripChartToolBar.h"
#include "QEStripChartRangeDialog.h"
#include "QEStripChartTimeDialog.h"
#include "QEStripChartState.h"

// Differed class declaration - no explicit dependency on Qwt header files.
//
class QEGraphic;

// Avoid mutual header references.
//
class QEStripChartItem;


class QEPLUGINLIBRARYSHARED_EXPORT QEStripChart : public QEFrame {
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

   /// Macro substitutions. The default is no substitutions. The format is NAME1=VALUE1[,] NAME2=VALUE2...
   /// Values may be quoted strings. For example, 'SAMPLE=SAM1, NAME = "Ref foil"'
   /// These substitutions are applied to all the variable names.
   Q_PROPERTY (QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)

   // NOTE: Where ever possible I spell colour properly.
   //
   Q_PROPERTY (QColor  colour1    READ getColour1Property        WRITE setColour1Property)
   Q_PROPERTY (QColor  colour2    READ getColour2Property        WRITE setColour2Property)
   Q_PROPERTY (QColor  colour3    READ getColour3Property        WRITE setColour3Property)
   Q_PROPERTY (QColor  colour4    READ getColour4Property        WRITE setColour4Property)
   Q_PROPERTY (QColor  colour5    READ getColour5Property        WRITE setColour5Property)
   Q_PROPERTY (QColor  colour6    READ getColour6Property        WRITE setColour6Property)
   Q_PROPERTY (QColor  colour7    READ getColour7Property        WRITE setColour7Property)
   Q_PROPERTY (QColor  colour8    READ getColour8Property        WRITE setColour8Property)
   Q_PROPERTY (QColor  colour9    READ getColour9Property        WRITE setColour9Property)
   Q_PROPERTY (QColor  colour10   READ getColour10Property       WRITE setColour10Property)
   Q_PROPERTY (QColor  colour11   READ getColour11Property       WRITE setColour11Property)
   Q_PROPERTY (QColor  colour12   READ getColour12Property       WRITE setColour12Property)

public:
   enum Constants {
      NUMBER_OF_PVS = 12
   };

   // Constructors
   //
   explicit QEStripChart (QWidget * parent = 0);
   ~QEStripChart ();

   QSize sizeHint () const;

   // Start time, end time, and duration (seconds).
   // Three values, but only two are indpendent.
   //
   // Setting end time keeps duration fixed and adjusts start time.
   // Setting duration keeps end time fixed and adjusts start time.
   // Setting a duration value < 1 limited to 1 second.
   //
   QDateTime getStartDateTime ();

   QDateTime getEndDateTime ();
   void setEndDateTime (QDateTime endDateTimeIn);

   int getDuration ();
   void setDuration (int durationIn);

   double getYMinimum ();
   void setYMinimum (const double yMinimumIn);

   double getYMaximum ();
   void setYMaximum (const double yMaximumIn);

   // Combine setYMinimum and setYMaximum.
   //
   void setYRange (const double yMinimumIn, const double yMaximumIn);

protected:
   // Drop only. Dragging is from individual embedded QEWidgets.
   //
   // Override QWidget functions - call up standard handlers defined in QEDragDrop.
   //
   void dragEnterEvent (QDragEnterEvent *event) { qcaDragEnterEvent (event); }
   void dropEvent (QDropEvent *event)           { qcaDropEvent (event); }
   void setDrop (QVariant drop);

   // Paste only
   //
   void paste (QVariant s);

   // override pure virtual functions
   //
   void setup ();
   qcaobject::QCaObject* createQcaItem (unsigned int variableIndex);
   void establishConnection (unsigned int variableIndex);
   void saveConfiguration (PersistanceManager* pm);
   void restoreConfiguration (PersistanceManager* pm, restorePhases restorePhase);

   // Also used by QEStripChartItem
   //
   friend class QEStripChartItem;

   void addToPredefinedList (const QString & pvName);
   QStringList getPredefinedPVNameList ();
   QString getPredefinedItem (int i);
   void setRecalcIsRequired () { this->recalcIsRequired = true; }
   void setReplotIsRequired () { this->replotIsRequired = true; }
   void evaluateAllowDrop ();

private:
   // Recalculates plots chart data
   //
   void recalculateData ();

   void plotSelectedArea ();
   void plotOriginToPoint ();
   void plotData ();

   // Internal widgets and state data.
   //
   QEPVNameSelectDialog *pvNameSelectDialog;

   QEStripChartToolBar* toolBar;
   QEResizeableFrame* toolBarResize;

   QFrame* pvFrame;
   QGridLayout *pvGridLayout;

   QScrollArea* pvScrollArea;
   QEResizeableFrame* pvResizeFrame;

   QEGraphic* plotArea;
   QFrame* plotFrame;

   QVBoxLayout* layout1;
   QVBoxLayout* layout2;

   QEStripChartItem* items [NUMBER_OF_PVS];
   QMenu* chartContextMenu;

   bool isNormalVideo;

   // State data
   //
   QEStripChartNames::ChartYRanges chartYScale;
   QEStripChartNames::YScaleModes yScaleMode;
   QEStripChartNames::ChartTimeModes chartTimeMode;
   double timeScale;             // 1 => units are seconds, 60 => x units are minutes, etc.
   QString timeUnits;

   QEStripChartStateList chartStateList;

   // Timer to keep strip chart scrolling
   //
   QTimer* tickTimer;
   int tickTimerCount;
   bool replotIsRequired;
   bool recalcIsRequired;

   // Chart time range in seconds.
   //
   int duration;
   QDateTime endDateTime;
   Qt::TimeSpec timeZoneSpec;

   QEStripChartTimeDialog *timeDialog;

   // Chart lower/upper range
   //
   double yMinimum;
   double yMaximum;
   QEStripChartRangeDialog *yRangeDialog;

   // Functions
   //
   void createInternalWidgets ();

   QEStripChartItem* getItem (unsigned int slot);
   void calcDisplayMinMax ();
   void setReadOut (const QString& text);
   void setNormalBackground (bool isNormalVideo);

   void pushState ();
   void prevState ();
   void nextState ();

   void captureState (QEStripChartState& chartState);
   void applyState (const QEStripChartState& chartState);

   // Checks if the differance (distance) between mouse down and mouse up
   // positions is sufficient and unambiguous. For a given diff, only one of
   // these functions can return true, however both may return false.
   //
   bool isValidTRangeSelection (const QPoint& distance) const;
   bool isValidYRangeSelection (const QPoint& distance) const;

   void addPvName (const QString& pvName);

   // Handles space separated set of names
   //
   void addPvNameSet (const QString& pvNameSet);  // make public ??

   // Property access support functions.
   //
   void    setVariableNameProperty (unsigned int slot, QString pvName);
   QString getVariableNameProperty (unsigned int slot);

   // The actual subsitutions are handled by embedded chart items and associated QELabels,
   // this String used just supports the property.
   //
   QString variableNameSubstitutions;

   void    setVariableNameSubstitutionsProperty (QString variableNameSubstitutions);
   QString getVariableNameSubstitutionsProperty();

   void   setColourProperty (unsigned int slot, QColor color);
   QColor getColourProperty (unsigned int slot);

   // Property access READ and WRITE functions.
   // We can define the access functions using a macro.
   // Alas, due to SDK limitation, we cannot embedded the property definition itself in a macro.
   //
   #define PROPERTY_ACCESS(slot)                                                                                 \
      void    setPropertyVariableName##slot (QString name) { this->setVariableNameProperty (slot - 1, name); }   \
      QString getPropertyVariableName##slot ()      { return this->getVariableNameProperty (slot - 1); }         \
                                                                                                                 \
      void   setColour##slot##Property (QColor colour) { this->setColourProperty (slot - 1, colour); }           \
      QColor getColour##slot##Property ()       { return this->getColourProperty (slot - 1); }

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

   void menuSetYScale (QEStripChartNames::ChartYRanges ys);

private slots:
   void tickTimeout ();

   // From chart context menu
   //
   void chartContextMenuRequested (const QPoint& pos);
   void chartContextMenuTriggered (QAction* action);

   // From tool bar
   //
   void stateSelected (const QEStripChartNames::StateModes mode);
   void videoModeSelected (const QEStripChartNames::VideoModes mode);
   void yScaleModeSelected (const QEStripChartNames::YScaleModes mode);
   void yRangeSelected (const QEStripChartNames::ChartYRanges scale);
   void durationSelected (const int seconds);
   void timeZoneSelected (const Qt::TimeSpec timeZoneSpec);
   void playModeSelected (const QEStripChartNames::PlayModes mode);
   void readArchiveSelected ();

   // From the QEGraphic plot object.
   //
   void plotMouseMove  (const QPointF& posn);
   void zoomInOut      (const QPointF& about, const int zoomAmount);
   void scaleSelect    (const QPointF& origin, const QPointF& offset);
   void lineSelected   (const QPointF& origin, const QPointF& offset);
};

# endif  // QESTRIPCHART_H
