//////////////////////////////////   all.h   /////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    05.12.90    Peter Reichert
// revisions:   12.06.91    Peter Reichert    ported to Sun
//              16.09.91    Peter Reichert    ostreams ctest, cjob, cfatal
//                                            introduced
//              02.11.93    Peter Reichert    SetFileType added
//              05.11.98    Peter Reichert    CARDLONG, INTLONG added
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// "all" implements general definitions for program AQUASIM
// ========================================================
//
//////////////////////////////////////////////////////////////////////////////
//
// Concepts
// ========
//
// If the program uses exclusively the numerical types CARDINAL, INTEGER and
// REAL, an implementation dependent adaptation of numeric data types consists
// only in changing one of the following typedef statements.
//
// The following global ostream pointers are introduced:
//    ctest:     ostream for writing output to test the classes (initialized
//               to &cout).
//    cjob:      ostream for writing warning error messages or job-log infor-
//               mation (initialized to &cout).
//    cfatal:    ostream for writing fatal error messages before interrupting
//               the program ( initialized to &cout).
// The procedures SetTestout, SetWarnOut and SetFatalOut may be used to change
// these ostream pointers.
//
// The function FatalError should be called only in case of an internal pro-
// gram error; it terminates the program with the error message given as
// argument.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef ALL_H
#define ALL_H

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
#define AQUA_WIN   9

//////////////////////////////////////////////////////////////////////////////

// check if the macros AQUA_VERSION and AQUA_OS are defined:

#ifndef AQUA_VERSION
#include "Please define macro AQUA_VERSION"
#endif

#ifndef AQUA_OS
#include "Please define macro AQUA_OS"
#endif

//////////////////////////////////////////////////////////////////////////////

#include <limits.h>

#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <strstream>

#if AQUA_VERSION==AQUA_XVT
#include "xvt.h"
#elif AQUA_VERSION==AQUA_VCC
#include "../stdafx.h"
#endif

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


enum JOBSTATUS { OK, INTERRUPT, PROBLEM };


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

typedef BOOLEAN (*CANCELFUN)();

//////////////////////////////////////////////////////////////////////////////

const char* ProgName();
const char* ProgVersion();

//////////////////////////////////////////////////////////////////////////////

extern char NotAvail[3];

//////////////////////////////////////////////////////////////////////////////

const CARDINAL TypeLen = 6;

// file types:

extern char FTaqu[TypeLen];
extern char FTdatR[TypeLen];
extern char FTdatW[TypeLen];
extern char FTlog[TypeLen];
extern char FTprn[TypeLen];
extern char FTsen[TypeLen];
extern char FTfit[TypeLen];
extern char FTlis[TypeLen];
extern char FTpts[TypeLen];

// file creators:

extern char FCaqu[TypeLen];
extern char FCdat[TypeLen];
extern char FClog[TypeLen];
extern char FCprn[TypeLen];
extern char FCsen[TypeLen];
extern char FCfit[TypeLen];
extern char FClis[TypeLen];
extern char FCpts[TypeLen];

// file extensions:

extern char FEaqu[TypeLen];
extern char FEdat[TypeLen];
extern char FElog[TypeLen];
extern char FEprn[TypeLen];
extern char FEsen[TypeLen];
extern char FEfit[TypeLen];
extern char FElis[TypeLen];
extern char FEpts[TypeLen];

//////////////////////////////////////////////////////////////////////////////

// program termination

void aqexit();

//////////////////////////////////////////////////////////////////////////////

// test if number is finite and defined

int aqfinite(REAL x);

//////////////////////////////////////////////////////////////////////////////

void HeapFileMessage(const char* txt);

void ListHeap(const char* txt);

//////////////////////////////////////////////////////////////////////////////

// file manipulations

// remove file

int RemoveFile(const char* filename);

// rename file

int RenameFile(const char* oldfilename, const char* newfilename);

// check if file exists

BOOLEAN ExistFile(const char* name);

// set file type and creator on the Macintosh (no effect on other systems!) 

BOOLEAN SetFileType(const char* name, const char* type, const char* creator);

//////////////////////////////////////////////////////////////////////////////

// string and number handling functions:

char ToUpper(char c);

int DefStringPrec();

void DefStringPrec(int prec);

void NumToString(CARDINAL num, char* buf, CARDINAL bufsize, CARDINAL right=0);

void NumToString(INTEGER num, char* buf, CARDINAL bufsize, CARDINAL right=0);

void NumToString(CARDLONG num, char* buf, CARDINAL bufsize, CARDINAL right=0);

void NumToString(INTLONG num, char* buf, CARDINAL bufsize, CARDINAL right=0);

void NumToString(REAL num, char* buf, CARDINAL bufsize, CARDINAL right=0,
		 int prec=0);

void NumToStr(REAL num, char* buf, CARDINAL bufsize,
		 BOOLEAN right, INTEGER semichars);

BOOLEAN StringToNum(char* buf, CARDINAL* num);

BOOLEAN StringToNum(char* buf, INTEGER* num);

BOOLEAN StringToNum(char* buf, CARDLONG* num);

BOOLEAN StringToNum(char* buf, INTLONG* num);

BOOLEAN StringToNum(char* buf, REAL* num);

BOOLEAN TimeToString(const char* form, char* string, CARDINAL size);

//////////////////////////////////////////////////////////////////////////////

// output procedures:

void SetTestOut(std::ostream* testout);


void SetJobOut(std::ostream* jobout);


void SetFatalOut(std::ostream* fatalout);


void FatalError(const char* message);

//////////////////////////////////////////////////////////////////////////////

// global variables:

extern const REAL aq_pi;

extern std::ostream* ctest;
extern std::ostream* cjob;
extern std::ostream* cfatal;

extern const char* initfile;
extern const char* ptstmpfile;


//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
