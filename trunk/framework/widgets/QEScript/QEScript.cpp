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
    qPushButtonExecute->setToolTip("Execute selected script");
    QObject::connect(qPushButtonExecute, SIGNAL(clicked()), this, SLOT(buttonExecuteClicked()));

    qTableWidgetScript->setColumnCount(6);
    qTableWidgetScript->setHorizontalHeaderItem(0, new QTableWidgetItem("#"));
    qTableWidgetScript->setHorizontalHeaderItem(1, new QTableWidgetItem("Process"));
    qTableWidgetScript->setHorizontalHeaderItem(2, new QTableWidgetItem("Parameter"));
    qTableWidgetScript->setHorizontalHeaderItem(3, new QTableWidgetItem("Timeout"));
    qTableWidgetScript->setHorizontalHeaderItem(4, new QTableWidgetItem("Stop"));
    qTableWidgetScript->setHorizontalHeaderItem(5, new QTableWidgetItem("Log"));
    qTableWidgetScript->setToolTip("Files contained in the specified directory");
    qTableWidgetScript->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qTableWidgetScript->setSelectionBehavior(QAbstractItemView::SelectRows);
    qTableWidgetScript->setSelectionMode(QAbstractItemView::SingleSelection);
    qTableWidgetScript->verticalHeader()->hide();
    qFont.setPointSize(9);
    qTableWidgetScript->setFont(qFont);
    QObject::connect(qTableWidgetScript, SIGNAL(itemActivated(QTableWidgetItem *)), this, SLOT(itemActivated(QTableWidgetItem *)));

    setShowScriptList(true);
    setDetailsLayout(TOP);

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
            qLayoutChild->addWidget(qComboBoxScriptList);
            qLayoutChild->addWidget(qPushButtonNew);
            qLayoutChild->addWidget(qPushButtonSave);
            qLayoutChild->addWidget(qPushButtonDelete);
            qLayoutChild->addWidget(qPushButtonExecute);
            qLayoutMain->addItem(qLayoutChild);
            qLayoutMain->addWidget(qTableWidgetScript);
            break;

        case BOTTOM:
            detailsLayout = BOTTOM;
            qLayoutMain = new QVBoxLayout(this);
            qLayoutMain->addWidget(qTableWidgetScript);
            qLayoutChild = new QHBoxLayout();
            qLayoutChild->addWidget(qComboBoxScriptList);
            qLayoutChild->addWidget(qPushButtonNew);
            qLayoutChild->addWidget(qPushButtonSave);
            qLayoutChild->addWidget(qPushButtonDelete);
            qLayoutChild->addWidget(qPushButtonExecute);
            qLayoutMain->addItem(qLayoutChild);
            break;

        case LEFT:
            detailsLayout = LEFT;
            qLayoutMain = new QHBoxLayout(this);
            qLayoutChild = new QVBoxLayout();
            qLayoutChild->addWidget(qComboBoxScriptList);
            qLayoutChild->addWidget(qPushButtonNew);
            qLayoutChild->addWidget(qPushButtonSave);
            qLayoutChild->addWidget(qPushButtonDelete);
            qLayoutChild->addWidget(qPushButtonExecute);
            qLayoutMain->addItem(qLayoutChild);
            qLayoutMain->addWidget(qTableWidgetScript);
            break;

        case RIGHT:
            detailsLayout = RIGHT;
            qLayoutMain = new QHBoxLayout(this);
            qLayoutChild = new QVBoxLayout();
            qLayoutChild->addWidget(qComboBoxScriptList);
            qLayoutChild->addWidget(qPushButtonNew);
            qLayoutChild->addWidget(qPushButtonSave);
            qLayoutChild->addWidget(qPushButtonDelete);
            qLayoutChild->addWidget(qPushButtonExecute);
            qLayoutMain->addWidget(qTableWidgetScript);
            qLayoutMain->addItem(qLayoutChild);
    }

}



int QEScript::getDetailsLayout()
{

    return detailsLayout;

}




