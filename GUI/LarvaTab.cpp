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

#include "LarvaTab.hpp"
#include "ui_LarvaTab.h"

LarvaTab::LarvaTab(TrackerSceneLarva *sceneLarva,
                   unsigned int maxFrames, 
                   LarvaeContainer *larvaeContainer,
                   QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LarvaTab)
{
    ui->setupUi(this);
    
    this->mInvertDialog     = new InvertDialog(this);
    this->mUniteDialog      = new UniteDialog(this);
    
    this->mSceneLarva       = sceneLarva;
    this->mMaxTime          = maxFrames;
    this->mLarvaeContainer  = larvaeContainer;
    
    /* Init modification flag */
    this->mModified         = false;
    
    this->mCurrentTime      = 0;
    
    /* Hide "Larva was changed" message */
    this->ui->widgetChanged->setVisible(false);
    
    /* Setup radius-indices bounds */
    this->ui->spinBoxRadiusPosition->setMinimum(1);
    this->ui->spinBoxRadiusPosition->setMaximum(this->mSceneLarva->getTotalNumberOfSpinePoints());
    
    /* Setup radius bounds */
    this->ui->doubleSpinBoxRadius->setMinimum(0);
    this->ui->doubleSpinBoxRadius->setMaximum(std::numeric_limits<double>::max());
    
    /* Flashing of scene-larva */
    this->mFlashTimer = new QTimer(this);
    this->mFlashTimer->setInterval(100); //100 ms
    
    this->ui->labelFrameView->setVisible(false);
    this->ui->sliderTime->setVisible(false);
    this->ui->spinBoxTime->setVisible(false);
    this->ui->label_4->setVisible(false);
    this->ui->spinBoxRadiusPosition->setVisible(false);
    this->ui->label_6->setVisible(false);
    this->ui->doubleSpinBoxRadius->setVisible(false);
    
    /* Setup time frame selectors */
    this->updateTimes();
    
    this->setupConnections();
    
    /* Init with random color */
    qsrand(QTime::currentTime().msec());
    this->setColor(QColor(qrand() % 256, qrand() % 256, qrand() % 256));
}

LarvaTab::~LarvaTab()
{
    delete ui;
    this->mFlashTimer->stop();
    delete this->mFlashTimer;
    delete this->mSceneLarva;
}

bool LarvaTab::canChangeTime()
{
    if(this->mModified)
    {
        QMessageBox msgBox(
                    QString::fromUtf8("Not Saved Modifications on Larva ").append(this->mSceneLarva->getID()),
                    QString::fromUtf8("The larva has been modified manually. Prior to the change of the frame, the modification must be saved."),
                    QMessageBox::Warning,
                    QMessageBox::Save,
                    QMessageBox::Discard,
                    QMessageBox::Cancel);		
        int ret = msgBox.exec();
        
        switch (ret)
        {
            case QMessageBox::Save:
                this->applyChanges();
                break;
            case QMessageBox::Discard:
                this->reset();
                break;
            case QMessageBox::Cancel:
                //Stop
                return false;
                break;
            default:
                // should never be reached
                break;
        }
    }	
    
    return true;
}

TrackerSceneLarva *LarvaTab::getSceneLarva() const
{
    return this->mSceneLarva;
}

void LarvaTab::updateTimes()
{
    std::vector<unsigned int> timePoints = this->mLarvaeContainer->getAllTimesteps(this->mSceneLarva->getBaseID());
    if(!timePoints.empty())
    {
        this->ui->sliderTime->setMinimum((*timePoints.begin()) + 1);
        this->ui->spinBoxTime->setMinimum((*timePoints.begin()) + 1);
        this->ui->sliderTime->setValue((*timePoints.begin()) + 1);
        this->ui->spinBoxTime->setValue((*timePoints.begin()) + 1);
        
        this->ui->sliderTime->setMaximum((*timePoints.rbegin()) + 1);
        this->ui->spinBoxTime->setMaximum((*timePoints.rbegin()) + 1);
        
        this->ui->labelFrameView->setText(QString::number((*timePoints.begin()) + 1).append("/").append(QString::number(this->mMaxTime)));
    }
    else
    {
        this->ui->sliderTime->setMinimum(0);
        this->ui->spinBoxTime->setMinimum(0);
        this->ui->sliderTime->setValue(0);
        this->ui->spinBoxTime->setValue(0);
        
        this->ui->sliderTime->setMaximum(0);
        this->ui->spinBoxTime->setMaximum(0);
    }
}

