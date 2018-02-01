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
     * @brief preprocessPreview calculates all contours necessary for preview. The image (src) is thresholded and the resultant binary
     * image is used to calculate the contours. All contours < minSizeThresh or > maxSizeThresh are removed.
     * @param src the input image
     * @param contoursDst the resultant contours
     * @param gThresh the gray value threshold
     * @param minSizeThresh the minimal size for contour size thresholding
     * @param maxSizeThresh the maximal size for contour size thresholding
     * @return all contours of sufficient size
     */
    static void preprocessPreview(cv::Mat const & src,
                                  contours_t & acceptedContoursDst,
                                  contours_t & biggerContoursDst,
                                  int const gThresh,
                                  int const minSizeThresh,
                                  int const maxSizeThresh);
    
    static void preprocessTracking(cv::Mat const & src,
                                   contours_t & acceptedContoursDst,
                                   contours_t & biggerContoursDst,
                                   int const gThresh,
                                   int const minSizeThresh,
                                   int const maxSizeThresh,
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
     * @brief roiRestriction checks the contours against the image borders and a user-selected region of interest
     *        and excludes every contour that does not lie entirely within image/ROI by checking its convex hull.
     *
     * @param contours input and output set of contours
     * @param img CV_8UC1 original image with the user-selected region of interest marked with non-zeros pixels
     * @param checkRoiBorders indicates if a ROI was selected by the user
     */
    static void borderRestriction(contours_t &contours, const cv::Mat &img, bool checkRoiBorders);
};

#endif // PREPROCESSOR_HPP
