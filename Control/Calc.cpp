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

#include "Calc.hpp"

using namespace cv;
using namespace FIMTypes;
using std::vector;


namespace Calc
{

    /// only for avoidance of linking error
    template float normL2(Point_<float> const& pt);
    template double normL2(Point_<double> const& pt);
    template int normL2(Point_<int> const& pt);

    template<class T>
    T normL2(Point_<T> const& pt)
    {
        return qSqrt(pt.x * pt.x + pt.y * pt.y);
    }

    qreal normL2(const QPointF& p)
    {
        return qSqrt(p.x() * p.x() + p.y() * p.y());
    }


    /// only for avoidance of linking error
    template Point_<float> scale(Point_<float> const& pt, float newLength);
    template Point_<double> scale(Point_<double> const& pt, double newLength);

    template<class T>
    Point_<T> scale(Point_<T> const& pt, T newLength)
    {
        Point_<T> retPoint;
        T curLength = normL2(pt);
        retPoint = pt * (1 / curLength);

        return retPoint * newLength;
    }

    double eucledianDist(Point const& pt1, Point const& pt2)
    {
        return sqrt(std::pow((double)(pt1.x - pt2.x), 2) + std::pow((double)(pt1.y - pt2.y), 2));
    }

    double eucledianDist(cv::Point const& pt1, QPointF const& pt2)
    {
        return sqrt(std::pow((double)(pt1.x - pt2.x()), 2) + std::pow((double)(pt1.y - pt2.y()), 2));
    }

    double eucledianDist(QPointF const& pt1, QPointF const& pt2)
    {
        double d1 = pt1.x() - pt2.x();
        double d2 = pt1.y() - pt2.y();
        return std::sqrt(d1 * d1 + d2 * d2);
    }

    double eucledianDist(QPointF const& p, QLineF const& l)
    {

        QPointF c = getClosestPointOnLinesegment(p, l);
        double distToLineSegment = eucledianDist(p, c);
        return distToLineSegment;
    }

    double eucledianDist(QPointF const& p, QRectF const& r, const bool ellipse)
    {
        if (!ellipse)
        {
            double leftDist     = 0.0;
            double topDist      = 0.0;
            double rightDist    = 0.0;
            double bottomDist   = 0.0;

            /*
             * tl --------- tr
             * |            |
             * |            |
             * |            |
             * |            |
             * bl --------- br
             */
            leftDist    = eucledianDist(p, QLineF(r.topLeft(),      r.bottomLeft()));
            topDist     = eucledianDist(p, QLineF(r.topLeft(),      r.topRight()));
            bottomDist  = eucledianDist(p, QLineF(r.bottomLeft(),   r.bottomRight()));
            rightDist   = eucledianDist(p, QLineF(r.topRight(),     r.bottomRight()));

            return qMin(bottomDist, qMin(rightDist, qMin(leftDist, topDist)));
        }
        else
        {
            double a, b, m, c, xm, ym;
            if (r.width() > r.height())
            {
                a = r.width() / 2;
                b = r.height() / 2;
            }
            else
            {
                a = r.height() / 2;
                b = r.width() / 2;
            }

            xm = r.center().x();
            ym = r.center().y();

            m = (ym - p.y()) / (xm - p.x());
            c = ym - m * xm;

            double A = (a * a) * (m * m) + b * b;
            double B = 2 * (a * a) * m * c - 2 * (a * a) * m * ym - 2 * (b * b) * xm;
            double C = (a * a) * (b * b) - (a * a) * (c * c) - (b * b) * (xm * xm) - (a * a) * (ym * ym) + 2 * (a * a) * c * ym;

            double D = sqrt(C / A + std::pow((B / (2 * A)), 2));

            QPointF p1 = QPointF(D - (B / (2 * A)), m * (D - (B / (2 * A))) + c);
            QPointF p2 = QPointF(-D - (B / (2 * A)), m * (-D - (B / (2 * A))) + c);

            return qMin(eucledianDist(p, p1), eucledianDist(p, p2));
        }
    }

    double calcAngle(Point const& p, Point const& p1, Point const& p2)
    {
        Point l1 = p1 - p;
        Point l2 = p2 - p;

        double norm1 = normL2(l1);
        double norm2 = normL2(l2);

        double testForNan = (l1.x * l2.x + l1.y * l2.y) / (norm1 * norm2);

        double angle;

        if (testForNan >= 1 || testForNan <= -1)
        {
            angle = 180.0;
        }
        else
        {
            angle = (acos((l1.x * l2.x + l1.y * l2.y) / (norm1 * norm2)) * 180 / CV_PI);
        }

        double sgn = (l1.x * l2.y) - (l1.y * l2.x);

        if (sgn < 0)
        {
            angle = 360 - angle;
        }
        return angle;
    }

