/*
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
 *  Copyright (c) 2011
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

/*!
  This class is a CA aware element selection widget based on the Qt push button widget.
  It is tighly integrated with the base class QCaWidget. Refer to QCaWidget.cpp for details

  This control and display widget associates one or two values with an element.
  A typical use of this widget is to move a one or two axis element reference foil stage
  When the user presses the button an element selection dialog appears.
  The user selects an element and associated values written to variables.
  The values written may be one of several static values such as the atomic weight of
  the element selected, or they may be user defined values, such as a stage position value.
  If subscribing and the variables change, the updated values are matched to an element and
  the element is displayed.
  A user defined string may be emitted on element selection.
 */

#include <QCaPeriodic.h>
#include <PeriodicDialog.h>


// Table containing all static element information
// (Another table - userInfo - contains dynamic element information that varies from instance to instance of this class)
QCaPeriodic::elementInfoStruct QCaPeriodic::elementInfo[NUM_ELEMENTS] = {
//    Number,	Atomic Weight,	Name,	Symbol, Melting Point (deg C),  Boiling  Point (deg C), Density,    Group,  Ionization energy, Table row, table column
    {	1,	1.0079,	"Hydrogen",	"H",	-259,	-253,	0.09,	1,	13.5984,	0,	0 },
    {	2,	4.0026,	"Helium",	"He",	-272,	-269,	0.18,	18,	24.5874,	0,	18 },
    {	3,	6.941,	"Lithium",	"Li",	180,	1347,	0.53,	1,	5.3917,	1,	0 },
    {	4,	9.0122,	"Beryllium",	"Be",	1278,	2970,	1.85,	2,	9.3227,	1,	1 },
    {	5,	10.811,	"Boron",	"B",	2300,	2550,	2.34,	13,	8.298,	1,	13 },
    {	6,	12.0107,	"Carbon",	"C",	3500,	4827,	2.26,	14,	11.2603,	1,	14 },
    {	7,	14.0067,	"Nitrogen",	"N",	-210,	-196,	1.25,	15,	14.5341,	1,	15 },
    {	8,	15.9994,	"Oxygen",	"O",	-218,	-183,	1.43,	16,	13.6181,	1,	16 },
    {	9,	18.9984,	"Fluorine",	"F",	-220,	-188,	1.7,	17,	17.4228,	1,	17 },
    {	10,	20.1797,	"Neon",	"Ne",	-249,	-246,	0.9,	18,	21.5645,	1,	18 },
    {	11,	22.9897,	"Sodium",	"Na",	98,	883,	0.97,	1,	5.1391,	2,	0 },
    {	12,	24.305,	"Magnesium",	"Mg",	639,	1090,	1.74,	2,	7.6462,	2,	1 },
    {	13,	26.9815,	"Aluminum",	"Al",	660,	2467,	2.7,	13,	5.9858,	2,	13 },
    {	14,	28.0855,	"Silicon",	"Si",	1410,	2355,	2.33,	14,	8.1517,	2,	14 },
    {	15,	30.9738,	"Phosphorus",	"P",	44,	280,	1.82,	15,	10.4867,	2,	15 },
    {	16,	32.065,	"Sulfur",	"S",	113,	445,	2.07,	16,	10.36,	2,	16 },
    {	17,	35.453,	"Chlorine",	"Cl",	-101,	-35,	3.21,	17,	12.9676,	2,	17 },
    {	18,	39.948,	"Argon",	"Ar",	-189,	-186,	1.78,	18,	15.7596,	2,	18 },
    {	19,	39.0983,	"Potassium",	"K",	64,	774,	0.86,	1,	4.3407,	3,	0 },
    {	20,	40.078,	"Calcium",	"Ca",	839,	1484,	1.55,	2,	6.1132,	3,	1 },
    {	21,	44.9559,	"Scandium",	"Sc",	1539,	2832,	2.99,	3,	6.5615,	3,	3 },
    {	22,	47.867,	"Titanium",	"Ti",	1660,	3287,	4.54,	4,	6.8281,	3,	4 },
    {	23,	50.9415,	"Vanadium",	"V",	1890,	3380,	6.11,	5,	6.7462,	3,	5 },
    {	24,	51.9961,	"Chromium",	"Cr",	1857,	2672,	7.19,	6,	6.7665,	3,	6 },
    {	25,	54.938,	"Manganese",	"Mn",	1245,	1962,	7.43,	7,	7.434,	3,	7 },
    {	26,	55.845,	"Iron",	"Fe",	1535,	2750,	7.87,	8,	7.9024,	3,	8 },
    {	27,	58.9332,	"Cobalt",	"Co",	1495,	2870,	8.9,	9,	7.881,	3,	9 },
    {	28,	58.6934,	"Nickel",	"Ni",	1453,	2732,	8.9,	10,	7.6398,	3,	10 },
    {	29,	63.546,	"Copper",	"Cu",	1083,	2567,	8.96,	11,	7.7264,	3,	11 },
    {	30,	65.39,	"Zinc",	"Zn",	420,	907,	7.13,	12,	9.3942,	3,	12 },
    {	31,	69.723,	"Gallium",	"Ga",	30,	2403,	5.91,	13,	5.9993,	3,	13 },
    {	32,	72.64,	"Germanium",	"Ge",	937,	2830,	5.32,	14,	7.8994,	3,	14 },
    {	33,	74.9216,	"Arsenic",	"As",	81,	613,	5.72,	15,	9.7886,	3,	15 },
    {	34,	78.96,	"Selenium",	"Se",	217,	685,	4.79,	16,	9.7524,	3,	16 },
    {	35,	79.904,	"Bromine",	"Br",	-7,	59,	3.12,	17,	11.8138,	3,	17 },
    {	36,	83.8,	"Krypton",	"Kr",	-157,	-153,	3.75,	18,	13.9996,	3,	18 },
    {	37,	85.4678,	"Rubidium",	"Rb",	39,	688,	1.63,	1,	4.1771,	4,	0 },
    {	38,	87.62,	"Strontium",	"Sr",	769,	1384,	2.54,	2,	5.6949,	4,	1 },
    {	39,	88.9059,	"Yttrium",	"Y",	1523,	3337,	4.47,	3,	6.2173,	4,	3 },
    {	40,	91.224,	"Zirconium",	"Zr",	1852,	4377,	6.51,	4,	6.6339,	4,	4 },
    {	41,	92.9064,	"Niobium",	"Nb",	2468,	4927,	8.57,	5,	6.7589,	4,	5 },
    {	42,	95.94,	"Molybdenum",	"Mo",	2617,	4612,	10.22,	6,	7.0924,	4,	6 },
    {	43,	98,	"Technetium",	"Tc",	2200,	4877,	11.5,	7,	7.28,	4,	7 },
    {	44,	101.07,	"Ruthenium",	"Ru",	2250,	3900,	12.37,	8,	7.3605,	4,	8 },
    {	45,	102.9055,	"Rhodium",	"Rh",	1966,	3727,	12.41,	9,	7.4589,	4,	9 },
    {	46,	106.42,	"Palladium",	"Pd",	1552,	2927,	12.02,	10,	8.3369,	4,	10 },
    {	47,	107.8682,	"Silver",	"Ag",	962,	2212,	10.5,	11,	7.5762,	4,	11 },
    {	48,	112.411,	"Cadmium",	"Cd",	321,	765,	8.65,	12,	8.9938,	4,	12 },
    {	49,	114.818,	"Indium",	"In",	157,	2000,	7.31,	13,	5.7864,	4,	13 },
    {	50,	118.71,	"Tin",	"Sn",	232,	2270,	7.31,	14,	7.3439,	4,	14 },
    {	51,	121.76,	"Antimony",	"Sb",	630,	1750,	6.68,	15,	8.6084,	4,	15 },
    {	52,	127.6,	"Tellurium",	"Te",	449,	990,	6.24,	16,	9.0096,	4,	16 },
    {	53,	126.9045,	"Iodine",	"I",	114,	184,	4.93,	17,	10.4513,	4,	17 },
    {	54,	131.293,	"Xenon",	"Xe",	-112,	-108,	5.9,	18,	12.1298,	4,	18 },
    {	55,	132.9055,	"Cesium",	"Cs",	29,	678,	1.87,	1,	3.8939,	5,	0 },
    {	56,	137.327,	"Barium",	"Ba",	725,	1140,	3.59,	2,	5.2117,	5,	1 },
    {	57,	138.9055,	"Lanthanum",	"La",	920,	3469,	6.15,	3,	5.5769,	8,	3 },
    {	58,	140.116,	"Cerium",	"Ce",	795,	3257,	6.77,	101,	5.5387,	8,	4 },
    {	59,	140.9077,	"Praseodymium",	"Pr",	935,	3127,	6.77,	101,	5.473,	8,	5 },
    {	60,	144.24,	"Neodymium",	"Nd",	1010,	3127,	7.01,	101,	5.525,	8,	6 },
    {	61,	145,	"Promethium",	"Pm",	1100,	3000,	7.3,	101,	5.582,	8,	7 },
    {	62,	150.36,	"Samarium",	"Sm",	1072,	1900,	7.52,	101,	5.6437,	8,	8 },
    {	63,	151.964,	"Europium",	"Eu",	822,	1597,	5.24,	101,	5.6704,	8,	9 },
    {	64,	157.25,	"Gadolinium",	"Gd",	1311,	3233,	7.9,	101,	6.1501,	8,	10 },
    {	65,	158.9253,	"Terbium",	"Tb",	1360,	3041,	8.23,	101,	5.8638,	8,	11 },
    {	66,	162.5,	"Dysprosium",	"Dy",	1412,	2562,	8.55,	101,	5.9389,	8,	12 },
    {	67,	164.9303,	"Holmium",	"Ho",	1470,	2720,	8.8,	101,	6.0215,	8,	13 },
    {	68,	167.259,	"Erbium",	"Er",	1522,	2510,	9.07,	101,	6.1077,	8,	14 },
    {	69,	168.9342,	"Thulium",	"Tm",	1545,	1727,	9.32,	101,	6.1843,	8,	15 },
    {	70,	173.04,	"Ytterbium",	"Yb",	824,	1466,	6.9,	101,	6.2542,	8,	16 },
    {	71,	174.967,	"Lutetium",	"Lu",	1656,	3315,	9.84,	101,	5.4259,	5,	3 },
    {	72,	178.49,	"Hafnium",	"Hf",	2150,	5400,	13.31,	4,	6.8251,	5,	4 },
    {	73,	180.9479,	"Tantalum",	"Ta",	2996,	5425,	16.65,	5,	7.5496,	5,	5 },
    {	74,	183.84,	"Tungsten",	"W",	3410,	5660,	19.35,	6,	7.864,	5,	6 },
    {	75,	186.207,	"Rhenium",	"Re",	3180,	5627,	21.04,	7,	7.8335,	5,	7 },
    {	76,	190.23,	"Osmium",	"Os",	3045,	5027,	22.6,	8,	8.4382,	5,	8 },
    {	77,	192.217,	"Iridium",	"Ir",	2410,	4527,	22.4,	9,	8.967,	5,	9 },
    {	78,	195.078,	"Platinum",	"Pt",	1772,	3827,	21.45,	10,	8.9587,	5,	10 },
    {	79,	196.9665,	"Gold",	"Au",	1064,	2807,	19.32,	11,	9.2255,	5,	11 },
    {	80,	200.59,	"Mercury",	"Hg",	-39,	357,	13.55,	12,	10.4375,	5,	12 },
    {	81,	204.3833,	"Thallium",	"Tl",	303,	1457,	11.85,	13,	6.1082,	5,	13 },
    {	82,	207.2,	"Lead",	"Pb",	327,	1740,	11.35,	14,	7.4167,	5,	14 },
    {	83,	208.9804,	"Bismuth",	"Bi",	271,	1560,	9.75,	15,	7.2856,	5,	15 },
    {	84,	209,	"Polonium",	"Po",	254,	962,	9.3,	16,	8.417,	5,	16 },
    {	85,	210,	"Astatine",	"At",	302,	337,	0,	17,	9.3,	5,	17 },
    {	86,	222,	"Radon",	"Rn",	-71,	-62,	9.73,	18,	10.7485,	5,	18 },
    {	87,	223,	"Francium",	"Fr",	27,	677,	0,	1,	4.0727,	6,	0 },
    {	88,	226,	"Radium",	"Ra",	700,	1737,	5.5,	2,	5.2784,	6,	1 },
    {	89,	227,	"Actinium",	"Ac",	1050,	3200,	10.07,	3,	5.17,	9,	3 },
    {	90,	232.0381,	"Thorium",	"Th",	1750,	4790,	11.72,	102,	6.3067,	9,	4 },
    {	91,	231.0359,	"Protactinium",	"Pa",	1568,	0,	15.4,	102,	5.89,	9,	5 },
    {	92,	238.0289,	"Uranium",	"U",	1132,	3818,	18.95,	102,	6.1941,	9,	6 },
    {	93,	237,	"Neptunium",	"Np",	640,	3902,	20.2,	102,	6.2657,	9,	7 },
    {	94,	244,	"Plutonium",	"Pu",	640,	3235,	19.84,	102,	6.0262,	9,	8 },
    {	95,	243,	"Americium",	"Am",	994,	2607,	13.67,	102,	5.9738,	9,	9 },
    {	96,	247,	"Curium",	"Cm",	1340,	0,	13.5,	102,	5.9915,	9,	10 },
    {	97,	247,	"Berkelium",	"Bk",	986,	0,	14.78,	102,	6.1979,	9,	11 },
    {	98,	251,	"Californium",	"Cf",	900,	0,	15.1,	102,	6.2817,	9,	12 },
    {	99,	252,	"Einsteinium",	"Es",	860,	0,	0,	102,	6.42,	9,	13 },
    {	100,	257,	"Fermium",	"Fm",	1527,	0,	0,	102,	6.5,	9,	14 },
    {	101,	258,	"Mendelevium",	"Md",	0,	0,	0,	102,	6.58,	9,	15 },
    {	102,	259,	"Nobelium",	"No",	827,	0,	0,	102,	6.65,	9,	16 },
    {	103,	262,	"Lawrencium",	"Lr",	1627,	0,	0,	102,	4.9,	6,	3 },
    {	104,	261,	"Rutherfordium",	"Rf",	0,	0,	0,	4,	0,	6,	4 },
    {	105,	262,	"Dubnium",	"Db",	0,	0,	0,	5,	0,	6,	5 },
    {	106,	266,	"Seaborgium",	"Sg",	0,	0,	0,	6,	0,	6,	6 },
    {	107,	264,	"Bohrium",	"Bh",	0,	0,	0,	7,	0,	6,	7 },
    {	108,	277,	"Hassium",	"Hs",	0,	0,	0,	8,	0,	6,	8 },
    {	109,	268,	"Meitnerium",	"Mt",	0,	0,	0,	9,	0,	6,	9 },
    {	110,	268,	"Uuu???",	"Uun",	0,	0,	0,	10,	0,	6,	10 },
    {	111,	268,	"Uuu???",	"Uuu",	0,	0,	0,	11,	0,	6,	11 },
    {	112,	268,	"Uub???",	"Uub",	0,	0,	0,	12,	0,	6,	12 },
    {	113,	268,	"Uuq???",	"Uuq",	0,	0,	0,	14,	0,	6,	14}
};


