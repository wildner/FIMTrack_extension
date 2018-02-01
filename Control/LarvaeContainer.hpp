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

#ifndef LARVAECONTAINER_HPP
#define LARVAECONTAINER_HPP

#include "Configuration/FIMTrack.hpp"

//#include <QtCore>
#include <QFileDialog>
#include <list>

#include "Data/Larva.hpp"
#include "InputGenerator.hpp"
#include "OutputGenerator.hpp"
#include "GUI/TrackerScene.hpp"
#include "GUI/TrackerSceneLarva.hpp"
#include "GUI/LandmarkContainer.hpp"

class LarvaeContainer : public QObject
{
    Q_OBJECT
    
private:
    std::vector < Larva >                       mLarvae;
    double                                      mMaxSpineLength;
    int                                         mMaximumNumberOfTimePoints;
    
    uint getLastValidLavaID() const;
    
    double calcMomentumDist(const uint larvaIndex, 
                            const uint timePoint, 
                            cv::Point const & curMomentum) const;
    
    void changeDirection(const uint larvaIndex, const uint timePoint);
    void eraseAt(const uint larvaIndex, const uint timePoint);
    
    double calcDistToOrigin(const uint larvaIndex, 
                            cv::Point const & curMomentum) const;

    int calcGoPhaseIndicator(const uint larvaIndex,
                             uint const timePoint,
                             cv::Point const & curMomentum,
                             double const curBending,
                             uint const velocityThresh,
                             uint const bendingThresh,
                             uint const timeWindow);
    double calcMovementDirection(const uint larvaIndex, 
                                 uint const timePoint,
                                 uint const timeWindow,
                                 cv::Point const & curMomentum);
    int calcHeadTailRecognitionIndicator1(const uint larvaIndex,
                               const uint from,
                               const uint to);
    int calcHeadTailRecognitionIndicator2(const uint larvaIndex,
                                          const uint from,
                                          const uint to);

    bool changeDirectionality(const uint larvaIndex, const uint timePoint, RawLarva const & rawlarva);
    template<class T> std::vector<T> reverseVec(std::vector<T> const & v);
    
    
    bool getIndexOfLarva(const uint id, size_t &index) const;
    FIMTypes::spine_t calcSpine(QPainterPath const& spinePath);
    
    void updateLarvaSpine(const int index, const uint time, QPainterPath const& paintSpine, const std::vector<double> &radii);
    void updateLarvaMomentum(const int index, const uint time, QPolygonF const& paintPolygon);
    void updateLarvaArea(const int index, const uint time, QPolygonF const& paintPolygon);
    void updateLarvaPerimeter(const int index, const uint time, QPolygonF const& paintPolygon);
    void updateLarvaDistance2Origin(const int index);
    void updateLarvaAccumulatedDistance(const int index);
    void updateIsCoiledIndicator(const int index, const uint time);
    
    void updateGoPhaseIndicator(const int index, const uint time);
    void updateTurnIndicator(const int index, const uint time);
    bool calcLeftTurnIndicator(const double curBending, const uint bendingThresh);
    bool calcRightTurnIndicator(const double curBending, const uint bendingThresh);
    void updateMovementDirection(const int index, const uint time);
    
    void recalculateLarvaDistanceParameter(const uint larvaID);
    void recalculateLarvaDistanceToOrigin(const size_t larvaIndex);
    void recalculateLarvaMomentumDistance(const size_t larvaIndex);
    void recalculateLarvaVelocityAndAcceleration(const size_t larvaIndex);
    void recalculateLarvaAcceleration(const size_t larvaIndex);
    
    void calcLandmarkParameter(QString const& name, QPointF const& p);
    void calcLandmarkParameter(QString const& name, QLineF const& l);
    void calcLandmarkParameter(QString const& name, QRectF const& r, const bool ellipse);
    void calcBearingAngle(QString const& name, QPointF const& landmarkPoint);
    void calcBearingAngle(const QString &name, const QLineF &l);
    
public:
    explicit LarvaeContainer(QObject *parent = 0);
    
