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

#ifndef BACKGROUNDSUBTRACTOR_HPP
#define BACKGROUNDSUBTRACTOR_HPP

#include <math.h>
#include "Configuration/FIMTrack.hpp"
#include "Configuration/TrackerConfig.hpp"
#include "Logger.hpp"
#include "Undistorter.hpp"

class Backgroundsubtractor : public QObject
{

    Q_OBJECT

signals:
    void sendLogMessage(QString msg, LOGLEVEL);

private:

    cv::Mat _backgroundImage;
    int     _stepSize;

    void readImage(std::string const& path, cv::Mat& dst);

    void updateBackgroundImage(cv::Mat const& grayImage);

    void generateBackgroundImage(std::vector<std::string> const& imagePaths, Undistorter const& undist);

    void generateBackgroundImage(std::vector<std::string> const& imagePaths, unsigned int iFrom, unsigned int iOffset, unsigned int iTo, Undistorter const& undist);

public:
    Backgroundsubtractor(std::vector<std::string> const& imagePaths, Undistorter const& undist, QObject* parent = 0);

    void substract(cv::Mat const& src, cv::Mat& dst) const;


    // NOTE: this method might do the same like subtract but in a slightly different fashion...
    // TODO: check for redundance and delete if necessary.
    void subtractViaThresh(cv::Mat const& src, int const gThresh, cv::Mat& dst) const;
};

#endif // BACKGROUNDSUBTRACTOR_HPP
