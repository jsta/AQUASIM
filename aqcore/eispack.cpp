///////////////////////////////   eispack.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    29.04.99    Peter Reichert
// revisions:   
//
//////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "fortran.h"
#include "eispack.h"

//////////////////////////////////////////////////////////////////////////////

// forward declarations of auxiliary routines:

void tqlrat(INTEGER n, REAL* d, REAL* e2, INTEGER& ierr);

void tred1(INTEGER nm, INTEGER n, REAL* a, REAL* d, REAL* e, REAL* e2);

void tred2(INTEGER nm, INTEGER n, REAL* a, REAL* d, REAL* e, REAL* z);

void tql2(INTEGER nm, INTEGER n, REAL* d, REAL* e, REAL* z, INTEGER& ierr);

REAL pythag(REAL a, REAL b);

//////////////////////////////////////////////////////////////////////////////

void rs(INTEGER nm, INTEGER n, REAL* a, INTEGER matz, 
	REAL* w, REAL* z, INTEGER& ierr)
{
#define A(I_,J_)	(*(a+(I_)*(nm)+(J_)))
#define Z(I_,J_)	(*(z+(I_)*(nm)+(J_)))

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
	
	//        FV1  and  FV2  are temporary storage arrays.
	
	//     Questions and comments should be directed to B. S. Garbow,
	//     APPLIED MATHEMATICS DIVISION, ARGONNE NATIONAL LABORATORY
	//     ------------------------------------------------------------------
	// ***REFERENCES  B. T. SMITH, J. M. BOYLE, J. J. DONGARRA, B. S. GARBOW,
	//                 Y. IKEBE, V. C. KLEMA, C. B. MOLER, *MATRIX EIGEN-
	//                 SYSTEM ROUTINES - EISPACK GUIDE*, SPRINGER-VERLAG,
	//                 1976.
	// ***ROUTINES CALLED  TQL2,TQLRAT,TRED1,TRED2
	// ***END PROLOGUE  RS
	
	REAL* fv1 = new REAL[nm];
	REAL* fv2 = new REAL[nm];

	// ***FIRST EXECUTABLE STATEMENT  RS
	if( n <= nm ) 
		goto L_10;
	ierr = 10*n;
	goto L_50;
	
L_10:
	if( matz != 0 ) 
		goto L_20;
	//     .......... FIND EIGENVALUES ONLY ..........
	tred1( nm, n, a, w, fv1, fv2 );
	tqlrat( n, w, fv2, ierr );
	goto L_50;
	//     .......... FIND BOTH EIGENVALUES AND EIGENVECTORS ..........
L_20:
	tred2( nm, n, a, w, fv1, z );
	tql2( nm, n, w, fv1, z, ierr );
L_50:
	delete fv1;
	delete fv2;

	return;
#undef	Z
#undef	A
} 

//////////////////////////////////////////////////////////////////////////////

static REAL machep = 1.0e0;