void LarvaTab::setColor(QColor color)
{
    this->mColor = color;
    this->ui->labelColorDialog->setStyleSheet(
                "QLabel { background-color : rgb("
                +QString::number(color.red())+","
                +QString::number(color.green())+","
                +QString::number(color.blue())
                +"); color : black; }");
    
    emit sendNewColor(color);
}

void LarvaTab::setTime(unsigned int time)
{
    if(this->mCurrentTime != time)
    {
        this->ui->spinBoxTime->setValue(time);
    }
}

void LarvaTab::setupConnections()
{	
    connect(this,                               SIGNAL(changedLarva(const TrackerSceneLarva*)),         this->mLarvaeContainer,                 SLOT(updateLarvaValues(const TrackerSceneLarva*)));
    
    connect(this->ui->pushButtonApplyChanges,   SIGNAL(clicked()),                                      this,                                   SLOT(applyChanges()));
    connect(this->ui->pushButtonReset,          SIGNAL(clicked()),                                      this,                                   SLOT(reset()));
    
    connect(this->ui->pushButtonColorDialog,	SIGNAL(clicked()),                                      this,                                   SLOT(updateColor()));
    
    connect(this->ui->pushFindLarva,			SIGNAL(clicked()),                                      this,                                   SLOT(centerSceneOnLarva()));
    connect(this->ui->pushFindLarva,			SIGNAL(clicked()),                                      this,                                   SLOT(flash()));
    connect(this->mFlashTimer,                  SIGNAL(timeout()),                                      this,                                   SLOT(flashEvent()));
    
    connect(this->ui->checkBoxDisplayLarva,     SIGNAL(clicked(bool)),                                  this,                                   SLOT(displaySceneLarva(bool)));	
    connect(this->ui->checkBoxPath,             SIGNAL(toggled(bool)),                                  this,                                   SLOT(displaySceneLarvaPath(bool)));	
    connect(this->ui->checkBoxDist2Origin,		SIGNAL(toggled(bool)),                                  this,                                   SLOT(displaySceneLarvaDistance2Origin(bool)));	
    
    connect(this->ui->pushButtonPrevCollision,	SIGNAL(clicked()),                                      this,                                   SLOT(setTimeToPrevAmbiguity()));	
    connect(this->ui->pushButtonNextCollision,	SIGNAL(clicked()),                                      this,                                   SLOT(setTimeToNextAmbiguity()));
    
    connect(this->ui->pushInvert,				SIGNAL(clicked()),                                      this,                                   SLOT(showInvertLarvaDialog()));
    connect(this->ui->pushAttach,               SIGNAL(clicked()),                                      this,                                   SLOT(attachOtherModel()));
    connect(this->ui->pushCopy2Prev,            SIGNAL(clicked()),                                      this,                                   SLOT(copyToPrevTimestep()));
    connect(this->ui->pushCopy2Succ,            SIGNAL(clicked()),                                      this,                                   SLOT(copyToNextTimestep()));
    connect(this->ui->pushDeleteCurrent,        SIGNAL(clicked()),                                      this,                                   SLOT(deleteCurrentModel()));
    connect(this->ui->pushDeleteLarva,          SIGNAL(clicked()),                                      this,                                   SLOT(deleteLarva()));
    
    connect(this,                               SIGNAL(sendNewColor(QColor)),                           this->mSceneLarva,                      SLOT(setColor(QColor)));
    connect(this->mSceneLarva,                  SIGNAL(sendTotalNumberOfSpinePoints(uint)),             this,                                   SLOT(updateSpinBoxRadiusPosition(uint)));
    connect(this->ui->spinBoxRadiusPosition,    SIGNAL(valueChanged(int)),                              this,                                   SLOT(updateDisplayedRadius(int)));
    connect(this->ui->doubleSpinBoxRadius,      SIGNAL(valueChanged(double)),                           this,                                   SLOT(applyRadiusToSceneLarva(double)));
    
    connect(this->mSceneLarva,                  SIGNAL(valuesChanged()),                                this,                                   SLOT(showLarvaWasChanged()));
    
    connect(this->mInvertDialog,                SIGNAL(sendParameter(bool,uint,bool,bool,uint)),        this,                                   SLOT(handleInvertParameter(bool,uint,bool,bool,uint)));
    connect(this->mUniteDialog,                 SIGNAL(sendParameter(uint)),                            this,                                   SLOT(handleAttachParameter(uint)));
}

