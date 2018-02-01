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
#include "SpineIPAN.hpp"

#include <cmath>

#include "Algorithm/Hungarian.hpp"

using namespace FIMTypes;
using namespace cv;
using namespace std;

SpineIPAN::SpineIPAN()
{
}

SpineIPAN::~SpineIPAN()
{
    this->clear();
}

void SpineIPAN::calcSpineParameters(contour_t* const _contour,
                                    spineF_t* const _spine,
                                    spine_t* const _discreteSpine,
                                    radii_t* const _animalRadii,
                                    unsigned int &_tailIndex,
                                    double &_spineLength,
                                    unsigned int _dMin,
                                    unsigned int _dMax,
                                    unsigned int _maskSize,
                                    double _distToMax,
                                    unsigned int _nPoints){

    this->clear();

    contour = _contour;
    spine = _spine;
    discreteSpine = _discreteSpine;
    animalRadii = _animalRadii;

    // calculate the curvature vector
    ipanFirstPass(_dMin, _dMax);

    // calculate index of point with max curvature
    calcMaxCurvatureIndex(_maskSize);

    // reorder members 'contour' and 'curvatures' that first point is the one with maximum curvature
    reorderParameters();

    // calculate index with second maximum curvature
    calcSecondMaxCurvatureIndexOrdered(_maskSize, _distToMax);

    // calculate all spine points
    calcSpine();

    // calculate nPoints on the spine for the discrete spine
    calcDiscreteSpine(_nPoints, _spineLength);

    _tailIndex = secondMaxCurvatureIndex;
}

void SpineIPAN::clear()
{
    this->curvatures.clear();
    this->firstHalf.clear();
    this->reverseSecondHalf.clear();
    this->firstDiscreteHalf.clear();
    this->reverseSecondDiscreteHalf.clear();

    distMapFirst.release();
    distMapSecond.release();
    absDiffDistMap.release();
    contourMask.release();
}

void SpineIPAN::ipanFirstPass(unsigned int dMin, unsigned int dMax){

    assert(contour != nullptr);

    // get the contour size
    size_t contourSize = contour->size();

    // for all points in the contour
    for (int i=0; i < (int) contourSize; ++i)
    {
        // get the current point
        Point curPoint = contour->at(i);
        // initialize the angle for this point with maximum value (i.e. 360 degree)
        double angle = 360;
        // for all points in the contour with distance in [dMin, dMax]
        for (unsigned int dist = dMin; dist <= dMax; ++dist)
        {
            // get the successor point
            Point successor = calcSuccessorPointWithDistance(i, dist);
            // get the predecessor point
            Point predecessor = calcPredecessorPointWithDistance(i, dist);
            // calculate the current angle value given by the triangle (predecessor, curPoint, successor)
            double curAngle = Calc::calcAngle(curPoint, predecessor, successor);
            // is the current angle smaller?
            if (curAngle < angle)
            {
                // update the angle
                angle = curAngle;
            }
        }  // end for-loop for points within distance in [dMin, dMax]

        // push the smallest found angle into the curvatures vector
        curvatures.push_back(angle);
    }

}

void SpineIPAN::calcMaxCurvatureIndex(unsigned int maskSize){

    assert(contour!=0);

    // guarantee odd mask size
    maskSize = (maskSize % 2 == 0) ? maskSize+1 : maskSize;

    // get the amount of points left / right from the mask center
    int moveLeftRight = (int) maskSize / 2;

    // initialize the meanBestCurvature with maximal curvature value (i.e. 360 degree)
    double meanBestCurv = 360;
    // initialize the bestIndex (i.e. index with sharpest angle for the given mask)
    int bestIndex = 0;

    // for all contour points
    for(int contourPointNo = 0; contourPointNo < (int) contour->size(); ++contourPointNo)
    {
        // set the current mean curvature to 0
        double curMeanCurvature = 0;
        // for all points within the mask (i.e. [-moveLeftRight, moveLeftRight])
        for (int offset = -moveLeftRight; offset <= moveLeftRight; ++offset)
        {
            // get the (circular) index
            int curIndex = Calc::calcCircularContourNeighbourIndex(contour->size(), contourPointNo, offset);
            // add the current curvature to the current mean curMeanCurvature
            curMeanCurvature += curvatures.at(curIndex);
        }

        // normalize the curMeanCurvature to get the real mean
        curMeanCurvature /= maskSize;

        // is the current curvature smaller than the best mean curvature?
        if (curMeanCurvature < meanBestCurv)
        {
            // update the best mean curvature
            meanBestCurv = curMeanCurvature;
            // update the bestIndex
            bestIndex = contourPointNo;
        }
    }

    // store the best index (with sharpest angle within the mask)
    maxCurvatureIndex = bestIndex;

    reorderParameters();
}

