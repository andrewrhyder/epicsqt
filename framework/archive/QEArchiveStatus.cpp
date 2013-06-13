/*  QEArchiveStatus.cpp
 *
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
 *  Copyright (c) 2013
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#include <QDebug>
#include <QLabel>
#include <QFrame>

#include <QECommon.h>
#include "QEArchiveStatus.h"

#define DEBUG  qDebug () << "QEArchiveStatus::" << __FUNCTION__ << __LINE__


//==============================================================================
//
class QEArchiveStatus::PrivateData {
public:
   PrivateData (QEArchiveStatus* parent);

   QEArchiveAccess *archiveAccess;
   QLabel* title;
   QVBoxLayout *vLayout;

   static const int NumberRows = 20;   // maximum.

   struct Rows {
      QFrame* frame;
      QHBoxLayout *hLayout;
      QLabel* hostNamePort;
      QLabel* endPoint;
      QLabel* state;
      QLabel* available;
      QLabel* read;
      QLabel* numberPVs;
   };

   Rows rowList [NumberRows + 1];

private:
   QWidget* spacer;
};

//------------------------------------------------------------------------------
//
QEArchiveStatus::PrivateData::PrivateData (QEArchiveStatus* parent)
{

#define CREATE_LABEL(member, width, align, text)  {              \
   this->rowList [j].member = new QLabel (text, frame);          \
   this->rowList [j].member->setIndent (6);                      \
   this->rowList [j].member->setFixedWidth (width);              \
   this->rowList [j].member->setAlignment (align);               \
   this->rowList [j].member->setStyleSheet (sheet);              \
   hLayout->addWidget (this->rowList [j].member);                \
}

   int j;
   QColor background;
   QString sheet;
   QFrame* frame;
   QHBoxLayout *hLayout;

   this->archiveAccess = new QEArchiveAccess (parent);

   this->title = new QLabel ("Archive Status Summary", parent);
   this->title->setFixedHeight (24);
   this->title->setStyleSheet (QEUtilities::colourToStyle (QColor (200, 222, 255, 255)));
   this->title->setAlignment (Qt::AlignHCenter);

   this->vLayout = new QVBoxLayout (parent);
   this->vLayout->setMargin (2);
   this->vLayout->setSpacing (2);
   this->vLayout->addWidget (this->title);

   // Use use the last row as a header row.
   //
   j = NumberRows;
   sheet = "";
   this->rowList [j].frame = frame = new QFrame (parent);
   frame->setFixedHeight (20);
   this->rowList [j].hLayout = hLayout = new QHBoxLayout (frame);
   hLayout->setMargin (2);
   hLayout->setSpacing (4);

   CREATE_LABEL (hostNamePort, 160, Qt::AlignLeft,    "Host:Port");
   CREATE_LABEL (endPoint,     220, Qt::AlignLeft,    "End Point");
   CREATE_LABEL (state,         88, Qt::AlignHCenter, "Status");
   CREATE_LABEL (available,     68, Qt::AlignRight,   "Available");
   CREATE_LABEL (read,          68, Qt::AlignRight,   "Read");
   CREATE_LABEL (numberPVs,     68, Qt::AlignRight,   "Num PVs");

   this->vLayout->addWidget (frame);


   background = QColor (220, 220, 220, 255);
   sheet = QEUtilities::colourToStyle (background);

   for (j = 0; j < NumberRows; j++ ) {

      this->rowList [j].frame = frame = new QFrame (parent);
      frame->setFixedHeight (20);

      this->rowList [j].hLayout = hLayout = new QHBoxLayout (frame);
      hLayout->setMargin (2);
      hLayout->setSpacing (4);

      CREATE_LABEL (hostNamePort, 160, Qt::AlignLeft,     " - ");
      CREATE_LABEL (endPoint,     220, Qt::AlignLeft,     " - ");
      CREATE_LABEL (state,         88, Qt::AlignHCenter,  " - ");
      CREATE_LABEL (available,     68, Qt::AlignRight,    " - ");
      CREATE_LABEL (read,          68, Qt::AlignRight,    " - ");
      CREATE_LABEL (numberPVs,     68, Qt::AlignRight,    " - ");


      this->vLayout->addWidget (frame);
   }

   this->spacer = new QWidget (parent);
   this->vLayout->addWidget (this->spacer);

#undef CREATE_LABEL

}


//==============================================================================
//
QEArchiveStatus::QEArchiveStatus (QWidget* parent) : QEFrame (parent)
{
   this->privateData = new PrivateData (this);

   QObject::connect (this->privateData->archiveAccess,
                     SIGNAL     (archiveStatus (const QEArchiveAccess::StatusList&)),
                     this, SLOT (archiveStatus (const QEArchiveAccess::StatusList&)));

   this->setMinimumSize (712, 60);
   this->privateData->archiveAccess->resendStatus ();
}


//------------------------------------------------------------------------------
//
QEArchiveStatus::~QEArchiveStatus ()
{
   delete this->privateData;
}

//------------------------------------------------------------------------------
//
QSize QEArchiveStatus::sizeHint () const
{
   return QSize (712, 100);
}

//------------------------------------------------------------------------------
//
void QEArchiveStatus::archiveStatus (const QEArchiveAccess::StatusList& statusList)
{
   int j;

   for (j = 0; j < QEArchiveStatus::PrivateData::NumberRows; j++ ) {
      QEArchiveStatus::PrivateData::Rows* row = &this->privateData->rowList [j];

      if (j <  statusList.count ()) {
         QEArchiveAccess::Status state = statusList.value (j);

         row->hostNamePort->setText (QString ("%1:%2").arg (state.hostName).arg (state.portNumber));
         row->endPoint->setText (state.endPoint);
         row->state->setText ( QEUtilities::enumToString (*this->privateData->archiveAccess,
                                                          QString ("States"), (int) state.state));
         row->available->setText (QString ("%1").arg (state.available));
         row->read->setText (QString ("%1").arg (state.read));
         row->numberPVs->setText (QString ("%1").arg (state.numberPVs));

         row->frame->setVisible (true);

      } else {
         row->frame->setVisible (false);
      }
   }
}

// end
