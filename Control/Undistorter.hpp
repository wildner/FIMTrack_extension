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

#ifndef UNDISTORTER_HPP
#define UNDISTORTER_HPP

#include "Configuration/FIMTrack.hpp"
#include "Control/InputGenerator.hpp"

class Undistorter
{
public:
    Undistorter();
    
    Undistorter(std::string const& path);
    
    Undistorter(cv::Mat const& cameraMatrix, 
                cv::Mat const& distCoeffs, 
                cv::Size const& imageSize);
    
    void setPath(std::string const& path);
    
    void setParameter(cv::Mat const& cameraMatrix, 
                      cv::Mat const& distCoeffs, 
                      cv::Size const& imageSize);
    
    void getUndistortImage(cv::Mat const& src, 
                           cv::Mat &dst) const;
    
    bool isReady() const;
    
    void setReady(bool flag);
    
    void reset();
    
private:
    bool        _isInitialised;
    cv::Mat     _cameraMatrix;
    cv::Mat     _distCoeffs;
    cv::Mat     _mapX, _mapY;
    cv::Size    _imageSize;
};

#endif // UNDISTORTER_HPP
