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

#include "MainGUI.hpp"
#include "ui_MainGUI.h"

using namespace cv;
using std::vector;
using std::string;

MainGUI::MainGUI(QWidget *parent) : 
    QMainWindow(parent), 
    ui(new Ui::MainGUI)
{
    
    try
    {
        ui->setupUi(this);
        this->_scene = this->ui->graphicsView->getScene();
        this->ui->labelCameraParameter->setStyleSheet("QLabel {color : red; }");
        this->ui->labelCameraParameter->setText("No Cameraparameter");
        
        this->ui->treeView->setColumnWidth(0, 120);
        
        /* initialice pointer */        
        this->_preferencesDialogWindow  = new PreferencesDialogWindow(this);
        this->_logWindow                = new LOGWindow(this);
        this->_trackingThread           = new QThread();
        
        dynamic_cast<TrackerGraphicsView*>(this->ui->graphicsView)->enableRIOContextMenu();
        dynamic_cast<TrackerGraphicsView*>(this->ui->graphicsView)->enableContextMenu(true);
        
        qRegisterMetaType<cv::Mat>("cv::Mat");
        qRegisterMetaType<std::vector<std::vector<std::string> > >("std::vector<std::vector<std::string> >");
        qRegisterMetaType<Undistorter>("Undistorter");
        
        connect(this,SIGNAL(startTrackingSignal(std::vector<std::vector<std::string> >,bool,Undistorter, const RegionOfInterestContainer*)),
                &_tracker, SLOT(startTrackingSlot(std::vector<std::vector<std::string> >,bool,Undistorter, const RegionOfInterestContainer*)));
        
        /* direct connection in order to be executed immediately instead of put into execution queue of running thread */
        connect(this, SIGNAL(stopTrackingSignal()),&_tracker,SLOT(stopTrackingSlot()),Qt::DirectConnection);
        
        connect(&_tracker, SIGNAL(previewTrackingImageSignal(cv::Mat)), this, SLOT(previewTrackingImageSlot(cv::Mat)));
        
        connect(&_tracker, SIGNAL(progressBarChangeSignal(int)), this,SLOT(on_progressBar_valueChanged(int)));
        
        connect(&_tracker, SIGNAL(trackingDoneSignal()), this, SLOT(trackingDoneSlot()));
        
        // bind worker (mTracker) to thread and start thread
        this->_tracker.moveToThread(_trackingThread);
        this->_trackingThread->start();
        
        connect(ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
        
        connect(this, SIGNAL(logMessage(QString,LOGLEVEL)), Logger::getInstance(), SLOT(handleLogMessage(QString,LOGLEVEL)));
        connect(Logger::getInstance(), SIGNAL(newMessageForMainGUI(QString)), this, SLOT(showMessage(QString)));
        
        /* Preferencesdialog Connections */
        connect(this->_preferencesDialogWindow,     SIGNAL(loadCameraParameter(QString)),                   this,                               SLOT(initUndistorer(QString)));
        connect(this,                               SIGNAL(loadCameraParameter(QString)),                   this->_preferencesDialogWindow,     SLOT(updateCameraParameterFile(QString)));
        connect(this,                               SIGNAL(updatePreferenceParameter()),                    this->_preferencesDialogWindow,     SLOT(setParameters ()));
        connect(this,                               SIGNAL(setBackgroundSpinboxesValues(unsigned int)),     this->_preferencesDialogWindow,     SLOT(updateBackgroungSpinboxesValues(unsigned int)));
        
        
        this->setupBaseGUIElements(false);
        this->showMaximized();
        emit logMessage("FIMTrack started!", INFO);
    }
    catch(cv::Exception& e)
    {
        emit logMessage(QString("cv::Exception caught: ").append(e.what()), FATAL);
    }
}

MainGUI::~MainGUI()
{
    emit logMessage("Program terminated!", DEBUG);
    
    delete this->ui;
    
    if(this->_trackingThread)
    {
        this->_trackingThread->quit();
        this->_trackingThread->wait();
        delete this->_trackingThread;
    }
    
    if(this->_preferencesDialogWindow)
    {
        delete this->_preferencesDialogWindow;
    }
    
    if(this->_logWindow)
    {
        delete this->_logWindow;
    }
    
    if(GeneralParameters::bSaveLog)
    {
        Logger::getInstance()->saveLog();
    }
}

void MainGUI::on_btnLoad_clicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Image Files"), "", QString::fromStdString(StringConstats::fileFormats));
    if(!fileNames.isEmpty())
    {
        QCollator col;
        col.setNumericMode(true); // THIS is important. This makes sure that numbers are sorted in a human natural way!
        col.setCaseSensitivity(Qt::CaseInsensitive);
        std::sort(fileNames.begin(), fileNames.end(), [&](const QString& a, const QString& b) {
            return col.compare(a, b) < 0;
        });
        this->_fileNames = fileNames;
        this->updateTreeViewer();
        this->setupBaseGUIElements(true);
        this->showImage(this->_fileNames.first());
    }
}

