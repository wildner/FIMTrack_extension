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

#include "PreferencesDialogWindow.hpp"
#include "ui_PreferencesDialogWindow.h"

PreferencesDialogWindow::PreferencesDialogWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialogWindow)
{
    ui->setupUi(this);
    connect(this, SIGNAL(logMessage(QString,LOGLEVEL)), Logger::getInstance(), SLOT(handleLogMessage(QString,LOGLEVEL)));
}

PreferencesDialogWindow::~PreferencesDialogWindow()
{
    delete ui;
}

void PreferencesDialogWindow::updateCameraParameterFile(QString file)
{
    this->ui->labelCameraParameter->setText(file);
}

bool PreferencesDialogWindow::checkParameters()
{
    bool result = true;
    
    if(this->ui->spinBoxBackgroundSubstractionToImage->value() > 0)
    {
        result = this->ui->spinBoxBackgroundSubstractionFromImage->value() < this->ui->spinBoxBackgroundSubstractionToImage->value();
    }
    
    if(!result)
    {
        QMessageBox::warning(this,"Warning", QString("Parameter \"From Image\" can't be greater as Parameter \"To Image\"\n\"From Image\"\t= ").append(QString::number(this->ui->spinBoxBackgroundSubstractionFromImage->value())).append("\n\"To Image\"\t= ").append(QString::number(this->ui->spinBoxBackgroundSubstractionToImage->value())), QMessageBox::Ok);
    }
    
    return result;
}

void PreferencesDialogWindow::on_btnLoadCameraParameter_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open Camera Parameter"), this->ui->labelCameraParameter->text(), tr("Camera Parameter File (*.yaml)"));
    if(!file.isNull() && !file.isEmpty())
    {
        this->ui->labelCameraParameter->setText(file);
        this->ui->doubleSpinBoxCameraParameterFSP->setEnabled(false);
        this->ui->doubleSpinBoxCameraParameterScaleFactor->setEnabled(false);
        emit loadCameraParameter(file);
    }
}

void PreferencesDialogWindow::on_checkBoxBackgroundSubstractionUseDefault_clicked(bool checked)
{
    this->ui->spinBoxBackgroundSubstractionFromImage->setEnabled(!checked);
    this->ui->spinBoxBackgroundSubstractionOffset->setEnabled(!checked);
    this->ui->spinBoxBackgroundSubstractionToImage->setEnabled(!checked);
}

void PreferencesDialogWindow::on_checkBoxLarvaeExtactionParameterUseDefauilt_clicked(bool checked)
{
    /* Larvae Extraction Parameters */
    this->ui->spinBoxLarvaeExtactionParameterNumerOfSpinePoints->setEnabled(!checked);
    
    /* Coiled Recognition Parameters */
    this->ui->doubleSpinBoxCoiledRecognitionParametersPerimeterToSpinelenghtThreshold->setEnabled(!checked);
    this->ui->doubleSpinBoxCoiledRecognitionParametersMidcirclePerimeterToPerimeterThreshold->setEnabled(!checked);
    this->ui->doubleSpinBoxCoiledRecognitionParametersMaxToMeanRadiusThreshold->setEnabled(!checked);
}

void PreferencesDialogWindow::on_btnCancel_clicked()
{
    this->close();
}

void PreferencesDialogWindow::on_btnOK_clicked()
{
    if(this->checkParameters())
    {
        this->readParameters();
        this->close();
    }
}

