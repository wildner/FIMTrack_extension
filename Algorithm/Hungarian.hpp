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

#ifndef HUNGARIAN_HPP
#define HUNGARIAN_HPP

#include "Configuration/FIMTrack.hpp"
#include <vector>

namespace Algorithms
{    
    typedef enum 
    {
        HUNGARIAN_MODE_MINIMIZE_COST,
        HUNGARIAN_MODE_MAXIMIZE_UTIL
    }MODE;
    
    /**
     * @brief The Hungarian class
     * Implementation of the hungarian algorithm based on http://csclab.murraystate.edu/bob.pilgrim/445/munkres.html
     */
    class Hungarian
    {
    private:
        
        static const bool                               DEBUGMODE       = false;
        
        static const uchar                              NONE            = 0;
        static const uchar                              STAR            = 1;
        static const uchar                              PRIME           = 2;
        
        static const int                                COLNOTFOUND     = -2;
        static const int                                ROWNOTFOUND     = -3;
        
        bool                                            _isInit;
        
        double                                          _cost;
        
        // The greatest cost in the initial cost matrix.
        double                                          _maxCost;
        
        // The size of the problem
        int                                             _rows;
        int                                             _cols;
        
        // The original cost matrix
        cv::Mat                                         _costMatrixOriginal;
        
        // The working cost matrix
        cv::Mat                                         _costMatrix;
        
        std::vector<std::pair<int, int> >               _assignment;
        
        std::vector<std::pair<int, int> >               _path;
        
        // The marks (star/prime/none) on each element of the cost matrix.
        cv::Mat                                         _marks;
        
        // Which rows and columns are currently covered.
        std::vector<bool>                               _rowsCovered;
        std::vector<bool>                               _colsCovered;
        
        // The number of stars in each column.
        std::vector<int>                                _starsInCol;
        
        /**
         * @brief init
         * @param inputMatrix
         * @param mode
         */
        void                                            init(cv::Mat_<double> const& inputMatrix, MODE mode);
        
        void                                            padMatrix(double padValue = 0.0);
        
        /**
         * @brief rowCovered
         * Is row 'row' covered?
         * @param row
         * @return 
         */
        bool                                            isRowCovered(int row)               const;
        
        /**
         * @brief coverRow
         * Cover row 'row'.
         * @param row
         */
        void                                            coverRow(int row);
        
        /**
         * @brief uncoverRow
         * Uncover row 'row'.
         * @param row
         */
        void                                            uncoverRow(int row);
        
        /**
         * @brief ColCovered
         * Is column col covered?
         * @param col
         * @return 
         */
        bool                                            isColCovered(int col)                           const;
        
        /**
         * @brief coverCol
         * Cover column col.
         * @param col
         */
        void                                            coverCol(int col);
        
        /**
         * @brief uncoverCol
         * Uncover column col.
         * @param col
         */
        void                                            uncoverCol(int col);
        
        /**
         * @brief clearCovers
         * Uncovery ever row and column in the matrix.
         */
        void                                            clearCovers();
        
        /**
         * @brief isStarred
         * Is the cell (row, col) starred?
         * @param row
         * @param col
         * @return 
         */
        bool                                            isZeroStarred(int row, int col)                 const;
        
        /**
         * @brief star
         * Mark cell (row, col) with a star
         * @param row
         * @param col
         */ 
        void                                            starZero(int row, int col);
        
        /**
         * @brief unStar
         * Remove a star from cell (row, col)
         * @param row
         * @param col
         */
        void                                            unStarZero(int row, int col);
        
        /**
         * @brief ColContainsStar
         * Does column col contain a star?
         * @param col
         * @return 
         */
        bool                                            colContainsStarredZero(int col)                 const;
        
        /**
         * @brief HungarianOptimizer::FindZero
         * Find an uncovered zero and store its coordinates in (zeroRow, zeroCol)
         * and return true, or return false if no such cell exists.
         * @param zeroRow
         * @param zeroCol
         * @return 
         */
        bool                                            findUncoveredZero(int& zeroRow, int& zeroCol)   const;
        
        /**
         * @brief IsPrimed
         * Is cell (row, col) marked with a prime?
         * @param row
         * @param col
         * @return 
         */
        bool                                            isZeroPrimed(int row, int col)                  const;
        
        /**
         * @brief Prime
         * Mark cell (row, col) with a prime.
         * @param row
         * @param col
         */
        void                                            primeZero(int row, int col);
        
