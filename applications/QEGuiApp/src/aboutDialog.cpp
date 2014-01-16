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
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

/* Description:
 *
 * Presents a dialog containing information about the QEGui application such as version numbers and credits
 */

#include "aboutDialog.h"
#include "ui_aboutDialog.h"
#include <QString>
#include <QLabel>
#include <QLibraryInfo>

aboutDialog::aboutDialog( QString QEGuiVersion,                // Version info and the build date/time at compile time of QEGui
                          QString QEFrameworkVersionQEGui,     // Version info and the build date/time at compile time of the copy of QEPlugin library loaded by QEGui
                          QString QEFrameworkVersionUILoader,  // Version info and the build date/time at compile time of the copy of QEPlugin library loaded by QUiLoader while creating QE widgets

                          QString macroSubstitutions,          // Macro substitutions (-m parameter)
                          QStringList pathList,                // Path list (-p parameter)
                          QStringList envPathList,             // Path list (environment variable)
                          QString userLevel,                   // Current user level

                          QStringList windowTitles,            // Window titles (must be same length as windowFiles)
                          QStringList windowFiles,             // Window file name (must be same length as windowTitles)

                          QString configurationFile,              // Configuration file
                          QString configurationName,              // Configuration name

                          QString defaultWindowCustomisationFile, // Default Window customisation file
                          QString defaultWindowCustomisationName, // Default Window customisation name
                          QString currentCustomisation,           // Current customisation set name

                          QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aboutDialog)
{
    ui->setupUi(this);

    // Version
    ui->QEGuiVersionLabel->setText( QEGuiVersion );

    ui->QtInstalledPluginsLabel->setText( QLibraryInfo::location ( QLibraryInfo::PluginsPath ) );
    ui->QEFrameworkVersionQEGuiLabel->setText( QEFrameworkVersionQEGui );
    ui->QEFrameworkVersionUILoaderLabel->setText( QEFrameworkVersionUILoader );

    // Environment
    ui->userLevelLabel->setText( userLevel );
    ui->macroSubstitutionsLabel->setText( macroSubstitutions );

    for( int i = 0; i < pathList.count(); i++ )
    {
        ui->pathParameterList->addItem( pathList[i] );
    }

    for( int i = 0; i < envPathList.count(); i++ )
    {
        ui->pathVariableList->addItem( envPathList[i] );
    }

    // Windows
    int rowCount = std::min( windowTitles.count(), windowFiles.count() );
    ui->windowsTable->setRowCount( rowCount );

    for( int i = 0; i < rowCount; i++ )
    {
        Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled;

        QTableWidgetItem* windowTitle = new QTableWidgetItem( windowTitles[i] );
        windowTitle->setFlags( flags );

        QTableWidgetItem* windowFile = new QTableWidgetItem( windowFiles[i] );
        windowFile->setFlags( flags );

        ui->windowsTable->setItem(i, 0, windowTitle );
        ui->windowsTable->setItem(i, 1, windowFile );
    }
    ui->windowsTable->resizeColumnsToContents();
    ui->windowsTable->setHorizontalHeaderLabels( QStringList() << "Title" << "File" );

    // Conficuration
    ui->configurationFileLabel->setText( configurationFile );
    ui->configurationNameLabel->setText( configurationName );

    // Customisation
    ui->defaultWindowCustomisationFileLabel->setText( defaultWindowCustomisationFile );
    ui->defaultWindowCustomisationNameLabel->setText( defaultWindowCustomisationName );
    ui->currentCustomisationLabel->setText( currentCustomisation );

}

aboutDialog::~aboutDialog()
{
    delete ui;
}