void SpineIPAN::calcSecondMaxCurvatureIndexOrdered(unsigned int maskSize, double distToMax){

    assert(contour!=nullptr);

    // guarantee odd mask size
    maskSize = (maskSize % 2 == 0) ? maskSize+1 : maskSize;

    // get the distance offset ot the maxCurvaturePoint
    int distOffset = contour->size() * distToMax;

    // get the amoutn of points left / right from the mask center
    int moveLeftRight = (int) maskSize / 2;

    // initialize second best mean curvature
    double meanSecondBestCurv = 360;
    // set second best index to the opposite of the (first) maximum curvature index (wich is 0)
    int secondBestIndex = contour->size() / 2;

    // for all contour points in [distOffset, contourSize-distOffset] (to guarantee a distance to the
    // maximum curvature index)
    for(int contourPointNo = distOffset; contourPointNo <= (int) contour->size() - distOffset; ++contourPointNo)
    {
        // initialize the current mean curvature
        double meanCurvature = 0;
        // for all contour points within the mask
        for (int offset = -moveLeftRight; offset <= moveLeftRight; ++offset)
        {
            // calculate the current index
            int curIndex = Calc::calcCircularContourNeighbourIndex(contour->size(), contourPointNo, offset);
            // sum up the curvature values (within the mask)
            meanCurvature += curvatures.at(curIndex);
        }

        // devide the summed curvature values to get the mean
        meanCurvature /= maskSize;

        // is the current mean curvature smaller than the second best mean curvature?
        if (meanCurvature < meanSecondBestCurv)
        {
            // update mean second best curvature and its index
            meanSecondBestCurv = meanCurvature;
            secondBestIndex = contourPointNo;
        }
    }

    // set the second best index (with second sharpest curvature with distance to the first)
    secondMaxCurvatureIndex = secondBestIndex;
}

