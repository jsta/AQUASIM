*GAMS : Fullsource of GAMCDF in DATAPAC from CAMSUN
* ======================================================================
* NIST Guide to Available Math Software.
* Fullsource for module GAMCDF from package DATAPAC.
* Retrieved from CAMSUN on Sat Apr 15 23:29:00 2000.
* ======================================================================
      SUBROUTINE GAMCDF(X,GAMMA,CDF)
C
C     PURPOSE--THIS SUBROUTINE COMPUTES THE CUMULATIVE DISTRIBUTION
C              FUNCTION VALUE FOR THE GAMMA
C              DISTRIBUTION WITH SINGLE PRECISION 
C              TAIL LENGTH PARAMETER = GAMMA.
C              THE GAMMA DISTRIBUTION USED
C              HEREIN HAS MEAN = GAMMA
C              AND STANDARD DEVIATION = SQRT(GAMMA).
C              THIS DISTRIBUTION IS DEFINED FOR ALL POSITIVE X,
C              AND HAS THE PROBABILITY DENSITY FUNCTION
C              F(X) = (1/CONSTANT) * (X**(GAMMA-1)) * EXP(-X)
C              WHERE THE CONSTANT = THE GAMMA FUNCTION EVALUATED
C              AT THE VALUE GAMMA.
C     INPUT  ARGUMENTS--X      = THE SINGLE PRECISION VALUE 
C                                AT WHICH THE CUMULATIVE DISTRIBUTION 
C                                FUNCTION IS TO BE EVALUATED.
C                                X SHOULD BE POSITIVE.
C                     --GAMMA  = THE SINGLE PRECISION VALUE 
C                                OF THE TAIL LENGTH PARAMETER.
C                                GAMMA SHOULD BE POSITIVE.
C     OUTPUT ARGUMENTS--CDF    = THE SINGLE PRECISION CUMULATIVE
C                                DISTRIBUTION FUNCTION VALUE.
C     OUTPUT--THE SINGLE PRECISION CUMULATIVE DISTRIBUTION
C             FUNCTION VALUE CDF FOR THE GAMMA DISTRIBUTION 
C             WITH TAIL LENGTH PARAMETER VALUE = GAMMA.
C     PRINTING--NONE UNLESS AN INPUT ARGUMENT ERROR CONDITION EXISTS. 
C     RESTRICTIONS--GAMMA SHOULD BE POSITIVE.
C                 --X SHOULD BE POSITIVE.
C     OTHER DATAPAC   SUBROUTINES NEEDED--NONE.
C     FORTRAN LIBRARY SUBROUTINES NEEDED--DEXP, DLOG.
C     MODE OF INTERNAL OPERATIONS--DOUBLE PRECISION.
C     LANGUAGE--ANSI FORTRAN. 
C     ACCURACY--(ON THE UNIVAC 1108, EXEC 8 SYSTEM AT NBS)
C               COMPARED TO THE KNOWN GAMMA = 1 (EXPONENTIAL)
C               RESULTS, AGREEMENT WAS HAD OUT TO 7 SIGNIFICANT
C               DIGITS FOR ALL TESTED X.
C               THE TESTED X VALUES COVERED THE ENTIRE
C               RANGE OF THE DISTRIBUTION--FROM THE 0.00001 
C               PERCENT POINT UP TO THE 99.99999 PERCENT POINT
C               OF THE DISTRIBUTION.
C     REFERENCES--WILK, GNANADESIKAN, AND HUYETT, 'PROBABILITY
C                 PLOTS FOR THE GAMMA DISTRIBUTION',
C                 TECHNOMETRICS, 1962, PAGES 1-15,
C                 ESPECIALLY PAGES 3-5. 
C               --NATIONAL BUREAU OF STANDARDS APPLIED MATHEMATICS
C                 SERIES 55, 1964, PAGE 257, FORMULA 6.1.41.
C               --JOHNSON AND KOTZ, CONTINUOUS UNIVARIATE
C                 DISTRIBUTIONS--1, 1970, PAGES 166-206.
C               --HASTINGS AND PEACOCK, STATISTICAL
C                 DISTRIBUTIONS--A HANDBOOK FOR
C                 STUDENTS AND PRACTITIONERS, 1975,
C                 PAGES 68-73.
C     WRITTEN BY--JAMES J. FILLIBEN
C                 STATISTICAL ENGINEERING LABORATORY (205.03)
C                 NATIONAL BUREAU OF STANDARDS
C                 WASHINGTON, D. C. 20234
C                 PHONE:  301-921-2315
C     ORIGINAL VERSION--NOVEMBER  1975. 
C
C---------------------------------------------------------------------
C
      DOUBLE PRECISION DX,DGAMMA,AI,TERM,SUM,CUT1,CUT2,CUTOFF,T
      DOUBLE PRECISION Z,Z2,Z3,Z4,Z5,DEN,A,B,C,D,G
      DOUBLE PRECISION DEXP,DLOG
      DIMENSION D(10)
      DATA C/ .918938533204672741D0/
      DATA D(1),D(2),D(3),D(4),D(5)
     1                 /+.833333333333333333D-1,-.277777777777777778D-2,
     1+.793650793650793651D-3,-.595238095238095238D-3,+.8417508417508417
     151D-3/
      DATA D(6),D(7),D(8),D(9),D(10)
     1     /-.191752691752691753D-2,+.641025641025641025D-2,-.2955065359
     147712418D-1,+.179644372368830573D0,-.139243221690590111D1/
