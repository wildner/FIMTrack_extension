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

#ifndef FILESTORAGEUTILITY_HPP
#define FILESTORAGEUTILITY_HPP

#include "Configuration/FIMTrack.hpp"
#include "Data/Larva.hpp"
#include "GUI/LandmarkContainer.hpp"
#include "GUI/RegionOfInterestContainer.hpp"

/// LARVA IN/OUTPUT
cv::FileStorage& operator<<(cv::FileStorage& fs, Larva const& larva);

void operator>>(cv::FileNode const& n, std::vector<cv::Point>& v);
void operator>>(cv::FileNode const& n, cv::Point& p);
void operator>>(cv::FileNode const & n, Larva & larva);




/// LANDMARK IN/OUTPUT
cv::FileStorage& operator<< (cv::FileStorage& fs, Landmark* l);
void operator>> (cv::FileNode const& n, Landmark* l);
void operator>> (cv::FileNode const& n, Landmark::Type& t);




/// RegionOfInterest IN/OUTPUT
cv::FileStorage& operator<< (cv::FileStorage& fs, RegionOfInterest const& r);
void operator>> (cv::FileNode const& n, RegionOfInterest& r);
void operator>> (cv::FileNode const& n, RegionOfInterest::RegionOfInterestType& t);




/// RegionOfInterestContainer IN/OUTPUT
cv::FileStorage& operator<< (cv::FileStorage& fs, RegionOfInterestContainer const& c);
cv::FileStorage& operator<< (cv::FileStorage& fs, RegionOfInterestContainer const* c);
void operator>> (cv::FileNode const& n, RegionOfInterestContainer& c);
void operator>> (cv::FileNode const& n, RegionOfInterestContainer* c);




/// QT IN/OUTPUT
cv::FileStorage& operator<< (cv::FileStorage& fs, QColor const& c);
cv::FileStorage& operator<< (cv::FileStorage& fs, QPointF const& p);
cv::FileStorage& operator<< (cv::FileStorage& fs, QRectF const& r);
cv::FileStorage& operator<< (cv::FileStorage& fs, QString const& s);

void operator>> (cv::FileNode const& n, QColor& c);
void operator>> (cv::FileNode const& n, QPointF& p);
void operator>> (cv::FileNode const& n, QRectF& r);
void operator>> (cv::FileNode const& n, QString& s);



#endif // FILESTORAGEUTILITY_HPP
