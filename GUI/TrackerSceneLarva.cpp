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

#include "TrackerSceneLarva.hpp"

TrackerSceneLarva::TrackerSceneLarva(QGraphicsScene *scene, 
                                     Larva *larva, 
                                     QColor const& color)
{
    this->mShowSpine                = true;
    this->mShowPath                 = true;
    this->mShowDistance2Origin      = false;
    this->mShowSilhouette           = true;
    
    this->mHeadTailRadius           = 1.0;
    this->mID                       = QString("Larva %1").arg(larva->getID());
    
    this->mScene                    = scene;
    this->mLarva                    = larva;
    this->mColor                    = color;
    this->mSpine                    = this->mScene->addPath(QPainterPath());
    this->mPath                     = this->mScene->addPath(QPainterPath());
    this->mDistance2Origin          = this->mScene->addPath(QPainterPath());
    this->mDistance2Origin->setVisible(this->mShowDistance2Origin);
    this->mSilhouette               = this->mScene->addPolygon(QPolygonF());
    this->mDrawID                   = this->mScene->addText(this->mID);
    this->mDrawID->setScale(0.5);
    
    this->mVelosityArrow            = new Arrow();
    this->mScene->addItem(this->mVelosityArrow);
    
    this->mDirectionArrow           = new Arrow();
    this->mScene->addItem(this->mDirectionArrow);
    
    this->initLarvaDrawing();
    this->setColor(this->mColor);  
}

TrackerSceneLarva::~TrackerSceneLarva()
{ 
}

void TrackerSceneLarva::invert(uint time)
{
    Q_UNUSED(time);
    QPainterPath spinePath = this->mSpine->path();
    if(spinePath.elementCount() % 2 == 0 || spinePath.elementCount() != this->mCircles.size())
    {
        return;
    }
    else
    {
        QPainterPath::Element e1, e2;
        TrackerSceneLarvaCircle* c1;
        TrackerSceneLarvaCircle* c2;
        
        for(int i = 0; i < spinePath.elementCount()/2; ++i)
        {
            e1 = spinePath.elementAt(i);
            e2 = spinePath.elementAt(spinePath.elementCount() - 1 - i);
            
            c1 = this->mCircles[i];
            c2 = this->mCircles[this->mCircles.size() - 1 - i];
            
            spinePath.setElementPositionAt(i, e2.x, e2.y);
            spinePath.setElementPositionAt(spinePath.elementCount() - 1 - i, e1.x, e1.y);
            
            this->mCircles[i] = c2;
            this->mCircles[this->mCircles.size() - 1 - i] = c1;
        }
    }
    this->mSpine->setPath(spinePath);
    this->adjustSilhouette();
    this->setColor(this->mColor);
    
    QLineF velosityLine = this->mVelosityArrow->line();
    this->mVelosityArrow->setLine(QLineF(velosityLine.p2(), velosityLine.p1()));
    
    QLineF directionLine = this->mDirectionArrow->line();
    this->mDirectionArrow->setLine(QLineF(directionLine.p2(), directionLine.p1()));
}

void TrackerSceneLarva::updateTime(unsigned int time)
{       
    if(this->checkForSpinePoints())
    {
        this->updateLarvaDrawing(time); 
    }
}

void TrackerSceneLarva::setVisible(bool visible)
{
    this->mPath->setVisible(visible);
    this->mSilhouette->setVisible(visible);
    this->mSpine->setVisible(visible);
    for(unsigned int i = 0; i < this->mCircles.size(); i++)
    {
        this->mCircles.at(i)->setVisible(visible);
    }
    this->mDrawID->setVisible(visible);
    this->mVelosityArrow->setVisible(visible);
    this->mDirectionArrow->setVisible(visible);
}