    double calcAngle(const Point2f& v1, const cv::Point2f& v2)
    {
        // the method caller is responsible that the norms of the vectors are not zero
        double norm1 = normL2(v1);
        double norm2 = normL2(v2);

        double testForNan = (v1.x * v2.x + v1.y * v2.y) / (norm1 * norm2);

        if (testForNan > 1)
        {
            testForNan = 1;
        }
        else if (testForNan < -1)
        {
            testForNan = -1;
        }

        double angle = (acos(testForNan) * 180 / CV_PI);

        return angle;
    }

    double calcAngleToYAxes(Point const& p1, Point const& p2)
    {
        Point yAxesPoint(p1.x, 0);
        double angle = Calc::calcAngle(p1, yAxesPoint, p2);
        return angle;
    }

    double calcCircularAngleSum(double const angle, double const offset)
    {
        double sum = angle + offset;
        double retAngle = sum;
        if (sum < 0)
        {
            retAngle = 360 - sum;
        }
        else if (sum > 360)
        {
            retAngle = sum - 360;
        }

        return retAngle;
    }

    double calcAngleDiff(const double angle1, const double angle2)
    {
        return std::min(std::abs(angle1 - angle2), (360 - (std::abs(angle1 - angle2))));
    }

    double angleToRadian(double const angle)
    {
        return ((angle * M_PI) / 180.0);
    }

    double calcPolygonArea(QPolygonF const& polygon)
    {
        double area = 0.0;
        QPointF p1;
        QPointF p2;

        for (int i = 0; i < polygon.size() - 1; ++i)
        {
            p1 = polygon.at(i);
            p2 = polygon.at(i + 1);
            area += (p1.x() * p2.y() - p2.x() * p1.y());
        }

        area /= 2;

        return abs(area);
    }

    cv::Point calcPolygonCenterOfMass(QPolygonF const& polygon)
    {
        cv::Point centerOfMass(0.0, 0.0);
        QPointF p1;
        QPointF p2;
        double area = calcPolygonArea(polygon);

        for (int i = 0; i < polygon.size() - 1; ++i)
        {
            p1 = polygon.at(i);
            p2 = polygon.at(i + 1);

            centerOfMass.x += ((p1.x() + p2.x()) * (p1.x() * p2.y() - p2.x() * p1.y()));
            centerOfMass.y += ((p1.y() + p2.y()) * (p1.x() * p2.y() - p2.x() * p1.y()));

        }

        centerOfMass.x /= (6 * area);
        centerOfMass.y /= (6 * area);

        centerOfMass.x = abs(centerOfMass.x);
        centerOfMass.y = abs(centerOfMass.y);

        return centerOfMass;
    }

    double calcSpineLength(std::vector<Point> const& spine)
    {
        return cv::arcLength(spine, false);
    }

    double calcPerimeter(QPolygonF const& polygon)
    {
        std::vector<Point> stdPolygon;
        for (int i = 0; i < polygon.size(); ++i)
        {
            stdPolygon.push_back(cv::Point(polygon.at(i).x(), polygon.at(i).y()));
        }

        return arcLength(stdPolygon, true);
    }

    double calcDotProduct(const QPointF& p1, const QPointF& p2)
    {
        return (p1.x() * p2.x() + p1.y() * p2.y());
    }

    double calcCrossProduct(const QPointF& p1, const QPointF& p2)
    {
        return (p1.x() * p2.y() - p1.y() * p2.x());
    }

    double calcSmallestAngle(const Point& p, const Point& p1, const Point& p2)
    {
        Point l1 = p1 - p;
        Point l2 = p2 - p;

        double norm1 = normL2(l1);
        double norm2 = normL2(l2);

        double testForNan = (l1.x * l2.x + l1.y * l2.y) / (norm1 * norm2);

        double angle;

        if (testForNan >= 1 || testForNan <= -1)
        {
            angle = 180.0;
        }
        else
        {
            angle = (acos((l1.x * l2.x + l1.y * l2.y) / (norm1 * norm2)) * 180 / CV_PI);
        }

        return angle;
    }

    unsigned int calcCircularContourNeighbourIndex(const size_t& contourSize, const unsigned int& pos, const int& offset)
    {
        int jump = pos + offset;
        unsigned int index;
        if (jump < 0)
        {
            index = contourSize + jump;
        }
        else if (jump >= (int) contourSize)
        {
            index = jump - contourSize;
        }
        else
        {
            index = jump;
        }

        return index;
    }


