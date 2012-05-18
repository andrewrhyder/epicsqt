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



QCaLog::QCaLog(QWidget *pParent):QWidget(pParent)
{

    qGridLayout = new QGridLayout(this);
    qTableWidget = new QTableWidget(this);
    qPushButtonClear = new QPushButton(this);
    qPushButtonSave = new QPushButton(this);


    qTableWidget->setColumnCount(3);
    qTableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("Time"));
    qTableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Type"));
    qTableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("Message"));
    qTableWidget->setToolTip("Current log messages");

    qTableWidget->setRowCount(4);

    qPushButtonClear->setText("Clear");
    qPushButtonClear->setToolTip("Clear log messages");
    QObject::connect(qPushButtonClear, SIGNAL(clicked()), this, SLOT(buttonClearClicked()));

    qPushButtonSave->setText("Save");
    qPushButtonSave->setToolTip("Save log messages");
    QObject::connect(qPushButtonSave, SIGNAL(clicked()), this, SLOT(buttonSaveClicked()));

    qGridLayout->addWidget(qTableWidget, 0, 0, 1, 2);
    qGridLayout->addWidget(qPushButtonClear, 1, 0);
    qGridLayout->addWidget(qPushButtonSave, 1, 1);


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
        qTableWidget->setRowCount(0);
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


void QCaLog::buttonSaveClicked()
{

    QString filename;

    filename = QFileDialog::getSaveFileName(this, "Save log messages", QString(), "Text (*.txt);All (*.*)");

//    qDebug() << filename;

}



