///////////////////////////////   edadv_c.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    11.07.94    Peter Reichert
// revisions:   18.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "all.h"
#include "init.h"
#include "aquasys.h"
#include "scrio_c.h"
#include "edopt_c.h"
#include "edadv_c.h"

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

static void ReadInitCond(AQUASYS& system, ADVCOMP* com);
static void ReadInput(AQUASYS& system, ADVCOMP* com);

//////////////////////////////////////////////////////////////////////////////

static void ReadInitCond(AQUASYS& system, ADVCOMP* com)
{
   while (1)
   {
      AQVAR* v;
      Write("\n   "); Write(scr.T(1092)); Write(" ");    // InitCond
      Write(com->Symbol()); Write(":\n");
      if ( com->NumInitCond() > 0 )
      {
         Write("\n   "); Write(scr.T(685));              // Index
         Write("   "); Write(scr.T(1093));               // Variable
         Write("("); Write(scr.T(1094)); Write(")");     // Zone
         Write("  :  "); Write(scr.T(1095));             // InitCond
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
      Write("\n   "); Write(scr.T(1096));             // Select
      Write("\n   1   =   "); Write(scr.T(1097));     // AddInitCond
      Write("\n   2   =   "); Write(scr.T(1098));     // EditInitCond
      Write("\n   3   =   "); Write(scr.T(1099));     // DeleteInitCond
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
            Write("\n   "); Write(scr.T(1100));           // EnterAddIndex
            index = com->NumInitCond()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(1101));           // EnterStateVar
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
               Write("\n   "); Write(scr.T(1102));        // SelectZone
               for ( CARDINAL i=0; i<com->NumZones(); i++ )
               {
                  NumToString(i+1,Buf1,BufLen,4);
                  Write("\n"); Write(Buf1); Write("   =   "); 
                  Write(com->ZoneName(i));                // ZoneName
               }
               Write("\n  "); Read(zone,zone+1); zone--;
               if ( zone >= com->NumZones() ) zone = com->NumZones()-1;
            }
            Write("\n   "); Write(scr.T(1103));           // EnterInitCond
            Read(Buf1,BufLen,"");
            
            if ( com->AddInitCond(system.Varlist(),zone,v,Buf1,
                                  Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1104)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1105)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(1106));            // EnterEditIndex
            Read(index,index); index--;
            if ( index >= com->NumInitCond() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                    // IllegalIndex
            }
            Write("\n   "); Write(scr.T(1101));            // Symbol
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
               Write("\n   "); Write(scr.T(1102));        // SelectZone
               for ( CARDINAL i=0; i<com->NumZones(); i++ )
               {
                  NumToString(i+1,Buf1,BufLen,4);
                  Write("\n"); Write(Buf1); Write("   =   "); 
                  Write(com->ZoneName(i));                // ZoneName
               }
               Write("\n  "); Read(zone,com->InitZone(index)+1); zone--;
            }
            Write("\n   "); Write(scr.T(1103));           // EnterInitCond
            (com->InitVal(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( com->ReplaceInitCond(system.Varlist(),zone,v,Buf1,
                                      Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1107)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1108)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(1109));               // EnterDelIndex
            Read(index,index); index--;
            if ( com->DeleteInitCond(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1110)); Write("\n");   // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1111)); Write("\n");   // Failure
            }
            break;
         default:
            break;
      }
   }
   return;
}

//////////////////////////////////////////////////////////////////////////////

