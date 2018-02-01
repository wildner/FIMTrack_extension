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

#ifndef RESULTSVIEWER_HPP
#define RESULTSVIEWER_HPP

#include <QtCore>
#include <QtGui>
#include <vector>

#include "Configuration/FIMTrack.hpp"
#include "Control/Undistorter.hpp"
#include "Control/InputGenerator.hpp"
#include "Control/LarvaeContainer.hpp"

#include "Data/Larva.hpp"

#include "TrackerGraphicsView.hpp"
#include "TrackerScene.hpp"
#include "LarvaTab.hpp"
#include "PlottingTab.hpp"

#include "PlotSettings.hpp"
#include "Utility/Plotter.hpp"

namespace Ui {
class ResultsViewer;
}

class ResultsViewer : public QDialog
{
    Q_OBJECT
    
public:
    explicit ResultsViewer(QWidget *parent = 0);
    ~ResultsViewer();
    
    LarvaeContainer* getLarvaeContainer() {return &this->mLarvaeContainer;}
    
signals:
    void newTimeStep(unsigned int);
    void sendShortestLarvaeTrackLength(uint);
    void sendAvailableLarvaIDs();
    
    void sendCroppedImage(QImage const&);
    void sendPlottingTimeStemp(int);
    
    void sendNewImageSize(QSize);
    
    void sendMaximumNumberOfTimePoints(int);
    
public slots:
    void cropImage(uint larvaID);
    
private slots:    
    void on_horizontalSlider_images_valueChanged(int value);
    
    void goOneTimeStepPrev();
    void goOneTimeStepNext();

    void moveSliderSlot(bool forward = true);
    void bringLarvaIntoFocus(qreal x, qreal y);
    void playPause();
    
    void resetView();
    
    void loadAllResults();
    bool loadImageFiles();
    bool loadYmlFile();
    void initUndistorer();
    void setupBaseGUIElements();
    void setupLarvaeTabs();
    
    void addTab();
    void updateLarvaeTabs();
    void updateLarvaePath(uint larvaID);
    void removeTabByLarvaID(uint larvaID);
    void removeTabByIndex(int tabIndex);
    bool findLarvaTab(uint larvaID, int &index);
    void updateLarvaPointer();
    
    void saveResultLarvae();
    
    void showImage(int const index);
    void showTable(int const index);
    
    void setupConnections();
    void connectTab(int index);
    void disconnectTab(int index);
    
    void resetAndClear();
    
    void removeAllLarvaeModelsWhichAreShorterThen();
    
    void adjustTimeSlider(int tabID);
    void adjustPlottingValues(QString larvaID, int currentTimeStep);
    
private:
    
    QSize                                   mImageSize;
    
    QToolButton*                            mBtnAddTab;
    
    bool                                    mDrawOpenCV;
    
    unsigned int                            mCurrentTimestep;
    
    Ui::ResultsViewer*                      ui;
    QStringList                             mFileNames;
    QString                                 mYmlFileName;
    QString                                 mUndistFileName;
    
    TrackerScene*                           mScene;

    std::vector<std::string>                mImgPaths;
    bool                                    mUseUndist;

    int                                     mNumberOfImages;
    
    bool                                    mPlayingModeOn;
    
    QTimer*                                 mTimer;
    int                                     mZoomFactor;
    Undistorter                             mUndistorer;
    LarvaeContainer                         mLarvaeContainer;
    
    bool                                    mPlottingTabVisible;
    int                                     mCurrentTimePointForPlotting;
    QPair<int,int>                          mTimeIntervalForPlotting;
    uint                                    mLarvaIDForCropping;
};

#endif // RESULTSVIEWER_HPP
