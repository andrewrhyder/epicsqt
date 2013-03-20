/*  QEStripChartContextMenu.cpp
 *
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
 *  Copyright (c) 2013
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#include <QVariant>
#include <QDebug>
#include "QEStripChartContextMenu.h"

//------------------------------------------------------------------------------
//
QEStripChartContextMenu::QEStripChartContextMenu (QWidget *parent) : QMenu (parent)
{
   QMenu *menu;

   this->setTitle ("PV Item");

   // Note: action items are not enabled until corresponding functionallity is implemented.
   //
   this->make (this, "Read Archive",                        false, SCCM_READ_ARCHIVE);

   menu = new QMenu ("Scale chart to this PV's", this);
   this->addMenu (menu);
   this->make (menu, "HOPR/LOPR values",                    false, SCCM_SCALE_CHART_AUTO)->setEnabled (false);
   this->make (menu, "Plotted min/max values",              false, SCCM_SCALE_CHART_PLOTTED)->setEnabled (false);
   this->make (menu, "Buffered min/max values",             false, SCCM_SCALE_CHART_BUFFERED)->setEnabled (false);


   menu = new QMenu ("Adjust/Scale PV", this);
   this->addMenu (menu);
   this->make (menu, "General...",                          false, SCCM_SCALE_PV_GENERAL)->setEnabled (false);
   this->make (menu, "Reset",                               false, SCCM_SCALE_PV_RESET)->setEnabled (false);
   this->make (menu, "HOPR/LOPR values map to chart range", false, SCCM_SCALE_PV_AUTO)->setEnabled (false);
   this->make (menu, "Plotted values map to chart range",   false, SCCM_SCALE_PV_PLOTTED)->setEnabled (false);
   this->make (menu, "Buffered values map to chart range",  false, SCCM_SCALE_PV_BUFFERED)->setEnabled (false);
   this->make (menu, "First value maps to chart centre",    false, SCCM_SCALE_PV_CENTRE)->setEnabled (false);

   menu = new QMenu ("Mode", this);
   this->addMenu (menu);
   this->make (menu, "Rectangular",                         false, SCCM_PLOT_RECTANGULAR)->setEnabled (false);
   this->make (menu, "Smooth",                              false, SCCM_PLOT_SMOOTH)->setEnabled (false);
   this->make (menu, "User PV Process Time",                false, SCCM_PLOT_SERVER_TIME)->setEnabled (false);
   this->make (menu, "Use Receive Time",                    false, SCCM_PLOT_CLIENT_TIME)->setEnabled (false);
   menu->addSeparator();
   this->make (menu, "Linear",                              false, SCCM_ARCH_LINEAR)->setEnabled (false);
   this->make (menu, "Plot Binning",                        false, SCCM_ARCH_PLOTBIN)->setEnabled (false);
   this->make (menu, "Raw",                                 false, SCCM_ARCH_RAW)->setEnabled (false);
   this->make (menu, "Spread Sheet",                        false, SCCM_ARCH_SHEET)->setEnabled (false);
   this->make (menu, "Averaged",                            false, SCCM_ARCH_AVERAGED)->setEnabled (false);


   menu = new QMenu ("Line", this);
   this->addMenu (menu);
   this->make (menu, "Hide",                                true,  SCCM_LINE_HIDE)->setEnabled (false);
   this->make (menu, "Regular",                             true,  SCCM_LINE_REGULAR)->setEnabled (false);
   this->make (menu, "Bold",                                true,  SCCM_LINE_BOLD)->setEnabled (false);
   this->make (menu, "Colour...",                           false, SCCM_LINE_COLOUR);

   this->make (this, "Edit PV Name...",                     false, SCCM_PV_EDIT_NAME);

   this->make (this, "Write PV trace to file...",           false, SCCM_PV_WRITE_TRACE)->setEnabled (false);

   this->make (this, "Generate Statistics",                 false, SCCM_PV_STATS)->setEnabled (false);

   this->make (this, "Clear",                               false, SCCM_PV_CLEAR);

   QObject::connect (this, SIGNAL (triggered             (QAction* ) ),
                     this, SLOT   (contextMenuTriggered  (QAction* )));
}

//------------------------------------------------------------------------------
//
QEStripChartContextMenu::~QEStripChartContextMenu ()
{
   // no special action
}

//------------------------------------------------------------------------------
//
void QEStripChartContextMenu::contextMenuTriggered (QAction* selectedItem)
{
   Options option;

   option = Options (selectedItem->data ().toInt ());
   if ((option > SCCM_NONE) && (option < SCCM_LAST)) {
      emit this->contextMenuSelected (option);
   }
}

//------------------------------------------------------------------------------
//
QAction* QEStripChartContextMenu::make (QMenu *parent,
                                        const QString & caption,
                                        const bool checkable,
                                        const Options option)
{
   QAction* action;

   action = new QAction (caption, parent);
   action->setCheckable (checkable);
   action->setData (QVariant (int (option)));
   parent->addAction (action);
   return action;
}

// end
