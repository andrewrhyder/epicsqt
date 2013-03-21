/*  QEFrameworkVersion.h
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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 *
 */

#ifndef QEFRAMEWORKVERSION_H
#define QEFRAMEWORKVERSION_H

#include <QString>
#include <QEPluginLibrary_global.h>

#define STRINGIFY2(s)         #s
#define STRINGIFY(s)          STRINGIFY2(s)

#define QE_VERSION_MAJOR      2
#define QE_VERSION_MINOR      4
#define QE_VERSION_RELEASE    0

#define QE_VERSION_STRING     STRINGIFY(QE_VERSION_MAJOR) "." \
                              STRINGIFY(QE_VERSION_MINOR) "." \
                              STRINGIFY(QE_VERSION_RELEASE)

#define QE_VERSION_DATE_TIME  __DATE__ " " __TIME__

// Provides values used to build shared library.
//
class QEPLUGINLIBRARYSHARED_EXPORT QEFrameworkVersion {
public:
   explicit QEFrameworkVersion();
   virtual ~QEFrameworkVersion();

   static unsigned int getMajor ();
   static unsigned int getMinor ();
   static unsigned int getRelease ();
   static const QString getString ();
   static const QString getDateTime ();
};

#endif // QEFRAMEWORKVERSION_H
