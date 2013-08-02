/*  QEFormGrid.h
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

#ifndef QEFORMGRID_H
#define QEFORMGRID_H

#include <QEFrame.h>
#include <QEForm.h>
#include <QString>
#include <QStringList>
#include <QGridLayout>

#include <QEPluginLibrary_global.h>

/// This class load a grid QEForms.
///
class QEPLUGINLIBRARYSHARED_EXPORT QEFormGrid : public QEFrame {
   Q_OBJECT

public:
   // QEFormGrid specific properties ===============================================
   //
   /// GridOrders specfies how grid elements are layed out.
   ///
   /// Row major (example 10, items, 3 cols):
   ///    0  1  2
   ///    3  4  5
   ///    6  7  8
   ///    9  -  -
   ///
   /// Row major (example 10, items, 3 cols):
   ///    0  4  8
   ///    1  5  9
   ///    2  6  -
   ///    3  7  -
   ///
   enum GridOrders {
      RowMajor = 0,
      ColMajor = 1
   };
   Q_ENUMS (GridOrders)


   /// The uiFile loaded into each QEForm element.
   /// Default value: ""
   Q_PROPERTY (QString uiFile          READ getUiFile          WRITE setUiFile )

   /// The total number of QEForms. This is restricted to the range 1 to 210.
   /// The upper limit 210 is the product of the first form prime numbers.
   /// Default value: 4.
   Q_PROPERTY (int number              READ getNumber           WRITE setNumber     )


   /// Specifies the number of columns.  This is restricted to the range 1 to 42.
   /// Default value: 1.
   Q_PROPERTY (int columns             READ getColumns          WRITE setColumns    )

   /// Specifies the gridOrder.
   /// Default value: RowMajor
   Q_PROPERTY (GridOrders gridOrder    READ getGridOrder        WRITE setGridOrder )

   /// Margin of the internal QGridLayout object.
   /// Default value: 2.
   Q_PROPERTY (int margin              READ getMargin           WRITE setMargin     )

   /// Spaceing of the internal QGridLayout object.
   /// Default value: 2.
   Q_PROPERTY (int spacing             READ getSpacing          WRITE setSpacing    )


   /// Specifies the SLOT macro number offset.
   /// Default value: 1.
   Q_PROPERTY (int slotNumberOffset    READ getSlotOffset       WRITE setSlotOffset  )

   /// Specifies the SLOT macro width format.
   /// Default value: 2.
   Q_PROPERTY (int slotNumberWidth     READ getSlotNumberWidth  WRITE setSlotNumberWidth  )

   /// Specifies the SLOTNAME macro values applied to each QEFrom grid element.
   /// Slot number allocation is impacted by the gridOrder property.
   /// Indexing of the slotStrings is NOT impacted by the slotNumberOffset.
   /// Default value: ""
   Q_PROPERTY (QStringList slotStrings READ getSlotStrings      WRITE setSlotStrings )

   // Ditto ROW and COL

   /// Specifies the ROW macro number offset.
   /// Default value: 1.
   Q_PROPERTY (int rowNumberOffset     READ getRowOffset        WRITE setRowOffset  )

   /// Specifies the COL macro width format.
   /// Default value: 2.
   Q_PROPERTY (int rowNumberWidth      READ getRowNumberWidth   WRITE setRowNumberWidth  )

   /// Specifies the ROWNAME macro values applied to each QEFrom grid element in a spefic row.
   /// Indexing of the rowStrings is NOT impacted by the rowNumberOffset.
   /// Default value: ""
   Q_PROPERTY (QStringList rowStrings  READ getRowStrings       WRITE setRowStrings )


   /// Specifies the COL macro number offset.
   /// Default value: 1.
   Q_PROPERTY (int colNumberOffset     READ getColOffset        WRITE setColOffset  )

   /// Specifies the COL macro width format.
   /// Default value: 2.
   Q_PROPERTY (int colNumberWidth      READ getColNumberWidth   WRITE setColNumberWidth  )

   /// Specifies the COLNAME macro values applied to each QEFrom grid element in a spefic column.
   /// Indexing of the colStrings is NOT impacted by the colNumberOffset.
   /// Default value: ""
   Q_PROPERTY (QStringList colStrings  READ getColStrings       WRITE setColStrings )

   //
   // End of QEFormGrid specific properties =========================================

public:
   /// Create a grid widget with default parameters.
   ///
   explicit QEFormGrid (QWidget* parent = 0);

   /// Destruction - place holder
   virtual ~QEFormGrid () { }

   // Property access functions.
   //
   void    setUiFile (QString uiFileName);
   QString getUiFile ();

   void setNumber (int n);
   int getNumber ();

   void setColumns (int n);
   int getColumns ();

   // The number of rows is dertmined from the overall number of items
   // and the number of columns. It cannot be independently set.
   //
   int getRows ();

   void setGridOrder (GridOrders go);
   GridOrders getGridOrder ();

   void setMargin (int n);
   int getMargin ();

   void setSpacing (int n);
   int getSpacing ();


   // Define propery access functions.
#define SET_GET_ATTRIBUTES(Attr, attr)               \
   void set##Attr##Offset (int n);                   \
   int  get##Attr##Offset ();                        \
   void set##Attr##NumberWidth (int n);              \
   int  get##Attr##NumberWidth ();                   \
   void set##Attr##Strings (QStringList& strings);   \
   QStringList get##Attr##Strings ();


SET_GET_ATTRIBUTES (Row, row)
SET_GET_ATTRIBUTES (Col, col)
SET_GET_ATTRIBUTES (Slot, slot)

#undef SET_GET_ATTRIBUTES


protected:
   QSize sizeHint () const;

private:
   static const int MaximumForms = 210;    // 2*3*5*7
   static const int MaximumColumns = 42;   // 2*3*7
   QGridLayout* layout;
   QList<QEForm*> formsList;              // holds a reference to each of the QEForms

   // Holds the property values.
   //
   QString uiFileName;
   int columns;
   GridOrders gridOrder;

   class MacroData {
   public:
      explicit MacroData ();
      int offset;          // defaults to 1 - typically 0 or 1
      int numberWidth;     // defaults to 2 - typically 1 to 3
      QStringList strings;
      QString prefix;

      // Generate macto sub. string of format
      // <prefix>NAME=
      QString genSubsitutions (const int n);
   };

   MacroData rowMacroData;
   MacroData colMacroData;
   MacroData slotMacroData;

   // Converts row and col number to slot number and vice-versa.
   // The conversions are gridOrder dependent.
   // Slots, rows and cols are 0 to N-1
   //
   int slotOf (const int row, const int col);
   void splitSlot (const int slot, int& row, int& col);

   // Local utility functions.
   //
   void createSubForm ();                       // Crates next QEForm instances
   void reLayoutForms ();                       // Re assigns all grid layouts
   void setFormSubstitutions (const int slot);  // Set up the macro substitutions for a single QEForm
   void setSubstitutions ();                    // Set up the macro substitutions for all QEForms

private slots:

};

#endif // QEFORMGRID_H
