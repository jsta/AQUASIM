/////////////////////////////////   init.h   /////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    17.09.91    Peter Reichert
// revisions:   
//
//////////////////////////////////////////////////////////////////////////////
//
// "init" implements a class for global initializations
// ====================================================
//
//////////////////////////////////////////////////////////////////////////////
//
// Concepts
// ========
//
// The class INIT reads integers, reals, field lengths and text from a file
// and makes them available for the program.
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// INIT:         Class for global integers, reals, field lengths and strings.
// -----
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class INIT
// =========================
//
// INIT(const char* filename, const char* identifier, const char* deftext,
//      CARDINAL nI, CARDINAL nR, CARDINAL nL, CARDINAL nT)
//                                   constructs an INIT class with nI
//                                   integers, nR reals, nL field lengths and
//                                   nT text strings which are read from the
//                                   section labeled 'identifier' from the
//                                   file 'filename'.
//                                   Default text strings are 'deftext'
//                                   appended by the number of the text.
//                                   Reading from the file occurs according to
//                                   'Load'-format of fileio.h in the
//                                   following sequence:
//                                    - ???
//                                    
//                                    
//                                    
//                                    
//                                    
//                                    
//                                    
//                                    
//                                    
//                                    
//
// INTEGER I(CARDINAL index)         returns the integer given by index
//                                   (index has to be between 1 and nI).
//
// REAL R(CARDINAL index)            returns the real given by index
//                                   (index has to be between 1 and nR).
//
// CARDINAL L(CARDINAL index)        returns the field length given by index
//                                   (index has to be between 1 and nL).
//
// const char* T(CARDINAL index)     returns a pointer to the const string
//                                   given by index (index has to be betweeen
//                                   1 and nT).
//
//////////////////////////////////////////////////////////////////////////////

#ifndef INIT_H
#define INIT_H

#include "all.h"

//////////////////////////////////////////////////////////////////////////////

class INIT
{
   public:

      INIT(const char* filename, const char* identifier, const char* deftext,
           CARDINAL nI, CARDINAL nR, CARDINAL nL, CARDINAL nT);
      ~INIT();

      INTEGER     I(CARDINAL index) const;       // integers
      REAL        R(CARDINAL index) const;       // integers
      CARDINAL    L(CARDINAL index) const;       // field length
      const char* T(CARDINAL index) const;       // text

   private:

      CARDINAL  nInteger;
      CARDINAL  nReal;
      CARDINAL  nLength;
      CARDINAL  nText;

      INTEGER*  Integer;
      REAL*     Real;
      CARDINAL* Length;
      char**    Text;
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////