void QEScript::comboBoxScriptSelected(int)
{

    //refreshButton();

}




void QEScript::buttonNewClicked()
{

    /*
    QDomElement rootElement;
    QDomElement recipeElement;
    QDomElement processVariableElement;
    QDomNode rootNode;
    _Field *fieldInfo;
    QString currentName;
    QString name;
    QString visible;
    bool flag;
    int count;
    int i;


    do
    {
        name = QInputDialog::getText(this, "New Recipe", "Name:", QLineEdit::Normal , "", &flag);
    }
    while(flag && name.isEmpty());

    if (name.isEmpty() == false)
    {
        flag = true;
        count = 0;
        rootElement = document.documentElement();
        if (rootElement.tagName() == "epicsqt")
        {
            rootNode = rootElement.firstChild();
            while (rootNode.isNull() == false)
            {
                recipeElement = rootNode.toElement();
                if (recipeElement.tagName() == "recipe")
                {
                    if (recipeElement.attribute("name").isEmpty())
                    {
                        currentName = "Recipe #" + QString::number(count);
                        count++;
                    }
                    else
                    {
                        currentName = recipeElement.attribute("name");
                    }
                    if (currentName.compare(name) == 0)
                    {
                        flag = false;
                        break;
                    }
                }
                rootNode = rootNode.nextSibling();
            }
        }
        if (flag == false)
        {
            visible = recipeElement.attribute("visible").toUpper();
            if (visible.isEmpty())
            {
                flag = true;
            }
            else if (visible == "USER")
            {
                flag = true;
            }
            else if (visible == "SCIENTIST")
            {
                flag = (currentUserType > 0);
            }
            else if (visible == "ENGINEER")
            {
                flag = (currentUserType > 1);
            }
            else
            {
                flag = false;
            }
            if (flag)
            {
                flag = (QMessageBox::question(this, "Info", "Do you want to replace existing recipe '" + name + "'?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes);
                if (flag)
                {
                    rootElement.removeChild(rootNode);
                }
            }
            else
            {
                QMessageBox::warning(this, "Warning", "Unable to create recipe '" + name + "' since it already exists and belongs to another user type with more priviledges!");
            }
        }
        else
        {
            switch (currentUserType)
            {
                case userLevelTypes::USERLEVEL_USER:
                    visible = "USER";
                    break;
                case userLevelTypes::USERLEVEL_SCIENTIST:
                    visible = "SCIENTIST";
                    break;
                default:
                    visible = "ENGINEER";
            }
        }
        if (flag)
        {
            recipeElement = document.createElement("recipe");
            recipeElement.setAttribute("name", name);
            recipeElement.setAttribute("visible", visible);
            for(i = 0; i < qEConfiguredLayoutRecipeFields->currentFieldList.size(); i++)
            {
                fieldInfo = qEConfiguredLayoutRecipeFields->currentFieldList.at(i);
                processVariableElement = document.createElement("processvariable");
                processVariableElement.setAttribute("name", fieldInfo->getProcessVariable());
                if (fieldInfo->getType() == BITSTATUS)
                {
                }
                else if (fieldInfo->getType() == BUTTON)
                {
                }
                else if (fieldInfo->getType() == LABEL)
                {
                }
                else if (fieldInfo->getType() == SPINBOX)
                {
                    processVariableElement.setAttribute("value", ((QESpinBox *) fieldInfo->qCaWidget)->text());
                }
                else if (fieldInfo->getType() == COMBOBOX)
                {
                    processVariableElement.setAttribute("value", ((QEComboBox *) fieldInfo->qCaWidget)->currentText());
                }
                else
                {
                    processVariableElement.setAttribute("value", ((QELineEdit *) fieldInfo->qCaWidget)->text());
                }
                recipeElement.appendChild(processVariableElement);
            }
            rootElement.appendChild(recipeElement);
            if (saveRecipeList())
            {
                QMessageBox::information(this, "Info", "The recipe '" + name + "' was successfully created!");
            }
            else
            {
                // TODO: restore original document if there is an error
                //rootElement.removeChild(recipeElement);
                QMessageBox::critical(this, "Error", "Unable to create recipe '" + name + "' in file '" + filename + "'!");
            }
        }
    }
    */

}




