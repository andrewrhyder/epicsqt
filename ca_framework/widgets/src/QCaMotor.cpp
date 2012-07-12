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
#include <QCaMotor.h>
#include <QDomDocument>



// ============================================================
//  QCAMOTOR METHODS
// ============================================================
QCaMotor::QCaMotor(QWidget *pParent):QWidget(pParent), QCaWidget(this)
{

    QHBoxLayout *qHBoxLayout;
    QVBoxLayout *qVBoxLayout;

    qScrollArea = new QScrollArea();
    qVBoxLayout = new QVBoxLayout();
    qHBoxLayout = new QHBoxLayout();
    qVBoxLayoutFields = new QVBoxLayout();

    qLabelDescriptionList = new QLabel();
    qHBoxLayout->addWidget(qLabelDescriptionList);

    qComboBoxMotor = new QComboBox();
    qComboBoxMotor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    qComboBoxMotor->setToolTip("Select item to be viewed/controlled");
    QObject::connect(qComboBoxMotor, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxMotorSelected(int)));
    qHBoxLayout->addWidget(qComboBoxMotor);

    qScrollArea->setWidgetResizable(true);
    qScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    qScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    qVBoxLayout->addLayout(qHBoxLayout);
    qVBoxLayout->addWidget(qScrollArea);

    setLayout(qVBoxLayout);
    setMotorConfigurationFile("");
    setMotorConfigurationText("");
    setDescriptionList("");
    setMotorConfigurationType(FROM_FILE);
    setShowMotorList(true);
    setCurrentUserType(getUserLevel());

}






void QCaMotor::setMotorConfigurationType(int pValue)
{

    motorConfigurationType = pValue;
    setMotorConfigurationFile(motorConfigurationFile);
    setMotorConfigurationText(motorConfigurationText);

}





int QCaMotor::getMotorConfigurationType()
{

    return motorConfigurationType;

}





void QCaMotor::setShowMotorList(bool pValue)
{

    qComboBoxMotor->setVisible(pValue);
    qLabelDescriptionList->setVisible(qLabelDescriptionList->text().isEmpty() == false);

}





bool QCaMotor::getShowMotorList()
{

    return qComboBoxMotor->isVisible();

}






void QCaMotor::setDescriptionList(QString pValue)
{

    qLabelDescriptionList->setText(pValue);
    qLabelDescriptionList->setVisible(qLabelDescriptionList->text().isEmpty() == false);

}





QString QCaMotor::getDescriptionList()
{

    return qLabelDescriptionList->text();

}




void QCaMotor::setMotorConfigurationFile(QString pValue)
{

    QFile *file;
    QString data;

    motorConfigurationFile = pValue;
    if (motorConfigurationType == FROM_FILE)
    {
        file = new QFile(motorConfigurationFile);
        if (file->open(QFile::ReadOnly | QFile::Text))
        {
            data = file->readAll();
            file->close();
            setMotorConfiguration(data);
        }
    }

}




QString QCaMotor::getMotorConfigurationFile()
{

    return motorConfigurationFile;

}





void QCaMotor::setMotorConfigurationText(QString pValue)
{

    motorConfigurationText = pValue;
    if (motorConfigurationType == FROM_TEXT)
    {
        setMotorConfiguration(motorConfigurationText);
    }

}





QString QCaMotor::getMotorConfigurationText()
{

    return motorConfigurationText;

}




