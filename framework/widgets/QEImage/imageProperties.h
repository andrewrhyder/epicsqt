/*  imageProperties.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
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
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef IMAGEPROPERTIES_H
#define IMAGEPROPERTIES_H

#include "QCaDateTime.h"
#include "imageDataFormats.h"
#include <brightnessContrast.h> // Remove this, or extract the general definitions used (eg rgbPixel) into another include file

/*!
 This class manages the image attributes required for generating a QImage from a QByteArray holding CA image data.
 It is used as the base class for the imageProcessor class.
 */
//!!! for all setfunctions below consider LOCKING ACCESS when implimenting multi threading
class imageProperties
{
public:
    imageProperties(); ///< Constructor

    // Rotation
    /// \enum rotationOptions
    /// Image rotation options
    enum rotationOptions { ROTATION_0,          ///< No image rotation
                           ROTATION_90_RIGHT,   ///< Rotate image 90 degrees clockwise
                           ROTATION_90_LEFT,    ///< Rotate image 90 degrees anticlockwise
                           ROTATION_180         ///< Rotate image 180 degrees
                         };

    // Image attribut set and get functions
    void setRotation( rotationOptions rotationIn ){ rotation = rotationIn; }
    rotationOptions getRotation(){ return rotation; }

    void setFlipVert( bool flipVertIn ){ flipVert = flipVertIn; }
    bool getFlipVert(){ return flipVert; }

    void setFlipHoz( bool flipHozIn ){ flipHoz = flipHozIn; }
    bool getFlipHoz(){ return flipHoz; }

    void setImageBuffWidth( unsigned long imageBuffWidthIn ){ imageBuffWidth = imageBuffWidthIn; }
    void setImageBuffHeight( unsigned long imageBuffHeightIn ){ imageBuffHeight = imageBuffHeightIn; }

    unsigned long getImageBuffWidth(){ return imageBuffWidth; }
    unsigned long getImageBuffHeight(){ return imageBuffHeight; }

    imageDataFormats::formatOptions getFormat();
    void setFormat( imageDataFormats::formatOptions formatIn );
    bool setFormat( const QString& text );

    void setBitDepth( unsigned int bitDepthIn )
    {
        // Invalidate pixel look up table if bit depth changes (it will be regenerated with the new depth when next needed)
        if( bitDepth != bitDepthIn )
        {
            pixelLookupValid = false;
        }
        bitDepth = bitDepthIn;
    }
    unsigned int getBitDepth(){ return bitDepth; }

    void setElementsPerPixel( unsigned long elementsPerPixelIn ){ elementsPerPixel = elementsPerPixelIn; }//LOCK ACCESS???

    void setImageDisplayProperties( imageDisplayProperties* imageDisplayPropsIn ){ imageDisplayProps = imageDisplayPropsIn; }

    // Methods to force reprocessing
    void setWidthHeightFromDimensions();  ///< // Update the image dimensions (width and height) from the area detector dimension variables.
    void invalidatePixelLookup(){ pixelLookupValid = false; } ///< recalculate (when next requried) pixel summary information
    QString getInfoText();                ///< Generate textual information regarding the current image

protected:

    imageDisplayProperties* imageDisplayProps;  // Dialog for user manuipulation (and storage of) brightness and contrast and related info

    // Options
    imageDataFormats::formatOptions mFormatOption;
    unsigned int bitDepth;

    // Image and related information
    unsigned long imageDataSize;      // Size of elements in image data (originating from CA data type)
    unsigned long elementsPerPixel;   // Number of data elements per pixel. Derived from image dimension 0 (only when there are three dimensions)
    unsigned long bytesPerPixel;      // Bytes in input data per pixel (imageDataSize * elementsPerPixel)
    QByteArray image;                 // Buffer to hold original image data. WARNING To avoid expensive memory copies, data is
                                      // generated using QByteArray::fromRawData(), where the raw data is the original CA update
                                      // data. This data is held until a subsequent update. The data in this QByteArray will
                                      // be valid only until the next update arrives.
    unsigned long receivedImageSize;  // Size as received on last CA update.
    QString previousMessageText;      // Previous message text - avoid repeats.
    QByteArray imageBuff;             // Buffer to hold data converted to format for generating QImage.
#define IMAGEBUFF_BYTES_PER_PIXEL 4   // 4 bytes for Format_RGB32
    unsigned long imageBuffWidth;     // Original image width (may be generated directly from a width variable, or selected from the relevent dimension variable)
    unsigned long imageBuffHeight;    // Original image height (may be generated directly from a width variable, or selected from the relevent dimension variable)

    unsigned long numDimensions;      // Image data dimensions. Expected values are 0 (all dimensions values are ignored), 2 (one data element per pixel, dimensions are width x height), 3 (multiple data elements per pixel, dimensions are pixel x width x height)
    unsigned long imageDimension0;    // Image data dimension 0. If two dimensions, this is the width, if three dimensions, this is the pixel depth (the elements used to represent each pixel)
    unsigned long imageDimension1;    // Image data dimension 1. If two dimensions, this is the height, if three dimensions, this is the width
    unsigned long imageDimension2;    // Image data dimension 2. If two dimensions, this is not used, if three dimensions, this is the height


    // Pixel information
    bool pixelLookupValid;            // pixelLookup is valid. It is invalid if anything that affects the translation changes, such as pixel format, local brigHtness, etc
    imageDisplayProperties::rgbPixel pixelLookup[256];
    int pixelLow;
    int pixelHigh;

    // Clipping info (determined from cliping variable data)
    bool clippingOn;
    unsigned int clippingLow;
    unsigned int clippingHigh;

    // Flip rotate options
    rotationOptions rotation;   // Rotation option
    bool flipVert;              // True if vertical flip option set
    bool flipHoz;               // True if horizontal flip option set
};

#endif // IMAGEPROPERTIES_H
