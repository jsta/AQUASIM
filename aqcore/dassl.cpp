////////////////////////////////   dassl.C   /////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    18.11.91    Peter Reichert
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//              26.05.04    Peter Reichert    1.0 replaced by unity in d1mach
//
//////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "all.h"
#include "fortran.h"
#include "linpack.h"
#include "dassl.h"

//////////////////////////////////////////////////////////////////////////////

// forward declaration of auxiliary functions:

void ddawts(INTEGER* neq, INTEGER* iwt, REAL rtol[], REAL atol[],
	    REAL y[], REAL wt[]);

void ddastp(REAL* x, REAL y[], REAL yprime[], INTEGER* neq,
	    void (*res)(REAL* t, REAL y[], REAL yprime[], REAL delta[],
			INTEGER* ires, REAL rpar[], INTEGER ipar[]),
	    void (*jac)(REAL* t, REAL y[], REAL yprime[], REAL pd[],
			REAL* cj, REAL rpar[], INTEGER ipar[]),
	    REAL* h, REAL wt[], INTEGER* jstart,
	    INTEGER* idid, REAL rpar[], INTEGER ipar[], REAL* phi,
	    REAL delta[], REAL e[], REAL wm[], INTEGER iwm[], REAL alpha[],
	    REAL beta[], REAL gamma[], REAL psi[], REAL sigma[], REAL* cj,
	    REAL* cjold, REAL* hold, REAL* s, REAL* hmin, REAL* uround,
	    INTEGER* iphase, INTEGER* jcalc, INTEGER* k, INTEGER* kold,
	    INTEGER* ns, INTEGER* nonneg);

void ddaslv(INTEGER* neq, REAL delta[], REAL wm[], INTEGER iwm[]);

void ddajac(INTEGER* neq, REAL* x, REAL y[], REAL yprime[], REAL delta[],
	    REAL* cj, REAL* h, INTEGER* ier, REAL wt[], REAL e[], REAL wm[],
	    INTEGER iwm[],
	    void (*res)(REAL* t, REAL y[], REAL yprime[], REAL delta[],
			INTEGER* ires, REAL rpar[], INTEGER ipar[]),
	    INTEGER* ires, REAL* uround,
	    void (*jac)(REAL* t, REAL y[], REAL yprime[], REAL pd[],
			REAL* cj, REAL rpar[], INTEGER ipar[]),
	    REAL rpar[], INTEGER ipar[]);

void ddatrp(REAL* x, REAL* xout, REAL yout[], REAL ypout[], INTEGER* neq,
	    INTEGER* kold, REAL* phi, REAL psi[]);

void ddaini(REAL* x, REAL y[], REAL yprime[], INTEGER* neq,
	    void (*res)(REAL* t, REAL y[], REAL yprime[], REAL delta[],
			INTEGER* ires, REAL rpar[], INTEGER ipar[]),
	    void (*jac)(REAL* t, REAL y[], REAL yprime[], REAL pd[],
			REAL* cj, REAL rpar[], INTEGER ipar[]),
	    REAL* h, REAL wt[], INTEGER* idid, REAL rpar[],
	    INTEGER ipar[], REAL* phi, REAL delta[], REAL e[], REAL wm[],
	    INTEGER iwm[], REAL* hmin, REAL* uround, INTEGER* nonneg);

REAL ddanrm(INTEGER* neq, REAL v[], REAL wt[]);

REAL d1mach();

//////////////////////////////////////////////////////////////////////////////


/* COMMON translations
 */

static struct t_dda001 {
	INTEGER npd, ntemp, lml, lmu, lmxord, lmtype, lnst, lnre, lnje, 
	 letf, lctf, lipvt;
	}       dda001;

/* end of COMMON translations
 */

//////////////////////////////////////////////////////////////////////////////

void dassl(void (*res)(REAL* t, REAL y[], REAL yprime[], REAL delta[],
		       INTEGER* ires, REAL rpar[], INTEGER ipar[]),
	   INTEGER* neq, REAL* t, REAL y[],
	   REAL yprime[], REAL* tout, INTEGER info[],
	   REAL rtol[], REAL atol[], INTEGER* idid,
	   REAL rwork[], INTEGER* lrw, INTEGER iwork[], INTEGER* liw,
	   REAL rpar[], INTEGER ipar[],
	   void (*jac)(REAL* t, REAL y[], REAL yprime[], REAL pd[],
		       REAL* cj, REAL rpar[], INTEGER ipar[]),
	   std::ostream& errmes, CARDINAL maxnumsteps)
{
static BOOLEAN done;
static INTEGER i, i_, itemp, le, leniw, lenpd, lenrw, liwm, ljcalc, 
	 lk, lkold, lns, lnstl, lpd, lphase, lphi, lwm, lwt, mband, msave, 
	 mxord, nzflg;
static REAL atoli, h, hmax, hmin, ho, r, rh, rtoli, 
	 tdist, tn, tnext, tstop, uround, ypnorm;
static INTEGER ltstop = 1;
static INTEGER lhmax = 2;
static INTEGER lh = 3;
static INTEGER ltn = 4;
static INTEGER lcj = 5;
static INTEGER lcjold = 6;
static INTEGER lhold = 7;
static INTEGER ls = 8;
static INTEGER lround = 9;
static INTEGER lalpha = 11;
static INTEGER lbeta = 17;
static INTEGER lgamma = 23;
static INTEGER lpsi = 29;
static INTEGER lsigma = 35;
static INTEGER ldelta = 41;


	if( info[0] != 0 )
		goto L_100;

	/*-----------------------------------------------------------------------
 *     THIS BLOCK IS EXECUTED FOR THE INITIAL CALL ONLY.
 *     IT CONTAINS CHECKING OF INPUTS AND INITIALIZATIONS.
 *-----------------------------------------------------------------------
 *
 *     FIRST CHECK INFO ARRAY TO MAKE SURE ALL ELEMENTS OF INFO
 *     ARE EITHER ZERO OR ONE.
 */
	for( i = 2; i <= 11; i++ ){
		i_ = i - 1;
		if( info[i_] != 0 && info[i_] != 1 )
			goto L_701;
		}

	if( *neq <= 0 )
		goto L_702;

/*     SET POINTERS INTO IWORK
 */
	dda001.lml = 1;
	dda001.lmu = 2;
	dda001.lmxord = 3;
	dda001.lmtype = 4;
	ljcalc = 5;
	lphase = 6;
	lk = 7;
	lkold = 8;
	lns = 9;
	lnstl = 10;
	dda001.lnst = 11;
	dda001.lnre = 12;
	dda001.lnje = 13;
	dda001.letf = 14;
	dda001.lctf = 15;
	dda001.lipvt = 21;
	liwm = 1;

/*     CHECK AND COMPUTE MAXIMUM ORDER
 */
	mxord = 5;
	if( info[8] == 0 )
		goto L_20;
	mxord = iwork[dda001.lmxord - 1];
	if( mxord < 1 || mxord > 5 )
		goto L_703;
L_20:
	iwork[dda001.lmxord - 1] = mxord;

/*     COMPUTE MTYPE,LENPD,LENRW.CHECK ML AND MU.
 */
	if( info[5] != 0 )
		goto L_40;
	lenpd = *neq**neq;
	lenrw = 40 + (iwork[dda001.lmxord - 1] + 4)**neq + lenpd;
	if( info[4] != 0 )
		goto L_30;
	iwork[dda001.lmtype - 1] = 2;
	goto L_60;
L_30:
	iwork[dda001.lmtype - 1] = 1;
	goto L_60;
L_40:
	if( iwork[dda001.lml - 1] < 0 || iwork[dda001.lml - 1] >= *neq )
		goto L_717;
	if( iwork[dda001.lmu - 1] < 0 || iwork[dda001.lmu - 1] >= *neq )
		goto L_718;
	lenpd = (2*iwork[dda001.lml - 1] + iwork[dda001.lmu - 1] + 
	 1)**neq;
	if( info[4] != 0 )
		goto L_50;
	iwork[dda001.lmtype - 1] = 5;
	mband = iwork[dda001.lml - 1] + iwork[dda001.lmu - 1] + 1;
	msave = (*neq/mband) + 1;
	lenrw = 40 + (iwork[dda001.lmxord - 1] + 4)**neq + lenpd + 
	 2*msave;
	goto L_60;
L_50:
	iwork[dda001.lmtype - 1] = 4;
	lenrw = 40 + (iwork[dda001.lmxord - 1] + 4)**neq + lenpd;

/*     CHECK LENGTHS OF RWORK AND IWORK
 */
L_60:
	leniw = 20 + *neq;
	if( *lrw < lenrw )
		goto L_704;
	if( *liw < leniw )
		goto L_705;

/*     CHECK TO SEE THAT TOUT IS DIFFERENT FROM T
 */
	if( *tout == *t )
		goto L_719;

	
/*     CHECK HMAX
 */
	if( info[6] == 0 )
		goto L_70;
	hmax = rwork[lhmax - 1];
	if( hmax <= 0.0e0 )
		goto L_710;
L_70:
	;

/*     INITIALIZE COUNTERS
 */
	iwork[dda001.lnst - 1] = 0;
	iwork[dda001.lnre - 1] = 0;
	iwork[dda001.lnje - 1] = 0;

	iwork[lnstl - 1] = 0;
	*idid = 1;
	goto L_200;

	/*-----------------------------------------------------------------------
 *     THIS BLOCK IS FOR CONTINUATION CALLS
 *     ONLY. HERE WE CHECK INFO(1),AND IF THE
 *     LAST STEP WAS INTERRUPTED WE CHECK WHETHER
 *     APPROPRIATE ACTION WAS TAKEN.
 *-----------------------------------------------------------------------
 */
L_100:
	;
	if( info[0] == 1 )
		goto L_110;
	if( info[0] != -1 )
		goto L_701;
/*     IF WE ARE HERE, THE LAST STEP WAS INTERRUPTED
 *     BY AN ERROR CONDITION FROM DDASTP,AND
 *     APPROPRIATE ACTION WAS NOT TAKEN. THIS
 *     IS A FATAL ERROR.
 */
	errmes << "\n   DASSL--  THE LAST STEP TERMINATED WITH A NEGATIVE"
	       << " VALUE"
	       << "\n   DASSL--  OF IDID = " << (*idid) << " AND NO"
	       << " APPROPRIATE"
	       << "\n   DASSL--  ACTION WAS TAKEN. RUN TERMINATED.";
	FatalError("dassl::dassl: Terminating error of DASSL");
	return;
L_110:
	;
	iwork[lnstl - 1] = iwork[dda001.lnst - 1];

	/*-----------------------------------------------------------------------
 *     THIS BLOCK IS EXECUTED ON ALL CALLS.
 *     THE ERROR TOLERANCE PARAMETERS ARE
 *     CHECKED, AND THE WORK ARRAY POINTERS
 *     ARE SET.
 *-----------------------------------------------------------------------
 */
L_200:
	;
/*     CHECK RTOL,ATOL
 */
	nzflg = 0;
	rtoli = rtol[0];
	atoli = atol[0];
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		if( info[1] == 1 )
			rtoli = rtol[i_];
		if( info[1] == 1 )
			atoli = atol[i_];
		if( rtoli > 0.0e0 || atoli > 0.0e0 )
			nzflg = 1;
		if( rtoli < 0.0e0 )
			goto L_706;
		if( atoli < 0.0e0 )
			goto L_707;
		}
	if( nzflg == 0 )
		goto L_708;

