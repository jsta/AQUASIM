///////////////////////////////   linpack.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    18.11.91    Peter Reichert
// revisions:   15.09.99    Peter Reichert    dgeco, dgedi, ,dgbco, dgbdi,
//                                            dpoco, dpofa, dpodi added
// revisions:   31.12.99    Peter Reichert    dcgdc added
//
//////////////////////////////////////////////////////////////////////////////
//
// "linpack" is a C++ translation of the FORTRAN linpack package
// =============================================================
//
//////////////////////////////////////////////////////////////////////////////
//
// Note: The following comments are the original comments of the FORTRAN
// ----- package. Due to the different range of indices of arrays in C and
//       FORTRAN all indices have to be reduced by one for the C++ versions.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef LINPACK_H
#define LINPACK_H

#include "fortran.h"

//////////////////////////////////////////////////////////////////////////////

void dgeco(REAL* a, INTEGER lda, INTEGER n, INTEGER ipvt[], 
           REAL* rcond, REAL z[]);

	//     dgeco factors a double precision matrix by gaussian elimination
	//     and estimates the condition of the matrix.
	
	//     if  rcond  is not needed, dgefa is slightly faster.
	//     to solve  a*x = b , follow dgeco by dgesl.
	//     to compute  inverse(a)*c , follow dgeco by dgesl.
	//     to compute  determinant(a) , follow dgeco by dgedi.
	//     to compute  inverse(a) , follow dgeco by dgedi.
	
	//     on entry
	
	//        a       double precision(lda, n)
	//                the matrix to be factored.
	
	//        lda     integer
	//                the leading dimension of the array  a .
	
	//        n       integer
	//                the order of the matrix  a .
	
	//     on return
	
	//        a       an upper triangular matrix and the multipliers
	//                which were used to obtain it.
	//                the factorization can be written  a = l*u  where
	//                l  is a product of permutation and unit lower
	//                triangular matrices and  u  is upper triangular.
	
	//        ipvt    integer(n)
	//                an integer vector of pivot indices.
	
	//        rcond   double precision
	//                an estimate of the reciprocal condition of  a .
	//                for the system  a*x = b , relative perturbations
	//                in  a  and  b  of size  epsilon  may cause
	//                relative perturbations in  x  of size  epsilon/rcond .
	//                if  rcond  is so small that the logical expression
	//                           1.0 + rcond .eq. 1.0
	//                is true, then  a  may be singular to working
	//                precision.  in particular,  rcond  is zero  if
	//                exact singularity is detected or the estimate
	//                underflows.
	
	//        z       double precision(n)
	//                a work vector whose contents are usually unimportant.
	//                if  a  is close to a singular matrix, then  z  is
	//                an approximate null vector in the sense that
	//                norm(a*z) = rcond*norm(a)*norm(z) .
	
	//     linpack. this version dated 08/14/78 .
	//     cleve moler, university of new mexico, argonne national lab.
	
	//     subroutines and functions
	
	//     linpack dgefa
	//     blas daxpy,ddot,dscal,dasum
	//     fortran dabs,dmax1,dsign
	
//////////////////////////////////////////////////////////////////////////////

void dgefa(REAL* a, INTEGER lda, INTEGER n, INTEGER ipvt[], INTEGER* info);

