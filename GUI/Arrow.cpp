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

#include "Arrow.hpp"

Arrow::Arrow(QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
{
    this->mColor        = Qt::red;
    
    this->setFlag(QGraphicsItem::ItemIsSelectable, false);
    this->setPen(QPen(this->mColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

Arrow::~Arrow()
{
}

QRectF Arrow::boundingRect() const
{
    return this->shape().boundingRect();
}

double Arrow::getAngle(QLineF const& l)
{
    return M_PI - (std::atan2(l.dx(),l.dy()));
}

void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    painter->setRenderHint(QPainter::Antialiasing);
    
    QPen pen = this->pen();
    pen.setColor(this->mColor);
    painter->setPen(pen);
    
    QBrush brush = QBrush(this->mColor);
    brush.setStyle(Qt::SolidPattern);
    painter->setBrush(brush);
    
    painter->drawLine(this->line());
    
    const double sz = 5.0; //pixels
    double arrowAngle = this->getAngle(line());
    
    if (line().dy() >= 0.0) 
        arrowAngle = (1.0 * M_PI) + arrowAngle;
    
    if(line().dy() >= 0.0)
    {
        const QPointF p0 = this->line().p2();
        
        const QPointF p1
                = p0 + QPointF(
                    std::sin(arrowAngle +  0.0 + (M_PI * 0.1)) * sz,
                    -std::cos(arrowAngle +  0.0 + (M_PI * 0.1)) * sz);
        const QPointF p2
                = p0 + QPointF(
                    std::sin(arrowAngle +  0.0 - (M_PI * 0.1)) * sz,
                    -std::cos(arrowAngle +  0.0 - (M_PI * 0.1)) * sz);
        
        painter->drawPolygon(QPolygonF() << p0 << p1 << p2);
    }
    else
    {
        const QPointF p0 = this->line().p2();
        
        const QPointF p1
                = p0 - QPointF(
                    std::sin(arrowAngle +  0.0 + (M_PI * 0.1)) * sz,
                    -std::cos(arrowAngle +  0.0 + (M_PI * 0.1)) * sz);
        const QPointF p2
                = p0 - QPointF(
                    std::sin(arrowAngle +  0.0 - (M_PI * 0.1)) * sz,
                    -std::cos(arrowAngle +  0.0 - (M_PI * 0.1)) * sz);
        
        painter->drawPolygon(QPolygonF() << p0 << p1 << p2);
    }
}
