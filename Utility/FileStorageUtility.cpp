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

#include "FileStorageUtility.hpp"

template <typename K, typename V>
void  operator>>(cv::FileNode const& n, std::map<K, V>& m)
{
    if(n.empty() || !n.isMap())
        return;
    
    for(auto fit = n.begin(); fit != n.end(); ++fit)
    {
        auto item = *fit;
        K somekey = static_cast<K>(item.name());
        V someval = static_cast<V>(item);
        m[somekey] = someval;
    }
}
template <typename K>
void operator>>(cv::FileNode const& n, std::map<K, bool>& m)
{
    if(n.empty() || !n.isMap())
        return;
    
    for(auto fit = n.begin(); fit != n.end(); ++fit)
    {
        auto item = *fit;
        K somekey = static_cast<K>(item.name());
        bool someval = static_cast<int>(item);
        m[somekey] = someval;
    }
}
void operator>>(cv::FileNode const& n, cv::Point& p)
{
    std::vector<int> val;
    n >> val;
    p.x = val.at(0);
    p.y = val.at(1);
    
}
void operator>>(cv::FileNode const& n, std::vector<cv::Point>& v)
{
    for (cv::FileNodeIterator it = n.begin(); it != n.end(); ++it)
    {
        cv::Point p;
        (*it) >> p;
        v.push_back(p);
    }
}
void operator>>(cv::FileNode const & n, Larva & larva)
{
    std::map<unsigned int, Larva::ValuesType> parametersMap;
    int id;
    n["larvaID"] >> id;
    
    larva.setID(static_cast<uint>(id));
    
    cv::FileNode parametersNode = n["parameters"];
    
    unsigned int nSpinePoints;
    
    for (cv::FileNodeIterator fnIt = parametersNode.begin(); fnIt != parametersNode.end(); ++fnIt)
    {
        Larva::ValuesType values;
        
        int timeStep;
        (*fnIt)["timeStep"] >> timeStep;
        
        cv::FileNode valuesNode = (*fnIt)["values"];
        cv::FileNodeIterator vIt = valuesNode.begin();
        
        (*vIt)["area"] >> values.area;
        (*vIt)["spine"] >> values.spine;
        nSpinePoints = values.spine.size();
        
        (*vIt)["momentum"] >> values.momentum;
        (*vIt)["spineRadii"] >> values.spineRadii;
        (*vIt)["mainBodyBendingAngle"] >> values.mainBodyBendingAngle;
        (*vIt)["spineLength"] >> values.spineLength;
        (*vIt)["perimeter"] >> values.perimeter;
        (*vIt)["isCoiled"] >> values.isCoiled;
        (*vIt)["isWellOriented"] >> values.isWellOriented;
        (*vIt)["distToOrigin"] >> values.distToOrigin;
        (*vIt)["momentumDist"] >> values.momentumDist;
        (*vIt)["accDist"] >> values.accDist;
        (*vIt)["goPhase"] >> values.goPhase;
        (*vIt)["isLeftBended"] >> values.isLeftBended;
        (*vIt)["isRightBended"] >> values.isRightBended;
        (*vIt)["movementDirection"] >> values.movementDirection;
        (*vIt)["distanceToLandmark"] >> values.distanceToLandmark;
        (*vIt)["isInLandmark"] >> values.isInLandmark;
        (*vIt)["bearinAngle"] >> values.bearinAngle;
        (*vIt)["velocity"] >> values.velosity;
        (*vIt)["acceleration"] >> values.acceleration;
        
        parametersMap.insert(std::pair<unsigned int, Larva::ValuesType>(timeStep,values));
    }
    
    larva.setParameters(parametersMap);
    larva.setNSpinePoints(nSpinePoints);
    
}

