////////////////////////////////   file_c.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    08.08.91    Peter Reichert
// revisions:   18.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "all.h"
#include "init.h"
#include "scrio_c.h"
#include "file_c.h"

//////////////////////////////////////////////////////////////////////////////

// global buffers and response characters

extern const CARDINAL  BufLen;
extern char            Buf[];
extern char            Buf1[];
extern char            Buf2[];

extern char            AQ_SaveFile[];
extern char            AQ_WriteFile[];
extern char            AQ_SensFile[];
extern char            AQ_FitFile[];
extern char            AQ_PlotFile[];
extern char            AQ_ListFile[];

extern char            C1, C2, E1, E2, Y1, Y2, N1, N2;

//////////////////////////////////////////////////////////////////////////////

static std::ifstream        infile;
static std::ofstream        outfile;

//////////////////////////////////////////////////////////////////////////////

static BOOLEAN Save(AQUASYS&);
static BOOLEAN SaveAs(AQUASYS&);


static BOOLEAN New(AQUASYS& system)
{
   if ( system.Saved() == FALSE )
   {
      Write("\n   "); Write(scr.T(17)); Write(" "); WriteYesNo(); // Save?
      Read(Buf,BufLen,Y1);
      if ( (Buf[0]!=N1) && (Buf[0]!=N2) )
      {
         if ( SaveAs(system) == FALSE ) return FALSE;
      }
   }
   system.Delete();
   AQ_SaveFile[0]  = '\0';
   AQ_WriteFile[0] = '\0';
   AQ_SensFile[0]  = '\0';
   AQ_FitFile[0]   = '\0';
   AQ_PlotFile[0]  = '\0';
   AQ_ListFile[0]  = '\0';
   return TRUE;
}


static BOOLEAN Open(AQUASYS& system, const char* filename=0)
{
   if ( filename == 0 )
   {
      if ( system.Saved() == FALSE )
      {
         Write("\n   "); Write(scr.T(17)); Write(" "); WriteYesNo(); // Save?
         Read(Buf,BufLen,Y1);
         if ( (Buf[0]!=N1) && (Buf[0]!=N2) )
         {
            if ( SaveAs(system) == FALSE ) return FALSE;
         }
      }
      Write("\n   "); Write(scr.T(22));  // FileName
      Read(Buf,BufLen,"");
   }
   else
   {
      if ( strlen(AQ_SaveFile) == 0 ) strcpy(AQ_SaveFile,filename);
      strcpy(Buf,AQ_SaveFile);
   }
   BOOLEAN b = FALSE;
   if ( ExistFile(Buf) == FALSE )
   {
      Write("\n   "); Write(scr.T(634));
      Write(" "); Write(Buf); Write("\n");
   }
   else
   {
      infile.open(Buf,std::ios::in);
      switch ( system.Load(infile) ) 
      {
         case LoadSuccess:
            Write("\n   "); Write(scr.T(23));
            Write(" \""); Write(Buf); Write("\"\n");
            b = TRUE;
            strcpy(AQ_SaveFile,Buf);
            break;
         case WrongFileType:
            Write("\n   "); Write(scr.T(24));
            Write(" \""); Write(Buf); Write("\"\n");
            AQ_SaveFile[0] = '\0';
            break;
         case WrongFileVersion:
            Write("\n   "); Write(scr.T(541));
            Write(" \""); Write(Buf);
            Write("\"\n   "); Write(scr.T(542));
            strcpy(Buf,ProgVersion()); Buf[11] = '\0';
            Write(" "); Write(Buf); Write("\n");
            AQ_SaveFile[0] = '\0';
            break;
         case ReadError: case LoadInterrupt:
            Write("\n   "); Write(scr.T(543));
            Write(" \""); Write(Buf); Write("\"\n");
            AQ_SaveFile[0] = '\0';
            break;
      }
   }
   AQ_WriteFile[0] = '\0';
   AQ_SensFile[0]  = '\0';
   AQ_FitFile[0]   = '\0';
   AQ_PlotFile[0]  = '\0';
   AQ_ListFile[0]  = '\0';
   infile.close();
   return b;
}


static BOOLEAN Save(AQUASYS& system)
{
   if ( strlen(AQ_SaveFile) == 0 ) return SaveAs(system);
   outfile.open(AQ_SaveFile,std::ios::out|std::ios::trunc);
   SetFileType(AQ_SaveFile,FTaqu,FCaqu);
   BOOLEAN b = TRUE;
   switch ( system.Save(outfile) )
   {  
      case OK:
         Write("\n   "); Write(scr.T(29)); Write(" \"");          // Success
         Write(AQ_SaveFile); Write("\"\n");
         break;
      case PROBLEM: case INTERRUPT:
         Write("\n   "); Write(scr.T(30)); Write(" \"");          // Failure
         Write(AQ_SaveFile); Write("\"\n");
         b = FALSE;
   }
   outfile.close();
   return b;
}


static BOOLEAN SaveAs(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(28));           // FileName
   Read(Buf,BufLen,AQ_SaveFile);
   strcpy(AQ_SaveFile,Buf);
   AQ_WriteFile[0] = '\0';
   AQ_SensFile[0]  = '\0';
   AQ_FitFile[0]   = '\0';
   AQ_PlotFile[0]  = '\0';
   AQ_ListFile[0]  = '\0';
   return Save(system);
}


