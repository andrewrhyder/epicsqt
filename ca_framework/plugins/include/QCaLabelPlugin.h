/*! 
  \class QCaLabelPlugin
  \version $Revision: #8 $
  \date $DateTime: 2010/02/18 15:15:02 $
  \author andrew.rhyder
  \brief CA Label Widget Plugin for designer.
 */
/*
 *  This file is part of the EPICS QT Framework.
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
 *    andrew@rhyder.org
 */

#ifndef QCALABELPLUGIN_H
#define QCALABELPLUGIN_H

#include <QTimer>
#include <QString>
#include <QCaLabel.h>
#include <QCaVariableNamePropertyManager.h>
#include <QCaStringFormatting.h>

class QCaLabelPlugin : public QCaLabel {
    Q_OBJECT

  public:
    /// Constructors
    QCaLabelPlugin( QWidget *parent = 0 );
    QCaLabelPlugin( QString variableNameProperty = "", QWidget *parent = 0 );

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

    /// Qt Designer Properties - localEnumeration
    Q_PROPERTY(QString/*localEnumerationList*/ localEnumeration READ getLocalEnumerationProperty WRITE setLocalEnumerationProperty)
    void setLocalEnumerationProperty( QString/*localEnumerationList*/ localEnumeration ){ stringFormatting.setLocalEnumeration( localEnumeration ); }
    QString/*localEnumerationList*/ getLocalEnumerationProperty(){ return stringFormatting.getLocalEnumeration(); }

    /// Qt Designer Properties - format
    Q_ENUMS(Formats)
    Q_PROPERTY(Formats format READ getFormatProperty WRITE setFormatProperty)
    enum Formats { Default          = QCaStringFormatting::FORMAT_DEFAULT,
                   Floating         = QCaStringFormatting::FORMAT_FLOATING,
                   Integer          = QCaStringFormatting::FORMAT_INTEGER,
                   UnsignedInteger  = QCaStringFormatting::FORMAT_UNSIGNEDINTEGER,
                   Time             = QCaStringFormatting::FORMAT_TIME,
                   LocalEnumeration = QCaStringFormatting::FORMAT_LOCAL_ENUMERATE };
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

    /// Qt Designer Properties - visible (widget is visible outside 'Designer')
    Q_PROPERTY(bool visible READ getVisibleProperty WRITE setVisibleProperty)
    // implemented in QCaLabel class void setVisibleProperty( bool visible ){ visibleProperty = visible; }
    bool getVisibleProperty(){ return visibleProperty; }


  private:
    QCaVariableNamePropertyManager variableNamePropertyManager;

  private slots:
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );
};

#endif /// QCALABELPLUGIN_H
