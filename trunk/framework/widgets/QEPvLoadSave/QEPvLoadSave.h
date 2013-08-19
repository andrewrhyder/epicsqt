/*  QEPvLoadSave.h
 *
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
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef QEPVLOADSAVE_H
#define QEPVLOADSAVE_H

#include <QString>
#include <QAbstractItemModel>
#include <QTreeView>

#include <QCaObject.h>
#include <QEFrame.h>
#include <QEWidget.h>
#include <QEPluginLibrary_global.h>
#include <QCaVariableNamePropertyManager.h>

#include "QEPvLoadSaveCommon.h"

/// This class provides the means to create/manages prescribed set of PVs and
/// their associated values, but more importantly the capability to:
/// a) read the current value for each PV from the system;
/// b) write the set of names/values to a file;
/// c) read a the set of names/values from a file; and
/// d) wite the values to the associated PV.
///
/// Note: This widget is intented to be use as the sole widget of a built in form
/// However, the widget may be used in any form if needs be.
///

// Differed declaration - avoids mutual header inclusions.
//
class QEPvLoadSaveModel;

class QEPLUGINLIBRARYSHARED_EXPORT QEPvLoadSave : public QEFrame  {

   Q_OBJECT
   // QEPvLoadSave specific properties ==============================================
   //
public:
   /// configurationFile
   ///
   Q_PROPERTY (QString configurationFileLeft  READ getConfigurationFileLeft  WRITE setConfigurationFileLeft)
   Q_PROPERTY (QString configurationFileRight READ getConfigurationFileRight WRITE setConfigurationFileRight)

   /// Macro substitutions. The default is no substitutions. The format is NAME1=VALUE1[,] NAME2=VALUE2...
   /// Values may be quoted strings. For example, 'PUMP=PMP3, NAME = "My Pump"'
   /// These substitutions are applied to variable names for all QE widgets.
   /// In some widgets are are also used for other purposes.
   ///
   Q_PROPERTY (QString defaultSubstitutions READ getSubstitutions WRITE setSubstitutions)

   //
   // End of QEPvLoadSave specific properties =====================================


   // Standard properties:
   // variableAsToolTip and displayAlarmState prob not sensible, but part of standard set.
   //
public:
   /// Create without a nominated config file.
   ///
   QEPvLoadSave (QWidget *parent = 0);

   /// Destruction
   virtual ~QEPvLoadSave ();

   /// Size hint
   virtual QSize sizeHint () const;

   // Property access functions.
   //
   // We use same mechanism that is used to manage variable names to manage the configuration file name.
   //
   void    setConfigurationFileLeft (QString configurationFile)  { this->half [0]->setConfigurationFile (configurationFile); }
   QString getConfigurationFileLeft ()                    { return this->half [0]->getConfigurationFile (); }

   void    setConfigurationFileRight (QString configurationFile) { this->half [1]->setConfigurationFile (configurationFile); }
   QString getConfigurationFileRight ()                   { return this->half [1]->getConfigurationFile (); }

   void    setSubstitutions (QString configurationFileSubstitutions);
   QString getSubstitutions ();

   // Used internally but needs to be public.
   static const int NumberOfButtons = 15;

protected:
   // We don't expect these to be called - but do override and output debug error
   //
   qcaobject::QCaObject* createQcaItem (unsigned int variableIndex);
   void establishConnection (unsigned int variableIndex);

   void resizeEvent (QResizeEvent* );

   // No Drag and Drop
   // No copy paste yet

private:
   enum Sides { LeftSide = 0, RightSide = 1, ErrorSide = 2 };

   class Halves {
   public:
      Halves (const Sides side, QEPvLoadSave* owner, QBoxLayout* layout);

      // Used to store/access properties.
      //
      void setConfigurationFile  (const QString& configurationFile);
      QString getConfigurationFile ();

      void setConfigurationSubstitutions (const QString& substitutions);
      QString getConfigurationSubstitutions ();

      void open (const QString& configurationFile);

      QFrame* container;
      QVBoxLayout* halfLayout;
      QFrame* header;
      QPushButton *headerPushButtons [NumberOfButtons];

      QTreeView* tree;    // this is the tree widget
      QFrame* footer;

      QEPvLoadSaveModel* model;
      QCaVariableNamePropertyManager vnpm;

   private:
      Sides side;
      QEPvLoadSave* owner;
      QString configurationFile;
   };

   QVBoxLayout* overallLayout;
   QFrame* sidesFrame;
   QHBoxLayout* sideBySidelayout;
   Halves *half [2];  // two halves make a whole ;-)
   QFrame* loadSaveStatus;
   QLabel* loadSaveTitle;
   QProgressBar* progressBar;
   QPushButton* abortButton;


   Sides sideOfSender (QObject *sentBy);

private slots:
   void useNewConfigurationFileProperty (QString configurationFileIn,
                                         QString configurationFileSubstitutionsIn,
                                         unsigned int variableIndex );

   void acceptActionComplete (QEPvLoadSaveCommon::ActionKinds, bool);

   void writeAllClicked (bool);
   void readAllClicked (bool);
   void writeSubsetClicked (bool);
   void readSubsetClicked (bool);
   void archiveTimeClicked (bool);
   void copyAllClicked (bool);
   void copySubsetClicked (bool);
   void loadClicked (bool);
   void saveClicked (bool);
   void deleteClicked (bool);
   void editClicked (bool);
   void sortClicked (bool);
   void compareClicked (bool);
   void abortClicked (bool);
};

#endif // QEPVLOADSAVE_H
