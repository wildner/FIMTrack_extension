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

#include "TrackerConfig.hpp"

namespace StringConstats 
{
   std::string  fileFormats = "*.tif *.tiff *.png";
   std::string  textFileCoding = "UTF-8";
}

namespace GeneralParameters
{
    int      iGrayThreshold                                         = 40;
    int      defaultGrayThreshold                                   = iGrayThreshold;
    
    int      iMinLarvaeArea                                         = 200;
    int      defaultMinLarvaeArea                                   = iMinLarvaeArea;
    
    int      iMaxLarvaeArea                                         = 500;
    int      deaultMaxLarvaeArea                                    = iMaxLarvaeArea;
    
    bool     bShowTrackingProgress                                  = true;
    bool     defaultShowTrackingProgress                            = true;
    
    bool     bSaveLog                                               = false;
    bool     defaultSaveLog                                         = bSaveLog;
    
    bool     bEnableDetailetOutput                                  = true;
    bool     defaultEnableDetailetOutput                            = bEnableDetailetOutput;
}

namespace CameraParameter
{
    QString  File                                                   = "";
    QString  defaultFile                                            = File;
    
    double   dFPS                                                   = 10.0;
    double   defaultFSP                                             = dFPS;
    
    double   dScaleFactor                                           = 1.00;
    double   defaultScaleFactor                                     = dScaleFactor;
}

namespace BackgroundSubstraction
{
    bool     bUseDefault                                            = true;
    bool     defaultUseDefault                                      = bUseDefault;
    
    int      iFromImage                                             = 0;
    int      defaultFromImage                                       = iFromImage;
    
    int      iOffset                                                = 0;
    int      defaultOffset                                          = iOffset;
    
    int      iToImage                                               = 0;
    int      defaultToImage                                         = iToImage;
}

namespace TrackingParameters
{   
    const QString momentumID                                        = QString("Momentum");
    const QString landmarkID                                        = QString("Distance to Landmark");
    const QString bearinAngleID                                     = QString("Bearinangle to Landmark");
    const QStringList parameterForPlotting                          = QStringList() << "Area" << momentumID << "Main Bodybending-Angle" << "Coiled-Indicator" << "Well-Oriented"
                                                                                    << "Perimeter" << "Distance To Origin" << "Momentum Distance" << "Accumulated Distance"
                                                                                    << "Go-Phase Indicator" << "Left-Bending Indicator" << "Right-Bending Indicator"
                                                                                    << "Movement Direction" << landmarkID << "Velocity" << "Acceleration" << bearinAngleID;
}

namespace LarvaeExtractionParameters
{
    bool     bUseDefault                                            = true;
    bool     defaultUseDefault                                      = bUseDefault;
    
    int      iNumerOfSpinePoints                                    = 5;
    int      defaultiNumerOfSpinePoints                             = iNumerOfSpinePoints;

    namespace IPANContourCurvatureParameters
    {
        bool     bUseDynamicIpanParameterCalculation                = true;
        bool     defaultUseDynamicIpanParameterCalculation          = bUseDynamicIpanParameterCalculation;
        
        int      iMinimalTriangelSideLenght                         = 7;
        int      defaultMinimalTriangelSideLenght                   = iMinimalTriangelSideLenght;
        
        int      iMaximalTriangelSideLenght                         = 9;
        int      defaultMaximalTriangelSideLenght                   = iMaximalTriangelSideLenght;
        
        double   dMaximalCurvaturePointsDistance                    = 0.25;
        double   defaultMaximalCurvaturePointsDistance              = dMaximalCurvaturePointsDistance;
        
        int      iCurvatureWindowSize                               = 7;
        int      defaultiCurvatureWindowSize                        = iCurvatureWindowSize;
    }
    
    namespace CoiledRecognitionParameters
    {
        double   dPerimeterToSpinelenghtThreshold                   = 2.4;
        double   defaultPerimeterToSpinelenghtThreshold             = dPerimeterToSpinelenghtThreshold;
        
        double   dMidcirclePerimeterToPerimeterThreshold            = 0.6;
        double   defaultMidcirclePerimeterToPerimeterThreshold      = dMidcirclePerimeterToPerimeterThreshold;

