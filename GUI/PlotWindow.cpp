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

#include "PlotWindow.hpp"

PlotWindow::PlotWindow(QWidget *parent)
    : QMdiSubWindow(parent)
{
    this->mPlotter = new Plotter(parent);
    this->layout()->addWidget(this->mPlotter);
}

PlotWindow::PlotWindow(const QString &ID, QWidget *parent)
{
    this->mPlotter = new Plotter(parent);
    this->layout()->addWidget(this->mPlotter);
    this->setID(ID);
}

PlotWindow::~PlotWindow()
{
    delete this->mPlotter;
    emit destroyed(this->getID());
}

void PlotWindow::setImageSize(const QSize &size)
{
    this->mPlotter->setImageSize(size);
}

void PlotWindow::plotDataAsLine(const QVector<double> &x, const QVector<double> &y)
{
    this->mPlotter->plotDataAsLine(x, y);
}

void PlotWindow::plotDataAsMarker(const QVector<double> &x, const QVector<double> &y)
{
    this->mPlotter->plotDataAsMarker(x, y);
}

void PlotWindow::plotCurrentTimeStepMarker(double timeStep, double firstTimeStep, bool isMarkerPlot)
{
    this->mPlotter->plotCurrentTimeStepMarker(timeStep, firstTimeStep, isMarkerPlot);
}

void PlotWindow::setPlottingParameter(const PlottingParameter &param)
{
    this->mPlottingParameter = param;
    
    if(!this->mPlottingParameter.xMin.isNull() && !this->mPlottingParameter.xMin.isEmpty() 
            && !this->mPlottingParameter.xMax.isNull() && !this->mPlottingParameter.xMax.isEmpty())
    {
        bool okXMin, okXMax;
        double xmin = this->mPlottingParameter.xMin.toDouble(&okXMin);
        double xmax = this->mPlottingParameter.xMax.toDouble(&okXMax);
        if(okXMin && okXMax)
        {
            this->mPlotter->setXAxisRange(xmin, xmax);
        }
    }
    
    if(!this->mPlottingParameter.xLabel.isNull() && !this->mPlottingParameter.xLabel.isEmpty())
        this->mPlotter->setXAxisLable(this->mPlottingParameter.xLabel);
    
    if(!this->mPlottingParameter.yMin.isNull() && !this->mPlottingParameter.yMin.isEmpty() 
            && !this->mPlottingParameter.yMax.isNull() && !this->mPlottingParameter.yMax.isEmpty())
    {
        bool okYMin, okYMax;
        double ymin = this->mPlottingParameter.yMin.toDouble(&okYMin);
        double ymax = this->mPlottingParameter.yMax.toDouble(&okYMax);
        if(okYMin && okYMax)
        {
            this->mPlotter->setYAxisRange(ymin, ymax);
        }
    }
    
    if(!this->mPlottingParameter.yLabel.isNull() && !this->mPlottingParameter.yLabel.isEmpty())
        this->mPlotter->setYAxisLable(this->mPlottingParameter.yLabel);
}
