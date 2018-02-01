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

#include "Landmark.hpp"

Landmark::Landmark()
{
    this->mType             = POINT;
    this->mName             = "";
    
    this->mIcon             = nullptr;
    
    this->mLine             = nullptr;
    this->mLineAnchorFirst  = nullptr;
    this->mLineAnchorSecond = nullptr;
    
    this->mRect             = nullptr;
    this->mRectAnchor       = nullptr;
    
    this->mEllipse          = nullptr;
}

Landmark::Landmark(const QString &name, qreal xPos, qreal yPos, int type)
{
    this->mType             = type;
    this->mName             = name;
    
    this->mIcon             = nullptr;
    
    this->mLine             = nullptr;
    this->mLineAnchorFirst  = nullptr;
    this->mLineAnchorSecond = nullptr;
    
    this->mRect             = nullptr;
    this->mRectAnchor       = nullptr;
    
    this->mEllipse          = nullptr;
    
    this->setToolTip(this->mName);
    this->initialize(xPos, yPos);
}

Landmark::~Landmark()
{    
}

QPointF Landmark::getPoint() const
{
    if(this->mType == POINT)
        return this->mapToScene(this->mIcon->pos());
    return QPointF();
}

QLineF Landmark::getLine() const
{
    if(this->mType == LINE) 
        return QLineF(this->mapToScene(this->mLine->line().p1()), this->mapToScene(this->mLine->line().p2()));
    return QLineF();
}

QRectF Landmark::getRectagle() const
{
    if(this->mType == RECTANGLE) 
        return this->mapToScene(this->mRect->rect()).boundingRect();
    return QRectF();
}

QRectF Landmark::getEllipse() const
{
    if(this->mType == ELLIPSE) 
        return QRectF(this->mapToScene(this->mEllipse->rect()).boundingRect());
    return QRectF();
}

