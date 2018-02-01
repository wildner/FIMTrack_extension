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
#ifndef SPINEIPAN_HPP
#define SPINEIPAN_HPP

#include "Configuration/FIMTrack.hpp"
#include "Configuration/TrackerConfig.hpp"

/**
 * @brief The SpineIPAN class is responsible for calculation of a spine / discrete spine based on
 *        the so called IPAN algorithm which is used to determine curvature values of contour points.
 */
class SpineIPAN{

public:

    SpineIPAN();
    ~SpineIPAN();

    /**
     * @brief calcSpineParameters calculates the spine, discrete spine and corresponding radii of
     *        a given animal (represented by its contour) using the so called IPAN algorithm for
     *        curvature calculation. The procedure is done by means of the following steps:
     *
     *        1. Calculate the curvature for every contour point (see ipanFirstPass method)
     *        2. Calculate index of the point with maximum curvature (see calcMaxCurvatureIndex method)
     *        3. Calculate index of the point with second maximum curvature (see calcSecondMaxCurvatureIndexOrdered method)
     *        4. Calculate the spine (see calcSpine method)
     *        5. Calculate a discrete spine (see calcDiscreteSpine method)
     *
     *
     * @param _contour pointer to a VALID contour of an animal
     * @param _spine pointer to an EMPTY spine vector
     * @param _discreteSpine pointer to an EMPTY discrete spine vector
     * @param _animalRadii pointer to an EMPTY animal radii vector
     * @param _tailIndex reference to the index of contour point that represents the tail
     * @param _spineLength arc length of calculated spine
     * @param _dMin minimal length of the line between P- P and P P+.
     * @param _dMax maximal length of the line between P- P and P P+.
     * @param _maskSize must be an odd size for a mask (i.e. a sliding window) to calculate
     *        the mean curvature under the anchor point (mid point of the mask).
     * @param _distToMax relative distance to the maxCurvatureIndex (e.g. distanceToMax=0.25
     *        forces the secondMaxCurvatureIndex to be at least 0.25*contour.size() contour
     *        points away from the maxCurvatureIndex.
     * @param _nPoints an odd number for the number of discrete spine points
     */
    void calcSpineParameters(FIMTypes::contour_t* const _contour,
                             FIMTypes::spineF_t* const _spine,
                             FIMTypes::spine_t* const _discreteSpine,
                             FIMTypes::radii_t* const _animalRadii,
                             unsigned int &_tailIndex,
                             double &_spineLength,
                             unsigned int _dMin,
                             unsigned int _dMax,
                             unsigned int _maskSize,
                             double _distToMax,
                             unsigned int _nPoints);

private:

    // Member variables

    /**
     * @brief contour of the raw larva. The first point (with index 0) contains the contour point with highest curvature
     */
    FIMTypes::contour_t* contour;
    /**
     * @brief curvatures contains the curvature values for every contour point (in the same order).
     */
    FIMTypes::contourCurvature_t curvatures;
    /**
     * @brief spine contains the central spine points
     */
    FIMTypes::spineF_t* spine;
    /**
     * @brief discreteSpine contains fraction of the spine (e.g. 5 points to describe the larva)
     */
    FIMTypes::spine_t* discreteSpine;
    /**
     * @brief maxCurvatureIndex stores the index to the maximal curvature point (is equal to 0).
     */
    unsigned int maxCurvatureIndex;
    /**
     * @brief secondMaxCurvatureIndex stores the index to the second maximal curvature point
     */
    unsigned int secondMaxCurvatureIndex;
    /**
     * @brief firstHalf stores the points of the first half
     */
    FIMTypes::spine_t firstHalf;
    /**
     * @brief reverseSecondHalf stores the points of the second half
     */
    FIMTypes::spine_t reverseSecondHalf;
    /**
     * @brief firstDiscreteHalf stores the discrete points of the first half
     */
    FIMTypes::spine_t firstDiscreteHalf;
    /**
     * @brief reverseSecondDiscreteHalf stores the points of the second discrete half
     */
    FIMTypes::spine_t reverseSecondDiscreteHalf;
    /**
     * @brief animalRadii stores the radii of the discrete spine points. First (head) and last (tail) radii are 0;
     *          radii inbetween are >0.
     */
    FIMTypes::radii_t* animalRadii;

    cv::Mat distMapFirst;
    cv::Mat distMapSecond;
    /**
     * @brief absDiffDistanceMap image containing the absolute differences of every points distances to the
     *        two contour halfs.
     */
    cv::Mat absDiffDistMap;
    /**
     * @brief contourMask contains the filled contour (intensity = 1)
     */
    cv::Mat contourMask;
    /**
     * @brief offset specifies the offset of a contour point relative to the distance map coordinates
     */
    cv::Point offset;

    // Member functions

    /**
     * @brief Sets all non-pointer vector member variables to empty vectors
     */
    void clear();

    /**
     * @brief ipanFirstPass calculates the curvatures for every contour point based onthe
     *        IPAN algorithm (Chetverikov, D. (2003). A Simple and Efficient Algorithm for
     *        Detection of High Curvature Points in Planar Curves. In Computer Analysis of
     *        Images and Patterns (Vol. 2756, pp. 746–753)).
     *        For every contour point P, a trinangle is calculated with edges P- P and P P+
     *        with length between dMin and dMax (in pixel). The smallest angle defined
     *        by the triangle (P- P P+) in range [dMin, dMax] is assigned to the respective
     *        curvature vector position.
     *
     *        This function calculates the curvature vector.
     *
     * @param dMin minimal length of the line between P- P and P P+.
     * @param dMax maximal length of the line between P- P and P P+.
     */
    void ipanFirstPass(unsigned int dMin, unsigned int dMax);


