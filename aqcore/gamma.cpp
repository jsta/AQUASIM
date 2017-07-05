/////////////////////////////////   gamma.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    16.10.00    Peter Reichert
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include "gamma.h"

//////////////////////////////////////////////////////////////////////////////

//GAMS : Fullsource of GAMCDF in DATAPAC from CAMSUN
// ======================================================================
// NIST Guide to Available Math Software.
// Fullsource for module GAMCDF from package DATAPAC.
// Retrieved from CAMSUN on Sat Apr 15 23:29:00 2000.
// ======================================================================

BOOLEAN gamcdf(REAL x, REAL gamma, REAL* cdf, std::ostream* cjob)
{
	INTLONG i, i_, ipr, maxit;
	REAL a, ai, b, cut1, cut2, cutoff, den, dgamma, dx, g, sum, 
	 t, term, z, z2, z3, z4, z5;
	static REAL d[10];
	static REAL c = .918938533204672741e0;
	static INTLONG _aini = 1;
	if( _aini ) {  // onetime initializations
		d[0] = .833333333333333333e-1;
		d[1] = -.277777777777777778e-2;
		d[2] = .793650793650793651e-3;
		d[3] = -.595238095238095238e-3;
		d[4] = .841750841750841751e-3;
		d[5] = -.191752691752691753e-2;
		d[6] = .641025641025641025e-2;
		d[7] = -.295506535947712418e-1;
		d[8] = .179644372368830573e0;
		d[9] = -.139243221690590111e1;
		_aini = 0;
	}

	//---------------------------------------------------------------------
	
	ipr = 6;
	
	//     CHECK THE INPUT ARGUMENTS FOR ERRORS
	
	if( x <= 0.0 ) 
		goto L_50;
	if( gamma <= 0.0 ) 
		goto L_55;
	goto L_90;
L_50:
	// writef( (INTLONG)ipr, FMTR(4), "\n" );
	// writef( (INTLONG)ipr, FMTR(46), "%g \n", x );
        if ( cjob != 0 )
        {
           *cjob << "\n*** NON-FATAL DIAGNOSTIC--THE FIRST  INPUT ARGUMENT TO THE GAMCDF SUBROUTINE IS NON-POSITIVE";
           *cjob << "\n*** THE VALUE OF THE ARGUMENT IS " << x << "\n";
        }
	*cdf = 0.0;
	return FALSE;
L_55:
	// writef( (INTLONG)ipr, FMTR(15), "\n" );
	// writef( (INTLONG)ipr, FMTR(46), "%g \n", gamma );
        if ( cjob != 0 )
        {
           *cjob << "\n*** NON-FATAL DIAGNOSTIC--THE FIRST  INPUT ARGUMENT TO THE GAMCDF SUBROUTINE IS NON-POSITIVE";
           *cjob << "\n*** THE VALUE OF THE ARGUMENT IS " << gamma << "\n";
        }
	*cdf = 0.0;
	return FALSE;
L_90:
	;
	
	//-----START POINT-----------------------------------------------------
	
	dx = x;
	dgamma = gamma;
	maxit = 10000;
	
	//     COMPUTE THE GAMMA FUNCTION USING THE ALGORITHM IN THE 
	//     NBS APPLIED MATHEMATICS SERIES REFERENCE.
	
	z = gamma;
	den = 1.0e0;
L_300:
	if( z >= 10.0e0 ) 
		goto L_400;
	den = den*z;
	z = z + 1;
	goto L_300;
L_400:
	z2 = z*z;
	z3 = z*z2;
	z4 = z2*z2;
	z5 = z2*z3;
	a = (z - 0.5e0)*log( z ) - z + c;
	b = d[0]/z + d[1]/z3 + d[2]/z5 + d[3]/(z2*z5) + d[4]/(z4*
	 z5) + d[5]/(z*z5*z5) + d[6]/(z3*z5*z5) + d[7]/(z5*z5*z5) + 
	 d[8]/(z2*z5*z5*z5);
	g = exp( a + b )/den;
	
	//     COMPUTE T-SUB-Q AS DEFINED ON PAGE 4 OF THE WILK, GNANADESIKAN, 
	//     AND HUYETT REFERENCE
	
	sum = 1.0e0/dgamma;
	term = 1.0e0/dgamma;
	cut1 = dx - dgamma;
	cut2 = dx*10000000000.0e0;
	for( i = 1; i <= maxit; i++ ) { 
		i_ = i-1;
		ai = i;
		term = dx*term/(dgamma + ai);
		sum = sum + term;
		cutoff = cut1 + (cut2*term/sum);
		if( ai > cutoff ) 
			goto L_250;
	}
	// writef( (INTLONG)ipr, FMTR(205), "%ld \n", maxit );
	// writef( (INTLONG)ipr, FMTR(206), "%g \n", x );
	// writef( (INTLONG)ipr, FMTR(207), "%g \n", gamma );
	// writef( (INTLONG)ipr, FMTR(208), "\n" );
        if ( cjob != 0 )
        {
           *cjob << "\n*** ERROR IN INTERNAL OPERATIONS IN THE GAMCDF SUBROUTINE--THE NUMBER OF ITERATIONS EXCEEDS " << maxit;
           *cjob << "\n***    THE INPUT VALUE OF X     IS " << x;
           *cjob << "\n***    THE INPUT VALUE OF GAMMA IS " << gamma;
           *cjob << "\n***    THE OUTPUT VALUE OF CDF HAS BEEN SET TO 1.0" << "\n";
        }
	*cdf = 1.0;
	return FALSE;
	
L_250:
	t = sum;
	*cdf = (pow(dx, dgamma))*(exp( -dx ))*t/g;
	
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

// ********************************************************************
//GAMS : Fullsource of GAMPPF in DATAPAC from CAMSUN
// ======================================================================
// NIST Guide to Available Math Software.
// Fullsource for module GAMPPF from package DATAPAC.
// Retrieved from CAMSUN on Sat Apr 15 23:24:04 2000.
// ======================================================================


BOOLEAN gamppf(REAL p, REAL gamma, REAL* ppf, std::ostream* cjob)
{
	INTLONG icount, iloop, ipr, j, j_, maxit;
	REAL a, ai, aj, b, cut1, cut2, cutoff, den, dgamma, dp, dx, 
	 g, pcalc, sum, t, term, xdel, xlower, xmax, xmid, xmin, xmin0, 
	 xupper, z, z2, z3, z4, z5;
	static REAL d[10];
	static REAL c = .918938533204672741e0;
	static INTLONG _aini = 1;
	if( _aini ) {  // onetime initializations
		d[0] = .833333333333333333e-1;
		d[1] = -.277777777777777778e-2;
		d[2] = .793650793650793651e-3;
		d[3] = -.595238095238095238e-3;
		d[4] = .841750841750841751e-3;
		d[5] = -.191752691752691753e-2;
		d[6] = .641025641025641025e-2;
		d[7] = -.295506535947712418e-1;
		d[8] = .179644372368830573e0;
		d[9] = -.139243221690590111e1;
		_aini = 0;
	}

	//---------------------------------------------------------------------
	
	ipr = 6;
	
	//     CHECK THE INPUT ARGUMENTS FOR ERRORS
	
	if( p <= 0.0 || p >= 1.0 ) 
		goto L_50;
	if( gamma <= 0.0 ) 
		goto L_55;
	goto L_90;
L_50:
	// writef( (INTLONG)ipr, FMTR(1), "\n" );
	// writef( (INTLONG)ipr, FMTR(46), "%g \n", p );
        if ( cjob != 0 )
        {
           *cjob << "\n*** FATAL ERROR--THE FIRST  INPUT ARGUMENT TO THE GAMPPF SUBROUTINE IS OUTSIDE THE ALLOWABLE (0,1) INTERVAL";
           *cjob << "\n*** THE VALUE OF THE ARGUMENT IS " << p << "\n";
        }
	*ppf = 0.0;
	return FALSE;
L_55:
	// writef( (INTLONG)ipr, FMTR(15), "\n" );
	// writef( (INTLONG)ipr, FMTR(46), "%g \n", gamma );
        if ( cjob != 0 )
        {
           *cjob << "\n*** FATAL ERROR--THE SECOND INPUT ARGUMENT TO THE GAMPPF SUBROUTINE IS NON-POSITIVE";
           *cjob << "\n*** THE VALUE OF THE ARGUMENT IS " << gamma << "\n";
        }
	*ppf = 0.0;
	return FALSE;
L_90:
	;
	
	//-----START POINT-----------------------------------------------------
	
	dp = p;
	dgamma = gamma;
	maxit = 10000;
	
	//     COMPUTE THE GAMMA FUNCTION USING THE ALGORITHM IN THE 
	//     NBS APPLIED MATHEMATICS SERIES REFERENCE.
	//     THIS GAMMA FUNCTION NEED BE CALCULATED ONLY ONCE.
	//     IT IS USED IN THE CALCULATION OF THE CDF BASED ON
	//     THE TENTATIVE VALUE OF THE PPF IN THE ITERATION.
	
	z = dgamma;
	den = 1.0e0;
L_150:
	if( z >= 10.0e0 ) 
		goto L_160;
	den = den*z;
	z = z + 1.0e0;
	goto L_150;
L_160:
	z2 = z*z;
	z3 = z*z2;
	z4 = z2*z2;
	z5 = z2*z3;
	a = (z - 0.5e0)*log( z ) - z + c;
	b = d[0]/z + d[1]/z3 + d[2]/z5 + d[3]/(z2*z5) + d[4]/(z4*
	 z5) + d[5]/(z*z5*z5) + d[6]/(z3*z5*z5) + d[7]/(z5*z5*z5) + 
	 d[8]/(z2*z5*z5*z5);
	g = exp( a + b )/den;
	
	//     DETERMINE LOWER AND UPPER LIMITS ON THE DESIRED 100P
	//     PERCENT POINT.
	
	iloop = 1;
	xmin0 = pow(dp*dgamma*g, 1.0e0/dgamma);
	xmin = xmin0;
	icount = 1;
L_350:
	ai = icount;
	xmax = ai*xmin0;
	dx = xmax;
	goto L_1000;
L_360:
	if( pcalc >= dp ) 
		goto L_370;
	xmin = xmax;
	icount = icount + 1;
	if( icount <= 30000 ) 
		goto L_350;
L_370:
	xmid = (xmin + xmax)/2.0e0;
	
	//     NOW ITERATE BY BISECTION UNTIL THE DESIRED ACCURACY IS ACHIEVED.
	
	iloop = 2;
	xlower = xmin;
	xupper = xmax;
	icount = 0;
L_550:
	dx = xmid;
	goto L_1000;
L_560:
	if( pcalc == dp ) 
		goto L_570;
	if( pcalc > dp ) 
		goto L_580;
	xlower = xmid;
	xmid = (xmid + xupper)/2.0e0;
	goto L_590;
L_580:
	xupper = xmid;
	xmid = (xmid + xlower)/2.0e0;
L_590:
	xdel = xmid - xlower;
	if( xdel < 0.0e0 ) 
		xdel = -xdel;
	icount = icount + 1;
	if( xdel < 0.0000000001e0 || icount > 100 ) 
		goto L_570;
	goto L_550;
L_570:
	*ppf = xmid;
	return TRUE;
	
	// ******************************************************************** 
	//     THIS SECTION BELOW IS LOGICALLY SEPARATE FROM THE ABOVE.
	//     THIS SECTION COMPUTES A CDF VALUE FOR ANY GIVEN TENTATIVE
	//     PERCENT POINT X VALUE AS DEFINED IN EITHER OF THE 2
	//     ITERATION LOOPS IN THE ABOVE CODE.
	
	//     COMPUTE T-SUB-Q AS DEFINED ON PAGE 4 OF THE WILK, GNANADESIKAN, 
	//     AND HUYETT REFERENCE
	
L_1000:
	sum = 1.0e0/dgamma;
	term = 1.0e0/dgamma;
	cut1 = dx - dgamma;
	cut2 = dx*10000000000.0e0;
	for( j = 1; j <= maxit; j++) { 
		j_ = j - 1;
		aj = j;
		term = dx*term/(dgamma + aj);
		sum = sum + term;
		cutoff = cut1 + (cut2*term/sum);
		if( aj > cutoff ) 
			goto L_750;
	}
	// writef( (INTLONG)ipr, FMTR(705), "%ld \n", maxit );
	// writef( (INTLONG)ipr, FMTR(706), "%g \n", p );
	// writef( (INTLONG)ipr, FMTR(707), "%g \n", gamma );
	// writef( (INTLONG)ipr, FMTR(708), "\n" );
        if ( cjob != 0 )
        {
           *cjob << "\n*** ERROR IN INTERNAL OPERATIONS IN THE GAMPPF 'SUBROUTINE--THE NUMBER OF ITERATIONS EXCEEDS " << maxit;
           *cjob << "\n***    THE INPUT VALUE OF P     IS " << p;
           *cjob << "\n***    THE INPUT VALUE OF GAMMA IS " << gamma;
           *cjob << "\n***    THE OUTPUT VALUE OF PPF HAS BEEN SET TO 0.0" << "\n";
        }
	*ppf = 0.0;
	return FALSE;
	
L_750:
	t = sum;
	pcalc = (pow(dx, dgamma))*(exp( -dx ))*t/g;
	if( iloop == 1 ) 
		goto L_360;
	goto L_560;
}

//////////////////////////////////////////////////////////////////////////////

// ********************************************************************
//GAMS : Fullsource of GAMMA in SPECFUN from NETLIB
// ======================================================================
// NIST Guide to Available Math Software.
// Fullsource for module GAMMA from package SPECFUN.
// Retrieved from NETLIB on Sat Apr 15 16:11:20 2000.
// ======================================================================


inline REAL gamma_conv(INTLONG i) { return (REAL)( (i) ); }


REAL gamma(REAL x)
{
	BOOLEAN parity;
	INTLONG i, i_, n;
	REAL fact, gamma_v, res, sum, xden, xnum, y, y1, ysq, z;
	static REAL one = 1.0e0;
	static REAL half = 0.5e0;
	static REAL twelve = 12.0e0;
	static REAL two = 2.0e0;
	static REAL zero = 0.0e0;
	static REAL sqrtpi = 0.9189385332046727417803297e0;
	static REAL pi = 3.1415926535897932384626434e0;
	static REAL xbig = 171.624e0;
	static REAL xminin = 2.23e-308;
	static REAL eps = 2.22e-16;
	static REAL xinf = 1.79e308;
	static REAL p[8]={-1.71618513886549492533811e0,2.47656508055759199108314e1,
	 -3.79804256470945635097577e2,6.29331155312818442661052e2,8.66966202790413211295064e2,
	 -3.14512729688483675254357e4,-3.61444134186911729807069e4,6.64561438202405440627855e4};
	static REAL q[8]={-3.08402300119738975254353e1,3.15350626979604161529144e2,
	 -1.01515636749021914166146e3,-3.10777167157231109440444e3,2.25381184209801510330112e4,
	 4.75584627752788110767815e3,-1.34659959864969306392456e5,-1.15132259675553483497211e5};
	static REAL c[7]={-1.910444077728e-03,8.4171387781295e-04,-5.952379913043012e-04,
	 7.93650793500350248e-04,-2.777777777777681622553e-03,8.333333333333333331554247e-02,
	 5.7083835261e-03};

	// *******************************************************************
	
	// Explanation of machine-dependent constants
	
	// beta   - radix for the floating-point representation
	// maxexp - the smallest positive power of beta that overflows
	// XBIG   - the largest argument for which GAMMA(X) is representable
	//          in the machine, i.e., the solution to the equation
	//                  GAMMA(XBIG) = beta**maxexp
	// XINF   - the largest machine representable floating-point number;
	//          approximately beta**maxexp
	// EPS    - the smallest positive floating-point number such that
	//          1.0+EPS .GT. 1.0
	// XMININ - the smallest positive floating-point number such that
	//          1/XMININ is machine representable
	
	//     Approximate values for some important machines are:
	
	//                            beta       maxexp        XBIG
	
	// CRAY-1         (S.P.)        2         8191        966.961
	// Cyber 180/855
	//   under NOS    (S.P.)        2         1070        177.803
	// IEEE (IBM/XT,
	//   SUN, etc.)   (S.P.)        2          128        35.040
	// IEEE (IBM/XT,
	//   SUN, etc.)   (D.P.)        2         1024        171.624
	// IBM 3033       (D.P.)       16           63        57.574
	// VAX D-Format   (D.P.)        2          127        34.844
	// VAX G-Format   (D.P.)        2         1023        171.489
	
	//                            XINF         EPS        XMININ
	
	// CRAY-1         (S.P.)   5.45E+2465   7.11E-15    1.84E-2466
	// Cyber 180/855
	//   under NOS    (S.P.)   1.26E+322    3.55E-15    3.14E-294
	// IEEE (IBM/XT,
	//   SUN, etc.)   (S.P.)   3.40E+38     1.19E-7     1.18E-38
	// IEEE (IBM/XT,
	//   SUN, etc.)   (D.P.)   1.79D+308    2.22D-16    2.23D-308
	// IBM 3033       (D.P.)   7.23D+75     2.22D-16    1.39D-76
	// VAX D-Format   (D.P.)   1.70D+38     1.39D-17    5.88D-39
	// VAX G-Format   (D.P.)   8.98D+307    1.11D-16    1.12D-308
	
	// *******************************************************************
	
	// Error returns
	
	//  The program returns the value XINF for singularities or
	//     when overflow would occur.  The computation is believed
	//     to be free of underflow and overflow.
	
	
	//  Intrinsic functions required are:
	
	//     INT, DBLE, EXP, LOG, REAL, SIN
	
	
	// References: "An Overview of Software Development for Special
	//              Functions", W. J. Cody, Lecture Notes in Mathematics,
	//              506, Numerical Analysis Dundee, 1975, G. A. Watson
	//              (ed.), Springer Verlag, Berlin, 1976.
	
	//              Computer Approximations, Hart, Et. Al., Wiley and
	//              sons, New York, 1968.
	
	//  Latest modification: October 12, 1989
	
	//  Authors: W. J. Cody and L. Stoltz
	//           Applied Mathematics Division
	//           Argonne National Laboratory
	//           Argonne, IL 60439
	
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	//  Mathematical constants
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	//  Machine dependent parameters
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	//  Numerator and denominator coefficients for rational minimax
	//     approximation over (1,2).
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	//  Coefficients for minimax approximation over (12, INF).
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	//  Statement functions for conversion between integer and float
	//----------------------------------------------------------------------
	parity = FALSE;
	fact = one;
	n = 0;
	y = x;
	if( y <= zero ) { 
		//----------------------------------------------------------------------
		//  Argument is negative
		//----------------------------------------------------------------------
		y = -x;
		y1 = floor( y );  // original translation was trunc
		res = y - y1;
		if( res != zero ) { 
		  if( y1 != floor( y1*half )*two ) // oritnal translation was trunc
				parity = TRUE;
			fact = -pi/sin( pi*res );
			y = y + one;
		}
		else { 
			res = xinf;
			goto L_900;
		}
	}
	//----------------------------------------------------------------------
	//  Argument is positive
	//----------------------------------------------------------------------
	if( y < eps ) { 
		//----------------------------------------------------------------------
		//  Argument .LT. EPS
		//----------------------------------------------------------------------
		if( y >= xminin ) { 
			res = one/y;
		}
		else { 
			res = xinf;
			goto L_900;
		}
	}
	else if( y < twelve ) { 
		y1 = y;
		if( y < one ) { 
			//----------------------------------------------------------------------
			//  0.0 .LT. argument .LT. 1.0
			//----------------------------------------------------------------------
			z = y;
			y = y + one;
		}
		else { 
			//----------------------------------------------------------------------
			//  1.0 .LT. argument .LT. 12.0, reduce argument if necessary
			//----------------------------------------------------------------------
			n = (long)( y ) - 1;
			y = y - gamma_conv( n );
			z = y - one;
		}
		//----------------------------------------------------------------------
		//  Evaluate approximation for 1.0 .LT. argument .LT. 2.0
		//----------------------------------------------------------------------
		xnum = zero;
		xden = one;
		for( i = 1; i <= 8; i++ ) { 
			i_ = i - 1;
			xnum = (xnum + p[i_])*z;
			xden = xden*z + q[i_];
		}
		res = xnum/xden + one;
		if( y1 < y ) { 
			//----------------------------------------------------------------------
			//  Adjust result for case  0.0 .LT. argument .LT. 1.0
			//----------------------------------------------------------------------
			res = res/y1;
		}
		else if( y1 > y ) { 
			//----------------------------------------------------------------------
			//  Adjust result for case  2.0 .LT. argument .LT. 12.0
			//----------------------------------------------------------------------
			for( i = 1; i <= n; i++ ) { 
				i_ = i - 1;
				res = res*y;
				y = y + one;
			}
		}
	}
	else { 
		//----------------------------------------------------------------------
		//  Evaluate for argument .GE. 12.0,
		//----------------------------------------------------------------------
		if( y <= xbig ) { 
			ysq = y*y;
			sum = c[6];
			for( i = 1; i <= 6; i++ ) { 
				i_ = i - 1;
				sum = sum/ysq + c[i_];
			}
			sum = sum/y - y + sqrtpi;
			sum = sum + (y - half)*log( y );
			res = exp( sum );
		}
		else { 
			res = xinf;
			goto L_900;
		}
	}
	//----------------------------------------------------------------------
	//  Final adjustments and return
	//----------------------------------------------------------------------
	if( parity == TRUE ) 
		res = -res;
	if( fact != one ) 
		res = fact/res;
L_900:
	gamma_v = res;
	return( gamma_v );
	// ---------- Last line of GAMMA ----------
}

//////////////////////////////////////////////////////////////////////////////





