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

#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QEScript.h>



// ============================================================
//  QESCRIPT METHODS
// ============================================================
QEScript::QEScript(QWidget *pParent):QWidget(pParent), QEWidget( this )
{

    QFont qFont;

    qComboBoxScriptList = new QComboBox(this);

    qPushButtonNew = new QPushButton(this);
    qPushButtonSave = new QPushButton(this);
    qPushButtonDelete = new QPushButton(this);
    qPushButtonExecute = new QPushButton(this);
    qPushButtonAdd = new QPushButton(this);
    qPushButtonRemove = new QPushButton(this);
    qPushButtonUp = new QPushButton(this);
    qPushButtonDown = new QPushButton(this);
    qPushButtonCopy = new QPushButton(this);
    qPushButtonPaste = new QPushButton(this);
    qTableWidgetScript = new _QTableWidgetScript(this);


    qComboBoxScriptList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    qComboBoxScriptList->setToolTip("Select script");
    QObject::connect(qComboBoxScriptList, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxScriptSelected(int)));

    qPushButtonNew->setText("New");
    qPushButtonNew->setToolTip("Create new script");
    QObject::connect(qPushButtonNew, SIGNAL(clicked()), this, SLOT(buttonNewClicked()));

    qPushButtonSave->setText("Save");
    qPushButtonSave->setToolTip("Save selected script");
    QObject::connect(qPushButtonSave, SIGNAL(clicked()), this, SLOT(buttonSaveClicked()));

    qPushButtonDelete->setText("Delete");
    qPushButtonDelete->setToolTip("Delete selected script");
    QObject::connect(qPushButtonDelete, SIGNAL(clicked()), this, SLOT(buttonDeleteClicked()));

    qPushButtonExecute->setText("Execute");
    qPushButtonExecute->setToolTip("Execute");
    QObject::connect(qPushButtonExecute, SIGNAL(clicked()), this, SLOT(buttonExecuteClicked()));


    qPushButtonAdd->setText("Add");
    qPushButtonAdd->setToolTip("Add row");
    QObject::connect(qPushButtonAdd, SIGNAL(clicked()), this, SLOT(buttonAddClicked()));

    qPushButtonRemove->setText("Remove");
    qPushButtonRemove->setToolTip("Remove selected row(s)");
    QObject::connect(qPushButtonRemove, SIGNAL(clicked()), this, SLOT(buttonRemoveClicked()));

    qPushButtonUp->setText("Up");
    qPushButtonUp->setToolTip("Move selected row up");
    QObject::connect(qPushButtonUp, SIGNAL(clicked()), this, SLOT(buttonUpClicked()));

    qPushButtonDown->setText("Down");
    qPushButtonDown->setToolTip("Move selected row down");
    QObject::connect(qPushButtonDown, SIGNAL(clicked()), this, SLOT(buttonDownClicked()));

    qPushButtonCopy->setText("Copy");
    qPushButtonCopy->setToolTip("Copy selected row(s)");
    QObject::connect(qPushButtonCopy, SIGNAL(clicked()), this, SLOT(buttonCopyClicked()));

    qPushButtonPaste->setText("Paste");
    qPushButtonPaste->setToolTip("Paste row(s)");
    QObject::connect(qPushButtonPaste, SIGNAL(clicked()), this, SLOT(buttonPasteClicked()));

    qTableWidgetScript->setColumnCount(7);
    qTableWidgetScript->setHorizontalHeaderItem(0, new QTableWidgetItem("#"));
    qTableWidgetScript->setHorizontalHeaderItem(1, new QTableWidgetItem("Enable"));
    qTableWidgetScript->setHorizontalHeaderItem(2, new QTableWidgetItem("Program"));
    qTableWidgetScript->setHorizontalHeaderItem(3, new QTableWidgetItem("Parameters"));
    qTableWidgetScript->setHorizontalHeaderItem(4, new QTableWidgetItem("Timeout"));
    qTableWidgetScript->setHorizontalHeaderItem(5, new QTableWidgetItem("Stop"));
    qTableWidgetScript->setHorizontalHeaderItem(6, new QTableWidgetItem("Log"));
    qTableWidgetScript->setToolTip("List of programs to execute");
    qTableWidgetScript->setSelectionBehavior(QAbstractItemView::SelectRows);
    qTableWidgetScript->verticalHeader()->hide();
    qFont.setPointSize(9);
    qTableWidgetScript->setFont(qFont);
    //QObject::connect(qTableWidgetScript, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
    QObject::connect(qTableWidgetScript->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));

    setShowScriptList(true);
    setOptionsLayout(TOP);
    updateWidgets();

}




