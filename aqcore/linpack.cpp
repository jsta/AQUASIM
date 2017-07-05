//////////////////////////////   linpack.cpp   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    18.11.91    Peter Reichert
// revisions:   15.09.99    Peter Reichert    dgeco, dgedi, ,dgbco, dgbdi,
//                                            dpoco, dpofa, dpodi, dasum,
//                                            dswap added
// revisions:   31.12.99    Peter Reichert    dcgdc added
//
//////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "fortran.h"
#include "linpack.h"

//////////////////////////////////////////////////////////////////////////////

// forward declarations of auxiliary routines:

REAL dasum(INTEGER n, REAL dx[], INTEGER incx);

void daxpy(INTEGER n, REAL* da, REAL dx[], INTEGER incx, REAL dy[],
           INTEGER incy);

void dscal(INTEGER n, REAL* da, REAL dx[], INTEGER incx);

REAL ddot(INTEGER n, REAL dx[], INTEGER incx, REAL dy[], INTEGER incy);

INTEGER idamax(INTEGER n, REAL dx[], INTEGER incx);

void dswap(INTEGER n, REAL dx[], INTEGER incx, REAL dy[], INTEGER incy);

//////////////////////////////////////////////////////////////////////////////

void dgeco(REAL* a, INTEGER lda, INTEGER n, INTEGER ipvt[], 
	 REAL* rcond, REAL z[])
{
#define A(I_,J_)	(*(a+(I_)*(lda)+(J_)))
INTEGER info, j, j_, k, k_, kb, kb_, kp1, l;
REAL anorm, ek, s, sm, t, wk, wkm, ynorm;

	//     compute 1-norm of a
	
	anorm = 0.0e0;
	for( j = 1; j <= n; j++ ) { 
		j_ = j - 1;
		anorm = fmax( anorm, dasum( n, &A(j_,0), 1 ) );
	}
	
	//     factor
	
	dgefa( a, lda, n, ipvt, &info );
	
	//     rcond = 1/(norm(a)*(estimate of norm(inverse(a)))) .
	//     estimate = norm(z)/norm(y) where  a*z = y  and  trans(a)*y = e .
	//     trans(a)  is the transpose of a .  the components of  e  are
	//     chosen to cause maximum local growth in the elements of w  where
	//     trans(u)*w = e .  the vectors are frequently rescaled to avoid
	//     overflow.
	
	//     solve trans(u)*w = e
	
	ek = 1.0e0;
	for( j = 1; j <= n; j++ ) { 
		j_ = j - 1;
		z[j_] = 0.0e0;
	}
	for( k = 1; k <= n; k++ ) { 
		k_ = k - 1;
		if( z[k_] != 0.0e0 ) 
			ek = fsign( ek, -z[k_] );
		if( fabs( ek - z[k_] ) <= fabs( A(k_,k_) ) ) 
			goto L_30;
		s = fabs( A(k_,k_) )/fabs( ek - z[k_] );
		dscal( n, &s, z, 1 );
		ek = s*ek;
L_30:
		;
		wk = ek - z[k_];
		wkm = -ek - z[k_];
		s = fabs( wk );
		sm = fabs( wkm );
		if( A(k_,k_) == 0.0e0 ) 
			goto L_40;
		wk = wk/A(k_,k_);
		wkm = wkm/A(k_,k_);
		goto L_50;
L_40:
		wk = 1.0e0;
		wkm = 1.0e0;
L_50:
		kp1 = k + 1;
		if( kp1 > n ) 
			goto L_90;
		for( j = kp1; j <= n; j++ ) {
			j_ = j - 1; 
			sm = sm + fabs( z[j_] + wkm*A(j_,k_) );
			z[j_] = z[j_] + wk*A(j_,k_);
			s = s + fabs( z[j_] );
		}
		if( s >= sm ) 
			goto L_80;
		t = wkm - wk;
		wk = wkm;
		for( j = kp1; j <= n; j++ ) {
			j_ = j - 1; 
			z[j_] = z[j_] + t*A(j_,k_);
		}
L_80:
		;
L_90:
		z[k_] = wk;
	}
	s = 1.0e0/dasum( n, z, 1 );
	dscal( n, &s, z, 1 );
	
	//     solve trans(l)*y = w
	
	for( kb = 1; kb <= n; kb++ ) {
		kb_ = kb - 1; 
		k = n + 1 - kb;
		if( k < n ) 
			z[k - 1] = z[k - 1] + ddot( n - k, &A(k - 1,k), 1, 
			 &z[k], 1 );
		if( fabs( z[k - 1] ) <= 1.0e0 ) 
			goto L_110;
		s = 1.0e0/fabs( z[k - 1] );
		dscal( n, &s, z, 1 );
L_110:
		l = ipvt[k - 1];
		t = z[l - 1];
		z[l - 1] = z[k - 1];
		z[k - 1] = t;
	}
	s = 1.0e0/dasum( n, z, 1 );
	dscal( n, &s, z, 1 );
	
	ynorm = 1.0e0;
	
	//     solve l*v = y
	
	for( k = 1; k <= n; k++ ) { 
		k_ = k - 1;
		l = ipvt[k_];
		t = z[l - 1];
		z[l - 1] = z[k_];
		z[k_] = t;
		if( k < n ) 
			daxpy( n - k, &t, &A(k_,k_ + 1), 1, &z[k_ + 1], 1 );
		if( fabs( z[k_] ) <= 1.0e0 ) 
			goto L_130;
		s = 1.0e0/fabs( z[k_] );
		dscal( n, &s, z, 1 );
		ynorm = s*ynorm;
L_130:
		;
	}
	s = 1.0e0/dasum( n, z, 1 );
	dscal( n, &s, z, 1 );
	ynorm = s*ynorm;
	
	//     solve  u*z = v
	
	for( kb = 1; kb <= n; kb++ ) { 
		k = n + 1 - kb;
		if( fabs( z[k - 1] ) <= fabs( A(k - 1,k - 1) ) ) 
			goto L_150;
		s = fabs( A(k - 1,k - 1) )/fabs( z[k - 1] );
		dscal( n, &s, z, 1 );
		ynorm = s*ynorm;
L_150:
		;
		if( A(k - 1,k - 1) != 0.0e0 ) 
			z[k - 1] = z[k - 1]/A(k - 1,k - 1);
		if( A(k - 1,k - 1) == 0.0e0 ) 
			z[k - 1] = 1.0e0;
		t = -z[k - 1];
		daxpy( k - 1, &t, &A(k - 1,0), 1, &z[0], 1 );
	}
	//     make znorm = 1.0
	s = 1.0e0/dasum( n, z, 1 );
	dscal( n, &s, z, 1 );
	ynorm = s*ynorm;
	
	if( anorm != 0.0e0 ) 
		*rcond = ynorm/anorm;
	if( anorm == 0.0e0 ) 
		*rcond = 0.0e0;
	return;
#undef	A
}

//////////////////////////////////////////////////////////////////////////////