    /**
     * @brief findMaxCurvaturePoint calculates the index of maximal curvature point on the contour.
     *        For every contour point, a mean curvature is calculated based on the odd
     *        maskSize: Given a maskSize of (m*2+1), m curvature values left and m
     *        curvature values right of a central point (anchor point) are summed up and
     *        devided by the maskSize.
     *
     *        After calculating the maxCurvatureIndex the contour and the curvature vector
     *        are reordered, to set the maxCurvatureIndex to 0 and thus the maxCurvaturePoint
     *        to the first element of the contour vector
     *
     * @param maskSize must be an odd size for a mask (i.e. a sliding window) to calculate
     *        the mean curvature under the anchor point (mid point of the mask).
     */
    void calcMaxCurvatureIndex(unsigned int maskSize);

    /**
     * @brief calcSecondMaxCurvatureIndexOrdered calculates the second maximal curvature
     *        index similar to the calcMaxCurvatureIndex function with a specified relative
     *        distance distanceToMax to the first contour point.
     *
     *        This function sets the value secondMaxCurvatureIndex
     *
     * @param maskSize must be an odd size for a mask (i.e. a sliding window) to calculate
     *        the mean curvature under the anchor point (mid point of the mask).
     * @param distToMax relative distance to the maxCurvatureIndex (e.g. distanceToMax=0.25
     *        forces the secondMaxCurvatureIndex to be at least 0.25*contour.size() contour
     *        points away from the maxCurvatureIndex.
     */
    void calcSecondMaxCurvatureIndexOrdered(unsigned int maskSize, double distToMax);


    /**
     * @brief calcSpine calculates the spine for this contour. This is done in 3 steps:
     *        1) contour is devided into firstHalf (from maxCurvaturePoint to
     *           secondMaxCurvaturePoint) and secondHalf (from secondMaxCurvaturePoint to
     *           end).
     *        2) longer contour half is reduced to a discrete set with same length than the
     *           shorter contour half. Thus, each point of the firstHalf corresponds to a
     *           specific point on the secondHalf.
     *        3) Midpoint between the corresponding contour half points is calculetes and stored
     *           in the spine vector
     *
     *        This function sets the spine vector.
     *        In addition, this funciton sets the discrete left and right contour half
     *        (i.e. firstDiscreteContourHalf and reverseSecondDiscreteContourHalf).
     *
     */
    void calcSpine();

    /**
     * @brief calcDiscreteSpine calculates a discrete spine containing nPoints points from
     *        the spine
     *
     *        This function sets the discreteSpine vector and the animalRadii vector
     *
     * @param nPoints an odd number for the number of discrete points
     * @param spineLength arc length of calculated spine
     */
    void calcDiscreteSpine(int nPoints, double &spineLength);

    // Helper for ipanFirstPass
    /**
     * @brief calcSuccessorPointWithDistance calculates the successor point on a
     *        contour for a given index curIndex with distance dist (in pixel)
     *
     * @param curIndex current index
     * @param dist distance to the curIndex position (in pixel)
     * @return returns the successor point to the contour point at curIndex with
     *         distance dist
     */
    cv::Point calcSuccessorPointWithDistance(unsigned int curIndex, unsigned int dist);

    // Helper for ipanFirstPass
    /**
     * @brief calcPredecessorPointWithDistancecalculates the predecessor point on a
     *        contour for a given index curIndex with distance dist (in pixel)
     *
     * @param curIndex current intex
     * @param dist distance to the curIndex position (in pixel)
     * @return returns the predecessor point to the contour point at curIndex with
     *         distance dist
     */
    cv::Point calcPredecessorPointWithDistance(unsigned int curIndex, unsigned int dist);

    // Helper for calcMaxCurvaturePoint
    /**
     * @brief reorderParameters is called in the calcMaxCurvaturePoint method.
     *        After calculating the maxCurvaturePoint, the contour and the curvature vector
     *        are reordered, to set the maxCurvatureIndex to 0 and thus the maxCurvaturePoint
     *        to the first element of the contour vector
     */
    void reorderParameters();

    // Helper for calcSpine()
    /**
     * @brief findMinAbsDiffPointBetween searches on the segment p1p2 for the point which is equally
     *        distant to both contour halfs. the search is approximated by testing n equidistant points
     *        on the segment (n is determined by the precision).
     *
     * @param p1 point of the first contour half
     * @param p2 point of the second contour half
     * @param precision determines the step width for searching task
     */
    cv::Point2f findMinAbsDiffPointBetween(cv::Point const &p1, cv::Point const &p2, double precision);

    /**
     * @brief calcDistanceMaps calculates the distance maps for checking the distance of points relative
     *        to the animal contour.
     *
     * @param gridSize specifies the grid size of distance maps (determined by precision of spine calculation)
     */
    void calcDistanceMaps(uint gridSize);
};

#endif // SPINEIPAN_HPP
