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

#ifndef REGIONOFINTERESTCONTAINER_HPP
#define REGIONOFINTERESTCONTAINER_HPP

#include "Configuration/FIMTrack.hpp"
#include "RegionOfInterest.hpp"

#include <QVector>
#include <QColor>
#include <QPen>
#include <QBrush>
#include <QRectF>
#include <QColorDialog>
#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneWheelEvent>

class RegionOfInterestContainer : public QGraphicsItem
{
    
private:
    bool                                mMousePressed;
    QPointF                             mLastMousePressedPoint;
    int                                 mCurrentROIIndex;
    int                                 mCurrentHoverROIIndex;
    
    QColor                              mColor;
    QPen                                mPen;
    QBrush                              mBrush;
    QRectF                              mBoundingBox;
    QVector<RegionOfInterest>           mRegionOfInterests;
    
    bool canMoveROITo(QPointF const& p, int index);
    bool hasIntersections(QRectF const& r);
    
    QRegion getClipingRegion();
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    
    void wheelEvent(QGraphicsSceneWheelEvent *event);
    
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    
public:
    RegionOfInterestContainer();
    explicit RegionOfInterestContainer(QRectF const& boundingBox);
    ~RegionOfInterestContainer();
    
    void addRegionOfInterest(QString name, QRectF const& roi, RegionOfInterest::RegionOfInterestType type);
    void removeRegionOfInterest(QPointF const& p);
    bool findIndexOfROI(QPointF const& p, int& index);
    
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    bool contains(const QPointF &point) const;
    QRectF boundingRect() const;
    
    void setColor(QColor const& color);
    void setBoundingBox(QRectF const& boundingBox);
    
    int getSize() const {return this->mRegionOfInterests.size();}
    bool isEmpty() const {return this->mRegionOfInterests.isEmpty();}
    
    QVector<RegionOfInterest> const& getRegionOfInterests() const {return this->mRegionOfInterests;}
    
    
    friend cv::FileStorage& operator<< (cv::FileStorage& fs, RegionOfInterestContainer const& c);  
    friend cv::FileStorage& operator<< (cv::FileStorage& fs, RegionOfInterestContainer const* c);
    friend void operator>> (cv::FileNode const& n, RegionOfInterestContainer& c);  
    friend void operator>> (cv::FileNode const& n, RegionOfInterestContainer* c);
};

#endif // REGIONOFINTERESTCONTAINER_HPP