void dgefa(REAL* a, INTEGER lda, INTEGER n, INTEGER ipvt[], INTEGER* info)
{
#define A(I_,J_)	(*(a+(I_)*(lda)+(J_)))
INTEGER j, j_, k, k_, kp1, l, nm1;
REAL t;


/*     GAUSSIAN ELIMINATION WITH PARTIAL PIVOTING
 */
	*info = 0;
	nm1 = n - 1;
	if( nm1 < 1 )
		goto L_70;
	for( k = 1; k <= nm1; k++ ){
		k_ = k - 1;
		kp1 = k + 1;

		/*        FIND L = PIVOT INDEX
		 * */
		l = idamax( n - k + 1, &A(k_,k_), 1 ) + 
		 k - 1;
		ipvt[k_] = l;

/*        ZERO PIVOT IMPLIES THIS COLUMN ALREADY TRIANGULARIZED
 */
		if( A(k_,l - 1) == 0.0e0 )
			goto L_40;

/*           INTERCHANGE IF NECESSARY
 */
		if( l == k )
			goto L_10;
		t = A(k_,l - 1);
		A(k_,l - 1) = A(k_,k_);
		A(k_,k_) = t;
L_10:
		;

/*           COMPUTE MULTIPLIERS
 */
		t = -1.0e0/A(k_,k_);
		dscal( n - k, &t, &A(k_,k_ + 1), 1 );

/*           ROW ELIMINATION WITH COLUMN INDEXING
 */
		for( j = kp1; j <= n; j++ ){
			j_ = j - 1;
			t = A(j_,l - 1);
			if( l == k )
				goto L_20;
			A(j_,l - 1) = A(j_,k_);
			A(j_,k_) = t;
L_20:
			;
			daxpy( n - k, &t, &A(k_,k_ + 1), 1, 
			 &A(j_,k_ + 1), 1 );
			}
		goto L_50;
L_40:
		;
		*info = k;
L_50:
		;
		}
L_70:
	;
	ipvt[n - 1] = n;
	if( A(n - 1,n - 1) == 0.0e0 )
		*info = n;
	return;
#undef	A
}

//////////////////////////////////////////////////////////////////////////////

void dgedi(REAL* a, INTEGER lda, INTEGER n, INTEGER ipvt[], 
           REAL det[], REAL work[], INTEGER job)
{
#define A(I_,J_)	(*(a+(I_)*(lda)+(J_)))
INTEGER i, i_, j, j_, k, k_, kb, kb_, kp1, l, nm1;
REAL t, ten;

	//     compute determinant
	
	if( job/10 == 0 ) 
		goto L_70;
	det[0] = 1.0e0;
	det[1] = 0.0e0;
	ten = 10.0e0;
	for( i = 1; i <= n; i++ ) {
		i_ = i - 1; 
		if( ipvt[i_] != i ) 
			det[0] = -det[0];
		det[0] = A(i_,i_)*det[0];
		//        ...exit
		if( det[0] == 0.0e0 ) 
			goto L_60;
L_10:
		if( fabs( det[0] ) >= 1.0e0 ) 
			goto L_20;
		det[0] = ten*det[0];
		det[1] = det[1] - 1.0e0;
		goto L_10;
L_20:
		;
L_30:
		if( fabs( det[0] ) < ten ) 
			goto L_40;
		det[0] = det[0]/ten;
		det[1] = det[1] + 1.0e0;
		goto L_30;
L_40:
		;
	}
L_60:
	;
L_70:
	//     compute inverse(u)
	
	if( fmod( job, 10 ) == 0 ) 
		goto L_150;
	for( k = 1; k <= n; k++ ) { 
		k_ = k - 1;
		A(k_,k_) = 1.0e0/A(k_,k_);
		t = -A(k_,k_);
		dscal( k - 1, &t, &A(k_,0), 1 );
		kp1 = k + 1;
		if( n < kp1 ) 
			goto L_90;
		for( j = kp1; j <= n; j++ ) {
			j_ = j - 1; 
			t = A(j_,k_);
			A(j_,k_) = 0.0e0;
			daxpy( k, &t, &A(k_,0), 1, &A(j_,0), 1 );
		}
L_90:
		;
	}
	
	//        form inverse(u)*inverse(l)
	
	nm1 = n - 1;
	if( nm1 < 1 ) 
		goto L_140;
	for( kb = 1; kb <= nm1; kb++ ) {
		kb_ = kb - 1; 
		k = n - kb;
		kp1 = k + 1;
		for( i = kp1; i <= n; i++ ) {
			i_ = i - 1; 
			work[i_] = A(k - 1,i_);
			A(k - 1,i_) = 0.0e0;
		}
		for( j = kp1; j <= n; j++ ) {
			j_ = j - 1; 
			t = work[j_];
			daxpy( n, &t, &A(j_,0), 1, &A(k - 1,0), 1 );
		}
		l = ipvt[k - 1];
		if( l != k ) 
			dswap( n, &A(k - 1,0), 1, &A(l - 1,0), 1 );
	}
L_140:
	;
L_150:
	;
	return;
#undef	A
} 

//////////////////////////////////////////////////////////////////////////////

void dgesl(REAL* a, INTEGER lda, INTEGER n, INTEGER ipvt[], REAL b[],
           INTEGER job)
{
#define A(I_,J_)	(*(a+(I_)*(lda)+(J_)))
INTEGER k, k_, kb, l, nm1;
REAL t;


	nm1 = n - 1;
	if( job != 0 )
		goto L_50;

/*        JOB = 0 , SOLVE  A * X = B
 *        FIRST SOLVE  L*Y = B
 */
	if( nm1 < 1 )
		goto L_30;
	for( k = 1; k <= nm1; k++ ){
		k_ = k - 1;
		l = ipvt[k_];
		t = b[l - 1];
		if( l == k )
			goto L_10;
		b[l - 1] = b[k_];
		b[k_] = t;
L_10:
		;
		daxpy( n - k, &t, &A(k_,k_ + 1), 1, &b[k_ + 1], 1 );
		}
L_30:
	;

/*        NOW SOLVE  U*X = Y
 */
	for( kb = 1; kb <= n; kb++ ){
		k = n + 1 - kb;
		b[k - 1] = b[k - 1]/A(k - 1,k - 1);
		t = -b[k - 1];
		daxpy( k - 1, &t, &A(k - 1,0), 1, &b[0], 1 );
		}
	goto L_100;
L_50:
	;

/*        JOB = NONZERO, SOLVE  TRANS(A) * X = B
 *        FIRST SOLVE  TRANS(U)*Y = B
 */
	for( k = 1; k <= n; k++ ){
		k_ = k - 1;
		t = ddot( k - 1, &A(k_,0), 1, &b[0], 1 );
		b[k_] = (b[k_] - t)/A(k_,k_);
		}

/*        NOW SOLVE TRANS(L)*X = Y
 */
	if( nm1 < 1 )
		goto L_90;
	for( kb = 1; kb <= nm1; kb++ ){
		k = n - kb;
		b[k - 1] = b[k - 1] + ddot( n - k, &A(k - 1,k), 1, &b[k], 1 );
		l = ipvt[k - 1];
		if( l == k )
			goto L_70;
		t = b[l - 1];
		b[l - 1] = b[k - 1];
		b[k - 1] = t;
L_70:
		;
		}
L_90:
	;
L_100:
	;
	return;
#undef	A
}

