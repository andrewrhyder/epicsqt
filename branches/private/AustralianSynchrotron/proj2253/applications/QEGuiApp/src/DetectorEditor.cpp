/*
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
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
 *  Copyright (c) 2013
 *
 *  Author:
 */

/*
 * This class is used to edit detectors
 */

#include <DetectorEditor.h>
#include <ui_DetectorEditor.h>
#include <QDebug>
#include <QPushButton>
#include <QFile>
#include <QMessageBox>
#include <QEWidget.h>

DetectorEditor::DetectorEditor( QList<QStringList> detectorList, QString xmlFile, QWidget *parent ) :
    QDialog(parent),
    ui(new Ui::DetectorEditor)
{
    ui->setupUi(this);
    xmlFileName = xmlFile;
    detectors = detectorList;
    for ( int i = 0; i < detectorList.count(); i ++ ){
        detectorNames.append(detectorList.at(i).at(0));
    }
    ui->detectorNameComboBox->addItems( detectorNames );
    enableSave();
}

DetectorEditor::~DetectorEditor()
{
    delete ui;
}

void DetectorEditor::on_newRadioButton_clicked( bool )
{
    ui->detectorName->setText("");
    ui->detectorPVPrefix->setText("");
    ui->detectorNameComboBox->setEnabled(false);
    ui->detectorName->setEnabled(true);
    ui->detectorTypeComboBox->setEnabled(true);
    ui->detectorPVPrefix->setEnabled(true);
    enableSave();
}

void DetectorEditor::on_deleteRadioButton_clicked( bool )
{
    ui->detectorNameComboBox->setEnabled(true);
    ui->detectorName->setEnabled(false);
    ui->detectorTypeComboBox->setEnabled(false);
    ui->detectorPVPrefix->setEnabled(false);
    on_detectorNameComboBox_activated( ui->detectorNameComboBox->currentIndex() );
    enableSave();
}

void DetectorEditor::on_editRadioButton_clicked( bool )
{
    ui->detectorNameComboBox->setEnabled(true);
    ui->detectorName->setEnabled(false);
    ui->detectorTypeComboBox->setEnabled(true);
    ui->detectorPVPrefix->setEnabled(true);
    on_detectorNameComboBox_activated( ui->detectorNameComboBox->currentIndex() );
    enableSave();
}


void DetectorEditor::on_detectorNameComboBox_activated( int )
{
    ui->detectorNameComboBox->currentText();
    QStringList detectorInfo;
    for ( int i = 0; i < detectors.count(); i ++ ){
        if (detectors.at(i).at(0) == ui->detectorNameComboBox->currentText()){
            detectorInfo = detectors.at(i);
            ui->detectorName->setText(detectors.at(i).at(0));
            QString PVPrefix = detectors.at(i).at(1);
            PVPrefix.chop(1);
            PVPrefix.remove(0,2);
            ui->detectorPVPrefix->setText(PVPrefix);
            QString type = detectors.at(i).at(2);
            type.chop(5);
            ui->detectorTypeComboBox->setCurrentIndex(ui->detectorTypeComboBox->findText(type));
            break;
        }
    }
    enableSave();
}

void DetectorEditor::on_detectorTypeComboBox_activated( int )
{
}

void DetectorEditor::on_detectorName_textChanged(const QString & ){
    enableSave();
}

void DetectorEditor::on_detectorPVPrefix_textChanged(const QString & ){
    enableSave();
}

bool DetectorEditor::isAddingDetector()
{
    return ui->newRadioButton->isChecked();
}

bool DetectorEditor::isDeletingDetector()
{
    return ui->deleteRadioButton->isChecked();
}

void DetectorEditor::enableSave()
{
    QPushButton* saveButton = ui->buttonBox->button(QDialogButtonBox::Save);
    if( saveButton )
    {
        saveButton->setEnabled( !ui->detectorName->text().isEmpty() && !ui->detectorPVPrefix->text().isEmpty() );
    }
}

