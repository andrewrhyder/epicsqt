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
 *  Copyright (c) 2011
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */


#include <QtDesigner>
#include <QtGui>

#include "QCaPeriodicPlugin.h"
#include "PeriodicSetupDialog.h"
#include "QCaPeriodicPluginTaskMenu.h"

QCaPeriodicPluginTaskMenu::QCaPeriodicPluginTaskMenu(QCaPeriodicPlugin *periodicPluginIn, QObject *parent)
    : QObject(parent)
{
    periodicPlugin = periodicPluginIn;

    editUserInfoAction = new QAction(tr("Edit User Info..."), this);
    connect(editUserInfoAction, SIGNAL(triggered()), this, SLOT(editUserInfo()));
}

void QCaPeriodicPluginTaskMenu::editUserInfo()
{
    PeriodicSetupDialog dialog(periodicPlugin);
    dialog.exec();
}

QAction *QCaPeriodicPluginTaskMenu::preferredEditAction() const
{
    return editUserInfoAction;
}

QList<QAction *> QCaPeriodicPluginTaskMenu::taskActions() const
{
    QList<QAction *> list;
    list.append(editUserInfoAction);
    return list;
}

QCaPeriodicPluginTaskMenuFactory::QCaPeriodicPluginTaskMenuFactory(QExtensionManager *parent)
    : QExtensionFactory(parent)
{
}

QObject *QCaPeriodicPluginTaskMenuFactory::createExtension(QObject *object,
                                                   const QString &iid,
                                                   QObject *parent) const
{
    if (iid != Q_TYPEID(QDesignerTaskMenuExtension))
        return 0;

    if (QCaPeriodicPlugin *pp = qobject_cast<QCaPeriodicPlugin*>(object))
        return new QCaPeriodicPluginTaskMenu(pp, parent);

    return 0;
}