C
C
      IPR=6
C
C     CHECK THE INPUT ARGUMENTS FOR ERRORS
C
      IF(X.LE.0.0)GOTO50
      IF(GAMMA.LE.0.0)GOTO55
      GOTO90
   50 WRITE(IPR,4)
      WRITE(IPR,46)X
      CDF=0.0
      RETURN
   55 WRITE(IPR,15) 
      WRITE(IPR,46)GAMMA
      CDF=0.0
      RETURN
   90 CONTINUE
    4 FORMAT(1H ,100H***** NON-FATAL DIAGNOSTIC--THE FIRST  INPUT ARGUME
     1NT TO THE GAMCDF SUBROUTINE IS NON-POSITIVE *****)
   15 FORMAT(1H , 91H***** FATAL ERROR--THE SECOND INPUT ARGUMENT TO THE
     1 GAMCDF SUBROUTINE IS NON-POSITIVE *****)
   46 FORMAT(1H , 35H***** THE VALUE OF THE ARGUMENT IS ,E15.8,6H *****)
C
C-----START POINT-----------------------------------------------------
C
      DX=X
      DGAMMA=GAMMA
      MAXIT=10000
C
C     COMPUTE THE GAMMA FUNCTION USING THE ALGORITHM IN THE 
C     NBS APPLIED MATHEMATICS SERIES REFERENCE.
C
      Z=GAMMA
      DEN=1.0D0
  300 IF(Z.GE.10.0D0)GOTO400
      DEN=DEN*Z
      Z=Z+1
      GOTO300
  400 Z2=Z*Z
      Z3=Z*Z2
      Z4=Z2*Z2
      Z5=Z2*Z3
      A=(Z-0.5D0)*DLOG(Z)-Z+C 
      B=D(1)/Z+D(2)/Z3+D(3)/Z5+D(4)/(Z2*Z5)+D(5)/(Z4*Z5)+
     1D(6)/(Z*Z5*Z5)+D(7)/(Z3*Z5*Z5)+D(8)/(Z5*Z5*Z5)+D(9)/(Z2*Z5*Z5*Z5)
      G=DEXP(A+B)/DEN