/*     DGEFA FACTORS A DOUBLE PRECISION MATRIX BY GAUSSIAN ELIMINATION.
 *
 *     DGEFA IS USUALLY CALLED BY DGECO, BUT IT CAN BE CALLED
 *     DIRECTLY WITH A SAVING IN TIME IF  RCOND  IS NOT NEEDED.
 *     (TIME FOR DGECO) = (1 + 9/N)*(TIME FOR DGEFA) .
 *
 *     ON ENTRY
 *
 *        A       DOUBLE PRECISION(LDA, N)
 *                THE MATRIX TO BE FACTORED.
 *
 *        LDA     INTEGER
 *                THE LEADING DIMENSION OF THE ARRAY  A .
 *
 *        N       INTEGER
 *                THE ORDER OF THE MATRIX  A .
 *
 *     ON RETURN
 *
 *        A       AN UPPER TRIANGULAR MATRIX AND THE MULTIPLIERS
 *                WHICH WERE USED TO OBTAIN IT.
 *                THE FACTORIZATION CAN BE WRITTEN  A = L*U  WHERE
 *                L  IS A PRODUCT OF PERMUTATION AND UNIT LOWER
 *                TRIANGULAR MATRICES AND  U  IS UPPER TRIANGULAR.
 *
 *        IPVT    INTEGER(N)
 *                AN INTEGER VECTOR OF PIVOT INDICES.
 *
 *        INFO    INTEGER
 *                = 0  NORMAL VALUE.
 *                = K  IF  U(K,K) .EQ. 0.0 .  THIS IS NOT AN ERROR
 *                     CONDITION FOR THIS SUBROUTINE, BUT IT DOES
 *                     INDICATE THAT DGESL OR DGEDI WILL DIVIDE BY ZERO
 *                     IF CALLED.  USE  RCOND  IN DGECO FOR A RELIABLE
 *                     INDICATION OF SINGULARITY.
 *
 *     LINPACK. THIS VERSION DATED 08/14/78 .	
 *     CLEVE MOLER, UNIVERSITY OF NEW MEXICO, ARGONNE NATIONAL LAB.
 *
 *     SUBROUTINES AND FUNCTIONS
 *
 *     BLAS DAXPY,DSCAL,IDAMAX
 */

//////////////////////////////////////////////////////////////////////////////

void dgedi(REAL* a, INTEGER lda, INTEGER n, INTEGER ipvt[], 
           REAL det[], REAL work[], INTEGER job);

	//     dgedi computes the determinant and inverse of a matrix
	//     using the factors computed by dgeco or dgefa.
	
	//     on entry
	
	//        a       double precision(lda, n)
	//                the output from dgeco or dgefa.
	
	//        lda     integer
	//                the leading dimension of the array  a .
	
	//        n       integer
	//                the order of the matrix  a .
	
	//        ipvt    integer(n)
	//                the pivot vector from dgeco or dgefa.
	
	//        work    double precision(n)
	//                work vector.  contents destroyed.
	
	//        job     integer
	//                = 11   both determinant and inverse.
	//                = 01   inverse only.
	//                = 10   determinant only.
	
	//     on return
	
	//        a       inverse of original matrix if requested.
	//                otherwise unchanged.
	
	//        det     double precision(2)
	//                determinant of original matrix if requested.
	//                otherwise not referenced.
	//                determinant = det(1) * 10.0**det(2)
	//                with  1.0 .le. dabs(det(1)) .lt. 10.0
	//                or  det(1) .eq. 0.0 .
	
	//     error condition
	
	//        a division by zero will occur if the input factor contains
	//        a zero on the diagonal and the inverse is requested.
	//        it will not occur if the subroutines are called correctly
	//        and if dgeco has set rcond .gt. 0.0 or dgefa has set
	//        info .eq. 0 .
	
	//     linpack. this version dated 08/14/78 .
	//     cleve moler, university of new mexico, argonne national lab.
	
	//     subroutines and functions
	
	//     blas daxpy,dscal,dswap
	//     fortran dabs,mod

//////////////////////////////////////////////////////////////////////////////

void dgesl(REAL* a, INTEGER lda, INTEGER n, INTEGER ipvt[], REAL b[],
           INTEGER job);

