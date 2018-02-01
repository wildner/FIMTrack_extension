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

#include "InputGenerator.hpp"

void InputGenerator::readMatrices(const std::string& path,
                                  cv::Mat& cameraMatrix,
                                  cv::Mat& distCoeffs,
                                  cv::Size& imageSize)
{
    cv::FileStorage fs = cv::FileStorage(path, cv::FileStorage::READ, StringConstats::textFileCoding);
    if (fs.isOpened())
    {
        fs["cameraMatrix"]  >> cameraMatrix;
        fs["distCoeffs"]    >> distCoeffs;
        fs["ImageHeight"]   >> imageSize.height;
        fs["ImageWidth"]    >> imageSize.width;
        fs.release();
    }
}

void InputGenerator::readOutputLarvae(const std::string& path, std::vector<Larva>& dstLarvae, std::vector<std::string>& imgPaths, bool& useUndist)
{
    cv::FileStorage fs = cv::FileStorage(path, cv::FileStorage::READ, StringConstats::textFileCoding);

    if (fs.isOpened())
    {
        int undist;
        fs["useUndist"] >> undist;
        useUndist = (undist == 0) ? false : true;

        fs["imgNames"] >> imgPaths;


        cv::FileNode node = fs["data"];

        for (auto const& n : node)
        {
            Larva l;
            n >> l;
            dstLarvae.push_back(l);
        }
    }
    fs.release();
}

void InputGenerator::readRegionOfInterests(const std::string& path, RegionOfInterestContainer* ROIContainert)
{
    cv::FileStorage fs = cv::FileStorage(path, cv::FileStorage::READ, StringConstats::textFileCoding);

    if (fs.isOpened())
    {
        fs["ROIContainer"] >> ROIContainert;
    }

    fs.release();
}

void InputGenerator::readLandmarks(const std::string& path, LandmarkContainer* landmarkContainer)
{
    cv::FileStorage fs = cv::FileStorage(path, cv::FileStorage::READ, StringConstats::textFileCoding);

    if (fs.isOpened())
    {
        fs["LandmarkContainer"] >> landmarkContainer;
    }

    fs.release();
}

void InputGenerator::loadConfiguration(const std::string& path)
{
    cv::FileStorage in = cv::FileStorage(path, cv::FileStorage::READ, StringConstats::textFileCoding);
    if (in.isOpened())
    {
        std::string temp;

        /* Read GeneralParameters */
        in["bEnableDetailetOutput"]     >> GeneralParameters::bEnableDetailetOutput;
        in["bSaveLog"]                  >> GeneralParameters::bSaveLog;
        in["bShowTrackingProgress"]     >> GeneralParameters::bShowTrackingProgress;
        in["iGrayThreshold"]            >> GeneralParameters::iGrayThreshold;
        in["iMaxLarvaeArea"]            >> GeneralParameters::iMaxLarvaeArea;
        in["iMinLarvaeArea"]            >> GeneralParameters::iMinLarvaeArea;

        /* Read CameraParameter */
        in["dFSP"]          >> CameraParameter::dFPS;
        in["dScaleFactor"]  >> CameraParameter::dScaleFactor;
        in["File"]          >> temp;
        CameraParameter::File = QtOpencvCore::str2qstr(temp);

        /* Read BackgroundSubstraction Parameters */
        in["BackgroundSubstractionbUseDefault"] >> BackgroundSubstraction::bUseDefault;
        in["iFromImage"]                        >> BackgroundSubstraction::iFromImage;
        in["iOffset"]                           >> BackgroundSubstraction::iOffset;
        in["iToImage"]                          >> BackgroundSubstraction::iToImage;

        /* Read LarvaeExtractionParameters */
        in["LarvaeExtractionParametersbUseDefault"] >> LarvaeExtractionParameters::bUseDefault;
        in["iNumerOfSpinePoints"]                   >> LarvaeExtractionParameters::iNumerOfSpinePoints;

        /* Read IPANContourCurvatureParameters */
        in["bUseDynamicIpanParameterCalculation"]   >> LarvaeExtractionParameters::IPANContourCurvatureParameters::bUseDynamicIpanParameterCalculation;
        in["dMaximalCurvaturePointsDistance"]       >> LarvaeExtractionParameters::IPANContourCurvatureParameters::dMaximalCurvaturePointsDistance;
        in["iCurvatureWindowSize"]                  >> LarvaeExtractionParameters::IPANContourCurvatureParameters::iCurvatureWindowSize;
        in["iMaximalTriangelSideLenght"]            >> LarvaeExtractionParameters::IPANContourCurvatureParameters::iMaximalTriangelSideLenght;
        in["iMinimalTriangelSideLenght"]            >> LarvaeExtractionParameters::IPANContourCurvatureParameters::iMinimalTriangelSideLenght;

        /* Read CoiledRecognitionParameters */
        in["dMidcirclePerimeterToPerimeterThreshold"]   >> LarvaeExtractionParameters::CoiledRecognitionParameters::dMidcirclePerimeterToPerimeterThreshold;
        in["dPerimeterToSpinelenghtThreshold"]          >> LarvaeExtractionParameters::CoiledRecognitionParameters::dPerimeterToSpinelenghtThreshold;
        in["dMaxToMeanRadiusThreshold"]                 >> LarvaeExtractionParameters::CoiledRecognitionParameters::dMaxToMeanRadiusThreshold;

        /* Read StopAndGoCalculation Parameter */
        in["bUseDynamicStopAndGoParameterCalculation"]  >> LarvaeExtractionParameters::StopAndGoCalculation::bUseDynamicStopAndGoParameterCalculation;
        in["iAngleThreshold"]                           >> LarvaeExtractionParameters::StopAndGoCalculation::iAngleThreshold;
        in["iFramesForSpeedCalculation"]                >> LarvaeExtractionParameters::StopAndGoCalculation::iFramesForSpeedCalculation;
        in["iSpeedThreshold"]                           >> LarvaeExtractionParameters::StopAndGoCalculation::iSpeedThreshold;
    }
    in.release();
}