/*     SET UP RWORK STORAGE.IWORK STORAGE IS FIXED
 *     IN DATA STATEMENT.
 */
	le = ldelta + *neq;
	lwt = le + *neq;
	lphi = lwt + *neq;
	lpd = lphi + (iwork[dda001.lmxord - 1] + 1)**neq;
	lwm = lpd;
	dda001.npd = 1;
	dda001.ntemp = dda001.npd + lenpd;
	if( info[0] == 1 )
		goto L_400;

	/*-----------------------------------------------------------------------
 *     THIS BLOCK IS EXECUTED ON THE INITIAL CALL
 *     ONLY. SET THE INITIAL STEP SIZE, AND
 *     THE ERROR WEIGHT VECTOR, AND PHI.
 *     COMPUTE INITIAL YPRIME, IF NECESSARY.
 *-----------------------------------------------------------------------
 */
// L_300: (not used)
	;
	tn = *t;
	*idid = 1;

/*     SET ERROR WEIGHT VECTOR WT
 */
	ddawts( neq, &info[1], rtol, atol, y, &rwork[lwt - 1] );
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		if( rwork[lwt + i_ - 1] <= 0.0e0 )
			goto L_713;
		}

/*     COMPUTE UNIT ROUNDOFF AND HMIN
 */
	uround = d1mach();
	rwork[lround - 1] = uround;
	hmin = 4.0e0*uround*fmax( fabs( *t ), fabs( *tout ) );

/*     CHECK INITIAL INTERVAL TO SEE THAT IT IS LONG ENOUGH
 */
	tdist = fabs( *tout - *t );
	if( tdist < hmin )
		goto L_714;

/*     CHECK HO, IF THIS WAS INPUT
 */
	if( info[7] == 0 )
		goto L_310;
	ho = rwork[lh - 1];
	if( (*tout - *t)*ho < 0.0e0 )
		goto L_711;
	if( ho == 0.0e0 )
		goto L_712;
	goto L_320;
L_310:
	;

/*     COMPUTE INITIAL STEPSIZE, TO BE USED BY EITHER
 *     DDASTP OR DDAINI, DEPENDING ON INFO(11)
 */
	ho = 0.001e0*tdist;
	ypnorm = ddanrm( neq, yprime, &rwork[lwt - 1] );
	if( ypnorm > 0.5e0/ho )
		ho = 0.5e0/ypnorm;
	ho = fsign( ho, *tout - *t );

/*     ADJUST HO IF NECESSARY TO MEET HMAX BOUND
 */
L_320:
	if( info[6] == 0 )
		goto L_330;
	rh = fabs( ho )/hmax;
	if( rh > 1.0e0 )
		ho = ho/rh;

/*     COMPUTE TSTOP, IF APPLICABLE
 */
L_330:
	if( info[3] == 0 )
		goto L_340;
	tstop = rwork[ltstop - 1];
	if( (tstop - *t)*ho < 0.0e0 )
		goto L_715;
	if( (*t + ho - tstop)*ho > 0.0e0 )
		ho = tstop - *t;
	if( (tstop - *tout)*ho < 0.0e0 )
		goto L_709;

/*     COMPUTE INITIAL DERIVATIVE, IF APPLICABLE
 */
L_340:
	if( info[10] == 0 )
		goto L_350;
	ddaini( t, y, yprime, neq, res, jac, &ho, &rwork[lwt - 1], idid, 
	 rpar, ipar, &rwork[lphi - 1], &rwork[ldelta - 1], &rwork[le - 1], 
	 &rwork[lwm - 1], &iwork[liwm - 1], &hmin, &rwork[lround - 1], 
	 &info[9] );
	if( *idid < 0 )
		goto L_390;

/*     LOAD H WITH HO.  STORE H IN RWORK(LH)
 */
L_350:
	h = ho;
	rwork[lh - 1] = h;

/*     LOAD Y AND H*YPRIME INTO PHI(*,1) AND PHI(*,2)
 */
// L_360: (not used)
	itemp = lphi + *neq;
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		rwork[lphi + i_ - 1] = y[i_];
		rwork[itemp + i_ - 1] = h*yprime[i_];
		}

L_390:
	goto L_500;

/*-------------------------------------------------------
 *     THIS BLOCK IS FOR CONTINUATION CALLS ONLY. ITS
 *     PURPOSE IS TO CHECK STOP CONDITIONS BEFORE
 *     TAKING A STEP.
 *     ADJUST H IF NECESSARY TO MEET HMAX BOUND
 *-------------------------------------------------------
 */
L_400:
	;
	done = FALSE;
	tn = rwork[ltn - 1];
	h = rwork[lh - 1];
	if( info[6] == 0 )
		goto L_410;
	rh = fabs( h )/hmax;
	if( rh > 1.0e0 )
		h = h/rh;
L_410:
	;
	if( *t == *tout )
		goto L_719;
	if( (*t - *tout)*h > 0.0e0 )
		goto L_711;
	if( info[3] == 1 )
		goto L_430;
	if( info[2] == 1 )
		goto L_420;
	if( (tn - *tout)*h < 0.0e0 )
		goto L_490;
	ddatrp( &tn, tout, y, yprime, neq, &iwork[lkold - 1], &rwork[lphi - 1], 
	 &rwork[lpsi - 1] );
	*t = *tout;
	*idid = 3;
	done = TRUE;
	goto L_490;
L_420:
	if( (tn - *t)*h <= 0.0e0 )
		goto L_490;
	if( (tn - *tout)*h > 0.0e0 )
		goto L_425;
	ddatrp( &tn, &tn, y, yprime, neq, &iwork[lkold - 1], &rwork[lphi - 1], 
	 &rwork[lpsi - 1] );
	*t = tn;
	*idid = 1;
	done = TRUE;
	goto L_490;
L_425:
	;
	ddatrp( &tn, tout, y, yprime, neq, &iwork[lkold - 1], &rwork[lphi - 1], 
	 &rwork[lpsi - 1] );
	*t = *tout;
	*idid = 3;
	done = TRUE;
	goto L_490;
L_430:
	if( info[2] == 1 )
		goto L_440;
	tstop = rwork[ltstop - 1];
	if( (tn - tstop)*h > 0.0e0 )
		goto L_715;
	if( (tstop - *tout)*h < 0.0e0 )
		goto L_709;
	if( (tn - *tout)*h < 0.0e0 )
		goto L_450;
	ddatrp( &tn, tout, y, yprime, neq, &iwork[lkold - 1], &rwork[lphi - 1], 
	 &rwork[lpsi - 1] );
	*t = *tout;
	*idid = 3;
	done = TRUE;
	goto L_490;
L_440:
	tstop = rwork[ltstop - 1];
	if( (tn - tstop)*h > 0.0e0 )
		goto L_715;
	if( (tstop - *tout)*h < 0.0e0 )
		goto L_709;
	if( (tn - *t)*h <= 0.0e0 )
		goto L_450;
	if( (tn - *tout)*h > 0.0e0 )
		goto L_445;
	ddatrp( &tn, &tn, y, yprime, neq, &iwork[lkold - 1], &rwork[lphi - 1], 
	 &rwork[lpsi - 1] );
	*t = tn;
	*idid = 1;
	done = TRUE;
	goto L_490;
L_445:
	;
	ddatrp( &tn, tout, y, yprime, neq, &iwork[lkold - 1], &rwork[lphi - 1], 
	 &rwork[lpsi - 1] );
	*t = *tout;
	*idid = 3;
	done = TRUE;
	goto L_490;
L_450:
	;
/*     CHECK WHETHER WE ARE WITH IN ROUNDOFF OF TSTOP
 */
	if( fabs( tn - tstop ) > 100.0e0*uround*(fabs( tn ) + fabs( h )) )
		goto L_460;
	*idid = 2;
	*t = tstop;
	done = TRUE;
	goto L_490;
L_460:
	tnext = tn + h*(1.0e0 + 4.0e0*uround);
	if( (tnext - tstop)*h <= 0.0e0 )
		goto L_490;
	h = (tstop - tn)*(1.0e0 - 4.0e0*uround);
	rwork[lh - 1] = h;

L_490:
	if( done == TRUE )
		goto L_590;

/*-------------------------------------------------------
 *     THE NEXT BLOCK CONTAINS THE CALL TO THE
 *     ONE-STEP INTEGRATOR DDASTP.
 *     THIS IS A LOOPING POINT FOR THE INTEGRATION
 *     STEPS.
 *     CHECK FOR TOO MANY STEPS.
 *     UPDATE WT.
 *     CHECK FOR TOO MUCH ACCURACY REQUESTED.
 *     COMPUTE MINIMUM STEPSIZE.
 *-------------------------------------------------------
 */
L_500:
	;
/*     CHECK FOR FAILURE TO COMPUTE INITIAL YPRIME
 */
	if( *idid == -12 )
		goto L_527;

/*     CHECK FOR TOO MANY STEPS
 */
	if( (iwork[dda001.lnst - 1] - iwork[lnstl - 1]) < maxnumsteps )
		goto L_510;
	*idid = -1;
	goto L_527;

/*     UPDATE WT
 */
L_510:
	ddawts( neq, &info[1], rtol, atol, &rwork[lphi - 1], &rwork[lwt - 1] );
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		if( rwork[i_ + lwt - 1] > 0.0e0 )
			goto L_520;
		*idid = -3;
		goto L_527;
L_520:
		;
		}

/*     TEST FOR TOO MUCH ACCURACY REQUESTED.
 */
	r = ddanrm( neq, &rwork[lphi - 1], &rwork[lwt - 1] )*
	 100.0e0*uround;
	if( r <= 1.0e0 )
		goto L_525;

/*     MULTIPLY RTOL AND ATOL BY R AND RETURN
 */
	if( info[1] == 1 )
		goto L_523;
	rtol[0] = r*rtol[0];
	atol[0] = r*atol[0];
	*idid = -2;
	goto L_527;
L_523:
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		rtol[i_] = r*rtol[i_];
		atol[i_] = r*atol[i_];
		}
	*idid = -2;
	goto L_527;
L_525:
	;

/*     COMPUTE MINIMUM STEPSIZE
 */
	hmin = 4.0e0*uround*fmax( fabs( tn ), fabs( *tout ) );

	ddastp( &tn, y, yprime, neq, res, jac, &h, &rwork[lwt - 1], &info[0], 
	 idid, rpar, ipar, &rwork[lphi - 1], &rwork[ldelta - 1], &rwork[le - 1], 
	 &rwork[lwm - 1], &iwork[liwm - 1], &rwork[lalpha - 1], &rwork[lbeta - 1], 
	 &rwork[lgamma - 1], &rwork[lpsi - 1], &rwork[lsigma - 1], 
	 &rwork[lcj - 1], &rwork[lcjold - 1], &rwork[lhold - 1], &rwork[ls - 1], 
	 &hmin, &rwork[lround - 1], &iwork[lphase - 1], &iwork[ljcalc - 1], 
	 &iwork[lk - 1], &iwork[lkold - 1], &iwork[lns - 1], &info[9] );

