/////////////////////////////////   gamma.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    16.10.00    Peter Reichert
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#ifndef GAMMA_H
#define GAMMA_H

#ifndef USE_TYPES_H
#include "all.h"
#else
#include "types.h"
#endif

//////////////////////////////////////////////////////////////////////////////

BOOLEAN gamcdf(REAL x, REAL gamma, REAL* cdf, std::ostream* cjob=0);

	//     PURPOSE--THIS SUBROUTINE COMPUTES THE CUMULATIVE DISTRIBUTION
	//              FUNCTION VALUE FOR THE GAMMA
	//              DISTRIBUTION WITH SINGLE PRECISION 
	//              TAIL LENGTH PARAMETER = GAMMA.
	//              THE GAMMA DISTRIBUTION USED
	//              HEREIN HAS MEAN = GAMMA
	//              AND STANDARD DEVIATION = SQRT(GAMMA).
	//              THIS DISTRIBUTION IS DEFINED FOR ALL POSITIVE X,
	//              AND HAS THE PROBABILITY DENSITY FUNCTION
	//              F(X) = (1/CONSTANT) * (X**(GAMMA-1)) * EXP(-X)
	//              WHERE THE CONSTANT = THE GAMMA FUNCTION EVALUATED
	//              AT THE VALUE GAMMA.
	//     INPUT  ARGUMENTS--X      = THE SINGLE PRECISION VALUE 
	//                                AT WHICH THE CUMULATIVE DISTRIBUTION 
	//                                FUNCTION IS TO BE EVALUATED.
	//                                X SHOULD BE POSITIVE.
	//                     --GAMMA  = THE SINGLE PRECISION VALUE 
	//                                OF THE TAIL LENGTH PARAMETER.
	//                                GAMMA SHOULD BE POSITIVE.
	//     OUTPUT ARGUMENTS--CDF    = THE SINGLE PRECISION CUMULATIVE
	//                                DISTRIBUTION FUNCTION VALUE.
	//     OUTPUT--THE SINGLE PRECISION CUMULATIVE DISTRIBUTION
	//             FUNCTION VALUE CDF FOR THE GAMMA DISTRIBUTION 
	//             WITH TAIL LENGTH PARAMETER VALUE = GAMMA.
	//     PRINTING--NONE UNLESS AN INPUT ARGUMENT ERROR CONDITION EXISTS. 
	//     RESTRICTIONS--GAMMA SHOULD BE POSITIVE.
	//                 --X SHOULD BE POSITIVE.
	//     OTHER DATAPAC   SUBROUTINES NEEDED--NONE.
	//     FORTRAN LIBRARY SUBROUTINES NEEDED--DEXP, DLOG.
	//     MODE OF INTERNAL OPERATIONS--DOUBLE PRECISION.
	//     LANGUAGE--ANSI FORTRAN. 
	//     ACCURACY--(ON THE UNIVAC 1108, EXEC 8 SYSTEM AT NBS)
	//               COMPARED TO THE KNOWN GAMMA = 1 (EXPONENTIAL)
	//               RESULTS, AGREEMENT WAS HAD OUT TO 7 SIGNIFICANT
	//               DIGITS FOR ALL TESTED X.
	//               THE TESTED X VALUES COVERED THE ENTIRE
	//               RANGE OF THE DISTRIBUTION--FROM THE 0.00001 
	//               PERCENT POINT UP TO THE 99.99999 PERCENT POINT
	//               OF THE DISTRIBUTION.
	//     REFERENCES--WILK, GNANADESIKAN, AND HUYETT, 'PROBABILITY
	//                 PLOTS FOR THE GAMMA DISTRIBUTION',
	//                 TECHNOMETRICS, 1962, PAGES 1-15,
	//                 ESPECIALLY PAGES 3-5. 
	//               --NATIONAL BUREAU OF STANDARDS APPLIED MATHEMATICS
	//                 SERIES 55, 1964, PAGE 257, FORMULA 6.1.41.
	//               --JOHNSON AND KOTZ, CONTINUOUS UNIVARIATE
	//                 DISTRIBUTIONS--1, 1970, PAGES 166-206.
	//               --HASTINGS AND PEACOCK, STATISTICAL
	//                 DISTRIBUTIONS--A HANDBOOK FOR
	//                 STUDENTS AND PRACTITIONERS, 1975,
	//                 PAGES 68-73.
	//     WRITTEN BY--JAMES J. FILLIBEN
	//                 STATISTICAL ENGINEERING LABORATORY (205.03)
	//                 NATIONAL BUREAU OF STANDARDS
	//                 WASHINGTON, D. C. 20234
	//                 PHONE:  301-921-2315
	//     ORIGINAL VERSION--NOVEMBER  1975. 


//////////////////////////////////////////////////////////////////////////////

