//////////////////////////////////////////////////////////////////////////////

#ifndef COMBIN_H
#define COMBIN_H

//////////////////////////////////////////////////////////////////////////////

#include <limits.h>

#ifndef USE_TYPES_H
#include "all.h"
#else
#include "types.h"
#endif

//////////////////////////////////////////////////////////////////////////////


class Combin
{
   public:

      Combin(CARDLONG n, CARDLONG p);          // n: set size,
      ~Combin();                               // p: subset size

      CARDLONG SetSize() const                 // returns dimension
         { return setsize; }
      CARDLONG SubsetSize() const              // returns dimension
         { return subsetsize; }
      CARDLONG CurrentIndex(CARDLONG i) const; // returns current index(i)
                                               // (between 1 and n)
      const CARDLONG* CurrentIndex() const     // returns current index as
         { return index; }                     // an array
      CARDLONG NumComb() const;                // returns number of comb.

      void ResetIndex();                       // resets index to 1,2,...,p
      BOOLEAN IncrementIndex();                // increments index if not at
                                               // n-p+1,n-p+2,...,n
   protected:

      CARDLONG  setsize;
      CARDLONG  subsetsize;
      CARDLONG* index;
      CARDLONG  mark;
};


//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