void QEScript::setShowScriptList(bool pValue)
{

    qComboBoxScriptList->setVisible(pValue);

}



bool QEScript::getShowScriptList()
{

    return qComboBoxScriptList->isVisible();

}



void QEScript::setShowNew(bool pValue)
{

    qPushButtonNew->setVisible(pValue);

}



bool QEScript::getShowNew()
{

    return qPushButtonNew->isVisible();

}




void QEScript::setShowSave(bool pValue)
{

    qPushButtonSave->setVisible(pValue);

}




bool QEScript::getShowSave()
{

    return qPushButtonSave->isVisible();

}



void QEScript::setShowDelete(bool pValue)
{

    qPushButtonDelete->setVisible(pValue);

}



bool QEScript::getShowDelete()
{

    return qPushButtonDelete->isVisible();

}



void QEScript::setShowExecute(bool pValue)
{

    qPushButtonExecute->setVisible(pValue);

}



bool QEScript::getShowExecute()
{

    return qPushButtonExecute->isVisible();

}




void QEScript::setShowTable(bool pValue)
{

    qTableWidgetScript->setVisible(pValue);

}



bool QEScript::getShowTable()
{

    return qTableWidgetScript->isVisible();

}



void QEScript::setShowTableControl(bool pValue)
{

    qPushButtonAdd->setVisible(pValue);
    qPushButtonRemove->setVisible(pValue);
    qPushButtonUp->setVisible(pValue);
    qPushButtonDown->setVisible(pValue);
    qPushButtonCopy->setVisible(pValue);
    qPushButtonPaste->setVisible(pValue);

}



bool QEScript::getShowTableControl()
{

    return (qPushButtonAdd->isVisible());

}



void QEScript::setShowColumnNumber(bool pValue)
{

    qTableWidgetScript->setColumnHidden(0, pValue == false);
    qTableWidgetScript->refreshSize();

}



bool QEScript::getShowColumnNumber()
{

    return (qTableWidgetScript->isColumnHidden(0) == false);

}



void QEScript::setShowColumnEnable(bool pValue)
{

    qTableWidgetScript->setColumnHidden(1, pValue == false);
    qTableWidgetScript->refreshSize();

}



bool QEScript::getShowColumnEnable()
{

    return (qTableWidgetScript->isColumnHidden(1) == false);

}



void QEScript::setShowColumnProgram(bool pValue)
{

    qTableWidgetScript->setColumnHidden(2, pValue == false);
    qTableWidgetScript->refreshSize();

}



bool QEScript::getShowColumnProgram()
{

    return (qTableWidgetScript->isColumnHidden(2) == false);

}



void QEScript::setShowColumnParameters(bool pValue)
{

    qTableWidgetScript->setColumnHidden(3, pValue == false);
    qTableWidgetScript->refreshSize();

}



bool QEScript::getShowColumnParameters()
{

    return (qTableWidgetScript->isColumnHidden(3) == false);

}




void QEScript::setShowColumnTimeOut(bool pValue)
{

    qTableWidgetScript->setColumnHidden(4, pValue == false);
    qTableWidgetScript->refreshSize();

}



bool QEScript::getShowColumnTimeOut()
{

    return (qTableWidgetScript->isColumnHidden(4) == false);

}



void QEScript::setShowColumnStop(bool pValue)
{

    qTableWidgetScript->setColumnHidden(5, pValue == false);
    qTableWidgetScript->refreshSize();

}



bool QEScript::getShowColumnStop()
{

    return (qTableWidgetScript->isColumnHidden(5) == false);

}



void QEScript::setShowColumnLog(bool pValue)
{

    qTableWidgetScript->setColumnHidden(6, pValue == false);
    qTableWidgetScript->refreshSize();

}



bool QEScript::getShowColumnLog()
{

    return (qTableWidgetScript->isColumnHidden(6) == false);

}



