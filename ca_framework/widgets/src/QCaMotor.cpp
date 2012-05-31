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
    qLabel->setFixedWidth(100);
    qHBoxLayout->addWidget(qLabel);

    qComboBoxMotor = new QComboBox();
    qComboBoxMotor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    qComboBoxMotor->setToolTip("Select current motor");
    QObject::connect(qComboBoxMotor, SIGNAL(currentIndexChanged(QString)), this, SLOT(comboBoxMotorSelected(QString)));
    qHBoxLayout->addWidget(qComboBoxMotor);

    qVBoxLayout->addLayout(qHBoxLayout);
    qVBoxLayout->addItem(new QSpacerItem(0, 20, QSizePolicy::Expanding, QSizePolicy::Fixed));
    qVBoxLayout->addLayout(qVBoxLayoutFields);
    qVBoxLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));

    motorCount = 0;

    setLayout(qVBoxLayout);
    setCurrentUserType(getUserLevel());

}



void QCaMotor::setMotorConfiguration(QString pValue)
{

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
    int i;


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
        motorCount = 0;
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
                            for (i = 0; i < motor->groupCount; i++)
                            {
                                if (motor->groupList[i].getName() == tmp)
                                {
                                    motor->groupList[i].addField(*field);
                                    flag = false;
                                    break;
                                }
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
                    motorList[motorCount] = *motor;
                    motorCount++;
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

    QString motor;
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
        motor = qComboBoxMotor->currentText();
        qComboBoxMotor->clear();
        for(i = 0; i < motorCount; i++)
        {
            if (motorList[i].getVisible().isEmpty() || motorList[i].getVisible().indexOf(userType, 0, Qt::CaseInsensitive) != -1)
            {
                qComboBoxMotor->addItem(motorList[i].getName());
            }
        }
        i = qComboBoxMotor->findText(motor);
        if (i != -1)
        {
           qComboBoxMotor->setCurrentIndex(i);
        }
        refreshFields();
    }

}



int QCaMotor::getCurrentUserType()
{

    return currentUserType;

}




