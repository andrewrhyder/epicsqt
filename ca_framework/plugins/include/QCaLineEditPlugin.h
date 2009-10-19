/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/plugins/include/QCaLineEditPlugin.h $
 * $Revision: #4 $
 * $DateTime: 2009/07/30 14:33:44 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class QCaLineEditPlugin
  \version $Revision: #4 $
  \date $DateTime: 2009/07/30 14:33:44 $
  \author andrew.rhyder
  \brief CA Line Edit Widget Plugin.
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

#ifndef QCALINEEDITPLUGIN_H
#define QCALINEEDITPLUGIN_H

#include <QTimer>
#include <QString>
#include <QCaLineEdit.h>
#include <QCaVariableNamePropertyManager.h>
#include <QCaStringFormatting.h>

class QCaLineEditPlugin : public QCaLineEdit {
    Q_OBJECT

  public:
    /// Constructors
    QCaLineEditPlugin( QWidget *parent = 0 );
    QCaLineEditPlugin( QString variableName = "", QWidget *parent = 0 );

    /// Qt Designer Properties - Variable Name
    /// Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
    /// A property name ending with 'Name' results in some sort of string a variable being displayed, but will only accept alphanumeric and won't generate callbacks on change.
    Q_PROPERTY(QString variable READ getVariableNameProperty WRITE setVariableNameProperty);
    void    setVariableNameProperty( QString variableName ){ variableNamePropertyManager.setVariableNameProperty( variableName ); }
    QString getVariableNameProperty(){ return variableNamePropertyManager.getVariableNameProperty(); }

