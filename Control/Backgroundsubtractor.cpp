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
#include "Backgroundsubtractor.hpp"

Backgroundsubtractor::Backgroundsubtractor(cv::Mat const& backgroundImage, QObject* parent)
	: QObject(parent)
{
	if (!backgroundImage.empty())
	{
		_backgroundImage = backgroundImage.clone();
		_isInitialized = true;
	}
}

Backgroundsubtractor::Backgroundsubtractor(std::vector<std::string> const& imagePaths, Undistorter const& undist, QObject* parent) 
    : QObject(parent)
{
    connect(this, SIGNAL(sendLogMessage(QString, LOGLEVEL)), Logger::getInstance(), SLOT(handleLogMessage(QString, LOGLEVEL)));
    
	if (imagePaths.size() > 0)
	{
		if (BackgroundSubstraction::bUseDefault)
		{
			// use about 20 images as default to reduce computation time; to nevertheless ensure a good quality, pick the images evenly from the whole set
			this->_stepSize = std::max(1, (int)round(imagePaths.size() / 20.0));
			this->generateBackgroundImage(imagePaths, undist);
		}
		else
		{
			this->generateBackgroundImage(imagePaths, BackgroundSubstraction::iFromImage, BackgroundSubstraction::iOffset, BackgroundSubstraction::iToImage, undist);
		}
		_isInitialized = true;
	}
}

void Backgroundsubtractor::subtract(cv::Mat const& src, cv::Mat& dst) const
{
	if (_isInitialized)
	{
		// background subtraction for dark background:
		//dst = src - this->_backgroundImage;

		// background subtraction for bright background:
		dst = this->_backgroundImage - src;
		
		// background subtraction for cases where the background can sometimes be darker and sometimes be brighter than the foreground.
		//dst = abs(src - this->_backgroundImage); 
	}
	else
	{
		src.copyTo(dst);
	}
}

void Backgroundsubtractor::subtractViaThresh(cv::Mat const& src, int const gThresh, cv::Mat& dst) const
{
	if (!_isInitialized)
	{
		src.copyTo(dst);
		return;
	}
    /*
     * Alternative implementation of subtract:
     * Since the background image is B(x,y) = min_{t=1,...,T} I(x,y,t), all pixel in
     * src get a bit darker (src(x,y,t) - B(x,y) < src(x,y,t)). Thus the threshold is
     * applied to the result of the subtraction and all pixel above the threshold are
     * used for further calculations:
     * dst(x,y,t) = src(x,y,t)  if src(x,y,t) - B(x,y) > gThresh     or
     *              0           otherwise
     */
    src.copyTo(dst);
    
    for (int row = 0; row < src.rows; ++row)
    {
        for (int col = 0; col < src.cols; ++col)
        {
            if (abs(src.at<uchar>(row, col) - this->_backgroundImage.at<uchar>(row, col) <= abs(gThresh - this->_backgroundImage.at<uchar>(row, col))))
            {
                dst.at<uchar>(row, col) = 0;
            }
        }
    }
}

void Backgroundsubtractor::readGrayImage(std::string const& path, cv::Mat& dst)
{
    dst = cv::imread(path, CV_LOAD_IMAGE_GRAYSCALE);
}

void Backgroundsubtractor::readColorImage(std::string const& path, cv::Mat& dst)
{
	dst = cv::imread(path, CV_LOAD_IMAGE_COLOR);
}

void Backgroundsubtractor::updateBackgroundImage(cv::Mat const& grayImage)
{
    cv::min(this->_backgroundImage, grayImage, this->_backgroundImage);
}

