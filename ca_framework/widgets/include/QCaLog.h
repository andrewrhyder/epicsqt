/*  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
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
 *    Ricardo Fernandes
 *  Contact details:
 *    ricardo.fernandes@synchrotron.org.au
 */

#ifndef QCALOG_H
#define QCALOG_H

#include <QWidget>
//#include <QDialog>
//#include <QTableView>
#include <QGroupBox>
#include <ContainerProfile.h>
#include <QLineEdit>
#include <QRadioButton>
#include <QGridLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QCaPluginLibrary_global.h>




class QCAPLUGINLIBRARYSHARED_EXPORT QCaLog:public QWidget, public ContainerProfile
{

    Q_OBJECT

    private:



    protected:
        QGridLayout *qGridLayout;
        QTableWidget *qTableWidget;
        QPushButton *qPushButtonClear;
        QPushButton *qPushButtonSave;


    public:

        QCaLog(QWidget *pParent = 0);
        virtual ~QCaLog(){}

        void setShowButtonClear(bool pValue);
        bool getShowButtonClear();

        void setShowButtonSave(bool pValue);
        bool getShowButtonSave();

        Q_PROPERTY(bool showButtonClear READ getShowButtonClear WRITE setShowButtonClear)

        Q_PROPERTY(bool showButtonSave READ getShowButtonSave WRITE setShowButtonSave)


    public slots:
        void buttonClearClicked();

        void buttonSaveClicked();


};





#endif /// QCALOG_H