void QEScript::buttonSaveClicked()
{

    /*
    QDomElement rootElement;
    QDomElement recipeElement;
    QDomElement processVariableElement;
    QDomNode rootNode;
    _Field *fieldInfo;
    QString currentName;
    QString name;
    int count;
    int i;


    currentName = qComboBoxRecipeList->currentText();

    if (QMessageBox::question(this, "Info", "Do you want to save the values in recipe '" + currentName + "'?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    {
        count = 0;
        rootElement = document.documentElement();
        if (rootElement.tagName() == "epicsqt")
        {
            rootNode = rootElement.firstChild();
            while (rootNode.isNull() == false)
            {
                recipeElement = rootNode.toElement();
                if (recipeElement.tagName() == "recipe")
                {
                    if (recipeElement.attribute("name").isEmpty())
                    {
                        name= "Recipe #" + QString::number(count);
                        count++;
                    }
                    else
                    {
                        name = recipeElement.attribute("name");
                    }
                    if (currentName.compare(name) == 0)
                    {
                        break;
                    }
                }
                rootNode = rootNode.nextSibling();
            }
        }

        while (recipeElement.hasChildNodes())
        {
            recipeElement.removeChild(recipeElement.lastChild());
        }

        for(i = 0; i < qEConfiguredLayoutRecipeFields->currentFieldList.size(); i++)
        {
            fieldInfo = qEConfiguredLayoutRecipeFields->currentFieldList.at(i);
            processVariableElement = document.createElement("processvariable");
            processVariableElement.setAttribute("name", fieldInfo->getProcessVariable());
            if (fieldInfo->getType() == BITSTATUS)
            {
            }
            else if (fieldInfo->getType() == BUTTON)
            {
            }
            else if (fieldInfo->getType() == LABEL)
            {
            }
            else if (fieldInfo->getType() == SPINBOX)
            {
                processVariableElement.setAttribute("value", ((QESpinBox *) fieldInfo->qCaWidget)->text());
            }
            else if (fieldInfo->getType() == COMBOBOX)
            {
                processVariableElement.setAttribute("value", ((QEComboBox *) fieldInfo->qCaWidget)->currentText());
            }
            else
            {
                processVariableElement.setAttribute("value", ((QELineEdit *) fieldInfo->qCaWidget)->text());
            }
            recipeElement.appendChild(processVariableElement);
        }

        if (saveRecipeList())
        {
            QMessageBox::information(this, "Info", "The recipe '" + currentName + "' was successfully saved!");
        }
        else
        {
            // TODO: restore original document if there is an error
            QMessageBox::critical(this, "Error", "Unable to save recipe '" + currentName + "' in file '" + filename + "'!");
        }
    }
    */

}




void QEScript::buttonDeleteClicked()
{

    /*
    QDomElement rootElement;
    QDomElement recipeElement;
    QDomNode rootNode;
    QString currentName;
    QString name;
    int count;


    currentName = qComboBoxRecipeList->currentText();

    if (QMessageBox::question(this, "Info", "Do you want to delete recipe '" + currentName + "'?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    {
        count = 0;
        rootElement = document.documentElement();
        if (rootElement.tagName() == "epicsqt")
        {
            rootNode = rootElement.firstChild();
            while (rootNode.isNull() == false)
            {
                recipeElement = rootNode.toElement();
                if (recipeElement.tagName() == "recipe")
                {
                    if (recipeElement.attribute("name").isEmpty())
                    {
                        name = "Recipe #" + QString::number(count);
                        count++;
                    }
                    else
                    {
                        name = recipeElement.attribute("name");
                    }
                    if (currentName.compare(name) == 0)
                    {
                        rootElement.removeChild(rootNode);
                        break;
                    }
                }
                rootNode = rootNode.nextSibling();
            }
        }
        if (saveRecipeList())
        {
            QMessageBox::information(this, "Info", "The recipe '" + currentName + "' was successfully delete!");
        }
        else
        {
            // TODO: restore original document if there is an error
            QMessageBox::critical(this, "Error", "Unable to delete recipe '" + currentName + "' in file '" + filename + "'!");
        }
    }
    */

}