void SpineIPAN::calcSpine(){

    assert(contour!=nullptr);
    assert(spine!=nullptr);

    double precision = 0.2; // PARAMS

    try{

        // reserve space for the first and the second contour half
        firstHalf.reserve(secondMaxCurvatureIndex);
        reverseSecondHalf.reserve(contour->size() - secondMaxCurvatureIndex);

        // push all contour points of the first half into the vector firstHalf (in [1,secondMaxCurvatureIndex) )
        for (unsigned int i = 1; i<secondMaxCurvatureIndex; ++i)
        {
            firstHalf.push_back(contour->at(i));
        }
        // push all contour points fo the second half into the vector
        // secondHalf (in [secondMaxCurvatureIndex+1, contourSize) )
        // and flip the order
        for (unsigned int i = contour->size() - 1; i > secondMaxCurvatureIndex; --i)
        {
            reverseSecondHalf.push_back(contour->at(i));
        }
        // the the size of the smaller half
        int discreteSize = std::min(firstHalf.size(), reverseSecondHalf.size());


        // reserve memory for both discrete half vectors
        firstDiscreteHalf.reserve(discreteSize);
        reverseSecondDiscreteHalf.reserve(discreteSize);

        // is the first half bigger than the second half? (first half will be reduced...)
        if(firstHalf.size() > reverseSecondHalf.size())
        {
            // calculate the fraction (>1) to jump to the positions in the longer first half
            double dOffset = (double) firstHalf.size() / (double) reverseSecondHalf.size();
            // initialize the double index
            double dIndex = 0.0;

            // initialize the integer indices for the first and second contour half
            unsigned int indexFirst = 0;
            unsigned int indexSecond = 0;

            // while there are still points in the second (shorter) contour half and
            // there are still points in the first (longer) contour half
            while(indexSecond < reverseSecondHalf.size() && indexFirst < firstHalf.size())
            {
                // add the points from the first and second half to the discrete vectors
                firstDiscreteHalf.push_back(firstHalf.at(indexFirst));
                reverseSecondDiscreteHalf.push_back(reverseSecondHalf.at(indexSecond));

                // incement the double index by the double offset (>1)
                dIndex += dOffset;
                // round the double index to get the first half index
                indexFirst = cvRound(dIndex);
                // increment the second half index
                ++indexSecond;
            }
        }

        // is the second contour half bigger than the first contour half?
        else if(reverseSecondHalf.size() > firstHalf.size())
        {
            // same steps as in the if-case above...
            double dOffset = (double) reverseSecondHalf.size() / (double) firstHalf.size();
            double dIndex = 0.0;

            unsigned int indexFirst = 0;
            unsigned int indexSecond = 0;

            while(indexFirst < firstHalf.size() && indexSecond < reverseSecondHalf.size())
            {

                firstDiscreteHalf.push_back(firstHalf.at(indexFirst));
                reverseSecondDiscreteHalf.push_back(reverseSecondHalf.at(indexSecond));

                dIndex += dOffset;
                ++indexFirst;
                indexSecond = cvRound(dIndex);
            }

        }
        // both contour halfs are equally sized?
        else
        {
            // discrete contour halves are the same to the non-discrete halves
            firstDiscreteHalf = firstHalf;
            reverseSecondDiscreteHalf = reverseSecondHalf;
        }

        // calculate distance maps, necessary for spine calculation
        this->calcDistanceMaps((uint)(1/precision));

        std::list<cv::Point2f> tempSpineList;
        cv::Point2f spinePoint = contour->at(maxCurvatureIndex);
        tempSpineList.push_back(spinePoint);
        cv::Point2f lastSpinePoint1 = spinePoint;
        cv::Point2f lastSpinePoint2 = spinePoint;
        cv::Point lastFirstHalfPoint = firstDiscreteHalf.at(0);
        cv::Point lastSecondHalfPoint = reverseSecondDiscreteHalf.at(0);
        cv::Point2f tailPoint = contour->at(secondMaxCurvatureIndex);
        // for all discrete contour half points
        for(unsigned int i = 1; i<firstDiscreteHalf.size(); ++i)
        {
            cv::Point curFirstHalfPoint = firstDiscreteHalf.at(i);
            cv::Point curSecondHalfPoint = reverseSecondDiscreteHalf.at(i);

            // Test if new line is located "right" compared to the last line
            if(!((!Calc::leftTurn(curFirstHalfPoint,curSecondHalfPoint,lastFirstHalfPoint)
                  && !Calc::leftTurn(curFirstHalfPoint,curSecondHalfPoint,lastSecondHalfPoint))
                 ||
                 (Calc::leftTurn(lastFirstHalfPoint,lastSecondHalfPoint,curFirstHalfPoint)
                  && Calc::leftTurn(lastFirstHalfPoint,lastSecondHalfPoint,curSecondHalfPoint))))
            {
                continue;
            }

            // find point with maximum distance to contour
            spinePoint = findMinAbsDiffPointBetween(curFirstHalfPoint,curSecondHalfPoint,precision);
            // check, if no spine point lying within the contour could have been found
            if(spinePoint.x == -1.0f){
                continue;
            }

            // skip duplicates, its important to check both last points
            if(spinePoint == lastSpinePoint2 || spinePoint == lastSpinePoint1)
            {
                continue;
            }

            tempSpineList.push_back(spinePoint);

            // check, if already added "tail"
            if(spinePoint == tailPoint)
            {
                break;
            }

            // refresh last points
            lastSpinePoint1 = lastSpinePoint2;
            lastSpinePoint2 = spinePoint;

            // refresh last points
            lastFirstHalfPoint = curFirstHalfPoint;
            lastSecondHalfPoint = curSecondHalfPoint;

        }
        // finally, add tail if not added yet
        if(tempSpineList.back() != tailPoint){
            tempSpineList.push_back(tailPoint);
        }

        // reserve memory for the spine and insert all spine points
        spine->reserve(tempSpineList.size());
        spine->insert(spine->end(), tempSpineList.begin(), tempSpineList.end());

    }catch(std::exception e){
        std::cout << e.what() << " - " << "exception during spine calculation in SpineIPAN.cpp" << std::endl;
    }

}