/*     CHECK FOR MAXIMUM STEPSIZE  (Statements added by PR 06.01.95)
*/
	if( info[6] != 0 ) { if ( h > hmax ) h = hmax; }

/*     (End of statements added by PR)
*/

L_527:
	if( *idid < 0 )
		goto L_600;

/*------------------------------------------------------
 *     THIS BLOCK HANDLES THE CASE OF A SUCCESSFUL
 *     RETURN FROM DDASTP (IDID=1) TEST FOR
 *     STOP CONDITIONS.
 *--------------------------------------------------------
 */
	if( info[3] != 0 )
		goto L_540;
	if( info[2] != 0 )
		goto L_530;
	if( (tn - *tout)*h < 0.0e0 )
		goto L_500;
	ddatrp( &tn, tout, y, yprime, neq, &iwork[lkold - 1], &rwork[lphi - 1], 
	 &rwork[lpsi - 1] );
	*idid = 3;
	*t = *tout;
	goto L_580;
L_530:
	if( (tn - *tout)*h >= 0.0e0 )
		goto L_535;
	*t = tn;
	*idid = 1;
	goto L_580;
L_535:
	ddatrp( &tn, tout, y, yprime, neq, &iwork[lkold - 1], &rwork[lphi - 1], 
	 &rwork[lpsi - 1] );
	*idid = 3;
	*t = *tout;
	goto L_580;
L_540:
	if( info[2] != 0 )
		goto L_550;
	if( (tn - *tout)*h < 0.0e0 )
		goto L_542;
	ddatrp( &tn, tout, y, yprime, neq, &iwork[lkold - 1], &rwork[lphi - 1], 
	 &rwork[lpsi - 1] );
	*t = *tout;
	*idid = 3;
	goto L_580;
L_542:
	if( fabs( tn - tstop ) <= 100.0e0*uround*(fabs( tn ) + fabs( h )) )
		goto L_545;
	tnext = tn + h*(1.0e0 + 4.0e0*uround);
	if( (tnext - tstop)*h <= 0.0e0 )
		goto L_500;
	h = (tstop - tn)*(1.0e0 - 4.0e0*uround);
	goto L_500;
L_545:
	*idid = 2;
	*t = tstop;
	goto L_580;
L_550:
	if( (tn - *tout)*h >= 0.0e0 )
		goto L_555;
	if( fabs( tn - tstop ) <= 100.0e0*uround*(fabs( tn ) + fabs( h )) )
		goto L_552;
	*t = tn;
	*idid = 1;
	goto L_580;
L_552:
	*idid = 2;
	*t = tstop;
	goto L_580;
L_555:
	ddatrp( &tn, tout, y, yprime, neq, &iwork[lkold - 1], &rwork[lphi - 1], 
	 &rwork[lpsi - 1] );
	*t = *tout;
	*idid = 3;
L_580:
	;

/*--------------------------------------------------------
 *     ALL SUCCESSFUL RETURNS FROM DASSL ARE MADE FROM
 *     THIS BLOCK.
 *--------------------------------------------------------
 */
L_590:
	;
	rwork[ltn - 1] = tn;
	rwork[lh - 1] = h;
	return;

	/*-----------------------------------------------------------------------
 *     THIS BLOCK HANDLES ALL UNSUCCESSFUL
 *     RETURNS OTHER THAN FOR ILLEGAL INPUT.
 *-----------------------------------------------------------------------
 */
L_600:
	;
	itemp = -*idid;
	switch( itemp ){
		case 1: goto L_610;
		case 2: goto L_620;
		case 3: goto L_630;
		case 4: goto L_690;
		case 5: goto L_690;
		case 6: goto L_640;
		case 7: goto L_650;
		case 8: goto L_660;
		case 9: goto L_670;
		case 10: goto L_675;
		case 11: goto L_680;
		case 12: goto L_685;
		}

/*     THE MAXIMUM NUMBER OF STEPS WAS TAKEN BEFORE
 *     REACHING TOUT
 */
L_610:
	errmes << "\n   DASSL--  AT CURRENT T = " << tn
	       << " " << maxnumsteps <<" STEPS"
	       << "\n   DASSL--  TAKEN ON THIS CALL BEFORE REACHING TOUT.";
	goto L_690;

/*     TOO MUCH ACCURACY FOR MACHINE PRECISION
 */
L_620:
	errmes << "\n   DASSL--  AT T = " << tn << " TOO MUCH ACCURACY"
	       << " REQUESTED"
	       << "\n   DASSL--  FOR PRECISION OF MACHINE. RTOL AND ATOL"
	       << "\n   DASSL--  WERE INCREASED TO APPROPRIATE VALUES.";
	goto L_690;

/*     WT(I) .LE. 0.0D0 FOR SOME I (NOT AT START OF PROBLEM)
 */
L_630:
	errmes << "\n   DASSL--  AT T = " << tn << " SOME ELEMENT OF WT"
	       << "\n   DASSL--  HAS BECOME <= 0.0.";
	goto L_690;

/*     ERROR TEST FAILED REPEATEDLY OR WITH H=HMIN
 */
L_640:
	errmes << "\n   DASSL--  AT T = " << tn << " AND STEPSIZE H = " << h
	       << "\n   DASSL--  THE ERROR TEST FAILED REPEATEDLY"
	       << "\n   DASSL--  OR ABS(H) = HMIN.";
	goto L_690;

/*     CORRECTOR CONVERGENCE FAILED REPEATEDLY OR WITH H=HMIN
 */
L_650:
	errmes << "\n   DASSL--  AT T = " << tn << " AND STEPSIZE H = " << h
	       << "\n   DASSL--  THE CORRECTOR FAILED TO CONVERGE REPEATEDLY"
	       << "\n   DASSL--  OR ABS(H) = HMIN.";    
	goto L_690;

/*     THE ITERATION MATRIX IS SINGULAR
 */
L_660:
	errmes << "\n   DASSL--  AT T = " << tn << " AND STEPSIZE H = " << h
	       << "\n   DASSL--  THE ITERATION MATRIX IS SINGULAR.";
	goto L_690;

/*     CORRECTOR FAILURE PRECEEDED BY ERROR TEST FAILURES.
 */
L_670:
	errmes << "\n   DASSL--  AT T = " << tn << " AND STEPSIZE H = " << h
	       << "\n   DASSL--  THE CORRECTOR COULD NOT CONVERGE. ALSO THE"
	       << "\n   DASSL--  ERROR TEST FAILED REPEATEDLY.";
	goto L_690;

/*     CORRECTOR FAILURE BECAUSE IRES = -1
 */
L_675:
	errmes << "\n   DASSL--  AT T = " << tn << " AND STEPSIZE H = " << h
	       << "\n   DASSL--  THE CORRECTOR COULD NOT CONVERGE BECAUSE"
	       << "\n   DASSL--  IRES WAS EQUAL TO MINUS ONE";
	goto L_690;

/*     FAILURE BECAUSE IRES = -2
 */
L_680:
	errmes << "\n   DASSL--  AT T = " << tn << " AND STEPSIZE H = " << h
	       << "\n   DASSL--  IRES WAS EQUAL TO MINUS TWO";
	goto L_690;

/*     FAILED TO COMPUTE INITIAL YPRIME
 */
L_685:
	errmes << "\n   DASSL--  AT T = " << tn << " AND STEPSIZE H = " << h
	       << "\n   DASSL--  THE INITIAL YPRIME COULD NOT BE COMPUTED";
	goto L_690;
L_690:
	;
	info[0] = -1;
	*t = tn;
	rwork[ltn - 1] = tn;
	rwork[lh - 1] = h;
	return;
	/*-----------------------------------------------------------------------
 *     THIS BLOCK HANDLES ALL ERROR RETURNS DUE
 *     TO ILLEGAL INPUT, AS DETECTED BEFORE CALLING
 *     DDASTP. FIRST THE ERROR MESSAGE ROUTINE IS
 *     CALLED. IF THIS HAPPENS TWICE IN
 *     SUCCESSION, EXECUTION IS TERMINATED
 *
 *-----------------------------------------------------------------------
 */
L_701:
	errmes << "\n   DASSL--  SOME ELEMENT OF INFO VECTOR IS NOT ZERO"
	       << " OR ONE";
	goto L_750;
L_702:
	errmes << "\n   DASSL--  NEQ = " << (*neq) << " <= 0";
	goto L_750;
L_703:
	errmes << "\n   DASSL--  MAXORD=" << mxord << " NOT IN RANGE";
	goto L_750;
L_704:
	errmes << "\n   DASSL--  RWORK LENGTH NEEDED, LENRW = " << lenrw
	       << ","
	       << "\n   DASSL--  EXCEEDS LRW = " << (*lrw);
	goto L_750;
L_705:
	errmes << "\n   DASSL--  IWORK LENGTH NEEDED, LENIW = " << leniw
	       << ","
	       << "\n   DASSL--  EXCEEDS LIW = " << (*liw);
	goto L_750;
L_706:
	errmes << "\n   DASSL--  SOME ELEMEMT OF RTOL IS < 0.0";
	goto L_750;
L_707:
	errmes << "\n   DASSL--  SOME ELEMEMT OF ATOL IS < 0.0";
	goto L_750;
L_708:
	errmes << "\n   DASSL--  ALL ELEMEMTS OF RTOL AND ATOL ARE ZERO";
	goto L_750;
L_709:
	errmes << "\n   DASSL--  INFO(4) = 1 AND TSTOP = " << tstop
	       << "\n   DASSL--  BEHIND TOUT=" << (*tout);
	goto L_750;
L_710:
	errmes << "\n   DASSL--  HMAX = " << hmax << " < 0.0";
	goto L_750;
L_711:
	errmes << "\n   DASSL--  TOUT = " << (*tout) << " BEHIND T = " << *t;
	goto L_750;
L_712:
	errmes << "\n   DASSL--  INFO(8) = 1 and H0 = 0.0";
	goto L_750;
L_713:
	errmes << "\n   DASSL--  SOME ELEMENT OF WT IS <= 0.0";
	goto L_750;
L_714:
	errmes << "\n   DASSL--  TOUT = " << (*tout) << " TOO CLOSE TO T = "
	       << (*t)
	       << "\n   DASSL--  TO START INTEGRATION";
	goto L_750;
L_715:
	errmes << "\n   DASSL--  INFO(4) = 1 AND TSTOP = " << tstop
	       << " BEHIND T = " << (*t);
	goto L_750;
L_717:
	errmes << "\n   DASSL--  ML = " << iwork[dda001.lml - 1] << "ILLEGAL."
	       << "\n   DASSL--  EITHER < 0 OR > NEQ";
	goto L_750;
L_718:
	errmes << "\n   DASSL--  MU = " << iwork[dda001.lmu - 1] << "ILLEGAL."
	       << "\n   DASSL--  EITHER < 0 OR > NEQ";
	goto L_750;
L_719:
	errmes << "\n   DASSL--  TOUT = " << (*tout) << " IS EQUAL TO T = "
	       << (*t);
	goto L_750;
L_750:
	if( info[0] == -1 )
		goto L_760;
	info[0] = -1;
	*idid = -33;
	return;
