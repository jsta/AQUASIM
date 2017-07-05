////////////////////////////////   intpol.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    29.12.91    Peter Reichert
// revisions:   05.11.98    Peter Reichert    CARDLONG introduced
//
//////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "intpol.h"

//////////////////////////////////////////////////////////////////////////////

// Determination of relevant index.

CARDINAL Index(const REAL xa[], CARDINAL n, REAL x, CARDINAL* last)
{
   if ( n == 0 ) return 0;
   INTEGER low = 0;
   if ( n > 1 )
   {
      BOOLEAN bisection = TRUE;
      if ( last != 0 )
      {
         if ( *last < n ) bisection = FALSE;
      }
      if ( bisection == TRUE )           // bisection
      {
         INTEGER high = n-1;
         while ( high-low > 1 )
         {
            INTEGER i = (high+low)/2;
            if ( x < xa[i] ) high = i;
            else             low  = i;
         }
      }
      else                               // look for neighbours
      {
         low = *last;
         if ( low >= n-1 ) low = n-2;
         if ( x < xa[low] )
         {
            while ( x < xa[low] )
            {
               if ( low == 0 ) break;
               low--;
            }
         }
         else
         {
            while ( x >= xa[low+1] )
            {
               low++;
               if ( low == n-1 ) break;
            }
         }
      }
   }
   if ( last != 0 ) *last = low;
   return (CARDINAL)low;
}

CARDLONG Index(const REAL xa[], CARDLONG n, REAL x, CARDLONG* last)
{
   if ( n == 0 ) return 0;
   INTLONG low = 0;
   if ( n > 1 )
   {
      BOOLEAN bisection = TRUE;
      if ( last != 0 )
      {
         if ( *last < n ) bisection = FALSE;
      }
      if ( bisection == TRUE )           // bisection
      {
         INTLONG high = n-1;
         while ( high-low > 1 )
         {
            INTLONG i = (high+low)/2;
            if ( x < xa[i] ) high = i;
            else             low  = i;
         }
      }
      else                               // look for neighbours
      {
         low = *last;
         if ( low >= n-1 ) low = n-2;
         if ( x < xa[low] )
         {
            while ( x < xa[low] )
            {
               if ( low == 0 ) break;
               low--;
            }
         }
         else
         {
            while ( x >= xa[low+1] )
            {
               low++;
               if ( low == n-1 ) break;
            }
         }
      }
   }
   if ( last != 0 ) *last = low;
   return (CARDLONG)low;
}

//////////////////////////////////////////////////////////////////////////////

// Linear interpolation.

REAL LinInt(const REAL xa[], const REAL ya[], CARDINAL n,
            REAL x, CARDINAL* last)
{
   if ( n == 0 ) return 0.0;
   if ( n == 1 ) return ya[0];

   if ( x <= xa[0] )   return ya[0];
   if ( x >= xa[n-1] ) return ya[n-1];

   CARDINAL low  = Index(xa,n,x,last);
   CARDINAL high = low+1;

   REAL     a    = (xa[high]-x)/(xa[high]-xa[low]);
   REAL     b    = (x-xa[low])/(xa[high]-xa[low]);
   return a*ya[low] + b*ya[high];
}

REAL LinInt(const REAL xa[], const REAL ya[], CARDLONG n,
            REAL x, CARDLONG* last)
{
   if ( n == 0 ) return 0.0;
   if ( n == 1 ) return ya[0];

   if ( x <= xa[0] )   return ya[0];
   if ( x >= xa[n-1] ) return ya[n-1];

   CARDLONG low  = Index(xa,n,x,last);
   CARDLONG high = low+1;

   REAL     a    = (xa[high]-x)/(xa[high]-xa[low]);
   REAL     b    = (x-xa[low])/(xa[high]-xa[low]);
   return a*ya[low] + b*ya[high];
}

//////////////////////////////////////////////////////////////////////////////

// Cubic spline interpolation according to Press, Flannery, Teukolsky and
// Vetterling: Numerical Recipes, Cambridge University Press 1989.

void SplineInit(const REAL xa[], const REAL ya[], CARDINAL n, REAL yaxx[])
{
   INTEGER i;
   REAL* u = new REAL[n];

   u[0] = 3.0/(xa[1]-xa[0])*(ya[1]-ya[0])/(xa[1]-xa[0]);
   yaxx[0] = -0.5;

   for ( i=1; i<n-1; i++ )
   {
      REAL sig = (xa[i]-xa[i-1])/(xa[i+1]-xa[i-1]);
      REAL p   = sig*yaxx[i-1]+2.0;
      yaxx[i]  = (sig-1.0)/p;
      u[i]     = ( 6.0*( (ya[i+1]-ya[i])/(xa[i+1]-xa[i]) 
                        -(ya[i]-ya[i-1])/(xa[i]-xa[i-1]) )
                      /(xa[i+1]-xa[i-1])
                  -sig*u[i-1] )
                 /p;
   }

   u[n-1]    = -3.0/(xa[n-1]-xa[n-2])*(ya[n-1]-ya[n-2])/(xa[n-1]-xa[n-2]);
   yaxx[n-1] = (u[n-1]-0.5*u[n-2])/(0.5*yaxx[n-2]+1.0);

   for ( i=0; i<n-1; i++ )
   {
      yaxx[n-i-2] = yaxx[n-i-2]*yaxx[n-i-1]+u[n-i-2];
   }

   delete u;
   return;
}


