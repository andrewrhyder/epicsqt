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



QCaMotor::QCaMotor(QWidget *pParent):QWidget(pParent), QCaWidget(this)
{

    setMotorConfiguration("");
    setCurrentUserType(USERLEVEL_USER);
    setDetailsLayout(RIGHT);

}



void QCaMotor::setMotorConfiguration(QString pValue)
{

    QDomDocument document;
    QDomElement rootElement;
    QDomElement motorElement;
    QDomElement fieldElement;
    QDomNode rootNode;
    QDomNode motorNode;
    QDomNode fieldNode;
    QFile *file;
    _Motor motor;
    _Group group;
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
        rootElement = document.documentElement();
        if (rootElement.tagName().toLower() == "epicsqt")
        {
            count = 0;
            rootNode = rootElement.firstChild();
            while (rootNode.isNull() == false)
            {
                motorElement = rootNode.toElement();
                if (motorElement.tagName().toLower() == "motor")
                {
                    motor = _Motor();
                    if (motorElement.attribute("name").isEmpty())
                    {
                        motor.setName("Motor #" + count);
                        count++;
                    }
                    else
                    {
                        motor.setName(motorElement.attribute("name"));
                    }

                    motorNode = motorElement.firstChild();
                    while (motorNode.isNull() == false)
                    {
                        fieldElement = motorNode.toElement();

                        if (fieldElement.tagName().toLower() == "field")
                        {
                            //motor.
                        }
                        motorNode = motorNode.nextSibling();
                    }

                    motorList.push_back(motor);
                }
                rootNode = rootNode.nextSibling();
            }
        }

        //refreshMotor();
    }

}




QString QCaMotor::getMotorConfiguration()
{

    return motorConfiguration;

}



void QCaMotor::setCurrentUserType(int pValue)
{

    switch(pValue)
    {
        case USERLEVEL_USER:
            currentUserType = USERLEVEL_USER;
            refreshDetailsLayout();
            break;

        case USERLEVEL_SCIENTIST:
            currentUserType = USERLEVEL_SCIENTIST;
            refreshDetailsLayout();
            break;

        case USERLEVEL_ENGINEER:
            currentUserType = USERLEVEL_ENGINEER;
            refreshDetailsLayout();
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
    QWidget *qWidgetMotor;
    int i;


    detailsLayout = pValue;


    qVBoxLayout = new QVBoxLayout(this);

    if (motorList.size() > 1)
    {
        qWidgetMotor = new QComboBox();
        // fill up with data
    }
    else
    {
        qWidgetMotor = new QLineEdit();
        // fill up with data
    }
    qWidgetMotor->setEnabled(false);


    qHBoxLayout = new QHBoxLayout();
    qHBoxLayout->addWidget(new QLabel("Motor"));
    qHBoxLayout->addWidget(qWidgetMotor);
    qVBoxLayout->addLayout(qHBoxLayout);


    for(i = 0; i < motorList.size(); i++)
    {
//        qHBoxLayout = new QHBoxLayout();
//        qHBoxLayout->addWidget(qLabelMotor);
//        qHBoxLayout->addWidget(qWidgetMotor);
//        ((QVBoxLayout *) qLayout)->addLayout(qHBoxLayout);
    }


}



int QCaMotor::getDetailsLayout()
{

    return detailsLayout;

}




void QCaMotor::refreshDetailsLayout()
{

    setDetailsLayout(detailsLayout);

}




void QCaMotor::buttonLoginClicked()
{

//    qCaMotorDialog = new _QDialogMotor(this);
//    qCaMotorDialog->exec();

}






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




void _Motor::addGroup(_Group pValue)
{

//    groupList.assign(insert( = new List <>;

}
