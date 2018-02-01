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

#ifndef LANDMARKCONTAINER_HPP
#define LANDMARKCONTAINER_HPP

#include "Configuration/FIMTrack.hpp"
#include "Landmark.hpp"

class LandmarkContainer : public QObject
{
    
    Q_OBJECT
    
private:
    QGraphicsScene*     mParentScene;
    QVector<Landmark*>  mLandmarks;
    
signals:
    void landmarkAdded(Landmark const*);
    void landmarkRemoved(QString);
    void sendAllLandmarkNames(QStringList);
    
private slots:
    void getMovedSignal(Landmark const* l);
    
public:
    LandmarkContainer(QGraphicsScene* parentScene);
    ~LandmarkContainer();
    
    void addLandmark(QString const& name, QPointF const& p, Landmark::Type type);
    void addLandmark(Landmark *l);
    void removeLandmak(QPointF const& p);
    
    bool contains(QPointF const& p) const;
    void clear(){this->mLandmarks.clear();}
    
    int         getSize()                   const   {return this->mLandmarks.size();}
    bool        isEmpty()                   const   {return this->mLandmarks.isEmpty();}
    
    QString     getLandmarkName(int index)  const   {return this->mLandmarks.at(index)->getName();}
    QStringList getAllLandmarkNames()       const;
    
    Landmark*   getLandmarkAt(int index)    const   {return this->mLandmarks.at(index);}
    
    friend cv::FileStorage& operator<< (cv::FileStorage& fs, LandmarkContainer const* c);
    
    friend void operator>> (cv::FileNode const& n, LandmarkContainer* c);
};

cv::FileStorage& operator<< (cv::FileStorage& fs, LandmarkContainer const* c);

void operator>> (cv::FileNode const& n, LandmarkContainer* c);

#endif // LANDMARKCONTAINER_HPP
