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

#include "PlotSettings.hpp"
#include "ui_PlotSettings.h"

PlotSettings::PlotSettings(uint larvaID, LarvaeContainer *larvaContainer, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotSettings)
{
    
    this->mLarvaContainer = nullptr;
    this->mLastModifiedPlotWindow = nullptr;    
    this->ui->setupUi(this);
    
    this->mLarvaID = larvaID;
    
    this->mLarvaContainer = larvaContainer;
    
    bool oldState = this->ui->comboBox->blockSignals(true);
    this->ui->comboBox->clear();
    this->ui->comboBox->addItems(TrackingParameters::parameterForPlotting);
    this->ui->comboBox->blockSignals(oldState);
    
    this->mCurrentTimeStep = 0;
    
    this->ui->cbLandmarkName->setVisible(false);
    
    connect(this->ui->comboBox,                     SIGNAL(currentIndexChanged(QString)),   this,   SLOT(updatePlotWindow()));
    connect(this->ui->cbLandmarkName,               SIGNAL(currentIndexChanged(QString)),   this,   SLOT(updatePlotWindow()));
    
    connect(this->mLarvaContainer,                  SIGNAL(sendLarvaModelDeleted()),        this,   SLOT(updatePlotWindow()));
    connect(this->mLarvaContainer,                  SIGNAL(sendUpdatedResultLarvaID(uint)), this,   SLOT(updatePlotsOnChange(uint)));
    
    connect(this->ui->btChangePlottingparameter,    SIGNAL(clicked()),                      this,   SLOT(readPlottingParameter()));
    
    this->ui->label->setVisible(false);
    this->ui->label_2->setVisible(false);
    this->ui->label_3->setVisible(false);
    this->ui->label_4->setVisible(false);
    this->ui->label_5->setVisible(false);
    this->ui->label_6->setVisible(false);
    this->ui->label_7->setVisible(false);
    
    this->ui->line->setVisible(false);
    this->ui->line_2->setVisible(false);
    
    this->ui->leTitel->setVisible(false);
    this->ui->leXLabel->setVisible(false);
    this->ui->leXMax->setVisible(false);
    this->ui->leXMin->setVisible(false);
    this->ui->leYLabel->setVisible(false);
    this->ui->leYMax->setVisible(false);
    this->ui->leYMin->setVisible(false);
    
    this->ui->btChangePlottingparameter->setVisible(false);
}

PlotSettings::~PlotSettings()
{
    delete ui;
    foreach(PlotWindow* w, this->mPlotWindows)
    {
        w->close();
    }
}

void PlotSettings::setVisible(bool visible)
{
    QWidget::setVisible(visible);
    if(visible)
    {
        foreach(PlotWindow* w, this->mPlotWindows)
        {
            w->show();
        }
    }
}

void PlotSettings::setCurrentTimeStep(int timeStep)
{
    this->mCurrentTimeStep = timeStep;
    foreach(PlotWindow* w, this->mPlotWindows)
    {
        w->plotCurrentTimeStepMarker(this->mCurrentTimeStep, 
                                     this->mLarvaContainer->getAllTimestepsForPlotting(this->mLarvaID).front(), 
                                     this->ui->comboBox->currentText().compare(TrackingParameters::momentumID) == 0);
    }
}

void PlotSettings::setImageSize(const QSize &size)
{
    this->mImageSize = size;
    foreach(PlotWindow* w, this->mPlotWindows)
    {
        w->setImageSize(this->mImageSize);
    }
}

void PlotSettings::setAvailableLandmarkNames(const QStringList &landmarkNames)
{
    bool oldState = this->ui->cbLandmarkName->blockSignals(true);
    this->ui->cbLandmarkName->clear();
    this->ui->cbLandmarkName->addItems(landmarkNames);
    this->ui->cbLandmarkName->blockSignals(oldState);
}

