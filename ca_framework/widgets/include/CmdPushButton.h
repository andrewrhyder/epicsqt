/*!
  \class CmdPushButton
  \version $Revision: #2 $
  \date $DateTime: 2010/02/01 15:54:01 $
  \author Last checked in by: $Author: rhydera $
  \brief Command Push Button Widget.
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