void TrackerSceneLarva::setColor(const QColor &color)
{
    this->mColor    = color;
    QBrush	brush   = QBrush(QColor(this->mColor.red(), this->mColor.green(), this->mColor.blue(), this->mColor.alpha() / 4));
    QPen	pen     = QPen(this->mColor);
    
    this->mDrawID->setHtml("<div style='background-color: transparent; color:rgb(" + 
                           QString::number(this->mColor.red()) + ", " + 
                           QString::number(this->mColor.green()) + ", " + 
                           QString::number(this->mColor.blue()) + ")'>" + this->mID + "</div>");
    this->mSpine->setPen(pen);	
    
    for(unsigned int i = 0; i < this->mCircles.size(); i++)
    {
        this->mCircles[i]->setPen(pen);
        this->mCircles[i]->setBrush(brush);	
    }
    
    this->mSilhouette->setPen(pen);	
    this->mSilhouette->setBrush(brush);
    
    this->mVelosityArrow->setPen(pen);
    this->mVelosityArrow->setColor(this->mColor);
    
    this->mDirectionArrow->setPen(pen);
    this->mDirectionArrow->setColor(this->mColor);
    
    pen.setStyle(Qt::DashLine);
    this->mPath->setPen(pen);
    
    pen.setStyle(Qt::DotLine);
    this->mDistance2Origin->setPen(pen);
    
    //Mark head
    if(this->mCircles.size() > 0)
    {
        QBrush brush = QBrush(QColor(255, 0, 0, 255));
        this->mCircles[0]->setBrush(brush);
    }
    
    //Mark tail
    if(this->mCircles.size() > 1)
    {
        QBrush brush = QBrush(QColor(0, 0, 255, 255));
        this->mCircles[this->mCircles.size() - 1]->setBrush(brush);
    }
}

void TrackerSceneLarva::setLarvaVisible(bool visible)
{
    this->mShowSpine = visible;
    this->mShowSilhouette = visible;
    
    this->mSpine->setVisible(this->mShowSpine);
    this->mSilhouette->setVisible(this->mShowSilhouette);
    for(unsigned int i = 0; i < this->mCircles.size(); ++i)
    {
        this->mCircles.at(i)->setVisible(this->mShowSilhouette);
    }
    
    this->mDrawID->setVisible(visible);
    
    this->mVelosityArrow->setVisible(visible);
    this->mDirectionArrow->setVisible(visible);
}

void TrackerSceneLarva::setPathVisible(bool visible)
{
    this->mShowPath = visible;
    this->mPath->setVisible(this->mShowPath);
}

void TrackerSceneLarva::setDistance2OriginVisible(bool visible)
{
    this->mShowDistance2Origin = visible;
    this->mDistance2Origin->setVisible(this->mShowDistance2Origin);
}

uint TrackerSceneLarva::getBaseID() const
{
    QStringList l = this->mID.split(" ");
    return l.at(l.size() - 1).toUInt();
}

void TrackerSceneLarva::recompute(unsigned int time)
{
    this->updateLarvaDrawing(time);
    this->adjustDistancePaths();
}

bool TrackerSceneLarva::checkForSpinePoints()
{
    if(this->mLarva->getNSpinePoints() < 1)
    {
        this->setVisible(false);
        return false;    
    }
    return true;
}

void TrackerSceneLarva::initLarvaDrawing()
{
    this->mCurrentTime = 0;
    FIMTypes::spine_t spine;
    std::vector<double> radii;
    std::vector<unsigned int> timeSteps = this->mLarva->getAllTimeSteps();
    
    if(timeSteps.size() < 1)
    {
        this->setVisible(false);
        return;
    }
    
    this->mLarva->getSpineAt(timeSteps.at(0), spine);
    this->mLarva->getSpineRadiiAt(timeSteps.at(0), radii);
    
    if(spine.size() < 1 || radii.size() < 1)
    {
        this->setVisible(false);
        return;
    }
    
    if(spine.size() != radii.size() && spine.size() != this->mLarva->getNSpinePoints() && radii.size() != this->mLarva->getNSpinePoints())
    {
        this->setVisible(false);
        return;
    }
    
    this->adjustCircles();
    this->adjustLarvaContourAndSpine(spine, radii);
    this->adjustDistancePaths();
    this->adjustSilhouette();
    
    if(timeSteps.front() > 0)
    {
        this->setVisible(false);
    }
}

void TrackerSceneLarva::updateLarvaDrawing(unsigned int time)
{    
    this->mCurrentTime = time;
    FIMTypes::spine_t spine;
    std::vector<double> radii;
    
    if(!this->mLarva->getSpineAt(time, spine) || !this->mLarva->getSpineRadiiAt(time, radii))
    {
        this->setVisible(false);
        return;
    }
    
    if(spine.size() < 1 || radii.size() < 1)
    {
        this->setVisible(false);
        return;
    }
    
    if(spine.size() != radii.size() && spine.size() != this->mLarva->getNSpinePoints() && radii.size() != this->mLarva->getNSpinePoints())
    {
        this->setVisible(false);
        return;
    }
    
    this->adjustCircles();
    this->adjustLarvaContourAndSpine(spine, radii);
    this->adjustSilhouette();
    
    this->restoreVisibility();
}

