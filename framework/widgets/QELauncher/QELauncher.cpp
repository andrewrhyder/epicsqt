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

#include <QELauncher.h>



// ============================================================
//  QELAUNCHER METHODS
// ============================================================
QELauncher::QELauncher(QWidget *pParent):QWidget(pParent), QCaWidget( this )
{

    QFont qFont;

    qlineEditDirectoryPath = new QLineEdit(this);
    qPushButtonDirectoryBrowser = new QPushButton(this);
    qPushButtonRefresh = new QPushButton(this);
    qTableWidgetLauncher = new _QTableWidgetLauncher(this);

    qlineEditDirectoryPath->setToolTip("Specify the directory where to browse for files");
    QObject::connect(qlineEditDirectoryPath, SIGNAL(textChanged(QString)), this, SLOT(lineEditDirectoryPathChanged(QString)));

    qPushButtonDirectoryBrowser->setText("...");
    qPushButtonDirectoryBrowser->setToolTip("Browse for a directory");
    QObject::connect(qPushButtonDirectoryBrowser, SIGNAL(clicked()), this, SLOT(buttonDirectoryBrowserClicked()));

    qPushButtonRefresh->setText("Refresh");
    qPushButtonRefresh->setToolTip("Refresh file browse result");
    QObject::connect(qPushButtonRefresh, SIGNAL(clicked()), this, SLOT(buttonRefreshClicked()));

    qTableWidgetLauncher->setColumnCount(3);
    qTableWidgetLauncher->setHorizontalHeaderItem(0, new QTableWidgetItem("Time"));
    qTableWidgetLauncher->setHorizontalHeaderItem(1, new QTableWidgetItem("Size"));
    qTableWidgetLauncher->setHorizontalHeaderItem(2, new QTableWidgetItem("Filename"));
    qTableWidgetLauncher->setToolTip("Files contained in the specified directory");
    qTableWidgetLauncher->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qTableWidgetLauncher->setSelectionBehavior(QAbstractItemView::SelectRows);
    qTableWidgetLauncher->setSelectionMode(QAbstractItemView::SingleSelection);
    qTableWidgetLauncher->verticalHeader()->hide();
    qFont.setPointSize(9);
    qTableWidgetLauncher->setFont(qFont);
    QObject::connect(qTableWidgetLauncher, SIGNAL(itemActivated(QTableWidgetItem *)), this, SLOT(itemActivated(QTableWidgetItem *)));

    setShowFileExtension(true);
    setFileFilter("");
    setDetailsLayout(TOP);

}




void QELauncher::setDirectoryPath(QString pValue)
{

    qlineEditDirectoryPath->setText(pValue);

}



QString QELauncher::getDirectoryPath()
{

    return qlineEditDirectoryPath->text();

}





void QELauncher::setShowDirectoryPath(bool pValue)
{

    qlineEditDirectoryPath->setVisible(pValue);

}



bool QELauncher::getShowDirectoryPath()
{

    return qlineEditDirectoryPath->isVisible();

}




void QELauncher::setShowDirectoryBrowser(bool pValue)
{

    qPushButtonDirectoryBrowser->setVisible(pValue);

}



bool QELauncher::getShowDirectoryBrowser()
{

    return qPushButtonDirectoryBrowser->isVisible();

}



void QELauncher::setShowRefresh(bool pValue)
{

    qPushButtonRefresh->setVisible(pValue);

}



bool QELauncher::getShowRefresh()
{

    return qPushButtonRefresh->isVisible();

}



void QELauncher::setFileFilter(QString pValue)
{

    fileFilter = pValue;
    updateTable();

}



QString QELauncher::getFileFilter()
{

    return fileFilter;

}




void QELauncher::setShowColumnTime(bool pValue)
{

    qTableWidgetLauncher->setColumnHidden(0, pValue == false);
    qTableWidgetLauncher->refreshSize();

}



bool QELauncher::getShowColumnTime()
{

    return (qTableWidgetLauncher->isColumnHidden(0) == false);

}



void QELauncher::setShowColumnSize(bool pValue)
{

    qTableWidgetLauncher->setColumnHidden(1, pValue == false);
    qTableWidgetLauncher->refreshSize();

}



bool QELauncher::getShowColumnSize()
{

    return (qTableWidgetLauncher->isColumnHidden(1) == false);

}


void QELauncher::setShowColumnFilename(bool pValue)
{

    qTableWidgetLauncher->setColumnHidden(2, pValue == false);
    qTableWidgetLauncher->refreshSize();

}



bool QELauncher::getShowColumnFilename()
{

    return (qTableWidgetLauncher->isColumnHidden(2) == false);

}



void QELauncher::setShowFileExtension(bool pValue)
{

    showFileExtension = pValue;
    updateTable();

}



bool QELauncher::getShowFileExtension()
{

    return showFileExtension;

}



