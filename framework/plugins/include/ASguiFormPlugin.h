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

#ifndef ASGUIFORMPLUGIN_H
#define ASGUIFORMPLUGIN_H

#include <QString>

#include <ASguiForm.h>
#include <QCaVariableNamePropertyManager.h>

/// Wrapper around ASguiForm to make it a Qt Designer Plugin
class ASguiFormPlugin : public ASguiForm
{
    Q_OBJECT

public:
    /// Constructors
    ASguiFormPlugin( QWidget *parent = 0 );

    /// Note, a property macro in the form 'Q_PROPERTY(QString uiFileName READ ...' doesn't work.
    /// A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
    Q_PROPERTY(QString uiFile READ getUiFileName WRITE setUiFileName);

    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)
    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions ){ variableNamePropertyManager.setSubstitutionsProperty( variableNameSubstitutions ); }
    QString getVariableNameSubstitutionsProperty(){ return variableNamePropertyManager.getSubstitutionsProperty(); }

    Q_PROPERTY(bool handleGuiLaunchRequests READ getHandleGuiLaunchRequests WRITE setHandleGuiLaunchRequests)

    Q_PROPERTY(unsigned int messageSourceId READ getMessageSourceId WRITE setMessageSourceId )

    Q_ENUMS(MessageFilterOptions)
    Q_PROPERTY(MessageFilterOptions messageFormFilter READ getMessageFormFilter WRITE setMessageFormFilter )
    Q_PROPERTY(MessageFilterOptions messageSourceFilter READ getMessageSourceFilter WRITE setMessageSourceFilter )
    enum MessageFilterOptions { /* Any   = UserMessage::MESSAGE_FILTER_ANY, This would allow ASguiForm widgets to get in a message resend loop */
                                Match = UserMessage::MESSAGE_FILTER_MATCH,
                                None  = UserMessage::MESSAGE_FILTER_NONE };
    MessageFilterOptions getMessageFormFilter(){ return (MessageFilterOptions)getFormFilter(); }
    void setMessageFormFilter( MessageFilterOptions messageFormFilter ){ setFormFilter( (message_filter_options)messageFormFilter ); }
    MessageFilterOptions getMessageSourceFilter(){ return (MessageFilterOptions)getSourceFilter(); }
    void setMessageSourceFilter( MessageFilterOptions messageSourceFilter ){ setSourceFilter( (message_filter_options)messageSourceFilter ); }

  private:
    QCaVariableNamePropertyManager variableNamePropertyManager; // Note, this is only used to manage the macro substitutions that will be passed down to the form's QCa widgets. The form has no varialbe name


  private slots:
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );
};

#endif // ASGUIFORMPLUGIN_H
