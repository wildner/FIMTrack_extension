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

#include "LOGWindow.hpp"
#include "ui_LOGWindow.h"

LOGWindow::LOGWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LOGWindow)
{
    try
    {
        ui->setupUi(this);
        
        /* in order to have a free-standing window */
        this->setWindowFlags(Qt::Window);
        
        connect(Logger::getInstance(), SIGNAL(newLogMessage(QString)), ui->plainTextEdit, SLOT(appendPlainText(QString)));
    }
    catch(...)
    {
        Logger::getInstance()->addLogMessage(QString("Fatal Error in LOGWindow::LOGWindow"), FATAL); 
    }
}

LOGWindow::~LOGWindow()
{
    try
    {
        delete ui;
    }
    catch(...)
    {
        Logger::getInstance()->addLogMessage(QString("Fatal Error in LOGWindow::~LOGWindow"), FATAL); 
    }
}

void LOGWindow::on_btnClose_clicked()
{
    try
    {
        this->close();
    }
    catch(...)
    {
        Logger::getInstance()->addLogMessage(QString("Fatal Error in LOGWindow::on_btnClose_clicked"), FATAL); 
    }
}