/*!
    Constructor with no initialisation
*/
QCaPeriodic::QCaPeriodic( QWidget *parent ) : QPushButton( parent ), QCaWidget() {
    setup();
}

/*!
    Constructor with known variable
*/
QCaPeriodic::QCaPeriodic( const QString &variableNameIn, QWidget *parent ) : QPushButton( parent ), QCaWidget() {
    setVariableName( variableNameIn, 0 );

    setup();

}

/*!
    Setup common to all constructors
*/
void QCaPeriodic::setup() {
    // Set up data
    // This control used a single data source
    setNumVariables(2);

    // Override default QCaWidget and QPushButton properties
    subscribe = false;
    setAutoDefault( false );

    // Set up default properties
    localEnabled = true;

    // Set the initial state
    lastSeverity = QCaAlarmInfo::getInvalidSeverity();
    isConnected = false;
    QWidget::setEnabled( false );  // Reflects initial disconnected state

    variableType1 = VARIABLE_TYPE_USER_VALUE_1;
    variableType1 = VARIABLE_TYPE_USER_VALUE_2;

    haveLastData1 = false;
    haveLastData2 = false;

    // Use push button signals
    QObject::connect( this, SIGNAL( clicked() ), this, SLOT( userClicked() ) );

    // Initialise user values
    for( int i = 0; i < NUM_ELEMENTS; i++ )
    {
        userInfo[i].value1 = 0.0;
        userInfo[i].value2 = 0.0;
        userInfo[i].enable = true;
    }

}