void MainGUI::showImage(QString path)
{
    this->readParameters();
    
    // get the selected image fileNames list
    cv::Mat img = cv::imread(QtOpencvCore::qstr2str(path), 0);
    
    if(img.empty())
    {
        QMessageBox::information(this, "Information", QString("Can't load image ").append(path));
        return;
    }
    
    // undistor the input image
    if(this->_undistorter.isReady())
    {
        cv::Mat dst;
        this->_undistorter.getUndistortImage(img, dst);
        dst.copyTo(img);
        
        if(img.empty())
        {
            QMessageBox::information(this, "Information", QString("Can't undistore image"));
            return;
        }
    }
    
    // generate a contours container
    contours_t contours;
    
    contours_t collidedContours;
    
    // preprocess the image for preview
    Preprocessor::preprocessPreview(img, 
                                    contours, 
                                    collidedContours, 
                                    GeneralParameters::iGrayThreshold, 
                                    GeneralParameters::iMinLarvaeArea, 
                                    GeneralParameters::iMaxLarvaeArea);
    
    cv::Mat grayImg = img.clone();
    
    // set the color to BGR to draw contours and contour sizes into the image
    cv::cvtColor(img, img, cv::COLOR_GRAY2BGR);
    
    // draw contours into the image
    cv::drawContours(img,contours,-1, Scalar(255,0,0), 6);
    cv::drawContours(img,collidedContours,-1,Scalar(0,0,255), 8);
    
    // draw contour sizes into the image
    for(auto const& c : contours)
    {
        // get the current contour size
        int currentContourSize = cv::contourArea(c);
        
        // convert int to string
        std::stringstream ss;
        ss << currentContourSize;
        
        // draw the contour size into the image
        cv::putText(img, ss.str(), c.at(0), cv::FONT_HERSHEY_PLAIN, 3, Scalar(255,255,0), 4);
        
        // draw raw larval informations
        // generate a RawLarva Object
        RawLarva rawLarva(c,grayImg);
        
        // get the discrete spine
        vector<Point> discreteSpine = rawLarva.getDiscreteSpine();
        
        vector<double> larvalRadii = rawLarva.getLarvalRadii();
        
        bool isCoiled = rawLarva.getIsCoiledIndicator();
        Scalar color;
        if(isCoiled)
            color = Scalar(100,0,180);
        else
            color = Scalar(0,255,255);
        
        // draw the sharpest contour point
        cv::circle(img,discreteSpine.at(0),2,Scalar(0,0,255),2);
        
        // draw the spine points
        for (unsigned int i=1; i<discreteSpine.size()-1; ++i)
        {
            cv::circle(img,discreteSpine.at(i),larvalRadii.at(i),color,2);
        }
        
        // draw the second sharpest contour point
        cv::circle(img,discreteSpine.at(discreteSpine.size()-1), 2, Scalar(0,255,0),2);
    }
    
    QImage qimg = QtOpencvCore::img2qimg(img);
    
    // convert the opencv image to a QPixmap (to show in a QLabel)
    QPixmap pixMap = QPixmap::fromImage(qimg);
    
    this->_scene->setPixmap(pixMap);
}

// button "Delete Job"
void MainGUI::on_btnDelete_clicked()
{   
    int index = -1;
    
    // top imtem has chidren
    if(this->ui->treeView->model()->hasChildren(this->ui->treeView->currentIndex()))
    {
        // index of the given top-level item, or -1 if the item cannot be found.
        index = this->ui->treeView->indexOfTopLevelItem(this->ui->treeView->currentItem());
    }
    else
    {
        // child item has to have a valid parent item
        if(this->ui->treeView->model()->parent(this->ui->treeView->currentIndex()).isValid())
        {
            index = this->ui->treeView->indexOfTopLevelItem(this->ui->treeView->currentItem()->parent());
        }
    }
    
    // job is only removed if a top level item (i.e. a job) is selected
    if(index > -1)
    {
        QTreeWidgetItem *item = this->ui->treeView->currentItem();
        this->ui->treeView->takeTopLevelItem(index);
        delete item;
        this->_jobs.removeAt(index);
        QTreeWidgetItemIterator it = QTreeWidgetItemIterator(this->ui->treeView, QTreeWidgetItemIterator::HasChildren);
        
        if(*it)
        {
            while(*it)
            {
                int idx = this->ui->treeView->indexOfTopLevelItem((*it));
                if(idx > -1 && idx >= index)
                {
                    QString text = (*it)->text(0);
                    int jobNumber = text.split(" ").at(1).toInt();
                    (*it)->setText(0, QString("Job ").append(QString::number(--jobNumber)));
                }
                ++it;
            }
        }
        else
        {
            this->_scene->clearScene();
        }
    }
    
    if(this->ui->treeView->topLevelItemCount() == 0)
    {
        this->setupBaseGUIElements(false);
        this->_scene->clearScene();
    }
}