bool DetectorEditor::save()
{
    QDomDocument doc;
    // Open to write a new/updated detector configurations to its xmlFile
    ContainerProfile containerProfile;
    QFile* file = QEWidget::findQEFile(xmlFileName, &containerProfile);
    if (!file && !file->open(QIODevice::ReadWrite))
    {
        QString error = file->errorString();
        qDebug() << "Could not open customisation file" << xmlFileName << error;
        return false;
    }
    // if named customisation exists, replace it
    if ( !doc.setContent( file ) )
    {
        qDebug() << "Could not parse the XML in the customisations file" << xmlFileName;
        file->close();
        delete file;
        return false;
    }
    file->close();

    QDomElement docElem = doc.documentElement();
    QDomElement customisationElement = docElem.firstChildElement( "Customisation" );
   if ( !customisationElement.isNull() )
    {
        QString customisationName = customisationElement.attribute( "Name" );
        if( !customisationName.isEmpty() )
        {
            // get a first node
            QDomNode node = customisationElement.firstChild();
            while (!node.isNull())
            {
                QDomElement element = node.toElement();
                if( element.tagName() == "Menu" ){
                    QString menuName = element.attribute( "Name" );
                    if (menuName == "Detector"){
                        QDomNode selectNode = element.firstChild();
                        if (selectNode.toElement().attribute( "Name" ) == "Select"){
                            QString detectorName = ui->detectorName->text();
                            QString detectorType = ui->detectorTypeComboBox->currentText();
                            QString detectorPV = ui->detectorPVPrefix->text();
                            // create a new item element per existing info from the editor
                            QDomElement detectorItem = createDetectorElement(doc, detectorName, detectorType, detectorPV);
                            if (isAddingDetector()){
                                // check if it is existing
                                QDomNode itemNode = selectNode.toElement().firstChild();
                                while (!itemNode.isNull()){
                                    QDomElement itemElement = itemNode.toElement();
                                    if( itemElement.tagName() == "Item" && itemElement.attribute( "Name" ) == detectorName ){
                                        // can't add as there is one there
                                        QMessageBox msgBox(QMessageBox::NoIcon,
                                                           "Detector Editor Information",
                                                           "Can't add it as the detector name has been used already. \nPlease use a different name.");
                                        msgBox.exec();
                                        return false;
                                    }
                                    itemNode = itemNode.nextSibling();
                                }
                                // add new detector element
                                selectNode.appendChild(detectorItem);
                            }
                            else{
                                QDomNode itemNode = selectNode.toElement().firstChild();
                                while (!itemNode.isNull()){
                                    QDomElement itemElement = itemNode.toElement();
                                    if( itemElement.tagName() == "Item" && itemElement.attribute( "Name" ) == detectorName ){
                                        if (isDeletingDetector()){
                                            // delete a detector element
                                            selectNode.removeChild(itemElement);
                                        }
                                        else{
                                            // update it
                                            selectNode.replaceChild(detectorItem, itemElement);
                                        }
                                        break;
                                    }
                                    itemNode = itemNode.nextSibling();
                                }
                            }
                            break;
                        }
                    }
                }
                node = node.nextSibling();
            }
        }
    }

   if (!file->open(QIODevice::WriteOnly))
   {
       QString error = file->errorString();
       qDebug() << "Could not open customisation file" << xmlFileName << error;
       file->close();
       delete file;
       return false;
   }
    // save it
    QTextStream out(file);
    out << doc.toString();
    file->close();
    delete file;

    QMessageBox msgBox(QMessageBox::NoIcon,
                       "Detector Editor Information",
                       "The changes have been saved. \nPlease restart application to get the detectors updated.");
    msgBox.exec();
    return true;
}

