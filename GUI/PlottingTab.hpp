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

#ifndef PLOTTINGTAB_HPP
#define PLOTTINGTAB_HPP

#include <QWidget>
#include <QRegExp>

#include "PlotSettings.hpp"
#include "Utility/Plotter.hpp"

#include "Control/LarvaeContainer.hpp"

namespace Ui {
class PlottingTab;
}

class PlottingTab : public QWidget
{
    Q_OBJECT
    
public:
    explicit PlottingTab(QWidget *parent = 0);
    ~PlottingTab();
    
    void setLarvaeContainerPointer(LarvaeContainer *larvaeContainer);
    
public slots:
    void setAvailableLarvaIDs();
    void setAvailableLandmarkNames(QStringList const& landmarkNames);
    void showCroppedImage(QImage const& img);
    void bookmarkCroppedTimeStemp(int timeStemp);
    void setImageSize(QSize size);
    
    void addSubWindow(PlotWindow* w);
    
private slots:
    void adjustPlottingSettings(QString larvaID);
    void setRightLarvaID(QMdiSubWindow* subWindow);
    int findComboboxIndex(QString larvaID);
    
    void removePlottingSettings(uint larvaID);
signals:
    void sendCurrentPlottingParameter(QString, int);
    
private:
    Ui::PlottingTab *ui;
    QVector<PlotSettings*>                  mPlottingSettings;
    LarvaeContainer                         *mLarvaeContainer;
    
    QSize                                   mImageSize;
    QStringList                             mLandmarkNames;
    
    void                                    setupConnections();
    
    bool                                    checkForPlottingSettings(uint larvaID);
    bool                                    findPlottingSettings(uint larvaID, int &index);
    void                                    createPlottingSettings(uint larvaID);
};

#endif // PLOTTINGTAB_HPP
