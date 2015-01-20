/*  QEToolTip.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2009, 2010 Australian Synchrotron
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef QEToolTip_H
#define QEToolTip_H

#include <QWidget>
#include <QString>
#include <QStringList>

class QEToolTip
{
    public:
        QEToolTip( QWidget* ownerIn );
        virtual ~QEToolTip(){}
        void setNumberToolTipVariables ( const unsigned int number );
        void updateToolTipVariable ( const QString& variable, const unsigned int variableIndex );
        void updateToolTipAlarm ( const QString& alarm, const unsigned int variableIndex );
        void updateToolTipConnection ( bool connection, const unsigned int variableIndex = 0 );
        void updateToolTipCustom ( const QString& custom );

        void setVariableAsToolTip( const bool variableAsToolTip );
        bool getVariableAsToolTip() const;

    private:
        bool variableAsToolTip;         // Flag the tool tip should be set to the variable name
        void displayToolTip();
        unsigned int number;
        QStringList toolTipVariable;
        QStringList toolTipAlarm;
        QList<bool> toolTipIsConnected;
        QString toolTipCustom;          // Custion tool tip extra for specific widget types
        QWidget* owner;
};

#endif // QEToolTip_H
