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

#ifndef PREPROCESSOR_HPP
#define PREPROCESSOR_HPP

#include <vector>
#include <iterator>
#include <list>

#include "Configuration/FIMTrack.hpp"
#include "Control/Backgroundsubtractor.hpp"

#include <QMessageBox>

/**
 * @brief The Preprocessor class is used to process images and calculate contours from images.
 */
class Preprocessor
{
public:
	/**
	* @brief modifier for the function findPath(), which only allows paths leading to a contour border
	*/
	static const int NO_DEAD_ENDS = -1;
	/**
	 * @brief preprocessPreview calculates all contours necessary for preview. The image (src) is thresholded and the resultant binary
	 * image is used to calculate the contours. All contours < minSizeThresh or > maxSizeThresh are removed.
	 * @param src the input image
	 * @param contoursDst the resultant contours
	 * @param gThresh the gray value threshold
	 * @param minSizeThresh the minimal size for contour size thresholding
	 * @param maxSizeThresh the maximal size for contour size thresholding
	 * @param valleyThresh pixels on the convolution image which are below or equal to this threshold are considered as black
	 * @return all contours of sufficient size
	 */
	static void preprocessPreview(cv::Mat const & src,
		contours_t & acceptedContoursDst,
		contours_t & biggerContoursDst,
		int const gThresh,
		int const minSizeThresh,
		int const maxSizeThresh,
		int const valleyThresh,
		Backgroundsubtractor const & bs);

	static void preprocessTracking(cv::Mat const & src,
		contours_t & acceptedContoursDst,
		contours_t & biggerContoursDst,
		int const gThresh,
		int const minSizeThresh,
		int const maxSizeThresh,
		int const valleyThresh,
		Backgroundsubtractor const & bs,
		bool checkRoiBorders);

private:
	Preprocessor();
	/**
	 * @brief graythresh calculates a binary image from src (8UC1) and stores it in dst (all pixel > thresh are set to 255)
	 * @param src the input image (8U grayscale)
	 * @param thresh the threhsold (every pixel > thresh is set to 255)
	 * @param dst the output binary image (in {0,255})
	 * @return the dst output binary image
	 */
	static void graythresh(cv::Mat const & src,
		int const thresh,
		cv::Mat & dst);

	/**
	 * @brief calcContours calculates and returns the contours in an image
	 * @param src input (binary 8UC1) image
	 * @param contours container with several contours
	 * @return the calculated contours
	 */
	static void calcContours(cv::Mat const & src, contours_t & contours);

	/**
	 * @brief sizethreshold removes all contours < minSizeThresh and > maxSizeThresh from the given contoursSrc
	 * and stores the results in contoursDst.
	 * @param contoursSrc input set of contours
	 * @param minSizeThresh minimal contour size
	 * @param maxSizeThresh maximal contour size
	 * @param contoursDst resultant reduced set of contours
	 * @return reduced set of contours
	 */
	static void sizethreshold(contours_t const & contoursSrc,
		int const minSizeThresh,
		int const maxSizeThresh,
		contours_t & correctContoursDst,
		contours_t & biggerContoursDst);

	/**
	* @brief filterByFeatures removes all contours which cannot be a larva/fish and filters the remaining contours depending on if they could possibly consist of multiple larvae/fish
	* and stores the results in contoursDst.
	* @param contoursSrc input set of contours
	* @param minSizeThresh minimal contour size
	* @param maxSizeThresh maximal contour size
	* @param contoursDst resultant reduced set of contours
	* @return reduced set of contours
	*/
	static void filterByFeatures(cv::Mat const & img,
		contours_t const & contoursSrc,
		int const minSizeThresh,
		int const maxSizeThresh,
		contours_t & correctContoursDst,
		contours_t & biggerContoursDst);

	/**
	 * @brief roiRestriction checks the contours against the image borders and a user-selected region of interest
	 *        and excludes every contour that does not lie entirely within image/ROI by checking its convex hull.
	 *
	 * @param contours input and output set of contours
	 * @param img CV_8UC1 original image with the user-selected region of interest marked with non-zeros pixels
	 * @param checkRoiBorders indicates if a ROI was selected by the user
	 */
	static void borderRestriction(contours_t &contours, const cv::Mat &img, bool checkRoiBorders);

	/**
	* @brief splitBiggerContours tries to split contours which are bigger than one larva/fish.
	* TODO: add parameters to user preferences?
	*
	* ######### OPTIONAL #############
	* splitBiggerContours is based on the assumption that the observed objects
	* do not overlap and that collided contours can always be separated.
	* Since this is not true for fish, this function is probably useless.
	*/
	static void splitBiggerContours(cv::Mat const & src, contours_t & splittedContoursDst, contours_t & biggerContoursDst, int const meanLarvaSize);

	/**
	* @brief performContourCut cuts one contour into two parts. The exact cut is determined by a given vector of points. The start and end of the cut are given by two contour indices.
	*/
	static void performContourCut(contour_t & srcContour, contour_t & dstContour1, contour_t & dstContour2, int idxStart, int idxEnd, std::vector<cv::Point> cutPoints);

	/**
	* @brief performSimpleContourCut uses a straight line to cut one contour into two parts. The start and end of the line are given by two contour indices.
	*/
	static void performSimpleContourCut(cv::Mat const & image, contour_t & srcContour, contour_t & dstContour1, contour_t & dstContour2, int idxStart, int idxEnd);

	/**
	* @brief enhanceValleys tries to find and enhance the valleys (area between different objects) inside of given contours
	*/
	static void enhanceValleys(cv::Mat const & srcImg, cv::Mat const & threshImg, contours_t & acceptedContoursDst, contours_t & biggerContoursDst, int const minLarvaSize, int const maxLarvaSize, const int valleyThresh);

	/**
	* @brief findPath tries to find a path between contours following the darkest pixels of the convolution image
	* @param minDeadEndLength indicates how long a dead end must be to be returned. Value "-1" means, dead ends are not returned
	*/
	static path_t findPath(cv::Mat const & convolutionImg, cv::Mat const & lookupImg, cv::Point & startPoint, cv::Vec2d & startVec, const int & valleyThresh, const int & minDeadEndLength);

	/**
	* @brief createStraightPath creates a straight path between two points. Every point in between has two orthogonal neighbours.
	*/
	static path_t createStraightPath(cv::Mat const & convolutionImg, cv::Mat const & lookupImg, cv::Point & startPoint, cv::Point & endPoint);

	/**
	* @brief checkIfConnectable checks if two points are close enough and their vectors are pointing to each other
	*/
	static bool checkIfConnectable(cv::Point & point1, cv::Point & point2, cv::Vec2d & curvVec1, cv::Vec2d & curvVec2, const double & defaultMinimaConnectorDistance, const double & connectionEnhancerAngle);

	/**
	* @brief countNeighboursUsingThresh counts all pixels inside the sqared neighbourhood of a pixel which are below or above a given threshold
	*/
	static int countNeighboursUsingThresh(cv::Mat const & img, cv::Point2i point, int const squareLength, int const threshold, bool useBelowThresh);

	/**
	* @brief eliminateAllInnerContourCrossings splits all contours where a contour crosses itself and would therefore lead to wrong curvature calculations
	*/
	static void eliminateAllInnerContourCrossings(contours_t & srcContours, contours_t & validatedContoursDst);

	/**
	* @brief eliminateInnerContourCrossings splits a contour if the contour crosses itself and would therefore lead to wrong curvature calculations
	*/
	static void eliminateInnerContourCrossings(contour_t & srcContour, contours_t & validatedContoursDst);

};

#endif // PREPROCESSOR_HPP