void QEScript::setScriptFile(QString pValue)
{

    QDomElement rootElement;
    QFile *file;
    QString data;
    bool flag;



    scriptFile = pValue;

    document.clear();

    if (scriptFile.isEmpty())
    {
        QFileInfo fileInfo;
        fileInfo.setFile(defaultFileLocation(), "QEScript.xml");
        filename = fileInfo.filePath();
    }
    else
    {
        filename = scriptFile;
    }

    file = openQEFile(filename, (QIODevice::OpenModeFlag)((int)(QFile::ReadOnly | QFile::Text)));
    if (file)
    {
        data = file->readAll();
        file->close();
        flag = document.setContent(data);
    }
    else
    {
        flag = false;
    }

    if (flag)
    {
        refreshScriptList();
    }
    else
    {
        rootElement = document.createElement("epicsqt");
        document.appendChild(rootElement);
    }


}




QString QEScript::getScriptFile()
{

    return scriptFile;

}



void QEScript::setExecuteText(QString pValue)
{

    qPushButtonExecute->setText(pValue);

}




QString QEScript::getExecuteText()
{

    return qPushButtonExecute->text();

}




void QEScript::setOptionsLayout(int pValue)
{

    QLayout *qLayoutMain;
    QLayout *qLayoutChild;

    delete layout();

    switch(pValue)
    {
        case TOP:
            optionsLayout = TOP;
            qLayoutMain = new QVBoxLayout(this);
            qLayoutChild = new QHBoxLayout();
            qLayoutChild->addWidget(qComboBoxScriptList);
            qLayoutChild->addWidget(qPushButtonNew);
            qLayoutChild->addWidget(qPushButtonSave);
            qLayoutChild->addWidget(qPushButtonDelete);
            qLayoutChild->addWidget(qPushButtonExecute);
            qLayoutMain->addItem(qLayoutChild);
            qLayoutChild = new QHBoxLayout();
            qLayoutChild->addWidget(qPushButtonAdd);
            qLayoutChild->addWidget(qPushButtonRemove);
            qLayoutChild->addWidget(qPushButtonUp);
            qLayoutChild->addWidget(qPushButtonDown);
            qLayoutChild->addWidget(qPushButtonCopy);
            qLayoutChild->addWidget(qPushButtonPaste);
            qLayoutMain->addItem(qLayoutChild);
            qLayoutMain->addWidget(qTableWidgetScript);
            break;

        case BOTTOM:
            optionsLayout = BOTTOM;
            qLayoutMain = new QVBoxLayout(this);
            qLayoutMain->addWidget(qTableWidgetScript);
            qLayoutChild = new QHBoxLayout();
            qLayoutChild->addWidget(qComboBoxScriptList);
            qLayoutChild->addWidget(qPushButtonNew);
            qLayoutChild->addWidget(qPushButtonSave);
            qLayoutChild->addWidget(qPushButtonDelete);
            qLayoutChild->addWidget(qPushButtonExecute);
            qLayoutMain->addItem(qLayoutChild);
            qLayoutChild = new QHBoxLayout();
            qLayoutChild->addWidget(qPushButtonAdd);
            qLayoutChild->addWidget(qPushButtonRemove);
            qLayoutChild->addWidget(qPushButtonUp);
            qLayoutChild->addWidget(qPushButtonDown);
            qLayoutChild->addWidget(qPushButtonCopy);
            qLayoutChild->addWidget(qPushButtonPaste);
            qLayoutMain->addItem(qLayoutChild);
            break;

        case LEFT:
            optionsLayout = LEFT;
            qLayoutMain = new QHBoxLayout(this);
            qLayoutChild = new QVBoxLayout();
            qLayoutChild->addWidget(qComboBoxScriptList);
            qLayoutChild->addWidget(qPushButtonNew);
            qLayoutChild->addWidget(qPushButtonSave);
            qLayoutChild->addWidget(qPushButtonDelete);
            qLayoutChild->addWidget(qPushButtonExecute);
            qLayoutChild->addWidget(qPushButtonAdd);
            qLayoutChild->addWidget(qPushButtonRemove);
            qLayoutChild->addWidget(qPushButtonUp);
            qLayoutChild->addWidget(qPushButtonDown);
            qLayoutChild->addWidget(qPushButtonCopy);
            qLayoutChild->addWidget(qPushButtonPaste);
            qLayoutMain->addItem(qLayoutChild);
            qLayoutMain->addWidget(qTableWidgetScript);
            break;

        case RIGHT:
            optionsLayout = RIGHT;
            qLayoutMain = new QHBoxLayout(this);
            qLayoutChild = new QVBoxLayout();
            qLayoutChild->addWidget(qComboBoxScriptList);
            qLayoutChild->addWidget(qPushButtonNew);
            qLayoutChild->addWidget(qPushButtonSave);
            qLayoutChild->addWidget(qPushButtonDelete);
            qLayoutChild->addWidget(qPushButtonExecute);
            qLayoutChild->addWidget(qPushButtonAdd);
            qLayoutChild->addWidget(qPushButtonRemove);
            qLayoutChild->addWidget(qPushButtonUp);
            qLayoutChild->addWidget(qPushButtonDown);
            qLayoutChild->addWidget(qPushButtonCopy);
            qLayoutChild->addWidget(qPushButtonPaste);
            qLayoutMain->addWidget(qTableWidgetScript);            
            qLayoutMain->addItem(qLayoutChild);
    }

}