void Landmark::initialize(qreal x, qreal y)
{
    qsrand(QTime::currentTime().msec());
    int r,g,b;
    
    r = qrand() % 256;
    g = qrand() % 256;
    b = qrand() % 256;
    QColor col = QColor(r,g,b, this->mDefaultAlphaValue);
    
    switch(this->mType)
    {
        case POINT: 
            this->mIsResizeable = false;
            
            this->mIcon         = new QGraphicsPixmapItem(QIcon(":/ResultViewer/Icons/Icons/landmark.png").pixmap(QSize(this->mDefaultIconWidth, this->mDefaultIconHeight)));
            this->addToGroup(this->mIcon);
            break;
            
        case LINE: 
            this->setAcceptHoverEvents(true);
            this->mIsResizeable = true;
            
            this->mLine         = new QGraphicsLineItem(0,0,this->mDefaultLineLength,0);
            
            this->mLineAnchorFirst  = new QGraphicsEllipseItem(this->mLine->line().p1().x() - this->mDefaultLineAnchorFirstWidthOffset, 
                                                               this->mLine->line().p1().y() - this->mDefaultLineAnchorFirstHeightOffset,
                                                               this->mDefaultLineAnchorFirstWidth, 
                                                               this->mDefaultLineAnchorFirstHeight,
                                                               this->mLine);
            this->mLineAnchorSecond = new QGraphicsEllipseItem(this->mLine->line().p2().x() - this->mDefaultLineAnchorSecondWidthOffset, 
                                                               this->mLine->line().p2().y() - this->mDefaultLineAnchorSecondHeightOffset,
                                                               this->mDefaultLineAnchorSecondWidth,
                                                               this->mDefaultLineAnchorSecondHeight,
                                                               this->mLine);
            
            this->addToGroup(this->mLine);
            this->addToGroup(this->mLineAnchorFirst);
            this->addToGroup(this->mLineAnchorSecond);
            
            this->mLineAnchorFirst->setVisible(false);
            this->mLineAnchorSecond->setVisible(false);
            
            break;
            
        case RECTANGLE:
            this->setAcceptHoverEvents(true);
            this->mIsResizeable = true;
            
            this->mRect         = new QGraphicsRectItem();
            this->mRect->setRect(
                        0,
                        0,
                        this->mDefaultRectWidth, 
                        this->mDefaultRectHeight);	
            
            this->mRectAnchor   = new QGraphicsEllipseItem(this->mRect->rect().x() + this->mRect->rect().width() - this->mDefaultRectAnchorWidthOffset,
                                                           this->mRect->rect().y() + this->mRect->rect().height() - this->mDefaultRectAnchorHeightOffset,
                                                           this->mDefaultRectAnchorWidth,
                                                           this->mDefaultRectAnchorHeight,
                                                           this->mRect);
            this->mRectAnchor->setVisible(false);
            
            this->mIcon         = new QGraphicsPixmapItem(QIcon(":/ResultViewer/Icons/Icons/landmark.png").pixmap(QSize(this->mDefaultIconWidth, this->mDefaultIconHeight)));
            this->mIcon->setX(this->mRect->rect().center().x() - this->mDefaultIconWidth / 2);
            this->mIcon->setY(this->mRect->rect().center().y() - this->mDefaultIconHeight / 2);
            
            this->addToGroup(this->mIcon);          
            this->addToGroup(this->mRectAnchor);
            this->addToGroup(this->mRect);
            break;
            
        case ELLIPSE:
            this->setAcceptHoverEvents(true);
            this->mIsResizeable = true;   
            
            this->mEllipse      = new QGraphicsEllipseItem();
            this->mEllipse->setRect(
                        0,
                        0,
                        this->mDefaultRectWidth, 
                        this->mDefaultRectHeight);	
            
            this->mEllipseAnchor   = new QGraphicsEllipseItem(this->mEllipse->rect().x() + this->mEllipse->rect().width() - this->mDefaultRectAnchorWidthOffset,
                                                              this->mEllipse->rect().y() + this->mEllipse->rect().height() - this->mDefaultRectAnchorHeightOffset,
                                                              this->mDefaultRectAnchorWidth,
                                                              this->mDefaultRectAnchorHeight,
                                                              this->mEllipse);
            this->mEllipseAnchor->setVisible(false);
            
            this->mIcon         = new QGraphicsPixmapItem(QIcon(":/ResultViewer/Icons/Icons/landmark.png").pixmap(QSize(this->mDefaultIconWidth, this->mDefaultIconHeight)));
            this->mIcon->setX(this->mEllipse->rect().center().x() - this->mDefaultIconWidth / 2);
            this->mIcon->setY(this->mEllipse->rect().center().y() - this->mDefaultIconHeight / 2);
        
            this->addToGroup(this->mEllipse);
            this->addToGroup(this->mEllipseAnchor);
            this->addToGroup(this->mIcon);
            break;
    }
    
    this->setColor(col);
    this->setPos(x,y);
}

void Landmark::initalizeAfterLoading(QPointF const& scenePos)
{
    if(this->mType == POINT)
    {
        this->mIsResizeable = false;
        
        this->mIcon         = new QGraphicsPixmapItem(QIcon(":/ResultViewer/Icons/Icons/landmark.png").pixmap(QSize(this->mDefaultIconWidth, this->mDefaultIconHeight)));
        
        this->setToolTip(this->mName);
        this->addToGroup(this->mIcon);
        
        this->setColor(this->mColor);
        this->setPos(scenePos.x(), scenePos.y());
    }
}

void Landmark::initalizeAfterLoading(const QPointF &sceneP1, const QPointF &sceneP2)
{
    if(this->mType == LINE)
    {
        this->setAcceptHoverEvents(true);
        this->mIsResizeable = true;
        
        this->mLine         = new QGraphicsLineItem(0,0,this->mDefaultLineLength,0);
        QLineF l            = QLineF(sceneP1, sceneP2);
        this->mLine->setLine(l);
        
        this->mLineAnchorFirst  = new QGraphicsEllipseItem(this->mLine->line().p1().x() - this->mDefaultLineAnchorFirstWidthOffset, 
                                                           this->mLine->line().p1().y() - this->mDefaultLineAnchorFirstHeightOffset,
                                                           this->mDefaultLineAnchorFirstWidth, 
                                                           this->mDefaultLineAnchorFirstHeight,
                                                           this->mLine);
        this->mLineAnchorSecond = new QGraphicsEllipseItem(this->mLine->line().p2().x() - this->mDefaultLineAnchorSecondWidthOffset, 
                                                           this->mLine->line().p2().y() - this->mDefaultLineAnchorSecondHeightOffset,
                                                           this->mDefaultLineAnchorSecondWidth,
                                                           this->mDefaultLineAnchorSecondHeight,
                                                           this->mLine);
        
        this->setToolTip(this->mName);
        
        this->addToGroup(this->mLine);
        this->addToGroup(this->mLineAnchorFirst);
        this->addToGroup(this->mLineAnchorSecond);
        
        this->mLineAnchorFirst->setVisible(false);
        this->mLineAnchorSecond->setVisible(false);
        
        this->setColor(this->mColor);
    }
}