void PlotSettings::updatePlotWindow()
{    
    QString plotValue = this->ui->comboBox->currentText();
    QString plotWindowID = this->generatePlotWindowID(plotValue);
    
    if(plotValue.compare(TrackingParameters::landmarkID) == 0 && this->ui->cbLandmarkName->count() > 0)
    {
        this->ui->cbLandmarkName->setVisible(true);
    }
    else if(plotValue.compare(TrackingParameters::bearinAngleID) == 0 && this->ui->cbLandmarkName->count() > 0)
    {
        this->ui->cbLandmarkName->setVisible(true);
    }
    else
    {
        this->ui->cbLandmarkName->setVisible(false);
    }
    
    int index;
    if(this->findPlottingWindow(plotWindowID, index))
    {
        PlotWindow *w = this->mPlotWindows.at(index);
        w->setID(this->generatePlotWindowID(plotValue));
        
        QVector<cv::Point> moments;
        QVector<double> x,y;
        switch(TrackingParameters::parameterForPlotting.indexOf(plotValue))
        {           
        // Area
        case 0:
            w->plotDataAsLine(this->mLarvaContainer->getAllTimestepsForPlotting(this->mLarvaID), this->mLarvaContainer->getAllAreaValues(this->mLarvaID));
            break;
            
            // Momentum
        case 1:
            moments = this->mLarvaContainer->getAllMomentumValues(this->mLarvaID);
            foreach(cv::Point p, moments)
            {
                x << (static_cast<double>(p.x));
                y << this->mImageSize.height() - (static_cast<double>(p.y));
            }
            
            w->plotDataAsMarker(x, y);
            break;
            
            // Main Bodybending-Angle
        case 2:
            w->plotDataAsLine(this->mLarvaContainer->getAllTimestepsForPlotting(this->mLarvaID), this->mLarvaContainer->getAllMainBodybendingAngle(this->mLarvaID));
            break;
            
            // Coiled-Indicator
        case 3:
            w->plotDataAsLine(this->mLarvaContainer->getAllTimestepsForPlotting(this->mLarvaID), this->mLarvaContainer->getAllCoiledIndicator(this->mLarvaID));
            break;
            
            // Well-Oriented-Indicator
        case 4:
            w->plotDataAsLine(this->mLarvaContainer->getAllTimestepsForPlotting(this->mLarvaID), this->mLarvaContainer->getAllIsWellOriented(this->mLarvaID));
            break;

            // Perimeter
        case 5:
            w->plotDataAsLine(this->mLarvaContainer->getAllTimestepsForPlotting(this->mLarvaID), this->mLarvaContainer->getAllPerimeter(this->mLarvaID));
            break;
            
            // Distance To Origin
        case 6:
            w->plotDataAsLine(this->mLarvaContainer->getAllTimestepsForPlotting(this->mLarvaID), this->mLarvaContainer->getAllDistanceToOrigin(this->mLarvaID));
            break;
            
            // Momentum Distance
        case 7:
            w->plotDataAsLine(this->mLarvaContainer->getAllTimestepsForPlotting(this->mLarvaID), this->mLarvaContainer->getAllMomentumDistance(this->mLarvaID));
            break;
            
            // Accumulated Distance
        case 8:
            w->plotDataAsLine(this->mLarvaContainer->getAllTimestepsForPlotting(this->mLarvaID), this->mLarvaContainer->getAllAccumulatedDistance(this->mLarvaID));
            break;
            
            // Go-Phase Indicator
        case 9:
            w->plotDataAsLine(this->mLarvaContainer->getAllTimestepsForPlotting(this->mLarvaID), this->mLarvaContainer->getAllGoPhaseIndicator(this->mLarvaID));
            break;
            
            // Left-Bending Indicator
        case 10:
            w->plotDataAsLine(this->mLarvaContainer->getAllTimestepsForPlotting(this->mLarvaID), this->mLarvaContainer->getAllLeftBendingIndicator(this->mLarvaID));
            break;
            
            // Right-Bending Indicator
        case 11:
            w->plotDataAsLine(this->mLarvaContainer->getAllTimestepsForPlotting(this->mLarvaID), this->mLarvaContainer->getAllRightBendingIndicator(this->mLarvaID));
            break;
            
            // Movement Direction
        case 12:
            w->plotDataAsLine(this->mLarvaContainer->getAllTimestepsForPlotting(this->mLarvaID), this->mLarvaContainer->getAllMovementDirection(this->mLarvaID));
            break;
            
            // Distance to Landmark
        case 13:
            w->plotDataAsLine(this->mLarvaContainer->getAllTimestepsForPlotting(this->mLarvaID), this->mLarvaContainer->getAllDistancesToLandmark(this->mLarvaID, this->ui->cbLandmarkName->currentText()));
            break;
            
            // Velocity
        case 14:
            w->plotDataAsLine(this->mLarvaContainer->getAllTimestepsForPlotting(this->mLarvaID), this->mLarvaContainer->getVelocity(this->mLarvaID));
            break;
            
            // Acceleration
        case 15:
            w->plotDataAsLine(this->mLarvaContainer->getAllTimestepsForPlotting(this->mLarvaID), this->mLarvaContainer->getAcceleration(this->mLarvaID));
            break;
            
            // Bearinangle to Landmark
        case 16:
            w->plotDataAsLine(this->mLarvaContainer->getAllTimestepsForPlotting(this->mLarvaID), this->mLarvaContainer->getAllBearingAnglesToLandmark(this->mLarvaID, this->ui->cbLandmarkName->currentText()));
            break;
        }
    }
    this->setCurrentTimeStep(this->mCurrentTimeStep);
    
}

