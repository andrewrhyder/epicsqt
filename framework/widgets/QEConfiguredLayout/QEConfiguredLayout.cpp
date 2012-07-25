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

#include <QEConfiguredLayout.h>
#include <QDomDocument>



// ============================================================
//  QECONFIGUREDLAYOUT METHODS
// ============================================================
QEConfiguredLayout::QEConfiguredLayout(QWidget *pParent):QWidget(pParent), QCaWidget(this)
{

    qScrollArea = new QScrollArea(this);
    qLabelItemDescription = new QLabel(this);
    qVBoxLayoutFields = new QVBoxLayout();

    qComboBoxItemList = new QComboBox(this);
    qComboBoxItemList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    qComboBoxItemList->setToolTip("Select item to be viewed/controlled");
    QObject::connect(qComboBoxItemList, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxItemSelected(int)));

    qScrollArea->setWidgetResizable(true);
    qScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    qScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    setItemDescription("");
    setConfigurationFile("");
    setConfigurationText("");
    setConfigurationType(FROM_FILE);
    setShowItemList(true);
    setDetailsLayout(TOP);
    setCurrentUserType(getUserLevel());

}





void QEConfiguredLayout::setConfigurationType(int pValue)
{

    configurationType = pValue;
    setConfigurationFile(configurationFile);
    setConfigurationText(configurationText);

}





int QEConfiguredLayout::getConfigurationType()
{

    return configurationType;

}



void QEConfiguredLayout::setItemDescription(QString pValue)
{

    qLabelItemDescription->setText(pValue);
    qLabelItemDescription->setVisible(qLabelItemDescription->text().isEmpty() == false);

}





QString QEConfiguredLayout::getItemDescription()
{

    return qLabelItemDescription->text();

}





void QEConfiguredLayout::setShowItemList(bool pValue)
{

    qComboBoxItemList->setVisible(pValue);
    qLabelItemDescription->setVisible(qLabelItemDescription->text().isEmpty() == false);

}





bool QEConfiguredLayout::getShowItemList()
{

    return qComboBoxItemList->isVisible();

}




void QEConfiguredLayout::setConfigurationFile(QString pValue)
{

    QFile *file;
    QString data;

    configurationFile = pValue;
    if (configurationType == FROM_FILE)
    {
        file = new QFile(configurationFile);
        if (file->open(QFile::ReadOnly | QFile::Text))
        {
            data = file->readAll();
            file->close();
        }
        setConfiguration(data);
    }

}




QString QEConfiguredLayout::getConfigurationFile()
{

    return configurationFile;

}





void QEConfiguredLayout::setConfigurationText(QString pValue)
{

    configurationText = pValue;
    if (configurationType == FROM_TEXT)
    {
        setConfiguration(configurationText);
    }

}





QString QEConfiguredLayout::getConfigurationText()
{

    return configurationText;

}




