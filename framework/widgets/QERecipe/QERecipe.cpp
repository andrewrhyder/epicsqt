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

#include <QERecipe.h>



// ============================================================
//  QERECIPE METHODS
// ============================================================
QERecipe::QERecipe(QWidget *pParent):QWidget(pParent), QCaWidget(this)
{

    qLabelRecipeDescription = new QLabel(this);
    qComboBoxRecipeList = new QComboBox(this);
    qPushButtonNew = new QPushButton(this);
    qPushButtonSave = new QPushButton(this);
    qPushButtonDelete = new QPushButton(this);
    qPushButtonApply = new QPushButton(this);
    qPushButtonRead = new QPushButton(this);
    qEConfiguredLayoutRecipeFields = new QEConfiguredLayout(this);


    qComboBoxRecipeList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    qComboBoxRecipeList->setToolTip("Select recipe");
    QObject::connect(qComboBoxRecipeList, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxRecipeSelected(int)));

    qPushButtonNew->setText("New");
    qPushButtonNew->setToolTip("Create new recipe");
    QObject::connect(qPushButtonNew, SIGNAL(clicked()), this, SLOT(buttonNewClicked()));

    qPushButtonSave->setText("Save");
    qPushButtonSave->setToolTip("Save values in the selected recipe");
    qPushButtonSave->setEnabled(false);
    QObject::connect(qPushButtonSave, SIGNAL(clicked()), this, SLOT(buttonSaveClicked()));

    qPushButtonDelete->setText("Delete");
    qPushButtonDelete->setToolTip("Delete selected recipe");
    qPushButtonDelete->setEnabled(false);
    QObject::connect(qPushButtonDelete, SIGNAL(clicked()), this, SLOT(buttonDeleteClicked()));

    qPushButtonApply->setText("Apply");
    qPushButtonApply->setToolTip("Apply values to process variables");
    qPushButtonApply->setEnabled(false);
    QObject::connect(qPushButtonApply, SIGNAL(clicked()), this, SLOT(buttonApplyClicked()));

    qPushButtonRead->setText("Read");
    qPushButtonRead->setToolTip("Read values from process variables");
    QObject::connect(qPushButtonRead, SIGNAL(clicked()), this, SLOT(buttonReadClicked()));

    qEConfiguredLayoutRecipeFields->setShowItemList(false);

    setRecipeFile("");
    setConfigurationFile("");
    setConfigurationText("");
    setConfigurationType(FROM_FILE);
    setShowRecipeList(true);
    setDetailsLayout(TOP);
    setCurrentUserType(getUserLevel());

}





void QERecipe::setRecipeDescription(QString pValue)
{

    qLabelRecipeDescription->setText(pValue);
    qLabelRecipeDescription->setVisible(qLabelRecipeDescription->text().isEmpty() == false);

}





QString QERecipe::getRecipeDescription()
{

    return qLabelRecipeDescription->text();

}




void QERecipe::setShowRecipeList(bool pValue)
{

    qComboBoxRecipeList->setVisible(pValue);

}



bool QERecipe::getShowRecipeList()
{

    return qComboBoxRecipeList->isVisible();

}




void QERecipe::setShowNew(bool pValue)
{

    qPushButtonNew->setVisible(pValue);

}



bool QERecipe::getShowNew()
{

    return qPushButtonNew->isVisible();

}





void QERecipe::setShowSave(bool pValue)
{

    qPushButtonSave->setVisible(pValue);

}




bool QERecipe::getShowSave()
{

    return qPushButtonSave->isVisible();

}




void QERecipe::setShowDelete(bool pValue)
{

    qPushButtonDelete->setVisible(pValue);

}



bool QERecipe::getShowDelete()
{

    return qPushButtonDelete->isVisible();

}



void QERecipe::setShowApply(bool pValue)
{

    qPushButtonApply->setVisible(pValue);

}



bool QERecipe::getShowApply()
{

    return qPushButtonApply->isVisible();

}




void QERecipe::setShowRead(bool pValue)
{

    qPushButtonRead->setVisible(pValue);

}




bool QERecipe::getShowRead()
{

    return qPushButtonRead->isVisible();

}



void QERecipe::setShowFields(bool pValue)
{

    qEConfiguredLayoutRecipeFields->setVisible(pValue);

}



bool QERecipe::getShowFields()
{

    return qEConfiguredLayoutRecipeFields->isVisible();

}




void QERecipe::setConfigurationType(int pValue)
{

    qEConfiguredLayoutRecipeFields->setConfigurationType(pValue);

}




int QERecipe::getConfigurationType()
{

    return qEConfiguredLayoutRecipeFields->getConfigurationType();

}





void QERecipe::setConfigurationFile(QString pValue)
{

    qEConfiguredLayoutRecipeFields->setConfigurationFile(pValue);

}




QString QERecipe::getConfigurationFile()
{

    return qEConfiguredLayoutRecipeFields->getConfigurationFile();

}