L_760:
	errmes << "\n   DASSL--  REPEATED OCCURRENCES OF ILLEGAL INPUT"
	       << "\n   DASSL--  RUN TERMINATED. APPARENT INFINITE LOOP";
	FatalError("dassl::dassl: Terminating error of DASSL");
	return;
/*-----------END OF SUBROUTINE DASSL-------------------------------------
 */
}

//////////////////////////////////////////////////////////////////////////////

void ddawts(INTEGER* neq, INTEGER* iwt, REAL rtol[], REAL atol[],
	    REAL y[], REAL wt[])

/****BEGIN PROLOGUE  DDAWTS
 ****REFER TO  DASSL
 ****ROUTINES CALLED  (NONE)
 ****DATE WRITTEN   830315   (YYMMDD)
 ****REVISION DATE  830315   (YYMMDD)
 ****END PROLOGUE  DDAWTS
 *-----------------------------------------------------------------------
 *     THIS SUBROUTINE SETS THE ERROR WEIGHT VECTOR
 *     WT ACCORDING TO WT(I)=RTOL(I)*ABS(Y(I))+ATOL(I),
 *     I=1,-,N.
 *     RTOL AND ATOL ARE SCALARS IF IWT = 0,
 *     AND VECTORS IF IWT = 1.
 *-----------------------------------------------------------------------
 */

{
static INTEGER i, i_;
static REAL atoli, rtoli;

	rtoli = rtol[0];
	atoli = atol[0];
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		if( *iwt == 0 )
			goto L_10;
		rtoli = rtol[i_];
		atoli = atol[i_];
L_10:
		wt[i_] = rtoli*fabs( y[i_] ) + atoli;
		}
	return;
/*-----------END OF SUBROUTINE DDAWTS-------------------------------------
 */
}

//////////////////////////////////////////////////////////////////////////////

void ddastp(REAL* x, REAL y[], REAL yprime[], INTEGER* neq,
	    void (*res)(REAL* t, REAL y[], REAL yprime[], REAL delta[],
			INTEGER* ires, REAL rpar[], INTEGER ipar[]),
	    void (*jac)(REAL* t, REAL y[], REAL yprime[], REAL pd[],
			REAL* cj, REAL rpar[], INTEGER ipar[]),
	    REAL* h, REAL wt[], INTEGER* jstart,
	    INTEGER* idid, REAL rpar[], INTEGER ipar[], REAL* phi,
	    REAL delta[], REAL e[], REAL wm[], INTEGER iwm[], REAL alpha[],
	    REAL beta[], REAL gamma[], REAL psi[], REAL sigma[], REAL* cj,
	    REAL* cjold, REAL* hold, REAL* s, REAL* hmin, REAL* uround,
	    INTEGER* iphase, INTEGER* jcalc, INTEGER* k, INTEGER* kold,
	    INTEGER* ns, INTEGER* nonneg)

/****BEGIN PROLOGUE  DDASTP
 ****REFER TO  DASSL
 ****ROUTINES CALLED  DDANRM,DDAJAC,DDASLV,DDATRP
 ****COMMON BLOCKS    DDA001
 ****DATE WRITTEN   830315   (YYMMDD)
 ****REVISION DATE  830315   (YYMMDD)
 ****END PROLOGUE  DDASTP
 *
 *
 *-----------------------------------------------------------------------
 *     DASTEP SOLVES A SYSTEM OF DIFFERENTIAL
 *     ALGEBRAIC EQUATIONS OF THE FORM
 *     G(X,Y,YPRIME) = 0,  FOR ONE STEP (NORMALLY
 *     FROM X TO X+H).
 *
 *     THE METHODS USED ARE MODIFIED DIVIDED
 *     DIFFERENCE,FIXED LEADING COEFFICIENT
 *     FORMS OF BACKWARD DIFFERENTIATION
 *     FORMULAS. THE CODE ADJUSTS THE STEPSIZE
 *     AND ORDER TO CONTROL THE LOCAL ERROR PER
 *     STEP.
 *
 *
 *     THE PARAMETERS REPRESENT
 *     X  --        INDEPENDENT VARIABLE
 *     Y  --        SOLUTION VECTOR AT X
 *     YPRIME --    DERIVATIVE OF SOLUTION VECTOR
 *                  AFTER SUCCESSFUL STEP
 *     NEQ --       NUMBER OF EQUATIONS TO BE INTEGRATED
 *     RES --       EXTERNAL USER-SUPPLIED SUBROUTINE
 *                  TO EVALUATE THE RESIDUAL.  THE CALL IS
 *                  CALL RES(X,Y,YPRIME,DELTA,IRES,RPAR,IPAR)
 *                  X,Y,YPRIME ARE INPUT.  DELTA IS OUTPUT.
 *                  ON INPUT, IRES=0.  RES SHOULD ALTER IRES ONLY
 *                  IF IT ENCOUNTERS AN ILLEGAL VALUE OF Y OR A
 *                  STOP CONDITION.  SET IRES=-1 IF AN INPUT VALUE
 *                  OF Y IS ILLEGAL, AND DASTEP WILL TRY TO SOLVE
 *                  THE PROBLEM WITHOUT GETTING IRES = -1.  IF
 *                  IRES=-2, DASTEP RETURNS CONTROL TO THE CALLING
 *                  PROGRAM WITH IDID = -11.
 *     JAC --       EXTERNAL USER-SUPPLIED ROUTINE TO EVALUATE
 *                  THE ITERATION MATRIX (THIS IS OPTIONAL)
 *                  THE CALL IS OF THE FORM
 *                  CALL JAC(X,Y,YPRIME,PD,CJ,RPAR,IPAR)
 *                  PD IS THE MATRIX OF PARTIAL DERIVATIVES,
 *                  PD=DG/DY+CJ*DG/DYPRIME
 *     H --         APPROPRIATE STEP SIZE FOR NEXT STEP.
 *                  NORMALLY DETERMINED BY THE CODE
 *     WT --        VECTOR OF WEIGHTS FOR ERROR CRITERION.
 *     JSTART --    INTEGER VARIABLE SET 0 FOR
 *                  FIRST STEP, 1 OTHERWISE.
 *     IDID --      COMPLETION CODE WITH THE FOLLOWING MEANINGS
 *                  IDID= 1 -- THE STEP WAS COMPLETED SUCCESSFULLY
 *                  IDID=-6 -- THE ERROR TEST FAILED REPEATEDLY
 *                  IDID=-7 -- THE CORRECTOR COULD NOT CONVERGE
 *                  IDID=-8 -- THE ITERATION MATRIX IS SINGULAR
 *                  IDID=-9 -- THE CORRECTOR COULD NOT CONVERGE.
 *                             THERE WERE REPEATED ERROR TEST
 *                             FAILURES ON THIS STEP.
 *                  IDID=-10-- THE CORRECTOR COULD NOT CONVERGE
 *                             BECAUSE IRES WAS EQUAL TO MINUS ONE
 *                  IDID=-11-- IRES EQUAL TO -2 WAS ENCOUNTERED,
 *                             AND CONTROL IS BEING RETURNED TO
 *                             THE CALLING PROGRAM
 *     RPAR,IPAR -- REAL AND INTEGER PARAMETER ARRAYS THAT
 *                  ARE USED FOR COMMUNICATION BETWEEN THE
 *                  CALLING PROGRAM AND EXTERNAL USER ROUTINES
 *                  THEY ARE NOT ALTERED BY DASTEP
 *     PHI --       ARRAY OF DIVIDED DIFFERENCES USED BY
 *                  DASTEP. THE LENGTH IS NEQ*(K+1),WHERE
 *                  K IS THE MAXIMUM ORDER
 *     DELTA,E --   WORK VECTORS FOR DASTEP OF LENGTH NEQ
 *     WM,IWM --    REAL AND INTEGER ARRAYS STORING
 *                  MATRIX INFORMATION SUCH AS THE MATRIX
 *                  OF PARTIAL DERIVATIVES,PERMUTATION
 *                  VECTOR,AND VARIOUS OTHER INFORMATION.
 *
 *     THE OTHER PARAMETERS ARE INFORMATION
 *     WHICH IS NEEDED INTERNALLY BY DASTEP TO
 *     CONTINUE FROM STEP TO STEP.
 *
 *-----------------------------------------------------------------------
 */

{
#define PHI(I_,J_)      (*(phi+(I_)*(*neq)+(J_)))
static BOOLEAN convgd;
static INTEGER i, i_, ier, ires, j, j1, j_, kdiff, km1, knew, 
	 kp1, kp2, m, ncf, nef, nsf, nsp1;
static REAL alpha0, alphas, cjlast, ck, delnrm, enorm, 
	 erk, erkm1, erkm2, erkp1, err, est, hnew, oldnrm, pnorm, r, rate, 
	 temp1, temp2, terk, terkm1, terkm2, terkp1, xold;
static INTEGER maxit = 4;
static REAL xrate = 0.25e0;
/*-----------------------------------------------------------------------
 *     BLOCK 1.
 *     INITIALIZE. ON THE FIRST CALL,SET
 *     THE ORDER TO 1 AND INITIALIZE
 *     OTHER VARIABLES.
 *-----------------------------------------------------------------------
 *
 *     INITIALIZATIONS FOR ALL CALLS
 */
	*idid = 1;
	xold = *x;
	ncf = 0;
	nsf = 0;
	nef = 0;
	if( *jstart != 0 )
		goto L_120;

/*     IF THIS IS THE FIRST STEP,PERFORM
 *     OTHER INITIALIZATIONS
 */
	iwm[dda001.letf - 1] = 0;
	iwm[dda001.lctf - 1] = 0;
	*k = 1;
	*kold = 0;
	*hold = 0.0e0;
	*jstart = 1;
	psi[0] = *h;
	*cjold = 1.0e0/ *h;
	*cj = *cjold;
	*s = 100.e0;
	*jcalc = -1;
	delnrm = 1.0e0;
	*iphase = 0;
	*ns = 0;
L_120:
	;

	/*-----------------------------------------------------------------------
 *     BLOCK 2
 *     COMPUTE COEFFICIENTS OF FORMULAS FOR
 *     THIS STEP.
 *-----------------------------------------------------------------------
 */
L_200:
	;
	kp1 = *k + 1;
	kp2 = *k + 2;
	km1 = *k - 1;
	xold = *x;
	if( *h != *hold || *k != *kold )
		*ns = 0;
	*ns = fmin( *ns + 1, *kold + 2 );
	nsp1 = *ns + 1;
	if( kp1 < *ns )
		goto L_230;

	beta[0] = 1.0e0;
	alpha[0] = 1.0e0;
	temp1 = *h;
	gamma[0] = 0.0e0;
	sigma[0] = 1.0e0;
	for( i = 2; i <= kp1; i++ ){
		i_ = i - 1;
		temp2 = psi[i_ - 1];
		psi[i_ - 1] = temp1;
		beta[i_] = beta[i_ - 1]*psi[i_ - 1]/temp2;
		temp1 = temp2 + *h;
		alpha[i_] = *h/temp1;
		sigma[i_] = (REAL)( i - 1 )*sigma[i_ - 1]*alpha[i_];
		gamma[i_] = gamma[i_ - 1] + alpha[i_ - 1]/ *h;
		}
	psi[kp1 - 1] = temp1;
L_230:
	;

/*     COMPUTE ALPHAS, ALPHA0
 */
	alphas = 0.0e0;
	alpha0 = 0.0e0;
	for( i = 1; i <= *k; i++ ){
		i_ = i - 1;
		alphas = alphas - 1.0e0/(REAL)( i );
		alpha0 = alpha0 - alpha[i_];
		}

/*     COMPUTE LEADING COEFFICIENT CJ
 */
	cjlast = *cj;
	*cj = -alphas/ *h;

/*     COMPUTE VARIABLE STEPSIZE ERROR COEFFICIENT CK
 */
	ck = fabs( alpha[kp1 - 1] + alphas - alpha0 );
	ck = fmax( ck, alpha[kp1 - 1] );

/*     DECIDE WHETHER NEW JACOBIAN IS NEEDED
 */
	temp1 = (1.0e0 - xrate)/(1.0e0 + xrate);
	temp2 = 1.0e0/temp1;
	if( *cj/ *cjold < temp1 || *cj/ *cjold > temp2 )
		*jcalc = -1;
	if( *cj != cjlast )
		*s = 100.e0;

/*     CHANGE PHI TO PHI STAR
 */
	if( kp1 < nsp1 )
		goto L_280;
	for( j = nsp1; j <= kp1; j++ ){
		j_ = j - 1;
		for( i = 1; i <= *neq; i++ ){
			i_ = i - 1;
			PHI(j_,i_) = beta[j_]*PHI(j_,i_);
			}
		}
L_280:
	;

/*     UPDATE TIME
 */
	*x = *x + *h;

	/*-----------------------------------------------------------------------
 *     BLOCK 3
 *     PREDICT THE SOLUTION AND DERIVATIVE,
 *     AND SOLVE THE CORRECTOR EQUATION
 *-----------------------------------------------------------------------
 *
 *     FIRST,PREDICT THE SOLUTION AND DERIVATIVE
 */
L_300:
	;
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		y[i_] = PHI(0,i_);
		yprime[i_] = 0.0e0;
		}
	for( j = 2; j <= kp1; j++ ){
		j_ = j - 1;
		for( i = 1; i <= *neq; i++ ){
			i_ = i - 1;
			y[i_] = y[i_] + PHI(j_,i_);
			yprime[i_] = yprime[i_] + gamma[j_]*PHI(j_,i_);
			}
		}
	pnorm = ddanrm( neq, y, wt );


