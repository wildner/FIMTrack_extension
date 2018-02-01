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

#include "Larva.hpp"

using namespace cv;
using std::vector;
using std::map;
using std::string;

Larva::Larva()
{
}

void Larva::setParameters(const std::map<unsigned int, ValuesType> &parameters)
{
    this->parameters = parameters;
}

bool Larva::setSpineAt(const unsigned int timePoint, FIMTypes::spine_t const& spine)
{
    map<unsigned int, ValuesType>::iterator it;
    bool exists = false;
    
    it = parameters.find(timePoint);
    if(it != parameters.end())
    {
        it->second.spine = spine;
        exists = true;
    }
    
    return exists;
}

std::vector<cv::Point> Larva::getAllMidPoints() const
{
    std::vector<cv::Point> midPoints;
    for (std::map<unsigned int, ValuesType>::const_iterator it = parameters.begin(); it!=parameters.end(); ++it)
    {
        midPoints.push_back(it->second.spine.at(this->getSpineMidPointIndex()));
    }
    
    return midPoints;
}

std::vector<cv::Point> Larva::getAllHeadPoints() const
{
    std::vector<cv::Point> headPoints;
    for (std::map<unsigned int, ValuesType>::const_iterator it = parameters.begin(); it!=parameters.end(); ++it)
    {
        headPoints.push_back(it->second.spine.at(0));
    }
    
    return headPoints;
}

std::vector<cv::Point> Larva::getAllTailPoints() const
{
    std::vector<cv::Point> tailPoints;
    for (std::map<unsigned int, ValuesType>::const_iterator it = parameters.begin(); it!=parameters.end(); ++it)
    {
        tailPoints.push_back(it->second.spine.at(this->nSpinePoints-1));
    }
    
    return tailPoints;
}

std::vector<unsigned int> Larva::getAllTimeSteps() const
{
    std::vector<unsigned int> allTimeSteps;
    for (std::map<unsigned int,ValuesType>::const_iterator it = parameters.begin(); it != parameters.end(); ++it)
    {
        allTimeSteps.push_back(it->first);
    }
    
    return allTimeSteps;
}

void Larva::invert(uint time)
{
    if(this->parameters.find(time) != this->parameters.end())
    {
        cv::Point p1;
        cv::Point p2;
        FIMTypes::spine_t spine = this->parameters[time].spine;
        
        for(size_t i = 0; i < spine.size() / 2; ++i)
        {
            p1 = spine.at(i);
            p2 = spine.at(spine.size() - 1 - i);
            
            spine[i] = p2;
            spine[spine.size() - 1 - i] = p1;
        }
        
        this->parameters[time].spine = spine;
    }
}

int Larva::getXorY(const Point &pt, unsigned int dimension) const
{
    int ret = -1;
    switch(dimension)
    {
        case 0: ret = pt.x; break;
        case 1: ret = pt.y; break;
    }
    return ret;
}

// getter methods
bool Larva::getSpineAt(unsigned int const timePoint, FIMTypes::spine_t & retSpine) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        retSpine = cIt->second.spine;
        exists = true;
    }
    
    return exists;
}

bool Larva::getSpineMidPointAt(const unsigned int timePoint, Point& retSpineMidPoint) const
{
    FIMTypes::spine_t spine;
    if(getSpineAt(timePoint, spine))
    {
        uint midPointIndex = getSpineMidPointIndex();
        retSpineMidPoint = spine.at(midPointIndex);
        return true;
    }
    return false;
}

bool Larva::getMomentumAt(const unsigned int timePoint, Point & retMomentum) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        retMomentum = cIt->second.momentum;
        exists = true;
    }
    
    return exists;
}

bool Larva::getHeadAt(const unsigned int timePoint, Point& retHead) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        retHead = cIt->second.spine.front();
        exists = true;
    }
    
    return exists;
}

bool Larva::getTailAt(const unsigned int timePoint, Point& retTail) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        retTail = cIt->second.spine.back();
        exists = true;
    }
    
    return exists;
}

bool Larva::getAreaAt(const unsigned int timePoint, double &retArea) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        retArea = cIt->second.area;
        exists = true;
    }
    
    return exists;
}

bool Larva::getVelosityAt(const unsigned int timePoint, double &retVelosity) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        retVelosity = cIt->second.velosity;
        exists = true;
    }
    
    return exists;
}

bool Larva::getAccelerationAt(const unsigned int timePoint, double &retAcceleration) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end() /*&& cIt->second.acceleration != std::numeric_limits<double>::min()*/)
    {
        retAcceleration = cIt->second.acceleration;
        exists = true;
    }
    
    return exists;
}

bool Larva::getSpineRadiiAt(const unsigned int timePoint, vector<double> & retSpineRadii) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        retSpineRadii = cIt->second.spineRadii;
        exists = true;
    }
    
    return exists;
}

bool Larva::getMainBodyBendingAngleAt(const unsigned int timePoint, double &retMainBodyBendingAngle) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        retMainBodyBendingAngle = cIt->second.mainBodyBendingAngle;
        exists = true;
    }
    
    return exists;
}

