///////////////////////////////   edfilm_c.C   ///////////////////////////////
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
#include "edfilm_c.h"

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

static void ReadInitCond(AQUASYS& system, FILMCOMP* com);
static void ReadInput(AQUASYS& system, FILMCOMP* com);

//////////////////////////////////////////////////////////////////////////////

static void ReadInitCond(AQUASYS& system, FILMCOMP* com)
{
   while (1)
   {
      AQVAR* v;
      Write("\n   "); Write(scr.T(1072)); Write(" ");    // InitCond
      Write(com->Symbol()); Write(":\n");
      if ( com->NumInitCond() > 0 )
      {
         Write("\n   "); Write(scr.T(685));              // Index
         Write("   "); Write(scr.T(1073));               // Variable
         Write("("); Write(scr.T(1074)); Write(")");     // Zone
         Write("  :  "); Write(scr.T(1075));             // InitCond
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
      Write("\n   "); Write(scr.T(1076));             // Select
      Write("\n   1   =   "); Write(scr.T(1077));     // AddInitCond
      Write("\n   2   =   "); Write(scr.T(1078));     // EditInitCond
      Write("\n   3   =   "); Write(scr.T(1079));     // DeleteInitCond
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
            Write("\n   "); Write(scr.T(1080));            // EnterAddIndex
            index = com->NumInitCond()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(1081));            // EnterStateVar
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
               Write("\n   "); Write(scr.T(1082));        // SelectZone
               for ( CARDINAL i=0; i<com->NumZones(); i++ )
               {
                  NumToString(i+1,Buf1,BufLen,4);
                  Write("\n"); Write(Buf1); Write("   =   "); 
                  Write(com->ZoneName(i));                // ZoneName
               }
               Write("\n  "); Read(zone,zone+1); zone--;
               if ( zone >= com->NumZones() ) zone = com->NumZones()-1;
            }
            Write("\n   "); Write(scr.T(1083));           // EnterInitCond
            Read(Buf1,BufLen,"");
            
            if ( com->AddInitCond(system.Varlist(),zone,v,Buf1,
                                  Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1084)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1085)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(1086));            // EnterEditIndex
            Read(index,index); index--;
            if ( index >= com->NumInitCond() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                    // IllegalIndex
            }
            Write("\n   "); Write(scr.T(1081));            // Symbol
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
               Write("\n   "); Write(scr.T(1082));         // SelectZone
               for ( CARDINAL i=0; i<com->NumZones(); i++ )
               {
                  NumToString(i+1,Buf1,BufLen,4);
                  Write("\n"); Write(Buf1); Write("   =   "); 
                  Write(com->ZoneName(i));                // ZoneName
               }
               Write("\n  "); Read(zone,com->InitZone(index)+1); zone--;
            }
            Write("\n   "); Write(scr.T(1083));           // EnterInitCond
            (com->InitVal(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( com->ReplaceInitCond(system.Varlist(),zone,v,Buf1,
                                      Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1087)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1088)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(1089));                // EnterDelIndex
            Read(index,index); index--;
            if ( com->DeleteInitCond(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1090)); Write("\n");   // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1091)); Write("\n");   // Failure
            }
            break;
         default:
            break;
      }
   }
   return;
}

//////////////////////////////////////////////////////////////////////////////

static void ReadInput(AQUASYS& system, FILMCOMP* com)
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
      Write("\n   "); Write(scr.T(1053)); Write("\n  "); // EnterExpression
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
      Write("\n   "); Write(scr.T(1054)); Write(" ");   // InputFluxes
      Write(com->Symbol()); Write(":\n");
      if ( com->NumInput() > 0 )
      {
         Write("\n   "); Write(scr.T(685));             // Index
         Write("   "); Write(scr.T(1055));              // Variable
         Write("  :  "); Write(scr.T(1056));            // Input
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
      Write("\n   "); Write(scr.T(1057));               // Select
      Write("\n   1   =   "); Write(scr.T(1058));       // AddInput
      Write("\n   2   =   "); Write(scr.T(1059));       // EditInput
      Write("\n   3   =   "); Write(scr.T(1060));       // DeleteInput
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL numerr;
      CARDINAL index = 1;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(1061));         // EnterAddIndex
            index = com->NumInput()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(1062));         // EnterSymbol
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1063));       // EnterInputFlux
            Read(Buf1,BufLen,"0");
            if ( com->AddInput(system.Varlist(),v,Buf1,
                                Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1064)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1065)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(1066));          // EnterEditIndex
            Read(index,index); index--;
            if ( index >= com->NumInput() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                  // IllegalIndex
            }
            Write("\n   "); Write(scr.T(1062));         // EnterSymbol
            Read(Buf1,BufLen,(com->InputVar(index))->Symbol());
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1063));      // EnterInputFlux
            (com->InputFlux(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( com->ReplaceInput(system.Varlist(),v,Buf1,
                                   Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1067)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1068)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(1069));              // EnterDelIndex
            Read(index,index); index--;
            if ( com->DeleteInput(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1070)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1071)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }
   }
   return;
}