/*     SOLVE THE CORRECTOR EQUATION USING A
 *     MODIFIED NEWTON SCHEME.
 */
	convgd = TRUE;
	m = 0;
	iwm[dda001.lnre - 1] = iwm[dda001.lnre - 1] + 1;
	ires = 0;
	(*res)( x, y, yprime, delta, &ires, rpar, ipar );
	if( ires < 0 )
		goto L_380;


/*     IF INDICATED,REEVALUATE THE
 *     ITERATION MATRIX PD = DG/DY + CJ*DG/DYPRIME
 *     (WHERE G(X,Y,YPRIME)=0). SET
 *     JCALC TO 0 AS AN INDICATOR THAT
 *     THIS HAS BEEN DONE.
 */
	if( *jcalc != -1 )
		goto L_340;
	iwm[dda001.lnje - 1] = iwm[dda001.lnje - 1] + 1;
	*jcalc = 0;
	ddajac( neq, x, y, yprime, delta, cj, h, &ier, wt, e, wm, iwm, 
	 res, &ires, uround, jac, rpar, ipar );
	*cjold = *cj;
	*s = 100.e0;
	if( ires < 0 )
		goto L_380;
	if( ier != 0 )
		goto L_380;
	nsf = 0;


/*     INITIALIZE THE ERROR ACCUMULATION VECTOR E.
 */
L_340:
	;
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		e[i_] = 0.0e0;
		}

	*s = 100.e0;


/*     CORRECTOR LOOP.
 */
L_350:
	;

/*     MULTIPLY RESIDUAL BY TEMP1 TO ACCELERATE CONVERGENCE
 */
	temp1 = 2.0e0/(1.0e0 + *cj/ *cjold);
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		delta[i_] = delta[i_]*temp1;
		}

/*     COMPUTE A NEW ITERATE (BACK-SUBSTITUTION).
 *     STORE THE CORRECTION IN DELTA.
 */
	ddaslv( neq, delta, wm, iwm );

/*     UPDATE Y,E,AND YPRIME
 */
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		y[i_] = y[i_] - delta[i_];
		e[i_] = e[i_] - delta[i_];
		yprime[i_] = yprime[i_] - *cj*delta[i_];
		}

/*     TEST FOR CONVERGENCE OF THE ITERATION
 */
	delnrm = ddanrm( neq, delta, wt );
	if( delnrm <= 100.e0**uround*pnorm )
		goto L_375;
	if( m > 0 )
		goto L_365;
	oldnrm = delnrm;
	goto L_367;
L_365:
	rate = pow(delnrm/oldnrm, 1.0e0/(REAL)( m ));
	if( rate > 0.90e0 )
		goto L_370;
	*s = rate/(1.0e0 - rate);
L_367:
	if( *s*delnrm <= 0.33e0 )
		goto L_375;

/*     THE CORRECTOR HAS NOT YET CONVERGED.
 *     UPDATE M AND TEST WHETHER THE
 *     MAXIMUM NUMBER OF ITERATIONS HAVE
 *     BEEN TRIED.
 */
	m = m + 1;
	if( m >= maxit )
		goto L_370;

/*     EVALUATE THE RESIDUAL
 *     AND GO BACK TO DO ANOTHER ITERATION
 */
	iwm[dda001.lnre - 1] = iwm[dda001.lnre - 1] + 1;
	ires = 0;
	(*res)( x, y, yprime, delta, &ires, rpar, ipar );
	if( ires < 0 )
		goto L_380;
	goto L_350;


/*     THE CORRECTOR FAILED TO CONVERGE IN MAXIT
 *     ITERATIONS. IF THE ITERATION MATRIX
 *     IS NOT CURRENT,RE-DO THE STEP WITH
 *     A NEW ITERATION MATRIX.
 */
L_370:
	;
	if( *jcalc == 0 )
		goto L_380;
	*jcalc = -1;
	goto L_300;


/*     THE ITERATION HAS CONVERGED.  IF NONNEGATIVITY OF SOLUTION IS
 *     REQUIRED, SET THE SOLUTION NONNEGATIVE, IF THE PERTURBATION
 *     TO DO IT IS SMALL ENOUGH.  IF THE CHANGE IS TOO LARGE, THEN
 *     CONSIDER THE CORRECTOR ITERATION TO HAVE FAILED.
 */
L_375:
	if( *nonneg == 0 )
		goto L_390;
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		delta[i_] = fmin( y[i_], 0.0e0 );
		}
	delnrm = ddanrm( neq, delta, wt );
	if( delnrm > 0.33e0 )
		goto L_380;
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		e[i_] = e[i_] - delta[i_];
		}
	goto L_390;


/*     EXITS FROM BLOCK 3
 *     NO CONVERGENCE WITH CURRENT ITERATION
 *     MATRIX,OR SINGULAR ITERATION MATRIX
 */
L_380:
	convgd = FALSE;
L_390:
	*jcalc = 1;
	if( convgd == FALSE )
		goto L_600;

	/*-----------------------------------------------------------------------
 *     BLOCK 4
 *     ESTIMATE THE ERRORS AT ORDERS K,K-1,K-2
 *     AS IF CONSTANT STEPSIZE WAS USED. ESTIMATE
 *     THE LOCAL ERROR AT ORDER K AND TEST
 *     WHETHER THE CURRENT STEP IS SUCCESSFUL.
 *-----------------------------------------------------------------------
 *
 *     ESTIMATE ERRORS AT ORDERS K,K-1,K-2
 */
	enorm = ddanrm( neq, e, wt );
	erk = sigma[*k]*enorm;
	terk = (float)( *k + 1 )*erk;
	est = erk;
	knew = *k;
	if( *k == 1 )
		goto L_430;
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		delta[i_] = PHI(kp1 - 1,i_) + e[i_];
		}
	erkm1 = sigma[*k - 1]*ddanrm( neq, delta, wt );
	terkm1 = (float)( *k )*erkm1;
	if( *k > 2 )
		goto L_410;
	if( terkm1 <= 0.5*terk )
		goto L_420;
	goto L_430;
L_410:
	;
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		delta[i_] = PHI(*k - 1,i_) + delta[i_];
		}
	erkm2 = sigma[*k - 2]*ddanrm( neq, delta, wt );
	terkm2 = (float)( *k - 1 )*erkm2;
	if( fmax( terkm1, terkm2 ) > terk )
		goto L_430;

/*     LOWER THE ORDER
 */
L_420:
	;
	knew = *k - 1;
	est = erkm1;


/*     CALCULATE THE LOCAL ERROR FOR THE CURRENT STEP
 *     TO SEE IF THE STEP WAS SUCCESSFUL
 */
L_430:
	;
	err = ck*enorm;
	if( err > 1.0e0 )
		goto L_600;

	/*-----------------------------------------------------------------------
 *     BLOCK 5
 *     THE STEP IS SUCCESSFUL. DETERMINE
 *     THE BEST ORDER AND STEPSIZE FOR
 *     THE NEXT STEP. UPDATE THE DIFFERENCES
 *     FOR THE NEXT STEP.
 *-----------------------------------------------------------------------
 */
	*idid = 1;
	iwm[dda001.lnst - 1] = iwm[dda001.lnst - 1] + 1;
	kdiff = *k - *kold;
	*kold = *k;
	*hold = *h;


/*     ESTIMATE THE ERROR AT ORDER K+1 UNLESS
 *        ALREADY DECIDED TO LOWER ORDER, OR
 *        ALREADY USING MAXIMUM ORDER, OR
 *        STEPSIZE NOT CONSTANT, OR
 *        ORDER RAISED IN PREVIOUS STEP
 */
	if( knew == km1 || *k == iwm[dda001.lmxord - 1] )
		*iphase = 1;
	if( *iphase == 0 )
		goto L_545;
	if( knew == km1 )
		goto L_540;
	if( *k == iwm[dda001.lmxord - 1] )
		goto L_550;
	if( kp1 >= *ns || kdiff == 1 )
		goto L_550;
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		delta[i_] = e[i_] - PHI(kp2 - 1,i_);
		}
	erkp1 = (1.0e0/(REAL)( *k + 2 ))*ddanrm( neq, delta, wt );
	terkp1 = (float)( *k + 2 )*erkp1;
	if( *k > 1 )
		goto L_520;
	if( terkp1 >= 0.5e0*terk )
		goto L_550;
	goto L_530;
L_520:
	if( terkm1 <= fmin( terk, terkp1 ) )
		goto L_540;
	if( terkp1 >= terk || *k == iwm[dda001.lmxord - 1] )
		goto L_550;

/*     RAISE ORDER
 */
L_530:
	*k = kp1;
	est = erkp1;
	goto L_550;

/*     LOWER ORDER
 */