void QEScript::buttonExecuteClicked()
{

    /*
    _Field *fieldInfo;
    int i;


    if (QMessageBox::question(this, "Info", "Do you want to read the values from process variables?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    {
        for(i = 0; i < qEConfiguredLayoutRecipeFields->currentFieldList.size(); i++)
        {
            fieldInfo = qEConfiguredLayoutRecipeFields->currentFieldList.at(i);
            if (fieldInfo->getVisibility())
            {
                fieldInfo->qCaWidget->readNow();
            }
        }
        QMessageBox::information(this, "Info", "The values were successfully read from the process variables!");
    }
    */

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
    refreshButton();
    qComboBoxScriptList->blockSignals(false);

}




void QEScript::refreshButton()
{

    QDomElement rootElement;
    QDomElement recipeElement;
    QDomElement processVariableElement;
    QDomNode rootNode;
    //_Field *fieldInfo;
    QString currentName;
    QString name;
    //int count;
    //int i;

    currentName = qComboBoxScriptList->currentText();


//    qDebug() << "recipe: " + currentName;

    /*
    count = 0;
    rootElement = document.documentElement();
    if (rootElement.tagName() == "epicsqt")
    {
        rootNode = rootElement.firstChild();
        while (rootNode.isNull() == false)
        {
            recipeElement = rootNode.toElement();
            if (recipeElement.tagName() == "recipe")
            {
                if (recipeElement.attribute("name").isEmpty())
                {
                    name = "Recipe #" + QString::number(count);
                    count++;
                }
                else
                {
                    name = recipeElement.attribute("name");
                }

                if (currentName.compare(name) == 0)
                {
                    for(i = 0; i < qEConfiguredLayoutRecipeFields->currentFieldList.size(); i++)
                    {
                        fieldInfo = qEConfiguredLayoutRecipeFields->currentFieldList.at(i);

                        if (fieldInfo->getVisibility())
                        {
                            rootNode = recipeElement.firstChild();
                            while (rootNode.isNull() == false)
                            {
                                processVariableElement = rootNode.toElement();
                                if (processVariableElement.tagName() == "processvariable")
                                {
                                    if (fieldInfo->getProcessVariable() == processVariableElement.attribute("name"))
                                    {

                                        if (fieldInfo->getType() == BITSTATUS)
                                        {
                                        }
                                        else if (fieldInfo->getType() == BUTTON)
                                        {
                                        }
                                        else if (fieldInfo->getType() == LABEL)
                                        {
                                        }
                                        else if (fieldInfo->getType() == SPINBOX)
                                        {
//                                            ((QESpinBox *) fieldInfo->qCaWidget)->setValue((float) processVariableElement.attribute("value"));
                                        }
                                        else if (fieldInfo->getType() == COMBOBOX)
                                        {
                                            ((QEComboBox *) fieldInfo->qCaWidget)->setEditText(processVariableElement.attribute("value"));
                                        }
                                        else
                                        {
                                            ((QELineEdit *) fieldInfo->qCaWidget)->setText(processVariableElement.attribute("value"));
                                        }
                                        break;
                                    }
                                }
                                rootNode = rootNode.nextSibling();
                            }
                        }

                    }

                    break;
                }
            }
            rootNode = rootNode.nextSibling();
        }
    }
    */


    qPushButtonSave->setEnabled(qComboBoxScriptList->currentText().isEmpty() == false);
    qPushButtonDelete->setEnabled(qComboBoxScriptList->currentText().isEmpty() == false);
    qPushButtonExecute->setEnabled(qComboBoxScriptList->currentText().isEmpty() == false);

}



void QEScript::updateTable()
{

    /*
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
    */

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



