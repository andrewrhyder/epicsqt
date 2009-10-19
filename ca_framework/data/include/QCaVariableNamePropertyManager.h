/* $File: //ASP/Dev/SBS/4_Controls/4_8_GUI_Frameworks/4_8_2_Qt/sw/ca_framework/data/include/QCaVariableNamePropertyManager.h $
 * $Revision: #1 $
 * $DateTime: 2009/07/14 15:59:56 $
 * Last checked in by: $Author: rhydera $
 */

/*! 
  \class QCaVariableNamePropertyManager
  \version $Revision: #1 $
  \date $DateTime: 2009/07/14 15:59:56 $
  \author andrew.rhyder
  \brief Handles variable name sets.
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

#ifndef QCAVARIABLENAMEPROPERTYMANAGER_H
#define QCAVARIABLENAMEPROPERTYMANAGER_H

#include <QTimer>
#include <QString>

class QCaVariableNamePropertyManager : public QTimer {
    Q_OBJECT

  public:
    QCaVariableNamePropertyManager();

    QString getVariableNameProperty();
    void setVariableNameProperty( QString variableNamePropertyIn );

    QString getSubstitutionsProperty();
    void setSubstitutionsProperty( QString substitutionsPropertyIn );

    void setVariableIndex( unsigned int variableIndexIn );

signals:
    void newVariableNameProperty( QString variable, QString Substitutions, unsigned int variableIndex );

private slots:
    void subscribeDelayExpired();       // Called a short time after a user stops typing in 'designer'


private:
    QString variableNameProperty;
    QString substitutionsProperty;

    unsigned int variableIndex;     // Index into the list of variable names maintained by a QCa widget
    bool interactive;   // If set, there is a user typing new variable names and macro substitutions. Use timers to wait for typing to finish.
};

#endif /// QCAVARIABLENAMEPROPERTYMANAGER_H
