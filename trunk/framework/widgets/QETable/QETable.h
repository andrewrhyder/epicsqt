/*  QETable.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
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
 *  Copyright (c) 2013, 2014 Australian Synchrotron
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef QE_TABLE_H
#define QE_TABLE_H

#include <QHBoxLayout>
#include <QList>
#include <QString>
#include <QSize>
#include <QTableWidget>
#include <QVector>

#include <QECommon.h>
#include <QEAbstractWidget.h>
#include <QEFloatingArray.h>
#include <QEFloatingFormatting.h>

#include <QCaObject.h>
#include <QEWidget.h>
#include <QCaVariableNamePropertyManager.h>
#include <QEPluginLibrary_global.h>

// QEAbstractWidget provides all standard QEWidget properties
//
class QEPLUGINLIBRARYSHARED_EXPORT QETable : public QEAbstractWidget {

   Q_OBJECT

   // Strictly this defines the max number of variables. Must be consistant with the
   // number of variable name properties below and calls to PROPERTY_ACCESS below.
   //
   #define NUMBER_OF_VARIABLES 12


   // QETable specific properties ===============================================
   // Note, a property macro in the form 'Q_PROPERTY(QString variableName READ ...' doesn't work.
   // A property name ending with 'Name' results in some sort of string a variable being displayed,
   // but will only accept alphanumeric and won't generate callbacks on change.

   /// Macro substitutions. The default is no substitutions. The format is NAME1=VALUE1[,] NAME2=VALUE2...
   /// Values may be quoted strings. For example, 'PUMP=PMP3, NAME = "My Pump"'
   /// These substitutions are applied to variable names for all QE widgets.
   /// In some widgets are are also used for other purposes.
   ///
   Q_PROPERTY (QString variableSubstitutions READ getSubstitutions WRITE setSubstitutions)

   /// EPICS variable names (CA PV)
   ///
   Q_PROPERTY (QString variableName1  READ getVariableName1    WRITE setVariableName1)
   Q_PROPERTY (QString variableName2  READ getVariableName2    WRITE setVariableName2)
   Q_PROPERTY (QString variableName3  READ getVariableName3    WRITE setVariableName3)
   Q_PROPERTY (QString variableName4  READ getVariableName4    WRITE setVariableName4)
   Q_PROPERTY (QString variableName5  READ getVariableName5    WRITE setVariableName5)
   Q_PROPERTY (QString variableName6  READ getVariableName6    WRITE setVariableName6)
   Q_PROPERTY (QString variableName7  READ getVariableName7    WRITE setVariableName7)
   Q_PROPERTY (QString variableName8  READ getVariableName8    WRITE setVariableName8)
   Q_PROPERTY (QString variableName9  READ getVariableName9    WRITE setVariableName9)
   Q_PROPERTY (QString variableName10 READ getVariableName10   WRITE setVariableName10)
   Q_PROPERTY (QString variableName11 READ getVariableName11   WRITE setVariableName11)
   Q_PROPERTY (QString variableName12 READ getVariableName12   WRITE setVariableName12)

   /// The maximum number of array elements that will be displayed. Defaults to 4096.
   ///
   Q_PROPERTY (int displayMaximum     READ getDisplayMaximum   WRITE setDisplayMaximum)

   /// Determines if the variable values are displayed in rows (orientation is horizontal)
   /// or in columns (orientation is vertical). The default is vertical.
   ///
   Q_PROPERTY (Qt::Orientation orientation READ getOrientation WRITE setOrientation)

   /// Controls if table grid is displayed. Default to true.
   ///
   Q_PROPERTY (bool showGrid          READ showGrid            WRITE setShowGrid)

   /// Sets table grid style. Defaults to SolidLine.
   ///
   Q_PROPERTY (Qt::PenStyle gridStyle READ gridStyle           WRITE setGridStyle)
   //
   // End of QETable specific properties =========================================

public:
   /// Create without a variable(s).
   /// Use setVariableName functions.
   //
   explicit QETable (QWidget* parent = 0);

   /// Destruction
   virtual ~QETable() { }

   // Single function for all set/get PV properties.
   //
   void    setVariableName (const int, const QString&);
   QString getVariableName (const int) const;

   void setSubstitutions (const QString& substitutions);
   QString getSubstitutions () const;

   void setDisplayMaximum (const int displayMaximum);
   int getDisplayMaximum () const;

   void setOrientation (const Qt::Orientation orientation);
   Qt::Orientation getOrientation () const;

   // Property access READ and WRITE functions. We can define the access functions
   // using a macro.  Alas, due to SDK limitation, we cannot embedded the property
   // definitions in a macro.
   //
   #define PROPERTY_ACCESS(symbol, slot)                                                 \
      void    setVariableName##symbol (const QString& name)  { this->setVariableName (slot, name); }      \
      QString getVariableName##symbol () const { return this->getVariableName (slot); }

   PROPERTY_ACCESS  (1,  0)
   PROPERTY_ACCESS  (2,  1)
   PROPERTY_ACCESS  (3,  2)
   PROPERTY_ACCESS  (4,  3)
   PROPERTY_ACCESS  (5,  4)
   PROPERTY_ACCESS  (6,  5)
   PROPERTY_ACCESS  (7,  6)
   PROPERTY_ACCESS  (8,  7)
   PROPERTY_ACCESS  (9,  8)
   PROPERTY_ACCESS  (10, 9)
   PROPERTY_ACCESS  (11, 10)
   PROPERTY_ACCESS  (12, 11)

   #undef PROPERTY_ACCESS

   // Expose access to the internal widget's set/get functions.
   //
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (table, bool,         showGrid,  setShowGrid)
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (table, Qt::PenStyle, gridStyle, setGridStyle)

public slots:
   // Selects col/row depending on orientation vertical/horizontal.
   //
   void setSelection (int value);

public:
   int getSelection () const;

signals:
   void selectionChanged (int value);

   // Note, the following signals are common to many QE widgets,
   // if changing the doxygen comments, ensure relevent changes are migrated to all instances
   /// Sent when the widget is updated following a data change
   /// Can be used to pass on EPICS data (as presented in this widget) to other widgets.
   /// For example a QList widget could log updates from this widget.
   /// Note: this widget emits the numeric enumeration value as opposed to the associated text.
   void dbValueChanged (const QVector<double>& out);

protected:
   QSize sizeHint () const;
   void fontChange (const QFont& font);

   // override QEWidget fnctions.
   //
   void establishConnection (unsigned int variableIndex);
   qcaobject::QCaObject* createQcaItem (unsigned int variableIndex);

   // Drag and Drop
   //
   void dragEnterEvent (QDragEnterEvent *event) { qcaDragEnterEvent (event); }
   void dropEvent (QDropEvent *event)           { qcaDropEvent (event); }
   void mousePressEvent (QMouseEvent *event)    { qcaMousePressEvent (event); }
   void setDrop (QVariant drop);
   QVariant getDrop ();

   // Copy paste
   //
   QString copyVariable ();
   QVariant copyData ();
   void paste (QVariant s);

private:
   bool isVertical () const;
   void rePopulateTable ();
   void addVariableName (const QString& pvName);

   // Provides consistant interpretation of variableIndex.
   // Must be consistent with variableIndex allocation in the contructor.
   //
   int slotOf  (const unsigned int vi) { return (vi); }

   QTableWidget* table;         // internal widget
   QHBoxLayout* layout;         // holds the internal widget - any layout type will do
   int displayMaximum;
   Qt::Orientation orientation;
   QEFloatingFormatting floatingFormatting;
   int selection;

   // Per PV data.
   //
   class DataSets {
   public:
      explicit DataSets ();
      ~DataSets ();
      void setContext (QETable* owner, const int slot);

      bool isInUse () const;
      void rePopulateTable ();

      QEFloatingArray data;
      QCaAlarmInfo alarmInfo;

      QString pvName;
      QCaVariableNamePropertyManager variableNameManager;
      bool isConnected;

   private:
      QETable* owner;
      int slot;
   };

   DataSets dataSet [NUMBER_OF_VARIABLES];

private slots:
   void setNewVariableName (QString variableNameIn,
                            QString variableNameSubstitutionsIn,
                            unsigned int variableIndex);

   void connectionChanged (QCaConnectionInfo& connectionInfo,
                           const unsigned int &variableIndex);

   void dataArrayChanged (const QVector<double>& values,
                          QCaAlarmInfo& alarmInfo,
                          QCaDateTime& timeStamp,
                          const unsigned int& variableIndex);

   void currentCellChanged (int currentRow,  int currentCol,
                            int previousRow, int previousCol);
};

#endif // QE_TABLE_H