/*     DGESL SOLVES THE DOUBLE PRECISION SYSTEM
 *     A * X = B  OR  TRANS(A) * X = B
 *     USING THE FACTORS COMPUTED BY DGECO OR DGEFA.
 *
 *     ON ENTRY
 *
 *        A       DOUBLE PRECISION(LDA, N)
 *                THE OUTPUT FROM DGECO OR DGEFA.
 *
 *        LDA     INTEGER
 *                THE LEADING DIMENSION OF THE ARRAY  A .
 *
 *        N       INTEGER
 *                THE ORDER OF THE MATRIX  A .
 *
 *        IPVT    INTEGER(N)
 *                THE PIVOT VECTOR FROM DGECO OR DGEFA.
 *
 *        B       DOUBLE PRECISION(N)
 *                THE RIGHT HAND SIDE VECTOR.
 *
 *        JOB     INTEGER
 *                = 0         TO SOLVE  A*X = B ,
 *                = NONZERO   TO SOLVE  TRANS(A)*X = B  WHERE
 *                            TRANS(A)  IS THE TRANSPOSE.
 *
 *     ON RETURN
 *
 *        B       THE SOLUTION VECTOR  X .
 *
 *     ERROR CONDITION
 *
 *        A DIVISION BY ZERO WILL OCCUR IF THE INPUT FACTOR CONTAINS A
 *        ZERO ON THE DIAGONAL.  TECHNICALLY THIS INDICATES SINGULARITY
 *        BUT IT IS OFTEN CAUSED BY IMPROPER ARGUMENTS OR IMPROPER
 *        SETTING OF LDA .  IT WILL NOT OCCUR IF THE SUBROUTINES ARE
 *        CALLED CORRECTLY AND IF DGECO HAS SET RCOND .GT. 0.0
 *        OR DGEFA HAS SET INFO .EQ. 0 .
 *
 *     TO COMPUTE  INVERSE(A) * C  WHERE  C  IS A MATRIX
 *     WITH  P  COLUMNS
 *           CALL DGECO(A,LDA,N,IPVT,RCOND,Z)
 *           IF (RCOND IS TOO SMALL) GO TO ...
 *           DO 10 J = 1, P
 *              CALL DGESL(A,LDA,N,IPVT,C(1,J),0)
 *        10 CONTINUE
 *
 *     LINPACK. THIS VERSION DATED 08/14/78 .
 *     CLEVE MOLER, UNIVERSITY OF NEW MEXICO, ARGONNE NATIONAL LAB.
 *
 *     SUBROUTINES AND FUNCTIONS
 *
 *     BLAS DAXPY,DDOT
 */

//////////////////////////////////////////////////////////////////////////////