void Backgroundsubtractor::generateBackgroundImage(std::vector<std::string> const& imagePaths, Undistorter const& undist)
{
    /* are there some images in the imagelist */
    assert(imagePaths.size() > 0);
    
    cv::Mat currentImage;
    
    /* read the first image as the first backgroundimage */
	this->readColorImage(imagePaths.at(0), this->_backgroundImage);
	int srcType = _backgroundImage.type();
    
    if (!this->_backgroundImage.empty())
    {
        emit sendLogMessage(QString("Start calculating Backgroundimage with Default Parameters"), INFO);
        emit sendLogMessage(QString("FromImage = ").append(QString::number(0)).append(" Offset = ").append(QString::number(this->_stepSize)).append(" ToImage = ").append(QString::number(imagePaths.size())), DEBUG);

		// use a container which is big enough to sum up the images (dividing every image
		// by the amount of images and adding it afterwards causes errors if one ore more
		// images cannot be found; in other words: the exact number of images is not known
		// before since loading of images can fail)
		this->_backgroundImage = cv::Mat::zeros(_backgroundImage.size(), CV_64FC3);
		int imgCounter = 0;
        for (size_t i = 0; i < imagePaths.size(); i += this->_stepSize)
        {
            /* read the next image */
			this->readColorImage(imagePaths.at(i), currentImage);
            
            /* if the image could be loaded, add the current image to the background image */
			if (!currentImage.empty())
            {
				currentImage.convertTo(currentImage, CV_64FC3);
				this->_backgroundImage += currentImage;
				imgCounter++;
            }
        }
		if (imgCounter > 1)
		{
			// divide the background image by the amount of images summed up before
			_backgroundImage /= imgCounter;
		}
		_backgroundImage.convertTo(_backgroundImage, srcType);
		cv::cvtColor(_backgroundImage, _backgroundImage, cv::COLOR_BGR2GRAY);
        
        if (undist.isReady())
        {
            cv::Mat dst;
            undist.getUndistortImage(this->_backgroundImage, dst);
            dst.copyTo(this->_backgroundImage);
            dst.release();
        }
        
		currentImage.release();
		emit sendLogMessage(QString("Calculation of the Backgroundimage Done (").append(QString::number(imgCounter)).append(" images)"), INFO);
    }
}

void Backgroundsubtractor::generateBackgroundImage(const std::vector<std::string>& imagePaths, unsigned int iFrom, unsigned int iOffset, unsigned int iTo, Undistorter const& undist)
{
	/* are there some images in the imagelist */
	assert(imagePaths.size() > 0);
	if (!(iFrom < imagePaths.size()))
	{
		iFrom = 0;
	}
    assert(iFrom < imagePaths.size());

	// ensure that all parameters are inside the bounds
	if (iTo > imagePaths.size())
	{
		iOffset = std::max(1, (int)round((float)iOffset * (imagePaths.size() / (float)iTo)));
		iTo = imagePaths.size();
	}
	if (iFrom >= iTo)
	{
		iFrom = iTo < 1 ? 0 : iTo - 1;
	}
    
	cv::Mat currentImage;
    
	/* read the first image as the first backgroundimage */
	this->readColorImage(imagePaths.at(0), this->_backgroundImage);
	int srcType = _backgroundImage.type();
    
    if (!this->_backgroundImage.empty())
    {
        emit sendLogMessage(QString("Start calculating Backgroundimage with user Parameters"), INFO);
        emit sendLogMessage(QString("FromImage = ").append(QString::number(iFrom)).append(" Offset = ").append(QString::number(iOffset)).append(" ToImage = ").append(QString::number(iTo)), DEBUG);
        
		// use a container which is big enough to sum up the images (dividing every image
		// by the amount of images and adding it afterwards causes errors if one ore more
		// images cannot be found; in other words: the exact number of images is not known
		// before since loading of images can fail)
		this->_backgroundImage = cv::Mat::zeros(_backgroundImage.size(), CV_64FC3);
		int imgCounter = 0;
        for (size_t i = iFrom; i < iTo; i += iOffset)
		{
			/* read the next image */
			this->readColorImage(imagePaths.at(i), currentImage);

			/* if the image could be loaded, add the current image to the background image */
			if (!currentImage.empty())
            {
				currentImage.convertTo(currentImage, CV_64FC3);
				this->_backgroundImage += currentImage;
				imgCounter++;
            }
        }
		if (imgCounter > 1)
		{
			// divide the background image by the amount of images summed up before
			_backgroundImage /= imgCounter;
		}
		_backgroundImage.convertTo(_backgroundImage, srcType);
		cv::cvtColor(_backgroundImage, _backgroundImage, cv::COLOR_BGR2GRAY);
        
        if (undist.isReady())
        {
            cv::Mat dst;
            undist.getUndistortImage(this->_backgroundImage, dst);
            dst.copyTo(this->_backgroundImage);
        }
		currentImage.release();
		emit sendLogMessage(QString("Calculation of the Backgroundimage Done (").append(QString::number(imgCounter)).append(" images)"), INFO);
    }
}