    /// Qt Designer Properties - variable substitutions Example: $SECTOR=01 will result in any occurance of $SECTOR in variable name being replaced with 01.
    Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutionsProperty WRITE setVariableNameSubstitutionsProperty)
    void    setVariableNameSubstitutionsProperty( QString variableNameSubstitutions ){ variableNamePropertyManager.setSubstitutionsProperty( variableNameSubstitutions ); }
    QString getVariableNameSubstitutionsProperty(){ return variableNamePropertyManager.getSubstitutionsProperty(); }

    /// Qt Designer Properties - write on lose focus
    Q_PROPERTY(bool writeOnLoseFocus READ getWriteOnLoseFocusProperty WRITE setWriteOnLoseFocusProperty)
    void setWriteOnLoseFocusProperty( bool writeOnLoseFocus ){ writeOnLoseFocusProperty = writeOnLoseFocus; }
    bool getWriteOnLoseFocusProperty(){ return writeOnLoseFocusProperty; }

    /// Qt Designer Properties - write on enter
    Q_PROPERTY(bool writeOnEnter READ getWriteOnEnterProperty WRITE setWriteOnEnterProperty)
    void setWriteOnEnterProperty( bool writeOnEnter ){ writeOnEnterProperty = writeOnEnter; }
    bool getWriteOnEnterProperty(){ return writeOnEnterProperty; }

    /// Qt Designer Properties - write on change
    Q_PROPERTY(bool writeOnChange READ getWriteOnChangeProperty WRITE setWriteOnChangeProperty)
    void setWriteOnChangeProperty( bool writeOnChange ){ writeOnChangeProperty = writeOnChange; }
    bool getWriteOnChangeProperty(){ return writeOnChangeProperty; }

    /// Qt Designer Properties - subscribe
    Q_PROPERTY(bool subscribe READ getSubscribeProperty WRITE setSubscribeProperty)
    void setSubscribeProperty( bool subscribe ){ subscribeProperty = subscribe; }
    bool getSubscribeProperty(){ return subscribeProperty; }

    /// Qt Designer Properties - variable as tool tip
    Q_PROPERTY(bool variableAsToolTip READ getVariableAsToolTipProperty WRITE setVariableAsToolTipProperty)
    void setVariableAsToolTipProperty( bool variableAsToolTip ){ variableAsToolTipProperty = variableAsToolTip; }
    bool getVariableAsToolTipProperty(){ return variableAsToolTipProperty; }

    /// Qt Designer Properties - enabled (override of widget enabled)
    Q_PROPERTY(bool enabled READ getEnabledProperty WRITE setEnabledProperty)
    void setEnabledProperty( bool enabled ){ setEnabled( enabled ); }
    bool getEnabledProperty(){ return enabledProperty; }

    /// String formatting properties

    /// Qt Designer Properties - precision
    Q_PROPERTY(unsigned int precision READ getPrecisionProperty WRITE setPrecisionProperty)
    void setPrecisionProperty( unsigned int precision ){ stringFormatting.setPrecision( precision ); }
    unsigned int getPrecisionProperty(){ return stringFormatting.getPrecision(); }

    /// Qt Designer Properties - useDbPrecision
    Q_PROPERTY(bool useDbPrecision READ getUseDbPrecisionProperty WRITE setUseDbPrecisionProperty)
    void setUseDbPrecisionProperty( bool useDbPrecision ){ stringFormatting.setUseDbPrecision( useDbPrecision); }
    bool getUseDbPrecisionProperty(){ return stringFormatting.getUseDbPrecision(); }

    /// Qt Designer Properties - leadingZero
    Q_PROPERTY(bool leadingZero READ getLeadingZeroProperty WRITE setLeadingZeroProperty)
    void setLeadingZeroProperty( bool leadingZero ){ stringFormatting.setLeadingZero( leadingZero ); }
    bool getLeadingZeroProperty(){ return stringFormatting.getLeadingZero(); }

    /// Qt Designer Properties - trailingZeros
    Q_PROPERTY(bool trailingZeros READ getTrailingZerosProperty WRITE setTrailingZerosProperty)
    void setTrailingZerosProperty( bool trailingZeros ){ stringFormatting.setTrailingZeros( trailingZeros ); }
    bool getTrailingZerosProperty(){ return stringFormatting.getTrailingZeros(); }

    /// Qt Designer Properties - addUnits
    Q_PROPERTY(bool addUnits READ getAddUnitsProperty WRITE setAddUnitsProperty)
    void setAddUnitsProperty( bool addUnits ){ stringFormatting.setAddUnits( addUnits ); }
    bool getAddUnitsProperty(){ return stringFormatting.getAddUnits(); }

    /// Qt Designer Properties - format
    Q_ENUMS(Formats)
    Q_PROPERTY(Formats format READ getFormatProperty WRITE setFormatProperty)
    enum Formats { Default         = QCaStringFormatting::FORMAT_DEFAULT,
                   Floating        = QCaStringFormatting::FORMAT_FLOATING,
                   Integer         = QCaStringFormatting::FORMAT_INTEGER,
                   UnsignedInteger = QCaStringFormatting::FORMAT_UNSIGNEDINTEGER,
                   Time            = QCaStringFormatting::FORMAT_TIME };
    void setFormatProperty( Formats format ){ stringFormatting.setFormat( (QCaStringFormatting::formats)format ); }
    Formats getFormatProperty(){ return (Formats)stringFormatting.getFormat(); }

    /// Qt Designer Properties - radix
    Q_PROPERTY(unsigned int radix READ getRadixProperty WRITE setRadixProperty)
    void setRadixProperty( unsigned int radix ){ stringFormatting.setRadix( radix); }
    unsigned int getRadixProperty(){ return stringFormatting.getRadix(); }

    /// Qt Designer Properties - notation
    Q_ENUMS(Notations)
    Q_PROPERTY(Notations notation READ getNotationProperty WRITE setNotationProperty)
    enum Notations { Fixed = QCaStringFormatting::NOTATION_FIXED,
                     Scientific   = QCaStringFormatting::NOTATION_SCIENTIFIC,
                     Automatic      = QCaStringFormatting::NOTATION_AUTOMATIC };
    void setNotationProperty( Notations notation ){ stringFormatting.setNotation( (QCaStringFormatting::notations)notation ); }
    Notations getNotationProperty(){ return (Notations)stringFormatting.getNotation(); }


  private:
    QCaVariableNamePropertyManager variableNamePropertyManager;

  private slots:
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );
};

#endif /// QCALINEEDITPLUGIN_H
