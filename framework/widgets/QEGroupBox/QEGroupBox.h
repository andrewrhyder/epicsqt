/*  QEGroupBox.h
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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef QEGROUPBOX_H
#define QEGROUPBOX_H

#include <QGroupBox>
#include <QCaWidget.h>
#include <QCaPluginLibrary_global.h>

/// The QEGroupBox class provides a minimalist extention to the QGroupBox class
/// in that it provides user level enabled and user level visibility control to
/// the group box but more significantly to all the widgets enclosed within the
/// QEGroupBox container also.
/// See QEFrame as well.
///
class QCAPLUGINLIBRARYSHARED_EXPORT QEGroupBox :
      public QGroupBox, public QCaWidget {

Q_OBJECT

    //=================================================================================
    // Standard properties
    // These properties should be identical for every widget using them.
    // WHEN MAKING CHANGES: search for STANDARDPROPERTIES and change all occurances.
    bool isEnabled() const { return getApplicationEnabled(); }
    void setEnabled( bool state ){ setApplicationEnabled( state ); }
    Q_PROPERTY(bool variableAsToolTip READ getVariableAsToolTip WRITE setVariableAsToolTip)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
    Q_PROPERTY(bool allowDrop READ getAllowDrop WRITE setAllowDrop)
    Q_PROPERTY(bool visible READ getRunVisible WRITE setRunVisible)
    Q_PROPERTY(unsigned int messageSourceId READ getMessageSourceId WRITE setMessageSourceId )
    Q_PROPERTY(QString userLevelUserStyle READ getStyleUser WRITE setStyleUser)
    Q_PROPERTY(QString userLevelScientistStyle READ getStyleScientist WRITE setStyleScientist)
    Q_PROPERTY(QString userLevelEngineerStyle READ getStyleEngineer WRITE setStyleEngineer)
    Q_ENUMS(UserLevels)
    Q_PROPERTY(UserLevels userLevelVisibility READ getUserLevelVisibilityProperty WRITE setUserLevelVisibilityProperty)
    Q_PROPERTY(UserLevels userLevelEnabled READ getUserLevelEnabledProperty WRITE setUserLevelEnabledProperty)
    //=================================================================================

public:
    explicit QEGroupBox (QWidget *parent = 0);
    virtual ~QEGroupBox ();
    QSize sizeHint () const;

};

#endif     /// QEGROUPBOX_H
