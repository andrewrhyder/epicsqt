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

    motorCount = 0;

//    setMotorConfiguration("");
//    setCurrentUserType(USERLEVEL_USER);
//    setDetailsLayout(RIGHT);

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
        setDetailsLayout(detailsLayout);
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

    switch(pValue)
    {
        case USERLEVEL_USER:
            currentUserType = USERLEVEL_USER;
            setDetailsLayout(detailsLayout);
            break;

        case USERLEVEL_SCIENTIST:
            currentUserType = USERLEVEL_SCIENTIST;
            setDetailsLayout(detailsLayout);
            break;

        case USERLEVEL_ENGINEER:
            currentUserType = USERLEVEL_ENGINEER;
            setDetailsLayout(detailsLayout);
    }

}



int QCaMotor::getCurrentUserType()
{

    return currentUserType;

}




void QCaMotor::setDetailsLayout(int pValue)
{

    QVBoxLayout *qVBoxLayout;
    QHBoxLayout *qHBoxLayout;
    QComboBox *qComboBox;
    QPushButton *qPushButton;
    QString userType;
    QString tmp;
    QCaLabel *qCaLabel;
    int i;
    int j;


    detailsLayout = pValue;



    if (layout() != NULL)
    {
        QLayoutItem* item;
        while ( ( item = layout()->takeAt( 0 ) ) != NULL )
        {
            layout()->removeItem(item);
            layout()->removeWidget(item->widget());
            delete item->widget();
            delete item;
        }
        delete layout();
    }


    qVBoxLayout = new QVBoxLayout(this);


    if (currentUserType == USERLEVEL_USER)
    {
        userType = "USER";
    }
    else
    {
        if (currentUserType == USERLEVEL_SCIENTIST)
        {
            userType = "SCIENTIST";
        }
        else
        {
            userType = "ENGINEER";
        }
    }

    qComboBox = new QComboBox();
    for(i = 0; i < motorCount; i++)
    {
        if (motorList[i].getVisible().isEmpty() || motorList[i].getVisible().indexOf(userType, 0, Qt::CaseInsensitive) != -1)
        {
            qComboBox->addItem(motorList[i].getName());
        }
    }


    qHBoxLayout = new QHBoxLayout();
    qHBoxLayout->addWidget(new QLabel("Motor"));
    qHBoxLayout->addWidget(qComboBox);
    qVBoxLayout->addLayout(qHBoxLayout);

    for(i = 0; i < motorList[0].groupCount; i++)
    {
        tmp = motorList[0].groupList[i].getName();
        if (tmp.isEmpty())
        {
            qPushButton = new QPushButton(this);
            qPushButton->setText(tmp);
            qVBoxLayout->addWidget(qPushButton);
        }
        else
        {
            for(j = 0; j < motorList[0].groupList[i].fieldCount; j++)
            {
                qHBoxLayout = new QHBoxLayout();
                if (motorList[0].groupList[i].fieldList[j].getVisible().isEmpty() || motorList[0].groupList[i].fieldList[j].getVisible().indexOf(userType, 0, Qt::CaseInsensitive) != -1)
                {
                    qHBoxLayout->addWidget(new QLabel(motorList[0].groupList[i].fieldList[j].getName()));
                    qCaLabel = new QCaLabel(motorList[0].groupList[i].fieldList[j].getProcessVariable());
                    qCaLabel->setEnabled(motorList[0].groupList[i].fieldList[j].getEditable().isEmpty() || motorList[0].groupList[i].fieldList[j].getEditable().indexOf(userType, 0, Qt::CaseInsensitive) != -1);
                    qHBoxLayout->addWidget(qCaLabel);
                }
                qVBoxLayout->addLayout(qHBoxLayout);
            }
        }
    }


}



int QCaMotor::getDetailsLayout()
{

    return detailsLayout;

}





void QCaMotor::buttonLoginClicked()
{

//    qCaMotorDialog = new _QDialogMotor(this);
//    qCaMotorDialog->exec();

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


