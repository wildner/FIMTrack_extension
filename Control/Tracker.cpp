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

#include "Tracker.hpp"

#include <ctime>

using namespace cv;
using std::vector;
using std::string;
using std::map;

Tracker::Tracker(QObject* parent) : QObject(parent), _stopTracking(false)
{
    qRegisterMetaType<LOGLEVEL>("LOGLEVEL");
    connect(this, SIGNAL(logMessageSignal(QString, LOGLEVEL)), Logger::getInstance(), SLOT(handleLogMessage(QString, LOGLEVEL)));
}

void Tracker::startTrackingSlot(const std::vector<std::vector<std::string>>& multiImgPaths,
                                bool showProgress,
                                const Undistorter& undist,
                                RegionOfInterestContainer const* ROIContainer)
{
    _multiImgPaths.clear();


    _multiImgPaths = multiImgPaths;
    _showTrackingProgress = showProgress;

    emit logMessageSignal("Start Tracking!", INFO);

    // go over all jobs
    for (auto const& paths : _multiImgPaths)
    {
        std::vector<string> imgPaths = paths;

        /****** Creation of Directory ******/

        std::string imgPath = imgPaths.at(0);

        QFileInfo fi(QtOpencvCore::str2qstr(imgPath));
        QString absPath = fi.absolutePath();
        absPath.append("/output_");

        QTime time = QTime::currentTime();
        QDate date = QDate::currentDate();
        QString strDate = date.toString("yyyy-MM-dd");
        QString strTime = time.toString("hh-mm-ss");
        absPath.append(strDate);
        absPath.append("_");
        absPath.append(strTime);

        QDir dir = QDir::root();
        dir.mkpath(absPath);

        /*********** Tracking **********/

        _larvaID = 0;

        _curRawLarvae.clear();
        _larvaeContainer.removeAllLarvae();

        Backgroundsubtractor bs(imgPaths, undist);

        uint numProcessed = track(imgPaths, bs, undist, ROIContainer);

        emit logMessageSignal(QString("Postprocessing and Storage of Tracking Results"), INFO);

        /****** Post-Tracking Steps ******/
        _larvaeContainer.interplolateLarvae();

        /********* Save Results *********/
        QString tablePath = absPath;
        tablePath.append("/table");
        tablePath.append("_");
        tablePath.append(strDate);
        tablePath.append("_");
        tablePath.append(strTime);
        tablePath.append(".csv");
        OutputGenerator::writeCSVFile(QtOpencvCore::qstr2str(tablePath), _larvaeContainer.getAllLarvae(), numProcessed);

        QString ymlPath = absPath;
        ymlPath.append("/output");
        ymlPath.append("_");
        ymlPath.append(strDate);
        ymlPath.append("_");
        ymlPath.append(strTime);
        ymlPath.append(".yml");
        OutputGenerator::writeYMLFile(QtOpencvCore::qstr2str(ymlPath), _larvaeContainer.getAllLarvae(), imgPaths, undist.isReady(), ROIContainer);

        QString trackImgPath = absPath;
        trackImgPath.append("/tracks");
        trackImgPath.append("_");
        trackImgPath.append(strDate);
        trackImgPath.append("_");
        trackImgPath.append(strTime);
        trackImgPath.append(".tif");
        OutputGenerator::drawTrackingResults(QtOpencvCore::qstr2str(trackImgPath), imgPaths, _larvaeContainer.getAllLarvae());

        QString trackImgNoNumbersPath = absPath;
        trackImgNoNumbersPath.append("/tracksNoNumbers");;
        trackImgNoNumbersPath.append("_");
        trackImgNoNumbersPath.append(strDate);
        trackImgNoNumbersPath.append("_");
        trackImgNoNumbersPath.append(strTime);
        trackImgNoNumbersPath.append(".tif");
        OutputGenerator::drawTrackingResultsNoNumbers(QtOpencvCore::qstr2str(trackImgNoNumbersPath), imgPaths, _larvaeContainer.getAllLarvae());

    }

    emit trackingDoneSignal();

    emit logMessageSignal("Tracking done!", INFO);
}