static BOOLEAN Revert(AQUASYS& system)
{
   if ( strlen(AQ_SaveFile) == 0 )
   {
      Write("\n   "); Write(scr.T(816)); Write("\n");     // Failure
      return FALSE;
   }
   if ( system.Saved() == FALSE )
   {
      Write("\n   "); Write(scr.T(818)); Write(" "); WriteYesNo(); // Delete?
      Read(Buf,BufLen,N1);
      if ( (Buf[0]!=Y1) && (Buf[0]!=Y2) )
      {
         Write("\n   "); Write(scr.T(817)); Write("\n");
         return FALSE;
      }
   }
   return Open(system,AQ_SaveFile);
}


static BOOLEAN Print(AQUASYS& system)
{
   if ( strlen(AQ_WriteFile)==0 && strlen(AQ_SaveFile)>0 )
   {
      strcpy(AQ_WriteFile,AQ_SaveFile);
      char* p = strchr(AQ_WriteFile,'.');
      if ( p != 0 ) *p = '\0';
      strcat(AQ_WriteFile,FEprn);
   }
   Write("\n   "); Write(scr.T(19));   // FileName
   Read(Buf,BufLen,AQ_WriteFile);
   strcpy(AQ_WriteFile,Buf);
   outfile.open(Buf,std::ios::out|std::ios::trunc);
   SetFileType(Buf,FTprn,FCprn);
   BOOLEAN b = TRUE;
   switch ( system.Write(outfile) )
   {
      case OK:
         Write("\n   "); Write(scr.T(20)); Write(" \"");       // Success
         Write(Buf); Write("\"\n");
         break;
      case PROBLEM: case INTERRUPT:
         Write("\n   "); Write(scr.T(21)); Write(" \"");       // Failure
         Write(Buf); Write("\"\n");
         b = FALSE;
         break;
   }                                                  
   outfile.close();
   return b;
}


static BOOLEAN Exit(AQUASYS& system)
{
   if ( system.Saved() == FALSE )
   {
      Write("\n   "); Write(scr.T(17)); Write(" "); WriteYesNo(); // Save?
      Read(Buf,BufLen,Y1);
      if ( (Buf[0]!=N1) && (Buf[0]!=N2) )
      {
         if ( SaveAs(system) == FALSE ) return FALSE;
      }
   }
   return TRUE;
}


static void About(AQUASYS& system)
{
   int width = 49; int spaces = 0;
   Write("\n*************************************************");
   Write("\n*                                               *");
   Write("\n*");
   strcpy(Buf,ProgName());
   spaces = (width-2-strlen(Buf))/2;
   Write(' ',spaces);
   Write(Buf);
   Write(' ',width-spaces-2-strlen(Buf));
   Write("*");
   Write("\n*                                               *");
   Write("\n*    Computer Program for the Identification    *");
   Write("\n*       and Simulation of Aquatic Systems       *");
   Write("\n*                                               *");
   Write("\n*");
   strcpy(Buf,ProgVersion());
   spaces = (width-2-strlen(Buf))/2;
   Write(' ',spaces);
   Write(Buf);
   Write(' ',width-spaces-2-strlen(Buf));
   Write("*");
   Write("\n*                                               *");
   Write("\n* Peter Reichert, Juerg Ruchti and Werner Simon *");
   Write("\n*        Department of Systems Analysis,        *");
   Write("\n*      Integrated Assessment and Modelling      *");
   Write("\n*   Swiss Federal Institute for Environmental   *");
   Write("\n*        Science and Technology  (EAWAG)        *");
   Write("\n*       CH - 8600 Duebendorf, Switzerland       *");
   Write("\n*                                               *");
   Write("\n*   Contact Person:  Peter Reichert             *");
   Write("\n*                    Email: reichert@eawag.ch   *");
   Write("\n*                    Fax:   +41 1 823 5398      *");
   Write("\n*                                               *");
   Write("\n*************************************************");
   Write("\n");
   return;
}


BOOLEAN File(AQUASYS& system, const char* filename)
{
   if ( filename != 0 )
   {
      Open(system,filename); return TRUE;
   }
   while (1)
   {
      Write("\n"); Write(scr.T(9)); Write("\n"); Write('-',strlen(scr.T(9)));
      Write("\n");                                // FileMenu
      Write("\n   1   =   "); Write(scr.T(10));   // New
      Write("\n   2   =   "); Write(scr.T(11));   // Open
      Write("\n   3   =   "); Write(scr.T(813));  // Close
      Write("\n   4   =   "); Write(scr.T(12));   // Save
      Write("\n   5   =   "); Write(scr.T(814));  // SaveAs
      Write("\n   6   =   "); Write(scr.T(815));  // Revert
      Write("\n   7   =   "); Write(scr.T(13));   // Print
      Write("\n   8   =   "); Write(scr.T(544));  // About
      Write("\n");
      Write("\n   "); Write(E1); Write("   =   "); Write(scr.T(8)); // Exit
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7)); // Cancel
      Read(Buf,BufLen);
      if ( (Buf[0]==C1) || (Buf[0]==C2) || (Buf[0]=='\0') ) return TRUE;
      switch (Buf[0])
      {
         case '1':
            if ( New(system) == TRUE ) return TRUE;
            break;
         case '2':
            if ( Open(system) == TRUE ) return TRUE;
            break;
         case '3':
            if ( New(system) == TRUE ) return TRUE;
            break;
         case '4':
            if ( Save(system) == TRUE ) return TRUE;
            break;
         case '5':
            if ( SaveAs(system) == TRUE ) return TRUE;
            break;
         case '6':
            if ( Revert(system) == TRUE ) return TRUE;
            break;
         case '7':
            if ( Print(system) == TRUE ) return TRUE;
            break;
         case '8':
            About(system);
            return TRUE;
         default:
            break;
      }
      if ( (Buf[0]==E1) || (Buf[0]==E2) )
      {
         if ( Exit(system) == TRUE ) return FALSE;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////
