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

#ifndef TRACKER_HPP
#define TRACKER_HPP

#include <vector>
#include <iterator>
#include <list>
#include <string>
#include <map>

#include <QTime>
#include <QMutex>
#include <QFileInfo>
#include <QDir>

#include "Configuration/FIMTrack.hpp"
#include "Backgroundsubtractor.hpp"
#include "Preprocessor.hpp"
#include "Data/RawLarva.hpp"
#include "Data/Larva.hpp"
#include "OutputGenerator.hpp"
#include "Logger.hpp"
#include "Undistorter.hpp"
#include "LarvaeContainer.hpp"
#include "GUI/RegionOfInterestContainer.hpp"
#include "Algorithm/Hungarian.hpp"

/**
 * @brief The Tracker class contains all necessary steps for tracking. Tracking is done in two major steps:
 * 1. for a current frame, all raw larva objects are calculated
 * 2. the raw larva objects are assigned to existing larva objects
 *
 * The main tracking is done in the method track(...).
 *
 * Since the Tracker class is able to send a signal to the main gui, it is a subclass of QObject.
 */
class Tracker : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Tracker is the main constructor for the tracker class
     * @param parent is the parent frame for the default constructor (must be 0)
     */
    Tracker(QObject *parent = 0);

//SIGNALS send to the main gui
signals:
    /**
     * @brief previewTrackingImageSignal is send to the main gui to induce the gui to refresh with the current image
     * @param img is a pointer to an image (must be generated in the main gui)
     */
    void previewTrackingImageSignal(cv::Mat img);

    void progressBarChangeSignal(int value);

    /**
     * @brief trackingDoneSignal is send to the main gui if all images are processed to reset the disabled
     *          buttons in the main gui
     */
    void trackingDoneSignal();

    void logMessageSignal(QString, LOGLEVEL);

// SLOTS activated by signals from the main gui
public slots:
    
    /**
     * @brief startTrackingSlot is used to start the tracking routine. Since the Tracker is started in a QThread, this slot must be invoked after starting the thread.
     * @param imgPaths the path containing the images
     * @param showProgress is true if the preview image shall be displayed in the main gui
     * @param previewImg is a pointer to an image for the tracking preview
     */
    void startTrackingSlot(std::vector<std::vector<std::string> > const & _multiImgPaths, bool showProgress, Undistorter const & undist, RegionOfInterestContainer const* ROIContainer = nullptr);

    /**
     * @brief invoked when stop button in main gui was pushed. indicates that tracking should stop after currently tracked image.
     */
    void stopTrackingSlot();


private:
    // MEMBER VARIABLES:  
    /**
     * @brief curRawLarvae stores the current raw larvae objects
     */
    std::vector<RawLarva> _curRawLarvae;
    /**
     * @brief larvae stores the larvae objects, which are generated and assigned from the raw larvae objects
     */
    LarvaeContainer _larvaeContainer;

    unsigned int _larvaID;

    /**
     * @brief imgPaths stores the path where to find the images
     */
    std::vector<std::vector<std::string> > _multiImgPaths;

    /**
     * @brief showTrackingProgress is used to activate / inactivate the live tracking preview in the main gui
     */
    bool _showTrackingProgress;

    /**
     * @brief indicates if stop button in main gui was pressed
     */
    bool _stopTracking;

    // MEMBER FUNCTIONS:
    /**
     * @brief track is the main function of the tracker class. It manages both, raw larvae extraction and the assignment of the current
     *          raw larvae to the existing (non-raw) larval objects.
     *
     * 1. extract raw larvae using extractRawLarvae member function
     * 2. assing current raw larvae to larvae using the assing member function
     *
     * @param bs is a background subtractor object
     * @param previewImg a pointer to an image for live tracking preview in the main gui
     * @return number of processed images (timepoint)
     */
    uint track(std::vector<std::string> const &imgPaths, Backgroundsubtractor const & bs, Undistorter const & undist, const RegionOfInterestContainer *ROIContainer = nullptr);

    /**
     * @brief extractRawLarvae extractes the raw larvae objects from the images.
     *
     * 1. current image is loaded (given by imgPath)
     * 2. backgroundsubtraction is applied to the image (background image given by the background (bs) object)
     * 3. contours are calculated using the Preprocessor
     * 3. current raw larvae are extracted from the contours and stored in the curRawLarvae member variable
     *
     * @param img current image
     * @param bs a Backgoundsubtractor object containing the background image
     * @param previewImage is a pointer to the preview image of the main gui to draw the contours
     * @param checkRoiBorders indicates if RegionOfInterest was selected (thus contours must be fully within this region to be valid)
     */
    void extractRawLarvae(const cv::Mat &img, Backgroundsubtractor const & bs, cv::Mat *previewImg, bool checkRoiBorders);
    
    /**
     * @brief assignByHungarian assigns larvae by minimizing overall cost (or maximizing overall utility) using the hungarian algorithm.
     *
     * @param timePoint current timepoint
     */
    void assignByHungarian(unsigned int timePoint);

    /**
     * @brief assignByGreedy assigns larvae by selecting the first accurate match for a raw larva.
     *
     * @param timePoint current timepoint
     */
    void assignByGreedy(unsigned int timePoint);
   
    /// helper for hungarian assignment function
    bool larvaHasPointInRawLarva(unsigned int const timePoint, Larva const & larva, RawLarva const& rawLarva);

};

#endif // TRACKER_HPP
