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

#include "RegionOfInterest.hpp"

RegionOfInterest::RegionOfInterest()
{
}

RegionOfInterest::RegionOfInterest(QString name, QRectF const& roi, RegionOfInterestType type)
{
    this->mName             = name;
    this->mType             = type;
    this->mRect             = roi;
    
    this->mIsMoveable       = true;
    this->mIsResizeable     = true;
    
    this->mResizeAnchor.setX(this->mRect.x() + this->mRect.width() - this->mDefaultResizeAnchorWidth/2);
    this->mResizeAnchor.setY(this->mRect.y() + this->mRect.height() - this->mDefaultResizeAnchorHeight/2);
    this->mResizeAnchor.setWidth(this->mDefaultResizeAnchorWidth);
    this->mResizeAnchor.setHeight(this->mDefaultResizeAnchorHeight);
    this->mResizeAnchorVisible = false;
    
    this->mResizeAnchorIconFileName     = QString(":/ResultViewer/Icons/Icons/gnome_list_add.png");
    this->mResizeAnchorIcon             = QIcon(this->mResizeAnchorIconFileName);
}

RegionOfInterest::~RegionOfInterest()
{
}

void RegionOfInterest::initalizeAfterLoading()
{
    this->mIsMoveable       = false;
    this->mIsResizeable     = false;
    
    this->mResizeAnchor.setX(this->mRect.x() + this->mRect.width() - this->mDefaultResizeAnchorWidth/2);
    this->mResizeAnchor.setY(this->mRect.y() + this->mRect.height() - this->mDefaultResizeAnchorHeight/2);
    this->mResizeAnchor.setWidth(this->mDefaultResizeAnchorWidth);
    this->mResizeAnchor.setHeight(this->mDefaultResizeAnchorHeight);
    this->mResizeAnchorVisible = false;
    
    this->mResizeAnchorIconFileName     = QString(":/ResultViewer/Icons/Icons/gnome_list_add.png");
    this->mResizeAnchorIcon             = QIcon(this->mResizeAnchorIconFileName);
}

bool RegionOfInterest::contains(QPointF p) const
{
    return (this->mRect.contains(p) || this->mResizeAnchor.contains(p));
}

void RegionOfInterest::translate(qreal dx, qreal dy)
{
    this->mRect.translate(dx, dy);
    this->mResizeAnchor.translate(dx,dy);
}

void RegionOfInterest::translate(QPointF const& p)
{
    this->mRect.translate(p);
    this->mResizeAnchor.translate(p);
}

bool RegionOfInterest::intersects(QRectF const& r)
{
    return this->mRect.intersects(r);
}

void RegionOfInterest::scale(qreal sx, qreal sy)
{
    this->mRect.setX(this->mRect.x() + (this->mRect.width() * ((1 - sx)/2)));
    this->mRect.setY(this->mRect.y() + (this->mRect.height() * ((1 - sy)/2)));

    this->mRect.setWidth(this->mRect.width()*sx);
    this->mRect.setHeight(this->mRect.height()*sy);
}

void RegionOfInterest::setBoundingBox(QRectF const& r)
{
    this->mRect = r;
    this->mResizeAnchor.setX(this->mRect.x() + this->mRect.width() - this->mDefaultResizeAnchorWidth/2);
    this->mResizeAnchor.setY(this->mRect.y() + this->mRect.height() - this->mDefaultResizeAnchorHeight/2);
    this->mResizeAnchor.setWidth(this->mDefaultResizeAnchorWidth);
    this->mResizeAnchor.setHeight(this->mDefaultResizeAnchorHeight);
}
