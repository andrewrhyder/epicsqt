/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/data/include/QCaEventFilter.h $
 * $Revision: #1 $
 * $DateTime: 2009/07/14 15:59:56 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class QCaEventFilter
  \version $Revision: #1 $
  \date $DateTime: 2009/07/14 15:59:56 $
  \author andrew.rhyder
  \brief CA event filter for QT.
 */

/* Copyright (c) 2009 Australian Synchrotron
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * Licence as published by the Free Software Foundation; either
 * version 2.1 of the Licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public Licence for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * Licence along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Contact details:
 * andrew.rhyder@synchrotron.org.au
 * 800 Blackburn Road, Clayton, Victoria 3168, Australia.
 *
 */

#ifndef QCAEVENTFILTER_H
#define QCAEVENTFILTER_H

#include <QtDebug>
#include <QObject>
#include <QMutex>

/*
    This class is simply a reference to an object that has installed an event filter (an instance of class QCaEventFilter)
    This class exists so that a list of references to QCaEventFilter instances can be maintained.
*/
class QCaInstalledFiltersListItem {
  public:
    QCaInstalledFiltersListItem( QObject* eventObjectIn ) { eventObject = eventObjectIn; referenceCount = 1; }
    /// QObject that an event filter has been added to
    QObject* eventObject;
    /// Count of how many QCaObjects expect the event filter to be on the QObject
    long referenceCount;
};

/*!
    This class allows instances of QCaObject to recieve events posted to them by foriegn CA threads.
    Each event recipient creates an instance of this class. This class, however, manages only a single
    event filter for all instances of itself.
*/
class QCaEventFilter : public QObject {
    Q_OBJECT
  public:
    /// Add a fitler to an object
    void addFilter( QObject* objectIn );
    /// Remove an event filter from an object
    void deleteFilter( QObject* objectIn );
    /// Event filter added to a QObject
    bool eventFilter( QObject *watched, QEvent *e );

  private:
    /// Used to protect access to installedFilters
    QMutex installedFiltersLock;
    /// List of QObjects the event filter is added to
    QList<QCaInstalledFiltersListItem> installedFilters;
};

#endif /// QCAEVENTFILTER_H
