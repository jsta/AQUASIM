//////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <math.h>

#include "mdi.h"

//////////////////////////////////////////////////////////////////////////////


MultiDimIndex::MultiDimIndex(CARDLONG d, CARDLONG* s)
{
   dim = d;
   if ( dim > 0 )
   {
      sizes = new CARDLONG[dim];
      index = new CARDLONG[dim];
      for ( CARDLONG i=0; i<dim; i++ )
      {
         sizes[i] = s[i];
         if ( sizes[i] < 1 ) sizes[i] = 1;
      }
   }
   else
   {
      sizes = 0; index = 0;
   }
   ResetIndex();
}


MultiDimIndex::MultiDimIndex(CARDLONG d, CARDLONG s)
{
   dim = d;
   if ( dim > 0 )
   {
      sizes = new CARDLONG[dim];
      index = new CARDLONG[dim];
      for ( CARDLONG i=0; i<dim; i++ )
      {
         sizes[i] = s;
         if ( sizes[i] < 1 ) sizes[i] = 1;
      }
   }
   else
   {
      sizes = 0; index = 0;
   }
   ResetIndex();
}


MultiDimIndex::~MultiDimIndex()
{
   delete sizes;
   delete index;
}


CARDLONG MultiDimIndex::Size(CARDLONG i) const
{
   if ( i >= dim ) return 0;
   return sizes[i];
}


CARDLONG MultiDimIndex::CurrentIndex(CARDLONG i) const
{
   if ( i >= dim ) return 0;
   return index[i];
}


CARDLONG MultiDimIndex::OneDimSize() const
{
   if ( dim == 0 ) return 0;
   CARDLONG onedims = sizes[0];
   for ( CARDLONG i=1; i<dim; i++ ) onedims *= sizes[i];
   return onedims;
}


CARDLONG MultiDimIndex::OneDimIndex(const CARDLONG* ind) const
{
   if ( dim == 0 ) return 0;
   CARDLONG onedimind = 0;
   CARDLONG offset = 1;
   for ( CARDLONG i=0; i<dim; i++ )
   {
      CARDLONG k = dim - i - 1;
      onedimind += (ind[k]-1)*offset;
      offset    = offset*sizes[k];
   }
   return onedimind;
}


void MultiDimIndex::ResetIndex()
{
   if ( dim == 0 ) return;
   for ( CARDLONG i=0; i<dim; i++ )
   {
      index[i] = 1;
   }
   mark = dim-1;
}


BOOLEAN MultiDimIndex::IncrementIndex()
{
   if ( dim == 0 ) return FALSE;
   do
   {
      if ( index[mark] == sizes[mark] )
      {
         if ( mark == 0 ) return FALSE;
         index[mark] = 1;
         mark--;
      }
      else
      {
         index[mark]++;
         mark = dim-1;
      }
   } while ( mark < dim-1 );
   return TRUE;
}


//////////////////////////////////////////////////////////////////////////////

