/*  QEPlotterToolBar.cpp
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
 *  but WITHOUT ANY WARRANTY { } without even the implied warranty of
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

#include <QDebug>
#include <QIcon>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QString>
#include <QVariant>
#include <QDateTime>

#include <QECommon.h>
#include "QEPlotterToolBar.h"


#define NUMBER_OF_BUTTONS  21
#define ICW                26         // icon width

// Special slots NUMBERS  - must be consistent with below
//
#define PREV_SLOT          0
#define NEXT_SLOT          1

struct PushButtonSpecifications {
   int gap;
   int width;
   bool isIcon;  // when false is caption
   const QString captionOrIcon;
   const QString toolTip;
   const char * member;
};

static const QString localZone = QEUtilities::getTimeZoneTLA (Qt::LocalTime, QDateTime::currentDateTime ());

static const struct PushButtonSpecifications buttonSpecs [NUMBER_OF_BUTTONS] = {
   { 0,   ICW, true,  QString ("go_back.png"),           QString ("Previous state"),               SLOT (prevStateClicked (bool))        },
   { 0,   ICW, true,  QString ("go_fwd.png"),            QString ("Next state"),                   SLOT (nextStateClicked (bool))        },

   { 8,   ICW, true,  QString ("normal_video.png"),      QString ("White background"),             SLOT (normalVideoClicked (bool))      },
   { 0,   ICW, true,  QString ("reverse_video.png"),     QString ("Black background"),             SLOT (reverseVideoClicked (bool))     },

   { 8,   ICW, true,  QString ("linear_scale.png"),      QString ("Linear scale"),                 SLOT (linearScaleClicked (bool))      },
   { 0,   ICW, true,  QString ("log_scale.png"),         QString ("Log Scale"),                    SLOT (logScaleClicked (bool))         },

   { 8,   ICW, false, QString ("M"),                     QString ("Manual Scale"),                 SLOT (manualYScaleClicked (bool))     },
   { 0,   ICW, false, QString ("P"),                     QString ("Plotted Data Scale"),           SLOT (plottedYScaleClicked (bool))    },
   { 0,   ICW, false, QString ("B"),                     QString ("Buffer Data Scale"),            SLOT (bufferedYScaleClicked (bool))   },
   { 0,   ICW, false, QString ("D"),                     QString ("Dynamic Scale"),                SLOT (dynamicYScaleClicked (bool))    },
   { 0,   ICW, false, QString ("F"),                     QString ("Fractional Scale"),             SLOT (normalisedYScaleClicked (bool)) },
   { 0,   ICW, false, QString ("N"),                     QString ("Normalised Scale"),             SLOT (normalisedYScaleClicked (bool)) },

   { 8,   ICW, true,  QString ("play.png"),              QString ("Play - Real time"),             SLOT (playClicked (bool))             },
   { 0,   ICW, true,  QString ("pause.png"),             QString ("Pause"),                        SLOT (pauseClicked (bool))            }
};



//==============================================================================
//
class QEPlotterToolBar::OwnWidgets : public QObject {
public:
   OwnWidgets (QEPlotterToolBar *parent);
   ~OwnWidgets ();

   QPushButton *pushButtons [NUMBER_OF_BUTTONS];

private:
};


//------------------------------------------------------------------------------
//
QEPlotterToolBar::OwnWidgets::OwnWidgets (QEPlotterToolBar *parent) : QObject (parent)
{
   int left;
   int j;
   QPushButton *button;
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
         button->setText (buttonSpecs[j].captionOrIcon);
      }

      button->setToolTip(buttonSpecs[j].toolTip);
      gap = buttonSpecs[j].gap;
      button->setGeometry (left + gap, 2, buttonSpecs[j].width, 26);
      left += gap + buttonSpecs[j].width + 2;
      if (buttonSpecs[j].member != NULL) {
         QObject::connect (button, SIGNAL (clicked (bool)),
                           parent, buttonSpecs[j].member);
      }
      this->pushButtons [j] = button;
   }
}

//------------------------------------------------------------------------------
//
QEPlotterToolBar::OwnWidgets::~OwnWidgets ()
{
  // no special action - place holder
}


//==============================================================================
//
QEPlotterToolBar::QEPlotterToolBar (QWidget *parent) : QFrame (parent)
{
   this->setFixedHeight (this->designHeight);
   this->ownWidgets = new OwnWidgets (this);
}

//------------------------------------------------------------------------------
//
QEPlotterToolBar::~QEPlotterToolBar ()
{
   // no special action - place holder
}


//------------------------------------------------------------------------------
//
void QEPlotterToolBar::resizeEvent (QResizeEvent *)
{
    // place holder
}

//------------------------------------------------------------------------------
//
void QEPlotterToolBar::prevStateClicked (bool)
{
   emit this->stateSelected (QEStripChartNames::previous);
}

void QEPlotterToolBar::nextStateClicked (bool)
{
   emit this->stateSelected (QEStripChartNames::next);
}

//------------------------------------------------------------------------------
//
void QEPlotterToolBar::normalVideoClicked (bool)
{
   emit this->videoModeSelected (QEStripChartNames::normal);
}

void QEPlotterToolBar::reverseVideoClicked (bool)
{
   emit this->videoModeSelected (QEStripChartNames::reverse);
}

//------------------------------------------------------------------------------
//
void QEPlotterToolBar::linearScaleClicked (bool)
{
}

void QEPlotterToolBar::logScaleClicked (bool)
{
}

//------------------------------------------------------------------------------
//
void QEPlotterToolBar::manualYScaleClicked (bool)
{
}

void QEPlotterToolBar::automaticYScaleClicked (bool)
{
}

void QEPlotterToolBar::plottedYScaleClicked (bool)
{
}

void QEPlotterToolBar::bufferedYScaleClicked (bool)
{
}

void QEPlotterToolBar::dynamicYScaleClicked (bool)
{
}

void QEPlotterToolBar::normalisedYScaleClicked (bool)
{
}


//------------------------------------------------------------------------------
//
void QEPlotterToolBar::playClicked (bool)
{
}

void QEPlotterToolBar::pauseClicked (bool)
{
}

// end