void dgbco(REAL* abd, INTEGER lda, INTEGER n, INTEGER ml, INTEGER mu, 
           INTEGER ipvt[], REAL* rcond, REAL z[]);

	//     dgbco factors a double precision band matrix by gaussian
	//     elimination and estimates the condition of the matrix.
	
	//     if  rcond  is not needed, dgbfa is slightly faster.
	//     to solve  a*x = b , follow dgbco by dgbsl.
	//     to compute  inverse(a)*c , follow dgbco by dgbsl.
	//     to compute  determinant(a) , follow dgbco by dgbdi.
	
	//     on entry
	
	//        abd     double precision(lda, n)
	//                contains the matrix in band storage.  the columns
	//                of the matrix are stored in the columns of  abd  and
	//                the diagonals of the matrix are stored in rows
	//                ml+1 through 2*ml+mu+1 of  abd .
	//                see the comments below for details.
	
	//        lda     integer
	//                the leading dimension of the array  abd .
	//                lda must be .ge. 2*ml + mu + 1 .
	
	//        n       integer
	//                the order of the original matrix.
	
	//        ml      integer
	//                number of diagonals below the main diagonal.
	//                0 .le. ml .lt. n .
	
	//        mu      integer
	//                number of diagonals above the main diagonal.
	//                0 .le. mu .lt. n .
	//                more efficient if  ml .le. mu .
	
	//     on return
	
	//        abd     an upper triangular matrix in band storage and
	//                the multipliers which were used to obtain it.
	//                the factorization can be written  a = l*u  where
	//                l  is a product of permutation and unit lower
	//                triangular matrices and  u  is upper triangular.
	
	//        ipvt    integer(n)
	//                an integer vector of pivot indices.
	
	//        rcond   double precision
	//                an estimate of the reciprocal condition of  a .
	//                for the system  a*x = b , relative perturbations
	//                in  a  and  b  of size  epsilon  may cause
	//                relative perturbations in  x  of size  epsilon/rcond .
	//                if  rcond  is so small that the logical expression
	//                           1.0 + rcond .eq. 1.0
	//                is true, then  a  may be singular to working
	//                precision.  in particular,  rcond  is zero  if
	//                exact singularity is detected or the estimate
	//                underflows.
	
	//        z       double precision(n)
	//                a work vector whose contents are usually unimportant.
	//                if  a  is close to a singular matrix, then  z  is
	//                an approximate null vector in the sense that
	//                norm(a*z) = rcond*norm(a)*norm(z) .
	
	//     band storage
	
	//           if  a  is a band matrix, the following program segment
	//           will set up the input.
	
	//                   ml = (band width below the diagonal)
	//                   mu = (band width above the diagonal)
	//                   m = ml + mu + 1
	//                   do 20 j = 1, n
	//                      i1 = max0(1, j-mu)
	//                      i2 = min0(n, j+ml)
	//                      do 10 i = i1, i2
	//                         k = i - j + m
	//                         abd(k,j) = a(i,j)
	//                10    continue
	//                20 continue
	
	//           this uses rows  ml+1  through  2*ml+mu+1  of  abd .
	//           in addition, the first  ml  rows in  abd  are used for
	//           elements generated during the triangularization.
	//           the total number of rows needed in  abd  is  2*ml+mu+1 .
	//           the  ml+mu by ml+mu  upper left triangle and the
	//           ml by ml  lower right triangle are not referenced.
	
	//     example..  if the original matrix is
	
	//           11 12 13  0  0  0
	//           21 22 23 24  0  0
	//            0 32 33 34 35  0
	//            0  0 43 44 45 46
	//            0  0  0 54 55 56
	//            0  0  0  0 65 66
	
	//      then  n = 6, ml = 1, mu = 2, lda .ge. 5  and abd should contain
	
	//            *  *  *  +  +  +  , * = not used
	//            *  * 13 24 35 46  , + = used for pivoting
	//            * 12 23 34 45 56
	//           11 22 33 44 55 66
	//           21 32 43 54 65  *
	
	//     linpack. this version dated 08/14/78 .
	//     cleve moler, university of new mexico, argonne national lab.
	
	//     subroutines and functions
	
	//     linpack dgbfa
	//     blas daxpy,ddot,dscal,dasum
	//     fortran dabs,dmax1,max0,min0,dsign

//////////////////////////////////////////////////////////////////////////////

void dgbfa(REAL* abd, INTEGER lda, INTEGER n, INTEGER ml, INTEGER mu,
           INTEGER ipvt[], INTEGER* info);

