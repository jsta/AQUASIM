////////////////////////////////   edriv_c.C   ///////////////////////////////
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
#include "edriv_c.h"

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

static BOOLEAN ReadGeo(AQUASYS& system, RIVCOMP* rivcomp);

static void    ReadInitCond(AQUASYS& system, RIVCOMP* com);
static void    ReadInput(AQUASYS& system, RIVCOMP* com);

//////////////////////////////////////////////////////////////////////////////

static void ReadInitCond(AQUASYS& system, RIVCOMP* com)
{
   while (1)
   {
      AQVAR* v;
      Write("\n   "); Write(scr.T(1170)); Write(" ");    // InitCond
      Write(com->Symbol()); Write(":\n");
      if ( com->NumInitCond() > 0 )
      {
         Write("\n   "); Write(scr.T(685));              // Index
         Write("   "); Write(scr.T(1171));               // Variable
         Write("("); Write(scr.T(1172)); Write(")");     // Zone
         Write("  :  "); Write(scr.T(1173));             // InitCond
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
      Write("\n   "); Write(scr.T(1174));             // Select
      Write("\n   1   =   "); Write(scr.T(1175));     // AddInitCond
      Write("\n   2   =   "); Write(scr.T(1176));     // EditInitCond
      Write("\n   3   =   "); Write(scr.T(1177));     // DeleteInitCond
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
            Write("\n   "); Write(scr.T(1178));            // EnterAddIndex
            index = com->NumInitCond()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(1179));            // EnterStateVar
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
               Write("\n   "); Write(scr.T(1180));         // SelectZone
               for ( CARDINAL i=0; i<com->NumZones(); i++ )
               {
                  NumToString(i+1,Buf1,BufLen,4);
                  Write("\n"); Write(Buf1); Write("   =   "); 
                  Write(com->ZoneName(i));                // ZoneName
               }
               Write("\n  "); Read(zone,zone+1); zone--;
               if ( zone >= com->NumZones() ) zone = com->NumZones()-1;
            }
            Write("\n   "); Write(scr.T(1181));            // EnterInitCond
            Read(Buf1,BufLen,"");
            
            if ( com->AddInitCond(system.Varlist(),zone,v,Buf1,
                                  Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1182)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1183)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(1184));            // EnterEditIndex
            Read(index,index); index--;
            if ( index >= com->NumInitCond() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                    // IllegalIndex
            }
            Write("\n   "); Write(scr.T(1179));            // EnterSymbol
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
               Write("\n   "); Write(scr.T(1180));        // SelectZone
               for ( CARDINAL i=0; i<com->NumZones(); i++ )
               {
                  NumToString(i+1,Buf1,BufLen,4);
                  Write("\n"); Write(Buf1); Write("   =   "); 
                  Write(com->ZoneName(i));                // ZoneName
               }
               Write("\n  "); Read(zone,com->InitZone(index)+1); zone--;
            }
            Write("\n   "); Write(scr.T(1181));           // EnterInitCond
            (com->InitVal(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( com->ReplaceInitCond(system.Varlist(),zone,v,Buf1,
                                      Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1185)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1186)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(1187));                // EnterDelIndex
            Read(index,index); index--;
            if ( com->DeleteInitCond(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1188)); Write("\n");   // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1189)); Write("\n");   // Failure
            }
            break;
         default:
            break;
      }
   }
   return;
}

//////////////////////////////////////////////////////////////////////////////