void Landmark::initalizeAfterLoading(const QRectF &sceneRect)
{
    switch(this->mType)
    {
        case RECTANGLE:
            this->setAcceptHoverEvents(true);
            this->mIsResizeable = true;
            
            this->mRect         = new QGraphicsRectItem();
            this->mRect->setRect(sceneRect);
            
            this->mRectAnchor   = new QGraphicsEllipseItem(this->mRect->rect().x() + this->mRect->rect().width() - this->mDefaultRectAnchorWidthOffset,
                                                           this->mRect->rect().y() + this->mRect->rect().height() - this->mDefaultRectAnchorHeightOffset,
                                                           this->mDefaultRectAnchorWidth,
                                                           this->mDefaultRectAnchorHeight,
                                                           this->mRect);
            this->mRectAnchor->setVisible(false);
            
            this->mIcon         = new QGraphicsPixmapItem(QIcon(":/ResultViewer/Icons/Icons/landmark.png").pixmap(QSize(this->mDefaultIconWidth, this->mDefaultIconHeight)));
            this->mIcon->setX(this->mRect->rect().center().x() - this->mDefaultIconWidth / 2);
            this->mIcon->setY(this->mRect->rect().center().y() - this->mDefaultIconHeight / 2);
            
            this->setToolTip(this->mName);
            
            this->addToGroup(this->mRect);
            this->addToGroup(this->mRectAnchor);
            this->addToGroup(this->mIcon);
            
            this->setColor(this->mColor);
            
            break;
            
        case ELLIPSE:
            this->setAcceptHoverEvents(true);
            this->mIsResizeable = true;   
            
            this->mEllipse      = new QGraphicsEllipseItem();
            this->mEllipse->setRect(sceneRect);	
            
            this->mEllipseAnchor   = new QGraphicsEllipseItem(this->mEllipse->rect().x() + this->mEllipse->rect().width() - this->mDefaultRectAnchorWidthOffset,
                                                              this->mEllipse->rect().y() + this->mEllipse->rect().height() - this->mDefaultRectAnchorHeightOffset,
                                                              this->mDefaultRectAnchorWidth,
                                                              this->mDefaultRectAnchorHeight,
                                                              this->mEllipse);
            this->mEllipseAnchor->setVisible(false);
            
            this->mIcon         = new QGraphicsPixmapItem(QIcon(":/ResultViewer/Icons/Icons/landmark.png").pixmap(QSize(this->mDefaultIconWidth, this->mDefaultIconHeight)));
            this->mIcon->setX(this->mEllipse->rect().center().x() - this->mDefaultIconWidth / 2);
            this->mIcon->setY(this->mEllipse->rect().center().y() - this->mDefaultIconHeight / 2);
            
            this->setToolTip(this->mName);
            
            this->addToGroup(this->mEllipse);
            this->addToGroup(this->mEllipseAnchor);
            this->addToGroup(this->mIcon);
            
            this->setColor(this->mColor);
            
            break;
    }
}