int QEScript::getOptionsLayout()
{

    return optionsLayout;

}




void QEScript::comboBoxScriptSelected(int)
{

    //refreshButton();

}




void QEScript::buttonNewClicked()
{

    QMessageBox::information(this, "Info", "To be implemented soon!");

}




void QEScript::buttonSaveClicked()
{

    QMessageBox::information(this, "Info", "To be implemented soon!");

}




void QEScript::buttonDeleteClicked()
{

    QMessageBox::information(this, "Info", "To be implemented soon!");

}




void QEScript::buttonExecuteClicked()
{

    QMessageBox::information(this, "Info", "To be implemented soon!");

}




void QEScript::buttonAddClicked()
{

    QTableWidgetItem *qTableWidgetItem;
    int row;
    int i;


    if (qTableWidgetScript->selectionModel()->selectedRows().count() == 0)
    {
        row = qTableWidgetScript->rowCount();
        qTableWidgetScript->insertRow(row);
    }
    else
    {
        row = qTableWidgetScript->selectedItems().at(0)->row();
        qTableWidgetScript->insertRow(row);
        qTableWidgetScript->selectRow(row);
    }

    for(i = 0; i < qTableWidgetScript->columnCount(); i++)
    {
        qTableWidgetItem = new QTableWidgetItem("");
        qTableWidgetScript->setItem(row, i, qTableWidgetItem);
    }

    for(i = row; i < qTableWidgetScript->rowCount(); i++)
    {
        qTableWidgetItem = new QTableWidgetItem(QString::number(i + 1));
        qTableWidgetScript->setItem(i, 0, qTableWidgetItem);
    }

    updateWidgets();

}



void QEScript::buttonRemoveClicked()
{

    QTableWidgetItem *qTableWidgetItem;
    int rowSelectedCount;
    int rowSelected;
    int i;

    rowSelectedCount = qTableWidgetScript->selectionModel()->selectedRows().count();
    rowSelected = qTableWidgetScript->selectedItems().at(0)->row();

    for(i = rowSelectedCount; i > 0; i--)
    {
        qTableWidgetScript->removeRow(qTableWidgetScript->selectedItems().at(i - 1)->row());
    }

    for(i = 0; i < qTableWidgetScript->rowCount(); i++)
    {
        qTableWidgetItem = new QTableWidgetItem(QString::number(i + 1));
        qTableWidgetScript->setItem(i, 0, qTableWidgetItem);
    }

    if (rowSelected < qTableWidgetScript->rowCount())
    {
        qTableWidgetScript->selectRow(rowSelected);
    }
    else
    {
        qTableWidgetScript->selectRow(qTableWidgetScript->rowCount() - 1);
    }

    updateWidgets();

}



void QEScript::buttonUpClicked()
{

    QModelIndexList qModelIndexList;
    QString tmp;
    int row;
    int i;

    qModelIndexList = qTableWidgetScript->selectionModel()->selectedRows();
    row = qModelIndexList.at(0).row();

    for(i = 1; i < qTableWidgetScript->columnCount(); i++)
    {
        tmp = qTableWidgetScript->item(row - 1, i)->text();
        qTableWidgetScript->item(row - 1, i)->setText(qTableWidgetScript->item(row, i)->text());
        qTableWidgetScript->item(row, i)->setText(tmp);
    }

    qTableWidgetScript->selectRow(row - 1);

    updateWidgets();

}



