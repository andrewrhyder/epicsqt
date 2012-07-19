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
#include <QDebug>




// ============================================================
//  QERECIPE METHODS
// ============================================================
QERecipe::QERecipe(QWidget *pParent):QWidget(pParent), QCaWidget( this )
{

    qLabelRecipe = new QLabel(this);
    qComboBoxRecipeList = new QComboBox(this);
    qPushButtonSave = new QPushButton(this);
    qPushButtonDelete = new QPushButton(this);
    qPushButtonApply = new QPushButton(this);
    qEConfiguredLayoutRecipeFields = new QEConfiguredLayout(this);


    qLabelRecipe->setText("Recipe");

    qComboBoxRecipeList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    qComboBoxRecipeList->setToolTip("Select recipe to be viewed/saved/delete/applied");
    QObject::connect(qComboBoxRecipeList, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxRecipeSelected(int)));

    qPushButtonSave->setText("Save");
    qPushButtonSave->setToolTip("Save current values in recipe");
    QObject::connect(qPushButtonSave, SIGNAL(clicked()), this, SLOT(buttonSaveClicked()));

    qPushButtonDelete->setText("Delete");
    qPushButtonDelete->setToolTip("Delete selected recipe");
    QObject::connect(qPushButtonDelete, SIGNAL(clicked()), this, SLOT(buttonDeleteClicked()));

    qPushButtonApply->setText("Apply");
    qPushButtonApply->setToolTip("Apply selected recipe");
    QObject::connect(qPushButtonApply, SIGNAL(clicked()), this, SLOT(buttonDeleteClicked()));

    qEConfiguredLayoutRecipeFields->setShowItemList(false);


//    setShowFileExtension(true);
//    setFileFilter("");
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


void QERecipe::setRecipeFile(QString pValue)
{

    recipeFile = pValue;

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


}




void QERecipe::buttonSaveClicked()
{

}




void QERecipe::buttonDeleteClicked()
{

}



void QERecipe::buttonApplyClicked()
{

}



