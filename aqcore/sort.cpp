//////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include "sort.h"

//////////////////////////////////////////////////////////////////////////////


void sort(CARDINAL n, REAL* array, CARDINAL* indices)
{
   CARDINAL i,j;

   // initialize indices:
   for ( j=1; j<=n; j++ ) indices[j-1] = j-1;

   if ( n < 2 ) return;

   CARDINAL l = n/2+1;
   CARDINAL ir = n;
   CARDINAL indxt;
   REAL     q;

   while ( 1 )
   {
      if ( l > 1 )
      {
         l--;
         indxt = indices[l-1];
         q = array[indxt];
      }
      else
      {
         indxt = indices[ir-1];
         q = array[indxt];
         indices[ir-1] = indices[0];
         ir--;
         if ( ir == 1 ) 
         {
            indices[0] = indxt;
            return;
         }
      }
      i = l;
      j = l+l;
      while ( j <= ir )
      {
         if ( j < ir )
         {
            if ( array[indices[j-1]] < array[indices[j]] ) j++;
         }
         if ( q < array[indices[j-1]] )
         {
            indices[i-1] = indices[j-1];
            i = j;
            j = j+j;
         }
         else
         {
            j = ir+1;
         }
      }
      indices[i-1] = indxt;
   }

   return;
}


//////////////////////////////////////////////////////////////////////////////


void sort(CARDLONG n, REAL* array, CARDLONG* indices)
{
   CARDLONG i,j;

   // initialize indices:
   for ( j=1; j<=n; j++ ) indices[j-1] = j-1;

   if ( n < 2 ) return;

   CARDLONG l = n/2+1;
   CARDLONG ir = n;
   CARDLONG indxt;
   REAL     q;

   while ( 1 )
   {
      if ( l > 1 )
      {
         l--;
         indxt = indices[l-1];
         q = array[indxt];
      }
      else
      {
         indxt = indices[ir-1];
         q = array[indxt];
         indices[ir-1] = indices[0];
         ir--;
         if ( ir == 1 ) 
         {
            indices[0] = indxt;
            return;
         }
      }
      i = l;
      j = l+l;
      while ( j <= ir )
      {
         if ( j < ir )
         {
            if ( array[indices[j-1]] < array[indices[j]] ) j++;
         }
         if ( q < array[indices[j-1]] )
         {
            indices[i-1] = indices[j-1];
            i = j;
            j = j+j;
         }
         else
         {
            j = ir+1;
         }
      }
      indices[i-1] = indxt;
   }

   return;
}


//////////////////////////////////////////////////////////////////////////////

