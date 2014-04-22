/*  QEForm.h
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
 *  Copyright (c) 2009, 2010
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef QEFORM_H
#define QEFORM_H

#include <QScrollArea>
#include <QEWidget.h>
#include <QWidget>
#include <QString>
#include <QLabel>
#include <QFileSystemWatcher>
#include <QEPluginLibrary_global.h>
#include <QCaVariableNamePropertyManager.h>
#include <QEActionRequests.h>

class QEPLUGINLIBRARYSHARED_EXPORT QEForm : public QWidget, public QEWidget
{
    Q_OBJECT
    public:

        QEForm( QWidget* parent = 0 );
        QEForm( const QString& uifileNameIn, QWidget* parent = 0 );
        void commonInit( const bool alertIfUINoFoundIn );

        virtual ~QEForm();

        QString getQEGuiTitle();     // Get the title to be used as the window or form title.
        QString getFullFileName();   // Get the standard, absolute UI file name
        QString getUiFileName();                // Get the fully substituted file name (Not the uiFile property)

        // Property convenience functions

        // Flag indicating form should handle gui form launch requests
        void setHandleGuiLaunchRequests( bool handleGuiLaunchRequests );
        bool getHandleGuiLaunchRequests();

        // Flag indicating form should resize contents to match form size (otherwise resize form to match contents)
        void setResizeContents( bool resizeContentsIn );
        bool getResizeContents();

        QString getContainedFrameworkVersion(); // Get the versino of the first QE widget (if any) of QE widgets by QUILoader
        QString getUniqueIdentifier(){ return uniqueIdentifier; }
        void setUniqueIdentifier( QString name ){ uniqueIdentifier = name; }
        void removeUi(){ if( ui ){ delete ui; ui = NULL; displayPlaceholder(false); } }

    public slots:
        bool readUiFile();

    private slots:
        void fileChanged ( const QString & path );
        // Note, in QEForm, the standard variable name mechanism is used for the UI file name
        void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex ) // !! move into Standard Properties section??
        {
            setVariableNameAndSubstitutions(variableNameIn, variableNameSubstitutionsIn, variableIndex);
        }
        void reloadLater(); // Slot for delaying form loading until after existing events have been processed

    protected:
        QString uiFileName; // As specified on creation
        QString fullUiFileName; // Full standard path
        // no implementation - void setVariableNameSubstitutions( QString variableNameSubstitutionsIn );
        bool handleGuiLaunchRequests;
        bool resizeContents;

    private:
        void establishConnection( unsigned int variableIndex );

        QString title;
        QWidget* ui;
        bool alertIfUINoFound;      // True if the UI file could not be read. No alert is required, for example, when a partial UI file name is being typed in Designer
        QFileSystemWatcher fileMon;

        void newMessage( QString msg, message_types type );
        void resizeEvent ( QResizeEvent * event );
        unsigned int childMessageFormId;

        QString containedFrameworkVersion;

        void saveConfiguration( PersistanceManager* pm );
        void restoreConfiguration( PersistanceManager* pm, restorePhases restorePhase );
        void reloadFile();

        void setWindowTitle( QString filename );    // Set the title to the name of the top level widget title, if it has one, or to the file name

        QString uniqueIdentifier; // Should be set up by an application using the persistance manager and creating more than one top level QEForm. Not required if only one QEForm is used.


        void paintEvent(QPaintEvent *event);                                    // When displaying a blank form, the form area is marked out on every update
        QLabel* placeholderLabel;                                               // Used to display a message when the QEForm could not be loaded with a .ui file
        void displayPlaceholder( bool display, QString message = QString() );   // Display or clear a message when the QEForm could not be loaded with a .ui file

        bool loadManually;                                                      // Set true when QEForm will be manually loaded by calling QEForm::readUiFile()

    public:
        // Note, a property macro in the form 'Q_PROPERTY(QString uiFileName READ ...' doesn't work.
        // A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
        Q_PROPERTY(QString uiFile READ getUiFileNameProperty WRITE setUiFileNameProperty)

        // Note, standard variable name and macros mechanism is used by QEForm for UI file name and marcos
        void    setUiFileNameProperty( QString uiFileName ){ variableNamePropertyManager.setVariableNameProperty( uiFileName ); }
        QString getUiFileNameProperty(){ return variableNamePropertyManager.getVariableNameProperty(); }


        Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)
        void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions ){ variableNamePropertyManager.setSubstitutionsProperty( variableNameSubstitutions ); }
        QString getVariableNameSubstitutionsProperty(){ return variableNamePropertyManager.getSubstitutionsProperty(); }

        Q_PROPERTY(bool handleGuiLaunchRequests READ getHandleGuiLaunchRequests WRITE setHandleGuiLaunchRequests)

        Q_PROPERTY(bool resizeContents READ getResizeContents WRITE setResizeContents)

        Q_PROPERTY(unsigned int messageSourceId READ getMessageSourceId WRITE setMessageSourceId )

        Q_ENUMS(MessageFilterOptions)
        Q_PROPERTY(MessageFilterOptions messageFormFilter READ getMessageFormFilter WRITE setMessageFormFilter )
        Q_PROPERTY(MessageFilterOptions messageSourceFilter READ getMessageSourceFilter WRITE setMessageSourceFilter )
        enum MessageFilterOptions { /* Any   = UserMessage::MESSAGE_FILTER_ANY, This would allow QEForm widgets to get in a message resend loop */
                                    Match = UserMessage::MESSAGE_FILTER_MATCH,
                                    None  = UserMessage::MESSAGE_FILTER_NONE };
        MessageFilterOptions getMessageFormFilter(){ return (MessageFilterOptions)getFormFilter(); }
        void setMessageFormFilter( MessageFilterOptions messageFormFilter ){ setFormFilter( (message_filter_options)messageFormFilter ); }
        MessageFilterOptions getMessageSourceFilter(){ return (MessageFilterOptions)getSourceFilter(); }
        void setMessageSourceFilter( MessageFilterOptions messageSourceFilter ){ setSourceFilter( (message_filter_options)messageSourceFilter ); }

      private:
        QCaVariableNamePropertyManager variableNamePropertyManager; // Note, this is only used to manage the macro substitutions that will be passed down to the form's QE widgets. The form has no variable name

};

#endif // QEFORM_H
