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

#ifndef QESCRIPT_H
#define QESCRIPT_H

#include <QTableWidgetItem>
#include <QPushButton>
#include <QEWidget.h>


enum details
{
    TOP,
    BOTTOM,
    LEFT,
    RIGHT
};



// ============================================================
//  _QTABLEWIDGETSCRIPT CLASS
// ============================================================
class _QTableWidgetScript:public QTableWidget
{

    Q_OBJECT

    private:
        bool initialized;


    protected:


    public:
        _QTableWidgetScript(QWidget * pParent = 0);
        void refreshSize();
        void resizeEvent(QResizeEvent *);
        void resize(int w, int h);


    private slots:

};




// ============================================================
//  _QCOPYPASTE CLASS
// ============================================================
class _CopyPaste
{

    private:
        bool enable;
        QString program;
        QString parameters;
        int timeOut;
        bool stop;
        bool log;

    public:
        _CopyPaste();

        _CopyPaste(bool pEnable, QString pProgram, QString pParameters, int pTimeOut, bool pStop, bool pLog);

        void setEnable(bool pEnable);
        bool getEnable();

        void setProgram(QString pProgram);
        QString getProgram();

        void setParameters(QString pParameters);
        QString getParameters();

        void setTimeOut(int pTimeOut);
        int getTimeOut();

        void setStop(bool pStop);
        bool getStop();

        void setLog(bool pLog);
        bool getLog();

};




// ============================================================
//  QESCRIPT METHODS
// ============================================================
class QEPLUGINLIBRARYSHARED_EXPORT QEScript:public QWidget, public QEWidget
{

    Q_OBJECT

    private:


    protected:
        QComboBox *qComboBoxScriptList;
        QPushButton *qPushButtonNew;
        QPushButton *qPushButtonSave;
        QPushButton *qPushButtonDelete;
        QPushButton *qPushButtonExecute;
        QPushButton *qPushButtonAdd;
        QPushButton *qPushButtonRemove;
        QPushButton *qPushButtonUp;
        QPushButton *qPushButtonDown;
        QPushButton *qPushButtonCopy;
        QPushButton *qPushButtonPaste;
        _QTableWidgetScript *qTableWidgetScript;
        QString scriptFile;
        int optionsLayout;
        QDomDocument document;
        QString filename;
        QList<_CopyPaste *> copyPasteList;


    public:

        QEScript(QWidget *pParent = 0);
        virtual ~QEScript(){}


        void setShowScriptList(bool pValue);
        bool getShowScriptList();

        void setShowNew(bool pValue);
        bool getShowNew();

        void setShowSave(bool pValue);
        bool getShowSave();

        void setShowDelete(bool pValue);
        bool getShowDelete();

        void setShowExecute(bool pValue);
        bool getShowExecute();

        void setShowTable(bool pValue);
        bool getShowTable();

        void setShowTableControl(bool pValue);
        bool getShowTableControl();

        void setShowColumnNumber(bool pValue);
        bool getShowColumnNumber();

        void setShowColumnEnable(bool pValue);
        bool getShowColumnEnable();

        void setShowColumnProgram(bool pValue);
        bool getShowColumnProgram();

        void setShowColumnParameters(bool pValue);
        bool getShowColumnParameters();

        void setShowColumnTimeOut(bool pValue);
        bool getShowColumnTimeOut();

        void setShowColumnStop(bool pValue);
        bool getShowColumnStop();

        void setShowColumnLog(bool pValue);
        bool getShowColumnLog();

        void setScriptFile(QString pValue);
        QString getScriptFile();

        void setExecuteText(QString pValue);
        QString getExecuteText();


        void setOptionsLayout(int pValue);
        int getOptionsLayout();


        void refreshScriptList();

        void updateWidgets();


        Q_PROPERTY(bool showScriptList READ getShowScriptList WRITE setShowScriptList)

        Q_PROPERTY(bool showNew READ getShowNew WRITE setShowNew)

        Q_PROPERTY(bool showSave READ getShowSave WRITE setShowSave)

        Q_PROPERTY(bool showDelete READ getShowDelete WRITE setShowDelete)

        Q_PROPERTY(bool showExecute READ getShowExecute WRITE setShowExecute)

        Q_PROPERTY(bool showTable READ getShowTable WRITE setShowTable)