void PreferencesDialogWindow::readParameters()
{
    /* General Parameters */
    GeneralParameters::bSaveLog = this->ui->checkBoxSaveLog->isChecked();
    GeneralParameters::bEnableDetailetOutput = this->ui->checkBoxEnableDetailetOutput->isChecked();
    
    /* Camera Parameters */
    CameraParameter::File = this->ui->labelCameraParameter->text();
    CameraParameter::dFPS = this->ui->doubleSpinBoxCameraParameterFSP->value();
    CameraParameter::dScaleFactor = this->ui->doubleSpinBoxCameraParameterScaleFactor->value();
    
    /* Background Substraction Parameters */
    BackgroundSubstraction::bUseDefault = this->ui->checkBoxBackgroundSubstractionUseDefault->isChecked();
    BackgroundSubstraction::iFromImage = this->ui->spinBoxBackgroundSubstractionFromImage->value();
    BackgroundSubstraction::iOffset = this->ui->spinBoxBackgroundSubstractionOffset->value();
    BackgroundSubstraction::iToImage = this->ui->spinBoxBackgroundSubstractionToImage->value();
    
    /* Larvae Extraction Parameters */
    LarvaeExtractionParameters::bUseDefault = this->ui->checkBoxLarvaeExtactionParameterUseDefauilt->isChecked();
    LarvaeExtractionParameters::iNumerOfSpinePoints = this->ui->spinBoxLarvaeExtactionParameterNumerOfSpinePoints->value();

    /* IPAN Contour Curvature Parameters */
    LarvaeExtractionParameters::IPANContourCurvatureParameters::bUseDynamicIpanParameterCalculation = this->ui->checkBoxLarvaeExtactionParametersUseDynamicIPAN->isChecked();
    LarvaeExtractionParameters::IPANContourCurvatureParameters::iMinimalTriangelSideLenght = this->ui->spinBoxIPANContourCurvatureMinimalTriangelSideLenght->value();
    LarvaeExtractionParameters::IPANContourCurvatureParameters::iMaximalTriangelSideLenght = this->ui->spinBoxIPANContourCurvatureMaximalTriangelSideLenght->value();
    LarvaeExtractionParameters::IPANContourCurvatureParameters::dMaximalCurvaturePointsDistance = this->ui->doubleSpinBoxIPANContourCurvatureMaximalCurvaturePointsDistance->value();
    LarvaeExtractionParameters::IPANContourCurvatureParameters::iCurvatureWindowSize = this->ui->spinBoxIPANContourCurvatureCurvatureWindowSize->value();
    /* Coiled Recognition Parameters */
    LarvaeExtractionParameters::CoiledRecognitionParameters::dPerimeterToSpinelenghtThreshold = this->ui->doubleSpinBoxCoiledRecognitionParametersPerimeterToSpinelenghtThreshold->value();
    LarvaeExtractionParameters::CoiledRecognitionParameters::dMidcirclePerimeterToPerimeterThreshold = this->ui->doubleSpinBoxCoiledRecognitionParametersMidcirclePerimeterToPerimeterThreshold->value();
    LarvaeExtractionParameters::CoiledRecognitionParameters::dMaxToMeanRadiusThreshold = this->ui->doubleSpinBoxCoiledRecognitionParametersMaxToMeanRadiusThreshold->value();
    /* Stop and Go Phase Calculation Parameters */
    LarvaeExtractionParameters::StopAndGoCalculation::bUseDynamicStopAndGoParameterCalculation = this->ui->checkBoxLarvaeExtactionParametersUseDynamicStopAndGo->isChecked();
    LarvaeExtractionParameters::StopAndGoCalculation::iFramesForSpeedCalculation = this->ui->spinBoxStopAndGoFramesForSpeedCalc->value();
    LarvaeExtractionParameters::StopAndGoCalculation::iSpeedThreshold = this->ui->spinBoxStopAndGoSpeedThreshold->value();
    LarvaeExtractionParameters::StopAndGoCalculation::iAngleThreshold = this->ui->spinBoxStopAndGoAngleThreshold->value();
    /* Body Bending Calculation Parameters */
    LarvaeExtractionParameters::BodyBendingParameters::bUseDynamicBodyBendingCalculation = this->ui->checkBoxLarvaeExtactionParametersUseDynamicBodyBending->isChecked();
    LarvaeExtractionParameters::BodyBendingParameters::dAngleThreshold = this->ui->doubleSpinBoxBodyBendingAngleThreshold->value();
    /* Movement Direction Calculation Parameters */
    LarvaeExtractionParameters::MovementDirectionParameters::bUseDynamicMovementDirectionParameterCalculation = this->ui->checkBoxLarvaeExtactionParametersUseDynamicMovementDirection->isChecked();
    LarvaeExtractionParameters::MovementDirectionParameters::iFramesForMovementDirectionCalculation = this->ui->spinBoxMovementDirectionFramesForDirectionCalc->value();

    /* Tracking Assignment Parameters */
    LarvaeExtractionParameters::AssignmentParameters::eAssignmentMethod = static_cast<LarvaeExtractionParameters::AssignmentParameters::AssignmentMethod>(this->ui->comboBoxTrackingAssignmentMethod->currentIndex());
    LarvaeExtractionParameters::AssignmentParameters::eCostMeasure = static_cast<LarvaeExtractionParameters::AssignmentParameters::CostMeasure>(this->ui->comboBoxTrackingAssignmentCostMeasure->currentIndex());
    LarvaeExtractionParameters::AssignmentParameters::dDistanceThreshold = this->ui->doubleSpinBoxGreedyAssignmentThreshold->value();
    LarvaeExtractionParameters::AssignmentParameters::dOverlapThreshold = this->ui->doubleSpinBoxGreedyAssignmentThreshold->value();
}

