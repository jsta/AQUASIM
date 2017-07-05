////////////////////////////////   intpol.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    29.12.91    Peter Reichert
// revisions:   05.11.98    Peter Reichert    CARDLONG introduced
//
//////////////////////////////////////////////////////////////////////////////

#ifndef INTPOL_H
#define INTPOL_H

#ifndef USE_TYPES_H
#include "all.h"
#else
#include "types.h"
#endif

//////////////////////////////////////////////////////////////////////////////

CARDINAL Index(const REAL xa[], CARDINAL n, REAL x, CARDINAL* last=0);
CARDLONG Index(const REAL xa[], CARDLONG n, REAL x, CARDLONG* last=0);

//////////////////////////////////////////////////////////////////////////////

REAL LinInt(const REAL xa[], const REAL ya[], CARDINAL n,
            REAL x, CARDINAL* last=0);
REAL LinInt(const REAL xa[], const REAL ya[], CARDLONG n,
            REAL x, CARDLONG* last=0);

void SplineInit(const REAL xa[], const REAL ya[], CARDINAL n, REAL yaxx[]);
REAL Spline(const REAL xa[], const REAL ya[], const REAL yaxx[], CARDINAL n,
            REAL x, CARDINAL* last=0);
void SplineInit(const REAL xa[], const REAL ya[], CARDLONG n, REAL yaxx[]);
REAL Spline(const REAL xa[], const REAL ya[], const REAL yaxx[], CARDLONG n,
            REAL x, CARDLONG* last=0);

REAL Smooth(const REAL xa[], const REAL ya[], CARDINAL n, REAL x,
            REAL width, CARDINAL* last=0, const REAL* sya=0);
REAL Smooth(const REAL xa[], const REAL ya[], CARDLONG n, REAL x,
            REAL width, CARDLONG* last=0, const REAL* sya=0);

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////

