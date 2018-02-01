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

#ifndef LARVA_HPP
#define LARVA_HPP

#include "Configuration/FIMTrack.hpp"

#include <vector>
#include <map>
#include <iterator>
#include <cmath>

#include "RawLarva.hpp"


/**
 * @brief The Larva class is the main class to store all larval parameters
 *
 * In FIMTrack, the Larva Object is one of the most important objects, which stores all calculated features.
 * Larval objects are calculated in basically three steps:
 * 1) In the first step, raw larval objects (i.e. RawLarva) are generated from the images by calculating the
 * contour and all contour related parameters (like the covered area or the curvyness of the contour; see
 * RawLarva class). Note that a RawLarva object is not associated with existing larvae.
 * 2) In a second step, all RawLarva objects are assigned to existing Larva objects (or a new Larva object is
 * created). This step is done in the Tracker Class.
 * 3) If the RawLarva is assigned to an existing Larva object, all measurements from the RawLarva must be
 * assigned to this Larva object (see Larva::insert(...)). Furthermore, time-related parameters must becalculated.
 * This is all done in the third step.
 *
 * Basic structure of the Larva class:
 * The main parameter is the parameters map. This map associates time points with measurements with values
 * (i.e. features). All values are collected in the ValuesStruct, thus the paremeters map is defined by:
 * parameters: (time point) -> (values)
 *
 * To avoid access to values at time points where the larva do not exists, getter methods are used for access.
 * The return value of these getter methods is always a boolean value (true if the parameter exists and false
 * otherwise). If the value exists, it is stored in the reference, given as a parameter in the specific getter
 * method.
 *
 * The Larva class provides both: storage of the features and calculation (interpolation) of some larval related
 * features. Thus, it contains getter- and setter-methods and methods for feature calculation.
 */
class Larva
{
public:
    
    /**
     * @brief Larva empty constructor mainly used to load larval objects for the input generator
     *
     *  To load larval objects from a tracking results (i.e. output) file, empty larval objects are generated
     *  (using this constructor) and subsequently filled with values (using <<-operator in Larva.cpp).
     */
    Larva();
    
    /**
     * @brief The ValuesType struct is the basic struct storing all measurements (i.e. features)
     *          for a single time point
     */
    struct ValuesType {
        /**
         * @brief spine of the larva
         */
        FIMTypes::spine_t spine;
        /**
         * @brief momentum point of the larval contour
         */
        cv::Point momentum;
        /**
         * @brief area covered area of the larval contour
         */
        double area;
        /**
         * @brief spineRadii radii of all spine points (specifying the thickness of the larva)
         */
        std::vector<double> spineRadii;
        /**
         * @brief mainBodyBendingAngle the main body bending angle (calculated on the first, middle and last spine point)
         */
        double mainBodyBendingAngle;
        /**
         * @brief spineLength overall length of the spine
         */
        double spineLength;
        /**
         * @brief perimeter of the larval contour
         */
        double perimeter;
        /**
         * @brief isCoiled if the larva is in a coiled structure (i.e. if the larval contour is approximatly a circle) this value is set to 1 (0 otherwise)
         */
        bool isCoiled;
        /**
         * @brief isWellOriented if the larval orientation could be verified (i.e. head and tail detection successfully performed)
         */
        bool isWellOriented;
        /**
         * @brief distToOrigin the distance to the origin (i.e. first measured position; based on the momentum)
         */
        double distToOrigin;
        /**
         * @brief momentumDist the distance between consecutive momentum points
         */
        double momentumDist;
        /**
         * @brief accDist the total accumulated distance (based on the momentum)
         */
        double accDist;
        /**
         * @brief goPhase indicator to specify the current phase (go or stop phase)
         *
         *  1 = go phase; 0 = stop phase; -1 = impossible to calculate
         */
        int goPhase;
        /**
         * @brief isLeftBended indicator to mark right bended events (1=true)
         */
        bool isLeftBended;
        /**
         * @brief isRightBended indicator to mark right bended events (1=true)
         */
        bool isRightBended;
        /**
         * @brief movementDirection movement direction relative to the y-axis (calculated using consecutive momentum points)
         *
         *  Possible values (i.e. movement to the top,bottom,left,right):
         *  -1 = impossible to calculate;
         *  0 or 360 = top;
         *  90 = right;
         *  180 = bottom;
         *  270 = left
         */
        double movementDirection;
        /**
         * @brief velosity
         */
        double velosity;
        /**
         * @brief acceleration
         */
        double acceleration;
        /**
         * @brief distanceToLandmark
         */
        std::map<std::string, double>   distanceToLandmark;
        