template <typename T> 
cv::FileStorage& operator<<(cv::FileStorage& fs, std::vector<T> const& v)
{
    fs << "[";
    for (typename std::vector<T>::const_iterator iter = v.begin(); iter != v.end(); ++iter)
    {
        fs << *iter;
    }
    fs << "]";
    
    return fs;
}
template <typename K, typename V>
cv::FileStorage& operator<<(cv::FileStorage& fs, std::map<K, V> const& m)
{
    fs << "{";
    
    for(auto it = m.begin(); it != m.end(); ++it)
    {
        fs << it->first << it->second;
    }
    
    fs << "}";
    
    return fs;
}
cv::FileStorage& operator<<(cv::FileStorage& fs, Larva const& larva)
{
    typedef std::map<unsigned int, Larva::ValuesType>::const_iterator mapIterType;
    
    int id = (int) larva.id;
    fs << "{" << "larvaID" << id;
    
    fs << "parameters" << "[";
    for (mapIterType it = larva.parameters.begin(); it != larva.parameters.end(); ++it)
    {
        fs << "{";
        int timeStep = (int) it->first;
        fs << "timeStep" << timeStep;
        fs << "values" << "[";
        Larva::ValuesType values = it->second;
        fs << "{";
        fs << "area" << values.area;
        fs << "spine" << values.spine;
        fs << "momentum" << values.momentum;
        fs << "spineRadii" << values.spineRadii;
        fs << "mainBodyBendingAngle" << values.mainBodyBendingAngle;
        fs << "spineLength" << values.spineLength;
        fs << "perimeter" << values.perimeter;
        fs << "isCoiled" << values.isCoiled;
        fs << "isWellOriented" << values.isWellOriented;
        fs << "distToOrigin" << values.distToOrigin;
        fs << "momentumDist" << values.momentumDist;
        fs << "accDist" << values.accDist;
        fs << "goPhase" << values.goPhase;
        fs << "isLeftBended" << values.isLeftBended;
        fs << "isRightBended" << values.isRightBended;
        fs << "movementDirection" << values.movementDirection;
        fs << "velocity" << values.velosity;
        fs << "acceleration" << values.acceleration;
        if(!values.distanceToLandmark.empty())
            fs << "distanceToLandmark" << values.distanceToLandmark;
        if(!values.isInLandmark.empty())
            fs << "isInLandmark" << values.isInLandmark;
        if(!values.bearinAngle.empty())
            fs << "bearinAngle" << values.bearinAngle;
        fs << "}";
        
        fs << "]";
        fs << "}";
    }
    fs << "]";
    fs << "}";
    
    return fs;
}



cv::FileStorage& operator<< (cv::FileStorage& fs, Landmark* l)
{
    fs << "{";
    
    fs << "Color"   << l->mColor;
    fs << "Name"    << l->mName;
    fs << "Type"    << l->mType;
    
    switch(l->mType)
    {
        case Landmark::POINT:
            fs << "LandmarkPoint"                   << l->mIcon->pos();
            fs << "LandmarkPointScenePos"           << l->mapToScene(l->mIcon->pos());
            break;
        case Landmark::LINE:
            fs << "LandmarkLinePointLeft"           << l->mLine->line().p1();
            fs << "LandmarkLinePointLeftScenePos"   << l->mapToScene(l->mLine->line().p1());
            fs << "LandmarkLinePointRight"          << l->mLine->line().p2();
            fs << "LandmarkLinePointRightScenePos"  << l->mapToScene(l->mLine->line().p2());
            break;
        case Landmark::RECTANGLE:
            fs << "LandmarkRect"                    << l->mRect->rect();
            fs << "LandmarkRectScenePos"            << l->mapToScene(l->mRect->rect()).boundingRect();
            break;
        case Landmark::ELLIPSE:
            fs << "LandmarkEllipse"                 << l->mEllipse->rect();
            fs << "LandmarkEllipseScenePos"         << l->mapToScene(l->mEllipse->rect()).boundingRect();
            break;
    }
    
    fs << "}";
    
    return fs;
}
void operator>> (cv::FileNode const& n, Landmark* l)
{
    QPointF p1,p2;
    QRectF r;
    
    n["Color"]  >> l->mColor;
    n["Name"]   >> l->mName;
    n["Type"]   >> l->mType;
    
    switch(l->mType)
    {
        case Landmark::POINT:
            n["LandmarkPointScenePos"] >> p1;
            l->initalizeAfterLoading(p1);
            break;
        case Landmark::LINE:
            n["LandmarkLinePointLeftScenePos"]   >> p1;
            n["LandmarkLinePointRightScenePos"]  >> p2;
            l->initalizeAfterLoading(p1, p2);
            break;
        case Landmark::RECTANGLE:
            n["LandmarkRectScenePos"] >> r;
            l->initalizeAfterLoading(r);
            break;
        case Landmark::ELLIPSE:
            n["LandmarkEllipseScenePos"] >> r;
            l->initalizeAfterLoading(r);
            break;
    }
    
}
void operator>> (cv::FileNode const& n, Landmark::Type& t)
{
    int type;
    n >> type;
    t = static_cast<Landmark::Type>(type);
}




