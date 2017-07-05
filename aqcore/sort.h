//////////////////////////////////////////////////////////////////////////////

#ifndef AQSORT_H
#define AQSORT_H

//////////////////////////////////////////////////////////////////////////////

#ifndef USE_TYPES_H
#include "all.h"
#else
#include "types.h"
#endif

//////////////////////////////////////////////////////////////////////////////


void sort(CARDINAL n, REAL* array, CARDINAL* indices);

// Implementation of the heap sort algorithm according to
// Press, W.H., Flannery, B.P., Teukolsky, S.A. and Vetterling, W.T.
// Numerical Recipes - The Art of Scientific Computing
// Cambridge University Press

// Parameters:
//
// Input:
// n        dimension of the array
// array    one dimensional array of REAL numbers array[0] ... array[n-1]
// indices  CARDINAL array of dimension n, arbitrary content
//
// Output:
// n        unchanged
// array    unchanged
// indices  contains an index transformation such that array[indices[0]]
//          to array[indices[n-1]] is ascending


//////////////////////////////////////////////////////////////////////////////


void sort(CARDLONG n, REAL* array, CARDLONG* indices);

// Implementation of the heap sort algorithm according to
// Press, W.H., Flannery, B.P., Teukolsky, S.A. and Vetterling, W.T.
// Numerical Recipes - The Art of Scientific Computing
// Cambridge University Press

// Parameters:
//
// Input:
// n        dimension of the array
// array    one dimensional array of REAL numbers array[0] ... array[n-1]
// indices  CARDLONG array of dimension n, arbitrary content
//
// Output:
// n        unchanged
// array    unchanged
// indices  contains an index transformation such that array[indices[0]]
//          to array[indices[n-1]] is ascending


//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
