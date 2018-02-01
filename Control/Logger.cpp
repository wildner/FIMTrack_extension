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

#include "Logger.hpp"

Logger* Logger::_instance = nullptr;
QList<QString> Logger::_logList = QList<QString>();

Logger::Logger(QObject* parent) :
    QObject(parent)
{
}

Logger* Logger::getInstance()
{
    static Guard g;
    if (_instance == nullptr)
    {
        _instance = new Logger();
    }
    return _instance;
}

void Logger::saveLog()
{
    QFile outfile;
    QString fileName = "";

    fileName.append("log_");
    fileName.append(QDateTime::currentDateTime().toString(Qt::ISODate).replace(":", "-"));
    fileName.append(".txt");

    outfile.setFileName(fileName);
    outfile.open(QIODevice::Append | QIODevice::Text);

    QTextStream out(&outfile);
    for (int i = 0; i < Logger::_logList.size(); i++)
    {
        out << Logger::_logList.at(i) << endl;
    }
}

void Logger::saveLog(QString dir)
{
    QFile outfile;
    QString fileName = dir;

    fileName.append("/log_");
    fileName.append(QDateTime::currentDateTime().toString(Qt::ISODate).replace(":", "-"));
    fileName.append(".txt");

    outfile.setFileName(fileName);
    outfile.open(QIODevice::Append | QIODevice::Text);

    QTextStream out(&outfile);
    for (int i = 0; i < Logger::_logList.size(); i++)
    {
        out << Logger::_logList.at(i) << endl;
    }
}

void Logger::addLogMessage(QString msg, LOGLEVEL level)
{
    Logger::getInstance()->handleLogMessage(msg, level);
}


void Logger::handleLogMessage(QString msg, LOGLEVEL level)
{
    QString res = "";

    switch (level)
    {
        case INFO:
            res.append(" INFO\t");
            emit newMessageForMainGUI(msg);
            break;
        case DEBUG:
            res.append(" DEBUG\t");
            break;
        case WARNING:
            res.append(" WARNING\t");
            break;
        case MERROR:
            res.append(" ERROR\t");
            break;
        case FATAL:
            res.append(" FATAL\t");
            break;
    }

    res.append(QDateTime::currentDateTime().toString(Qt::ISODate));
    res.append("\t");
    res.append(msg);

    this->_logList.append(res);

    emit newLogMessage(res);
}