//////////////////////////////////////////////////////////////////////////////

void dgbco(REAL* abd, INTEGER lda, INTEGER n, INTEGER ml, INTEGER mu, 
           INTEGER ipvt[], REAL* rcond, REAL z[])
{
#define ABD(I_,J_)	(*(abd+(I_)*(lda)+(J_)))
INTEGER info, is, j, j_, ju, k, k_, kb, kb_, kp1, l, la, lm, lz, m, mm;
REAL anorm, ek, s, sm, t, wk, wkm, ynorm;

	//     compute 1-norm of a
	
	anorm = 0.0e0;
	l = ml + 1;
	is = l + mu;
	for( j = 1; j <= n; j++ ) { 
		j_ = j - 1;
		anorm = fmax( anorm, dasum( l, &ABD(j_,is - 1), 1 ) );
		if( is > ml + 1 ) 
			is = is - 1;
		if( j <= mu ) 
			l = l + 1;
		if( j >= n - ml ) 
			l = l - 1;
	}
	
	//     factor
	
	dgbfa( abd, lda, n, ml, mu, ipvt, &info );
	
	//     rcond = 1/(norm(a)*(estimate of norm(inverse(a)))) .
	//     estimate = norm(z)/norm(y) where  a*z = y  and  trans(a)*y = e .
	//     trans(a)  is the transpose of a .  the components of  e  are
	//     chosen to cause maximum local growth in the elements of w  where
	//     trans(u)*w = e .  the vectors are frequently rescaled to avoid
	//     overflow.
	
	//     solve trans(u)*w = e
	
	ek = 1.0e0;
	for( j = 1; j <= n; j++ ) { 
		j_ = j - 1;
		z[j_] = 0.0e0;
	}
	m = ml + mu + 1;
	ju = 0;
	for( k = 1; k <= n; k++ ) {
		k_ = k - 1; 
		if( z[k_] != 0.0e0 ) 
			ek = fsign( ek, -z[k_] );
		if( fabs( ek - z[k_] ) <= fabs( ABD(k_,m - 1) ) ) 
			goto L_30;
		s = fabs( ABD(k_,m - 1) )/fabs( ek - z[k_] );
		dscal( n, &s, z, 1 );
		ek = s*ek;
L_30:
		;
		wk = ek - z[k_];
		wkm = -ek - z[k_];
		s = fabs( wk );
		sm = fabs( wkm );
		if( ABD(k_,m - 1) == 0.0e0 ) 
			goto L_40;
		wk = wk/ABD(k_,m - 1);
		wkm = wkm/ABD(k_,m - 1);
		goto L_50;
L_40:
		;
		wk = 1.0e0;
		wkm = 1.0e0;
L_50:
		;
		kp1 = k + 1;
		ju = fmin( fmax( ju, mu + ipvt[k_] ), n );
		mm = m;
		if( kp1 > ju ) 
			goto L_90;
		for( j = kp1; j <= ju; j++ ) {
			j_ = j - 1; 
			mm = mm - 1;
			sm = sm + fabs( z[j_] + wkm*ABD(j_,mm - 1) );
			z[j_] = z[j_] + wk*ABD(j_,mm - 1);
			s = s + fabs( z[j_] );
		}
		if( s >= sm ) 
			goto L_80;
		t = wkm - wk;
		wk = wkm;
		mm = m;
		for( j = kp1; j <= ju; j++ ) { 
			j_ = j - 1;
			mm = mm - 1;
			z[j_] = z[j_] + t*ABD(j_,mm - 1);
		}
L_80:
		;
L_90:
		z[k_] = wk;
	}
	s = 1.0e0/dasum( n, z, 1 );
	dscal( n, &s, z, 1 );
	
	//     solve trans(l)*y = w
	
	for( kb = 1; kb <= n; kb++ ) { 
		kb_ = kb - 1;
		k = n + 1 - kb;
		lm = fmin( ml, n - k );
		if( k < n ) 
			z[k - 1] = z[k - 1] + ddot( lm, &ABD(k - 1,m), 1, 
			 &z[k], 1 );
		if( fabs( z[k - 1] ) <= 1.0e0 ) 
			goto L_110;
		s = 1.0e0/fabs( z[k - 1] );
		dscal( n, &s, z, 1 );
L_110:
		;
		l = ipvt[k - 1];
		t = z[l - 1];
		z[l - 1] = z[k - 1];
		z[k - 1] = t;
	}
	s = 1.0e0/dasum( n, z, 1 );
	dscal( n, &s, z, 1 );
	
	ynorm = 1.0e0;
	
	//     solve l*v = y
	
	for( k = 1; k <= n; k++ ) { 
		k_ = k - 1;
		l = ipvt[k_];
		t = z[l - 1];
		z[l - 1] = z[k_];
		z[k_] = t;
		lm = fmin( ml, n - k );
		if( k < n ) 
			daxpy( lm, &t, &ABD(k_,m), 1, &z[k_ + 1], 1 );
		if( fabs( z[k_] ) <= 1.0e0 ) 
			goto L_130;
		s = 1.0e0/fabs( z[k_] );
		dscal( n, &s, z, 1 );
		ynorm = s*ynorm;
L_130:
		;
	}
	s = 1.0e0/dasum( n, z, 1 );
	dscal( n, &s, z, 1 );
	ynorm = s*ynorm;
	
	//     solve  u*z = w
	
	for( kb = 1; kb <= n; kb++ ) {
		kb_ = kb - 1; 
		k = n + 1 - kb;
		if( fabs( z[k - 1] ) <= fabs( ABD(k - 1,m - 1) ) ) 
			goto L_150;
		s = fabs( ABD(k - 1,m - 1) )/fabs( z[k - 1] );
		dscal( n, &s, z, 1 );
		ynorm = s*ynorm;
L_150:
		;
		if( ABD(k - 1,m - 1) != 0.0e0 ) 
			z[k - 1] = z[k - 1]/ABD(k - 1,m - 1);
		if( ABD(k - 1,m - 1) == 0.0e0 ) 
			z[k - 1] = 1.0e0;
		lm = fmin( k, m ) - 1;
		la = m - lm;
		lz = k - lm;
		t = -z[k - 1];
		daxpy( lm, &t, &ABD(k - 1,la - 1), 1, &z[lz - 1], 1 );
	}
	//     make znorm = 1.0
	s = 1.0e0/dasum( n, z, 1 );
	dscal( n, &s, z, 1 );
	ynorm = s*ynorm;
	
	if( anorm != 0.0e0 ) 
		*rcond = ynorm/anorm;
	if( anorm == 0.0e0 ) 
		*rcond = 0.0e0;
	return;
#undef	ABD
}

