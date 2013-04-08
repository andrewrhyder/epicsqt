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

#ifndef QEToolTip_H
#define QEToolTip_H

#include <QWidget>
#include <QEString.h>

class QEToolTip
{
    public:
        QEToolTip( QWidget* ownerIn );
        virtual ~QEToolTip(){}
        void updateToolTipVariable ( const QString& variable );
        void updateToolTipAlarm ( const QString& alarm );
        void updateToolTipConnection ( bool connection );

        void setVariableAsToolTip( bool variableAsToolTip );
        bool getVariableAsToolTip();

    private:
        bool variableAsToolTip;         // Flag the tool tip should be set to the variable name
        void displayToolTip();
        QString toolTipVariable;
        QString toolTipAlarm;
        bool isConnected;
        QWidget* owner;
};

#endif // QEToolTip_H