C
C     COMPUTE T-SUB-Q AS DEFINED ON PAGE 4 OF THE WILK, GNANADESIKAN, 
C     AND HUYETT REFERENCE
C
      SUM=1.0D0/DGAMMA
      TERM=1.0D0/DGAMMA
      CUT1=DX-DGAMMA
      CUT2=DX*10000000000.0D0 
      DO 200 I=1,MAXIT
      AI=I
      TERM=DX*TERM/(DGAMMA+AI)
      SUM=SUM+TERM
      CUTOFF=CUT1+(CUT2*TERM/SUM)
      IF(AI.GT.CUTOFF)GOTO250 
  200 CONTINUE
      WRITE(IPR,205)MAXIT
      WRITE(IPR,206)X
      WRITE(IPR,207)GAMMA
      WRITE(IPR,208)
      CDF=1.0
      RETURN
C
  250 T=SUM
      CDF=(DX**DGAMMA)*(DEXP(-DX))*T/G
C
  205 FORMAT(1H ,48H*****ERROR IN INTERNAL OPERATIONS IN THE GAMCDF , 
     1 45HSUBROUTINE--THE NUMBER OF ITERATIONS EXCEEDS ,I7) 
  206 FORMAT(1H ,33H     THE INPUT VALUE OF X     IS ,E15.8)
  207 FORMAT(1H ,33H     THE INPUT VALUE OF GAMMA IS ,E15.8)
  208 FORMAT(1H ,48H     THE OUTPUT VALUE OF CDF HAS BEEN SET TO 1.0) 
C
      RETURN
      END 
C********************************************************************
*GAMS : Fullsource of GAMPPF in DATAPAC from CAMSUN
* ======================================================================
* NIST Guide to Available Math Software.
* Fullsource for module GAMPPF from package DATAPAC.
* Retrieved from CAMSUN on Sat Apr 15 23:24:04 2000.
* ======================================================================
      SUBROUTINE GAMPPF(P,GAMMA,PPF)