//////////////////////////////////////////////////////////////////////////////

FILMCOMP* EditFilmComp(AQUASYS& system, FILMCOMP* filmcomp)
{
   ReadSymbol(filmcomp);

   ReadCompInd(filmcomp);

   while ( 1 )
   {
      if ( filmcomp->NX() > 0 )
      {
         Write("\n   "); Write(scr.T(492)); Write(":\n"); // PartList
         for ( CARDINAL i=0; i<filmcomp->NX(); i++ )
         {
            if ( i % scr.I(1) == 0 ) Write("\n   ");
            const char* sym = filmcomp->Part(i)->Symbol();
            Write(sym); Write(' ',scr.L(1)-strlen(sym));
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(493)); Write(":");     // Select
      Write("\n   1   =   "); Write(scr.T(494));         // AddPart
      Write("\n   2   =   "); Write(scr.T(495));         // EditPart
      Write("\n   3   =   "); Write(scr.T(496));         // DelPart
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      AQVAR* var = 0;
      switch (Buf1[0])
      {
         case '1':
            Write("\n   "); Write(scr.T(497)); Write(": ");
            Read(Buf,BufLen);                         // EnterSymbol
            var = system.GetVar(Buf);
            if ( filmcomp->AddX(var) == FALSE )
            {
                Write("\n   "); Write(scr.T(498)); Write("\n");
            }                                         // AddFailure
            break;
         case '2':
            Write("\n   "); Write(scr.T(501)); Write(": ");
            Read(Buf,BufLen);                         // EnterSymbol
            var = system.GetVar(Buf);
            CARDINAL i;
            for ( i=0; i<filmcomp->NX(); i++ )
            {
               if ( filmcomp->Part(i) == var )
               {
                  CARDINAL numerr;
                  CARDINAL n = 0;
                  while (1)
                  {
                     if ( filmcomp->Rho(i)
                                   ->UnParse(Buf2,BufLen-1) != 0 )
                        Buf2[0] = '\0';
                     Write("\n   "); Write(scr.T(503)); Write("\n   ");
                     Read(Buf1,BufLen,Buf2);        // Enter rho
                     if ( filmcomp->Rho(var,Buf1,system.Varlist(),
                                        Buf2,numerr) == TRUE )
                     {
                         break;
                     }
                     else
                     {
                        n++;
                        if ( n > 2 )
                        {
                           delete filmcomp; return 0;
                        }
                        Write("\n   "); Write(scr.T(504));
                        Write("\n");
                     }
                  }
                  n = 0;
                  while (1)
                  {
                     if ( filmcomp->SurfKat(i)
                                   ->UnParse(Buf2,BufLen-1) != 0 )
                        Buf2[0] = '\0';
                     Write("\n   "); Write(scr.T(505)); Write("\n   ");
                     Read(Buf1,BufLen,Buf2);        // Enter surfkat
                     if ( filmcomp->SurfKat(var,Buf1,system.Varlist(),
                                        Buf2,numerr) == TRUE )
                     {
                         break;
                     }
                     else
                     {
                        n++;
                        if ( n > 2 )
                        {
                           delete filmcomp; return 0;
                        }
                        Write("\n   "); Write(scr.T(506));
                        Write("\n");
                     }
                  }
                  n = 0;
                  while (1)
                  {
                     if ( filmcomp->SurfKde(i)
                                   ->UnParse(Buf2,BufLen-1) != 0 )
                        Buf2[0] = '\0';
                     Write("\n   "); Write(scr.T(507)); Write("\n   ");
                     Read(Buf1,BufLen,Buf2);        // Enter surfkde
                     if ( filmcomp->SurfKde(var,Buf1,system.Varlist(),
                                        Buf2,numerr) == TRUE )
                     {
                         break;
                     }
                     else
                     {
                        n++;
                        if ( n > 2 )
                        {
                           delete filmcomp; return 0;
                        }
                        Write("\n   "); Write(scr.T(508));
                        Write("\n");
                     }
                  }
                  n = 0;
                  while (1)
                  {
                     if ( filmcomp->VolKat(i)
                                   ->UnParse(Buf2,BufLen-1) != 0 )
                        Buf2[0] = '\0';
                     Write("\n   "); Write(scr.T(1253)); Write("\n   ");
                     Read(Buf1,BufLen,Buf2);        // Enter volkat
                     if ( filmcomp->VolKat(var,Buf1,system.Varlist(),
                                           Buf2,numerr) == TRUE )
                     {
                         break;
                     }
                     else
                     {
                        n++;
                        if ( n > 2 )
                        {
                           delete filmcomp; return 0;
                        }
                        Write("\n   "); Write(scr.T(1254));
                        Write("\n");
                     }
                  }
                  n = 0;
                  while (1)
                  {
                     if ( filmcomp->VolKde(i)
                                   ->UnParse(Buf2,BufLen-1) != 0 )
                        Buf2[0] = '\0';
                     Write("\n   "); Write(scr.T(1255)); Write("\n   ");
                     Read(Buf1,BufLen,Buf2);        // Enter volkde
                     if ( filmcomp->VolKde(var,Buf1,system.Varlist(),
                                           Buf2,numerr) == TRUE )
                     {
                         break;
                     }
                     else
                     {
                        n++;
                        if ( n > 2 )
                        {
                           delete filmcomp; return 0;
                        }
                        Write("\n   "); Write(scr.T(1256));
                        Write("\n");
                     }
                  }
                  n = 0;
                  while (1)
                  {
                     if ( filmcomp->ResX(i)
                                   ->UnParse(Buf2,BufLen-1) != 0 )
                        Buf2[0] = '\0';
                     Write("\n   "); Write(scr.T(509)); Write("\n   ");
                     Read(Buf1,BufLen,Buf2);        // Enter ResX
                     if ( filmcomp->ResX(var,Buf1,system.Varlist(),
                                         Buf2,numerr) == TRUE )
                     {
                         break;
                     }
                     else
                     {
                        n++;
                        if ( n > 2 )
                        {
                           delete filmcomp; return 0;
                        }
                        Write("\n   "); Write(scr.T(510));
                        Write("\n");
                     }
                  }
                  n = 0;
                  while (1)
                  {
                     if ( filmcomp->PoreDiffX(i)
                                   ->UnParse(Buf2,BufLen-1) != 0 )
                        Buf2[0] = '\0';
                     Write("\n   "); Write(scr.T(1257)); Write("\n   ");
                     Read(Buf1,BufLen,Buf2);        // Enter DXpore
                     if ( filmcomp->PoreDiffX(var,Buf1,system.Varlist(),
                                              Buf2,numerr) == TRUE )
                     {
                         break;
                     }
                     else
                     {
                        n++;
                        if ( n > 2 )
                        {
                           delete filmcomp; return 0;
                        }
                        Write("\n   "); Write(scr.T(1258));
                        Write("\n");
                     }
                  }
                  n = 0;
                  while (1)
                  {
                     if ( filmcomp->MatrixDiffX(i)
                                   ->UnParse(Buf2,BufLen-1) != 0 )
                        Buf2[0] = '\0';
                     Write("\n   "); Write(scr.T(511)); Write("\n   ");
                     Read(Buf1,BufLen,Buf2);        // Enter DXmat
                     if ( filmcomp->MatrixDiffX(var,Buf1,system.Varlist(),
                                                Buf2,numerr) == TRUE )
                     {
                         break;
                     }
                     else
                     {
                        n++;
                        if ( n > 2 )
                        {
                           delete filmcomp; return 0;
                        }
                        Write("\n   "); Write(scr.T(512));
                        Write("\n");
                     }
                  }
                  break;
               }
            }
            if ( i >= filmcomp->NX() )
            {
               Write("\n   "); Write(scr.T(502)); Write("\n");
            }                                         // PartNotFound
            break;
         case '3':
            Write("\n   "); Write(scr.T(499)); Write(": ");
            Read(Buf,BufLen);                         // EnterSymbol
            var = system.GetVar(Buf);
            if ( filmcomp->DelX(var) == FALSE )
            {
                Write("\n   "); Write(scr.T(500)); Write("\n");
            }                                         // DelFailure
            break;
      }
   }

   while ( 1 )
   {
      if ( filmcomp->NS() > 0 )
      {
         Write("\n   "); Write(scr.T(513)); Write(":\n"); // DissList
         for ( CARDINAL i=0; i<filmcomp->NS(); i++ )
         {
            if ( i % scr.I(1) == 0 ) Write("\n   ");
            const char* sym = filmcomp->Diss(i)->Symbol();
            Write(sym); Write(' ',scr.L(1)-strlen(sym));
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(514)); Write(":");     // Select
      Write("\n   1   =   "); Write(scr.T(515));         // AddDiss
      Write("\n   2   =   "); Write(scr.T(516));         // EditDiss
      Write("\n   3   =   "); Write(scr.T(517));         // DelDiss
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      AQVAR* var = 0;
      switch (Buf1[0])
      {
         case '1':
            Write("\n   "); Write(scr.T(518)); Write(": ");
            Read(Buf,BufLen);                         // EnterSymbol
            var = system.GetVar(Buf);
            if ( filmcomp->AddS(var) == FALSE )
            {
                Write("\n   "); Write(scr.T(519)); Write("\n");
            }                                         // AddFailure
            break;
         case '2':
            Write("\n   "); Write(scr.T(522)); Write(": ");
            Read(Buf,BufLen);                         // EnterSymbol
            var = system.GetVar(Buf);
            CARDINAL i;
            for ( i=0; i<filmcomp->NS(); i++ )
            {
               if ( filmcomp->Diss(i) == var )
               {
                  CARDINAL numerr;
                  CARDINAL n = 0;
                  while (1)
                  {
                     if ( filmcomp->ResS(i)
                                   ->UnParse(Buf2,BufLen-1) != 0 )
                        Buf2[0] = '\0';
                     Write("\n   "); Write(scr.T(524)); Write("\n   ");
                     Read(Buf1,BufLen,Buf2);        // Enter ResS
                     if ( filmcomp->ResS(var,Buf1,system.Varlist(),
                                         Buf2,numerr) == TRUE )
                     {
                         break;
                     }
                     else
                     {
                        n++;
                        if ( n > 2 )
                        {
                           delete filmcomp; return 0;
                        }
                        Write("\n   "); Write(scr.T(525));
                        Write("\n");
                     }
                  }
                  n = 0;
                  while (1)
                  {
                     if ( filmcomp->PoreDiffS(i)
                                   ->UnParse(Buf2,BufLen-1) != 0 )
                        Buf2[0] = '\0';
                     Write("\n   "); Write(scr.T(526)); Write("\n   ");
                     Read(Buf1,BufLen,Buf2);        // Enter DS
                     if ( filmcomp->PoreDiffS(var,Buf1,system.Varlist(),
                                              Buf2,numerr) == TRUE )
                     {
                         break;
                     }
                     else
                     {
                        n++;
                        if ( n > 2 )
                        {
                           delete filmcomp; return 0;
                        }
                        Write("\n   "); Write(scr.T(527));
                        Write("\n");
                     }
                  }
                  break;
               }
            }
            if ( i >= filmcomp->NS() )
            {
               Write("\n   "); Write(scr.T(523)); Write("\n");
            }                                         // DissNotFound
            break;
         case '3':
            Write("\n   "); Write(scr.T(520)); Write(": ");
            Read(Buf,BufLen);                         // EnterSymbol
            var = system.GetVar(Buf);
            if ( filmcomp->DelS(var) == FALSE )
            {
                Write("\n   "); Write(scr.T(521)); Write("\n");
            }                                         // DelFailure
            break;
      }
   }

   Buf2[0] = '1';
   if ( filmcomp->Confined() == FALSE ) Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(532));          // SelVolType
   Write("\n   1   =   "); Write(scr.T(533));  // Confined
   Write("\n   2   =   "); Write(scr.T(534));  // Unconfined
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0]=='1' )
   {
       REAL vol;
       filmcomp->Confined(TRUE);
       Write("\n   "); Write(scr.T(535));      // EnterReactVol
       Read(vol,filmcomp->ReactVol());
       filmcomp->ReactVol(vol);
   }
   else
   {
       REAL vol;
       filmcomp->Confined(FALSE);
       Write("\n   "); Write(scr.T(536));      // EnterBulkVol
       Read(vol,filmcomp->BulkVol());
       filmcomp->BulkVol(vol);
   }

   Buf2[0] = '1';
   if ( filmcomp->PoreParticles() == TRUE ) Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(1250));          // SelPoreType
   Write("\n   1   =   "); Write(scr.T(1251));  // PoreLiquidOnly
   Write("\n   2   =   "); Write(scr.T(1252));  // PoreWithPart
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0]=='1' )
   {
       filmcomp->PoreParticles(FALSE);
   }
   else
   {
       filmcomp->PoreParticles(TRUE);
   }

   Buf2[0] = '1';
   if ( filmcomp->MatrixDiff() == TRUE ) Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(643));          // SelMatrixType
   Write("\n   1   =   "); Write(scr.T(644));  // Rigid
   Write("\n   2   =   "); Write(scr.T(645));  // Diffusive
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   BOOLEAN readude = TRUE;
   if ( Buf1[0]=='1' )
   {
       filmcomp->MatrixDiff(FALSE);
   }
   else
   {
      filmcomp->MatrixDiff(TRUE);
      Buf2[0] = '1';
      if ( filmcomp->DetachGlobal() == TRUE ) Buf2[0] = '2';
      Buf2[1] = '\0';
      Write("\n   "); Write(scr.T(646));          // SelDetType
      Write("\n   1   =   "); Write(scr.T(647));  // Individual
      Write("\n   2   =   "); Write(scr.T(648));  // Global
      Write("\n  ");
      Read(Buf1,BufLen,Buf2);
      if ( Buf1[0]=='1' )
      {
         filmcomp->DetachGlobal(FALSE);
         readude = FALSE;
      }
   }
   CARDINAL numerr;
   CARDINAL n = 0;
   if ( readude == TRUE )
   {
      while (1)
      {
         if ( filmcomp->Udet() != 0 )
         {
            if ( (filmcomp->Udet())->UnParse(Buf2,BufLen-1) != 0 )
               Buf2[0] = '\0';
         }
         else
         {
            Buf2[0] = '\0';
         }
         Write("\n   "); Write(scr.T(649)); Write("\n  ");
         Read(Buf1,BufLen,Buf2);               // EnterUde
         if ( filmcomp->Udet(Buf1,system.Varlist(),Buf2,numerr)
                                                            == TRUE )
         {
            break;                                  // Success
         }
         else
         {
            n++;
            if ( n > 2 )
            {
               delete filmcomp; return 0;
            }
            if ( numerr == 0 )
            {                                       // IllegalDep
               Write("\n   "); Write(scr.T(650)); Write("\n");
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
      filmcomp->DetachGlobal(TRUE);
   }

   n = 0;
   while (1)
   {
      if ( filmcomp->Surface() != 0 )
      {
         if ( (filmcomp->Surface())->UnParse(Buf2,BufLen-1) != 0 )
            Buf2[0] = '\0';
      }
      else
      {
         Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(528)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);               // EnterSurface
      if ( filmcomp->Surface(Buf1,system.Varlist(),Buf2,numerr)
                                                            == TRUE )
      {
         break;                                  // Success
      }
      else
      {
         n++;
         if ( n > 2 )
         {
            delete filmcomp; return 0;
         }
         if ( numerr == 0 )
         {                                       // IllegalDep
            Write("\n   "); Write(scr.T(529)); Write("\n");
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
      if ( filmcomp->RateEps() != 0 )
      {
         if ( (filmcomp->RateEps())->UnParse(Buf2,BufLen-1) != 0 )
            Buf2[0] = '\0';
      }
      else
      {
         Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(530)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);               // EnterRateEps
      if ( filmcomp->RateEps(Buf1,system.Varlist(),Buf2,numerr)
                                                            == TRUE )
      {
         break;                                  // Success
      }
      else
      {
         n++;
         if ( n > 2 )
         {
            delete filmcomp; return 0;
         }
         if ( numerr == 0 )
         {                                       // IllegalDep
            Write("\n   "); Write(scr.T(531)); Write("\n");
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

   Write("\n   "); Write(scr.T(540));   // EnterNumGrid
   Read(n,filmcomp->NumGridPts()); filmcomp->NumGridPts(n);
   if ( filmcomp->HighRes() == FALSE ) Buf2[0] = '1';
   else                                Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(657));          // Select
   Write("\n   1   =   "); Write(scr.T(658));  // LowRes
   Write("\n   2   =   "); Write(scr.T(659));  // HighRes
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0]=='1' ) filmcomp->HighRes(FALSE);
   if ( Buf1[0]=='2' ) filmcomp->HighRes(TRUE);

   if ( filmcomp->Active() == TRUE ) Buf2[0] = '1';
   else                              Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(1395));                 // Status
   Write("\n   1   =   "); Write(scr.T(1396));         // Active
   Write("\n   2   =   "); Write(scr.T(1397));         // Inactive
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '2' ) filmcomp->Active(FALSE);
   else                  filmcomp->Active(TRUE);

   ReadActVar(system,filmcomp);
   ReadActProc(system,filmcomp);

   ReadInitCond(system,filmcomp);
   ReadInput(system,filmcomp);

   return filmcomp;
}


//////////////////////////////////////////////////////////////////////////////