bool Larva::getIsCoiledIndicatorAt(const unsigned int timePoint, bool &retIsCoiledIndicator) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        retIsCoiledIndicator = cIt->second.isCoiled;
        exists = true;
    }
    
    return exists;
}

bool Larva::getIsWellOrientedAt(const unsigned int timePoint, bool &retIsWellOriented) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        retIsWellOriented = cIt->second.isWellOriented;
        exists = true;
    }
    
    return exists;
}

bool Larva::getSpineLengthAt(unsigned int const timePoint, double &retSpineLength) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        retSpineLength = cIt->second.spineLength;
        exists = true;
    }
    return exists;
}

bool Larva::getPerimeterAt(const unsigned int timePoint, double &retPerimeter) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        retPerimeter = cIt->second.perimeter;
        exists = true;
    }
    return exists;
}

bool Larva::getMomentumDistAt(const unsigned int timePoint, double &retMomentumDist) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        retMomentumDist = cIt->second.momentumDist;
        exists = true;
    }
    return exists;
}

bool Larva::getDistToOriginAt(const unsigned int timePoint, double &retDistToOrigin) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        retDistToOrigin = cIt->second.distToOrigin;
        exists = true;
    }
    return exists;
}

bool Larva::getGoPhaseIndicatorAt(const unsigned int timePoint, int &retGoPhaseIndicator) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        int tmpGoPhaseIndicator = cIt->second.goPhase;
        if (tmpGoPhaseIndicator != -1)
        {
            retGoPhaseIndicator = tmpGoPhaseIndicator;
            exists = true;
        }
    }
    return exists;
}

bool Larva::getLeftBendingIndicatorAt(const unsigned int timePoint, bool &retLeftBended) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        retLeftBended = cIt->second.isLeftBended;
        exists = true;
    }
    
    return exists;
}

bool Larva::getRightBendingIndicatorAt(const unsigned int timePoint, bool &retRightBended) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        retRightBended = cIt->second.isRightBended;
        exists = true;
    }
    
    return exists;
}

bool Larva::getMovementDirectionAt(const unsigned int timePoint, double &retMovementDirection) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        double tmpMovementDirection = cIt->second.movementDirection;
        if(tmpMovementDirection != -1)
        {
            retMovementDirection = tmpMovementDirection;
            exists = true;
        }
    }
    
    return exists;
}

bool Larva::getDistanceToLandmark(const unsigned int timePoint, std::string landmarkName, double &retDistanceToLandmark) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        std::map<std::string, double> tmpMovementDirection = cIt->second.distanceToLandmark;
        if(tmpMovementDirection.find(landmarkName) != tmpMovementDirection.end())
        {
            retDistanceToLandmark = tmpMovementDirection[landmarkName];
            exists = true;
        }
    }
    
    return exists;
}

bool Larva::getBearingAngleToLandmark(const unsigned int timePoint, std::string landmarkName, double &retBearingAngleToLandmark) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        std::map<std::string, double> tmpBearingAngle = cIt->second.bearinAngle;
        if(tmpBearingAngle.find(landmarkName) != tmpBearingAngle.end())
        {
            retBearingAngleToLandmark = tmpBearingAngle[landmarkName];
            exists = true;
        }
    }
    
    return exists;
}

bool Larva::getIsInLandmarkIndicator(const unsigned int timePoint, std::string landmarkName, bool &retIsInLandmark) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        std::map<std::string, bool> tmpIsInLandmark = cIt->second.isInLandmark;
        if(tmpIsInLandmark.find(landmarkName) != tmpIsInLandmark.end())
        {
            retIsInLandmark = tmpIsInLandmark[landmarkName];
            exists = true;
        }
    }
    
    return exists;
}

bool Larva::getSpinePointAt(const unsigned int timePoint, const unsigned int index, Point &spinePoint) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end() && cIt->second.spine.size() > index)
    {
        spinePoint = cIt->second.spine.at(index);
        exists = true;
    }
    return exists;
}

bool Larva::getAccDistAt(const unsigned int timePoint, double &retAccDist) const
{
    map<unsigned int, ValuesType>::const_iterator cIt;
    bool exists = false;
    
    cIt = parameters.find(timePoint);
    if(cIt != parameters.end())
    {
        retAccDist = cIt->second.accDist;
        exists = true;
    }
    return exists;
}


// string getter methods
string Larva::getStrMomentum(const unsigned int timePoint, const unsigned int dimension) const
{
    std::stringstream ss;
    Point momentum;
    if(getMomentumAt(timePoint,momentum))
    {
        ss << getXorY(momentum,dimension);
    }
    return ss.str();
}

string Larva::getStrArea(const unsigned int timePoint) const
{
    std::stringstream ss;
    double area;
    if(getAreaAt(timePoint,area))
    {
        ss << area;
    }
    return ss.str();
}

std::string Larva::getStrVelosity(const unsigned int timePoint) const
{
    std::stringstream ss;
    double velosity;
    if(getVelosityAt(timePoint,velosity))
    {
        ss << velosity;
    }
    return ss.str();
}