        /**
         * @brief isInLandmark
         */
        std::map<std::string, bool>     isInLandmark;
        
        /**
         * @brief isInLandmark
         */
        std::map<std::string, double>     bearinAngle;
        
    } values;
    
    // Parameters:
    
    /**
     * @brief parameters map all time points to the values (i.e. features) for this time point
     *
     * In general, all parameters (for a given time point) are accessable via getter methods
     * to catch access to features at time points in which the larva do not exist (see getter
     * methods below).
     */
    std::map<unsigned int, ValuesType> parameters;
    
    /**
     * @brief contour is the last detected contour of this larva
     */
    FIMTypes::contour_t contour;
    
    /**
     * @brief operator << is overloaded to store larva into files (i.e. fs << larva)
     * @param fs the file storage in which the (whole larva object) should be stored
     * @param larva the larva object to be stored
     * @return returns the file storage object
     */
    friend cv::FileStorage& operator<<(cv::FileStorage& fs, Larva const& larva);
    
    /**
     * @brief operator >> is overloaded to read all larval parameters from a FileNode (i.e. the
     *          yaml-representation file storage representation of a larval object)
     * @param n the file node which is read from a file storage object (i.e. yaml-file)
     * @param larva the larva object to be filled with the data from the file node
     */
    friend void operator>>(cv::FileNode const& n, Larva& larva);
    
    /**
     * @brief nSpinePoints stores the number of spine points for descrete spine representation
     */
    unsigned int nSpinePoints;
    
    /**
     * @brief origin is the point in which the larva has been detected for the first time point
     */
    cv::Point origin;
    
    /**
     * @brief id specifies the unique larval ID
     */
    unsigned int id;
    
    // setter methods
    /**
     * @brief setID sets the ID of the larva (used for loading larvae from files)
     * @param id the ID of the larva
     */
    void setID(unsigned int id) {this->id = id;}
    
    /**
     * @brief setParameters sets the parameters map for this larva (used for loading larvae from files)
     * @param parameters the parameters map
     */
    void setParameters(std::map<unsigned int, ValuesType> const & parameters);
    /**
     * @brief setNSpinePoints sets the number of spine points parameter (used for loading larvae form files)
     * @param nSpinePoints the number of spine poits used for the discretized spine
     */
    void setNSpinePoints(unsigned int nSpinePoints) {this->nSpinePoints = nSpinePoints;}
    
    void setOrigin(cv::Point const& p) {this->origin = p;}
    
    bool setSpineAt(unsigned int const timePoint, FIMTypes::spine_t const& spine);
    
    // getter methods
    unsigned int getNSpinePoints() const {return nSpinePoints;}
    
    unsigned int getSpineMidPointIndex(void) const {return ((unsigned int) (nSpinePoints - 1) /2);}
    
    unsigned int getID (void) const {return id;}
    
    std::vector<cv::Point> getAllMidPoints(void) const;
    std::vector<cv::Point> getAllHeadPoints(void) const;
    std::vector<cv::Point> getAllTailPoints(void) const;
    
    std::vector<unsigned int> getAllTimeSteps(void) const;
    
    void invert(uint time);
    