bool Landmark::contains(const QPointF &point) const
{
    QPointF p = point - this->pos();
    if(this->mType != ELLIPSE)
        return QGraphicsItemGroup::contains(p);
    else
    {
        qreal a,b;
        if(this->mEllipse->rect().width() > this->mEllipse->rect().height())
        {
            a = pow(p.x() - this->mEllipse->rect().center().x(), 2) / pow(this->mEllipse->rect().width() / 2, 2);
            b = pow(p.y() - this->mEllipse->rect().center().y(), 2) / pow(this->mEllipse->rect().height() / 2, 2);
        }
        else
        {
            a = pow(p.x() - this->mEllipse->rect().center().x(), 2) / pow(this->mEllipse->rect().height() / 2, 2);
            b = pow(p.y() - this->mEllipse->rect().center().y(), 2) / pow(this->mEllipse->rect().width() / 2, 2);
        }
        
        return ((a+b) <= 1.0);
    }
}

void Landmark::cleanupAfterRemoving()
{
    if(this->mIcon)
        delete this->mIcon;
    
    if(this->mLine)
        delete this->mLine;
    if(this->mLineAnchorFirst)
        delete this->mLineAnchorFirst;
    if(this->mLineAnchorSecond)
        delete this->mLineAnchorSecond;
    
    if(this->mRect)
        delete this->mRect;
    if(this->mRectAnchor)
        delete this->mRectAnchor; 
    
    if(this->mEllipse)
        delete this->mEllipse;
}

void Landmark::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        this->setCursor(Qt::ClosedHandCursor);
        this->mLastMousePos = event->pos();
        this->mMoved = false;
        this->mLeftMouseButtonPressed = true;
    }
}

void Landmark::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(this->mLeftMouseButtonPressed)
    {
        QPointF trans = (event->pos() - this->mLastMousePos);		
        
        this->mMoved = true;
        
        switch(this->mType)
        {
            case LINE:
                if(this->mLineAnchorFirst->contains(event->pos()))
                {
                    this->removeFromGroup(this->mLine);
                    this->removeFromGroup(this->mLineAnchorFirst);
                    this->removeFromGroup(this->mLineAnchorSecond);
                    
                    QLineF currentLine = this->mLine->line();
                    currentLine.setP1(event->pos());
                    this->mLine->setLine(currentLine);
                    this->mLineAnchorFirst->setRect(this->mLine->line().p1().x() - this->mDefaultLineAnchorFirstWidthOffset, 
                                                    this->mLine->line().p1().y() - this->mDefaultLineAnchorFirstHeightOffset,
                                                    this->mDefaultLineAnchorFirstWidth, 
                                                    this->mDefaultLineAnchorFirstHeight);
                    
                    this->addToGroup(this->mLine);
                    this->addToGroup(this->mLineAnchorFirst);
                    this->addToGroup(this->mLineAnchorSecond);
                    return; /// break;
                }
                
                if(this->mLineAnchorSecond->contains(event->pos()))
                {
                    this->removeFromGroup(this->mLine);
                    this->removeFromGroup(this->mLineAnchorFirst);
                    this->removeFromGroup(this->mLineAnchorSecond);
                    
                    QLineF currentLine = this->mLine->line();
                    currentLine.setP2(event->pos());
                    this->mLine->setLine(currentLine);
                    this->mLineAnchorSecond->setRect(this->mLine->line().p2().x() - this->mDefaultLineAnchorSecondWidthOffset, 
                                                     this->mLine->line().p2().y() - this->mDefaultLineAnchorSecondHeightOffset,
                                                     this->mDefaultLineAnchorSecondWidth,
                                                     this->mDefaultLineAnchorSecondHeight);
                    
                    this->addToGroup(this->mLine);
                    this->addToGroup(this->mLineAnchorFirst);
                    this->addToGroup(this->mLineAnchorSecond);
                    return; /// break;
                }
                break;
                
            case RECTANGLE:
                if(this->mRectAnchor->contains(event->pos()))
                {
                    if(event->pos().x() > this->mRect->rect().x() && event->pos().y() > this->mRect->rect().y())
                    {
                        this->removeFromGroup(this->mRect);
                        this->removeFromGroup(this->mRectAnchor);
                        this->removeFromGroup(this->mIcon);
                        
                        QRectF rect = this->mRect->rect();
                        rect.setWidth(qAbs(event->pos().x() - this->mRect->rect().x()));
                        rect.setHeight(qAbs(event->pos().y() - this->mRect->rect().y()));
                        
                        this->mRect->setRect(rect);
                        this->mRectAnchor->setRect(this->mRect->rect().x() + this->mRect->rect().width() - this->mDefaultRectAnchorWidthOffset,
                                                   this->mRect->rect().y() + this->mRect->rect().height() - this->mDefaultRectAnchorHeightOffset,
                                                   this->mDefaultRectAnchorWidth,
                                                   this->mDefaultRectAnchorHeight);
                        
                        this->mIcon->setX(this->mRect->rect().center().x() - this->mDefaultIconWidth / 2);
                        this->mIcon->setY(this->mRect->rect().center().y() - this->mDefaultIconHeight / 2);
                        this->mIcon->moveBy(this->mRect->x(), this->mRect->y());
                        
                        this->addToGroup(this->mRect);
                        this->addToGroup(this->mRectAnchor);
                        this->addToGroup(this->mIcon);
                        return; /// break;
                    }
                    else
                    {
                        return;
                    }
                }
                break;
                
            case ELLIPSE:
                if(this->mEllipseAnchor->contains(event->pos()))
                {
                    if(event->pos().x() > this->mEllipse->rect().x() && event->pos().y() > this->mEllipse->rect().y())
                    {
                        this->removeFromGroup(this->mEllipse);
                        this->removeFromGroup(this->mEllipseAnchor);
                        this->removeFromGroup(this->mIcon);
                        
                        QRectF rect = this->mEllipse->rect();
                        rect.setWidth(qAbs(event->pos().x() - this->mEllipse->rect().x()));
                        rect.setHeight(qAbs(event->pos().y() - this->mEllipse->rect().y()));
                        
                        this->mEllipse->setRect(rect);
                        this->mEllipseAnchor->setRect(this->mEllipse->rect().x() + this->mEllipse->rect().width() - this->mDefaultRectAnchorWidthOffset,
                                                      this->mEllipse->rect().y() + this->mEllipse->rect().height() - this->mDefaultRectAnchorHeightOffset,
                                                      this->mDefaultRectAnchorWidth,
                                                      this->mDefaultRectAnchorHeight);
                        
                        this->mIcon->setX(this->mEllipse->rect().center().x() - this->mDefaultIconWidth / 2);
                        this->mIcon->setY(this->mEllipse->rect().center().y() - this->mDefaultIconHeight / 2);
                        this->mIcon->moveBy(this->mEllipse->x(), this->mEllipse->y());
                        
                        this->addToGroup(this->mEllipse);
                        this->addToGroup(this->mEllipseAnchor);
                        this->addToGroup(this->mIcon);
                        return; /// break;
                    }
                    else
                    {
                        return;
                    }
                }
                break;
        }
        
        //move
        this->moveBy(trans.x(), trans.y());
    }
}

