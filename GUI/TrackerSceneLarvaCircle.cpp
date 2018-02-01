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

#include "TrackerSceneLarvaCircle.hpp"

TrackerSceneLarvaCircle::TrackerSceneLarvaCircle(bool resizable)
{
    this->mResizable = resizable;
}

void TrackerSceneLarvaCircle::setResizable(bool resizable)
{
    this->mResizable = resizable;
}

void TrackerSceneLarvaCircle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    /* Resize on ctrl */
    if(this->mResizable && event->modifiers() == Qt::ControlModifier)
    {
        this->setCursor(Qt::SizeAllCursor);
    }
    /* Else move */
    else
    {
        this->setCursor(Qt::ClosedHandCursor);
    }
}

void TrackerSceneLarvaCircle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{    
    /* Resize on ctrl */
    if(this->mResizable && event->modifiers() == Qt::ControlModifier)
    {
        this->setCursor(Qt::SizeAllCursor);
        QPointF origin = this->rect().center();
        float radius = this->rect().width() / 2;
        QPointF last = event->lastPos();
        QPointF current = event->pos();
        float lastdistane = sqrt(pow(origin.x() - last.x(), 2) + 
                                 pow(origin.y() - last.y(), 2));
        
        float currentdistance = sqrt(pow(origin.x() - current.x(), 2) + 
                                     pow(origin.y() - current.y(), 2));
        
        float currentScaleFactor = currentdistance - lastdistane;
        
        radius += currentScaleFactor;
        
        this->setRect(
                    origin.x() - radius, 
                    origin.y() - radius, 
                    radius * 2, 
                    radius * 2);
        
        emit changed();
    }
    else
    {
        this->setCursor(Qt::ClosedHandCursor);
        QPointF trans = (event->pos() - event->lastPos());		
        this->setRect(this->rect().translated(trans));
        emit changed();
    }
}

void TrackerSceneLarvaCircle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    this->setCursor(Qt::ArrowCursor);
}
