/*  QEPlotter.cpp
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

#include <QDebug>
#include <QCheckBox>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>

#include <QECommon.h>
#include <QEInteger.h>
#include <QEFloating.h>
#include <QEResizeableFrame.h>

#include <QEPlotter.h>

#define DEBUG qDebug() << "QEPlotter::" << __FUNCTION__ << ":" << __LINE__

#define elsif  else if

static const QColor clWhite (0xFF, 0xFF, 0xFF, 0xFF);
static const QColor clBlack (0x00, 0x00, 0x00, 0xFF);
static const QColor clGridLine (0xC0C0C0);

// Define default colours: essentially RGB byte triplets
//
static const QColor item_colours [1 + QEPlotter::NUMBER_OF_PLOTS] = {
   clWhite,
   QColor (0xFF0000), QColor (0x0000FF), QColor (0x008000), QColor (0xFF8000),
   QColor (0x4080FF), QColor (0x800000), QColor (0x008080), QColor (0x808000),
   QColor (0x800080), QColor (0x00FF00), QColor (0x00FFFF), QColor (0xFFFF00),
   QColor (0x8F00C0), QColor (0xC0008F), QColor (0xB040B0), clBlack
};

static const QString item_labels [1 + QEPlotter::NUMBER_OF_PLOTS] = {
   QString ("X"),
   QString ("A"), QString ("B"), QString ("C"), QString ("D"),
   QString ("E"), QString ("F"), QString ("G"), QString ("H"),
   QString ("I"), QString ("J"), QString ("K"), QString ("L"),
   QString ("M"), QString ("N"), QString ("O"), QString ("P")
};


static const QEExpressionEvaluation::InputKinds Normal = QEExpressionEvaluation::Normal;
static const QEExpressionEvaluation::InputKinds Primed = QEExpressionEvaluation::Primed;


//=================================================================================
// Tagged check box.
//=================================================================================
//
class TCheckBox : public QCheckBox {
public:
   explicit TCheckBox (QWidget* parent=0);
   explicit TCheckBox (const QString& text, QWidget* parent = 0);

   int tag;
};

//---------------------------------------------------------------------------------
//
TCheckBox::TCheckBox (QWidget* parent) : QCheckBox (parent)
{
   this->tag = 0;
}

//---------------------------------------------------------------------------------
//
TCheckBox::TCheckBox (const QString& text, QWidget* parent) : QCheckBox (text, parent)
{
   this->tag = 0;
}


//=================================================================================
// PrivateData
//=================================================================================
//
class QEPlotter::PrivateData {
public:
   // Constructor
   //
   PrivateData (QEPlotter* parent);

   QEPlotter* plotter;

   QVBoxLayout* vLayout;
   QHBoxLayout* hLayout;
   QVBoxLayout* pLayout;

   QEResizeableFrame* toolBarResize;
   QFrame* toolBarFrame;
   QFrame* theRestFrame;

   QFrame*  plotFrame;
   QwtPlot*  plotArea;
   QwtPlotGrid* plotGrid;

   QEResizeableFrame* itemResize;
   QFrame* itemFrame;

   QLabel* itemNames [1 + NUMBER_OF_PLOTS];
   TCheckBox* checkBoxes [1 + NUMBER_OF_PLOTS];
};

//---------------------------------------------------------------------------------
//
QEPlotter::PrivateData::PrivateData (QEPlotter* parent)
{
   int slot;
   int y;

   this->plotter = parent;

   // Main layout.
   //
   this->vLayout = new QVBoxLayout (parent);
   this->vLayout->setMargin (4);
   this->vLayout->setSpacing (4);

   this->toolBarResize = new QEResizeableFrame (QEResizeableFrame::BottomEdge, 4, 48, parent);
   this->toolBarResize->setFrameShape (QFrame::StyledPanel);
   this->toolBarResize->setFrameShadow (QFrame::Raised);
   this->toolBarResize->setFixedHeight (48);
   this->toolBarResize->setMinimumWidth (300);
   this->toolBarResize->setMaximumWidth (1920);
   this->vLayout->addWidget (this->toolBarResize);

   this->toolBarFrame = new QFrame (NULL);  // will be re-parented.
   this->toolBarFrame->setFrameShape (QFrame::NoFrame);
   this->toolBarFrame->setFrameShadow (QFrame::Plain);
   this->toolBarFrame->setFixedHeight (48);
   this->toolBarResize->setWidget (this->toolBarFrame);

   this->theRestFrame = new QFrame (parent);
   this->theRestFrame->setFrameShape (QFrame::NoFrame);
   this->theRestFrame->setFrameShadow (QFrame::Plain);
   this->vLayout->addWidget (this->theRestFrame);

   // Inside plot stuff frame - layout left to right.
   //
   this->hLayout = new QHBoxLayout (this->theRestFrame);
   this->hLayout->setMargin (0);
   this->hLayout->setSpacing (4);

   this->plotFrame = new QFrame (this->theRestFrame);
   this->plotFrame->setFrameShape (QFrame::StyledPanel);
   this->plotFrame->setFrameShadow (QFrame::Raised);
   this->hLayout->addWidget (this->plotFrame);

   // Inside plot frame - whole thing.
   //
   this->pLayout = new QVBoxLayout (this->plotFrame);
   this->pLayout->setMargin (4);
   this->pLayout->setSpacing (4);

   this->plotArea = new QwtPlot (this->plotFrame);
   this->plotArea->setCanvasLineWidth (1);
   this->plotArea->setLineWidth (1);

   this->plotGrid = new QwtPlotGrid ();
   this->plotGrid->attach (this->plotArea);

   this->pLayout->addWidget (this->plotArea);

   this->itemResize = new QEResizeableFrame (QEResizeableFrame::LeftEdge, 40, 400, this->theRestFrame);
   this->itemResize->setFrameShape (QFrame::StyledPanel);
   this->itemResize->setFrameShadow (QFrame::Raised);
   this->itemResize->setFixedWidth (256);
   this->hLayout->addWidget (this->itemResize);

   this->itemFrame = new QFrame (NULL); // re-parented.
   this->itemFrame->setFrameShape (QFrame::NoFrame);
   this->itemFrame->setFrameShadow (QFrame::Plain);
   this->itemResize->setWidget (this->itemFrame);

   for (slot = 0; slot < ARRAY_LENGTH (this->itemNames); slot++) {
      QLabel* label = new QLabel (this->itemFrame);
      TCheckBox* box = NULL;

      y = 4 + (22*slot) + (slot ? 4 : 0);

      label->setGeometry (4, y, 200, 17);
      label->setIndent (6);
      label->setStyleSheet (QEUtilities::colourToStyle (item_colours [slot]));

      if (slot != 0) {
         box = new TCheckBox (this->itemFrame);
         box->tag = slot;
         box->setGeometry (208, y, 17, 17);
         box->setChecked (true);
      }

      // Save widget references.
      //
      this->itemNames [slot] = label;
      this->checkBoxes [slot] = box;
   }
}


//=================================================================================
// DataSets
//=================================================================================
//
QEPlotter::DataSets::DataSets ()
{
   this->calculator = new QEExpressionEvaluation ();
   this->dataKind = NotInUse;
   this->sizeKind = NotSpecified;
   // this->colour = item_colours [slot];
   this->fixedSize = 0;
   this->dbSize = 0;
   this->pvName = "";
   this->aliasName = "";
   this->expression = "";
   this->dataIsConnected = false;
   this->sizeIsConnected = false;
   this->isDisplayed = true;
}

//---------------------------------------------------------------------------------
//
int QEPlotter::DataSets::effectiveSize ()
{
   int result = 0;
   switch (this->sizeKind) {
      case QEPlotter::NotSpecified:
         // Use number of data points.
         //
         result = this->data.size ();
         break;

      case QEPlotter::Constant:
         result = this->fixedSize;
         break;

      case QEPlotter::SizePVName:
         result = this->sizeIsConnected ? this->dbSize : 0;
         break;
   }

   return result;
}

//=================================================================================
//
//=================================================================================
//
QEPlotter::QEPlotter (QWidget* parent) : QEFrame (parent)
{
   QCaVariableNamePropertyManager* vpnm;
   int slot;

   this->privateData = new PrivateData (this);

   this->setNumVariables (2*ARRAY_LENGTH (this->xy));

   for (slot = 0; slot < ARRAY_LENGTH (this->xy); slot++) {
      this->xy [slot].colour = item_colours [slot];

      this->updateLabel (slot);

      // Set variable index numbers.
      // Must be consistent with the isDataIndex ()  etc. functions.
      //
      this->xy [slot].dataVariableNameManager.setVariableIndex (2*slot + 0);
      this->xy [slot].sizeVariableNameManager.setVariableIndex (2*slot + 1);

      if (this->privateData->checkBoxes [slot]) {
         QObject::connect (this->privateData->checkBoxes [slot], SIGNAL (stateChanged (int)),
                           this,                         SLOT   (checkBoxstateChanged (int)));

      }
   }

   // configure the panel and create contents
   //
   this->setFrameShape (QFrame::Panel);
   this->setFrameShadow (QFrame::Plain);
   this->setMinimumSize (500, 448);

   this->isReverse = false;

   this->setAllowDrop (false);
   this->setDisplayAlarmState (false);

   // Set up a connection to recieve variable name property changes
   //
   // The variable name property manager class only delivers an updated
   // variable name after the user has stopped typing.
   //
   for (slot = 0 ; slot < ARRAY_LENGTH (this->xy); slot++) {
      vpnm = &this->xy [slot].dataVariableNameManager;
      QObject::connect (vpnm, SIGNAL (newVariableNameProperty (QString, QString, unsigned int)),
                        this, SLOT   (setNewVariableName      (QString, QString, unsigned int)));

      vpnm = &this->xy [slot].sizeVariableNameManager;
      QObject::connect (vpnm, SIGNAL (newVariableNameProperty (QString, QString, unsigned int)),
                        this, SLOT   (setNewVariableName      (QString, QString, unsigned int)));

   }

   // Refresh plot check at ~10Hz.
   //
   this->timer = new QTimer (this);
   connect (this->timer, SIGNAL (timeout ()), this, SLOT (tickTimeout ()));
   this->timer->start (100);  // mSec == 0.1 s

   // Do an initial plot - this clears the refresh plot required flag.
   //
   this->plot ();
}

//---------------------------------------------------------------------------------
//
QEPlotter::~QEPlotter ()
{
   int slot;

   this->timer->stop ();
   this->releaseCurves ();

   for (slot = 0; slot < ARRAY_LENGTH (this->xy); slot++) {
      delete this->xy [slot].calculator;
   }

   // The widgets referenced by privateData are parented (indirectly) by
   // this so will be automagically deleted.
   //
   delete this->privateData;
}

//------------------------------------------------------------------------------
//
QSize QEPlotter::sizeHint () const {
   return QSize (600, 500);
}

//---------------------------------------------------------------------------------
// Slot range checking macro function.
// Set default to nil for void functions.
//
#define SLOT_CHECK(slot, default) {                           \
   if ((slot < 0) || (slot >= ARRAY_LENGTH (this->xy))) {     \
      DEBUG << "slot out of range: " << slot;                 \
      return default;                                         \
   }                                                          \
}

//---------------------------------------------------------------------------------
//
void QEPlotter::updateLabel (const int slot)
{
   SLOT_CHECK (slot,);

   DataSets* ds = &this->xy [slot];
   QLabel* t = this->privateData->itemNames [slot];
   QString text = item_labels [slot];

   text.append (": ");

   switch (ds->dataKind) {
      case NotInUse:
         break;

      case DataPlot:
         // If an alias name is defined - use it.
         //
         if (!ds->aliasName.isEmpty()) {
            text.append (ds->aliasName);
         } else {
            text.append (ds->pvName);
         }
         break;

      case CalculationPlot:
         text.append ("= ");
         text.append (ds->expression);
         break;

      case InvalidExpression:
         text.append ("invalid expr.");
         break;

   }

   t->setText (text);
}

//------------------------------------------------------------------------------
//
void QEPlotter::checkBoxstateChanged (int state)
{
   TCheckBox* box = dynamic_cast <TCheckBox*> (this->sender ());
   int slot;

   if (!box) return;
   slot = box->tag;
   SLOT_CHECK (slot,);
   this->xy [slot].isDisplayed = (state == Qt::Checked);
   this->replotIsRequired = true;
}

//---------------------------------------------------------------------------------
//
void QEPlotter::setNewVariableName (QString variableName,
                                    QString variableNameSubstitutions,
                                    unsigned int variableIndex)
{
   int slot = this->slotOf (variableIndex);
   QString pvName;

   SLOT_CHECK (slot,);

   this->setVariableNameAndSubstitutions (variableName, variableNameSubstitutions, variableIndex);
   pvName = this->getSubstitutedVariableName (variableIndex).trimmed ();

   // We need to test for null "PV names" here as the framework does not call
   // establishConnection in these cases.
   //
   if (this->isDataIndex (variableIndex)) {

      if (pvName.isEmpty()) {
         this->xy [slot].dataKind = NotInUse;
         this->replotIsRequired = true;
      }

      this->xy [slot].pvName = pvName;
      this->updateLabel (slot);

   } elsif (this->isSizeIndex (variableIndex)) {

      if (pvName.isEmpty()) {
         this->xy [slot].sizeKind = NotSpecified;
         this->replotIsRequired = true;
      }
   }
}

//---------------------------------------------------------------------------------
// Implementation of QEWidget's virtual funtion to create the specific type of
// QCaObject required. QCaObjects that streams doubles and integers are required.
//
qcaobject::QCaObject* QEPlotter::createQcaItem (unsigned int variableIndex)
{
   qcaobject::QCaObject* result = NULL;
   QString pvName;
   int size;
   bool okay;
   int slot;

   pvName = this->getSubstitutedVariableName (variableIndex).trimmed ();

   slot = this->slotOf (variableIndex);
   SLOT_CHECK (slot, NULL);

   if (this->isDataIndex (variableIndex)) {
      // Has designer/user defined a calculation (as opposed to a PV name)?.
      // Note: no valid PV name starts with =.
      //
      if (pvName.left (1).compare (QString ("=")) == 0) {

         this->xy[slot].expression = pvName.remove (0, 1);

         okay = this->xy[slot].calculator->initialise (this->xy[slot].expression);
         if (okay) {
            this->xy [slot].dataKind = CalculationPlot;
         } else {
            this->xy [slot].dataKind = InvalidExpression;
         }

      } else {
         this->xy [slot].dataKind = DataPlot;
         result = new QEFloating (pvName, this, &this->floatingFormatting, variableIndex);
      }

      this->replotIsRequired = true;
      this->updateLabel (slot);

   } elsif (this->isSizeIndex (variableIndex)) {

      // Has designer/user just set an integer (as opposed to a PV name)?.
      // Note: no sensible PV names are just integers.
      //
      size = pvName.toInt (&okay);
      if (okay) {
         this->xy [slot].sizeKind = Constant;
         this->xy[slot].fixedSize = size;
         this->xy[slot].dbSize = 0;
         this->replotIsRequired = true;

      } else {
         // Assume it is a PV.
         //
         this->xy [slot].sizeKind = SizePVName;
         result = new QEInteger (pvName, this, &this->integerFormatting, variableIndex);
         this->xy[slot].fixedSize = 0;
         this->xy[slot].dbSize = 0;
      }
   }

   return result;
}

//---------------------------------------------------------------------------------
// Start updating.
// Implementation of VariableNameManager's virtual funtion to establish a
// connection to a PV as the variable name has changed.
// This function may also be used to initiate updates when loaded as a plugin.
//
void QEPlotter::establishConnection (unsigned int variableIndex)
{
   // Create a connection.
   // If successfull, the QCaObject object that will supply data update signals will be returned
   // Note createConnection creates the connection and returns reference to existing QCaObject.
   //
   qcaobject::QCaObject*  qca = createConnection (variableIndex);

   if (!qca) {
      return;
   }

   if (this->isDataIndex (variableIndex)) {
      QObject::connect (qca, SIGNAL (floatingConnectionChanged (QCaConnectionInfo &, const unsigned int &)),
                        this, SLOT  (dataConnectionChanged     (QCaConnectionInfo &, const unsigned int &)));

      QObject::connect (qca, SIGNAL (floatingArrayChanged (const QVector<double>&, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)),
                        this, SLOT  (dataArrayChanged     (const QVector<double>&, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)));

   } elsif (this->isSizeIndex (variableIndex)) {
      QObject::connect (qca, SIGNAL (integerConnectionChanged  (QCaConnectionInfo &, const unsigned int &)),
                        this, SLOT  (sizeConnectionChanged     (QCaConnectionInfo &, const unsigned int &)));

      QObject::connect (qca, SIGNAL (integerChanged   (const long &, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)),
                        this, SLOT  (sizeValueChanged (const long &, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)));
   }
}


//---------------------------------------------------------------------------------
//
void QEPlotter::dataConnectionChanged (QCaConnectionInfo& connectionInfo,
                                       const unsigned int &variableIndex)
{
   const int slot = this->slotOf (variableIndex);

   SLOT_CHECK (slot,);
   this->xy [slot].dataIsConnected = connectionInfo.isChannelConnected ();
   this->replotIsRequired = true;
}

//---------------------------------------------------------------------------------
//
void QEPlotter::dataArrayChanged (const QVector<double>& values,
                                  QCaAlarmInfo&,
                                  QCaDateTime&,
                                  const unsigned int& variableIndex)
{
   const int slot = this->slotOf (variableIndex);

   SLOT_CHECK (slot,);
   this->xy [slot].data = QEFloatingArray (values);
   this->replotIsRequired = true;
}

//---------------------------------------------------------------------------------
//
void QEPlotter::sizeConnectionChanged (QCaConnectionInfo& connectionInfo,
                                       const unsigned int &variableIndex)
{
   const int slot = this->slotOf (variableIndex);

   SLOT_CHECK (slot,);
   this->xy [slot].sizeIsConnected = connectionInfo.isChannelConnected ();
   this->replotIsRequired = true;
}

//---------------------------------------------------------------------------------
//
void QEPlotter::sizeValueChanged (const long& value,
                                  QCaAlarmInfo&,
                                  QCaDateTime&,
                                  const unsigned int& variableIndex)
{
   const int slot = this->slotOf (variableIndex);

   SLOT_CHECK (slot,);
   this->xy [slot].dbSize = value;
   this->replotIsRequired = true;
}


//------------------------------------------------------------------------------
//
QwtPlotCurve* QEPlotter::allocateCurve (const int slot)
{
   QwtPlotCurve* result = NULL;
   QPen pen;

   SLOT_CHECK (slot, NULL);

   result = new QwtPlotCurve ();

   // Attach to the plot area and save a reference.
   //
   result->attach (this->privateData->plotArea);
   this->curve_list.append (result);

   // Set curve propeties plus item Pen which include its colour.
   //
   result->setRenderHint (QwtPlotItem::RenderAntialiased);
   result->setStyle (QwtPlotCurve::Lines);

   pen.setColor (this->xy [slot].colour);
   pen.setWidth (1);
   result->setPen (pen);

   return result;
}

//---------------------------------------------------------------------------------
//
void QEPlotter::releaseCurves ()
{
   int j;
   QwtPlotCurve *curve;

   for (j = 0; j < this->curve_list.count(); j++) {
      curve = this->curve_list.value (j);
      curve->detach ();
      delete curve;
   }

   this->curve_list.clear ();
}

//---------------------------------------------------------------------------------
//
void QEPlotter::plot ()
{
   QPen pen;
   int slot;
   QwtPlotCurve *curve;
   DataSets* xs;
   DataSets* ys;
   QEFloatingArray xdata;
   QEFloatingArray ydata;
   int effectiveXSize;
   int effectiveYSize;
   int number;
   double xMin, xMax, xMajor;
   double yMin, yMax, yMajor;

   // Prequel
   //
   this->doAnyCalculations ();

   // First release any/all previosly allocated curves.
   //
   this->releaseCurves ();

   // Set up brackground and grid.
   //
#if QWT_VERSION >= 0x060000
   this->privateData->plotArea->setCanvasBackground (QBrush (this->xy [0].colour));
#else
   this->privateData->plotArea->setCanvasBackground (this->xy [0].colour);
#endif

   pen.setColor (clGridLine);
   pen.setStyle (Qt::DashLine);
   this->privateData->plotGrid->setPen (pen);

   xMin = 0.0;
   xMax = 1.0;
   yMin = 0.0;
   yMax = 1.0;

   xs = &this->xy [0];
   effectiveXSize = xs->effectiveSize ();

   for (slot = 1; slot < ARRAY_LENGTH (this->xy); slot++) {
      ys = &this->xy [slot];

      // Only display if requested.
      //
      if (!ys->isDisplayed) {
         continue;
      }

      // Ignore not in use and invalid expression items.
      //
      if ((ys->dataKind != DataPlot) && (ys->dataKind != CalculationPlot)) {
         continue;
      }

      effectiveYSize = ys->effectiveSize ();

      // Calculate actual number of points to plot.
      // Skip if none or only a single point.
      //
      number = MIN (effectiveXSize, effectiveYSize);

      if (number < 2)  {
         continue;
      }

      curve = this->allocateCurve (slot);
      if (!curve) {
         DEBUG << "Unable to allocate curve";
         continue;
      }

      // Truncate both data sets to the same length.
      //
      xdata = QEFloatingArray (xs->data.mid (0, number));
      ydata = QEFloatingArray (ys->data.mid (0, number));

#if QWT_VERSION >= 0x060000
      curve->setSamples (xdata, ydata);
#else
      curve->setData (xdata, ydata);
#endif

      xMin = MIN (xMin, xdata.minimumValue ());
      xMax = MAX (xMax, xdata.maximumValue ());
      yMin = MIN (yMin, ydata.minimumValue ());
      yMax = MAX (yMax, ydata.maximumValue ());
   }

   QEPlotter::adjustMinMax (xMin, xMax, xMin, xMax, xMajor);
   QEPlotter::adjustMinMax (yMin, yMax, yMin, yMax, yMajor);

   this->privateData->plotArea->setAxisScale (QwtPlot::xBottom, xMin, xMax, xMajor);
   this->privateData->plotArea->setAxisScale (QwtPlot::yLeft,   yMin, yMax, yMajor);

   this->privateData->plotArea->replot ();

   // Ensure next timer tick only invokes plot if needs be.
   //
   this->replotIsRequired = false;
}

//---------------------------------------------------------------------------------
//
void QEPlotter::doAnyCalculations ()
{
   const int x = QEExpressionEvaluation::indexOf ('X');
   const int s = QEExpressionEvaluation::indexOf ('S');

   QEExpressionEvaluation::CalculateArguments userArguments;
   DataSets* xs;
   DataSets* ys;
   int effectiveXSize;
   int effectiveYSize;
   int n;
   int j;
   int slot;
   int tols;
   double value;
   bool okay;


   xs = &this->xy [0];

   switch (xs->dataKind) {
      case NotInUse:
         xs->data.clear ();
         if (xs->sizeKind == Constant) {
            // Use default calculation.
            //
            n = xs->fixedSize;
            for (j = 0; j < n; j++) {
               xs->data.append ((double) j);
            }

         }
         break;

      case DataPlot:
         // Leave as the data, if any, supplied via PV.
         break;

      case CalculationPlot:
         xs->data.clear ();
         n = xs->effectiveSize ();
         for (j = 0; j < n; j++) {
            QEExpressionEvaluation::clear (userArguments);
            userArguments [Normal][s] = (double) j;
            value = xs->calculator->evaluate (userArguments, &okay);
            xs->data.append (value);
         }
         break;

      case InvalidExpression:
         xs->data.clear ();
         break;
   }

   effectiveXSize = xs->effectiveSize ();

   for (slot = 1; slot < ARRAY_LENGTH (this->xy); slot++) {
      ys = &this->xy [slot];
      if (ys->dataKind == DataPlot) {
         ys->dyByDx = ys->data.calcDyByDx (xs->data);
      }
   }

   for (slot = 1; slot < ARRAY_LENGTH (this->xy); slot++) {
      ys = &this->xy [slot];
      if (ys->dataKind == CalculationPlot) {

         ys->data.clear ();
         effectiveYSize = ys->effectiveSize ();

         // We ARE doing a calculation and size not specified, so set to X size.
         if (ys->sizeKind == NotSpecified) {
            effectiveYSize = effectiveXSize;
         }

         n = MIN (effectiveXSize, effectiveYSize);

         for (j = 0; j < n; j++) {
            QEExpressionEvaluation::clear (userArguments);

            // Pre-defined values: S and Z
            userArguments [Normal][s] = (double) j;
            userArguments [Normal][x] = xs->data.value (j);
            userArguments [Primed][x] = 1.0;    // by defitions.

            for (tols = 1; tols < slot; tols++) {
               userArguments [Normal] [tols - 1] = this->xy [tols].data.value (j, 0.0);
               userArguments [Primed] [tols - 1] = this->xy [tols].dyByDx.value (j, 0.0);
            }

            value = ys->calculator->evaluate (userArguments, &okay);
            ys->data.append (value);
         }

         ys->dyByDx = ys->data.calcDyByDx (xs->data);
      }
   }
}

//---------------------------------------------------------------------------------
//
void QEPlotter::tickTimeout ()
{
   if (this->replotIsRequired) {
      this->plot ();
   }
}


//---------------------------------------------------------------------------------
// Property functions.
//---------------------------------------------------------------------------------
//
void QEPlotter::setVariableSubstitutions (QString defaultSubstitutions)
{
   int slot;

   // Use same default subsitutions for all PVs used by this widget.
   //
   for (slot = 0 ; slot < ARRAY_LENGTH (this->xy); slot++) {
      this->xy [slot].dataVariableNameManager.setSubstitutionsProperty (defaultSubstitutions);
      this->xy [slot].sizeVariableNameManager.setSubstitutionsProperty (defaultSubstitutions);
   }
}

//---------------------------------------------------------------------------------
//
QString QEPlotter::getVariableSubstitutions ()
{
   // Any one of these name managers can provide the subsitutions.
   //
   return this->xy [0].dataVariableNameManager.getSubstitutionsProperty ();
}

//---------------------------------------------------------------------------------
//
void QEPlotter::setXYDataPV (const int slot, const QString& pvName)
{
   SLOT_CHECK (slot,);
   this->xy [slot].dataVariableNameManager.setVariableNameProperty (pvName);
}

//---------------------------------------------------------------------------------
//
QString QEPlotter::getXYDataPV (const int slot)
{
   SLOT_CHECK (slot, "");
   return this->xy [slot].dataVariableNameManager.getVariableNameProperty ();
}

//---------------------------------------------------------------------------------
//
void QEPlotter::setXYSizePV (const int slot, const QString& pvName)
{
   SLOT_CHECK (slot,);
   this->xy [slot].sizeVariableNameManager.setVariableNameProperty (pvName);
}

//---------------------------------------------------------------------------------
//
QString QEPlotter::getXYSizePV (const int slot)
{
   SLOT_CHECK (slot, "");
   return this->xy [slot].sizeVariableNameManager.getVariableNameProperty ();
}

//---------------------------------------------------------------------------------
//
void QEPlotter::setXYAlias (const int slot, const QString& aliasName)
{
   SLOT_CHECK (slot,);
   this->xy[slot].aliasName = aliasName;
   this->updateLabel (slot);
}

//---------------------------------------------------------------------------------
//
QString QEPlotter::getXYAlias (const int slot)
{
   SLOT_CHECK (slot, "");
   return this->xy[slot].aliasName;
}

//---------------------------------------------------------------------------------
//
void QEPlotter::setXYColour (const int slot, const QColor& colour)
{
   SLOT_CHECK (slot,);

   // Slot 0 (X) and last slot (P) have fixed colours.
   //
   if (slot != 0 && slot != ARRAY_LENGTH (this->xy) - 1) {
      QLabel* t = this->privateData->itemNames [slot];
      this->xy[slot].colour = colour;
      t->setStyleSheet (QEUtilities::colourToStyle (colour));
   }
}

//---------------------------------------------------------------------------------
//
QColor QEPlotter::getXYColour (const int slot)
{
   SLOT_CHECK (slot, QColor (0,0,0,0));
   return this->xy[slot].colour;
}

//---------------------------------------------------------------------------------
//
void  QEPlotter::adjustMinMax (const double minIn, const double maxIn,
                               double& minOut, double& maxOut, double& majorOut)
{
   static const double majorValues [] = {
      1.0e-3,  2.0e-3,  5.0e-3,    1.0e-2,  2.0e-2,  5.0e-2,    1.0e-1,  2.0e-1,  5.0e-1,
      1.0e+0,  2.0e+0,  5.0e+0,    1.0e+1,  2.0e+1,  5.0e+1,    1.0e+2,  2.0e+2,  5.0e+2,
      1.0e+3,  2.0e+3,  5.0e+3,    1.0e+4,  2.0e+4,  5.0e+4,    1.0e+5,  2.0e+5,  5.0e+5,
      1.0e+6,  2.0e+6,  5.0e+6,    1.0e+7,  2.0e+7,  5.0e+7,    1.0e+8,  2.0e+8,  5.0e+8,
      1.0e+9,  2.0e+9,  5.0e+9,    1.0e+10, 2.0e+10, 5.0e+10,   1.0e+11, 2.0e+11, 5.0e+11,
      1.0e+12, 2.0e+12, 5.0e+12
   };

   double major;
   int s;
   int p, q;

   // Find estimated major value.
   //
   major = (maxIn - minIn) / 12;

   // Round up major to next standard value.
   //
   s = 0;
   while ((major > majorValues [s]) && ((s + 1) < ARRAY_LENGTH (majorValues))) s++;
   majorOut = major = majorValues [s];

   p = (int) (minIn / major);
   if ((p * major) > minIn) p--;

   q = (int) (maxIn / major);
   if ((q * major) < maxIn) q++;

   q = MAX (q, p+1);   // Ensure q > p

   minOut = p * major;
   maxOut = q * major;
}

// end
