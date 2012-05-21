/*  This file is part of the EPICS QT Framework, initially developed at
 *  the Australian Synchrotron.
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

#include <QMessageBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QCaLog.h>
#include <ContainerProfile.h>
#include <QDebug>
#include <QFileDialog>


#include <iostream>
#include <fstream>
using namespace std;



QCaLog::QCaLog(QWidget *pParent):QWidget(pParent)
{

    qLayout = NULL;

    qTableWidget = new QTableWidget(this);
    qPushButtonClear = new QPushButton(this);
    qPushButtonSave = new QPushButton(this);

    qTableWidget->setColumnCount(3);
    qTableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("Time"));
    qTableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Type"));
    qTableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("Message"));
    qTableWidget->setToolTip("Current log messages");
    qTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    qTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    qTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //qTableWidget->verticalHeader()->setVisible(false);


    qColorInfo = new QColor(0, 0, 255);
    qColorWarning = new QColor(127, 127, 0);
    qColorError = new QColor(255, 0, 0);

    qPushButtonClear->setText("Clear");
    qPushButtonClear->setToolTip("Clear log messages");
    QObject::connect(qPushButtonClear, SIGNAL(clicked()), this, SLOT(buttonClearClicked()));

    qPushButtonSave->setText("Save");
    qPushButtonSave->setToolTip("Save log messages");
    QObject::connect(qPushButtonSave, SIGNAL(clicked()), this, SLOT(buttonSaveClicked()));

    setDetailsLayout(BOTTOM);

    clearLog();

    addLog(INFO, "This is the first line!");
    addLog(INFO, "This is the second line!");
    addLog(WARNING, "This is the third line!");
    addLog(ERROR, "This is the fourth line!");

}



void QCaLog::setShowColumnTime(bool pValue)
{

    qPushButtonClear->setVisible(pValue);

}



bool QCaLog::getShowColumnTime()
{

    return qPushButtonClear->isVisible();

}



void QCaLog::setShowColumnType(bool pValue)
{

    qPushButtonClear->setVisible(pValue);

}



bool QCaLog::getShowColumnType()
{

    return qPushButtonClear->isVisible();

}


void QCaLog::setShowColumnMessage(bool pValue)
{

    qPushButtonClear->setVisible(pValue);

}



bool QCaLog::getShowColumnMessage()
{

    return qPushButtonClear->isVisible();

}




void QCaLog::setShowButtonClear(bool pValue)
{

    qPushButtonClear->setVisible(pValue);

}



bool QCaLog::getShowButtonClear()
{

    return qPushButtonClear->isVisible();

}




void QCaLog::buttonClearClicked()
{

    if (QMessageBox::question(this, "Info", "Do you want to clear the log messages?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    {
        clearLog();
    }

}




void QCaLog::setShowButtonSave(bool pValue)
{

    qPushButtonSave->setVisible(pValue);

}



bool QCaLog::getShowButtonSave()
{

    return qPushButtonSave->isVisible();

}





void QCaLog::setDetailsLayout(int pValue)
{

    QLayout *layout;


    switch(pValue)
    {
        case TOP:
            if (qLayout)
            {
                delete qLayout;
            }
            detailsLayout = TOP;
            qLayout = new QGridLayout(this);
            layout = new QHBoxLayout();
            layout->addWidget(qPushButtonClear);
            layout->addWidget(qPushButtonSave);
            qLayout->addLayout(layout, 0, 0, 1, 2);
            qLayout->addWidget(qTableWidget, 1, 0);
            break;

        case BOTTOM:
            if (qLayout)
            {
                delete qLayout;
            }
            detailsLayout = BOTTOM;
            qLayout = new QGridLayout(this);
            layout = new QHBoxLayout();
            layout->addWidget(qPushButtonClear);
            layout->addWidget(qPushButtonSave);
            qLayout->addWidget(qTableWidget, 0, 0);
            qLayout->addLayout(layout, 1, 0, 1, 2);
            break;

        case LEFT:
            if (qLayout)
            {
                delete qLayout;
            }
            detailsLayout = LEFT;
            qLayout = new QGridLayout(this);
            layout = new QVBoxLayout();
            layout->addWidget(qPushButtonClear);
            layout->addWidget(qPushButtonSave);
            qLayout->addLayout(layout, 0, 0);
            qLayout->addWidget(qTableWidget, 0, 1);
            break;

        case RIGHT:
            if (qLayout)
            {
                delete qLayout;
            }
            detailsLayout = RIGHT;
            qLayout = new QGridLayout(this);
            layout = new QVBoxLayout();
            layout->addWidget(qPushButtonClear);
            layout->addWidget(qPushButtonSave);
            qLayout->addWidget(qTableWidget, 0, 0);
            qLayout->addLayout(layout, 0, 1);

    }

//    qDebug() << "setDetailsLayout() = " << detailsLayout;

}



int QCaLog::getDetailsLayout()
{

//    qDebug() << "getDetailsLayout() = " << detailsLayout;

    return detailsLayout;

}





void QCaLog::setInfoColor(QColor &pValue)
{

    //qColorInfo = pValue;

}



QColor QCaLog::getInfoColor()
{

    //return qColorInfo;

}



void QCaLog::setWarningColor(QColor &pValue)
{

    //qColorWarning = pValue;

}



QColor QCaLog::getWarningColor()
{

    //return &qColorWarning;

}



void QCaLog::setErrorColor(QColor &pValue)
{

    //qColorError = pValue;

}



QColor QCaLog::getErrorColor()
{

    //return qColorError;

}




void QCaLog::buttonSaveClicked()
{

    QString filename;
    ofstream fileStream;
    int i;

    filename = QFileDialog::getSaveFileName(this, "Save log messages", QString(), "Text (*.txt);All (*.*)");

    if (filename != "")
    {
        fileStream.open(filename.toUtf8().constData());
        if (fileStream.is_open())
        {
            for(i = 0; i < qTableWidget->rowCount(); i++)
            {
                fileStream << ((QString *) qTableWidget->cellWidget(i, 0))->toUtf8().constData() << "," << ((QString *) qTableWidget->cellWidget(i, 1))->toUtf8().constData() << "," << ((QString *) qTableWidget->cellWidget(i, 2))->toUtf8().constData() << "\n";
            }
            fileStream.close();
            QMessageBox::information(this, "Info", "The log messages were successfully saved in file '" + filename + "'!");
        }
        else
        {
            QMessageBox::critical(this, "Error", "Unable to save log messages in file '" + filename + "'!");
        }
    }


}





void QCaLog::clearLog()
{

    qTableWidget->setRowCount(0);

}



void QCaLog::addLog(int pType, QString pMessage)
{


    QTableWidgetItem *qTableWidgetItem;
    QString qString;
    QColor *qColor;
    int i;

    switch(pType)
    {
        case INFO:
            qColor = qColorInfo;
            break;

        case WARNING:
            qColor = qColorWarning;
            break;

        case ERROR:
            qColor = qColorError;
            break;

        default:
            qColor = NULL;
    }

    if (qColor)
    {
        i = qTableWidget->rowCount();
        qTableWidget->insertRow(i);

        qTableWidgetItem = new QTableWidgetItem("yyyy/mm/dd * hh:mm:ss");
//        qTableWidgetItem->setBackgroundColor(qColor);
        qTableWidget->setItem(i, 0, qTableWidgetItem);

        qTableWidgetItem = new QTableWidgetItem("INFO|WARNING|ERROR");
//        qTableWidgetItem->setBackgroundColor(qColor);
        qTableWidget->setItem(i, 1, qTableWidgetItem);

        qTableWidgetItem = new QTableWidgetItem(pMessage);
//        qTableWidgetItem->setBackgroundColor(qColor);
        qTableWidget->setItem(i, 2, qTableWidgetItem);

    }

}


