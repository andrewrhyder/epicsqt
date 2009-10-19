/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/plugins/include/QCaPluginLibrary_global.h $
 * $Revision: #1 $ 
 * $DateTime: 2009/08/18 13:45:11 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \version $Revision: #1 $
  \date $DateTime: 2009/08/18 13:45:11 $
  \author <andrew.rhyder>
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
 *
 * Description:
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