L_540:
	*k = km1;
	est = erkm1;
	goto L_550;

/*     IF IPHASE = 0, INCREASE ORDER BY ONE AND MULTIPLY STEPSIZE BY
 *     FACTOR TWO
 */
L_545:
	*k = kp1;
	hnew = *h*2.0e0;
	*h = hnew;
	goto L_575;


/*     DETERMINE THE APPROPRIATE STEPSIZE FOR
 *     THE NEXT STEP.
 */
L_550:
	hnew = *h;
	temp2 = *k + 1;
	r = pow(2.0e0*est + 0.0001e0, -1.0e0/temp2);
	if( r < 2.0e0 )
		goto L_555;
	hnew = 2.0e0**h;
	goto L_560;
L_555:
	if( r > 1.0e0 )
		goto L_560;
	r = fmax( 0.5e0, fmin( 0.9e0, r ) );
	hnew = *h*r;
L_560:
	*h = hnew;


/*     UPDATE DIFFERENCES FOR NEXT STEP
 */
L_575:
	;
	if( *kold == iwm[dda001.lmxord - 1] )
		goto L_585;
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		PHI(kp2 - 1,i_) = e[i_];
		}
L_585:
	;
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		PHI(kp1 - 1,i_) = PHI(kp1 - 1,i_) + e[i_];
		}
	for( j1 = 2; j1 <= kp1; j1++ ){
		j = kp1 - j1 + 1;
		for( i = 1; i <= *neq; i++ ){
			i_ = i - 1;
			PHI(j - 1,i_) = PHI(j - 1,i_) + PHI(j,i_);
			}
		}
	return;

	/*-----------------------------------------------------------------------
 *     BLOCK 6
 *     THE STEP IS UNSUCCESSFUL. RESTORE X,PSI,PHI
 *     DETERMINE APPROPRIATE STEPSIZE FOR
 *     CONTINUING THE INTEGRATION, OR EXIT WITH
 *     AN ERROR FLAG IF THERE HAVE BEEN MANY
 *     FAILURES.
 *-----------------------------------------------------------------------
 */
L_600:
	*iphase = 1;

/*     RESTORE X,PHI,PSI
 */
	*x = xold;
	if( kp1 < nsp1 )
		goto L_630;
	for( j = nsp1; j <= kp1; j++ ){
		j_ = j - 1;
		temp1 = 1.0e0/beta[j_];
		for( i = 1; i <= *neq; i++ ){
			i_ = i - 1;
			PHI(j_,i_) = temp1*PHI(j_,i_);
			}
		}
L_630:
	;
	for( i = 2; i <= kp1; i++ ){
		i_ = i - 1;
		psi[i_ - 1] = psi[i_] - *h;
		}


/*     TEST WHETHER FAILURE IS DUE TO CORRECTOR ITERATION
 *     OR ERROR TEST
 */
	if( convgd == TRUE )
		goto L_660;
	iwm[dda001.lctf - 1] = iwm[dda001.lctf - 1] + 1;


/*     THE NEWTON ITERATION FAILED TO CONVERGE WITH
 *     A CURRENT ITERATION MATRIX.  DETERMINE THE CAUSE
 *     OF THE FAILURE AND TAKE APPROPRIATE ACTION.
 */
	if( ier == 0 )
		goto L_650;

/*     THE ITERATION MATRIX IS SINGULAR. REDUCE
 *     THE STEPSIZE BY A FACTOR OF 4. IF
 *     THIS HAPPENS THREE TIMES IN A ROW ON
 *     THE SAME STEP, RETURN WITH AN ERROR FLAG
 */
	nsf = nsf + 1;
	r = 0.25e0;
	*h = *h*r;
	if( nsf < 3 && fabs( *h ) >= *hmin )
		goto L_690;
	*idid = -8;
	goto L_675;


/*     THE NEWTON ITERATION FAILED TO CONVERGE FOR A REASON
 *     OTHER THAN A SINGULAR ITERATION MATRIX.  IF IRES = -2, THEN
 *     RETURN.  OTHERWISE, REDUCE THE STEPSIZE AND TRY AGAIN, UNLESS
 *     TOO MANY FAILURES HAVE OCCURED.
 */
L_650:
	;
	if( ires > -2 )
		goto L_655;
	*idid = -11;
	goto L_675;
L_655:
	ncf = ncf + 1;
	r = 0.25e0;
	*h = *h*r;
	if( ncf < 10 && fabs( *h ) >= *hmin )
		goto L_690;
	*idid = -7;
	if( ires < 0 )
		*idid = -10;
	if( nef >= 3 )
		*idid = -9;
	goto L_675;


/*     THE NEWTON SCHEME CONVERGED,AND THE CAUSE
 *     OF THE FAILURE WAS THE ERROR ESTIMATE
 *     EXCEEDING THE TOLERANCE.
 */
L_660:
	nef = nef + 1;
	iwm[dda001.letf - 1] = iwm[dda001.letf - 1] + 1;
	if( nef > 1 )
		goto L_665;

/*     ON FIRST ERROR TEST FAILURE, KEEP CURRENT ORDER OR LOWER
 *     ORDER BY ONE.  COMPUTE NEW STEPSIZE BASED ON DIFFERENCES
 *     OF THE SOLUTION.
 */
	*k = knew;
	temp2 = *k + 1;
	r = 0.90e0*pow(2.0e0*est + 0.0001e0, -1.0e0/temp2);
	r = fmax( 0.25e0, fmin( 0.9e0, r ) );
	*h = *h*r;
	if( fabs( *h ) >= *hmin )
		goto L_690;
	*idid = -6;
	goto L_675;

/*     ON SECOND ERROR TEST FAILURE, USE THE CURRENT ORDER OR
 *     DECREASE ORDER BY ONE.  REDUCE THE STEPSIZE BY A FACTOR OF
 *     ONE QUARTER.
 */
L_665:
	if( nef > 2 )
		goto L_670;
	*k = knew;
	*h = 0.25e0**h;
	if( fabs( *h ) >= *hmin )
		goto L_690;
	*idid = -6;
	goto L_675;

/*     ON THIRD AND SUBSEQUENT ERROR TEST FAILURES, SET THE ORDER TO
 *     ONE AND REDUCE THE STEPSIZE BY A FACTOR OF ONE QUARTER
 */
L_670:
	*k = 1;
	*h = 0.25e0**h;
	if( fabs( *h ) >= *hmin )
		goto L_690;
	*idid = -6;
	goto L_675;


/*     FOR ALL CRASHES, RESTORE Y TO ITS LAST VALUE,
 *     INTERPOLATE TO FIND YPRIME AT LAST X, AND RETURN
 */
L_675:
	;
	ddatrp( x, x, y, yprime, neq, k, phi, psi );
	return;


/*     GO BACK AND TRY THIS STEP AGAIN
 */
L_690:
	goto L_200;

/*------END OF SUBROUTINE DASTEP------
 */
#undef  PHI
}

//////////////////////////////////////////////////////////////////////////////

void ddaslv(INTEGER* neq, REAL delta[], REAL wm[], INTEGER iwm[])

/****BEGIN PROLOGUE  DDASLV
 ****REFER TO  DASSL
 ****ROUTINES CALLED DGESL,DGBSL
 ****COMMON BLOCKS    DDA001
 ****DATE WRITTEN   830315   (YYMMDD)
 ****REVISION DATE  830315   (YYMMDD)
 ****END PROLOGUE  DDASLV
 *-----------------------------------------------------------------------
 *     THIS ROUTINE MANAGES THE SOLUTION OF THE LINEAR
 *     SYSTEM ARISING IN THE NEWTON ITERATION.
 *     MATRICES AND REAL TEMPORARY STORAGE AND
 *     REAL INFORMATION ARE STORED IN THE ARRAY WM.
 *     INTEGER MATRIX INFORMATION IS STORED IN
 *     THE ARRAY IWM.
 *     FOR A DENSE MATRIX, THE LINPACK ROUTINE
 *     DGESL IS CALLED.
 *     FOR A BANDED MATRIX,THE LINPACK ROUTINE
 *     DGBSL IS CALLED.
 *-----------------------------------------------------------------------
 */

{
static INTEGER meband, mtype;


	mtype = iwm[dda001.lmtype - 1];
	switch( mtype ){
		case 1: goto L_100;
		case 2: goto L_100;
		case 3: goto L_300;
		case 4: goto L_400;
		case 5: goto L_400;
		}

/*     DENSE MATRIX
 */
L_100:
	dgesl( &wm[dda001.npd - 1], *neq, *neq, &iwm[dda001.lipvt - 1], 
	 delta, 0 );
	return;

/*     DUMMY SECTION FOR MTYPE=3
 */
L_300:
	;
	return;

/*     BANDED MATRIX
 */
L_400:
	meband = 2*iwm[dda001.lml - 1] + iwm[dda001.lmu - 1] + 1;
	dgbsl( &wm[dda001.npd - 1], meband, *neq, iwm[dda001.lml - 1], 
	 iwm[dda001.lmu - 1], &iwm[dda001.lipvt - 1], delta, 0 );
	return;

/*------END OF SUBROUTINE DDASLV------
 */
}

//////////////////////////////////////////////////////////////////////////////

void ddajac(INTEGER* neq, REAL* x, REAL y[], REAL yprime[], REAL delta[],
	    REAL* cj, REAL* h, INTEGER* ier, REAL wt[], REAL e[], REAL wm[],
	    INTEGER iwm[],
	    void (*res)(REAL* t, REAL y[], REAL yprime[], REAL delta[],
			INTEGER* ires, REAL rpar[], INTEGER ipar[]),
	    INTEGER* ires, REAL* uround,
	    void (*jac)(REAL* t, REAL y[], REAL yprime[], REAL pd[],
			REAL* cj, REAL rpar[], INTEGER ipar[]),
	    REAL rpar[], INTEGER ipar[])

