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

#include "OutputGenerator.hpp"

using namespace cv;
using std::vector;

void OutputGenerator::saveConfiguration(const std::string& path)
{
    cv::FileStorage out = cv::FileStorage(path, cv::FileStorage::WRITE, StringConstats::textFileCoding);
    
    if (out.isOpened())
    {
        /* Write GeneralParameters */
        out << "bEnableDetailetOutput"  << GeneralParameters::bEnableDetailetOutput;
        out << "bSaveLog"               << GeneralParameters::bSaveLog;
        out << "bShowTrackingProgress"  << GeneralParameters::bShowTrackingProgress;
        out << "iGrayThreshold"         << GeneralParameters::iGrayThreshold;
        out << "iMaxLarvaeArea"         << GeneralParameters::iMaxLarvaeArea;
        out << "iMinLarvaeArea"         << GeneralParameters::iMinLarvaeArea;
        
        /* Write CameraParameter */
        out << "dFSP"           << CameraParameter::dFPS;
        out << "dScaleFactor"   << CameraParameter::dScaleFactor;
        out << "File"           << QtOpencvCore::qstr2str(CameraParameter::File);
        
        /* Write BackgroundSubstraction Parameters */
        out << "BackgroundSubstractionbUseDefault"  << BackgroundSubstraction::bUseDefault;
        out << "iFromImage"                         << BackgroundSubstraction::iFromImage;
        out << "iOffset"                            << BackgroundSubstraction::iOffset;
        out << "iToImage"                           << BackgroundSubstraction::iToImage;
        
        /* Write LarvaeExtractionParameters */
        out << "LarvaeExtractionParametersbUseDefault"  << LarvaeExtractionParameters::bUseDefault;
        out << "iNumerOfSpinePoints"                    << LarvaeExtractionParameters::iNumerOfSpinePoints;
        
        /* Write IPANContourCurvatureParameters */
        out << "bUseDynamicIpanParameterCalculation"    << LarvaeExtractionParameters::IPANContourCurvatureParameters::bUseDynamicIpanParameterCalculation;
        out << "dMaximalCurvaturePointsDistance"        << LarvaeExtractionParameters::IPANContourCurvatureParameters::dMaximalCurvaturePointsDistance;
        out << "iCurvatureWindowSize"                   << LarvaeExtractionParameters::IPANContourCurvatureParameters::iCurvatureWindowSize;
        out << "iMaximalTriangelSideLenght"             << LarvaeExtractionParameters::IPANContourCurvatureParameters::iMaximalTriangelSideLenght;
        out << "iMinimalTriangelSideLenght"             << LarvaeExtractionParameters::IPANContourCurvatureParameters::iMinimalTriangelSideLenght;
        
        /* Write CoiledRecognitionParameters */
        out << "dMidcirclePerimeterToPerimeterThreshold"    << LarvaeExtractionParameters::CoiledRecognitionParameters::dMidcirclePerimeterToPerimeterThreshold;
        out << "dPerimeterToSpinelenghtThreshold"           << LarvaeExtractionParameters::CoiledRecognitionParameters::dPerimeterToSpinelenghtThreshold;
        out << "dMaxToMeanRadiusRatio"                      << LarvaeExtractionParameters::CoiledRecognitionParameters::dMaxToMeanRadiusThreshold;
        
        /* Write StopAndGoCalculation Parameter */
        out << "bUseDynamicStopAndGoParameterCalculation"   << LarvaeExtractionParameters::StopAndGoCalculation::bUseDynamicStopAndGoParameterCalculation;
        out << "iAngleThreshold"                            << LarvaeExtractionParameters::StopAndGoCalculation::iAngleThreshold;
        out << "iFramesForSpeedCalculation"                 << LarvaeExtractionParameters::StopAndGoCalculation::iFramesForSpeedCalculation;
        out << "iSpeedThreshold"                            << LarvaeExtractionParameters::StopAndGoCalculation::iSpeedThreshold;
    }
    
    out.release();
}