void QEConfiguredLayout::setConfiguration(QString pValue)
{

    QDomDocument document;
    QDomElement rootElement;
    QDomElement itemElement;
    QDomElement fieldElement;
    QDomNode rootNode;
    QDomNode itemNode;
    _Item *item;
    _Group *group;
    _Field *field;
    QString tmp;
    bool flag0;
    bool flag1;
    int count;
    int i;


    itemList.clear();

    if (document.setContent(pValue))
    {
        rootElement = document.documentElement();
        if (rootElement.tagName() == "epicsqt")
        {
            count = 0;
            rootNode = rootElement.firstChild();
            while (rootNode.isNull() == false)
            {
                itemElement = rootNode.toElement();
                if (itemElement.tagName() == "item")
                {
                    flag1 = true;
                    item = new _Item();
                    if (itemElement.attribute("name").isEmpty())
                    {
                        item->setName("Item #" + QString::number(count));
                        count++;
                    }
                    else
                    {
                        item->setName(itemElement.attribute("name"));
                    }
                    item->setSubstitution(itemElement.attribute("substitution"));
                    item->setVisible(itemElement.attribute("visible"));
                    itemNode = itemElement.firstChild();
                    while (itemNode.isNull() == false)
                    {
                        fieldElement = itemNode.toElement();
                        if (fieldElement.tagName() == "field")
                        {
                            field = new _Field();
                            field->setName(fieldElement.attribute("name"));
                            field->setProcessVariable(fieldElement.attribute("processvariable"));
                            field->setJoin(fieldElement.attribute("join").compare("true", Qt::CaseInsensitive) == 0);
                            field->setType(fieldElement.attribute("type"));
                            field->setVisible(fieldElement.attribute("visible"));
                            field->setEditable(fieldElement.attribute("editable"));
                            tmp = fieldElement.attribute("group");
                            if (tmp.isEmpty())
                            {
                                if (flag1)
                                {
                                    flag1 = false;
                                }
                            }
                            else
                            {
                                flag1 = true;
                            }
                            flag0 = true;
                            if (flag1)
                            {
                                for(i = 0; i < item->groupList.size(); i++)
                                {
                                    if (((_Group *) item->groupList.at(i))->getName() == tmp)
                                    {
                                        flag0 = false;
                                        break;
                                    }
                                }
                            }
                            if (flag0)
                            {
                                group = new _Group();
                                group->setName(tmp);
                                group->addField(*field);
                                item->addGroup(*group);
                            }
                        }
                        itemNode = itemNode.nextSibling();
                    }
                    itemList.push_back(item);
                }
                rootNode = rootNode.nextSibling();
            }
        }
    }
    setCurrentUserType(currentUserType);


}





void QEConfiguredLayout::setDetailsLayout(int pValue)
{

    QLayout *qLayoutMain;
    QLayout *qLayoutChild;


    //TODO: fix issue of buttons not being centered when using LEFT and RIGHT layout

    delete layout();

    switch(pValue)
    {
        case TOP:
            detailsLayout = TOP;
            qLayoutMain = new QVBoxLayout(this);
            qLayoutChild = new QHBoxLayout();
            qLayoutChild->addWidget(qLabelItemDescription);
            qLayoutChild->addWidget(qComboBoxItemList);
            qLayoutMain->addItem(qLayoutChild);
            qLayoutMain->addWidget(qScrollArea);
            break;

        case BOTTOM:
            detailsLayout = BOTTOM;
            qLayoutMain = new QVBoxLayout(this);
            qLayoutChild = new QHBoxLayout();
            qLayoutMain->addWidget(qScrollArea);
            qLayoutChild->addWidget(qLabelItemDescription);
            qLayoutChild->addWidget(qComboBoxItemList);
            qLayoutMain->addItem(qLayoutChild);
            break;

        case LEFT:
            detailsLayout = LEFT;
            qLayoutMain = new QHBoxLayout(this);
            qLayoutChild = new QVBoxLayout();
            qLayoutChild->addWidget(qLabelItemDescription);
            qLayoutChild->addWidget(qComboBoxItemList);
            qLayoutMain->addItem(qLayoutChild);
            qLayoutMain->addWidget(qScrollArea);
            break;

        case RIGHT:
            detailsLayout = RIGHT;
            qLayoutMain = new QHBoxLayout(this);
            qLayoutChild = new QVBoxLayout();
            qLayoutMain->addWidget(qScrollArea);
            qLayoutChild->addWidget(qLabelItemDescription);
            qLayoutChild->addWidget(qComboBoxItemList);
            qLayoutMain->addItem(qLayoutChild);
    }

}



int QEConfiguredLayout::getDetailsLayout()
{

    return detailsLayout;

}






void QEConfiguredLayout::userLevelChanged(userLevels pValue)
{

    setCurrentUserType(pValue);

}