void QELauncher::setDetailsLayout(int pValue)
{

    QLayout *qLayoutMain;
    QLayout *qLayoutChild;


    delete layout();

    switch(pValue)
    {
        case TOP:
            detailsLayout = TOP;
            qLayoutMain = new QVBoxLayout(this);
            qLayoutChild = new QHBoxLayout();
            qLayoutChild->addWidget(qlineEditDirectoryPath);
            qLayoutChild->addWidget(qPushButtonDirectoryBrowser);
            qLayoutChild->addWidget(qPushButtonRefresh);
            qLayoutMain->addItem(qLayoutChild);
            qLayoutMain->addWidget(qTableWidgetLauncher);
            break;

        case BOTTOM:
            detailsLayout = BOTTOM;
            qLayoutMain = new QVBoxLayout(this);
            qLayoutMain->addWidget(qTableWidgetLauncher);
            qLayoutChild = new QHBoxLayout();
            qLayoutChild->addWidget(qlineEditDirectoryPath);
            qLayoutChild->addWidget(qPushButtonDirectoryBrowser);
            qLayoutChild->addWidget(qPushButtonRefresh);
            qLayoutMain->addItem(qLayoutChild);
            break;

        case LEFT:
            detailsLayout = LEFT;
            qLayoutMain = new QHBoxLayout(this);
            qLayoutChild = new QVBoxLayout();
            qLayoutChild->addWidget(qlineEditDirectoryPath);
            qLayoutChild->addWidget(qPushButtonDirectoryBrowser);
            qLayoutChild->addWidget(qPushButtonRefresh);
            qLayoutMain->addItem(qLayoutChild);
            qLayoutMain->addWidget(qTableWidgetLauncher);
            break;

        case RIGHT:
            detailsLayout = RIGHT;
            qLayoutMain = new QHBoxLayout(this);
            qLayoutChild = new QVBoxLayout();
            qLayoutChild->addWidget(qlineEditDirectoryPath);
            qLayoutChild->addWidget(qPushButtonDirectoryBrowser);
            qLayoutChild->addWidget(qPushButtonRefresh);
            qLayoutMain->addWidget(qTableWidgetLauncher);
            qLayoutMain->addItem(qLayoutChild);
    }

}



int QELauncher::getDetailsLayout()
{

    return detailsLayout;

}




void QELauncher::lineEditDirectoryPathChanged(QString)
{

    updateTable();

}





void QELauncher::buttonDirectoryBrowserClicked()
{

    QString directory;

    directory = QFileDialog::getExistingDirectory(this, "Select directory", qlineEditDirectoryPath->text(), QFileDialog::ShowDirsOnly);
    if (directory != NULL)
    {
        qlineEditDirectoryPath->setText(directory);
    }

}




void QELauncher::buttonRefreshClicked()
{

    updateTable();

}




void QELauncher::itemActivated(QTableWidgetItem *)
{

    QModelIndexList selectedRows;
    QString filename;
    QString data;


    selectedRows = qTableWidgetLauncher->selectionModel()->selectedRows();
    data = qTableWidgetLauncher->item(selectedRows.at(0).row(), 2)->text();

    if (qlineEditDirectoryPath->text().endsWith(QDir::separator()))
    {
        filename = qlineEditDirectoryPath->text() + data;
    }
    else
    {
        filename = qlineEditDirectoryPath->text() + QDir::separator() + data;
    }

    emit selected(filename);

}




void QELauncher::updateTable()
{

    QTableWidgetItem *qTableWidgetItem;
    QDir directory;
    QFileInfoList fileList;
    int i;
    int j;

    qTableWidgetLauncher->setRowCount(0);
    directory.setPath(qlineEditDirectoryPath->text());
    directory.setFilter(QDir::Files);
    if (fileFilter.isEmpty() == false)
    {
        directory.setNameFilters(fileFilter.split(";"));
    }
    fileList = directory.entryInfoList();

    for(i = 0; i < fileList.size(); i++)
    {
        j = qTableWidgetLauncher->rowCount();
        qTableWidgetLauncher->insertRow(j);

        qTableWidgetItem = new QTableWidgetItem(fileList.at(i).lastModified().toString("yyyy/MM/dd - hh:mm:ss"));
        qTableWidgetLauncher->setItem(j, 0, qTableWidgetItem);

        qTableWidgetItem = new QTableWidgetItem(QString::number(fileList.at(i).size()) + " bytes");
        qTableWidgetLauncher->setItem(j, 1, qTableWidgetItem);

        if (showFileExtension)
        {
            qTableWidgetItem = new QTableWidgetItem(fileList.at(i).fileName());
        }
        else
        {
            qTableWidgetItem = new QTableWidgetItem(fileList.at(i).baseName());
        }
        qTableWidgetLauncher->setItem(j, 2, qTableWidgetItem);
    }

}






// ============================================================
//  _QTABLEWIDGETLAUNCHER METHODS
// ============================================================
_QTableWidgetLauncher::_QTableWidgetLauncher(QWidget *pParent):QTableWidget(pParent)
{

    initialized = false;

}



void _QTableWidgetLauncher::refreshSize()
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


}



void _QTableWidgetLauncher::resizeEvent(QResizeEvent *)
{

    // TODO: this condition should always be execute when inside Qt Designer
    if (initialized == false)
    {
        refreshSize();
        initialized = true;
    }

}