/****BEGIN PROLOGUE  DDAJAC
 ****REFER TO  DASSL
 ****ROUTINES CALLED  DGEFA,DGBFA
 ****COMMON BLOCKS    DDA001
 ****DATE WRITTEN   830315   (YYMMDD)
 ****REVISION DATE  830315   (YYMMDD)
 ****END PROLOGUE  DDAJAC
 *-----------------------------------------------------------------------
 *     THIS ROUTINE COMPUTES THE ITERATION MATRIX
 *     PD=DG/DY+CJ*DG/DYPRIME (WHERE G(X,Y,YPRIME)=0).
 *     HERE PD IS COMPUTED BY THE USER-SUPPLIED
 *     ROUTINE JAC IF IWM(MTYPE) IS 1 OR 4, AND
 *     IT IS COMPUTED BY NUMERICAL FINITE DIFFERENCING
 *     IF IWM(MTYPE)IS 2 OR 5
 *     THE PARAMETERS HAVE THE FOLLOWING MEANINGS.
 *     Y        = ARRAY CONTAINING PREDICTED VALUES
 *     YPRIME   = ARRAY CONTAINING PREDICTED DERIVATIVES
 *     DELTA    = RESIDUAL EVALUATED AT (X,Y,YPRIME)
 *                (USED ONLY IF IWM(MTYPE)=2 OR 5)
 *     CJ       = SCALAR PARAMETER DEFINING ITERATION MATRIX
 *     H        = CURRENT STEPSIZE IN INTEGRATION
 *     IER      = VARIABLE WHICH IS .NE. 0
 *                IF ITERATION MATRIX IS SINGULAR,
 *                AND 0 OTHERWISE.
 *     WT       = VECTOR OF WEIGHTS FOR COMPUTING NORMS
 *     E        = WORK SPACE (TEMPORARY) OF LENGTH NEQ
 *     WM       = REAL WORK SPACE FOR MATRICES. ON
 *                OUTPUT IT CONTAINS THE LU DECOMPOSITION
 *                OF THE ITERATION MATRIX.
 *     IWM      = INTEGER WORK SPACE CONTAINING
 *                MATRIX INFORMATION
 *     RES      = NAME OF THE EXTERNAL USER-SUPPLIED ROUTINE
 *                TO EVALUATE THE RESIDUAL FUNCTION G(X,Y,YPRIME)
 *     IRES     = FLAG WHICH IS EQUAL TO ZERO IF NO ILLEGAL VALUES
 *                IN RES, AND LESS THAN ZERO OTHERWISE.  (IF IRES
 *                IS LESS THAN ZERO, THE MATRIX WAS NOT COMPLETED)
 *                IN THIS CASE (IF IRES .LT. 0), THEN IER = 0.
 *     UROUND   = THE UNIT ROUNDOFF ERROR OF THE MACHINE BEING USED.
 *     JAC      = NAME OF THE EXTERNAL USER-SUPPLIED ROUTINE
 *                TO EVALUATE THE ITERATION MATRIX (THIS ROUTINE
 *                IS ONLY USED IF IWM(MTYPE) IS 1 OR 4)
 *-----------------------------------------------------------------------
 */

{
static INTEGER _d_l, _d_m, _do0, _do1, _do2, _do3, i, i1, i2, i_, ii, ipsave, 
	 isave, j, k, l, l_, lenpd, mba, mband, meb1, meband, msave, 
	 mtype, n, n_, npdm1, nrow;
static REAL del, delinv, squr, ypsave, ysave;


	*ier = 0;
	npdm1 = dda001.npd - 1;
	mtype = iwm[dda001.lmtype - 1];
	switch( mtype ){
		case 1: goto L_100;
		case 2: goto L_200;
		case 3: goto L_300;
		case 4: goto L_400;
		case 5: goto L_500;
		}


/*     DENSE USER-SUPPLIED MATRIX
 */
L_100:
	lenpd = *neq**neq;
	for( i = 1; i <= lenpd; i++ ){
		i_ = i - 1;
		wm[npdm1 + i_] = 0.0e0;
		}
	(*jac)( x, y, yprime, &wm[dda001.npd - 1], cj, rpar, ipar );
	goto L_230;


/*     DENSE FINITE-DIFFERENCE-GENERATED MATRIX
 */
L_200:
	*ires = 0;
	nrow = npdm1;
	squr = sqrt( *uround );
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		del = squr*fmax( fabs( y[i_] ), fabs( *h*yprime[i_] ),
				  fabs( wt[i_] ) );
		del = fsign( del, *h*yprime[i_] );
		del = (y[i_] + del) - y[i_];
		ysave = y[i_];
		ypsave = yprime[i_];
		y[i_] = y[i_] + del;
		yprime[i_] = yprime[i_] + *cj*del;
		(*res)( x, y, yprime, e, ires, rpar, ipar );
		if( *ires < 0 )
			return;
		delinv = 1.0e0/del;
		for( l = 1; l <= *neq; l++ ){
			l_ = l - 1;
			wm[nrow + l_] = (e[l_] - delta[l_])*delinv;
			}
		nrow = nrow + *neq;
		y[i_] = ysave;
		yprime[i_] = ypsave;
		}


/*     DO DENSE-MATRIX LU DECOMPOSITION ON PD
 */
L_230:
	dgefa( &wm[dda001.npd - 1], *neq, *neq, &iwm[dda001.lipvt - 1], 
	 ier );
	return;


/*     DUMMY SECTION FOR IWM(MTYPE)=3
 */
L_300:
	return;


/*     BANDED USER-SUPPLIED MATRIX
 */
L_400:
	lenpd = (2*iwm[dda001.lml - 1] + iwm[dda001.lmu - 1] + 1)*
	 *neq;
	for( i = 1; i <= lenpd; i++ ){
		i_ = i - 1;
		wm[npdm1 + i_] = 0.0e0;
		}
	(*jac)( x, y, yprime, &wm[dda001.npd - 1], cj, rpar, ipar );
	meband = 2*iwm[dda001.lml - 1] + iwm[dda001.lmu - 1] + 1;
	goto L_550;


/*     BANDED FINITE-DIFFERENCE-GENERATED MATRIX
 */
L_500:
	mband = iwm[dda001.lml - 1] + iwm[dda001.lmu - 1] + 1;
	mba = fmin( mband, *neq );
	meband = mband + iwm[dda001.lml - 1];
	meb1 = meband - 1;
	msave = (*neq/mband) + 1;
	isave = dda001.ntemp - 1;
	ipsave = isave + msave;
	*ires = 0;
	squr = sqrt( *uround );
	for( j = 1; j <= mba; j++ ){
		for( n = j, _do0=DOCNT(n,*neq,_do1 = mband); _do0 > 0; n += _do1, _do0-- ){
			n_ = n - 1;
			k = (n - j)/mband + 1;
			wm[isave + k - 1] = y[n_];
			wm[ipsave + k - 1] = yprime[n_];
			del = squr*fmax( fabs( y[n_] ), fabs( *h*yprime[n_] ), 
			 fabs( wt[n_] ) );
			del = fsign( del, *h*yprime[n_] );
			del = (y[n_] + del) - y[n_];
			y[n_] = y[n_] + del;
			yprime[n_] = yprime[n_] + *cj*del;
			}
		(*res)( x, y, yprime, e, ires, rpar, ipar );
		if( *ires < 0 )
			return;
		for( n = j, _do2=DOCNT(n,*neq,_do3 = mband); _do2 > 0; n += _do3, _do2-- ){
			n_ = n - 1;
			k = (n - j)/mband + 1;
			y[n_] = wm[isave + k - 1];
			yprime[n_] = wm[ipsave + k - 1];
			del = squr*fmax( fabs( y[n_] ), fabs( *h*yprime[n_] ), 
			 fabs( wt[n_] ) );
			del = fsign( del, *h*yprime[n_] );
			del = (y[n_] + del) - y[n_];
			delinv = 1.0e0/del;
			i1 = fmax( 1, n - iwm[dda001.lmu - 1] );
			i2 = fmin( *neq, n + iwm[dda001.lml - 1] );
			ii = n*meb1 - iwm[dda001.lml - 1] + npdm1;
			for( i = i1; i <= i2; i++ ){
				i_ = i - 1;
				wm[ii + i_] = (e[i_] - delta[i_])*delinv;
				}
			}
		}


/*     DO LU DECOMPOSITION OF BANDED PD
 */
L_550:
	dgbfa( &wm[dda001.npd - 1], meband, *neq, iwm[dda001.lml - 1], 
	 iwm[dda001.lmu - 1], &iwm[dda001.lipvt - 1], ier );
	return;

/*------END OF SUBROUTINE DDAJAC------
 */
}

//////////////////////////////////////////////////////////////////////////////

void ddatrp(REAL* x, REAL* xout, REAL yout[], REAL ypout[], INTEGER* neq,
	    INTEGER* kold, REAL* phi, REAL psi[])

/****BEGIN PROLOGUE  DDATRP
 ****REFER TO  DASSL
 ****ROUTINES CALLED  (NONE)
 ****DATE WRITTEN   830315   (YYMMDD)
 ****REVISION DATE  830315   (YYMMDD)
 ****END PROLOGUE  DDATRP
 *
 *-----------------------------------------------------------------------
 *     THE METHODS IN SUBROUTINE DASTEP USE POLYNOMIALS
 *     TO APPROXIMATE THE SOLUTION. DDATRP APPROXIMATES THE
 *     SOLUTION AND ITS DERIVATIVE AT TIME XOUT BY EVALUATING
 *     ONE OF THESE POLYNOMIALS,AND ITS DERIVATIVE,THERE.
 *     INFORMATION DEFINING THIS POLYNOMIAL IS PASSED FROM
 *     DASTEP, SO DDATRP CANNOT BE USED ALONE.
 *
 *     THE PARAMETERS ARE
 *     X     THE CURRENT TIME IN THE INTEGRATION.
 *     XOUT  THE TIME AT WHICH THE SOLUTION IS DESIRED
 *     YOUT  THE INTERPOLATED APPROXIMATION TO Y AT XOUT
 *           (THIS IS OUTPUT)
 *     YPOUT THE INTERPOLATED APPROXIMATION TO YPRIME AT XOUT
 *           (THIS IS OUTPUT)
 *     NEQ   NUMBER OF EQUATIONS
 *     KOLD  ORDER USED ON LAST SUCCESSFUL STEP
 *     PHI   ARRAY OF SCALED DIVIDED DIFFERENCES OF Y
 *     PSI   ARRAY OF PAST STEPSIZE HISTORY
 *-----------------------------------------------------------------------
 */

{
#define PHI(I_,J_)      (*(phi+(I_)*(*neq)+(J_)))
static INTEGER i, i_, j, j_, koldp1;
static REAL c, d, gamma, temp1;


	koldp1 = *kold + 1;
	temp1 = *xout - *x;
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		yout[i_] = PHI(0,i_);
		ypout[i_] = 0.0e0;
		}
	c = 1.0e0;
	d = 0.0e0;
	gamma = temp1/psi[0];
	for( j = 2; j <= koldp1; j++ ){
		j_ = j - 1;
		d = d*gamma + c/psi[j_ - 1];
		c = c*gamma;
		gamma = (temp1 + psi[j_ - 1])/psi[j_];
		for( i = 1; i <= *neq; i++ ){
			i_ = i - 1;
			yout[i_] = yout[i_] + c*PHI(j_,i_);
			ypout[i_] = ypout[i_] + d*PHI(j_,i_);
			}
		}
	return;

/*------END OF SUBROUTINE DDATRP------
 */
#undef  PHI
}

//////////////////////////////////////////////////////////////////////////////

void ddaini(REAL* x, REAL y[], REAL yprime[], INTEGER* neq,
	    void (*res)(REAL* t, REAL y[], REAL yprime[], REAL delta[],
			INTEGER* ires, REAL rpar[], INTEGER ipar[]),
	    void (*jac)(REAL* t, REAL y[], REAL yprime[], REAL pd[],
			REAL* cj, REAL rpar[], INTEGER ipar[]),
	    REAL* h, REAL wt[], INTEGER* idid, REAL rpar[],
	    INTEGER ipar[], REAL* phi, REAL delta[], REAL e[], REAL wm[],
	    INTEGER iwm[], REAL* hmin, REAL* uround, INTEGER* nonneg)