REAL Spline(const REAL xa[], const REAL ya[], const REAL yaxx[], CARDINAL n,
            REAL x, CARDINAL* last)
{
   if ( n == 0 ) return 0.0;
   if ( n == 1 ) return ya[0];

   if ( x <= xa[0] )   return ya[0];
   if ( x >= xa[n-1] ) return ya[n-1];

   CARDINAL low  = Index(xa,n,x,last);
   CARDINAL high = low+1;

   REAL     h    = xa[high]-xa[low];
   REAL     a    = (xa[high]-x)/h;
   REAL     b    = (x-xa[low])/h;
   return   a*ya[low]+b*ya[high]
            +( a*(a*a-1.0)*yaxx[low] + b*(b*b-1.0)*yaxx[high] ) * h*h/6.0;
}


void SplineInit(const REAL xa[], const REAL ya[], CARDLONG n, REAL yaxx[])
{
   INTLONG i;
   REAL* u = new REAL[n];

   u[0] = 3.0/(xa[1]-xa[0])*(ya[1]-ya[0])/(xa[1]-xa[0]);
   yaxx[0] = -0.5;

   for ( i=1; i<n-1; i++ )
   {
      REAL sig = (xa[i]-xa[i-1])/(xa[i+1]-xa[i-1]);
      REAL p   = sig*yaxx[i-1]+2.0;
      yaxx[i]  = (sig-1.0)/p;
      u[i]     = ( 6.0*( (ya[i+1]-ya[i])/(xa[i+1]-xa[i]) 
                        -(ya[i]-ya[i-1])/(xa[i]-xa[i-1]) )
                      /(xa[i+1]-xa[i-1])
                  -sig*u[i-1] )
                 /p;
   }

   u[n-1]    = -3.0/(xa[n-1]-xa[n-2])*(ya[n-1]-ya[n-2])/(xa[n-1]-xa[n-2]);
   yaxx[n-1] = (u[n-1]-0.5*u[n-2])/(0.5*yaxx[n-2]+1.0);

   for ( i=0; i<n-1; i++ )
   {
      yaxx[n-i-2] = yaxx[n-i-2]*yaxx[n-i-1]+u[n-i-2];
   }

   delete u;
   return;
}


REAL Spline(const REAL xa[], const REAL ya[], const REAL yaxx[], CARDLONG n,
            REAL x, CARDLONG* last)
{
   if ( n == 0 ) return 0.0;
   if ( n == 1 ) return ya[0];

   if ( x <= xa[0] )   return ya[0];
   if ( x >= xa[n-1] ) return ya[n-1];

   CARDLONG low  = Index(xa,n,x,last);
   CARDLONG high = low+1;

   REAL     h    = xa[high]-xa[low];
   REAL     a    = (xa[high]-x)/h;
   REAL     b    = (x-xa[low])/h;
   return   a*ya[low]+b*ya[high]
            +( a*(a*a-1.0)*yaxx[low] + b*(b*b-1.0)*yaxx[high] ) * h*h/6.0;
}


//////////////////////////////////////////////////////////////////////////////

// Smoothing by local fit of a parabola to data points weighted by a normal
// distribution of given width (standard deviation). If points are separated
// by more than the standard deviation of the normal distribution, additional
// linearly interpolated points are inserted.
//
// Fit of parabola according to Bevington: Data Reduction and Error Analysis
// for the physical sciences, McGraw-Hill 1969 p. 136.