//////////////////////////////////////////////////////////////////////////////

void dgbfa(REAL* abd, INTEGER lda, INTEGER n, INTEGER ml, INTEGER mu,
           INTEGER ipvt[], INTEGER* info)
{
#define ABD(I_,J_)	(*(abd+(I_)*(lda)+(J_)))
INTEGER i, i0, i_, j, j0, j1, j_, ju, jz, jz_, 
	 k, k_, kp1, l, lm, m, mm, nm1;
REAL t;


	m = ml + mu + 1;
	*info = 0;

/*     ZERO INITIAL FILL-IN COLUMNS
 */
	j0 = mu + 2;
	j1 = fmin( n, m ) - 1;
	if( j1 < j0 )
		goto L_30;
	for( jz = j0; jz <= j1; jz++ ){
		jz_ = jz - 1;
		i0 = m + 1 - jz;
		for( i = i0; i <= ml; i++ ){
			i_ = i - 1;
			ABD(jz_,i_) = 0.0e0;
			}
		}
L_30:
	;
	jz = j1;
	ju = 0;

/*     GAUSSIAN ELIMINATION WITH PARTIAL PIVOTING
 */
	nm1 = n - 1;
	if( nm1 < 1 )
		goto L_130;
	for( k = 1; k <= nm1; k++ ){
		k_ = k - 1;
		kp1 = k + 1;

/*        ZERO NEXT FILL-IN COLUMN
 */
		jz = jz + 1;
		if( jz > n )
			goto L_50;
		if( ml < 1 )
			goto L_50;
		for( i = 1; i <= ml; i++ ){
			i_ = i - 1;
			ABD(jz - 1,i_) = 0.0e0;
			}
L_50:
		;

/*        FIND L = PIVOT INDEX
 */
		lm = fmin( ml, n - k );
		l = idamax( lm + 1, &ABD(k_,m - 1), 1 ) + m - 1;
		ipvt[k_] = l + k - m;

/*        ZERO PIVOT IMPLIES THIS COLUMN ALREADY TRIANGULARIZED
 */
		if( ABD(k_,l - 1) == 0.0e0 )
			goto L_100;

/*           INTERCHANGE IF NECESSARY
 */
		if( l == m )
			goto L_60;
		t = ABD(k_,l - 1);
		ABD(k_,l - 1) = ABD(k_,m - 1);
		ABD(k_,m - 1) = t;
L_60:
		;

/*           COMPUTE MULTIPLIERS
 */
		t = -1.0e0/ABD(k_,m - 1);
		dscal( lm, &t, &ABD(k_,m), 1 );

/*           ROW ELIMINATION WITH COLUMN INDEXING
 */
		ju = fmin( fmax( ju, mu + ipvt[k_] ), n );
		mm = m;
		if( ju < kp1 )
			goto L_90;
		for( j = kp1; j <= ju; j++ ){
			j_ = j - 1;
			l = l - 1;
			mm = mm - 1;
			t = ABD(j_,l - 1);
			if( l == mm )
				goto L_70;
			ABD(j_,l - 1) = ABD(j_,mm - 1);
			ABD(j_,mm - 1) = t;
L_70:
			;
			daxpy( lm, &t, &ABD(k_,m), 1, &ABD(j_,mm), 1 );
			}
L_90:
		;
		goto L_110;
L_100:
		;
		*info = k;
L_110:
		;
		}
L_130:
	;
	ipvt[n - 1] = n;
	if( ABD(n - 1,m - 1) == 0.0e0 )
		*info = n;
	return;
#undef	ABD
}

//////////////////////////////////////////////////////////////////////////////

void dgbdi(REAL* abd, INTEGER lda, INTEGER n, INTEGER ml, INTEGER mu, 
           INTEGER ipvt[], REAL det[])
{
#define ABD(I_,J_)	(*(abd+(I_)*(lda)+(J_)))
INTEGER i, i_, m;
REAL ten;

	m = ml + mu + 1;
	det[0] = 1.0e0;
	det[1] = 0.0e0;
	ten = 10.0e0;
	for( i = 1; i <= n; i++ ) {
		i_ = i - 1; 
		if( ipvt[i_] != i ) 
			det[0] = -det[0];
		det[0] = ABD(i_,m - 1)*det[0];
		//     ...exit
		if( det[0] == 0.0e0 ) 
			goto L_60;
L_10:
		if( fabs( det[0] ) >= 1.0e0 ) 
			goto L_20;
		det[0] = ten*det[0];
		det[1] = det[1] - 1.0e0;
		goto L_10;
L_20:
		;
L_30:
		if( fabs( det[0] ) < ten ) 
			goto L_40;
		det[0] = det[0]/ten;
		det[1] = det[1] + 1.0e0;
		goto L_30;
L_40:
		;
	}
L_60:
	return;
#undef	ABD
}

//////////////////////////////////////////////////////////////////////////////

void dgbsl(REAL* abd, INTEGER lda, INTEGER n, INTEGER ml, INTEGER mu,
           INTEGER ipvt[], REAL b[], INTEGER job)
{
#define ABD(I_,J_)	(*(abd+(I_)*(lda)+(J_)))
INTEGER k, k_, kb, l, la, lb, lm, m, nm1;
REAL t;


	m = mu + ml + 1;
	nm1 = n - 1;
	if( job != 0 )
		goto L_50;

/*        JOB = 0 , SOLVE  A * X = B
 *        FIRST SOLVE L*Y = B
 */
	if( ml == 0 )
		goto L_30;
	if( nm1 < 1 )
		goto L_30;
	for( k = 1; k <= nm1; k++ ){
		k_ = k - 1;
		lm = fmin( ml, n - k );
		l = ipvt[k_];
		t = b[l - 1];
		if( l == k )
			goto L_10;
		b[l - 1] = b[k_];
		b[k_] = t;
L_10:
		;
		daxpy( lm, &t, &ABD(k_,m), 1, &b[k_ + 1], 1 );
		}
L_30:
	;

/*        NOW SOLVE  U*X = Y
 */
	for( kb = 1; kb <= n; kb++ ){
		k = n + 1 - kb;
		b[k - 1] = b[k - 1]/ABD(k - 1,m - 1);
		lm = fmin( k, m ) - 1;
		la = m - lm;
		lb = k - lm;
		t = -b[k - 1];
		daxpy( lm, &t, &ABD(k - 1,la - 1), 1, &b[lb - 1], 1 );
		}
	goto L_100;
L_50:
	;

/*        JOB = NONZERO, SOLVE  TRANS(A) * X = B
 *        FIRST SOLVE  TRANS(U)*Y = B
 */
	for( k = 1; k <= n; k++ ){
		k_ = k - 1;
		lm = fmin( k, m ) - 1;
		la = m - lm;
		lb = k - lm;
		t = ddot( lm, &ABD(k_,la - 1), 1, &b[lb - 1], 1 );
		b[k_] = (b[k_] - t)/ABD(k_,m - 1);
		}

/*        NOW SOLVE TRANS(L)*X = Y
 */
	if( ml == 0 )
		goto L_90;
	if( nm1 < 1 )
		goto L_90;
	for( kb = 1; kb <= nm1; kb++ ){
		k = n - kb;
		lm = fmin( ml, n - k );
		b[k - 1] = b[k - 1] + ddot( lm, &ABD(k - 1,m), 1, &b[k], 1 );
		l = ipvt[k - 1];
		if( l == k )
			goto L_70;
		t = b[l - 1];
		b[l - 1] = b[k - 1];
		b[k - 1] = t;
L_70:
		;
		}
L_90:
	;
L_100:
	;
	return;
#undef	ABD
}

