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

#include "UniteDialog.hpp"
#include "ui_UniteDialog.h"

UniteDialog::UniteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UniteDialog)
{
    ui->setupUi(this);
}

UniteDialog::~UniteDialog()
{
    delete ui;
}

void UniteDialog::setup(QString const& larvaID, QStringList const& contemplableLarvaeIDs)
{
    this->ui->label->setText(QString("Attach to %1").arg(larvaID));
    bool oldState = this->ui->comboBox->blockSignals(true);
    this->ui->comboBox->clear();
    this->ui->comboBox->addItems(contemplableLarvaeIDs);
    this->ui->comboBox->blockSignals(oldState);
}

void UniteDialog::on_pushButton_clicked()
{
    QStringList l = this->ui->comboBox->currentText().split(" ");
    emit sendParameter(l.value(l.size() - 1).toUInt());
    this->close();
}

void UniteDialog::on_pushButton_2_clicked()
{
    this->close();
}