std::string Larva::getStrAcceleration(const unsigned int timePoint) const
{
    std::stringstream ss;
    double acceleration;
    if(getAccelerationAt(timePoint,acceleration))
    {
        ss << acceleration;
    }
    return ss.str();
}

string Larva::getStrSpine(const unsigned int timePoint, const unsigned int index, const unsigned int dimension) const
{
    std::stringstream ss;
    Point spinePoint;
    if(getSpinePointAt(timePoint,index,spinePoint))
    {
        ss << getXorY(spinePoint, dimension);
    }
    return ss.str();
}

string Larva::getStrSpineRadius(const unsigned int timePoint, const unsigned int index) const
{
    std::stringstream ss;
    vector<double> radii;
    if(getSpineRadiiAt(timePoint,radii))
    {
        ss << radii.at(index);
    }
    return ss.str();
}

string Larva::getStrMainBodyBendingAngle(const unsigned int timePoint) const
{
    std::stringstream ss;
    double mainBodyBendingAngle;
    if(getMainBodyBendingAngleAt(timePoint,mainBodyBendingAngle))
    {
        ss << mainBodyBendingAngle;
    }
    return ss.str();
}

string Larva::getStrIsCoiledIndicator(const unsigned int timePoint) const
{
    std::stringstream ss;
    bool isCoiled;
    if(getIsCoiledIndicatorAt(timePoint,isCoiled))
    {
        if(isCoiled)
            ss << 1;
        else
            ss << 0;
    }
    return ss.str();
}

string Larva::getStrIsWellOriented(const unsigned int timePoint) const
{
    std::stringstream ss;
    bool isWellOriented;
    if(getIsWellOrientedAt(timePoint,isWellOriented))
    {
        if(isWellOriented)
            ss << 1;
        else
            ss << 0;
    }
    return ss.str();
}

string Larva::getStrSpineLength(const unsigned int timePoint) const
{
    std::stringstream ss;
    double spineLength;
    if(getSpineLengthAt(timePoint, spineLength))
    {
        ss << spineLength;
    }
    return ss.str();
}

string Larva::getStrPerimeter(const unsigned int timePoint) const
{
    std::stringstream ss;
    double perimeter;
    if(getPerimeterAt(timePoint,perimeter))
    {
        ss << perimeter;
    }
    return ss.str();
}

string Larva::getStrMomentumDist(const unsigned int timePoint) const
{
    std::stringstream ss;
    double momentumDist;
    if(getMomentumDistAt(timePoint,momentumDist))
    {
        ss << momentumDist;
    }
    return ss.str();
}

string Larva::getStrDistToOrigin(const unsigned int timePoint) const
{
    std::stringstream ss;
    double distToOrigin;
    if(getDistToOriginAt(timePoint,distToOrigin))
    {
        ss << distToOrigin;
    }
    return ss.str();
}

string Larva::getStrAccDist(const unsigned int timePoint) const
{
    std::stringstream ss;
    double accDist;
    if(getAccDistAt(timePoint,accDist))
    {
        ss << accDist;
    }
    return ss.str();
}

string Larva::getStrGoPhaseIndicator(const unsigned int timePoint) const
{
    std::stringstream ss;
    int goPhase;
    if(getGoPhaseIndicatorAt(timePoint,goPhase))
    {
        ss << goPhase;
    }
    return ss.str();
}

string Larva::getStrLeftBendingIndicator(const unsigned int timePoint) const
{
    std::stringstream ss;
    bool isLeftBended;
    if(getLeftBendingIndicatorAt(timePoint,isLeftBended))
    {
        if(isLeftBended)
            ss << 1;
        else
            ss << 0;
    }
    return ss.str();
}

string Larva::getStrRightBendingIndicator(const unsigned int timePoint) const
{
    std::stringstream ss;
    bool isRightBended;
    if(getRightBendingIndicatorAt(timePoint,isRightBended))
    {
        if(isRightBended)
            ss << 1;
        else
            ss << 0;
    }
    return ss.str();
}

std::string Larva::getStrMovementDirection(const unsigned int timePoint) const
{
    std::stringstream ss;
    double movementDirection;
    if(getMovementDirectionAt(timePoint,movementDirection))
    {
        ss << movementDirection;
    }
    return ss.str();
}

std::string Larva::getStrDistanceToLandmark(const unsigned int timePoint, std::string landmarkName) const
{
    std::stringstream ss;
    double distanceToLandmark;
    if(getDistanceToLandmark(timePoint, landmarkName, distanceToLandmark))
    {
        ss << distanceToLandmark;
    }
    return ss.str();
}

std::string Larva::getStrIsInLandmark(const unsigned int timePoint, std::string landmarkName) const
{
    std::stringstream ss;
    bool isInLandmark;
    if(getIsInLandmarkIndicator(timePoint, landmarkName, isInLandmark))
    {
        ss << isInLandmark;
    }
    return ss.str();
}

std::string Larva::getStrBearingAngleToLandmark(const unsigned int timePoint, std::string landmarkName) const
{
    std::stringstream ss;
    double bearingAngle;
    if(getBearingAngleToLandmark(timePoint, landmarkName, bearingAngle))
    {
        ss << bearingAngle;
    }
    return ss.str();
}
