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

#include "Undistorter.hpp"

Undistorter::Undistorter()
{
    this->_isInitialised = false;
}

Undistorter::Undistorter(std::string const& path)
{
    InputGenerator::readMatrices(path, this->_cameraMatrix, this->_distCoeffs, this->_imageSize);
    cv::initUndistortRectifyMap(this->_cameraMatrix,
                                this->_distCoeffs,
                                cv::Mat(),
                                this->_cameraMatrix,
                                this->_imageSize,
                                CV_32FC1,
                                this->_mapX, this->_mapY);
    this->_isInitialised = true;
    
}

Undistorter::Undistorter(cv::Mat const& cameraMatrix, 
                         cv::Mat const& distCoeffs, 
                         cv::Size const& imageSize)
{
    this->_cameraMatrix = cameraMatrix;
    this->_distCoeffs = distCoeffs;
    this->_imageSize = imageSize;
    
    cv::initUndistortRectifyMap(this->_cameraMatrix,
                                this->_distCoeffs,
                                cv::Mat(),
                                this->_cameraMatrix,
                                this->_imageSize,
                                CV_32FC1,
                                this->_mapX, this->_mapY);
    this->_isInitialised = true;
}

void Undistorter::setPath(std::string const& path)
{
    InputGenerator::readMatrices(path, this->_cameraMatrix, this->_distCoeffs, this->_imageSize);
    cv::initUndistortRectifyMap(this->_cameraMatrix,
                                this->_distCoeffs,
                                cv::Mat(),
                                this->_cameraMatrix,
                                this->_imageSize,
                                CV_32FC1,
                                this->_mapX, this->_mapY);
    this->_isInitialised = true;
}

void Undistorter::setParameter(cv::Mat const& cameraMatrix, 
                               cv::Mat const& distCoeffs, 
                               cv::Size const& imageSize)
{
    this->_cameraMatrix = cameraMatrix;
    this->_distCoeffs = distCoeffs;
    this->_imageSize = imageSize;
    
    cv::initUndistortRectifyMap(this->_cameraMatrix,
                                this->_distCoeffs,
                                cv::Mat(),
                                this->_cameraMatrix,
                                this->_imageSize,
                                CV_32FC1,
                                this->_mapX, this->_mapY);
    this->_isInitialised = true;
}

void Undistorter::getUndistortImage(const cv::Mat &src, cv::Mat &dst) const
{
    if(this->_isInitialised)
    {
        cv::remap(src, dst, this->_mapX, this->_mapY, cv::INTER_CUBIC);
    }
}

bool Undistorter::isReady() const
{
    return this->_isInitialised;
}

void Undistorter::setReady(bool flag)
{
    this->_isInitialised = flag;
}

void Undistorter::reset()
{
    this->_isInitialised = false;
    this->_cameraMatrix.release();
    this->_distCoeffs.release();
    this->_mapX.release();
    this->_mapY.release();
    this->_imageSize.height = this->_imageSize.width = 0;
}
