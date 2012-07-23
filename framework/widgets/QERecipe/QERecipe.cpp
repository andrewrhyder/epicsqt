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

    qLabelRecipe = new QLabel(this);
    qComboBoxRecipeList = new QComboBox(this);
    qPushButtonNew = new QPushButton(this);
    qPushButtonSave = new QPushButton(this);
    qPushButtonDelete = new QPushButton(this);
    qPushButtonApply = new QPushButton(this);
    qEConfiguredLayoutRecipeFields = new QEConfiguredLayout(this);

    qLabelRecipe->setText("Recipe");

    qComboBoxRecipeList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    qComboBoxRecipeList->setToolTip("Select recipe");
    QObject::connect(qComboBoxRecipeList, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxRecipeSelected(int)));
//    QObject::connect(qComboBoxRecipeList, SIGNAL(editTextChanged(QString)), this, SLOT(comboBoxRecipeListChanged(QString)));

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

    qEConfiguredLayoutRecipeFields->setShowItemList(false);

    setRecipeFile("");
    setConfigurationFile("");
    setConfigurationText("");
    setConfigurationType(FROM_FILE);
    setShowRecipeList(true);
    setDetailsLayout(TOP);
    setCurrentUserType(getUserLevel());

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
    QDomElement recipeElement;
    QDomNode rootNode;
    QFile *file;
    QString data;
    QString filename;
    bool flag;
    int count;


    recipeFile = pValue;
    document.clear();
    qComboBoxRecipeList->clear();

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
                    data = recipeElement.attribute("name");
                    if (data.isEmpty())
                    {
                        qComboBoxRecipeList->addItem("Recipe #" + QString::number(count));
                        count++;
                    }
                    else
                    {
                        qComboBoxRecipeList->addItem(data);
                    }
                }
                rootNode = rootNode.nextSibling();
            }
        }
    }
    else
    {
        rootNode = document.createElement("epicsqt");
        document.appendChild(rootNode);
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
            qLayoutChild->addWidget(qLabelRecipe);
            qLayoutChild->addWidget(qComboBoxRecipeList);
            qLayoutChild->addWidget(qPushButtonNew);
            qLayoutChild->addWidget(qPushButtonSave);
            qLayoutChild->addWidget(qPushButtonDelete);
            qLayoutChild->addWidget(qPushButtonApply);
            qLayoutMain->addItem(qLayoutChild);
            qLayoutMain->addWidget(qEConfiguredLayoutRecipeFields);
            break;

        case BOTTOM:
            detailsLayout = BOTTOM;
            qLayoutMain = new QVBoxLayout(this);
            qLayoutMain->addWidget(qEConfiguredLayoutRecipeFields);
            qLayoutChild = new QHBoxLayout();
            qLayoutChild->addWidget(qLabelRecipe);
            qLayoutChild->addWidget(qComboBoxRecipeList);
            qLayoutChild->addWidget(qPushButtonNew);
            qLayoutChild->addWidget(qPushButtonSave);
            qLayoutChild->addWidget(qPushButtonDelete);
            qLayoutChild->addWidget(qPushButtonApply);
            qLayoutMain->addItem(qLayoutChild);
            break;

        case LEFT:
            detailsLayout = LEFT;
            qLayoutMain = new QHBoxLayout(this);
            qLayoutChild = new QVBoxLayout();
            qLayoutChild->addWidget(qLabelRecipe);
            qLayoutChild->addWidget(qComboBoxRecipeList);
            qLayoutChild->addWidget(qPushButtonNew);
            qLayoutChild->addWidget(qPushButtonSave);
            qLayoutChild->addWidget(qPushButtonDelete);
            qLayoutChild->addWidget(qPushButtonApply);
            qLayoutMain->addItem(qLayoutChild);
            qLayoutMain->addWidget(qEConfiguredLayoutRecipeFields);
            break;

        case RIGHT:
            detailsLayout = RIGHT;
            qLayoutMain = new QHBoxLayout(this);
            qLayoutChild = new QVBoxLayout();
            qLayoutChild->addWidget(qLabelRecipe);
            qLayoutChild->addWidget(qComboBoxRecipeList);
            qLayoutChild->addWidget(qPushButtonNew);
            qLayoutChild->addWidget(qPushButtonSave);
            qLayoutChild->addWidget(qPushButtonDelete);
            qLayoutChild->addWidget(qPushButtonApply);
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

//    list<_Item *>::iterator iterator;
//    QString tmp;
//    QString userType;
//    int i;


//    if (pValue == USERLEVEL_USER || pValue == USERLEVEL_SCIENTIST || pValue == USERLEVEL_ENGINEER)
//    {
//        currentUserType = pValue;
//        switch (currentUserType)
//        {
//            case USERLEVEL_USER:
//                userType = "USER";
//                break;
//            case USERLEVEL_SCIENTIST:
//                userType = "SCIENTIST";
//                break;
//            default:
//                userType = "ENGINEER";
//        }
//        qComboBoxItemList->blockSignals(true);
//        tmp = qComboBoxItemList->currentText();
//        qComboBoxItemList->clear();
//        iterator = itemList.begin();
//        while(iterator != itemList.end())
//        {
//            if ((*iterator)->getVisible().isEmpty() || (*iterator)->getVisible().split(",").contains(userType, Qt::CaseInsensitive))
//            {
//                qComboBoxItemList->addItem((*iterator)->getName());
//            }
//            iterator++;
//        }
//        i = qComboBoxItemList->findText(tmp);
//        if (i == -1)
//        {
//            qComboBoxItemList->setCurrentIndex(0);
//        }
//        else
//        {
//           qComboBoxItemList->setCurrentIndex(i);
//        }
//        refreshFields();
//        qComboBoxItemList->blockSignals(false);
//    }

    qEConfiguredLayoutRecipeFields->setCurrentUserType(pValue);


}