/*     DGBFA FACTORS A DOUBLE PRECISION BAND MATRIX BY ELIMINATION.
 *
 *     DGBFA IS USUALLY CALLED BY DGBCO, BUT IT CAN BE CALLED
 *     DIRECTLY WITH A SAVING IN TIME IF  RCOND  IS NOT NEEDED.
 *
 *     ON ENTRY
 *
 *        ABD     DOUBLE PRECISION(LDA, N)
 *                CONTAINS THE MATRIX IN BAND STORAGE.  THE COLUMNS
 *                OF THE MATRIX ARE STORED IN THE COLUMNS OF  ABD  AND
 *                THE DIAGONALS OF THE MATRIX ARE STORED IN ROWS
 *                ML+1 THROUGH 2*ML+MU+1 OF  ABD .
 *                SEE THE COMMENTS BELOW FOR DETAILS.
 *
 *        LDA     INTEGER
 *                THE LEADING DIMENSION OF THE ARRAY  ABD .
 *                LDA MUST BE .GE. 2*ML + MU + 1 .
 *
 *        N       INTEGER
 *                THE ORDER OF THE ORIGINAL MATRIX.
 *
 *        ML      INTEGER
 *                NUMBER OF DIAGONALS BELOW THE MAIN DIAGONAL.
 *                0 .LE. ML .LT. N .
 *
 *        MU      INTEGER
 *                NUMBER OF DIAGONALS ABOVE THE MAIN DIAGONAL.
 *                0 .LE. MU .LT. N .
 *                MORE EFFICIENT IF  ML .LE. MU .
 *     ON RETURN
 *
 *        ABD     AN UPPER TRIANGULAR MATRIX IN BAND STORAGE AND
 *                THE MULTIPLIERS WHICH WERE USED TO OBTAIN IT.
 *                THE FACTORIZATION CAN BE WRITTEN  A = L*U  WHERE
 *                L  IS A PRODUCT OF PERMUTATION AND UNIT LOWER
 *                TRIANGULAR MATRICES AND  U  IS UPPER TRIANGULAR.
 *
 *        IPVT    INTEGER(N)
 *                AN INTEGER VECTOR OF PIVOT INDICES.
 *
 *        INFO    INTEGER
 *                = 0  NORMAL VALUE.
 *                = K  IF  U(K,K) .EQ. 0.0 .  THIS IS NOT AN ERROR
 *                     CONDITION FOR THIS SUBROUTINE, BUT IT DOES
 *                     INDICATE THAT DGBSL WILL DIVIDE BY ZERO IF
 *                     CALLED.  USE  RCOND  IN DGBCO FOR A RELIABLE
 *                     INDICATION OF SINGULARITY.
 *
 *     BAND STORAGE
 *
 *           IF  A  IS A BAND MATRIX, THE FOLLOWING PROGRAM SEGMENT
 *           WILL SET UP THE INPUT.
 *
 *                   ML = (BAND WIDTH BELOW THE DIAGONAL)
 *                   MU = (BAND WIDTH ABOVE THE DIAGONAL)
 *                   M = ML + MU + 1
 *                   DO 20 J = 1, N
 *                      I1 = MAX0(1, J-MU)
 *                      I2 = MIN0(N, J+ML)
 *                      DO 10 I = I1, I2
 *                         K = I - J + M
 *                         ABD(K,J) = A(I,J)
 *                10    CONTINUE
 *                20 CONTINUE
 *
 *           THIS USES ROWS  ML+1  THROUGH  2*ML+MU+1  OF  ABD .
 *           IN ADDITION, THE FIRST  ML  ROWS IN  ABD  ARE USED FOR
 *           ELEMENTS GENERATED DURING THE TRIANGULARIZATION.
 *           THE TOTAL NUMBER OF ROWS NEEDED IN  ABD  IS  2*ML+MU+1 .
 *           THE  ML+MU BY ML+MU  UPPER LEFT TRIANGLE AND THE
 *           ML BY ML  LOWER RIGHT TRIANGLE ARE NOT REFERENCED.
 *
 *     LINPACK. THIS VERSION DATED 08/14/78 .
 *     CLEVE MOLER, UNIVERSITY OF NEW MEXICO, ARGONNE NATIONAL LAB.
 *
 *     SUBROUTINES AND FUNCTIONS
 *
 *     BLAS DAXPY,DSCAL,IDAMAX
 *     FORTRAN MAX0,MIN0
 */

//////////////////////////////////////////////////////////////////////////////

void dgbdi(REAL* abd, INTEGER lda, INTEGER n, INTEGER ml, INTEGER mu, 
           INTEGER ipvt[], REAL det[]);

	//     dgbdi computes the determinant of a band matrix
	//     using the factors computed by dgbco or dgbfa.
	//     if the inverse is needed, use dgbsl  n  times.
	
	//     on entry
	
	//        abd     double precision(lda, n)
	//                the output from dgbco or dgbfa.
	
	//        lda     integer
	//                the leading dimension of the array  abd .
	
	//        n       integer
	//                the order of the original matrix.
	
	//        ml      integer
	//                number of diagonals below the main diagonal.
	
	//        mu      integer
	//                number of diagonals above the main diagonal.
	
	//        ipvt    integer(n)
	//                the pivot vector from dgbco or dgbfa.
	
	//     on return
	
	//        det     double precision(2)
	//                determinant of original matrix.
	//                determinant = det(1) * 10.0**det(2)
	//                with  1.0 .le. dabs(det(1)) .lt. 10.0
	//                or  det(1) = 0.0 .
	
	//     linpack. this version dated 08/14/78 .
	//     cleve moler, university of new mexico, argonne national lab.
	
	//     subroutines and functions
	
	//     fortran dabs

