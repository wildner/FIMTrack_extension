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

#include "ResultsViewer.hpp"
#include "ui_ResultsViewer.h"

ResultsViewer::ResultsViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResultsViewer)
{
    ui->setupUi(this);
    
    mDrawOpenCV       = false;
    
    mTimer            = new QTimer(this);
    mZoomFactor       = 0;
    mPlayingModeOn    = false;
    
    mScene            = ui->graphicsView->getScene();    
    mBtnAddTab        = new QToolButton(ui->larvaTabWidget);
    mBtnAddTab->setIcon(QIcon::fromTheme("list-add"));
    mBtnAddTab->setText("+");
    mBtnAddTab->setCursor(Qt::ArrowCursor);
    mBtnAddTab->setAutoRaise(true);
    ui->larvaTabWidget->setCornerWidget(mBtnAddTab, Qt::TopRightCorner);
    
    dynamic_cast<TrackerGraphicsView*>(ui->graphicsView)->enableLandmarkContextMenu();
    dynamic_cast<TrackerGraphicsView*>(ui->graphicsView)->enableContextMenu(true);
    ui->graphicsView->setEnabled(false);
    ui->pbtOneStepPrevTime->setEnabled(false);
    ui->pbtPlayPause->setEnabled(false);
    ui->pbtOneStepNextTime->setEnabled(false);
    ui->larvaTabWidget->setEnabled(false);
    ui->horizontalSlider_images->setEnabled(false);
    ui->tableView->setEnabled(false);
    ui->tabWidget_images->setEnabled(false);
    
    mPlottingTabVisible = false;
    mLarvaIDForCropping = 0;
    
    ui->tab_3->setLarvaeContainerPointer(&mLarvaeContainer);
    
    setupConnections();
    
    setWindowFlags(Qt::Window);
    showMaximized();
}

ResultsViewer::~ResultsViewer()
{
    delete ui;
}

void ResultsViewer::showImage(int const index)
{
    if (!mFileNames.empty())
    {
        /* get the selected image fileNames list */
        cv::Mat img = cv::imread(QtOpencvCore::qstr2str(mFileNames.at(index)), 0);
        
        mImageSize.setWidth(img.size().width);
        mImageSize.setHeight(img.size().height);
        
        emit sendNewImageSize(mImageSize);
        
        if(mUndistorer.isReady())
        {
            cv::Mat tmpImg;
            mUndistorer.getUndistortImage(img,tmpImg);
            tmpImg.copyTo(img);
        }
        
        cv::Mat cImg = cv::Mat::zeros(img.size(), CV_8UC3);
        cv::cvtColor(img,cImg,CV_GRAY2BGR);
        
        QImage qimg = QtOpencvCore::img2qimg(cImg);
        
        /* convert the opencv image to a QPixmap (to show in a QLabel) */
        QPixmap pixMap = QPixmap::fromImage(qimg);
        
        mScene->setPixmap(pixMap);
    }
}