void LarvaTab::centerSceneOnLarva()
{
    unsigned int spineMidPointIndex;
    if(this->mLarvaeContainer->getSpineMidPointIndex(this->mSceneLarva->getBaseID(), spineMidPointIndex))
    {
        cv::Point spineMidPoint;
        if(this->mLarvaeContainer->getSpinePointAt(this->mSceneLarva->getBaseID(), this->mCurrentTime, spineMidPointIndex, spineMidPoint))
        {
            emit requestCenterOn(qreal(spineMidPoint.x), qreal(spineMidPoint.y));
        }
    }
}

void LarvaTab::update(unsigned int time)
{
    if(this->canChangeTime())
    {
        this->mCurrentTime = time;
        this->ui->spinBoxTime->setValue(this->mCurrentTime);
        this->ui->sliderTime->setValue(this->mCurrentTime);
        this->mSceneLarva->updateTime(this->mCurrentTime);
        
        double val;
        if(this->mLarvaeContainer->getAccDistAt(this->mSceneLarva->getBaseID(), this->mCurrentTime, val))
        {
            this->ui->labelAccDistance->setText(QString::number(val));
        }
        else
        {
            this->ui->labelAccDistance->setText("");
        }
        if(this->mLarvaeContainer->getDistToOriginAt(this->mSceneLarva->getBaseID(), this->mCurrentTime, val))
        {
            this->ui->labelDist2Origin->setText(QString::number(val));
        }
        else
        {
            this->ui->labelDist2Origin->setText("");
        }
    }
}

void LarvaTab::redrawPaths()
{
    this->mSceneLarva->adjustDistancePaths();
}

void LarvaTab::updateColor()
{
    QColor color = QColorDialog::getColor(this->mColor, this, QString::fromUtf8("Please Choose The New Color For This Larva"));
    if(color.isValid())
    {
        this->setColor(color);
    }
}

void LarvaTab::setTimeToPrevAmbiguity()
{
    std::vector<uint> timesteps = this->mLarvaeContainer->getAllTimesteps(this->mSceneLarva->getBaseID());
    bool flag = false;
    
    QVector< QPair<uint, uint> > timeGaps = this->findTimeStepGaps(timesteps);
    
    int gapIndex;
    if(this->findNextTimeGap(timeGaps, gapIndex))
    {
        if(this->mCurrentTime > timeGaps.at(gapIndex).second)
        {
            this->mCurrentTime = timeGaps.at(gapIndex).second;
            emit sendAmbiguityTimepoint(this->mCurrentTime + 1);
            this->ui->pushFindLarva->click();
            return;
        }
        if(this->mCurrentTime > timeGaps.at(gapIndex).first)
        {
            this->mCurrentTime = timeGaps.at(gapIndex).first;
            emit sendAmbiguityTimepoint(this->mCurrentTime + 1);
            this->ui->pushFindLarva->click();
            return;
        }
    }
    
    for(int i = timesteps.size() - 1; i >= 0; --i) 
    {
        /* only the timesteps before current timestep are importent for us */
        if(timesteps.at(i) >= this->mCurrentTime)
        {
            continue;
        }
        
        /* if coiledindicator is set... */
        if(this->mLarvaeContainer->getIsCoiledIndicatorAt(this->mSceneLarva->getBaseID(), timesteps.at(i), flag))
        {
            /* ... and if the larva is coiled at timestep t */
            if(flag)
            {
                this->mCurrentTime = timesteps.at(i);
                emit sendAmbiguityTimepoint(this->mCurrentTime + 1);
                this->ui->pushFindLarva->click();
                return;
            }
        }
    }
    
    /* if we are here then the larva is not coiled at any previous timestep 
     * so we check if the firts timepoint of the larva is zero or not */
    if((timesteps.front() != this->mCurrentTime) && (timesteps.front() != 0))
    {
        this->mCurrentTime = timesteps.front();
        emit sendAmbiguityTimepoint(this->mCurrentTime + 1);
        this->ui->pushFindLarva->click();
    }
}