        double   dMaxToMeanRadiusThreshold                          = 2.3;
        double   defaultMaxToMeanRadiusThreshold                    = dMaxToMeanRadiusThreshold;
    }
    
    namespace StopAndGoCalculation
    {
        bool bUseDynamicStopAndGoParameterCalculation               = true;
        bool defauUseDynamicStopAndGoParameterCalculation           = bUseDynamicStopAndGoParameterCalculation;
        
        int iFramesForSpeedCalculation                              = 10;
        int defaultFramesForSpeedCalculation                        = iFramesForSpeedCalculation;
        
        int iSpeedThreshold                                         = 6;
        int defaultSpeedThreshold                                   = iSpeedThreshold;
        
        int iAngleThreshold                                         = 30;
        int defaultAngleThreshold                                   = iAngleThreshold;
    }

    namespace BodyBendingParameters
    {
        bool bUseDynamicBodyBendingCalculation                      = true;
        bool defaultUseDynamicBodyBendingCalculation                = bUseDynamicBodyBendingCalculation;
        
        double dAngleThreshold                                      = 30.0;
        double defaultAngleThreshold                                = dAngleThreshold;
    }

    namespace MovementDirectionParameters
    {
        bool bUseDynamicMovementDirectionParameterCalculation       = true;
        bool defaultUseDynamicMovementDirectionParameterCalculation = bUseDynamicMovementDirectionParameterCalculation;
        
        int iFramesForMovementDirectionCalculation                  = 10;
        int defaultFramesForMovementDirectionCalculation            = iFramesForMovementDirectionCalculation;
    }

    namespace AssignmentParameters
    {
        AssignmentMethod eAssignmentMethod = HUNGARIAN;
        AssignmentMethod defaultAssignmentMethod = eAssignmentMethod;

        CostMeasure eCostMeasure = OVERLAP;
        CostMeasure defaultCostMeasure = eCostMeasure;

        double dDistanceThreshold = 0.5;
        double defaultDistanceThreshold = dDistanceThreshold;

        double dOverlapThreshold = 0.5;
        double defaultOverlapThreshold = dOverlapThreshold;
    }
}

