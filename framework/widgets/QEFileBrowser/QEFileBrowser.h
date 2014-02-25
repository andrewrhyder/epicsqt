/*  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
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

#ifndef QEFILEBROWSER_H
#define QEFILEBROWSER_H

#include <QTableWidget>
#include <QPushButton>
#include <QEWidget.h>
#include <QELineEdit.h>


enum details
{
    TOP,
    BOTTOM,
    LEFT,
    RIGHT
};



// ============================================================
//  _QTABLEWIDGETFILEBROWSER CLASS
// ============================================================
class _QTableWidgetFileBrowser:public QTableWidget
{

    Q_OBJECT

    private:
        bool initialized;


    protected:


    public:
        _QTableWidgetFileBrowser(QWidget * pParent = 0);
        void refreshSize();
        void resizeEvent(QResizeEvent *);
        void resize(int w, int h);


    private slots:

};





// ============================================================
//  QEFILEBROWSER METHODS
// ============================================================
class QEPLUGINLIBRARYSHARED_EXPORT QEFileBrowser:public QWidget, public QEWidget
{

    Q_OBJECT

    private:


    protected:
        QELineEdit *qeLineEditDirectoryPath;
        QPushButton *qPushButtonDirectoryBrowser;
        QPushButton *qPushButtonRefresh;
        _QTableWidgetFileBrowser *qTableWidgetFileBrowser;
        QString fileFilter;
        bool showFileExtension;
        bool fileDialogDirectoriesOnly;
        int detailsLayout;


    public:

        QEFileBrowser(QWidget *pParent = 0);
        virtual ~QEFileBrowser(){}


        void setVariableName(QString pValue);
        QString getVariableName();

        void setVariableNameSubstitutions(QString pValue);
        QString getVariableNameSubstitutions();

        void setDirectoryPath(QString pValue);
        QString getDirectoryPath();

        void setShowDirectoryPath(bool pValue);
        bool getShowDirectoryPath();

        void setShowDirectoryBrowser(bool pValue);
        bool getShowDirectoryBrowser();

        void setShowRefresh(bool pValue);
        bool getShowRefresh();

        void setShowTable(bool pValue);
        bool getShowTable();

        void setShowColumnTime(bool pValue);
        bool getShowColumnTime();

        void setShowColumnSize(bool pValue);
        bool getShowColumnSize();

        void setShowColumnFilename(bool pValue);
        bool getShowColumnFilename();

        void setShowFileExtension(bool pValue);
        bool getShowFileExtension();

        void setFileFilter(QString pValue);
        QString getFileFilter();        

        void setFileDialogDirectoriesOnly(bool pValue);
        bool getFileDialogDirectoriesOnly();

        void setDetailsLayout(int pValue);
        int getDetailsLayout();

        void updateTable();


        Q_PROPERTY(QString variable READ getVariableName WRITE setVariableName)

        Q_PROPERTY(QString variableSubstitutions READ getVariableNameSubstitutions WRITE setVariableNameSubstitutions)

        Q_PROPERTY(QString directoryPath READ getDirectoryPath WRITE setDirectoryPath)

        Q_PROPERTY(bool showDirectoryPath READ getShowDirectoryPath WRITE setShowDirectoryPath)

        Q_PROPERTY(bool showDirectoryBrowser READ getShowDirectoryBrowser WRITE setShowDirectoryBrowser)

        Q_PROPERTY(bool showRefresh READ getShowRefresh WRITE setShowRefresh)

        Q_PROPERTY(bool showTable READ getShowTable WRITE setShowTable)

        Q_PROPERTY(bool showColumnTime READ getShowColumnTime WRITE setShowColumnTime)

        Q_PROPERTY(bool showColumnSize READ getShowColumnSize WRITE setShowColumnSize)

        Q_PROPERTY(bool showColumnFilename READ getShowColumnFilename WRITE setShowColumnFilename)

        Q_PROPERTY(bool showFileExtension READ getShowFileExtension WRITE setShowFileExtension)

        Q_PROPERTY(bool fileDialogDirectoriesOnly READ getFileDialogDirectoriesOnly WRITE setFileDialogDirectoriesOnly)

        Q_PROPERTY(QString fileFilter READ getFileFilter WRITE setFileFilter)

        Q_ENUMS(detailsLayoutProperty)
        Q_PROPERTY(detailsLayoutProperty detailsLayout READ getDetailsLayoutProperty WRITE setDetailsLayoutProperty)
        enum detailsLayoutProperty
        {
            Top = TOP,
            Bottom = BOTTOM,
            Left = LEFT,
            Right = RIGHT
        };        

        void setDetailsLayoutProperty(detailsLayoutProperty pDetailsLayout)
        {
            setDetailsLayout((detailsLayoutProperty) pDetailsLayout);
        }
        detailsLayoutProperty getDetailsLayoutProperty()
        {
            return (detailsLayoutProperty) getDetailsLayout();
        }


    private slots:
        void lineEditDirectoryPathChanged(QString);

        void buttonDirectoryBrowserClicked();

        void buttonRefreshClicked();

        void itemActivated(QTableWidgetItem *);


    signals:
        void selected(QString pFilename);

};





#endif // QEFILEBROWSER_H


