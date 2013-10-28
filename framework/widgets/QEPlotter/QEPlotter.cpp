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

#include "QEPlotter.h"

#define DEBUG qDebug() << "QEPlotter::" << __FUNCTION__ << ":" << __LINE__

static const QColor clWhite (0xFF, 0xFF, 0xFF, 0xFF);
static const QColor clBlack (0x00, 0x00, 0x00, 0xFF);
static const QColor clGridLine (0xC0C0C0);
static const QColor clStatus (0xF0F0F0);

// Define default colours: essentially RGB byte triplets
//
static const QColor item_colours [1 + NUMBER_OF_PLOTS] = {
   clWhite,
   QColor (0xFF0000), QColor (0x0000FF), QColor (0x008000), QColor (0xFF8000),
   QColor (0x4080FF), QColor (0x800000), QColor (0x008080), QColor (0x808000),
   QColor (0x800080), QColor (0x00FF00), QColor (0x00FFFF), QColor (0xFFFF00),
   QColor (0x8F00C0), QColor (0xC0008F), QColor (0xB040B0), clBlack
};

static const QString item_labels [1 + NUMBER_OF_PLOTS] = {
   QString ("X"),
   QString ("A"), QString ("B"), QString ("C"), QString ("D"),
   QString ("E"), QString ("F"), QString ("G"), QString ("H"),
   QString ("I"), QString ("J"), QString ("K"), QString ("L"),
   QString ("M"), QString ("N"), QString ("O"), QString ("P")
};


static const QEExpressionEvaluation::InputKinds Normal = QEExpressionEvaluation::Normal;
static const QEExpressionEvaluation::InputKinds Primed = QEExpressionEvaluation::Primed;


//==============================================================================
// Local support classes.
//==============================================================================
// Tagged check box.
//
class TCheckBox : public QCheckBox {
public:
   explicit TCheckBox (QWidget* parent=0);
   explicit TCheckBox (const QString& text, QWidget* parent = 0);

   int tag;
};

//------------------------------------------------------------------------------
//
TCheckBox::TCheckBox (QWidget* parent) : QCheckBox (parent)
{
   this->tag = 0;
}

//------------------------------------------------------------------------------
//
TCheckBox::TCheckBox (const QString& text, QWidget* parent) : QCheckBox (text, parent)
{
   this->tag = 0;
}