void TrackerSceneLarva::adjustCircles()
{
    /* Reduce circles if neccessary */
    while(this->mCircles.size() > this->mLarva->getNSpinePoints())
    {
        QGraphicsItem* circle = *(this->mCircles.end() - 1);
        this->mScene->removeItem(circle);
        delete circle;
        this->mCircles.pop_back();
    }
    
    /* Add circles if neccessary */
    while(this->mCircles.size() < this->mLarva->getNSpinePoints())
    {
        TrackerSceneLarvaCircle *circle = new TrackerSceneLarvaCircle();
        connect(circle, SIGNAL(changed()), this, SLOT(updateDrawing()));
        this->mScene->addItem(circle);
        this->mCircles.push_back(circle);
    }
    
    emit sendTotalNumberOfSpinePoints(this->mCircles.size());
}

void TrackerSceneLarva::adjustLarvaContourAndSpine(FIMTypes::spine_t const& spine, 
                                                   std::vector<double> const& radii)
{
    /* Iterators */
    FIMTypes::spine_t::const_iterator                 spineIt         = spine.begin();
    FIMTypes::spine_t::const_iterator                 spineItEnd		= spine.end();	
    std::vector<double>::const_iterator                 radiiIt         = radii.begin();
    
    QPainterPath spinePath;    
    spinePath.moveTo(qreal(spineIt->x), qreal(spineIt->y));
    
    for(int i = 0; spineIt != spineItEnd; i++, spineIt++, radiiIt++)
    {
        /* Center-Line */
        spinePath.lineTo(qreal(spineIt->x), qreal(spineIt->y));
        if(*(radiiIt) == 0.0)
        {
            /* Width-Circles */
            this->mCircles[i]->setRect(qreal(spineIt->x - this->mHeadTailRadius),
                                       qreal(spineIt->y - this->mHeadTailRadius),
                                       qreal(this->mHeadTailRadius*2),
                                       qreal(this->mHeadTailRadius*2));
            this->mCircles[i]->setResizable(false);
        }
        else
        {
            /* Width-Circles */
            this->mCircles[i]->setRect(qreal(spineIt->x - (*radiiIt)),
                                       qreal(spineIt->y - (*radiiIt)),
                                       qreal(*radiiIt*2),
                                       qreal(*radiiIt*2));
        }
    }
    
    this->mSpine->setPath(spinePath);
}

void TrackerSceneLarva::adjustDistancePaths()
{
    QPainterPath travelPath;
    QPainterPath dist2OriginPath;
    cv::Point momentum;
    std::vector<unsigned int> timeSteps = this->mLarva->getAllTimeSteps();
    
    this->mLarva->getMomentumAt(timeSteps.front(), momentum);
    travelPath.moveTo(qreal(momentum.x), qreal(momentum.y));
    dist2OriginPath.moveTo(qreal(momentum.x), qreal(momentum.y));
    
    for(unsigned int i = 1; i < timeSteps.size(); ++i)
    {
        if(this->mLarva->getMomentumAt(timeSteps.at(i), momentum))
        {
            travelPath.lineTo(qreal(momentum.x), qreal(momentum.y));
        }
    }
    
    
    this->mLarva->getMomentumAt(timeSteps.back(), momentum);
    travelPath.lineTo(qreal(momentum.x), qreal(momentum.y));
    dist2OriginPath.lineTo(qreal(momentum.x), qreal(momentum.y));
    
    this->mPath->setPath(travelPath);
    this->mDistance2Origin->setPath(dist2OriginPath);
}

void TrackerSceneLarva::removeFromScene()
{   
    this->mScene->removeItem(this->mSpine);
    delete this->mSpine;
    
    this->mScene->removeItem(this->mPath);
    delete this->mPath;
    
    this->mScene->removeItem(this->mDistance2Origin);
    delete this->mDistance2Origin;
    
    this->mScene->removeItem(this->mSilhouette);
    delete this->mSilhouette;
    
    foreach (TrackerSceneLarvaCircle* c, this->mCircles) 
    {
        this->mScene->removeItem(c);
        delete c;
    }
    
    this->mScene->removeItem(this->mDrawID);
    delete this->mDrawID;
    
    this->mScene->removeItem(this->mVelosityArrow);
    delete this->mVelosityArrow;
    
    this->mScene->removeItem(this->mDirectionArrow);
    delete this->mDirectionArrow;
}