static void ReadInput(AQUASYS& system, ADVCOMP* com)
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
      Write("\n   "); Write(scr.T(1112)); Write("\n  "); // EnterExpression
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
      Write("\n   "); Write(scr.T(1113)); Write(" ");    // InputFluxes
      Write(com->Symbol()); Write(":\n");
      if ( com->NumInput() > 0 )
      {
         Write("\n   "); Write(scr.T(685));             // Index
         Write("   "); Write(scr.T(1114));              // Variable
         Write("  :  "); Write(scr.T(1115));            // Input
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
      Write("\n   "); Write(scr.T(1116));               // Select
      Write("\n   1   =   "); Write(scr.T(1117));       // AddInput
      Write("\n   2   =   "); Write(scr.T(1118));       // EditInput
      Write("\n   3   =   "); Write(scr.T(1119));       // DeleteInput
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL numerr;
      CARDINAL index = 1;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(1120));         // EnterAddIndex
            index = com->NumInput()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(1121));         // EnterSymbol
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1122));       // EnterInputFlux
            Read(Buf1,BufLen,"0");
            if ( com->AddInput(system.Varlist(),v,Buf1,
                                Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1123)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1124)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(1125));          // EnterEditIndex
            Read(index,index); index--;
            if ( index >= com->NumInput() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                  // IllegalIndex
            }
            Write("\n   "); Write(scr.T(1121));          // EnterSymbol
            Read(Buf1,BufLen,(com->InputVar(index))->Symbol());
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1122));       // EnterInputFlux
            (com->InputFlux(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( com->ReplaceInput(system.Varlist(),v,Buf1,
                                   Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1126)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1127)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(1128));              // EnterDelIndex
            Read(index,index); index--;
            if ( com->DeleteInput(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1129)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1130)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }
   }

   n = 0;
   while ( n < 3 )
   {
      n++;
      if ( com->LatInflow() != 0 )
      {
         if ( (com->LatInflow())->UnParse(Buf2,BufLen-1) != 0 )
         {
            Buf2[0] = '0'; Buf2[1] = '\0';
         }
      }
      else
      {
            Buf2[0] = '0'; Buf2[1] = '\0';
      }
      Write("\n   "); Write(scr.T(890)); Write("\n  "); // EnterExpression
      Read(Buf1,BufLen,Buf2);
      if ( com->LatInflow(system.Varlist(),Buf1,Buf2) == TRUE ) break;
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
      Write("\n   "); Write(scr.T(891)); Write(" ");    // InputConc
      Write(com->Symbol()); Write(":\n");
      if ( com->NumLatInput() > 0 )
      {
         Write("\n   "); Write(scr.T(685));             // Index
         Write("   "); Write(scr.T(892));               // Variable
         Write("  :  "); Write(scr.T(893));             // InputConc
         for ( CARDINAL i=0; i<com->NumLatInput(); i++ )
         {
            Write("\n");
            NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
            Write(' ',strlen(scr.T(685)));
            v = com->LatInputVar(i); Write(v->Symbol()); Write("  :  ");
            const FORMVAR* cfv = com->LatInputConc(i);
            cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(894));                // Select
      Write("\n   1   =   "); Write(scr.T(895));        // AddInput
      Write("\n   2   =   "); Write(scr.T(896));        // EditInput
      Write("\n   3   =   "); Write(scr.T(897));        // DeleteInput
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL numerr;
      CARDINAL index = 1;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(898));          // EnterAddIndex
            index = com->NumLatInput()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(899));          // EnterSymbol
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(900));       // EnterInputConc
            Read(Buf1,BufLen,"0");
            if ( com->AddLatInput(system.Varlist(),v,Buf1,
                                Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(901)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(902)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(903));          // EnterEditIndex
            Read(index,index); index--;
            if ( index >= com->NumLatInput() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                  // IllegalIndex
            }
            Write("\n   "); Write(scr.T(899));          // EnterSymbol
            Read(Buf1,BufLen,(com->LatInputVar(index))->Symbol());
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(893));       // EnterInputConc
            (com->LatInputConc(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( com->ReplaceLatInput(system.Varlist(),v,Buf1,
                                   Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(904)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(905)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(906));              // EnterDelIndex
            Read(index,index); index--;
            if ( com->DeleteLatInput(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(907)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(908)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }
   }

   return;
}

//////////////////////////////////////////////////////////////////////////////

ADVCOMP* EditAdvComp(AQUASYS& system, ADVCOMP* advcomp)
{
   ReadSymbol(advcomp);

   ReadCompInd(advcomp);

   CARDINAL n = 0; REAL r,s;

   Write("\n   "); Write(scr.T(909));   // EnterStart
   Read(r,advcomp->XStart());

   Write("\n   "); Write(scr.T(910));   // EnterEnd
   Read(s,advcomp->XEnd()); advcomp->XStartEnd(r,s);

   CARDINAL numerr;
   n = 0;
   while (1)
   {
      if ( advcomp->CrossSect() != 0 )
      {
         if ( (advcomp->CrossSect())->UnParse(Buf2,BufLen-1) != 0 )
            Buf2[0] = '\0';
      }
      else
      {
         Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(912)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);               // EnterCrossSect
      if ( advcomp->CrossSect(Buf1,system.Varlist(),Buf2,numerr)
                                                            == TRUE )
      {
         break;                                  // Success
      }
      else
      {
         n++;
         if ( n > 2 )
         {
            delete advcomp; return 0;
         }
         if ( numerr == 0 )
         {                                       // IllegalDep
            Write("\n   "); Write(scr.T(911)); Write("\n");
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

   if ( advcomp->WithDiffusion() == TRUE ) Buf2[0] = '1';
   else                                    Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(913));          // Select
   Write("\n   1   =   "); Write(scr.T(914));  // WithDiffusion
   Write("\n   2   =   "); Write(scr.T(915));  // WithoutDiffusion
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0]=='1' )
   {
      n = 0;
      while (1)
      {
         if ( advcomp->GlobDiff() != 0 )
         {
            if ( (advcomp->GlobDiff())->UnParse(Buf2,BufLen-1) != 0 )
               Buf2[0] = '\0';
         }
         else
         {
            Buf2[0] = '\0';
         }
         Write("\n   "); Write(scr.T(916)); Write("\n  ");
         Read(Buf1,BufLen,Buf2);            // EnterGlobDiff
         if ( advcomp->GlobDiff(Buf1,system.Varlist(),Buf2,numerr)
                                                                == TRUE )
         {
            break;
         }
         else
         {
            n++;
            if ( n > 2 )
            {
               delete advcomp; return 0;
            }
            if ( numerr == 0 )
            {                                       // IllegalDep
               Write("\n   "); Write(scr.T(911)); Write("\n");
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
      advcomp->WithDiffusion(TRUE);

#ifdef DIFFTEST
      while (1)
      {
         AQVAR* v;
         Write("\n   "); Write(scr.T(1029)); Write(":\n"); // IndDiffs
         if ( advcomp->NumIndDiff() > 0 )
         {
            Write("\n   "); Write(scr.T(685));             // Index
            Write("   "); Write(scr.T(1030));              // Variable
            Write("  :  "); Write(scr.T(1031));            // IndDiff
            for ( CARDINAL i=0; i<advcomp->NumIndDiff(); i++ )
            {
               Write("\n");
               NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
               Write(' ',strlen(scr.T(685)));
               v = advcomp->IndDiffVar(i); Write(v->Symbol()); Write("  :  ");
               const FORMVAR* cfv = advcomp->IndDiffVal(i);
               cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
            }
            Write("\n");
         }
         Write("\n   "); Write(scr.T(1032));               // Select
         Write("\n   1   =   "); Write(scr.T(1033));       // AddIndDiff
         Write("\n   2   =   "); Write(scr.T(1034));       // EditIndDiff
         Write("\n   3   =   "); Write(scr.T(1035));       // DeleteIndDiff
         Write("\n");
         Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
         Read(Buf1,BufLen);
         if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
         CARDINAL numerr;
         CARDINAL index = 1;
         switch ( Buf1[0] )
         {
            case '1':
               Write("\n   "); Write(scr.T(1036));         // EnterAddIndex
               index = advcomp->NumIndDiff()+1; Read(index,index); index--;
               Write("\n   "); Write(scr.T(1037));         // EnterSymbol
               Read(Buf1,BufLen,"");
               v = system.GetVar(Buf1);
               if ( v == 0 )
               {
                  Write("\n   "); Write(scr.T(118)); Write(" ");
                  Write(Buf1); Write(" "); Write(scr.T(126));
                  Write("\n");                             // VarNotFound
                  break;
               }
               Write("\n   "); Write(scr.T(1038));      // EnterIndDiff
               Read(Buf1,BufLen,"0");
               if ( advcomp->AddIndDiff(system.Varlist(),v,Buf1,
                                        Buf2,numerr,index) == TRUE )
               {
                  Write("\n   "); Write(scr.T(1039)); Write("\n"); // Success
               }
               else
               {
                  Write("\n   "); Write(scr.T(1040)); Write("\n"); // Failure
               }
               break;
            case '2':
               Write("\n   "); Write(scr.T(1041));         // EnterEditIndex
               Read(index,index); index--;
               if ( index >= advcomp->NumIndDiff() )
               {
                   Write("\n   "); Write(scr.T(733)); Write("\n");
                   break;                                  // IllegalIndex
               }
               Write("\n   "); Write(scr.T(1037));         // EnterSymbol
               Read(Buf1,BufLen,(advcomp->IndDiffVar(index))->Symbol());
               v = system.GetVar(Buf1);
               if ( v == 0 )
               {
                  Write("\n   "); Write(scr.T(118)); Write(" ");
                  Write(Buf1); Write(" "); Write(scr.T(126));
                  Write("\n");                             // VarNotFound
                  break;
               }
               Write("\n   "); Write(scr.T(1038));       // EnterIndDiff
               (advcomp->IndDiffVal(index))->UnParse(Buf2,BufLen);
               Read(Buf1,BufLen,Buf2);
               if ( advcomp->ReplaceIndDiff(system.Varlist(),v,Buf1,
                                            Buf2,numerr,index) == TRUE )
               {
                  Write("\n   "); Write(scr.T(1042)); Write("\n"); // Success
               }
               else
               {
                  Write("\n   "); Write(scr.T(1043)); Write("\n"); // Failure
               }
               break;
            case '3':
               Write("\n   "); Write(scr.T(1044));             // EnterDelIndex
               Read(index,index); index--;
               if ( advcomp->DeleteIndDiff(index) == TRUE )
               {
                  Write("\n   "); Write(scr.T(1045)); Write("\n"); // Success
               }
               else
               {
                  Write("\n   "); Write(scr.T(1046)); Write("\n"); // Failure
               }
               break;
            default:
               break;
         }
      }
#endif /* DIFFTEST */

   }
   else
   {
      advcomp->WithDiffusion(FALSE);
   }

   Write("\n   "); Write(scr.T(917));   // EnterNumGrid
   Read(n,advcomp->NumGridPts()); advcomp->NumGridPts(n);
   if ( advcomp->HighRes() == FALSE ) Buf2[0] = '1';
   else                                 Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(918));          // Select
   Write("\n   1   =   "); Write(scr.T(919));  // LowRes
   Write("\n   2   =   "); Write(scr.T(920));  // HighRes
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0]=='1' ) advcomp->HighRes(FALSE);
   if ( Buf1[0]=='2' ) advcomp->HighRes(TRUE);

   if ( advcomp->Active() == TRUE ) Buf2[0] = '1';
   else                             Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(1395));                 // Status
   Write("\n   1   =   "); Write(scr.T(1396));         // Active
   Write("\n   2   =   "); Write(scr.T(1397));         // Inactive
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '2' ) advcomp->Active(FALSE);
   else                  advcomp->Active(TRUE);

   ReadActVar(system,advcomp);
   ReadActProc(system,advcomp);

   ReadInitCond(system,advcomp);
   ReadInput(system,advcomp);

   return advcomp;
}


//////////////////////////////////////////////////////////////////////////////
