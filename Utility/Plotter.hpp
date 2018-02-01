/*****************************************************************************
 * Copyright (c) 2011-2016 The FIMTrack Team as listed in CREDITS.txt        *
 * http://fim.uni-muenster.de                                             	 *
 *                                                                           *
 * This file is part of FIMTrack.                                            *
 * FIMTrack is available under multiple licenses.                            *
 * The different licenses are subject to terms and condition as provided     *
 * in the files specifying the license. See "LICENSE.txt" for details        *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * FIMTrack is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation, either version 3 of the License, or         *
 * (at your option) any later version. See "LICENSE-gpl.txt" for details.    *
 *                                                                           *
 * FIMTrack is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
 * GNU General Public License for more details.                              *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * For non-commercial academic use see the license specified in the file     *
 * "LICENSE-academic.txt".                                                   *
 *                                                                           *
 *****************************************************************************
 *                                                                           *
 * If you are interested in other licensing models, including a commercial-  *
 * license, please contact the author at fim@uni-muenster.de      			 *
 *                                                                           *
 *****************************************************************************/

#ifndef PLOTTER_HPP
#define PLOTTER_HPP

#include "qcustomplot.h"

class Plotter : public QCustomPlot
{
    
private:
    QVector<double>     _xLine;
    QVector<double>     _yLine;
    
    QVector<double>     _xMarker;
    QVector<double>     _yMarker;
    
    double              _markerSize;
    
    QSize               _imageSize;
    
public:
    Plotter(QWidget *parent = 0);
    
    void setXAxisLable(QString const& lable);
    void setYAxisLable(QString const& lable);
    
    void setXAxisRange(double from, double to);
    void setYAxisRange(double from, double to);
    
    void setImageSize(QSize const& size) {this->_imageSize = size;}
    
public slots:
    void plotDataAsLine(QVector<double> const& x, QVector<double> const& y);
    void plotDataAsMarker(QVector<double> const& x, QVector<double> const& y);
    void plotCurrentTimeStepMarker(double timeStep, double firstTimeStep, bool isMarkerPlot);
    void reset();
    
};

#endif // PLOTTER_HPP