/*!
    Implementation of QCaWidget's virtual funtion to create the specific type of QCaObject required.
    For a push button a QCaObject that streams strings is required.
*/
qcaobject::QCaObject* QCaPeriodic::createQcaItem( unsigned int variableIndex ) {

    // Create the items as a QCaFloating
    return new QCaFloating( getSubstitutedVariableName( variableIndex ), this, &floatingFormatting, variableIndex );
}

/*!
    Start updating.
    Implementation of VariableNameManager's virtual funtion to establish a connection to a PV as the variable name has changed.
    This function may also be used to initiate updates when loaded as a plugin.
*/
void QCaPeriodic::establishConnection( unsigned int variableIndex ) {

    // Create a connection.
    // If successfull, the QCaObject object that will supply data update signals will be returned
    qcaobject::QCaObject* qca = createConnection( variableIndex );

    // If a QCaObject object is now available to supply data update signals, connect it to the appropriate slots
    if(  qca ) {
        // Get updates if subscribing
        if( subscribe )
        {
            setText( "--" );
            QObject::connect( qca,  SIGNAL( floatingChanged( const double&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ),
                              this, SLOT( setElement( const double&, QCaAlarmInfo&, QCaDateTime&, const unsigned int& ) ) );
        }

        // Get conection status changes always (subscribing or not)
        QObject::connect( qca,  SIGNAL( connectionChanged( QCaConnectionInfo& ) ),
                          this, SLOT( connectionChanged( QCaConnectionInfo& ) ) );
    }
}

/*!
    Update the tool tip as requested by QCaToolTip.
*/
void QCaPeriodic::updateToolTip ( const QString & toolTip ) {
    setToolTip( toolTip );
}

/*!
    Act on a connection change.
    Change how the label looks and change the tool tip
    This is the slot used to recieve connection updates from a QCaObject based class.
 */
void QCaPeriodic::connectionChanged( QCaConnectionInfo& connectionInfo )
{

    /// If connected enabled the widget if required.
    if( connectionInfo.isChannelConnected() )
    {
        isConnected = true;
        updateToolTipConnection( isConnected );

        if( localEnabled )
            QWidget::setEnabled( true );
    }

    /// If disconnected always disable the widget.
    else
    {
        isConnected = false;
        updateToolTipConnection( isConnected );

        QWidget::setEnabled( false );
    }
}

/*!
  Implement a slot to set the current text of the push button
  This is the slot used to recieve data updates from a QCaObject based class.
*/
void QCaPeriodic::setElement( const double& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex )
{
    /// If not subscribing, then do nothing.
    /// Note, This will still be called even if not subscribing as there may be an initial sing shot read
    /// to ensure we have valid information about the variable when it is time to do a write.
    if( !subscribe )
        return;

    /// Signal a database value change to any Link widgets
    emit dbValueChanged( value );

    switch( variableIndex )
    {
        case 0:
            lastData1 = value;
            haveLastData1 = true;
            break;

        case 1:
            lastData2 = value;
            haveLastData2 = true;
            break;
    }


    // Get the variables (used to see if one or two must match)
    QCaString *qca1 = (QCaString*)getQcaItem(0);
    QCaString *qca2 = (QCaString*)getQcaItem(1);

    // If all required data is available...
    if( ( qca1 && haveLastData1 && qca2 && haveLastData2 ) ||   // If both inputs are required and are present
        ( qca1 && haveLastData1 && !qca2 ) ||                   // Or if only first is required and is present
        ( !qca1 && qca2 && haveLastData2 ) )                    // Or if only second is required and is present
    {
        // ... update the element

        int i;
        bool match = false;

        // Look for the index of the currently selected element
        for( i = 0; i < NUM_ELEMENTS; i++ )
        {
            if( elementInfo[i].symbol.compare( text() ) == 0 )
                break;
        }

        // If there is a currently selected element, check if it matches the current values
        if( i != NUM_ELEMENTS )
        {
            match =  elementMatch( i, qca1!=NULL, lastData1, qca2!=NULL, lastData2 );
        }

        // If there was no currently selected element, or it didn't match the current values,
        // check each element looking for one that matches the current values
        if( !match )
        {
            for( i = 0; i < NUM_ELEMENTS; i++ )
            {
                match =  elementMatch( i, qca1!=NULL, lastData1, qca2!=NULL, lastData2 );
                if( match )
                    break;
            }
        }

        // If an element matched, display it and emit any related text
        // Note, 'i' is valid if a match has been found
        if( match )
        {
            setText( elementInfo[i].symbol );
            emit dbElementChanged( userInfo[i].elementText );
        }

        // If no element matched, display a neutral string and it emit an empty string
        else
        {
            setText( "--" );
            emit dbElementChanged( "" );
        }
    }

    // If in alarm, display as an alarm
    if( alarmInfo.getSeverity() != lastSeverity )
    {
            updateToolTipAlarm( alarmInfo.severityName() );
            setStyleSheet( alarmInfo.style() );
            lastSeverity = alarmInfo.getSeverity();
    }
}

// Determine if the value or values recieved match an element
// Used in QCaPeriodic::setElement() above only
bool QCaPeriodic::elementMatch( int i,
                                bool haveFirstVariable,
                                double lastData1,
                                bool haveSecondVariable,
                                double lastData2 )
{
    // Value selected from element info or user info depending on type
    double value;

    // Assume an element matches
    bool match = true;

    // If first variable is used, check if current element doesn't match
    if( haveFirstVariable )
    {
        switch( variableType1 )
        {
        case VARIABLE_TYPE_NUMBER:            value = elementInfo[i].number;           break;
        case VARIABLE_TYPE_ATOMIC_WEIGHT:     value = elementInfo[i].atomicWeight;     break;
        case VARIABLE_TYPE_MELTING_POINT:     value = elementInfo[i].meltingPoint;     break;
        case VARIABLE_TYPE_BOILING_POINT:     value = elementInfo[i].boilingPoint;     break;
        case VARIABLE_TYPE_DENSITY:           value = elementInfo[i].density;          break;
        case VARIABLE_TYPE_GROUP:             value = elementInfo[i].group;            break;
        case VARIABLE_TYPE_IONIZATION_ENERGY: value = elementInfo[i].ionizationEnergy; break;
        case VARIABLE_TYPE_USER_VALUE_1:      value = userInfo[i].value1;              break;
        case VARIABLE_TYPE_USER_VALUE_2:      value = userInfo[i].value2;              break;
        }

        if( value < lastData1 - variableTolerance1 ||
            value > lastData1 + variableTolerance1 )
        {
            match = false;
        }
    }

    // If second variable is used, check if current element doesn't match (but don't bother checking if alread no match because of first variable)
    if( haveSecondVariable && match )
    {
        switch( variableType2 )
        {
        case VARIABLE_TYPE_NUMBER:            value = elementInfo[i].number;           break;
        case VARIABLE_TYPE_ATOMIC_WEIGHT:     value = elementInfo[i].atomicWeight;     break;
        case VARIABLE_TYPE_MELTING_POINT:     value = elementInfo[i].meltingPoint;     break;
        case VARIABLE_TYPE_BOILING_POINT:     value = elementInfo[i].boilingPoint;     break;
        case VARIABLE_TYPE_DENSITY:           value = elementInfo[i].density;          break;
        case VARIABLE_TYPE_GROUP:             value = elementInfo[i].group;            break;
        case VARIABLE_TYPE_IONIZATION_ENERGY: value = elementInfo[i].ionizationEnergy; break;
        case VARIABLE_TYPE_USER_VALUE_1:      value = userInfo[i].value1;              break;
        case VARIABLE_TYPE_USER_VALUE_2:      value = userInfo[i].value2;              break;
        }

        if( value < lastData2 - variableTolerance2 ||
            value > lastData2 + variableTolerance2 )
        {
            match = false;
        }
    }

    // If neither variable caused a match fail, the element has matched.
    return match;
}

/*!
    Button click event.
*/
void QCaPeriodic::userClicked() {
    /// Get the variable to write to
    QCaFloating *qca1 = (QCaFloating*)getQcaItem(0);
    QCaFloating *qca2 = (QCaFloating*)getQcaItem(1);

    /// If a QCa object is present (if there is a variable to write to)
    /// Present the element selection dialog
    /// then write the value
    if( qca1 || qca2 )
    {
        // Build a list of what buttons should be enabled
        // !! This could be build once during construction, or when userInfo enabled is changed??
        QList<bool> enabledList;
        QList<QString> elementList;
        for( int i = 0; i < NUM_ELEMENTS; i++ )
        {
            elementList.append( elementInfo[i].symbol );
            enabledList.append( userInfo[i].enable );
        }

        // Present the element selection dialog
        PeriodicDialog dialog(this);
        dialog.setElement( this->text(), enabledList, elementList );
        dialog.exec();
        QString selection = dialog.getElement();
        if( selection.size() )
        {
            QString symbol = dialog.getElement();
            setText( symbol );

            // Value selected from element info or user info depending on type
            double value;

            for( int i = 0; i < NUM_ELEMENTS; i++ )
            {
                if( elementInfo[i].symbol.compare( symbol ) == 0 )
                {
                    // Write the user values to the variables if present
                    if( qca1 )
                    {
                        switch( variableType1 )
                        {
                        case VARIABLE_TYPE_NUMBER:            value = elementInfo[i].number;           break;
                        case VARIABLE_TYPE_ATOMIC_WEIGHT:     value = elementInfo[i].atomicWeight;     break;
                        case VARIABLE_TYPE_MELTING_POINT:     value = elementInfo[i].meltingPoint;     break;
                        case VARIABLE_TYPE_BOILING_POINT:     value = elementInfo[i].boilingPoint;     break;
                        case VARIABLE_TYPE_DENSITY:           value = elementInfo[i].density;          break;
                        case VARIABLE_TYPE_GROUP:             value = elementInfo[i].group;            break;
                        case VARIABLE_TYPE_IONIZATION_ENERGY: value = elementInfo[i].ionizationEnergy; break;
                        case VARIABLE_TYPE_USER_VALUE_1:      value = userInfo[i].value1;              break;
                        case VARIABLE_TYPE_USER_VALUE_2:      value = userInfo[i].value2;              break;
                        }
                        qca1->writeFloating( value );
                    }
                    if( qca2 )
                    {
                        switch( variableType2 )
                        {
                        case VARIABLE_TYPE_NUMBER:            value = elementInfo[i].number;           break;
                        case VARIABLE_TYPE_ATOMIC_WEIGHT:     value = elementInfo[i].atomicWeight;     break;
                        case VARIABLE_TYPE_MELTING_POINT:     value = elementInfo[i].meltingPoint;     break;
                        case VARIABLE_TYPE_BOILING_POINT:     value = elementInfo[i].boilingPoint;     break;
                        case VARIABLE_TYPE_DENSITY:           value = elementInfo[i].density;          break;
                        case VARIABLE_TYPE_GROUP:             value = elementInfo[i].group;            break;
                        case VARIABLE_TYPE_IONIZATION_ENERGY: value = elementInfo[i].ionizationEnergy; break;
                        case VARIABLE_TYPE_USER_VALUE_1:      value = userInfo[i].value1;              break;
                        case VARIABLE_TYPE_USER_VALUE_2:      value = userInfo[i].value2;              break;
                        }
                        qca2->writeFloating( value );
                    }
                    break;
                }
            }
        }
    }
}

/*!
   Override the default widget isEnabled to allow alarm states to override current enabled state
 */
bool QCaPeriodic::isEnabled() const
{
    // Return what the state of widget would be if connected.
    return localEnabled;
}

/*!
   Override the default widget setEnabled to allow alarm states to override current enabled state
 */
void QCaPeriodic::setEnabled( const bool& state )
{
    // Note the new 'enabled' state
    localEnabled = state;

    // Set the enabled state of the widget only if connected
    if( isConnected )
        QWidget::setEnabled( localEnabled );
}

/*!
   Slot similar to default widget setEnabled slot, but will use our own setEnabled which will allow alarm states to override current enabled state
 */
void QCaPeriodic::requestEnabled( const bool& state )
{
    QCaPeriodic::setEnabled(state);
}

//==============================================================================
// Property convenience functions


// Variable Name and substitution
void QCaPeriodic::setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex )
{
    setVariableNameSubstitutions( variableNameSubstitutionsIn );
    setVariableName( variableNameIn, variableIndex );
    establishConnection( variableIndex );
}

