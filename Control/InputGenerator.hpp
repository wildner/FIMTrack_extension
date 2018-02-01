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

#ifndef INPUTGENERATOR_HPP
#define INPUTGENERATOR_HPP

#include "Configuration/FIMTrack.hpp"
#include "Configuration/TrackerConfig.hpp"
#include "Data/Larva.hpp"
#include "GUI/RegionOfInterestContainer.hpp"
#include "GUI/LandmarkContainer.hpp"

#include "Utility/FileStorageUtility.hpp"

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <iterator>



class InputGenerator
{
public:
    static void readMatrices(std::string const& path,
                             cv::Mat& cameraMatrix,
                             cv::Mat& distCoeffs,
                             cv::Size& imageSize);

    static void loadConfiguration(std::string const& path);

    /**
     * @brief readOutputLarvae reads all larvaes from a yml file for output
     * @param path the path where to find the larvae yml file
     * @param dstLarvae the container containing all the larvae after reading from the file
     * @param imgPaths contains the paths to the images
     * @param useUndist is a flag to indicate if undistortion (for lens distortion using a camera matrix) is used
     */
    static void readOutputLarvae(std::string const& path, std::vector<Larva>& dstLarvae, std::vector<std::string>& imgPaths, bool& useUndist);

    static void readRegionOfInterests(const std::string& path, RegionOfInterestContainer* ROIContainert);

    static void readLandmarks(const std::string& path, LandmarkContainer* landmarkContainer);

private:


};

#endif // INPUTGENERATOR_HPP
