/*  QEArchiveNameSearch.cpp
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
#include <QRegExp>

#include <QECommon.h>
#include "QEArchiveNameSearch.h"

#define DEBUG  qDebug () << "QEArchiveNameSearch::" << __FUNCTION__ << __LINE__


//==============================================================================
//
QEArchiveNameSearch::QEArchiveNameSearch (QWidget* parent) : QEFrame (parent)
{
   this->archiveAccess = new QEArchiveAccess (this);
   this->createInternalWidgets ();

   QObject::connect (this->searchButton, SIGNAL (clicked       (bool)),
                     this,               SLOT   (searchClicked (bool)));

   QObject::connect (this->clearButton,  SIGNAL (clicked       (bool)),
                     this,               SLOT   (clearClicked  (bool)));

}

//------------------------------------------------------------------------------
//
QEArchiveNameSearch::~QEArchiveNameSearch ()
{
   // place holder
}

//------------------------------------------------------------------------------
//
void QEArchiveNameSearch::searchClicked (bool /* clicked */ )
{
   QString searchText;
   QString filter;
   QStringList matchingNames;

   searchText = this->lineEdit->text ().trimmed ();

   if (searchText.isEmpty ()) return;

   // Replace multiple spaces with wild card.
   // Sneaking: using a reg exp to generate a reg exp.
   //
   searchText.replace (QRegExp ("\\s+"), ".*");

   DEBUG << "searchText" << searchText;

   filter = QString (".*").append (searchText).append (".*");

   // QEArchiveAccess ensures the list is sorted.
   //
   matchingNames = QEArchiveAccess::getMatchingPVnames (filter);

   this->listWidget->clear ();
   this->listWidget->addItems (matchingNames);
}

//------------------------------------------------------------------------------
//
void QEArchiveNameSearch::clearClicked (bool /* clicked */)
{
   this->lineEdit->setText ("");
   this->listWidget->clear ();
}


//------------------------------------------------------------------------------
//
QSize QEArchiveNameSearch::sizeHint () const
{
   return QSize (700, 260);
}

//------------------------------------------------------------------------------
//
void QEArchiveNameSearch::createInternalWidgets ()
{
   this->setMinimumSize (512, 212);

   this->verticalLayout = new QVBoxLayout (this);
   this->verticalLayout->setSpacing (4);
   this->verticalLayout->setContentsMargins (2, 4, 2, 2);

   this->searchFrame = new QFrame (this);
   this->searchFrame->setMinimumSize (QSize(0, 40));
   this->searchFrame->setFrameShape (QFrame::StyledPanel);
   this->searchFrame->setFrameShadow (QFrame::Raised);

   this->horizontalLayout = new QHBoxLayout (searchFrame);
   this->horizontalLayout->setSpacing (8);
   this->horizontalLayout->setContentsMargins (6, 4, 6, 4);

   this->lineEdit = new QLineEdit (searchFrame);

   this->horizontalLayout->addWidget (lineEdit);

   this->searchButton = new QPushButton ("Search", searchFrame);
   this->searchButton->setMinimumSize (QSize (64, 0));
   this->searchButton->setMaximumSize (QSize (64, 16777215));

   this->horizontalLayout->addWidget (searchButton);

   this->clearButton = new QPushButton ("Clear", searchFrame);
   this->clearButton->setMinimumSize (QSize (64, 0));
   this->clearButton->setMaximumSize (QSize (64, 16777215));

   this->horizontalLayout->addWidget (clearButton);

   this->verticalLayout->addWidget (searchFrame);

   this->listWidget = new QListWidget (this);
   this->listWidget->setMinimumSize (QSize (500, 156));
   this->listWidget->setMaximumSize (QSize (1000, 16777215));

   QFont font;
   font.setFamily (QString::fromUtf8 ("Monospace"));

   this->listWidget->setFont (font);
   this->listWidget->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOn);
   this->listWidget->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
   this->listWidget->setSelectionMode (QAbstractItemView::ExtendedSelection);
   this->listWidget->setUniformItemSizes (true);

   this->verticalLayout->addWidget (listWidget);

   this->listWidget->setCurrentRow (-1);
}

// end
