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

#include "Preprocessor.hpp"

using namespace cv;
using std::vector;

Preprocessor::Preprocessor()
{
}

void Preprocessor::graythresh(Mat const & src,
                              int const thresh,
                              Mat & dst)
{
    threshold(src, dst, thresh, 255.0, THRESH_BINARY);
}

void Preprocessor::calcContours(Mat const & src, contours_t & contours)
{
    findContours(src, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
}

void Preprocessor::sizethreshold(const contours_t &contoursSrc, const int minSizeThresh, const int maxSizeThresh, contours_t &correctContoursDst, contours_t &biggerContoursDst)
{
    correctContoursDst.clear();
    biggerContoursDst.clear();
    
    // iterate over all contours
    for(auto const& c : contoursSrc)
    {
        // calculate the current size of the contour area
        double current_size = cv::contourArea(c);
        
        // check the size (maxSizeThresh > current_size > minSizeThresh)
        if(current_size <= maxSizeThresh && current_size >= minSizeThresh)
        {
            correctContoursDst.push_back(c);
        }
        else if(current_size > maxSizeThresh)
        {
            biggerContoursDst.push_back(c);
        }
    }
}

void Preprocessor::filterByFeatures(cv::Mat const & img,
	contours_t const & contoursSrc,
	int const minSizeThresh,
	int const maxSizeThresh,
	contours_t & correctContoursDst,
	contours_t & biggerContoursDst)
{
	// ############ PARAMETERS #################
	double minLength2Thickness = 2.5;
	// see TrackerConfig - Coiled Recognition Parameters
	double defaultMaxLength2Thickness = 25.0;
	double defaultMaxSpineLength = 100.0;
	double defaultmaxThickness2meanThickness = 2.3;
	double maxLength2Thickness = defaultMaxLength2Thickness;
	double maxSpineLength = defaultMaxSpineLength;
	double maxThickness2meanThickness = defaultmaxThickness2meanThickness;
		if(LarvaeExtractionParameters::bUseDefault == false)
		{
			maxLength2Thickness = LarvaeExtractionParameters::CoiledRecognitionParameters::dPerimeterToSpinelenghtThreshold;
			maxSpineLength = LarvaeExtractionParameters::CoiledRecognitionParameters::dMidcirclePerimeterToPerimeterThreshold;
			maxThickness2meanThickness = LarvaeExtractionParameters::CoiledRecognitionParameters::dMaxToMeanRadiusThreshold;
		}

	correctContoursDst.clear();
	biggerContoursDst.clear();

	// iterate over all contours
	for (const contour_t c : contoursSrc)
	{
		// calculate the current size of the contour area
		double current_size = cv::contourArea(c);
		if (current_size > maxSizeThresh)
		{
			biggerContoursDst.push_back(c);
		}
		else if (current_size >= minSizeThresh)
		{
			RawLarva rLarva = RawLarva(c, img);

			std::vector<double> currThicknessVector = rLarva.getLarvaThicknessVector();
			//// use MEAN for average thickness
			//double averageThickness = std::accumulate(currThicknessVector.begin(), currThicknessVector.end(), 0) / (double)currThicknessVector.size();
			// use MEDIAN for average thickness
			double averageThickness = Calc::medianOfVector(currThicknessVector);

			if (rLarva.getSpineLength() > maxSpineLength /*|| rLarva.getSpineLength() < minSpineLength*/ || ((double)(rLarva.getSpineLength())) < minLength2Thickness * averageThickness || ((double)(rLarva.getSpineLength())) > maxLength2Thickness * averageThickness)
			{
				biggerContoursDst.push_back(c);
				continue;
			}

			bool isBiggerContour = false;
			for (int i = (int)std::ceil(currThicknessVector.size() / 8.0); i < currThicknessVector.size() - (int)std::ceil(currThicknessVector.size() / 8.0); i++)
			{
				if (currThicknessVector[i] > maxThickness2meanThickness * averageThickness /* || currThicknessVector[i] >= maxThickness * 1.5*/)
					{
						isBiggerContour = true;
						break;
					}
			}

			if (isBiggerContour)
			{
				biggerContoursDst.push_back(c);
			}
			else
			{
				correctContoursDst.push_back(c);
			}
		}
		// else: DO NOTHING. Too small contours are completely dropped.
	}
}

void Preprocessor::borderRestriction(contours_t &contours, const Mat& img, bool checkRoiBorders)
{
    contours_t validContours;
    validContours.reserve(contours.size());
    
    for(auto const& contour : contours)
    {
        cv::Rect rect = cv::boundingRect(contour);
        
        // necessity of extending the rect by 2 pixels, because findContours returns
        // contours that have at least 1 pixel distance to image-borders
        int x1 = rect.x - 2;
        int y1 = rect.y - 2;
        int x2 = rect.x + rect.width + 2;
        int y2 = rect.y + rect.height + 2;
        
        // at first: check, if bounding-rect outside of the images range
        if(x1 < 0 || y1 < 0 || x2 >= img.cols || y2 >= img.rows)
        {
            continue;
        }
        
        bool valid = true;
        if(checkRoiBorders)
        {
            // at second: check, if convex-hull not within ROI
            FIMTypes::contour_t convexHull;
            cv::convexHull(contour, convexHull);
            
            // for every point of the convex hull ...
            foreach(cv::Point p, convexHull)
            {
                // check, if at least one of the four neighbours lies outside the ROI
                if(img.at<uchar>(p.y-1,p.x-1) == 0
                        || img.at<uchar>(p.y+1,p.x-1) == 0
                        || img.at<uchar>(p.y-1,p.x+1) == 0
                        || img.at<uchar>(p.y+1,p.x+1) == 0)
                {
                    valid = false;
                    break;
                }
            }
        }
        
        // at this point, check if contour is valid
        if(valid)
        {
            validContours.push_back(contour);
        }
    }
    contours = validContours;
}

void Preprocessor::preprocessPreview(const Mat &src,
                                     contours_t &acceptedContoursDst,
                                     contours_t &biggerContoursDst,
                                     const int gThresh,
                                     const int minSizeThresh,
                                     const int maxSizeThresh,
									 const int valleyThresh,
									 Backgroundsubtractor const & bs)
{
    // generate a scratch image
    Mat tmpImg = Mat::zeros(src.size(), src.type());

	// subtract background
	bs.subtract(src, tmpImg);

	// perform gray threshold
	Preprocessor::graythresh(tmpImg,gThresh,tmpImg);

    // generate a contours container scratch
    contours_t contours;
    
    // calculate the contours
    Preprocessor::calcContours(tmpImg,contours);

	// holds contours without inner crossings
	contours_t validatedContours;

	Preprocessor::eliminateAllInnerContourCrossings(contours, validatedContours);
    
    // filter the contours
	//Preprocessor::sizethreshold(validatedContours, minSizeThresh, maxSizeThresh, acceptedContoursDst, biggerContoursDst);
	Preprocessor::filterByFeatures(src, validatedContours, minSizeThresh, maxSizeThresh, acceptedContoursDst, biggerContoursDst);

	/* 
	   ################### TODO ##########################
	   Valley enhancement does not work currently; errors occured after the implementation of the background subtraction.
	   If valley enhancement can be fixed, do not forget to make the corresponding spinBox visible again (see MainGUI "hide valley threshold")!
	*/ 
	//// enhance the valleys (area between different larvae) inside of given contours
	//Preprocessor::enhanceValleys(src, tmpImg, acceptedContoursDst, biggerContoursDst, minSizeThresh, maxSizeThresh, valleyThresh);

	/*
	  ######### OPTIONAL #############
	  splitBiggerContours is based on the assumption that the observed objects
	  do not overlap and that collided contours can always be separated.
	  Since this is not true for fish, this function is probably useless.
	*/
	//// try to split contours which are bigger than one larva/fish
	//contours_t splittedContours;
	//Preprocessor::splitBiggerContours(src, splittedContours, biggerContoursDst, maxSizeThresh - minSizeThresh);

	//// filter the contours again
	//contours_t acceptedSplittedContours;
	//contours_t biggerSplittedContours;
	//Preprocessor::filterByFeatures(src, splittedContours, minSizeThresh, maxSizeThresh, acceptedSplittedContours, biggerSplittedContours);
	//acceptedContoursDst.insert(acceptedContoursDst.end(), acceptedSplittedContours.begin(), acceptedSplittedContours.end());
	//biggerContoursDst.insert(biggerContoursDst.end(), biggerSplittedContours.begin(), biggerSplittedContours.end());
}

void Preprocessor::preprocessTracking(Mat const & src,
                                      contours_t & acceptedContoursDst,
                                      contours_t & biggerContoursDst,
                                      int const gThresh,
                                      int const minSizeThresh,
                                      int const maxSizeThresh,
									  int const valleyThresh,
                                      Backgroundsubtractor const & bs,
                                      bool checkRoiBorders)
{
    // generate a scratch image
    Mat tmpImg = Mat::zeros(src.size(), src.type());
    
	// subtract background
	bs.subtract(src, tmpImg);
    
    // generate a contours container scratch
    contours_t contours;
    
    // perform gray threshold
	Preprocessor::graythresh(tmpImg,gThresh,tmpImg);
    
    // calculate the contours
    Preprocessor::calcContours(tmpImg,contours);
    
    // check if contours overrun image borders (as well as ROI-borders, if ROI selected)
    Preprocessor::borderRestriction(contours, src, checkRoiBorders);

	// holds contours without inner crossings
	contours_t validatedContours;

	Preprocessor::eliminateAllInnerContourCrossings(contours, validatedContours);
    
    // filter the contours
	Preprocessor::sizethreshold(validatedContours, minSizeThresh, maxSizeThresh, acceptedContoursDst, biggerContoursDst);
	//Preprocessor::filterByFeatures(src, contours, minSizeThresh, maxSizeThresh, acceptedContoursDst, biggerContoursDst);

	/*
	################### TODO ##########################
	Valley enhancement does not work currently; errors occured after the implementation of the background subtraction.
	If valley enhancement can be fixed, do not forget to make the corresponding spinBox visible again (see MainGUI "hide valley threshold")!
	*/
	//// enhance the valleys (area between different larvae) inside of given contours
	//Preprocessor::enhanceValleys(src, tmpImg, acceptedContoursDst, biggerContoursDst, minSizeThresh, maxSizeThresh, valleyThresh);

	/*
	######### OPTIONAL #############
	splitBiggerContours is based on the assumption that the observed objects
	do not overlap and that collided contours can always be separated.
	Since this is not true for fish, this function is probably useless.
	*/
	//// try to split contours which are bigger than one larva
	//contours_t splittedContours;
	//Preprocessor::splitBiggerContours(src, splittedContours, biggerContoursDst, maxSizeThresh - minSizeThresh);

	//// filter the contours again
	//contours_t acceptedSplittedContours;
	//contours_t biggerSplittedContours;
	//Preprocessor::filterByFeatures(src, splittedContours, minSizeThresh, maxSizeThresh, acceptedSplittedContours, biggerSplittedContours);
	//acceptedContoursDst.insert(acceptedContoursDst.end(), acceptedSplittedContours.begin(), acceptedSplittedContours.end());
	//biggerContoursDst.insert(biggerContoursDst.end(), biggerSplittedContours.begin(), biggerSplittedContours.end());
}

void Preprocessor::splitBiggerContours(Mat const & src, contours_t & splittedContoursDst, contours_t & biggerContoursDst, int const meanLarvaSize)
{
	// ######### PARAMETERS, PARTLY REDUNDANT TO filterByFeatures ###############
	int offsetRange = 10; // offsetRange to find the best point to cut two larvae with 0 minima and 2 maxima
	int epsilon = 10;
	double distFactorInEpsilonRange = 0.8;
	double maxDiffToDirVec = 45.0; // maximum angle difference to the corresponding direction vector
	//int minDistBetweenMinima = 10; // number of pixels
	double minContourPortion = 0.3; // minimum portion of contour pixels which has to be distributed to each of the two resulting contours
	double maxCutLength = 20.0;
	double maxSpineLength = 55.0;

	contours_t biggerContours = biggerContoursDst;
	biggerContoursDst.clear();

	// iterate over all contours which are bigger than one larva
	for (contour_t bigContour : biggerContours)
	{
		int currContourVectorSize = bigContour.size();
		double currAreaSize = cv::contourArea(bigContour);

		if (currAreaSize > meanLarvaSize * 3)
		{
			// very big contours do not consist of only two objects; no further processing
			biggerContoursDst.push_back(bigContour);
		}
		else
		{
			SpineIPAN curvatureCalculator;
			FIMTypes::contourCurvature_t curvatures;
			FIMTypes::contourCurvVectors_t curvVectors;
			minima_t minimaIndices;
			maxima_t maximaIndices;
			// 1. find parts of the contour with high curvature (which ideally represent heads and tails)
			// 2. find parts of the contour with high reverse curvature (which ideally represent points contact between two colliding objects)
			curvatureCalculator.calcContourCurvatures(&bigContour, curvatures, curvVectors, maximaIndices, minimaIndices);

			// PROBLEM: This step currently also cuts too big but correct larvae
			if (minimaIndices.size() == 0 && maximaIndices.size() == 2)
			{
				int max1ToMax2Dist = abs(maximaIndices[1] - maximaIndices[0]);
				if (max1ToMax2Dist > currContourVectorSize * minContourPortion && max1ToMax2Dist < currContourVectorSize - currContourVectorSize * minContourPortion)
				{
					int cutIdx1 = Calc::calcCircularContourNeighbourIndex(currContourVectorSize, maximaIndices[0], max1ToMax2Dist / 2);
					int cutIdx2 = Calc::calcCircularContourNeighbourIndex(currContourVectorSize, maximaIndices[1], (currContourVectorSize - max1ToMax2Dist) / 2);

					double bestCurvature = curvatures[cutIdx1];
					int bestIdx = cutIdx1;
					for (int offset = -offsetRange; offset <= offsetRange; offset++)
					{
						int currIdx1 = Calc::calcCircularContourNeighbourIndex(currContourVectorSize, cutIdx1, offset);
						double currCurvature1 = curvatures[currIdx1];
						if (currCurvature1 < bestCurvature)
						{
							bestIdx = currIdx1;
							bestCurvature = currCurvature1;
						}
						int currIdx2 = Calc::calcCircularContourNeighbourIndex(currContourVectorSize, cutIdx2, offset);
						double currCurvature2 = curvatures[currIdx2];
						if (currCurvature2 < bestCurvature)
						{
							bestIdx = currIdx2;
							bestCurvature = currCurvature2;
						}
					}
					minimaIndices.push_back(bestIdx);
				}
				else
				{
					biggerContoursDst.push_back(bigContour);
				}
			}
			if (maximaIndices.size() > 5 || (minimaIndices.size() != 2 && minimaIndices.size() != 1))
			{
				biggerContoursDst.push_back(bigContour);
			}
			else
			{				
				if (minimaIndices.size() == 2)
				{
					bool anglesAllowConnection = checkIfConnectable(bigContour[minimaIndices[0]], bigContour[minimaIndices[1]], curvVectors[minimaIndices[0]], curvVectors[minimaIndices[1]], (maxSpineLength / 2.0), 20.0);
					if (!anglesAllowConnection || abs(minimaIndices[0] - minimaIndices[1]) < (int)(currContourVectorSize * minContourPortion) || abs(minimaIndices[0] - minimaIndices[1]) > currContourVectorSize - (int)(currContourVectorSize * minContourPortion))
					{
						if (curvatures[minimaIndices[0]] > curvatures[minimaIndices[1]])
						{
							minimaIndices = FIMTypes::minima_t({ minimaIndices[0] });
						}
						else
						{
							minimaIndices = FIMTypes::minima_t({ minimaIndices[1] });
						}						
					}
					else
					{
						contour_t c0;
						contour_t c1;
						performSimpleContourCut(src, bigContour, c0, c1, minimaIndices[0], minimaIndices[1]);

						splittedContoursDst.push_back(c0);
						splittedContoursDst.push_back(c1);
					}
				}
				if (minimaIndices.size() == 1)
				{
					int minIndex = minimaIndices[0];
					int oppositeIndex = Calc::calcCircularContourNeighbourIndex(currContourVectorSize, minIndex, (int)(currContourVectorSize / 2));
					int minDistBetweenMinima = (int)((double)currContourVectorSize * minContourPortion * 0.9);
					double smallestDistFound = Calc::eucledianDist(bigContour[minIndex], bigContour[oppositeIndex]);
					bool isIndexJumping = (Calc::calcCircularContourNeighbourIndex(currContourVectorSize, minIndex, -minDistBetweenMinima) > Calc::calcCircularContourNeighbourIndex(currContourVectorSize, minIndex, minDistBetweenMinima)) ? true : false;

					for (int i = 0; i < currContourVectorSize; ++i)
					{
						// make sure that the current i is distant to the first minima index
						bool isDistantToMinIndex = false;
						if (isIndexJumping)
						{
							isDistantToMinIndex = (i < Calc::calcCircularContourNeighbourIndex(currContourVectorSize, minIndex, -minDistBetweenMinima) && i > Calc::calcCircularContourNeighbourIndex(currContourVectorSize, minIndex, minDistBetweenMinima)) ? true : false;
						}
						else
						{
							isDistantToMinIndex = (i < minIndex - minDistBetweenMinima || i > minIndex + minDistBetweenMinima) ? true : false;
						}
						if (isDistantToMinIndex)
						{
							// find the most probable point on the other side of the contour by using the vector pointing inside the contour
							QPointF p1 = QPointF(curvVectors[minIndex][0], curvVectors[minIndex][1]);
							Point2d p2temp = bigContour[i] - bigContour[minIndex];
							Vec2d p2Vector = Vec2d(p2temp.x, p2temp.y);
							p2Vector = cv::normalize(p2Vector);
							QPointF p2 = QPointF(p2Vector[0], p2Vector[1]);
							double currAngle = Calc::calcInnerAngleOfVectors(p1, p2);
							if (currAngle > maxDiffToDirVec)
							{
								continue;
							}
							double currDist = Calc::eucledianDist(bigContour[minIndex], bigContour[i]);
							if (currAngle <= epsilon) {
								currDist *= distFactorInEpsilonRange;
							}
							if (currDist < smallestDistFound)
							{
								oppositeIndex = i;
								smallestDistFound = currDist;
							}							
						}
					}
					if (abs(minIndex - oppositeIndex) < (int)(currContourVectorSize * minContourPortion) || abs(minIndex - oppositeIndex) > currContourVectorSize - (int)(currContourVectorSize * minContourPortion))
					{
						oppositeIndex = Calc::calcCircularContourNeighbourIndex(currContourVectorSize, minIndex, (int)(currContourVectorSize / 2));
					}

					if (Calc::eucledianDist(bigContour[minIndex], bigContour[oppositeIndex]) <= maxCutLength)
					{
						contour_t c0;
						contour_t c1;
						performSimpleContourCut(src, bigContour, c0, c1, minIndex, oppositeIndex);

						splittedContoursDst.push_back(c0);
						splittedContoursDst.push_back(c1);
					}
					else
					{
						biggerContoursDst.push_back(bigContour);
					}
				}
			}
		}
	}
}

void Preprocessor::performContourCut(contour_t & srcContour, contour_t & dstContour1, contour_t & dstContour2, int idxStart, int idxEnd, vector<cv::Point> cutPoints)
{
	vector<int> cutInverval;
	// make sure the smaller index is added first so that the vector is in ascending order
	if (idxStart < idxEnd)
	{
		cutInverval.push_back(idxStart);
		cutInverval.push_back(idxEnd);
	}
	else
	{
		cutInverval.push_back(idxEnd);
		cutInverval.push_back(idxStart);
	}

	// currLarvaIndex specifies to which of the two resulting contours the points of the source contour should be added
	int currLarvaIndex = 0;
	// go over the whole source contour
	for (int i = 0; i < srcContour.size(); ++i)
	{
		// watch out for the index where to begin the cut
		if (currLarvaIndex == 0 && i == cutInverval[currLarvaIndex])
		{
			// pause adding to dstContour1 and start with dstContour2
			currLarvaIndex++;
		}
		// watch out for the index where to end the cut
		else if (currLarvaIndex == 1 && i == cutInverval[currLarvaIndex])
		{
			bool isorderAscending;
			if (cutPoints[0] == srcContour[cutInverval[0]])
			{
				isorderAscending = true;
			}
			else if (cutPoints[0] == srcContour[cutInverval[1]])
			{
				isorderAscending = false;
			}
			else
			{
				return;
			}
			// add the points which cut the contour into two parts (both resulting contours will contain the points)
			// or more precise: add all points between the two cut indices to dstContour2 (which is finished after that) and to dstContour1 (which is probably not finished yet)
			for (int j = 0; j < cutPoints.size(); ++j)
			{
				if (isorderAscending)
				{
					dstContour1.push_back(cutPoints[j]);
					dstContour2.push_back(cutPoints[cutPoints.size() - 1 - j]);
				}
				else
				{
					dstContour1.push_back(cutPoints[cutPoints.size() - 1 - j]);
					dstContour2.push_back(cutPoints[j]);
				}
			}
			currLarvaIndex++;
		}
		else if (currLarvaIndex == 0 || currLarvaIndex == 2)
		{
			// add points from the source contour to the first resulting contour
			dstContour1.push_back(srcContour[i]);
		}
		else if (currLarvaIndex == 1)
		{
			// add points from the source contour to the second resulting contour
			dstContour2.push_back(srcContour[i]);
		}
	}
}

void Preprocessor::performSimpleContourCut(Mat const & image, contour_t & srcContour, contour_t & dstContour1, contour_t & dstContour2, int idxStart, int idxEnd)
{
	// create a line between the two given points, ensuring that every point has two orthogonal neighbours
	LineIterator lineIter(image, srcContour[idxStart], srcContour[idxEnd], 8, false);
	std::vector<cv::Point> linePoints;
	for (int i = 0; i < lineIter.count; ++i)
	{
		linePoints.push_back(lineIter.pos());
		lineIter++;
	}
	// use the line to cut the contour into two parts
	Preprocessor::performContourCut(srcContour, dstContour1, dstContour2, idxStart, idxEnd, linePoints);
}

void Preprocessor::enhanceValleys(cv::Mat const & srcImg, cv::Mat const & threshImg, contours_t & acceptedContoursDst, contours_t & biggerContoursDst, int const minLarvaSize, int const maxLarvaSize, const int valleyThresh)
{
	// ############ PARAMETERS ##################
	int minSizeOfHoles = 20;
	int maxSizeOfHoles = 2000;
	int minDeadEndLength = 3;
	double defaultMinimaConnectorDistance = 2.25;
	double connectionEnhancerAngle = 60.0;

	// store all contours which seem to be one larva/fish only
	contours_t previousAcceptedContours = acceptedContoursDst;
	acceptedContoursDst.clear();

	// convolve the source image to improve the local contrast
	cv::Mat convolutionImg = srcImg.clone();
	cv::Mat convKernel = (Mat_<int>(5, 5) <<
		-1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1,
		-1, -1, 25, -1, -1,
		-1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1);
	filter2D(convolutionImg, convolutionImg, -1, convKernel);
	cv::normalize(convolutionImg, convolutionImg, 0, 255, NORM_MINMAX, CV_16S);

	// initialize the lookup image with bigger contours only
	// the lookup image is always updated so that intermediate results can be used for the next steps
	cv::Mat lookupImg = cv::Mat::zeros(srcImg.size(), srcImg.type());
	cv::cvtColor(lookupImg, lookupImg, cv::COLOR_GRAY2BGR);
	cv::drawContours(lookupImg, biggerContoursDst, -1, Scalar(1, 1, 1), CV_FILLED);

	// the holes image consists of all holes inside of contours
	cv::Mat holesImg = threshImg.clone();
	cv::cvtColor(holesImg, holesImg, cv::COLOR_GRAY2BGR);
	holesImg = holesImg.mul(lookupImg);
	holesImg = lookupImg - holesImg;

	// draw only those holes into the lookup image which are bigger than noise
	contours_t allHoleContours;
	cv::cvtColor(holesImg, holesImg, cv::COLOR_BGR2GRAY);
	Preprocessor::calcContours(holesImg, allHoleContours);
	borderRestriction(allHoleContours, holesImg, false);
	contours_t holeContoursFiltered;
	contours_t tooBigHoles;
	sizethreshold(allHoleContours, std::max((int)std::round(minLarvaSize / 20), 10), (minLarvaSize + 100) * 30, holeContoursFiltered, tooBigHoles);
	cv::drawContours(lookupImg, holeContoursFiltered, -1, Scalar(0, 0, 0), CV_FILLED);
	cv::cvtColor(lookupImg, lookupImg, cv::COLOR_BGR2GRAY);

	/*
	* FIRST ROUND: look for paths starting inside (walk along contour of holes)
	*/
	for (contour_t holeContour : holeContoursFiltered)
	{
		SpineIPAN curvatureCalculator;
		FIMTypes::contourCurvature_t curvatures;
		FIMTypes::contourCurvVectors_t curvVectors;
		minima_t minimaIndices;
		maxima_t maximaIndices;
		// 1. find parts of the contour with high curvature (which ideally represent heads and tails)
		// 2. find parts of the contour with high reverse curvature (which ideally represent points contact between two colliding objects)
		curvatureCalculator.calcContourCurvatures(&holeContour, curvatures, curvVectors, maximaIndices, minimaIndices);
		for (int cIdx = 0; cIdx < holeContour.size(); cIdx++)
		{
			Point startPoint = holeContour[cIdx];
			// all curvature vectors point inside, so for holes the vectors must be reverted
			Vec2d destVec2d = -curvVectors[cIdx];
			Point destPoint = Point(startPoint.x + (int)round(destVec2d[0]), startPoint.y + (int)round(destVec2d[1]));
			if (cv::pointPolygonTest(holeContour, destPoint, false) < 0)
			{
				path_t path = Preprocessor::findPath(convolutionImg, lookupImg, startPoint, destVec2d, valleyThresh, 0);
				for (Point pathPoint : path)
				{
					// enhance the path by blackening the corresponding pixels on the lookup image
					lookupImg.at<uchar>(pathPoint.y, pathPoint.x) = 0;
				}
			}
		}
	}

	/*
	* SECOND ROUND: look for paths starting outside the contour (near minimal curvatures only!)
	*/
	for (contour_t bigContour : biggerContoursDst)
	{
		int currContourVectorSize = bigContour.size();
		SpineIPAN curvatureCalculator;
		FIMTypes::contourCurvature_t curvatures;
		FIMTypes::contourCurvVectors_t curvVectors;
		minima_t minimaIndices;
		maxima_t maximaIndices;

		std::vector<int> connectedMinima;
		// 1. find parts of the contour with high curvature (which ideally represent heads and tails)
		// 2. find parts of the contour with high reverse curvature (which ideally represent points contact between two colliding objects)
		curvatureCalculator.calcContourCurvatures(&bigContour, curvatures, curvVectors, maximaIndices, minimaIndices);
		for (int minCurvIdx = 0; minCurvIdx < minimaIndices.size(); minCurvIdx++)
		{
			int currMinIdx = minimaIndices[minCurvIdx];
			Vec2d destVec2d = curvVectors[currMinIdx];
			Point startPoint = bigContour[currMinIdx];

			// don't look for paths if this minimum curvature is already connected
			if (std::find(connectedMinima.begin(), connectedMinima.end(), currMinIdx) != connectedMinima.end())
			{
				continue;
			}

			/*
			* before starting the path, look for another minimum nearby and check if a connection can be created
			*/
			bool connectionFound = false;
			for (int otherCurvIdx = minCurvIdx + 1; otherCurvIdx < minimaIndices.size(); otherCurvIdx++)
			{
				int otherMinIdx = minimaIndices[otherCurvIdx];
				Vec2d otherVec2d = curvVectors[otherMinIdx];
				Point otherPoint = bigContour[otherMinIdx];
				// check if a connection is allowed
				if (checkIfConnectable(startPoint, otherPoint, destVec2d, otherVec2d, defaultMinimaConnectorDistance, connectionEnhancerAngle))
				{
					path_t straightPath = createStraightPath(convolutionImg, lookupImg, startPoint, otherPoint);

					for (Point pathPoint : straightPath)
					{
						lookupImg.at<uchar>(pathPoint.y, pathPoint.x) = 0;
					}
					connectionFound = true;
					connectedMinima.push_back(currMinIdx);
					connectedMinima.push_back(otherMinIdx);
				}
			}
			if (connectionFound)
			{
				if (minimaIndices.size() < 3)
				{
					break;
				}
				else
				{
					continue;
				}
			}

			// try to find a path from each minCurvIdx and its direct neighbours
			for (float i = 0.0; i <= 2.0; i++)
			{
				int orderedOffset = ceil(i / 2) * pow(-1, i);
				int currCurvIdx = Calc::calcCircularContourNeighbourIndex(currContourVectorSize, currMinIdx, orderedOffset);

				path_t path = Preprocessor::findPath(convolutionImg, lookupImg, bigContour[currCurvIdx], curvVectors[currCurvIdx], valleyThresh, minDeadEndLength);

				for (Point pathPoint : path)
				{
					// enhance the path by blackening the corresponding pixels on the lookup image
					lookupImg.at<uchar>(pathPoint.y, pathPoint.x) = 0;
				}
			}
		}
	}

	/*
	* CHECK INTERMEDIATE RESULT: apply calcContours() on lookupImg and filter again using sizethreshold()
	*/
	cv::Mat intermediateImg = lookupImg.clone();
	contours_t intermediateContours;
	Preprocessor::calcContours(intermediateImg, intermediateContours);
	borderRestriction(intermediateContours, intermediateImg, false);
	Preprocessor::filterByFeatures(intermediateImg, intermediateContours, minLarvaSize, maxLarvaSize, acceptedContoursDst, biggerContoursDst);

	/*
	* THIRD ROUND: look for paths starting outside the contour (walk along exterior of contours)
	*/
	for (contour_t bigContour : biggerContoursDst)
	{
		/*for (int cIdx = 0; cIdx < bigContour.size(); cIdx++)
		{
			
		}*/

		int currContourVectorSize = bigContour.size();
		SpineIPAN curvatureCalculator;
		FIMTypes::contourCurvature_t curvatures;
		FIMTypes::contourCurvVectors_t curvVectors;
		minima_t minimaIndices;
		maxima_t maximaIndices;

		std::vector<int> connectedMinima;
		// 1. find parts of the contour with high curvature (which ideally represent heads and tails)
		// 2. find parts of the contour with high reverse curvature (which ideally represent points contact between two colliding objects)
		curvatureCalculator.calcContourCurvatures(&bigContour, curvatures, curvVectors, maximaIndices, minimaIndices);
		for (int minCurvIdx = 0; minCurvIdx < minimaIndices.size(); minCurvIdx++)
		{			
			int currMinIdx = minimaIndices[minCurvIdx];

			// TEST - show min curvatures
			Vec2d destVec2d = curvVectors[currMinIdx];
			Point startPoint = bigContour[currMinIdx];
			cv::Mat tempImg = srcImg.clone();
			cv::cvtColor(tempImg, tempImg, cv::COLOR_GRAY2BGR);
			tempImg.at<int>(startPoint.y - (int)round(destVec2d[1]), startPoint.x - (int)round(destVec2d[0]), 0) = 0;
			tempImg.at<int>(startPoint.y - (int)round(destVec2d[1] * 2), startPoint.x - (int)round(destVec2d[0] * 2), 0) = 0;
			tempImg.at<int>(startPoint.y - (int)round(destVec2d[1] * 3), startPoint.x - (int)round(destVec2d[0] * 3), 0) = 0;
			tempImg.at<int>(startPoint.y - (int)round(destVec2d[1] * 4), startPoint.x - (int)round(destVec2d[0] * 4), 0) = 0;
			tempImg.at<int>(startPoint.y - (int)round(destVec2d[1] * 5), startPoint.x - (int)round(destVec2d[0] * 5), 0) = 0;
			tempImg.at<int>(startPoint.y - (int)round(destVec2d[1] * 6), startPoint.x - (int)round(destVec2d[0] * 6), 0) = 0;
			tempImg.at<int>(startPoint.y - (int)round(destVec2d[1] * 7), startPoint.x - (int)round(destVec2d[0] * 7), 0) = 0;
			cv::cvtColor(tempImg, tempImg, cv::COLOR_BGR2GRAY);
			tempImg.copyTo(srcImg);

			// don't look for paths if this minimum curvature is already connected
			if (std::find(connectedMinima.begin(), connectedMinima.end(), currMinIdx) != connectedMinima.end())
			{
				continue;
			}

			/*
			* before starting the path, look for another minimum nearby and check if a connection can be created
			*/
			bool connectionFound = false;
			for (int otherCurvIdx = minCurvIdx + 1; otherCurvIdx < minimaIndices.size(); otherCurvIdx++)
			{
				int otherMinIdx = minimaIndices[otherCurvIdx];
				Vec2d otherVec2d = curvVectors[otherMinIdx];
				Point otherPoint = bigContour[otherMinIdx];
				// check if a connection is allowed
				if (checkIfConnectable(startPoint, otherPoint, destVec2d, otherVec2d, defaultMinimaConnectorDistance, connectionEnhancerAngle))
				{
					path_t straightPath = createStraightPath(convolutionImg, lookupImg, startPoint, otherPoint);

					for (Point pathPoint : straightPath)
					{
						// enhance the path by blackening the corresponding pixels on the lookup image
						lookupImg.at<uchar>(pathPoint.y, pathPoint.x) = 0;
					}
					connectionFound = true;
					connectedMinima.push_back(currMinIdx);
					connectedMinima.push_back(otherMinIdx);
				}
			}
			if (connectionFound)
			{
				if (minimaIndices.size() < 3)
				{
					break;
				}
				else
				{
					continue;
				}
			}
		}

		// for all contour points try to find a path
		for (int i = 0; i < bigContour.size(); i++)
		{
			if (curvatures[i] > 150)
			{
				path_t path = Preprocessor::findPath(convolutionImg, lookupImg, bigContour[i], curvVectors[i], valleyThresh, NO_DEAD_ENDS);

				for (Point pathPoint : path)
				{
					// enhance the path by blackening the corresponding pixels on the lookup image
					lookupImg.at<uchar>(pathPoint.y, pathPoint.x) = 0;
				}
			}
		}
	}


	/*
	* CHECK INTERMEDIATE RESULT: apply calcContours() on lookupImg and filter again using sizethreshold()
	* TODO: This step contains redundant parts! We do not need to calculate all contours again... (create new lookupImg?)
	*/

	intermediateImg = lookupImg.clone();

	Preprocessor::calcContours(intermediateImg, intermediateContours);
	borderRestriction(intermediateContours, intermediateImg, false);
	Preprocessor::filterByFeatures(intermediateImg, intermediateContours, minLarvaSize, maxLarvaSize, acceptedContoursDst, biggerContoursDst);

	// at last, add all contours which were already accepted before
	acceptedContoursDst.insert(acceptedContoursDst.end(), previousAcceptedContours.begin(), previousAcceptedContours.end());


	// ################## TEST TEST TEST TEST TEST TEST ##################
	// the following three lines highlight the contours to which the valley enhancement was applied
	lookupImg += 1.0;
	lookupImg = lookupImg.mul(srcImg/2);
	lookupImg.copyTo(srcImg);
}

path_t Preprocessor::findPath(cv::Mat const & convolutionImg, cv::Mat const & lookupImg, cv::Point & startPoint, cv::Vec2d & startVec, const int & valleyThresh, const int & minDeadEndLength)
{
	path_t path;
	// add the starting point as the first point of the path
	path.push_back(startPoint);
	// the starting vector specifies the direction where to look for the second point
	Vec2d prevVec2d = cv::normalize(startVec);
	Point prevPoint = startPoint;
	bool isSearching = true;
	/*
		The loop stops as soon as a border of the contour is found or when a dead end is reached
	*/
	while (isSearching) {
		// vector which points to the next discrete point (=>pixel)
		Vec2i destVec2i = Calc::calcDiscreteDirectionVector(prevVec2d);
		// vectors which point to the neighbours of the next pixel
		Vec2i leftNeighbour = Calc::calcCircularPixelNeighbourVector(GeneralParameters::pixelNeighbourhood8, destVec2i, 1);
		Vec2i rightNeighbour = Calc::calcCircularPixelNeighbourVector(GeneralParameters::pixelNeighbourhood8, destVec2i, -1);
		// determine the points where the vectors point at
		QPoint pLeft = QPoint(leftNeighbour[0], leftNeighbour[1]);
		QPointF pMid = QPointF(prevVec2d[0], prevVec2d[1]);
		QPoint pRight = QPoint(rightNeighbour[0], rightNeighbour[1]);
		// check if the original direction vector points to the left or to the right of the discrete mid point
		int rightTendency = Calc::calcInnerAngleOfVectors(pMid, pRight) < Calc::calcInnerAngleOfVectors(pLeft, pMid) ? 1 : -1;
		bool darkPixelFound = false;
		int darkPixelFoundValue = -1;
		// stores the "correct" pixels if there will be found any during this search step
		std::vector<Point> darkPixels;
		Vec2d darkPixelVec2d;
		// look for dark pixels, first in direction of the direction vector and, after that, check the neighbouring pixels
		for (float tempIdx = 0.0; tempIdx <= 4.0; tempIdx++)
		{
			// determine which pixel to look at
			int i = ceil(tempIdx / 2) * pow(-1, tempIdx) * rightTendency;
			Vec2i currVec2i = Calc::calcCircularPixelNeighbourVector(GeneralParameters::pixelNeighbourhood8, destVec2i, i);
			Point currPoint = Point(prevPoint.x + currVec2i[0], prevPoint.y + currVec2i[1]);
			// check if pixel is already black on the lookup image (which means it is a pixel at the border of the contour and the search can be stopped)
			if (lookupImg.at<uchar>(currPoint.y, currPoint.x) == 0)
			{
				// length of the path must be at least 2 or the direction of the found pixel may differ only little from the previous direction
				if (path.size() > 1 || abs(i) <= 1)
				{
					// path found! check if last step goes diagonal so that we need to darken an orthogonal pixel, too
					if (currVec2i[0] * currVec2i[1] != 0)
					{
						Point left = Calc::calcCircularPixelNeighbourPoint(GeneralParameters::pixelNeighbourhood8, currVec2i, prevPoint, 1);
						Point right = Calc::calcCircularPixelNeighbourPoint(GeneralParameters::pixelNeighbourhood8, currVec2i, prevPoint, -1);
						// add the point which is darker on the convolution image
						path.push_back((convolutionImg.at<int>(left.y, left.x, 0) % 256 < convolutionImg.at<int>(right.y, right.x, 0) % 256) ? left : right);
					}
					path.push_back(currPoint); // do we need this? (not answered yet) - currPoint should be black already, on the lookup image
					return path;
				}
			}
			// check if pixel value is below or equal to the defined valley threshold
			else if (!darkPixelFound && convolutionImg.at<int>(currPoint.y, currPoint.x, 0) % 256 <= valleyThresh)
			{
				// dark pixel found. check if current step goes diagonal so that we need to darken an orthogonal pixel, too
				if (currVec2i[0] * currVec2i[1] != 0)
				{
					Point left = Calc::calcCircularPixelNeighbourPoint(GeneralParameters::pixelNeighbourhood8, currVec2i, prevPoint, 1);
					Point right = Calc::calcCircularPixelNeighbourPoint(GeneralParameters::pixelNeighbourhood8, currVec2i, prevPoint, -1);
					// add the point which is darker on the convolution image
					darkPixels.push_back((convolutionImg.at<int>(left.y, left.x, 0) % 256 < convolutionImg.at<int>(right.y, right.x, 0) % 256) ? left : right);
				}
				darkPixels.push_back(currPoint);
				darkPixelFound = true;
				darkPixelFoundValue = convolutionImg.at<int>(currPoint.y, currPoint.x, 0) % 256;
				darkPixelVec2d = cv::normalize(prevVec2d * 2 + (Vec2d)currVec2i);
			}
			 // go along the minimum in the convolution image to avoide wrong paths (check if there is a "better"/darker pixel nearby in comparison to the already found pixel)
			else if (darkPixelFoundValue > 0 && abs(i) <= 1 && path.size() > 1 && convolutionImg.at<int>(currPoint.y, currPoint.x, 0) % 256 + (int)(valleyThresh / 10.0) < (int)(darkPixelFoundValue / (abs(i) + 1.0)))
			{
				// "better" pixel found, delete old one
				darkPixels.clear();
				// dark pixel found. check if current step goes diagonal so that we need to darken an orthogonal pixel, too
				if (currVec2i[0] * currVec2i[1] != 0)
				{
					Point left = Calc::calcCircularPixelNeighbourPoint(GeneralParameters::pixelNeighbourhood8, currVec2i, prevPoint, 1);
					Point right = Calc::calcCircularPixelNeighbourPoint(GeneralParameters::pixelNeighbourhood8, currVec2i, prevPoint, -1);
					// add the point which is darker on the convolution image
					darkPixels.push_back((convolutionImg.at<int>(left.y, left.x, 0) % 256 < convolutionImg.at<int>(right.y, right.x, 0) % 256) ? left : right);
				}
				darkPixels.push_back(currPoint);
				darkPixelFoundValue = convolutionImg.at<int>(currPoint.y, currPoint.x, 0) % 256;
				darkPixelVec2d = cv::normalize(prevVec2d * 2 + (Vec2d)currVec2i);
			}
		}

		// add all pixels to the path which were found this step
		// check if the found pixels are not part of the path yet - circles are not allowed since they can cause an infinite loop!
		if (darkPixelFound && std::find(path.begin(), path.end(), darkPixels[darkPixels.size() - 1]) == path.end())
		{
			path.insert(path.end(), darkPixels.begin(), darkPixels.end());
			prevVec2d = darkPixelVec2d;
			prevPoint = darkPixels[darkPixels.size() - 1];
		}
		else // (no contour border found, path is a dead end)
		{
			// delete paths shorter than minimum(2, minDeadEndLength) if they are a dead end
			if (path.size() < std::min(2, minDeadEndLength))
			{
				path.clear();
				isSearching = false;
			}
			else
			{
				// if path size (of a dead end) is long enough, path could probably be found looking for a black pixel in lookupImg using a bigger search range
				destVec2i *= 2;
				Vec2i newNeighbour = Calc::calcCircularPixelNeighbourVector(GeneralParameters::pixelNeighbourhood16, destVec2i, -rightTendency);
				if (Calc::calcInnerAngleOfVectors(QPoint(newNeighbour[0], newNeighbour[1]), pMid) < Calc::calcInnerAngleOfVectors(QPoint(destVec2i[0], destVec2i[1]), pMid))
				{
					destVec2i = newNeighbour;
				}
				leftNeighbour = Calc::calcCircularPixelNeighbourVector(GeneralParameters::pixelNeighbourhood16, destVec2i, 1);
				rightNeighbour = Calc::calcCircularPixelNeighbourVector(GeneralParameters::pixelNeighbourhood16, destVec2i, -1);
				pLeft = QPoint(leftNeighbour[0], leftNeighbour[1]);
				pRight = QPoint(rightNeighbour[0], rightNeighbour[1]);
				rightTendency = Calc::calcInnerAngleOfVectors(pMid, pRight) < Calc::calcInnerAngleOfVectors(pLeft, pMid) ? 1 : -1;
				Point rangedPoint;
				bool rangedPixelFound = false;
				// search range for the last pixel; if a ranged pixel (contour border) can be found, the dead and can be converted to a complete path
				for (float tempIdx = 0.0; tempIdx <= 6.0; tempIdx++)
				{
					int i = ceil(tempIdx / 2) * pow(-1, tempIdx) * rightTendency;
					Vec2i currVec2i = Calc::calcCircularPixelNeighbourVector(GeneralParameters::pixelNeighbourhood16, destVec2i, i);
					Point currPoint = Point(prevPoint.x + currVec2i[0], prevPoint.y + currVec2i[1]);
					// the desired point needs to be black on the lookup image (which represents a border of the contour)
					if (lookupImg.at<uchar>(currPoint.y, currPoint.x) == 0)
					{
						rangedPoint = currPoint;
						rangedPixelFound = true;
					}
				}

				if (rangedPixelFound)
				{
					// ranged pixel found; create a path between the previous pixel and the ranged pixel
					path_t straightPath = createStraightPath(convolutionImg, lookupImg, prevPoint, rangedPoint);
					if (straightPath.size() > 1)
					{
						path.insert(path.end(), straightPath.begin() + 1, straightPath.end());
					}
					// dead end was successfully converted to a full path and can be returned
					return path;
				}
				// value "-1" means, dead ends are not allowed and must be deleted
				// otherwise: delete paths shorter than minDeadEndLength if they are a dead end
				else if (minDeadEndLength == -1 || path.size() < minDeadEndLength)
				{
					path.clear();
					isSearching = false;
				}
				else
				{
					// case: a dead end which is long enough to be returned
					isSearching = false;
				}
			}
		}
	}
	return path;
}

path_t Preprocessor::createStraightPath(cv::Mat const & convolutionImg, cv::Mat const & lookupImg, cv::Point & startPoint, cv::Point & endPoint)
{
	path_t straightPath;
	straightPath.push_back(startPoint);
	if (startPoint == endPoint)
	{
		return straightPath;
	}
	cv::Point prevPoint = startPoint;
	// create a line between the starting point and the endpoint
	LineIterator lineIter(lookupImg, startPoint, endPoint, 8, false);
	lineIter++;
	for (int i = 1; i < lineIter.count; i++)
	{
		Point currPoint = lineIter.pos();
		Vec2i currVec2i = Vec2i(currPoint.x - prevPoint.x, currPoint.y - prevPoint.y);
		// check if last step goes diagonal so that we need to darken an orthogonal pixel, too
		if (currVec2i[0] * currVec2i[1] != 0)
		{
			Point left = Calc::calcCircularPixelNeighbourPoint(GeneralParameters::pixelNeighbourhood8, currVec2i, prevPoint, 1);
			Point right = Calc::calcCircularPixelNeighbourPoint(GeneralParameters::pixelNeighbourhood8, currVec2i, prevPoint, -1);
			// add the orthogonal pixel only if both neighbours are not black on the lookup image yet
			if (lookupImg.at<uchar>(left.y, left.x) != 0 && lookupImg.at<uchar>(right.y, right.x) != 0)
			{
				straightPath.push_back((convolutionImg.at<int>(left.y, left.x, 0) % 256 < convolutionImg.at<int>(right.y, right.x, 0) % 256) ? left : right);
			}
		}
		straightPath.push_back(currPoint);
		prevPoint = currPoint;
		lineIter++;
	}
	return straightPath;
}

bool Preprocessor::checkIfConnectable(cv::Point & point1, cv::Point & point2, cv::Vec2d & curvVec1, cv::Vec2d & curvVec2, const double & defaultMinimaConnectorDistance, const double & connectionEnhancerAngle)
{
	if (point1 == point2)
	{
		return true;
	}
	Point connectorVec = point2 - point1;
	double angle1 = Calc::calcAngle((Point2f)curvVec1, (Point2f)connectorVec);
	double angle2 = Calc::calcAngle((Point2f)curvVec2, (Point2f)-connectorVec);
	// check if the vectors of two points are pointing to each other and are close enough
	// ########### TODO: add user defined parameters instead of fixed values? ###############
	if (angle1 < 110.0 && angle2 < 110.0 || (max(angle1, angle2) < 150 && min(angle1, angle2) < connectionEnhancerAngle))
	{
		double connectDist = defaultMinimaConnectorDistance;
		if (angle1 < connectionEnhancerAngle)
		{
			// first vector is roughly pointing to the second point; allowed connector distance is increased
			connectDist += defaultMinimaConnectorDistance / 2;
		}
		if (angle2 < connectionEnhancerAngle)
		{
			// second vector is roughly pointing to the first point; allowed connector distance is increased
			connectDist += defaultMinimaConnectorDistance / 2;
		}
		return (Calc::eucledianDist(point1, point2) < connectDist) ? true : false;
	}
	else
	{
		return false;
	}
}

int Preprocessor::countNeighboursUsingThresh(cv::Mat const & img, cv::Point2i point, int const squareLength, int const threshold, bool useBelowThresh)
{
	int halfLength = (int)((squareLength - 1) / 2);
	int pixelCount = 0;
	// go through all points of the square ensuring all pixels to be inside the image borders
	for (int y = max(0, point.y - halfLength); y <= min(img.size().height - 1, point.y + halfLength); y++)
	{
		for (int x = max(0, point.x - halfLength); x <= min(img.size().width - 1, point.x + halfLength); x++)
		{
			if (useBelowThresh)
			{
				// Mat.at() uses x and y the other way round
				if (img.at<int>(y, x, 0) % 256 <= threshold)
				{
					pixelCount++;
				}
			}
			else
			{
				// Mat.at() uses x and y the other way round
				if (img.at<int>(y, x, 0) % 256 >= threshold)
				{
					pixelCount++;
				}
			}
		}
	}
	return pixelCount;
}

void Preprocessor::eliminateAllInnerContourCrossings(contours_t & srcContours, contours_t & validatedContoursDst)
{
	for (int i = 0; i < srcContours.size(); i++)
	{
		eliminateInnerContourCrossings(srcContours.at(i), validatedContoursDst);
	}
}

void Preprocessor::eliminateInnerContourCrossings(contour_t & srcContour, contours_t & validatedContoursDst)
{
	// break condition for too small contours
	int contLength = srcContour.size();
	if (cv::contourArea(srcContour) < GeneralParameters::iMinLarvaeArea || contLength < 10)
	{
		return;
	}

	// go through all points of the contour
	for (int i = 0; i < contLength; i++)
	{
		// check distance to all canditate points of the contour
		for (int j = i + 3; j < contLength - 3; j++)
		{
			int pixelDistance = std::max(std::abs(srcContour.at(j).x - srcContour.at(i).x), std::abs(srcContour.at(j).y - srcContour.at(i).y));
			// distance too small - split contour in two parts and validate both again
			if (pixelDistance < 2)
			{
				contour_t c1;
				contour_t c2;
				performContourCut(srcContour, c1, c2, i, j, { srcContour.at(i), srcContour.at(j) });
				eliminateInnerContourCrossings(c1, validatedContoursDst);
				eliminateInnerContourCrossings(c2, validatedContoursDst);
				return;
			}
			else
			{
				// second point is sufficently distant - increase iterator
				j += pixelDistance - 2;
			}
		}
	}
	// no crossing found - add contour to validated contours
	validatedContoursDst.push_back(srcContour);
}