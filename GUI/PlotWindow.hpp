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

#ifndef PLOTWINDOW_HPP
#define PLOTWINDOW_HPP

#include <QMdiSubWindow>
#include <QLayout>

#include "Utility/Plotter.hpp"

struct PlottingParameter{
    QString titel;
    QString xMin;
    QString xMax;
    QString xLabel;
    QString yMin;
    QString yMax;
    QString yLabel;
};

class PlotWindow : public QMdiSubWindow
{
    Q_OBJECT
    
private:
    Plotter             *mPlotter;
    QString             mID;
    PlottingParameter   mPlottingParameter;
    
public:
    PlotWindow(QWidget *parent = 0);
    PlotWindow(QString const& ID, QWidget *parent = 0);
    ~PlotWindow();
    
    void        setID(QString const& id){this->mID = id; this->setWindowTitle(this->mID);}
    QString     getID() const {return this->mID;}
    void        setImageSize(QSize const& size);
    
signals:
    void        destroyed(QString);
    
public slots:
    void        plotDataAsLine(QVector<double> const& x, QVector<double> const& y);
    void        plotDataAsMarker(QVector<double> const& x, QVector<double> const& y);
    void        plotCurrentTimeStepMarker(double timeStep, double firstTimeStep, bool isMarkerPlot);
    void        setPlottingParameter(PlottingParameter const& param);
};

#endif // PLOTWINDOW_HPP