// button "Reset All Jobs"
void MainGUI::on_btnReset_clicked()
{
    this->resetListViewe();
}

void MainGUI::on_btnTrack_clicked()
{
    if(this->ui->btnTrack->text() == "Track")
    {
        this->readParameters();
        
        QString msg;
        msg.append("Start Tracking with parameters: GrayThresh:");
        msg.append(QString::number(this->ui->spinBox_graythresh->value()));
        
        msg.append(" MinSizeThresh:");
        msg.append(QString::number(this->ui->spinBox_minSizeThresh->value()));
        
        msg.append(" MaxSizeThresh:");
        msg.append(QString::number(this->ui->spinBox_maxSizeThresh->value()));
        emit logMessage(msg,DEBUG);
        
        this->ui->btnTrack->setText(QString("Stop"));
        this->ui->btnPreview->setEnabled(false);
        this->ui->btnReset->setEnabled(false);
        this->ui->btnDelete->setEnabled(false);
        this->ui->btnLoad->setEnabled(false);
        this->ui->spinBox_graythresh->setEnabled(false);
        this->ui->spinBox_maxSizeThresh->setEnabled(false);
        this->ui->spinBox_minSizeThresh->setEnabled(false);
        this->ui->progressBar->setEnabled(true);
        this->ui->treeView->setEnabled(false);
        
        std::vector<std::vector<std::string> > multiImgPaths;
        std::vector<std::string> strList;
        for(int i = 0; i < this->ui->treeView->topLevelItemCount(); i++)
        {
            for(int j = 0; j < this->ui->treeView->topLevelItem(i)->childCount(); j++)
            {
                QString path = this->ui->treeView->topLevelItem(i)->text(1);
                path.append("/");
                path.append(this->ui->treeView->topLevelItem(i)->child(j)->text(1));
                strList.push_back(QtOpencvCore::qstr2str(path));
            }
            multiImgPaths.push_back(strList);
            strList.clear();
        }
        emit startTrackingSignal(multiImgPaths, this->ui->checkBoxShowTrackingProgress->isChecked(), this->_undistorter, this->_scene->getROIContainer());
    }
    
    else if(this->ui->btnTrack->text() == "Stop")
    {
        emit stopTrackingSignal();
        
        this->ui->btnTrack->setEnabled(false);
    }
}

void MainGUI::readParameters()
{
    GeneralParameters::iGrayThreshold = this->ui->spinBox_graythresh->value();
    GeneralParameters::iMinLarvaeArea = this->ui->spinBox_minSizeThresh->value();
    GeneralParameters::iMaxLarvaeArea = this->ui->spinBox_maxSizeThresh->value();
    GeneralParameters::bShowTrackingProgress = this->ui->checkBoxShowTrackingProgress->isChecked();
}

void MainGUI::setupBaseGUIElements(bool enable)
{
    this->ui->btnDelete->setEnabled(enable);
    this->ui->btnPreview->setEnabled(enable);
    this->ui->btnReset->setEnabled(enable);
    this->ui->btnTrack->setEnabled(enable);
    
    this->ui->spinBox_graythresh->setEnabled(enable);
    this->ui->spinBox_graythresh->setValue(GeneralParameters::iGrayThreshold);
    this->ui->spinBox_maxSizeThresh->setEnabled(enable);
    this->ui->spinBox_maxSizeThresh->setValue(GeneralParameters::iMaxLarvaeArea);
    this->ui->spinBox_minSizeThresh->setEnabled(enable);
    this->ui->spinBox_minSizeThresh->setValue(GeneralParameters::iMinLarvaeArea);
    
    this->ui->checkBoxShowTrackingProgress->setEnabled(enable);
    this->ui->checkBoxShowTrackingProgress->setChecked(GeneralParameters::bShowTrackingProgress);
    
    this->ui->treeView->setEnabled(enable);
    this->ui->graphicsView->setEnabled(enable);
    
    this->ui->progressBar->setEnabled(enable);
    this->ui->progressBar->setValue(0);
}