//////////////////////////////////////////////////////////////////////////////

void dpoco(REAL* a, INTEGER lda,  INTEGER n, REAL* rcond, REAL z[], 
           INTEGER* info)
{
#define A(I_,J_)	(*(a+(I_)*(lda)+(J_)))
INTEGER i, i_, j, j_, jm1, k, k_, kb, kb_, kp1;
REAL anorm, ek, s, sm, t, wk, wkm, ynorm;

	//     find norm of a using only upper half
	
	for( j = 1; j <= n; j++ ) {
		j_ = j - 1; 
		z[j_] = dasum( j, &A(j_,0), 1 );
		jm1 = j - 1;
		if( jm1 < 1 ) 
			goto L_20;
		for( i = 1; i <= jm1; i++ ) {
			i_ = i - 1; 
			z[i_] = z[i_] + fabs( A(j_,i_) );
		}
L_20:
		;
	}
	anorm = 0.0e0;
	for( j = 1; j <= n; j++ ) {
		j_ = j - 1; 
		anorm = fmax( anorm, z[j_] );
	}
	
	//     factor
	
	dpofa( a, lda, n, info );
	if( info != 0 ) 
		goto L_180;
	
	//        rcond = 1/(norm(a)*(estimate of norm(inverse(a)))) .
	//        estimate = norm(z)/norm(y) where  a*z = y  and  a*y = e .
	//        the components of  e  are chosen to cause maximum local
	//        growth in the elements of w  where  trans(r)*w = e .
	//        the vectors are frequently rescaled to avoid overflow.
	
	//        solve trans(r)*w = e
	
	ek = 1.0e0;
	for( j = 1; j <= n; j++ ) {
		j_ = j - 1; 
		z[j_] = 0.0e0;
	}
	for( k = 1; k <= n; k++ ) {
		k_ = k - 1; 
		if( z[k_] != 0.0e0 ) 
			ek = fsign( ek, -z[k_] );
		if( fabs( ek - z[k_] ) <= A(k_,k_) ) 
			goto L_60;
		s = A(k_,k_)/fabs( ek - z[k_] );
		dscal( n, &s, z, 1 );
		ek = s*ek;
L_60:
		;
		wk = ek - z[k_];
		wkm = -ek - z[k_];
		s = fabs( wk );
		sm = fabs( wkm );
		wk = wk/A(k_,k_);
		wkm = wkm/A(k_,k_);
		kp1 = k + 1;
		if( kp1 > n ) 
			goto L_100;
		for( j = kp1; j <= n; j++ ) {
			j_ = j - 1; 
			sm = sm + fabs( z[j_] + wkm*A(j_,k_) );
			z[j_] = z[j_] + wk*A(j_,k_);
			s = s + fabs( z[j_] );
		}
		if( s >= sm ) 
			goto L_90;
		t = wkm - wk;
		wk = wkm;
		for( j = kp1; j <= n; j++ ) {
			j_ = j - 1; 
			z[j_] = z[j_] + t*A(j_,k_);
		}
L_90:
		;
L_100:
		z[k_] = wk;
	}
	s = 1.0e0/dasum( n, z, 1 );
	dscal( n, &s, z, 1 );
	
	//        solve r*y = w
	
	for( kb = 1; kb <= n; kb++ ) {
		kb_ = kb - 1; 
		k = n + 1 - kb;
		if( fabs( z[k - 1] ) <= A(k - 1,k - 1) ) 
			goto L_120;
		s = A(k - 1,k - 1)/fabs( z[k - 1] );
		dscal( n, &s, z, 1 );
L_120:
		;
		z[k - 1] = z[k - 1]/A(k - 1,k - 1);
		t = -z[k - 1];
		daxpy( k - 1, &t, &A(k - 1,0), 1, &z[0], 1 );
	}
	s = 1.0e0/dasum( n, z, 1 );
	dscal( n, &s, z, 1 );
	
	ynorm = 1.0e0;
	
	//        solve trans(r)*v = y
	
	for( k = 1; k <= n; k++ ) { 
		k_ = k - 1;
		z[k_] = z[k_] - ddot( k - 1, &A(k_,0), 1, &z[0], 1 );
		if( fabs( z[k_] ) <= A(k_,k_) ) 
			goto L_140;
		s = A(k_,k_)/fabs( z[k_] );
		dscal( n, &s, z, 1 );
		ynorm = s*ynorm;
L_140:
		;
		z[k_] = z[k_]/A(k_,k_);
	}
	s = 1.0e0/dasum( n, z, 1 );
	dscal( n, &s, z, 1 );
	ynorm = s*ynorm;
	
	//        solve r*z = v
	
	for( kb = 1; kb <= n; kb++ ) {
		kb_ = kb - 1; 
		k = n + 1 - kb;
		if( fabs( z[k - 1] ) <= A(k - 1,k - 1) ) 
			goto L_160;
		s = A(k - 1,k - 1)/fabs( z[k - 1] );
		dscal( n, &s, z, 1 );
		ynorm = s*ynorm;
L_160:
		;
		z[k - 1] = z[k - 1]/A(k - 1,k - 1);
		t = -z[k - 1];
		daxpy( k - 1, &t, &A(k - 1,0), 1, &z[0], 1 );
	}
	//        make znorm = 1.0
	s = 1.0e0/dasum( n, z, 1 );
	dscal( n, &s, z, 1 );
	ynorm = s*ynorm;
	
	if( anorm != 0.0e0 ) 
		*rcond = ynorm/anorm;
	if( anorm == 0.0e0 ) 
		*rcond = 0.0e0;
L_180:
	;
	return;
#undef	A
}

//////////////////////////////////////////////////////////////////////////////

