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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef QCALABELPLUGIN_H
#define QCALABELPLUGIN_H

#include <QELabel.h>

// QCaLabelPlugin IS deprecated - use QELabel.
class QCaLabelPlugin : public QELabel {
    Q_OBJECT

  public:
    QCaLabelPlugin( QWidget *parent = 0 ) : QELabel( parent ) {}

    // This deprecated widget is now just a shell around its replacement.
    // Its replacement does not recognise any properties defined in using a Q_ENUM in the original widget,
    // so these properties must still be implemented in the deprecated widget
    enum Formats { Default          = QEStringFormatting::FORMAT_DEFAULT,
                   Floating         = QEStringFormatting::FORMAT_FLOATING,
                   Integer          = QEStringFormatting::FORMAT_INTEGER,
                   UnsignedInteger  = QEStringFormatting::FORMAT_UNSIGNEDINTEGER,
                   Time             = QEStringFormatting::FORMAT_TIME,
                   LocalEnumeration = QEStringFormatting::FORMAT_LOCAL_ENUMERATE };
    void setFormatProperty( Formats format ){ setFormat( (QEStringFormatting::formats)format ); }
    Formats getFormatProperty(){ return (Formats)getFormat(); }
    Q_ENUMS(Formats)
    Q_PROPERTY(Formats format READ getFormatProperty WRITE setFormatProperty)

    enum Notations { Fixed = QEStringFormatting::NOTATION_FIXED,
                     Scientific   = QEStringFormatting::NOTATION_SCIENTIFIC,
                     Automatic      = QEStringFormatting::NOTATION_AUTOMATIC };
    void setNotationProperty( Notations notation ){ setNotation( (QEStringFormatting::notations)notation ); }
    Notations getNotationProperty(){ return (Notations)getNotation(); }
    Q_ENUMS(Notations)
    Q_PROPERTY(Notations notation READ getNotationProperty WRITE setNotationProperty)

    enum ArrayActions { Append = QEStringFormatting::APPEND,
                        Ascii  = QEStringFormatting::ASCII,
                        Index  = QEStringFormatting::INDEX };
    void setArrayActionProperty( ArrayActions arrayAction ){ setArrayAction( (QEStringFormatting::arrayActions)arrayAction ); }
    ArrayActions getArrayActionProperty(){ return (ArrayActions)getArrayAction(); }
    Q_ENUMS(ArrayActions)
    Q_PROPERTY(ArrayActions arrayAction READ getArrayActionProperty WRITE setArrayActionProperty)

    Q_ENUMS(UpdateOptions)
    Q_PROPERTY(UpdateOptions updateOption READ getUpdateOptionProperty WRITE setUpdateOptionProperty)
    enum UpdateOptions { Text     = QELabel::UPDATE_TEXT,
                         Picture  = QELabel::UPDATE_PIXMAP };
    void setUpdateOptionProperty( UpdateOptions updateOption ){ QELabel::setUpdateOptionProperty( (QELabel::UpdateOptions)updateOption ); }
    UpdateOptions getUpdateOptionProperty(){ return (UpdateOptions)QELabel::getUpdateOptionProperty(); }

};

#endif // QCALABELPLUGIN_H