C
C     PURPOSE--THIS SUBROUTINE COMPUTES THE PERCENT POINT
C              FUNCTION VALUE FOR THE GAMMA DISTRIBUTION
C              WITH SINGLE PRECISION
C              TAIL LENGTH PARAMETER = GAMMA.
C              THE GAMMA DISTRIBUTION USED
C              HEREIN HAS MEAN = GAMMA
C              AND STANDARD DEVIATION = SQRT(GAMMA).
C              THIS DISTRIBUTION IS DEFINED FOR ALL POSITIVE X,
C              AND HAS THE PROBABILITY DENSITY FUNCTION
C              F(X) = (1/CONSTANT) * (X**(GAMMA-1)) * EXP(-X)
C              WHERE THE CONSTANT = THE GAMMA FUNCTION EVALUATED
C              AT THE VALUE GAMMA.
C              NOTE THAT THE PERCENT POINT FUNCTION OF A DISTRIBUTION 
C              IS IDENTICALLY THE SAME AS THE INVERSE CUMULATIVE
C              DISTRIBUTION FUNCTION OF THE DISTRIBUTION.
C     INPUT  ARGUMENTS--P      = THE SINGLE PRECISION VALUE 
C                                (BETWEEN 0.0 (EXCLUSIVELY) 
C                                AND 1.0 (EXCLUSIVELY))
C                                AT WHICH THE PERCENT POINT 
C                                FUNCTION IS TO BE EVALUATED.
C                     --GAMMA  = THE SINGLE PRECISION VALUE OF THE
C                                TAIL LENGTH PARAMETER.
C                                GAMMA SHOULD BE POSITIVE.
C     OUTPUT ARGUMENTS--PPF    = THE SINGLE PRECISION PERCENT
C                                POINT FUNCTION VALUE.
C     OUTPUT--THE SINGLE PRECISION PERCENT POINT FUNCTION . 
C             VALUE PPF FOR THE GAMMA DISTRIBUTION
C             WITH TAIL LENGTH PARAMETER VALUE = GAMMA.
C     PRINTING--NONE UNLESS AN INPUT ARGUMENT ERROR CONDITION EXISTS. 
C     RESTRICTIONS--GAMMA SHOULD BE POSITIVE.
C                 --P SHOULD BE BETWEEN 0.0 (EXCLUSIVELY)
C                   AND 1.0 (EXCLUSIVELY).
C     OTHER DATAPAC   SUBROUTINES NEEDED--NONE.
C     FORTRAN LIBRARY SUBROUTINES NEEDED--DEXP, DLOG.
C     MODE OF INTERNAL OPERATIONS--DOUBLE PRECISION.
C     LANGUAGE--ANSI FORTRAN. 
C     ACCURACY--(ON THE UNIVAC 1108, EXEC 8 SYSTEM AT NBS)
C               COMPARED TO THE KNOWN GAMMA = 1 (EXPONENTIAL)
C               RESULTS, AGREEMENT WAS HAD OUT TO 6 SIGNIFICANT
C               DIGITS FOR ALL TESTED P IN THE RANGE P = .001 TO
C               P = .999.  FOR P = .95 AND SMALLER, THE AGREEMENT
C               WAS EVEN BETTER--7 SIGNIFICANT DIGITS.
C               (NOTE THAT THE TABULATED VALUES GIVEN IN THE WILK,
C               GNANADESIKAN, AND HUYETT REFERENCE BELOW, PAGE 20,
C               ARE IN ERROR FOR AT LEAST THE GAMMA = 1 CASE--
C               THE WORST DETECTED ERROR WAS AGREEMENT TO ONLY 3
C               SIGNIFICANT DIGITS (IN THEIR 8 SIGNIFICANT DIGIT TABLE)
C               FOR P = .999.)
C     REFERENCES--WILK, GNANADESIKAN, AND HUYETT, 'PROBABILITY
C                 PLOTS FOR THE GAMMA DISTRIBUTION',
C                 TECHNOMETRICS, 1962, PAGES 1-15,
C                 ESPECIALLY PAGES 3-5. 
C               --NATIONAL BUREAU OF STANDARDS APPLIED MATHEMATICS
C                 SERIES 55, 1964, PAGE 257, FORMULA 6.1.41.
C               --JOHNSON AND KOTZ, CONTINUOUS UNIVARIATE
C                 DISTRIBUTIONS--1, 1970, PAGES 166-206.
C               --HASTINGS AND PEACOCK, STATISTICAL
C                 DISTRIBUTIONS--A HANDBOOK FOR
C                 STUDENTS AND PRACTITIONERS, 1975,
C                 PAGES 68-73.
C     WRITTEN BY--JAMES J. FILLIBEN
C                 STATISTICAL ENGINEERING LABORATORY (205.03)
C                 NATIONAL BUREAU OF STANDARDS
C                 WASHINGTON, D. C. 20234
C                 PHONE:  301-921-2315
C     ORIGINAL VERSION--NOVEMBER  1974. 
C     UPDATED         --SEPTEMBER 1975. 
C     UPDATED         --NOVEMBER  1975. 
C
C---------------------------------------------------------------------
C
      DOUBLE PRECISION DP,DGAMMA
      DOUBLE PRECISION Z,Z2,Z3,Z4,Z5,DEN,A,B,C,D,G
      DOUBLE PRECISION XMIN0,XMIN,AI,XMAX,DX,PCALC,XMID
      DOUBLE PRECISION XLOWER,XUPPER,XDEL
      DOUBLE PRECISION SUM,TERM,CUT1,CUT2,AJ,CUTOFF,T
      DOUBLE PRECISION DEXP,DLOG
      DIMENSION D(10)
      DATA C/ .918938533204672741D0/
      DATA D(1),D(2),D(3),D(4),D(5)
     1                 /+.833333333333333333D-1,-.277777777777777778D-2,
     1+.793650793650793651D-3,-.595238095238095238D-3,+.8417508417508417
     151D-3/
      DATA D(6),D(7),D(8),D(9),D(10)
     1     /-.191752691752691753D-2,+.641025641025641025D-2,-.2955065359
     147712418D-1,+.179644372368830573D0,-.139243221690590111D1/