void tqlrat(INTEGER n, REAL* d, REAL* e2, INTEGER& ierr)
{
	INTEGER i, i_, ii, ii_, j, l, l1, l_, m, m_, mml;
	REAL b, c, f, g, h, p, r, s;

	// ***BEGIN PROLOGUE  TQLRAT
	// ***DATE WRITTEN   760101   (YYMMDD)
	// ***REVISION DATE  830518   (YYMMDD)
	// ***CATEGORY NO.  D4A5,D4C2A
	// ***KEYWORDS  EIGENVALUES,EIGENVECTORS,EISPACK
	// ***AUTHOR  SMITH, B. T., ET AL.
	// ***PURPOSE  Computes eigenvalues of symmetric tridiagonal matrix
	//            a rational variant of the QL method.
	// ***DESCRIPTION
	
	//     This subroutine is a translation of the ALGOL procedure TQLRAT,
	//     ALGORITHM 464, COMM. ACM 16, 689(1973) by Reinsch.
	
	//     This subroutine finds the eigenvalues of a SYMMETRIC
	//     TRIDIAGONAL matrix by the rational QL method.
	
	//     On Input
	
	//        N is the order of the matrix.
	
	//        D contains the diagonal elements of the input matrix.
	
	//        E2 contains the squares of the subdiagonal elements of the
	//          input matrix in its last N-1 positions.  E2(1) is arbitrary.
	
	//      On Output
	
	//        D contains the eigenvalues in ascending order.  If an
	//          error exit is made, the eigenvalues are correct and
	//          ordered for indices 1,2,...IERR-1, but may not be
	//          the smallest eigenvalues.
	
	//        E2 has been destroyed.
	
	//        IERR is set to
	//          Zero       for normal return,
	//          J          if the J-th eigenvalue has not been
	//                     determined after 30 iterations.
	
	//     Calls PYTHAG(A,B) for sqrt(A**2 + B**2).
	
	//     Questions and comments should be directed to B. S. Garbow,
	//     APPLIED MATHEMATICS DIVISION, ARGONNE NATIONAL LABORATORY
	//     ------------------------------------------------------------------
	// ***REFERENCES  B. T. SMITH, J. M. BOYLE, J. J. DONGARRA, B. S. GARBOW,
	//                 Y. IKEBE, V. C. KLEMA, C. B. MOLER, *MATRIX EIGEN-
	//                 SYSTEM ROUTINES - EISPACK GUIDE*, SPRINGER-VERLAG,
	//                 1976.
	// ***ROUTINES CALLED  PYTHAG
	// ***END PROLOGUE  TQLRAT
	
	
	// ***FIRST EXECUTABLE STATEMENT  TQLRAT
	if( machep != 1.0e0 ) 
		goto L_10;
	
	//   --- This code fails to compute MACHEP correctly on IBM machines. ---
	//   --- Replaced by call to R1MACH on 15 Jun 94 by Ron Boisvert.     ---
	
L_5:
	machep = 0.5e0*machep;
	if( 1.0e0 + machep > 1.0e0 ) 
		goto L_5;
	machep = 2.0e0*machep;
	
	//   --- Alternative if code above fails ---
	//     MACHEP = R1MACH(4)
	
L_10:
	ierr = 0;
	if( n == 1 ) 
		goto L_1001;
	
	for( i = 2; i <= n; i++ ) { 
		i_ = i - 1;
		e2[i_ - 1] = e2[i_];
	}
	
	f = 0.0e0;
	b = 0.0e0;
	e2[n - 1] = 0.0e0;
	
	for( l = 1; l <= n; l++ ) { 
		l_ = l - 1;
		j = 0;
		h = machep*(fabs( d[l_] ) + sqrt( e2[l_] ));
		if( b > h ) 
			goto L_105;
		b = h;
		c = b*b;
		//     .......... LOOK FOR SMALL SQUARED SUB-DIAGONAL ELEMENT ..........
L_105:
		for( m = l; m <= n; m++ ) {
			m_ = m - 1; 
			if( e2[m_] <= c ) 
				goto L_120;
			//     .......... E2(N) IS ALWAYS ZERO, SO THERE IS NO EXIT
			//                THROUGH THE BOTTOM OF THE LOOP ..........
		}
		
L_120:
		if( m == l ) 
			goto L_210;
L_130:
		if( j == 30 ) 
			goto L_1000;
		j = j + 1;
		//     .......... FORM SHIFT ..........
		l1 = l + 1;
		s = sqrt( e2[l_] );
		g = d[l_];
		p = (d[l1 - 1] - g)/(2.0e0*s);
		r = pythag( p, 1.0e0 );
		d[l_] = s/(p + fsign( r, p ));
		h = g - d[l_];
		
		for( i = l1; i <= n; i++ ) {
			i_ = i - 1; 
			d[i_] = d[i_] - h;
		}
		
		f = f + h;
		//     .......... RATIONAL QL TRANSFORMATION ..........
		g = d[m - 1];
		if( g == 0.0e0 ) 
			g = b;
		h = g;
		s = 0.0e0;
		mml = m - l;
		//     .......... FOR I=M-1 STEP -1 UNTIL L DO -- ..........
		for( ii = 1L; ii <= mml; ii++ ) {
			ii_ = ii - 1; 
			i = m - ii;
			p = g*h;
			r = p + e2[i - 1];
			e2[i] = s*r;
			s = e2[i - 1]/r;
			d[i] = h + s*(h + d[i - 1]);
			g = d[i - 1] - e2[i - 1]/g;
			if( g == 0.0e0 ) 
				g = b;
			h = g*p/r;
		}
		
		e2[l_] = s*g;
		d[l_] = h;
		//     .......... GUARD AGAINST UNDERFLOW IN CONVERGENCE TEST ..........
		if( h == 0.0e0 ) 
			goto L_210;
		if( fabs( e2[l_] ) <= fabs( c/h ) ) 
			goto L_210;
		e2[l_] = h*e2[l_];
		if( e2[l_] != 0.0e0 ) 
			goto L_130;
L_210:
		p = d[l_] + f;
		//     .......... ORDER EIGENVALUES ..........
		if( l == 1 ) 
			goto L_250;
		//     .......... FOR I=L STEP -1 UNTIL 2 DO -- ..........
		for( ii = 2; ii <= l; ii++ ) {
			ii_ = ii - 1; 
			i = l + 2 - ii;
			if( p >= d[i - 2] ) 
				goto L_270;
			d[i - 1] = d[i - 2];
		}
		
L_250:
		i = 1;
L_270:
		d[i - 1] = p;
	}
	
	goto L_1001;
	//     .......... SET ERROR -- NO CONVERGENCE TO AN
	//                EIGENVALUE AFTER 30 ITERATIONS ..........
L_1000:
	ierr = l;
L_1001:
	return;
}

