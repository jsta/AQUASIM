////////////////////////////////   fileio.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    27.07.91    Peter Reichert
// revisions:   17.09.91    Peter Reichert    FILEIO::FILEIO(const FILEIO&)
//                                            constructor and
//                                            FILEIO::operator=(const FILEIO&)
//                                            operator added.
//              20.11.91    Peter Reichert    WriteDate and WriteTime added
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// "fileio" implements procedures for file input and output
// ========================================================
//
//////////////////////////////////////////////////////////////////////////////
//
// Concepts
// ========
//
// The class FILEIO provides file input and output procedures for derived 
// classes (the procedures are declared static and thus may also be used from
// outside of the class).
// The following notational convention is applied:
//    Write:   For writing to a file any combination of characters is allowed.
//             The purpose of such a file is to be used externally (mainly to
//             be printed). It is not possible to reload data from such a
//             file.
//    Save:    In saving to a file strings (char*) and numbers (CARDINAL, 
//             INTEGER and REAL) are enclosed in unique start and end deli-
//             miters. This allows to reload them from a file. 
//    Load:    In loading from a file the strings and numbers between start
//             and end delimiters are extracted.
// The class contains a  BOOLEAN variable to indicate if the derived object
// has already been saved.
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// FILEIO:       Class providing file input and output procedures.
// -------
//
//////////////////////////////////////////////////////////////////////////////
//
// Methods of the class FILEIO
// ===========================
//
//
// static BOOLEAN SaveStart(ostream& out)
//                                   writes a start delimiter to out and re-
//                                   turns TRUE if no output problems
//                                   occurred, returns FALSE otherwise.
//
// static BOOLEAN SaveEnd(ostream& out)
//                                   writes a end delimiter to out and re-
//                                   turns TRUE if no output problems
//                                   occurred, returns FALSE otherwise.
//
// static BOOLEAN SaveString(ostream& out, const char* str)
//                                   writes the string str enclosed by start
//                                   and end delimiters to out and returns 
//                                   TRUE if no output problems occurred,
//                                   returns FALSE otherwise.
//
// static BOOLEAN SaveNumber(ostream& out, CARDINAL num)
// static BOOLEAN SaveNumber(ostream& out, INTEGER num)
// static BOOLEAN SaveNumber(ostream& out, REAL num)
//                                   writes the number num enclosed by start
//                                   and end delimiters to out and returns
//                                   TRUE if no output problems occurred,
//                                   returns FALSE otherwise.
//
// static BOOLEAN SaveBoolean(ostream& out, BOOLEAN b)
//                                   writes a string representation of the
//                                   BOOLEAN b enclosed by start and end de-
//                                   limiters to out and returns TRUE if no
//                                   output problems occurred, returns FALSE
//                                   otherwise.
//
// static BOOLEAN LoadStart(istream& in)
//                                   reads from in until a start delimiter, a
//                                   end delimiter or an input problem occurs;
//                                   returns TRUE if a start delimiter was
//                                   read, FALSE otherwise.
//
// static BOOLEAN LoadEnd(istream& in)
//                                   reads from in until a start delimiter, a
//                                   end delimiter or an input problem occurs;
//                                   returns TRUE if a end delimiter was
//                                   read, FALSE otherwise.
//
// static BOOLEAN LoadString(istream& in, char* str, CARDINAL dim)
//                                   reads the next between start and end de-
//                                   limiters enclosed string of at most
//                                   dim-1 characters. Returns TRUE if this
//                                   was possible, FALSE otherwise.
//
// static BOOLEAN LoadNumber(istream& in, CARDINAL* num)
// static BOOLEAN LoadNumber(istream& in, INTEGER* num)
// static BOOLEAN LoadNumber(istream& in, REAL* num)
//                                   read the next between start and end de-
//                                   limiters enclosed number and return TRUE
//                                   if this was possible, return FALSE
//                                   otherwise.
//
// static BOOLEAN LoadBoolean(istream& in, BOOLEAN* b)
//                                   reads the next between start and end de-
//                                   limiters enclosed string and returns TRUE
//                                   if it could be interpreted as BOOLEAN,
//                                   returns FALSE otherwise.
//
// JOBSTATUS Save(ostream& out)      virtual base function for saving derived
//                                   classes. Sets the variable 'saved' to 
//                                   TRUE.
//                              
// JOBSTATUS Write(ostream& out)     virtual base function for writing derived
//                                   classes.
//
// BOOLEAN Saved()                   sets the variable 'saved' to TRUE.
//
// void Changed()                    sets the variable 'saved' to FALSE.
//
// void Test()                       prints the variables of the class to
//                                   ctest (see all.h). Virtual member.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef FILEIO_H
#define FILEIO_H

