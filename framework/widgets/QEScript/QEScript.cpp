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

#include <QEScript.h>



// ============================================================
//  QESCRIPT METHODS
// ============================================================
QEScript::QEScript(QWidget *pParent):QWidget(pParent), QCaWidget( this )
{

    QFont qFont;

    qlineEditDirectoryPath = new QLineEdit(this);
    qPushButtonDirectoryBrowser = new QPushButton(this);
    qPushButtonRefresh = new QPushButton(this);
    qTableWidgetScript = new _QTableWidgetScript(this);

    qlineEditDirectoryPath->setToolTip("Specify the directory where to browse for files");
    QObject::connect(qlineEditDirectoryPath, SIGNAL(textChanged(QString)), this, SLOT(lineEditDirectoryPathChanged(QString)));

    qPushButtonDirectoryBrowser->setText("...");
    qPushButtonDirectoryBrowser->setToolTip("Browse for a directory");
    QObject::connect(qPushButtonDirectoryBrowser, SIGNAL(clicked()), this, SLOT(buttonDirectoryBrowserClicked()));

    qPushButtonRefresh->setText("Refresh");
    qPushButtonRefresh->setToolTip("Refresh file browse result");
    QObject::connect(qPushButtonRefresh, SIGNAL(clicked()), this, SLOT(buttonRefreshClicked()));

    qTableWidgetScript->setColumnCount(3);
    qTableWidgetScript->setHorizontalHeaderItem(0, new QTableWidgetItem("Time"));
    qTableWidgetScript->setHorizontalHeaderItem(1, new QTableWidgetItem("Size"));
    qTableWidgetScript->setHorizontalHeaderItem(2, new QTableWidgetItem("Filename"));
    qTableWidgetScript->setToolTip("Files contained in the specified directory");
    qTableWidgetScript->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qTableWidgetScript->setSelectionBehavior(QAbstractItemView::SelectRows);
    qTableWidgetScript->setSelectionMode(QAbstractItemView::SingleSelection);
    qTableWidgetScript->verticalHeader()->hide();
    qFont.setPointSize(9);
    qTableWidgetScript->setFont(qFont);
    QObject::connect(qTableWidgetScript, SIGNAL(itemActivated(QTableWidgetItem *)), this, SLOT(itemActivated(QTableWidgetItem *)));

    setShowFileExtension(true);
    setFileFilter("");
    setDetailsLayout(TOP);

}




void QEScript::setDirectoryPath(QString pValue)
{

    qlineEditDirectoryPath->setText(pValue);

}



QString QEScript::getDirectoryPath()
{

    return qlineEditDirectoryPath->text();

}





void QEScript::setShowDirectoryPath(bool pValue)
{

    qlineEditDirectoryPath->setVisible(pValue);

}



bool QEScript::getShowDirectoryPath()
{

    return qlineEditDirectoryPath->isVisible();

}




void QEScript::setShowDirectoryBrowser(bool pValue)
{

    qPushButtonDirectoryBrowser->setVisible(pValue);

}



bool QEScript::getShowDirectoryBrowser()
{

    return qPushButtonDirectoryBrowser->isVisible();

}



void QEScript::setShowRefresh(bool pValue)
{

    qPushButtonRefresh->setVisible(pValue);

}



bool QEScript::getShowRefresh()
{

    return qPushButtonRefresh->isVisible();

}



void QEScript::setFileFilter(QString pValue)
{

    fileFilter = pValue;
    updateTable();

}



QString QEScript::getFileFilter()
{

    return fileFilter;

}




void QEScript::setShowColumnTime(bool pValue)
{

    qTableWidgetScript->setColumnHidden(0, pValue == false);
    qTableWidgetScript->refreshSize();

}



bool QEScript::getShowColumnTime()
{

    return (qTableWidgetScript->isColumnHidden(0) == false);

}



void QEScript::setShowColumnSize(bool pValue)
{

    qTableWidgetScript->setColumnHidden(1, pValue == false);
    qTableWidgetScript->refreshSize();

}



bool QEScript::getShowColumnSize()
{

    return (qTableWidgetScript->isColumnHidden(1) == false);

}


