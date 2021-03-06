///////////////////////////////   readwords.h   //////////////////////////////
//
//              date:       person:           comments:
//
// creation:    11.02.00    Peter Reichert    
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#ifndef READWORDS_H
#define READWORDS_H

//////////////////////////////////////////////////////////////////////////////

#ifndef USE_TYPES_H
#include "all.h"
#else
#include "types.h"
#endif

//////////////////////////////////////////////////////////////////////////////


CARDINAL ReadWords(std::istream& in, char**& words);


void DeleteWords(CARDINAL num, char**& words);


//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