void ResultsViewer::showTable(const int index)
{
    if (!mFileNames.empty())
    {
        if(mLarvaeContainer.hasLoadedLarvae())
        {
            unsigned int nLarvae = mLarvaeContainer.getNumberOfLarvae();
            Larva l;
            int nLandmarks = 0;
            QStringList landmarkNames;
            if( mScene->hasLandmarkContainer())
            {
                landmarkNames = mScene->getLandmarkContainer()->getAllLandmarkNames();
                nLandmarks = landmarkNames.size();
            }
            
            unsigned int nParameters = 16 +2*nLandmarks;
            
            
            QStandardItemModel *tableModel = new QStandardItemModel(nParameters,nLarvae,this);
            for (unsigned int cols = 0; cols < nLarvae; ++cols)
            {
                if(mLarvaeContainer.getLarva(cols, l))
                {
                    QString hHeader("larva(");
                    hHeader.append(QString::number(l.getID()));
                    hHeader.append(")");
                    tableModel->setHorizontalHeaderItem(cols, new QStandardItem(hHeader));
                }
            }
            
            tableModel->setVerticalHeaderItem(0, new QStandardItem(QString("area:")));
            tableModel->setVerticalHeaderItem(1, new QStandardItem(QString("mom_x:")));
            tableModel->setVerticalHeaderItem(2, new QStandardItem(QString("mom_y:")));
            tableModel->setVerticalHeaderItem(3, new QStandardItem(QString("bodyBending:")));
            tableModel->setVerticalHeaderItem(4, new QStandardItem(QString("isCoiled:")));
            tableModel->setVerticalHeaderItem(5, new QStandardItem(QString("isWellOriented:")));
            tableModel->setVerticalHeaderItem(6, new QStandardItem(QString("isLeftBended:")));
            tableModel->setVerticalHeaderItem(7, new QStandardItem(QString("isRightBended:")));
            tableModel->setVerticalHeaderItem(8, new QStandardItem(QString("isGoPhase:")));
            tableModel->setVerticalHeaderItem(9, new QStandardItem(QString("movDirection:")));
            tableModel->setVerticalHeaderItem(10, new QStandardItem(QString("spineLength:")));
            tableModel->setVerticalHeaderItem(11, new QStandardItem(QString("perimeter:")));
            tableModel->setVerticalHeaderItem(12, new QStandardItem(QString("momDist:")));
            tableModel->setVerticalHeaderItem(13, new QStandardItem(QString("accDist:")));
            tableModel->setVerticalHeaderItem(14, new QStandardItem(QString("distToOrigin:")));
            tableModel->setVerticalHeaderItem(15, new QStandardItem(QString("velocity:")));
            tableModel->setVerticalHeaderItem(16, new QStandardItem(QString("acceleration:")));

            int landmarkIndex = 0;
            for (uint i=17; i<nParameters; i+=2)
            {
                QString curLandmarkName = landmarkNames[landmarkIndex];
                tableModel->setVerticalHeaderItem(i, new QStandardItem(curLandmarkName+QString(" dist")));
                tableModel->setVerticalHeaderItem(i+1, new QStandardItem(curLandmarkName+QString(" angle")));
                ++landmarkIndex;
            }
            
            cv::Point mom;
            double area;
            double bodyBending;
            double spineLength;
            double perimeter;
            double momDist;
            double accDist;
            double distToOrigin;
            bool isCoiled;
            bool isWellOriented;
            bool isLeftBended;
            bool isRightBended;
            int goPhaseIndicator;
            double movementDirection;
            int isCoiledInt;
            int isWellOrientedInt;
            int isLeftBendedInt;
            int isRightBendedInt;
            double velocity;
            double acceleration;
            double distToLandmark;
            double bearingAngle;
            
            
            for (unsigned int cols = 0; cols < nLarvae; ++cols)
            {
                if(mLarvaeContainer.getLarva(cols, l))
                {
                    if (l.getMomentumAt(index,mom))
                    {
                        l.getAreaAt(index,area);
                        l.getMainBodyBendingAngleAt(index,bodyBending);
                        l.getSpineLengthAt(index,spineLength);
                        l.getPerimeterAt(index,perimeter);
                        l.getMomentumDistAt(index,momDist);
                        l.getAccDistAt(index,accDist);
                        l.getDistToOriginAt(index,distToOrigin);                        
                        
                        tableModel->setItem(0,cols, new QStandardItem(QString::number(area)));
                        tableModel->setItem(1,cols, new QStandardItem(QString::number(mom.x)));
                        tableModel->setItem(2,cols, new QStandardItem(QString::number(mom.y)));
                        tableModel->setItem(3,cols, new QStandardItem(QString::number(bodyBending)));
                        if(l.getIsCoiledIndicatorAt(index,isCoiled))
                        {
                            isCoiledInt = isCoiled ? 1 : 0;
                            tableModel->setItem(4,cols, new QStandardItem(QString::number(isCoiledInt)));
                        }

                        if(l.getIsWellOrientedAt(index,isWellOriented))
                        {
                            isWellOrientedInt = isWellOriented ? 1 : 0;
                            tableModel->setItem(5,cols, new QStandardItem(QString::number(isWellOrientedInt)));
                        }
                        
                        if(l.getLeftBendingIndicatorAt(index,isLeftBended))
                        {
                            isLeftBendedInt = isLeftBended ? 1 : 0;
                            tableModel->setItem(6,cols, new QStandardItem(QString::number(isLeftBendedInt)));
                        }
                        
                        if(l.getRightBendingIndicatorAt(index,isRightBended))
                        {
                            isRightBendedInt = isRightBended ? 1 : 0;
                            tableModel->setItem(7,cols, new QStandardItem(QString::number(isRightBendedInt)));
                        }
                        
                        if(l.getGoPhaseIndicatorAt(index,goPhaseIndicator))
                        {
                            tableModel->setItem(8,cols, new QStandardItem(QString::number(goPhaseIndicator)));
                        }
                        
                        if(l.getMovementDirectionAt(index,movementDirection))
                        {
                            tableModel->setItem(9,cols, new QStandardItem(QString::number(movementDirection)));
                        }
                        
                        l.getVelosityAt(index, velocity);
                        l.getAccelerationAt(index, acceleration);
                        
                        tableModel->setItem(10,cols, new QStandardItem(QString::number(spineLength)));
                        tableModel->setItem(11,cols, new QStandardItem(QString::number(perimeter)));
                        tableModel->setItem(12,cols, new QStandardItem(QString::number(momDist)));
                        tableModel->setItem(13,cols, new QStandardItem(QString::number(accDist)));
                        tableModel->setItem(14,cols, new QStandardItem(QString::number(distToOrigin)));
                        tableModel->setItem(15, cols, new QStandardItem(QString::number(velocity)));
                        tableModel->setItem(16, cols, new QStandardItem(QString::number(acceleration)));

                        int landmarkIndex = 0;
                        for (unsigned int i=17; i<nParameters; i+=2)
                        {
                            QString curLandmarkName = landmarkNames[landmarkIndex];
                            if(l.getDistanceToLandmark(index, curLandmarkName.toStdString(), distToLandmark))
                            {
                                l.getBearingAngleToLandmark(index, curLandmarkName.toStdString(), bearingAngle);
                                tableModel->setItem(i, cols, new QStandardItem(QString::number(distToLandmark)));
                                tableModel->setItem(i+1, cols, new QStandardItem(QString::number(bearingAngle)));
                            }
                            ++landmarkIndex;
                        }
                    }
                    
                }
            }
            
            ui->tableView->setModel(tableModel);
        }
    }
}

