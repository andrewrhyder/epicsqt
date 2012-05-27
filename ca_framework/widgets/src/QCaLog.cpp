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
#include <QDateTime>
#include <QCaLog.h>
#include <ContainerProfile.h>
#include <QDebug>
#include <QFileDialog>
#include <QHeaderView>
#include <QSize>
#include <iostream>
#include <fstream>
using namespace std;



QCaLog::QCaLog(QWidget *pParent):QWidget(pParent), QCaWidget( this )
{

    QFont qFont;

    qTableWidget = new TableWidget(this);
    qPushButtonClear = new QPushButton(this);
    qPushButtonSave = new QPushButton(this);

    qTableWidget->setColumnCount(3);
    qTableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("Time"));
    qTableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Type"));
    qTableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("Message"));
    qTableWidget->setToolTip("Current log messages");
    qTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    qTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    qTableWidget->verticalHeader()->hide();

    qFont.setPointSize(9);
    qTableWidget->setFont(qFont);

    qPushButtonClear->setText("Clear");
    qPushButtonClear->setToolTip("Clear log messages");
    QObject::connect(qPushButtonClear, SIGNAL(clicked()), this, SLOT(buttonClearClicked()));

    qPushButtonSave->setText("Save");
    qPushButtonSave->setToolTip("Save log messages");
    QObject::connect(qPushButtonSave, SIGNAL(clicked()), this, SLOT(buttonSaveClicked()));

    qLayout = NULL;

    setInfoColor(QColor(0, 0, 255));
    setWarningColor(QColor(255, 160, 0));
    setErrorColor(QColor(255, 0, 0));
    setDetailsLayout(BOTTOM);

    clearLog();

    // Set up the UserMessage class to match on any messages from widgets on the same form by default
    setFormFilter( MESSAGE_FILTER_MATCH );
    setSourceFilter( MESSAGE_FILTER_NONE );

    // Set the form ID to use when matching the form of received message
    setChildFormId( getFormId() );

//    addLog(INFO, "This is the first line!");
//    addLog(INFO, "This is the second line!");
//    addLog(WARNING, "This is the third line!");
//    addLog(ERROR, "This is the fourth line!");

}



void QCaLog::setShowColumnTime(bool pValue)
{

    qTableWidget->setColumnHidden(0, pValue == false);
    qTableWidget->refreshSize();

}



bool QCaLog::getShowColumnTime()
{

    return (qTableWidget->isColumnHidden(0) == false);

}



void QCaLog::setShowColumnType(bool pValue)
{

    qTableWidget->setColumnHidden(1, pValue == false);
    qTableWidget->refreshSize();

}



bool QCaLog::getShowColumnType()
{

    return (qTableWidget->isColumnHidden(1) == false);

}


void QCaLog::setShowColumnMessage(bool pValue)
{

    qTableWidget->setColumnHidden(2, pValue == false);
    qTableWidget->refreshSize();

}