//////////////////////////////////////////////////////////////////////////////

void tred1(INTEGER nm, INTEGER n, REAL* a, REAL* d, REAL* e, REAL* e2)
{
#define A(I_,J_)	(*(a+(I_)*(nm)+(J_)))

	INTEGER i, i_, ii, ii_, j, j_, jp1, k, k_, l;
	REAL f, g, h, scale;

	// ***BEGIN PROLOGUE  TRED1
	// ***DATE WRITTEN   760101   (YYMMDD)
	// ***REVISION DATE  830518   (YYMMDD)
	// ***CATEGORY NO.  D4C1B1
	// ***KEYWORDS  EIGENVALUES,EIGENVECTORS,EISPACK
	// ***AUTHOR  SMITH, B. T., ET AL.
	// ***PURPOSE  Reduce real symmetric matrix to symmetric tridiagonal
	//            matrix using orthogonal similarity transformations.
	// ***DESCRIPTION
	
	//     This subroutine is a translation of the ALGOL procedure TRED1,
	//     NUM. MATH. 11, 181-195(1968) by Martin, Reinsch, and Wilkinson.
	//     HANDBOOK FOR AUTO. COMP., VOL.II-LINEAR ALGEBRA, 212-226(1971).
	
	//     This subroutine reduces a REAL SYMMETRIC matrix
	//     to a symmetric tridiagonal matrix using
	//     orthogonal similarity transformations.
	
	//     On Input
	
	//        NM must be set to the row dimension of two-dimensional
	//          array parameters as declared in the calling program
	//          dimension statement.
	
	//        N is the order of the matrix.
	
	//        A contains the real symmetric input matrix.  Only the
	//          lower triangle of the matrix need be supplied.
	
	//     On Output
	
	//        A contains information about the orthogonal trans-
	//          formations used in the reduction in its strict lower
	//          triangle.  The full upper triangle of A is unaltered.
	
	//        D contains the diagonal elements of the tridiagonal matrix.
	
	//        E contains the subdiagonal elements of the tridiagonal
	//          matrix in its last N-1 positions.  E(1) is set to zero.
	
	//        E2 contains the squares of the corresponding elements of E.
	//          E2 may coincide with E if the squares are not needed.
	
	//     Questions and comments should be directed to B. S. Garbow,
	//     APPLIED MATHEMATICS DIVISION, ARGONNE NATIONAL LABORATORY
	//     ------------------------------------------------------------------
	// ***REFERENCES  B. T. SMITH, J. M. BOYLE, J. J. DONGARRA, B. S. GARBOW,
	//                 Y. IKEBE, V. C. KLEMA, C. B. MOLER, *MATRIX EIGEN-
	//                 SYSTEM ROUTINES - EISPACK GUIDE*, SPRINGER-VERLAG,
	//                 1976.
	// ***ROUTINES CALLED  (NONE)
	// ***END PROLOGUE  TRED1
	
	
	// ***FIRST EXECUTABLE STATEMENT  TRED1
	for( i = 1; i <= n; i++ ) {
		i_ = i - 1; 
		d[i_] = A(i_,i_);
	}
	//     .......... FOR I=N STEP -1 UNTIL 1 DO -- ..........
	for( ii = 1; ii <= n; ii++ ) {
		ii_ = ii - 1; 
		i = n + 1 - ii;
		l = i - 1;
		h = 0.0e0;
		scale = 0.0e0;
		if( l < 1 ) 
			goto L_130;
		//     .......... SCALE ROW (ALGOL TOL THEN NOT NEEDED) ..........
		for( k = 1; k <= l; k++ ) {
			k_ = k - 1; 
			scale = scale + fabs( A(k_,i - 1) );
		}
		
		if( scale != 0.0e0 ) 
			goto L_140;
L_130:
		e[i - 1] = 0.0e0;
		e2[i - 1] = 0.0e0;
		goto L_290;
		
L_140:
		for( k = 1; k <= l; k++ ) {
			k_ = k - 1; 
			A(k_,i - 1) = A(k_,i - 1)/scale;
			h = h + A(k_,i - 1)*A(k_,i - 1);
		}
		
		e2[i - 1] = scale*scale*h;
		f = A(l - 1,i - 1);
		g = -fsign( sqrt( h ), f );
		e[i - 1] = scale*g;
		h = h - f*g;
		A(l - 1,i - 1) = f - g;
		if( l == 1 ) 
			goto L_270;
		f = 0.0e0;
		
		for( j = 1; j <= l; j++ ) {
			j_ = j - 1; 
			g = 0.0e0;
			//     .......... FORM ELEMENT OF A*U ..........
			for( k = 1; k <= j; k++ ) {
				k_ = k - 1; 
				g = g + A(k_,j_)*A(k_,i - 1);
			}
			
			jp1 = j + 1;
			if( l < jp1 ) 
				goto L_220;
			
			for( k = jp1; k <= l; k++ ) {
				k_ = k - 1; 
				g = g + A(j_,k_)*A(k_,i - 1);
			}
			//     .......... FORM ELEMENT OF P ..........
L_220:
			e[j_] = g/h;
			f = f + e[j_]*A(j_,i - 1);
		}
		
		h = f/(h + h);
		//     .......... FORM REDUCED A ..........
		for( j = 1; j <= l; j++ ) {
			j_ = j - 1; 
			f = A(j_,i - 1);
			g = e[j_] - h*f;
			e[j_] = g;
			
			for( k = 1; k <= j; k++ ) {
				k_ = k - 1; 
				A(k_,j_) = A(k_,j_) - f*e[k_] - g*A(k_,i - 1);
			}
		}
		
L_270:
		for( k = 1; k <= l; k++ ) {
			k_ = k - 1; 
			A(k_,i - 1) = scale*A(k_,i - 1);
		}
		
L_290:
		h = d[i - 1];
		d[i - 1] = A(i - 1,i - 1);
		A(i - 1,i - 1) = h;
	}
	
	return;
#undef	A
}