void ResultsViewer::setupConnections()
{
    connect(mTimer,                               SIGNAL(timeout()),                                  this,                                               SLOT(moveSliderSlot()));
    connect(ui->pbtPlayPause,                     SIGNAL(clicked()),                                  this,                                               SLOT(playPause()));
    connect(ui->pushButton_loadAllResults,        SIGNAL(clicked()),                                  &mLarvaeContainer,                            SLOT(removeAllLarvae()));
    connect(ui->pushButton_SaveResults,           SIGNAL(clicked()),                                  this,                                               SLOT(saveResultLarvae()));
    
    connect(ui->pbtOneStepPrevTime,               SIGNAL(clicked()),                                  this,                                               SLOT(goOneTimeStepPrev()));
    connect(ui->pbtOneStepNextTime,               SIGNAL(clicked()),                                  this,                                               SLOT(goOneTimeStepNext()));
    
    connect(&mLarvaeContainer,                    SIGNAL(sendLarvaModelDeleted()),                    this,                                               SLOT(updateLarvaeTabs()));
    connect(&mLarvaeContainer,                    SIGNAL(sendRemovedResultLarvaID(uint)),             this,                                               SLOT(removeTabByLarvaID(uint)));
    connect(&mLarvaeContainer,                    SIGNAL(reset()),                                    this,                                               SLOT(resetAndClear()));
    connect(&mLarvaeContainer,                    SIGNAL(sendUpdatedResultLarvaID(uint)),             this,                                               SLOT(updateLarvaePath(uint)));
    
    connect(mScene->getLandmarkContainer(),       SIGNAL(landmarkAdded(const Landmark*)),             &mLarvaeContainer,                            SLOT(updateLandmark(const Landmark*)));
    connect(mScene->getLandmarkContainer(),       SIGNAL(landmarkRemoved(QString)),                   &mLarvaeContainer,                            SLOT(removeLandmark(QString)));
    
    connect(ui->pbtRemoveAllWhichAreShorterThen,  SIGNAL(clicked()),                                  this,                                               SLOT(removeAllLarvaeModelsWhichAreShorterThen()));
    connect(this,                                       SIGNAL(sendShortestLarvaeTrackLength(uint)),        &mLarvaeContainer,                            SLOT(removeShortTracks(uint)));
    
    connect(this,                                       SIGNAL(sendMaximumNumberOfTimePoints(int)),         &mLarvaeContainer,                            SLOT(setMaximumNumberOfTimePoints(int)));
    
    connect(ui->tabWidget_images,                 SIGNAL(currentChanged(int)),                        this,                                               SLOT(adjustTimeSlider(int)));
    
    connect(ui->tab_3,                            SIGNAL(sendCurrentPlottingParameter(QString,int)),  this,                                               SLOT(adjustPlottingValues(QString,int)));
    connect(this,                                       SIGNAL(sendAvailableLarvaIDs()),                    ui->tab_3,                                    SLOT(setAvailableLarvaIDs()));
    connect(this,                                       SIGNAL(sendPlottingTimeStemp(int)),                 ui->tab_3,                                    SLOT(bookmarkCroppedTimeStemp(int)));
    connect(this,                                       SIGNAL(sendCroppedImage(QImage)),                   ui->tab_3,                                    SLOT(showCroppedImage(QImage)));
    connect(this,                                       SIGNAL(sendNewImageSize(QSize)),                    ui->tab_3,                                    SLOT(setImageSize(QSize)));
    connect(mScene->getLandmarkContainer(),       SIGNAL(sendAllLandmarkNames(QStringList)),          ui->tab_3,                                    SLOT(setAvailableLandmarkNames(QStringList)));
}