//==============================================================================
// QEPlotter
//==============================================================================
//
void QEPlotter::createInternalWidgets ()
{
   int slot;
   int y;
   QString styleSheet;

   // Main layout.
   //
   this->vLayout = new QVBoxLayout (this);
   this->vLayout->setMargin (4);
   this->vLayout->setSpacing (4);

   this->toolBarResize = new QEResizeableFrame (QEResizeableFrame::BottomEdge, 4, 48, this);
   this->toolBarResize->setFrameShape (QFrame::StyledPanel);
   this->toolBarResize->setFrameShadow (QFrame::Raised);
   this->toolBarResize->setFixedHeight (48);
   this->toolBarResize->setMinimumWidth (300);
   this->toolBarResize->setMaximumWidth (1920);
   this->vLayout->addWidget (this->toolBarResize);

   this->toolBarFrame = new QFrame (NULL);  // will be re-parented.
   this->toolBarFrame->setFrameShape (QFrame::NoFrame);
   this->toolBarFrame->setFrameShadow (QFrame::Plain);
   this->toolBarFrame->setFixedHeight (40);
   this->toolBarResize->setWidget (this->toolBarFrame);

   this->theMainFrame = new QFrame (this);
   this->theMainFrame->setFrameShape (QFrame::NoFrame);
   this->theMainFrame->setFrameShadow (QFrame::Plain);
   this->vLayout->addWidget (this->theMainFrame);

   this->statusFrame = new QFrame (this);
   this->statusFrame->setFrameShape (QFrame::StyledPanel);
   this->statusFrame->setFrameShadow (QFrame::Raised);
   this->statusFrame->setFixedHeight (30);
   this->vLayout->addWidget (this->statusFrame);

   // Inside main frame - layout left to right.
   //
   this->hLayout = new QHBoxLayout (this->theMainFrame);
   this->hLayout->setMargin (0);
   this->hLayout->setSpacing (4);

   this->plotFrame = new QFrame (this->theMainFrame);
   this->plotFrame->setFrameShape (QFrame::StyledPanel);
   this->plotFrame->setFrameShadow (QFrame::Raised);
   this->hLayout->addWidget (this->plotFrame);

   // Inside plot frame - whole thing.
   //
   this->pLayout = new QVBoxLayout (this->plotFrame);
   this->pLayout->setMargin (4);
   this->pLayout->setSpacing (4);

   this->plotArea = new QwtPlot (this->plotFrame);
#if QWT_VERSION < 0x060100
   this->plotArea->setCanvasLineWidth (1);
#endif
   this->plotArea->setLineWidth (1);

   this->plotArea->canvas()->setMouseTracking (true);
   this->plotArea->canvas()->installEventFilter (this);

   this->plotGrid = new QwtPlotGrid ();
   this->plotGrid->attach (this->plotArea);

   this->pLayout->addWidget (this->plotArea);

   this->itemResize = new QEResizeableFrame (QEResizeableFrame::LeftEdge, 40, 400, this->theMainFrame);
   this->itemResize->setFrameShape (QFrame::StyledPanel);
   this->itemResize->setFrameShadow (QFrame::Raised);
   this->itemResize->setFixedWidth (256);
   this->hLayout->addWidget (this->itemResize);

   this->itemFrame = new QFrame (NULL); // re-parented.
   this->itemFrame->setFrameShape (QFrame::NoFrame);
   this->itemFrame->setFrameShadow (QFrame::Plain);
   this->itemResize->setWidget (this->itemFrame);

   for (slot = 0; slot < ARRAY_LENGTH (this->xy); slot++) {
      QLabel* label = new QLabel (this->itemFrame);
      QEPlotterMenu* menu = new QEPlotterMenu (slot, this);
      TCheckBox* box = NULL;

      y = 4 + (22*slot) + (slot ? 4 : 0);

      label->setGeometry (4, y, 200, 17);
      label->setIndent (6);
      label->setStyleSheet (QEUtilities::colourToStyle (item_colours [slot]));

      label->setAcceptDrops (true);
      label->installEventFilter (this);
      label->setContextMenuPolicy (Qt::CustomContextMenu);

      QObject::connect (label, SIGNAL ( customContextMenuRequested (const QPoint &)),
                        this,  SLOT   ( contextMenuRequested (const QPoint &)));

      QObject::connect (menu, SIGNAL ( contextMenuSelected (const int, const QEPlotterMenu::ContextMenuOptions) ),
                        this,  SLOT  ( contextMenuSelected (const int, const QEPlotterMenu::ContextMenuOptions) ));

      if (slot != 0) {
         box = new TCheckBox (this->itemFrame);
         box->tag = slot;
         box->setGeometry (208, y, 17, 17);
         box->setChecked (true);

         QObject::connect (box,  SIGNAL (stateChanged (int)),
                           this, SLOT   (checkBoxstateChanged (int)));
      }

      // Save widget references.
      //
      this->xy [slot].itemName = label;
      this->xy [slot].checkBox = box;
      this->xy [slot].itemMenu = menu;
   }

   // Status frame.
   //
   // Inside status frame - layout left to right.
   //
   this->sLayout = new QHBoxLayout (this->statusFrame);
   this->sLayout->setMargin (4);
   this->sLayout->setSpacing (8);

   this->slotIndicator = new QLabel ("", this->statusFrame);
   this->minLabel   = new QLabel ("Min:", this->statusFrame);
   this->minValue   = new QLabel ("-", this->statusFrame);
   this->maxLabel   = new QLabel ("Max:", this->statusFrame);
   this->maxValue   = new QLabel ("-", this->statusFrame);
   this->maxAtLabel = new QLabel ("Max At:", this->statusFrame);
   this->maxAtValue = new QLabel ("-", this->statusFrame);
   this->fwhmLabel  = new QLabel ("FWHM:", this->statusFrame);
   this->fwhmValue  = new QLabel ("-", this->statusFrame);
   this->comLabel   = new QLabel ("CoM:", this->statusFrame);
   this->comValue   = new QLabel ("-", this->statusFrame);

   this->slotIndicator->setFixedWidth (40);
   this->minLabel->setFixedWidth (32);
   this->maxLabel->setFixedWidth (32);
   this->maxAtLabel->setFixedWidth (52);
   this->fwhmLabel->setFixedWidth (48);
   this->comLabel->setFixedWidth (36);

   styleSheet = QEUtilities::colourToStyle (clStatus);
   this->slotIndicator->setStyleSheet (styleSheet);
   this->slotIndicator->setAlignment (Qt::AlignHCenter);

#define SET_VALUE_LABEL(nameValue, tip)  {              \
   this->nameValue->setStyleSheet  (styleSheet);        \
   this->nameValue->setIndent (6);                      \
   this->nameValue->setAlignment (Qt::AlignRight);      \
   this->nameValue->setToolTip (tip);                   \
}
   SET_VALUE_LABEL (minValue, "Minimum Value");
   SET_VALUE_LABEL (maxValue, "Maximum Value");
   SET_VALUE_LABEL (maxAtValue, "Maximum Value X co-ordinate");
   SET_VALUE_LABEL (fwhmValue, "Full Width at Half Maximum");
   SET_VALUE_LABEL (comValue, "Centre of Mass (median position)");

#undef SET_VALUE_LABEL

   this->sLayout->addWidget (this->slotIndicator);
   this->sLayout->addWidget (this->minLabel);
   this->sLayout->addWidget (this->minValue);
   this->sLayout->addWidget (this->maxLabel);
   this->sLayout->addWidget (this->maxValue);
   this->sLayout->addWidget (this->maxAtLabel);
   this->sLayout->addWidget (this->maxAtValue);
   this->sLayout->addWidget (this->fwhmLabel);
   this->sLayout->addWidget (this->fwhmValue);
   this->sLayout->addWidget (this->comLabel);
   this->sLayout->addWidget (this->comValue);

   this->colourDialog = new QColorDialog (this);
   this->dataDialog = new QEPlotterItemDialog (this);
}


//==============================================================================
// DataSets
//==============================================================================
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
   this->isBold = false;
   this->showDots = false;
}

//------------------------------------------------------------------------------
//
QEPlotter::DataSets::~DataSets ()
{
   delete this->calculator;
}

//------------------------------------------------------------------------------
//
bool QEPlotter::DataSets::isInUse ()
{
   return (this->dataKind != NotInUse);
}

//------------------------------------------------------------------------------
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

