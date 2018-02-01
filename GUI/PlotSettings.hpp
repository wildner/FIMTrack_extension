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

#ifndef PLOTSETTINGS_HPP
#define PLOTSETTINGS_HPP

#include <QWidget>

#include "Configuration/TrackerConfig.hpp"
#include "PlotWindow.hpp"
#include "Control/LarvaeContainer.hpp"

namespace Ui {
    class PlotSettings;
}

class PlotSettings : public QWidget
{
    Q_OBJECT
    
public:
    explicit PlotSettings(uint larvaID, LarvaeContainer *larvaContainer, QWidget *parent = 0);
    ~PlotSettings();
 
    
    uint                    getLarvaID() const {return this->mLarvaID;}
    PlotWindow*             getLastModifiedPlotWindowPointer() const {return this->mLastModifiedPlotWindow;}
    
    int                     getCurrentTimeStep() const {return this->mCurrentTimeStep;}
    
    void                    setVisible(bool visible);
    void                    setCurrentTimeStep(int timeStep);
    void                    setImageSize(QSize const& size);
    
    void                    setAvailableLandmarkNames(QStringList const& landmarkNames);
    
    
public slots:
    void                    updatePlotWindow();
signals:
    void sendNewSubwindowPointer(PlotWindow*);
    void sendNewPlottingParameter(PlottingParameter const& p);
    
private slots:
    
    bool                    checkForPlottingWindow(QString plotWindoID);
    bool                    findPlottingWindow(QString plotWindoID, int &index);
    void                    createPlottingWindow(QString plotWindoID);
    
    void                    removePlottingWindow(QString windowID);
    
    QString                 generatePlotWindowID(QString const& plotValue);
    
    void                    updatePlotsOnChange(uint lID);
    
    void                    readPlottingParameter();
    
private:
    uint                    mLarvaID;
    int                     mCurrentTimeStep;
    
    QSize                   mImageSize;
    
    LarvaeContainer         *mLarvaContainer;
    PlotWindow              *mLastModifiedPlotWindow;
    QVector<PlotWindow*>    mPlotWindows;
    Ui::PlotSettings        *ui;
};

#endif // PLOTSETTINGS_HPP