//////////////////////////////////////////////////////////////////////////////

void tred2(INTEGER nm, INTEGER n, REAL* a, REAL* d, REAL* e, REAL* z)
{
#define A(I_,J_)	(*(a+(I_)*(nm)+(J_)))
#define Z(I_,J_)	(*(z+(I_)*(nm)+(J_)))

	INTEGER i, i_, ii, ii_, j, j_, jp1, k, k_, l;
	REAL f, g, h, hh, scale;

	// ***BEGIN PROLOGUE  TRED2
	// ***DATE WRITTEN   760101   (YYMMDD)
	// ***REVISION DATE  830518   (YYMMDD)
	// ***CATEGORY NO.  D4C1B1
	// ***KEYWORDS  EIGENVALUES,EIGENVECTORS,EISPACK
	// ***AUTHOR  SMITH, B. T., ET AL.
	// ***PURPOSE  Reduce real symmetric matrix to symmetric tridiagonal
	//            matrix using and accumulating orthogonal transformation
	// ***DESCRIPTION
	
	//     This subroutine is a translation of the ALGOL procedure TRED2,
	//     NUM. MATH. 11, 181-195(1968) by Martin, Reinsch, and Wilkinson.
	//     HANDBOOK FOR AUTO. COMP., VOL.II-LINEAR ALGEBRA, 212-226(1971).
	
	//     This subroutine reduces a REAL SYMMETRIC matrix to a
	//     symmetric tridiagonal matrix using and accumulating
	//     orthogonal similarity transformations.
	
	//     On Input
	
	//        NM must be set to the row dimension of two-dimensional
	//          array parameters as declared in the calling program
	//          dimension statement.
	
	//        N is the order of the matrix.
	
	//        A contains the real symmetric input matrix.  Only the
	//          lower triangle of the matrix need be supplied.
	
	//     On Output
	
	//        D contains the diagonal elements of the tridiagonal matrix.
	
	//        E contains the subdiagonal elements of the tridiagonal
	//          matrix in its last N-1 positions.  E(1) is set to zero.
	
	//        Z contains the orthogonal transformation matrix
	//          produced in the reduction.
	
	//        A and Z may coincide.  If distinct, A is unaltered.
	
	//     Questions and comments should be directed to B. S. Garbow,
	//     APPLIED MATHEMATICS DIVISION, ARGONNE NATIONAL LABORATORY
	//     ------------------------------------------------------------------
	// ***REFERENCES  B. T. SMITH, J. M. BOYLE, J. J. DONGARRA, B. S. GARBOW,
	//                 Y. IKEBE, V. C. KLEMA, C. B. MOLER, *MATRIX EIGEN-
	//                 SYSTEM ROUTINES - EISPACK GUIDE*, SPRINGER-VERLAG,
	//                 1976.
	// ***ROUTINES CALLED  (NONE)
	// ***END PROLOGUE  TRED2
	
	
	// ***FIRST EXECUTABLE STATEMENT  TRED2
	for( i = 1; i <= n; i++ ) {
		i_ = i - 1; 
		
		for( j = 1; j <= i; j++ ) {
			j_ = j - 1; 
			Z(j_,i_) = A(j_,i_);
		}
	}
	
	if( n == 1 ) 
		goto L_320;
	//     .......... FOR I=N STEP -1 UNTIL 2 DO -- ..........
	for( ii = 2; ii <= n; ii++ ) {
		ii_ = ii - 1; 
		i = n + 2 - ii;
		l = i - 1;
		h = 0.0e0;
		scale = 0.0e0;
		if( l < 2 ) 
			goto L_130;
		//     .......... SCALE ROW (ALGOL TOL THEN NOT NEEDED) ..........
		for( k = 1; k <= l; k++ ) {
			k_ = k - 1; 
			scale = scale + fabs( Z(k_,i - 1) );
		}
		
		if( scale != 0.0e0 ) 
			goto L_140;
L_130:
		e[i - 1] = Z(l - 1,i - 1);
		goto L_290;
		
L_140:
		for( k = 1; k <= l; k++ ) {
			k_ = k - 1; 
			Z(k_,i - 1) = Z(k_,i - 1)/scale;
			h = h + Z(k_,i - 1)*Z(k_,i - 1);
		}
		
		f = Z(l - 1,i - 1);
		g = -fsign( sqrt( h ), f );
		e[i - 1] = scale*g;
		h = h - f*g;
		Z(l - 1,i - 1) = f - g;
		f = 0.0e0;
		
		for( j = 1; j <= l; j++ ) {
			j_ = j - 1; 
			Z(i - 1,j_) = Z(j_,i - 1)/h;
			g = 0.0e0;
			//     .......... FORM ELEMENT OF A*U ..........
			for( k = 1; k <= j; k++ ) {
				k_ = k - 1; 
				g = g + Z(k_,j_)*Z(k_,i - 1);
			}
			
			jp1 = j + 1;
			if( l < jp1 ) 
				goto L_220;
			
			for( k = jp1; k <= l; k++ ) {
				k_ = k - 1; 
				g = g + Z(j_,k_)*Z(k_,i - 1);
			}
			//     .......... FORM ELEMENT OF P ..........
L_220:
			e[j_] = g/h;
			f = f + e[j_]*Z(j_,i - 1);
		}
		
		hh = f/(h + h);
		//     .......... FORM REDUCED A ..........
		for( j = 1; j <= l; j++ ) { 
			j_ = j - 1;
			f = Z(j_,i - 1);
			g = e[j_] - hh*f;
			e[j_] = g;
			
			for( k = 1; k <= j; k++ ) { 
				k_ = k - 1;
				Z(k_,j_) = Z(k_,j_) - f*e[k_] - g*Z(k_,i - 1);
			}
		}
		
L_290:
		d[i - 1] = h;
	}
	
L_320:
	d[0L] = 0.0e0;
	e[0L] = 0.0e0;
	//     .......... ACCUMULATION OF TRANSFORMATION MATRICES ..........
	for( i = 1; i <= n; i++ ) {
		i_ = i - 1; 
		l = i - 1;
		if( d[i_] == 0.0e0 ) 
			goto L_380;
		
		for( j = 1; j <= l; j++ ) {
			j_ = j - 1; 
			g = 0.0e0;
			
			for( k = 1; k <= l; k++ ) {
				k_ = k - 1; 
				g = g + Z(k_,i_)*Z(j_,k_);
			}
			
			for( k = 1; k <= l; k++ ) { 
				k_ = k - 1;
				Z(j_,k_) = Z(j_,k_) - g*Z(i_,k_);
			}
		}
		
L_380:
		d[i_] = Z(i_,i_);
		Z(i_,i_) = 1.0e0;
		if( l < 1 ) 
			goto L_500;
		
		for( j = 1; j <= l; j++ ) {
			j_ = j - 1; 
			Z(j_,i_) = 0.0e0;
			Z(i_,j_) = 0.0e0;
		}
		
L_500:
		;
	}
	
	return;
#undef	Z
#undef	A
}