void SpineIPAN::calcDiscreteSpine(int nPoints, double &spineLength){

    assert(contour!=nullptr);
    assert(spine!=nullptr);
    assert(discreteSpine!=nullptr);
    assert(animalRadii!=nullptr);

    try{

        // guarantee odd number of points
        nPoints = (nPoints % 2 == 0) ? nPoints+1 : nPoints;

        // reserve memory for the discrete spine
        discreteSpine->reserve(nPoints);

        // reserve memory for the radii vector
        animalRadii->reserve(nPoints);
        // add the first radius (which is 0)
        animalRadii->push_back(0.0);

        // add the first point to the discrete spine
        discreteSpine->push_back(contour->at(maxCurvatureIndex));

        // first, calculate total length of spine and store lengths of segments
        spineLength = 0.0;
        vector<double> segLengths;
        segLengths.reserve(spine->size()-1);
        for(uint i = 1; i < spine->size(); i++)
        {
            double segLength = Calc::normL2(spine->at(i)-spine->at(i-1));
            spineLength += segLength;
            segLengths.push_back(segLength);
        }

        // distance between two points on discrete spine
        double dist = spineLength / (nPoints-1);
        // helper variables for loop
        double cumLength = 0.0;
        double neededLength = dist; // length of spine until next discrete spine point
        int pointsAdded = 0; // count points added in for loop

        for(uint i = 1; i < spine->size() && pointsAdded < nPoints-2;)
        {
            // check if next discrete point lies between (i-1). and (i). spine point
            if(cumLength + segLengths.at(i-1) <= neededLength)
            {
                cumLength += segLengths.at(i-1);
                i++;
                continue;
            }
            // cumulated length up to i-th point sufficient, so add the point
            // lying at cumLength + fraction on the spine (relative to first spine point)
            double neededLengthOnCurSegment = neededLength - cumLength;
            double fraction = neededLengthOnCurSegment / segLengths.at(i-1);

            Point2f dirVector = spine->at(i) - spine->at(i-1);
            Point2f discreteSpinePoint = spine->at(i) + fraction * dirVector;

            discreteSpine->push_back(discreteSpinePoint);

            double radius = cv::pointPolygonTest(*contour,discreteSpinePoint,true);
            if(radius < 0.0){
                radius = 0.0;
            }
            animalRadii->push_back(radius);

            // refresh needed length and points added
            neededLength += dist;
            pointsAdded++;
        }

        // add the last point to the discrete spine
        discreteSpine->push_back(contour->at(secondMaxCurvatureIndex));

        // add the last radius (which is 0.0)
        animalRadii->push_back(0.0);

    }catch(std::exception e){
        std::cout << e.what() << " - " << "exception during discrete spine calculation in SpineIPAN.cpp" << std::endl;
    }
}

