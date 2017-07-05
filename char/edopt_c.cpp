///////////////////////////////   edopt_c.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    14.03.94    Peter Reichert
// revisions:   18.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "all.h"
#include "init.h"
#include "mixcomp.h"
#include "filmcomp.h"
#include "rivcomp.h"
#include "scrio_c.h"
#include "edmix_c.h"
#include "edfilm_c.h"
#include "edriv_c.h"
#include "edopt_c.h"

//////////////////////////////////////////////////////////////////////////////

// global buffers and response characters (aquasim.C)

extern const CARDINAL  BufLen;
extern char            Buf[];
extern char            Buf1[];
extern char            Buf2[];
extern char            Buf3[];
extern char            Buf4[];

extern char            C1, C2, Y1, Y2, N1, N2;

//////////////////////////////////////////////////////////////////////////////

static std::ifstream infile;
static std::ofstream outfile;

//////////////////////////////////////////////////////////////////////////////


void ReadSymbol(SYMBOL* sym)
{
   while (1)
   {
      Write("\n   "); Write(scr.T(44));               // EnterSymbol
      Read(Buf1,BufLen,sym->Symbol());
      if ( (FORMVAR::ReservedSymbol(Buf1)==FALSE)
           && (sym->Symbol(Buf1)==TRUE)           ) break;
      Write("\n   "); Write(scr.T(45)); Write("\n");  // IllegalSymbol
   }
   Write("\n   "); Write(scr.T(46));                  // EnterDescription
   Read(Buf1,BufLen,sym->Description()); sym->Description(Buf1);
   return;
}


//////////////////////////////////////////////////////////////////////////////


void ReadCompInd(AQCOMP* com)
{
   CARDINAL i;
   Write("\n   "); Write(scr.T(1248));               // EnterCompInd
   Read(i,com->CmpIndex());
   com->CmpIndex(i);
   return;
}


//////////////////////////////////////////////////////////////////////////////


void ReadLinkInd(AQLINK* link)
{
   CARDINAL i;
   Write("\n   "); Write(scr.T(1249));               // EnterLinkInd
   Read(i,link->LnkIndex());
   link->LnkIndex(i);
   return;
}


//////////////////////////////////////////////////////////////////////////////


void ReadActVar(AQUASYS& system, AQCOMP* com)
{
   while (1)
   {
      Write("\n   "); Write(scr.T(310)); Write(":\n");  // AvailVar
      if ( system.Varlist()->Size() > 0 )
      {
         CARDINAL i = 0;
         AQVAR* var = system.Varlist()->First();
         while ( var != 0 )
         {
            if ( i % scr.I(1) == 0 ) Write("\n   ");
            i++;
            Write(var->Symbol()); Write(' ',scr.L(1)-strlen(var->Symbol()));
            var = var->Next();
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(311)); Write(":\n");  // ActiveVar
      if ( com->NumActVar() > 0 )
      {
         for ( CARDINAL i=0; i<com->NumActVar(); i++ )
         {
            if ( i % scr.I(1) == 0 ) Write("\n   ");
            Write((com->ActVar(i))->Symbol());
            Write(' ',scr.L(1)-strlen((com->ActVar(i))->Symbol()));
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(312)); Write(":");  // Select
      Write("\n   1   =   "); Write(scr.T(313));      // ActivateProc
      Write("\n   2   =   "); Write(scr.T(314));      // InactivateProc
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL index = com->NumActVar()+1;
      switch (Buf1[0])
      {
         case '1':
            Write("\n   "); Write(scr.T(315));                 // EnterSymbol
            Read(Buf1,BufLen,"");
            Write("\n   "); Write(scr.T(833));                 // EnterPos
            Read(index,index); index--;
            if ( com->AddActVar(system.GetVar(Buf1),index) == TRUE )
            {
               Write("\n   "); Write(scr.T(317)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(318)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(316));                 // EnterSymbol
            Read(Buf1,BufLen,"");
            if ( com->RemoveActVar(system.GetVar(Buf1)) == TRUE )
            {
               Write("\n   "); Write(scr.T(319)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(320)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }
   }
}


//////////////////////////////////////////////////////////////////////////////


void ReadActProc(AQUASYS& system, AQCOMP* com)
{
   while (1)
   {
      Write("\n   "); Write(scr.T(203)); Write(":\n");  // AvailProc
      if ( system.Proclist()->Size() > 0 )
      {
         CARDINAL i = 0;
         AQPROC* proc = system.Proclist()->First();
         while ( proc != 0 )
         {
            if ( i % scr.I(1) == 0 ) Write("\n   ");
            i++;
            Write(proc->Symbol());
            Write(' ',scr.L(1)-strlen(proc->Symbol()));
            proc = proc->Next();
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(204)); Write(":\n");  // ActiveProc
      if ( com->NumActProc() > 0 )
      {
         for ( CARDINAL i=0; i<com->NumActProc(); i++ )
         {
            if ( i % scr.I(1) == 0 ) Write("\n   ");
            Write((com->ActProc(i))->Symbol());
            Write(' ',scr.L(1)-strlen((com->ActProc(i))->Symbol()));
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(207)); Write(":");  // Select
      Write("\n   1   =   "); Write(scr.T(208));      // ActivateProc
      Write("\n   2   =   "); Write(scr.T(209));      // InactivateProc
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL index = com->NumActProc()+1;
      switch (Buf1[0])
      {
         case '1':
            Write("\n   "); Write(scr.T(205));                 // EnterSymbol
            Read(Buf1,BufLen,"");
            Write("\n   "); Write(scr.T(834));                 // EnterPos
            Read(index,index); index--;
            if ( com->AddActProc(system.GetProc(Buf1),index) == TRUE )
            {
               Write("\n   "); Write(scr.T(210)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(211)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(206));                 // EnterSymbol
            Read(Buf1,BufLen,"");
            if ( com->RemoveActProc(system.GetProc(Buf1)) == TRUE )
            {
               Write("\n   "); Write(scr.T(212)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(213)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }
   }
}


//////////////////////////////////////////////////////////////////////////////
