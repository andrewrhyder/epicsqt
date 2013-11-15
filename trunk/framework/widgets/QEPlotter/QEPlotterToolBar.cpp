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
#include <QString>
#include <QVariant>

#include <QECommon.h>
#include "QEPlotterToolBar.h"


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

static const struct PushButtonSpecifications buttonSpecs [] = {
   { 0,   ICW, true,  QString ("go_back.png"),           QString ("Previous state"),               SLOT (prevStateClicked (bool))        },
   { 0,   ICW, true,  QString ("go_fwd.png"),            QString ("Next state"),                   SLOT (nextStateClicked (bool))        },

   { 8,   ICW, true,  QString ("normal_video.png"),      QString ("White background"),             SLOT (normalVideoClicked (bool))      },
   { 0,   ICW, true,  QString ("reverse_video.png"),     QString ("Black background"),             SLOT (reverseVideoClicked (bool))     },

   { 8,   ICW, true,  QString ("linear_scale.png"),      QString ("Linear scale"),                 SLOT (linearScaleClicked (bool))      },
   { 0,   ICW, true,  QString ("log_scale.png"),         QString ("Log Scale"),                    SLOT (logScaleClicked (bool))         },

   { 8,   ICW, false, QString ("M"),                     QString ("Manual Scale"),                 SLOT (manualYScaleClicked (bool))     },
   { 0,   ICW, false, QString ("A"),                     QString ("Data Range Scale"),             SLOT (automaticYScaleClicked (bool))  },
   { 0,   ICW, false, QString ("D"),                     QString ("Dynamic Scale"),                SLOT (dynamicYScaleClicked (bool))    },
   { 0,   ICW, false, QString ("N"),                     QString ("Noramalised Scale"),            SLOT (normalisedYScaleClicked (bool)) },
   { 0,   ICW, false, QString ("F"),                     QString ("Fractional Scale"),             SLOT (fractionalYScaleClicked (bool)) },

   { 8,   ICW, true,  QString ("play.png"),              QString ("Play - Real time"),             SLOT (playClicked (bool))             },
   { 0,   ICW, true,  QString ("pause.png"),             QString ("Pause"),                        SLOT (pauseClicked (bool))            }
};


//==============================================================================
//
QEPlotterToolBar::QEPlotterToolBar (QWidget *parent) : QFrame (parent)
{
   int left;
   int j;
   QPushButton *button;
   QString iconPathName;
   int gap;

   this->setFixedHeight (this->designHeight);

   this->pushButtons [TOOLBAR_NONE] = NULL;

   // Create toobar buttons
   // TODO: Try QToolBar - it may auto layout.
   //
   left = 4;
   for (j = 0; j < ARRAY_LENGTH (buttonSpecs); j++) {

      button = new QPushButton (this);

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
                           this, buttonSpecs[j].member);
      }
      this->pushButtons [j + 1] = button;  // save reference
   }
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
void QEPlotterToolBar::setEnabled (const ToolBarOptions item, const bool value)
{
   if (this->pushButtons [item]) {
      this->pushButtons [item]->setEnabled (value);
   }
}

//------------------------------------------------------------------------------
//
void QEPlotterToolBar::prevStateClicked (bool)
{
   emit this->selected (TOOLBAR_PREV);
}

void QEPlotterToolBar::nextStateClicked (bool)
{
   emit this->selected (TOOLBAR_NEXT);
}

//------------------------------------------------------------------------------
//
void QEPlotterToolBar::normalVideoClicked (bool)
{
   emit this->selected (TOOLBAR_NORMAL_VIDEO);
}

void QEPlotterToolBar::reverseVideoClicked (bool)
{
   emit this->selected (TOOLBAR_REVERSE_VIDEO);
}

//------------------------------------------------------------------------------
//
void QEPlotterToolBar::linearScaleClicked (bool)
{
   emit this->selected (TOOLBAR_LINEAR_Y_SCALE);
}

void QEPlotterToolBar::logScaleClicked (bool)
{
   emit this->selected (TOOLBAR_LOG_Y_SCALE);
}

//------------------------------------------------------------------------------
//
void QEPlotterToolBar::manualYScaleClicked (bool)
{
   emit this->selected (TOOLBAR_MANUAL_Y_RANGE);
}

void QEPlotterToolBar::automaticYScaleClicked (bool)
{
   emit this->selected (TOOLBAR_CURRENT_Y_RANGE);
}

void QEPlotterToolBar::dynamicYScaleClicked (bool)
{
   emit this->selected (TOOLBAR_DYNAMIC_Y_RANGE);
}

void QEPlotterToolBar::normalisedYScaleClicked (bool)
{
   emit this->selected (TOOLBAR_NORAMLISED_Y_RANGE);
}

void QEPlotterToolBar::fractionalYScaleClicked (bool)
{
   emit this->selected (TOOLBAR_FRACTIONAL_Y_RANGE);
}

//------------------------------------------------------------------------------
//
void QEPlotterToolBar::playClicked (bool)
{
   emit this->selected (TOOLBAR_PLAY);
}

void QEPlotterToolBar::pauseClicked (bool)
{
   emit this->selected (TOOLBAR_PAUSE);
}

// end