void QEScript::buttonDownClicked()
{

    QModelIndexList qModelIndexList;
    QString tmp;
    int row;
    int i;

    qModelIndexList = qTableWidgetScript->selectionModel()->selectedRows();
    row = qModelIndexList.at(0).row();

    for(i = 1; i < qTableWidgetScript->columnCount(); i++)
    {
        tmp = qTableWidgetScript->item(row + 1, i)->text();
        qTableWidgetScript->item(row + 1, i)->setText(qTableWidgetScript->item(row, i)->text());
        qTableWidgetScript->item(row, i)->setText(tmp);
    }

    qTableWidgetScript->selectRow(row + 1);

    updateWidgets();

}



void QEScript::buttonCopyClicked()
{

    QModelIndexList qModelIndexList;
    int row;
    int i;

    for(i = 0 ; i < copyPasteList.count(); i++)
    {
        delete copyPasteList.at(i);
        copyPasteList.removeAt(i);
    }

    qModelIndexList = qTableWidgetScript->selectionModel()->selectedRows();
    for(i = 0; i < qModelIndexList.count(); i++)
    {
        row = qModelIndexList.at(i).row();
        copyPasteList.append(new _CopyPaste(qTableWidgetScript->item(row, 1)->text(), qTableWidgetScript->item(row, 2)->text(), qTableWidgetScript->item(row, 3)->text(), qTableWidgetScript->item(row, 4)->text(), qTableWidgetScript->item(row, 5)->text(), qTableWidgetScript->item(row, 6)->text()));
    }

    updateWidgets();

}



void QEScript::buttonPasteClicked()
{

    QModelIndexList qModelIndexList;
    QTableWidgetItem *qTableWidgetItem;
    int row;
    int i;


    qModelIndexList = qTableWidgetScript->selectionModel()->selectedRows();
    if (qModelIndexList.isEmpty())
    {
        row = 0;
    }
    else
    {
        row = qModelIndexList.at(0).row();
    }

    for(i = 0; i < copyPasteList.count(); i++)
    {
        qTableWidgetScript->insertRow(row + i);

        qTableWidgetItem = new QTableWidgetItem(copyPasteList.at(i)->getEnable());
        qTableWidgetScript->setItem(row + i, 1, qTableWidgetItem);

        qTableWidgetItem = new QTableWidgetItem(copyPasteList.at(i)->getProgram());
        qTableWidgetScript->setItem(row + i, 2, qTableWidgetItem);

        qTableWidgetItem = new QTableWidgetItem(copyPasteList.at(i)->getParameters());
        qTableWidgetScript->setItem(row + i, 3, qTableWidgetItem);

        qTableWidgetItem = new QTableWidgetItem(copyPasteList.at(i)->getTimeOut());
        qTableWidgetScript->setItem(row + i, 4, qTableWidgetItem);

        qTableWidgetItem = new QTableWidgetItem(copyPasteList.at(i)->getStop());
        qTableWidgetScript->setItem(row + i, 5, qTableWidgetItem);

        qTableWidgetItem = new QTableWidgetItem(copyPasteList.at(i)->getLog());
        qTableWidgetScript->setItem(row + i, 6, qTableWidgetItem);
    }

    for(i = 0; i < qTableWidgetScript->rowCount(); i++)
    {
        qTableWidgetItem = new QTableWidgetItem(QString::number(i + 1));
        qTableWidgetScript->setItem(row + i, 0, qTableWidgetItem);
    }

    updateWidgets();

}



void QEScript::selectionChanged(const QItemSelection &, const QItemSelection &)
{

    updateWidgets();

}




void QEScript::itemActivated(QTableWidgetItem *)
{

    /*
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
    */

}





