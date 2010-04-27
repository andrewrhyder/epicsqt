/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/widgets/include/ASguiForm.h $
 * $Revision: #6 $ 
 * $DateTime: 2010/04/07 12:14:19 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class ASguiForm
  \version $Revision: #6 $
  \date $DateTime: 2010/04/07 12:14:19 $
  \author andrew.rhyder
  \brief A base form GUI
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

#ifndef ASGUIFORM_H
#define ASGUIFORM_H

#include <QScrollArea>
#include <QWidget>
#include <UserMessage.h>
#include <QString>
#include <QFileSystemWatcher>

class ASguiForm : public QScrollArea
{
    Q_OBJECT
    public:

        ASguiForm( QWidget* parent = 0 );
        ASguiForm( const QString& uifileNameIn = "", QWidget* parent = 0 );
        void commonInit( const bool alertIfUINoFoundIn );

        ~ASguiForm();

        QString getASGuiTitle();        // Get the title to be used as the window or form title.
        QString getGuiFileName();       // Get the UI file name used to build the gui

        enum creationOptions { CREATION_OPTION_OPEN, CREATION_OPTION_NEW_TAB, CREATION_OPTION_NEW_WINDOW };

    public slots:
        void requestEnabled( const bool& state );
        void readUiFile();
        void onGeneralMessage( QString message );

    private slots:
        void onGuiLaunch( QString guiName, QString substitutions, creationOptions createOption );
        void fileChanged ( const QString & path );


    protected:
        QString uiFileNameProperty;
        void setVariableNameSubstitutions( QString variableNameSubstitutionsIn );
        bool handleGuiLaunchRequestsProperty;

    private:
        QString title;
        QWidget* ui;
        bool alertIfUINoFound;      // True if the UI file could not be read. No alert is required, for example, when a partial UI file name is being typed in Designer
        QFileSystemWatcher fileMon;

        QString variableNameSubstitutions;
        UserMessage userMessage;
        void generalMessage( QString message );
};

#endif // ASGUIFORM_H
