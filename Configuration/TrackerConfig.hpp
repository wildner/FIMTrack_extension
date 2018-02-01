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

#ifndef TRACKERCONFIG_HPP
#define TRACKERCONFIG_HPP

#include <QString>
#include <QStringList>
#include <vector>
#include <opencv2/opencv.hpp>

namespace FIMTypes
{
    /**
     * @brief contourType is used to store contour points
     */
    typedef std::vector<cv::Point> contour_t;
    /**
     * @brief contourTypeF is used to store contour points with float coordinates
     */
    typedef std::vector<cv::Point2f> contourF_t;
    /**
     * @brief contourTypeD is used to store contour points with double coordinates
     */
    typedef std::vector<cv::Point2d> contourD_t;
    /**
     * @brief contoursType is used to store vectors of contourType
     */
    typedef std::vector<contour_t> contours_t;
    /**
     * @brief spineType is used to store spine points
     */
    typedef std::vector<cv::Point> spine_t;
    /**
     * @brief spineTypeF is used to store spine points with float coordinates
     */
    typedef std::vector<cv::Point2f> spineF_t;
    /**
     * @brief contourCurvatureType is used to store curvatures of contour points
     */
    typedef std::vector<double> contourCurvature_t;
    /**
     * @brief radiiType is used to store the radii of the discrete spine points
     */
    typedef std::vector<double> radii_t;
    /**
     * @brief BezierCurve contains control points of a Bezier Curve
     */
    typedef std::vector<cv::Point2d> BezierCurve_t;
    /**
     * @brief BezierCurves contains several Bezier Curves that approximate a digitized curve
     */
    typedef std::vector<BezierCurve_t> BezierCurves_t;
}

namespace StringConstats 
{
   extern std::string   fileFormats;
   extern std::string   textFileCoding;
}

namespace GeneralParameters
{
    extern int      iGrayThreshold;
    extern int      iMinLarvaeArea;
    extern int      iMaxLarvaeArea;
    extern bool     bShowTrackingProgress;
    extern bool     bSaveLog;
    extern bool     bEnableDetailetOutput;
}

namespace CameraParameter
{
    extern QString  File;
    extern double   dFPS;
    extern double   dScaleFactor;
}

namespace BackgroundSubstraction 
{
    extern bool     bUseDefault;
    extern int      iFromImage;
    extern int      iOffset;
    extern int      iToImage;
}


namespace TrackingParameters 
{  
    extern const QString momentumID;
    extern const QString landmarkID;
    extern const QString bearinAngleID;
    extern const QStringList parameterForPlotting;
}

namespace LarvaeExtractionParameters
{
    extern bool     bUseDefault;
    extern int      iNumerOfSpinePoints;
    
    namespace IPANContourCurvatureParameters 
    {
        extern bool     bUseDynamicIpanParameterCalculation;
        extern int      iMinimalTriangelSideLenght;
        extern int      iMaximalTriangelSideLenght;
        extern double   dMaximalCurvaturePointsDistance;
        extern int      iCurvatureWindowSize;
    }
    
    namespace CoiledRecognitionParameters 
    {
        extern double   dPerimeterToSpinelenghtThreshold;
        extern double   dMidcirclePerimeterToPerimeterThreshold;
        extern double   dMaxToMeanRadiusThreshold;
    }
    
    namespace StopAndGoCalculation
    {
        extern bool bUseDynamicStopAndGoParameterCalculation;
        extern int iFramesForSpeedCalculation;
        extern int iSpeedThreshold;
        extern int iAngleThreshold;
    }
    
    namespace BodyBendingParameters
    {
        extern bool bUseDynamicBodyBendingCalculation;
        extern double dAngleThreshold;
    }
    
    namespace MovementDirectionParameters
    {
        extern bool bUseDynamicMovementDirectionParameterCalculation;
        extern int iFramesForMovementDirectionCalculation;
    }
    
    namespace AssignmentParameters
    {
        
        enum AssignmentMethod 
        {
            HUNGARIAN, 
            GREEDY
        };
        
        enum CostMeasure
        {
            MOMENTUM, 
            OVERLAP, 
            MID_SPINE_POINT
        };
        
        extern AssignmentMethod eAssignmentMethod;
        extern CostMeasure eCostMeasure;
        extern double dDistanceThreshold;
        extern double dOverlapThreshold;
    }
}

namespace TrackerConfig
{
    void reset();
}

#endif // TRACKERCONFIG_HPP