void MainGUI::on_btnPreview_clicked()
{
    if(this->ui->treeView->currentItem()->parent())
    {
        QString path = this->ui->treeView->currentItem()->parent()->text(this->ui->treeView->currentColumn());
        path.append("/");
        path.append(this->ui->treeView->currentItem()->text(this->ui->treeView->currentColumn()));
        this->showImage(path);
    }
}

void MainGUI::previewTrackingImageSlot(cv::Mat img)
{
    try
    {
        QImage qimg = QtOpencvCore::img2qimg(img);
        
        // convert the opencv image to a QPixmap (to show in a QLabel)
        QPixmap pixMap = QPixmap::fromImage(qimg);     
        
        this->_scene->setPixmap(pixMap);
    }
    catch(...)
    {
        emit logMessage(QString("Fatal Error in MainGUI::previewTrackingImageSlot"), FATAL);
    }
}

void MainGUI::trackingDoneSlot()
{
    try
    {
        this->_fileNames.clear();
        this->ui->btnTrack->setText(QString("Track"));
        this->ui->btnTrack->setEnabled(true);
        this->ui->btnPreview->setEnabled(true);
        this->ui->btnReset->setEnabled(true);
        this->ui->btnDelete->setEnabled(true);
        this->ui->btnLoad->setEnabled(true);
        this-> ui->spinBox_graythresh->setEnabled(true);
        this-> ui->spinBox_maxSizeThresh->setEnabled(true);
        this->ui->spinBox_minSizeThresh->setEnabled(true);
        this->ui->treeView->setEnabled(true);
        this->ui->progressBar->setEnabled(false);
    }
    catch(...)
    {
        emit logMessage(QString("Fatal Error in MainGUI::trackingDoneSlot"), FATAL);
    }
}

void MainGUI::initUndistorer(const QString &file)
{
    try
    {
        this->_undistorter.setPath(QtOpencvCore::qstr2str(file));
        if(this->_undistorter.isReady())
        {
            this->ui->labelCameraParameter->setStyleSheet("QLabel {color : green; }");
            this->ui->labelCameraParameter->setText("Cameraparameter Loaded");
        }
    }
    catch(...)
    {
        emit logMessage(QString("Fatal Error in MainGUI::initUndistorer"), FATAL);
    }
}

void MainGUI::updateTreeViewer()
{
    if(!this->_fileNames.isEmpty())
    {
        this->_jobs.push_back(_fileNames);
        QFileInfo fInfo(_fileNames.at(0));
        
        QTreeWidgetItem *item = new QTreeWidgetItem(this->ui->treeView);
        item->setText(0, QString("Job ").append(QString::number(this->_jobs.size())));
        item->setText(1, fInfo.absolutePath());
        this->ui->treeView->addTopLevelItem(item);
        
        for (int i = 0; i < this->_fileNames.size(); i++) 
        {
            QFileInfo fi = QFileInfo(this->_fileNames.at(i));
            QTreeWidgetItem *childItem = new QTreeWidgetItem();
            childItem->setText(1, QString(fi.fileName()));
            item->addChild(childItem);
        }
        
        emit logMessage("Load files...", DEBUG);
        emit logMessage(fInfo.absolutePath(), DEBUG);
        QString msg;
        msg.append(QString::number(_fileNames.size()));
        msg.append(" images selected!");
        emit logMessage(msg, DEBUG);
        emit setBackgroundSpinboxesValues(_fileNames.size());
    }    
}

void MainGUI::resetListViewe()
{
    this->_scene->clearScene();
    this->_fileNames.clear();
    this->ui->treeView->clear();
    this->_jobs.clear();
    
    this->setupBaseGUIElements(false);
    
    emit setBackgroundSpinboxesValues(0);
}

void MainGUI::showMessage(QString msg)
{
    this->ui->statusBar->showMessage(msg);
}

void MainGUI::on_actionPreferences_triggered()
{
    try
    {
        emit updatePreferenceParameter();
        this->_preferencesDialogWindow->show();
    }
    catch(...)
    {
        emit logMessage(QString("Fatal Error in MainGUI::on_actionPreferences_triggered"), FATAL);
    }
}

void MainGUI::on_actionOpen_LOG_Window_triggered()
{
    try
    {
        this->_logWindow->show();
    }
    catch(...)
    {
        emit logMessage(QString("Fatal Error in MainGUI::on_actionOpen_LOG_Window_triggered"), FATAL);
    }
}

