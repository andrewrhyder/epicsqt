/*  QEGuiLaunchRequests.h
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
 *
 */

#ifndef QEGUILAUNCHREQUESTS_H
#define QEGUILAUNCHREQUESTS_H

#include <QString>
#include <QStringList>
#include <QVariant>

/* Note: Still under devlopment and not acutually used yet.
 */

class QEGuiLaunchRequests {
public:

   enum Kinds { KindNone,            // no action
                KindFileName,        // by file name
                KindStripChart,      // application's strip chart
                KindPvProperties };  // application's pv properties

   enum Options { OptionOpen,
                  OptionNewtab,
                  OptionNewWindow };


   // Constructors.
   //
   QEGuiLaunchRequests ();

   QEGuiLaunchRequests (const Kinds kindIn,
                        const QStringList &argumentsIn,
                        const Options optionIn);

   // Equivilent to original QEForm::launchGui request (ui file name plus create option)
   //
   QEGuiLaunchRequests (const QString &filename,
                        const Options optionIn);

   // set and get functions
   //
   void setKind (const Kinds kindIn);
   Kinds getKind () const;

   void setArguments (const QStringList & argumentsIn);
   QStringList getArguments () const;

   void setOption (const Options optionIn);
   Options getOption () const;

private:
   Kinds kind;
   QStringList arguments;
   Options option;
};

Q_DECLARE_METATYPE (QEGuiLaunchRequests)

# endif // QEGUILAUNCHREQUESTS_H