void ResultsViewer::connectTab(int index)
{
    // setup Larvatab-connections
    ui->larvaTabWidget->widget(index)->blockSignals(false);
    
    connect(ui->larvaTabWidget->widget(index),    SIGNAL(requestCenterOn(qreal, qreal)),      this,                                           SLOT(bringLarvaIntoFocus(qreal, qreal)));
    connect(ui->larvaTabWidget->widget(index),    SIGNAL(sendAmbiguityTimepoint(int)),        this,                                           SLOT(on_horizontalSlider_images_valueChanged(int)));       
    connect(this,                                       SIGNAL(newTimeStep(uint)),                  ui->larvaTabWidget->widget(index),        SLOT(update(uint)));
}

void ResultsViewer::disconnectTab(int index)
{
    ui->larvaTabWidget->widget(index)->blockSignals(true);
    disconnect(ui->larvaTabWidget->widget(index),    SIGNAL(requestCenterOn(qreal, qreal)),      this,                                        SLOT(bringLarvaIntoFocus(qreal, qreal)));
    disconnect(ui->larvaTabWidget->widget(index),    SIGNAL(sendAmbiguityTimepoint(int)),        this,                                        SLOT(on_horizontalSlider_images_valueChanged(int)));       
    disconnect(this,                                       SIGNAL(newTimeStep(uint)),                  ui->larvaTabWidget->widget(index),     SLOT(update(uint)));    
}

void ResultsViewer::resetAndClear()
{
    while(ui->larvaTabWidget->count() > 0)
    {
        removeTabByIndex(0);
    }
    
    mCurrentTimestep = 0;
    
    mFileNames.clear();
    mYmlFileName.clear();
    mUndistFileName.clear();
    
    mScene->clearScene();
    
    mImgPaths.clear();
    
    mNumberOfImages = 0;
    
    mPlayingModeOn = false;
    mTimer->stop();
    
    mUndistorer.reset();
    
    loadAllResults();
}

void ResultsViewer::removeAllLarvaeModelsWhichAreShorterThen()
{
    QString result = QInputDialog::getText(0, "Remove Lavaemodels", "Minimum Tracklength (in Timesteps):");
    if(!result.isNull() && !result.isEmpty())
    {
        bool ok;
        uint minTimeSteps = result.toUInt(&ok);
        if(ok)
        {
            emit sendShortestLarvaeTrackLength(minTimeSteps);
        }
    }   
}

void ResultsViewer::adjustTimeSlider(int tabID)
{
    if(tabID == 2)
    {
        mPlottingTabVisible = true;
        cropImage(mLarvaIDForCropping);   
        emit sendPlottingTimeStemp(mCurrentTimestep);
    }
    else
    {
        mPlottingTabVisible = false;
        showImage(mCurrentTimestep);
        showTable(mCurrentTimestep);
        
        updateLarvaeTabs();
        emit newTimeStep(mCurrentTimestep);
    }
}

