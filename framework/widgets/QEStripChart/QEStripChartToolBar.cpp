/*  QEStripChartToolBar.cpp
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY { } without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2013 Australian Synchrotron
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 *
 */

#include <QDebug>
#include <QIcon>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QString>
#include <QVariant>
#include <QDateTime>

#include <QECommon.h>
#include "QEStripChartUtilities.h"
#include "QEStripChartToolBar.h"


#define NUMBER_OF_BUTTONS  34
#define ICW                26         // icon width
#define DBW                28         // duration buttton width
#define VALUE_PROPERTY     "QESTRIPCHART_BUTTON_VALUE"

// Special slots NUMBERS  - must be consistent with below
//
#define PREV_SLOT          0
#define NEXT_SLOT          1
#define YSCALE_SLOT        6
#define TSCALE_SLOT        12


// Structure used in buttonSpecs for definining strip chart tool bar.
// Note, a similar structure is used in QEPlotter. If they are the same name a
// strange problem occurs when built with GCC 4.5.1 where the QString destructor is
// called inappropriately on exit causing a crash.
//
struct QEStripChartPushButtonSpecifications {
   int gap;
   int width;
   int value;
   bool isIcon;  // when false is caption
   const QString captionOrIcon;
   const QString toolTip;
   const char* member;
};

static const QString localZone = QEUtilities::getTimeZoneTLA (Qt::LocalTime, QDateTime::currentDateTime ());

static const struct QEStripChartPushButtonSpecifications buttonSpecs [NUMBER_OF_BUTTONS] = {
   { 0,   ICW, 0, true,  QString ("go_back.png"),           QString ("Previous state"),               SLOT (prevStateClicked (bool))        },
   { 0,   ICW, 0, true,  QString ("go_fwd.png"),            QString ("Next state"),                   SLOT (nextStateClicked (bool))        },

   { 8,   ICW, 0, true,  QString ("normal_video.png"),      QString ("White background"),             SLOT (normalVideoClicked (bool))      },
   { 0,   ICW, 0, true,  QString ("reverse_video.png"),     QString ("Black background"),             SLOT (reverseVideoClicked (bool))     },

   { 8,   ICW, 0, true,  QString ("linear_scale.png"),      QString ("Linear scale"),                 SLOT (linearScaleClicked (bool))      },
   { 0,   ICW, 0, true,  QString ("log_scale.png"),         QString ("Log Scale"),                    SLOT (logScaleClicked (bool))         },

   { 8,   ICW, 0, false, QString ("M"),                     QString ("Manual Scale"),                 SLOT (manualYScaleClicked (bool))     },
   { 0,   ICW, 0, false, QString ("A"),                     QString ("HOPR/LOPR Scale"),              SLOT (automaticYScaleClicked (bool))  },
   { 0,   ICW, 0, false, QString ("P"),                     QString ("Plotted Data Scale"),           SLOT (plottedYScaleClicked (bool))    },
   { 0,   ICW, 0, false, QString ("B"),                     QString ("Buffer Data Scale"),            SLOT (bufferedYScaleClicked (bool))   },
   { 0,   ICW, 0, false, QString ("D"),                     QString ("Dynamic Scale"),                SLOT (dynamicYScaleClicked (bool))    },
   { 0,   ICW, 0, false, QString ("N"),                     QString ("Normalised Scale"),             SLOT (normalisedYScaleClicked (bool)) },

   { 8,   DBW, 60,     false, QString ("1m"),               QString ("Select chart duration"),        SLOT (duration2Clicked (bool))        },
   { 0,   DBW, 120,    false, QString ("2m"),               QString ("Select chart duration"),        SLOT (duration2Clicked (bool))        },
   { 0,   DBW, 300,    false, QString ("5m"),               QString ("Select chart duration"),        SLOT (duration2Clicked (bool))        },
   { 0,   DBW, 600,    false, QString ("10m"),              QString ("Select chart duration"),        SLOT (duration2Clicked (bool))        },
   { 0,   DBW, 1200,   false, QString ("20m"),              QString ("Select chart duration"),        SLOT (duration2Clicked (bool))        },
   { 0,   DBW, 1800,   false, QString ("30m"),              QString ("Select chart duration"),        SLOT (duration2Clicked (bool))        },
   { 0,   DBW, 3600,   false, QString ("1h"),               QString ("Select chart duration"),        SLOT (duration2Clicked (bool))        },
   { 0,   DBW, 7200,   false, QString ("2h"),               QString ("Select chart duration"),        SLOT (duration2Clicked (bool))        },
   { 0,   DBW, 14400,  false, QString ("4h"),               QString ("Select chart duration"),        SLOT (duration2Clicked (bool))        },
   { 0,   DBW, 21600,  false, QString ("6h"),               QString ("Select chart duration"),        SLOT (duration2Clicked (bool))        },
   { 0,   DBW, 43200,  false, QString ("12h"),              QString ("Select chart duration"),        SLOT (duration2Clicked (bool))        },
   { 0,   DBW, 86400,  false, QString ("1d"),               QString ("Select chart duration"),        SLOT (duration2Clicked (bool))        },
   { 0,   DBW, 172800, false, QString ("2d"),               QString ("Select chart duration"),        SLOT (duration2Clicked (bool))        },
   { 0,   ICW, 0, true, QString ("select_time.png"),        QString ("Select chart duration"),        SLOT (selectDurationClicked (bool))   },

   { 8,   40,  0, false, localZone,                         QString ("Use local time"),               SLOT (localTimeClicked (bool))        },
   { 0,   40,  0, false, QString ("UTC"),                   QString ("Use UTC (GMT) time"),           SLOT (utcTimeClicked (bool))          },

   { 8,   ICW, 0, true,  QString ("archive.png"),           QString ("Extract data from archive(s)"), SLOT (readArchiveClicked (bool))      },
   { 0,   ICW, 0, true,  QString ("select_date_times.png"), QString ("Set chart start/end time"),     SLOT (selectTimeClicked (bool))       },
   { 0,   ICW, 0, true,  QString ("play.png"),              QString ("Play - Real time"),             SLOT (playClicked (bool))             },
   { 0,   ICW, 0, true,  QString ("pause.png"),             QString ("Pause"),                        SLOT (pauseClicked (bool))            },
   { 0,   ICW, 0, true,  QString ("page_backward.png"),     QString ("Back one page"),                SLOT (backwardClicked (bool))         },
   { 0,   ICW, 0, true,  QString ("page_forward.png"),      QString ("Forward one page"),             SLOT (forwardClicked (bool))          },
};



