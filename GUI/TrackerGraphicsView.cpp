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

#include "TrackerGraphicsView.hpp"

TrackerGraphicsView::TrackerGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    this->mContextMenuEnabled = false;
    this->mContextMenuLandmarkEnabled = false;
    this->mContextMenuROIEnabled = false;
    this->mScene = new TrackerScene(this);
    this->setScene(this->mScene);
}

TrackerGraphicsView::TrackerGraphicsView(TrackerScene *scene, QWidget *parent) : QGraphicsView(parent)
{
    this->mContextMenuEnabled = false;
    this->mContextMenuLandmarkEnabled = false;
    this->mContextMenuROIEnabled = false;
    this->mScene = scene;
    this->setScene(this->mScene);
}

TrackerGraphicsView::~TrackerGraphicsView()
{
}

TrackerScene *TrackerGraphicsView::getScene()
{
    return this->mScene;
}

void TrackerGraphicsView::setScale(const qreal scale)
{
    QTransform transform = this->transform();
    this->scale(scale/transform.m11(), scale/transform.m22());
}

void TrackerGraphicsView::enableContextMenu(bool enable)
{
    this->mContextMenuEnabled = enable;
}

void TrackerGraphicsView::enableLandmarkContextMenu()
{
    this->mLandmarkMenu             = new QMenu(this);
    
    this->mActionPointLandmark      = new QAction("Point", this);
    connect(this->mActionPointLandmark, SIGNAL(triggered()), this, SLOT(addPointLandmark()));
    
    this->mActionLineLandmark       = new QAction("Line", this);
    connect(this->mActionLineLandmark, SIGNAL(triggered()), this, SLOT(addLineLandmark()));
    
    this->mActionRectLandmark       = new QAction("Rectangle", this);
    connect(this->mActionRectLandmark, SIGNAL(triggered()), this, SLOT(addRectLandmark()));
    
    this->mActionEllipseLandmark    = new QAction("Ellipse", this);
    connect(this->mActionEllipseLandmark, SIGNAL(triggered()), this, SLOT(addEllipseLandmark()));
    
    this->mLandmarkMenu->addAction(this->mActionPointLandmark);
    this->mLandmarkMenu->addAction(this->mActionLineLandmark);
    this->mLandmarkMenu->addAction(this->mActionRectLandmark);
    this->mLandmarkMenu->addAction(this->mActionEllipseLandmark);
    
    this->mActionLandmark           = new QAction(QIcon(":/ResultViewer/Icons/Icons/landmark.png"), "Add Landmark here", this);
    this->mActionLandmark->setMenu(this->mLandmarkMenu);
    this->mContextMenu.addAction(this->mActionLandmark);
    
    
    this->mActionRemoveLandmark = new QAction(QIcon(":/ResultViewer/Icons/Icons/removeLandmark.png"), "Remove this Landmark", this);	
    this->mContextMenu.addAction(this->mActionRemoveLandmark);
    connect(this->mActionRemoveLandmark, SIGNAL(triggered()), this, SLOT(removeLandmark()));
    
    this->mContextMenuLandmarkEnabled = true;
}

void TrackerGraphicsView::enableRIOContextMenu()
{
    this->mContextMenu.addSeparator();
    
    this->mROIMenu                  = new QMenu(this);
    this->mActionAddRectROI = new QAction(tr("Rectangular ROI"), this);
    connect(this->mActionAddRectROI, SIGNAL(triggered()), this, SLOT(addRectROI()));
    
    this->mActionAddEllipseROI = new QAction(tr("Elliptical ROI"), this);
    connect(this->mActionAddEllipseROI, SIGNAL(triggered()), this, SLOT(addEllipseROI()));
    
    this->mROIMenu->addAction(this->mActionAddRectROI);
    this->mROIMenu->addAction(this->mActionAddEllipseROI);
    
    this->mActionROI           = new QAction(QIcon(":/ResultViewer/Icons/Icons/addRegionOfInterest.png"), "Add ROI here", this);
    this->mActionROI->setMenu(this->mROIMenu);
    this->mContextMenu.addAction(this->mActionROI);
    
    this->mActionRemoveROI = new QAction(QIcon(":/ResultViewer/Icons/Icons/removeRegionOfInterest.png"), tr("Remove ROI"), this);
    this->mContextMenu.addAction(this->mActionRemoveROI);
    connect(this->mActionRemoveROI, SIGNAL(triggered()), this, SLOT(removeROI()));
    
    this->mContextMenuROIEnabled = true;
}

void TrackerGraphicsView::wheelEvent(QWheelEvent *event)
{
    if(event->modifiers().testFlag(Qt::ControlModifier))
    {
        int numDegrees = event->delta() / 8;
        int numSteps = numDegrees / 15;
        
        if(numSteps <= 0)
        {
            event->ignore();
        }
        
        qreal factor = 1.0 + qreal(numSteps) / 300.0;
        
        if(event->modifiers().testFlag(Qt::ShiftModifier))
        {
            factor += qreal(numSteps) / 10.0;
        }
        
        if(factor > 0.001)
        {
            this->scale(factor, factor);
            event->accept();
        }
        else
        {
            event->ignore();
        }
    }
    else
    {
        QGraphicsView::wheelEvent(event);
    }
}

void TrackerGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    if(this->mContextMenuEnabled)
    {
        this->mLastContextMenuPosition = this->mapToScene(event->pos());
        
        if(this->mContextMenuLandmarkEnabled)
        {
            
            this->mActionLandmark->setEnabled(true);
            this->mActionRemoveLandmark->setEnabled(false);
            
            //Landmark sensitive options
            if(this->mScene->landmarkContainsPoint(this->mLastContextMenuPosition))
            {
                //option to delte landmark
                this->mActionLandmark->setEnabled(false);
                this->mActionRemoveLandmark->setEnabled(true);
            }
        }
        
        if(this->mContextMenuROIEnabled)
        {
            this->mActionROI->setEnabled(true);
            this->mActionRemoveROI->setEnabled(false);
            //ROI sensitive options
            if(this->mScene->ROIContainsPoint(this->mLastContextMenuPosition))
            {
                //option to delte landmark
                this->mActionROI->setEnabled(false);
                this->mActionRemoveROI->setEnabled(true);
            }
        }
        this->mContextMenu.popup(event->globalPos());
    }
}

void TrackerGraphicsView::addPointLandmark()
{
    this->mScene->addLandmark(this->mLastContextMenuPosition, Landmark::POINT);
}

void TrackerGraphicsView::addLineLandmark()
{
    this->mScene->addLandmark(this->mLastContextMenuPosition, Landmark::LINE);
}

void TrackerGraphicsView::addRectLandmark()
{
    this->mScene->addLandmark(this->mLastContextMenuPosition, Landmark::RECTANGLE);
}

void TrackerGraphicsView::addEllipseLandmark()
{
    this->mScene->addLandmark(this->mLastContextMenuPosition, Landmark::ELLIPSE);
}

void TrackerGraphicsView::removeLandmark()
{
    this->mScene->removeLandmark(this->mLastContextMenuPosition);
}

void TrackerGraphicsView::addRectROI()
{
    this->mScene->addROI(this->mLastContextMenuPosition, RegionOfInterest::RECTANGLE);
}

void TrackerGraphicsView::addEllipseROI()
{
    this->mScene->addROI(this->mLastContextMenuPosition, RegionOfInterest::ELLIPSE);
}

void TrackerGraphicsView::removeROI()
{
    this->mScene->removeROI(this->mLastContextMenuPosition);
}

void TrackerGraphicsView::setJobName(QString const& jobName)
{
    this->mCurrentJobName = jobName;
}