void LarvaTab::setTimeToNextAmbiguity()
{
    std::vector<uint> timesteps = this->mLarvaeContainer->getAllTimesteps(this->mSceneLarva->getBaseID());
    bool flag = false;
    
    QVector< QPair<uint, uint> > timeGaps = this->findTimeStepGaps(timesteps);
    
    int gapIndex;
    if(this->findNextTimeGap(timeGaps, gapIndex))
    {
        if(this->mCurrentTime < timeGaps.at(gapIndex).first)
        {
            this->mCurrentTime = timeGaps.at(gapIndex).first;
            emit sendAmbiguityTimepoint(this->mCurrentTime + 1);
            this->ui->pushFindLarva->click();
            return;
        }
        if(this->mCurrentTime < timeGaps.at(gapIndex).second)
        {
            this->mCurrentTime = timeGaps.at(gapIndex).second;
            emit sendAmbiguityTimepoint(this->mCurrentTime + 1);
            this->ui->pushFindLarva->click();
            return;
        }
    }
    
    for(uint i = 0; i < timesteps.size(); ++i) 
    {
        /* only the timesteps after current timestep are importent for us */
        if(timesteps.at(i) <= this->mCurrentTime)
        {
            continue;
        }
        
        /* if there is one coiledindicator... */
        if(this->mLarvaeContainer->getIsCoiledIndicatorAt(this->mSceneLarva->getBaseID(), timesteps.at(i), flag))
        {
            /* ...and if the larva is coiled at timestep t */
            if(flag)
            {
                this->mCurrentTime = timesteps.at(i);
                emit sendAmbiguityTimepoint(this->mCurrentTime + 1);
                this->ui->pushFindLarva->click();
                return;
            }
        }
    }
    
    /* if we are here then the larva is not coiled at any previous timestep 
     * so we check if the firts timepoint of the larva is zero or not */
    if((timesteps.back() != this->mCurrentTime) && (timesteps.back() != this->mMaxTime - 1))
    {
        this->mCurrentTime = timesteps.back();
        emit sendAmbiguityTimepoint(this->mCurrentTime + 1);
        this->ui->pushFindLarva->click();
    }
}

void LarvaTab::handleInvertParameter(bool before, uint bTime, bool current, bool after, uint aTime)
{
    if(before)
    {
        this->mLarvaeContainer->invertLarva(this->mSceneLarva->getBaseID(), this->mCurrentTime, bTime - 1);
    }
    else if(current)
    {
        this->mLarvaeContainer->invertLarva(this->mSceneLarva->getBaseID(), this->mCurrentTime, this->mCurrentTime);
    }
    else if(after)
    {
        this->mLarvaeContainer->invertLarva(this->mSceneLarva->getBaseID(), this->mCurrentTime, aTime - 1);
    }
    this->reset();
}

void LarvaTab::handleAttachParameter(uint contemplableLarvaeID)
{
    this->mLarvaeContainer->attachToLarva(this->mSceneLarva->getBaseID(), contemplableLarvaeID);
    this->reset();
}

void LarvaTab::showLarvaWasChanged()
{
    this->ui->widgetChanged->setVisible(true);
    this->mModified = true;
}

void LarvaTab::updateSpinBoxRadiusPosition(uint maxValue)
{
    this->ui->spinBoxRadiusPosition->setMaximum(maxValue);
}

QVector< QPair<uint, uint> > LarvaTab::findTimeStepGaps(std::vector<uint> const& timeSteps)
{
    QVector< QPair<uint, uint> > result;
    int dist;
    
    for(size_t i = 1; i < timeSteps.size(); ++i)
    {
        dist = timeSteps.at(i) - timeSteps.at(i-1);
        if(qAbs(dist) > 1)
        {
            result.push_back(QPair<uint, uint>(timeSteps.at(i-1), timeSteps.at(i)));
        }
    }
    return result;
}

