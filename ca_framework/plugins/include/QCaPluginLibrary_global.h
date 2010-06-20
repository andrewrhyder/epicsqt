/*!
  \class
  \version $Revision: #1 $
  \date $DateTime: 2009/08/18 13:45:11 $
  \author <andrew.rhyder>
  \brief
 */
/*
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
 *  Copyright (c) 2009, 2010
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org
 */

/* Description:
 *
 * Manage the declaration of library public functions as either exported functions
 * when building the library, or imported functions when using the library.
 *
 */

#ifndef QCAPLUGINLIBRARY_GLOBAL_H
#define QCAPLUGINLIBRARY_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QCAPLUGIN_LIBRARY)
#  define QCAPLUGINLIBRARYSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QCAPLUGINLIBRARYSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QCAPLUGINLIBRARY_GLOBAL_H