bool QCaLog::getShowColumnMessage()
{

    return (qTableWidget->isColumnHidden(2) == false);

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





void QCaLog::setInfoColor(QColor pValue)
{

    qColorInfo = pValue;
    refreshLog();

}



QColor QCaLog::getInfoColor()
{

    return qColorInfo;

}



void QCaLog::setWarningColor(QColor pValue)
{

    qColorWarning = pValue;
    refreshLog();

}



QColor QCaLog::getWarningColor()
{

    return qColorWarning;

}



void QCaLog::setErrorColor(QColor pValue)
{

    qColorError = pValue;
    refreshLog();

}



QColor QCaLog::getErrorColor()
{

    return qColorError;

}




void QCaLog::buttonSaveClicked()
{

    QFileDialog *qFileDialog;
    QString filename;
    ofstream fileStream;
    QString line;
    int i;

    qFileDialog = new QFileDialog(this, "Save log messages", QString());
    qFileDialog->setAcceptMode(QFileDialog::AcceptSave);

    if (qFileDialog->exec())
    {
        filename = qFileDialog->selectedFiles().at(0);
        fileStream.open(filename.toUtf8().constData());
        if (fileStream.is_open())
        {
            for(i = 0; i < qTableWidget->rowCount(); i++)
            {
                if (getShowColumnTime())
                {
                    line = qTableWidget->item(i, 0)->text();
                }
                else
                {
                    line = "";
                }
                if (getShowColumnType())
                {
                    if (line.isEmpty())
                    {
                        line = qTableWidget->item(i, 1)->text();
                    }
                    else
                    {
                        line += ", " + qTableWidget->item(i, 1)->text();
                    }
                }
                if (getShowColumnMessage())
                {
                    if (line.isEmpty())
                    {
                        line = qTableWidget->item(i, 2)->text();
                    }
                    else
                    {
                        line += ", " + qTableWidget->item(i, 2)->text();
                    }
                }
                fileStream << line.toUtf8().constData() << "\n";
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
    qPushButtonClear->setEnabled(false);
    qPushButtonSave->setEnabled(false);

}



void QCaLog::addLog(int pType, QString pMessage)
{

    QTableWidgetItem *qTableWidgetItem;
    QString type;
    QColor color;
    int i;


    switch(pType)
    {
        case INFO:
            type = "INFO";
            color = qColorInfo;
            break;

        case WARNING:
            type = "WARNING";
            color = qColorWarning;
            break;

        case ERROR:
            type = "ERROR";
            color = qColorError;
            break;

        default:
            type = "";
    }


    if (type.isEmpty() == false)
    {
        i = qTableWidget->rowCount();
        qTableWidget->insertRow(i);
        qTableWidgetItem = new QTableWidgetItem(QDateTime().currentDateTime().toString("yyyy/MM/dd - hh:mm:ss"));
        qTableWidgetItem->setTextColor(color);
        qTableWidget->setItem(i, 0, qTableWidgetItem);
        qTableWidgetItem = new QTableWidgetItem(type);
//        qTableWidgetItem->setTextAlignment(Qt::AlignCenter);
        qTableWidgetItem->setTextColor(color);
        qTableWidget->setItem(i, 1, qTableWidgetItem);
        qTableWidgetItem = new QTableWidgetItem(pMessage);
        qTableWidgetItem->setTextColor(color);
        qTableWidget->setItem(i, 2, qTableWidgetItem);
        qPushButtonClear->setEnabled(true);
        qPushButtonSave->setEnabled(true);
    }

}





void QCaLog::refreshLog()
{

    QTableWidgetItem *qTableWidgetItem;
    QColor color;
    int i;

    for(i = 0; i < qTableWidget->rowCount(); i++)
    {

        qTableWidgetItem = qTableWidget->item(i, 1);

        if (qTableWidgetItem->text() == "INFO")
        {
            color = qColorInfo;
        }
        else
        {
            if (qTableWidgetItem->text() == "WARNING")
            {
                color = qColorWarning;
            }
            else
            {
                color = qColorError;
            }
        }
        qTableWidgetItem->setTextColor(color);

        qTableWidgetItem = qTableWidget->item(i, 0);
        qTableWidgetItem->setTextColor(color);

        qTableWidgetItem = qTableWidget->item(i, 2);
        qTableWidgetItem->setTextColor(color);

    }

}



// Receive new log messages from other QCaWidgets.
void QCaLog::newMessage( QString msg, message_types type )
{
    // Add the message to the log
    addLog( type, msg );
}




TableWidget::TableWidget(QWidget *pParent):QTableWidget(pParent)
{

    initialized = false;

}




void TableWidget::refreshSize()
{

    int sizeColumn0;
    int sizeColumn1;
    int sizeColumn2;


    if (this->isColumnHidden(0))
    {
        if (this->isColumnHidden(1))
        {
            if (this->isColumnHidden(2))
            {
                sizeColumn0 = 0;
                sizeColumn1 = 0;
                sizeColumn2 = 0;
            }
            else
            {
                sizeColumn0 = 0;
                sizeColumn1 = 0;
                sizeColumn2 = this->width();
            }
        }
        else
        {
            if (this->isColumnHidden(2))
            {
                sizeColumn0 = 0;
                sizeColumn1 = this->width();
                sizeColumn2 = 0;
            }
            else
            {
                sizeColumn0 = 0;
                sizeColumn1 = 1 * this->width() / 5;
                sizeColumn2 = 4 * this->width() / 5 - 1;
            }
        }
    }
    else
    {
        if (this->isColumnHidden(1))
        {
            if (this->isColumnHidden(2))
            {
                sizeColumn0 = this->width();
                sizeColumn1 = 0;
                sizeColumn2 = 0;
            }
            else
            {
                sizeColumn0 = 1 * this->width() / 5;
                sizeColumn1 = 0;
                sizeColumn2 = 4 * this->width() / 5 - 1;
            }
        }
        else
        {
            if (this->isColumnHidden(2))
            {
                sizeColumn0 = this->width() / 2;
                sizeColumn1 = this->width() / 2 - 1;
                sizeColumn2 = 0;
            }
            else
            {
                sizeColumn0 = 1 * this->width() / 5;
                sizeColumn1 = 1 * this->width() / 5;
                sizeColumn2 = 3 * this->width() / 5 - 1;
            }
        }
    }

    this->setColumnWidth(0, sizeColumn0);
    this->setColumnWidth(1, sizeColumn1);
    this->setColumnWidth(2, sizeColumn2);

    //qDebug() << "inside refreshSize";

}




void TableWidget::resizeEvent(QResizeEvent *pEvent)
{

    if (initialized == false)
    {
        refreshSize();
        initialized = true;
    }

}


