/////////////////////////////////   types.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    11.02.00    Peter Reichert    type conversions added to 
//                                            pure header file
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#ifndef TYPES_H
#define TYPES_H

//////////////////////////////////////////////////////////////////////////////

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <strstream>

//////////////////////////////////////////////////////////////////////////////


// possible values for AQUA_VERSION:

#define AQUA_BATCH 1
#define AQUA_CHAR  2
#define AQUA_XVT   3
#define AQUA_VCC   4
#define AQUA_MOTIF 5

// possible values for AQUA_OS:

#define AQUA_SUN   1
#define AQUA_NT    2
#define AQUA_MAC   3
#define AQUA_HP    4
#define AQUA_IBM   5
#define AQUA_VMS   6
#define AQUA_DEC   7
#define AQUA_LNX   8


//////////////////////////////////////////////////////////////////////////////


// data types:

typedef unsigned int  CARDINAL;	 // maximum must be defined below
#define CARDINAL_MAX  UINT_MAX

typedef int           INTEGER; 	 // maximum must be defined below
#define INTEGER_MAX   INT_MAX  

typedef unsigned long CARDLONG;  // maximum must be defined below
#define CARDLONG_MAX  ULONG_MAX

typedef long int      INTLONG; 	 // maximum must be defined below
#define INTLONG_MAX   LONG_MAX  

typedef double        REAL;

// define BOOLEAN if neither "xvt.h" nor "stdafx.h" nor "winnt.h" was included:

#if ( AQUA_VERSION!=AQUA_XVT && AQUA_VERSION!=AQUA_VCC && AQUA_OS!=AQUA_WIN )

#if ( AQUA_OS==AQUA_VMS || AQUA_OS==AQUA_IBM || AQUA_OS==AQUA_MAC || AQUA_OS==AQUA_DEC || AQUA_OS==AQUA_LNX )

typedef short BOOLEAN;     // BOOLEAN definition compatible with VMS, IBM, etc.
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  1
#endif

#else

enum BOOLEAN { FALSE, TRUE };  // alternative BOOLEAN definition

#endif

#endif

enum JOBSTATUS { OK, INTERRUPT, PROBLEM };


//////////////////////////////////////////////////////////////////////////////


// type conversions:

void NumToString(CARDINAL num, char* buf, CARDINAL bufsize, CARDINAL right=0);

void NumToString(INTEGER num, char* buf, CARDINAL bufsize, CARDINAL right=0);

void NumToString(CARDLONG num, char* buf, CARDINAL bufsize, CARDINAL right=0);

void NumToString(INTLONG num, char* buf, CARDINAL bufsize, CARDINAL right=0);

void NumToString(REAL num, char* buf, CARDINAL bufsize, CARDINAL right=0,
		 int prec=0);

BOOLEAN StringToNum(char* buf, CARDINAL* num);

BOOLEAN StringToNum(char* buf, INTEGER* num);

BOOLEAN StringToNum(char* buf, CARDLONG* num);

BOOLEAN StringToNum(char* buf, INTLONG* num);

BOOLEAN StringToNum(char* buf, REAL* num);

BOOLEAN TimeToString(const char* form, char* string, CARDINAL size);


//////////////////////////////////////////////////////////////////////////////

// test if number is finite and defined

int aqfinite(REAL x);

//////////////////////////////////////////////////////////////////////////////


// global variables:

extern const REAL aq_pi;

extern std::ostream* cjob;

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