// subscribe
void QCaPeriodic::setSubscribe( bool subscribeIn )
{
    subscribe = subscribeIn;
}
bool QCaPeriodic::getSubscribe()
{
    return subscribe;
}

// variable as tool tip
void QCaPeriodic::setVariableAsToolTip( bool variableAsToolTipIn )
{
    variableAsToolTip = variableAsToolTipIn;
}
bool QCaPeriodic::getVariableAsToolTip()
{
    return variableAsToolTip;
}

// variable 1 type
void QCaPeriodic::setVariableType1( variableTypes variableType1In )
{
    variableType1 = variableType1In;
}
QCaPeriodic::variableTypes QCaPeriodic::getVariableType1()
{
    return variableType1;
}

// variable 2 type
void QCaPeriodic::setVariableType2( variableTypes variableType2In )
{
    variableType2 = variableType2In;
}
QCaPeriodic::variableTypes QCaPeriodic::getVariableType2()
{
    return variableType2;
}

// variable 1 tolerance
void QCaPeriodic::setVariableTolerance1( double variableTolerance1In )
{
    variableTolerance1 = variableTolerance1In;
}
double QCaPeriodic::getVariableTolerance1()
{
    return variableTolerance1;
}

// variable 2 tolerance
void QCaPeriodic::setVariableTolerance2( double variableTolerance2In )
{
    variableTolerance2 = variableTolerance2In;
}
double QCaPeriodic::getVariableTolerance2()
{
    return variableTolerance2;
}

