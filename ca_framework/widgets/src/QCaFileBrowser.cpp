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
#include <QDialog>
#include <QGroupBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QCaFileBrowser.h>
#include <QDebug>




// ============================================================
//  QCAFILEBROWSER METHODS
// ============================================================
QCaFileBrowser::QCaFileBrowser(QWidget *pParent):QWidget(pParent), QCaWidget( this )
{

    QFont qFont;


    qlineEditDirectoryPath = new QLineEdit(this);
    qPushButtonDirectoryBrowser = new QPushButton(this);
    qPushButtonRefresh = new QPushButton(this);
    qTableWidgetFileBrowser = new _QTableWidgetFileBrowser(this);

    qlineEditDirectoryPath->setToolTip("Specify the directory where to browse for files");
    QObject::connect(qlineEditDirectoryPath, SIGNAL(textChanged(QString)), this, SLOT(lineEditDirectoryPathChanged(QString)));

    qPushButtonDirectoryBrowser->setText("...");
    qPushButtonDirectoryBrowser->setToolTip("Browse for a directory");
    QObject::connect(qPushButtonDirectoryBrowser, SIGNAL(clicked()), this, SLOT(buttonDirectoryBrowserClicked()));

    qPushButtonRefresh->setText("Refresh");
    qPushButtonRefresh->setToolTip("Refresh file browse result");
    QObject::connect(qPushButtonRefresh, SIGNAL(clicked()), this, SLOT(buttonRefreshClicked()));

    qTableWidgetFileBrowser->setColumnCount(3);
    qTableWidgetFileBrowser->setHorizontalHeaderItem(0, new QTableWidgetItem("Time"));
    qTableWidgetFileBrowser->setHorizontalHeaderItem(1, new QTableWidgetItem("Size"));
    qTableWidgetFileBrowser->setHorizontalHeaderItem(2, new QTableWidgetItem("Filename"));
    qTableWidgetFileBrowser->setToolTip("Files contained in the specified directory");
    qTableWidgetFileBrowser->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qTableWidgetFileBrowser->setSelectionBehavior(QAbstractItemView::SelectRows);
    qTableWidgetFileBrowser->setSelectionMode(QAbstractItemView::SingleSelection);
    qTableWidgetFileBrowser->verticalHeader()->hide();
    qFont.setPointSize(9);
    qTableWidgetFileBrowser->setFont(qFont);
    QObject::connect(qTableWidgetFileBrowser, SIGNAL(itemActivated(QTableWidgetItem *)), this, SLOT(itemActivated(QTableWidgetItem *)));

    setShowFileExtension(true);
    setFileFilter("");
    setDetailsLayout(TOP);

}




void QCaFileBrowser::setDirectoryPath(QString pValue)
{

    qlineEditDirectoryPath->setText(pValue);

}



QString QCaFileBrowser::getDirectoryPath()
{

    return qlineEditDirectoryPath->text();

}





void QCaFileBrowser::setShowDirectoryPath(bool pValue)
{

    qlineEditDirectoryPath->setVisible(pValue);

}



bool QCaFileBrowser::getShowDirectoryPath()
{

    return qlineEditDirectoryPath->isVisible();

}




void QCaFileBrowser::setShowDirectoryBrowser(bool pValue)
{

    qPushButtonDirectoryBrowser->setVisible(pValue);

}



bool QCaFileBrowser::getShowDirectoryBrowser()
{

    return qPushButtonDirectoryBrowser->isVisible();

}



void QCaFileBrowser::setShowRefresh(bool pValue)
{

    qPushButtonRefresh->setVisible(pValue);

}



bool QCaFileBrowser::getShowRefresh()
{

    return qPushButtonRefresh->isVisible();

}



void QCaFileBrowser::setFileFilter(QString pValue)
{

    fileFilter = pValue;
    updateTable();

}



QString QCaFileBrowser::getFileFilter()
{

    return fileFilter;

}




void QCaFileBrowser::setShowColumnTime(bool pValue)
{

    qTableWidgetFileBrowser->setColumnHidden(0, pValue == false);
    qTableWidgetFileBrowser->refreshSize();

}



bool QCaFileBrowser::getShowColumnTime()
{

    return (qTableWidgetFileBrowser->isColumnHidden(0) == false);

}



void QCaFileBrowser::setShowColumnSize(bool pValue)
{

    qTableWidgetFileBrowser->setColumnHidden(1, pValue == false);
    qTableWidgetFileBrowser->refreshSize();

}



bool QCaFileBrowser::getShowColumnSize()
{

    return (qTableWidgetFileBrowser->isColumnHidden(1) == false);

}


void QCaFileBrowser::setShowColumnFilename(bool pValue)
{

    qTableWidgetFileBrowser->setColumnHidden(2, pValue == false);
    qTableWidgetFileBrowser->refreshSize();

}



bool QCaFileBrowser::getShowColumnFilename()
{

    return (qTableWidgetFileBrowser->isColumnHidden(2) == false);

}



