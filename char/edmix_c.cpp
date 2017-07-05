////////////////////////////////   edmix_c.C   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    10.03.94    Peter Reichert
// revisions:   18.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "all.h"
#include "init.h"
#include "aquasys.h"
#include "scrio_c.h"
#include "edopt_c.h"
#include "edmix_c.h"

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

static void ReadInitCond(AQUASYS& system, MIXCOMP* com);
static void ReadInput(AQUASYS& system, MIXCOMP* com);

//////////////////////////////////////////////////////////////////////////////

static void ReadInitCond(AQUASYS& system, MIXCOMP* com)
{
   while (1)
   {
      AQVAR* v;
      Write("\n   "); Write(scr.T(237)); Write(" ");     // InitCond
      Write(com->Symbol()); Write(":\n");
      if ( com->NumInitCond() > 0 )
      {
         Write("\n   "); Write(scr.T(685));              // Index
         Write("   "); Write(scr.T(238));                // Variable
         Write("("); Write(scr.T(836)); Write(")");      // Zone
         Write("  :  "); Write(scr.T(239));              // InitCond
         for ( CARDINAL i=0; i<com->NumInitCond(); i++ )
         {
            Write("\n");
            NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
            Write(' ',strlen(scr.T(685)));
            v = com->InitVar(i); Write(v->Symbol());
            Write(" ("); Write(com->ZoneName(com->InitZone(i)));
            Write(")"); Write("  :  ");
            const FORMVAR* cfv = com->InitVal(i);
            cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(240));              // Select
      Write("\n   1   =   "); Write(scr.T(241));      // AddInitCond
      Write("\n   2   =   "); Write(scr.T(242));      // EditInitCond
      Write("\n   3   =   "); Write(scr.T(243));      // DeleteInitCond
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL numerr;
      CARDINAL index = 1;
      CARDINAL zone = 0;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(229));            // EnterAddIndex
            index = com->NumInitCond()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(244));            // EnterStateVar
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                               // VarNotFound
               break;
            }
            if ( com->NumZones() > 1 )
            {
               Write("\n   "); Write(scr.T(249));         // SelectZone
               for ( CARDINAL i=0; i<com->NumZones(); i++ )
               {
                  NumToString(i+1,Buf1,BufLen,4);
                  Write("\n"); Write(Buf1); Write("   =   "); 
                  Write(com->ZoneName(i));                // ZoneName
               }
               Write("\n  "); Read(zone,zone+1); zone--;
               if ( zone >= com->NumZones() ) zone = com->NumZones()-1;
            }
            Write("\n   "); Write(scr.T(245));            // EnterInitCond
            Read(Buf1,BufLen,"");
            
            if ( com->AddInitCond(system.Varlist(),zone,v,Buf1,
                                  Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(246)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(247)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(248));            // EnterEditIndex
            Read(index,index); index--;
            if ( index >= com->NumInitCond() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                    // IllegalIndex
            }
            Write("\n   "); Write(scr.T(244));            // Symbol
            Read(Buf1,BufLen,(com->InitVar(index))->Symbol());
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                               // VarNotFound
               break;
            }
            if ( com->NumZones() > 1 )
            {
               Write("\n   "); Write(scr.T(249));         // SelectZone
               for ( CARDINAL i=0; i<com->NumZones(); i++ )
               {
                  NumToString(i+1,Buf1,BufLen,4);
                  Write("\n"); Write(Buf1); Write("   =   "); 
                  Write(com->ZoneName(i));                // ZoneName
               }
               Write("\n  "); Read(zone,com->InitZone(index)+1); zone--;
            }
            Write("\n   "); Write(scr.T(245));            // EnterInitCond
            (com->InitVal(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( com->ReplaceInitCond(system.Varlist(),zone,v,Buf1,
                                      Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(250)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(251)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(254));                // EnterDelIndex
            Read(index,index); index--;
            if ( com->DeleteInitCond(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(255)); Write("\n");    // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(256)); Write("\n");    // Failure
            }
            break;
         default:
            break;
      }
   }
   return;
}

//////////////////////////////////////////////////////////////////////////////

