/*  QEAxisPainter.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2015 Australian Synchrotron
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrew.starritt@synchrotron.org.au
 */

#ifndef QE_AXIS_PAINTER_H
#define QE_AXIS_PAINTER_H

#include <QColor>
#include <QList>
#include <QObject>
#include <QWidget>
#include <QEAxisIterator.h>
#include <QEColourBandList.h>

#include <QEPluginLibrary_global.h>

// Support widget for QAnalogSlider (and maybe in future) for QAnalogIndicator
// It could be promoted to a plugin widget in its own right if ever necessary.
//
class QEPLUGINLIBRARYSHARED_EXPORT QEAxisPainter : public QWidget {
   Q_OBJECT
public:
   /// \enum    Orientations
   /// The orientation of the axis
   enum Orientations { Left_To_Right,      ///< Left to right
                       Top_To_Bottom,      ///< Top to bottom
                       Right_To_Left,      ///< Right to left
                       Bottom_To_Top       ///< Bottom to top
                   };
   Q_ENUMS (Orientations)

   enum TextPositions { BelowLeft,  ///< Below when Orientation is horizontal, Left when Orientation is vertical
                        AboveRight  ///< Above when Orientation is horizontal, Right when Orientation is vertical
                      };
   Q_ENUMS (TextPositions)

#define NUMBER_OF_MARKERS   4


   explicit QEAxisPainter (QWidget* parent = 0);
   ~QEAxisPainter ();

   // Set/get minimum axis limit. Default is 0.0
   //
   void setMinimum (const double minimum);
   double getMinimum () const;

   // Set/get maximum axis limit. Default is 10.0
   //
   void setMaximum (const double maximum);
   double getMaximum () const;

   // Set/get minor tick interval. Altough not enforced, this is most sensibly
   // something like 0.01, 0.5, 1.0.  Default is 0.2
   //
   void setMinorInterval (const double minorInterval);
   double getMinorInterval  () const;

   // Set/get ratio of major to minor intervals.
   // Constrained to be >= 1.  Default is 5.
   //
   void setMajorMinorRatio (const int majorMinorRatio);
   int getMajorMinorRatio  () const;

   // Set/get log scaling. Default is false (i.e. linear scaling)
   //
   void setLogScale (const bool isLog);
   bool getLogScale () const;

   // Set/get axis orientation. Default is Left_To_Right.
   //
   void setOrientation (const Orientations orientation);
   Orientations getOrientation () const;

   // Set/get test position with respect to the axis. Default is BelowLeft.
   //
   void setTextPosition (const TextPositions textPositions);
   TextPositions getTextPosition () const;

   // Set/get pen colour. Default is black.
   //
   void setPenColour (const QColor colour);
   QColor getPenColour () const;

   // Set/get axis start/finish indentation with respect to widget width when the
   // orientation is horizontal, or with respect to widget height when the orientation
   // is vertical. Default is 20.
   //
   void setIndent (const int indent);
   int getIndent  () const;

   // Set/get axis gap or margin from top edge of the widget when the orientation is
   // horizontal, or the vertical gap from the left/right edge when the orientation
   // vertical. Default is 2.
   //
   void setGap (const int gap);
   int getGap  () const;

   // Set/get marker attributes. Index is constrained to the range 0 .. 3.
   // Default values are clear; false; and  0.0.
   //
   void setMarkerColour (const int index, const QColor& colour);
   QColor getMarkerColour (const int index) const;

   void setMarkerVisible (const int index, const bool isVisible);
   bool getMarkerVisible (const int index) const;

   void setMarkerValue (const int index, const double value);
   double getMarkerValue (const int index) const;

   // Set/get the band background colours. Default is an empty list.
   //
   void setColourBandList (const QEColourBandList& bandList);
   QEColourBandList getColourBandList () const;

protected:
   void paintEvent (QPaintEvent *event);

private:
   bool isLeftRight () const;
   double calcFraction (const double value);

   void drawAxisText (QPainter& painter, const QPoint& position,
                      const QString& text, const int pointSize);

   QColor markerColour [NUMBER_OF_MARKERS];
   bool   markerVisible [NUMBER_OF_MARKERS];
   double markerValue [NUMBER_OF_MARKERS];

   QEColourBandList bandList;
   QColor mPenColour;
   double mMinimum;
   double mMaximum;
   double mMinorInterval;
   int mMajorMinorRatio;
   int mIndent;
   int mGap;
   Orientations mOrientation;
   TextPositions mTextPosition;
   bool mIsLogScale;
   QEAxisIterator* iterator;
};


#endif  // QE_AXIS_PAINTER_H