//==============================================================================
// QEPlotter
//==============================================================================
//
QEPlotter::QEPlotter (QWidget* parent) : QEFrame (parent)
{
   QCaVariableNamePropertyManager* vpnm;
   int slot;

   this->createInternalWidgets ();

   this->plotLeftIsDefined = false;
   this->plotRightIsDefined = false;

   this->setNumVariables (2*ARRAY_LENGTH (this->xy));

   for (slot = 0; slot < ARRAY_LENGTH (this->xy); slot++) {
      this->xy [slot].colour = item_colours [slot];

      this->updateLabel (slot);

      // Set variable index numbers.
      // Must be consistent with the isDataIndex ()  etc. functions.
      //
      this->xy [slot].dataVariableNameManager.setVariableIndex (2*slot + 0);
      this->xy [slot].sizeVariableNameManager.setVariableIndex (2*slot + 1);
   }

   // Configure the panel.
   //
   this->setFrameShape (QFrame::Panel);
   this->setFrameShadow (QFrame::Plain);
   this->setMinimumSize (500, 500);

   this->isReverse = false;
   this->selectedDataSet = 0;

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

   this->xScaleMode = smDynamic;
   this->yScaleMode = smDynamic;

   this->fixedMinX = 0.0;
   this->fixedMaxX = 10.0;
   this->fixedMinY = 0.0;
   this->fixedMaxY = 10.0;

   // Refresh plot check at ~10Hz.
   //
   this->timer = new QTimer (this);
   connect (this->timer, SIGNAL (timeout ()), this, SLOT (tickTimeout ()));
   this->timer->start (50);  // mSec == 0.05 s

   // Do an initial plot - this clears the refresh plot required flag.
   //
   this->plot ();
}

//------------------------------------------------------------------------------
//
QEPlotter::~QEPlotter ()
{
   this->timer->stop ();

   // Note: must detach curves and grids, otherwise some (older) versions of qwt
   // cause a segmentation fault when the associated QwtPolot object is deleted.
   //
   this->releaseCurves ();

   if (this->plotGrid) {
      this->plotGrid->detach();
      delete this->plotGrid;
      this->plotGrid  = NULL;
   }
}

//------------------------------------------------------------------------------
//
QSize QEPlotter::sizeHint () const {
   return QSize (600, 500);
}

//------------------------------------------------------------------------------
// Slot range checking macro function.
// Set default to nil for void functions.
//
#define SLOT_CHECK(slot, default) {                           \
   if ((slot < 0) || (slot >= ARRAY_LENGTH (this->xy))) {     \
      DEBUG << "slot out of range: " << slot;                 \
      return default;                                         \
   }                                                          \
}

//------------------------------------------------------------------------------
//
void QEPlotter::updateLabel (const int slot)
{
   SLOT_CHECK (slot,);

   DataSets* ds = &this->xy [slot];
   QString text = item_labels [slot];

   text.append (" ");

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
         text.append (":= ");
         text.append (ds->expression);
         break;

      case InvalidExpression:
         text.append ("invalid expr.");
         break;

   }

   ds->itemName->setText (text);
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

//------------------------------------------------------------------------------
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

   } else if (this->isSizeIndex (variableIndex)) {

      if (pvName.isEmpty()) {
         this->xy [slot].sizeKind = NotSpecified;
         this->replotIsRequired = true;
      }
   }
}

