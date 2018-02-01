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

#ifndef TRACKERGRAPHICSVIEW_HPP
#define TRACKERGRAPHICSVIEW_HPP

#include <QGraphicsView>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QMenu>

#include "TrackerScene.hpp"
#include "Landmark.hpp"
#include "RegionOfInterestContainer.hpp"

class TrackerGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    TrackerGraphicsView(QWidget *parent = 0);
    TrackerGraphicsView(TrackerScene *scene, QWidget *parent = 0);
    
    ~TrackerGraphicsView();
    
    /**
    * @return Scene displayed in this view.
    */
    TrackerScene* getScene();

    /**
    * Sets the scale (zooming factor) for the scene.
    *
    * @param scale	New scale this scene shall be
    *				scaled to.
    */
    void setScale(const qreal scale);
    
    void enableContextMenu(bool enable);
    void enableLandmarkContextMenu();
    void enableRIOContextMenu();
    
protected:
    /**
    * Event for scrolling/scaling.
    */
    void wheelEvent(QWheelEvent *event);
    
    void contextMenuEvent(QContextMenuEvent *event);
    
private:
    
    bool                                    mContextMenuEnabled;
    bool                                    mContextMenuLandmarkEnabled;
    bool                                    mContextMenuROIEnabled;
    
    QString                                 mCurrentJobName;
    
    /**
    * Contextmenu of this view.
    */
    QMenu                                   mContextMenu;
    QMenu*                                  mLandmarkMenu;
    QMenu*                                  mROIMenu;
    QAction*                                mActionROI;
    QAction*                                mActionAddRectROI;
    QAction*                                mActionAddEllipseROI;
    QAction*                                mActionRemoveROI;
    QAction*                                mActionLandmark;
    QAction*                                mActionPointLandmark;
    QAction*                                mActionLineLandmark;
    QAction*                                mActionRectLandmark;
    QAction*                                mActionEllipseLandmark;
    QAction*                                mActionRemoveLandmark;
    
    QPointF                                 mLastContextMenuPosition;
    
    /**
    * Scene displayed by this view.
    */
    TrackerScene*                           mScene;
    
private slots:
    void addPointLandmark();
    void addLineLandmark();
    void addRectLandmark();
    void addEllipseLandmark();
    void removeLandmark();
    
    void addRectROI();
    void addEllipseROI();
    void removeROI();
    
public slots:
    void setJobName(QString const& jobName);
};

#endif // TRACKERGRAPHICSVIEW_HPP
