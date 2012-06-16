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
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

#ifndef QCAIMAGE_H
#define QCAIMAGE_H

#include <QScrollArea>
#include <QCaWidget.h>
#include <videowidget.h>
#include <QCaLabel.h>
#include <QVBoxLayout>
#include <QGridLayout>

#include <QCaPluginLibrary_global.h>
#include <QCaIntegerFormatting.h>


class QCAPLUGINLIBRARYSHARED_EXPORT QCaImage : public QFrame, public QCaWidget {
    Q_OBJECT

  public:

    QCaImage( QWidget *parent = 0 );
    QCaImage( const QString &variableName, QWidget *parent = 0 );
    ~QCaImage();

    bool isEnabled() const;
    void setEnabled( bool state );

    // Property convenience functions

    // Variable Name and substitution
    void setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );

    // variable as tool tip
    void setVariableAsToolTip( bool variableAsToolTip );
    bool getVariableAsToolTip();

    // visible (widget is visible outside 'Designer')
    void setRunVisible( bool visibleIn );
    bool getRunVisible();

    // Allow user to drop new PVs into this widget
    void setAllowDrop( bool allowDropIn );
    bool getAllowDrop();

    // Allow user to set the video format
    enum formatOptions{ GREY8, GREY12, RGB_888 };
    void setFormatOption( formatOptions formatOption );
    formatOptions getFormatOption();

    // Size options
    enum sizeOptions { SIZE_OPTION_ZOOM, SIZE_OPTION_FIT, SIZE_OPTION_RESIZE };
    void setSizeOption( sizeOptions sizeOptionIn );
    sizeOptions getSizeOption();

    // Zoom
    void setZoom( int zoomIn );
    int getZoom();

    // Rotation
    void setRotation( double rotationIn );
    double getRotation();

    void setInitialHozScrollPos( int initialHosScrollPosIn );
    int getInitialHozScrollPos();

    void setInitialVertScrollPos( int initialVertScrollPosIn );
    int getInitialVertScrollPos();


    void setDisplayRegionOfInterest( bool displayRoiLayoutIn );
    bool getDisplayRegionOfInterest();

    // Display the acquire period
    void setDisplayAcquirePeriod( bool displayAcquirePeriodIn );
    bool getDisplayAcquirePeriod();

    // Display the exposure time
    void setDisplayExposureTime( bool displayExposureTimeIn );
    bool getDisplayExposureTime();


    void setShowButtonPause(bool pValue);
    bool getShowButtonPause();

    void setShowButtonSave(bool pValue);
    bool getShowButtonSave();

    void setShowTime(bool pValue);
    bool getShowTime();

    void setShowTimeColor(QColor pValue);
    QColor getShowTimeColor();



  protected:
    QCaIntegerFormatting integerFormatting; // Integer formatting options.
    bool caEnabled;

    void establishConnection( unsigned int variableIndex );

    bool caVisible;               // Flag true if the widget should be visible outside 'Designer'

    bool allowDrop;

    enum variableIndexes{ IMAGE_VARIABLE, WIDTH_VARIABLE, HEIGHT_VARIABLE, ACQUIREPERIOD_VARIABLE, EXPOSURETIME_VARIABLE, ROI_X_VARIABLE, ROI_Y_VARIABLE, ROI_W_VARIABLE, ROI_H_VARIABLE, QCAIMAGE_NUM_VARIABLES };

    sizeOptions sizeOption;
    int zoom;
    double rotation;
    int initialHozScrollPos;
    int initialVertScrollPos;
    bool displayAcquirePeriod;
    bool displayExposureTime;
    bool displayRoiLayout;


private slots:
    void connectionChanged( QCaConnectionInfo& connectionInfo );
    void setImage( const QByteArray& image, unsigned long dataSize, QCaAlarmInfo&, QCaDateTime&, const unsigned int& );
    void setDimension( const long& value, QCaAlarmInfo& alarmInfo, QCaDateTime&, const unsigned int& variableIndex);

    void buttonPauseClicked();

    void buttonSaveClicked();


  public slots:
    void requestEnabled( const bool& state );

  signals:
    void dbValueChanged( const QString& out );
    void requestResend();

  private:
    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int variableIndex );
    void updateToolTip( const QString& tip );


    QCAALARMINFO_SEVERITY lastSeverity;
    bool isConnected;

    void setImageBuff();

    QVBoxLayout *mainLayout;
    QGridLayout *labelLayout;
    QGridLayout *roiLayout;
    QGridLayout *buttonLayout;

    QCaLabel* acquirePeriodQCaLabel;
    QLabel* acquirePeriodLabel;

    QCaLabel* exposureTimeQCaLabel;
    QLabel* exposureTimeLabel;

    QCaLabel* roiXQCaLabel;
    QLabel* roiXLabel;

    QCaLabel* roiYQCaLabel;
    QLabel* roiYLabel;

    QCaLabel* roiWQCaLabel;
    QLabel* roiWLabel;

    QCaLabel* roiHQCaLabel;
    QLabel* roiHLabel;

    QScrollArea* scrollArea;
    bool initScrollPosSet;

    VideoWidget *videoWidget;

    QColor qColorShowTime;
    QPushButton *qPushButtonPause;
    QPushButton *qPushButtonSave;

    bool pauseEnabled;
    bool showTimeEnabled;


    void manageRoiLayout();
    void manageAcquirePeriodLabel();
    void manageExposureTimeLabel();

    QByteArray imageBuff;
#define IMAGEBUFF_BYTES_PER_PIXEL 4   // 4 bytes for Format_RGB32
    unsigned long imageBuffWidth;
    unsigned long imageBuffHeight;

    formatOptions formatOption;

    // Drag and Drop
protected:
    void dragEnterEvent(QDragEnterEvent *event) { qcaDragEnterEvent( event ); }
    void dropEvent(QDropEvent *event)           { qcaDropEvent( event ); }
    // Don't drag from interactive widget void mousePressEvent(QMouseEvent *event)    { qcaMousePressEvent( event ); }
    void setDropText( QString text );
    QString getDropText();

    void resizeEvent(QResizeEvent* );

};

#endif // QCAIMAGE_H