void QERecipe::setConfigurationText(QString pValue)
{

    qEConfiguredLayoutRecipeFields->setConfigurationText(pValue);

}




QString QERecipe::getConfigurationText()
{

    return qEConfiguredLayoutRecipeFields->getConfigurationText();

}




void QERecipe::setRecipeFile(QString pValue)
{

    QDomElement rootElement;
    QFile *file;
    QString data;
    bool flag;


    recipeFile = pValue;
    document.clear();

    if (recipeFile.isEmpty())
    {
        //TODO: the EPICS Qt core should provide a method which returns the path where resources should be stored
        filename = QApplication::applicationFilePath() + QDir::separator() + "QERecipe.xml";
    }
    else
    {
        filename = recipeFile;
    }

    file = new QFile(filename);
    if (file->open(QFile::ReadOnly | QFile::Text))
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
        refreshRecipeList();
    }
    else
    {
        rootElement = document.createElement("epicsqt");
        document.appendChild(rootElement);
    }

}



QString QERecipe::getRecipeFile()
{

    return recipeFile;

}



void QERecipe::setDetailsLayout(int pValue)
{

    QLayout *qLayoutMain;
    QLayout *qLayoutChild;


    delete layout();


    //TODO: fix issue of buttons not being centered when using LEFT and RIGHT layout

    switch(pValue)
    {
        case TOP:
            detailsLayout = TOP;
            qLayoutMain = new QVBoxLayout(this);
            qLayoutChild = new QHBoxLayout();
            qLayoutChild->addWidget(qLabelRecipeDescription);
            qLayoutChild->addWidget(qComboBoxRecipeList);
            qLayoutChild->addWidget(qPushButtonNew);
            qLayoutChild->addWidget(qPushButtonSave);
            qLayoutChild->addWidget(qPushButtonDelete);
            qLayoutChild->addWidget(qPushButtonApply);
            qLayoutChild->addWidget(qPushButtonRead);
            qLayoutMain->addItem(qLayoutChild);
            qLayoutMain->addWidget(qEConfiguredLayoutRecipeFields);
            break;

        case BOTTOM:
            detailsLayout = BOTTOM;
            qLayoutMain = new QVBoxLayout(this);
            qLayoutMain->addWidget(qEConfiguredLayoutRecipeFields);
            qLayoutChild = new QHBoxLayout();
            qLayoutChild->addWidget(qLabelRecipeDescription);
            qLayoutChild->addWidget(qComboBoxRecipeList);
            qLayoutChild->addWidget(qPushButtonNew);
            qLayoutChild->addWidget(qPushButtonSave);
            qLayoutChild->addWidget(qPushButtonDelete);
            qLayoutChild->addWidget(qPushButtonApply);
            qLayoutChild->addWidget(qPushButtonRead);
            qLayoutMain->addItem(qLayoutChild);
            break;

        case LEFT:
            detailsLayout = LEFT;
            qLayoutMain = new QHBoxLayout(this);
            qLayoutChild = new QVBoxLayout();
            qLayoutChild->addWidget(qLabelRecipeDescription);
            qLayoutChild->addWidget(qComboBoxRecipeList);
            qLayoutChild->addWidget(qPushButtonNew);
            qLayoutChild->addWidget(qPushButtonSave);
            qLayoutChild->addWidget(qPushButtonDelete);
            qLayoutChild->addWidget(qPushButtonApply);
            qLayoutChild->addWidget(qPushButtonRead);
            qLayoutMain->addItem(qLayoutChild);
            qLayoutMain->addWidget(qEConfiguredLayoutRecipeFields);
            break;

        case RIGHT:
            detailsLayout = RIGHT;
            qLayoutMain = new QHBoxLayout(this);
            qLayoutChild = new QVBoxLayout();
            qLayoutChild->addWidget(qLabelRecipeDescription);
            qLayoutChild->addWidget(qComboBoxRecipeList);
            qLayoutChild->addWidget(qPushButtonNew);
            qLayoutChild->addWidget(qPushButtonSave);
            qLayoutChild->addWidget(qPushButtonDelete);
            qLayoutChild->addWidget(qPushButtonApply);
            qLayoutChild->addWidget(qPushButtonRead);
            qLayoutMain->addWidget(qEConfiguredLayoutRecipeFields);
            qLayoutMain->addItem(qLayoutChild);
    }

}



int QERecipe::getDetailsLayout()
{

    return detailsLayout;

}




void QERecipe::setCurrentUserType(int pValue)
{

    if (pValue == USERLEVEL_USER || pValue == USERLEVEL_SCIENTIST || pValue == USERLEVEL_ENGINEER)
    {
        currentUserType = pValue;
        refreshRecipeList();
        qEConfiguredLayoutRecipeFields->setCurrentUserType(currentUserType);
    }

}



int QERecipe::getCurrentUserType()
{

    return currentUserType;

}





void QERecipe::comboBoxRecipeSelected(int)
{

    //TODO: should update the widgets with the values of the recipe

    refreshButton();

}