    float minSquaredEuclideanDistanceTo(const contour_t& contour, const Point2f& pt)
    {

        float minSquaredDist = std::numeric_limits<float>::max();

        // check every point on the (closed or open) contour
        for (auto const& p : contour)
        {

            float curSquaredDist = std::pow(p.x - pt.x, 2) + std::pow(p.y - pt.y, 2);

            minSquaredDist = std::min<float>(curSquaredDist, minSquaredDist);
        }

        return minSquaredDist;
    }

    bool leftTurn(const Point& p1, const Point& p2, const Point& p3)
    {
        /* calculate determinant of the following matrix ...

           |  1  p1.x  p1.y  |
           |  1  p2.x  p2.y  |
           |  1  p3.x  p3.y  |

           ...with sarrus rule
        */
        double part1 = (1 * p2.x * p3.y) + (1 * p3.x * p1.y) + (1 * p1.x * p2.y);
        double part2 = (1 * p2.x * p1.y) + (1 * p3.x * p2.y) + (1 * p1.x * p3.y);

        double det = part1 - part2;

        return (det > 0);
    }

    uint calcOverlap(const contour_t& firstContour, const contour_t& secondContour)
    {
        contour_t summedContour = firstContour;
        summedContour.insert(summedContour.end(), secondContour.begin(), secondContour.end());

        cv::Rect rect = cv::boundingRect(summedContour);
        cv::Point offset(rect.x, rect.y);

        // create black images for both contours
        cv::Mat firstImage = cv::Mat::zeros(rect.size(), CV_8UC1);
        cv::Mat secondImage = cv::Mat::zeros(rect.size(), CV_8UC1);
        cv::Mat overlapImage = cv::Mat::zeros(rect.size(), CV_8UC1);

        // draw filled (i.e. inner) contours into respective images
        contours_t contourContainer;
        contourContainer.push_back(firstContour);
        cv::drawContours(firstImage, contourContainer, -1, cv::Scalar(1), CV_FILLED, 8,
                         cv::noArray(), 2147483647, cv::Point(-offset.x, -offset.y));

        contourContainer.clear();
        contourContainer.push_back(secondContour);
        cv::drawContours(secondImage, contourContainer, -1, cv::Scalar(1), CV_FILLED, 8,
                         cv::noArray(), 2147483647, cv::Point(-offset.x, -offset.y));

        // logical AND gives the overlap as a result
        overlapImage = firstImage & secondImage;

        cv::Scalar overlap = cv::sum(overlapImage);

        return static_cast<uint>(overlap.val[0]);
    }

    bool isPointInEllipse(const QPointF& p, const QRectF& r)
    {
        qreal h = r.center().x();
        qreal k = r.center().y();

        qreal rx = r.width() / 2;
        qreal ry = r.height() / 2;

        h = (p.x() - h) * (p.x() - h);
        k = (p.y() - k) * (p.y() - k);

        rx = rx * rx;
        ry = ry * ry;

        return ((h / rx) + (k / ry)) <= 1;
    }

    QPointF getClosestPointOnLinesegment(const QPointF& p, const QLineF& l)
    {
        // First point of the segment
        QPointF p1 = l.p1();

        // Second point of the segment
        QPointF p2 = l.p2();

        double xDelta = p2.x() - p1.x();
        double yDelta = p2.y() - p1.y();

        if ((xDelta == 0) && (yDelta == 0))
        {
            std::cout << "p1 and p2 cannot be the same point" << std::endl;
        }

        double u = ((p.x() - p1.x()) * xDelta + (p.y() - p1.y()) * yDelta) / (xDelta * xDelta + yDelta * yDelta);

        // orthogonal projection of point p on the linesegment l
        // is "before" p1 so just return p1
        if (u < 0.0)
        {
            return p1;
        }

        // orthogonal projection of point p on the linesegment l
        // is "after" p2 so just return p2
        else if (u > 1.0)
        {
            return p2;
        }

        // orthogonal projection of point p on the linesegment l
        // is "between" p1 and p2 (so is on the linesegment l)
        else
        {
            return QPointF(p1.x() + u * xDelta, p1.y() + u * yDelta);
        }
    }

    double calcInnerAngleOfVectors(const QPointF& v1, const QPointF& v2)
    {
        qreal n1 = Calc::normL2(v1);
        qreal n2 = Calc::normL2(v2);
        QPointF v1Norm = v1 / n1;
        QPointF v2Norm = v2 / n2;

        double cos_phi = Calc::calcDotProduct(v1Norm, v2Norm);
        return acos(cos_phi) / M_PI * 180;
    }

}
