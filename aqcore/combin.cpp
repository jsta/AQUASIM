//////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <math.h>

#include "combin.h"

//////////////////////////////////////////////////////////////////////////////


Combin::Combin(CARDLONG n, CARDLONG p)
{
   setsize = n;
   if ( n == 0 ) n = 1;
   subsetsize = p;
   if ( p == 0 ) p = 1;
   if ( p > setsize ) p = setsize;
   index = new CARDLONG[subsetsize];
   ResetIndex();
}


Combin::~Combin()
{
   delete [] index;
}


CARDLONG Combin::CurrentIndex(CARDLONG i) const
{
   if ( i >= subsetsize ) return 0;
   return index[i];
}


CARDLONG Combin::NumComb() const
{
   if ( subsetsize == 1 ) return setsize;
   REAL num = 1;
   for ( CARDLONG i=0; i<subsetsize; i++ ) 
   {
      num *= ( REAL(setsize-i) / REAL(i+1) );
   }
   if ( num < (REAL)CARDLONG_MAX ) return (CARDLONG) (num + 0.5);
   return 0;
}


void Combin::ResetIndex()
{
   for ( CARDLONG i=0; i<subsetsize; i++ )
   {
      index[i] = i+1;
   }
   mark = subsetsize-1;
}


BOOLEAN Combin::IncrementIndex()
{
   do
   {
      if ( index[mark] == setsize-subsetsize+mark+1 )
      {
         if ( mark == 0 ) return FALSE;
         mark--;
      }
      else
      {
         index[mark]++;
         for ( CARDLONG i=mark+1; i<subsetsize; i++ )
         {
            index[i] = index[mark] + i - mark;
         }
         mark = subsetsize-1;
      }
   } while ( mark < subsetsize-1 );
   return TRUE;
}


//////////////////////////////////////////////////////////////////////////////

