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
#include <QGroupBox>
#include <ContainerProfile.h>
#include <QLineEdit>
#include <QRadioButton>
#include <QGridLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QCaPluginLibrary_global.h>


enum details
{
    TOP,
    BOTTOM,
    LEFT,
    RIGHT
};



enum messageTypes
{
    INFO,
    WARNING,
    ERROR
};


class QCAPLUGINLIBRARYSHARED_EXPORT QCaLog:public QWidget, public ContainerProfile
{

    Q_OBJECT

    private:



    protected:
        QGridLayout *qLayout;
        QTableWidget *qTableWidget;
        QPushButton *qPushButtonClear;
        QPushButton *qPushButtonSave;
        QColor qColorInfo;
        QColor qColorWarning;
        QColor qColorError;
        int detailsLayout;


    public:

        QCaLog(QWidget *pParent = 0);
        virtual ~QCaLog(){}

        void setShowColumnTime(bool pValue);
        bool getShowColumnTime();

        void setShowColumnType(bool pValue);
        bool getShowColumnType();

        void setShowColumnMessage(bool pValue);
        bool getShowColumnMessage();

        void setShowButtonClear(bool pValue);
        bool getShowButtonClear();

        void setShowButtonSave(bool pValue);
        bool getShowButtonSave();

        void setDetailsLayout(int pValue);
        int getDetailsLayout();

        void setInfoColor(QColor pValue);
        QColor getInfoColor();

        void setWarningColor(QColor pValue);
        QColor getWarningColor();

        void setErrorColor(QColor pValue);
        QColor getErrorColor();

        void clearLog();

        void addLog(int pType, QString pMessage);

        void refreshLog();



        Q_PROPERTY(bool showColumnTime READ getShowColumnTime WRITE setShowColumnTime)

        Q_PROPERTY(bool showColumnType READ getShowColumnType WRITE setShowColumnType)

        Q_PROPERTY(bool showColumnMessage READ getShowColumnMessage WRITE setShowColumnMessage)

        Q_PROPERTY(bool showButtonClear READ getShowButtonClear WRITE setShowButtonClear)

        Q_PROPERTY(bool showButtonSave READ getShowButtonSave WRITE setShowButtonSave)


        Q_ENUMS(detailsLayoutProperty)
        Q_PROPERTY(detailsLayoutProperty detailsLayout READ getDetailsLayoutProperty WRITE setDetailsLayoutProperty)
        enum detailsLayoutProperty
        {
            Top = TOP,
            Bottom = BOTTOM,
            Left = LEFT,
            Right = RIGHT
        };

        void setDetailsLayoutProperty(detailsLayoutProperty pDetailsLayout)
        {
            setDetailsLayout((detailsLayoutProperty) pDetailsLayout);
        }
        detailsLayoutProperty getDetailsLayoutProperty()
        {
            return (detailsLayoutProperty) getDetailsLayout();
        }


        Q_PROPERTY(QColor infoColor READ getInfoColor WRITE setInfoColor)

        Q_PROPERTY(QColor warningColor READ getWarningColor WRITE setWarningColor)

        Q_PROPERTY(QColor errorColor READ getErrorColor WRITE setErrorColor)


    private slots:
        void buttonClearClicked();

        void buttonSaveClicked();


};





#endif /// QCALOG_H