void QCaMotor::setMotorConfiguration(QString pValue)
{

    list<_Group *>::iterator iterator;
    QDomDocument document;
    QDomElement rootElement;
    QDomElement motorElement;
    QDomElement fieldElement;
    QDomNode rootNode;
    QDomNode motorNode;
    _Motor *motor;
    _Group *group;
    _Field *field;
    QString tmp;
    bool  flag0;
    bool  flag1;
    int count;


    motorList.clear();

    if (document.setContent(pValue))
    {
        rootElement = document.documentElement();
        if (rootElement.tagName() == "epicsqt")
        {
            count = 0;
            rootNode = rootElement.firstChild();
            while (rootNode.isNull() == false)
            {
                motorElement = rootNode.toElement();
                if (motorElement.tagName() == "motor")
                {
                    flag1 = true;
                    motor = new _Motor();
                    if (motorElement.attribute("name").isEmpty())
                    {
                        motor->setName("Motor #" + QString::number(count));
                        count++;
                    }
                    else
                    {
                        motor->setName(motorElement.attribute("name"));
                    }

                    motor->setSubstitution(motorElement.attribute("substitution"));
                    motor->setVisible(motorElement.attribute("visible"));
                    motorNode = motorElement.firstChild();
                    while (motorNode.isNull() == false)
                    {
                        fieldElement = motorNode.toElement();
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
                                iterator = motor->groupList.begin();
                                while(iterator != motor->groupList.end())
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
                                motor->addGroup(*group);
                            }
                        }
                        motorNode = motorNode.nextSibling();
                    }
                    motorList.push_back(motor);
                }
                rootNode = rootNode.nextSibling();
            }
        }
    }
    setCurrentUserType(currentUserType);


}





void QCaMotor::userLevelChanged(userLevels pValue)
{

    setCurrentUserType(pValue);

}




void QCaMotor::setCurrentUserType(int pValue)
{

    list<_Motor *>::iterator iterator;
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
        qComboBoxMotor->blockSignals(true);
        tmp = qComboBoxMotor->currentText();
        qComboBoxMotor->clear();
        iterator = motorList.begin();
        while(iterator != motorList.end())
        {
            if ((*iterator)->getVisible().isEmpty() || (*iterator)->getVisible().split(",").contains(userType, Qt::CaseInsensitive))
            {
                qComboBoxMotor->addItem((*iterator)->getName());
            }
            iterator++;
        }
        i = qComboBoxMotor->findText(tmp);
        if (i == -1)
        {
            qComboBoxMotor->setCurrentIndex(0);
        }
        else
        {
           qComboBoxMotor->setCurrentIndex(i);
        }
        refreshFields();
        qComboBoxMotor->blockSignals(false);
    }


}



int QCaMotor::getCurrentUserType()
{

    return currentUserType;

}



