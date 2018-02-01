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

#include "RawLarva.hpp"

using namespace cv;
using namespace FIMTypes;
using std::vector;

RawLarva::RawLarva(const contour_t& _contour, Mat const & img)
{

    Q_UNUSED(img);
    contour=_contour;

    // set the contour area value
    area = contourArea(contour);

    // number of discrete spine points
    int nPoints = 5;

    // setting the relative distToMax value for secondMaxCurvaturePoint calculation (at least
    // 25% away from the first curvature maximum
    double distToMax = 0.25;

    // dMin and dMax values for the IPAN algorithm (i.e. curvature calculation)
    int dMin = contour.size() * 0.06;
    int dMax = dMin + 2;

    // odd mask size for max curvature calculation
    int maskSize = (dMin % 2 == 0) ? dMin+1 : dMin;

    double peri2spineLengthThresh = 2.6;
    double midCirclePeri2PeriThresh = 0.5;
    double max2meanRadiusThresh = 2.3;

    if(!LarvaeExtractionParameters::bUseDefault)
    {
        nPoints = LarvaeExtractionParameters::iNumerOfSpinePoints;

        peri2spineLengthThresh = LarvaeExtractionParameters::CoiledRecognitionParameters::dPerimeterToSpinelenghtThreshold;
        midCirclePeri2PeriThresh = LarvaeExtractionParameters::CoiledRecognitionParameters::dMidcirclePerimeterToPerimeterThreshold;
        max2meanRadiusThresh = LarvaeExtractionParameters::CoiledRecognitionParameters::dMaxToMeanRadiusThreshold;
    }

    if(!LarvaeExtractionParameters::IPANContourCurvatureParameters::bUseDynamicIpanParameterCalculation)
    {
        dMin = LarvaeExtractionParameters::IPANContourCurvatureParameters::iMinimalTriangelSideLenght;
        dMax = LarvaeExtractionParameters::IPANContourCurvatureParameters::iMaximalTriangelSideLenght;
        maskSize = LarvaeExtractionParameters::IPANContourCurvatureParameters::iCurvatureWindowSize;
        distToMax = LarvaeExtractionParameters::IPANContourCurvatureParameters::dMaximalCurvaturePointsDistance;
    }

    SpineIPAN spineCalc;
    spineCalc.calcSpineParameters(&contour,
                                  &spine,
                                  &discreteSpine,
                                  &larvalRadii,
                                  tailIndex,
                                  spineLength,
                                  dMin,
                                  dMax,
                                  maskSize,
                                  distToMax,
                                  nPoints);


    // set the momentum
    calcMomentum();

    calcIsCoiledIndicator(peri2spineLengthThresh,midCirclePeri2PeriThresh,max2meanRadiusThresh);

}

void RawLarva::calcIsCoiledIndicator(const double peri2spineLengthThresh,
                                     const double midCirclePeri2PeriThresh,
                                     const double max2meanRadiusThresh)
{

    /* FIRST: peri2spineLengthRatio */
    double perimeter = getContourPerimeter();
    double spineLength = getSpineLength();
    double peri2spineLengthRatio = perimeter / spineLength;

    /* SECOND: midCirclePeri2PeriRatio */
    // for checking the ratio, first calculate maximum radius by using distance map
    cv::Rect rect = cv::boundingRect(contour);
    // enlarge bounding rect
    rect.x = rect.x - 1;
    rect.y = rect.y - 1;
    rect.width = rect.width + 2;
    rect.height = rect.height + 2;
    cv::Point offset = cv::Point(rect.x,rect.y);
    // draw filled contour into an image to have a mask
    cv::Mat contourMask = cv::Mat::zeros(rect.size(), CV_8UC1);
    std::vector<std::vector<cv::Point> > contourContainer;
    contourContainer.push_back(contour);
    cv::drawContours(contourMask, contourContainer, -1, cv::Scalar(1), CV_FILLED, 8,
                     cv::noArray(), 2147483647, cv::Point(-offset.x,-offset.y));
    // calculate distance map
    cv::Mat distMap = cv::Mat::zeros(rect.size(), CV_32FC1);
    cv::distanceTransform(contourMask, distMap, CV_DIST_L2, CV_DIST_MASK_PRECISE);

    // now, calc maximum radius and midCirclePeri2PeriRatio
    //int midPointIndex = (int) ((discreteSpine.size()-1) /2);
    double maxRadius;// = larvalRadii.at(midPointIndex);
    cv::minMaxLoc(distMap, 0, &maxRadius);
    double midCirclePeri2PeriRatio = (2*maxRadius*CV_PI) / perimeter;

    /* THIRD: max2meanRadiusRatio */
    // in general mean radius can be approximated by 0.5*(area/spineLength)
    // but spineLength is corrupted by the coiled status, so spineLength
    // will be approximated by the longer contour half
    contour_t firstHalf;
    firstHalf.reserve(tailIndex);
    firstHalf.insert(firstHalf.begin(), contour.begin(), contour.begin() + tailIndex);
    contour_t secondHalf;
    secondHalf.reserve(contour.size() - tailIndex);
    secondHalf.insert(secondHalf.begin(), contour.begin() + tailIndex, contour.end());
    // calc lengths of the two halfs
    double firstHalfLength = cv::arcLength(firstHalf, false);
    double secondHalfLength = cv::arcLength(secondHalf, false);
    // choose maximum of those lengths
    double maxHalfLength = std::max(firstHalfLength, secondHalfLength);
    // approx mean radius and calc max2meanRadiusRatio
    double meanRadius = 0.5*(area / maxHalfLength);
    double max2meanRadiusRatio = maxRadius / meanRadius;

    if((peri2spineLengthRatio >= peri2spineLengthThresh &&
        midCirclePeri2PeriRatio >= midCirclePeri2PeriThresh)
           || max2meanRadiusRatio >= max2meanRadiusThresh)
    {
        isCoiled = true;
    }
    else
    {
        isCoiled = false;
    }
}

void RawLarva::calcMomentum(void)
{
    // calculate the moments of the contour
    Moments moms = moments(contour);
    // calculate center of the moments
    double cx = moms.m10 / moms.m00;
    double cy = moms.m01 / moms.m00;
    // set the momentum
    momentum = Point(cx,cy);
}