void QEScript::setShowColumnFilename(bool pValue)
{

    qTableWidgetScript->setColumnHidden(2, pValue == false);
    qTableWidgetScript->refreshSize();

}



bool QEScript::getShowColumnFilename()
{

    return (qTableWidgetScript->isColumnHidden(2) == false);

}



void QEScript::setShowFileExtension(bool pValue)
{

    showFileExtension = pValue;
    updateTable();

}



bool QEScript::getShowFileExtension()
{

    return showFileExtension;

}



void QEScript::setDetailsLayout(int pValue)
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
            qLayoutMain->addWidget(qTableWidgetScript);
            break;

        case BOTTOM:
            detailsLayout = BOTTOM;
            qLayoutMain = new QVBoxLayout(this);
            qLayoutMain->addWidget(qTableWidgetScript);
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
            qLayoutMain->addWidget(qTableWidgetScript);
            break;

        case RIGHT:
            detailsLayout = RIGHT;
            qLayoutMain = new QHBoxLayout(this);
            qLayoutChild = new QVBoxLayout();
            qLayoutChild->addWidget(qlineEditDirectoryPath);
            qLayoutChild->addWidget(qPushButtonDirectoryBrowser);
            qLayoutChild->addWidget(qPushButtonRefresh);
            qLayoutMain->addWidget(qTableWidgetScript);
            qLayoutMain->addItem(qLayoutChild);
    }

}



int QEScript::getDetailsLayout()
{

    return detailsLayout;

}




void QEScript::lineEditDirectoryPathChanged(QString)
{

    updateTable();

}





void QEScript::buttonDirectoryBrowserClicked()
{

    QString directory;

    directory = QFileDialog::getExistingDirectory(this, "Select directory", qlineEditDirectoryPath->text(), QFileDialog::ShowDirsOnly);
    if (directory != NULL)
    {
        qlineEditDirectoryPath->setText(directory);
    }

}




void QEScript::buttonRefreshClicked()
{

    updateTable();

}




void QEScript::itemActivated(QTableWidgetItem *)
{

    QModelIndexList selectedRows;
    QString filename;
    QString data;


    selectedRows = qTableWidgetScript->selectionModel()->selectedRows();
    data = qTableWidgetScript->item(selectedRows.at(0).row(), 2)->text();

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




void QEScript::updateTable()
{

    QTableWidgetItem *qTableWidgetItem;
    QDir directory;
    QFileInfoList fileList;
    int i;
    int j;

    qTableWidgetScript->setRowCount(0);
    directory.setPath(qlineEditDirectoryPath->text());
    directory.setFilter(QDir::Files);
    if (fileFilter.isEmpty() == false)
    {
        directory.setNameFilters(fileFilter.split(";"));
    }
    fileList = directory.entryInfoList();

    for(i = 0; i < fileList.size(); i++)
    {
        j = qTableWidgetScript->rowCount();
        qTableWidgetScript->insertRow(j);

        qTableWidgetItem = new QTableWidgetItem(fileList.at(i).lastModified().toString("yyyy/MM/dd - hh:mm:ss"));
        qTableWidgetScript->setItem(j, 0, qTableWidgetItem);

        qTableWidgetItem = new QTableWidgetItem(QString::number(fileList.at(i).size()) + " bytes");
        qTableWidgetScript->setItem(j, 1, qTableWidgetItem);

        if (showFileExtension)
        {
            qTableWidgetItem = new QTableWidgetItem(fileList.at(i).fileName());
        }
        else
        {
            qTableWidgetItem = new QTableWidgetItem(fileList.at(i).baseName());
        }
        qTableWidgetScript->setItem(j, 2, qTableWidgetItem);
    }

}






// ============================================================
//  _QTABLEWIDGETSCRIPT METHODS
// ============================================================
_QTableWidgetScript::_QTableWidgetScript(QWidget *pParent):QTableWidget(pParent)
{

    initialized = false;

}



void _QTableWidgetScript::refreshSize()
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



void _QTableWidgetScript::resizeEvent(QResizeEvent *)
{

    // TODO: this condition should always be execute when inside Qt Designer
    if (initialized == false)
    {
        refreshSize();
        initialized = true;
    }

}