void Landmark::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        this->setCursor(Qt::ArrowCursor);    
        if(this->mMoved)
        {
            emit moved(this);
        }
    }
    this->mLeftMouseButtonPressed = false;
}

void Landmark::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    switch(this->mType)
    {
        case LINE:
        case RECTANGLE:
        case ELLIPSE:
            
            QColor col = QColorDialog::getColor();
            
            if(col.isValid())
            {
                this->setColor(col);
            }
            
            break;
    }
}

void Landmark::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    QGraphicsItemGroup::wheelEvent(event);
}

void Landmark::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if(event->modifiers().testFlag(Qt::ControlModifier))
    {
        switch(this->mType)
        {
            case LINE:
                this->mLineAnchorFirst->setVisible(this->mLineAnchorFirst->contains(event->pos()));
                this->mLineAnchorSecond->setVisible(this->mLineAnchorSecond->contains(event->pos()));
                break;
            case RECTANGLE:
                this->mRectAnchor->setVisible(true);
                break;
            case ELLIPSE:
                this->mEllipseAnchor->setVisible(true);
                break;
        } 
    }
    else
    {
        switch(this->mType)
        {
            case LINE:
                this->mLineAnchorFirst->setVisible(this->mLineAnchorFirst->contains(event->pos()));
                this->mLineAnchorSecond->setVisible(this->mLineAnchorSecond->contains(event->pos()));
                break;
            case RECTANGLE:
                this->mRectAnchor->setVisible(this->mRectAnchor->contains(event->pos()));
                break;
            case ELLIPSE:
                this->mEllipseAnchor->setVisible(this->mEllipseAnchor->contains(event->pos()));
                break;
        }
    }
}

