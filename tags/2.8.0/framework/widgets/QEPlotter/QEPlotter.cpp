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
 *  Copyright (c) 2013 Australian Synchrotron.
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QCheckBox>
#include <QApplication>

#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <QEGraphic.h>

#include <QECommon.h>
#include <QEFloating.h>
#include <QEInteger.h>
#include <QEScaling.h>

#include "QEPlotter.h"

#define DEBUG qDebug() << "QEPlotter::" << __FUNCTION__ << ":" << __LINE__

static const QColor clWhite (0xFF, 0xFF, 0xFF, 0xFF);
static const QColor clBlack (0x00, 0x00, 0x00, 0xFF);
static const QColor clGridLine (0xC0C0C0);
static const QColor clReverseGridLine (0x404040);
static const QColor clStatus (0xF0F0F0);

static const QString letterStyle ("QWidget { background-color: #e8e8e8; }");

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
// Slot range checking macro function.
// Set default to nil for void functions.
//
#define SLOT_CHECK(slot, default) {                           \
   if ((slot < 0) || (slot >= ARRAY_LENGTH (this->xy))) {     \
      DEBUG << "slot out of range: " << slot;                 \
      return default;                                         \
   }                                                          \
}


//==============================================================================
// QEPlotter
//==============================================================================
//
void QEPlotter::createSlotWidgets (const int slot)
{
   SLOT_CHECK(slot,)

   QFrame* frame = new QFrame (this->itemFrame);
   frame->setFixedHeight (16);
   this->itemLayout->addWidget (frame);
   if (slot == 0) {
      this->itemLayout->addSpacing (4);
   }

   QHBoxLayout* frameLayout = new QHBoxLayout (frame);
   frameLayout->setMargin (0);
   frameLayout->setSpacing (2);

   QPushButton* letter = new QPushButton (frame);
   QLabel* label = new QLabel (frame);
   QCheckBox* box = NULL;

   letter->setFixedWidth (16);
   letter->setText (item_labels [slot]);
   letter->setStyleSheet (letterStyle);
   frameLayout->addWidget (letter);

   QObject::connect (letter, SIGNAL ( clicked (bool)),
                     this,   SLOT   ( letterButtonClicked (bool)));

   label->setMinimumWidth (16);
   label->setMaximumWidth (400);
   label->setIndent (6);
   label->setStyleSheet (QEUtilities::colourToStyle (item_colours [slot]));
   label->setAcceptDrops (true);
   label->installEventFilter (this);
   label->setContextMenuPolicy (Qt::CustomContextMenu);
   frameLayout->addWidget (label);

   QObject::connect (label, SIGNAL ( customContextMenuRequested (const QPoint &)),
                     this,  SLOT   ( itemContextMenuRequested (const QPoint &)));

   if (slot == 0) {
      frameLayout->addSpacing (4 + 17);
   } else {
      frameLayout->addSpacing (4);
      box = new QCheckBox (frame);
      box->setFixedWidth (17);
      box->setChecked (true);
      frameLayout->addWidget (box);

      QObject::connect (box,  SIGNAL (stateChanged (int)),
                        this, SLOT   (checkBoxStateChanged (int)));
   }

   // Save widget references.
   //
   this->xy [slot].frame = frame;
   this->xy [slot].frameLayout = frameLayout;
   this->xy [slot].letterButton = letter;
   this->xy [slot].itemName = label;
   this->xy [slot].checkBox = box;

   // Setup widget to slot mapping.
   //
   this->widgetToSlot.insert (frame, slot);
   this->widgetToSlot.insert (letter, slot);
   this->widgetToSlot.insert (label, slot);
   this->widgetToSlot.insert (box, slot);

   // Add spacer at the bottom of the last item.
   //
   if (slot == ARRAY_LENGTH (this->xy) - 1) {
      QSpacerItem* verticalSpacer = new QSpacerItem (10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);
      this->itemLayout->addItem (verticalSpacer);
   }
}

//------------------------------------------------------------------------------
//
void QEPlotter::createInternalWidgets ()
{
   int slot;
   QString styleSheet;

   // Main layout.
   //
   this->vLayout = new QVBoxLayout (this);
   this->vLayout->setMargin (4);
   this->vLayout->setSpacing (4);

   // Create tool bar frame and tool buttons.
   //
   this->toolBar = new QEPlotterToolBar (); // this will become parented by toolBarResize

   // Connect various tool bar signal to the plotter slot.
   // The tool bar generates the same signals at the context menus.
   //
   this->connectMenuOrToolBar (this->toolBar);

   this->toolBarResize = new QEResizeableFrame (QEResizeableFrame::BottomEdge, 8, 8 + this->toolBar->designHeight, this);
   this->toolBarResize->setFrameShape (QFrame::StyledPanel);
   this->toolBarResize->setFrameShadow (QFrame::Raised);
   this->toolBarResize->setFixedHeight (8 + this->toolBar->designHeight);
   this->toolBarResize->setMinimumWidth (300);
   this->toolBarResize->setMaximumWidth (1920);
   this->toolBarResize->setWidget (this->toolBar);
   this->vLayout->addWidget (this->toolBarResize);

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
   this->plotLayout = new QVBoxLayout (this->plotFrame);
   this->plotLayout->setMargin (4);
   this->plotLayout->setSpacing (4);

   this->plotArea = new QEGraphic (this->plotFrame);
   this->plotLayout->addWidget (this->plotArea);

   QObject::connect (this->plotArea, SIGNAL (mouseMove     (const QPointF&)),
                     this,           SLOT   (plotMouseMove (const QPointF&)));

   QObject::connect (this->plotArea, SIGNAL (wheelRotate   (const QPointF&, const int)),
                     this,           SLOT   (zoomInOut     (const QPointF&, const int)));

   QObject::connect (this->plotArea, SIGNAL (leftSelected  (const QPointF&, const QPointF&)),
                     this,           SLOT   (scaleSelect   (const QPointF&, const QPointF&)));

   QObject::connect (this->plotArea, SIGNAL (rightSelected (const QPointF&, const QPointF&)),
                     this,           SLOT   (lineSelected  (const QPointF&, const QPointF&)));

   this->itemResize = new QEResizeableFrame (QEResizeableFrame::LeftEdge, 60, 400, this->theMainFrame);
   this->itemResize->setFrameShape (QFrame::StyledPanel);
   this->itemResize->setFrameShadow (QFrame::Raised);
   this->itemResize->setFixedWidth (256);
   this->hLayout->addWidget (this->itemResize);

   this->itemFrame = new QFrame (NULL); // re-parented.
   this->itemFrame->setFrameShape (QFrame::NoFrame);
   this->itemFrame->setFrameShadow (QFrame::Plain);
   this->itemResize->setWidget (this->itemFrame);

   this->itemLayout = new QVBoxLayout (this->itemFrame);
   this->itemLayout->setMargin (2);
   this->itemLayout->setSpacing (6);

   for (slot = 0; slot < ARRAY_LENGTH (this->xy); slot++) {
      this->createSlotWidgets (slot);
   }

   // Status frame.
   //
   // Inside status frame - layout left to right.
   //
   this->statusLayout = new QHBoxLayout (this->statusFrame);
   this->statusLayout->setMargin (4);
   this->statusLayout->setSpacing (8);

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

   this->statusLayout->addWidget (this->slotIndicator);
   this->statusLayout->addWidget (this->minLabel);
   this->statusLayout->addWidget (this->minValue);
   this->statusLayout->addWidget (this->maxLabel);
   this->statusLayout->addWidget (this->maxValue);
   this->statusLayout->addWidget (this->maxAtLabel);
   this->statusLayout->addWidget (this->maxAtValue);
   this->statusLayout->addWidget (this->fwhmLabel);
   this->statusLayout->addWidget (this->fwhmValue);
   this->statusLayout->addWidget (this->comLabel);
   this->statusLayout->addWidget (this->comValue);

   this->colourDialog = new QColorDialog (this);
   this->dataDialog = new QEPlotterItemDialog (this);
   this->rangeDialog = new QEStripChartRangeDialog (this);
   this->rangeDialog->setWindowTitle ("Plotter Y Range");
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
   this->expressionIsValid = false;
   this->dataIsConnected = false;
   this->sizeIsConnected = false;
   this->isDisplayed = true;
   this->isBold = false;
   this->showDots = false;

   this->frame = NULL;
   this->frameLayout = NULL;
   this->letterButton = NULL;
   this->itemName = NULL;
   this->checkBox = NULL;
   this->itemMenu = NULL;
}