BOOLEAN gamppf(REAL p, REAL gamma, REAL* ppf, std::ostream* cjob=0);

	//     PURPOSE--THIS SUBROUTINE COMPUTES THE PERCENT POINT
	//              FUNCTION VALUE FOR THE GAMMA DISTRIBUTION
	//              WITH SINGLE PRECISION
	//              TAIL LENGTH PARAMETER = GAMMA.
	//              THE GAMMA DISTRIBUTION USED
	//              HEREIN HAS MEAN = GAMMA
	//              AND STANDARD DEVIATION = SQRT(GAMMA).
	//              THIS DISTRIBUTION IS DEFINED FOR ALL POSITIVE X,
	//              AND HAS THE PROBABILITY DENSITY FUNCTION
	//              F(X) = (1/CONSTANT) * (X**(GAMMA-1)) * EXP(-X)
	//              WHERE THE CONSTANT = THE GAMMA FUNCTION EVALUATED
	//              AT THE VALUE GAMMA.
	//              NOTE THAT THE PERCENT POINT FUNCTION OF A DISTRIBUTION 
	//              IS IDENTICALLY THE SAME AS THE INVERSE CUMULATIVE
	//              DISTRIBUTION FUNCTION OF THE DISTRIBUTION.
	//     INPUT  ARGUMENTS--P      = THE SINGLE PRECISION VALUE 
	//                                (BETWEEN 0.0 (EXCLUSIVELY) 
	//                                AND 1.0 (EXCLUSIVELY))
	//                                AT WHICH THE PERCENT POINT 
	//                                FUNCTION IS TO BE EVALUATED.
	//                     --GAMMA  = THE SINGLE PRECISION VALUE OF THE
	//                                TAIL LENGTH PARAMETER.
	//                                GAMMA SHOULD BE POSITIVE.
	//     OUTPUT ARGUMENTS--PPF    = THE SINGLE PRECISION PERCENT
	//                                POINT FUNCTION VALUE.
	//     OUTPUT--THE SINGLE PRECISION PERCENT POINT FUNCTION . 
	//             VALUE PPF FOR THE GAMMA DISTRIBUTION
	//             WITH TAIL LENGTH PARAMETER VALUE = GAMMA.
	//     PRINTING--NONE UNLESS AN INPUT ARGUMENT ERROR CONDITION EXISTS. 
	//     RESTRICTIONS--GAMMA SHOULD BE POSITIVE.
	//                 --P SHOULD BE BETWEEN 0.0 (EXCLUSIVELY)
	//                   AND 1.0 (EXCLUSIVELY).
	//     OTHER DATAPAC   SUBROUTINES NEEDED--NONE.
	//     FORTRAN LIBRARY SUBROUTINES NEEDED--DEXP, DLOG.
	//     MODE OF INTERNAL OPERATIONS--DOUBLE PRECISION.
	//     LANGUAGE--ANSI FORTRAN. 
	//     ACCURACY--(ON THE UNIVAC 1108, EXEC 8 SYSTEM AT NBS)
	//               COMPARED TO THE KNOWN GAMMA = 1 (EXPONENTIAL)
	//               RESULTS, AGREEMENT WAS HAD OUT TO 6 SIGNIFICANT
	//               DIGITS FOR ALL TESTED P IN THE RANGE P = .001 TO
	//               P = .999.  FOR P = .95 AND SMALLER, THE AGREEMENT
	//               WAS EVEN BETTER--7 SIGNIFICANT DIGITS.
	//               (NOTE THAT THE TABULATED VALUES GIVEN IN THE WILK,
	//               GNANADESIKAN, AND HUYETT REFERENCE BELOW, PAGE 20,
	//               ARE IN ERROR FOR AT LEAST THE GAMMA = 1 CASE--
	//               THE WORST DETECTED ERROR WAS AGREEMENT TO ONLY 3
	//               SIGNIFICANT DIGITS (IN THEIR 8 SIGNIFICANT DIGIT TABLE)
	//               FOR P = .999.)
	//     REFERENCES--WILK, GNANADESIKAN, AND HUYETT, 'PROBABILITY
	//                 PLOTS FOR THE GAMMA DISTRIBUTION',
	//                 TECHNOMETRICS, 1962, PAGES 1-15,
	//                 ESPECIALLY PAGES 3-5. 
	//               --NATIONAL BUREAU OF STANDARDS APPLIED MATHEMATICS
	//                 SERIES 55, 1964, PAGE 257, FORMULA 6.1.41.
	//               --JOHNSON AND KOTZ, CONTINUOUS UNIVARIATE
	//                 DISTRIBUTIONS--1, 1970, PAGES 166-206.
	//               --HASTINGS AND PEACOCK, STATISTICAL
	//                 DISTRIBUTIONS--A HANDBOOK FOR
	//                 STUDENTS AND PRACTITIONERS, 1975,
	//                 PAGES 68-73.
	//     WRITTEN BY--JAMES J. FILLIBEN
	//                 STATISTICAL ENGINEERING LABORATORY (205.03)
	//                 NATIONAL BUREAU OF STANDARDS
	//                 WASHINGTON, D. C. 20234
	//                 PHONE:  301-921-2315
	//     ORIGINAL VERSION--NOVEMBER  1974. 
	//     UPDATED         --SEPTEMBER 1975. 
	//     UPDATED         --NOVEMBER  1975. 

//////////////////////////////////////////////////////////////////////////////

double gamma(REAL x);

	// This routine calculates the GAMMA function for a real argument X.
	//   Computation is based on an algorithm outlined in reference 1.
	//   The program uses rational functions that approximate the GAMMA
	//   function to at least 20 significant decimal digits.  Coefficients
	//   for the approximation over the interval (1,2) are unpublished.
	//   Those for the approximation for X .GE. 12 are from reference 2.
	//   The accuracy achieved depends on the arithmetic system, the
	//   compiler, the intrinsic functions, and proper selection of the
	//   machine-dependent constants.

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
