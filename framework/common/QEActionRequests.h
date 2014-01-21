/*  QEActionRequests.h
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

#ifndef QEActionRequests_H
#define QEActionRequests_H

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QEPluginLibrary_global.h>
#include <QAction>


// Forward class declarations
class windowCreationListItem;
class componentHostListItem;

// Class defining an action an application should carry out on behalf od a QE Widget it has created.
class QEPLUGINLIBRARYSHARED_EXPORT QEActionRequests {
public:

   // Type of request
   enum Kinds { KindNone,            // no action (default, not valid in any request)
                KindOpenFile,        // by file name
                KindOpenFiles,       // by file names
                KindAction,          // inbuilt application action
                KindWidgetAction,    // inbuilt QE widget action
                KindHostComponents };// application to host a widget on behalf of a QE widget. For example a QEImage widget can create a profile plot in a QFrame and either display it within itself or ask the application to host it is a dock window.


   enum Options { OptionOpen,        // How new windows created for the request are to be presented. May not be relevent for all requests
                  OptionNewTab,
                  OptionNewWindow,
                  OptionTopDockWindow,
                  OptionBottomDockWindow,
                  OptionLeftDockWindow,
                  OptionRightDockWindow,
                  OptionTopDockWindowTabbed,
                  OptionBottomDockWindowTabbed,
                  OptionLeftDockWindowTabbed,
                  OptionRightDockWindowTabbed,
                  OptionFloatingDockWindow };

   // Constructors.
   //
   QEActionRequests ();

   // Action (for the application)
   QEActionRequests (const QString& action,
                     const QString& pvName);


   // .ui file name plus create option
   QEActionRequests (const QString &filename,
                     const QString &config,
                     const Options optionIn);

   // a set of windows to create
   QEActionRequests( const QList<windowCreationListItem> windowsIn );

   // Action (for a QE widget)
   QEActionRequests( const QString& actionIn,
                     const QString& widgetNameIn,
                     const QStringList& argumentsIn,
                     const bool& initialiseIn,
                     QAction* originator );

   // single component to host for a QE widget
   QEActionRequests( const componentHostListItem& componentIn );

   // a set of components to host for a QE widget
   QEActionRequests( const QList<componentHostListItem>& componentsIn );

   // set and get functions
   //
   void setKind (const Kinds kindIn);
   Kinds getKind () const;

   void setArguments (const QStringList & argumentsIn);
   QStringList getArguments () const;

   void setOption (const Options optionIn);
   Options getOption () const;

   void setAction( const QString actionIn );
   QString getAction() const;

   void setWidgetName(const QString widgetNameIn );
   QString getWidgetName() const;

   void setInitialise (const bool initialiseIn );
   bool getInitialise() const;

   void setOriginator( QAction* originatorIn );
   QAction* getOriginator() const;

   QString getCustomisation() const;

   QList<windowCreationListItem> getWindows() const;
   QList<componentHostListItem> getComponents() const;

   static bool isDockCreationOption( const Options createOption );          // Return true if creation option creates a dock
   static bool isTabbedDockCreationOption( const Options createOption );    // Return true if creation option creates a tabbed dock

private:
   Kinds kind;
   QString action;
   QStringList arguments;
   Options option;
   QString customisation;  // Window configuration (menus, buttons, etc)
   QList<windowCreationListItem> windows;
   QList<componentHostListItem> components;
   QString widgetName;
   bool initialise;     // If true, initial preperation to do this action, don't actually do it. For example, set initial checked state of menu item
   QAction* originator; // A copy would be safer???
};

// Class to hold window creation instructions
class windowCreationListItem
{
public:
    windowCreationListItem(){ hidden = false; creationOption = QEActionRequests::OptionNewWindow; }
    windowCreationListItem(windowCreationListItem* item ){ uiFile = item->uiFile;
                                                           macroSubstitutions = item->macroSubstitutions;
                                                           customisationName = item->customisationName;
                                                           creationOption = item->creationOption;
                                                           hidden = item->hidden;
                                                           title = item->title; }
    QString                   uiFile;               // .UI file to open when this item is actioned
    QString                   macroSubstitutions;   // Macro substitutions to apply when this item is actioned
    QString                   customisationName;    // Customisation name to apply to any main windows created when this item is actioned
    QEActionRequests::Options creationOption;       // Creation option defining how the UI file is presented (in a new window, a tabbed dock, etc)
    bool                      hidden;               // If true, any new dock is created hidden
    QString                   title;                // Title of this menu item
};

// Class to hold component hosting instructions.
// (an application can host a widget on behalf of a QE widget.
//  For example a QEImage widget can create a profile plot in a QFrame and either
//  display it within itself or ask the application to host it is a dock window.)
class componentHostListItem
{
public:
    componentHostListItem(){ widget = NULL; hidden = false; creationOption = QEActionRequests::OptionFloatingDockWindow; }
    componentHostListItem( QWidget*                  widgetIn,
                           QEActionRequests::Options creationOptionIn,
                           bool                      hiddenIn,
                           QString                   titleIn )
                            {
                                widget = widgetIn;
                                creationOption = creationOptionIn;
                                hidden = hiddenIn;
                                title = titleIn;
                            }

    componentHostListItem(componentHostListItem* item ){ widget = item->widget;
                                                         creationOption = item->creationOption;
                                                         hidden = item->hidden;
                                                         title = item->title; }
    QWidget*                  widget;
    QEActionRequests::Options creationOption;
    bool                      hidden;
    QString                   title;
};

Q_DECLARE_METATYPE (QEActionRequests)

# endif // QEActionRequests_H
