/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/plugins/include/LinkPlugin.h $
 * $Revision: #3 $
 * $DateTime: 2010/02/18 15:15:02 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class LinkPlugin
  \version $Revision: #3 $
  \date $DateTime: 2010/02/18 15:15:02 $
  \author andrew.rhyder
  \brief Link Widget Plugin for designer.
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

#ifndef LINKPLUGIN_H
#define LINKPLUGIN_H

#include <QString>
#include <Link.h>

class LinkPlugin : public Link {
    Q_OBJECT

  public:
    /// Constructors
    LinkPlugin( QWidget *parent = 0 );

    /// Qt Designer Properties - condition
    Q_ENUMS(Conditions)
    Q_PROPERTY(Conditions condition READ getConditionProperty WRITE setConditionProperty)
    enum Conditions { Equal              = Link::CONDITION_EQ,
                      NotEqual           = Link::CONDITION_NE,
                      GreaterThan        = Link::CONDITION_GT,
                      GreaterThanOrEqual = Link::CONDITION_GE,
                      LessThan           = Link::CONDITION_LT,
                      LessThanOrEqual    = Link::CONDITION_LE };
    void setConditionProperty( Conditions condition ){ conditionProperty = (Link::conditions)condition; }
    Conditions getConditionProperty(){ return (Conditions)conditionProperty; }

    /// Qt Designer Properties - comparisonValue Value to compare input signals to
    Q_PROPERTY(QString comparisonValue READ getComparisonValueProperty WRITE setComparisonValueProperty)
    void    setComparisonValueProperty( QString comparisonValue ){ comparisonValueProperty = QVariant(comparisonValue); }
    QString getComparisonValueProperty(){ return comparisonValueProperty.toString(); }

    /// Qt Designer Properties - signalTrue (Signal if condition is met)
    Q_PROPERTY(bool signalTrue READ getSignalTrueProperty WRITE setSignalTrueProperty)
    void setSignalTrueProperty( bool signalTrue ){ signalTrueProperty = signalTrue; }
    bool getSignalTrueProperty(){ return signalTrueProperty; }

    /// Qt Designer Properties - signalFalse (Signal if condition not met)
    Q_PROPERTY(bool signalFalse READ getSignalFalseProperty WRITE setSignalFalseProperty)
    void setSignalFalseProperty( bool signalFalse ){ signalFalseProperty = signalFalse; }
    bool getSignalFalseProperty(){ return signalFalseProperty; }

    /// Qt Designer Properties - outTrueValue Value to emit if condition is met
    Q_PROPERTY(QString outTrueValue READ getOutTrueValueProperty WRITE setOutTrueValueProperty)
    void    setOutTrueValueProperty( QString outTrueValue ){ outTrueValueProperty = QVariant(outTrueValue); }
    QString getOutTrueValueProperty(){ return outTrueValueProperty.toString(); }

    /// Qt Designer Properties - outFalseValue Value to emit if condition is not met
    Q_PROPERTY(QString outFalseValue READ getOutFalseValueProperty WRITE setOutFalseValueProperty)
    void    setOutFalseValueProperty( QString outFalseValue ){ outFalseValueProperty = QVariant(outFalseValue); }
    QString getOutFalseValueProperty(){ return outFalseValueProperty.toString(); }

    /// Qt Designer Properties - visible (widget is visible outside 'Designer')
    Q_PROPERTY(bool visible READ getVisibleProperty WRITE setVisibleProperty)
    // implemented in Link class void setVisibleProperty( bool visible ){ visibleProperty = visible; }
    bool getVisibleProperty(){ return visibleProperty; }


};

#endif /// LINKPLUGIN_H