void dpofa(REAL* a, INTEGER lda, INTEGER n, INTEGER* info)
{
#define A(I_,J_)	(*(a+(I_)*(lda)+(J_)))
INTEGER j, j_, jm1, k, k_;
REAL s, t;

	//     begin block with ...exits to 40
	
	
	for( j = 1; j <= n; j++ ) {
		j_ = j - 1; 
		*info = j;
		s = 0.0e0;
		jm1 = j - 1;
		if( jm1 < 1 ) 
			goto L_20;
		for( k = 1; k <= jm1; k++ ) {
			k_ = k - 1; 
			t = A(j_,k_) - ddot( k - 1, &A(k_,0), 1, &A(j_,0), 
			 1 );
			t = t/A(k_,k_);
			A(j_,k_) = t;
			s = s + t*t;
		}
L_20:
		s = A(j_,j_) - s;
		//     ......exit
		if( s <= 0.0e0 ) 
			goto L_40;
		A(j_,j_) = sqrt( s );
	}
	*info = 0;
L_40:
	;
	return;
#undef	A
} 

//////////////////////////////////////////////////////////////////////////////

void dpodi(REAL* a, INTEGER lda, INTEGER n, REAL det[], INTEGER job)
{
#define A(I_,J_)	(*(a+(I_)*(lda)+(J_)))
INTEGER i, i_, j, j_, jm1, k, k_, kp1;
REAL s, t;

	//     compute determinant
	
	if( job/10 == 0 ) 
		goto L_70;
	det[0] = 1.0e0;
	det[1] = 0.0e0;
	s = 10.0e0;
	for( i = 1; i <= n; i++ ) {
		i_ = i - 1; 
		det[0] = pow(A(i_,i_), 2)*det[0];
		//        ...exit
		if( det[0] == 0.0e0 ) 
			goto L_60;
L_10:
		if( det[0] >= 1.0e0 ) 
			goto L_20;
		det[0] = s*det[0];
		det[1] = det[1] - 1.0e0;
		goto L_10;
L_20:
		;
L_30:
		if( det[0] < s ) 
			goto L_40;
		det[0] = det[0]/s;
		det[1] = det[1] + 1.0e0;
		goto L_30;
L_40:
		;
	}
L_60:
	;
L_70:
	//     compute inverse(r)
	
	if( fmod( job, 10 ) == 0 ) 
		goto L_140;
	for( k = 1; k <= n; k++ ) {
		k_ = k - 1; 
		A(k_,k_) = 1.0e0/A(k_,k_);
		t = -A(k_,k_);
		dscal( k - 1, &t, &A(k_,0), 1 );
		kp1 = k + 1;
		if( n < kp1 ) 
			goto L_90;
		for( j = kp1; j <= n; j++ ) {
			j_ = j - 1; 
			t = A(j_,k_);
			A(j_,k_) = 0.0e0;
			daxpy( k, &t, &A(k_,0), 1, &A(j_,0), 1 );
		}
L_90:
		;
	}
	
	//        form  inverse(r) * trans(inverse(r))
	
	for( j = 1; j <= n; j++ ) { 
		j_ = j - 1;
		jm1 = j - 1;
		if( jm1 < 1 ) 
			goto L_120;
		for( k = 1; k <= jm1; k++ ) {
			k_ = k - 1; 
			t = A(j_,k_);
			daxpy( k, &t, &A(j_,0), 1, &A(k_,0), 1 );
		}
L_120:
		;
		t = A(j_,j_);
		dscal( j, &t, &A(j_,0), 1 );
	}
L_140:
	;
	return;
#undef	A
} 

//////////////////////////////////////////////////////////////////////////////

void dchdc(REAL* a, INTEGER lda, INTEGER p, REAL work[], 
	   INTEGER jpvt[], INTEGER job, INTEGER* info)
{
#define A(I_,J_)	(*(a+(I_)*(lda)+(J_)))
BOOLEAN negk, swapk;
INTEGER j, j_, jp, jt, k, k_, kb, kb_, km1, kp1, l, l_, maxl, pl, plp1, pu;
REAL maxdia, temp;

	pl = 1;
	pu = 0;
	*info = p;
	if( job == 0 ) 
		goto L_160;
	
	//        pivoting has been requested. rearrange the
	//        the elements according to jpvt.
	
	for( k = 1; k <= p; k++ ) {
		k_ = k - 1; 
		if ( jpvt[k_] > 0 ) swapk = TRUE;
		else                swapk = FALSE;
		if ( jpvt[k_] < 0 ) negk = TRUE;
		else                negk = FALSE;
		jpvt[k_] = k;
		if( negk == TRUE ) 
			jpvt[k_] = -jpvt[k_];
		if( swapk == FALSE ) 
			goto L_60;
		if( k == pl ) 
			goto L_50;
		dswap( pl - 1, &A(k_,0), 1, &A(pl - 1,0), 1 );
		temp = A(k_,k_);
		A(k_,k_) = A(pl - 1,pl - 1);
		A(pl - 1,pl - 1) = temp;
		plp1 = pl + 1;
		if( p < plp1 ) 
			goto L_40;
		for( j = plp1; j <= p; j++ ) { 
			j_ = j - 1;
			if( j >= k ) 
				goto L_10;
			temp = A(j_,pl - 1);
			A(j_,pl - 1) = A(k_,j_);
			A(k_,j_) = temp;
			goto L_20;
L_10:
			;
			if( j == k ) 
				goto L_20;
			temp = A(j_,k_);
			A(j_,k_) = A(j_,pl - 1);
			A(j_,pl - 1) = temp;
L_20:
			;
		}
L_40:
		;
		jpvt[k_] = jpvt[pl - 1];
		jpvt[pl - 1] = k;
L_50:
		;
		pl = pl + 1;
L_60:
		;
	}
	pu = p;
	if( p < pl ) 
		goto L_150;
	for( kb = pl; kb <= p; kb++ ) {
		kb_ = kb - 1; 
		k = p - kb + pl;
		if( jpvt[k - 1] >= 0 ) 
			goto L_130;
		jpvt[k - 1] = -jpvt[k - 1];
		if( pu == k ) 
			goto L_120;
		dswap( k - 1, &A(k - 1,0), 1, &A(pu - 1,0), 1 );
		temp = A(k - 1,k - 1);
		A(k - 1,k - 1) = A(pu - 1,pu - 1);
		A(pu - 1,pu - 1) = temp;
		kp1 = k + 1;
		if( p < kp1 ) 
			goto L_110;
		for( j = kp1; j <= p; j++ ) {
			j_ = j - 1; 
			if( j >= pu ) 
				goto L_80;
			temp = A(j_,k - 1);
			A(j_,k - 1) = A(pu - 1,j_);
			A(pu - 1,j_) = temp;
			goto L_90;
L_80:
			;
			if( j == pu ) 
				goto L_90;
			temp = A(j_,k - 1);
			A(j_,k - 1) = A(j_,pu - 1);
			A(j_,pu - 1) = temp;
L_90:
			;
		}
L_110:
		;
		jt = jpvt[k - 1];
		jpvt[k - 1] = jpvt[pu - 1];
		jpvt[pu - 1] = jt;
L_120:
		;
		pu = pu - 1;
L_130:
		;
	}
L_150:
	;
L_160:
	;
	for( k = 1; k <= p; k++ ) {
		k_ = k - 1; 
		
		//        reduction loop.
		
		maxdia = A(k_,k_);
		kp1 = k + 1;
		maxl = k;
		
		//        determine the pivot element.
		
		if( k < pl || k >= pu ) 
			goto L_190;
		for( l = kp1; l <= pu; l++ ) {
			l_ = l - 1; 
			if( A(l_,l_) <= maxdia ) 
				goto L_170;
			maxdia = A(l_,l_);
			maxl = l;
L_170:
			;
		}
L_190:
		;
		
		//        quit if the pivot element is not positive.
		
		if( maxdia > 0.0e0 ) 
			goto L_200;
		*info = k - 1;
		//     ......exit
		goto L_280;
L_200:
		;
		if( k == maxl ) 
			goto L_210;
		
		//           start the pivoting and update jpvt.
		
		km1 = k - 1;
		dswap( km1, &A(k_,0), 1, &A(maxl - 1,0), 1 );
		A(maxl - 1,maxl - 1) = A(k_,k_);
		A(k_,k_) = maxdia;
		jp = jpvt[maxl - 1];
		jpvt[maxl - 1] = jpvt[k_];
		jpvt[k_] = jp;
L_210:
		;
		
		//        reduction step. pivoting is contained across the rows.
		
		work[k_] = sqrt( A(k_,k_) );
		A(k_,k_) = work[k_];
		if( p < kp1 ) 
			goto L_260;
		for( j = kp1; j <= p; j++ ) { 
			j_ = j - 1;
			if( k == maxl ) 
				goto L_240;
			if( j >= maxl ) 
				goto L_220;
			temp = A(j_,k_);
			A(j_,k_) = A(maxl - 1,j_);
			A(maxl - 1,j_) = temp;
			goto L_230;
L_220:
			;
			if( j == maxl ) 
				goto L_230;
			temp = A(j_,k_);
			A(j_,k_) = A(j_,maxl - 1);
			A(j_,maxl - 1) = temp;
L_230:
			;
L_240:
			;
			A(j_,k_) = A(j_,k_)/work[k_];
			work[j_] = A(j_,k_);
			temp = -A(j_,k_);
			daxpy( j - k, &temp, &work[kp1 - 1], 1, &A(j_,kp1 - 1), 
			 1 );
		}
L_260:
		;
	}
L_280:
	;
	return;
#undef	A
} 