void QEScript::refreshScriptList()
{

    QDomElement rootElement;
    QDomElement recipeElement;
    QDomNode rootNode;
    QString visible;
    QString tmp;
    bool flag;
    int count;
    int i;


    qComboBoxScriptList->blockSignals(true);
    tmp = qComboBoxScriptList->currentText();
    qComboBoxScriptList->clear();
    rootElement = document.documentElement();
    if (rootElement.tagName() == "epicsqt")
    {
        count = 0;
        rootNode = rootElement.firstChild();
        while (rootNode.isNull() == false)
        {
            recipeElement = rootNode.toElement();
            if (recipeElement.tagName() == "script")
            {
                visible = recipeElement.attribute("visible").toUpper();
                if (visible.isEmpty())
                {
                    flag = true;
                }
                else
                {
                    if (visible == "USER")
                    {
                        flag = true;
                    }
                    else if (visible == "SCIENTIST")
                    {
                        //flag = (currentUserType > 0);
                    }
                    else if (visible == "ENGINEER")
                    {
                        //flag = (currentUserType > 1);
                    }
                    else
                    {
                        flag = false;
                    }
                    flag = false;
                }
                if (flag)
                {
                    if (recipeElement.attribute("name").isEmpty())
                    {
                        qComboBoxScriptList->addItem("Script #" + QString::number(count));
                        count++;
                    }
                    else
                    {
                        qComboBoxScriptList->addItem(recipeElement.attribute("name"));
                    }
                }
            }
            rootNode = rootNode.nextSibling();
        }
    }
    i = qComboBoxScriptList->findText(tmp);
    if (i == -1)
    {
        qComboBoxScriptList->setCurrentIndex(0);
    }
    else
    {
       qComboBoxScriptList->setCurrentIndex(i);
    }
    //refreshButton();
    qComboBoxScriptList->blockSignals(false);

}




void QEScript::updateWidgets()
{

    int rowCount;
    int rowSelectedCount;

    rowCount = qTableWidgetScript->rowCount();
    rowSelectedCount = qTableWidgetScript->selectionModel()->selectedRows().count();

    qPushButtonSave->setDisabled(qComboBoxScriptList->currentText().isEmpty());
    qPushButtonDelete->setDisabled(qComboBoxScriptList->currentText().isEmpty());
    qPushButtonExecute->setDisabled(rowCount == 0);

    qPushButtonAdd->setEnabled(rowSelectedCount <= 1);
    qPushButtonRemove->setEnabled(rowSelectedCount > 0);

    qPushButtonUp->setEnabled(rowSelectedCount == 1 && qTableWidgetScript->selectionModel()->selectedRows().at(0).row() > 0);
    qPushButtonDown->setEnabled(rowSelectedCount == 1 && qTableWidgetScript->selectionModel()->selectedRows().at(0).row() < rowCount - 1);

    qPushButtonCopy->setEnabled(rowSelectedCount > 0);
    qPushButtonPaste->setEnabled(copyPasteList.isEmpty() == false);

}




// ============================================================
//  _QCOPYPASTE CLASS
// ============================================================
_CopyPaste::_CopyPaste(QString pEnable, QString pProgram, QString pParameters, QString pTimeOut, QString pStop, QString pLog)
{

    setEnable(pEnable);

    setProgram(pProgram);

    setParameters(pParameters);

    setTimeOut(pTimeOut);

    setStop(pStop);

    setLog(pLog);

};




void _CopyPaste::setEnable(QString pEnable)
{

    enable = pEnable;

}



QString _CopyPaste::getEnable()
{

    return enable;

}



void _CopyPaste::setProgram(QString pProgram)
{

    program = pProgram;

}



QString _CopyPaste::getProgram()
{

    return program;

}




void _CopyPaste::setParameters(QString pParameters)
{

    parameters = pParameters;

}



QString _CopyPaste::getParameters()
{

    return parameters;

}



void _CopyPaste::setTimeOut(QString pTimeOut)
{

    timeOut = pTimeOut;

}



QString _CopyPaste::getTimeOut()
{

    return timeOut;

}




void _CopyPaste::setStop(QString pStop)
{

    stop = pStop;

}



QString _CopyPaste::getStop()
{

    return stop;

}




void _CopyPaste::setLog(QString pLog)
{

    log = pLog;

}



QString _CopyPaste::getLog()
{

    return log;

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

    int i;
    int hidden;


    hidden = 0;
    for(i = 0; i < this->columnCount(); i++)
    {
        if (this->isColumnHidden(i))
        {
            hidden++;
        }
    }


    for(i = 0; i < this->columnCount(); i++)
    {
        this->setColumnWidth(i, this->width() / (this->columnCount() - hidden));
    }

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


