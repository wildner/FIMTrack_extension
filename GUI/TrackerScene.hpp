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

#ifndef TRACKERSCENE_HPP
#define TRACKERSCENE_HPP

#include <QGraphicsScene>
#include <QPixmap>
#include <QRect>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QInputDialog>

#include <QDebug>

#include "Data/Larva.hpp"
#include "Control/InputGenerator.hpp"
#include "TrackerSceneLarva.hpp"
#include "RegionOfInterestContainer.hpp"
#include "LandmarkContainer.hpp"

class TrackerSceneLarva;

class TrackerScene : public QGraphicsScene
{
    
public:
    TrackerScene(QObject *parent = 0);
    ~TrackerScene();
    
    TrackerSceneLarva* addLarva(Larva *larva, QColor color);
        
    /**
    * Sets the image to be displayed
    *
    * @param img
    */
    void setPixmap (const QPixmap &img);
        
    /**
    * Returns the currently displayed image.
    *
    * @return Currently displayed image
    */
    QPixmap getPixmap();
    
    QRectF boundingRect() const;

    /**
     * @brief clearScene clears the scene and reinitializes the imaged displayed in the scene and the region of interest
     */
    void clearScene();
    
    bool hasROIContainer() const {if(this->mROIContainer) return true; return false;}
    RegionOfInterestContainer const* getROIContainer() const {return this->mROIContainer;}
    
    bool hasLandmarkContainer() const {if(this->mLandmarkContainer) return true; return false;}
    LandmarkContainer const* getLandmarkContainer() const {return this->mLandmarkContainer;}
    
    QImage getSaveImage(QVector<TrackerSceneLarva *> larvaVector);
    
public slots:
    void addROI(QPointF const& p, RegionOfInterest::RegionOfInterestType type);
    void removeROI(QPointF const& p);
    bool ROIContainsPoint(QPointF const& p);
    void loadROIContainer(QString const& path);
    
    void addLandmark(QPointF const& p, Landmark::Type type);
    void removeLandmark(QPointF const& p);
    bool landmarkContainsPoint(QPointF const& p);
    void loadLandmarkContainer(QString const& path);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    
    
private:
    
    static const int                                mDefaultROIWidth    = 100;
    static const int                                mDefaultROIHeight   = 100;

    bool                                            mMousePressed;

    /**
    * List of LarvaModel-representations
    */
    std::vector<TrackerSceneLarva*>                 mLarvae;
    
    /**
    * List of QGraphicsItem that visualize
    * the region of interest.
    */
    RegionOfInterestContainer*                      mROIContainer;
    
    LandmarkContainer*                              mLandmarkContainer;

    /**
    * Image to be displayed in  this scene.
    */
    QGraphicsPixmapItem*                            mImage;	
    
    void checkForRIOContainer();
};

#endif // TRACKERSCENE_HPP
