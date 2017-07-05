///////////////////////////////   edcomp_c.C   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    08.08.91    Peter Reichert
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
#include "edadv_c.h"
#include "edlake_c.h"
#include "edsoil_c.h"
#include "edcomp_c.h"

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


static AQCOMP* NewComp(AQUASYS& system, AQCOMP* oldcomp)
{
   while (1)
   {
      if ( oldcomp == 0 )
      {
         Write("\n   "); Write(scr.T(200));           // SelectCompType
         Write("\n   1   =   "); Write(scr.T(198));   // MixComp
         Write("\n   2   =   "); Write(scr.T(442));   // FilmComp
         Write("\n   3   =   "); Write(scr.T(888));   // AdvComp
         Write("\n   4   =   "); Write(scr.T(443));   // SoilComp
         Write("\n   5   =   "); Write(scr.T(199));   // RivComp
         Write("\n   6   =   "); Write(scr.T(889));   // LakeComp
         Write("\n");
         Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
         Read(Buf1,BufLen);
      }
      else
      {
         Write("\n   "); Write(scr.T(201)); Write(" ");
         Write(oldcomp->Symbol()); Write(" "); Write(scr.T(202)); Write(" ");
         switch ( oldcomp->Type() )                   // CompIsA
         {
            case MixComp:
               Buf1[0] = '1'; Write(scr.T(198));      // MixComp
               break;
            case FilmComp:
               Buf1[0] = '2'; Write(scr.T(442));      // FilmComp
               break;
            case AdvComp:
               Buf1[0] = '3'; Write(scr.T(888));      // AdvComp
               break;
            case SoilComp:
               Buf1[0] = '4'; Write(scr.T(443));      // SoilComp
               break;
            case RivComp:
               Buf1[0] = '5'; Write(scr.T(199));      // RivComp
               break;
            case LakeComp:
               Buf1[0] = '6'; Write(scr.T(889));      // LakeComp
               break;
            default:
               FatalError("aquaedit::NewComp: Unknown comp type");
         }
         Write("\n");
      }
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) return 0;
      switch (Buf1[0])
      {
         case '1':
         {
            MIXCOMP* mixcomp = new MIXCOMP(oldcomp);
            return EditMixComp(system,mixcomp);
         }
         case '2':
         {
            FILMCOMP* filmcomp = new FILMCOMP(oldcomp);
            return EditFilmComp(system,filmcomp);
         }
         case '3':
         {
            ADVCOMP* advcomp = new ADVCOMP(oldcomp);
            return EditAdvComp(system,advcomp);
         }
         case '4':
         {
            SOILCOMP* soilcomp = new SOILCOMP(oldcomp);
            return EditSoilComp(system,soilcomp);
         }
         case '5':
         {
            RIVCOMP* rivcomp = new RIVCOMP(oldcomp);
            return EditRivComp(system,rivcomp);
         }
         case '6':
         {
            LAKECOMP* lakecomp = new LAKECOMP(oldcomp);
            return EditLakeComp(system,lakecomp);
         }
         default:
            break;
      }
   }
}


//////////////////////////////////////////////////////////////////////////////


static void AddComp(AQUASYS& system)
{
   AQCOMP* com = NewComp(system,0);
   if ( system.AddComp(com) == TRUE )
   {
      Write("\n   "); Write(scr.T(188)); Write("\n"); // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(189)); Write("\n"); // Failure
      delete com;
   }
   return;
}


//////////////////////////////////////////////////////////////////////////////


static void CopyComp(AQUASYS& system)
{
   Buf1[0] = '\0';
   AQCOMP* oldcomp = system.Complist()->First();
   if ( system.Complist()->Size() > 1 )
   {
      Write("\n   "); Write(scr.T(369));                 // EnterSymbol
      Read(Buf1,BufLen,"");
      oldcomp = system.GetComp(Buf1);
   }
   if ( oldcomp == 0 )
   {
      Write("\n   "); Write(scr.T(191)); Write(" "); Write(Buf1);
      Write(" "); Write(scr.T(192)); Write("\n");     // CompNotFound
      return;
   }
   AQCOMP* com = NewComp(system,oldcomp);
   if ( system.AddComp(com) == TRUE )
   {
      Write("\n   "); Write(scr.T(370)); Write("\n"); // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(418)); Write("\n"); // Failure
      delete com;
   }
   return;
}


//////////////////////////////////////////////////////////////////////////////


static void EditComp(AQUASYS& system)
{
   Buf1[0] = '\0';
   AQCOMP* oldcomp = system.Complist()->First();
   if ( system.Complist()->Size() > 1 )
   {
      Write("\n   "); Write(scr.T(190));              // EnterSymbol
      Read(Buf1,BufLen,"");
      oldcomp = system.GetComp(Buf1);
   }
   if ( oldcomp == 0 )
   {
      Write("\n   "); Write(scr.T(191)); Write(" "); Write(Buf1);
      Write(" "); Write(scr.T(192)); Write("\n");     // CompNotFound
      return;
   }
   AQCOMP* com = NewComp(system,oldcomp);
   if ( system.ReplaceComp(oldcomp,com) == TRUE )
   {
      Write("\n   "); Write(scr.T(193)); Write("\n"); // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(194)); Write("\n"); // Failure
      delete com;
   }
   return;
}


//////////////////////////////////////////////////////////////////////////////


static void DeleteComp(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(195));                 // EnterSymbol
   Read(Buf1,BufLen,"");
   if ( system.DeleteComp(Buf1) == TRUE )
   {
      Write("\n   "); Write(scr.T(196)); Write("\n"); // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(197)); Write("\n"); // Failure
   }
   return;
}


//////////////////////////////////////////////////////////////////////////////


void EditCompartments(AQUASYS& system)
{
   while (1)
   {
      if ( system.Complist()->Size() > 0 )
      {
         Write("\n"); Write(scr.T(149)); Write(":\n");  // ActCompList
         CARDINAL i = 0;
         AQCOMP* com = system.Complist()->First();
         while ( com != 0 )
         {
            i++;
            if ( (i-1) % scr.I(1) == 0 ) Write("\n   ");
            Write(com->Symbol()); Write(' ',scr.L(1)-strlen(com->Symbol()));
            com = com->Next();
         }
         Write("\n");
      }
      Write("\n"); Write(scr.T(150)); Write("\n");  // EditCompMenu
      Write('-',strlen(scr.T(150)));
      Write("\n");
      Write("\n   1   =   "); Write(scr.T(151));    // AddComp
      Write("\n   2   =   "); Write(scr.T(368));    // CopyComp
      Write("\n   3   =   "); Write(scr.T(152));    // EditComp
      Write("\n   4   =   "); Write(scr.T(153));    // DeleteComp
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) return;
      switch (Buf1[0])
      {
         case '1':
            AddComp(system);
            break;
         case '2':
            CopyComp(system);
            break;
         case '3':
            EditComp(system);
            break;
         case '4':
            DeleteComp(system);
            break;
         default:
            break;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////