bool PlotSettings::checkForPlottingWindow(QString plotWindoID)
{
    foreach(PlotWindow* w, this->mPlotWindows)
    {
        if(w->getID() == plotWindoID)
        {
            return true;
        }
    }
    return false;
}

bool PlotSettings::findPlottingWindow(QString plotWindoID, int &index)
{
    if(!this->checkForPlottingWindow(plotWindoID))
    {
        this->createPlottingWindow(plotWindoID);
    }
    
    for(int i = 0; i < this->mPlotWindows.size(); ++i)
    {
        if(this->mPlotWindows.at(i)->getID() == plotWindoID)
        {
            index = i;
            this->mLastModifiedPlotWindow = this->mPlotWindows.at(i);
            return true;
        }
    }
    
    return false;
}

void PlotSettings::createPlottingWindow(QString plotWindoID)
{
    this->mPlotWindows.push_back(new PlotWindow(plotWindoID));
    connect(this->mPlotWindows.last(), SIGNAL(destroyed(QString)), this, SLOT(removePlottingWindow(QString)));
    
    this->mPlotWindows.last()->setImageSize(this->mImageSize);
    this->mLastModifiedPlotWindow = this->mPlotWindows.last();
    emit sendNewSubwindowPointer(this->mPlotWindows.last());
}

void PlotSettings::removePlottingWindow(QString windowID)
{
    int index;
    if(this->findPlottingWindow(windowID, index))
    {
        this->mPlotWindows.remove(index);
    }
    
    if(!this->mPlotWindows.isEmpty())
        this->mLastModifiedPlotWindow = this->mPlotWindows.last();
    else
        this->mLastModifiedPlotWindow = nullptr;
}

QString PlotSettings::generatePlotWindowID(QString const& plotValue)
{
    if(plotValue.compare(TrackingParameters::landmarkID) == 0 && this->ui->cbLandmarkName->count() > 0)
    {
        return QString("Larva ").append(QString::number(this->mLarvaID)).append(" -> ").append(plotValue).append(": ").append(this->ui->cbLandmarkName->currentText());
    }
    else if (plotValue.compare(TrackingParameters::bearinAngleID) == 0 && this->ui->cbLandmarkName->count() > 0)
    {
        return QString("Larva ").append(QString::number(this->mLarvaID)).append(" -> ").append(plotValue).append(": ").append(this->ui->cbLandmarkName->currentText());
    }
    else
    {
        return QString("Larva ").append(QString::number(this->mLarvaID)).append(" -> ").append(plotValue);
    }
}

void PlotSettings::updatePlotsOnChange(uint lID)
{
    if(this->mLarvaID == lID)
    {
        this->updatePlotWindow();
    }
}

void PlotSettings::readPlottingParameter()
{
    PlottingParameter p;
    p.titel     = this->ui->leTitel->text();
    
    p.xMin      = this->ui->leXMin->text();
    p.xMax      = this->ui->leXMax->text();
    p.xLabel    = this->ui->leXLabel->text();
    
    p.yMin      = this->ui->leYMin->text();
    p.yMax      = this->ui->leYMax->text();
    p.yLabel    = this->ui->leYLabel->text();
    
    QString plotValue = this->ui->comboBox->currentText();
    QString plotWindowID = this->generatePlotWindowID(plotValue);
    if(checkForPlottingWindow(plotWindowID))
    {
        int i;
        if(this->findPlottingWindow(plotWindowID, i))
        {
            this->mPlotWindows.at(i)->setPlottingParameter(p);
        }
    }
}