    Larva* createDefaultLarva(const uint timeStep);
    
    /**
     * @brief createNewLarva function to initialize a larva with a given time point.
     *
     *  This function is used during tracking to initialize new larval objects
     *
     * @param timePoint specifies the first detection for this larva
     * @param rawLarva contains the uprocessed raw larva and is used to calculate all larval parameters
     * @param larvaID specifies the unique ID of this larval object
     */
    void createNewLarva(const uint timePoint, RawLarva const & rawLarva, unsigned int larvaID);
    
    /**
     * @brief insert is used to add new measurements (given by rawLarva) to this larva at a given time point
     * @param timePoint the given time point (for the parameters map)
     * @param rawLarva contains all raw larval values (i.e. features; e.g. the contour).
     */
    void insertRawLarva(const uint larvaID, 
                        const uint timePoint, 
                        RawLarva const & rawLarva);
    
    /**
     * @brief interpolateHeadTailOverTime changes the head/tail classification if necessary
     *
     * This is a post-processing step, which is called after the whole video is processed.
     */
    void interpolateHeadTailOverTime(const uint larvaIndex);
    void interpolateHeadTailOverTime();
    
    /**
     * @brief fillTimeSamplingGaps some features are impossible to calculate online for all time steps,
     * which leads to sampling gaps (see below). These gaps are filled by this function.
     *
     * This is a post-processing step, which is called after the whole video is processed.
     * Sampling gaps are caused if frame rates lower than the actual frame rate of the movie
     * are used to calculate features (e.g. for the movement direction, 10fps are to fast to
     * recognice actual movement. However 1 second is sufficient to capture movement, thus the
     * movement direction can be calculated by resampling, i.e. calculate the direction between
     * the current frame and 10 frames in the past, which leads to gaps in the beginning of the
     * movie).
     */
    void fillTimeSamplingGaps(const uint larvaIndex);
    void fillTimeSamplingGaps();
    void updateLarvaParameterAfterHeadTailInterpolation();
    
    /**
     * @brief isAssignedAt returns true if this larva is assigned for the given time point
     * @param timePoint time point to check existence of this larva
     * @return true if the larva exists for this time point, false otherwise
     */
    bool isAssignedAt(const uint larvaID, const uint timePoint) const;
    
    void interplolateLarvae();
    
    void readLarvae(QString const& ymlFileName, 
                    std::vector<std::string> &imgPaths, 
                    bool useUndist);
    
    QPair<QVector<uint>, QVector<uint> > getVisibleLarvaID(uint time);
    
    QStringList getAllTimestepsBefore(const uint id, const uint time);
    QStringList getAllTimestepsAfter(const uint id, const uint time);
    QStringList getAllContemplableLarvaeIDsForAttach(const uint id);
    
    void invertLarva(const uint larvaID, const uint currentTime, const uint toTime);
    
    void attachToLarva(const uint toLarvaID, const uint fromLarvaID);
    
    bool copyModel2PrevTimeStep(const uint larvaID, const uint currentTime);
    bool copyModel2NextTimeStep(const uint larvaID, const uint currentTime);
    
    bool eraseLarvaAt(const uint larvaID, const uint time);
    bool eraseLarva(const uint larvaID);
    
    void saveResultLarvae(const std::vector<std::string> &imgPaths, 
                          QImage const& img,
                          const bool useUndist, 
                          RegionOfInterestContainer const* ROIContainer = NULL,
                          LandmarkContainer const* landmarkContainer = NULL);

    void processUntrackedLarvae(const uint timePoint);
    
    /// Getter
    std::vector< Larva > getAllLarvae() const {return this->mLarvae;}
    QStringList getAllLarvaeIDs() const;
    int getNumberOfLarvae() const {return this->mLarvae.size();}
    Larva* getLarvaPointer(const unsigned int index)
    {
        if(index < this->mLarvae.size())
        {
            return &(this->mLarvae.at(index));
        }
        return NULL;
    }
    