//////////////////////////////////////////////////////////////////////////////

REAL dasum(INTEGER n, REAL dx[], INTEGER incx)
{
	INTEGER _d_l, _d_m,_do0, _do1, i, i_, m, mp1, nincx;
	REAL dasum_v, dtemp;

	
	//     takes the sum of the absolute values.
	//     jack dongarra, linpack, 3/11/78.
	//     modified 3/93 to return if incx .le. 0.
	//     modified 12/3/93, array(1) declarations changed to array(*)
	
	
	dasum_v = 0.0e0;
	dtemp = 0.0e0;
	if( n <= 0 || incx <= 0 ) 
		return( dasum_v );
	if( incx == 1 ) 
		goto L_20;
	
	//        code for increment not equal to 1
	
	nincx = n*incx;
	for( i = 1, _do0=DOCNT(i,nincx,_do1 = incx); _do0 > 0; i += _do1, _do0-- ){
		i_ = i - 1; 
		dtemp = dtemp + fabs( dx[i_] );
	}
	dasum_v = dtemp;
	return( dasum_v );
	
	//        code for increment equal to 1
	
	
	//        clean-up loop
	
L_20:
	m = fmod( n, 6 );
	if( m == 0 ) 
		goto L_40;
	for( i = 1; i <= m; i++ ) {
		i_ = i - 1; 
		dtemp = dtemp + fabs( dx[i_] );
	}
	if( n < 6 ) 
		goto L_60;
L_40:
	mp1 = m + 1;
	for( i = mp1; i <= n; i += 6 ) {
		i_ = i - 1; 
		dtemp = dtemp + fabs( dx[i_] ) + fabs( dx[i_ + 1] ) + fabs( dx[i_ + 2] ) + 
		 fabs( dx[i_ + 3] ) + fabs( dx[i_ + 4] ) + fabs( dx[i_ + 5] );
	}
L_60:
	dasum_v = dtemp;
	return( dasum_v );
}

//////////////////////////////////////////////////////////////////////////////

void daxpy(INTEGER n, REAL* da, REAL dx[], INTEGER incx, REAL dy[],
           INTEGER incy)
{
INTEGER i, i_, ix, iy, m, mp1;


/*     CONSTANT TIMES A VECTOR PLUS A VECTOR.
 *     USES UNROLLED LOOPS FOR INCREMENTS EQUAL TO ONE.
 *     JACK DONGARRA, LINPACK, 3/11/78.
 */

	if( n <= 0 )
		return;
	if( *da == 0.0e0 )
		return;
	if( incx == 1 && incy == 1 )
		goto L_20;

/*        CODE FOR UNEQUAL INCREMENTS OR EQUAL INCREMENTS
 *          NOT EQUAL TO 1
 */
	ix = 1;
	iy = 1;
	if( incx < 0 )
		ix = (-n + 1)*incx + 1;
	if( incy < 0 )
		iy = (-n + 1)*incy + 1;
	for( i = 1; i <= n; i++ ){
		i_ = i - 1;
		dy[iy - 1] = dy[iy - 1] + *da*dx[ix - 1];
		ix = ix + incx;
		iy = iy + incy;
		}
	return;

/*        CODE FOR BOTH INCREMENTS EQUAL TO 1
 *
 *
 *        CLEAN-UP LOOP
 */
L_20:
	m = n%4;
	if( m == 0 )
		goto L_40;
	for( i = 1; i <= m; i++ ){
		i_ = i - 1;
		dy[i_] = dy[i_] + *da*dx[i_];
		}
	if( n < 4 )
		return;
L_40:
	mp1 = m + 1;
	for( i = mp1; i <= n; i += 4 ){
		i_ = i - 1;
		dy[i_] = dy[i_] + *da*dx[i_];
		dy[i_ + 1] = dy[i_ + 1] + *da*dx[i_ + 1];
		dy[i_ + 2] = dy[i_ + 2] + *da*dx[i_ + 2];
		dy[i_ + 3] = dy[i_ + 3] + *da*dx[i_ + 3];
		}
	return;
}

//////////////////////////////////////////////////////////////////////////////

