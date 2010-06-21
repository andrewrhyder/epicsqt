/*!
  \class GuiPushButton
  \version $Revision: #2 $
  \date $DateTime: 2010/02/01 15:54:01 $
  \author Last checked in by: $Author: rhydera $
  \brief Launch GUI Push Button Widget.
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
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef GUIPUSHBUTTON_H
#define GUIPUSHBUTTON_H

#include <QPushButton>
#include <ASguiForm.h>
#include <UserMessage.h>
#include <ContainerProfile.h>

class GuiPushButton : public QPushButton {
    Q_OBJECT

  public:
    GuiPushButton( QWidget *parent = 0 );

    QString variableNameSubstitutions;
    ASguiForm::creationOptions creationOption;


  public slots:
    void userClicked();
    void onGeneralMessage( QString message );

    void requestEnabled( const bool& state );


private slots:
    void launchGui( QString guiName, QString substitutions, ASguiForm::creationOptions creationOption );

  signals:
    void newGui( QString guiName, QString substitutions, ASguiForm::creationOptions creationOption );

  protected:
    QString guiName;      /// GUI file name to launch

  private:
    void updateToolTip ( const QString & toolTip );
    UserMessage userMessage;
    ContainerProfile profile;

};

#endif /// GUIPUSHBUTTON_H
