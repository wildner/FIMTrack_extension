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

#include "PlottingTab.hpp"
#include "ui_PlottingTab.h"

PlottingTab::PlottingTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlottingTab)
{
    this->mLarvaeContainer = nullptr;
    
    ui->setupUi(this);
    
    this->ui->label_2->setAlignment(Qt::AlignCenter);
    this->ui->label_2->setScaledContents(true);
    
    this->setupConnections();
}

PlottingTab::~PlottingTab()
{
    delete ui;
}

void PlottingTab::setupConnections()
{
    connect(this->ui->mdiArea,                      SIGNAL(subWindowActivated(QMdiSubWindow*)),         this,               SLOT(setRightLarvaID(QMdiSubWindow*)));
    connect(this->ui->cbAvailableLarvaIDs,          SIGNAL(currentIndexChanged(QString)),               this,               SLOT(adjustPlottingSettings(QString)));
}

void PlottingTab::setLarvaeContainerPointer(LarvaeContainer *larvaeContainer)
{
    this->mLarvaeContainer = larvaeContainer;
    if(this->mLarvaeContainer)
        connect(this->mLarvaeContainer, SIGNAL(sendRemovedResultLarvaID(uint)), this, SLOT(removePlottingSettings(uint)));
    else
        disconnect(this, SLOT(removePlottingSettings(uint)));
}

void PlottingTab::setAvailableLarvaIDs()
{
    QStringList ids = this->mLarvaeContainer->getAllLarvaeIDs();
    bool oldState = this->ui->cbAvailableLarvaIDs->blockSignals(true);
    this->ui->cbAvailableLarvaIDs->clear();
    this->ui->cbAvailableLarvaIDs->addItems(ids);
    this->ui->cbAvailableLarvaIDs->blockSignals(oldState);
}

void PlottingTab::setAvailableLandmarkNames(const QStringList &landmarkNames)
{
    this->mLandmarkNames = landmarkNames;
    foreach(PlotSettings* p, this->mPlottingSettings)
        p->setAvailableLandmarkNames(this->mLandmarkNames);
}

void PlottingTab::showCroppedImage(const QImage &img)
{
    if(!img.isNull())
        this->ui->label_2->setPixmap(QPixmap::fromImage(img));
}

void PlottingTab::bookmarkCroppedTimeStemp(int timeStemp)
{
    bool ok;
    uint lID = this->ui->cbAvailableLarvaIDs->currentText().split(" ").first().toUInt(&ok);
    
    if(ok)
    {
        int idx;
        if(this->findPlottingSettings(lID, idx))
        {
            PlotSettings* p = this->mPlottingSettings.at(idx);
            p->setCurrentTimeStep(timeStemp);
        }
    }
}

void PlottingTab::setImageSize(QSize size)
{
    this->mImageSize = size;
    foreach(PlotSettings* p, this->mPlottingSettings)
    {
        p->setImageSize(this->mImageSize);
    }
}

void PlottingTab::addSubWindow(PlotWindow *w)
{
    this->ui->mdiArea->addSubWindow(w);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->show();
}

void PlottingTab::adjustPlottingSettings(QString larvaID)
{
    bool ok;
    QString qstrLID = larvaID.split(" ").first();
    uint lID = qstrLID.toUInt(&ok);
    if(ok)
    {             
        int idx;
        if(this->findPlottingSettings(lID, idx))
        {
            foreach(PlotSettings* p, this->mPlottingSettings)
                p->setVisible(false);
            
            PlotSettings* p = this->mPlottingSettings.at(idx);
            p->setVisible(true);
            emit sendCurrentPlottingParameter(qstrLID, p->getCurrentTimeStep());
        }
    }
}

void PlottingTab::setRightLarvaID(QMdiSubWindow *subWindow)
{
    if(subWindow)
    {
        QRegExp rx = QRegExp("(\\d+)");
        if(rx.indexIn(subWindow->windowTitle()) != -1)
        {
            int index = this->findComboboxIndex(rx.cap(1));
            if(index > -1 && index < this->ui->cbAvailableLarvaIDs->count())
                this->ui->cbAvailableLarvaIDs->setCurrentIndex(index);
        }
    }
}

int PlottingTab::findComboboxIndex(QString larvaID)
{
    QRegExp rx = QRegExp("(\\d+)");
    for(int i = 0; i < this->ui->cbAvailableLarvaIDs->count(); ++i)
    {
        if(rx.indexIn(this->ui->cbAvailableLarvaIDs->itemText(i)) != -1)
        {
            if(rx.cap(1).compare(larvaID) == 0)
            {
                return i;
            }
        }
    }
    return -1;
}

void PlottingTab::removePlottingSettings(uint larvaID)
{
    if(this->checkForPlottingSettings(larvaID))
    {
        int index;
        if(this->findPlottingSettings(larvaID, index))
        {
            this->mPlottingSettings.remove(index);
            bool oldState = this->ui->cbAvailableLarvaIDs->blockSignals(true);
            this->ui->cbAvailableLarvaIDs->clear();
            this->ui->cbAvailableLarvaIDs->addItems(this->mLarvaeContainer->getAllLarvaeIDs());
            this->ui->cbAvailableLarvaIDs->blockSignals(oldState);
        }
    }
}

bool PlottingTab::checkForPlottingSettings(uint larvaID)
{
    foreach(PlotSettings* p, this->mPlottingSettings)
    {
        if(p->getLarvaID() == larvaID)
            return true;
    }
    
    return false;
}

bool PlottingTab::findPlottingSettings(uint larvaID, int& index)
{
    if(!this->checkForPlottingSettings(larvaID))
    {
        this->createPlottingSettings(larvaID);
    }
    
    for(int i = 0; i < this->mPlottingSettings.size(); ++i)
    {
        if(this->mPlottingSettings.at(i)->getLarvaID() == larvaID)
        {
            index = i;
            return true;
        }
    }
    
    return false;
}

void PlottingTab::createPlottingSettings(uint larvaID)
{
    this->mPlottingSettings.push_back(new PlotSettings(larvaID, this->mLarvaeContainer));
    connect(this->mPlottingSettings.last(), SIGNAL(sendNewSubwindowPointer(PlotWindow*)), this, SLOT(addSubWindow(PlotWindow*)));
    
    this->ui->gridLayout->addWidget(this->mPlottingSettings.back(), 3, 0, 1, 2);
    this->mPlottingSettings.back()->setVisible(false);
    this->mPlottingSettings.back()->setAvailableLandmarkNames(this->mLandmarkNames);
    this->mPlottingSettings.back()->setImageSize(this->mImageSize);
}