void QEConfiguredLayout::setCurrentUserType(int pValue)
{

    QString tmp;
    QString userType;
    int i;


    if (pValue == USERLEVEL_USER || pValue == USERLEVEL_SCIENTIST || pValue == USERLEVEL_ENGINEER)
    {
        currentUserType = pValue;
        switch (currentUserType)
        {
            case USERLEVEL_USER:
                userType = "USER";
                break;
            case USERLEVEL_SCIENTIST:
                userType = "SCIENTIST";
                break;
            default:
                userType = "ENGINEER";
        }
        qComboBoxItemList->blockSignals(true);
        tmp = qComboBoxItemList->currentText();
        qComboBoxItemList->clear();
        for(i = 0; i < itemList.size(); i++)
        {
            if (itemList.at(i)->getVisible().isEmpty() || itemList.at(i)->getVisible().split(",").contains(userType, Qt::CaseInsensitive))
            {
                qComboBoxItemList->addItem(itemList.at(i)->getName());
            }
        }
        i = qComboBoxItemList->findText(tmp);
        if (i == -1)
        {
            qComboBoxItemList->setCurrentIndex(0);
        }
        else
        {
           qComboBoxItemList->setCurrentIndex(i);
        }
        refreshFields();
        qComboBoxItemList->blockSignals(false);
    }


}



int QEConfiguredLayout::getCurrentUserType()
{

    return currentUserType;

}



