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

#ifndef RESULTSVIEWERLARVAPARAMETER_HPP
#define RESULTSVIEWERLARVAPARAMETER_HPP

#include <QWidget>
#include <QTimer>
#include <QMessageBox>

#include "Data/Larva.hpp"
#include "Control/LarvaeContainer.hpp"
#include "TrackerSceneLarva.hpp"
#include "InvertDialog.hpp"
#include "UniteDialog.hpp"

namespace Ui {
    class LarvaTab;
}

class LarvaTab : public QWidget
{
    Q_OBJECT
    
public:
    explicit LarvaTab(TrackerSceneLarva *sceneLarva, 
                      unsigned int maxFrames, 
                      LarvaeContainer* larvaeContainer,
                      QWidget *parent = 0);
    ~LarvaTab();
    
    /**
    * Getter for visualization of this components larva
    *
    * @return	Visualization object
    */
    TrackerSceneLarva* getSceneLarva() const;
    
    
private:
    Ui::LarvaTab*                           ui;
    
    LarvaeContainer*                        mLarvaeContainer;
    
    QColor									mColor;
    
    InvertDialog*                           mInvertDialog;
    UniteDialog*                            mUniteDialog;
    
    TrackerSceneLarva*                      mSceneLarva;
    unsigned int							mCurrentTime;
    unsigned int                            mMaxTime;
    
    
    /**
    * Timer allows flashing of this Larva.
    */
    QTimer*                                 mFlashTimer;
    
    /**
    * Number of times the scene larva will flash.
    */
    unsigned int							mFlashCount;
    
    /**
    * True, if larva was modified by user.
    */
    bool									mModified;
    
    /**
    * Checks wether the timeindex for this model may be changed.
    * Might not be allowed in case of unsaved changes to this model.
    *
    * @return	True, if timechange is allowed.
    */
    bool canChangeTime();
    
    /**
    * Update min/max-Time
    */
    void updateTimes();
    
    /**
    * Changed this larvas color in the scene.
    *
    * @param	color	Color the visualization of this model shall have.
    */
    void setColor(QColor color);
    
signals:
    void changedLarva(TrackerSceneLarva const* sceneLarva);
    void deleteResultsLarva(TrackerSceneLarva* sceneLarva);
    void requestCenterOn(qreal x, qreal y);
    
    void sendNewColor(QColor color);
    
    void sendAmbiguityTimepoint(int time);
    
public slots:
    void setTime(unsigned int time);
    void update(unsigned int time);	
    
    void redrawPaths();
    
    void setLarvaPointer(Larva* pLarva) {this->mSceneLarva->setLarvaPointer(pLarva);}
    
private slots:
    void setupConnections();    
    void centerSceneOnLarva();
    void updateColor();	
    void setTimeToPrevAmbiguity();
    void setTimeToNextAmbiguity();
    
    void handleInvertParameter(bool before, uint bTime, bool current, bool after, uint aTime);
    void handleAttachParameter(uint contemplableLarvaeID);
    
    void showLarvaWasChanged();
    
    void updateSpinBoxRadiusPosition(uint maxValue);
    
    QVector< QPair<uint, uint> > findTimeStepGaps(std::vector<uint> const& timeSteps);
    bool findNextTimeGap(const QVector<QPair<uint, uint> > &timeGaps, int &gapIndex);
    
    /**
    *	Radius modification via left-hand-gui
    **/
    void updateDisplayedRadius(int index);
    void applyRadiusToSceneLarva(double	radius);
    
    /**
    *	Unite model with different model from results (add before/after all timesteps of this model)
    **/
    void attachOtherModel();
    
    /**
    *	Delete Larva from results
    **/
    void deleteLarva();
    
    /**
    *	Delete Model of current TimeStep from results
    **/
    void deleteCurrentModel();
    
    /**
    *	Reset larva after manual changes in scene
    **/
    void reset();
    
    /**
    *	Apply manual changes in scene
    **/
    void applyChanges();
    
    /**
    *	Apply manual changes in scene
    **/
    void showInvertLarvaDialog();
    
    /**
    *	copy current larva-data to previous/next timestep
    **/
    void copyToPrevTimestep();
    void copyToNextTimestep();
    bool canCopyModel(bool prev = true);
    
    /**
    *	Visualization visibilities
    **/
    void displaySceneLarva(bool visible);
    void displaySceneLarvaPath(bool visible);
    void displaySceneLarvaDistance2Origin(bool visible);
    
    /**
    *	flashing of visualization for easier recognition.
    **/
    void flash();
    void flashEvent();
};

#endif // RESULTSVIEWERLARVAPARAMETER_HPP