C
      IPR=6
C
C     CHECK THE INPUT ARGUMENTS FOR ERRORS
C
      IF(P.LE.0.0.OR.P.GE.1.0)GOTO50
      IF(GAMMA.LE.0.0)GOTO55
      GOTO90
   50 WRITE(IPR,1)
      WRITE(IPR,46)P
      PPF=0.0
      RETURN
   55 WRITE(IPR,15) 
      WRITE(IPR,46)GAMMA
      PPF=0.0
      RETURN
   90 CONTINUE
    1 FORMAT(1H ,115H***** FATAL ERROR--THE FIRST  INPUT ARGUMENT TO THE
     1 GAMPPF SUBROUTINE IS OUTSIDE THE ALLOWABLE (0,1) INTERVAL *****)
   15 FORMAT(1H , 91H***** FATAL ERROR--THE SECOND INPUT ARGUMENT TO THE
     1 GAMPPF SUBROUTINE IS NON-POSITIVE *****)
   46 FORMAT(1H , 35H***** THE VALUE OF THE ARGUMENT IS ,E15.8,6H *****)
C
C-----START POINT-----------------------------------------------------
C
      DP=P
      DGAMMA=GAMMA
      MAXIT=10000
C
C     COMPUTE THE GAMMA FUNCTION USING THE ALGORITHM IN THE 
C     NBS APPLIED MATHEMATICS SERIES REFERENCE.
C     THIS GAMMA FUNCTION NEED BE CALCULATED ONLY ONCE.
C     IT IS USED IN THE CALCULATION OF THE CDF BASED ON
C     THE TENTATIVE VALUE OF THE PPF IN THE ITERATION.
C
      Z=DGAMMA
      DEN=1.0D0
  150 IF(Z.GE.10.0D0)GOTO160
      DEN=DEN*Z
      Z=Z+1.0D0
      GOTO150
  160 Z2=Z*Z
      Z3=Z*Z2
      Z4=Z2*Z2
      Z5=Z2*Z3
      A=(Z-0.5D0)*DLOG(Z)-Z+C 
      B=D(1)/Z+D(2)/Z3+D(3)/Z5+D(4)/(Z2*Z5)+D(5)/(Z4*Z5)+
     1D(6)/(Z*Z5*Z5)+D(7)/(Z3*Z5*Z5)+D(8)/(Z5*Z5*Z5)+D(9)/(Z2*Z5*Z5*Z5)
      G=DEXP(A+B)/DEN
C
C     DETERMINE LOWER AND UPPER LIMITS ON THE DESIRED 100P
C     PERCENT POINT.
C
      ILOOP=1
      XMIN0=(DP*DGAMMA*G)**(1.0D0/DGAMMA)
      XMIN=XMIN0
      ICOUNT=1
  350 AI=ICOUNT
      XMAX=AI*XMIN0 
      DX=XMAX
      GOTO1000
  360 IF(PCALC.GE.DP)GOTO370
      XMIN=XMAX
      ICOUNT=ICOUNT+1
      IF(ICOUNT.LE.30000)GOTO350
  370 XMID=(XMIN+XMAX)/2.0D0
C
C     NOW ITERATE BY BISECTION UNTIL THE DESIRED ACCURACY IS ACHIEVED.
C
      ILOOP=2
      XLOWER=XMIN
      XUPPER=XMAX
      ICOUNT=0
  550 DX=XMID
      GOTO1000
  560 IF(PCALC.EQ.DP)GOTO570
      IF(PCALC.GT.DP)GOTO580
      XLOWER=XMID
      XMID=(XMID+XUPPER)/2.0D0
      GOTO590
  580 XUPPER=XMID
      XMID=(XMID+XLOWER)/2.0D0
  590 XDEL=XMID-XLOWER
      IF(XDEL.LT.0.0D0)XDEL=-XDEL
      ICOUNT=ICOUNT+1
      IF(XDEL.LT.0.0000000001D0.OR.ICOUNT.GT.100)GOTO570
      GOTO550
  570 PPF=XMID
      RETURN