// user info
void QCaPeriodic::setUserInfo( QString inStr )
{
    QTextStream stream;
    stream.setString( &inStr );

    while( stream.atEnd() == false )
    {
        int i;
        stream >> i;

        // If a good element index, get the values
        // Format is 'elementIndex value1 value2 "text" '
        if( i > 0 && i <= NUM_ELEMENTS )
        {
            i--;
            int b;
            stream >> b;
            userInfo[i].enable = b;
            stream >> userInfo[i].value1;
            stream >> userInfo[i].value2;
            QString text;
            stream >> text;
            text = text.mid( 1, text.size()-2 );    // Remove enclosing quotes
            userInfo[i].elementText = restoreWSpace( text );
        }

        // if a bad element index
        else
        {
            break;
        }
    }
}
QString QCaPeriodic::getUserInfo()
{
    QTextStream stream;
    QString outStr;

    // Set up the stream that will perform most conversions
    stream.setString( &outStr );


    // Format is ':elementIndex value1 value2 "text" '
    for( int i = 0; i < NUM_ELEMENTS; i++ )
    {
        stream << i+1;
        stream << " ";
        stream << userInfo[i].enable;
        stream << " ";
        stream << userInfo[i].value1;
        stream << " ";
        stream << userInfo[i].value2;
        stream << " \"";
        stream << hideWSpace( userInfo[i].elementText );
        stream << "\" ";
    }
    return outStr;
}


// Encode white space as characters.
// restoreWSpace() must complement this function.
// Encoding is as follows:
//      Encode '%' as '%%'
//      Encode ' ' as '%s'
QString QCaPeriodic::hideWSpace( QString text )
{
    text = text.replace("%", "%%");
    text = text.replace(" ", "%s");
    return text;
}

// Recover white space from encoded characters
// See hideWSpace() for encoding details
QString QCaPeriodic::restoreWSpace( QString text )
{
    // Examine each position in the string, and if an encoding sequence is found, replace it with the original unencoded character
    for( int i = 0; i < text.size()-1; i++)
    {
        if( text[i].toAscii() == '%')
        {
            switch( text[i+1].toAscii() )
            {
                case '%':
                    text = text.left(i).append( text.right( text.size()-i-1 ) );
                    break;
                case 's':
                    text = text.left(i).append( " " ).append( text.right( text.size()-i-2 ) );
                    break;
            }
        }
    }

    // Return the recoovered string
    return text;
}