void QCaFileBrowser::setShowFileExtension(bool pValue)
{

    showFileExtension = pValue;
    updateTable();

}



bool QCaFileBrowser::getShowFileExtension()
{

    return showFileExtension;

}



void QCaFileBrowser::setDetailsLayout(int pValue)
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
            qLayoutMain->addWidget(qTableWidgetFileBrowser);
            break;

        case BOTTOM:
            detailsLayout = BOTTOM;
            qLayoutMain = new QVBoxLayout(this);
            qLayoutMain->addWidget(qTableWidgetFileBrowser);
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
            qLayoutMain->addWidget(qTableWidgetFileBrowser);
            break;

        case RIGHT:
            detailsLayout = RIGHT;
            qLayoutMain = new QHBoxLayout(this);
            qLayoutChild = new QVBoxLayout();
            qLayoutChild->addWidget(qlineEditDirectoryPath);
            qLayoutChild->addWidget(qPushButtonDirectoryBrowser);
            qLayoutChild->addWidget(qPushButtonRefresh);
            qLayoutMain->addWidget(qTableWidgetFileBrowser);
            qLayoutMain->addItem(qLayoutChild);
    }

}



int QCaFileBrowser::getDetailsLayout()
{

    return detailsLayout;

}




void QCaFileBrowser::lineEditDirectoryPathChanged(QString)
{

    updateTable();

}





void QCaFileBrowser::buttonDirectoryBrowserClicked()
{

    QString directory;

    directory = QFileDialog::getExistingDirectory(this, "Select directory", qlineEditDirectoryPath->text(), QFileDialog::ShowDirsOnly);
    if (directory != NULL)
    {
        qlineEditDirectoryPath->setText(directory);
    }

}




void QCaFileBrowser::buttonRefreshClicked()
{

    updateTable();

}




void QCaFileBrowser::itemActivated(QTableWidgetItem *)
{

    QModelIndexList selectedRows;
    QString filename;
    QString data;


    selectedRows = qTableWidgetFileBrowser->selectionModel()->selectedRows();
    data = qTableWidgetFileBrowser->item(selectedRows.at(0).row(), 2)->text();

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




void QCaFileBrowser::updateTable()
{

    QTableWidgetItem *qTableWidgetItem;
    QDir directory;
    QFileInfoList fileList;
    int i;
    int j;

    qTableWidgetFileBrowser->setRowCount(0);
    directory.setPath(qlineEditDirectoryPath->text());
    directory.setFilter(QDir::Files);
    if (fileFilter.isEmpty() == false)
    {
        directory.setNameFilters(fileFilter.split(";"));
    }
    fileList = directory.entryInfoList();

    for(i = 0; i < fileList.size(); i++)
    {
        j = qTableWidgetFileBrowser->rowCount();
        qTableWidgetFileBrowser->insertRow(j);

        qTableWidgetItem = new QTableWidgetItem(fileList.at(i).lastModified().toString("yyyy/MM/dd - hh:mm:ss"));
        qTableWidgetFileBrowser->setItem(j, 0, qTableWidgetItem);

        qTableWidgetItem = new QTableWidgetItem(QString::number(fileList.at(i).size()) + " bytes");
        qTableWidgetFileBrowser->setItem(j, 1, qTableWidgetItem);

        if (showFileExtension)
        {
            qTableWidgetItem = new QTableWidgetItem(fileList.at(i).fileName());
        }
        else
        {
            qTableWidgetItem = new QTableWidgetItem(fileList.at(i).baseName());
        }
        qTableWidgetFileBrowser->setItem(j, 2, qTableWidgetItem);
    }

}






// ============================================================
//  _QTABLEWIDGETFILEBROWSER METHODS
// ============================================================
_QTableWidgetFileBrowser::_QTableWidgetFileBrowser(QWidget *pParent):QTableWidget(pParent)
{

    initialized = false;

}



void _QTableWidgetFileBrowser::refreshSize()
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
                sizeColumn2 = 4 * this->width() / 5 - 2;
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
                sizeColumn2 = 4 * this->width() / 5 - 2;
            }
        }
        else
        {
            if (this->isColumnHidden(2))
            {
                sizeColumn0 = this->width() / 2;
                sizeColumn1 = this->width() / 2 - 2;
                sizeColumn2 = 0;
            }
            else
            {
                sizeColumn0 = 1 * this->width() / 5;
                sizeColumn1 = 1 * this->width() / 5;
                sizeColumn2 = 3 * this->width() / 5 - 2;
            }
        }
    }

    this->setColumnWidth(0, sizeColumn0);
    this->setColumnWidth(1, sizeColumn1);
    this->setColumnWidth(2, sizeColumn2);


}



void _QTableWidgetFileBrowser::resizeEvent(QResizeEvent *)
{

    // TODO: this condition should always be execute when inside Qt Designer
    if (initialized == false)
    {
        refreshSize();
        initialized = true;
    }

}