C
C******************************************************************** 
C     THIS SECTION BELOW IS LOGICALLY SEPARATE FROM THE ABOVE.
C     THIS SECTION COMPUTES A CDF VALUE FOR ANY GIVEN TENTATIVE
C     PERCENT POINT X VALUE AS DEFINED IN EITHER OF THE 2
C     ITERATION LOOPS IN THE ABOVE CODE.
C
C     COMPUTE T-SUB-Q AS DEFINED ON PAGE 4 OF THE WILK, GNANADESIKAN, 
C     AND HUYETT REFERENCE
C
 1000 SUM=1.0D0/DGAMMA
      TERM=1.0D0/DGAMMA
      CUT1=DX-DGAMMA
      CUT2=DX*10000000000.0D0 
      DO 700 J=1,MAXIT
      AJ=J
      TERM=DX*TERM/(DGAMMA+AJ)
      SUM=SUM+TERM
      CUTOFF=CUT1+(CUT2*TERM/SUM)
      IF(AJ.GT.CUTOFF)GOTO750 
  700 CONTINUE
      WRITE(IPR,705)MAXIT
      WRITE(IPR,706)P
      WRITE(IPR,707)GAMMA
      WRITE(IPR,708)
      PPF=0.0
      RETURN
C
  750 T=SUM
      PCALC=(DX**DGAMMA)*(DEXP(-DX))*T/G
      IF(ILOOP.EQ.1)GOTO360
      GOTO560
C
  705 FORMAT(1H ,48H*****ERROR IN INTERNAL OPERATIONS IN THE GAMPPF , 
     1 45HSUBROUTINE--THE NUMBER OF ITERATIONS EXCEEDS ,I7) 
  706 FORMAT(1H ,33H     THE INPUT VALUE OF P     IS ,E15.8)
  707 FORMAT(1H ,33H     THE INPUT VALUE OF GAMMA IS ,E15.8)
  708 FORMAT(1H ,48H     THE OUTPUT VALUE OF PPF HAS BEEN SET TO 0.0) 
C
      END 
C********************************************************************
*GAMS : Fullsource of GAMMA in SPECFUN from NETLIB
* ======================================================================
* NIST Guide to Available Math Software.
* Fullsource for module GAMMA from package SPECFUN.
* Retrieved from NETLIB on Sat Apr 15 16:11:20 2000.
* ======================================================================
      DOUBLE PRECISION FUNCTION GAMMA(X)
