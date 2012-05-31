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
    qHBoxLayout->addWidget(qLabel);

    qComboBoxMotor = new QComboBox();
    qHBoxLayout->addWidget(qComboBoxMotor);

    QObject::connect(qComboBoxMotor, SIGNAL(currentIndexChanged(QString)), this, SLOT(comboBoxMotorSelected(QString)));

    qVBoxLayout->addLayout(qHBoxLayout);


    motorCount = 0;

    setLayout(qVBoxLayout);

//    layout()->addItem(qVBoxLayout);
//    layout()->addItem(qVBoxLayoutFields);

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

    QString motorSelected;
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
        motorSelected = qComboBoxMotor->currentText();
        qComboBoxMotor->clear();
        for(i = 0; i < motorCount; i++)
        {
            if (motorList[i].getVisible().isEmpty() || motorList[i].getVisible().indexOf(userType, 0, Qt::CaseInsensitive) != -1)
            {
                qComboBoxMotor->addItem(motorList[i].getName());
            }
        }
        i = qComboBoxMotor->findText(motorSelected);
        if (i != -1)
        {
           qComboBoxMotor->setCurrentIndex(i);
        }
//        setDetailsLayout(detailsLayout);
        refreshFields();
    }

}



int QCaMotor::getCurrentUserType()
{

    return currentUserType;

}




void QCaMotor::refreshFields()
{

    QVBoxLayout *qVBoxLayout;
    QHBoxLayout *qHBoxLayout;
    QPushButton *qPushButton;
    QString userType;
    QString tmp;
    QCaLabel *qCaLabel;
    _Motor *motorSelected;
    int i;
    int j;


//    detailsLayout = pValue;


//    QLayoutItem* item;
//    while ((item = qVBoxLayoutFields->takeAt(0)) != NULL)
//    {
//        qVBoxLayoutFields->removeItem(item);
//        qVBoxLayoutFields->removeWidget(item->widget());
//        delete item->widget();
//        delete item;
//    }


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


    motorSelected = NULL;
    for(i = 0; i < motorCount; i++)
    {
        if (motorList[i].getName() == qComboBoxMotor->currentText())
        {
            motorSelected = &motorList[i];
            break;
        }
    }


    if (motorSelected)
    {
        for(i = 0; i < motorSelected->groupCount; i++)
        {
            tmp = motorSelected->groupList[i].getName();

            if (tmp.isEmpty())
            {
                for(j = 0; j < motorSelected->groupList[i].fieldCount; j++)
                {
                    qHBoxLayout = new QHBoxLayout();
                    if (motorSelected->groupList[i].fieldList[j].getVisible().isEmpty() || motorSelected->groupList[i].fieldList[j].getVisible().indexOf(userType, 0, Qt::CaseInsensitive) != -1)
                    {
                        qHBoxLayout->addWidget(new QLabel(motorSelected->groupList[i].fieldList[j].getName()));
                        qCaLabel = new QCaLabel(motorSelected->groupList[i].fieldList[j].getProcessVariable());
                        qCaLabel->setEnabled(motorSelected->groupList[i].fieldList[j].getEditable().isEmpty() || motorSelected->groupList[i].fieldList[j].getEditable().indexOf(userType, 0, Qt::CaseInsensitive) != -1);
                        qHBoxLayout->addWidget(qCaLabel);
                    }
                    ((QVBoxLayout *) layout())->addLayout(qHBoxLayout);
                }
            }
            else
            {
                qPushButton = new QPushButton();
                qPushButton->setText(tmp);
                ((QVBoxLayout *) layout())->addWidget(qPushButton);
            }
        }
    }


}



//int QCaMotor::getDetailsLayout()
//{

//    return detailsLayout;

//}





void QCaMotor::buttonLoginClicked()
{

//    qCaMotorDialog = new _QDialogMotor(this);
//    qCaMotorDialog->exec();

}




void QCaMotor::comboBoxMotorSelected(QString pValue)
{

//    setDetailsLayout(detailsLayout);
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