static void ReadInput(AQUASYS& system, RIVCOMP* com)
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
      Write("\n   "); Write(scr.T(1190)); Write("\n  "); // EnterExpression
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
      Write("\n   "); Write(scr.T(1191)); Write(" ");   // InputFluxes
      Write(com->Symbol()); Write(":\n");
      if ( com->NumInput() > 0 )
      {
         Write("\n   "); Write(scr.T(685));             // Index
         Write("   "); Write(scr.T(1192));              // Variable
         Write("  :  "); Write(scr.T(1193));            // Input
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
      Write("\n   "); Write(scr.T(1194));               // Select
      Write("\n   1   =   "); Write(scr.T(1195));       // AddInput
      Write("\n   2   =   "); Write(scr.T(1196));       // EditInput
      Write("\n   3   =   "); Write(scr.T(1197));       // DeleteInput
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL numerr;
      CARDINAL index = 1;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(1198));         // EnterAddIndex
            index = com->NumInput()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(1199));          // EnterSymbol
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1200));       // EnterInputFlux
            Read(Buf1,BufLen,"0");
            if ( com->AddInput(system.Varlist(),v,Buf1,
                                Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1201)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1202)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(1203));          // EnterEditIndex
            Read(index,index); index--;
            if ( index >= com->NumInput() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                  // IllegalIndex
            }
            Write("\n   "); Write(scr.T(1199));         // EnterSymbol
            Read(Buf1,BufLen,(com->InputVar(index))->Symbol());
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1200));      // EnterInputFlux
            (com->InputFlux(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( com->ReplaceInput(system.Varlist(),v,Buf1,
                                   Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1204)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1205)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(1206));             // EnterDelIndex
            Read(index,index); index--;
            if ( com->DeleteInput(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1207)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1208)); Write("\n"); // Failure
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
      Write("\n   "); Write(scr.T(847)); Write("\n  "); // EnterExpression
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
      Write("\n   "); Write(scr.T(848)); Write(" ");    // InputConc
      Write(com->Symbol()); Write(":\n");
      if ( com->NumLatInput() > 0 )
      {
         Write("\n   "); Write(scr.T(685));             // Index
         Write("   "); Write(scr.T(849));               // Variable
         Write("  :  "); Write(scr.T(850));             // InputConc
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
      Write("\n   "); Write(scr.T(851));                // Select
      Write("\n   1   =   "); Write(scr.T(852));        // AddInput
      Write("\n   2   =   "); Write(scr.T(853));        // EditInput
      Write("\n   3   =   "); Write(scr.T(854));        // DeleteInput
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL numerr;
      CARDINAL index = 1;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(855));          // EnterAddIndex
            index = com->NumLatInput()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(856));          // EnterSymbol
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(857));       // EnterInputConc
            Read(Buf1,BufLen,"0");
            if ( com->AddLatInput(system.Varlist(),v,Buf1,
                                Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(858)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(859)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(860));          // EnterEditIndex
            Read(index,index); index--;
            if ( index >= com->NumLatInput() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                  // IllegalIndex
            }
            Write("\n   "); Write(scr.T(861));          // EnterSymbol
            Read(Buf1,BufLen,(com->LatInputVar(index))->Symbol());
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(857));       // EnterInputConc
            (com->LatInputConc(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( com->ReplaceLatInput(system.Varlist(),v,Buf1,
                                   Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(861)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(862)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(863));              // EnterDelIndex
            Read(index,index); index--;
            if ( com->DeleteLatInput(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(864)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(865)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }
   }

   return;
}

//////////////////////////////////////////////////////////////////////////////

static BOOLEAN ReadGeo(AQUASYS& system, RIVCOMP* rivcomp)
{
   REAL r,s;

   do
   {
      Write("\n   "); Write(scr.T(258));   // EnterStart
      Read(r,rivcomp->XStart());
      Write("\n   "); Write(scr.T(259));   // EnterEnd
      Read(s,rivcomp->XEnd());
   } while ( rivcomp->XStartEnd(r,s) == FALSE );

   do
   {
      Write("\n   "); Write(scr.T(1052));  // EnterGravit
      Read(r,rivcomp->Gravit());
   } while ( rivcomp->Gravit(r) == FALSE );

   CARDINAL numerr;
   CARDINAL n = 0;
   while (1)
   {
      if ( rivcomp->CrossSect() != 0 )
      {
         if ( (rivcomp->CrossSect())->UnParse(Buf2,BufLen-1) != 0 )
            Buf2[0] = '\0';
      }
      else
      {
         Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(260)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);               // EnterCrossSect
      if ( rivcomp->CrossSect(Buf1,system.Varlist(),Buf2,numerr)
                                                            == TRUE )
      {
         break;                                  // Success
      }
      else
      {
         n++;
         if ( n > 2 )
         {
            delete rivcomp; return FALSE;
         }
         if ( numerr == 0 )
         {                                       // IllegalDep
            Write("\n   "); Write(scr.T(421)); Write("\n");
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

   n = 0;
   while (1)
   {
      if ( rivcomp->Peri() != 0 )
      {
         if ( (rivcomp->Peri())->UnParse(Buf2,BufLen-1) != 0 )
            Buf2[0] = '\0';
      }
      else
      {
         Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(261)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);                // EnterWettPerimeter
      if ( rivcomp->Peri(Buf1,system.Varlist(),Buf2,numerr)
                                                           == TRUE )
      {
         break;
      }
      else
      {
         n++;
         if ( n > 2 )
         {
            delete rivcomp; return FALSE;
         }
         if ( numerr == 0 )
         {                                       // IllegalDep
            Write("\n   "); Write(scr.T(421)); Write("\n");
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

   n = 0;
   while (1)
   {
      if ( rivcomp->Wi() != 0 )
      {
         if ( (rivcomp->Wi())->UnParse(Buf2,BufLen-1) != 0 )
            Buf2[0] = '\0';
      }
      else
      {
         Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(262)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);            // EnterSurfWidth
      if ( rivcomp->Wi(Buf1,system.Varlist(),Buf2,numerr) == TRUE )
      {
         break;
      }
      else
      {
         n++;
         if ( n > 2 )
         {
            delete rivcomp; return FALSE;
         }
         if ( numerr == 0 )
         {                                       // IllegalDep
            Write("\n   "); Write(scr.T(421)); Write("\n");
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
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

RIVCOMP* EditRivComp(AQUASYS& system, RIVCOMP* rivcomp)
{
   ReadSymbol(rivcomp);

   ReadCompInd(rivcomp);

   if ( ReadGeo(system,rivcomp) == FALSE ) return 0;

   CARDINAL numerr;
   CARDINAL n = 0;
   while (1)
   {
      if ( rivcomp->FrictionSlope() != 0 )
      {
         if ( (rivcomp->FrictionSlope())->UnParse(Buf2,BufLen-1)
                                                              != 0 )
            Buf2[0] = '\0';
      }
      else
      {
         Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(215)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);            // EnterFrictSlope
      if ( rivcomp->FrictionSlope(Buf1,system.Varlist(),Buf2,numerr)
                                                            == TRUE )
      {
         break;
      }
      else
      {
         n++;
         if ( n > 2 )
         {
            delete rivcomp; return 0;
         }
         if ( numerr == 0 )
         {                                       // IllegalDep
            Write("\n   "); Write(scr.T(421)); Write("\n");
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

   if ( rivcomp->WithDispersion() == TRUE ) Buf2[0] = '1';
   else                                       Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(635));          // Select
   Write("\n   1   =   "); Write(scr.T(636));  // WithDispersion
   Write("\n   2   =   "); Write(scr.T(637));  // WithoutDispersion
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0]=='1' )
   {
      n = 0;
      while (1)
      {
         if ( rivcomp->Dispersion() != 0 )
         {
            if ( (rivcomp->Dispersion())->UnParse(Buf2,BufLen-1) != 0 )
               Buf2[0] = '\0';
         }
         else
         {
            Buf2[0] = '\0';
         }
         Write("\n   "); Write(scr.T(420)); Write("\n  ");
         Read(Buf1,BufLen,Buf2);            // EnterDispersion
         if ( rivcomp->Dispersion(Buf1,system.Varlist(),Buf2,numerr)
                                                                == TRUE )
         {
            break;
         }
         else
         {
            n++;
            if ( n > 2 )
            {
               delete rivcomp; return 0;
            }
            if ( numerr == 0 )
            {                                       // IllegalDep
               Write("\n   "); Write(scr.T(421)); Write("\n");
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
      rivcomp->WithDispersion(TRUE);
   }
   else
   {
      rivcomp->WithDispersion(FALSE);
   }

   switch ( rivcomp->EndLevelType() )
   {
      case NormalLevel:   Buf2[0] = '1'; break;
      case CriticalLevel: Buf2[0] = '2'; break;
      case GivenLevel:    Buf2[0] = '3'; break;
      default:
         FatalError("aquaedit::NewComp: unknown EndLevelType");
   }
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(555));          // Select
   Write("\n   1   =   "); Write(scr.T(556));  // NormalLevel
   Write("\n   2   =   "); Write(scr.T(557));  // CriticalLevel
   Write("\n   3   =   "); Write(scr.T(558));  // GivenLevel
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0]=='1' )
   {
      rivcomp->EndLevelType(NormalLevel);
   }
   if ( Buf1[0]=='2' )
   {
      rivcomp->EndLevelType(CriticalLevel);
   }
   if ( Buf1[0]=='3' )
   {
      n = 0;
      while (1)
      {
         if ( rivcomp->EndLevel() != 0 )
         {
            if ( (rivcomp->EndLevel())->UnParse(Buf2,BufLen-1) != 0 )
               Buf2[0] = '\0';
         }
         else
         {
            Buf2[0] = '\0';
         }
         Write("\n   "); Write(scr.T(563)); Write("\n  ");
         Read(Buf1,BufLen,Buf2);            // EnterEndLevel
         if ( rivcomp->EndLevel(Buf1,system.Varlist(),Buf2,numerr)
                                                                == TRUE )
         {
            break;
         }
         else
         {
            n++;
            if ( n > 2 )
            {
               delete rivcomp; return 0;
            }
            if ( numerr == 0 )
            {                                       // IllegalDep
               Write("\n   "); Write(scr.T(421)); Write("\n");
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
      rivcomp->EndLevelType(GivenLevel);
   }

   switch ( rivcomp->Method() )
   {
      case KinWave:  Buf2[0] = '1'; break;
      case DiffWave: Buf2[0] = '2'; break;
      default:
         FatalError("aquaedit::NewComp: unknown Method");
   }
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(559));          // Select
   Write("\n   1   =   "); Write(scr.T(560));  // KinWave
   Write("\n   2   =   "); Write(scr.T(561));  // DiffWave
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0]=='1' )
   {
      rivcomp->Method(KinWave);
   }
   if ( Buf1[0]=='2' )
   {
      rivcomp->Method(DiffWave);
   }

   // new section for sediment layers 29.12.98:

   if ( rivcomp->CalcSed() == FALSE ) Buf2[0] = '1';
   else                               Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(1407));          // Select
   Write("\n   1   =   "); Write(scr.T(1408));  // calcSedno
   Write("\n   2   =   "); Write(scr.T(1409));  // calcSedyes
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0]=='1' ) rivcomp->CalcSed(FALSE);
   if ( Buf1[0]=='2' ) rivcomp->CalcSed(TRUE);

   if ( rivcomp->CalcSed() == TRUE )
   {
      while (1)
      {
         Write("\n   "); Write(scr.T(1410));            // SedLayers
         Write(":\n");
         if ( rivcomp->NumSedLayers() > 0 )
         {
            Write("\n   "); Write(scr.T(685));          // Index
            Write("   "); Write(scr.T(1411));           // ZoneInd
            Write("  :  "); Write(scr.T(1412));         // Area
            Write(", "); Write(scr.T(1413));            // Porosity
            Write(", "); Write(scr.T(1414));            // Discharge
            Write(", "); Write(scr.T(1434));            // Dispersion
            Write(", "); Write(scr.T(1415));            // ExchCoeff
            for ( CARDINAL i=0; i<rivcomp->NumSedLayers(); i++ )
            {
               Write("\n");
               NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
               Write(' ',strlen(scr.T(685)));
               NumToString(rivcomp->SedLayer(i)->ZoneIndex(),Buf1,BufLen);
			   Write(Buf1); Write("  :  ");
               const FORMVAR* cfv = rivcomp->SedLayer(i)->Area();
               cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
               Write(", ");
               cfv = rivcomp->SedLayer(i)->Porosity();
               cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
               Write(", ");
               cfv = rivcomp->SedLayer(i)->Discharge();
               cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
               Write(", ");
               cfv = rivcomp->SedLayer(i)->Dispersion();
               cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
               Write(", ");
               cfv = rivcomp->SedLayer(i)->ExchCoeff();
               cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
            }
            Write("\n");
         }
         Write("\n   "); Write(scr.T(1416));               // Select
         Write("\n   1   =   "); Write(scr.T(1417));       // AddLayer
         Write("\n   2   =   "); Write(scr.T(1418));       // EditLayer
         Write("\n   3   =   "); Write(scr.T(1419));       // DeleteLayer
         Write("\n");
         Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
         Read(Buf1,BufLen);
         if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
         CARDINAL numerr;
         CARDINAL index = 1;
         CARDINAL c;
         switch ( Buf1[0] )
         {
            case '1':
               {
                  RIVSEDLAYER* newlayer = new RIVSEDLAYER;
                  Write("\n   "); Write(scr.T(1420));          // EnterAddIndex
                  index = rivcomp->NumSedLayers()+1; Read(index,index); index--;
                  do
                  {
                     Write("\n   "); Write(scr.T(1421));   // EnterZoneInd
                     Read(c,newlayer->ZoneIndex());
                  } while ( newlayer->ZoneIndex(c) == FALSE );
                  do
                  {
                     Write("\n   "); Write(scr.T(1422));   // EnterArea
                     (newlayer->Area())->UnParse(Buf2,BufLen);
                     Read(Buf1,BufLen,Buf2);
				  } while ( newlayer->Area(Buf1,system.Varlist(),Buf2,numerr) == FALSE );
                  do
                  {
                     Write("\n   "); Write(scr.T(1423));   // EnterPorosity
                     (newlayer->Porosity())->UnParse(Buf2,BufLen);
                     Read(Buf1,BufLen,Buf2);
				  } while ( newlayer->Porosity(Buf1,system.Varlist(),Buf2,numerr) == FALSE );
                  do
                  {
                     Write("\n   "); Write(scr.T(1424));   // EnterDischarge
                     (newlayer->Discharge())->UnParse(Buf2,BufLen);
                     Read(Buf1,BufLen,Buf2);
				  } while ( newlayer->Discharge(Buf1,system.Varlist(),Buf2,numerr) == FALSE );
                  do
                  {
                     Write("\n   "); Write(scr.T(1435));   // EnterDispersion
                     (newlayer->Dispersion())->UnParse(Buf2,BufLen);
                     Read(Buf1,BufLen,Buf2);
				  } while ( newlayer->Dispersion(Buf1,system.Varlist(),Buf2,numerr) == FALSE );
                  do
                  {
                     Write("\n   "); Write(scr.T(1425));   // EnterExchCoeff
                     (newlayer->ExchCoeff())->UnParse(Buf2,BufLen);
                     Read(Buf1,BufLen,Buf2);
				  } while ( newlayer->ExchCoeff(Buf1,system.Varlist(),Buf2,numerr) == FALSE );
                  if ( rivcomp->AddSedLayer(newlayer,index) == TRUE )
                  {
                     Write("\n   "); Write(scr.T(1426)); Write("\n"); // Success
                  }
                  else
                  {
                     Write("\n   "); Write(scr.T(1427)); Write("\n"); // Failure
                     delete newlayer;
                  }
               }
               break;
            case '2':
               {
                  if ( rivcomp->NumSedLayers() == 0 ) break;
                  do
                  {
                     Write("\n   "); Write(scr.T(1428));          // EnterEditIndex
                     index = 1; Read(index,index); index--;
                  } while ( index >= rivcomp->NumSedLayers() );
                  RIVSEDLAYER* newlayer = new RIVSEDLAYER(rivcomp->SedLayer(index));
                  do
                  {
                     Write("\n   "); Write(scr.T(1421));   // EnterZoneInd
                     Read(c,newlayer->ZoneIndex());
                  } while ( newlayer->ZoneIndex(c) == FALSE );
                  do
                  {
                     Write("\n   "); Write(scr.T(1422));   // EnterArea
                     (newlayer->Area())->UnParse(Buf2,BufLen);
                     Read(Buf1,BufLen,Buf2);
				  } while ( newlayer->Area(Buf1,system.Varlist(),Buf2,numerr) == FALSE );
                  do
                  {
                     Write("\n   "); Write(scr.T(1423));   // EnterPorosity
                     (newlayer->Porosity())->UnParse(Buf2,BufLen);
                     Read(Buf1,BufLen,Buf2);
				  } while ( newlayer->Porosity(Buf1,system.Varlist(),Buf2,numerr) == FALSE );
                  do
                  {
                     Write("\n   "); Write(scr.T(1424));   // EnterDischarge
                     (newlayer->Discharge())->UnParse(Buf2,BufLen);
                     Read(Buf1,BufLen,Buf2);
				  } while ( newlayer->Discharge(Buf1,system.Varlist(),Buf2,numerr) == FALSE );
                  do
                  {
                     Write("\n   "); Write(scr.T(1435));   // EnterDispersion
                     (newlayer->Dispersion())->UnParse(Buf2,BufLen);
                     Read(Buf1,BufLen,Buf2);
				  } while ( newlayer->Dispersion(Buf1,system.Varlist(),Buf2,numerr) == FALSE );
                  do
                  {
                     Write("\n   "); Write(scr.T(1425));   // EnterExchcoeff
                     (newlayer->ExchCoeff())->UnParse(Buf2,BufLen);
                     Read(Buf1,BufLen,Buf2);
				  } while ( newlayer->ExchCoeff(Buf1,system.Varlist(),Buf2,numerr) == FALSE );
                  if ( rivcomp->ReplaceSedLayer(newlayer,index) == TRUE )
                  {
                     Write("\n   "); Write(scr.T(1429)); Write("\n"); // Success
                  }
                  else
                  {
                     Write("\n   "); Write(scr.T(1430)); Write("\n"); // Failure
                     delete newlayer;
                  }
               }
               break;
            case '3':
               Write("\n   "); Write(scr.T(1431));              // EnterDelIndex
               Read(index,index); index--;
               if ( rivcomp->DeleteSedLayer(index) == TRUE )
               {
                  Write("\n   "); Write(scr.T(1432)); Write("\n"); // Success
               }
               else
               {
                  Write("\n   "); Write(scr.T(1433)); Write("\n"); // Failure
               }
               break;
            default:
               break;
         }
      }
   }
   
   // end of new section for sediment layers
   
   Write("\n   "); Write(scr.T(257));   // EnterNumGrid
   Read(n,rivcomp->NumGridPts()); rivcomp->NumGridPts(n);
   if ( rivcomp->HighRes() == FALSE ) Buf2[0] = '1';
   else                                 Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(654));          // Select
   Write("\n   1   =   "); Write(scr.T(655));  // LowRes
   Write("\n   2   =   "); Write(scr.T(656));  // HighRes
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0]=='1' ) rivcomp->HighRes(FALSE);
   if ( Buf1[0]=='2' ) rivcomp->HighRes(TRUE);

   if ( rivcomp->Active() == TRUE ) Buf2[0] = '1';
   else                             Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(1395));                 // Status
   Write("\n   1   =   "); Write(scr.T(1396));         // Active
   Write("\n   2   =   "); Write(scr.T(1397));         // Inactive
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '2' ) rivcomp->Active(FALSE);
   else                  rivcomp->Active(TRUE);

   ReadActVar(system,rivcomp);
   ReadActProc(system,rivcomp);

   ReadInitCond(system,rivcomp);
   ReadInput(system,rivcomp);

   return rivcomp;
}


//////////////////////////////////////////////////////////////////////////////
