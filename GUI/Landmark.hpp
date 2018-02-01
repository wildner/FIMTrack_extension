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

#ifndef LANDMARK_HPP
#define LANDMARK_HPP

#include "Configuration/FIMTrack.hpp"

#include <QString>
#include <QTime>
#include <QRectF>
#include <QPointF>
#include <QRegion>
#include <QIcon>
#include <QCursor>
#include <QColorDialog>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>
#include <QGraphicsPixmapItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsSceneHoverEvent>

class Landmark : public QObject, public QGraphicsItemGroup
{
    
    Q_OBJECT
    
signals:
    void moved(Landmark const* l);
    
private:
    
    /* Default Constants */
    static const int        mDefaultAlphaValue                      = 100;
    
    static const int        mDefaultIconWidth                       = 36;
    static const int        mDefaultIconHeight                      = 36;
    
    static const int        mDefaultRectWidth                       = mDefaultIconWidth * 2;
    static const int        mDefaultRectHeight                      = mDefaultIconHeight * 2;
    
    static const int        mDefaultEllipseWidth                    = mDefaultIconWidth * 2;
    static const int        mDefaultEllipseHeight                   = mDefaultIconHeight * 2;
    
    static const int        mDefaultLineLength                      = 100;
    static const int        mDefaultLineAnchorFirstWidthOffset      = 15;
    static const int        mDefaultLineAnchorFirstWidth            = mDefaultLineAnchorFirstWidthOffset * 2;
    static const int        mDefaultLineAnchorFirstHeightOffset     = 15;
    static const int        mDefaultLineAnchorFirstHeight           = mDefaultLineAnchorFirstHeightOffset * 2;
    static const int        mDefaultLineAnchorSecondWidthOffset     = 15;
    static const int        mDefaultLineAnchorSecondWidth           = mDefaultLineAnchorSecondWidthOffset * 2;
    static const int        mDefaultLineAnchorSecondHeightOffset    = 15;
    static const int        mDefaultLineAnchorSecondHeight          = mDefaultLineAnchorSecondHeightOffset * 2;
    
    static const int        mDefaultRectAnchorWidthOffset           = 15;
    static const int        mDefaultRectAnchorWidth                 = mDefaultRectAnchorWidthOffset * 2;
    static const int        mDefaultRectAnchorHeightOffset          = 15;
    static const int        mDefaultRectAnchorHeight                = mDefaultRectAnchorHeightOffset * 2;
    
    static const int        mDefaultEllipseAnchorWidthOffset        = 15;
    static const int        mDefaultEllipseAnchorWidth              = mDefaultEllipseAnchorWidthOffset * 2;
    static const int        mDefaultEllipseAnchorHeightOffset       = 15;
    static const int        mDefaultEllipseAnchorHeight             = mDefaultEllipseAnchorHeightOffset * 2;
    
    bool                    mLeftMouseButtonPressed;
    bool                    mMoved;
    
    bool                    mIsResizeable;
    int                     mType;    
    QString                 mName;
    QGraphicsScene*         mParentScene;
    QColor                  mColor;
    
    /// Type = POINT | RECTANGLE | ELLIPSE
    QGraphicsPixmapItem*    mIcon;
    
    /// Type = LINE
    QGraphicsLineItem*      mLine;
    QGraphicsEllipseItem*   mLineAnchorFirst;
    QGraphicsEllipseItem*   mLineAnchorSecond;
    
    /// Type = RECTANGLE
    QGraphicsRectItem*      mRect;
    QGraphicsEllipseItem*   mRectAnchor;
    
    /// Type = ELLIPSE
    QGraphicsEllipseItem*	mEllipse;
    QGraphicsEllipseItem*   mEllipseAnchor;
    
    QPointF                 mLastMousePos;
    
    void initialize(qreal x, qreal y);
    
    void initalizeAfterLoading(QPointF const& scenePos);
    void initalizeAfterLoading(QPointF const& sceneP1, QPointF const& sceneP2);
    void initalizeAfterLoading(QRectF const& sceneRect);
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    
    void wheelEvent(QGraphicsSceneWheelEvent *event);
    
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    
    void setColor(QColor &color);
    
public:
    
    enum Type
    {
        POINT,
        LINE,
        RECTANGLE,
        ELLIPSE
    };
    
    Landmark();
    Landmark(QString const& name, qreal xPos, qreal yPos, int type);
    ~Landmark(); 
    
    /// GETTER
    QString         getName()       const   {return this->mName;}
    int             getType()       const   {return this->mType;}
    
    QPointF         getPoint()      const;
    QLineF          getLine()       const; 
    QRectF          getRectagle()   const;
    QRectF          getEllipse()    const;
    
    bool contains(const QPointF &point) const;
    
    void cleanupAfterRemoving();
    
    friend cv::FileStorage& operator<< (cv::FileStorage& fs, Landmark* l);
    friend void operator>> (cv::FileNode const& n, Landmark* l);
};

#endif // LANDMARK_HPP