void QCaMotor::refreshFields()
{

    list<_Motor *>::iterator iteratorMotor;
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
    _Motor *motor;
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
    iteratorMotor = motorList.begin();
    while(iteratorMotor != motorList.end())
    {
        if ((*iteratorMotor)->getName() == qComboBoxMotor->currentText())
        {
            motor = *iteratorMotor;
            flag = true;
            break;
        }
        iteratorMotor++;
    }

    if (flag)
    {

        qHBoxLayoutLast = NULL;
        iteratorGroup = motor->groupList.begin();

        while(iteratorGroup != motor->groupList.end())
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
                            ((_QCaSpinBox *) qCaWidget)->setMotorName(motor->getName());
                            ((_QCaSpinBox *) qCaWidget)->setFieldName(field->getName());
//                            ((_QCaSpinBox *) qCaWidget)->setNotation(QCaStringFormatting::NOTATION_AUTOMATIC);
                            ((_QCaSpinBox *) qCaWidget)->setVariableNameAndSubstitutions(field->getProcessVariable(), motor->getSubstitution(), 0);
                            ((_QCaSpinBox *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                            qHBoxLayout->addWidget((QCaSpinBox *) qCaWidget);
                        }
                        else
                        {
                            if (field->getType().compare("combobox", Qt::CaseInsensitive) == 0)
                            {
                                qCaWidget = new _QCaComboBox();
                                ((_QCaComboBox *) qCaWidget)->setMotorName(motor->getName());
                                ((_QCaComboBox *) qCaWidget)->setFieldName(field->getName());
//                                ((_QCaComboBox *) qCaWidget)->setNotation(QCaStringFormatting::NOTATION_AUTOMATIC);
                                ((_QCaComboBox *) qCaWidget)->setVariableNameAndSubstitutions(field->getProcessVariable(), motor->getSubstitution(), 0);
                                ((_QCaComboBox *) qCaWidget)->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                                qHBoxLayout->addWidget((QCaComboBox *) qCaWidget);
                            }
                            else
                            {
                                qCaWidget = new _QCaLineEdit();
                                ((_QCaLineEdit *) qCaWidget)->setMotorName(motor->getName());
                                ((_QCaLineEdit *) qCaWidget)->setFieldName(field->getName());
                                ((_QCaLineEdit *) qCaWidget)->setNotation(QCaStringFormatting::NOTATION_AUTOMATIC);
                                ((_QCaLineEdit *) qCaWidget)->setVariableNameAndSubstitutions(field->getProcessVariable(), motor->getSubstitution(), 0);
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
                    qPushButtonGroup->motor = motor;
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





void QCaMotor::comboBoxMotorSelected(int)
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
//  MOTOR METHODS
// ============================================================
_Motor::_Motor()
{

}



void _Motor::setName(QString pValue)
{

    name = pValue;

}



QString _Motor::getName()
{

    return name;

}




void _Motor::setSubstitution(QString pValue)
{

    substitution = pValue;

}



QString _Motor::getSubstitution()
{

    return substitution;

}




void _Motor::setVisible(QString pValue)
{

    visible = pValue;

}



QString _Motor::getVisible()
{

    return visible;

}



void _Motor::addGroup(_Group &pGroup)
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
//  _QDIALOGMOTOR METHODS
// ============================================================
_QDialogMotor::_QDialogMotor(QWidget *pParent, int pCurrentUserType, _Motor *pMotor, _Group *pGroup, Qt::WindowFlags pF):QDialog(pParent, pF)
{

    list<_Field *>::iterator iterator;
    QVBoxLayout *qVBoxLayout;
    QHBoxLayout *qHBoxLayout;
    QLabel *qLabel;
    _QCaLineEdit *qCaLineEdit;
    QString userType;
    _Field *field;



    qVBoxLayout = new QVBoxLayout();
    qPushButtonClose = new QPushButton();

    setWindowTitle(pMotor->getName() + " (" + pGroup->getName() + ")");

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
            qCaLineEdit = new _QCaLineEdit();
            qCaLineEdit->setMotorName(pMotor->getName());
            qCaLineEdit->setFieldName(field->getName());
            qCaLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            qCaLineEdit->setVariableNameAndSubstitutions(field->getProcessVariable(), pMotor->getSubstitution(), 0);
            qCaLineEdit->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
            qHBoxLayout->addWidget(qCaLineEdit);
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




void _QDialogMotor::buttonCloseClicked()
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


}



void _QCaLineEdit::setMotorName(QString pMotorName)
{

    motorName = pMotorName;

}





QString _QCaLineEdit::getMotorName()
{

    return motorName;

}



void _QCaLineEdit::setFieldName(QString pFieldName)
{

    fieldName = pFieldName;

}





QString _QCaLineEdit::getFieldName()
{

    return fieldName;

}




void _QCaLineEdit::valueWritten(QString pNewValue, QString pOldValue)
{

    if (pOldValue != pNewValue)
    {
        sendMessage("The value of field '" + getFieldName() + "' (of item '" + getMotorName() + "') was changed from '" + pOldValue + "' to '" + pNewValue + "'.");
    }

}






// ============================================================
//  _QCACOMBOBOX METHODS
// ============================================================
_QCaComboBox::_QCaComboBox(QWidget *pParent):QCaComboBox(pParent)
{


}



void _QCaComboBox::setMotorName(QString pMotorName)
{

    motorName = pMotorName;

}





QString _QCaComboBox::getMotorName()
{

    return motorName;

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
        sendMessage("The value of field '" + getFieldName() + "' (of item '" + getMotorName() + "') was changed from '" + pOldValue + "' to '" + pNewValue + "'.");
    }

}






// ============================================================
//  _QCASPINBOX METHODS
// ============================================================
_QCaSpinBox::_QCaSpinBox(QWidget *pParent):QCaSpinBox(pParent)
{


}



void _QCaSpinBox::setMotorName(QString pMotorName)
{

    motorName = pMotorName;

}





QString _QCaSpinBox::getMotorName()
{

    return motorName;

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
        sendMessage("The value of field '" + getFieldName() + "' (of item '" + getMotorName() + "') was changed from '" + pOldValue + "' to '" + pNewValue + "'.");
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

    _QDialogMotor *qDialogMotor;

    qDialogMotor = new _QDialogMotor(this, currentUserType, motor, group);
    qDialogMotor->exec();

}




void _QPushButtonGroup::buttonGroupClicked()
{

    showDialogGroup();

}


