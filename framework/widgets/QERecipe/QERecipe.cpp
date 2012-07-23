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
#include <QERecipe.h>
#include <QDomDocument>
#include <QDebug>




// ============================================================
//  QERECIPE METHODS
// ============================================================
QERecipe::QERecipe(QWidget *pParent):QWidget(pParent), QCaWidget( this )
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
    qPushButtonSave->setToolTip("Save current values in the selected recipe");
    qPushButtonSave->setEnabled(false);
    QObject::connect(qPushButtonSave, SIGNAL(clicked()), this, SLOT(buttonSaveClicked()));

    qPushButtonDelete->setText("Delete");
    qPushButtonDelete->setToolTip("Delete selected recipe");
    qPushButtonDelete->setEnabled(false);
    QObject::connect(qPushButtonDelete, SIGNAL(clicked()), this, SLOT(buttonDeleteClicked()));

    qPushButtonApply->setText("Apply");
    qPushButtonApply->setToolTip("Apply selected recipe");
    qPushButtonApply->setEnabled(false);
    QObject::connect(qPushButtonApply, SIGNAL(clicked()), this, SLOT(buttonApplyClicked()));

    qPushButtonRead->setText("Read");
    qPushButtonRead->setToolTip("Read current values from process variables");
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
    QString filename;
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

    qPushButtonSave->setEnabled(qComboBoxRecipeList->currentText().isEmpty() == false);
    qPushButtonDelete->setEnabled(qComboBoxRecipeList->currentText().isEmpty() == false);
    qPushButtonApply->setEnabled(qComboBoxRecipeList->currentText().isEmpty() == false);

}





void QERecipe::buttonNewClicked()
{

    QDomElement rootElement;
    QDomElement recipeElement;
    QDomNode rootNode;
    QString filename;
    QString newName;
    QString currentName;
    bool flag;
    int count;


    do
    {
        newName = QInputDialog::getText(this, "New Recipe", "Name:", QLineEdit::Normal , "", &flag);
    }
    while(flag && newName.isEmpty());


    if (newName.isEmpty() == false)
    {
        flag = true;
        count = 0;
        rootElement = document.documentElement();
        if (rootElement.tagName() == "epicsqt")
        {
            rootNode = rootElement.firstChild();
            while (flag && rootNode.isNull() == false)
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
                    if (currentName.compare(newName) == 0)
                    {
                        flag = false;
                    }
                }
                rootNode = rootNode.nextSibling();
            }
        }

        if (flag == false)
        {
            flag = (QMessageBox::question(this, "Info", "Do you want to replace existing recipe '" + newName + "'?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes);
        }

        if (flag)
        {
            rootElement = document.documentElement();
            recipeElement = document.createElement("recipe");
            recipeElement.setAttribute("name", newName);
            switch (currentUserType)
            {
                case USERLEVEL_USER:
                    recipeElement.setAttribute("visible", "USER");
                    break;
                case USERLEVEL_SCIENTIST:
                    recipeElement.setAttribute("visible", "SCIENTIST");
                    break;
                default:
                    recipeElement.setAttribute("visible", "ENGINEER");
            }
            rootElement.appendChild(recipeElement);

            if (recipeFile.isEmpty())
            {
                //TODO: the EPICS Qt core should provide a method which returns the path where resources should be stored
                filename = QApplication::applicationFilePath() + QDir::separator() + "QERecipe.xml";
            }
            else
            {
                filename = recipeFile;
            }
            if (saveRecipeList(filename))
            {
                QMessageBox::information(this, "Info", "The recipe '" + newName + "' was successfully saved!");
            }
            else
            {
                QMessageBox::critical(this, "Error", "Unable to save recipe '" + newName + "' in file '" + filename + "'!");
            }

        }
    }


}




void QERecipe::buttonSaveClicked()
{

    QString filename;

    if (QMessageBox::question(this, "Info", "Do you want to save the current values in recipe '" + qComboBoxRecipeList->currentText() + "'?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    {
        if (recipeFile.isEmpty())
        {
            //TODO: the EPICS Qt core should provide a method which returns the path where resources should be stored
            filename = QApplication::applicationFilePath() + QDir::separator() + "QERecipe.xml";
        }
        else
        {
            filename = recipeFile;
        }
        if (saveRecipeList(filename))
        {
            QMessageBox::information(this, "Info", "The recipe '" + qComboBoxRecipeList->currentText() + "' was successfully saved!");
        }
        else
        {
            QMessageBox::critical(this, "Error", "Unable to save recipe '" + qComboBoxRecipeList->currentText() + "' in file '" + filename + "'!");
        }
    }

}




void QERecipe::buttonDeleteClicked()
{

    QString filename;

    if (QMessageBox::question(this, "Info", "Do you want to delete recipe '" + qComboBoxRecipeList->currentText() + "'?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    {
        if (recipeFile.isEmpty())
        {
            //TODO: the EPICS Qt core should provide a method which returns the path where resources should be stored
            filename = QApplication::applicationFilePath() + QDir::separator() + "QERecipe.xml";
        }
        else
        {
            filename = recipeFile;
        }
        if (saveRecipeList(filename))
        {
            QMessageBox::information(this, "Info", "The recipe '" + qComboBoxRecipeList->currentText() + "' was successfully delete!");
        }
        else
        {
            QMessageBox::critical(this, "Error", "Unable to delete recipe '" + qComboBoxRecipeList->currentText() + "' in file '" + filename + "'!");
        }
    }

}




void QERecipe::buttonApplyClicked()
{

    if (QMessageBox::question(this, "Info", "Do you want to apply recipe '" + qComboBoxRecipeList->currentText() + "'?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    {


    }

}



void QERecipe::buttonReadClicked()
{

    if (QMessageBox::question(this, "Info", "Do you want to read current values from process variables?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    {


    }

}




bool QERecipe::saveRecipeList(QString pFilename)
{

    QFile *file;

    file = new QFile(pFilename);
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
    qComboBoxRecipeList->blockSignals(false);


    // TODO: check that the slot "itemchanged" is called
    i = qComboBoxRecipeList->findText(tmp);
    if (i == -1)
    {
        qComboBoxRecipeList->setCurrentIndex(0);
    }
    else
    {
       qComboBoxRecipeList->setCurrentIndex(i);
    }

}





void QERecipe::userLevelChanged(userLevels pValue)
{

    setCurrentUserType(pValue);

}


