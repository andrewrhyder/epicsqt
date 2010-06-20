/*!
  \class CmdPushButton
  \version $Revision: #2 $
  \date $DateTime: 2010/02/01 15:54:01 $
  \author Last checked in by: $Author: rhydera $
  \brief Command Push Button Widget.
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

#ifndef CMDPUSHBUTTON_H
#define CMDPUSHBUTTON_H

#include <QPushButton>
#include <ContainerProfile.h>

class CmdPushButton : public QPushButton {
    Q_OBJECT

  public:
    CmdPushButton( QWidget *parent = 0 );

    QString variableNameSubstitutions;


  public slots:
    void userClicked();

    void requestEnabled( const bool& state );


  protected:
    QString program;        /// Program to run
    QStringList arguments;  /// Program arguments

  private:
    void updateToolTip ( const QString & toolTip );
    ContainerProfile profile;
};

#endif /// CMDPUSHBUTTON_H