void PreferencesDialogWindow::setParameters()
{
    /* General Parameters */
    this->ui->checkBoxSaveLog->setChecked(GeneralParameters::bSaveLog);
    this->ui->checkBoxEnableDetailetOutput->setChecked(GeneralParameters::bEnableDetailetOutput);
    
    /* Camera Parameters */
    this->ui->labelCameraParameter->setText(CameraParameter::File);
    this->ui->doubleSpinBoxCameraParameterFSP->setValue(CameraParameter::dFPS);
    this->ui->doubleSpinBoxCameraParameterScaleFactor->setValue(CameraParameter::dScaleFactor);
    
    this->ui->doubleSpinBoxCameraParameterFSP->setEnabled(CameraParameter::File.isEmpty());
    this->ui->doubleSpinBoxCameraParameterScaleFactor->setEnabled(CameraParameter::File.isEmpty());
    
    /* Background Substraction Parameters */
    this->ui->checkBoxBackgroundSubstractionUseDefault->setChecked(BackgroundSubstraction::bUseDefault);
    this->ui->spinBoxBackgroundSubstractionFromImage->setValue(BackgroundSubstraction::iFromImage);
    this->ui->spinBoxBackgroundSubstractionOffset->setValue(BackgroundSubstraction::iOffset);
    this->ui->spinBoxBackgroundSubstractionToImage->setValue(BackgroundSubstraction::iToImage);
    
    this->ui->spinBoxBackgroundSubstractionFromImage->setValue(BackgroundSubstraction::iFromImage);
    this->ui->spinBoxBackgroundSubstractionOffset->setValue(BackgroundSubstraction::iOffset);
    this->ui->spinBoxBackgroundSubstractionToImage->setValue(BackgroundSubstraction::iToImage);
    
    this->ui->spinBoxBackgroundSubstractionFromImage->setEnabled(!this->ui->checkBoxBackgroundSubstractionUseDefault->isChecked());
    this->ui->spinBoxBackgroundSubstractionOffset->setEnabled(!this->ui->checkBoxBackgroundSubstractionUseDefault->isChecked());
    this->ui->spinBoxBackgroundSubstractionToImage->setEnabled(!this->ui->checkBoxBackgroundSubstractionUseDefault->isChecked());
    
    /* Larvae Extraction Parameters */
    this->ui->checkBoxLarvaeExtactionParameterUseDefauilt->setChecked(LarvaeExtractionParameters::bUseDefault);
    this->ui->spinBoxLarvaeExtactionParameterNumerOfSpinePoints->setValue(LarvaeExtractionParameters::iNumerOfSpinePoints);
    
    /* IPAN Contour Curvature Parameters */
    this->ui->checkBoxLarvaeExtactionParametersUseDynamicIPAN->setChecked(LarvaeExtractionParameters::IPANContourCurvatureParameters::bUseDynamicIpanParameterCalculation);
    this->ui->spinBoxIPANContourCurvatureMinimalTriangelSideLenght->setValue(LarvaeExtractionParameters::IPANContourCurvatureParameters::iMinimalTriangelSideLenght);
    this->ui->spinBoxIPANContourCurvatureMaximalTriangelSideLenght->setValue(LarvaeExtractionParameters::IPANContourCurvatureParameters::iMaximalTriangelSideLenght);
    this->ui->doubleSpinBoxIPANContourCurvatureMaximalCurvaturePointsDistance->setValue(LarvaeExtractionParameters::IPANContourCurvatureParameters::dMaximalCurvaturePointsDistance);
    this->ui->spinBoxIPANContourCurvatureCurvatureWindowSize->setValue(LarvaeExtractionParameters::IPANContourCurvatureParameters::iCurvatureWindowSize);

    this->ui->spinBoxIPANContourCurvatureMinimalTriangelSideLenght->setEnabled(!this->ui->checkBoxLarvaeExtactionParametersUseDynamicIPAN->isChecked());
    this->ui->spinBoxIPANContourCurvatureMaximalTriangelSideLenght->setEnabled(!this->ui->checkBoxLarvaeExtactionParametersUseDynamicIPAN->isChecked());
    this->ui->doubleSpinBoxIPANContourCurvatureMaximalCurvaturePointsDistance->setEnabled(!this->ui->checkBoxLarvaeExtactionParametersUseDynamicIPAN->isChecked());
    this->ui->spinBoxIPANContourCurvatureCurvatureWindowSize->setEnabled(!this->ui->checkBoxLarvaeExtactionParametersUseDynamicIPAN->isChecked());

    /* Coiled Recognition Parameters */
    this->ui->doubleSpinBoxCoiledRecognitionParametersPerimeterToSpinelenghtThreshold->setValue(LarvaeExtractionParameters::CoiledRecognitionParameters::dPerimeterToSpinelenghtThreshold);
    this->ui->doubleSpinBoxCoiledRecognitionParametersMidcirclePerimeterToPerimeterThreshold->setValue(LarvaeExtractionParameters::CoiledRecognitionParameters::dMidcirclePerimeterToPerimeterThreshold);
    this->ui->doubleSpinBoxCoiledRecognitionParametersMaxToMeanRadiusThreshold->setValue(LarvaeExtractionParameters::CoiledRecognitionParameters::dMaxToMeanRadiusThreshold);
    /* Stop and Go Phase Calculation */
    this->ui->spinBoxStopAndGoFramesForSpeedCalc->setValue(LarvaeExtractionParameters::StopAndGoCalculation::iFramesForSpeedCalculation);
    this->ui->spinBoxStopAndGoSpeedThreshold->setValue(LarvaeExtractionParameters::StopAndGoCalculation::iSpeedThreshold);
    this->ui->spinBoxStopAndGoAngleThreshold->setValue(LarvaeExtractionParameters::StopAndGoCalculation::iAngleThreshold);

    this->ui->doubleSpinBoxCoiledRecognitionParametersPerimeterToSpinelenghtThreshold->setEnabled(!this->ui->checkBoxLarvaeExtactionParameterUseDefauilt->isChecked());
    this->ui->doubleSpinBoxCoiledRecognitionParametersMidcirclePerimeterToPerimeterThreshold->setEnabled(!this->ui->checkBoxLarvaeExtactionParameterUseDefauilt->isChecked());
    this->ui->doubleSpinBoxCoiledRecognitionParametersMaxToMeanRadiusThreshold->setEnabled(!this->ui->checkBoxLarvaeExtactionParameterUseDefauilt);

    /* Stop and Go calculation */
    this->ui->spinBoxStopAndGoFramesForSpeedCalc->setEnabled(!this->ui->checkBoxLarvaeExtactionParametersUseDynamicStopAndGo->isChecked());
    this->ui->spinBoxStopAndGoSpeedThreshold->setEnabled(!this->ui->checkBoxLarvaeExtactionParametersUseDynamicStopAndGo->isChecked());
    this->ui->spinBoxStopAndGoAngleThreshold->setEnabled(!this->ui->checkBoxLarvaeExtactionParametersUseDynamicStopAndGo->isChecked());

    /* Body bending Calculation */
    this->ui->doubleSpinBoxBodyBendingAngleThreshold->setEnabled(!this->ui->checkBoxLarvaeExtactionParametersUseDynamicBodyBending->isChecked());

    /* Movement Direction Calculation */
    this->ui->spinBoxMovementDirectionFramesForDirectionCalc->setEnabled(!this->ui->checkBoxLarvaeExtactionParametersUseDynamicMovementDirection->isChecked());

    /* Tracking Assignment Parameters */
    this->ui->comboBoxTrackingAssignmentMethod->setCurrentIndex(LarvaeExtractionParameters::AssignmentParameters::eAssignmentMethod);
    this->ui->comboBoxTrackingAssignmentCostMeasure->setCurrentIndex(LarvaeExtractionParameters::AssignmentParameters::eCostMeasure);
    this->ui->doubleSpinBoxGreedyAssignmentThreshold->setValue(LarvaeExtractionParameters::AssignmentParameters::dDistanceThreshold);
}