void MainGUI::on_progressBar_valueChanged(int value)
{
    try
    {
        ui->progressBar->setValue(value);
    }
    catch(...)
    {
        emit logMessage(QString("Fatal Error in MainGUI::on_progressBar_valueChanged"), FATAL);
    }
}

void MainGUI::on_actionLoad_Camera_Setting_triggered()
{
    try
    {
        QString file = QFileDialog::getOpenFileName(this, tr("Open Camera Parameter"), "", tr("Camera Parameter File (*.yaml)"));
        if(!file.isNull() && !file.isEmpty())
        {
            this->_undistorter.setPath(QtOpencvCore::qstr2str(file));
            if(this->_undistorter.isReady())
            {
                this->ui->labelCameraParameter->setStyleSheet("QLabel {color : green; }");
                this->ui->labelCameraParameter->setText("Cameraparameter Loaded");
            }
            emit loadCameraParameter(file);
        }
        
        CameraParameter::File = file;
    }
    catch(...)
    {
        emit logMessage(QString("Fatal Error in MainGUI::on_actionLoad_Camera_Setting_triggered"), FATAL);
    }
}

void MainGUI::on_actionSave_triggered()
{
    try
    {
        if(this->_configurationFile.isNull() || this->_configurationFile.isEmpty())
        {
            this->_configurationFile = QFileDialog::getSaveFileName(this, "Save Configuration", "", "FIMTrack Configuration File (*.conf)");
            if(!this->_configurationFile.isNull() && !this->_configurationFile.isEmpty())
            {
                OutputGenerator::saveConfiguration(QtOpencvCore::qstr2str(this->_configurationFile));
            }
        }
        else
        {
            OutputGenerator::saveConfiguration(QtOpencvCore::qstr2str(this->_configurationFile));
        }
    }
    catch(...)
    {
        emit logMessage(QString("Fatal Error in MainGUI::on_actionSave_triggered"), FATAL);
    }
}

void MainGUI::on_actionSave_As_triggered()
{
    try
    {
        this->_configurationFile = QFileDialog::getSaveFileName(this, "Save Configuration As", "", "FIMTrack Configuration File (*.conf)");
        if(!this->_configurationFile.isNull() && !this->_configurationFile.isEmpty())
        {
            OutputGenerator::saveConfiguration(QtOpencvCore::qstr2str(this->_configurationFile));
        }
    }
    catch(...)
    {
        emit logMessage(QString("Fatal Error in MainGUI::on_actionSave_As_triggered"), FATAL);
    }
}

void MainGUI::on_actionOpen_triggered()
{
    try
    {
        this->_configurationFile = QFileDialog::getOpenFileName(this, tr("Open Configuration File"), "", "FIMTrack Configuration File (*.conf)");
        if(!this->_configurationFile.isNull() && !this->_configurationFile.isEmpty())
        {
            InputGenerator::loadConfiguration(QtOpencvCore::qstr2str(this->_configurationFile));
            
            /* Restore Cameraparameters if there are some */
            if(!CameraParameter::File.isNull() && !CameraParameter::File.isEmpty())
            {
                this->initUndistorer(CameraParameter::File);
            }
            else
            {
                this->ui->labelCameraParameter->setStyleSheet("QLabel {color : red; }");
                this->ui->labelCameraParameter->setText("No Cameraparameter");
            }
            this->setupBaseGUIElements(true);
        }
    }
    catch(...)
    {
        emit logMessage(QString("Fatal Error in MainGUI::on_actionLoad_Camera_Setting_triggered"), FATAL);
    }
}

void MainGUI::on_actionNew_triggered()
{
    TrackerConfig::reset();
    this->resetListViewe();
    this->_configurationFile = "";
    this->_undistorter.setReady(false);
    this->ui->labelCameraParameter->setStyleSheet("QLabel {color : red; }");
    this->ui->labelCameraParameter->setText("No Cameraparameter");
    this->setupBaseGUIElements(false);
}

void MainGUI::on_actionResults_Viewer_triggered()
{
    this->_resultsViewer = new ResultsViewer(this);
    _resultsViewer->setAttribute(Qt::WA_DeleteOnClose);
    _resultsViewer->show();
}

void MainGUI::on_treeView_itemSelectionChanged()
{
    QList<QTreeWidgetItem *> items = ui->treeView->selectedItems();
    if(items.size() == 1)
    {
        QTreeWidgetItem *item = items.at(0);
        if(item->parent())
        {
            QString path = item->parent()->text(1);
            path.append("/");
            path.append(item->text(1));
            this->showImage(path);
        }
    }
}