cv::FileStorage& operator<< (cv::FileStorage& fs, RegionOfInterest const& r)
{
    fs << "{";
    
    fs << "Name"    << r.mName;
    fs << "Type"    << r.mType;
    fs << "Rect"    << r.mRect;
    
    fs << "}";
    
    return fs;
}
void operator>> (cv::FileNode const& n, RegionOfInterest& r)
{
    QRectF rect;
    QString name;
    RegionOfInterest::RegionOfInterestType type;
    
    n["Name"] >> name;
    n["Type"] >> type;
    n["Rect"] >> rect;
    
    r.mName = name;
    r.mRect = rect;
    r.mType = type;
    
    r.initalizeAfterLoading();
}
void operator>> (cv::FileNode const& n, RegionOfInterest::RegionOfInterestType& t)
{
    int type;
    n >> type;
    t = static_cast<RegionOfInterest::RegionOfInterestType>(type);
}




cv::FileStorage& operator<< (cv::FileStorage& fs, RegionOfInterestContainer const& c)
{
    fs << "Color" << c.mColor;
    for(int i = 0; i < c.mRegionOfInterests.size(); ++i)
    {
        fs << c.mRegionOfInterests.at(i).getName() << c.mRegionOfInterests.at(i);
    }
    
    return fs;
}
cv::FileStorage& operator<< (cv::FileStorage& fs, RegionOfInterestContainer const* c)
{
    fs << "{";
    
    fs << "Color" << c->mColor;
    
    fs << "ROIs" << "[";
    for(int i = 0; i < c->mRegionOfInterests.size(); ++i)
    {
        fs << c->mRegionOfInterests.at(i);
    }
    fs << "]";
    
    fs << "}";
    
    return fs;
}
void operator>> (cv::FileNode const& n, RegionOfInterestContainer& c)
{
    QColor col;
    n["Color"] >> col;
    c.mColor = col;
    
    cv::FileNode parametersNode = n["ROIs"];
    for (cv::FileNodeIterator fnIt = parametersNode.begin(); fnIt != parametersNode.end(); ++fnIt)
    {
        RegionOfInterest r;
        (*fnIt) >> r;
        c.mRegionOfInterests.push_back(r);
    }
}
void operator>> (cv::FileNode const& n, RegionOfInterestContainer* c) 
{
    QColor col;
    n["Color"] >> col;
    c->mColor = col;
    
    cv::FileNode parametersNode = n["ROIs"];
    for (cv::FileNodeIterator fnIt = parametersNode.begin(); fnIt != parametersNode.end(); ++fnIt)
    {
        RegionOfInterest r;
        (*fnIt) >> r;
        c->mRegionOfInterests.push_back(r);
    }
    
    c->setColor(c->mColor);
}




cv::FileStorage& operator<< (cv::FileStorage& fs, QColor const& c)
{
    return fs << "[:" << c.red() << c.green() << c.blue() << c.alpha() << "]";
}
void operator>> (cv::FileNode const& n, QColor& c)
{
    std::vector<int> val;
    n >> val;
    
    if(!val.empty())
    {
        c.setRed(val.at(0));
        c.setGreen(val.at(1));
        c.setBlue(val.at(2));
        //    c.setAlpha(val.at(3));
    }
}

cv::FileStorage& operator<< (cv::FileStorage& fs, QPointF const& p)
{
    return fs << "[:" << p.x() << p.y() << "]";
}
void operator>> (cv::FileNode const& n, QPointF& p)
{
    std::vector<double> val;
    n >> val;
    if(!val.empty())
    {
        p.setX(val.at(0));
        p.setY(val.at(1));
    }
}

cv::FileStorage& operator<< (cv::FileStorage& fs, QRectF const& r)
{
    return fs << "[:" << r.x() << r.y() << r.width() << r.height() << "]";
}
void operator>> (cv::FileNode const& n, QRectF& r)
{
    std::vector<double> val;
    n >> val;
    if(!val.empty())
    {
        r.setX(val.at(0));
        r.setY(val.at(1));
        r.setWidth(val.at(2));
        r.setHeight(val.at(3));
    }
}

cv::FileStorage& operator<< (cv::FileStorage& fs, QString const& s)
{
    return fs << QtOpencvCore::qstr2str(s);
}
void operator>> (cv::FileNode const& n, QString& s)
{
    std::string name;
    n >> name;
    s = QtOpencvCore::str2qstr(name);
}