void PreferencesDialogWindow::updateBackgroungSpinboxesValues(unsigned int iNumberOfFiles)
{
    if(iNumberOfFiles > 0)
    {
        BackgroundSubstraction::iFromImage = 0;
        this->ui->spinBoxBackgroundSubstractionFromImage->setMinimum(0);
        this->ui->spinBoxBackgroundSubstractionFromImage->setMaximum(iNumberOfFiles - 1);
        
        BackgroundSubstraction::iOffset = 1;
        this->ui->spinBoxBackgroundSubstractionOffset->setMinimum(1);
        this->ui->spinBoxBackgroundSubstractionOffset->setMaximum(iNumberOfFiles - 1);
        
        BackgroundSubstraction::iToImage = iNumberOfFiles;
        this->ui->spinBoxBackgroundSubstractionToImage->setMinimum(1);
        this->ui->spinBoxBackgroundSubstractionToImage->setMaximum(iNumberOfFiles);
    }
    else
    {
        BackgroundSubstraction::iFromImage = 0;
        this->ui->spinBoxBackgroundSubstractionFromImage->setMinimum(0);
        this->ui->spinBoxBackgroundSubstractionFromImage->setMaximum(0);
        
        BackgroundSubstraction::iOffset = 0;
        this->ui->spinBoxBackgroundSubstractionOffset->setMinimum(0);
        this->ui->spinBoxBackgroundSubstractionOffset->setMaximum(0);
        
        BackgroundSubstraction::iToImage = 0;
        this->ui->spinBoxBackgroundSubstractionToImage->setMinimum(0);
        this->ui->spinBoxBackgroundSubstractionToImage->setMaximum(0);
    }
}