//////////////////////////////////////////////////////////////////////////////

void dgbsl(REAL* abd, INTEGER lda, INTEGER n, INTEGER ml, INTEGER mu,
           INTEGER ipvt[], REAL b[], INTEGER job);

/*     DGBSL SOLVES THE DOUBLE PRECISION BAND SYSTEM
 *     A * X = B  OR  TRANS(A) * X = B
 *     USING THE FACTORS COMPUTED BY DGBCO OR DGBFA.
 *
 *     ON ENTRY
 *
 *        ABD     DOUBLE PRECISION(LDA, N)
 *                THE OUTPUT FROM DGBCO OR DGBFA.
 *
 *        LDA     INTEGER
 *                THE LEADING DIMENSION OF THE ARRAY  ABD .
 *
 *        N       INTEGER
 *                THE ORDER OF THE ORIGINAL MATRIX.
 *
 *        ML      INTEGER
 *                NUMBER OF DIAGONALS BELOW THE MAIN DIAGONAL.
 *
 *        MU      INTEGER
 *                NUMBER OF DIAGONALS ABOVE THE MAIN DIAGONAL.
 *
 *        IPVT    INTEGER(N)
 *                THE PIVOT VECTOR FROM DGBCO OR DGBFA.
 *
 *        B       DOUBLE PRECISION(N)
 *                THE RIGHT HAND SIDE VECTOR.
 *
 *        JOB     INTEGER
 *                = 0         TO SOLVE  A*X = B ,
 *                = NONZERO   TO SOLVE  TRANS(A)*X = B , WHERE
 *                            TRANS(A)  IS THE TRANSPOSE.
 *
 *     ON RETURN
 *
 *        B       THE SOLUTION VECTOR  X .
 *
 *     ERROR CONDITION
 *
 *        A DIVISION BY ZERO WILL OCCUR IF THE INPUT FACTOR CONTAINS A
 *        ZERO ON THE DIAGONAL.  TECHNICALLY THIS INDICATES SINGULARITY
 *        BUT IT IS OFTEN CAUSED BY IMPROPER ARGUMENTS OR IMPROPER
 *        SETTING OF LDA .  IT WILL NOT OCCUR IF THE SUBROUTINES ARE
 *        CALLED CORRECTLY AND IF DGBCO HAS SET RCOND .GT. 0.0
 *        OR DGBFA HAS SET INFO .EQ. 0 .
 *
 *     TO COMPUTE  INVERSE(A) * C  WHERE  C  IS A MATRIX
 *     WITH  P  COLUMNS
 *           CALL DGBCO(ABD,LDA,N,ML,MU,IPVT,RCOND,Z)
 *           IF (RCOND IS TOO SMALL) GO TO ...
 *           DO 10 J = 1, P
 *              CALL DGBSL(ABD,LDA,N,ML,MU,IPVT,C(1,J),0)
 *        10 CONTINUE
 *
 *     LINPACK. THIS VERSION DATED 08/14/78 .
 *     CLEVE MOLER, UNIVERSITY OF NEW MEXICO, ARGONNE NATIONAL LAB.
 *
 *     SUBROUTINES AND FUNCTIONS
 *
 *     BLAS DAXPY,DDOT
 *     FORTRAN MIN0
 */

//////////////////////////////////////////////////////////////////////////////