/****BEGIN PROLOGUE  DDAINI
 ****REFER TO  DASSL
 ****ROUTINES CALLED  DDANRM,DDAJAC,DDASLV
 ****COMMON BLOCKS    DDA001
 ****DATE WRITTEN   830315   (YYMMDD)
 ****REVISION DATE  830315   (YYMMDD)
 ****END PROLOGUE DDAINI
 *
 *-------------------------------------------------------
 *     DDAINI TAKES ONE STEP OF SIZE H OR SMALLER
 *     WITH THE BACKWARD EULER METHOD, TO
 *     FIND YPRIME AT THE INITIAL TIME X. A MODIFIED
 *     DAMPED NEWTON ITERATION IS USED TO
 *     SOLVE THE CORRECTOR ITERATION.
 *
 *     THE INITIAL GUESS YPRIME IS USED IN THE
 *     PREDICTION, AND IN FORMING THE ITERATION
 *     MATRIX, BUT IS NOT INVOLVED IN THE
 *     ERROR TEST. THIS MAY HAVE TROUBLE
 *     CONVERGING IF THE INITIAL GUESS IS NO
 *     GOOD, OR IF G(XY,YPRIME) DEPENDS
 *     NONLINEARLY ON YPRIME.
 *
 *     THE PARAMETERS REPRESENT:
 *     X --         INDEPENDENT VARIABLE
 *     Y --         SOLUTION VECTOR AT X
 *     YPRIME --    DERIVATIVE OF SOLUTION VECTOR
 *     NEQ --       NUMBER OF EQUATIONS
 *     H --         STEPSIZE. IMDER MAY USE A STEPSIZE
 *                  SMALLER THAN H.
 *     WT --        VECTOR OF WEIGHTS FOR ERROR
 *                  CRITERION
 *     IDID --      COMPLETION CODE WITH THE FOLLOWING MEANINGS
 *                  IDID= 1 -- YPRIME WAS FOUND SUCCESSFULLY
 *                  IDID=-12 -- DDAINI FAILED TO FIND YPRIME
 *     RPAR,IPAR -- REAL AND INTEGER PARAMETER ARRAYS
 *                  THAT ARE NOT ALTERED BY DDAINI
 *     PHI --       WORK SPACE FOR DDAINI
 *     DELTA,E --   WORK SPACE FOR DDAINI
 *     WM,IWM --    REAL AND INTEGER ARRAYS STORING
 *                  MATRIX INFORMATION
 *
 *-----------------------------------------------------------------
 */

{
#define PHI(I_,J_)      (*(phi+(I_)*(*neq)+(J_)))
static BOOLEAN convgd;
static INTEGER i, i_, ier, ires, jcalc, m, ncf, nef, nsf;
static REAL cj, delnrm, err, oldnrm, r, rate, s, xnew, 
	 ynorm;
static INTEGER maxit = 10;
static INTEGER mjac = 5;
static REAL damp = 0.75e0;


/*---------------------------------------------------
 *     BLOCK 1.
 *     INITIALIZATIONS.
 *---------------------------------------------------
 */
	*idid = 1;
	nef = 0;
	ncf = 0;
	nsf = 0;
	ynorm = ddanrm( neq, y, wt );

/*     SAVE Y AND YPRIME IN PHI
 */
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		PHI(0,i_) = y[i_];
		PHI(1,i_) = yprime[i_];
		}


/*----------------------------------------------------
 *     BLOCK 2.
 *     DO ONE BACKWARD EULER STEP.
 *----------------------------------------------------
 *
 *     SET UP FOR START OF CORRECTOR ITERATION
 */
L_200:
	cj = 1.0e0/ *h;
	xnew = *x + *h;

/*     PREDICT SOLUTION AND DERIVATIVE
 */
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		y[i_] = y[i_] + *h*yprime[i_];
		}

	jcalc = -1;
	m = 0;
	convgd = TRUE;


/*     CORRECTOR LOOP.
 */
L_300:
	iwm[dda001.lnre - 1] = iwm[dda001.lnre - 1] + 1;
	ires = 0;

	(*res)( &xnew, y, yprime, delta, &ires, rpar, ipar );
	if( ires < 0 )
		goto L_430;


/*     EVALUATE THE ITERATION MATRIX
 */
	if( jcalc != -1 )
		goto L_310;
	iwm[dda001.lnje - 1] = iwm[dda001.lnje - 1] + 1;
	jcalc = 0;
	ddajac( neq, &xnew, y, yprime, delta, &cj, h, &ier, wt, e, wm, 
	 iwm, res, &ires, uround, jac, rpar, ipar );

	s = 1000000.e0;
	if( ires < 0 )
		goto L_430;
	if( ier != 0 )
		goto L_430;
	nsf = 0;


/*     MULTIPLY RESIDUAL BY DAMPING FACTOR
 */
L_310:
	;
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		delta[i_] = delta[i_]*damp;
		}


/*     COMPUTE A NEW ITERATE (BACK SUBSTITUTION)
 *     STORE THE CORRECTION IN DELTA
 */

	ddaslv( neq, delta, wm, iwm );


/*     UPDATE Y AND YPRIME
 */
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		y[i_] = y[i_] - delta[i_];
		yprime[i_] = yprime[i_] - cj*delta[i_];
		}


/*     TEST FOR CONVERGENCE OF THE ITERATION.
 */
	delnrm = ddanrm( neq, delta, wt );
	if( delnrm <= 100.e0**uround*ynorm )
		goto L_400;

	if( m > 0 )
		goto L_340;
	oldnrm = delnrm;
	goto L_350;

L_340:
	rate = pow(delnrm/oldnrm, 1.0e0/(REAL)( m ));
	if( rate > 0.90e0 )
		goto L_430;
	s = rate/(1.0e0 - rate);

L_350:
	if( s*delnrm <= 0.33e0 )
		goto L_400;


/*     THE CORRECTOR HAS NOT YET CONVERGED. UPDATE
 *     M AND AND TEST WHETHER THE MAXIMUM
 *     NUMBER OF ITERATIONS HAVE BEEN TRIED.
 *     EVERY MJAC ITERATIONS, GET A NEW
 *     ITERATION MATRIX.
 */

	m = m + 1;
	if( m >= maxit )
		goto L_430;

	if( (m/mjac)*mjac == m )
		jcalc = -1;

	goto L_300;


/*     THE ITERATION HAS CONVERGED.
 *     CHECK NONNEGATIVITY CONSTRAINTS
 */
L_400:
	if( *nonneg == 0 )
		goto L_450;
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		delta[i_] = fmin( y[i_], 0.0e0 );
		}

	delnrm = ddanrm( neq, delta, wt );
	if( delnrm > 0.33e0 )
		goto L_430;

	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		y[i_] = y[i_] - delta[i_];
		yprime[i_] = yprime[i_] - cj*delta[i_];
		}
	goto L_450;


/*     EXITS FROM CORRECTOR LOOP.
 */
L_430:
	convgd = FALSE;
L_450:
	if( convgd == FALSE )
		goto L_600;



/*-----------------------------------------------------
 *     BLOCK 3.
 *     THE CORRECTOR ITERATION CONVERGED.
 *     DO ERROR TEST.
 *-----------------------------------------------------
 */

	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		e[i_] = y[i_] - PHI(0,i_);
		}

	err = ddanrm( neq, e, wt );

	if( err <= 1.0e0 )
		return;


/*--------------------------------------------------------
 *     BLOCK 4.
 *     THE BACKWARD EULER STEP FAILED. RESTORE Y
 *     AND YPRIME TO THEIR ORIGINAL VALUES.
 *     REDUCE STEPSIZE AND TRY AGAIN, IF
 *     POSSIBLE.
 *---------------------------------------------------------
 */

L_600:
	;
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		y[i_] = PHI(0,i_);
		yprime[i_] = PHI(1,i_);
		}

	if( convgd == TRUE )
		goto L_640;
	if( ier == 0 )
		goto L_620;
	nsf = nsf + 1;
	*h = *h*0.25e0;
	if( nsf < 3 && fabs( *h ) >= *hmin )
		goto L_690;
	*idid = -12;
	return;
L_620:
	if( ires > -2 )
		goto L_630;
	*idid = -12;
	return;
L_630:
	ncf = ncf + 1;
	*h = *h*0.25e0;
	if( ncf < 10 && fabs( *h ) >= *hmin )
		goto L_690;
	*idid = -12;
	return;

L_640:
	nef = nef + 1;
	r = 0.90e0/(2.0e0*err + 0.0001e0);
	r = fmax( 0.1e0, fmin( 0.5e0, r ) );
	*h = *h*r;
	if( fabs( *h ) >= *hmin && nef < 10 )
		goto L_690;
	*idid = -12;
	return;
L_690:
	goto L_200;

/*-------------END OF SUBROUTINE DDAINI----------------------
 */
#undef  PHI
}

//////////////////////////////////////////////////////////////////////////////

REAL ddanrm(INTEGER* neq, REAL v[], REAL wt[])

/****BEGIN PROLOGUE  DDANRM
 ****REFER TO  DASSL
 ****ROUTINES CALLED  (NONE)
 ****DATE WRITTEN   830315   (YYMMDD)
 ****REVISION DATE  830315   (YYMMDD)
 ****END PROLOGUE  DDANRM
 *-----------------------------------------------------------------------
 *     THIS FUNCTION ROUTINE COMPUTES THE WEIGHTED
 *     ROOT-MEAN-SQUARE NORM OF THE VECTOR OF LENGTH
 *     NEQ CONTAINED IN THE ARRAY V,WITH WEIGHTS
 *     CONTAINED IN THE ARRAY WT OF LENGTH NEQ.
 *        DDANRM=SQRT((1/NEQ)*SUM(V(I)/WT(I))**2)
 *-----------------------------------------------------------------------
 */

{
static INTEGER i, i_;
static REAL ddanrm_v, sum, vmax;


	ddanrm_v = 0.0e0;
	vmax = 0.0e0;
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		if( fabs( v[i_]/wt[i_] ) > vmax )
			vmax = fabs( v[i_]/wt[i_] );
		}
	if( vmax <= 0.0e0 )
		goto L_30;
	sum = 0.0e0;
	for( i = 1; i <= *neq; i++ ){
		i_ = i - 1;
		sum = sum + ((v[i_]/wt[i_])/vmax)*((v[i_]/wt[i_])/vmax);
		}
	ddanrm_v = vmax*sqrt( sum/(REAL)( *neq ) );
L_30:
	;
	return( ddanrm_v );

/*------END OF FUNCTION DDANRM------
 */
}


//////////////////////////////////////////////////////////////////////////////

REAL d1mach()

/*-----------------------------------------------------------------------
 * THIS ROUTINE COMPUTES THE UNIT ROUNDOFF OF THE MACHINE IN DOUBLE
 * PRECISION.  THIS IS DEFINED AS THE SMALLEST POSITIVE MACHINE NUMBER
 * U SUCH THAT  1.0E0 + U .NE. 1.0E0 (IN SINGLE PRECISION).
 *-----------------------------------------------------------------------
 */

{
REAL r, d1mach_v, u, unity;

    unity = 1.0;
	u = 1.0;
L_10:
	u = u*0.5;
	r = unity + u;
	if( r != unity )
		goto L_10;
	d1mach_v = u*2.0;
	return( d1mach_v );

/*----------------------- END OF FUNCTION D1MACH ------------------------
 */
}

//////////////////////////////////////////////////////////////////////////////