QDomElement DetectorEditor::createDetectorElement( QDomDocument doc, QString name, QString type, QString pv )
{
    // create a new item element
    QDomElement detectorItem = doc.createElement( "Item" );
    detectorItem.setAttribute("Name", name);
    // create window child and add it to the item
    QDomElement windowItem = doc.createElement( "Window" );
    detectorItem.appendChild(windowItem);
    // create children
    QDomElement UiFileItem = doc.createElement( "UiFile" );
    QDomText text = doc.createTextNode("areaDetector/ADApp/op/ui/Empty.ui");
    UiFileItem.appendChild(text);

    QDomElement CustomisationNameItem = doc.createElement( "CustomisationName" );
    text = doc.createTextNode(type + " Type");
    CustomisationNameItem.appendChild(text);

    QDomElement MacroSubstitutionsItem = doc.createElement( "MacroSubstitutions" );
    text = doc.createTextNode("P=" + pv + ":");
    MacroSubstitutionsItem.appendChild(text);

    QDomElement TitleItem = doc.createElement( "Title" );
    text = doc.createTextNode(ui->detectorName->text() + " Detector Control System");
    TitleItem.appendChild(text);

    QDomElement CreationOptionItem = doc.createElement( "CreationOption" );
    text = doc.createTextNode("Open");
    CreationOptionItem.appendChild(text);
    // add children to window
    windowItem.appendChild(UiFileItem);
    windowItem.appendChild(CustomisationNameItem);
    windowItem.appendChild(MacroSubstitutionsItem);
    windowItem.appendChild(TitleItem);
    windowItem.appendChild(CreationOptionItem);

    return detectorItem;
}

bool DetectorEditor::loadDetectorData( QString xmlFile, QList<QStringList>& list )
{
    // If no file specified, silently return (no error)
    if( xmlFile.isEmpty() )
    {
        return false;
    }

    QDomDocument doc;

    // Read and parse xmlFile
    ContainerProfile containerProfile;
    QFile* file = QEWidget::findQEFile(xmlFile, &containerProfile);
    if (!file && !file->open(QIODevice::ReadOnly))
    {
        QString error = file->errorString();
        qDebug() << "Could not open customisation file" << xmlFile << error;
        delete file;
        return false;
    }
    // if named customisation exists, replace it
    if ( !doc.setContent( file ) )
    {
        qDebug() << "Could not parse the XML in the customisations file" << xmlFile;
        file->close();
        delete file;
        return false;
    }
    file->close();
    delete file;
    file = NULL;

    QDomElement docElem = doc.documentElement();
    // Parse XML using Qt's Document Object Model.
    QDomElement customisationElement = docElem.firstChildElement( "Customisation" );
   if ( !customisationElement.isNull() )
    {
        QString customisationName = customisationElement.attribute( "Name" );
        if( !customisationName.isEmpty() )
        {
            // get a first node
            QDomNode node = customisationElement.firstChild();
            while (!node.isNull())
            {
                QDomElement element = node.toElement();
                if( element.tagName() == "Menu" ){
                    QString menuName = element.attribute( "Name" );
                    if (menuName == "Detector"){
                        QDomNode selectNode = element.firstChild();
                        if (selectNode.toElement().attribute( "Name" ) == "Select"){

                            QDomNode itemNode = selectNode.toElement().firstChild();
                            while (!itemNode.isNull()){
                                QDomElement itemElement = itemNode.toElement();
                                if( itemElement.tagName() == "Item" ){
                                    // get detector data
                                    QStringList detectorInfo;
                                    QString detectorName = itemElement.attribute( "Name" );
                                    detectorInfo.append(detectorName);
                                    QString detectorPVPrefix, detectorType;
                                     // Read windows to create
                                    QDomElement windowElement = itemElement.firstChildElement( "Window" );
                                    while ( !windowElement.isNull() )
                                    {
                                        // Read optional macro substitutions
                                        QDomElement macroSubstitutionsElement = windowElement.firstChildElement( "MacroSubstitutions" );
                                        if( !macroSubstitutionsElement.isNull() )
                                        {
                                            detectorPVPrefix = macroSubstitutionsElement.text();
                                            detectorInfo.append(detectorPVPrefix);
                                        }

                                        // Read optional customisation name
                                        QDomElement customisationNameElement = windowElement.firstChildElement( "CustomisationName" );
                                        if( !customisationNameElement.isNull() )
                                        {
                                            detectorType = customisationNameElement.text();
                                            detectorInfo.append(detectorType);
                                        }
                                        windowElement = windowElement.nextSiblingElement( "Window" );
                                    }
                                    list.append(detectorInfo);
                                }
                                itemNode = itemNode.nextSibling();
                            }
                        }
                    }
                }
                node = node.nextSibling();
            }
        }
    }
    return true;
}

