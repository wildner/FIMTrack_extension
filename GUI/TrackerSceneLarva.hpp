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

#ifndef TRACKERSCENELARVA_HPP
#define TRACKERSCENELARVA_HPP

#include <QtCore>
#include <QtGui>

#include <cmath>

#include "Data/Larva.hpp"
#include "TrackerSceneLarvaCircle.hpp"
#include "TrackerScene.hpp"
#include "Arrow.hpp"

class TrackerSceneLarva: public QObject
{
    Q_OBJECT
    
public:
    TrackerSceneLarva (QGraphicsScene *scene, 
                       Larva *larva, 
                       const QColor &color);
    
    ~TrackerSceneLarva ();
    
public slots:
    
    void invert(uint time);
    
    void updateTime (unsigned int time);
    void updateDrawing();
    void setVisible (bool visible);
    
    /**
     * Changes the color
     *
     * @param _color	New color of this representations components.
     */
    void setColor (QColor const& mColor);
    
    void setLarvaVisible (bool visible);
    void setPathVisible (bool visible);
    void setDistance2OriginVisible (bool visible);
    
    void setLarvaPointer(Larva* larva) {this->mLarva = larva;}
    
    QGraphicsScene* getScene() const {return this->mScene;}
    
    QString     getID()         const {return this->mID;}
    uint        getBaseID()     const;
    
    int getTotalNumberOfSpinePoints() const {return this->mCircles.size();}
    
    qreal getRadiusAt(unsigned int index) const
    {
        if((index < this->mCircles.size()))
            return this->mCircles.at(index)->rect().width() / 2; 
        else 
            return 0.0;
    }
    
    void setRadiusAt(const unsigned int index, const qreal radius) 
    {
        if((index < this->mCircles.size()))
        {
            QRectF r = this->mCircles[index]->rect();
            this->mCircles[index]->setRect(r.x(),
                                           r.y(),
                                           radius*2,
                                           radius*2);
            this->updateDrawing();
        }
    }
    
    void recompute(unsigned int time);
    
    int getCurrentTimePoint() const {return this->mCurrentTime;}
    
    QPolygonF getPoligon() const {return this->mSilhouette->polygon();}
    QPainterPath getSpine() const {return this->mSpine->path();}
    std::vector<double> getSpineRadii() const 
    {
        std::vector<double> radii;
        for(unsigned int i = 0; i < this->mCircles.size(); ++i)
        {
            radii.push_back(this->mCircles.at(i)->rect().width() / 2);
        }
        
        return radii;
    }
    
    void adjustDistancePaths();
    
    void removeFromScene();
    
    void prepareForSave(bool prepare);
    
private slots:
    bool checkForSpinePoints();
    
    void initLarvaDrawing();
    void updateLarvaDrawing(unsigned int time);
    void adjustCircles();
    void adjustLarvaContourAndSpine(FIMTypes::spine_t const& spine, std::vector<double> const& radii);
    void adjustSilhouette();
    void restoreVisibility();
    
    bool isFirstTimestepOfLarva();
    
    
signals:
    void sendTotalNumberOfSpinePoints(unsigned int spinePoints);
    void valuesChanged();
    
private:
    
    bool                                    mShowSpine;
    bool                                    mShowPath;
    bool                                    mShowDistance2Origin;
    bool                                    mShowSilhouette;
    
    double                                  mHeadTailRadius;
    
    int                                     mCurrentTime;
    
    Larva*                                  mLarva;
    
    QColor                                  mColor;
    
    /**
     * Name of the LarvaModel represented by this object.
     */
    QString                                 mID;
    
    /**
     * Scene in which this LarvaModel is displayed.
     */
    QGraphicsScene*                         mScene;	

    /**
     * Set of circles representing spine points and sizes
     * of LarvaModel. May be edited by user.
     */
    std::vector<TrackerSceneLarvaCircle*>	mCircles;

    /**
     * Visualization of LarvaModel ID
     */
    QGraphicsTextItem*                      mDrawID;
    
    /**
     * Visualization of LarvaModel spine
     */
    QGraphicsPathItem*                      mSpine;

    /**
     * Path the LarvaModel has travelled
     */
    QGraphicsPathItem*                      mPath;

    /**
     * Line to origin
     */
    QGraphicsPathItem*                      mDistance2Origin;

    /**
     * Polygon visualizing the larvas silhouette.
     */
    QGraphicsPolygonItem*                   mSilhouette;
    
    /**
     *
     */
    Arrow*                                  mVelosityArrow;
    Arrow*                                  mDirectionArrow;
    
    
};

#endif // TRACKERSCENELARVA_HPP