//==============================================================================
//
class QEStripChartToolBar::OwnWidgets : public QObject {
public:
   explicit OwnWidgets (QEStripChartToolBar *parent);
   ~OwnWidgets ();

   QPushButton *pushButtons [NUMBER_OF_BUTTONS];
   QLabel *yScaleStatus;
   QLabel *timeStatus;
   QLabel *durationStatus;
};


//------------------------------------------------------------------------------
//
QEStripChartToolBar::OwnWidgets::OwnWidgets (QEStripChartToolBar *parent) : QObject (parent)
{
   int left;
   int j;
   QPushButton* button;
   QString iconPathName;
   int gap;

   // Create toobar buttons
   // TODO: Try QToolBar - it may auto layout.
   //
   left = 4;
   for (j = 0 ; j < NUMBER_OF_BUTTONS; j++) {
      button = new QPushButton (parent);

      // Set up icon or caption text.
      //
      if (buttonSpecs[j].isIcon) {
         iconPathName = ":/qe/stripchart/";
         iconPathName.append (buttonSpecs[j].captionOrIcon);
         button->setIcon (QIcon (iconPathName));
      } else {
         QFont f;
         f = button->font();
         f.setPointSize (8);
         button->setFont (f);
         button->setText (buttonSpecs[j].captionOrIcon);

      }

      button->setFocusPolicy (Qt::NoFocus);
      button->setProperty (VALUE_PROPERTY, buttonSpecs[j].value);
      button->setToolTip (buttonSpecs[j].toolTip);
      gap = buttonSpecs[j].gap;
      button->setGeometry (left + gap, 2, buttonSpecs[j].width, 26);
      left += gap + buttonSpecs[j].width;
      if (buttonSpecs[j].member != NULL) {
         QObject::connect (button, SIGNAL (clicked (bool)),
                           parent, buttonSpecs[j].member);
      }

      this->pushButtons [j] = button;
   }

   // Set up status labels.
   //
   this->timeStatus = new QLabel (parent);
   left = this->pushButtons [TSCALE_SLOT]->geometry().x ();
   this->timeStatus->setGeometry (left, 28, 328, 16);    // left top width height

   left = this->timeStatus->geometry ().right () + 8;

   this->durationStatus = new QLabel (parent);
   this->durationStatus->setGeometry (left, 28, 84, 16);    // left top width height
   this->durationStatus->setAlignment (Qt::AlignRight);

   QFont font = this->timeStatus->font ();
   font.setFamily ("Monospace");
   font.setPointSize (8);

   this->timeStatus->setFont (font);
   this->durationStatus->setFont (font);

   this->yScaleStatus = new QLabel ("Dynamic", parent);
   left = this->pushButtons [YSCALE_SLOT]->geometry().x ();
   this->yScaleStatus->setGeometry (left, 28, 160, 16);
   this->yScaleStatus->setAlignment (Qt::AlignHCenter);
   this->yScaleStatus->setFont (font);

   // this->timeStatus->setStyleSheet     ("QWidget { background-color: #ffffe0; }");
   // this->durationStatus->setStyleSheet ("QWidget { background-color: #ffffe0; }");
   // this->yScaleStatus->setStyleSheet   ("QWidget { background-color: #ffffe0; }");
}

//------------------------------------------------------------------------------
//
QEStripChartToolBar::OwnWidgets::~OwnWidgets ()
{
  // no special action - place holder
}


//==============================================================================
//
QEStripChartToolBar::QEStripChartToolBar (QWidget *parent) : QFrame (parent)
{
   this->setFixedHeight (32);
   this->ownWidgets = new OwnWidgets (this);
}