void Tracker::stopTrackingSlot()
{
    _stopTracking = true;
}


uint Tracker::track(const std::vector<std::string>& imgPaths,
                    const Backgroundsubtractor& bs,
                    const Undistorter& undist,
                    RegionOfInterestContainer const* ROIContainer)
{
    unsigned int timePoint = 0;

    /***** Create ROI Mask ******/
    cv::Mat mask;
    if (ROIContainer != nullptr && imgPaths.size() > 0)
    {
        cv::Mat firstImg = imread(imgPaths.at(0), CV_LOAD_IMAGE_GRAYSCALE);
        mask = cv::Mat::zeros(firstImg.size(), firstImg.type());

        for (int i = 0; i < ROIContainer->getRegionOfInterests().size(); ++i)
        {
            switch (ROIContainer->getRegionOfInterests().at(i).getType())
            {
                case RegionOfInterest::RECTANGLE:
                    mask(QtOpencvCore::qRect2Rect(ROIContainer->getRegionOfInterests().at(i).getBoundingBox())) = 255;
                    break;
                case RegionOfInterest::ELLIPSE:
                    cv::Mat ellipseMask = cv::Mat::zeros(mask.size(), mask.type());
                    cv::ellipse(ellipseMask, QtOpencvCore::qRect2RotatedRect(ROIContainer->getRegionOfInterests().at(i).getBoundingBox()), cv::Scalar(255, 255, 255), CV_FILLED);
                    mask |= ellipseMask;
                    break;
            }
        }
    }

    /******* iterate over all images and track larvae ********/
    for (auto const& path : imgPaths)
    {
        // check, if stop tracking button clicked in the meantime
        if (_stopTracking)
        {
            emit logMessageSignal(QString("Stop Tracking after processing of image: ").append(QtOpencvCore::str2qstr(path)), INFO);

            _stopTracking = false;

            return timePoint;
        }

        emit logMessageSignal(QString("Process Image: ").append(QtOpencvCore::str2qstr(path)), INFO);

        cv::Mat img = imread(path, CV_LOAD_IMAGE_GRAYSCALE);
        if (ROIContainer != nullptr)
        {
            img &= mask;
        }

        cv::Mat previewImg;

        if (_showTrackingProgress)
        {
            cvtColor(img, previewImg, CV_GRAY2BGR);
        }

        extractRawLarvae(img, bs, &previewImg, false);

        // assignment task
        switch (LarvaeExtractionParameters::AssignmentParameters::eAssignmentMethod)
        {
            case LarvaeExtractionParameters::AssignmentParameters::HUNGARIAN:
                assignByHungarian(timePoint);
                break;
            case LarvaeExtractionParameters::AssignmentParameters::GREEDY:
                assignByGreedy(timePoint);
                break;
        }

        // delete latest contour etc. for saving RAM
        _larvaeContainer.processUntrackedLarvae(timePoint);

        // show tracking result for current timepoint
        if (_showTrackingProgress)
        {
            std::vector<Larva> larvae = _larvaeContainer.getAllLarvae();
            for (auto const& l : larvae)
            {
                FIMTypes::spine_t spine;
                string goText = "";
                std::stringstream ss;
                if (l.getSpineAt(timePoint, spine))
                {
                    bool isCoiled;
                    l.getIsCoiledIndicatorAt(timePoint, isCoiled);
                    Scalar color;
                    if (isCoiled)
                    {
                        color = Scalar(100, 0, 180);
                    }
                    else
                    {
                        color = Scalar(0, 255, 255);
                    }

                    for (auto const& spine_point : spine)
                    {
                        circle(previewImg, spine_point, 2, color, 2);
                    }
                    circle(previewImg, spine.at(0), 3, Scalar(0, 0, 255), 3);
                    circle(previewImg, spine.at(spine.size() - 1), 3, Scalar(255, 0, 0), 3);

                    int goPhaseIndicator;
                    if (l.getGoPhaseIndicatorAt(timePoint, goPhaseIndicator))
                    {
                        if (goPhaseIndicator == 1)
                        {
                            goText = "go";
                        }
                        else if (goPhaseIndicator == 0)
                        {
                            bool leftBended = false;
                            bool rightBended = false;

                            l.getLeftBendingIndicatorAt(timePoint, leftBended);
                            l.getRightBendingIndicatorAt(timePoint, rightBended);
                            if (leftBended)
                            {
                                goText = "stop:left";
                            }
                            else if (rightBended)
                            {
                                goText = "stop:right";
                            }
                            else
                            {
                                goText = "stop";
                            }
                        }
                    }
                    ss << l.getID();
                    ss << ":" << goText;
                    putText(previewImg, ss.str(), spine.at(0), cv::FONT_HERSHEY_PLAIN, 2, Scalar(255, 255, 255), 2);
                }
            }
            emit previewTrackingImageSignal(previewImg);
        }

        ++timePoint;

        int progressbarValue = cvRound((timePoint * 100) / imgPaths.size());
        emit progressBarChangeSignal(progressbarValue);
    }

    return timePoint;
}

