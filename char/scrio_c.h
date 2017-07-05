////////////////////////////////   scrio_c.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    21.08.91    Peter Reichert
// revisions:   
//
//////////////////////////////////////////////////////////////////////////////

#ifndef SCRIO_C_H
#define SCRIO_C_H

#include "all.h"
#include "init.h"

//////////////////////////////////////////////////////////////////////////////

void Write(const char* string);

void Write(char c, INTEGER num=1);

void Write(CARDINAL num);

void Write(CARDLONG num);

void Write(INTEGER num);

void Write(REAL num);

void WriteYesNo();

void WriteBuffer();

void Read(char* string, CARDINAL size);

void Read(char* string, CARDINAL size, const char* def);

void Read(char* string, CARDINAL size, char def);

void Read(CARDINAL& c, CARDINAL def);

void Read(CARDLONG& c, CARDINAL def);

void Read(INTEGER& i, INTEGER def);

void Read(REAL& r, REAL def);

//////////////////////////////////////////////////////////////////////////////

extern INIT scr;

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////

