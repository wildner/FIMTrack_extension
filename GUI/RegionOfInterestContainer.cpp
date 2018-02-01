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

#include "RegionOfInterestContainer.hpp"

RegionOfInterestContainer::RegionOfInterestContainer()
{
}

RegionOfInterestContainer::RegionOfInterestContainer(QRectF const& boundingBox)
{
    this->mMousePressed = false;
    
    this->mBoundingBox = boundingBox;
    
    qsrand(QTime::currentTime().msec());
    this->setColor(QColor(qrand() % 256, qrand() % 256, qrand() % 256));
    this->mBrush.setStyle(Qt::SolidPattern);
    
    this->mCurrentHoverROIIndex     = -1;
    this->mCurrentROIIndex          = -1;
    
    this->setAcceptHoverEvents(true);
}

RegionOfInterestContainer::~RegionOfInterestContainer()
{
    this->mRegionOfInterests.clear();
}

void RegionOfInterestContainer::addRegionOfInterest(QString name, QRectF const& roi, RegionOfInterest::RegionOfInterestType type)
{
    this->mRegionOfInterests.push_back(RegionOfInterest(name, roi, type));
}

void RegionOfInterestContainer::removeRegionOfInterest(QPointF const& p)
{
    int index;
    if(this->findIndexOfROI(p, index))
    {
        this->mRegionOfInterests.remove(index);
        this->update();
    }
}

bool RegionOfInterestContainer::findIndexOfROI(QPointF const& p, int &index)
{
    bool found = false;
    
    for(int i = 0; i < this->mRegionOfInterests.size(); ++i)
    {
        if(this->mRegionOfInterests.at(i).contains(p))
        {
            index = i;
            found = true;
            break;
        }
    }
    
    return found;
}

void RegionOfInterestContainer::setColor(const QColor &color)
{
    this->mColor = color;
    this->mPen.setColor(this->mColor);
    this->mColor.setAlpha(this->mColor.alpha() / 4);
    this->mBrush.setColor(this->mColor);
}

void RegionOfInterestContainer::setBoundingBox(QRectF const& boundingBox)
{
    this->mBoundingBox = boundingBox;
}

bool RegionOfInterestContainer::contains(QPointF const& point) const
{
    bool contains = false;
    for(int i = 0; i < this->mRegionOfInterests.size(); ++i)
    {
        if(this->mRegionOfInterests.at(i).contains(point))
        {
            contains = true;
            break;
        }
    }
    
    return contains;
}

void RegionOfInterestContainer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    painter->save();
    
    painter->setPen(this->mPen);
    this->mBrush.setStyle(Qt::NoBrush);
    painter->setBrush(this->mBrush);
    
    for(int i = 0; i < this->mRegionOfInterests.size(); ++i)
    {
        switch(this->mRegionOfInterests.at(i).getType())
        {
            case RegionOfInterest::RECTANGLE:
                painter->drawRect(this->mRegionOfInterests.at(i).getBoundingBox());
                break;
            case RegionOfInterest::ELLIPSE:
                painter->drawEllipse(this->mRegionOfInterests.at(i).getBoundingBox());
                break;
        }
        
        if(this->mRegionOfInterests.at(i).getResizeAnchorVisibility())
        {
            painter->drawPixmap(this->mRegionOfInterests.at(i).getResizeAnchor().toRect(), this->mRegionOfInterests.at(i).getResizeAnchorIconPixmap());
        }
    }
    
    this->mBrush.setStyle(Qt::SolidPattern);
    painter->setBrush(this->mBrush);
    
    painter->setClipRegion(this->getClipingRegion());
    painter->drawRect(this->mBoundingBox);
    
    painter->restore();
}

QRectF RegionOfInterestContainer::boundingRect() const
{
    return this->mBoundingBox;
}

bool RegionOfInterestContainer::canMoveROITo(QPointF const& p, int index)
{
    QRectF bBox = this->mRegionOfInterests.at(index).getBoundingBox();
    bBox.translate(p);
    return (this->mBoundingBox.contains(bBox));// && !this->hasIntersections(bBox));
}

bool RegionOfInterestContainer::hasIntersections(const QRectF &r)
{
    for(int i = 0; i < this->mRegionOfInterests.size(); ++i)
    {
        if(this->mRegionOfInterests[i].intersects(r))
        {
            return true;    
        }
    }
    
    return false;
}