void Tracker::extractRawLarvae(Mat const& img, Backgroundsubtractor const& bs, Mat* previewImg, bool checkRoiBorders)
{
    contours_t contours;
    contours_t collidedContours;
    Preprocessor::preprocessTracking(img,
                                     contours,
                                     collidedContours,
                                     GeneralParameters::iGrayThreshold,
                                     GeneralParameters::iMinLarvaeArea,
                                     GeneralParameters::iMaxLarvaeArea,
                                     bs,
                                     checkRoiBorders);

    if (_showTrackingProgress)
    {
        drawContours(*previewImg, contours, -1, Scalar(130, 200, 80), 3);
        drawContours(*previewImg, collidedContours, -1, Scalar(0, 0, 255), 8);
    }

    _curRawLarvae.clear();
    _curRawLarvae.reserve(contours.size());

    for (auto const& c : contours)
    {
        RawLarva rl(c, img);
        _curRawLarvae.push_back(rl);

    }
}

void Tracker::assignByHungarian(unsigned int timePoint)
{
    if (_larvaeContainer.isEmpty())
    {
        for (auto const& rawLarva : _curRawLarvae)
        {
            _larvaeContainer.createNewLarva(timePoint, rawLarva, _larvaID);
            ++_larvaID;
        }
    }
    else
    {
        std::vector<int> validLarvaeIDs = _larvaeContainer.getAllValidLarvaeIDS(timePoint - 1);
        cv::Mat costMatrix = cv::Mat::zeros(_curRawLarvae.size(), validLarvaeIDs.size(), CV_64F); // init value

        // if there are no detections in the last frame
        // inswert current dections as new larvae in larvaecontainer
        if (costMatrix.rows == 0 || costMatrix.cols == 0)
        {
            for (auto const& rawLarva : _curRawLarvae)
            {
                _larvaeContainer.createNewLarva(timePoint, rawLarva, _larvaID);
                ++_larvaID;
            }
        }
        else
        {

            Algorithms::MODE optimizingMode = Algorithms::HUNGARIAN_MODE_MINIMIZE_COST;
            contour_t lastContour, curContour;
            cv::Point lastMomentum, curMomentum, lastMidPoint, curMidPoint;


            switch (LarvaeExtractionParameters::AssignmentParameters::eCostMeasure)
            {
                case LarvaeExtractionParameters::AssignmentParameters::OVERLAP:

                    optimizingMode = Algorithms::HUNGARIAN_MODE_MAXIMIZE_UTIL;

                    for (size_t i = 0; i < _curRawLarvae.size(); ++i)
                    {
                        curContour = _curRawLarvae.at(i).getContour();

                        for (size_t j = 0; j < validLarvaeIDs.size(); ++j)
                        {
                            if (_larvaeContainer.getContour(validLarvaeIDs.at(j), lastContour))
                            {
                                costMatrix.at<double>(i, j) = Calc::calcOverlap(lastContour, curContour);
                            }
                        }
                    }

                    break;

                case LarvaeExtractionParameters::AssignmentParameters::MOMENTUM:

                    optimizingMode = Algorithms::HUNGARIAN_MODE_MINIMIZE_COST;

                    for (size_t i = 0; i < _curRawLarvae.size(); ++i)
                    {
                        curMomentum = _curRawLarvae.at(i).getMomentum();

                        for (size_t j = 0; j < validLarvaeIDs.size(); ++j)
                        {
                            if (_larvaeContainer.getMomentumAt(validLarvaeIDs.at(j), timePoint - 1, lastMomentum))
                            {
                                costMatrix.at<double>(i, j) = Calc::eucledianDist(lastMomentum, curMomentum);
                            }
                            else
                            {
                                costMatrix.at<double>(i, j) = std::numeric_limits<double>::max();
                            }
                        }
                    }

                    break;

                case LarvaeExtractionParameters::AssignmentParameters::MID_SPINE_POINT:

                    optimizingMode = Algorithms::HUNGARIAN_MODE_MINIMIZE_COST;

                    for (size_t i = 0; i < _curRawLarvae.size(); ++i)
                    {
                        curMidPoint = _curRawLarvae.at(i).getMidPoint();

                        for (size_t j = 0; j < validLarvaeIDs.size(); ++j)
                        {
                            uint midPointIndex;
                            if (_larvaeContainer.getSpineMidPointIndex(validLarvaeIDs.at(j), midPointIndex))
                            {
                                _larvaeContainer.getSpinePointAt(validLarvaeIDs.at(j), timePoint - 1, midPointIndex, lastMidPoint);
                                costMatrix.at<double>(i, j) = Calc::eucledianDist(lastMidPoint, curMidPoint);
                            }
                            else
                            {
                                costMatrix.at<double>(i, j) = std::numeric_limits<double>::max();
                            }
                        }
                    }

                    break;
            }

            Algorithms::Hungarian hs = Algorithms::Hungarian(costMatrix, optimizingMode);
            cv::Mat assigments = hs.getAssignmentAsMatrix();
            bool foundLarvaForRawLarva = false;

            for (int i = 0; i < assigments.rows; ++i)
            {

                foundLarvaForRawLarva = false;
                for (int j = 0; j < assigments.cols; ++j)
                {
                    uint midPointIndex;
                    cv::Point midPoint;
                    _larvaeContainer.getSpineMidPointIndex(validLarvaeIDs.at(j), midPointIndex);
                    _larvaeContainer.getSpinePointAt(validLarvaeIDs.at(j), timePoint - 1, midPointIndex, midPoint);

                    if (assigments.at<uchar>(i, j) == 1
                        && _larvaeContainer.larvaHasPointInContour(timePoint, validLarvaeIDs.at(j), _curRawLarvae.at(i).getContour()))
                    {
                        foundLarvaForRawLarva = true;
                        _larvaeContainer.insertRawLarva(validLarvaeIDs.at(j), timePoint, _curRawLarvae.at(i));
                        break;
                    }
                }


                if (!foundLarvaForRawLarva)
                {
                    _larvaeContainer.createNewLarva(timePoint, _curRawLarvae.at(i), _larvaID);
                    ++_larvaID;
                }
            }
        }
    }
}

