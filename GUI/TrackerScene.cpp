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

#include "TrackerScene.hpp"

TrackerScene::TrackerScene(QObject *parent) : QGraphicsScene(parent)
{
    this->mMousePressed         = false;
    this->mImage                = this->addPixmap(QPixmap());
    this->mROIContainer         = nullptr;
    this->mLandmarkContainer    = new LandmarkContainer(this);
}

TrackerScene::~TrackerScene()
{
    this->clear();
}

TrackerSceneLarva* TrackerScene::addLarva(Larva* larva, QColor color)
{
    TrackerSceneLarva *res = new TrackerSceneLarva(this, larva, color);
    return res;
}

QPixmap TrackerScene::getPixmap()
{
    return this->mImage->pixmap();
}

QRectF TrackerScene::boundingRect() const
{
    return this->sceneRect();
}

void TrackerScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);
}

void TrackerScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
}

void TrackerScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    this->mMousePressed = false;
    QGraphicsScene::mouseReleaseEvent(event);
}

void TrackerScene::checkForRIOContainer()
{
    if(!this->mROIContainer)
    {
        this->mROIContainer = new RegionOfInterestContainer(this->sceneRect());
        this->addItem(this->mROIContainer);
    }
}
void TrackerScene::addROI(const QPointF &p, RegionOfInterest::RegionOfInterestType type)
{
    bool ok;
    QString roiName;
    
    if(this->mROIContainer)
    {
        roiName = QInputDialog::getText(nullptr, 
                                        "Add new Region of Interest", 
                                        "RIO-Name:",
                                        QLineEdit::Normal,
                                        "Region of Interest " + QString::number(this->mROIContainer->getSize() + 1),
                                        &ok);
    }
    else
    {
        roiName = QInputDialog::getText(nullptr, 
                                        "Add new Region of Interest", 
                                        "RIO-Name:",
                                        QLineEdit::Normal,
                                        "Region of Interest 1",
                                        &ok);
    }
    
    if(ok)
    {
        this->checkForRIOContainer();
        if(roiName.isNull() || roiName.isEmpty())
        {
            switch(type)
            {
                case RegionOfInterest::RECTANGLE:
                    this->mROIContainer->addRegionOfInterest("RIO " + QString::number(this->mROIContainer->getSize() + 1),
                                                             QRectF(p.x(), p.y(), this->mDefaultROIWidth, this->mDefaultROIHeight),
                                                             RegionOfInterest::RECTANGLE);
                    break;
                case RegionOfInterest::ELLIPSE:
                    this->mROIContainer->addRegionOfInterest("RIO " + QString::number(this->mROIContainer->getSize() + 1),
                                                             QRectF(p.x(), p.y(), this->mDefaultROIWidth, this->mDefaultROIHeight),
                                                             RegionOfInterest::ELLIPSE);
                    break;
            }
        }
        else
        {
            switch(type)
            {
                case RegionOfInterest::RECTANGLE:
                    this->mROIContainer->addRegionOfInterest(roiName,
                                                             QRectF(p.x(), p.y(), this->mDefaultROIWidth, this->mDefaultROIHeight),
                                                             RegionOfInterest::RECTANGLE);
                    break;
                case RegionOfInterest::ELLIPSE:
                    this->mROIContainer->addRegionOfInterest(roiName,
                                                             QRectF(p.x(), p.y(), this->mDefaultROIWidth, this->mDefaultROIHeight),
                                                             RegionOfInterest::ELLIPSE);
                    break;
            }
        }
    }
}

void TrackerScene::removeROI(QPointF const& p)
{
    if(this->mROIContainer)
    {
        this->mROIContainer->removeRegionOfInterest(p);
        if(this->mROIContainer->isEmpty())
        {
            this->removeItem(this->mROIContainer);
            this->mROIContainer = nullptr;
        }
    }
}

bool TrackerScene::ROIContainsPoint(QPointF const& p)
{
    if(this->mROIContainer)
        return this->mROIContainer->contains(p);
    return false;
}

void TrackerScene::loadROIContainer(const QString &path)
{
    this->checkForRIOContainer();
    InputGenerator::readRegionOfInterests(QtOpencvCore::qstr2str(path), this->mROIContainer);
    if(this->mROIContainer->isEmpty())
    {
        this->removeItem(this->mROIContainer);
        delete this->mROIContainer;
        this->mROIContainer = nullptr;
    }
}