void ResultsViewer::adjustPlottingValues(QString larvaID, int currentTimeStep)
{    
    bool ok;
    int lID = larvaID.toUInt(&ok);
    if(ok)
    {        
        mLarvaIDForCropping = lID;
        cropImage(mLarvaIDForCropping);
    }
}

void ResultsViewer::moveSliderSlot(bool forward)
{
    int position;
    if(forward)
    {
        position = (ui->horizontalSlider_images->value() < mNumberOfImages)
                ? ui->horizontalSlider_images->value() : 0;
        
        ui->horizontalSlider_images->setValue(++position);
    }
    else
    {
        position = (ui->horizontalSlider_images->value() > 1)
                ? ui->horizontalSlider_images->value() : mNumberOfImages + 1;
        
        ui->horizontalSlider_images->setValue(--position);
    }
}

void ResultsViewer::bringLarvaIntoFocus(qreal x, qreal y)
{
    ui->graphicsView->centerOn(x,y);
}

void ResultsViewer::playPause()
{
    if(mPlayingModeOn)
    {
        mTimer->stop();
        ui->pbtPlayPause->setIcon(QIcon(":/ResultViewer/Icons/Icons/media-playback-start.png"));
        ui->pbtOneStepPrevTime->setEnabled(true);
        ui->pbtOneStepNextTime->setEnabled(true);
    }
    else
    {
        mTimer->start(1000/10);
        ui->pbtPlayPause->setIcon(QIcon(":/ResultViewer/Icons/Icons/media-playback-pause.png"));
        ui->pbtOneStepPrevTime->setEnabled(false);
        ui->pbtOneStepNextTime->setEnabled(false);
    }
    
    mPlayingModeOn = !mPlayingModeOn;
}

void ResultsViewer::resetView()
{
    // Reset all lables
    ui->label_imagePath->setText(QString("No image path selected"));
    ui->label_trackingResults->setText(QString("No tracking result selected"));
    ui->label_cameraMatrix->setText(QString("No camera matrix selected"));
    ui->label_imageNumber->setText(QString("0/0"));
    ui->horizontalSlider_images->setValue(1);
    ui->horizontalSlider_images->setMaximum(1);
}

void ResultsViewer::loadAllResults()
{
    resetView();
    
    if(loadImageFiles()) 
    {
        mNumberOfImages = mFileNames.size();
        
        if(loadYmlFile())
        {
            mScene->loadROIContainer(mYmlFileName);
            mScene->loadLandmarkContainer(mYmlFileName);
            initUndistorer();
            setupBaseGUIElements();
            setupLarvaeTabs();
            
            showImage(0);
            showTable(0);
            
            emit sendMaximumNumberOfTimePoints(mImgPaths.size());
        }
    }
}

bool ResultsViewer::loadImageFiles()
{
    // 1. Load image files...
    mFileNames.clear();
    mYmlFileName.clear();
    mUseUndist = false;
    mImgPaths.clear();
    mScene->clearScene();
    
    mUndistFileName.clear();
    
    mFileNames = QFileDialog::getOpenFileNames(this, tr("Open Image Files"), "", QString::fromStdString(StringConstats::fileFormats));
    mFileNames.sort();
    
    return !mFileNames.isEmpty();
}

bool ResultsViewer::loadYmlFile()
{
    mYmlFileName = QFileDialog::getOpenFileName(this, tr("Open Output File"), "", tr("YML File (*.yml)"));
    
    if (!mYmlFileName.isEmpty())
    {
        mLarvaeContainer.readLarvae(mYmlFileName,
                                          mImgPaths,
                                          mUseUndist);
        
        if(mFileNames.size() != mImgPaths.size())
        {
            QMessageBox messageBox;
            messageBox.critical(0,"Error", "Number of loaded images do not fit with output file!");
            messageBox.setFixedSize(500,200);
            messageBox.show();
            mLarvaeContainer.removeAllLarvae();
            return false;
        }
        else
        {
            return true;
        }
    }
    else 
    {
        return false;
    }
}