void OutputGenerator::writePoints(std::string const& path,
                                  std::vector<cv::Point> const& points)
{
    cv::FileStorage fs = cv::FileStorage(path, cv::FileStorage::WRITE, StringConstats::textFileCoding);
    if (fs.isOpened())
    {
        fs << "CreationDate" << QDateTime::currentDateTime().toString("ddd MMM dd yyyy hh:mm:ss.zzz").toStdString();
        fs << "features" << "[";
        for (auto const& p : points)
        {
            fs << p;
        }
        fs << "]";
    }
    fs.release();
}

void OutputGenerator::writeCSVFile(std::string const& path,
                                   std::vector<Larva> const& larvae,
                                   size_t movieLength,
                                   LandmarkContainer const* landmarkContainer)
{
    std::ofstream ofs;
    ofs.open(path.c_str());
    
    for (size_t i = 0; i < larvae.size(); ++i)
    {
        ofs << "," << "larva(" << larvae.at(i).getID() << ")";
    }
    ofs << std::endl;
    
    // write momentum_x
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "mom_x(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrMomentum(t, 0).c_str();
        }
        
        ofs << std::endl;
    }
    
    // write momentum_y
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "mom_y(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrMomentum(t, 1).c_str();
        }
        
        ofs << std::endl;
    }
    
    // write distance momentum
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "mom_dst(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrMomentumDist(t).c_str();
        }
        
        ofs << std::endl;
    }
    
    // write accumulated distance momentum
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "acc_dst(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrAccDist(t).c_str();
        }
        
        ofs << std::endl;
    }
    
    // write distance to origin (momentum)
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "dst_to_origin(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrDistToOrigin(t).c_str();
        }
        
        ofs << std::endl;
    }
    
    // write area
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "area(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrArea(t).c_str();
        }
        
        ofs << std::endl;
    }
    
    // write perimeter
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "perimeter(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrPerimeter(t).c_str();
        }
        
        ofs << std::endl;
    }
    
    // write spine length
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "spine_length(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrSpineLength(t).c_str();
        }
        
        ofs << std::endl;
    }
    
    // write body bending
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "bending(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrMainBodyBendingAngle(t).c_str();
        }
        
        ofs << std::endl;
    }
    
    // write head x position
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "head_x(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrSpine(t, 0, 0).c_str();
        }
        
        ofs << std::endl;
    }
    
    // write head y position
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "head_y(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrSpine(t, 0, 1).c_str();
        }
        
        ofs << std::endl;
    }
    
    // write spinepoints
    unsigned int midPos = (int)(((larvae.at(0)).getNSpinePoints() - 1) / 2);
    
    for (uint i = 1; i < larvae.at(0).getNSpinePoints() - 1; ++i)
    {
        // write x position of spinepoint i
        for (size_t t = 0; t < movieLength; ++t)
        {
            ofs << "spinepoint_" << i << "_x(" << t << ")";
            
            for (auto const& l : larvae)
            {
                ofs << "," << l.getStrSpine(t, i, 0).c_str();
            }
            
            ofs << std::endl;
        }
        
        // write y position of spinepoint i
        for (size_t t = 0; t < movieLength; ++t)
        {
            ofs << "spinepoint_" << i << "_y(" << t << ")";
            
            for (auto const& l : larvae)
            {
                ofs << "," << l.getStrSpine(t, i, 1).c_str();
            }
            
            ofs << std::endl;
        }
    }
    
    // write tail x position
    unsigned int tailPos = larvae.at(0).getNSpinePoints() - 1;
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "tail_x(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrSpine(t, tailPos, 0).c_str();
        }
        
        ofs << std::endl;
    }
    
    // write tail y position
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "tail_y(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrSpine(t, tailPos, 1).c_str();
        }
        
        ofs << std::endl;
    }
    
    // write spinepoint radii
    for (uint i = 1; i < larvae.at(0).getNSpinePoints() - 1; ++i)
    {
        for (size_t t = 0; t < movieLength; ++t)
        {
            ofs << "radius_" << i << "(" << t << ")";
            
            for (auto const& l : larvae)
            {
                ofs << "," << l.getStrSpineRadius(t, i).c_str();
            }
            
            ofs << std::endl;
        }
    }
    
    // write is coiled indicator
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "is_coiled(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrIsCoiledIndicator(t);
        }
        
        ofs << std::endl;
    }
    
    // write is well oriented indicator
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "is_well_oriented(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrIsWellOriented(t);
        }
        
        ofs << std::endl;
    }
    
    // write go phase indicator
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "go_phase(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrGoPhaseIndicator(t);
        }
        
        ofs << std::endl;
    }
    
    // write left bended indicator
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "left_bended(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrLeftBendingIndicator(t);
        }
        
        ofs << std::endl;
    }
    
    // write right bended indicator
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "right_bended(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrRightBendingIndicator(t);
        }
        
        ofs << std::endl;
    }
    
    // write movement direction
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "mov_direction(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrMovementDirection(t);
        }
        
        ofs << std::endl;
    }
    
    // write velocity
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "velocity(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrVelosity(t);
        }
        
        ofs << std::endl;
    }
    
    // write acceleration
    for (size_t t = 0; t < movieLength; ++t)
    {
        ofs << "acceleration(" << t << ")";
        
        for (auto const& l : larvae)
        {
            ofs << "," << l.getStrAcceleration(t);
        }
        
        ofs << std::endl;
    }
    
    // write distances to landmarks
    if (landmarkContainer != nullptr)
    {
        // get number of landmarks
        int numberOfLandmarks = landmarkContainer->getSize();
        for (int i = 0; i < numberOfLandmarks; ++i)
        {
            // get name of landmark i
            std::string landmarkName = landmarkContainer->getLandmarkName(i).toStdString();
            
            // write distance to landmark i
            for (unsigned int t = 0; t < movieLength; ++t)
            {
                ofs << "dist_to_" << landmarkName << "(" << t << ")";
                
                for (auto const& l : larvae)
                {
                    ofs << "," << l.getStrDistanceToLandmark(t, landmarkName);
                }
                
                ofs << std::endl;
            }
        }
        
        // write indicator if larva is in landmark
        for (int i = 0; i < numberOfLandmarks; ++i)
        {
            // get name of landmark i
            std::string landmarkName = landmarkContainer->getLandmarkName(i).toStdString();
            
            // write is in landmark i
            for (unsigned int t = 0; t < movieLength; ++t)
            {
                ofs << "is_in_" << landmarkName << "(" << t << ")";
                
                for (auto const& l : larvae)
                {
                    ofs << "," << l.getStrIsInLandmark(t, landmarkName);
                }
                
                ofs << std::endl;
            }
        }
        
        // write bearing angle to landmark
        for (int i = 0; i < numberOfLandmarks; ++i)
        {
            // get name of landmark i
            std::string landmarkName = landmarkContainer->getLandmarkName(i).toStdString();
            
            // write bearing angle to landmark i
            for (unsigned int t = 0; t < movieLength; ++t)
            {
                ofs << "bearing_angle_to_" << landmarkName << "(" << t << ")";
                
                for (auto const& l : larvae)
                {
                    ofs << "," << l.getStrBearingAngleToLandmark(t, landmarkName);
                }
                
                ofs << std::endl;
            }
        }
    }
    
    ofs.flush();
    ofs.close();
    
}