void Tracker::assignByGreedy(unsigned int timePoint)
{
    if (_larvaeContainer.isEmpty())
    {
        for (auto const& rawLarva : _curRawLarvae)
        {
            _larvaeContainer.createNewLarva(timePoint, rawLarva, _larvaID);
            ++_larvaID;
        }
    }
    else
    {

        std::vector<int> validLarvaeIDs = _larvaeContainer.getAllValidLarvaeIDS(timePoint - 1);

        if (validLarvaeIDs.empty())
        {
            for (auto const& rawLarva : _curRawLarvae)
            {
                _larvaeContainer.createNewLarva(timePoint, rawLarva, _larvaID);
                ++_larvaID;
            }
        }
        else
        {

            contour_t lastContour, curContour;
            cv::Point lastMomentum, curMomentum;
            cv::Point lastMidPoint, curMidPoint;
            double overlap, distance, lastArea, lastSpineLength; // changed and used in every iteration
            double distanceThresh = LarvaeExtractionParameters::AssignmentParameters::dDistanceThreshold;
            double overlapThresh = LarvaeExtractionParameters::AssignmentParameters::dOverlapThreshold;
            bool larvaFound = false;
            uint midPointIndex;

            switch (LarvaeExtractionParameters::AssignmentParameters::eCostMeasure)
            {
                case LarvaeExtractionParameters::AssignmentParameters::OVERLAP:

                    for (size_t i = 0; i < _curRawLarvae.size(); ++i)
                    {
                        curContour = _curRawLarvae.at(i).getContour();

                        for (size_t j = 0; j < validLarvaeIDs.size(); ++j)
                        {
                            if (_larvaeContainer.getContour(validLarvaeIDs.at(j), lastContour))
                            {
                                overlap = Calc::calcOverlap(lastContour, curContour);
                                _larvaeContainer.getAreaAt(validLarvaeIDs.at(j), timePoint - 1, lastArea);

                                if (overlap > overlapThresh * lastArea)
                                {
                                    larvaFound = true;
                                    _larvaeContainer.insertRawLarva(validLarvaeIDs.at(j), timePoint, _curRawLarvae.at(i));
                                    validLarvaeIDs.erase(validLarvaeIDs.begin() + j);
                                    break;
                                }
                            }

                        }
                        if (!larvaFound)
                        {
                            _larvaeContainer.createNewLarva(timePoint, _curRawLarvae.at(i), _larvaID);
                            ++_larvaID;
                        }
                        larvaFound = false;
                    }

                    break;

                case LarvaeExtractionParameters::AssignmentParameters::MOMENTUM:

                    for (size_t i = 0; i < _curRawLarvae.size(); ++i)
                    {
                        curMomentum = _curRawLarvae.at(i).getMomentum();

                        for (size_t j = 0; j < validLarvaeIDs.size(); ++j)
                        {
                            if (_larvaeContainer.getMomentumAt(validLarvaeIDs.at(j), timePoint - 1, lastMomentum))
                            {
                                distance = Calc::eucledianDist(lastMomentum, curMomentum);
                                _larvaeContainer.getSpineLengthAt(validLarvaeIDs.at(j), timePoint - 1, lastSpineLength);

                                if (distance < distanceThresh * lastSpineLength)
                                {
                                    larvaFound = true;
                                    _larvaeContainer.insertRawLarva(validLarvaeIDs.at(j), timePoint, _curRawLarvae.at(i));
                                    validLarvaeIDs.erase(validLarvaeIDs.begin() + j);
                                    break;
                                }
                            }

                        }
                        if (!larvaFound)
                        {
                            _larvaeContainer.createNewLarva(timePoint, _curRawLarvae.at(i), _larvaID);
                            ++_larvaID;
                        }
                        larvaFound = false;
                    }

                    break;

                case LarvaeExtractionParameters::AssignmentParameters::MID_SPINE_POINT:

                    for (size_t i = 0; i < _curRawLarvae.size(); ++i)
                    {
                        curMidPoint = _curRawLarvae.at(i).getMidPoint();

                        for (size_t j = 0; j < validLarvaeIDs.size(); ++j)
                        {
                            if (_larvaeContainer.getSpineMidPointIndex(validLarvaeIDs.at(j), midPointIndex))
                            {
                                _larvaeContainer.getSpinePointAt(validLarvaeIDs.at(j), timePoint - 1, midPointIndex, lastMidPoint);
                                distance = Calc::eucledianDist(lastMidPoint, curMidPoint);
                                _larvaeContainer.getSpineLengthAt(validLarvaeIDs.at(j), timePoint - 1, lastSpineLength);

                                if (distance < distanceThresh * lastSpineLength)
                                {
                                    larvaFound = true;
                                    _larvaeContainer.insertRawLarva(validLarvaeIDs.at(j), timePoint, _curRawLarvae.at(i));
                                    validLarvaeIDs.erase(validLarvaeIDs.begin() + j);
                                    break;
                                }
                            }

                        }
                        if (!larvaFound)
                        {
                            _larvaeContainer.createNewLarva(timePoint, _curRawLarvae.at(i), _larvaID);
                            ++_larvaID;
                        }
                        larvaFound = false;
                    }
                    break;
            }
        }
    }
}