REAL Smooth(const REAL xa[], const REAL ya[], CARDINAL n, REAL x,
            REAL width, CARDINAL* last, const REAL* sya)
{
   const CARDINAL numwidth = 5;
   REAL           dist     = numwidth*width;

   if ( n == 0 ) return 0.0;
   if ( n == 1 ) return ya[0];

   if ( x <= xa[0]  -dist ) return ya[0];
   if ( x >= xa[n-1]+dist ) return ya[n-1];

   INTEGER index = Index(xa,n,x,last);
   INTEGER N = 0;
   while ( xa[index]>x-dist && index>0 ) index--;
   INTEGER i = index;
   if ( i == 0 )
   {
      INTEGER k = (INTEGER) ( (xa[0]-(x-dist))/width );
      if ( k < 0 ) k = 0;
      N += k + 1;
   }
   while ( xa[i]<x+dist && i<n-1 )
   {
      INTEGER k = (INTEGER) ( (xa[i+1]-xa[i])/width );
      N += k + 1;
      i++;
   }
   if ( i == n-1 )
   {
      INTEGER k = (INTEGER) ( ((x+dist)-xa[i])/width );
      if ( k < 0 ) k = 0;
      N += k;
   }
   if ( N < 2*numwidth )
   {
#ifndef USE_TYPES_H
      FatalError("Smooth: error calculating number of data points");
#else
      exit(1);
#endif
   }
   REAL* XA = new REAL[N];
   REAL* YA = new REAL[N];
   REAL* SYA = 0; if ( sya != 0 ) SYA = new REAL[N];
   INTEGER j = 0;
   i = index;
   if ( i == 0 )
   {
      INTEGER k = (INTEGER) ( (xa[0]-(x-dist))/width );
      for ( INTEGER m=0; m<k; m++ )
      {
         XA[j] = xa[0]-(k-m)*width;
         YA[j] = ya[0];
         if ( SYA != 0 ) SYA[j] = sya[0];
         j++;
      }
      XA[j] = xa[0];
      YA[j] = ya[0];
      if ( SYA != 0 ) SYA[j] = sya[0];
      j++;
   }
   while ( xa[i]<x+dist && i<n-1 )
   {
      INTEGER k = (INTEGER) ( (xa[i+1]-xa[i])/width );
      for ( INTEGER m=0; m<k; m++ )
      {
         XA[j] = xa[i] + (m+1)*(xa[i+1]-xa[i])/(k+1);
         YA[j] = ((XA[j]-xa[i])*ya[i+1]+(xa[i+1]-XA[j])*ya[i])
                 /(xa[i+1]-xa[i]);
         if ( SYA != 0 ) 
            SYA[j] = ((XA[j]-xa[i])*sya[i+1]+(xa[i+1]-XA[j])*sya[i])
                     /(xa[i+1]-xa[i]);
         j++;
      }
      XA[j] = xa[i+1];
      YA[j] = ya[i+1];
      if ( SYA != 0 ) SYA[j] = sya[i+1];
      j++;
      i++;
   }
   if ( i == n-1 )
   {
      INTEGER k = (INTEGER) ( ((x+dist)-xa[i])/width );
      for ( INTEGER m=0; m<k; m++ )
      {
         XA[j] = xa[n-1]+(m+1)*width;
         YA[j] = ya[n-1];
         if ( SYA != 0 ) SYA[j] = sya[n-1];
         j++;
      }
   }
   if ( j != N )
   {
#ifndef USE_TYPES_H
      FatalError("Smooth: inconsistent number of data points");
#else
      exit(1);
#endif
   }

   REAL W, X, XX, XXX, XXXX, Y, XY, XXY;
   W=0.0; X=0.0; XX=0.0; XXX=0.0; XXXX=0.0; Y=0.0; XY=0.0; XXY=0.0;
   for ( i=0; i<N; i++ )
   {
      REAL w = exp(-(XA[i]-x)*(XA[i]-x)/(2.0*width*width));
      if ( SYA != 0 ) w = w/(SYA[i]*SYA[i]);
      W    += w;
      X    += w * (XA[i]-x);
      XX   += w * (XA[i]-x) * (XA[i]-x);
      XXX  += w * (XA[i]-x) * (XA[i]-x) * (XA[i]-x);
      XXXX += w * (XA[i]-x) * (XA[i]-x) * (XA[i]-x) * (XA[i]-x);
      Y    += w * YA[i];
      XY   += w * (XA[i]-x) * YA[i];
      XXY  += w * (XA[i]-x) * (XA[i]-x) * YA[i];
   }
   REAL d =   W  * ( XX * XXXX  -  XXX * XXX )
            - X  * ( X  * XXXX  -  XX  * XXX )
            + XX * ( X  * XXX   -  XX  * XX  );
   REAL y = 1.0/d  *  ( + Y   * ( XX * XXXX  -  XXX * XXX )
                        - XY  * ( X  * XXXX  -  XX  * XXX )
                        + XXY * ( X  * XXX   -  XX  * XX  )  );

   delete [] XA;
   delete [] YA;
   delete [] SYA;
   return y;
}