        /**
         * @brief unPrimeZero
         * Remove a prime from cell (row, col)
         * @param row
         * @param col
         */
        void                                            unPrimeZero(int row, int col);
        
        /**
         * @brief FindPrimeInRow
         * Find a column in row containing a prime, or return
         * COLNOTFOUND if no such column exists.
         * @param row
         * @return 
         */
        int                                             findPrimedZeroInRow(int row)                    const;
        
        /**
         * @brief ClearPrimes
         * Remove the prime marks from every cell in the matrix.
         */
        void                                            clearPrimes();
        
        /**
         * @brief FindStarInRow
         * Find a column in row 'row' containing a star, or return
         * COLNOTFOUND if no such column exists.
         * @param row
         * @return 
         */
        int                                             findStarInRow(int row)                          const;
        
        /**
         * @brief FindStarInCol
         * Find a row in column 'col' containing a star, or return
         * ROWNOTFOUND if no such row exists.
         * @param col
         * @return 
         */
        int                                             findStarInCol(int col)                          const;
        
        void                                            augmentPath(const int pathLength);
        
        /**
         * @brief FindSmallestUncovered
         * Find the smallest uncovered cell in the costmatrix.
         * @return 
         */
        double                                          findSmallestUncoveredValue()                    const;
        
        /**
         * @brief findAssignments
         * Convert the final cost matrix into a set of assignments
         */
        void                                            findAssignments();
        
        void                                            showCostMatrix();
        
        void                                            showMaskMatrix();
        
        void                                            showAssignment();
        
        /**
         * @brief stepOne
         * Step 1.
         * For each row of the matrix, find the smallest element and subtract it
         * from every element in its row.  Go to Step 2.
         * @return 2 => Go to Step 2.
         */
        int                                             stepOne();
        
        /**
         * @brief stepTwo
         * Find a zero (Z) in the resulting matrix.  If there is no starred zero 
         * in its row or column, star Z. Repeat for each element in the matrix. 
         * Go to Step 3.
         * @return 
         */
        int                                             stepTwo();
        
        /**
         * @brief stepThree
         * Cover each column containing a starred zero. If all columns are covered, 
         * the starred zeros describe a complete set of unique assignments.  
         * In this case, terminate the algorithm, otherwise, Go to Step 4.
         * @return 
         */
        int                                             stepThree();
        
        /**
         * @brief stepFour
         * Find a noncovered zero and prime it. If there is no starred zero in 
         * the row containing this primed zero, Go to Step 5. Otherwise, cover 
         * this row and uncover the column containing the starred zero. 
         * Continue in this manner until there are no uncovered zeros left. 
         * Save the smallest uncovered value and Go to Step 6.
         * @return 
         */
        int                                             stepFour();
        
        /**
         * @brief stepFive
         * Construct a series of alternating primed and starred zeros as follows. 
         * Let Z0 represent the uncovered primed zero found in Step 4. Let Z1 denote 
         * the starred zero in the column of Z0 (if any). Let Z2 denote the primed 
         * zero in the row of Z1 (there will always be one). Continue until the 
         * series terminates at a primed zero that has no starred zero in its column. 
         * Unstar each starred zero of the series, star each primed zero of the series, 
         * erase all primes and uncover every line in the matrix. 
         * Return to Step 3.
         * @return 
         */
        int                                             stepFive();
        
        /**
         * @brief stepSix
         * Add the smallest uncovered value in the costmatrix to every element of each 
         * covered row, and subtract it from every element of each uncovered column.
         * Return to Step 4 without altering any stars, primes, or covered lines.
         * @return 
         */
        int                                             stepSix();
        
        void                                            stepSeven();
        
        void                                            runMunkres();
        
    public:
        /** This method initialize the hungarian_problem structure and init 
          *  the  cost matrices (missing lines or columns are filled with 0).
          *  It returns the size of the quadratic(!) assignment matrix. 
        **/
        
        Hungarian();
        Hungarian(cv::Mat_<double> const& inputMatrix, 
                  MODE mode);
        
        
        /** This method computes the optimal assignment. **/
        void                                            solve(cv::Mat_<double> const& inputMatrix, 
                                                              MODE mode);
        
        /** Accessor for the cost **/
        double cost()                                              const;
        
        /** Reference accessor for assignment **/
        std::vector<std::pair<int, int> >   const&      getAssignment()                                        const;
        
        /** Reference accessor for assignment **/
        cv::Mat      getAssignmentAsMatrix();
    };
}
#endif // HUNGARIAN_HPP
