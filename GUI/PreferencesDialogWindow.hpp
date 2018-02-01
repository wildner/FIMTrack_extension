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

#ifndef PREFERENCESDIALOGWINDOW_HPP
#define PREFERENCESDIALOGWINDOW_HPP

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>

#include "Configuration/FIMTrack.hpp"
#include "Control/Logger.hpp"

namespace Ui {
class PreferencesDialogWindow;
}

class PreferencesDialogWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit PreferencesDialogWindow(QWidget *parent = 0);
    ~PreferencesDialogWindow();
    
signals:
    void logMessage(QString, LOGLEVEL);
    void loadCameraParameter(QString);
    
public slots:
    void updateCameraParameterFile(QString file);
    bool checkParameters();
    void readParameters();
    void setParameters();
    void updateBackgroungSpinboxesValues(unsigned int iNumberOfFiles);
    
private slots:
    void on_btnLoadCameraParameter_clicked();
    void on_checkBoxBackgroundSubstractionUseDefault_clicked(bool checked);
    void on_checkBoxLarvaeExtactionParameterUseDefauilt_clicked(bool checked);
    void on_btnCancel_clicked();
    void on_btnOK_clicked();
    
    void on_spinBoxLarvaeExtactionParameterNumerOfSpinePoints_valueChanged(int arg1);
    
    void on_spinBoxIPANContourCurvatureCurvatureWindowSize_valueChanged(int arg1);
    
    void on_checkBoxLarvaeExtactionParametersUseDynamicIPAN_clicked(bool checked);

    void on_checkBoxLarvaeExtactionParametersUseDynamicStopAndGo_clicked(bool checked);

    void on_checkBoxLarvaeExtactionParametersUseDynamicBodyBending_clicked(bool checked);

    void on_doubleSpinBoxBodyBendingAngleThreshold_editingFinished();

    void on_checkBoxLarvaeExtactionParametersUseDynamicMovementDirection_clicked(bool checked);

    void on_comboBoxTrackingAssignmentMethod_currentIndexChanged(int index);

private:
    Ui::PreferencesDialogWindow *ui;
};

#endif // PREFERENCESDIALOGWINDOW_HPP
