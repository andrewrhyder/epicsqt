/*  QEScalarHistogram.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
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
 *  Copyright (c) 2014 Australian Synchrotron.
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef QE_SCALAR_HISTOGRAM_H
#define QE_SCALAR_HISTOGRAM_H

#include <QLayout>
#include <QString>
#include <QVector>

#include <QECommon.h>
#include <QEHistogram.h>
#include <QEFrame.h>
#include <QEWidget.h>
#include <QEFloating.h>
#include <QEFloatingFormatting.h>
#include <QCaVariableNamePropertyManager.h>
#include <QEPluginLibrary_global.h>

/// The QEScalarHistogram class is a EPICS aware histogram widget.
/// The value of, i.e. the length of each bar of the histogram is controlled by
/// its own process variable.
/// When a variable is defined (connected), the bar length is updated, and
/// optionally the bar colour set to reflect the variable's severity status.
/// The bar is 'grayed-out' when its variable is disconnected (although the bar
/// retains its last known value/length).
/// The histogram nature of the this widget is provided by a QEHistogram widget.
/// The QEScalarHistogram widget is tighly integrated with the base class QEWidget,
/// via QEFrame, which provides generic support such as macro substitutions,
/// drag/drop, and standard properties.
///
// Maximum number of variables.
#define QE_HISTOGRAM_NUMBER_VARIABLES 100

class QEPLUGINLIBRARYSHARED_EXPORT QEScalarHistogram:public QEFrame {
   Q_OBJECT

   // All standard properties are inherited from QEFrame.
   //
   /// Macro substitutions. The default is no substitutions.
   /// The format is NAME1=VALUE1[,] NAME2=VALUE2... Values may be quoted strings.
   /// For example, 'SAMPLE=SAM1, NAME = "Ref foil"'
   /// These substitutions are applied to all the variable names.
   Q_PROPERTY (QString variableSubstitutions READ getPvNameSubstitutions WRITE
               setPvNameSubstitutions)

   // Histogram properties
   //
   Q_PROPERTY (bool   autoBarGapWidths READ getAutoBarGapWidths WRITE setAutoBarGapWidths)
   Q_PROPERTY (int    barWidth         READ getBarWidth         WRITE setBarWidth)
   Q_PROPERTY (int    gap              READ getGap              WRITE setGap)
   Q_PROPERTY (double maximum          READ getMaximum          WRITE setMaximum)
   Q_PROPERTY (double minimum          READ getMinimum          WRITE setMinimum)
   Q_PROPERTY (double baseLine         READ getBaseLine         WRITE setBaseLine)
   Q_PROPERTY (bool   logScale         READ getLogScale         WRITE setLogScale  )
   // When dislayAlarmState set ture, this property value effectively ignored.
   Q_PROPERTY (QColor barColour        READ getBarColour        WRITE setBarColour)
   Q_PROPERTY (bool   drawBorder       READ getDrawBorder       WRITE setDrawBorder)
   Q_PROPERTY (Qt::Orientation orientation READ getOrientation  WRITE setOrientation)

   /// EPICS variable names (CA PV).
   // Note variableN uses variable index N-1.
   //
   Q_PROPERTY (QString variable1  READ getPvName0   WRITE setPvName0)
   Q_PROPERTY (QString variable2  READ getPvName1   WRITE setPvName1)
   Q_PROPERTY (QString variable3  READ getPvName2   WRITE setPvName2)
   Q_PROPERTY (QString variable4  READ getPvName3   WRITE setPvName3)
   Q_PROPERTY (QString variable5  READ getPvName4   WRITE setPvName4)
   Q_PROPERTY (QString variable6  READ getPvName5   WRITE setPvName5)
   Q_PROPERTY (QString variable7  READ getPvName6   WRITE setPvName6)
   Q_PROPERTY (QString variable8  READ getPvName7   WRITE setPvName7)
   Q_PROPERTY (QString variable9  READ getPvName8   WRITE setPvName8)
   Q_PROPERTY (QString variable10 READ getPvName9   WRITE setPvName9)
   Q_PROPERTY (QString variable11 READ getPvName10 WRITE setPvName10)
   Q_PROPERTY (QString variable12 READ getPvName11 WRITE setPvName11)
   Q_PROPERTY (QString variable13 READ getPvName12 WRITE setPvName12)
   Q_PROPERTY (QString variable14 READ getPvName13 WRITE setPvName13)
   Q_PROPERTY (QString variable15 READ getPvName14 WRITE setPvName14)
   Q_PROPERTY (QString variable16 READ getPvName15 WRITE setPvName15)
   Q_PROPERTY (QString variable17 READ getPvName16 WRITE setPvName16)
   Q_PROPERTY (QString variable18 READ getPvName17 WRITE setPvName17)
   Q_PROPERTY (QString variable19 READ getPvName18 WRITE setPvName18)
   Q_PROPERTY (QString variable20 READ getPvName19 WRITE setPvName19)
   Q_PROPERTY (QString variable21 READ getPvName20 WRITE setPvName20)
   Q_PROPERTY (QString variable22 READ getPvName21 WRITE setPvName21)
   Q_PROPERTY (QString variable23 READ getPvName22 WRITE setPvName22)
   Q_PROPERTY (QString variable24 READ getPvName23 WRITE setPvName23)
   Q_PROPERTY (QString variable25 READ getPvName24 WRITE setPvName24)
   Q_PROPERTY (QString variable26 READ getPvName25 WRITE setPvName25)
   Q_PROPERTY (QString variable27 READ getPvName26 WRITE setPvName26)
   Q_PROPERTY (QString variable28 READ getPvName27 WRITE setPvName27)
   Q_PROPERTY (QString variable29 READ getPvName28 WRITE setPvName28)
   Q_PROPERTY (QString variable30 READ getPvName29 WRITE setPvName29)
   Q_PROPERTY (QString variable31 READ getPvName30 WRITE setPvName30)
   Q_PROPERTY (QString variable32 READ getPvName31 WRITE setPvName31)
   Q_PROPERTY (QString variable33 READ getPvName32 WRITE setPvName32)
   Q_PROPERTY (QString variable34 READ getPvName33 WRITE setPvName33)
   Q_PROPERTY (QString variable35 READ getPvName34 WRITE setPvName34)
   Q_PROPERTY (QString variable36 READ getPvName35 WRITE setPvName35)
   Q_PROPERTY (QString variable37 READ getPvName36 WRITE setPvName36)
   Q_PROPERTY (QString variable38 READ getPvName37 WRITE setPvName37)
   Q_PROPERTY (QString variable39 READ getPvName38 WRITE setPvName38)
   Q_PROPERTY (QString variable40 READ getPvName39 WRITE setPvName39)
   Q_PROPERTY (QString variable41 READ getPvName40 WRITE setPvName40)
   Q_PROPERTY (QString variable42 READ getPvName41 WRITE setPvName41)
   Q_PROPERTY (QString variable43 READ getPvName42 WRITE setPvName42)
   Q_PROPERTY (QString variable44 READ getPvName43 WRITE setPvName43)
   Q_PROPERTY (QString variable45 READ getPvName44 WRITE setPvName44)
   Q_PROPERTY (QString variable46 READ getPvName45 WRITE setPvName45)
   Q_PROPERTY (QString variable47 READ getPvName46 WRITE setPvName46)
   Q_PROPERTY (QString variable48 READ getPvName47 WRITE setPvName47)
   Q_PROPERTY (QString variable49 READ getPvName48 WRITE setPvName48)
   Q_PROPERTY (QString variable50 READ getPvName49 WRITE setPvName49)
   Q_PROPERTY (QString variable51 READ getPvName50 WRITE setPvName50)
   Q_PROPERTY (QString variable52 READ getPvName51 WRITE setPvName51)
   Q_PROPERTY (QString variable53 READ getPvName52 WRITE setPvName52)
   Q_PROPERTY (QString variable54 READ getPvName53 WRITE setPvName53)
   Q_PROPERTY (QString variable55 READ getPvName54 WRITE setPvName54)
   Q_PROPERTY (QString variable56 READ getPvName55 WRITE setPvName55)
   Q_PROPERTY (QString variable57 READ getPvName56 WRITE setPvName56)
   Q_PROPERTY (QString variable58 READ getPvName57 WRITE setPvName57)
   Q_PROPERTY (QString variable59 READ getPvName58 WRITE setPvName58)
   Q_PROPERTY (QString variable60 READ getPvName59 WRITE setPvName59)
   Q_PROPERTY (QString variable61 READ getPvName60 WRITE setPvName60)
   Q_PROPERTY (QString variable62 READ getPvName61 WRITE setPvName61)
   Q_PROPERTY (QString variable63 READ getPvName62 WRITE setPvName62)
   Q_PROPERTY (QString variable64 READ getPvName63 WRITE setPvName63)
   Q_PROPERTY (QString variable65 READ getPvName64 WRITE setPvName64)
   Q_PROPERTY (QString variable66 READ getPvName65 WRITE setPvName65)
   Q_PROPERTY (QString variable67 READ getPvName66 WRITE setPvName66)
   Q_PROPERTY (QString variable68 READ getPvName67 WRITE setPvName67)
   Q_PROPERTY (QString variable69 READ getPvName68 WRITE setPvName68)
   Q_PROPERTY (QString variable70 READ getPvName69 WRITE setPvName69)
   Q_PROPERTY (QString variable71 READ getPvName70 WRITE setPvName70)
   Q_PROPERTY (QString variable72 READ getPvName71 WRITE setPvName71)
   Q_PROPERTY (QString variable73 READ getPvName72 WRITE setPvName72)
   Q_PROPERTY (QString variable74 READ getPvName73 WRITE setPvName73)
   Q_PROPERTY (QString variable75 READ getPvName74 WRITE setPvName74)
   Q_PROPERTY (QString variable76 READ getPvName75 WRITE setPvName75)
   Q_PROPERTY (QString variable77 READ getPvName76 WRITE setPvName76)
   Q_PROPERTY (QString variable78 READ getPvName77 WRITE setPvName77)
   Q_PROPERTY (QString variable79 READ getPvName78 WRITE setPvName78)
   Q_PROPERTY (QString variable80 READ getPvName79 WRITE setPvName79)
   Q_PROPERTY (QString variable81 READ getPvName80 WRITE setPvName80)
   Q_PROPERTY (QString variable82 READ getPvName81 WRITE setPvName81)
   Q_PROPERTY (QString variable83 READ getPvName82 WRITE setPvName82)
   Q_PROPERTY (QString variable84 READ getPvName83 WRITE setPvName83)
   Q_PROPERTY (QString variable85 READ getPvName84 WRITE setPvName84)
   Q_PROPERTY (QString variable86 READ getPvName85 WRITE setPvName85)
   Q_PROPERTY (QString variable87 READ getPvName86 WRITE setPvName86)
   Q_PROPERTY (QString variable88 READ getPvName87 WRITE setPvName87)
   Q_PROPERTY (QString variable89 READ getPvName88 WRITE setPvName88)
   Q_PROPERTY (QString variable90 READ getPvName89 WRITE setPvName89)
   Q_PROPERTY (QString variable91 READ getPvName90 WRITE setPvName90)
   Q_PROPERTY (QString variable92 READ getPvName91 WRITE setPvName91)
   Q_PROPERTY (QString variable93 READ getPvName92 WRITE setPvName92)
   Q_PROPERTY (QString variable94 READ getPvName93 WRITE setPvName93)
   Q_PROPERTY (QString variable95 READ getPvName94 WRITE setPvName94)
   Q_PROPERTY (QString variable96 READ getPvName95 WRITE setPvName95)
   Q_PROPERTY (QString variable97 READ getPvName96 WRITE setPvName96)
   Q_PROPERTY (QString variable98 READ getPvName97 WRITE setPvName97)
   Q_PROPERTY (QString variable99 READ getPvName98 WRITE setPvName98)
   Q_PROPERTY (QString variable100 READ getPvName99 WRITE setPvName99)

public:
   explicit QEScalarHistogram (QWidget* parent = 0);

   // Expose access to the internal widget's set/get functions.
   //
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (histogram, double, setMinimum, getMinimum)
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (histogram, double, setMaximum, getMaximum)
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (histogram, double, setBaseLine, getBaseLine)
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (histogram, int,    setGap, getGap)
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (histogram, int,    setBarWidth, getBarWidth)
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (histogram, bool,   setAutoBarGapWidths, getAutoBarGapWidths)
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (histogram, bool,   setLogScale, getLogScale)
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (histogram, bool,   setDrawBorder, getDrawBorder)
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (histogram, QColor, setBarColour, getBarColour)
   QE_EXPOSE_INTERNAL_OBJECT_FUNCTIONS (histogram, Qt::Orientation, setOrientation, getOrientation)

protected:
   qcaobject::QCaObject* createQcaItem (unsigned int variableIndex);
   void establishConnection (unsigned int variableIndex);
   bool eventFilter (QObject* obj, QEvent* event);

   // Drag and Drop
   void dragEnterEvent (QDragEnterEvent* event) { qcaDragEnterEvent (event);  }
   void dropEvent (QDropEvent* event)           { qcaDropEvent (event);       }
   void mousePressEvent (QMouseEvent* event)    { qcaMousePressEvent (event); }
   void setDrop (QVariant drop);
   QVariant getDrop ();

   // Copy paste
   QString copyVariable ();
   QVariant copyData ();


private:
   void setPvNameSubstitutions (const QString& subs);
   QString getPvNameSubstitutions () const;
   void setReadOut (const QString& text);
   void genReadOut (const int index);

   QEHistogram* histogram;
   QHBoxLayout* layout;         // holds the histogram - any layout type will do
   QCaVariableNamePropertyManager vnpm [QE_HISTOGRAM_NUMBER_VARIABLES];
   QEFloatingFormatting floatingFormatting;
   int selectedChannel;         //

private slots:
   void newVariableNameProperty (QString pvName, QString subs, unsigned int variableIndex);

   void connectionChanged (QCaConnectionInfo& connectionInfo, const unsigned int&variableIndex);

   void setChannelValue (const double&value, QCaAlarmInfo&,
                         QCaDateTime&, const unsigned int&);

private:
   // Define a variable access
   // Note, the Q_PROPERTY declaration itself can't be in a macro.
   //
#define VARIABLE_PROPERTY_ACCESS(VAR_INDEX)                                    \
   void setPvName##VAR_INDEX (const QString& pvName) {                         \
   this->vnpm [VAR_INDEX].setVariableNameProperty (pvName);                    \
}                                                                              \
                                                                               \
   QString getPvName##VAR_INDEX () const {                                     \
   return this->vnpm [VAR_INDEX].getVariableNameProperty ();                   \
}


   VARIABLE_PROPERTY_ACCESS (0)
   VARIABLE_PROPERTY_ACCESS (1)
   VARIABLE_PROPERTY_ACCESS (2)
   VARIABLE_PROPERTY_ACCESS (3)
   VARIABLE_PROPERTY_ACCESS (4)
   VARIABLE_PROPERTY_ACCESS (5)
   VARIABLE_PROPERTY_ACCESS (6)
   VARIABLE_PROPERTY_ACCESS (7)
   VARIABLE_PROPERTY_ACCESS (8)
   VARIABLE_PROPERTY_ACCESS (9)
   VARIABLE_PROPERTY_ACCESS (10)
   VARIABLE_PROPERTY_ACCESS (11)
   VARIABLE_PROPERTY_ACCESS (12)
   VARIABLE_PROPERTY_ACCESS (13)
   VARIABLE_PROPERTY_ACCESS (14)
   VARIABLE_PROPERTY_ACCESS (15)
   VARIABLE_PROPERTY_ACCESS (16)
   VARIABLE_PROPERTY_ACCESS (17)
   VARIABLE_PROPERTY_ACCESS (18)
   VARIABLE_PROPERTY_ACCESS (19)
   VARIABLE_PROPERTY_ACCESS (20)
   VARIABLE_PROPERTY_ACCESS (21)
   VARIABLE_PROPERTY_ACCESS (22)
   VARIABLE_PROPERTY_ACCESS (23)
   VARIABLE_PROPERTY_ACCESS (24)
   VARIABLE_PROPERTY_ACCESS (25)
   VARIABLE_PROPERTY_ACCESS (26)
   VARIABLE_PROPERTY_ACCESS (27)
   VARIABLE_PROPERTY_ACCESS (28)
   VARIABLE_PROPERTY_ACCESS (29)
   VARIABLE_PROPERTY_ACCESS (30)
   VARIABLE_PROPERTY_ACCESS (31)
   VARIABLE_PROPERTY_ACCESS (32)
   VARIABLE_PROPERTY_ACCESS (33)
   VARIABLE_PROPERTY_ACCESS (34)
   VARIABLE_PROPERTY_ACCESS (35)
   VARIABLE_PROPERTY_ACCESS (36)
   VARIABLE_PROPERTY_ACCESS (37)
   VARIABLE_PROPERTY_ACCESS (38)
   VARIABLE_PROPERTY_ACCESS (39)
   VARIABLE_PROPERTY_ACCESS (40)
   VARIABLE_PROPERTY_ACCESS (41)
   VARIABLE_PROPERTY_ACCESS (42)
   VARIABLE_PROPERTY_ACCESS (43)
   VARIABLE_PROPERTY_ACCESS (44)
   VARIABLE_PROPERTY_ACCESS (45)
   VARIABLE_PROPERTY_ACCESS (46)
   VARIABLE_PROPERTY_ACCESS (47)
   VARIABLE_PROPERTY_ACCESS (48)
   VARIABLE_PROPERTY_ACCESS (49)
   VARIABLE_PROPERTY_ACCESS (50)
   VARIABLE_PROPERTY_ACCESS (51)
   VARIABLE_PROPERTY_ACCESS (52)
   VARIABLE_PROPERTY_ACCESS (53)
   VARIABLE_PROPERTY_ACCESS (54)
   VARIABLE_PROPERTY_ACCESS (55)
   VARIABLE_PROPERTY_ACCESS (56)
   VARIABLE_PROPERTY_ACCESS (57)
   VARIABLE_PROPERTY_ACCESS (58)
   VARIABLE_PROPERTY_ACCESS (59)
   VARIABLE_PROPERTY_ACCESS (60)
   VARIABLE_PROPERTY_ACCESS (61)
   VARIABLE_PROPERTY_ACCESS (62)
   VARIABLE_PROPERTY_ACCESS (63)
   VARIABLE_PROPERTY_ACCESS (64)
   VARIABLE_PROPERTY_ACCESS (65)
   VARIABLE_PROPERTY_ACCESS (66)
   VARIABLE_PROPERTY_ACCESS (67)
   VARIABLE_PROPERTY_ACCESS (68)
   VARIABLE_PROPERTY_ACCESS (69)
   VARIABLE_PROPERTY_ACCESS (70)
   VARIABLE_PROPERTY_ACCESS (71)
   VARIABLE_PROPERTY_ACCESS (72)
   VARIABLE_PROPERTY_ACCESS (73)
   VARIABLE_PROPERTY_ACCESS (74)
   VARIABLE_PROPERTY_ACCESS (75)
   VARIABLE_PROPERTY_ACCESS (76)
   VARIABLE_PROPERTY_ACCESS (77)
   VARIABLE_PROPERTY_ACCESS (78)
   VARIABLE_PROPERTY_ACCESS (79)
   VARIABLE_PROPERTY_ACCESS (80)
   VARIABLE_PROPERTY_ACCESS (81)
   VARIABLE_PROPERTY_ACCESS (82)
   VARIABLE_PROPERTY_ACCESS (83)
   VARIABLE_PROPERTY_ACCESS (84)
   VARIABLE_PROPERTY_ACCESS (85)
   VARIABLE_PROPERTY_ACCESS (86)
   VARIABLE_PROPERTY_ACCESS (87)
   VARIABLE_PROPERTY_ACCESS (88)
   VARIABLE_PROPERTY_ACCESS (89)
   VARIABLE_PROPERTY_ACCESS (90)
   VARIABLE_PROPERTY_ACCESS (91)
   VARIABLE_PROPERTY_ACCESS (92)
   VARIABLE_PROPERTY_ACCESS (93)
   VARIABLE_PROPERTY_ACCESS (94)
   VARIABLE_PROPERTY_ACCESS (95)
   VARIABLE_PROPERTY_ACCESS (96)
   VARIABLE_PROPERTY_ACCESS (97)
   VARIABLE_PROPERTY_ACCESS (98)
   VARIABLE_PROPERTY_ACCESS (99)
#undef VARIABLE_PROPERTY_ACCESS

};

#endif                          // QE_SCALAR_HISTOGRAM_H