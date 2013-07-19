/*  QEPlotter.h
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

#ifndef QEPLOTTER_H
#define QEPLOTTER_H

#include <QColor>
#include <QObject>
#include <QTimer>
#include <QVector>
#include <QWidget>

#include <QEFloatingArray.h>
#include <QEFloatingFormatting.h>
#include <QEIntegerFormatting.h>
#include <QEFrame.h>
#include <QEExpressionEvaluation.h>
#include <QCaVariableNamePropertyManager.h>

class QwtPlotCurve;

class QEPLUGINLIBRARYSHARED_EXPORT QEPlotter : public QEFrame {
   Q_OBJECT

   /// Default macro substitutions. The default is no substitutions.
   /// The format is NAME1=VALUE1[,] NAME2=VALUE2...
   /// Values may be quoted strings. For example, 'SAMPLE=SAM1, NAME = "Ref foil"'
   /// These substitutions are applied to all the variable names.
   //
   Q_PROPERTY (QString variableSubstitutions READ getVariableSubstitutions WRITE setVariableSubstitutions)

   // Data and Size properties,
   //
   Q_PROPERTY (QString DataVariableX   READ getDataPVX  WRITE setDataPVX)
   Q_PROPERTY (QString DataVariableA   READ getDataPVA  WRITE setDataPVA)
   Q_PROPERTY (QString DataVariableB   READ getDataPVB  WRITE setDataPVB)
   Q_PROPERTY (QString DataVariableC   READ getDataPVC  WRITE setDataPVC)
   Q_PROPERTY (QString DataVariableD   READ getDataPVD  WRITE setDataPVD)
   Q_PROPERTY (QString DataVariableE   READ getDataPVE  WRITE setDataPVE)
   Q_PROPERTY (QString DataVariableF   READ getDataPVF  WRITE setDataPVF)
   Q_PROPERTY (QString DataVariableG   READ getDataPVG  WRITE setDataPVG)
   Q_PROPERTY (QString DataVariableH   READ getDataPVH  WRITE setDataPVH)
   Q_PROPERTY (QString DataVariableI   READ getDataPVI  WRITE setDataPVI)
   Q_PROPERTY (QString DataVariableJ   READ getDataPVJ  WRITE setDataPVJ)
   Q_PROPERTY (QString DataVariableK   READ getDataPVK  WRITE setDataPVK)
   Q_PROPERTY (QString DataVariableL   READ getDataPVL  WRITE setDataPVL)
   Q_PROPERTY (QString DataVariableM   READ getDataPVM  WRITE setDataPVM)
   Q_PROPERTY (QString DataVariableN   READ getDataPVN  WRITE setDataPVN)
   Q_PROPERTY (QString DataVariableO   READ getDataPVO  WRITE setDataPVO)
   Q_PROPERTY (QString DataVariableP   READ getDataPVP  WRITE setDataPVP)

   Q_PROPERTY (QString SizeVariableX   READ getSizePVX  WRITE setSizePVX)
   Q_PROPERTY (QString SizeVariableA   READ getSizePVA  WRITE setSizePVA)
   Q_PROPERTY (QString SizeVariableB   READ getSizePVB  WRITE setSizePVB)
   Q_PROPERTY (QString SizeVariableC   READ getSizePVC  WRITE setSizePVC)
   Q_PROPERTY (QString SizeVariableD   READ getSizePVD  WRITE setSizePVD)
   Q_PROPERTY (QString SizeVariableE   READ getSizePVE  WRITE setSizePVE)
   Q_PROPERTY (QString SizeVariableF   READ getSizePVF  WRITE setSizePVF)
   Q_PROPERTY (QString SizeVariableG   READ getSizePVG  WRITE setSizePVG)
   Q_PROPERTY (QString SizeVariableH   READ getSizePVH  WRITE setSizePVH)
   Q_PROPERTY (QString SizeVariableI   READ getSizePVI  WRITE setSizePVI)
   Q_PROPERTY (QString SizeVariableJ   READ getSizePVJ  WRITE setSizePVJ)
   Q_PROPERTY (QString SizeVariableK   READ getSizePVK  WRITE setSizePVK)
   Q_PROPERTY (QString SizeVariableL   READ getSizePVL  WRITE setSizePVL)
   Q_PROPERTY (QString SizeVariableM   READ getSizePVM  WRITE setSizePVM)
   Q_PROPERTY (QString SizeVariableN   READ getSizePVN  WRITE setSizePVN)
   Q_PROPERTY (QString SizeVariableO   READ getSizePVO  WRITE setSizePVO)
   Q_PROPERTY (QString SizeVariableP   READ getSizePVP  WRITE setSizePVP)

   Q_PROPERTY (QString AliasNameX      READ getAliasX   WRITE setAliasX)
   Q_PROPERTY (QString AliasNameA      READ getAliasA   WRITE setAliasA)
   Q_PROPERTY (QString AliasNameB      READ getAliasB   WRITE setAliasB)
   Q_PROPERTY (QString AliasNameC      READ getAliasC   WRITE setAliasC)
   Q_PROPERTY (QString AliasNameD      READ getAliasD   WRITE setAliasD)
   Q_PROPERTY (QString AliasNameE      READ getAliasE   WRITE setAliasE)
   Q_PROPERTY (QString AliasNameF      READ getAliasF   WRITE setAliasF)
   Q_PROPERTY (QString AliasNameG      READ getAliasG   WRITE setAliasG)
   Q_PROPERTY (QString AliasNameH      READ getAliasH   WRITE setAliasH)
   Q_PROPERTY (QString AliasNameI      READ getAliasI   WRITE setAliasI)
   Q_PROPERTY (QString AliasNameJ      READ getAliasJ   WRITE setAliasJ)
   Q_PROPERTY (QString AliasNameK      READ getAliasK   WRITE setAliasK)
   Q_PROPERTY (QString AliasNameL      READ getAliasL   WRITE setAliasL)
   Q_PROPERTY (QString AliasNameM      READ getAliasM   WRITE setAliasM)
   Q_PROPERTY (QString AliasNameN      READ getAliasN   WRITE setAliasN)
   Q_PROPERTY (QString AliasNameO      READ getAliasO   WRITE setAliasO)
   Q_PROPERTY (QString AliasNameP      READ getAliasP   WRITE setAliasP)

   // There is no X colour.
   Q_PROPERTY (QColor  ColourA         READ getColourA  WRITE setColourA)
   Q_PROPERTY (QColor  ColourB         READ getColourB  WRITE setColourB)
   Q_PROPERTY (QColor  ColourC         READ getColourC  WRITE setColourC)
   Q_PROPERTY (QColor  ColourD         READ getColourD  WRITE setColourD)
   Q_PROPERTY (QColor  ColourE         READ getColourE  WRITE setColourE)
   Q_PROPERTY (QColor  ColourF         READ getColourF  WRITE setColourF)
   Q_PROPERTY (QColor  ColourG         READ getColourG  WRITE setColourG)
   Q_PROPERTY (QColor  ColourH         READ getColourH  WRITE setColourH)
   Q_PROPERTY (QColor  ColourI         READ getColourI  WRITE setColourI)
   Q_PROPERTY (QColor  ColourJ         READ getColourJ  WRITE setColourJ)
   Q_PROPERTY (QColor  ColourK         READ getColourK  WRITE setColourK)
   Q_PROPERTY (QColor  ColourL         READ getColourL  WRITE setColourL)
   Q_PROPERTY (QColor  ColourM         READ getColourM  WRITE setColourM)
   Q_PROPERTY (QColor  ColourN         READ getColourN  WRITE setColourN)
   Q_PROPERTY (QColor  ColourO         READ getColourO  WRITE setColourO)
   // There is no P colour. It is fixed at black (or white for reverse video).

public:
   explicit QEPlotter (QWidget *parent = 0);
   ~QEPlotter();
   QSize sizeHint () const;

   static const int NUMBER_OF_PLOTS = 16;

   // Single function for all 'Data Set' properties.
   // Make public? make slots?
   //
   void    setXYDataPV (const int, const QString&);
   QString getXYDataPV (const int);

   void    setXYSizePV (const int, const QString&);
   QString getXYSizePV (const int);

   void    setXYAlias (const int, const QString&);
   QString getXYAlias (const int);

   void   setXYColour (const int, const QColor&);
   QColor getXYColour (const int);

protected:
   // Implementation of QEWidget's virtual funtions
   //
   qcaobject::QCaObject* createQcaItem (unsigned int variableIndex);
   void establishConnection (unsigned int variableIndex);

private:
   // Internal widgets and associated support data. These are declared in
   // PrivateData. If these items are declared at class level, there is a run
   // time exception. PrivateData also allows, what is essentially private,
   // to be actually private, well at least declared in QEStripChart.cpp
   //
   class PrivateData;
   PrivateData *privateData;

   // Keep a list of allocated curves so that we can track and delete them.
   //
   typedef QList<QwtPlotCurve*> QwtCurveList;
   QwtCurveList curve_list;

   bool isReverse;
   QTimer* timer;
   QEIntegerFormatting  integerFormatting;
   QEFloatingFormatting floatingFormatting;
   bool replotIsRequired;

   enum DataPlotKinds { NotInUse,            // blank  - not in use - no data - no plot
                        DataPlot,            // use specified PV to provide plot data
                        CalculationPlot,     // "= ..." - use given calculation for plot data
                        InvalidExpression }; // "= ..." - is invalid  - no data - no plot.

   enum SizePlotKinds { NotSpecified,        // blank - use maximum, available no. points
                        Constant,            // "[0-9]*" - used fixed integer as number of points
                        SizePVName };        // use speficed PV to provide number of points.


   class DataSets {
   public:
      DataSets ();

      QCaVariableNamePropertyManager dataVariableNameManager;
      QCaVariableNamePropertyManager sizeVariableNameManager;
      DataPlotKinds dataKind;
      SizePlotKinds sizeKind;
      QString pvName;
      QString aliasName;
      QString expression;
      QEExpressionEvaluation* calculator;
      bool dataIsConnected;
      bool sizeIsConnected;
      int fixedSize;     // size set by user/designer
      int dbSize;        // max. number of elements of data to process.
      QEFloatingArray data;
      QEFloatingArray dyByDx;

      // n/a for the X data set, Y data sets only.
      //
      QColor colour;
      bool isDisplayed;

      int effectiveSize ();
   };

   // Slot 0 used for X data - some redundancy (e.g. colour)
   //
   DataSets xy [1 + NUMBER_OF_PLOTS];

   QwtPlotCurve* allocateCurve (const int slot);
   void releaseCurves ();
   void plot ();
   void doAnyCalculations ();

   // Property READ WRITE functions.
   //
   void    setVariableSubstitutions (QString variableNameSubstitutions);
   QString getVariableSubstitutions ();

   // Property access READ and WRITE functions.
   // We can define the access functions using a macro.
   // Alas, due to SDK limitation, we cannot embedded the property definition in a macro.
   //
   #define PROPERTY_ACCESS(letter, slot)                                                 \
      void    setDataPV##letter (QString name) { this->setXYDataPV (slot, name); }       \
      QString getDataPV##letter ()      { return this->getXYDataPV (slot); }             \
                                                                                         \
      void    setSizePV##letter (QString name) { this->setXYSizePV (slot, name); }       \
      QString getSizePV##letter ()      { return this->getXYSizePV (slot); }             \
                                                                                         \
      void    setAlias##letter  (QString name) { this->setXYAlias (slot, name); }        \
      QString getAlias##letter  ()      { return this->getXYAlias (slot); }              \
                                                                                         \
      void    setColour##letter (QColor colour) { this->setXYColour (slot, colour); }    \
      QColor  getColour##letter ()      { return  this->getXYColour (slot); }


   PROPERTY_ACCESS  (X, 0)
   PROPERTY_ACCESS  (A, 1)
   PROPERTY_ACCESS  (B, 2)
   PROPERTY_ACCESS  (C, 3)
   PROPERTY_ACCESS  (D, 4)
   PROPERTY_ACCESS  (E, 5)
   PROPERTY_ACCESS  (F, 6)
   PROPERTY_ACCESS  (G, 7)
   PROPERTY_ACCESS  (H, 8)
   PROPERTY_ACCESS  (I, 9)
   PROPERTY_ACCESS  (J, 10)
   PROPERTY_ACCESS  (K, 11)
   PROPERTY_ACCESS  (L, 12)
   PROPERTY_ACCESS  (M, 13)
   PROPERTY_ACCESS  (N, 14)
   PROPERTY_ACCESS  (O, 15)
   PROPERTY_ACCESS  (P, 16)

   #undef PROPERTY_ACCESS

   // Provides consistant interpretation of variableIndex.
   // Must be consistent with variableIndex allocation in the contructor.
   //
   bool isDataIndex (const unsigned int vi) { return (vi % 2) == 0; }
   bool isSizeIndex (const unsigned int vi) { return !isDataIndex (vi); }
   bool isXIndex    (const unsigned int vi) { return (vi < 2); }
   bool isYIndex    (const unsigned int vi) { return !isXIndex (vi); }
   int  slotOf      (const unsigned int vi) { return (vi / 2); }

   void updateLabel (const int slot);

   // Move to a utility class?
   //
   static void adjustMinMax (const double minIn, const double maxIn,
                             double& minOut, double& maxOut, double& majorOut);

private slots:
   void setNewVariableName (QString variableName,
                            QString variableNameSubstitutions,
                            unsigned int variableIndex);

   void dataConnectionChanged (QCaConnectionInfo& connectionInfo,
                               const unsigned int& variableIndex);

   void dataArrayChanged (const QVector<double>& values,
                          QCaAlarmInfo& alarmInfo,
                          QCaDateTime& timeStamp,
                          const unsigned int& variableIndex);

   void sizeConnectionChanged (QCaConnectionInfo& connectionInfo,
                               const unsigned int& variableIndex);

   void sizeValueChanged (const long& value,
                          QCaAlarmInfo& alarmInfo,
                          QCaDateTime& timeStamp,
                          const unsigned int& variableIndex);

   void checkBoxstateChanged (int state);
   void tickTimeout ();

   friend class PrivateData;
   friend class DataSets;
   friend class DoubleVectors;
};

#endif // QEPLOTTER_H