void QERecipe::buttonNewClicked()
{

    QDomElement rootElement;
    QDomElement recipeElement;
    QDomElement processVariableElement;
    QDomNode rootNode;
    QCaWidget *qCaWidget;
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
                case USERLEVEL_USER:
                    visible = "USER";
                    break;
                case USERLEVEL_SCIENTIST:
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
                qCaWidget = qEConfiguredLayoutRecipeFields->currentFieldList.at(i);
                processVariableElement = document.createElement("processvariable");
                processVariableElement.setAttribute("name", ((_QELineEdit *) qCaWidget)->getProcessVariable());
                processVariableElement.setAttribute("value", ((_QELineEdit *) qCaWidget)->text());
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

}




void QERecipe::buttonSaveClicked()
{

    QDomElement rootElement;
    QDomElement recipeElement;
    QDomElement processVariableElement;
    QDomNode rootNode;
    QCaWidget *qCaWidget;
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
            qCaWidget = qEConfiguredLayoutRecipeFields->currentFieldList.at(i);
            processVariableElement = document.createElement("processvariable");
            processVariableElement.setAttribute("name", ((_QELineEdit *) qCaWidget)->getProcessVariable());
            processVariableElement.setAttribute("value", ((_QELineEdit *) qCaWidget)->text());
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

}




void QERecipe::buttonDeleteClicked()
{

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

}




void QERecipe::buttonApplyClicked()
{

    QCaWidget *qCaWidget;
    int i;

    if (QMessageBox::question(this, "Info", "Do you want to apply recipe '" + qComboBoxRecipeList->currentText() + "' to process variables?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    {
        for(i = 0; i < qEConfiguredLayoutRecipeFields->currentFieldList.size(); i++)
        {
            qCaWidget = qEConfiguredLayoutRecipeFields->currentFieldList.at(i);
            if (((_QELineEdit *) qCaWidget)->isVisible())
            {
                // TODO: should call method to apply values to PVs
            }
        }
        QMessageBox::information(this, "Info", "The recipe '" + qComboBoxRecipeList->currentText() + "' was successfully applied to process variables!");
    }

}



void QERecipe::buttonReadClicked()
{

    QCaWidget *qCaWidget;
    int i;

    if (QMessageBox::question(this, "Info", "Do you want to read the values from process variables?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    {
        for(i = 0; i < qEConfiguredLayoutRecipeFields->currentFieldList.size(); i++)
        {
            qCaWidget = qEConfiguredLayoutRecipeFields->currentFieldList.at(i);
            if (((_QELineEdit *) qCaWidget)->isVisible())
            {
                // TODO: should call method to read values from PVs
            }
        }
        QMessageBox::information(this, "Info", "The values were successfully read from the process variables!");
    }

}




bool QERecipe::saveRecipeList()
{

    QFile *file;

    file = new QFile(filename);
    if (file->open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream stream(file);
        document.save(stream, 3);
        file->close();
        refreshRecipeList();
        return true;
    }
    else
    {
        return false;
    }

}




void QERecipe::refreshRecipeList()
{

    QDomElement rootElement;
    QDomElement recipeElement;
    QDomNode rootNode;
    QString visible;
    QString tmp;
    bool flag;
    int count;
    int i;


    qComboBoxRecipeList->blockSignals(true);
    tmp = qComboBoxRecipeList->currentText();
    qComboBoxRecipeList->clear();
    rootElement = document.documentElement();
    if (rootElement.tagName() == "epicsqt")
    {
        count = 0;
        rootNode = rootElement.firstChild();
        while (rootNode.isNull() == false)
        {
            recipeElement = rootNode.toElement();
            if (recipeElement.tagName() == "recipe")
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
                }
                if (flag)
                {
                    if (recipeElement.attribute("name").isEmpty())
                    {
                        qComboBoxRecipeList->addItem("Recipe #" + QString::number(count));
                        count++;
                    }
                    else
                    {
                        qComboBoxRecipeList->addItem(recipeElement.attribute("name"));
                    }
                }
            }
            rootNode = rootNode.nextSibling();
        }
    }
    i = qComboBoxRecipeList->findText(tmp);
    if (i == -1)
    {
        qComboBoxRecipeList->setCurrentIndex(0);
    }
    else
    {
       qComboBoxRecipeList->setCurrentIndex(i);
    }
    refreshButton();
    qComboBoxRecipeList->blockSignals(false);

}




void QERecipe::refreshButton()
{

    qPushButtonSave->setEnabled(qComboBoxRecipeList->currentText().isEmpty() == false);
    qPushButtonDelete->setEnabled(qComboBoxRecipeList->currentText().isEmpty() == false);
    qPushButtonApply->setEnabled(qComboBoxRecipeList->currentText().isEmpty() == false);

}



void QERecipe::userLevelChanged(userLevels pValue)
{

    setCurrentUserType(pValue);

}