QRegion RegionOfInterestContainer::getClipingRegion()
{
    QRegion clipRegion = QRegion(this->mBoundingBox.toRect(), QRegion::Rectangle);
    QRegion currentRegion;
    
    for(int i = 0; i < this->mRegionOfInterests.size(); ++i)
    {
        switch(this->mRegionOfInterests.at(i).getType())
        {
            case RegionOfInterest::RECTANGLE:
                currentRegion = QRegion(this->mRegionOfInterests.at(i).getBoundingBox().x(), 
                                        this->mRegionOfInterests.at(i).getBoundingBox().y(), 
                                        this->mRegionOfInterests.at(i).getBoundingBox().width(),
                                        this->mRegionOfInterests.at(i).getBoundingBox().height(), 
                                        QRegion::Rectangle);
                break;
            case RegionOfInterest::ELLIPSE:
                currentRegion = QRegion(this->mRegionOfInterests.at(i).getBoundingBox().x(),
                                        this->mRegionOfInterests.at(i).getBoundingBox().y(), 
                                        this->mRegionOfInterests.at(i).getBoundingBox().width(), 
                                        this->mRegionOfInterests.at(i).getBoundingBox().height(), 
                                        QRegion::Ellipse);
                break;
        }
        
        clipRegion -= currentRegion;
    }
    
    return clipRegion;
}

void RegionOfInterestContainer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(this->findIndexOfROI(event->pos(), this->mCurrentROIIndex))
    {
        this->mMousePressed = true;
        this->mLastMousePressedPoint = event->pos();
    }
}

void RegionOfInterestContainer::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(this->mMousePressed && this->mRegionOfInterests.at(this->mCurrentROIIndex).isMoveable())
    {
        // change Size
        if(this->mRegionOfInterests.at(this->mCurrentROIIndex).getResizeAnchor().contains(event->pos()))
        {
            QRectF rect = this->mRegionOfInterests.at(this->mCurrentROIIndex).getBoundingBox();
            if(event->pos().x() > rect.x() && event->pos().y() > rect.y())
            {
                rect.setWidth(qAbs(event->pos().x() - rect.x()));
                rect.setHeight(qAbs(event->pos().y() - rect.y()));
                
                this->mRegionOfInterests[this->mCurrentROIIndex].setBoundingBox(rect);
                this->update();
                this->mLastMousePressedPoint = event->pos();
            }
        }
        //move
        else
        {
            QPointF trans = (event->pos() - this->mLastMousePressedPoint);
            if(this->canMoveROITo(trans, this->mCurrentROIIndex))
            {
                this->mRegionOfInterests[this->mCurrentROIIndex].translate(trans);
                this->update();
                this->mLastMousePressedPoint = event->pos();
            }
        }
    }
}

void RegionOfInterestContainer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    this->mMousePressed = false;
}

void RegionOfInterestContainer::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(this->getClipingRegion().contains(event->pos().toPoint()))
    {
        QColor col = QColorDialog::getColor();
        if(col.isValid())
        {
            this->setColor(col);
        }
    }
}

void RegionOfInterestContainer::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    QGraphicsItem::wheelEvent(event);
}

void RegionOfInterestContainer::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
}

void RegionOfInterestContainer::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    int index;
    if(this->findIndexOfROI(event->pos(), index))
    {
        if(this->mRegionOfInterests.at(index).isResizeable())
        {
            if(this->mCurrentHoverROIIndex == index)
            {
                if(event->modifiers().testFlag(Qt::ControlModifier))
                {
                    this->mRegionOfInterests[index].setResizeAnchorVisibility(true);
                    this->update();
                }
                else
                {
                    this->mRegionOfInterests[index].setResizeAnchorVisibility(event->pos());
                    this->update();
                }
            }
            else
            {
                if(this->mCurrentHoverROIIndex > -1)
                {
                    this->mRegionOfInterests[this->mCurrentHoverROIIndex].setResizeAnchorVisibility(false);
                }
                
                if(event->modifiers().testFlag(Qt::ControlModifier))
                {
                    this->mRegionOfInterests[index].setResizeAnchorVisibility(true);
                    this->update();
                }
                else
                {
                    this->mRegionOfInterests[index].setResizeAnchorVisibility(event->pos());
                    this->update();
                }
                this->mCurrentHoverROIIndex = index;
            }
        }
    }
    else
    {
        if(this->mCurrentHoverROIIndex > -1)
        {
            this->mRegionOfInterests[this->mCurrentHoverROIIndex].setResizeAnchorVisibility(false);
            this->mCurrentHoverROIIndex = -1;
            this->update();
        }
    }
}

void RegionOfInterestContainer::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
}