        Q_PROPERTY(bool showTableControl READ getShowTableControl WRITE setShowTableControl)

        Q_PROPERTY(bool showColumnNumber READ getShowColumnNumber WRITE setShowColumnNumber)

        Q_PROPERTY(bool showColumnEnable READ getShowColumnEnable WRITE setShowColumnEnable)

        Q_PROPERTY(bool showColumnProgram READ getShowColumnProgram WRITE setShowColumnProgram)

        Q_PROPERTY(bool showColumnParameters READ getShowColumnParameters WRITE setShowColumnParameters)

        Q_PROPERTY(bool showColumnTimeOut READ getShowColumnTimeOut WRITE setShowColumnTimeOut)

        Q_PROPERTY(bool showColumnStop READ getShowColumnStop WRITE setShowColumnStop)

        Q_PROPERTY(bool showColumnLog READ getShowColumnLog WRITE setShowColumnLog)

        Q_PROPERTY(QString scriptFile READ getScriptFile WRITE setScriptFile)

        Q_PROPERTY(QString executeText READ getExecuteText WRITE setExecuteText)


        Q_ENUMS(optionsLayoutProperty)
        Q_PROPERTY(optionsLayoutProperty optionsLayout READ getOptionsLayoutProperty WRITE setOptionsLayoutProperty)
        enum optionsLayoutProperty
        {
            Top = TOP,
            Bottom = BOTTOM,
            Left = LEFT,
            Right = RIGHT
        };        

        void setOptionsLayoutProperty(optionsLayoutProperty pOptionsLayout)
        {
            setOptionsLayout((optionsLayoutProperty) pOptionsLayout);
        }
        optionsLayoutProperty getOptionsLayoutProperty()
        {
            return (optionsLayoutProperty) getOptionsLayout();
        }





        // BEGIN-STANDARD-PROPERTIES ======================================================
        // Standard properties
        // These properties should be identical for every widget using them.
        // WHEN MAKING CHANGES: Use the update_widget_properties script in the
        // resources directory.
    public:
        /// Use the variable as the tool tip. Default is true. Tool tip property will be overwritten by the variable name.
        ///
        Q_PROPERTY(bool variableAsToolTip READ getVariableAsToolTip WRITE setVariableAsToolTip)

        /// Allow drag/drops operations to this widget. Default is false. Any dropped text will be used as a new variable name.
        ///
        Q_PROPERTY(bool allowDrop READ getAllowDrop WRITE setAllowDrop)

        /// Display the widget. Default is true.
        /// Setting this property false is usefull if widget is only used to provide a signal - for example, when supplying data to a QELink widget.
        /// Note, when false the widget will still be visible in Qt Designer.
        Q_PROPERTY(bool visible READ getRunVisible WRITE setRunVisible)

        /// Set the ID used by the message filtering system. Default is zero.
        /// Widgets or applications that use messages from the framework have the option of filtering on this ID.
        /// For example, by using a unique message source ID a QELog widget may be set up to only log messages from a select set of widgets.
        Q_PROPERTY(unsigned int messageSourceId READ getMessageSourceId WRITE setMessageSourceId )

        /// Style Sheet string to be applied when the widget is displayed in 'User' mode. Default is an empty string.
        /// The syntax is the standard Qt Style Sheet syntax. For example, 'background-color: red'
        /// This Style Sheet string will be applied by the styleManager class.
        /// Refer to the styleManager class for details about how this Style Sheet string will be merged with any pre-existing Style Sheet string
        /// and any Style Sheet strings generated during the display of data.
        Q_PROPERTY(QString userLevelUserStyle READ getStyleUser WRITE setStyleUser)

        /// Style Sheet string to be applied when the widget is displayed in 'Scientist' mode. Default is an empty string.
        /// The syntax is the standard Qt Style Sheet syntax. For example, 'background-color: red'
        /// This Style Sheet string will be applied by the styleManager class.
        /// Refer to the styleManager class for details about how this Style Sheet string will be merged with any pre-existing Style Sheet string
        /// and any Style Sheet strings generated during the display of data.
        Q_PROPERTY(QString userLevelScientistStyle READ getStyleScientist WRITE setStyleScientist)

