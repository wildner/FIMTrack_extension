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

#ifndef OUTPUTGENERATOR_HPP
#define OUTPUTGENERATOR_HPP

#include <iterator>
#include <fstream>

#include <QString>
#include <QStringList>
#include <QDateTime>

#include <map>
#include <string>
#include <vector>

#include "Configuration/FIMTrack.hpp"
#include "Configuration/TrackerConfig.hpp"
#include "Data/Larva.hpp"
#include "GUI/RegionOfInterestContainer.hpp"
#include "GUI/LandmarkContainer.hpp"

#include "Utility/FileStorageUtility.hpp"


class OutputGenerator
{
    
public:
    
    static void saveConfiguration(std::string const& path);
    
    static void writePoints(std::string const& path,
                            std::vector<cv::Point> const& points);
    
    static void writeCSVFile(std::string const& path,
                             std::vector<Larva> const& larvae,
                             size_t movieLength,
                             LandmarkContainer const* landmarkContainer = nullptr);
    
    static void writeYMLFile(std::string const& path,
                             std::vector<Larva> const& larvae,
                             std::vector<std::string> const& imgPaths,
                             const bool useUndist,
                             RegionOfInterestContainer const* RIOContainer = nullptr,
                             LandmarkContainer const* landmarkContainer = nullptr);
    
    static void drawTrackingResults(std::string const& trackImgPath,
                                    std::vector<std::string> const& imgPaths,
                                    std::vector<Larva> const& larvae);
    
    static void drawTrackingResultsNoNumbers(std::string const& trackImgPath,
                                             std::vector<std::string> const& imgPaths,
                                             std::vector<Larva> const& larvae);
    
    static void saveResultImage(QString const& path, QImage const& img);
    
private:
    static bool getTimeIntervall(std::vector<Larva> const& larvae, std::pair<int, int>& timeInterval);
    static uint getMinimumNumberOfSpinePoints(std::vector<Larva> const& larvae);
};

#endif // OUTPUTGENERATOR_HPP