void dpoco(REAL* a, INTEGER lda,  INTEGER n, REAL* rcond, REAL z[], 
           INTEGER* info);

	//     dpoco factors a double precision symmetric positive definite
	//     matrix and estimates the condition of the matrix.
	
	//     if  rcond  is not needed, dpofa is slightly faster.
	//     to solve  a*x = b , follow dpoco by dposl.
	//     to compute  inverse(a)*c , follow dpoco by dposl.
	//     to compute  determinant(a) , follow dpoco by dpodi.
	//     to compute  inverse(a) , follow dpoco by dpodi.
	
	//     on entry
	
	//        a       double precision(lda, n)
	//                the symmetric matrix to be factored.  only the
	//                diagonal and upper triangle are used.
	
	//        lda     integer
	//                the leading dimension of the array  a .
	
	//        n       integer
	//                the order of the matrix  a .
	
	//     on return
	
	//        a       an upper triangular matrix  r  so that  a = trans(r)*r
	//                where  trans(r)  is the transpose.
	//                the strict lower triangle is unaltered.
	//                if  info .ne. 0 , the factorization is not complete.
	//                NOTE: often trans(r) ist used instead of r!.
	
	//        rcond   double precision
	//                an estimate of the reciprocal condition of  a .
	//                for the system  a*x = b , relative perturbations
	//                in  a  and  b  of size  epsilon  may cause
	//                relative perturbations in  x  of size  epsilon/rcond .
	//                if  rcond  is so small that the logical expression
	//                           1.0 + rcond .eq. 1.0
	//                is true, then  a  may be singular to working
	//                precision.  in particular,  rcond  is zero  if
	//                exact singularity is detected or the estimate
	//                underflows.  if info .ne. 0 , rcond is unchanged.
	
	//        z       double precision(n)
	//                a work vector whose contents are usually unimportant.
	//                if  a  is close to a singular matrix, then  z  is
	//                an approximate null vector in the sense that
	//                norm(a*z) = rcond*norm(a)*norm(z) .
	//                if  info .ne. 0 , z  is unchanged.
	
	//        info    integer
	//                = 0  for normal return.
	//                = k  signals an error condition.  the leading minor
	//                     of order  k  is not positive definite.
	
	//     linpack.  this version dated 08/14/78 .
	//     cleve moler, university of new mexico, argonne national lab.
	
	//     subroutines and functions
	
	//     linpack dpofa
	//     blas daxpy,ddot,dscal,dasum
	//     fortran dabs,dmax1,dreal,dsign

//////////////////////////////////////////////////////////////////////////////

void dpofa(REAL* a, INTEGER lda, INTEGER n, INTEGER* info);

	//     dpofa factors a double precision symmetric positive definite
	//     matrix.
	
	//     dpofa is usually called by dpoco, but it can be called
	//     directly with a saving in time if  rcond  is not needed.
	//     (time for dpoco) = (1 + 18/n)*(time for dpofa) .
	
	//     on entry
	
	//        a       double precision(lda, n)
	//                the symmetric matrix to be factored.  only the
	//                diagonal and upper triangle are used.
	
	//        lda     integer
	//                the leading dimension of the array  a .
	
	//        n       integer
	//                the order of the matrix  a .
	
	//     on return
	
	//        a       an upper triangular matrix  r  so that  a = trans(r)*r
	//                where  trans(r)  is the transpose.
	//                the strict lower triangle is unaltered.
	//                if  info .ne. 0 , the factorization is not complete.
	
	//        info    integer
	//                = 0  for normal return.
	//                = k  signals an error condition.  the leading minor
	//                     of order  k  is not positive definite.
	
	//     linpack.  this version dated 08/14/78 .
	//     cleve moler, university of new mexico, argonne national lab.
	
	//     subroutines and functions
	
	//     blas ddot
	//     fortran dsqrt

//////////////////////////////////////////////////////////////////////////////