#include "all.h"

//////////////////////////////////////////////////////////////////////////////

class FILEIO
{
   public:

      FILEIO();
      FILEIO(const FILEIO& fileio);
      FILEIO(const FILEIO* fileio);
      virtual ~FILEIO() { };

      FILEIO&           operator=(const FILEIO& fileio);

      static void       ResetSaveCount()
                           { savecount = 0; }
      static void       ResetPrintCount()
                           { printcount = 0; }

      static CARDINAL   PrintWidth()
                           { return printwidth; }
      static void       PrintWidth(CARDINAL width);
      static CARDINAL   PrintIndent()
                           { return printindent; }
      static void       PrintIndent(CARDINAL indent);

      static BOOLEAN    PrintLn(std::ostream& out, char c='\0');
      static BOOLEAN    Print(std::ostream& out, char c,
                              CARDINAL pos=0, CARDINAL repeat=1);
      static BOOLEAN    Print(std::ostream& out, const char* string,
                              CARDINAL pos=0);
      static BOOLEAN    Print(std::ostream& out, CARDINAL num,
                              CARDINAL pos=0);
      static BOOLEAN    Print(std::ostream& out, CARDLONG num,
                              CARDINAL pos=0);
      static BOOLEAN    Print(std::ostream& out, INTEGER num,
                              CARDINAL pos=0);
      static BOOLEAN    Print(std::ostream& out, INTLONG num,
                              CARDINAL pos=0);
      static BOOLEAN    Print(std::ostream& out, REAL num,
                              CARDINAL pos=0);
      static BOOLEAN    PrintTime(std::ostream& out, const char* form,
                                  CARDINAL pos=0);
      static BOOLEAN    PrintBuffer(std::ostream& out);

      static BOOLEAN    SaveStart(std::ostream& out);
      static BOOLEAN    SaveEnd(std::ostream& out);
      static BOOLEAN    SaveString(std::ostream& out, const char* str);
      static BOOLEAN    SaveNumber(std::ostream& out, REAL num);
      static BOOLEAN    SaveNumber(std::ostream& out, CARDINAL num);
      static BOOLEAN    SaveNumber(std::ostream& out, INTEGER num);
      static BOOLEAN    SaveBoolean(std::ostream& out, BOOLEAN b);

      static BOOLEAN    LoadStart(std::istream& in);
      static BOOLEAN    LoadEnd(std::istream& in);
      static BOOLEAN    LoadString(std::istream& in, char* str, CARDINAL dim);
      static BOOLEAN    LoadNumber(std::istream& in, REAL* num);
      static BOOLEAN    LoadNumber(std::istream& in, CARDINAL* num);
      static BOOLEAN    LoadNumber(std::istream& in, INTEGER* num);
      static BOOLEAN    LoadBoolean(std::istream& in, BOOLEAN* b);

      virtual JOBSTATUS Save(std::ostream& out);
      virtual JOBSTATUS Write(std::ostream& out, BOOLEAN sh=FALSE);

      BOOLEAN           Saved()
                           { return saved; }
      void              Changed()
                           { saved = FALSE; }

      virtual void      Test();

   protected:

      static CARDINAL   printwidth;
      static CARDINAL   printindent;
      static CARDINAL   printcount;

      static CARDINAL   savecount;

      BOOLEAN           saved;
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