Point SpineIPAN::calcSuccessorPointWithDistance(unsigned int curIndex, unsigned int dist)
{
    size_t contourSize = contour->size();
    Point curPoint = contour->at(curIndex);

    int offset = 1;
    int nextIndex = Calc::calcCircularContourNeighbourIndex(contourSize, curIndex, offset);
    Point nextPoint = contour->at(nextIndex);

    double curDist = Calc::eucledianDist(curPoint, nextPoint);

    while(curDist < (double) dist && offset < (int) contourSize)
    {
        ++offset;
        nextIndex = Calc::calcCircularContourNeighbourIndex(contourSize, curIndex, offset);
        nextPoint = contour->at(nextIndex);
        curDist = Calc::eucledianDist(curPoint, nextPoint);
    }

    return nextPoint;
}

Point SpineIPAN::calcPredecessorPointWithDistance(unsigned int curIndex, unsigned int dist)
{
    size_t contourSize = contour->size();
    Point curPoint = contour->at(curIndex);

    int offset = -1;
    int nextIndex = Calc::calcCircularContourNeighbourIndex(contourSize, curIndex, offset);
    Point nextPoint = contour->at(nextIndex);

    double curDist = Calc::eucledianDist(curPoint, nextPoint);

    while(curDist < (double) dist && abs(offset) < (int) contourSize)
    {
        --offset;
        nextIndex = Calc::calcCircularContourNeighbourIndex(contourSize, curIndex, offset);
        nextPoint = contour->at(nextIndex);
        curDist = Calc::eucledianDist(curPoint, nextPoint);
    }

    return nextPoint;
}

void SpineIPAN::reorderParameters()
{
    assert(contour!=nullptr);

    // get the contour size
    size_t contourSize = contour->size();

    // define parameters for the ordered contour and ordered curvatures vectors
    contour_t orderedContour;
    orderedContour.reserve(contourSize);

    contourCurvature_t orderedCurvatures;
    orderedCurvatures.reserve(contourSize);

    // for all elements inside the contour and curvatures vector from maxCurvatureIndex to contourSize
    for (unsigned int i = maxCurvatureIndex; i < contourSize; ++i)
    {
        // add the respective elements into the new ordered vectors
        orderedContour.push_back(contour->at(i));
        orderedCurvatures.push_back(curvatures.at(i));
    }
    // add the remaining elements from the contour and curvatures vector to the ordered vectors
    for (unsigned int i = 0; i < maxCurvatureIndex; ++i)
    {
        orderedContour.push_back(contour->at(i));
        orderedCurvatures.push_back(curvatures.at(i));
    }

    // set the contour and curvatures vector to the ordered vectors
    *contour = orderedContour;
    curvatures = orderedCurvatures;

    // now the index of maximum curvature point is equal to zero
    maxCurvatureIndex = 0;
}

cv::Point2f SpineIPAN::findMinAbsDiffPointBetween(Point const &p1, Point const &p2, double precision)
{
    // if points are equal, return p1
    if(p1 == p2)
    {
        return p1;
    }

    // distance between the two contour points
    double p1p2Dist = Calc::eucledianDist(p1,p2);

    // direction vector from p1 to p2
    cv::Point2f dirVector = p2 - p1;
    // initialization
    float x = -1.0f;
    float y = -1.0f;
    cv::Point2f minAbsDiffPoint(x,y);
    double minAbsDiff = std::numeric_limits<double>::max();

    dirVector = Calc::scale<float>(dirVector, 1.0);

    int xDiscreteOld = -1; // x-coordinate of upper-left discrete point (from last iteration)
    int yDiscreteOld = -1; // corresponding y-coordinate
    // add a fraction of the dirVector to p1 and test the resulting point
    // step width (here 0.1) determines the precision
    for(double i = precision; i < p1p2Dist; i = i + precision)
    {
        x = p1.x + i * dirVector.x;
        y = p1.y + i * dirVector.y;
        cv::Point2f curPoint(x,y);

        // corresponding discrete x/y-coordinates of upper-left point (from this iteration)
        int xDiscreteNew = (int)x;
        int yDiscreteNew = (int)y;

        // discrete x- or y-coordinate changed
        if(xDiscreteNew != xDiscreteOld || yDiscreteNew != yDiscreteOld)
        {
            // refresh old discrete coordinates
            yDiscreteOld = yDiscreteNew;
            xDiscreteOld = xDiscreteNew;
            // first check, if all 4 discrete neighbours lie within contour
            if(contourMask.at<uchar>(yDiscreteNew-offset.y,xDiscreteNew-offset.x) == 0
                    || contourMask.at<uchar>(yDiscreteNew-offset.y,xDiscreteNew-offset.x+1) == 0
                    || contourMask.at<uchar>(yDiscreteNew-offset.y+1,xDiscreteNew-offset.x) == 0
                    || contourMask.at<uchar>(yDiscreteNew-offset.y+1,xDiscreteNew-offset.x+1) == 0)
            {
                continue;
            }
        }

        // coordinates in the finer discretized distance maps (determined by precision)
        int xN = cvRound((x-offset.x)*(1/precision));
        int yN = cvRound((y-offset.y)*(1/precision));

        float absDiff = absDiffDistMap.at<float>(yN,xN);

        if(absDiff < minAbsDiff)
        {
            minAbsDiff = absDiff;
            minAbsDiffPoint = curPoint;
        }

    }

    return minAbsDiffPoint;
}