void ResultsViewer::initUndistorer()
{
    if(mUseUndist)
    {
        mUndistFileName = QFileDialog::getOpenFileName(this, tr("Open Distortion File"), "", tr("YAML File (*.yaml)"));
        
        if (!mUndistFileName.isEmpty())
        {
            QFileInfo fileInfo = QFileInfo(mUndistFileName);
            ui->label_cameraMatrix->setText(fileInfo.path());
            mUndistorer.setPath(QtOpencvCore::qstr2str(mUndistFileName));
            
        }
    }
}

void ResultsViewer::setupBaseGUIElements()
{
    QFileInfo fileInfo = QFileInfo(mFileNames.at(0));
    ui->label_imagePath->setText(fileInfo.path());
    fileInfo = QFileInfo(mYmlFileName);
    ui->label_trackingResults->setText(fileInfo.path());
    
    QString qstr;
    qstr.append(QString::number(1));
    qstr.append("/");
    qstr.append(QString::number(mNumberOfImages));
    ui->label_imageNumber->setText(qstr);
    
    ui->horizontalSlider_images->setValue(1);
    ui->horizontalSlider_images->setMaximum(mNumberOfImages);
    
    ui->graphicsView->setEnabled(true);
    ui->pbtOneStepPrevTime->setEnabled(true);
    ui->pbtPlayPause->setEnabled(true);
    ui->pbtOneStepNextTime->setEnabled(true);
    ui->larvaTabWidget->setEnabled(true);
    ui->horizontalSlider_images->setEnabled(true);
    ui->tableView->setEnabled(true);
    ui->tabWidget_images->setEnabled(true);
    
    emit sendAvailableLarvaIDs();
}

void ResultsViewer::setupLarvaeTabs()
{
    // create one tab for each Larva
    for (int i = 0; i < mLarvaeContainer.getNumberOfLarvae(); ++i)
    {
        qsrand(QTime::currentTime().msec());
        TrackerSceneLarva* tLarva = mScene->addLarva(
                    mLarvaeContainer.getLarvaPointer(i), 
                    QColor(qrand() % 256, qrand() % 256, qrand() % 256));
        
        ui->larvaTabWidget->addTab(
                    new LarvaTab(tLarva, 
                                 mFileNames.size(),
                                 &mLarvaeContainer,
                                 ui->larvaTabWidget),
                    tLarva->getID());
        
        if(mLarvaeContainer.getAllTimesteps(mLarvaeContainer.getLarvaPointer(i)->getID()).front() > 0)
        {
            ui->larvaTabWidget->setTabEnabled(i, false);
        }
        
        connectTab(i);
    }
}

void ResultsViewer::addTab()
{
}

void ResultsViewer::updateLarvaeTabs()
{
    QPair<QVector<uint>, QVector<uint> > visibilities = mLarvaeContainer.getVisibleLarvaID(mCurrentTimestep);
    int tabIndex;
    
    foreach(uint i, visibilities.first)
    {
        if(findLarvaTab(i, tabIndex))
        {
            ui->larvaTabWidget->setTabEnabled(tabIndex, true);
        }
    }
    
    foreach(uint i, visibilities.second)
    {
        if(findLarvaTab(i, tabIndex))
        {
            ui->larvaTabWidget->setTabEnabled(tabIndex, false);
        }
    }
}

void ResultsViewer::updateLarvaePath(uint larvaID)
{
    int i;
    if(findLarvaTab(larvaID, i))
    {
        dynamic_cast<LarvaTab*>(ui->larvaTabWidget->widget(i))->redrawPaths();
    }
}

void ResultsViewer::removeTabByLarvaID(uint larvaID)
{
    int i;
    if(findLarvaTab(larvaID, i))
    {
        dynamic_cast<LarvaTab*>(ui->larvaTabWidget->widget(i))->getSceneLarva()->removeFromScene();
        ui->larvaTabWidget->widget(i)->deleteLater();
        disconnectTab(i);
        ui->larvaTabWidget->widget(i)->close();
        ui->larvaTabWidget->removeTab(i);
    }
    updateLarvaPointer();
}

void ResultsViewer::removeTabByIndex(int tabIndex)
{
    if(tabIndex < ui->larvaTabWidget->count())
    {
        dynamic_cast<LarvaTab*>(ui->larvaTabWidget->widget(tabIndex))->getSceneLarva()->removeFromScene();
        ui->larvaTabWidget->widget(tabIndex)->deleteLater();
        disconnectTab(tabIndex);
        ui->larvaTabWidget->widget(tabIndex)->close();
        ui->larvaTabWidget->removeTab(tabIndex);
    }
}