//------------------------------------------------------------------------------
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

   } else if (this->isSizeIndex (variableIndex)) {

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

//------------------------------------------------------------------------------
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
   qcaobject::QCaObject* qca = createConnection (variableIndex);

   if (!qca) {
      return;
   }

   if (this->isDataIndex (variableIndex)) {
      QObject::connect (qca, SIGNAL (floatingConnectionChanged (QCaConnectionInfo &, const unsigned int &)),
                        this, SLOT  (dataConnectionChanged     (QCaConnectionInfo &, const unsigned int &)));

      QObject::connect (qca, SIGNAL (floatingArrayChanged (const QVector<double>&, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)),
                        this, SLOT  (dataArrayChanged     (const QVector<double>&, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)));

   } else if (this->isSizeIndex (variableIndex)) {
      QObject::connect (qca, SIGNAL (integerConnectionChanged  (QCaConnectionInfo &, const unsigned int &)),
                        this, SLOT  (sizeConnectionChanged     (QCaConnectionInfo &, const unsigned int &)));

      QObject::connect (qca, SIGNAL (integerChanged   (const long &, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)),
                        this, SLOT  (sizeValueChanged (const long &, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)));
   }
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
//
int QEPlotter::findSlot (QObject *obj)
{
   int result = -1;
   int slot;

   for (slot = 0 ; slot < ARRAY_LENGTH (this->xy); slot++) {
      if (this->xy [slot].itemName == obj) {
         // found it.
         //
         result = slot;
         break;
      }
   }

   return result;
}

//------------------------------------------------------------------------------
//
void QEPlotter::selectDataSet (const int slot)
{
   QString styleSheet;
   QString text;

   SLOT_CHECK (slot,);

   if (this->selectedDataSet == slot) {
      this->selectedDataSet = 0;  // none.
   } else {
      this->selectedDataSet = slot;
   }

   if (this->selectedDataSet > 0) {
      text = item_labels [slot];
      styleSheet = this->xy [slot].itemName->styleSheet ();
   } else {
      text = "";
      styleSheet = QEUtilities::colourToStyle (clStatus);

      this->minValue->setText ("-");
      this->maxValue->setText ("-");
      this->maxAtValue->setText ("-");
      this->fwhmValue->setText ("-");
      this->comValue->setText ("-");
   }

   this->slotIndicator->setText (text);
   this->slotIndicator->setStyleSheet (styleSheet);

   this->replotIsRequired = true;
}

//------------------------------------------------------------------------------
//
void QEPlotter::highLight (const int slot, const bool isHigh)
{
   QString styleSheet;

   SLOT_CHECK (slot,);

   if (isHigh) {
      styleSheet = QEUtilities::colourToStyle (clWhite);
   } else {
      styleSheet = QEUtilities::colourToStyle (this->xy [slot].colour);
   }

   this->xy [slot].itemName->setStyleSheet (styleSheet);
}

//------------------------------------------------------------------------------
//
void QEPlotter::contextMenuRequested (const QPoint& pos)
{
   QObject *obj = this->sender();   // who sent the signal.
   int slot = this->findSlot (obj);
   QPoint golbalPos;

   SLOT_CHECK (slot,);
   DataSets* ds = &(this->xy [slot]);

   if (slot > 0) {
      // Only meaningful for y data sets.
      //
      ds->itemMenu->setState (ds->isDisplayed, ds->isBold, ds->showDots);
   }

   golbalPos = ds->itemName->mapToGlobal (pos);
   ds->itemMenu->exec (golbalPos, 0);
}

//------------------------------------------------------------------------------
//
void QEPlotter::contextMenuSelected (const int slot, const QEPlotterMenu::ContextMenuOptions option)
{
   SLOT_CHECK (slot,);

   DataSets* ds = &(this->xy [slot]);
   int n;

   switch (option) {

      case QEPlotterMenu::PLOTTER_LINE_BOLD:
         ds->isBold = ! ds->isBold;
         this->replotIsRequired = true;
         break;

      case QEPlotterMenu::PLOTTER_LINE_DOTS:
         ds->showDots = !ds->showDots;
         this->replotIsRequired = true;
         break;

      case QEPlotterMenu::PLOTTER_LINE_VISIBLE:
         ds->isDisplayed = ! ds->isDisplayed;
         ds->checkBox->setChecked (ds->isDisplayed);
         this->replotIsRequired = true;
         break;

      case QEPlotterMenu::PLOTTER_LINE_COLOUR:
         this->colourDialog->setCurrentColor (ds->colour);
         n = this->colourDialog->exec();
         if (n == 1) {
            ds->colour = this->colourDialog->currentColor ();
            ds->itemName->setStyleSheet (QEUtilities::colourToStyle (ds->colour) );

            this->replotIsRequired = true;
         }
         break;

      case QEPlotterMenu::PLOTTER_DATA_SELECT:
         if (slot > 0) {
            this->selectDataSet (slot);
         }
         break;

      case QEPlotterMenu::PLOTTER_DATA_DIALOG:
         this->dataDialog->setFieldInformation (this->getXYDataPV (slot),
                                                this->getXYAlias  (slot),
                                                this->getXYSizePV (slot));
         n = this->dataDialog->exec (this);
         if (n == 1) {
            QString newData;
            QString newAlias;
            QString newSize;

            this->dataDialog->getFieldInformation (newData, newAlias, newSize);
            this->setXYDataPV (slot, newData);
            this->setXYAlias  (slot, newAlias);
            this->setXYSizePV (slot, newSize);
            this->replotIsRequired = true;
         }
         break;

      case QEPlotterMenu::PLOTTER_DATA_CLEAR:
         this->setXYDataPV (slot, "");
         this->setXYAlias  (slot, "");
         this->setXYSizePV (slot, "");
         this->replotIsRequired = true;
         break;

      case QEPlotterMenu::PLOTTER_SCALE_TO_MIN_MAX:
         if ((slot > 0) && (ds->dataKind == DataPlot || ds->dataKind == CalculationPlot)) {
            this->fixedMinY = ds->plottedMin;
            this->fixedMaxY = ds->plottedMax;
            this->yScaleMode = smFixed;
         }
         break;

      case QEPlotterMenu::PLOTTER_SCALE_TO_ZERO_MAX:
         if ((slot > 0) && (ds->dataKind == DataPlot || ds->dataKind == CalculationPlot)) {
            this->fixedMinY = 0;
            this->fixedMaxY = ds->plottedMax;
            this->yScaleMode = smFixed;
         }
         break;

      default:
         DEBUG << slot <<  option;
         break;
   }
}

//------------------------------------------------------------------------------
//
void QEPlotter::pvNameDropEvent (const int slot, QDropEvent *event)
{
   SLOT_CHECK (slot,);

   // If no text available, do nothing
   //
   if (!event->mimeData()->hasText ()){
      event->ignore ();
      return;
   }

   // Get the drop data
   //
   const QMimeData *mime = event->mimeData ();

   // If there is any text, drop the text

   if (!mime->text().isEmpty ()) {
      // Get the component textual parts
      //
      QStringList pieces = mime->text ().split (QRegExp ("\\s+"),
                                                QString::SkipEmptyParts);

      // Carry out the drop action
      //
      this->setXYDataPV (slot, pieces.value (0, ""));
      this->setXYSizePV (slot, pieces.value (1, ""));
      this->setXYAlias  (slot, pieces.value (2, ""));
   }

   // Tell the dropee that the drop has been acted on
   //
   if (event->source() == this) {
      event->setDropAction(Qt::CopyAction);
      event->accept();
   } else {
      event->acceptProposedAction ();
   }
}

//------------------------------------------------------------------------------
//
void QEPlotter::setReadOut (const QString& text)
{
   message_types mt (MESSAGE_TYPE_INFO, MESSAGE_KIND_STATUS);
   this->sendMessage (text, mt);
}

//------------------------------------------------------------------------------
//
void QEPlotter::pushState ()
{
   this->replotIsRequired = true;
}

//------------------------------------------------------------------------------
//
void QEPlotter::prevState ()
{
   // place holder
}

//------------------------------------------------------------------------------
//
void QEPlotter::nextState ()
{
   // place holder
}

//------------------------------------------------------------------------------
//
QPointF QEPlotter::plotToReal (const QPoint& pos) const
{
   double x, y;

   // Perform basic invsere transformation.
   //
   x = this->plotArea->invTransform (QwtPlot::xBottom, pos.x ());
   y = this->plotArea->invTransform (QwtPlot::yLeft,   pos.y ());

   // Scale to real world units - none yet
   //
   return QPointF (x, y);
}

//------------------------------------------------------------------------------
//
void QEPlotter::onCanvasMouseMove (QMouseEvent* event)
{
   const QPointF real = this->plotToReal (event->pos ());
   QString mouseReadOut;
   QString f;

   mouseReadOut = "";

   f.sprintf ("  x: %+.5g", real.x ());
   mouseReadOut.append (f);

   f.sprintf ("  y: %+.5g", real.y ());
   mouseReadOut.append (f);

   if (this->plotRightIsDefined) {
      const QPointF origin = this->plotToReal (this->plotRightButton);
      const QPointF offset = this->plotToReal (this->plotCurrent);
      const double dx = offset.x() - origin.x ();
      const double dy = offset.y() - origin.y ();

      f.sprintf ("  dx: %+.5g", dx);
      mouseReadOut.append (f);

      f.sprintf ("  dy: %+.5g", dy);
      mouseReadOut.append (f);

      // Calculate slope, but avoid the divide by 0.
      if (dx != 0.0) {
         f.sprintf ("  dy/dx: %+.5g", dy/dx);
         mouseReadOut.append (f);
      }
   }

   this->setReadOut (mouseReadOut);
}

//------------------------------------------------------------------------------
//
bool QEPlotter::isValidXRangeSelection (const QPoint& origin, const QPoint& offset) const
{
   const int minDiff = 8;
   const int deltaX = offset.x () - origin.x ();
   const int deltaY = offset.y () - origin.y ();
   return ((deltaX > minDiff) && (deltaX > ABS (3 * deltaY)));
}

//------------------------------------------------------------------------------
//
bool QEPlotter::isValidYRangeSelection (const QPoint& origin, const QPoint& offset) const
{
   const int minDiff = 8;
   const int deltaX = offset.x () - origin.x ();
   const int deltaY = offset.y () - origin.y ();
   return ((deltaY > minDiff) && (deltaY > ABS (3 * deltaX)));
}

//------------------------------------------------------------------------------
//
void QEPlotter::setXRange (const double xMinimumIn, const double xMaximumIn)
{
   this->fixedMinX = xMinimumIn;
   this->fixedMaxX = xMaximumIn;
   this->xScaleMode = smFixed;
   this->pushState ();
}

//------------------------------------------------------------------------------
//
void QEPlotter::setYRange (const double yMinimumIn, const double yMaximumIn)
{
   this->fixedMinY = yMinimumIn;
   this->fixedMaxY = yMaximumIn;
   this->yScaleMode = smFixed;
   this->pushState ();
}

//------------------------------------------------------------------------------
//
void QEPlotter::onPlaneScaleSelect(const QPoint& origin, const QPoint& offset)
{
   const QPointF rTopLeft     = this->plotToReal (origin);
   const QPointF rBottomRight = this->plotToReal (offset);

   // Only proceed if user has un-ambiguously selected time scaling or y scaling.
   //
   if (this->isValidYRangeSelection (origin, offset)) {
      // Makeing a Y scale adjustment.
      //
      this->setYRange (rBottomRight.y (), rTopLeft.y ());
      this->pushState ();

   } else if (this->isValidXRangeSelection (origin, offset)) {
      // Makeing a X scale adjustment.
      //
      this->setXRange (rTopLeft.x (), rBottomRight.x ());
      this->pushState ();
   } // else doing nothing
}

//------------------------------------------------------------------------------
//
bool QEPlotter::eventFilter (QObject *obj, QEvent *event)
{
   const QEvent::Type type = event->type ();
   QMouseEvent* mouseEvent = NULL;
   int slot;

   switch (type) {

      case QEvent::MouseButtonPress:
         mouseEvent = static_cast<QMouseEvent *> (event);
         slot = this->findSlot (obj);
         if (slot > 0 && (mouseEvent->button () ==  Qt::LeftButton)) {
            // Leverage of menu handler
            this->contextMenuSelected (slot, QEPlotterMenu::PLOTTER_DATA_SELECT);
            return true;  // we have handled this mouse press
         }

         if (obj == this->plotArea->canvas ()) {
            switch (mouseEvent->button ()) {
               case Qt::LeftButton:
                  this->plotLeftButton = mouseEvent->pos ();
                  this->plotLeftIsDefined = true;
                  return true;  // we have handled this mouse press
                  break;

               case Qt::RightButton:
                  this->plotRightButton = mouseEvent->pos ();
                  this->plotRightIsDefined = true;
                  return true;  // we have handled this mouse press
                  break;

               default:
                  break;
            }
         }
         break;


      case QEvent::MouseButtonRelease:
         mouseEvent = static_cast<QMouseEvent *> (event);
         if (obj == this->plotArea->canvas ()) {
            switch (mouseEvent->button ()) {
               case Qt::LeftButton:
                  if (this->plotLeftIsDefined) {
                     this->onPlaneScaleSelect (this->plotLeftButton, this->plotCurrent);
                     this->plotLeftIsDefined = false;
                     this->replotIsRequired = true;
                     return true;  // we have handled this mouse press
                  }
                  break;

               case Qt::RightButton:
                  if (this->plotRightIsDefined) {
                     this->plotRightIsDefined = false;
                     this->replotIsRequired = true;
                     return true;  // we have handled this mouse press
                  }
                  break;

               default:
                  break;
            }
         }
         break;


      case QEvent::MouseMove:
         mouseEvent = static_cast<QMouseEvent *> (event);

         if (obj == this->plotArea->canvas ()) {
            this->plotCurrent = mouseEvent->pos ();
            if (this->plotLeftIsDefined ||this->plotRightIsDefined) {
               this->replotIsRequired = true;
            }
            this->onCanvasMouseMove (mouseEvent);
            return true;  // we have handled move nouse event
         }
         break;


      case QEvent::MouseButtonDblClick:
         slot = this->findSlot (obj);
         if (slot >= 0) {
            // Leverage of menu handler
            this->contextMenuSelected (slot, QEPlotterMenu::PLOTTER_DATA_DIALOG);
            return true;  // we have handled double click
         }
         break;


      case QEvent::DragEnter:
         slot = this->findSlot (obj);
         if (slot >= 0) {
            QDragEnterEvent* dragEnterEvent = static_cast<QDragEnterEvent*> (event);

            // Can only drop if text and not in use.
            //
            if ((dragEnterEvent->mimeData()->hasText ()) &&
                (this->xy [slot].dataKind== NotInUse)) {
               dragEnterEvent->setDropAction (Qt::CopyAction);
               dragEnterEvent->accept ();
               this->highLight (slot, true);
            } else {
               dragEnterEvent->ignore ();
               this->highLight (slot, false);
            }
            return true;
         }
         break;


      case QEvent::DragLeave:
         slot = this->findSlot (obj);
         if (slot >= 0) {
            this->highLight (slot, false);
            return true;
         }
         break;


      case QEvent::Drop:
         slot = this->findSlot (obj);
         if (slot >= 0) {
            QDropEvent* dropEvent = static_cast<QDropEvent*> (event);
            this->pvNameDropEvent (slot, dropEvent);
            this->highLight (slot, false);
            return true;
         }
         break;


      default:
         // Just fall through
         break;
   }

   return false;
}

//------------------------------------------------------------------------------
//
void QEPlotter::paste (QVariant s)
{
   QStringList list;
   QString pvNameSet;

   // s.toSring is a bit limiting when s is a StringList or a List of String.
   // We don't worry about List 0f StringList or List of List of String etc.
   //
   pvNameSet = "";
   list = s.toStringList ();
   for (int j = 0 ; j < list.count(); j++) {
      pvNameSet.append(" ").append (list.value (j));
   }

   // Use pasted text to add a PV(s) to the chart.
   //
   this->addPvNameSet (pvNameSet);
}

//------------------------------------------------------------------------------
//
void QEPlotter::saveConfiguration (PersistanceManager* pm)
{
   const QString formName = this->persistantName ("QEPlotter");

   PMElement formElement = pm->addNamedConfiguration (formName);

   // Save each active PV.
   //
   PMElement pvListElement = formElement.addElement ("PV_List");

   for (int slot = 0; slot < ARRAY_LENGTH (this->xy); slot++) {
      DataSets* ds = &(this->xy [slot]);
      QString strValue;

      if (ds->isInUse ()) {
         PMElement pvElement = pvListElement.addElement ("PV");
         pvElement.addAttribute ("id", slot);

         strValue = this->getXYDataPV (slot);
         if (!strValue.isEmpty()) {
            pvElement.addValue ("Data",strValue );
         }

         strValue = this->getXYSizePV (slot);
         if (!strValue.isEmpty()) {
            pvElement.addValue ("Size",strValue );
         }

         strValue = this->getXYAlias (slot);
         if (!strValue.isEmpty()) {
            pvElement.addValue ("Alais",strValue );
         }

      }
   }
}

//------------------------------------------------------------------------------
//
void QEPlotter::restoreConfiguration (PersistanceManager* pm, restorePhases restorePhase)
{
   if (restorePhase != FRAMEWORK) return;

   const QString formName = this->persistantName ("QEPlotter");

   PMElement formElement = pm->getNamedConfiguration (formName);

   // Restore each PV.
   //
   PMElement pvListElement = formElement.getElement ("PV_List");

   for (int slot = 0; slot < ARRAY_LENGTH (this->xy); slot++) {
      PMElement pvElement = pvListElement.getElement ("PV", "id", slot);
      QString strValue;
      bool status;

      if (pvElement.isNull ()) continue;

      // Attempt to extract a PV names
      //
      status = pvElement.getValue ("Data", strValue);
      if (status) {
         this->setXYDataPV (slot, strValue);
      }

      status = pvElement.getValue ("Size", strValue);
      if (status) {
         this->setXYSizePV (slot, strValue);
      }

      status = pvElement.getValue ("Alais", strValue);
      if (status) {
         this->setXYAlias(slot, strValue);
      }
   }
}

//------------------------------------------------------------------------------
//
void QEPlotter::addPvName (const QString& pvName)
{
   for (int slot = 0; slot < ARRAY_LENGTH (this->xy); slot++) {
      DataSets* ds = &(this->xy [slot]);
      if (ds->isInUse () == false) {
         // Found an empty slot.
         //
         this->setXYDataPV (slot, pvName);
         break;
      }
   }
}

//------------------------------------------------------------------------------
//
void QEPlotter::addPvNameSet (const QString& pvNameSet)
{
   QStringList pvNameList;
   int j;

   // Split input string using white space as delimiter.
   //
   pvNameList = pvNameSet.split (QRegExp ("\\s+"), QString::SkipEmptyParts);
   for (j = 0; j < pvNameList.count (); j++) {
      this->addPvName (pvNameList.value (j));
   }
}

//------------------------------------------------------------------------------
// Slots receiving PV data
//------------------------------------------------------------------------------
//
void QEPlotter::dataConnectionChanged (QCaConnectionInfo& connectionInfo,
                                       const unsigned int &variableIndex)
{
   const int slot = this->slotOf (variableIndex);

   SLOT_CHECK (slot,);
   this->xy [slot].dataIsConnected = connectionInfo.isChannelConnected ();
   this->replotIsRequired = true;
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
//
void QEPlotter::sizeConnectionChanged (QCaConnectionInfo& connectionInfo,
                                       const unsigned int &variableIndex)
{
   const int slot = this->slotOf (variableIndex);

   SLOT_CHECK (slot,);
   this->xy [slot].sizeIsConnected = connectionInfo.isChannelConnected ();
   this->replotIsRequired = true;
}

//------------------------------------------------------------------------------
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
// Plot and plot related functions
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
   result->attach (this->plotArea);
   this->curve_list.append (result);

   // Set curve propeties plus item Pen which include its colour.
   //
   result->setRenderHint (QwtPlotItem::RenderAntialiased);
   result->setStyle (QwtPlotCurve::Lines);

   pen.setColor (this->xy [slot].colour);
   if (this->xy [slot].isBold) {
      pen.setWidth (2);
   } else {
      pen.setWidth (1);
   }
   result->setPen (pen);

   return result;
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
//
void QEPlotter::plotSelectedArea ()
{
   double t1, y1;
   double t2, y2;
   QVector<double> tdata;
   QVector<double> ydata;
   QwtPlotCurve *curve;
   QPen pen;

   // Do inverse transform on button press postions so that they can be re-transformed when plotted ;-)
   // At least we don't need to worry about duration/log scaling here - that looks after itself.
   //
   t1 = this->plotArea->invTransform (QwtPlot::xBottom, this->plotLeftButton.x ());
   y1 = this->plotArea->invTransform (QwtPlot::yLeft,   this->plotLeftButton.y ());

   t2 = this->plotArea->invTransform (QwtPlot::xBottom, this->plotCurrent.x ());
   y2 = this->plotArea->invTransform (QwtPlot::yLeft,   this->plotCurrent.y ());

   tdata << t1;  ydata << y1;
   tdata << t2;  ydata << y1;
   tdata << t2;  ydata << y2;
   tdata << t1;  ydata << y2;
   tdata << t1;  ydata << y1;

   // Set curve propeties plus item Pen which include its colour.
   //
   curve = this->allocateCurve (0);
   curve->setRenderHint (QwtPlotItem::RenderAntialiased);
   curve->setStyle (QwtPlotCurve::Lines);
   if (this->isValidXRangeSelection (this->plotLeftButton, this->plotCurrent) ||
       this->isValidYRangeSelection (this->plotLeftButton, this->plotCurrent) ) {
      pen.setColor(QColor (0x60C060));  // greenish
   } else {
      pen.setColor(QColor (0x808080));  // gray
   }
   pen.setWidth (1);
   curve->setPen (pen);

#if QWT_VERSION >= 0x060000
   curve->setSamples (tdata, ydata);
#else
   curve->setData (tdata, ydata);
#endif
}

//------------------------------------------------------------------------------
//
void QEPlotter::plotOriginToPoint ()
{
   double t1, y1;
   double t2, y2;
   QVector<double> tdata;
   QVector<double> ydata;
   QwtPlotCurve *curve;
   QPen pen;

   // Do inverse transform on button press postions so that they can be re-transformed when plotted ;-)
   // At least we don't need to worry about duration/log scaling here - that looks after itself.
   //
   t1 = this->plotArea->invTransform (QwtPlot::xBottom, this->plotRightButton.x ());
   y1 = this->plotArea->invTransform (QwtPlot::yLeft,   this->plotRightButton.y ());

   t2 = this->plotArea->invTransform (QwtPlot::xBottom, this->plotCurrent.x ());
   y2 = this->plotArea->invTransform (QwtPlot::yLeft,   this->plotCurrent.y ());

   tdata << t1;  ydata << y1;
   tdata << t2;  ydata << y2;

   // Set curve propeties plus item Pen which include its colour.
   //
   curve = this->allocateCurve (0);
   curve->setRenderHint (QwtPlotItem::RenderAntialiased);
   curve->setStyle (QwtPlotCurve::Lines);
   pen.setColor(QColor (0x8080C0));  // blueish
   pen.setWidth (1);
   curve->setPen (pen);

#if QWT_VERSION >= 0x060000
   curve->setSamples (tdata, ydata);
#else
   curve->setData (tdata, ydata);
#endif
}

//------------------------------------------------------------------------------
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
   this->plotArea->setCanvasBackground (QBrush (this->xy [0].colour));
#else
   this->plotArea->setCanvasBackground (this->xy [0].colour);
#endif

   pen.setColor (clGridLine);
   pen.setStyle (Qt::DashLine);
   this->plotGrid->setPen (pen);

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

      ys->plottedMin = ydata.minimumValue ();
      ys->plottedMax = ydata.maximumValue ();

      xMin = MIN (xMin, xdata.minimumValue ());
      xMax = MAX (xMax, xdata.maximumValue ());
      yMin = MIN (yMin, ys->plottedMin);
      yMax = MAX (yMax, ys->plottedMax);

      if (slot == this->selectedDataSet) {
         QString image;
         double value;
         int jAtMax;
         int j;
         double limit;
         int lower;
         int upper;
         double sxy, sy;

         image = QString ("%1").arg (yMin);
         this->minValue->setText (image);

         image = QString ("%1").arg (yMax);
         this->maxValue->setText (image);

         value = 0.0;
         jAtMax = 0;
         for (j = 0; j < number; j++) {
            if (ydata.value(j) == yMax) {
               value = xdata.value (j);
               jAtMax = j;
               break;
            }
         }

         image = QString ("%1").arg (value);
         this->maxAtValue->setText (image);

         // FWHM: half max ias relative to min value.
         //
         limit = (yMax + yMin) / 2.0;
         for (lower = jAtMax; lower > 0          && ydata.value (lower) >= limit; lower--);
         for (upper = jAtMax; upper < number - 1 && ydata.value (upper) >= limit; upper++);

         value = xdata.value (upper) - xdata.value (lower);
         image = QString ("%1").arg (ABS(value));
         this->fwhmValue->setText (image);

         sxy = 0.0;
         sy = 0.0;
         for (j = 0; j < number; j++) {
            sxy += xdata.value (j)* ydata.value (j);
            sy  += ydata.value (j);
         }

         value = sxy / sy;
         image = QString ("%1").arg (value);
         this->comValue->setText (image);
      }
   }

   // Draw selected area box if defined.
   //
   if (this->plotLeftIsDefined) {
      this->plotSelectedArea ();
   }

   // Draw origin to target line if defined..
   //
   if (this->plotRightIsDefined) {
      this->plotOriginToPoint ();
   }

   QEPlotter::adjustMinMax (xMin, xMax, xMin, xMax, xMajor);
   QEPlotter::adjustMinMax (yMin, yMax, yMin, yMax, yMajor);

   this->plotArea->setAxisScale (QwtPlot::xBottom, xMin, xMax, xMajor);
   this->plotArea->setAxisScale (QwtPlot::yLeft,   yMin, yMax, yMajor);

   this->plotArea->replot ();

   // Ensure next timer tick only invokes plot if needs be.
   //
   this->replotIsRequired = false;
}

