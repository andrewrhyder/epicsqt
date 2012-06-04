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
#include <QCaMotor.h>
#include <ContainerProfile.h>
#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QCaLabel.h>
#include <QCaLineEdit.h>




// ============================================================
//  QCAMOTOR METHODS
// ============================================================
QCaMotor::QCaMotor(QWidget *pParent):QWidget(pParent), QCaWidget(this)
{

    QHBoxLayout *qHBoxLayout;
    QVBoxLayout *qVBoxLayout;
    QLabel *qLabel;

    qVBoxLayout = new QVBoxLayout();
    qHBoxLayout = new QHBoxLayout();
    qVBoxLayoutFields = new QVBoxLayout();

    qLabel = new QLabel();
    qLabel->setText("Motor");
    qLabel->setFixedWidth(110);
    qHBoxLayout->addWidget(qLabel);

    qComboBoxMotor = new QComboBox();
    qComboBoxMotor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    qComboBoxMotor->setToolTip("Select current motor");
    QObject::connect(qComboBoxMotor, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxMotorSelected(int)));
    qHBoxLayout->addWidget(qComboBoxMotor);

    qVBoxLayout->addLayout(qHBoxLayout);
    qVBoxLayout->addItem(new QSpacerItem(0, 20, QSizePolicy::Expanding, QSizePolicy::Fixed));
    qVBoxLayout->addLayout(qVBoxLayoutFields);
    qVBoxLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    setLayout(qVBoxLayout);
    setCurrentUserType(getUserLevel());

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
    QFile *file;
    _Motor *motor;
    _Group *group;
    _Field *field;
    QString tmp;
    bool  flag;
    int count;


    motorConfiguration = pValue;

    document = QDomDocument("epicsqt");
    file = new QFile(motorConfiguration);

    if (file->open(QFile::ReadOnly | QFile::Text))
    {
        flag = document.setContent(file);
        file->close();
    }
    else
    {
        flag = document.setContent(motorConfiguration);
    }

    if (flag)
    {
        motorList.clear();
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
                            field->setDescription(fieldElement.attribute("description"));
                            field->setMask(fieldElement.attribute("mask"));
                            field->setVisible(fieldElement.attribute("visible"));
                            field->setEditable(fieldElement.attribute("editable"));
                            tmp = fieldElement.attribute("group");
                            flag = true;
                            iterator = motor->groupList.begin();
                            while(iterator != motor->groupList.end())
                            {
                                if ((*iterator)->getName() == tmp)
                                {
                                    (*iterator)->addField(*field);
                                    flag = false;
                                    break;
                                }
                                iterator++;
                            }
                            if (flag)
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
        setCurrentUserType(currentUserType);
    }

}




QString QCaMotor::getMotorConfiguration()
{

    return motorConfiguration;

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
    QWidget *qWidget;
    QLabel *qLabel;
    QString userType;
    QCaLineEdit *qCaLineEdit;
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

        iteratorGroup = motor->groupList.begin();
        while(iteratorGroup != motor->groupList.end())
        {
            group = *iteratorGroup;
            if (group->getName().isEmpty())
            {
                iteratorField = group->fieldList.begin();
                while(iteratorField != group->fieldList.end())
                {
                    qHBoxLayout = new QHBoxLayout();
                    field = *iteratorField;
                    if (field->getVisible().isEmpty() || field->getVisible().split(",").contains(userType, Qt::CaseInsensitive))
                    {
                        qLabel = new QLabel();
                        qLabel->setText(field->getName());
                        qLabel->setFixedWidth(110);
                        qHBoxLayout->addWidget(qLabel);
                        qCaLineEdit = new QCaLineEdit();
                        qCaLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
                        qCaLineEdit->setVariableNameAndSubstitutions(field->getProcessVariable(), motor->getSubstitution(), 0);
                        qCaLineEdit->setEnabled(field->getEditable().isEmpty() || field->getEditable().split(",").contains(userType, Qt::CaseInsensitive));
                        qHBoxLayout->addWidget(qCaLineEdit);
                    }
                    qVBoxLayoutFields->addLayout(qHBoxLayout);
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
                    qHBoxLayout->addWidget(qPushButtonGroup);
                    qVBoxLayoutFields->addLayout(qHBoxLayout);
                }
            }
            iteratorGroup++;
        }
    }


}





void QCaMotor::comboBoxMotorSelected(int pValue)
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



QString _Field::getDescription()
{

    return description;

}


void _Field::setDescription(QString pValue)
{

    description = pValue;

}




QString _Field::getMask()
{

    return mask;

}


void _Field::setMask(QString pValue)
{

    mask = pValue;

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





// ============================================================
//  _QDIALOGMOTOR METHODS
// ============================================================
_QDialogMotor::_QDialogMotor(QWidget *pParent, int pCurrentUserType, _Motor *pMotor, _Group *pGroup, Qt::WindowFlags pF):QDialog(pParent, pF)
{

    list<_Field *>::iterator iterator;
    QVBoxLayout *qVBoxLayout;
    QHBoxLayout *qHBoxLayout;
    QLabel *qLabel;
    QCaLineEdit *qCaLineEdit;
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
    while(iterator != pGroup->fieldList.end())
    {
        field = *iterator;
        qHBoxLayout = new QHBoxLayout();
        if (field->getVisible().isEmpty() || field->getVisible().split(",").contains(userType, Qt::CaseInsensitive))
        {
            qLabel = new QLabel();
            qLabel->setText(field->getName());
            qLabel->setFixedWidth(110);
            qHBoxLayout->addWidget(qLabel);
            qCaLineEdit = new QCaLineEdit();
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