void QCaMotor::refreshFields()
{

    QLayout *qLayout;
    QHBoxLayout *qHBoxLayout;
    QWidget *qWidget;
    QLabel *qLabel;
    QString userType;
    QString tmp;
    QCaLabel *qCaLabel;
    QPushButton *qPushButtonEdit;
    _QPushButtonGroup *qPushButtonGroup;
    _Motor *motor;
    _Group *group;
    bool flag;
    int i;
    int j;


    if (qVBoxLayoutFields)
    {
        while(qVBoxLayoutFields->isEmpty() == false)
        {
            qLayout = qVBoxLayoutFields->takeAt(0)->layout();
            if (qLayout)
            {
                while(qLayout->isEmpty() == false)
                {
                    qWidget = qLayout->takeAt(0)->widget();
                    delete qWidget;
                }
                delete qLayout;
            }
        }
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

    motor = NULL;
    for(i = 0; i < motorCount; i++)
    {
        if (motorList[i].getName() == qComboBoxMotor->currentText())
        {
            motor = &motorList[i];
            break;
        }
    }

    if (motor)
    {
        for(i = 0; i < motor->groupCount; i++)
        {
            group = &motor->groupList[i];
            tmp = group->getName();
            if (tmp.isEmpty())
            {
                for(j = 0; j < group->fieldCount; j++)
                {
                    qHBoxLayout = new QHBoxLayout();
                    if (group->fieldList[j].getVisible().isEmpty() || group->fieldList[j].getVisible().indexOf(userType, 0, Qt::CaseInsensitive) != -1)
                    {
                        qLabel = new QLabel();
                        qLabel->setText(group->fieldList[j].getName());
                        qLabel->setFixedWidth(100);
                        qHBoxLayout->addWidget(qLabel);
                        qCaLabel = new QCaLabel();
                        qCaLabel->setFrameStyle(1);
                        qCaLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
                        qCaLabel->setVariableNameAndSubstitutions(group->fieldList[j].getProcessVariable(), motor->getSubstitution(), 0);
                        qHBoxLayout->addWidget(qCaLabel);
                        if (group->fieldList[j].getEditable().isEmpty() || group->fieldList[j].getEditable().indexOf(userType, 0, Qt::CaseInsensitive) != -1)
                        {
                            qPushButtonEdit = new QPushButton();
                            qPushButtonEdit->setFixedSize(30, 22);
                            qPushButtonEdit->setText("...");
                            qPushButtonEdit->setToolTip("Edit value of field '" + group->fieldList[j].getName() + "'");
                            qHBoxLayout->addWidget(qPushButtonEdit);
                        }
                    }
                    qVBoxLayoutFields->addLayout(qHBoxLayout);
                }
            }
            else
            {
                flag = false;
                for(j = 0; j < group->fieldCount; j++)
                {
                    if (group->fieldList[j].getVisible().isEmpty() || group->fieldList[j].getVisible().indexOf(userType, 0, Qt::CaseInsensitive) != -1)
                    {
                        flag = true;
                        break;
                    }
                }
                if (flag)
                {
                    qHBoxLayout = new QHBoxLayout();
                    qPushButtonGroup = new _QPushButtonGroup();
                    qPushButtonGroup->setText(tmp);
                    qPushButtonGroup->setToolTip("Show fields of group '" + tmp + "'");
                    qPushButtonGroup->motor = motor;
                    qPushButtonGroup->group = group;
                    qPushButtonGroup->currentUserType = currentUserType;
                    qHBoxLayout->addWidget(qPushButtonGroup);
                    qVBoxLayoutFields->addLayout(qHBoxLayout);
                }
            }
        }
    }


}





void QCaMotor::comboBoxMotorSelected(QString pValue)
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

    groupCount = 0;

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



void _Motor::addGroup(_Group pValue)
{

    groupList[groupCount] = pValue;
    groupCount++;

}





// ============================================================
//  GROUP METHODS
// ============================================================
_Group::_Group()
{

    fieldCount = 0;

}


void _Group::setName(QString pValue)
{

    name = pValue;

}


QString _Group::getName()
{

    return name;

}



void _Group::addField(_Field pValue)
{

    fieldList[fieldCount] = pValue;
    fieldCount++;

}





// ============================================================
//  _QDIALOGMOTOR METHODS
// ============================================================
_QDialogMotor::_QDialogMotor(QWidget *pParent, int pCurrentUserType, _Motor *pMotor, _Group *pGroup, Qt::WindowFlags pF):QDialog(pParent, pF)
{

    QVBoxLayout *qVBoxLayout;
    QHBoxLayout *qHBoxLayout;
    QPushButton *qPushButtonEdit;
    QPushButton *qPushButtonClose;
    QLabel *qLabel;
    QCaLabel *qCaLabel;
    QString userType;
    int i;


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

    for(i = 0; i < pGroup->fieldCount; i++)
    {
        qHBoxLayout = new QHBoxLayout();
        if (pGroup->fieldList[i].getVisible().isEmpty() || pGroup->fieldList[i].getVisible().indexOf(userType, 0, Qt::CaseInsensitive) != -1)
        {
            qLabel = new QLabel();
            qLabel->setText(pGroup->fieldList[i].getName());
            qLabel->setFixedWidth(100);
            qHBoxLayout->addWidget(qLabel);
            qCaLabel = new QCaLabel();
            qCaLabel->setFrameStyle(1);
            qCaLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            qCaLabel->setVariableNameAndSubstitutions(pGroup->fieldList[i].getProcessVariable(), pMotor->getSubstitution(), 0);
            qHBoxLayout->addWidget(qCaLabel);
            if (pGroup->fieldList[i].getEditable().isEmpty() || pGroup->fieldList[i].getEditable().indexOf(userType, 0, Qt::CaseInsensitive) != -1)
            {
                qPushButtonEdit = new QPushButton();
                qPushButtonEdit->setFixedSize(30, 22);
                qPushButtonEdit->setText("...");
                qPushButtonEdit->setToolTip("Edit value of field '" + pGroup->fieldList[i].getName() + "'");
                qHBoxLayout->addWidget(qPushButtonEdit);
            }
        }
        qVBoxLayout->addLayout(qHBoxLayout);
    }

    qPushButtonClose->setText("Close");
    qPushButtonClose->setToolTip("Close fields window");
    QObject::connect(qPushButtonClose, SIGNAL(clicked()), this, SLOT(buttonCloseClicked()));
    qVBoxLayout->addWidget(qPushButtonClose);

    setLayout(qVBoxLayout);

}




void _QDialogMotor::buttonCloseClicked()
{

    this->close();

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