//////////////////////////////////////////////////////////////////////////////

void tql2(INTEGER nm, INTEGER n, REAL* d, REAL* e, REAL* z, INTEGER& ierr)
{
#define Z(I_,J_)	(*(z+(I_)*(nm)+(J_)))

	INTEGER i, i_, ii, ii_, j, j_, k, k_, l, l1, l2, l_, m, m_, mml;
	REAL b, c, c2, c3, dl1, el1, f, g, h, p, r, s, s2;

	// ***BEGIN PROLOGUE  TQL2
	// ***DATE WRITTEN   760101   (YYMMDD)
	// ***REVISION DATE  830518   (YYMMDD)
	// ***CATEGORY NO.  D4A5,D4C2A
	// ***KEYWORDS  EIGENVALUES,EIGENVECTORS,EISPACK
	// ***AUTHOR  SMITH, B. T., ET AL.
	// ***PURPOSE  Compute eigenvalues and eigenvectors of symmetric
	//            tridiagonal matrix.
	// ***DESCRIPTION
	
	//     This subroutine is a translation of the ALGOL procedure TQL2,
	//     NUM. MATH. 11, 293-306(1968) by Bowdler, Martin, Reinsch, and
	//     Wilkinson.
	//     HANDBOOK FOR AUTO. COMP., VOL.II-LINEAR ALGEBRA, 227-240(1971).
	
	//     This subroutine finds the eigenvalues and eigenvectors
	//     of a SYMMETRIC TRIDIAGONAL matrix by the QL method.
	//     The eigenvectors of a FULL SYMMETRIC matrix can also
	//     be found if  TRED2  has been used to reduce this
	//     full matrix to tridiagonal form.
	
	//     On Input
	
	//        NM must be set to the row dimension of two-dimensional
	//          array parameters as declared in the calling program
	//          dimension statement.
	
	//        N is the order of the matrix.
	
	//        D contains the diagonal elements of the input matrix.
	
	//        E contains the subdiagonal elements of the input matrix
	//          in its last N-1 positions.  E(1) is arbitrary.
	
	//        Z contains the transformation matrix produced in the
	//          reduction by  TRED2, if performed.  If the eigenvectors
	//          of the tridiagonal matrix are desired, Z must contain
	//          the identity matrix.
	
	//      On Output
	
	//        D contains the eigenvalues in ascending order.  If an
	//          error exit is made, the eigenvalues are correct but
	//          unordered for indices 1,2,...,IERR-1.
	
	//        E has been destroyed.
	
	//        Z contains orthonormal eigenvectors of the symmetric
	//          tridiagonal (or full) matrix.  If an error exit is made,
	//          Z contains the eigenvectors associated with the stored
	//          eigenvalues.
	
	//        IERR is set to
	//          Zero       for normal return,
	//          J          if the J-th eigenvalue has not been
	//                     determined after 30 iterations.
	
	//     Calls PYTHAG(A,B) for sqrt(A**2 + B**2).
	
	//     Questions and comments should be directed to B. S. Garbow,
	//     APPLIED MATHEMATICS DIVISION, ARGONNE NATIONAL LABORATORY
	//     ------------------------------------------------------------------
	// ***REFERENCES  B. T. SMITH, J. M. BOYLE, J. J. DONGARRA, B. S. GARBOW,
	//                 Y. IKEBE, V. C. KLEMA, C. B. MOLER, *MATRIX EIGEN-
	//                 SYSTEM ROUTINES - EISPACK GUIDE*, SPRINGER-VERLAG,
	//                 1976.
	// ***ROUTINES CALLED  PYTHAG
	// ***END PROLOGUE  TQL2
	
	
	// ***FIRST EXECUTABLE STATEMENT  TQL2
	ierr = 0;
	if( n == 1 ) 
		goto L_1001;
	
	for( i = 2; i <= n; i++ ) {
		i_ = i - 1; 
		e[i_ - 1] = e[i_];
	}
	
	f = 0.0e0;
	b = 0.0e0;
	e[n - 1] = 0.0e0;
	
	for( l = 1; l <= n; l++ ) { 
		l_ = l - 1;
		j = 0;
		h = fabs( d[l_] ) + fabs( e[l_] );
		if( b < h ) 
			b = h;
		//     .......... LOOK FOR SMALL SUB-DIAGONAL ELEMENT ..........
		for( m = l; m <= n; m++ ) { 
			m_ = m - 1;
			if( b + fabs( e[m_] ) == b ) 
				goto L_120;
			//     .......... E(N) IS ALWAYS ZERO, SO THERE IS NO EXIT
			//                THROUGH THE BOTTOM OF THE LOOP ..........
		}
		
L_120:
		if( m == l ) 
			goto L_220;
L_130:
		if( j == 30 ) 
			goto L_1000;
		j = j + 1;
		//     .......... FORM SHIFT ..........
		l1 = l + 1;
		l2 = l1 + 1;
		g = d[l_];
		p = (d[l1 - 1] - g)/(2.0e0*e[l_]);
		r = pythag( p, 1.0e0 );
		d[l_] = e[l_]/(p + fsign( r, p ));
		d[l1 - 1] = e[l_]*(p + fsign( r, p ));
		dl1 = d[l1 - 1];
		h = g - d[l_];
		if( l2 > n ) 
			goto L_145;
		
		for( i = l2; i <= n; i++ ) { 
			i_ = i - 1;
			d[i_] = d[i_] - h;
		}
		
L_145:
		f = f + h;
		//     .......... QL TRANSFORMATION ..........
		p = d[m - 1];
		c = 1.0e0;
		c2 = c;
		el1 = e[l1 - 1];
		s = 0.0e0;
		mml = m - l;
		//     .......... FOR I=M-1 STEP -1 UNTIL L DO -- ..........
		for( ii = 1; ii <= mml; ii++ ) { 
			ii_ = ii - 1;
			c3 = c2;
			c2 = c;
			s2 = s;
			i = m - ii;
			g = c*e[i - 1];
			h = c*p;
			if( fabs( p ) < fabs( e[i - 1] ) ) 
				goto L_150;
			c = e[i - 1]/p;
			r = sqrt( c*c + 1.0e0 );
			e[i] = s*p*r;
			s = c/r;
			c = 1.0e0/r;
			goto L_160;
L_150:
			c = p/e[i - 1];
			r = sqrt( c*c + 1.0e0 );
			e[i] = s*e[i - 1]*r;
			s = 1.0e0/r;
			c = c*s;
L_160:
			p = c*d[i - 1] - s*g;
			d[i] = h + s*(c*g + s*d[i - 1]);
			//     .......... FORM VECTOR ..........
			for( k = 1; k <= n; k++ ) { 
				k_ = k - 1;
				h = Z(i,k_);
				Z(i,k_) = s*Z(i - 1,k_) + c*h;
				Z(i - 1,k_) = c*Z(i - 1,k_) - s*h;
			}
			
		}
		
		p = -s*s2*c3*el1*e[l_]/dl1;
		e[l_] = s*p;
		d[l_] = c*p;
		if( b + fabs( e[l_] ) > b ) 
			goto L_130;
L_220:
		d[l_] = d[l_] + f;
	}
	//     .......... ORDER EIGENVALUES AND EIGENVECTORS ..........
	for( ii = 2; ii <= n; ii++ ) {
		ii_ = ii - 1; 
		i = ii - 1;
		k = i;
		p = d[i - 1];
		
		for( j = ii; j <= n; j++ ) { 
			j_ = j - 1;
			if( d[j_] >= p ) 
				goto L_260;
			k = j;
			p = d[j_];
L_260:
			;
		}
		
		if( k == i ) 
			goto L_300;
		d[k - 1] = d[i - 1];
		d[i - 1] = p;
		
		for( j = 1; j <= n; j++ ) { 
			j_ = j - 1;
			p = Z(i - 1,j_);
			Z(i - 1,j_) = Z(k - 1,j_);
			Z(k - 1,j_) = p;
		}
		
L_300:
		;
	}
	
	goto L_1001;
	//     .......... SET ERROR -- NO CONVERGENCE TO AN
	//                EIGENVALUE AFTER 30 ITERATIONS ..........
L_1000:
	ierr = l;
L_1001:
	return;
#undef	Z
}

//////////////////////////////////////////////////////////////////////////////

REAL pythag(REAL a, REAL b)
{
	REAL p, pythag_v, q, r, s, t;

	// ***BEGIN PROLOGUE  PYTHAG
	// ***REFER TO  EISDOC
	
	//     Finds sqrt(A**2+B**2) without overflow or destructive underflow
	// ***ROUTINES CALLED  (NONE)
	// ***END PROLOGUE  PYTHAG
	
	// ***FIRST EXECUTABLE STATEMENT  PYTHAG
	p = fmax( fabs( a ), fabs( b ) );
	q = fmin( fabs( a ), fabs( b ) );
	if( q == 0.0e0 ) 
		goto L_20;
L_10:
	;
	r = pow(q/p, 2);
	t = 4.0e0 + r;
	if( t == 4.0e0 ) 
		goto L_20;
	s = r/t;
	p = p + 2.0e0*p*s;
	q = q*s;
	goto L_10;
L_20:
	pythag_v = p;
	return pythag_v;
}

//////////////////////////////////////////////////////////////////////////////


