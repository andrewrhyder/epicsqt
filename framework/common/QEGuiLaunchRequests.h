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
#include <QEPluginLibrary_global.h>


// Class defining a window to create
class windowCreationListItem;

class QEPLUGINLIBRARYSHARED_EXPORT QEGuiLaunchRequests {
public:

   // Type of request
   enum Kinds { KindNone,            // no action (default, not valid in any request)
                KindOpenFile,        // by file name
                KindOpenFiles,       // by file names
                KindAction };        // Inbuilt application action


   enum Options { OptionOpen,        // How new windows created for the request are to be presented. May not be relevent for all requests
                  OptionNewTab,
                  OptionNewWindow,
                  OptionTopDockWindow,
                  OptionBottomDockWindow,
                  OptionLeftDockWindow,
                  OptionRightDockWindow,
                  OptionFloatingDockWindow };

   // Constructors.
   //
   QEGuiLaunchRequests ();

   // Action
   QEGuiLaunchRequests (const QString& action,
                        const QString& pvName);


   // .ui file name plus create option
   QEGuiLaunchRequests (const QString &filename,
                        const QString &config,
                        const Options optionIn);

   // a set of .ui files
   QEGuiLaunchRequests( const QList<windowCreationListItem> windowsIn );

   // set and get functions
   //
   void setKind (const Kinds kindIn);
   Kinds getKind () const;

   void setArguments (const QStringList & argumentsIn);
   QStringList getArguments () const;

   void setOption (const Options optionIn);
   Options getOption () const;

   QString getCustomisation() const;

   QList<windowCreationListItem> getWindows() const;

private:
   Kinds kind;
   QStringList arguments;
   Options option;
   QString customisation;  // Window configuration (menus, buttons, etc)
   QList<windowCreationListItem> windows;
};

class windowCreationListItem
{
public:
    windowCreationListItem(){ hidden = false; creationOption = QEGuiLaunchRequests::OptionNewWindow; }
    windowCreationListItem(windowCreationListItem* item ){ uiFile = item->uiFile;
                                                           macroSubstitutions = item->macroSubstitutions;
                                                           customisationName = item->customisationName;
                                                           creationOption = item->creationOption;
                                                           hidden = item->hidden; }
    QString uiFile;
    QString macroSubstitutions;
    QString customisationName;
    QEGuiLaunchRequests::Options creationOption;
    bool hidden;
};


Q_DECLARE_METATYPE (QEGuiLaunchRequests)

# endif // QEGUILAUNCHREQUESTS_H