C----------------------------------------------------------------------
C
C This routine calculates the GAMMA function for a real argument X.
C   Computation is based on an algorithm outlined in reference 1.
C   The program uses rational functions that approximate the GAMMA
C   function to at least 20 significant decimal digits.  Coefficients
C   for the approximation over the interval (1,2) are unpublished.
C   Those for the approximation for X .GE. 12 are from reference 2.
C   The accuracy achieved depends on the arithmetic system, the
C   compiler, the intrinsic functions, and proper selection of the
C   machine-dependent constants.
C
C
C*******************************************************************
C*******************************************************************
C
C Explanation of machine-dependent constants
C
C beta   - radix for the floating-point representation
C maxexp - the smallest positive power of beta that overflows
C XBIG   - the largest argument for which GAMMA(X) is representable
C          in the machine, i.e., the solution to the equation
C                  GAMMA(XBIG) = beta**maxexp
C XINF   - the largest machine representable floating-point number;
C          approximately beta**maxexp
C EPS    - the smallest positive floating-point number such that
C          1.0+EPS .GT. 1.0
C XMININ - the smallest positive floating-point number such that
C          1/XMININ is machine representable
C
C     Approximate values for some important machines are:
C
C                            beta       maxexp        XBIG
C
C CRAY-1         (S.P.)        2         8191        966.961
C Cyber 180/855
C   under NOS    (S.P.)        2         1070        177.803
C IEEE (IBM/XT,
C   SUN, etc.)   (S.P.)        2          128        35.040
C IEEE (IBM/XT,
C   SUN, etc.)   (D.P.)        2         1024        171.624
C IBM 3033       (D.P.)       16           63        57.574
C VAX D-Format   (D.P.)        2          127        34.844
C VAX G-Format   (D.P.)        2         1023        171.489
C
C                            XINF         EPS        XMININ
C
C CRAY-1         (S.P.)   5.45E+2465   7.11E-15    1.84E-2466
C Cyber 180/855
C   under NOS    (S.P.)   1.26E+322    3.55E-15    3.14E-294
C IEEE (IBM/XT,
C   SUN, etc.)   (S.P.)   3.40E+38     1.19E-7     1.18E-38
C IEEE (IBM/XT,
C   SUN, etc.)   (D.P.)   1.79D+308    2.22D-16    2.23D-308
C IBM 3033       (D.P.)   7.23D+75     2.22D-16    1.39D-76
C VAX D-Format   (D.P.)   1.70D+38     1.39D-17    5.88D-39
C VAX G-Format   (D.P.)   8.98D+307    1.11D-16    1.12D-308
C
C*******************************************************************
C*******************************************************************
C
C Error returns
C
C  The program returns the value XINF for singularities or
C     when overflow would occur.  The computation is believed
C     to be free of underflow and overflow.
C
C
C  Intrinsic functions required are:
C
C     INT, DBLE, EXP, LOG, REAL, SIN
C
C
C References: "An Overview of Software Development for Special
C              Functions", W. J. Cody, Lecture Notes in Mathematics,
C              506, Numerical Analysis Dundee, 1975, G. A. Watson
C              (ed.), Springer Verlag, Berlin, 1976.
C
C              Computer Approximations, Hart, Et. Al., Wiley and
C              sons, New York, 1968.
C
C  Latest modification: October 12, 1989
C
C  Authors: W. J. Cody and L. Stoltz
C           Applied Mathematics Division
C           Argonne National Laboratory
C           Argonne, IL 60439
C
C----------------------------------------------------------------------
      INTEGER I,N
      LOGICAL PARITY
      DOUBLE PRECISION 
     1    C,CONV,EPS,FACT,HALF,ONE,P,PI,Q,RES,SQRTPI,SUM,TWELVE,
     2    TWO,X,XBIG,XDEN,XINF,XMININ,XNUM,Y,Y1,YSQ,Z,ZERO
      DIMENSION C(7),P(8),Q(8)
C----------------------------------------------------------------------
C  Mathematical constants
C----------------------------------------------------------------------
      DATA ONE,HALF,TWELVE,TWO,ZERO/1.0D0,0.5D0,12.0D0,2.0D0,0.0D0/,
     1     SQRTPI/0.9189385332046727417803297D0/,
     2     PI/3.1415926535897932384626434D0/
C----------------------------------------------------------------------
C  Machine dependent parameters
C----------------------------------------------------------------------
      DATA XBIG,XMININ,EPS/171.624D0,2.23D-308,2.22D-16/,
     1     XINF/1.79D308/
C----------------------------------------------------------------------
C  Numerator and denominator coefficients for rational minimax
C     approximation over (1,2).
C----------------------------------------------------------------------
      DATA P/-1.71618513886549492533811D+0,2.47656508055759199108314D+1,
     1       -3.79804256470945635097577D+2,6.29331155312818442661052D+2,
     2       8.66966202790413211295064D+2,-3.14512729688483675254357D+4,
     3       -3.61444134186911729807069D+4,6.64561438202405440627855D+4/
      DATA Q/-3.08402300119738975254353D+1,3.15350626979604161529144D+2,
     1      -1.01515636749021914166146D+3,-3.10777167157231109440444D+3,
     2        2.25381184209801510330112D+4,4.75584627752788110767815D+3,
     3      -1.34659959864969306392456D+5,-1.15132259675553483497211D+5/
