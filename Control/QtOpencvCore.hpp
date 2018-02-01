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

#ifndef QTOPENCVCORE_HPP
#define QTOPENCVCORE_HPP

#include <QImage>
#include <QString>
#include <QStringList>
#include <QSize>

#include <string>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <cstdlib>

#include "Configuration/FIMTrack.hpp"


namespace QtOpencvCore
{
    /**
     * @brief This function takes a cv::Mat image and converts it to a QImage
     *
     *
     * @param  img is a cv::Mat image which will be converted to a QImage
     * @return QImage of the cv::Mat image img of size labelSize
     */
    QImage img2qimg(cv::Mat &img);   
    cv::Mat qimg2img(QImage const &qimg);
    
    /**
     * @brief qstrList2strList
     * @param qStringList
     * @param strList
     * @return 
     */
    std::vector<std::string>& qstrList2strList(QStringList const& qStringList, std::vector<std::string>& strList);
    
    
    /**
     * @brief Converts a QString to a std string
     * @param QString to be converted
     * @return  Std string created from the QString
     */
    std::string qstr2str(QString const& qstr);
    
    
    /**
     * @brief Converts a std string to a QString
     * @param Std string to be converted
     * @return QString created from the std string
     */
    QString str2qstr(std::string const& str);
    
    cv::Rect qRect2Rect(QRectF const& r);
    
    cv::RotatedRect qRect2RotatedRect(QRectF const& r);
    
    QPointF point2qpoint(cv::Point const& p);
    cv::Point qpoint2point(QPointF const& p);
}

#endif // QTOPENCVCORE_HPP
