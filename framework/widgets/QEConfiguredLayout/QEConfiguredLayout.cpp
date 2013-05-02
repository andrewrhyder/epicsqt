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



// ============================================================
//  QECONFIGUREDLAYOUT METHODS
// ============================================================
QEConfiguredLayout::QEConfiguredLayout(QWidget *pParent, bool pSubscription):QWidget(pParent), QEWidget(this)
{

    subscription = pSubscription;

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
    _Field *field;
    int count;


    currentFieldList.clear();
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
                            if (fieldElement.attribute("type").compare("bitstatus", Qt::CaseInsensitive) == 0)
                            {
                                field->setType(BITSTATUS);
                            }
                            else if (fieldElement.attribute("type").compare("button", Qt::CaseInsensitive) == 0)
                            {
                                field->setType(BUTTON);
                            }
                            else if (fieldElement.attribute("type").compare("label", Qt::CaseInsensitive) == 0)
                            {
                                field->setType(LABEL);
                            }
                            else if (fieldElement.attribute("type").compare("spinbox", Qt::CaseInsensitive) == 0)
                            {
                                field->setType(SPINBOX);
                            }
                            else if (fieldElement.attribute("type").compare("combobox", Qt::CaseInsensitive) == 0)
                            {
                                field->setType(COMBOBOX);
                            }
                            else
                            {
                                field->setType(LINEEDIT);
                            }
                            field->setGroup(fieldElement.attribute("group"));
                            field->setVisible(fieldElement.attribute("visible"));
                            field->setEditable(fieldElement.attribute("editable"));
                            item->fieldList.append(field);
                        }
                        itemNode = itemNode.nextSibling();
                    }
                    itemList.append(item);
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
            qLayoutMain->setAlignment(qLayoutChild,Qt::AlignHCenter);
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

    if (subscription)
    {
        setCurrentUserType(pValue);
    }

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

    QLayout *qLayout;
    QHBoxLayout *qHBoxLayout;
    QHBoxLayout *qHBoxLayoutLast;
    QWidget *qWidget;
    QLabel *qLabel;
    QString userType;
    QEWidget *qCaWidget = NULL;
    _QPushButtonGroup *qPushButtonGroup;
    _Item *item;
    _Field *field = NULL;
    _Field *fieldInfo;
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

    item = NULL;
    for(i = 0; i < itemList.size(); i++)
    {
        if (itemList.at(i)->getName() == qComboBoxItemList->currentText())
        {
            item = itemList.at(i);
            break;
        }
    }

    if (item != NULL)
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

        currentFieldList.clear();

        for(i = 0; i < item->fieldList.size(); i++)
        {
            field = item->fieldList.at(i);
            if (field->getVisible().isEmpty() || field->getVisible().split(",").contains(userType, Qt::CaseInsensitive))
            {
                fieldInfo = new _Field();
                if (field->getType() == BITSTATUS)
                {
//                    qCaWidget = new QEBitStatus();
//                    ((QESpinBox *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
//                    QObject::connect(((QESpinBox *) qCaWidget), SIGNAL(userChange(const QString &, const QString &, const QString &)), this, SLOT(valueWritten(const QString &, const QString &, const QString &)));
                }
                else if (field->getType() == BUTTON)
                {
                    qCaWidget = new QEPushButton();
                    ((QEPushButton *) qCaWidget)->setSubscribe(subscription);
                    ((QEPushButton *) qCaWidget)->setText(field->getName());
                    ((QEPushButton *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                }
                else if (field->getType() == LABEL)
                {
                    qCaWidget = new QELabel();
//                    ((QELabel *) qCaWidget)->setSubscribe(false);
                    ((QELabel *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                }
                else if (field->getType() == SPINBOX)
                {
                    qCaWidget = new QESpinBox();
                    ((QESpinBox *) qCaWidget)->setSubscribe(subscription);
                    ((QESpinBox *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                    QObject::connect(((QESpinBox *) qCaWidget), SIGNAL(userChange(const QString &, const QString &, const QString &)), this, SLOT(valueWritten(const QString &, const QString &, const QString &)));
                }
                else if (field->getType() == COMBOBOX)
                {
                    qCaWidget = new QEComboBox();
                    ((QEComboBox *) qCaWidget)->setSubscribe(subscription);
                    ((QEComboBox *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                    ((QEComboBox *) qCaWidget)->setWriteOnChange(false);
                    QObject::connect(((QEComboBox *) qCaWidget), SIGNAL(userChange(const QString &, const QString &, const QString &)), this, SLOT(valueWritten(const QString &, const QString &, const QString &)));
                }
                else
                {
                    qCaWidget = new QELineEdit();
                    ((QELineEdit *) qCaWidget)->setText(field->getName());
                    ((QELineEdit *) qCaWidget)->setSubscribe(subscription);
                    ((QELineEdit *) qCaWidget)->setNotation(QEStringFormatting::NOTATION_AUTOMATIC);
                    ((QELineEdit *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                    ((QELineEdit *) qCaWidget)->setWriteOnFinish(false);
                    ((QELineEdit *) qCaWidget)->setConfirmWrite(false);
                    QObject::connect(((QELineEdit *) qCaWidget), SIGNAL(userChange(const QString &, const QString &, const QString &)), this, SLOT(valueWritten(const QString &, const QString &, const QString &)));
                }
                fieldInfo->qCaWidget = qCaWidget;
                fieldInfo->setGroup(field->getGroup());
                fieldInfo->setName(field->getName());
                fieldInfo->setProcessVariable(field->getProcessVariable());
                fieldInfo->setJoin(field->getJoin());
                fieldInfo->setType(field->getType());
                fieldInfo->setVisibility(field->getVisible().isEmpty() || field->getVisible().split(",").contains(userType, Qt::CaseInsensitive));
                currentFieldList.append(fieldInfo);
            }
        }


        qHBoxLayoutLast = NULL;
        for(i = 0; i < currentFieldList.size(); i++)
        {
            fieldInfo = currentFieldList.at(i);
            if (fieldInfo->getGroup().isEmpty())
            {
//                if (fieldInfo->getVisibility())
//                {
                    if (fieldInfo->getJoin())
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

                    if (fieldInfo->getType() != BUTTON && fieldInfo->getName().isEmpty() == false)
                    {
                        qLabel = new QLabel();
                        qLabel->setText(fieldInfo->getName());
                        qLabel->setFixedWidth(130);
                        qHBoxLayout->addWidget(qLabel);
                    }

                    if (field->getType() == BITSTATUS)
                    {
                        qHBoxLayout->addWidget((QEBitStatus *) fieldInfo->qCaWidget);
                    }
                    else if (field->getType() == BUTTON)
                    {
                        qHBoxLayout->addWidget((QEPushButton *) fieldInfo->qCaWidget);
                    }
                    else if (field->getType() == LABEL)
                    {
                        qHBoxLayout->addWidget((QELabel *) fieldInfo->qCaWidget);
                    }
                    else if (field->getType() == SPINBOX)
                    {
                        qHBoxLayout->addWidget((QESpinBox *) fieldInfo->qCaWidget);
                    }
                    else if (field->getType() == COMBOBOX)
                    {
                        qHBoxLayout->addWidget((QEComboBox *) fieldInfo->qCaWidget);
                    }
                    else
                    {
                        qHBoxLayout->addWidget((QELineEdit *) fieldInfo->qCaWidget);
                    }

                    if (flag)
                    {
                        qVBoxLayoutFields->addLayout(qHBoxLayout);
                    }
//                }
            }
            else
            {
                flag = true;
                for(j = 0; j < i; j++)
                {
                    if (fieldInfo->getGroup() == currentFieldList.at(j)->getGroup())
                    {
                        flag = false;
                        break;
                    }
                }
                if (flag)
                {
                    flag = false;
                    for(j = 0; j < currentFieldList.size(); j++)
                    {
                        if (fieldInfo->getGroup() == currentFieldList.at(j)->getGroup())
                        {
                            if (currentFieldList.at(j)->getVisibility())
                            {
                                flag = true;
                                break;
                            }
                        }
                    }

                    if (flag)
                    {
                        qHBoxLayout = new QHBoxLayout();
                        qPushButtonGroup = new _QPushButtonGroup(this, item->getName(), fieldInfo->getGroup(), &currentFieldList);
                        qPushButtonGroup->setText(fieldInfo->getGroup());
                        qPushButtonGroup->setToolTip("Show fields of group '" + fieldInfo->getGroup() + "'");

                        if (fieldInfo->getJoin())
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

            }
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




void QEConfiguredLayout::valueWritten(const QString &pNewValue, const QString &pOldValue, const QString &)
{

    if (pOldValue != pNewValue)
    {
//        if (pFieldName.isEmpty())
//        {
            sendMessage("The field was changed from '" + pOldValue + "' to '" + pNewValue + "'.");
//        }
//        else
//        {
//            sendMessage("The field '" + pFieldName + "' was changed from '" + pOldValue + "' to '" + pNewValue + "'.");
//        }
    }

}





// ============================================================
//  FIELD METHODS
// ============================================================
_Field::_Field()
{


}



//QEWidget _Field::getWidget()
//{

//    return qCaWidget;

//}



//void _Field::setWidget(QEWidget &pValue)
//{

//    qCaWidget = pValue;

//}



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



int _Field::getType()
{

    return type;

}



void _Field::setType(int pValue)
{

    type = pValue;

}



void _Field::setGroup(QString pValue)
{

    group = pValue;

}



QString _Field::getGroup()
{

    return group;

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




void _Field::setVisibility(bool pValue)
{

    visibility = pValue;

}



bool _Field::getVisibility()
{

    return visibility;

}




//void _Field::setValue(QString pValue)
//{

//    value = pValue;

//}



//QString _Field::getValue()
//{

//    return value;

//}






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






// ============================================================
//  _QPUSHBUTTONGROUP METHODS
// ============================================================
_QPushButtonGroup::_QPushButtonGroup(QWidget *pParent, QString pItemName, QString pGroupName, QList <_Field *> *pCurrentFieldList):QPushButton(pParent)
{

    itemName = pItemName;
    groupName = pGroupName;
    currentFieldList = pCurrentFieldList;

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



void _QPushButtonGroup::buttonGroupClicked()
{

    showDialogGroup();

}




void _QPushButtonGroup::showDialogGroup()
{

    _QDialogItem *qDialogItem;

    qDialogItem = new _QDialogItem(this, itemName, groupName, currentFieldList);
    qDialogItem->exec();

}





_QDialogItem::_QDialogItem(QWidget *pParent, QString pItemName, QString pGroupName, QList <_Field *> *pCurrentFieldList, Qt::WindowFlags pF):QDialog(pParent, pF)
{

    QVBoxLayout *qVBoxLayout;
    QHBoxLayout *qHBoxLayout;
    QHBoxLayout *qHBoxLayoutLast;
    _Field *fieldInfo;
    QLabel *qLabel;
    bool flag;
    int i;


    qVBoxLayout = new QVBoxLayout();
    qPushButtonClose = new QPushButton();

    setWindowTitle(pItemName + " (" + pGroupName + ")");


    qHBoxLayoutLast = NULL;
    for(i = 0; i < pCurrentFieldList->size(); i++)
    {

        fieldInfo = pCurrentFieldList->at(i);

        if (fieldInfo->getGroup().compare(pGroupName) == 0)
        {
            if (fieldInfo->getVisibility())
            {
                if (fieldInfo->getJoin())
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

                if (fieldInfo->getType() != BUTTON && fieldInfo->getName().isEmpty() == false)
                {
                    qLabel = new QLabel();
                    qLabel->setText(fieldInfo->getName());
                    qLabel->setFixedWidth(130);
                    qHBoxLayout->addWidget(qLabel);
                }

                if (fieldInfo->getType() == BITSTATUS)
                {
                    qHBoxLayout->addWidget((QEBitStatus *) pCurrentFieldList->at(i)->qCaWidget);
                }
                else if (fieldInfo->getType() == BUTTON)
                {
                    qHBoxLayout->addWidget((QEPushButton *) pCurrentFieldList->at(i)->qCaWidget);
                }
                else if (fieldInfo->getType() == LABEL)
                {
                    qHBoxLayout->addWidget((QELabel *) pCurrentFieldList->at(i)->qCaWidget);
                }
                else if (fieldInfo->getType() == SPINBOX)
                {
                    qHBoxLayout->addWidget((QESpinBox *) pCurrentFieldList->at(i)->qCaWidget);
                }
                else if (fieldInfo->getType() == COMBOBOX)
                {
                    qHBoxLayout->addWidget((QEComboBox *) pCurrentFieldList->at(i)->qCaWidget);
                }
                else
                {
                    qHBoxLayout->addWidget((QELineEdit *) pCurrentFieldList->at(i)->qCaWidget);
                }

                if (flag)
                {
                    qVBoxLayout->addLayout(qHBoxLayout);
                }
            }
        }
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



