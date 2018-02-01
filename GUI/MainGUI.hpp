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

#ifndef MAINGUI_HPP
#define MAINGUI_HPP

#include <QMainWindow>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QPixmap>
#include <QThread>
#include <QFileInfo>
#include <QtCore>
#include <QtGui>

#include "GUI/PreferencesDialogWindow.hpp"
#include "GUI/LOGWindow.hpp"
#include "GUI/ResultsViewer.hpp"

#include "Configuration/FIMTrack.hpp"

#include "Control/Undistorter.hpp"
#include "Control/Backgroundsubtractor.hpp"
#include "Control/Preprocessor.hpp"
#include "Control/InputGenerator.hpp"
#include "Control/OutputGenerator.hpp"
#include "Control/Tracker.hpp"

#include "Data/RawLarva.hpp"

#include "TrackerScene.hpp"

namespace Ui {
    class MainGUI;
}

class MainGUI : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainGUI(QWidget *parent = 0);    
    ~MainGUI();

signals:
    void startTrackingSignal(std::vector<std::vector<std::string> > const &, bool, Undistorter const &, RegionOfInterestContainer const* = nullptr);
    void stopTrackingSignal();
    void logMessage(QString, LOGLEVEL);
    void loadCameraParameter(QString);
    void updatePreferenceParameter();
    void setBackgroundSpinboxesValues(unsigned int);
    
private slots:
    void on_btnLoad_clicked();   
    void on_btnReset_clicked();
    void on_btnTrack_clicked();
    void on_btnPreview_clicked();
    void on_actionPreferences_triggered();
    void on_actionOpen_LOG_Window_triggered();
    void on_progressBar_valueChanged(int value);
    void on_actionLoad_Camera_Setting_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionOpen_triggered();
    void on_actionNew_triggered();
    void on_actionResults_Viewer_triggered();
    void on_btnDelete_clicked();
    void on_treeView_itemSelectionChanged();
    
    void previewTrackingImageSlot(cv::Mat img);
    void trackingDoneSlot();
    void initUndistorer(QString const& file);
    void updateTreeViewer();
    void resetListViewe();
    void showMessage(QString msg);

private:
    Ui::MainGUI*                ui;
    PreferencesDialogWindow*    _preferencesDialogWindow;
    LOGWindow*                  _logWindow;
    ResultsViewer*              _resultsViewer;
    TrackerScene*               _scene;

    QThread*                    _trackingThread;
    
    QStringList                 _fileNames;
    QList<QStringList>          _jobs;
    
    Undistorter                 _undistorter;

    Tracker                     _tracker;
    cv::Mat                     _trackingPreviewImg;
    
    QString                     _configurationFile;
    
    void showImage(QString path);
    void readParameters();
    
    void setupBaseGUIElements(bool enable);
};

#endif // MAINGUI_HPP
    

    

    