int QERecipe::getCurrentUserType()
{

    return currentUserType;

}





void QERecipe::comboBoxRecipeSelected(int)
{

    qPushButtonSave->setEnabled(true);
    qPushButtonDelete->setEnabled(true);
    qPushButtonApply->setEnabled(true);

}




//void QERecipe::comboBoxRecipeListChanged(QString pValue)
//{

//    qPushButtonSave->setEnabled(pValue.isEmpty() == false);
//    qPushButtonDelete->setEnabled(pValue.isEmpty() == false);

//}





void QERecipe::buttonNewClicked()
{

    QDomElement rootElement;
    QDomElement recipeElement;
    QString filename;


    rootElement = document.documentElement();
    recipeElement = document.createElement("recipe");
    recipeElement.attribute("name", "myName");
    recipeElement.attribute("value", "myValue");
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


    QFile outFile(filename);

    if(outFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&outFile);
        stream << document.toString();
        outFile.close();
//        QMessageBox::information(this, "Info", "The new recipe '" + qComboBoxRecipeList->lineEdit()->text() + "' was successfully created!");
    }
    else
    {
//        QMessageBox::critical(this, "Error", "Unable to create new recipe '" + qComboBoxRecipeList->lineEdit()->text() + "' in file '" + filename + "'!");
    }


}




void QERecipe::buttonSaveClicked()
{

    QString filename;
    bool flag;


    if (QMessageBox::question(this, "Info", "Do you want to save the current values in recipe '" + qComboBoxRecipeList->lineEdit()->text() + "'?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    {

        if (recipeFile.isEmpty())
        {
            filename = "QERecipe.xml";
        }
        else
        {
            filename = recipeFile;
        }


        // store document in file
        flag = true;

        if (flag)
        {
            QMessageBox::information(this, "Info", "The current values were successfully saved in recipe '" + qComboBoxRecipeList->lineEdit()->text() + "'!");
        }
        else
        {
            QMessageBox::critical(this, "Error", "Unable to save recipe '" + qComboBoxRecipeList->lineEdit()->text() + "' in file '" + filename + "'!");
        }

    }


}




void QERecipe::buttonDeleteClicked()
{

    if (QMessageBox::question(this, "Info", "Do you want to delete recipe '" + qComboBoxRecipeList->lineEdit()->text() + "'?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    {


    }

}




void QERecipe::buttonApplyClicked()
{

    if (QMessageBox::question(this, "Info", "Do you want to apply recipe '" + qComboBoxRecipeList->lineEdit()->text() + "'?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    {


    }

}


