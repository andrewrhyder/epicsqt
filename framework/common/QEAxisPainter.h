/*  QEAnalogSlider.h
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

#include <QCaObject.h>
#include <QEPluginLibrary_global.h>

// This is (currently) as support widget used by QAnalogIndicator/QAnalogSlider.
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


   explicit QEAxisPainter (QWidget* parent = 0);
   ~QEAxisPainter () { }

   void setMinimum (const double minimum);
   double getMinimum () const;

   void setMaximum (const double maximum);
   double getMaximum () const;

   void setMinorInterval (const double minorInterval);
   double getMinorInterval  () const;

   void setMajorMinorRatio (const int majorMinorRatio);
   int getMajorMinorRatio  () const;

   void setLogScale (const bool isLog);
   bool getLogScale () const;

   void setOrientation (const Orientations orientation);
   Orientations getOrientation () const;

   void setTextPosition (const TextPositions textPositions);
   TextPositions getTextPosition () const;

   void setColour (const QColor colour);
   QColor getColour () const;

   void setIndent (const int indent);
   int getIndent  () const;

   void setGap (const int gap);
   int getGap  () const;

   struct ColourBand {
      double lower;
      double upper;
      QColor colour;
   };

   typedef QList <ColourBand> ColourBandLists;

   void setColourBandList (const ColourBandLists& bandList);
   ColourBandLists getColourBandList () const;

   // Conveniance function to calculate a band list based upon alarm levels
   // from within the given QCaObject.
   //
   ColourBandLists calcAlarmColourBandList (qcaobject::QCaObject* qca);

protected:
   void paintEvent (QPaintEvent *event);

private:
   // Value iterator.
   // itc is the iterator control value.
   //
   bool firstValue (int& itc, double& value, bool& isMajor);
   bool nextValue  (int& itc, double& value, bool& isMajor);

   bool isLeftRight () const;
   double calcFraction (const double value);

   void drawAxisText (QPainter& painter, const QPoint& position,
                      const QString& text, const int pointSize);

   ColourBand createColourBand (const double lower, const double upper, const QColor& colour);
   ColourBand createColourBand (const double lower, const double upper, const unsigned short severity);

   ColourBandLists bandList;
   QColor mColour;
   double mMinimum;
   double mMaximum;
   double mMinorInterval;
   int mMajorMinorRatio;
   int mIndent;
   int mGap;
   Orientations mOrientation;
   TextPositions mTextPosition;
   bool mIsLogScale;
};

#endif  // QE_AXIS_PAINTER_H
