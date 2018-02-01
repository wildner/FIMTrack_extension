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

#ifndef CALC_HPP
#define CALC_HPP

#include "Configuration/FIMTrack.hpp"
#include <vector>
#include <iterator>
#include <cmath>

#include <QLineF>
#include <QRegion>
#include <QPolygonF>
#include <qmath.h>

namespace Calc
{

    /**
    * @brief normL2 calculates the L2-norm for a 2D point pt
    * @param pt is a 2D point for L2-norm calculation
    * @return the resultant L2-norm for pt
    */
    template<class T>
    T normL2(cv::Point_<T> const& pt);
    qreal normL2(QPointF const& p);

    /**
     * @brief scale rescales the vector pt to newLength
     * @param pt the vector to rescale
     * @param newLength the new length
     * @return rescaled vector
     */
    template<class T>
    cv::Point_<T> scale(cv::Point_<T> const& pt, T newLength);

    /**
    * @brief eucledianDist calculates the eucledean distance between two 2D points
    * @param pt1 first 2D point for eucledian distance calculation
    * @param pt2 second 2D point for eucledian distance calculation
    * @return the eucledian distance between pt1 and pt2
    */
    double eucledianDist(cv::Point const& pt1, cv::Point const& pt2);

    /**
    * @brief eucledianDist calculates the eucledean distance between two 2D points
    * @param pt1 first 2D point for eucledian distance calculation
    * @param pt2 second 2D point for eucledian distance calculation
    * @return the eucledian distance between pt1 and pt2
    */
    double eucledianDist(cv::Point const& pt1, QPointF const& pt2);
    double eucledianDist(QPointF const& pt1, QPointF const& pt2);
    double eucledianDist(QPointF const& p, QLineF const& l);
    double eucledianDist(QPointF const& p, QRectF const& r, const bool ellipse);

    /**
     * @see http://math.stackexchange.com/a/76463
     * @brief isPointInEllipse
     * @param p
     * @param r
     * @return
     */
    bool isPointInEllipse(QPointF const& p, QRectF const& r);

    /**
    * @brief calcAngle calculates the angle between two lines given by the
    *        2D points (p1 p) and (p p2). Result is in degree.
    * @param p central 2D point in the triangle
    * @param p1 point for the first line (p1 p)
    * @param p2 point for the second line (p p2)
    * @return anlge (in degree) given by the triangle between the 2D lines
    *         defined by the three points
    */
    double calcAngle(cv::Point const& p, cv::Point const& p1, cv::Point const& p2);

    double calcAngle(cv::Point2f const& v1, cv::Point2f const& v2);

    double calcSmallestAngle(cv::Point const& p, cv::Point const& p1, cv::Point const& p2);

    /**
    * @brief calcAngleToYAxes calculates the angle between the points p1 and p2 measured to the y-axes
    * @param p1 first point
    * @param p2 second point
    * @return the angle (in degree) between the to points measured to the y-axes
    */
    double calcAngleToYAxes(cv::Point const& p1, cv::Point const& p2);

    /**
    * @brief calcCircularAngleSum adds an offset to an angle
    * @param angle the angle
    * @param offset the offset to be added
    * @return sum of the angle and the offest (angle + offset)
    */
    double calcCircularAngleSum(double const angle, double const offset);

    /**
    * @brief calcAngleDiff calculates the difference between two angles
    * @param angle1 first angle
    * @param angle2 second angle
    * @return resultant difference between the two given angles
    */
    double calcAngleDiff(double const angle1, double const angle2);

    double angleToRadian(double const angle);

    /**
    * @brief calcPoligonArea
    * @see http://mathworld.wolfram.com/PolygonArea.html
    * @param poligon
    * @return
    */
    double calcPolygonArea(QPolygonF const& polygon);

    /**
     * @brief calcPolygonCenterOfMass
     * @see http://en.wikipedia.org/wiki/Centroid#Centroid_of_polygon
     * @param poligon
     * @return
     */
    cv::Point calcPolygonCenterOfMass(QPolygonF const& polygon);

    /**
     * @brief calcSpineLength
     * @param spine
     * @return
     */
    double calcSpineLength(std::vector<cv::Point> const& spine);

    /**
     * @brief calcPerimeter
     * @param polygon
     * @return
     */
    double calcPerimeter(QPolygonF const& polygon);

    /**
     * @brief calcDotProduct
     * @param p1
     * @param p2
     * @return
     */
    double calcDotProduct(QPointF const& p1, QPointF const& p2);

    /**
     * @brief calcCrossProduct
     * @param p1
     * @param p2
     * @return
     */
    double calcCrossProduct(QPointF const& p1, QPointF const& p2);

    /**
     * @brief calcCircularContourNeighbourIndex calculates the index of a circular contour or
     *        curvature vector (if (0 <= (pos + offset) < contourSize) return: (pos + offset);
     *        else: return the "circular index" in [0 contourSize) ).
     * @param contourSize specifies the size of the contour
     * @param pos specifies the current position
     * @param offset specifies the offest to the current position (i.e. pos + offset)
     * @return the circular index defined by (pos + offset)
     */
    unsigned int calcCircularContourNeighbourIndex(const size_t& contourSize, const unsigned int& pos, const int& offset);

    /**
     * @brief leftTurn finds out if the sequence of the three points p1, p2, p3 (traversed in this very order) indicates
     *        a left turn.
     * @param p1 starting point
     * @param p2 intermediate point
     * @param p3 endpoint
     */
    bool leftTurn(cv::Point const& p1, cv::Point const& p2, cv::Point const& p3);

    /**
     * @brief minSquaredEuclideanDistanceTo calculates the minimal squared euclidean distance of a point
     * to an open or closed contour by checking every point on the contour.
     *
     * @param contour the contour to check (open or closed)
     * @param pt the point to check
     * @return the minimal squared euclidean distance of the point (pt) to the contour
     */
    float minSquaredEuclideanDistanceTo(const FIMTypes::contour_t& contour, const cv::Point2f& pt);

    /**
     * @brief calcOverlap calculates the overlap (in pixels) of the INNER of two given contours
     *
     * @param firstContour
     * @param secondContour
     */
    uint calcOverlap(const FIMTypes::contour_t& firstContour, const FIMTypes::contour_t& secondContour);

    /**
     * @brief getClosestPointOnLinesegment
     * @see http://paulbourke.net/geometry/pointlineplane/
     * @param p Point to which we want to know the closes point of the segment defined by l=(p1,p2)
     * @param l segment
     * @return closest point on segment l for point p
     */
    QPointF getClosestPointOnLinesegment(QPointF const& p, QLineF const& l);
    double calcInnerAngleOfVectors(QPointF const& v1, QPointF const& v2);
}

#endif // CALC_HPP
