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

#ifndef QCAPUSHBUTTONPLUGIN_H
#define QCAPUSHBUTTONPLUGIN_H

#include <QEPushButton.h>

// QCaPushButtonPlugin IS deprecated - use QEPushButton.
class QCaPushButtonPlugin : public QEPushButton {
    Q_OBJECT

  public:
    QCaPushButtonPlugin( QWidget *parent = 0 ) : QEPushButton( parent ) {}

    // This deprecated widget is now just a shell around its replacement.
    // Its replacement does not recognise any properties defined in using a Q_ENUM in the original widget,
    // so these properties must still be implemented in the deprecated widget
    Q_ENUMS(UpdateOptions)
    Q_PROPERTY(UpdateOptions updateOption READ getUpdateOptionProperty WRITE setUpdateOptionProperty)
    enum UpdateOptions { Text        = QEPushButton::UPDATE_TEXT,
                         Icon        = QEPushButton::UPDATE_ICON,
                         TextAndIcon = QEPushButton::UPDATE_TEXT_AND_ICON,
                         State       = QEPushButton::UPDATE_STATE };
    void setUpdateOptionProperty( UpdateOptions updateOption ){ setUpdateOption( (QEPushButton::updateOptions)updateOption ); }
    UpdateOptions getUpdateOptionProperty(){ return (UpdateOptions)getUpdateOption(); }

    Q_ENUMS(Formats)
    Q_PROPERTY(Formats format READ getFormatProperty WRITE setFormatProperty)
    enum Formats { Default         = QEStringFormatting::FORMAT_DEFAULT,
                   Floating        = QEStringFormatting::FORMAT_FLOATING,
                   Integer         = QEStringFormatting::FORMAT_INTEGER,
                   UnsignedInteger = QEStringFormatting::FORMAT_UNSIGNEDINTEGER,
                   Time            = QEStringFormatting::FORMAT_TIME,
                   LocalEnumeration = QEStringFormatting::FORMAT_LOCAL_ENUMERATE };
    void setFormatProperty( Formats format ){ setFormat( (QEStringFormatting::formats)format ); }
    Formats getFormatProperty(){ return (Formats)getFormat(); }

    Q_ENUMS(Notations)
    Q_PROPERTY(Notations notation READ getNotationProperty WRITE setNotationProperty)
    enum Notations { Fixed      = QEStringFormatting::NOTATION_FIXED,
                     Scientific = QEStringFormatting::NOTATION_SCIENTIFIC,
                     Automatic  = QEStringFormatting::NOTATION_AUTOMATIC };
    void setNotationProperty( Notations notation ){ setNotation( (QEStringFormatting::notations)notation ); }
    Notations getNotationProperty(){ return (Notations)getNotation(); }

    Q_ENUMS(CreationOptionNames)
    Q_PROPERTY(CreationOptionNames creationOption READ getCreationOptionProperty WRITE setCreationOptionProperty)
    enum CreationOptionNames { Open = QEActionRequests::OptionOpen,
                               NewTab = QEActionRequests::OptionNewTab,
                               NewWindow = QEActionRequests::OptionNewWindow };
    void setCreationOptionProperty( CreationOptionNames creationOptionIn ){ setCreationOption( (QEActionRequests::Options)creationOptionIn ); }
    CreationOptionNames getCreationOptionProperty(){ return (CreationOptionNames)getCreationOption(); }

};

#endif // QCAPUSHBUTTONPLUGIN_H
