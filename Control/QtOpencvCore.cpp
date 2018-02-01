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

#include "QtOpencvCore.hpp"

namespace QtOpencvCore
{
    QImage img2qimg(cv::Mat& img)
    {

         // convert the color to RGB (OpenCV uses BGR)
         switch (img.type()) {
         case CV_8UC1:
            cv::cvtColor(img, img, CV_GRAY2RGB);
            break;
         case CV_8UC3:
            cv::cvtColor(img, img, CV_BGR2RGB);
            break;
         }


         // return the QImage
         return QImage((uchar*) img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
    }
    
    std::vector<std::string>& qstrList2strList(QStringList const& qStringList, std::vector<std::string>& strList)
    {
        for (QStringList::const_iterator it = qStringList.begin(); it != qStringList.end(); it++)
        {
            strList.push_back(qstr2str((*it)));    
        }
        return strList;
    }

    std::string qstr2str(QString const& qstr)
    {
        // return the converted QString (now std string)
        return qstr.toStdString();
    }

    QString str2qstr(std::string const& str)
    {
        // return the converted std string (now QString)
        return QString::fromStdString(str);
    }
    
    cv::Rect qRect2Rect(QRectF const& r)
    {
        return cv::Rect(r.x(), r.y(), r.width(), r.height());
    }
    
    cv::RotatedRect qRect2RotatedRect(QRectF const& r)
    {
        cv::Rect cvRect = qRect2Rect(r);
        return cv::RotatedRect(cv::Point2f((float)cvRect.x + cvRect.width/2, (float)cvRect.y + cvRect.height/2), cv::Size2f((float)cvRect.width, (float)cvRect.height), 180.0);
    }
    
    QPointF point2qpoint(cv::Point const& p)
    {
        return QPointF(p.x, p.y);
    }

    cv::Point qpoint2point(QPointF const& p)
    {
        return cv::Point(p.x(), p.y());
    }

    cv::Mat qimg2img(const QImage &qimg)
    {
        cv::Mat img;
        img = cv::Mat(qimg.height(), qimg.width(), CV_8UC4, const_cast<uchar*>(qimg.bits()), qimg.bytesPerLine());
        return img;
    }

} // namespace QtOpencvCore