bool ResultsViewer::findLarvaTab(uint larvaID, int& index)
{
    QStringList l;
    for(int i = 0; i < ui->larvaTabWidget->count(); ++i)
    {
        l = ui->larvaTabWidget->tabText(i).split(" ");
        if(larvaID == l.value(l.size()-1).toUInt())
        {
            index = i;
            return true;
        }
    }
    
    return false;
}

void ResultsViewer::updateLarvaPointer()
{
    int tabIndex;
    Larva* l;
    for(int i = 0; i < mLarvaeContainer.getNumberOfLarvae(); ++i)
    {
        l = mLarvaeContainer.getLarvaPointer(i);
        if(findLarvaTab(l->getID(), tabIndex))
        {
            dynamic_cast<LarvaTab *>(ui->larvaTabWidget->widget(tabIndex))->setLarvaPointer(l);
        }
    }
}

void ResultsViewer::saveResultLarvae()
{    
    
    QVector<TrackerSceneLarva*> larvaVector;
    for(int i = 0; i < ui->larvaTabWidget->count(); ++i)
    {
        larvaVector.push_back(dynamic_cast<LarvaTab*>(ui->larvaTabWidget->widget(i))->getSceneLarva());
    }
    
    mLarvaeContainer.saveResultLarvae(mImgPaths, mScene->getSaveImage(larvaVector), mUndistorer.isReady(), mScene->getROIContainer(), mScene->getLandmarkContainer());
    
}

void ResultsViewer::on_horizontalSlider_images_valueChanged(int value)
{
    QString qstr;
    qstr.append(QString::number(value));
    qstr.append("/");
    qstr.append(QString::number(mNumberOfImages));
    ui->label_imageNumber->setText(qstr);
    
    mCurrentTimestep = value - 1;
    
    if(!mPlottingTabVisible)
    {
        showImage(value - 1);
        showTable(value - 1);
        
        updateLarvaeTabs();
        emit newTimeStep(mCurrentTimestep);
    }
    else
    {       
        cropImage(mLarvaIDForCropping);   
        emit sendPlottingTimeStemp(mCurrentTimestep);
    }
}

void ResultsViewer::goOneTimeStepPrev()
{
    moveSliderSlot(false);
}

void ResultsViewer::goOneTimeStepNext()
{
    moveSliderSlot(true);
}

void ResultsViewer::cropImage(uint larvaID)
{
    Larva l;
    if (!mFileNames.empty() && mCurrentTimestep < mFileNames.size() && mLarvaeContainer.getLarvaByID(larvaID, l))
    {
        /* get the selected image fileNames list */
        cv::Mat img = cv::imread(QtOpencvCore::qstr2str(mFileNames.at(mCurrentTimestep)), 0);
        
        if(mUndistorer.isReady())
        {
            cv::Mat tmpImg;
            mUndistorer.getUndistortImage(img,tmpImg);
            tmpImg.copyTo(img);
        }
        
        mImageSize.setWidth(img.size().width);
        mImageSize.setHeight(img.size().height);
        emit sendNewImageSize(mImageSize);
        
        cv::Point mom;        
        double spineLength = mLarvaeContainer.getMaxSpineLength();
        if(spineLength > 0.0 && l.getMomentumAt(mCurrentTimestep, mom))
        {
            cv::Rect box;
            
            box.x = mom.x - spineLength;
            if(box.x < 0)
                box.x = 0;
            
            box.y = mom.y - spineLength;
            if(box.y < 0)
                box.y = 0;
            
            box.width = 2*spineLength;
            if(box.x + box.width > img.cols)
                box.width = img.cols - box.x -1;
            
            box.height = 2*spineLength;
            if(box.y + box.height > img.rows)
                box.height = img.rows - box.y -1;
            
            cv::Mat cImg = cv::Mat::zeros(img.size(), CV_8UC3);
            cv::cvtColor(img,cImg,CV_GRAY2BGR);
            
            cImg = cImg(box);
            
            QImage qimg = QtOpencvCore::img2qimg(cImg);
            emit sendCroppedImage(qimg);
        }   
    }
    else
    {
        emit sendCroppedImage(QImage());
    }
}