namespace TrackerConfig
{
    void reset()
    {
        GeneralParameters::iGrayThreshold                                                                           = GeneralParameters::defaultGrayThreshold;
        GeneralParameters::iMinLarvaeArea                                                                           = GeneralParameters::defaultMinLarvaeArea;
        GeneralParameters::iMaxLarvaeArea                                                                           = GeneralParameters::deaultMaxLarvaeArea;
        GeneralParameters::bShowTrackingProgress                                                                    = GeneralParameters::defaultShowTrackingProgress;
        GeneralParameters::bSaveLog                                                                                 = GeneralParameters::defaultSaveLog;
        GeneralParameters::bEnableDetailetOutput                                                                    = GeneralParameters::defaultEnableDetailetOutput;
    
        CameraParameter::File                                                                                       = CameraParameter::defaultFile;
        CameraParameter::dFPS                                                                                       = CameraParameter::defaultFSP;
        CameraParameter::dScaleFactor                                                                               = CameraParameter::defaultScaleFactor;
    
        BackgroundSubstraction::bUseDefault                                                                         = BackgroundSubstraction::defaultUseDefault;
        BackgroundSubstraction::iFromImage                                                                          = BackgroundSubstraction::defaultFromImage;
        BackgroundSubstraction::iOffset                                                                             = BackgroundSubstraction::defaultOffset;
        BackgroundSubstraction::iToImage                                                                            = BackgroundSubstraction::defaultToImage;
    
        LarvaeExtractionParameters::bUseDefault                                                                     = LarvaeExtractionParameters::defaultUseDefault;
        LarvaeExtractionParameters::iNumerOfSpinePoints                                                             = LarvaeExtractionParameters::defaultiNumerOfSpinePoints;
    
        LarvaeExtractionParameters::IPANContourCurvatureParameters::bUseDynamicIpanParameterCalculation             = LarvaeExtractionParameters::IPANContourCurvatureParameters::defaultUseDynamicIpanParameterCalculation;
        LarvaeExtractionParameters::IPANContourCurvatureParameters::iMinimalTriangelSideLenght                      = LarvaeExtractionParameters::IPANContourCurvatureParameters::defaultMinimalTriangelSideLenght;
        LarvaeExtractionParameters::IPANContourCurvatureParameters::iMaximalTriangelSideLenght                      = LarvaeExtractionParameters::IPANContourCurvatureParameters::defaultMaximalTriangelSideLenght;
        LarvaeExtractionParameters::IPANContourCurvatureParameters::dMaximalCurvaturePointsDistance                 = LarvaeExtractionParameters::IPANContourCurvatureParameters::defaultMaximalCurvaturePointsDistance;
        LarvaeExtractionParameters::IPANContourCurvatureParameters::iCurvatureWindowSize                            = LarvaeExtractionParameters::IPANContourCurvatureParameters::defaultiCurvatureWindowSize;
        
        LarvaeExtractionParameters::CoiledRecognitionParameters::dPerimeterToSpinelenghtThreshold                   = LarvaeExtractionParameters::CoiledRecognitionParameters::defaultPerimeterToSpinelenghtThreshold;
        LarvaeExtractionParameters::CoiledRecognitionParameters::dMidcirclePerimeterToPerimeterThreshold            = LarvaeExtractionParameters::CoiledRecognitionParameters::defaultMidcirclePerimeterToPerimeterThreshold;
        LarvaeExtractionParameters::CoiledRecognitionParameters::dMaxToMeanRadiusThreshold                          = LarvaeExtractionParameters::CoiledRecognitionParameters::defaultMaxToMeanRadiusThreshold;

        LarvaeExtractionParameters::StopAndGoCalculation::bUseDynamicStopAndGoParameterCalculation                  = LarvaeExtractionParameters::StopAndGoCalculation::defauUseDynamicStopAndGoParameterCalculation;
        LarvaeExtractionParameters::StopAndGoCalculation::iFramesForSpeedCalculation                                = LarvaeExtractionParameters::StopAndGoCalculation::defaultFramesForSpeedCalculation;
        LarvaeExtractionParameters::StopAndGoCalculation::iSpeedThreshold                                           = LarvaeExtractionParameters::StopAndGoCalculation::defaultSpeedThreshold;
        LarvaeExtractionParameters::StopAndGoCalculation::iAngleThreshold                                           = LarvaeExtractionParameters::StopAndGoCalculation::defaultAngleThreshold;

        LarvaeExtractionParameters::BodyBendingParameters::bUseDynamicBodyBendingCalculation                        = LarvaeExtractionParameters::BodyBendingParameters::defaultUseDynamicBodyBendingCalculation;
        LarvaeExtractionParameters::BodyBendingParameters::dAngleThreshold                                          = LarvaeExtractionParameters::BodyBendingParameters::defaultAngleThreshold;

        LarvaeExtractionParameters::MovementDirectionParameters::bUseDynamicMovementDirectionParameterCalculation   = LarvaeExtractionParameters::MovementDirectionParameters::defaultUseDynamicMovementDirectionParameterCalculation;
        LarvaeExtractionParameters::MovementDirectionParameters::iFramesForMovementDirectionCalculation             = LarvaeExtractionParameters::MovementDirectionParameters::defaultFramesForMovementDirectionCalculation;

        LarvaeExtractionParameters::AssignmentParameters::eAssignmentMethod                                         = LarvaeExtractionParameters::AssignmentParameters::defaultAssignmentMethod;
        LarvaeExtractionParameters::AssignmentParameters::eCostMeasure                                              = LarvaeExtractionParameters::AssignmentParameters::defaultCostMeasure;
        LarvaeExtractionParameters::AssignmentParameters::dDistanceThreshold                                        = LarvaeExtractionParameters::AssignmentParameters::defaultDistanceThreshold;
        LarvaeExtractionParameters::AssignmentParameters::dOverlapThreshold                                         = LarvaeExtractionParameters::AssignmentParameters::defaultOverlapThreshold;
    }
}
