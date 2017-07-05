///////////////////////////////   eispack.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    29.04.99    Peter Reichert
// revisions:   
//
//////////////////////////////////////////////////////////////////////////////
//
// "eispack" is a C++ translation of the FORTRAN eispack package
// =============================================================
//
//////////////////////////////////////////////////////////////////////////////
//
// Note: The following comments are the original comments of the FORTRAN
// ----- package. Due to the different range of indices of arrays in C and
//       FORTRAN all indices have to be reduced by one for the C++ versions.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef EISPACK_H
#define EISPACK_H

#ifndef USE_TYPES_H
#include "all.h"
#else
#include "types.h"
#endif

//////////////////////////////////////////////////////////////////////////////

void rs(INTEGER nm, INTEGER n, REAL* a, INTEGER matz, 
	REAL* w, REAL* z, INTEGER& ierr);

	// ***BEGIN PROLOGUE  RS
	// ***DATE WRITTEN   760101   (YYMMDD)
	// ***REVISION DATE  830518   (YYMMDD)
	// ***CATEGORY NO.  D4A1
	// ***KEYWORDS  EIGENVALUES,EIGENVECTORS,EISPACK
	// ***AUTHOR  SMITH, B. T., ET AL.
	// ***PURPOSE  Computes eigenvalues and, optionally, eigenvectors of
	//            real symmetric matrix.
	// ***DESCRIPTION
	
	//     This subroutine calls the recommended sequence of
	//     subroutines from the eigensystem subroutine package (EISPACK)
	//     to find the eigenvalues and eigenvectors (if desired)
	//     of a REAL SYMMETRIC matrix.
	
	//     On Input
	
	//        NM  must be set to the row dimension of the two-dimensional
	//        array parameters as declared in the calling program
	//        dimension statement.
	
	//        N  is the order of the matrix  A.
	
	//        A  contains the real symmetric matrix.
	
	//        MATZ  is an integer variable set equal to zero if
	//        only eigenvalues are desired.  Otherwise it is set to
	//        any non-zero integer for both eigenvalues and eigenvectors.
	
	//     On Output
	
	//        W  contains the eigenvalues in ascending order.
	
	//        Z  contains the eigenvectors if MATZ is not zero.
	
	//        IERR  is an integer output variable set equal to an
	//        error completion code described in section 2B of the
	//        documentation.  The normal completion code is zero.
	
	//     Questions and comments should be directed to B. S. Garbow,
	//     APPLIED MATHEMATICS DIVISION, ARGONNE NATIONAL LABORATORY
	//     ------------------------------------------------------------------
	// ***REFERENCES  B. T. SMITH, J. M. BOYLE, J. J. DONGARRA, B. S. GARBOW,
	//                 Y. IKEBE, V. C. KLEMA, C. B. MOLER, *MATRIX EIGEN-
	//                 SYSTEM ROUTINES - EISPACK GUIDE*, SPRINGER-VERLAG,
	//                 1976.
	// ***ROUTINES CALLED  TQL2,TQLRAT,TRED1,TRED2
	// ***END PROLOGUE  RS
	
//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////