void OutputGenerator::writeYMLFile(const std::string& path,
                                   const std::vector<Larva>& larvae,
                                   const std::vector<std::string>& imgPaths,
                                   const bool useUndist,
                                   const RegionOfInterestContainer* RIOContainer,
                                   const LandmarkContainer* landmarkContainer)
{
    cv::FileStorage fs =  cv::FileStorage(path, cv::FileStorage::WRITE, "UTF-8");
    
    if (fs.isOpened())
    {
        fs << "storageDate" << QDateTime::currentDateTime().toString("ddd MMM dd yyyy hh:mm:ss.zzz").toStdString();
        
        fs << "imgNames" << imgPaths;
        
        fs << "useUndist" << useUndist;
        
        fs << "data" << "[";
        
        for (auto const& l : larvae)
        {
            fs << l;
        }
        
        fs << "]";
        
        if (RIOContainer != nullptr)
        {
            fs << "ROIContainer" << RIOContainer;
        }
        
        if (landmarkContainer != nullptr)
        {
            fs << "LandmarkContainer" << landmarkContainer;
        }
    }
    fs.release();
}

void OutputGenerator::drawTrackingResults(const std::string& trackImgPath,
                                          const std::vector<std::string>& imgPaths,
                                          const std::vector<Larva>& larvae)
{
    cv::Mat tmpImg = cv::imread(imgPaths.at(0), 0);
    // initialize resultant track image
    cv::Mat resultantTrackImage = cv::Mat::zeros(tmpImg.size(), CV_8UC3);
    
    qsrand(QTime::currentTime().msec());
    
    for (std::vector<Larva>::const_iterator larvaIt = larvae.begin(); larvaIt != larvae.end(); ++larvaIt)
    {
        std::vector<cv::Point> midPoints = larvaIt->getAllMidPoints();
        std::vector<cv::Point> headPoints = larvaIt->getAllHeadPoints();
        std::vector<cv::Point> tailPoints = larvaIt->getAllTailPoints();
        
        // calculate random color
        int b = qrand() % 256;
        int g = qrand() % 256;
        int r = qrand() % 256;
        cv::Scalar color(b, g, r);
        
        std::stringstream ss;
        ss << larvaIt->getID();
        cv::putText(resultantTrackImage, ss.str(), midPoints.at(0), CV_FONT_HERSHEY_PLAIN, 2, color, 1);
        
        for (unsigned int index = 0; index < midPoints.size(); ++index)
        {
            cv::line(resultantTrackImage, headPoints.at(index), midPoints.at(index), color, 2);
            cv::line(resultantTrackImage, midPoints.at(index), tailPoints.at(index), color, 2);
        }
    }
    
    cv::imwrite(trackImgPath, resultantTrackImage);
}

