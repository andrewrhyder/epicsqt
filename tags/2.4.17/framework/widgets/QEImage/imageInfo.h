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
 *  Copyright (c) 2013
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include <QGridLayout>
#include <QLabel>

class imageInfo
{
public:
    imageInfo();                                            // Initialise the information area
    void showInfo( bool show );

    QLayout* getInfoWidget();                               // Return the layout of the infomation area for insertion into the main QEImage widget
    void infoShow( const bool show );                       // Display or hide the contents of the information area

    void infoUpdateTarget();                                // Clear the target information
    void infoUpdateTarget( const int x, const int y );      // Update the target information

    void infoUpdateBeam();                                  // Clear the beam information
    void infoUpdateBeam( const int x, const int y );        // Update the beam information

    void infoUpdateVertProfile();                                                                   // Clear the vertical profile information
    void infoUpdateVertProfile( const int x, const unsigned int thickness );                        // Update the vertical profile information

    void infoUpdateHozProfile();                                                                    // Clear the horizontal profile information
    void infoUpdateHozProfile( const int y, const unsigned int thickness );                         // Update the horizontal profile information

    void infoUpdateProfile();                                                                       // Clear the arbitrarty profile information
    void infoUpdateProfile( const QPoint start, const QPoint end, const unsigned int thickness );   // Update the arbitrary profile information

    void infoUpdateRegion( const unsigned int region );                                             // Clear the region information
    void infoUpdateRegion( const unsigned int region, const int x1, const int y1, const int x2, const int y2 );// Update the region information

    void infoUpdatePixel();                                 // Clear the current pixel information
    void infoUpdatePixel( const QPoint pos, int value );    // Update the current pixel information


private:
    QGridLayout* infoLayout;
    QLabel* currentCursorPixelLabel;
    QLabel* currentVertPixelLabel;
    QLabel* currentHozPixelLabel;
    QLabel* currentLineLabel;
    QLabel* currentArea1Label;
    QLabel* currentArea2Label;
    QLabel* currentArea3Label;
    QLabel* currentArea4Label;
    QLabel* currentTargetLabel;
    QLabel* currentBeamLabel;
};

#endif // IMAGEINFO_H