void dpodi(REAL* a, INTEGER lda, INTEGER n, REAL det[], INTEGER job);

	//     dpodi computes the determinant and inverse of a certain
	//     double precision symmetric positive definite matrix (see below)
	//     using the factors computed by dpoco, dpofa or dqrdc.
	
	//     on entry
	
	//        a       double precision(lda, n)
	//                the output  a  from dpoco or dpofa
	//                or the output  x  from dqrdc.
	
	//        lda     integer
	//                the leading dimension of the array  a .
	
	//        n       integer
	//                the order of the matrix  a .
	
	//        job     integer
	//                = 11   both determinant and inverse.
	//                = 01   inverse only.
	//                = 10   determinant only.
	
	//     on return
	
	//        a       if dpoco or dpofa was used to factor  a  then
	//                dpodi produces the upper half of inverse(a) .
	//                if dqrdc was used to decompose  x  then
	//                dpodi produces the upper half of inverse(trans(x)*x)
	//                where trans(x) is the transpose.
	//                elements of  a  below the diagonal are unchanged.
	//                if the units digit of job is zero,  a  is unchanged.
	
	//        det     double precision(2)
	//                determinant of  a  or of  trans(x)*x  if requested.
	//                otherwise not referenced.
	//                determinant = det(1) * 10.0**det(2)
	//                with  1.0 .le. det(1) .lt. 10.0
	//                or  det(1) .eq. 0.0 .
	
	//     error condition
	
	//        a division by zero will occur if the input factor contains
	//        a zero on the diagonal and the inverse is requested.
	//        it will not occur if the subroutines are called correctly
	//        and if dpoco or dpofa has set info .eq. 0 .
	
	//     linpack.  this version dated 08/14/78 .
	//     cleve moler, university of new mexico, argonne national lab.
	
	//     subroutines and functions
	
	//     blas daxpy,dscal
	//     fortran mod

//////////////////////////////////////////////////////////////////////////////

void dchdc(REAL* a, INTEGER lda, INTEGER p, REAL work[], 
	   INTEGER jpvt[], INTEGER job, INTEGER* info);

	//     dchdc computes the cholesky decomposition of a positive definite
	//     matrix.  a pivoting option allows the user to estimate the
	//     condition of a positive definite matrix or determine the rank
	//     of a positive semidefinite matrix.
	
	//     on entry
	
	//         a      double precision(lda,p).
	//                a contains the matrix whose decomposition is to
	//                be computed.  only the upper half of a need be stored.
	//                the lower part of the array a is not referenced.
	
	//         lda    integer.
	//                lda is the leading dimension of the array a.
	
	//         p      integer.
	//                p is the order of the matrix.
	
	//         work   double precision(p).
	//                work is a work array.
	
	//         jpvt   integer(p).
	//                jpvt contains integers that control the selection
	//                of the pivot elements, if pivoting has been requested.
	//                each diagonal element a(k,k)
	//                is placed in one of three classes according to the
	//                value of jpvt(k).
	
	//                   if jpvt(k) .gt. 0, then x(k) is an initial
	//                                      element.
	
	//                   if jpvt(k) .eq. 0, then x(k) is a free element.
	
	//                   if jpvt(k) .lt. 0, then x(k) is a final element.
	
	//                before the decomposition is computed, initial elements
	//                are moved by symmetric row and column interchanges to
	//                the beginning of the array a and final
	//                elements to the end.  both initial and final elements
	//                are frozen in place during the computation and only
	//                free elements are moved.  at the k-th stage of the
	//                reduction, if a(k,k) is occupied by a free element
	//                it is interchanged with the largest free element
	//                a(l,l) with l .ge. k.  jpvt is not referenced if
	//                job .eq. 0.
	
	//        job     integer.
	//                job is an integer that initiates column pivoting.
	//                if job .eq. 0, no pivoting is done.
	//                if job .ne. 0, pivoting is done.
	
	//     on return
	
	//         a      a contains in its upper half the cholesky factor
	//                of the matrix a as it has been permuted by pivoting.
	
	//         jpvt   jpvt(j) contains the index of the diagonal element
	//                of a that was moved into the j-th position,
	//                provided pivoting was requested.
	
	//         info   contains the index of the last positive diagonal
	//                element of the cholesky factor.
	
	//     for positive definite matrices info = p is the normal return.
	//     for pivoting with positive semidefinite matrices info will
	//     in general be less than p.  however, info may be greater than
	//     the rank of a, since rounding error can cause an otherwise zero
	//     element to be positive. indefinite systems will always cause
	//     info to be less than p.
	
	//     linpack. this version dated 08/14/78 .
	//     j.j. dongarra and g.w. stewart, argonne national laboratory and
	//     university of maryland.
	
	//     blas daxpy,dswap
	//     fortran dsqrt

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////