void QEConfiguredLayout::refreshFields()
{

//    list<_Item *>::iterator iteratorItem;
//    list<_Group *>::iterator iteratorGroup;
//    list<_Field *>::iterator iteratorField;
    QLayout *qLayout;
    QHBoxLayout *qHBoxLayout;
    QHBoxLayout *qHBoxLayoutLast;
    QWidget *qWidget;
    QLabel *qLabel;
    QString userType;
    QCaWidget *qCaWidget;
    _QPushButtonGroup *qPushButtonGroup;
    _Item *item;
    _Group *group;
    _Field *field;
    bool flag;
    int i;
    int j;


    while(qVBoxLayoutFields->isEmpty() == false)
    {
        qLayout = qVBoxLayoutFields->takeAt(0)->layout();
        while(qLayout->isEmpty() == false)
        {
            qWidget = qLayout->takeAt(0)->widget();
            delete qWidget;
        }
        delete qLayout;
    }

    flag = false;
    for(i = 0; i < itemList.size(); i++)
    {
        if (itemList.at(i)->getName() == qComboBoxItemList->currentText())
        {
            item = itemList.at(i);
            flag = true;
            break;
        }
    }

    if (flag)
    {

        switch (currentUserType)
        {
            case USERLEVEL_USER:
                userType = "USER";
                break;
            case USERLEVEL_SCIENTIST:
                userType = "SCIENTIST";
                break;
            default:
                userType = "ENGINEER";
        }

        currentFields.clear();
        for(i = 0; i < item->groupList.size(); i++)
        {
            group = item->groupList.at(i);
            for(j = 0; j < group->fieldList.size(); j++)
            {
                field = group->fieldList.at(j);
                if (field->getType().compare("spinbox", Qt::CaseInsensitive) == 0)
                {
                    qCaWidget = new _QESpinBox();
                    ((_QESpinBox *) qCaWidget)->setItemName(item->getName());
                    ((_QESpinBox *) qCaWidget)->setGroupName(group->getName());
                    ((_QESpinBox *) qCaWidget)->setFieldName(field->getName());
                    ((_QESpinBox *) qCaWidget)->setFieldJoin(field->getJoin());
                    ((_QELineEdit *) qCaWidget)->setProcessVariable(field->getProcessVariable());
                    ((_QESpinBox *) qCaWidget)->setVariableNameAndSubstitutions(field->getProcessVariable(), item->getSubstitution(), 0);
                    ((_QESpinBox *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                }
                else if (field->getType().compare("combobox", Qt::CaseInsensitive) == 0)
                {
                    qCaWidget = new _QEComboBox();
                    ((_QEComboBox *) qCaWidget)->setItemName(item->getName());
                    ((_QEComboBox *) qCaWidget)->setGroupName(group->getName());
                    ((_QEComboBox *) qCaWidget)->setFieldName(field->getName());
                    ((_QEComboBox *) qCaWidget)->setFieldJoin(field->getJoin());
                    ((_QELineEdit *) qCaWidget)->setProcessVariable(field->getProcessVariable());
                    ((_QEComboBox *) qCaWidget)->setVariableNameAndSubstitutions(field->getProcessVariable(), item->getSubstitution(), 0);
                    ((_QEComboBox *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                }
                else
                {
                    qCaWidget = new _QELineEdit();
                    ((_QELineEdit *) qCaWidget)->setItemName(item->getName());
                    ((_QELineEdit *) qCaWidget)->setGroupName(group->getName());
                    ((_QELineEdit *) qCaWidget)->setFieldName(field->getName());
                    ((_QELineEdit *) qCaWidget)->setFieldJoin(field->getJoin());
                    ((_QELineEdit *) qCaWidget)->setProcessVariable(field->getProcessVariable());
                    ((_QELineEdit *) qCaWidget)->setNotation(QCaStringFormatting::NOTATION_AUTOMATIC);
                    ((_QELineEdit *) qCaWidget)->setVariableNameAndSubstitutions(field->getProcessVariable(), item->getSubstitution(), 0);
                    ((_QELineEdit *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                }
                currentFields.append(qCaWidget);
            }
        }



        qHBoxLayoutLast = NULL;
        for(i = 0; i < currentFields.size(); i++)
        {
            if (((_QELineEdit *) currentFields.at(i))->getGroupName().isEmpty())
            {
                if (((_QELineEdit *) currentFields.at(i))->getFieldJoin())
                {
                    if (qHBoxLayoutLast == NULL)
                    {
                        qHBoxLayout = new QHBoxLayout();
                        qHBoxLayoutLast = qHBoxLayout;
                        flag = true;
                    }
                    else
                    {
                        qHBoxLayout = qHBoxLayoutLast;
                        flag = false;
                    }
                }
                else
                {
                    qHBoxLayout = new QHBoxLayout();
                    qHBoxLayoutLast = qHBoxLayout;
                    flag = true;
                }
                if (field->getName().isEmpty() == false)
                {
                    qLabel = new QLabel();
                    qLabel->setText(field->getName());
                    qLabel->setFixedWidth(125);
                    qHBoxLayout->addWidget(qLabel);
                }
                qHBoxLayout->addWidget((_QELineEdit *) currentFields.at(i));
                if (flag)
                {
                    qVBoxLayoutFields->addLayout(qHBoxLayout);
                }
            }
            else
            {
                qDebug() << "create button";
            }
        }



        /*
        currentFields.clear();
        qHBoxLayoutLast = NULL;
        iteratorGroup = item->groupList.begin();

        while(iteratorGroup != item->groupList.end())
        {
            group = *iteratorGroup;
            if (group->getName().isEmpty())
            {
                iteratorField = group->fieldList.begin();
                while(iteratorField != group->fieldList.end())
                {
                    field = *iteratorField;
                    if (field->getVisible().isEmpty() || field->getVisible().split(",").contains(userType, Qt::CaseInsensitive))
                    {
                        qLabel = new QLabel();
                        qLabel->setText(field->getName());
                        qLabel->setFixedWidth(125);

                        if (field->getJoin())
                        {
                            if (qHBoxLayoutLast == NULL)
                            {
                                qHBoxLayout = new QHBoxLayout();
                                qHBoxLayoutLast = qHBoxLayout;
                                flag = true;
                            }
                            else
                            {
                                qHBoxLayout = qHBoxLayoutLast;
                                flag = false;
                            }
                        }
                        else
                        {
                            qHBoxLayout = new QHBoxLayout();
                            qHBoxLayoutLast = qHBoxLayout;
                            flag = true;
                        }
                        qHBoxLayout->addWidget(qLabel);

                        if (field->getType().compare("spinbox", Qt::CaseInsensitive) == 0)
                        {
                            qCaWidget = new _QESpinBox();
                            ((_QESpinBox *) qCaWidget)->setItemName(item->getName());
                            ((_QESpinBox *) qCaWidget)->setFieldName(field->getName());
//                            ((_QCaSpinBox *) qCaWidget)->setNotation(QCaStringFormatting::NOTATION_AUTOMATIC);
                            ((_QESpinBox *) qCaWidget)->setVariableNameAndSubstitutions(field->getProcessVariable(), item->getSubstitution(), 0);
                            ((_QESpinBox *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                            qHBoxLayout->addWidget((QCaSpinBox *) qCaWidget);
                        }
                        else if (field->getType().compare("combobox", Qt::CaseInsensitive) == 0)
                        {
                            qCaWidget = new _QEComboBox();
                            ((_QEComboBox *) qCaWidget)->setItemName(item->getName());
                            ((_QEComboBox *) qCaWidget)->setFieldName(field->getName());
//                                ((_QCaComboBox *) qCaWidget)->setNotation(QCaStringFormatting::NOTATION_AUTOMATIC);
                            ((_QEComboBox *) qCaWidget)->setVariableNameAndSubstitutions(field->getProcessVariable(), item->getSubstitution(), 0);
                            ((_QEComboBox *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                            qHBoxLayout->addWidget((QCaComboBox *) qCaWidget);
                        }
                        else
                        {
                            qCaWidget = new _QELineEdit();
                            ((_QELineEdit *) qCaWidget)->setItemName(item->getName());
                            ((_QELineEdit *) qCaWidget)->setFieldName(field->getName());
                            ((_QELineEdit *) qCaWidget)->setProcessVariable(field->getProcessVariable());
                            ((_QELineEdit *) qCaWidget)->setNotation(QCaStringFormatting::NOTATION_AUTOMATIC);
                            ((_QELineEdit *) qCaWidget)->setVariableNameAndSubstitutions(field->getProcessVariable(), item->getSubstitution(), 0);
                            ((_QELineEdit *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                            qHBoxLayout->addWidget((_QELineEdit *) qCaWidget);
                        }

                        //currentFields.push_back(qCaWidget);
                        currentFields.append(qCaWidget);


                        if (flag)
                        {
                            qVBoxLayoutFields->addLayout(qHBoxLayout);
                        }
                    }
                    iteratorField++;
                }
            }
            else
            {
                flag = false;
                iteratorField = group->fieldList.begin();
                while(iteratorField != group->fieldList.end())
                {
                    field = *iteratorField;
                    if (field->getVisible().isEmpty() || field->getVisible().split(",").contains(userType, Qt::CaseInsensitive))
                    {
                        flag = true;
                        break;
                    }
                    iteratorField++;
                }
                if (flag)
                {
                    qHBoxLayout = new QHBoxLayout();
                    qPushButtonGroup = new _QPushButtonGroup();
                    qPushButtonGroup->setText(group->getName());
                    qPushButtonGroup->setToolTip("Show fields of group '" + group->getName() + "'");
                    qPushButtonGroup->item = item;
                    qPushButtonGroup->group = group;
                    qPushButtonGroup->currentUserType = currentUserType;
                    if (field->getJoin())
                    {
                        if (qHBoxLayoutLast == NULL)
                        {
                            qHBoxLayout = new QHBoxLayout();
                            qHBoxLayoutLast = qHBoxLayout;
                            flag = true;
                        }
                        else
                        {
                            qHBoxLayout = qHBoxLayoutLast;
                            flag = false;
                        }
                    }
                    else
                    {
                        qHBoxLayout = new QHBoxLayout();
                        qHBoxLayoutLast = qHBoxLayout;
                        flag = true;
                    }
                    qHBoxLayout->addWidget(qPushButtonGroup);
                    if (flag)
                    {
                        qVBoxLayoutFields->addLayout(qHBoxLayout);
                    }
                }
            }
            iteratorGroup++;
        }
        */






        qWidget = new QWidget();
        qWidget->setLayout(qVBoxLayoutFields);
        qScrollArea->setWidget(qWidget);

    }


}





void QEConfiguredLayout::comboBoxItemSelected(int)
{

    refreshFields();

}






// ============================================================
//  FIELD METHODS
// ============================================================
_Field::_Field()
{


}



QString _Field::getName()
{

    return name;

}


void _Field::setName(QString pValue)
{

    name = pValue;

}



QString _Field::getProcessVariable()
{

    return processVariable;

}


void _Field::setProcessVariable(QString pValue)
{

    processVariable = pValue;

}



bool _Field::getJoin()
{

    return join;

}


void _Field::setJoin(bool pValue)
{

    join = pValue;

}



QString _Field::getType()
{

    return type;

}


void _Field::setType(QString pValue)
{

    type = pValue;

}



QString _Field::getVisible()
{

    return visible;

}


void _Field::setVisible(QString pValue)
{

    visible = pValue;

}



QString _Field::getEditable()
{

    return editable;

}


void _Field::setEditable(QString pValue)
{

    editable = pValue;

}




// ============================================================
//  ITEM METHODS
// ============================================================
_Item::_Item()
{

}



void _Item::setName(QString pValue)
{

    name = pValue;

}



QString _Item::getName()
{

    return name;

}





void _Item::setSubstitution(QString pValue)
{

    substitution = pValue;

}



QString _Item::getSubstitution()
{

    return substitution;

}




void _Item::setVisible(QString pValue)
{

    visible = pValue;

}



QString _Item::getVisible()
{

    return visible;

}



void _Item::addGroup(_Group &pGroup)
{

    groupList.push_back(&pGroup);

}





// ============================================================
//  GROUP METHODS
// ============================================================
_Group::_Group()
{


}


void _Group::setName(QString pValue)
{

    name = pValue;

}


QString _Group::getName()
{

    return name;

}




void _Group::addField(_Field &pField)
{

    fieldList.push_back(&pField);

}




//list <_Field> _Group::getFieldList()
//{

//    return fieldList;

//}





// ============================================================
//  _QDIALOGITEM METHODS
// ============================================================
_QDialogItem::_QDialogItem(QWidget *pParent, int pCurrentUserType, _Item *pItem, _Group *pGroup, Qt::WindowFlags pF):QDialog(pParent, pF)
{
    /*

    list<_Field *>::iterator iterator;
    QVBoxLayout *qVBoxLayout;
    QHBoxLayout *qHBoxLayout;
    QLabel *qLabel;
    QCaWidget *qCaWidget;
    QString userType;
    _Field *field;



    qVBoxLayout = new QVBoxLayout();
    qPushButtonClose = new QPushButton();

    setWindowTitle(pItem->getName() + " (" + pGroup->getName() + ")");

    switch (pCurrentUserType)
    {
        case USERLEVEL_USER:
            userType = "USER";
            break;
        case USERLEVEL_SCIENTIST:
            userType = "SCIENTIST";
            break;
        default:
            userType = "ENGINEER";
    }


    iterator = pGroup->fieldList.begin();
//    iterator = pGroup->getFieldList().begin();
    while(iterator != pGroup->fieldList.end())
//    while(iterator != pGroup->getFieldList().end())
    {
        field = *iterator;
        qHBoxLayout = new QHBoxLayout();
        if (field->getVisible().isEmpty() || field->getVisible().split(",").contains(userType, Qt::CaseInsensitive))
        {
            qLabel = new QLabel();
            qLabel->setText(field->getName());
            qLabel->setFixedWidth(125);
            qHBoxLayout->addWidget(qLabel);

            if (field->getType().compare("spinbox", Qt::CaseInsensitive) == 0)
            {
                qCaWidget = new _QESpinBox();
                ((_QESpinBox *) qCaWidget)->setItemName(pItem->getName());
                ((_QESpinBox *) qCaWidget)->setFieldName(field->getName());
//                ((_QCaSpinBox *) qCaWidget)->setNotation(QCaStringFormatting::NOTATION_AUTOMATIC);
                ((_QESpinBox *) qCaWidget)->setVariableNameAndSubstitutions(field->getProcessVariable(), pItem->getSubstitution(), 0);
                ((_QESpinBox *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                qHBoxLayout->addWidget((QCaSpinBox *) qCaWidget);
            }
            else
            {
                if (field->getType().compare("combobox", Qt::CaseInsensitive) == 0)
                {
                    qCaWidget = new _QEComboBox();
                    ((_QEComboBox *) qCaWidget)->setItemName(pItem->getName());
                    ((_QEComboBox *) qCaWidget)->setFieldName(field->getName());
//                    ((_QCaComboBox *) qCaWidget)->setNotation(QCaStringFormatting::NOTATION_AUTOMATIC);
                    ((_QEComboBox *) qCaWidget)->setVariableNameAndSubstitutions(field->getProcessVariable(), pItem->getSubstitution(), 0);
                    ((_QEComboBox *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                    qHBoxLayout->addWidget((QCaComboBox *) qCaWidget);
                }
                else
                {
                    qCaWidget = new _QELineEdit();
                    ((_QELineEdit *) qCaWidget)->setItemName(pItem->getName());
                    ((_QELineEdit *) qCaWidget)->setFieldName(field->getName());
                    ((_QELineEdit *) qCaWidget)->setNotation(QCaStringFormatting::NOTATION_AUTOMATIC);
                    ((_QELineEdit *) qCaWidget)->setVariableNameAndSubstitutions(field->getProcessVariable(), pItem->getSubstitution(), 0);
                    ((_QELineEdit *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                    qHBoxLayout->addWidget((_QELineEdit *) qCaWidget);
                }
            }

        }
        qVBoxLayout->addLayout(qHBoxLayout);
        iterator++;
    }

    qPushButtonClose->setText("Close");
    qPushButtonClose->setToolTip("Close window");
    QObject::connect(qPushButtonClose, SIGNAL(clicked()), this, SLOT(buttonCloseClicked()));
    qVBoxLayout->addWidget(qPushButtonClose);

    setLayout(qVBoxLayout);
    */

}




void _QDialogItem::buttonCloseClicked()
{

    if (focusWidget() == qPushButtonClose)
    {
        this->close();
    }

}





// ============================================================
//  _QELINEEDIT METHODS
// ============================================================
_QELineEdit::_QELineEdit(QWidget *pParent):QCaLineEdit(pParent)
{
    QObject::connect( this,  SIGNAL( userChange( const QString&, const QString&, const QString& ) ),
                      this, SLOT( valueWritten(  const QString&, const QString&, const QString&  ) ) );
}



void _QELineEdit::setItemName(QString pItemName)
{

    itemName = pItemName;

}





QString _QELineEdit::getItemName()
{

    return itemName;

}



void _QELineEdit::setGroupName(QString pGroupName)
{

    groupName = pGroupName;

}




QString _QELineEdit::getGroupName()
{

    return groupName;

}



void _QELineEdit::setFieldName(QString pFieldName)
{

    fieldName = pFieldName;

}





QString _QELineEdit::getFieldName()
{

    return fieldName;

}




void _QELineEdit::setFieldJoin(bool pFieldJoin)
{

    fieldJoin = pFieldJoin;

}





bool _QELineEdit::getFieldJoin()
{

    return fieldJoin;

}



void _QELineEdit::setProcessVariable(QString pProcessVariable)
{

    processVariable = pProcessVariable;

}



QString _QELineEdit::getProcessVariable()
{

    return processVariable;

}




void _QELineEdit::valueWritten(const QString& pNewValue, const QString& pOldValue, const QString&)
{

    if (pOldValue != pNewValue)
    {
        sendMessage("The value of field '" + getFieldName() + "' was changed from '" + pOldValue + "' to '" + pNewValue + "'.");
    }

}






// ============================================================
//  _QECOMBOBOX METHODS
// ============================================================
_QEComboBox::_QEComboBox(QWidget *pParent):QCaComboBox(pParent)
{


}



void _QEComboBox::setItemName(QString pItemName)
{

    itemName = pItemName;

}





QString _QEComboBox::getItemName()
{

    return itemName;

}




void _QEComboBox::setGroupName(QString pGroupName)
{

    groupName = pGroupName;

}




QString _QEComboBox::getGroupName()
{

    return groupName;

}



void _QEComboBox::setFieldName(QString pFieldName)
{

    fieldName = pFieldName;

}





QString _QEComboBox::getFieldName()
{

    return fieldName;

}




void _QEComboBox::setFieldJoin(bool pFieldJoin)
{

    fieldJoin = pFieldJoin;

}





bool _QEComboBox::getFieldJoin()
{

    return fieldJoin;

}



void _QEComboBox::setProcessVariable(QString pProcessVariable)
{

    processVariable = pProcessVariable;

}



QString _QEComboBox::getProcessVariable()
{

    return processVariable;

}





//TODO: refactor this method
void _QEComboBox::valueWritten(QString pNewValue, QString pOldValue)
{

    if (pOldValue != pNewValue)
    {
        sendMessage("The value of field '" + getFieldName() + "' (of item '" + getItemName() + "') was changed from '" + pOldValue + "' to '" + pNewValue + "'.");
    }

}






// ============================================================
//  _QESPINBOX METHODS
// ============================================================
_QESpinBox::_QESpinBox(QWidget *pParent):QCaSpinBox(pParent)
{


}



void _QESpinBox::setItemName(QString pItemName)
{

    itemName = pItemName;

}





QString _QESpinBox::getItemName()
{

    return itemName;

}





void _QESpinBox::setGroupName(QString pGroupName)
{

    groupName = pGroupName;

}





QString _QESpinBox::getGroupName()
{

    return groupName;

}



void _QESpinBox::setFieldName(QString pFieldName)
{

    fieldName = pFieldName;

}





QString _QESpinBox::getFieldName()
{

    return fieldName;

}




void _QESpinBox::setFieldJoin(bool pFieldJoin)
{

    fieldJoin = pFieldJoin;

}





bool _QESpinBox::getFieldJoin()
{

    return fieldJoin;

}




void _QESpinBox::setProcessVariable(QString pProcessVariable)
{

    processVariable = pProcessVariable;

}



QString _QESpinBox::getProcessVariable()
{

    return processVariable;

}




//TODO: refactor this method
void _QESpinBox::valueWritten(QString pNewValue, QString pOldValue)
{

    if (pOldValue != pNewValue)
    {
        sendMessage("The value of field '" + getFieldName() + "' (of item '" + getItemName() + "') was changed from '" + pOldValue + "' to '" + pNewValue + "'.");
    }

}





// ============================================================
//  _QPUSHBUTTONGROUP METHODS
// ============================================================
_QPushButtonGroup::_QPushButtonGroup(QWidget *pParent):QPushButton(pParent)
{


}




void _QPushButtonGroup::mouseReleaseEvent(QMouseEvent *qMouseEvent)
{

    if (qMouseEvent->button() & Qt::LeftButton)
    {
        showDialogGroup();
    }

}




void _QPushButtonGroup::keyPressEvent(QKeyEvent *pKeyEvent)
{

    QPushButton::keyPressEvent(pKeyEvent);

//    TODO: check if the mouse click, the enter and space keys are respected

    if (pKeyEvent->key () == Qt::Key_Enter || pKeyEvent->key () == Qt::Key_Space)
    {
        showDialogGroup();
    }

}




void _QPushButtonGroup::showDialogGroup()
{

    _QDialogItem *qDialogItem;

    qDialogItem = new _QDialogItem(this, currentUserType, item, group);
    qDialogItem->exec();

}




void _QPushButtonGroup::buttonGroupClicked()
{

    showDialogGroup();

}