void PreferencesDialogWindow::on_spinBoxLarvaeExtactionParameterNumerOfSpinePoints_valueChanged(int arg1)
{
    if(arg1 % 2 == 0)
    {
        this->ui->spinBoxLarvaeExtactionParameterNumerOfSpinePoints->setValue(++arg1);
    }
}

void PreferencesDialogWindow::on_spinBoxIPANContourCurvatureCurvatureWindowSize_valueChanged(int arg1)
{
    if(arg1 % 2 == 0)
    {
        this->ui->spinBoxIPANContourCurvatureCurvatureWindowSize->setValue(++arg1);
    }
}

void PreferencesDialogWindow::on_checkBoxLarvaeExtactionParametersUseDynamicIPAN_clicked(bool checked)
{
    /* IPAN Contour Curvature */
    this->ui->spinBoxIPANContourCurvatureMinimalTriangelSideLenght->setEnabled(!checked);
    this->ui->spinBoxIPANContourCurvatureMaximalTriangelSideLenght->setEnabled(!checked);
    this->ui->doubleSpinBoxIPANContourCurvatureMaximalCurvaturePointsDistance->setEnabled(!checked);
    this->ui->spinBoxIPANContourCurvatureCurvatureWindowSize->setEnabled(!checked);
}

void PreferencesDialogWindow::on_checkBoxLarvaeExtactionParametersUseDynamicStopAndGo_clicked(bool checked)
{
    /* Stop And Go Phase Calculation */
    this->ui->spinBoxStopAndGoFramesForSpeedCalc->setEnabled(!checked);
    this->ui->spinBoxStopAndGoSpeedThreshold->setEnabled(!checked);
    this->ui->spinBoxStopAndGoAngleThreshold->setEnabled(!checked);
}

void PreferencesDialogWindow::on_checkBoxLarvaeExtactionParametersUseDynamicBodyBending_clicked(bool checked)
{
    /* Body bending calculation */
    this->ui->doubleSpinBoxBodyBendingAngleThreshold->setEnabled(!checked);
}

void PreferencesDialogWindow::on_checkBoxLarvaeExtactionParametersUseDynamicMovementDirection_clicked(bool checked)
{
    /* Movement Direction Calculation */
    this->ui->spinBoxMovementDirectionFramesForDirectionCalc->setEnabled(!checked);
}

// guarantee that the angle for body bending calculation is bigger than the angle for stop and go phase calculation
void PreferencesDialogWindow::on_doubleSpinBoxBodyBendingAngleThreshold_editingFinished()
{
    double dAngleThresh = this->ui->doubleSpinBoxBodyBendingAngleThreshold->value();
    int stopAndGoAngle = this->ui->spinBoxStopAndGoAngleThreshold->value();
    double angle = (static_cast<int>(dAngleThresh) >= stopAndGoAngle) ? dAngleThresh : static_cast<double>(stopAndGoAngle);
    this->ui->doubleSpinBoxBodyBendingAngleThreshold->setValue(angle);
}


void PreferencesDialogWindow::on_comboBoxTrackingAssignmentMethod_currentIndexChanged(int index)
{
    this->ui->doubleSpinBoxGreedyAssignmentThreshold->setEnabled(index == 1);
}