void TrackerSceneLarva::prepareForSave(bool prepare)
{
    if(prepare)
    {
        this->mPath->setVisible(prepare);
        this->mDistance2Origin->setVisible(!prepare);
        
        foreach(TrackerSceneLarvaCircle* c, this->mCircles)
        {
            c->setVisible(!prepare);
        }
        
        this->mDirectionArrow->setVisible(!prepare);
        this->mSilhouette->setVisible(!prepare);
        this->mSpine->setVisible(!prepare);
        this->mVelosityArrow->setVisible(!prepare);
        this->mDrawID->setVisible(!prepare);
    }
    else
    {
        this->restoreVisibility();
    }
}

void TrackerSceneLarva::adjustSilhouette()
{
    double drawOffset = 5.0;
    QPolygonF polySilhouette;
    QVector<QPointF> silhouetteOtherSideTop;
    QVector<QPointF> silhouetteOtherSideBottom;
    
    QPainterPath spine = this->mSpine->path();
    
    silhouetteOtherSideBottom << QPointF(spine.elementAt(0).x, spine.elementAt(0).y);
    
    for(int i = 0; i < spine.elementCount() - 1; i++)
    {
        QVector2D v1 = QVector2D(spine.elementAt(i).x, spine.elementAt(i).y);
        QVector2D v2 = QVector2D(spine.elementAt(i + 1).x, spine.elementAt(i + 1).y);
        QVector2D vDirection = (v2 - v1);
        vDirection.normalize();
        vDirection.setX(vDirection.x() * (this->mCircles[i + 1]->rect().width() / 2));
        vDirection.setY(vDirection.y() * (this->mCircles[i + 1]->rect().width() / 2));
        QVector2D vMinus90 = QVector2D(-vDirection.y(), vDirection.x());
        QVector2D vPlus90 = QVector2D(vDirection.y(), -vDirection.x());
        
        silhouetteOtherSideBottom << QPointF(v2.x() + vMinus90.x(), v2.y() + vMinus90.y());
        silhouetteOtherSideTop << QPointF(v2.x() + vPlus90.x(), v2.y() + vPlus90.y());
    }    
    
    for(int i = 0; i < silhouetteOtherSideBottom.size(); i++)
    {
        polySilhouette << silhouetteOtherSideBottom.at(i);
    }
    
    for(int i = silhouetteOtherSideTop.size() - 1; i >= 0; i--)
    {
        polySilhouette << silhouetteOtherSideTop.at(i);
    }
    
    polySilhouette << polySilhouette[0];
    
    this->mSilhouette->setPolygon(polySilhouette);
    
    int goPhase;
    if(this->mLarva->getGoPhaseIndicatorAt(this->mCurrentTime, goPhase))
    {
        if(goPhase == 0)
            this->mDrawID->setHtml("<div style='background-color: transparent; color:rgb(" + 
                                   QString::number(this->mColor.red()) + ", " + 
                                   QString::number(this->mColor.green()) + ", " + 
                                   QString::number(this->mColor.blue()) + ")'>" + (this->mID + QString(":stop")) + "</div>");
        else if(goPhase == 1)
            this->mDrawID->setHtml("<div style='background-color: transparent; color:rgb(" + 
                                   QString::number(this->mColor.red()) + ", " + 
                                   QString::number(this->mColor.green()) + ", " + 
                                   QString::number(this->mColor.blue()) + ")'>" + (this->mID + QString(":go")) + "</div>");
        else
            this->mDrawID->setHtml("<div style='background-color: transparent; color:rgb(" + 
                                   QString::number(this->mColor.red()) + ", " + 
                                   QString::number(this->mColor.green()) + ", " + 
                                   QString::number(this->mColor.blue()) + ")'>" + this->mID + "</div>");
    }
    else
    {
        this->mDrawID->setHtml("<div style='background-color: transparent; color:rgb(" + 
                               QString::number(this->mColor.red()) + ", " + 
                               QString::number(this->mColor.green()) + ", " + 
                               QString::number(this->mColor.blue()) + ")'>" + this->mID + "</div>");
    }
    
    if(!this->isFirstTimestepOfLarva())
    {
        QLineF velosityLine;
        cv::Point p;
        double movementDirection;
        double vel;
        if(this->mLarva->getSpinePointAt(this->mCurrentTime, 0, p) && 
                this->mLarva->getMovementDirectionAt(this->mCurrentTime, movementDirection) &&
                this->mLarva->getMomentumDistAt(this->mCurrentTime, vel))
        {
            this->mDrawID->setRotation(movementDirection);
            movementDirection    = Calc::angleToRadian(movementDirection);
            vel = qMax(10.0, (vel * 5.0));
            
            double sinVal   = std::sin(movementDirection);
            double cosVal   = std::cos(movementDirection);
            
            velosityLine.setP1(QPointF(p.x, p.y));
            
            /**
             * because the angles are calculeted relativly to the y-axis 
             * the meaning of the sin- and cos-values is switched. So if you look at the 
             * circle-table you have to be carefully and keep this fact in mind.
             */
            if(cosVal > 0.0 && sinVal > 0.0)
            {
                velosityLine.setP2(QPointF(p.x + (vel * sinVal), p.y - (vel * cosVal)));
                this->mDrawID->setX(p.x + drawOffset);
                this->mDrawID->setY(p.y + drawOffset);
            }
            else if(cosVal > 0.0 && sinVal < 0.0)
            {
                velosityLine.setP2(QPointF(p.x + (vel * sinVal), p.y - (vel * cosVal)));
                this->mDrawID->setX(p.x + drawOffset);
                this->mDrawID->setY(p.y - drawOffset);
            }
            else if(cosVal < 0.0 && sinVal > 0.0)
            {
                velosityLine.setP2(QPointF(p.x + (vel * sinVal), p.y - (vel * cosVal)));
                this->mDrawID->setX(p.x - drawOffset);
                this->mDrawID->setY(p.y + drawOffset);
            }
            else
            {
                velosityLine.setP2(QPointF(p.x + (vel * sinVal), p.y - (vel * cosVal)));
                this->mDrawID->setX(p.x - drawOffset);
                this->mDrawID->setY(p.y - drawOffset);
            }
            
            this->mVelosityArrow->setLine(velosityLine);
        }
    }
    else
    {
        this->mDrawID->setX(spine.elementAt(0).x);
        this->mDrawID->setY(spine.elementAt(0).y);
    }
    
    cv::Point p1;
    cv::Point p2;
    if(this->mLarva->getSpinePointAt(this->mCurrentTime, 1, p1) && this->mLarva->getSpinePointAt(this->mCurrentTime, 0, p2))
    {
        QLineF directionLine;
        double xOffset  = 0; // (p2.x - p1.x);
        double yOffset  = 0; // (p2.y - p1.y);
        
        directionLine.setP1(QPointF(p1.x + xOffset, p1.y + yOffset));
        directionLine.setP2(QPointF(p2.x + xOffset, p2.y + yOffset));
        
        this->mDirectionArrow->setLine(directionLine);
    }  
}