void TrackerScene::addLandmark(QPointF const& p, Landmark::Type type)
{
    bool ok;
    QString landmarkName;
    
    if(this->mLandmarkContainer)
    {
        landmarkName = QInputDialog::getText(nullptr, 
                                             "Add new Landmark", 
                                             "Landmarkname:",
                                             QLineEdit::Normal,
                                             "Landmark " + QString::number(this->mLandmarkContainer->getSize() + 1),
                                             &ok);
    }
    else
    {
        landmarkName = QInputDialog::getText(nullptr, 
                                             "Add new Landmark", 
                                             "Landmarkname:",
                                             QLineEdit::Normal,
                                             "Landmark 1",
                                             &ok);
    }
    
    if(ok)
    {
        if(landmarkName.isNull() || landmarkName.isEmpty())
        {
            switch(type)
            {
                case Landmark::POINT:
                    this->mLandmarkContainer->addLandmark("Landmark " + QString::number(this->mLandmarkContainer->getSize() + 1),
                                                          p, Landmark::POINT);
                    break;
                case Landmark::LINE:
                    this->mLandmarkContainer->addLandmark("Landmark " + QString::number(this->mLandmarkContainer->getSize() + 1),
                                                          p, Landmark::LINE);
                    break;
                case Landmark::RECTANGLE:
                    this->mLandmarkContainer->addLandmark("Landmark " + QString::number(this->mLandmarkContainer->getSize() + 1),
                                                          p, Landmark::RECTANGLE);
                    break;
                case Landmark::ELLIPSE:
                    this->mLandmarkContainer->addLandmark("Landmark " + QString::number(this->mLandmarkContainer->getSize() + 1),
                                                          p, Landmark::ELLIPSE);
                    break;
            }
        }
        else
        {
            switch(type)
            {
                case Landmark::POINT:
                    this->mLandmarkContainer->addLandmark(landmarkName,
                                                          p, Landmark::POINT);
                    break;
                case Landmark::LINE:
                    this->mLandmarkContainer->addLandmark(landmarkName,
                                                          p, Landmark::LINE);
                    break;
                case Landmark::RECTANGLE:
                    this->mLandmarkContainer->addLandmark(landmarkName,
                                                          p, Landmark::RECTANGLE);
                    break;
                case Landmark::ELLIPSE:
                    this->mLandmarkContainer->addLandmark(landmarkName,
                                                          p, Landmark::ELLIPSE);
                    break;
            }
        }
    }
}

void TrackerScene::removeLandmark(const QPointF &p)
{
    if(this->mLandmarkContainer)
    {
        this->mLandmarkContainer->removeLandmak(p);
    }
}

bool TrackerScene::landmarkContainsPoint(const QPointF &p)
{
    if(this->mLandmarkContainer)
        return this->mLandmarkContainer->contains(p);
    return false;
}

void TrackerScene::loadLandmarkContainer(const QString &path)
{
    InputGenerator::readLandmarks(QtOpencvCore::qstr2str(path), this->mLandmarkContainer);
}

void TrackerScene::clearScene()
{
    this->clear();    
    //    this->mLarvae.clear();
    this->mImage = this->addPixmap(QPixmap());
    
    this->mROIContainer = nullptr;
    
    if(this->mLandmarkContainer)
    {
        this->mLandmarkContainer->clear();
    }
}

QImage TrackerScene::getSaveImage(QVector<TrackerSceneLarva*> larvaVector)
{
    QImage image(this->boundingRect().size().toSize(), QImage::Format_ARGB32_Premultiplied);  // Create the image with the exact size of the shrunk scene
    foreach(TrackerSceneLarva* tl, larvaVector)
    {
        this->mImage->setVisible(false);
        tl->prepareForSave(true);
        image.fill(Qt::black);    // Start all pixels black
        
        QPainter painter(&image);
        painter.setRenderHint(QPainter::Antialiasing);
        this->render(&painter);
    }
    
    foreach(TrackerSceneLarva* tl, larvaVector)
    {
        this->mImage->setVisible(true);
        tl->prepareForSave(false);
    }
    
    return image;
}

void TrackerScene::setPixmap(QPixmap const& img)
{
    this->mImage->setPixmap(img);
    this->setSceneRect(0, 0, img.width(), img.height());
    if(this->mROIContainer)
        this->mROIContainer->setBoundingBox(this->sceneRect());
}