void Landmark::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if(event->modifiers().testFlag(Qt::ControlModifier))
    {
        switch(this->mType)
        {
            case LINE:
                this->mLineAnchorFirst->setVisible(this->mLineAnchorFirst->contains(event->pos()));
                this->mLineAnchorSecond->setVisible(this->mLineAnchorSecond->contains(event->pos()));
                break;
            case RECTANGLE:
                this->mRectAnchor->setVisible(true);
                break;
            case ELLIPSE:
                this->mEllipseAnchor->setVisible(true);
                break;
        } 
    }
    else
    {
        switch(this->mType)
        {
            case LINE:
                this->mLineAnchorFirst->setVisible(this->mLineAnchorFirst->contains(event->pos()));
                this->mLineAnchorSecond->setVisible(this->mLineAnchorSecond->contains(event->pos()));
                break;
            case RECTANGLE:
                this->mRectAnchor->setVisible(this->mRectAnchor->contains(event->pos()));
                break;
            case ELLIPSE:
                this->mEllipseAnchor->setVisible(this->mEllipseAnchor->contains(event->pos()));
                break;
        }
    }
}

void Landmark::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    switch(this->mType)
    {
        case LINE:
            this->mLineAnchorFirst->setVisible(false);
            this->mLineAnchorSecond->setVisible(false);
            break;
        case RECTANGLE:
            this->mRectAnchor->setVisible(false);
            break;
        case ELLIPSE:
            this->mEllipseAnchor->setVisible(false);
            break;
    }
}

void Landmark::setColor(QColor& color)
{
    QPen p;
    QBrush b;
    this->mColor = color;
    this->mColor.setAlpha(255);
    switch(this->mType)
    {
        case LINE:
            p = this->mLine->pen();
            p.setWidth(3);
            p.setColor(color);
            this->mLine->setPen(p);
            
            p = this->mLineAnchorFirst->pen();
            p.setColor(color);
            this->mLineAnchorFirst->setPen(p);
            
            p = this->mLineAnchorSecond->pen();
            p.setColor(color);
            this->mLineAnchorSecond->setPen(p);
            
            color.setAlpha(this->mDefaultAlphaValue);
            
            b = this->mLineAnchorFirst->brush();
            b.setStyle(Qt::SolidPattern);
            b.setColor(color);
            this->mLineAnchorFirst->setBrush(b);
            
            b = this->mLineAnchorSecond->brush();
            b.setStyle(Qt::SolidPattern);
            b.setColor(color);
            this->mLineAnchorSecond->setBrush(b);
            break;
            
        case RECTANGLE:
            p = this->mRect->pen();
            p.setColor(color);
            this->mRect->setPen(p);
            
            p = this->mRectAnchor->pen();
            p.setColor(color);
            this->mRectAnchor->setPen(p);
            
            color.setAlpha(this->mDefaultAlphaValue);
            
            b = this->mRect->brush();
            b.setStyle(Qt::SolidPattern);
            b.setColor(color);
            this->mRect->setBrush(b);
            
            b = this->mRectAnchor->brush();
            b.setStyle(Qt::SolidPattern);
            b.setColor(color);
            this->mRectAnchor->setBrush(b);
            
            break;
            
        case ELLIPSE:
            p = this->mEllipse->pen();
            p.setColor(color);
            this->mEllipse->setPen(p);
            
            p = this->mEllipseAnchor->pen();
            p.setColor(color);
            this->mEllipseAnchor->setPen(p);
            
            color.setAlpha(this->mDefaultAlphaValue);
            
            b = this->mEllipse->brush();
            b.setStyle(Qt::SolidPattern);
            b.setColor(color);
            this->mEllipse->setBrush(b);
            
            b = this->mEllipseAnchor->brush();
            b.setStyle(Qt::SolidPattern);
            b.setColor(color);
            this->mEllipseAnchor->setBrush(b);
            break;
    }    
}