//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
//
void QEPlotter::tickTimeout ()
{
   if (this->replotIsRequired) {
      this->plot ();
   }
}


//------------------------------------------------------------------------------
// Property functions.
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
//
QString QEPlotter::getVariableSubstitutions ()
{
   // Any one of these name managers can provide the subsitutions.
   //
   return this->xy [0].dataVariableNameManager.getSubstitutionsProperty ();
}

//------------------------------------------------------------------------------
//
void QEPlotter::setXYDataPV (const int slot, const QString& pvName)
{
   SLOT_CHECK (slot,);
   this->xy [slot].dataVariableNameManager.setVariableNameProperty (pvName);
}

//------------------------------------------------------------------------------
//
QString QEPlotter::getXYDataPV (const int slot)
{
   SLOT_CHECK (slot, "");
   return this->xy [slot].dataVariableNameManager.getVariableNameProperty ();
}

//------------------------------------------------------------------------------
//
void QEPlotter::setXYSizePV (const int slot, const QString& pvName)
{
   SLOT_CHECK (slot,);
   this->xy [slot].sizeVariableNameManager.setVariableNameProperty (pvName);
}

//------------------------------------------------------------------------------
//
QString QEPlotter::getXYSizePV (const int slot)
{
   SLOT_CHECK (slot, "");
   return this->xy [slot].sizeVariableNameManager.getVariableNameProperty ();
}

//------------------------------------------------------------------------------
//
void QEPlotter::setXYAlias (const int slot, const QString& aliasName)
{
   SLOT_CHECK (slot,);
   this->xy[slot].aliasName = aliasName;
   this->updateLabel (slot);
}

//------------------------------------------------------------------------------
//
QString QEPlotter::getXYAlias (const int slot)
{
   SLOT_CHECK (slot, "");
   return this->xy[slot].aliasName;
}

//------------------------------------------------------------------------------
//
void QEPlotter::setXYColour (const int slot, const QColor& colour)
{
   SLOT_CHECK (slot,);

   // Slot 0 (X) and last slot (P) have fixed colours.
   //
   if (slot != 0 && slot != ARRAY_LENGTH (this->xy) - 1) {
      this->xy[slot].colour = colour;
      this->xy [slot].itemName->setStyleSheet (QEUtilities::colourToStyle (colour));
   }
}

//------------------------------------------------------------------------------
//
QColor QEPlotter::getXYColour (const int slot)
{
   SLOT_CHECK (slot, QColor (0,0,0,0));
   return this->xy[slot].colour;
}

//------------------------------------------------------------------------------
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