//------------------------------------------------------------------------------
//
QEPlotter::DataSets::~DataSets ()
{
   delete this->calculator;
}

//------------------------------------------------------------------------------
//
void QEPlotter::DataSets::setContext (QEPlotter* ownerIn, int slotIn)
{
   this->owner = ownerIn;
   this->slot = slotIn;
   this->letter = item_labels [this->slot];
}

//------------------------------------------------------------------------------
//
bool QEPlotter::DataSets::isInUse ()
{
   return (this->dataKind != NotInUse);
}

//------------------------------------------------------------------------------
//
int QEPlotter::DataSets::actualSize ()
{
   // use array (waveform) PV size or zero.
   //
   return (this->dataKind == QEPlotter::DataPVPlot) ? this->data.size () : 0;
}

//------------------------------------------------------------------------------
//
int QEPlotter::DataSets::effectiveSize ()
{
   int result = 0;

   switch (this->sizeKind) {

      case QEPlotter::NotSpecified:

         // The size has not been specified - we must choose best value we can
         // based on the specified data array (waveform) PVs.
         //
         switch (this->dataKind) {

            case QEPlotter::NotInUse:
               // No PV spcified either.
               //
               if (this->slot == 0) {
                  // This is the X - choose the maximum of all the y data sizes.
                  // There is an implied calculation X [s] = s
                  //
                  result = this->owner->maxActualYSizes ();
               } else {
                  // This is a Y item - zero is the only sensible choice here.
                  //
                  result = 0;
               }
               break;

            case QEPlotter::CalculationPlot:
               // Choose the maximum of all the actual Y data sizes.
               //
               if (this->slot == 0) {
                  // There is an actual calculation.
                  //
                  result = this->owner->maxActualYSizes ();
               } else {
                  // Go with the effective X size here.
                  // This may be driven by maxActualYSizes or may be a constant or a PV.
                  //
                  result = this->owner->xy [0].effectiveSize ();
               }
               break;


            case QEPlotter::DataPVPlot:
               // Use size of the specified array (waveform) PV to provide the size.
               //
               result = this->data.size ();
               break;
         }

         break;

      case QEPlotter::Constant:
         // Size specified as a constant - just use as is.
         //
         result = this->fixedSize;
         break;

      case QEPlotter::SizePVName:
         // Size specified as a PV - just use value (if we have it).
         //
         result = this->sizeIsConnected ? this->dbSize : 0;
         break;
   }

   return result;
}

//------------------------------------------------------------------------------
//
QString QEPlotter::DataSets::getDataData ()
{
   const int fw = 12;   // field width
   const int n = this->data.count ();
   QString result;
   DataSets* dx = &this->owner->xy [0];

   result = "\n";

   if (this == dx) {
      // x/this data only
      result.append (QString ("%1\n").arg ("X", fw));
      for (int j = 0 ; j < n; j++) {
         result.append ( QString ("%1\n").arg (this->data[j], fw));
      }
   } else {
      // x and y/this data
      result.append (QString ("%1\t%2\n").arg ("X", fw).arg (this->letter, fw));
      for (int j = 0 ; j < n; j++) {
         result.append ( QString ("%1\t%2\n").arg (dx->data[j], fw).arg (this->data[j], fw));
      }
   }
   return result;
}

