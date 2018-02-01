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

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <QObject>
#include <QString>
#include <QTime>
#include <QList>
#include <QFile>
#include <QTextStream>

enum LOGLEVEL
{
    INFO,
    DEBUG,
    WARNING,
    MERROR,
    FATAL
};

class Logger : public QObject
{
    Q_OBJECT
private:
    explicit Logger(QObject* parent = 0);
    static Logger* _instance;
    Logger(Logger const&);
    
    static QList<QString> _logList;
    
    ~Logger() {}
    
    class Guard
    {
    public:
        ~Guard() {
            if (Logger::_instance != NULL) {
                delete Logger::_instance;
            }
        }
    };
    friend class Guard;
    
public:
    static Logger* getInstance();
    static void saveLog();
    static void saveLog(QString dir);
    static void addLogMessage(QString msg, LOGLEVEL level);
    
public slots:
    void handleLogMessage(QString msg, LOGLEVEL level);
    
signals:
    void newLogMessage(QString);
    void newMessageForMainGUI(QString);
    
};

#endif // LOGGER_HPP
