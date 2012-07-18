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
 *  Copyright (c) 2009, 2010
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

/*!
  This class is based on the Qt label widget. It is NOT a CA aware widget.
  It does, however, use the QCaWidget and its VariableNameManager base class to manage substituting
  the label's text in the same way most other widgets manage substitutions in variable names.
  It is tighly integrated with the base class QCaWidget. Refer to QCaWidget.cpp for details, but watch
  out for references to varable names when it is actually just talking about the text that will be
  displayed in the QLabel widget.
  This widget is required to implement some virtual functions required by QCaWidget for
  managing data connections. These functions are just stubs.
 */


#ifndef QSUBSTITUTEDLABEL_H
#define QSUBSTITUTEDLABEL_H

#include <QLabel>
#include <QCaWidget.h>
#include <QCaPluginLibrary_global.h>

class QCAPLUGINLIBRARYSHARED_EXPORT QSubstitutedLabel : public QLabel, public QCaWidget {
    Q_OBJECT

  public:
    QSubstitutedLabel( QWidget *parent = 0 );

    void establishConnection( unsigned int variableIndex ); // Used, but not to connect to data, just used to trigger substitution of the QLabel text



    // Property convenience functions

    // Text substitution (uses variable name and variable name substitution mechanism, even though this widget does not make any connections)
    void setVariableNameAndSubstitutions( QString variableNameIn, QString variableNameSubstitutionsIn, unsigned int variableIndex );

    // label text (prior to substitution)
    void setLabelTextProperty( QString labelTextIn );
    QString getLabelTextProperty();

    // label text (prior to substitution) with line feeds encoded in C style to allow entry in designer
    QString getLabelTextPropertyFormat();
    void setLabelTextPropertyFormat( QString labelTextIn );


  protected:

    QString labelText;                                                 // Fixed text to which substitutions will be applied

private slots:

  public slots:

  signals:

  private:
    void setup();
    qcaobject::QCaObject* createQcaItem( unsigned int ){ return NULL; }; // Not used as this widget does not connect to any data source
    void updateToolTip( const QString& ){};                              // Not used as this widget does not connect to any data source


};

#endif /// QSUBSTITUTEDLABEL_H