static void ReadInput(AQUASYS& system, MIXCOMP* com)
{
   CARDINAL n = 0;
   while ( n < 3 )
   {
      n++;
      if ( com->Inflow() != 0 )
      {
         if ( (com->Inflow())->UnParse(Buf2,BufLen-1) != 0 )
         {
            Buf2[0] = '0'; Buf2[1] = '\0';
         }
      }
      else
      {
            Buf2[0] = '0'; Buf2[1] = '\0';
      }
      Write("\n   "); Write(scr.T(216)); Write("\n  "); // EnterExpression
      Read(Buf1,BufLen,Buf2);
      if ( com->Inflow(system.Varlist(),Buf1,Buf2) == TRUE ) break;
      FORMVAR::GetErrorMarks(Buf2,'^',Buf1);
      Write("\n    "); Write(Buf1);
      CARDINAL len = strlen(Buf1);
      for ( CARDINAL i=0; i<len; i++ )
      {
         if ( Buf2[i] != 0 )
         {
            FORMVAR::GetErrorMsg(Buf2[i],Buf1);
            Write("\n    "); Write(' ',i); Write(Buf1);
         }
      }
      Write("\n");
   }
   while (1)
   {
      AQVAR* v;
      Write("\n   "); Write(scr.T(217)); Write(" ");    // InputFluxes
      Write(com->Symbol()); Write(":\n");
      if ( com->NumInput() > 0 )
      {
         Write("\n   "); Write(scr.T(685));             // Index
         Write("   "); Write(scr.T(218));               // Variable
         Write("  :  "); Write(scr.T(219));             // Input
         for ( CARDINAL i=0; i<com->NumInput(); i++ )
         {
            Write("\n");
            NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
            Write(' ',strlen(scr.T(685)));
            v = com->InputVar(i); Write(v->Symbol()); Write("  :  ");
            const FORMVAR* cfv = com->InputFlux(i);
            cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(220));                // Select
      Write("\n   1   =   "); Write(scr.T(221));        // AddInput
      Write("\n   2   =   "); Write(scr.T(222));        // EditInput
      Write("\n   3   =   "); Write(scr.T(223));        // DeleteInput
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL numerr;
      CARDINAL index = 1;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(835));          // EnterAddIndex
            index = com->NumInput()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(224));          // EnterSymbol
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(225));       // EnterInputFlux
            Read(Buf1,BufLen,"0");
            if ( com->AddInput(system.Varlist(),v,Buf1,
                                Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(226)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(227)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(228));          // EnterEditIndex
            Read(index,index); index--;
            if ( index >= com->NumInput() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                  // IllegalIndex
            }
            Write("\n   "); Write(scr.T(224));          // EnterSymbol
            Read(Buf1,BufLen,(com->InputVar(index))->Symbol());
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(225));       // EnterInputFlux
            (com->InputFlux(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( com->ReplaceInput(system.Varlist(),v,Buf1,
                                   Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(230)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(231)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(234));              // EnterDelIndex
            Read(index,index); index--;
            if ( com->DeleteInput(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(235)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(236)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }
   }
   return;
}

//////////////////////////////////////////////////////////////////////////////

MIXCOMP* EditMixComp(AQUASYS& system, MIXCOMP* mixcomp)
{
   ReadSymbol(mixcomp);

   ReadCompInd(mixcomp);

   REAL x;
   if ( mixcomp->FixedVol() == TRUE ) Buf2[0] = '1';
   else                               Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(638));          // Select
   Write("\n   1   =   "); Write(scr.T(639));  // FixedVol
   Write("\n   2   =   "); Write(scr.T(640));  // VariaVol
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0]=='1' )
   {
      Write("\n   "); Write(scr.T(214));      // EnterVol
      Read(x,mixcomp->Vol()); mixcomp->Vol(x);
      mixcomp->FixedVol(TRUE);
   }
   else
   {
      CARDINAL n = 0; CARDINAL numerr;
      while (1)
      {
         if ( mixcomp->Outflow() != 0 )
         {
            if ( (mixcomp->Outflow())->UnParse(Buf2,BufLen-1) != 0 )
               Buf2[0] = '\0';
         }
         else
         { 
            Buf2[0] = '\0';
         } 
         Write("\n   "); Write(scr.T(641)); Write("\n  ");
         Read(Buf1,BufLen,Buf2);            // EnterOutflow
         if ( mixcomp->Outflow(Buf1,system.Varlist(),Buf2,numerr)
                                                            == TRUE )
         {
            break;
         }
         else
         {
            n++;
            if ( n > 2 )
            {
               delete mixcomp; return 0;
            }
            if ( numerr == 0 )
            {                                       // IllegalDep
               Write("\n   "); Write(scr.T(642)); Write("\n");
            }
            else
            {
               FORMVAR::GetErrorMarks(Buf2,'^',Buf1);
               Write("\n    "); Write(Buf1);
               CARDINAL len = strlen(Buf1);
               for ( CARDINAL i=0; i<len; i++ )
               {
                  if ( Buf2[i] != 0 )
                  {
                     FORMVAR::GetErrorMsg(Buf2[i],Buf1);
                     Write("\n    "); Write(' ',i);  Write(Buf1);
                  }
               }
               Write("\n");
            }
         }
      }
      mixcomp->FixedVol(FALSE);
   }

   if ( mixcomp->Active() == TRUE ) Buf2[0] = '1';
   else                             Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(1395));                 // Status
   Write("\n   1   =   "); Write(scr.T(1396));         // Active
   Write("\n   2   =   "); Write(scr.T(1397));         // Inactive
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '2' ) mixcomp->Active(FALSE);
   else                  mixcomp->Active(TRUE);

   ReadActVar(system,mixcomp);
   ReadActProc(system,mixcomp);

   ReadInitCond(system,mixcomp);
   ReadInput(system,mixcomp);

   return mixcomp;
}


//////////////////////////////////////////////////////////////////////////////