REAL Smooth(const REAL xa[], const REAL ya[], CARDLONG n, REAL x,
            REAL width, CARDLONG* last, const REAL* sya)
{
   const CARDINAL numwidth = 5;
   REAL           dist     = numwidth*width;

   if ( n == 0 ) return 0.0;
   if ( n == 1 ) return ya[0];

   if ( x <= xa[0]  -dist ) return ya[0];
   if ( x >= xa[n-1]+dist ) return ya[n-1];

   INTLONG index = Index(xa,n,x,last);
   INTLONG N = 0;
   while ( xa[index]>x-dist && index>0 ) index--;
   INTLONG i = index;
   if ( i == 0 )
   {
      INTLONG k = (INTLONG) ( (xa[0]-(x-dist))/width );
      if ( k < 0 ) k = 0;
      N += k + 1;
   }
   while ( xa[i]<x+dist && i<n-1 )
   {
      INTLONG k = (INTLONG) ( (xa[i+1]-xa[i])/width );
      N += k + 1;
      i++;
   }
   if ( i == n-1 )
   {
      INTLONG k = (INTLONG) ( ((x+dist)-xa[i])/width );
      if ( k < 0 ) k = 0;
      N += k;
   }
   if ( N < 2*numwidth )
   {
#ifndef USE_TYPES_H
      FatalError("Smooth: error calculating number of data points");
#else
      exit(1);
#endif
   }

   REAL* XA = new REAL[N];
   REAL* YA = new REAL[N];
   REAL* SYA = 0; if ( sya != 0 ) SYA = new REAL[N];
   INTLONG j = 0;
   i = index;
   if ( i == 0 )
   {
      INTLONG k = (INTLONG) ( (xa[0]-(x-dist))/width );
      for ( INTLONG m=0; m<k; m++ )
      {
         XA[j] = xa[0]-(k-m)*width;
         YA[j] = ya[0];
         if ( SYA != 0 ) SYA[j] = sya[0];
         j++;
      }
      XA[j] = xa[0];
      YA[j] = ya[0];
      if ( SYA != 0 ) SYA[j] = sya[0];
      j++;
   }
   while ( xa[i]<x+dist && i<n-1 )
   {
      INTLONG k = (INTLONG) ( (xa[i+1]-xa[i])/width );
      for ( INTLONG m=0; m<k; m++ )
      {
         XA[j] = xa[i] + (m+1)*(xa[i+1]-xa[i])/(k+1);
         YA[j] = ((XA[j]-xa[i])*ya[i+1]+(xa[i+1]-XA[j])*ya[i])
                 /(xa[i+1]-xa[i]);
         if ( SYA != 0 ) 
            SYA[j] = ((XA[j]-xa[i])*sya[i+1]+(xa[i+1]-XA[j])*sya[i])
                     /(xa[i+1]-xa[i]);
         j++;
      }
      XA[j] = xa[i+1];
      YA[j] = ya[i+1];
      if ( SYA != 0 ) SYA[j] = sya[i+1];
      j++;
      i++;
   }
   if ( i == n-1 )
   {
      INTLONG k = (INTLONG) ( ((x+dist)-xa[i])/width );
      for ( INTLONG m=0; m<k; m++ )
      {
         XA[j] = xa[n-1]+(m+1)*width;
         YA[j] = ya[n-1];
         if ( SYA != 0 ) SYA[j] = sya[n-1];
         j++;
      }
   }
   if ( j != N )
   {
#ifndef USE_TYPES_H
      FatalError("Smooth: inconsistent number of data points");
#else
      exit(1);
#endif
   }

   REAL W, X, XX, XXX, XXXX, Y, XY, XXY;
   W=0.0; X=0.0; XX=0.0; XXX=0.0; XXXX=0.0; Y=0.0; XY=0.0; XXY=0.0;
   for ( i=0; i<N; i++ )
   {
      REAL w = exp(-(XA[i]-x)*(XA[i]-x)/(2.0*width*width));
      if ( SYA != 0 ) w = w/(SYA[i]*SYA[i]);
      W    += w;
      X    += w * (XA[i]-x);
      XX   += w * (XA[i]-x) * (XA[i]-x);
      XXX  += w * (XA[i]-x) * (XA[i]-x) * (XA[i]-x);
      XXXX += w * (XA[i]-x) * (XA[i]-x) * (XA[i]-x) * (XA[i]-x);
      Y    += w * YA[i];
      XY   += w * (XA[i]-x) * YA[i];
      XXY  += w * (XA[i]-x) * (XA[i]-x) * YA[i];
   }
   REAL d =   W  * ( XX * XXXX  -  XXX * XXX )
            - X  * ( X  * XXXX  -  XX  * XXX )
            + XX * ( X  * XXX   -  XX  * XX  );
   REAL y = 1.0/d  *  ( + Y   * ( XX * XXXX  -  XXX * XXX )
                        - XY  * ( X  * XXXX  -  XX  * XXX )
                        + XXY * ( X  * XXX   -  XX  * XX  )  );

   delete [] XA;
   delete [] YA;
   delete [] SYA;
   return y;
}

//////////////////////////////////////////////////////////////////////////////