    double getMaxSpineLength() const {return this->mMaxSpineLength;}
    
    bool hasLoadedLarvae() const {return !this->mLarvae.empty();}
    bool isEmpty() const  {return this->mLarvae.empty();}
    bool getLarva(const uint index, Larva& l);
    bool getLarvaByID(uint larvaID, Larva& l);
    
    std::vector<uint>   getAllTimesteps(const uint larvaID);
    QPair<int, int>     getStartEndTimesteps(const uint larvaID);
    std::vector< int >  getAllValidLarvaeIDS(const uint timePoint);
    
    bool larvaHasPointInContour(uint const timePoint, const uint larvaID, FIMTypes::contour_t const & rawLarvaContour);
    
    /**************** GETTER **********************/
    bool getSpineMidPointIndex(const uint larvaID, uint &index) const;
    bool getSpinePointAt(const uint larvaID, const uint timePoint, const uint index, cv::Point &spinePoint) const;
    bool getSpineAt(const uint larvaID, const uint timePoint, FIMTypes::spine_t &spine) const;
    bool getAccDistAt(const uint larvaID, const uint timePoint, double & retAccDist);
    bool getDistToOriginAt(const uint larvaID, const uint timePoint, double & retDistToOrigin);
    bool getIsCoiledIndicatorAt(const uint larvaID, uint const timePoint, bool & retIsCoiledIndicator);
    bool getIsWellOrientedAt(const uint larvaID, uint const timePoint, bool & retIsWellOriented);
    bool getMomentumAt(const uint larvaID, const uint timePoint, cv::Point & retMomentum) const;
    bool getAreaAt(const uint larvaID, const uint timePoint, double & retArea) const;
    bool getSpineLengthAt(const uint larvaID, const uint timePoint, double & retSpineLength) const;
    bool getContour(const uint larvaID, FIMTypes::contour_t& contour) const;
    
    /***************** For Plotting **********************/
    QVector<cv::Point>          getAllMomentumValues(const uint larvaID);
    QVector<double>             getAllAreaValues(const uint larvaID);
    QVector<double>             getAllMainBodybendingAngle(const uint larvaID);
    QVector<double>             getAllCoiledIndicator(const uint larvaID);
    QVector<double>             getAllIsWellOriented(const uint larvaID);
    QVector<double>             getAllPerimeter(const uint larvaID);
    QVector<double>             getAllDistanceToOrigin(const uint larvaID);
    QVector<double>             getAllMomentumDistance(const uint larvaID);
    QVector<double>             getAllAccumulatedDistance(const uint larvaID);
    QVector<double>             getAllGoPhaseIndicator(const uint larvaID);
    QVector<double>             getAllLeftBendingIndicator(const uint larvaID);
    QVector<double>             getAllRightBendingIndicator(const uint larvaID);
    QVector<double>             getAllMovementDirection(const uint larvaID);
    QVector<double>             getAllDistancesToLandmark(const uint larvaID, QString const& landmarkID);
    QVector<double>             getAllBearingAnglesToLandmark(const uint larvaID, QString const& landmarkID);
    QVector<double>             getVelocity(const uint larvaID);
    QVector<double>             getAcceleration(const uint larvaID);
    
    QVector<double>             getAllTimestepsForPlotting(const uint larvaID);
    QVector<QVector<double> >   getAllTimestepGaps(const uint larvaID);
    
signals:
    void sendLarvaModelDeleted();
    
    void sendUpdatedResultLarvaID(uint);
    void sendRemovedResultLarvaID(uint);
    void reset();
    
public slots:
    void updateLarvaValues(TrackerSceneLarva const* tLarva);
    void removeAllLarvae();
    
    void updateLandmark(const Landmark *l);
    void removeLandmark(const QString name);
    
    void removeShortTracks(const uint minTrackLenght);
    
    void setMaximumNumberOfTimePoints(const int maxTimePoints);
    
};

#endif // LARVAECONTAINER_HPP
