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

#include "LandmarkContainer.hpp"

LandmarkContainer::LandmarkContainer(QGraphicsScene *parentScene)
{
    this->mParentScene = parentScene;
}

LandmarkContainer::~LandmarkContainer()
{
    while(this->mLandmarks.size() > 0)
    {
        Landmark* lndMk = this->mLandmarks.first();
        this->mParentScene->removeItem(lndMk);
        this->mLandmarks.remove(0);
        lndMk->cleanupAfterRemoving();
        delete lndMk; 
    }
}

void LandmarkContainer::addLandmark(QString const& name, QPointF const& p, Landmark::Type type)
{
    Landmark* l;
    if(name.isNull() || name.isEmpty())
    {
        l = new Landmark("Landmark " + QString::number(this->mLandmarks.size()), p.x(), p.y(), type);
    }
    else
    {
         l = new Landmark(name, p.x(), p.y(), type);
    }
    this->mLandmarks.push_back(l);
    this->mParentScene->addItem(l);
    connect(l, SIGNAL(moved(const Landmark*)), this, SLOT(getMovedSignal(const Landmark*)));
    emit landmarkAdded(l);
    emit sendAllLandmarkNames(this->getAllLandmarkNames());
}

void LandmarkContainer::addLandmark(Landmark *l)
{
    this->mLandmarks.push_back(l);
    this->mParentScene->addItem(l);
    connect(l, SIGNAL(moved(const Landmark*)), this, SLOT(getMovedSignal(const Landmark*)));
    emit landmarkAdded(l);
    emit sendAllLandmarkNames(this->getAllLandmarkNames());
}

void LandmarkContainer::removeLandmak(const QPointF &p)
{
    for(int i = 0; i < this->mLandmarks.size(); ++i)
    {
        if(this->mLandmarks.at(i)->contains(p))
        {
            Landmark* lndMk = this->mLandmarks.at(i);
            QString name = lndMk->getName();
            this->mParentScene->removeItem(lndMk);
            this->mLandmarks.remove(i);
            lndMk->cleanupAfterRemoving();
            disconnect(lndMk, SIGNAL(moved(const Landmark*)), this, SLOT(getMovedSignal(const Landmark*)));
            delete lndMk; 
            emit landmarkRemoved(name);
            emit sendAllLandmarkNames(this->getAllLandmarkNames());
            return;
        }
    }
}

bool LandmarkContainer::contains(const QPointF &p) const
{
    for(int i = 0; i < this->mLandmarks.size(); ++i)
    {
        if(this->mLandmarks.at(i)->contains(p))
        {
            return true;
        }
    }
    return false;
}

QStringList LandmarkContainer::getAllLandmarkNames() const
{
    QStringList res;
    foreach(Landmark* l, this->mLandmarks)
    {
        res << l->getName();
    }
    return res;
}

cv::FileStorage& operator<< (cv::FileStorage& fs, LandmarkContainer const* c)
{
    fs << "{";
    
    fs << "Landmarks" << "[";
    for(int i = 0; i < c->mLandmarks.size(); ++i)
    {
        fs << c->mLandmarks.at(i);
    }
    fs << "]";
    
    fs << "}";
    
    return fs;
}

void operator>> (cv::FileNode const& n, LandmarkContainer* c)
{
    cv::FileNode parametersNode = n["Landmarks"];
    for (cv::FileNodeIterator fnIt = parametersNode.begin(); fnIt != parametersNode.end(); ++fnIt)
    {
        Landmark* l = new Landmark();
        (*fnIt) >> l;
        c->addLandmark(l);
    }
}

void LandmarkContainer::getMovedSignal(const Landmark *l)
{
    emit landmarkAdded(l);
}