        /// Style Sheet string to be applied when the widget is displayed in 'Engineer' mode. Default is an empty string.
        /// The syntax is the standard Qt Style Sheet syntax. For example, 'background-color: red'
        /// This Style Sheet string will be applied by the styleManager class.
        /// Refer to the styleManager class for details about how this Style Sheet string will be merged with any pre-existing Style Sheet string
        /// and any Style Sheet strings generated during the display of data.
        Q_PROPERTY(QString userLevelEngineerStyle READ getStyleEngineer WRITE setStyleEngineer)

        /// \enum UserLevels
        /// User friendly enumerations for #userLevelVisibility and #userLevelEnabled properties - refer to #userLevelVisibility and #userLevelEnabled properties and userLevel enumeration for details.
        enum UserLevels { User      = userLevelTypes::USERLEVEL_USER,          ///< Refer to USERLEVEL_USER for details
                          Scientist = userLevelTypes::USERLEVEL_SCIENTIST,     ///< Refer to USERLEVEL_SCIENTIST for details
                          Engineer  = userLevelTypes::USERLEVEL_ENGINEER       ///< Refer to USERLEVEL_ENGINEER for details
                                  };
        Q_ENUMS(UserLevels)

        /// Lowest user level at which the widget is visible. Default is 'User'.
        /// Used when designing GUIs that display more and more detail according to the user mode.
        /// The user mode is set application wide through the QELogin widget, or programatically through setUserLevel()
        /// Widgets that are always visible should be visible at 'User'.
        /// Widgets that are only used by scientists managing the facility should be visible at 'Scientist'.
        /// Widgets that are only used by engineers maintaining the facility should be visible at 'Engineer'.
        Q_PROPERTY(UserLevels userLevelVisibility READ getUserLevelVisibilityProperty WRITE setUserLevelVisibilityProperty)

        /// Lowest user level at which the widget is enabled. Default is 'User'.
        /// Used when designing GUIs that allow access to more and more detail according to the user mode.
        /// The user mode is set application wide through the QELogin widget, or programatically through setUserLevel()
        /// Widgets that are always accessable should be visible at 'User'.
        /// Widgets that are only accessable to scientists managing the facility should be visible at 'Scientist'.
        /// Widgets that are only accessable to engineers maintaining the facility should be visible at 'Engineer'.
        Q_PROPERTY(UserLevels userLevelEnabled READ getUserLevelEnabledProperty WRITE setUserLevelEnabledProperty)

        UserLevels getUserLevelVisibilityProperty() { return (UserLevels)getUserLevelVisibility(); }            ///< Access function for #userLevelVisibility property - refer to #userLevelVisibility property for details
        void setUserLevelVisibilityProperty( UserLevels level ) { setUserLevelVisibility( (userLevelTypes::userLevels)level ); }///< Access function for #userLevelVisibility property - refer to #userLevelVisibility property for details
        UserLevels getUserLevelEnabledProperty() { return (UserLevels)getUserLevelEnabled(); }                  ///< Access function for #userLevelEnabled property - refer to #userLevelEnabled property for details
        void setUserLevelEnabledProperty( UserLevels level ) { setUserLevelEnabled( (userLevelTypes::userLevels)level ); }      ///< Access function for #userLevelEnabled property - refer to #userLevelEnabled property for details

        /// If set (default) widget will indicate the alarm state of any variable data is displaying.
        /// Typically the background colour is set to indicate the alarm state.
        /// Note, this property is included in the set of standard properties as it applies to most widgets. It
        /// will do nothing for widgets that don't display data.
        Q_PROPERTY(bool displayAlarmState READ getDisplayAlarmState WRITE setDisplayAlarmState)

    public:
        // END-STANDARD-PROPERTIES ========================================================




    private slots:
        void comboBoxScriptSelected(int);

        void buttonNewClicked();

        void buttonSaveClicked();

        void buttonDeleteClicked();

        void buttonExecuteClicked();

        void buttonAddClicked();

        void buttonRemoveClicked();

        void buttonUpClicked();

        void buttonDownClicked();

        void buttonCopyClicked();

        void buttonPasteClicked();

        void selectionChanged(const QItemSelection &, const QItemSelection &);

        void itemActivated(QTableWidgetItem *);


    signals:
        void selected(QString pFilename);

};





#endif // QESCRIPT_H