void dscal(INTEGER n, REAL* da, REAL dx[], INTEGER incx)
{
INTEGER _d_l, _d_m, _do0, _do1, i, i_, m, mp1, nincx;


/*     SCALES A VECTOR BY A CONSTANT.
 *     USES UNROLLED LOOPS FOR INCREMENT EQUAL TO ONE.
 *     JACK DONGARRA, LINPACK, 3/11/78.
 */

	if( n <= 0 )
		return;
	if( incx == 1 )
		goto L_20;

/*        CODE FOR INCREMENT NOT EQUAL TO 1
 */
	nincx = n*incx;
	for( i = 1, _do0=DOCNT(i,nincx,_do1 = incx); _do0 > 0; i += _do1, _do0-- ){
		i_ = i - 1;
		dx[i_] = *da*dx[i_];
		}
	return;

/*        CODE FOR INCREMENT EQUAL TO 1
 *
 *
 *        CLEAN-UP LOOP
 */
L_20:
	m = n%5;
	if( m == 0 )
		goto L_40;
	for( i = 1; i <= m; i++ ){
		i_ = i - 1;
		dx[i_] = *da*dx[i_];
		}
	if( n < 5 )
		return;
L_40:
	mp1 = m + 1;
	for( i = mp1; i <= n; i += 5 ){
		i_ = i - 1;
		dx[i_] = *da*dx[i_];
		dx[i_ + 1] = *da*dx[i_ + 1];
		dx[i_ + 2] = *da*dx[i_ + 2];
		dx[i_ + 3] = *da*dx[i_ + 3];
		dx[i_ + 4] = *da*dx[i_ + 4];
		}
	return;
}

//////////////////////////////////////////////////////////////////////////////

REAL ddot(INTEGER n, REAL dx[], INTEGER incx, REAL dy[], INTEGER incy)
{
INTEGER i, i_, ix, iy, m, mp1;
REAL ddot_v, dtemp;


/*     FORMS THE DOT PRODUCT OF TWO VECTORS.
 *     USES UNROLLED LOOPS FOR INCREMENTS EQUAL TO ONE.
 *     JACK DONGARRA, LINPACK, 3/11/78.
 */

	ddot_v = 0.0e0;
	dtemp = 0.0e0;
	if( n <= 0 )
		return( ddot_v );
	if( incx == 1 && incy == 1 )
		goto L_20;

/*        CODE FOR UNEQUAL INCREMENTS OR EQUAL INCREMENTS
 *          NOT EQUAL TO 1
 */
	ix = 1;
	iy = 1;
	if( incx < 0 )
		ix = (-n + 1)*incx + 1;
	if( incy < 0 )
		iy = (-n + 1)*incy + 1;
	for( i = 1; i <= n; i++ ){
		i_ = i - 1;
		dtemp = dtemp + dx[ix - 1]*dy[iy - 1];
		ix = ix + incx;
		iy = iy + incy;
		}
	ddot_v = dtemp;
	return( ddot_v );

/*        CODE FOR BOTH INCREMENTS EQUAL TO 1
 *
 *
 *        CLEAN-UP LOOP
 */
L_20:
	m = n%5;
	if( m == 0 )
		goto L_40;
	for( i = 1; i <= m; i++ ){
		i_ = i - 1;
		dtemp = dtemp + dx[i_]*dy[i_];
		}
	if( n < 5 )
		goto L_60;
L_40:
	mp1 = m + 1;
	for( i = mp1; i <= n; i += 5 ){
		i_ = i - 1;
		dtemp = dtemp + dx[i_]*dy[i_] + dx[i_ + 1]*dy[i_ + 1] + 
		 dx[i_ + 2]*dy[i_ + 2] + dx[i_ + 3]*dy[i_ + 3] + dx[i_ + 4]*
		 dy[i_ + 4];
		}
L_60:
	ddot_v = dtemp;
	return( ddot_v );
}

//////////////////////////////////////////////////////////////////////////////

INTEGER idamax(INTEGER n, REAL dx[], INTEGER incx)
{
INTEGER i, i_, idamax_v, ix;
REAL dmax;


/*     FINDS THE INDEX OF ELEMENT HAVING MAX. ABSOLUTE VALUE.
 *     JACK DONGARRA, LINPACK, 3/11/78.
 */

	idamax_v = 0;
	if( n < 1 )
		return( idamax_v );
	idamax_v = 1;
	if( n == 1 )
		return( idamax_v );
	if( incx == 1 )
		goto L_20;

/*        CODE FOR INCREMENT NOT EQUAL TO 1
 */
	ix = 1;
	dmax = fabs( dx[0] );
	ix = ix + incx;
	for( i = 2; i <= n; i++ ){
		i_ = i - 1;
		if( fabs( dx[ix - 1] ) <= dmax )
			goto L_5;
		idamax_v = i;
		dmax = fabs( dx[ix - 1] );
L_5:
		ix = ix + incx;
		}
	return( idamax_v );

/*        CODE FOR INCREMENT EQUAL TO 1
 */
L_20:
	dmax = fabs( dx[0] );
	for( i = 2; i <= n; i++ ){
		i_ = i - 1;
		if( fabs( dx[i_] ) <= dmax )
			goto L_30;
		idamax_v = i;
		dmax = fabs( dx[i_] );
L_30:
		;
		}
	return( idamax_v );
}

//////////////////////////////////////////////////////////////////////////////

void dswap(INTEGER n, REAL dx[], INTEGER incx, REAL dy[], INTEGER incy)
{
INTEGER i, i_, ix, iy, m, mp1;
REAL dtemp;

	//     interchanges two vectors.
	//     uses unrolled loops for increments equal one.
	//     jack dongarra, linpack, 3/11/78.
	//     modified 12/3/93, array(1) declarations changed to array(*)
	
	
	if( n <= 0 ) 
		return;
	if( incx == 1 && incy == 1 ) 
		goto L_20;
	
	//       code for unequal increments or equal increments not equal
	//         to 1
	
	ix = 1;
	iy = 1;
	if( incx < 0 ) 
		ix = (-n + 1)*incx + 1;
	if( incy < 0 ) 
		iy = (-n + 1)*incy + 1;
	for( i = 1; i <= n; i++ ) {
		i_ = i - 1; 
		dtemp = dx[ix - 1];
		dx[ix - 1] = dy[iy - 1];
		dy[iy - 1] = dtemp;
		ix = ix + incx;
		iy = iy + incy;
	}
	return;
	
	//       code for both increments equal to 1
	
	
	//       clean-up loop
	
L_20:
	m = fmod( n, 3 );
	if( m == 0 ) 
		goto L_40;
	for( i = 1; i <= m; i++ ) {
		i_ = i - 1; 
		dtemp = dx[i_];
		dx[i_] = dy[i_];
		dy[i_] = dtemp;
	}
	if( n < 3 ) 
		return;
L_40:
	mp1 = m + 1;
	for( i = mp1; i <= n; i += 3 ) {
		i_ = i - 1; 
		dtemp = dx[i_];
		dx[i_] = dy[i_];
		dy[i_] = dtemp;
		dtemp = dx[i_ + 1];
		dx[i_ + 1] = dy[i_ + 1];
		dy[i_ + 1] = dtemp;
		dtemp = dx[i_ + 2];
		dx[i_ + 2] = dy[i_ + 2];
		dy[i_ + 2] = dtemp;
	}
	return;
}

//////////////////////////////////////////////////////////////////////////////