    cv::Point getOrigin() const {return this->origin;}
    bool getSpineAt(unsigned int const timePoint, FIMTypes::spine_t & retSpine) const;
    bool getSpineMidPointAt(unsigned int const timePoint, cv::Point & retSpineMidPoint) const;
    bool getMomentumAt(unsigned int const timePoint, cv::Point & retMomentum) const;
    bool getHeadAt(unsigned int const timePoint, cv::Point & retHead) const;
    bool getTailAt(unsigned int const timePoint, cv::Point & retTail) const;
    bool getAreaAt(unsigned int const timePoint, double & retArea) const;
    bool getVelosityAt(unsigned int const timePoint, double & retVelosity) const;
    bool getAccelerationAt(unsigned int const timePoint, double & retAcceleration) const;
    bool getSpineRadiiAt(unsigned int const timePoint, std::vector<double> & retSpineRadii) const;
    bool getMainBodyBendingAngleAt(unsigned int const timePoint, double & retMainBodyBendingAngle) const;
    bool getIsCoiledIndicatorAt(unsigned int const timePoint, bool & retIsCoiledIndicator) const;
    bool getIsWellOrientedAt(unsigned int const timePoint, bool & retIsWellOriented) const;
    bool getSpineLengthAt(unsigned int const timePoint, double & retSpineLength) const;
    bool getPerimeterAt(unsigned int const timePoint, double & retPerimeter) const;
    bool getDistToOriginAt(unsigned int const timePoint, double & retDistToOrigin) const;
    bool getMomentumDistAt(unsigned int const timePoint, double & retMomentumDist) const;
    bool getAccDistAt(unsigned int const timePoint, double & retAccDist) const;
    bool getGoPhaseIndicatorAt(unsigned int const timePoint, int & retGoPhaseIndicator) const;
    bool getLeftBendingIndicatorAt(unsigned int const timePoint, bool & retLeftBended) const;
    bool getRightBendingIndicatorAt(unsigned int const timePoint, bool & retRightBended) const;
    bool getMovementDirectionAt(unsigned int const timePoint, double & retMovementDirection) const;
    bool getDistanceToLandmark(unsigned int const timePoint, std::string landmarkName, double & retDistanceToLandmark) const;
    bool getBearingAngleToLandmark(unsigned int const timePoint, std::string landmarkName, double & retBearingAngleToLandmark) const;
    bool getIsInLandmarkIndicator(unsigned int const timePoint, std::string landmarkName, bool & retIsInLandmark) const;
    bool getSpinePointAt(unsigned int const timePoint, unsigned int const index, cv::Point & spinePoint) const;
    
    // string getter methods
    std::string getStrMomentum(unsigned int const timePoint, unsigned int const dimension) const;
    std::string getStrArea(unsigned int const timePoint) const;
    std::string getStrVelosity(unsigned int const timePoint) const;
    std::string getStrAcceleration(unsigned int const timePoint) const;
    std::string getStrSpine(unsigned int const timePoint, unsigned int const index, unsigned int const dimension) const;
    std::string getStrSpineRadius(unsigned int const timePoint, unsigned int const index) const;
    std::string getStrMainBodyBendingAngle(unsigned int const timePoint) const;
    std::string getStrIsCoiledIndicator(unsigned int const timePoint) const;
    std::string getStrIsWellOriented(unsigned int const timePoint) const;
    std::string getStrSpineLength(unsigned int const timePoint) const;
    std::string getStrPerimeter(unsigned int const timePoint) const;
    std::string getStrDistToOrigin(unsigned int const timePoint) const;
    std::string getStrMomentumDist(unsigned int const timePoint) const;
    std::string getStrAccDist(unsigned int const timePoint) const;
    std::string getStrGoPhaseIndicator(unsigned int const timePoint) const;
    std::string getStrLeftBendingIndicator(unsigned int const timePoint) const;
    std::string getStrRightBendingIndicator(unsigned int const timePoint) const;
    std::string getStrMovementDirection(unsigned int const timePoint) const;
    std::string getStrDistanceToLandmark(unsigned int const timePoint, std::string landmarkName) const;
    std::string getStrIsInLandmark(unsigned int const timePoint, std::string landmarkName) const;
    std::string getStrBearingAngleToLandmark(unsigned int const timePoint, std::string landmarkName) const;
    
private:
    int getXorY(cv::Point const & pt, unsigned int dimension) const;
};

#endif // LARVA_HPP