void OutputGenerator::drawTrackingResultsNoNumbers(const std::string& trackImgPath,
                                                   const std::vector<std::string>& imgPaths,
                                                   const std::vector<Larva>& larvae)
{
    cv::Mat tmpImg = cv::imread(imgPaths.at(0), 0);
    
    // initialize resultant track image
    cv::Mat resultantTrackImage = cv::Mat::zeros(tmpImg.size(), CV_8UC3);
    
    qsrand(QTime::currentTime().msec());
    
    for (std::vector<Larva>::const_iterator larvaIt = larvae.begin(); larvaIt != larvae.end(); ++larvaIt)
    {
        std::vector<cv::Point> midPoints    = larvaIt->getAllMidPoints();
        std::vector<cv::Point> headPoints   = larvaIt->getAllHeadPoints();
        std::vector<cv::Point> tailPoints   = larvaIt->getAllTailPoints();
        
        // calculate random color
        int b = qrand() % 256;
        int g = qrand() % 256;
        int r = qrand() % 256;
        cv::Scalar color(b, g, r);
        
        for (unsigned int index = 0; index < midPoints.size(); ++index)
        {
            cv::line(resultantTrackImage, headPoints.at(index), midPoints.at(index), color, 2);
            cv::line(resultantTrackImage, midPoints.at(index), tailPoints.at(index), color, 2);
        }
    }
    
    cv::imwrite(trackImgPath, resultantTrackImage);
    tmpImg.release();
    resultantTrackImage.release();
}

void OutputGenerator::saveResultImage(const QString& path, const QImage& img)
{
    cv::imwrite(QtOpencvCore::qstr2str(path), QtOpencvCore::qimg2img(img));
}

bool OutputGenerator::getTimeIntervall(std::vector<Larva> const& larvae, std::pair<int, int>& timeInterval)
{
    bool foundSomething = false;
    timeInterval.first  = std::numeric_limits<int>::max();
    timeInterval.second = -1;
    
    std::vector<uint> timeSteps;
    
    for (size_t i = 0; i < larvae.size(); ++i)
    {
        timeSteps           = larvae.at(i).getAllTimeSteps();
        timeInterval.first  = std::min(timeInterval.first, static_cast<int>(timeSteps.front()));
        timeInterval.second = std::max(timeInterval.second, static_cast<int>(timeSteps.back()));
        foundSomething = true;
    }
    
    return foundSomething;
}

uint OutputGenerator::getMinimumNumberOfSpinePoints(const std::vector<Larva>& larvae)
{
    uint minNumber = std::numeric_limits<uint>::max();
    for (size_t i = 0; i < larvae.size(); ++i)
    {
        minNumber = std::min(minNumber, larvae.at(i).getNSpinePoints());
    }
    return minNumber;
}


