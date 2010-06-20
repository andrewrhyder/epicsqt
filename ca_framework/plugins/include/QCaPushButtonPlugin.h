/*! 
  \class QCaPushButtonPlugin
  \version $Revision: #6 $
  \date $DateTime: 2010/02/01 15:54:01 $
  \author andrew.rhyder
  \brief CA Push Button Widget Plugin.
 */
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
 *    andrew.rhyder@synchrotron.org
 */

#ifndef QCAPUSHBUTTONPLUGIN_H
#define QCAPUSHBUTTONPLUGIN_H

#include <QTimer>
#include <QString>
#include <QCaPushButton.h>
#include <QCaVariableNamePropertyManager.h>
#include <QCaStringFormatting.h>

class QCaPushButtonPlugin : public QCaPushButton {
    Q_OBJECT

  public:
    /// Constructors
    QCaPushButtonPlugin( QWidget *parent = 0 );
    QCaPushButtonPlugin( QString variableNameProperty = "", QWidget *parent = 0 );

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

    /// Qt Designer Properties - write on press
    Q_PROPERTY(bool writeOnPress READ getWriteOnPressProperty WRITE setWriteOnPressProperty)
    void setWriteOnPressProperty( bool writeOnPress ){ QCaPushButton::writeOnPress = writeOnPress; }
    bool getWriteOnPressProperty(){ return QCaPushButton::writeOnPress; }

    /// Qt Designer Properties - write on release
    Q_PROPERTY(bool writeOnRelease READ getWriteOnReleaseProperty WRITE setWriteOnReleaseProperty)
    void setWriteOnReleaseProperty( bool writeOnRelease ){ QCaPushButton::writeOnRelease = writeOnRelease; }
    bool getWriteOnReleaseProperty(){ return QCaPushButton::writeOnRelease; }

    /// Qt Designer Properties - write on click
    Q_PROPERTY(bool writeOnClick READ getWriteOnClickProperty WRITE setWriteOnClickProperty)
    void setWriteOnClickProperty( bool writeOnClick ){ QCaPushButton::writeOnClick = writeOnClick; }
    bool getWriteOnClickProperty(){ return QCaPushButton::writeOnClick; }


    /// Qt Designer Properties - press value
    Q_PROPERTY(QString pressText READ getPressTextProperty WRITE setPressTextProperty)
    void setPressTextProperty( QString pressText ){ QCaPushButton::pressText = pressText; }
    QString getPressTextProperty(){ return QCaPushButton::pressText; }

    /// Qt Designer Properties - release value
    Q_PROPERTY(QString releaseText READ getReleaseTextProperty WRITE setReleaseTextProperty)
    void setReleaseTextProperty( QString releaseText ){ QCaPushButton::releaseText = releaseText; }
    QString getReleaseTextProperty(){ return QCaPushButton::releaseText; }

    /// Qt Designer Properties - click value
    Q_PROPERTY(QString clickText READ getClickTextProperty WRITE setClickTextProperty)
    void setClickTextProperty( QString clickText ){ QCaPushButton::clickText = clickText; }
    QString getClickTextProperty(){ return QCaPushButton::clickText; }

  private:
    QCaVariableNamePropertyManager variableNamePropertyManager;

  private slots:
    void useNewVariableNameProperty( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );
};

#endif /// QCAPUSHBUTTONPLUGIN_H
