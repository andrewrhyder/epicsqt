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
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef QEFORM_H
#define QEFORM_H

#include <QScrollArea>
#include <QEWidget.h>
#include <QWidget>
#include <QString>
#include <QFileSystemWatcher>
#include <QEPluginLibrary_global.h>
#include <QCaVariableNamePropertyManager.h>

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

        enum creationOptions { CREATION_OPTION_OPEN, CREATION_OPTION_NEW_TAB, CREATION_OPTION_NEW_WINDOW };

        // Property convenience functions

        // Variable name related properties (for a form no variable name will be set, only the substitutions that will be passed on to widgets within the form)
        void setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );

        // UI file name
        void    setUiFileName( QString uiFile );
        QString getUiFileName();

        // Flag indicating form should handle gui form launch requests
        void setHandleGuiLaunchRequests( bool handleGuiLaunchRequests );
        bool getHandleGuiLaunchRequests();

        // Flag indicating form should resize contents to match form size (otherwise resize form to match contents)
        void setResizeContents( bool resizeContentsIn );
        bool getResizeContents();

        QString getContainedFrameworkVersion(); // Get the versino of the first QE widget (if any) of QE widgets by QUILoader

    public slots:
        bool readUiFile();

        void launchGui( QString guiName, QEForm::creationOptions createOption );

    private slots:
        void fileChanged ( const QString & path );
        void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex ) // !! move into Standard Properties section??
        {
            // Note, for a form, variable name is not used. Substitutions still are
            setVariableNameAndSubstitutions(variableNameIn, variableNameSubstitutionsIn, variableIndex);
        }

    protected:
        QString uiFileName; // As specified on creation
        QString fullUiFileName; // Full standard path
        void setVariableNameSubstitutions( QString variableNameSubstitutionsIn );
        bool handleGuiLaunchRequests;
        bool resizeContents;

    private:
        QString title;
        QWidget* ui;
        bool alertIfUINoFound;      // True if the UI file could not be read. No alert is required, for example, when a partial UI file name is being typed in Designer
        QFileSystemWatcher fileMon;

        QString variableNameSubstitutions;

        void newMessage( QString msg, message_types type );
        void resizeEvent ( QResizeEvent * event );
        unsigned int childMessageFormId;

        QString containedFrameworkVersion;

        void saveConfiguration( PersistanceManager* pm );
        void restoreConfiguration( PersistanceManager* pm );


    public:
        // Note, a property macro in the form 'Q_PROPERTY(QString uiFileName READ ...' doesn't work.
        // A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
        Q_PROPERTY(QString uiFile READ getUiFileName WRITE setUiFileName)

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
        QCaVariableNamePropertyManager variableNamePropertyManager; // Note, this is only used to manage the macro substitutions that will be passed down to the form's QE widgets. The form has no varialbe name

};

#endif // QEFORM_H
