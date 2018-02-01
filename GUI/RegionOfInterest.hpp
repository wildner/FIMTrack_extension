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

#ifndef REGIONOFINTEREST_HPP
#define REGIONOFINTEREST_HPP

#include "Configuration/FIMTrack.hpp"

#include <QTime>
#include <QString>
#include <QRectF>
#include <QIcon>

class RegionOfInterest
{    
    
private:   
    
    static const int        mDefaultResizeAnchorWidth   = 50;
    static const int        mDefaultResizeAnchorHeight  = 50;
    
    bool                    mIsMoveable;
    bool                    mIsResizeable;
    QString                 mName;
    int                     mType;
    QRectF                  mRect;
    bool                    mResizeAnchorVisible;
    QRectF                  mResizeAnchor;
    QIcon                   mResizeAnchorIcon;
    QString                 mResizeAnchorIconFileName;
    
    void                    initalizeAfterLoading();
    
public:
    
    enum RegionOfInterestType
    {
        RECTANGLE,
        ELLIPSE
    };
    
    explicit RegionOfInterest();
    explicit RegionOfInterest(QString name, QRectF const& roi, RegionOfInterestType type);
    ~RegionOfInterest();
    
    bool contains(QPointF p) const;
    void translate(qreal dx, qreal dy);
    void translate(QPointF const& p);
    
    bool intersects (QRectF const& r);
    
    void scale(qreal sx, qreal sy);
    
    /// GETTER
    QString                 getName()                                   const   {return this->mName;}
    int                     getType()                                   const   {return this->mType;}
    QRectF                  getBoundingBox()                            const   {return this->mRect;}
    QRectF                  getResizeAnchor()                           const   {return this->mResizeAnchor;}
    bool                    getResizeAnchorVisibility()                 const   {return this->mResizeAnchorVisible;}
    QPixmap                 getResizeAnchorIconPixmap()                 const   {return this->mResizeAnchorIcon.pixmap(this->mDefaultResizeAnchorWidth, this->mDefaultResizeAnchorHeight);}
    
    bool                    isMoveable()                                const   {return this->mIsMoveable;}
    bool                    isResizeable()                              const   {return this->mIsResizeable;}
    
    /// SETTER
    void                    setBoundingBox(QRectF const& r);
    void                    setResizeAnchorVisibility(bool visible)             {this->mResizeAnchorVisible = visible;}
    void                    setResizeAnchorVisibility(QPointF const& p)         {this->mResizeAnchorVisible = this->mResizeAnchor.contains(p);}
    
    friend cv::FileStorage& operator<< (cv::FileStorage& fs, RegionOfInterest const& r);
    friend void operator>> (cv::FileNode const& n, RegionOfInterest& r);
};

#endif // REGIONOFINTEREST_HPP