bool LarvaTab::findNextTimeGap(QVector<QPair<uint, uint> > const& timeGaps, int& gapIndex)
{
    int dist2Gap = std::numeric_limits<int>::max();
    int currentDist;
    int leftDist;
    int rightDist;
    bool found = false;
    
    for(int i = 0; i < timeGaps.size(); ++i)
    {
        leftDist = timeGaps.at(i).first - this->mCurrentTime;
        rightDist = timeGaps.at(i).second - this->mCurrentTime;
        currentDist = qMin(qAbs(leftDist), qAbs(rightDist));
        if(currentDist < dist2Gap)
        {
            dist2Gap = currentDist;
            gapIndex = i;
            found = true;
        }
    }
    
    return found;
}

void LarvaTab::flash()
{
    this->mFlashCount = 10;
    this->mFlashTimer->start();
}

void LarvaTab::flashEvent()
{
    this->mSceneLarva->setVisible(this->mFlashCount % 2);
    --this->mFlashCount;
    if (this->mFlashCount <= 0) 
    { 
        this->mSceneLarva->setVisible(this->ui->checkBoxDisplayLarva->isChecked());
        this->mFlashTimer->stop(); 
    }
}

void LarvaTab::displaySceneLarva(bool visible)
{
    this->mSceneLarva->setLarvaVisible(visible);
}

void LarvaTab::displaySceneLarvaPath(bool visible)
{
    this->mSceneLarva->setPathVisible(visible);
}

void LarvaTab::displaySceneLarvaDistance2Origin(bool visible)
{
    this->mSceneLarva->setDistance2OriginVisible(visible);
}

void LarvaTab::updateDisplayedRadius(int index)
{
    qreal sceneRadius = this->mSceneLarva->getRadiusAt(index - 1);
    this->ui->doubleSpinBoxRadius->setValue(sceneRadius);
}

void LarvaTab::applyRadiusToSceneLarva(double radius)
{
    int index = this->ui->spinBoxRadiusPosition->value();
    qreal currentRadius = this->mSceneLarva->getRadiusAt(index);
    if(radius != currentRadius)
    {
        this->mSceneLarva->setRadiusAt(index - 1, radius);
    }
}

void LarvaTab::attachOtherModel()
{
    QStringList contemplableLarvaeIDsForAttach = this->mLarvaeContainer->getAllContemplableLarvaeIDsForAttach(this->mSceneLarva->getBaseID());
    
    if(!contemplableLarvaeIDsForAttach.isEmpty())
    {
        this->mUniteDialog->move(this->parentWidget()->window()->frameGeometry().topLeft() +
                                 this->parentWidget()->window()->rect().center());
        this->mUniteDialog->setModal(true);
        this->mUniteDialog->setup(this->mSceneLarva->getID(), contemplableLarvaeIDsForAttach);
        this->mUniteDialog->show();
    }
}

void LarvaTab::deleteLarva()
{
    QMessageBox msgBox;
    msgBox.setText(QString::fromUtf8("Should the Larva be really deleted?"));
    msgBox.setInformativeText(QString::fromUtf8("After this operation, the larva is removed completely from the results. This refers to all time steps."));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Warning);
    if(msgBox.exec() == QMessageBox::Ok)
    {
        this->mLarvaeContainer->eraseLarva(this->mSceneLarva->getBaseID());
    }
}

void LarvaTab::deleteCurrentModel()
{
    QMessageBox msgBox;
    msgBox.setText(QString::fromUtf8("Should the Larva be really deleted for this timestep?"));
    msgBox.setInformativeText(QString::fromUtf8("After this operation, the results of this larva for the current time step will be permanently deleted."));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setIcon(QMessageBox::Warning);
    if(msgBox.exec() == QMessageBox::Ok)
    {
        if(this->mLarvaeContainer->eraseLarvaAt(this->mSceneLarva->getBaseID(), this->mCurrentTime))
        {
            this->reset();
            this->setEnabled(false);
        }
    }
}

void LarvaTab::reset()
{
    this->mModified = false;
    this->mSceneLarva->recompute(this->mCurrentTime);
    this->ui->widgetChanged->setVisible(false);
}

