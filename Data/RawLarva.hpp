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

#ifndef RAWLARVA_HPP
#define RAWLARVA_HPP

#include <vector>
#include <iterator>
#include <cmath>

#include <QTime>

#include "Configuration/FIMTrack.hpp"
#include "Configuration/TrackerConfig.hpp"
#include "Calculation/SpineIPAN.hpp"

using namespace FIMTypes;

/**
 * @brief The RawLarva class stores and calculates all necessary parameters from a given contour to describe a larval object.
 * The acutal (non-raw) larval object is later created from the larval object.
 */

class RawLarva
{
public:

    /**
     * @brief RawLarva constructor generates a raw larva object based on the contour.
     *
     * In this constructor, dMin is 6% of the contour size, dMax = dMin + 2 and maskSize = dMin
     * if dMin is odd, dMin+1 otherwise and distToMax = 0.25 (secondMaxCurvaturePoint must be
     * at least 25% of the contour size away from the maxCurvaturePoint).
     *
     * @param _contour the contour defining the RawLarva
     * @param img  the image to calculate the head position based on brightness information
     */
    RawLarva(contour_t const& _contour, cv::Mat const & img);

    /** GETTER METHODS **/

    contour_t getContour(void) const {return contour;}
    spineF_t getSpine(void) const {return spine;}
    spine_t getDiscreteSpine(void) const {return discreteSpine;}
    cv::Point getMidPoint(void) const {return discreteSpine.at((discreteSpine.size() + 1 ) / 2);}
    cv::Point getMomentum(void) const {return momentum;}
    double getArea(void) const {return area;}
    std::vector<double> getLarvalRadii(void) const {return larvalRadii;}
    double getSpineLength(void) const {return spineLength;}
    double getContourPerimeter(void) const {return arcLength(contour,true);}
    bool getIsBezierSpine(void) const{return bezierSpine;}
    bool getIsCoiledIndicator(void) const {return isCoiled;}

private:

    /**
     * @brief contour of the raw larva. The first point (with index 0) contains the contour point with highest curvature
     */
    contour_t contour;
    /**
     * @brief spine contains the central spine points with floating precision
     */
    spineF_t spine;
    /**
     * @brief discreteSpine contains fraction of the spine (e.g. 5 points to describe the larva), whereas all of these points
     *        have discrete coordinates.
     */
    spine_t discreteSpine;
    /**
     * @brief bezierSpine indicates if discreteSpine was calculated based on a bezier-spline fitted
     * through the spine (true) or if it was calculated in spineIPAN based on chord lenghts of the spine (false).
     */
    bool bezierSpine;
    /**
     * @brief momentum is the momentum of the contour
     */
    cv::Point momentum;

    /**
     * @brief area is the area of the contour
     */
    double area;
    /**
     * @brief spineLength arclength of the spine
     */
    double spineLength;
    /**
     * @brief larvalRadii stores the radii of the discrete spine points. First (head) and last (tail) radii are 0;
     *          radii inbetween are >0.
     */
    std::vector<double> larvalRadii;
    /**
     * @brief tailIndex index of contour point, that represents the tail
     *        Annotation: head index is always 0
     */
    uint tailIndex;

    bool isCoiled;

    /**
     * @brief calcMomentum calculates the momentum of the contour
     *
     *        This function sets the momentum of the larva
     */
    void calcMomentum(void);

    void calcIsCoiledIndicator(double const peri2spineLengthThresh,
                               double const midCirclePeri2PeriThresh,
                               double const max2meanRadiusThresh);

};

#endif // RAWLARVA_HPP
