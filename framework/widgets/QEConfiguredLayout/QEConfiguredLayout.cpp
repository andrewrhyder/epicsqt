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

#include <QCaSpinBox.h>
#include <QCaComboBox.h>
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

    list<_Group *>::iterator iterator;
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
    bool  flag0;
    bool  flag1;
    int count;


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
                            field->setMask(fieldElement.attribute("mask"));
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
                                iterator = item->groupList.begin();
                                while(iterator != item->groupList.end())
                                {
                                    if ((*iterator)->getName() == tmp)
                                    {
                                        (*iterator)->addField(*field);
                                        flag0 = false;
                                        break;
                                    }
                                    iterator++;
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

    list<_Item *>::iterator iterator;
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
        iterator = itemList.begin();
        while(iterator != itemList.end())
        {
            if ((*iterator)->getVisible().isEmpty() || (*iterator)->getVisible().split(",").contains(userType, Qt::CaseInsensitive))
            {
                qComboBoxItemList->addItem((*iterator)->getName());
            }
            iterator++;
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

    list<_Item *>::iterator iteratorItem;
    list<_Group *>::iterator iteratorGroup;
    list<_Field *>::iterator iteratorField;
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

    flag = false;
    iteratorItem = itemList.begin();
    while(iteratorItem != itemList.end())
    {
        if ((*iteratorItem)->getName() == qComboBoxItemList->currentText())
        {
            item = *iteratorItem;
            flag = true;
            break;
        }
        iteratorItem++;
    }

    if (flag)
    {

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
                            qCaWidget = new _QCaSpinBox();
                            ((_QCaSpinBox *) qCaWidget)->setItemName(item->getName());
                            ((_QCaSpinBox *) qCaWidget)->setFieldName(field->getName());
//                            ((_QCaSpinBox *) qCaWidget)->setNotation(QCaStringFormatting::NOTATION_AUTOMATIC);
                            ((_QCaSpinBox *) qCaWidget)->setVariableNameAndSubstitutions(field->getProcessVariable(), item->getSubstitution(), 0);
                            ((_QCaSpinBox *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                            qHBoxLayout->addWidget((QCaSpinBox *) qCaWidget);
                        }
                        else
                        {
                            if (field->getType().compare("combobox", Qt::CaseInsensitive) == 0)
                            {
                                qCaWidget = new _QCaComboBox();
                                ((_QCaComboBox *) qCaWidget)->setItemName(item->getName());
                                ((_QCaComboBox *) qCaWidget)->setFieldName(field->getName());
//                                ((_QCaComboBox *) qCaWidget)->setNotation(QCaStringFormatting::NOTATION_AUTOMATIC);
                                ((_QCaComboBox *) qCaWidget)->setVariableNameAndSubstitutions(field->getProcessVariable(), item->getSubstitution(), 0);
                                ((_QCaComboBox *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                                qHBoxLayout->addWidget((QCaComboBox *) qCaWidget);
                            }
                            else
                            {
                                qCaWidget = new _QCaLineEdit();
                                ((_QCaLineEdit *) qCaWidget)->setItemName(item->getName());
                                ((_QCaLineEdit *) qCaWidget)->setFieldName(field->getName());
                                ((_QCaLineEdit *) qCaWidget)->setNotation(QCaStringFormatting::NOTATION_AUTOMATIC);
                                ((_QCaLineEdit *) qCaWidget)->setVariableNameAndSubstitutions(field->getProcessVariable(), item->getSubstitution(), 0);
                                ((_QCaLineEdit *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                                qHBoxLayout->addWidget((_QCaLineEdit *) qCaWidget);
                            }
                        }
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




QString _Field::getMask()
{

    return mask;

}


void _Field::setMask(QString pValue)
{

    mask = pValue;

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
                qCaWidget = new _QCaSpinBox();
                ((_QCaSpinBox *) qCaWidget)->setItemName(pItem->getName());
                ((_QCaSpinBox *) qCaWidget)->setFieldName(field->getName());
//                ((_QCaSpinBox *) qCaWidget)->setNotation(QCaStringFormatting::NOTATION_AUTOMATIC);
                ((_QCaSpinBox *) qCaWidget)->setVariableNameAndSubstitutions(field->getProcessVariable(), pItem->getSubstitution(), 0);
                ((_QCaSpinBox *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                qHBoxLayout->addWidget((QCaSpinBox *) qCaWidget);
            }
            else
            {
                if (field->getType().compare("combobox", Qt::CaseInsensitive) == 0)
                {
                    qCaWidget = new _QCaComboBox();
                    ((_QCaComboBox *) qCaWidget)->setItemName(pItem->getName());
                    ((_QCaComboBox *) qCaWidget)->setFieldName(field->getName());
//                    ((_QCaComboBox *) qCaWidget)->setNotation(QCaStringFormatting::NOTATION_AUTOMATIC);
                    ((_QCaComboBox *) qCaWidget)->setVariableNameAndSubstitutions(field->getProcessVariable(), pItem->getSubstitution(), 0);
                    ((_QCaComboBox *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                    qHBoxLayout->addWidget((QCaComboBox *) qCaWidget);
                }
                else
                {
                    qCaWidget = new _QCaLineEdit();
                    ((_QCaLineEdit *) qCaWidget)->setItemName(pItem->getName());
                    ((_QCaLineEdit *) qCaWidget)->setFieldName(field->getName());
                    ((_QCaLineEdit *) qCaWidget)->setNotation(QCaStringFormatting::NOTATION_AUTOMATIC);
                    ((_QCaLineEdit *) qCaWidget)->setVariableNameAndSubstitutions(field->getProcessVariable(), pItem->getSubstitution(), 0);
                    ((_QCaLineEdit *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                    qHBoxLayout->addWidget((_QCaLineEdit *) qCaWidget);
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

}




void _QDialogItem::buttonCloseClicked()
{

    if (focusWidget() == qPushButtonClose)
    {
        this->close();
    }

}





// ============================================================
//  _QCALINEEDIT METHODS
// ============================================================
_QCaLineEdit::_QCaLineEdit(QWidget *pParent):QCaLineEdit(pParent)
{
    QObject::connect( this,  SIGNAL( userChange( const QString&, const QString&, const QString& ) ),
                      this, SLOT( valueWritten(  const QString&, const QString&, const QString&  ) ) );
}



void _QCaLineEdit::setItemName(QString pItemName)
{

    itemName = pItemName;

}





QString _QCaLineEdit::getItemName()
{

    return itemName;

}



void _QCaLineEdit::setFieldName(QString pFieldName)
{

    fieldName = pFieldName;

}





QString _QCaLineEdit::getFieldName()
{

    return fieldName;

}




void _QCaLineEdit::valueWritten(const QString& pNewValue, const QString& pOldValue, const QString&)
{

    if (pOldValue != pNewValue)
    {
        sendMessage("The value of field '" + getFieldName() + "' (of item '" + getItemName() + "') was changed from '" + pOldValue + "' to '" + pNewValue + "'.");
    }

}






// ============================================================
//  _QCACOMBOBOX METHODS
// ============================================================
_QCaComboBox::_QCaComboBox(QWidget *pParent):QCaComboBox(pParent)
{


}



void _QCaComboBox::setItemName(QString pItemName)
{

    itemName = pItemName;

}





QString _QCaComboBox::getItemName()
{

    return itemName;

}



void _QCaComboBox::setFieldName(QString pFieldName)
{

    fieldName = pFieldName;

}





QString _QCaComboBox::getFieldName()
{

    return fieldName;

}




void _QCaComboBox::valueWritten(QString pNewValue, QString pOldValue)
{

    if (pOldValue != pNewValue)
    {
        sendMessage("The value of field '" + getFieldName() + "' (of item '" + getItemName() + "') was changed from '" + pOldValue + "' to '" + pNewValue + "'.");
    }

}






// ============================================================
//  _QCASPINBOX METHODS
// ============================================================
_QCaSpinBox::_QCaSpinBox(QWidget *pParent):QCaSpinBox(pParent)
{


}



void _QCaSpinBox::setItemName(QString pItemName)
{

    itemName = pItemName;

}





QString _QCaSpinBox::getItemName()
{

    return itemName;

}



void _QCaSpinBox::setFieldName(QString pFieldName)
{

    fieldName = pFieldName;

}





QString _QCaSpinBox::getFieldName()
{

    return fieldName;

}




void _QCaSpinBox::valueWritten(QString pNewValue, QString pOldValue)
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


