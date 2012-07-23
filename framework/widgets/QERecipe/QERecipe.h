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

#ifndef QERECIPE_H
#define QERECIPE_H

#include <QWidget>
#include <QGroupBox>
#include <QCaWidget.h>
#include <QLineEdit>
#include <QRadioButton>
#include <QGridLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QEConfiguredLayout.h>
#include <QDomDocument>
#include <QCaPluginLibrary_global.h>




// ============================================================
//  QERECIPE METHODS
// ============================================================
class QCAPLUGINLIBRARYSHARED_EXPORT QERecipe:public QWidget, public QCaWidget
{

    Q_OBJECT

    private:


    protected:
        QLabel *qLabelRecipeDescription;
        QComboBox *qComboBoxRecipeList;
        QPushButton *qPushButtonNew;
        QPushButton *qPushButtonSave;
        QPushButton *qPushButtonDelete;
        QPushButton *qPushButtonApply;
        QPushButton *qPushButtonRead;
        QEConfiguredLayout *qEConfiguredLayoutRecipeFields;
        QDomDocument document;
        QString recipeFile;
        int detailsLayout;
        int currentUserType;


    public:

        QERecipe(QWidget *pParent = 0);
        virtual ~QERecipe(){}

        void setRecipeDescription(QString pValue);
        QString getRecipeDescription();

        void setShowRecipeList(bool pValue);
        bool getShowRecipeList();

        void setShowNew(bool pValue);
        bool getShowNew();

        void setShowSave(bool pValue);
        bool getShowSave();

        void setShowDelete(bool pValue);
        bool getShowDelete();

        void setShowApply(bool pValue);
        bool getShowApply();

        void setShowRead(bool pValue);
        bool getShowRead();

        void setShowFields(bool pValue);
        bool getShowFields();

        void setConfigurationType(int pValue);
        int getConfigurationType();

        void setConfigurationFile(QString pValue);
        QString getConfigurationFile();

        void setRecipeFile(QString pValue);
        QString getRecipeFile();

        void setConfigurationText(QString pValue);
        QString getConfigurationText();

        void setDetailsLayout(int pValue);
        int getDetailsLayout();

        void setCurrentUserType(int pValue);
        int getCurrentUserType();

        bool saveRecipeList(QString pFilename);

        void refreshRecipeList();

        void userLevelChanged(userLevels pValue);


        Q_PROPERTY(QString recipeDescription READ getRecipeDescription WRITE setRecipeDescription)

        Q_PROPERTY(bool showRecipeList READ getShowRecipeList WRITE setShowRecipeList)

        Q_PROPERTY(bool showNew READ getShowNew WRITE setShowNew)

        Q_PROPERTY(bool showSave READ getShowSave WRITE setShowSave)

        Q_PROPERTY(bool showDelete READ getShowDelete WRITE setShowDelete)

        Q_PROPERTY(bool showApply READ getShowApply WRITE setShowApply)

        Q_PROPERTY(bool showRead READ getShowRead WRITE setShowRead)

        Q_PROPERTY(bool showFields READ getShowFields WRITE setShowFields)

        Q_PROPERTY(QString recipeFile READ getRecipeFile WRITE setRecipeFile)


        Q_ENUMS(configurationTypesProperty)
        Q_PROPERTY(configurationTypesProperty configurationType READ getConfigurationTypeProperty WRITE setConfigurationTypeProperty)
        enum configurationTypesProperty
        {
            File = FROM_FILE,
            Text = FROM_TEXT
        };

        void setConfigurationTypeProperty(configurationTypesProperty pConfigurationType)
        {
            setConfigurationType((configurationTypesProperty) pConfigurationType);
        }
        configurationTypesProperty getConfigurationTypeProperty()
        {
            return (configurationTypesProperty) getConfigurationType();
        }

        Q_PROPERTY(QString configurationFile READ getConfigurationFile WRITE setConfigurationFile)

        Q_PROPERTY(QString configurationText READ getConfigurationText WRITE setConfigurationText)

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


        Q_ENUMS(userTypesProperty)
        Q_PROPERTY(userTypesProperty currentUserType READ getCurrentUserTypeProperty WRITE setCurrentUserTypeProperty)
        enum userTypesProperty
        {
            User = USERLEVEL_USER,
            Scientist = USERLEVEL_SCIENTIST,
            Engineer = USERLEVEL_ENGINEER
        };

        void setCurrentUserTypeProperty(userTypesProperty pUserType)
        {
            setCurrentUserType((userTypesProperty) pUserType);
        }
        userTypesProperty getCurrentUserTypeProperty()
        {
            return (userTypesProperty) getCurrentUserType();
        }


    private slots:
        void comboBoxRecipeSelected(int);

        void buttonNewClicked();

        void buttonSaveClicked();

        void buttonDeleteClicked();

        void buttonApplyClicked();

        void buttonReadClicked();

};





#endif /// QERECIPE_H