//------------------------------------------------------------------------------
//
QString QEPlotter::DataSets::getSizeData ()
{
   const int n = this->data.count ();
   QString result;

   result = QString ("%1").arg (n);
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

   this->widgetToSlot.clear();
   this->createInternalWidgets ();

   this->setContextMenuPolicy (Qt::CustomContextMenu);
   QObject::connect (this, SIGNAL (customContextMenuRequested  (const QPoint &)),
                     this, SLOT   (generalContextMenuRequested (const QPoint &)));

   // The actual meni widget is created as and when needed.
   //
   this->generalContextMenu = NULL;

   this->setNumVariables (2*ARRAY_LENGTH (this->xy));

   for (slot = 0; slot < ARRAY_LENGTH (this->xy); slot++) {
      this->xy [slot].setContext (this, slot);   // set owner and slot number.
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
   this->setMinimumSize (240, 120);

   this->enableConextMenu = true;
   this->isLogarithmic = false;
   this->isReverse = false;
   this->isPaused = false;
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

   // Connect action requests to consumer, e.g. qegui.
   //
   QObject* consumer = this->getGuiLaunchConsumer ();
   if (consumer) {
      QObject::connect (this,      SIGNAL (requestAction (const QEActionRequests& )),
                        consumer,  SLOT   (requestAction (const QEActionRequests& )));
   }

   this->xScaleMode = QEPlotterNames::smDynamic;
   this->yScaleMode = QEPlotterNames::smDynamic;

   this->currentMinX = this->fixedMinX = 0.0;
   this->currentMaxX = this->fixedMaxX = 10.0;
   this->currentMinY = this->fixedMinY = 0.0;
   this->currentMaxY = this->fixedMaxY = 10.0;

   // Refresh plot check at ~10Hz.
   //
   this->tickTimer = new QTimer (this);
   this->tickTimerCount = 0;
   this->replotIsRequired = true; // ensure process on first tick.

   connect (this->tickTimer, SIGNAL (timeout ()), this, SLOT (tickTimeout ()));
   this->tickTimer->start (50);  // mSec == 0.05 s

   this->setToolTipSummary ();
   this->pushState ();  // baseline state - there is always at least one.
}

//------------------------------------------------------------------------------
//
QEPlotter::~QEPlotter ()
{
   this->tickTimer->stop ();
}

//------------------------------------------------------------------------------
//
QSize QEPlotter::sizeHint () const {
   return QSize (600, 500);
}

//------------------------------------------------------------------------------
//
void QEPlotter::updateLabel (const int slot)
{
   SLOT_CHECK (slot,);

   DataSets* ds = &this->xy [slot];
   QString text;

   text.clear ();

   switch (ds->dataKind) {
      case NotInUse:
         break;

      case DataPVPlot:
         // If an alias name is defined - use it.
         //
         if (!ds->aliasName.isEmpty()) {
            text.append (ds->aliasName);
         } else {
            text.append (ds->pvName);
         }
         break;

      case CalculationPlot:
         if (ds->expressionIsValid) {
            text.append (":= ");
            text.append (ds->expression);
         } else {
            text.append ("invalid expr.");
         }
         break;

   }

   if (ds->itemName) {
      ds->itemName->setText (text);
   }
}

//------------------------------------------------------------------------------
//
void QEPlotter::letterButtonClicked (bool)
{
   QPushButton* button =  dynamic_cast <QPushButton*> (this->sender ());
   int slot;

   slot = this->findSlot (button);
   if (slot >= 0) {
      this->runDataDialog (slot, button);
   }
}

//------------------------------------------------------------------------------
//
void QEPlotter::checkBoxStateChanged (int state)
{
   QCheckBox* box = dynamic_cast <QCheckBox*> (this->sender ());
   int slot;

   slot = this->findSlot (box);
   if (slot >= 0) {
      this->xy [slot].isDisplayed = (state == Qt::Checked);
      this->replotIsRequired = true;
   }
}

//------------------------------------------------------------------------------
//
void QEPlotter::setNewVariableName (QString variableName,
                                    QString variableNameSubstitutions,
                                    unsigned int variableIndex)
{
   int slot = this->slotOf (variableIndex);

   SLOT_CHECK (slot,);

   // First clear out any status - this is a new PV name or cleared PV name.
   // Note: we must clear the xxxxIsConnect state - we do not get an initial
   // xxxxConnectionChanged (set not connected) signal.
   //
   if (this->isDataIndex (variableIndex)) {
      this->xy [slot].dataKind = NotInUse;
      this->xy [slot].dataIsConnected = false;
   } else if (this->isSizeIndex (variableIndex)) {
      this->xy [slot].sizeKind = NotSpecified;
      this->xy [slot].sizeIsConnected = false;
   }

   // Note: essentially calls createQcaItem.
   //
   this->setVariableNameAndSubstitutions (variableName, variableNameSubstitutions, variableIndex);

   if (this->isDataIndex (variableIndex)) {
      QString pvName;
      pvName = this->getSubstitutedVariableName (variableIndex).trimmed ();
      this->xy [slot].pvName = pvName;
      this->updateLabel (slot);
   }

   this->replotIsRequired = true;
   this->setToolTipSummary ();
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

         this->xy [slot].dataKind = CalculationPlot;
         this->xy [slot].expression = pvName.remove (0, 1);

         okay = this->xy[slot].calculator->initialise (this->xy[slot].expression);
         this->xy [slot].expressionIsValid = okay;

      } else {
         this->xy [slot].dataKind = DataPVPlot;
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
         this->xy [slot].fixedSize = size;
         this->xy [slot].dbSize = 0;
         this->replotIsRequired = true;

      } else {
         // Assume it is a PV.
         //
         this->xy [slot].sizeKind = SizePVName;
         result = new QEInteger (pvName, this, &this->integerFormatting, variableIndex);
         this->xy [slot].fixedSize = 0;
         this->xy [slot].dbSize = 0;
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
      QObject::connect (qca, SIGNAL (connectionChanged     (QCaConnectionInfo &, const unsigned int &)),
                        this, SLOT  (dataConnectionChanged (QCaConnectionInfo &, const unsigned int &)));

      QObject::connect (qca, SIGNAL (floatingArrayChanged (const QVector<double>&, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)),
                        this, SLOT  (dataArrayChanged     (const QVector<double>&, QCaAlarmInfo &, QCaDateTime &, const unsigned int &)));

   } else if (this->isSizeIndex (variableIndex)) {
      QObject::connect (qca, SIGNAL (connectionChanged     (QCaConnectionInfo &, const unsigned int &)),
                        this, SLOT  (sizeConnectionChanged (QCaConnectionInfo &, const unsigned int &)));

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
   return this->widgetToSlot.value (obj, -1);
}

//------------------------------------------------------------------------------
//
QString QEPlotter::getXYExpandedDataPV (const int slot)
{
   return this->getSubstitutedVariableName (2*slot);
}

//------------------------------------------------------------------------------
//
QString QEPlotter::getXYExpandedSizePV (const int slot)
{
   return this->getSubstitutedVariableName (2*slot + 1);
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
void QEPlotter::sendRequestAction (const QString& action, const QString& pvName)
{
   if (!pvName.isEmpty ()) {
      QEActionRequests request (action, pvName);
      emit this->requestAction (request);
   }
}

//------------------------------------------------------------------------------
//
void QEPlotter::generalContextMenuRequested (const QPoint& pos)
{
   QPoint golbalPos;

   // Don't want to do context menu over plot canvas area - we use right-click
   // for other stuff.
   //
   // NOTE: The 2nd part of this check relies on the fact that the right mouse
   // button event handler is called before this slot is invoked.
   //
   if (this->plotArea->rightButtonPressed () == true) {
       return;
   }

   // Because they take so long top create (~ 50mSec), menu are only created as
   // and when needed. A one off 50 mS hardly noticable, while an 900 mS delay
   // when widget created, especially if there are several instances on form,
   // stands out like a sore thumb.
   //
   if (!this->generalContextMenu)  {
      this->generalContextMenu = new QEPlotterMenu (this);
      QEScaling::applyToWidget (this->generalContextMenu);
      this->connectMenuOrToolBar (this->generalContextMenu);
   }

   // Disable/enable show/hide menu items.
   //
   this->generalContextMenu->setActionEnabled (QEPlotterNames::PLOTTER_SHOW_HIDE_TOOLBAR,
                                               this->enableConextMenu);
   this->generalContextMenu->setActionEnabled (QEPlotterNames::PLOTTER_SHOW_HIDE_PV_ITEMS,
                                               this->enableConextMenu);
   this->generalContextMenu->setActionEnabled (QEPlotterNames::PLOTTER_SHOW_HIDE_STATUS,
                                               this->enableConextMenu);

   // Set current checked states.
   //
   this->generalContextMenu->setActionChecked (QEPlotterNames::PLOTTER_SHOW_HIDE_TOOLBAR,
                                               this->getToolBarVisible ());
   this->generalContextMenu->setActionChecked (QEPlotterNames::PLOTTER_SHOW_HIDE_PV_ITEMS,
                                               this->getPvItemsVisible ());
   this->generalContextMenu->setActionChecked (QEPlotterNames::PLOTTER_SHOW_HIDE_STATUS,
                                               this->getStatusVisible ());

   golbalPos = this->mapToGlobal (pos);
   this->generalContextMenu->exec (golbalPos, 0);
}

//------------------------------------------------------------------------------
//
void QEPlotter::itemContextMenuRequested (const QPoint& pos)
{
   QObject *obj = this->sender();   // who sent the signal.
   int slot = this->findSlot (obj);
   QPoint golbalPos;

   SLOT_CHECK (slot,);
   DataSets* ds = &(this->xy [slot]);

   // Differed creation - see generalContextMenuRequested comment.
   //
   if (!this->xy [slot].itemMenu) {
      this->xy [slot].itemMenu = new QEPlotterMenu (slot, this);
      QEScaling::applyToWidget (this->xy [slot].itemMenu);
      this->connectMenuOrToolBar (this->xy [slot].itemMenu);
   }

   // Allow paste PV menu if and only if we have something to paste?
   //
   bool pasteAllowed = !(QApplication::clipboard ()->text ().trimmed().isEmpty ());

   ds->itemMenu->setActionEnabled (QEPlotterNames::PLOTTER_PASTE_DATA_PV, pasteAllowed);
   ds->itemMenu->setActionEnabled (QEPlotterNames::PLOTTER_PASTE_SIZE_PV, pasteAllowed);

   // Allow edit PV menu if and only if we are using the engineer use level.
   //
   bool inEngineeringMode = (this->getUserLevel () == userLevelTypes::USERLEVEL_ENGINEER);

   ds->itemMenu->setActionVisible (QEPlotterNames::PLOTTER_GENERAL_DATA_PV_EDIT, inEngineeringMode);
   ds->itemMenu->setActionVisible (QEPlotterNames::PLOTTER_GENERAL_SIZE_PV_EDIT, inEngineeringMode);

   if (slot > 0) {
      // Only meaningful for y data sets.
      //
      ds->itemMenu->setState (ds->isDisplayed, ds->isBold, ds->showDots);
   }

   golbalPos = ds->itemName->mapToGlobal (pos);
   ds->itemMenu->exec (golbalPos, 0);
}

//------------------------------------------------------------------------------
// Both the QEPlotterMenu and QEPlotterToolBar widgets use the same
// signal signature.
//
bool QEPlotter::connectMenuOrToolBar (QWidget* item)
{
   return QObject::connect (item, SIGNAL (selected     (const QEPlotterNames::MenuActions, const int)),
                            this, SLOT   (menuSelected (const QEPlotterNames::MenuActions, const int)));
}

//------------------------------------------------------------------------------
//
void QEPlotter::runDataDialog (const int slot, QWidget* control)
{
   SLOT_CHECK (slot,);
   int n;

   // Note: we populate the dialog with the expanded variable name.
   //
   this->dataDialog->setFieldInformation (this->getXYExpandedDataPV (slot),
                                          this->getXYAlias (slot),
                                          this->getXYExpandedSizePV (2*slot + 1));

   n = this->dataDialog->exec (control ? control: this);
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
}


//------------------------------------------------------------------------------
//
void QEPlotter::menuSelected (const QEPlotterNames::MenuActions action, const int slot)
{
   SLOT_CHECK (slot,);  // We know slot is 0 (i.e valid) when not used.

   QWidget* wsender = dynamic_cast <QWidget*> (this->sender ());
   QClipboard* cb = NULL;
   QString copyText;
   QString pasteText;
   QString pvName;
   DataSets* ds = &(this->xy [slot]);
   int n;

   switch (action) {

      case QEPlotterNames::PLOTTER_SHOW_HIDE_TOOLBAR:
         this->setToolBarVisible (! this->getToolBarVisible ());
         break;

      case QEPlotterNames::PLOTTER_SHOW_HIDE_PV_ITEMS:
         this->setPvItemsVisible (! this->getPvItemsVisible ());
         break;

      case QEPlotterNames::PLOTTER_SHOW_HIDE_STATUS:
         this->setStatusVisible (! this->getStatusVisible ());
         break;

      case QEPlotterNames::PLOTTER_PREV:
         this->prevState ();
         break;

      case QEPlotterNames::PLOTTER_NEXT:
         this->nextState ();
         break;

      case QEPlotterNames::PLOTTER_NORMAL_VIDEO:
         this->isReverse = false;
         this->setXYColour (NUMBER_OF_PLOTS, clBlack);
         this->pushState ();
         break;

      case QEPlotterNames::PLOTTER_REVERSE_VIDEO:
         this->isReverse = true;
         this->setXYColour (NUMBER_OF_PLOTS, clWhite);
         this->pushState ();
         break;

      case QEPlotterNames::PLOTTER_LINEAR_Y_SCALE:
         this->isLogarithmic = false;
         this->pushState ();
         break;

      case QEPlotterNames::PLOTTER_LOG_Y_SCALE:
         this->isLogarithmic = true;
         this->pushState ();
         break;

      case QEPlotterNames::PLOTTER_MANUAL_Y_RANGE:
         this->rangeDialog->setWindowTitle ("Plotter Y Range");
         this->rangeDialog->setRange (this->fixedMinY, this->fixedMaxY);
         n = this->rangeDialog->exec (wsender ? wsender : this);
         if (n == 1) {
            // User has selected okay.
            //
            this->yScaleMode = QEPlotterNames::smFixed;
            this->fixedMinY = this->rangeDialog->getMinimum ();
            this->fixedMaxY = this->rangeDialog->getMaximum ();
            this->pushState ();
         }
         break;

      case QEPlotterNames::PLOTTER_CURRENT_Y_RANGE:
         this->yScaleMode = QEPlotterNames::smFixed;
         this->fixedMinY = this->currentMinY;
         this->fixedMaxY = this->currentMaxY;
         this->pushState ();
         break;

      case QEPlotterNames::PLOTTER_DYNAMIC_Y_RANGE:
         this->yScaleMode = QEPlotterNames::smDynamic;
         this->pushState ();
         break;

      case QEPlotterNames::PLOTTER_MANUAL_X_RANGE:
         this->rangeDialog->setWindowTitle ("Plotter X Range");
         this->rangeDialog->setRange (this->fixedMinX, this->fixedMaxX);
         n = this->rangeDialog->exec (wsender ? wsender : this);
         if (n == 1) {
            // User has selected okay.
            //
            this->xScaleMode = QEPlotterNames::smFixed;
            this->fixedMinX = this->rangeDialog->getMinimum ();
            this->fixedMaxX = this->rangeDialog->getMaximum ();
            this->pushState ();
         }
         break;

      case QEPlotterNames::PLOTTER_CURRENT_X_RANGE:
         this->xScaleMode = QEPlotterNames::smFixed;
         this->fixedMinX = this->currentMinX;
         this->fixedMaxX = this->currentMaxX;
         this->pushState ();
         break;

      case QEPlotterNames::PLOTTER_DYNAMIC_X_RANGE:
         this->xScaleMode = QEPlotterNames::smDynamic;
         this->pushState ();
         break;

      case QEPlotterNames::PLOTTER_NORAMLISED_Y_RANGE:
         this->yScaleMode = QEPlotterNames::smNormalised;
         this->fixedMinY = 0.0;
         this->fixedMaxY = 1.0;
         this->pushState ();
         break;

      case QEPlotterNames::PLOTTER_FRACTIONAL_Y_RANGE:
         this->yScaleMode = QEPlotterNames::smFractional;
         this->fixedMinY = 0.0;
         this->fixedMaxY = 1.0;
         this->pushState ();
         break;

      case  QEPlotterNames::PLOTTER_PLAY:
         this->isPaused = false;
         // TODO - request framefork to resend data for all iuse channels.
         this->pushState ();
         break;

      case QEPlotterNames::PLOTTER_PAUSE:
         this->isPaused = true;
         this->pushState ();
         break;

         // PV item specific.
         //
      case QEPlotterNames::PLOTTER_LINE_BOLD:
         ds->isBold = ! ds->isBold;
         this->replotIsRequired = true;
         break;

      case QEPlotterNames::PLOTTER_LINE_DOTS:
         ds->showDots = !ds->showDots;
         this->replotIsRequired = true;
         break;

      case QEPlotterNames::PLOTTER_LINE_VISIBLE:
         ds->isDisplayed = ! ds->isDisplayed;
         ds->checkBox->setChecked (ds->isDisplayed);
         this->replotIsRequired = true;
         break;

      case QEPlotterNames::PLOTTER_LINE_COLOUR:
         this->colourDialog->setCurrentColor (ds->colour);
         n = this->colourDialog->exec();
         if (n == 1) {
            ds->colour = this->colourDialog->currentColor ();
            ds->itemName->setStyleSheet (QEUtilities::colourToStyle (ds->colour) );

            this->replotIsRequired = true;
         }
         break;

      case QEPlotterNames::PLOTTER_DATA_SELECT:
         if (slot > 0) {
            this->selectDataSet (slot);
         }
         break;

      case QEPlotterNames::PLOTTER_DATA_DIALOG:
         this->runDataDialog (slot, wsender);
         break;

      case  QEPlotterNames::PLOTTER_PASTE_DATA_PV:
         cb = QApplication::clipboard ();
         pasteText = cb->text ().trimmed ();

         if (! pasteText.isEmpty ()) {
            this->setXYDataPV (slot, pasteText);
            this->replotIsRequired = true;
         }
         break;

      case  QEPlotterNames::PLOTTER_PASTE_SIZE_PV:
         cb = QApplication::clipboard ();
         pasteText = cb->text ().trimmed ();

         if (! pasteText.isEmpty ()) {
            this->setXYSizePV (slot, pasteText);
            this->replotIsRequired = true;
         }
         break;


      case QEPlotterNames::PLOTTER_COPY_DATA_VARIABLE:
         // Note: we copy the expanded variable name.
         //
         copyText = this->getSubstitutedVariableName (2 * slot);
         cb = QApplication::clipboard ();
         cb->setText (copyText);
         break;

      case QEPlotterNames::PLOTTER_COPY_SIZE_VARIABLE:
         // Note: we copy the expanded variable name.
         //
         copyText = this->getSubstitutedVariableName (2 * slot + 1);
         cb = QApplication::clipboard ();
         cb->setText (copyText);
         break;


      case QEPlotterNames::PLOTTER_COPY_DATA_DATA:
         copyText = ds->getDataData ();
         cb = QApplication::clipboard ();
         cb->setText (copyText);
         break;

      case QEPlotterNames::PLOTTER_COPY_SIZE_DATA:
         copyText = ds->getSizeData ();
         cb = QApplication::clipboard ();
         cb->setText (copyText);
         break;


      case QEPlotterNames::PLOTTER_SHOW_DATA_PV_PROPERTIES:
         pvName = this->getXYExpandedDataPV (slot);
         this->sendRequestAction (QEActionRequests::actionPvProperties (), pvName);
         break;

      case QEPlotterNames::PLOTTER_SHOW_SIZE_PV_PROPERTIES:
         pvName = this->getXYExpandedSizePV (slot);
         this->sendRequestAction (QEActionRequests::actionPvProperties (), pvName);
         break;


      case QEPlotterNames::PLOTTER_ADD_DATA_PV_TO_STRIPCHART:
         pvName = this->getXYExpandedDataPV (slot);
         this->sendRequestAction (QEActionRequests::actionStripChart (), pvName);
         break;

      case QEPlotterNames::PLOTTER_ADD_SIZE_PV_TO_STRIPCHART:
         pvName = this->getXYExpandedSizePV (slot);
         this->sendRequestAction (QEActionRequests::actionStripChart (), pvName);
         break;


      case QEPlotterNames::PLOTTER_ADD_DATA_PV_TO_SCRATCH_PAD:
         pvName = this->getXYExpandedDataPV (slot);
         this->sendRequestAction (QEActionRequests::actionScratchPad (), pvName);
         break;

      case QEPlotterNames::PLOTTER_ADD_SIZE_PV_TO_SCRATCH_PAD:
         pvName = this->getXYExpandedSizePV (slot);
         this->sendRequestAction (QEActionRequests::actionScratchPad (), pvName);
         break;


      case QEPlotterNames::PLOTTER_GENERAL_DATA_PV_EDIT:
         pvName = this->getXYExpandedDataPV (slot);
         this->sendRequestAction (QEActionRequests::actionGeneralPvEdit (), pvName);
         break;

      case QEPlotterNames::PLOTTER_GENERAL_SIZE_PV_EDIT:
         pvName = this->getXYExpandedSizePV (slot);
         this->sendRequestAction (QEActionRequests::actionGeneralPvEdit (), pvName);
         break;


      case QEPlotterNames::PLOTTER_DATA_CLEAR:
         this->setXYDataPV (slot, "");
         this->setXYAlias  (slot, "");
         this->setXYSizePV (slot, "");
         this->replotIsRequired = true;
         break;

      case QEPlotterNames::PLOTTER_SCALE_TO_MIN_MAX:
         if ((slot > 0) && (ds->dataKind == DataPVPlot || ds->dataKind == CalculationPlot)) {
            this->fixedMinY = ds->plottedMin;
            this->fixedMaxY = ds->plottedMax;
            this->yScaleMode = QEPlotterNames::smFixed;
         }
         break;

      case QEPlotterNames::PLOTTER_SCALE_TO_ZERO_MAX:
         if ((slot > 0) && (ds->dataKind == DataPVPlot || ds->dataKind == CalculationPlot)) {
            this->fixedMinY = 0;
            this->fixedMaxY = ds->plottedMax;
            this->yScaleMode = QEPlotterNames::smFixed;
         }
         break;

      default:
         DEBUG << slot <<  action;
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
void QEPlotter::captureState (QEPlotterState& state)
{
   // Capture current state.
   //
   state.isLogarithmic = this->isLogarithmic;
   state.isReverse = this->isReverse;
   state.isPaused = this->isPaused;
   state.xMinimum = this->fixedMinX;
   state.xMaximum = this->fixedMaxX;
   state.xScaleMode = this->xScaleMode;
   state.yMinimum = this->fixedMinY;
   state.yMaximum = this->fixedMaxY;
   state.yScaleMode = this->yScaleMode;
}

//------------------------------------------------------------------------------
//
void QEPlotter::applyState (const QEPlotterState& state)
{
   this->isLogarithmic  = state.isLogarithmic;
   this->isReverse  = state.isReverse;
   this->isPaused  = state.isPaused;
   this->fixedMinX  = state.xMinimum;
   this->fixedMaxX  = state.xMaximum;
   this->xScaleMode  = state.xScaleMode;
   this->fixedMinY  = state.yMinimum;
   this->fixedMaxY  = state.yMaximum;
   this->yScaleMode  = state.yScaleMode;
   this->replotIsRequired = true;
}

//------------------------------------------------------------------------------
//
void QEPlotter::pushState ()
{
   QEPlotterState state;

   // Capture current state.
   //
   this->captureState (state);
   this->stateList.push (state);

   // Enable/disble buttons according to availability.
   //
   this->toolBar->setEnabled (QEPlotterNames::PLOTTER_PREV, (this->stateList.prevAvailable ()));
   this->toolBar->setEnabled (QEPlotterNames::PLOTTER_NEXT, (this->stateList.nextAvailable ()));
   this->replotIsRequired = true;
}

//------------------------------------------------------------------------------
//
void QEPlotter::prevState ()
{
   QEPlotterState state;

   if (this->stateList.prev (state)) {
      this->applyState (state);
      this->toolBar->setEnabled (QEPlotterNames::PLOTTER_PREV, (this->stateList.prevAvailable ()));
      this->toolBar->setEnabled (QEPlotterNames::PLOTTER_NEXT, (this->stateList.nextAvailable ()));
   }
}

//------------------------------------------------------------------------------
//
void QEPlotter::nextState ()
{
   QEPlotterState state;

   if (this->stateList.next (state)) {
      this->applyState (state);
      this->toolBar->setEnabled (QEPlotterNames::PLOTTER_PREV, (this->stateList.prevAvailable ()));
      this->toolBar->setEnabled (QEPlotterNames::PLOTTER_NEXT, (this->stateList.nextAvailable ()));
   }
}

//------------------------------------------------------------------------------
//
void QEPlotter::plotMouseMove (const QPointF& posn)
{
   QString mouseReadOut;
   QString f;
   QPointF slope;

   mouseReadOut = "";

   f.sprintf ("  x: %+.6g", posn.x ());
   mouseReadOut.append (f);

   f.sprintf ("  y: %+.6g", posn.y ());
   mouseReadOut.append (f);

   if (this->plotArea->getRightIsDefined (slope)) {
      const double dx = slope.x ();
      const double dy = slope.y ();

      f.sprintf ("  dx: %+.6g", dx);
      mouseReadOut.append (f);

      f.sprintf ("  dy: %+.6g", dy);
      mouseReadOut.append (f);

      // Calculate slope, but avoid the divide by 0.
      //
      mouseReadOut.append ("  dy/dx: ");
      if (dx != 0.0) {
         f.sprintf ("%+.6g", dy/dx);
      } else {
         if (dy != 0.0) {
            f.sprintf ("%sinf", (dy >= 0.0) ? "+" : "-");
         } else {
            f.sprintf ("n/a");
         }
      }
      mouseReadOut.append (f);
   }

   this->setReadOut (mouseReadOut);

   if (this->plotArea->getLeftIsDefined () ||
       this->plotArea->getRightIsDefined ()) {
      this->replotIsRequired = true;
   }
}

//------------------------------------------------------------------------------
//
void QEPlotter::zoomInOut (const QPointF& about, const int zoomAmount)
{
   if (zoomAmount) {
      // We really only need the sign of the zoomAmount.
      //
      const double factor = (zoomAmount >= 0) ? 0.95 : (1.0 / 0.95);

      double newMin;
      double newMax;

      if (this->isLogarithmic) {
         const double logAboutY = LOG10 (about.y ());

         newMin = EXP10 (logAboutY + (LOG10 (this->fixedMinY) - logAboutY) * factor);
         newMax = EXP10 (logAboutY + (LOG10 (this->fixedMaxY) - logAboutY) * factor);
      } else {
         newMin = about.y () + (this->fixedMinY - about.y ()) * factor;
         newMax = about.y () + (this->fixedMaxY - about.y ()) * factor;
      }

      this->setYRange (newMin, newMax);
      this->pushState ();
   }
}

//------------------------------------------------------------------------------
//
bool QEPlotter::isValidXRangeSelection (const QPoint& diff) const
{
   const int minDiff = 8;
   return ((diff.x () > minDiff) && (diff.x () > ABS (3 * diff.y ())));
}

//------------------------------------------------------------------------------
//
bool QEPlotter::isValidYRangeSelection (const QPoint& diff) const
{
   const int minDiff = 8;
   return ((diff.y () > minDiff) && (diff.y () > ABS (3 * diff.x ())));
}

//------------------------------------------------------------------------------
//
void QEPlotter::setXRange (const double xMinimumIn, const double xMaximumIn)
{
   this->fixedMinX = xMinimumIn;
   this->fixedMaxX = xMaximumIn;
   if (this->xScaleMode == QEPlotterNames::smDynamic) {
      this->xScaleMode = QEPlotterNames::smFixed;
   }
   this->pushState ();
}

//------------------------------------------------------------------------------
//
void QEPlotter::setYRange (const double yMinimumIn, const double yMaximumIn)
{
   if (this->isLogarithmic) {
      this->fixedMinY = LIMIT (yMinimumIn,  0.0, +1.0e23);
   } else {
      this->fixedMinY = LIMIT (yMinimumIn, -1.0e24, +1.0e23);
   }

   this->fixedMaxY = LIMIT (yMaximumIn, this->fixedMinY + 1.0e-20, +1.0e24);

   if (this->yScaleMode == QEPlotterNames::smDynamic) {
      this->yScaleMode = QEPlotterNames::smFixed;
   }
   this->pushState ();
}

//------------------------------------------------------------------------------
//
void QEPlotter::scaleSelect (const QPointF& start, const QPointF& finish)
{
   QPoint distance = this->plotArea->pixelDistance (start, finish);

   // Only proceed if user has un-ambiguously selected x scaling or y scaling.
   //
   if (this->isValidYRangeSelection (distance)) {
      // Makeing a Y scale adjustment.
      //
      this->setYRange (finish.y (), start.y ());
      this->pushState ();

   } else if (this->isValidXRangeSelection (distance)) {
      // Makeing a X scale adjustment.
      //
      this->setXRange (start.x (), finish.x ());
      this->pushState ();

   } else {
      this->replotIsRequired = true;
   }
}

//------------------------------------------------------------------------------
//
void QEPlotter::lineSelected (const QPointF&, const QPointF&)
{
   // no action per se - just request a replot (without the line).
   //
   this->replotIsRequired = true;
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
            this->menuSelected (QEPlotterNames::PLOTTER_DATA_SELECT, slot);
            return true;  // we have handled this mouse press
         }
         break;

      case QEvent::MouseButtonRelease:
         mouseEvent = static_cast<QMouseEvent *> (event);
         break;

      case QEvent::MouseMove:
         mouseEvent = static_cast<QMouseEvent *> (event);
         break;

      case QEvent::MouseButtonDblClick:
         slot = this->findSlot (obj);
         if (slot >= 0) {
            this->runDataDialog (slot, dynamic_cast <QWidget*> (obj));
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
      QString strData = this->getXYExpandedDataPV (slot);
      QString strSize = this->getXYExpandedSizePV (slot);
      QString strAlias = this->getXYAlias (slot);

      // If at least one sub-item is defined then create the PV element.
      //
      if ((!strData.isEmpty ()) || (!strSize.isEmpty ()) || (!strAlias.isEmpty ()) ) {
         PMElement pvElement = pvListElement.addElement ("PV");
         pvElement.addAttribute ("id", slot);

         if (!strData.isEmpty ()) {
            pvElement.addValue ("Data", strData);
         }

         if (!strSize.isEmpty ()) {
            pvElement.addValue ("Size", strSize);
         }

         if (!strAlias.isEmpty ()) {
            pvElement.addValue ("Alias", strAlias);
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

      status = pvElement.getValue ("Alias", strValue);
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
//
void QEPlotter::setToolTipSummary ()
{
   int connected = 0;
   int disconnected = 0;
   int total;
   bool no_disconnects;
   QString customText;

   for (int slot = 0; slot < ARRAY_LENGTH (this->xy); slot++) {
       DataSets* ds = &this->xy [slot];

       if (ds->dataKind == DataPVPlot) {
          if (ds->dataIsConnected) {
             connected++;
          } else {
             disconnected++;
          }
       }

       if (ds->sizeKind == SizePVName) {
          if (ds->sizeIsConnected) {
             connected++;
          } else {
             disconnected++;
          }
       }
   }

   total = connected + disconnected;

   // Only disconnected when ALL, if any, aare disconnected.
   //
   no_disconnects = ((connected > 0) || (total == 0));

   this->updateConnectionStyle (no_disconnects);   // Is this sensible?

   if (total > 0) {
      if (connected == 0) {
         customText = "\nAll PVs disconnected";
      } else if (connected == total) {
         customText = "\nAll PVs connected";
      } else {
         customText = QString ("\n%1 out of %2 PVs connected").arg (connected).arg (total);
      }
   } else {
      customText = "";
   }

   this->updateToolTipCustom (customText);
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
   this->updateToolTipConnection (this->xy [slot].dataIsConnected, variableIndex);
   this->replotIsRequired = true;
   this->setToolTipSummary ();
}

//------------------------------------------------------------------------------
//
void QEPlotter::dataArrayChanged (const QVector<double>& values,
                                  QCaAlarmInfo& alarmInfo,
                                  QCaDateTime&,
                                  const unsigned int& variableIndex)
{
   const int slot = this->slotOf (variableIndex);

   SLOT_CHECK (slot,);
   if (this->isPaused) return;
   this->xy [slot].data = QEFloatingArray (values);
   this->replotIsRequired = true;
   this->processAlarmInfo (alarmInfo, variableIndex);
   this->setToolTipSummary ();
}

//------------------------------------------------------------------------------
//
void QEPlotter::sizeConnectionChanged (QCaConnectionInfo& connectionInfo,
                                       const unsigned int &variableIndex)
{
   const int slot = this->slotOf (variableIndex);

   SLOT_CHECK (slot,);
   this->xy [slot].sizeIsConnected = connectionInfo.isChannelConnected ();
   this->updateToolTipConnection (this->xy [slot].sizeIsConnected, variableIndex);
   this->replotIsRequired = true;
   this->setToolTipSummary ();
}

//------------------------------------------------------------------------------
//
void QEPlotter::sizeValueChanged (const long& value,
                                  QCaAlarmInfo& alarmInfo,
                                  QCaDateTime&,
                                  const unsigned int& variableIndex)
{
   const int slot = this->slotOf (variableIndex);

   SLOT_CHECK (slot,);
   if (this->isPaused) return;
   this->xy [slot].dbSize = value;
   this->replotIsRequired = true;
   this->processAlarmInfo (alarmInfo, variableIndex);
   this->setToolTipSummary ();
}


//------------------------------------------------------------------------------
// Plot and plot related functions
//------------------------------------------------------------------------------
//
void QEPlotter::plotSelectedArea ()
{
   QPoint s;
   QPen pen;

   if (this->plotArea->getLeftIsDefined (s)) {
      this->plotArea->setCurveRenderHint (QwtPlotItem::RenderAntialiased);
      this->plotArea->setCurveStyle (QwtPlotCurve::Lines);

      if (this->isValidXRangeSelection (s) ||
          this->isValidYRangeSelection (s)) {
         pen.setColor(QColor (0x60E060));   // greenish
      } else {
         pen.setColor(QColor (0xC08080));   // redish gray
      }
      pen.setWidth (1);
      this->plotArea->setCurvePen (pen);

      this->plotArea->plotSelectedLeft (true);
   }
}

//------------------------------------------------------------------------------
//
void QEPlotter::plotOriginToPoint ()
{
   QPen pen;

   if (this->plotArea->getRightIsDefined ()) {
      this->plotArea->setCurveRenderHint (QwtPlotItem::RenderAntialiased);
      this->plotArea->setCurveStyle (QwtPlotCurve::Lines);

      pen.setColor(QColor (0x80C0E0));  // blueish
      pen.setWidth (1);
      this->plotArea->setCurvePen (pen);

      this->plotArea->plotSelectedRight (false);
   }
}

//------------------------------------------------------------------------------
//
void QEPlotter::plot ()
{
   QColor background;
   QColor grid;
   QPen pen;
   int slot;
   DataSets* xs;
   DataSets* ys;
   QEFloatingArray xdata;
   QEFloatingArray ydata;
   int effectiveXSize;
   int effectiveYSize;
   int number;
   double xMin, xMax;
   double yMin, yMax;
   bool xMinMaxDefined;
   bool yMinMaxDefined;

   // Prequel
   //
   this->doAnyCalculations ();

   // First release any/all previously allocated curves.
   //
   this->plotArea->releaseCurves ();

   // Set up brackground and grid.
   //
   if (this->isReverse) {
      background = clBlack;
      grid = clReverseGridLine;
   } else {
      background = clWhite;
      grid = clGridLine;
   }

   this->plotArea->setBackgroundColour (background);

   pen.setColor (grid);
   pen.setWidth (1);
   pen.setStyle (Qt::DashLine);
   this->plotArea->setGridPen (pen);

   xMinMaxDefined = false;
   xMin = 0.0;   // defaults when no values.
   xMax = 1.0;

   yMinMaxDefined = false;
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

      // Ignore not in use or invalid expression
      //
      if ((ys->dataKind == NotInUse) ||
          (ys->dataKind == CalculationPlot && ! ys->expressionIsValid)) {
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

      // Truncate both data sets to the same length.
      //
      xdata = QEFloatingArray (xs->data.mid (0, number));
      ydata = QEFloatingArray (ys->data.mid (0, number));

      // Gather, save  and aggregate minima and maxima
      //
      if (xMinMaxDefined) {
         // merge
         xMin = MIN (xMin, xdata.minimumValue ());
         xMax = MAX (xMax, xdata.maximumValue ());
      } else {
         xMin = xdata.minimumValue ();
         xMax = xdata.maximumValue ();
         xMinMaxDefined = true;
      }

      ys->plottedMin = ydata.minimumValue ();
      ys->plottedMax = ydata.maximumValue ();

      if (yMinMaxDefined) {
         // merge
         yMin = MIN (yMin, ys->plottedMin);
         yMax = MAX (yMax, ys->plottedMax);
      } else {
         yMin = ys->plottedMin;
         yMax = ys->plottedMax;
         yMinMaxDefined = true;
      }

      // This this item is the selected item, then calculate and display item attributes.
      //
      if (slot == this->selectedDataSet) {
         processSelectedItem (xdata, ydata, ys->plottedMin, ys->plottedMax);
      }

      // Scale the y data as required.
      //
      if ((this->yScaleMode == QEPlotterNames::smNormalised) ||
          (this->yScaleMode == QEPlotterNames::smFractional)) {
         double m;
         double c;

         if (this->yScaleMode == QEPlotterNames::smNormalised) {
            m = 1.0 / MAX (ys->plottedMax - ys->plottedMin, 1.0e-6);
            c = -m * ys->plottedMin;
         } else {
            m = 1.0 / MAX (ys->plottedMax, 1.0e-6);
            c = 0.0;
         }

         for (int j = 0; j < number; j++) {
            double t = ydata [j];
            ydata [j] = m*t + c;
         }
      }

      // Lastly plot the data.
      //
      pen.setColor (ys->colour);
      if (ys->isBold) {
         pen.setWidth (2);
      } else {
         pen.setWidth (1);
      }
      pen.setStyle (Qt::SolidLine);

      this->plotArea->setCurvePen (pen);
      this->plotArea->setCurveRenderHint (QwtPlotItem::RenderAntialiased);
      this->plotArea->setCurveStyle (QwtPlotCurve::Lines);

      this->plotArea->plotCurveData (xdata, ydata);
   }

   // Draw selected area box if defined.
   //
   this->plotSelectedArea ();

   // Draw origin to target line if defined..
   //
   this->plotOriginToPoint ();

   // Save current min/max values.
   //
   this->currentMinX = xMin;
   this->currentMaxX = xMax;
   this->currentMinY = yMin;
   this->currentMaxY = yMax;

   // Determine plot x and y range to use.
   // If not dynamic, use the fixed values.
   //
   if (this->xScaleMode != QEPlotterNames::smDynamic) {
      xMin = this->fixedMinX;
      xMax = this->fixedMaxX;
   }

   // Repeat for y  - essentially the same excpet for log scale adjustment.
   //
   if (this->yScaleMode != QEPlotterNames::smDynamic) {
      yMin = this->fixedMinY;
      yMax = this->fixedMaxY;
   }

   this->plotArea->setYLogarithmic (this->isLogarithmic);
   this->plotArea->setXRange (xMin, xMax, QEGraphic::SelectBySize, 40);
   this->plotArea->setYRange (yMin, yMax, QEGraphic::SelectBySize, 40);

   this->plotArea->replot ();

   // Ensure next timer tick only invokes plot if needs be.
   //
   this->replotIsRequired = false;
}

//------------------------------------------------------------------------------
//
int QEPlotter::maxActualYSizes ()
{
   int result = 0;

   for (int j = 1; j < ARRAY_LENGTH (this->xy); j++) {
      result = MAX (result, this->xy [j].actualSize ());
   }

   return result;
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
   bool dataIsAvailable;

   xs = &this->xy [0];  // use a alias pointer for brevity
   effectiveXSize = xs->effectiveSize ();

   switch (xs->dataKind) {

      case NotInUse:
         xs->data.clear ();

         // Use default calculation which is just x = index position 0 .. (n-1)
         //
         for (j = 0; j < effectiveXSize; j++) {
            xs->data.append ((double) j);
         }
         break;

      case DataPVPlot:
         // Leave as the data, if any, as supplied by the specified PV.
         break;

      case CalculationPlot:
         xs->data.clear ();
         if (xs->expressionIsValid) {
            for (j = 0; j < effectiveXSize; j++) {
               QEExpressionEvaluation::clear (userArguments);
               userArguments [Normal][s] = (double) j;
               value = xs->calculator->evaluate (userArguments, &okay);
               xs->data.append (value);
            }
         }
   }

   // Next calc slope of actual y fdta values.
   //
   for (slot = 1; slot < ARRAY_LENGTH (this->xy); slot++) {
      ys = &this->xy [slot];
      if (ys->dataKind == DataPVPlot) {
         ys->dyByDx = ys->data.calcDyByDx (xs->data);
      }
   }

   for (slot = 1; slot < ARRAY_LENGTH (this->xy); slot++) {
      ys = &this->xy [slot];
      if (ys->dataKind == CalculationPlot) {

         ys->data.clear ();
         effectiveYSize = ys->effectiveSize ();

         n = MIN (effectiveXSize, effectiveYSize);

         dataIsAvailable = true;
         for (j = 0; j < n; j++) {
            QEExpressionEvaluation::clear (userArguments);

            // Pre-defined values: S and X
            //
            userArguments [Normal][s] = (double) j;
            userArguments [Normal][x] = xs->data.value (j);
            userArguments [Primed][x] = 1.0;    // by defitions.

            for (tols = 1; tols < slot; tols++) {
               DataSets* ts = &this->xy [tols];

               // TODO: determine if this arg used?
               userArguments [Normal] [tols - 1] = ts->data.value (j, 0.0);
               userArguments [Primed] [tols - 1] = ts->dyByDx.value (j, 0.0);
            }

            if (!dataIsAvailable) break;

            value = ys->calculator->evaluate (userArguments, &okay);
            ys->data.append (value);
         }

         // Calculate slope of calculated plot.
         //
         ys->dyByDx = ys->data.calcDyByDx (xs->data);
      }
   }
}

//------------------------------------------------------------------------------
//
void QEPlotter::processSelectedItem (const QEFloatingArray& xdata,
                                     const QEFloatingArray& ydata,
                                     const double yMin, const double yMax)
{
   const int number = ydata.count ();
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

//------------------------------------------------------------------------------
//
void QEPlotter::tickTimeout ()
{
   this->tickTimerCount = (this->tickTimerCount + 1) % 20;

   // Progress any on-going cgart rescaling.
   //
   this->plotArea->doDynamicRescaling ();

   if ((this->tickTimerCount % 20) == 0) {
      // 20th update, i.e. 1 second has passed - must replot.
      this->replotIsRequired = true;
   }

   if (this->replotIsRequired) {
      this->plot ();   // clears replotIsRequired
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

   // Slot 0 (X) has a fixed colour.
   //
   if (slot != 0) {
      this->xy [slot].colour = colour;
      if (this->xy [slot].itemName) {
         this->xy [slot].itemName->setStyleSheet (QEUtilities::colourToStyle (colour));
      }
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
void QEPlotter::setEnableConextMenu (bool enable)
{
   this->enableConextMenu = enable;
}

bool QEPlotter::getEnableConextMenu ()
{
   return this->enableConextMenu;
}

//------------------------------------------------------------------------------
//
void QEPlotter::setToolBarVisible (bool visible)
{
   this->toolBarResize->setVisible (visible);
}

bool QEPlotter::getToolBarVisible ()
{
   return this->toolBarResize->isVisible ();
}

//------------------------------------------------------------------------------
//
void QEPlotter::setPvItemsVisible (bool visible)
{
   this->itemResize->setVisible (visible);
}

bool QEPlotter::getPvItemsVisible ()
{
   return this->itemResize->isVisible ();
}

//------------------------------------------------------------------------------
//
void QEPlotter::setStatusVisible (bool visible)
{
   this->statusFrame->setVisible (visible);
}

bool QEPlotter::getStatusVisible ()
{
   return this->statusFrame->isVisible ();
}

// end