void SpineIPAN::calcDistanceMaps(uint gridSize)
{
    /********************************
    Calculate distance maps, needed
    for calculation of spine points
    *********************************/

    cv::Rect rect = cv::boundingRect(*contour);
    // enlarge bounding rect
    rect.x = rect.x - 1;
    rect.y = rect.y - 1;
    rect.width = rect.width + 2;
    rect.height = rect.height + 2;
    offset = cv::Point(rect.x,rect.y);

    // draw filled contour into an image to have a mask
    contourMask = cv::Mat::zeros(rect.size() , CV_8UC1);
    std::vector<std::vector<cv::Point> > contourContainer;
    contourContainer.push_back(*contour);
    cv::drawContours(contourMask, contourContainer, -1, cv::Scalar(1), CV_FILLED, 8,
                     cv::noArray(), 2147483647, cv::Point(-offset.x,-offset.y));
    // CV_FILLED also draws the contour border. not desired here.
    cv::drawContours(contourMask, contourContainer, -1, cv::Scalar(0), 1, 8,
                     cv::noArray(), 2147483647, cv::Point(-offset.x,-offset.y));

    // images, each containing one of the contour halfs
    cv::Mat imgFirstHalf = cv::Mat::ones(rect.height*gridSize, rect.width*gridSize, CV_8UC1);
    cv::Mat imgSecondHalf = cv::Mat::ones(rect.height*gridSize, rect.width*gridSize, CV_8UC1);

    cv::Rect tempRect;
    tempRect.height = gridSize;
    tempRect.width = gridSize;
    int value = 0;
    // draw halfs into the constructed images (as zero pixels)
    for(uint i = 0; i < firstHalf.size(); i++)
    {
        cv::Point curPoint = firstHalf.at(i);
        tempRect.x = gridSize*(curPoint.x - offset.x);
        tempRect.y = gridSize*(curPoint.y - offset.y);
        imgFirstHalf(tempRect) = value;
    }
    for(uint i = 0; i < reverseSecondHalf.size(); i++)
    {
        cv::Point curPoint = reverseSecondHalf.at(i);
        tempRect.x = gridSize*(curPoint.x - offset.x);
        tempRect.y = gridSize*(curPoint.y - offset.y);
        imgSecondHalf(tempRect) = value;
    }

    // calculate respective distance maps
    cv::distanceTransform(imgFirstHalf, distMapFirst, CV_DIST_L2, 3);
    cv::distanceTransform(imgSecondHalf, distMapSecond, CV_DIST_L2, 3);

    // calculate the absolute differences of all distance values in the two images
    cv::absdiff(distMapSecond, distMapFirst, absDiffDistMap);

    imgSecondHalf.release();
    imgFirstHalf.release();
    contourContainer.clear();
}
