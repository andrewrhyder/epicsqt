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

#ifndef LINKPLUGIN_H
#define LINKPLUGIN_H

#include <QELink.h>

/// LinkPlugin IS deprecated - use QELink.
class LinkPlugin : public QELink {
    Q_OBJECT

  public:
    LinkPlugin( QWidget *parent = 0 ) : QELink( parent ) {}

    // This deprecated widget is now just a shell around its replacement.
    // Its replacement does not recognise any properties defined in using a Q_ENUM in the original widget,
    // so these properties must still be implemented in the deprecated widget
    Q_ENUMS(ConditionNames)
    Q_PROPERTY(ConditionNames condition READ getConditionProperty WRITE setConditionProperty)
    enum ConditionNames { Equal              = QELink::CONDITION_EQ,
                          NotEqual           = QELink::CONDITION_NE,
                          GreaterThan        = QELink::CONDITION_GT,
                          GreaterThanOrEqual = QELink::CONDITION_GE,
                          LessThan           = QELink::CONDITION_LT,
                          LessThanOrEqual    = QELink::CONDITION_LE };
    void setConditionProperty( ConditionNames condition ){ setCondition( (QELink::conditions)condition ); }
    ConditionNames getConditionProperty(){ return (ConditionNames)getCondition(); }
};

#endif /// LINKPLUGIN_H