void LarvaTab::applyChanges()
{
    this->mModified = false;
    this->ui->widgetChanged->setVisible(false);
    emit changedLarva(this->mSceneLarva);
}

void LarvaTab::showInvertLarvaDialog()
{
    this->mInvertDialog->move(this->parentWidget()->window()->frameGeometry().topLeft() +
                              this->parentWidget()->window()->rect().center());
    this->mInvertDialog->setModal(true);
    this->mInvertDialog->show();
    this->mInvertDialog->setBeforeTimeSteps(this->mLarvaeContainer->getAllTimestepsBefore(this->mSceneLarva->getBaseID(), this->mCurrentTime));
    this->mInvertDialog->setAfterTimeSteps(this->mLarvaeContainer->getAllTimestepsAfter(this->mSceneLarva->getBaseID(), this->mCurrentTime));
}

void LarvaTab::copyToPrevTimestep()
{
    if(this->canCopyModel(true))
    {
        if(this->mLarvaeContainer->copyModel2PrevTimeStep(this->mSceneLarva->getBaseID(), this->mCurrentTime))
        {
            --this->mCurrentTime;
            emit sendAmbiguityTimepoint(this->mCurrentTime + 1);
        }
    }
    else
    {
        int frameID = this->mCurrentTime - 1;
        if(frameID > -1)
        {
            QMessageBox msgBox;
            msgBox.setText("Information");
            msgBox.setInformativeText(QString("At Timestep %1 there is already a Larvamodel defined.").arg(frameID + 1));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            int ret = msgBox.exec();
            Q_UNUSED(ret);
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Information");
            msgBox.setInformativeText(QString("It is not possible to define a Larvamodel at Timestep %1").arg(frameID));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            int ret = msgBox.exec();
            Q_UNUSED(ret);
        }
    }
}

void LarvaTab::copyToNextTimestep()
{
    if(this->canCopyModel(false))
    {
        if(this->mLarvaeContainer->copyModel2NextTimeStep(this->mSceneLarva->getBaseID(), this->mCurrentTime))
        {
            ++this->mCurrentTime;
            emit sendAmbiguityTimepoint(this->mCurrentTime + 1);
        }
    }
    else
    {
        uint frameID = this->mCurrentTime + 1;
        if(frameID < this->mMaxTime - 1)
        {
            QMessageBox msgBox;
            msgBox.setText("Information");
            msgBox.setInformativeText(QString("At Timestep %1 there is already a Larvamodel defined.").arg(frameID + 1));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            int ret = msgBox.exec();
            Q_UNUSED(ret);
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Information");
            msgBox.setInformativeText(QString("It is not possible to define a Larvamodel at Timestep %1 because the Video consits only %2 Frames").arg(frameID + 1).arg(this->mMaxTime));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            int ret = msgBox.exec();
            Q_UNUSED(ret);
        }
    }
}

bool LarvaTab::canCopyModel(bool prev)
{
    /* insert at the previous Timestep */
    if(prev)
    {
        /* if we are not at the begining of the video */
        if(this->mCurrentTime > 0)
        {
            QStringList timePoints = this->mLarvaeContainer->getAllTimestepsBefore(this->getSceneLarva()->getBaseID(), this->mCurrentTime);
            if(timePoints.isEmpty())
            {
                return true;
            }
            else
            {
                /* no substraction is needed because of zerobased index and because we want to insert the model in the past */
                return (timePoints.front().toUInt() != this->mCurrentTime);
            }
        }
        else
        {
            return false;
        }
    }
    /* insert at the next Timestep */
    else
    {
        /* if we are not at the end of the video */
        if(this->mCurrentTime < this->mMaxTime - 1)
        {
            QStringList timePoints = this->mLarvaeContainer->getAllTimestepsAfter(this->getSceneLarva()->getBaseID(), this->mCurrentTime);
            if(timePoints.isEmpty())
            {
                return true;
            }
            else
            {
                /* +2 because of zerobased index and because we want to insert the model in the future */
                return (timePoints.front().toUInt() != this->mCurrentTime + 2);
            }
        }
        else
        {
            return false;
        }
    }
}