C----------------------------------------------------------------------
C  Coefficients for minimax approximation over (12, INF).
C----------------------------------------------------------------------
      DATA C/-1.910444077728D-03,8.4171387781295D-04,
     1     -5.952379913043012D-04,7.93650793500350248D-04,
     2     -2.777777777777681622553D-03,8.333333333333333331554247D-02,
     3      5.7083835261D-03/
C----------------------------------------------------------------------
C  Statement functions for conversion between integer and float
C----------------------------------------------------------------------
      CONV(I) = DBLE(I)
      PARITY = .FALSE.
      FACT = ONE
      N = 0
      Y = X
      IF (Y .LE. ZERO) THEN
C----------------------------------------------------------------------
C  Argument is negative
C----------------------------------------------------------------------
            Y = -X
            Y1 = AINT(Y)
            RES = Y - Y1
            IF (RES .NE. ZERO) THEN
                  IF (Y1 .NE. AINT(Y1*HALF)*TWO) PARITY = .TRUE.
                  FACT = -PI / SIN(PI*RES)
                  Y = Y + ONE
               ELSE
                  RES = XINF
                  GO TO 900
            END IF
      END IF
C----------------------------------------------------------------------
C  Argument is positive
C----------------------------------------------------------------------
      IF (Y .LT. EPS) THEN
C----------------------------------------------------------------------
C  Argument .LT. EPS
C----------------------------------------------------------------------
            IF (Y .GE. XMININ) THEN
                  RES = ONE / Y
               ELSE
                  RES = XINF
                  GO TO 900
            END IF
         ELSE IF (Y .LT. TWELVE) THEN
            Y1 = Y
            IF (Y .LT. ONE) THEN
C----------------------------------------------------------------------
C  0.0 .LT. argument .LT. 1.0
C----------------------------------------------------------------------
                  Z = Y
                  Y = Y + ONE
               ELSE
C----------------------------------------------------------------------
C  1.0 .LT. argument .LT. 12.0, reduce argument if necessary
C----------------------------------------------------------------------
                  N = INT(Y) - 1
                  Y = Y - CONV(N)
                  Z = Y - ONE
            END IF
C----------------------------------------------------------------------
C  Evaluate approximation for 1.0 .LT. argument .LT. 2.0
C----------------------------------------------------------------------
            XNUM = ZERO
            XDEN = ONE
            DO 260 I = 1, 8
               XNUM = (XNUM + P(I)) * Z
               XDEN = XDEN * Z + Q(I)
  260       CONTINUE
            RES = XNUM / XDEN + ONE
            IF (Y1 .LT. Y) THEN
C----------------------------------------------------------------------
C  Adjust result for case  0.0 .LT. argument .LT. 1.0
C----------------------------------------------------------------------
                  RES = RES / Y1
               ELSE IF (Y1 .GT. Y) THEN
C----------------------------------------------------------------------
C  Adjust result for case  2.0 .LT. argument .LT. 12.0
C----------------------------------------------------------------------
                  DO 290 I = 1, N
                     RES = RES * Y
                     Y = Y + ONE
  290             CONTINUE
            END IF
         ELSE
C----------------------------------------------------------------------
C  Evaluate for argument .GE. 12.0,
C----------------------------------------------------------------------
            IF (Y .LE. XBIG) THEN
                  YSQ = Y * Y
                  SUM = C(7)
                  DO 350 I = 1, 6
                     SUM = SUM / YSQ + C(I)
  350             CONTINUE
                  SUM = SUM/Y - Y + SQRTPI
                  SUM = SUM + (Y-HALF)*LOG(Y)
                  RES = EXP(SUM)
               ELSE
                  RES = XINF
                  GO TO 900
            END IF
      END IF
C----------------------------------------------------------------------
C  Final adjustments and return
C----------------------------------------------------------------------
      IF (PARITY) RES = -RES
      IF (FACT .NE. ONE) RES = FACT / RES
  900 GAMMA = RES
      RETURN
C ---------- Last line of GAMMA ----------
      END