//------------------------------------------------------------------------------
//
QEStripChartToolBar::~QEStripChartToolBar ()
{
   // no special action - place holder
}

//------------------------------------------------------------------------------
//
void QEStripChartToolBar::setYRangeStatus (const QString & status)
{
   this->ownWidgets->yScaleStatus->setText (status);
}

//------------------------------------------------------------------------------
//
void QEStripChartToolBar::setTimeStatus (const QString& timeStatusIn)
{
   this->ownWidgets->timeStatus->setText (timeStatusIn);
}

//------------------------------------------------------------------------------
//
void QEStripChartToolBar::setDurationStatus (const QString& durationStatusIn)
{
   this->ownWidgets->durationStatus->setText (durationStatusIn);
}

//------------------------------------------------------------------------------
//
void QEStripChartToolBar::setStateSelectionEnabled (const QEStripChartNames::StateModes mode, const bool enabled)
{
   switch (mode) {
      case QEStripChartNames::previous:
         this->ownWidgets->pushButtons [PREV_SLOT]->setEnabled (enabled);
         break;

      case QEStripChartNames::next:
         this->ownWidgets->pushButtons [NEXT_SLOT]->setEnabled (enabled);
         break;
   }
}

//------------------------------------------------------------------------------
//
void QEStripChartToolBar::resizeEvent (QResizeEvent *)
{
    // place holder
}

//------------------------------------------------------------------------------
//
void QEStripChartToolBar::duration2Clicked (bool)
{
   QPushButton* button = dynamic_cast <QPushButton*> (this->sender ());
   if (button) {
      int d;
      bool okay;
      d = button->property(VALUE_PROPERTY).toInt(&okay);
      if (okay) {
         emit this->durationSelected (d);
      }
   }
}

//------------------------------------------------------------------------------
//
void QEStripChartToolBar::selectDurationClicked (bool)
{
   emit this->selectDuration ();
}

//------------------------------------------------------------------------------
//
void QEStripChartToolBar::prevStateClicked (bool)
{
   emit this->stateSelected (QEStripChartNames::previous);
}

void QEStripChartToolBar::nextStateClicked (bool)
{
   emit this->stateSelected (QEStripChartNames::next);
}

//------------------------------------------------------------------------------
//
void QEStripChartToolBar::normalVideoClicked (bool)
{
   emit this->videoModeSelected (QEStripChartNames::normal);
}

void QEStripChartToolBar::reverseVideoClicked (bool)
{
   emit this->videoModeSelected (QEStripChartNames::reverse);
}

//------------------------------------------------------------------------------
//
void QEStripChartToolBar::linearScaleClicked (bool)
{
   this->yScaleModeSelected (QEStripChartNames::linear);
}

void QEStripChartToolBar::logScaleClicked (bool)
{
   this->yScaleModeSelected (QEStripChartNames::log);
}

//------------------------------------------------------------------------------
//
void QEStripChartToolBar::manualYScaleClicked (bool)
{
   emit this->yRangeSelected (QEStripChartNames::manual);
}

void QEStripChartToolBar::automaticYScaleClicked (bool)
{
   emit this->yRangeSelected (QEStripChartNames::operatingRange);
}

void QEStripChartToolBar::plottedYScaleClicked (bool)
{
   emit this->yRangeSelected (QEStripChartNames::plotted);
}

void QEStripChartToolBar::bufferedYScaleClicked (bool)
{
   emit this->yRangeSelected (QEStripChartNames::buffered);
}

void QEStripChartToolBar::dynamicYScaleClicked (bool)
{
   emit this->yRangeSelected (QEStripChartNames::dynamic);
}

void QEStripChartToolBar::normalisedYScaleClicked (bool)
{
   emit this->yRangeSelected (QEStripChartNames::normalised);
}


//------------------------------------------------------------------------------
//
void QEStripChartToolBar::playClicked (bool)
{
   emit this->playModeSelected (QEStripChartNames::play);
}

void QEStripChartToolBar::pauseClicked (bool)
{
   emit this->playModeSelected (QEStripChartNames::pause);
}

void QEStripChartToolBar::forwardClicked (bool)
{
   emit this->playModeSelected (QEStripChartNames::forward);
}

void QEStripChartToolBar::backwardClicked (bool)
{
   emit this->playModeSelected (QEStripChartNames::backward);
}

void QEStripChartToolBar::selectTimeClicked (bool)
{
   emit this->playModeSelected (QEStripChartNames::selectTimes);
}

//------------------------------------------------------------------------------
//
void QEStripChartToolBar::readArchiveClicked (bool)
{
    emit this->readArchiveSelected ();
}

//------------------------------------------------------------------------------
//
void  QEStripChartToolBar::localTimeClicked (bool)
{
   emit this->timeZoneSelected (Qt::LocalTime);
}

void  QEStripChartToolBar::utcTimeClicked (bool)
{
   emit this->timeZoneSelected (Qt::UTC);
}

// end
