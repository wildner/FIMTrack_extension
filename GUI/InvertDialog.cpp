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

#include "InvertDialog.hpp"

#include "ui_InvertDialog.h"

InvertDialog::InvertDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InvertDialog)
{
    ui->setupUi(this);
    
    this->setWindowTitle(QString("InvertDialog"));
    
    this->setupConnections();
}

InvertDialog::~InvertDialog()
{
    delete ui;
}

void InvertDialog::setBeforeTimeSteps(QStringList const& timeSteps)
{
    bool oldState = this->ui->cbBefore->blockSignals(true);
    this->ui->cbBefore->clear();
    this->ui->cbBefore->addItems(timeSteps);
    this->ui->cbBefore->blockSignals(oldState);
    
    this->ui->cbBefore->blockSignals(oldState);
    
    this->ui->rbBefore->setEnabled(!timeSteps.isEmpty());
}

void InvertDialog::setAfterTimeSteps(QStringList const& timeSteps)
{
    bool oldState = this->ui->cbAfter->blockSignals(true);
    this->ui->cbAfter->clear();
    this->ui->cbAfter->addItems(timeSteps);
    this->ui->cbAfter->blockSignals(oldState);
    
    this->ui->rbAfter->setEnabled(!timeSteps.isEmpty());
}

void InvertDialog::setupConnections()
{
    connect(this->ui->rbBefore,     SIGNAL(toggled(bool)),      this->ui->cbBefore,     SLOT(setEnabled(bool)));
    connect(this->ui->rbAfter,      SIGNAL(toggled(bool)),      this->ui->cbAfter,      SLOT(setEnabled(bool)));
}

void InvertDialog::on_pbtOK_clicked()
{
    emit sendParameter(this->ui->rbBefore->isChecked(),
                       this->ui->cbBefore->currentText().toUInt(),
                       this->ui->rbCurrent->isChecked(),
                       this->ui->rbAfter->isChecked(),
                       this->ui->cbAfter->currentText().toUInt());
    this->close();
}

void InvertDialog::on_pbtCancel_clicked()
{
    this->close();
}