void TrackerSceneLarva::restoreVisibility()
{
    this->mSpine->setVisible(this->mShowSpine);
    this->mPath->setVisible(this->mShowPath);
    this->mDistance2Origin->setVisible(this->mShowDistance2Origin);
    this->mSilhouette->setVisible(this->mShowSilhouette);
    for(unsigned int i = 0; i < this->mCircles.size(); ++i)
    {
        this->mCircles.at(i)->setVisible(this->mShowSilhouette);
    }
    this->mDrawID->setVisible(this->mShowSilhouette);
    this->mVelosityArrow->setVisible(this->mShowSilhouette);
    this->mDirectionArrow->setVisible(this->mShowSilhouette);
}

bool TrackerSceneLarva::isFirstTimestepOfLarva()
{
    return (this->mLarva->getAllTimeSteps().front() == this->mCurrentTime);
}

void TrackerSceneLarva::updateDrawing()
{   
    // adjust spine-path
    QPainterPath spinePath;
    qreal radius = this->mCircles.at(0)->rect().width()/2;
    
    spinePath.moveTo(this->mCircles.at(0)->rect().x() + radius, 
                     this->mCircles.at(0)->rect().y() + radius);
    
    for(unsigned int i = 1; i < this->mCircles.size(); i++)
    {
        radius = this->mCircles.at(i)->rect().width()/2;
        
        /* Center-Line */
        spinePath.lineTo(this->mCircles.at(i)->rect().x() + radius, 
                         this->mCircles.at(i)->rect().y() + radius);
    }
    
    this->mSpine->setPath(spinePath);
    this->adjustSilhouette();
    
    if(!this->isFirstTimestepOfLarva())
    {
        QLineF directionLine;
        
        double xOffset  = 0;
        double yOffset  = 0;
        
        directionLine.setP1(QPointF(
                                spinePath.elementAt(1).x + xOffset, 
                                spinePath.elementAt(1).y + yOffset));
        
        directionLine.setP2(QPointF(
                                spinePath.elementAt(0).x + xOffset, 
                                spinePath.elementAt(0).y + yOffset));
        
        this->mDirectionArrow->setLine(directionLine);
    }
    
    emit valuesChanged();
}
