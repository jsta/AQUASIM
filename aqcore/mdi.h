//////////////////////////////////////////////////////////////////////////////

#ifndef MDI_H
#define MDI_H

//////////////////////////////////////////////////////////////////////////////

#include <limits.h>

#ifndef USE_TYPES_H
#include "all.h"
#else
#include "types.h"
#endif

//////////////////////////////////////////////////////////////////////////////


class MultiDimIndex
{
   public:

      MultiDimIndex(CARDLONG d, CARDLONG* s);  // d: dimension, 
      MultiDimIndex(CARDLONG d, CARDLONG s);   // s: size or array of sizes 
      ~MultiDimIndex();

      CARDLONG Dim() const                     // returns dimension
         { return dim; }
      CARDLONG Size(CARDLONG i) const;         // returns size(i)
      CARDLONG CurrentIndex(CARDLONG i) const; // returns current index(i)
                                               // (between 1 and size(i))
      const CARDLONG* CurrentIndex() const     // returns current index as
         { return index; }                     // an array

      CARDLONG OneDimSize() const;             // returns size of 1d index
      CARDLONG OneDimIndex(const CARDLONG* ind) const;  
                                               // returns zero-based 1d index

      void ResetIndex();                       // resets index to 1,1,...,1
      BOOLEAN IncrementIndex();                // increments index if not at
                                               // size,size,...,size
   protected:

      CARDLONG  dim;
      CARDLONG* sizes;
      CARDLONG* index;
      CARDLONG  mark;
};


//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
