///////////////////////////////   edlake_c.C   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    26.09.94    Peter Reichert
// revisions:   22.05.98    Peter Reichert    Release 2.0 with sediment layers
//              18.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "all.h"
#include "init.h"
#include "aquasys.h"
#include "scrio_c.h"
#include "edopt_c.h"
#include "edlake_c.h"

//////////////////////////////////////////////////////////////////////////////

// global buffers and response characters (aquasim.C)

extern const CARDINAL  BufLen;
extern char            Buf[];
extern char            Buf1[];
extern char            Buf2[];
extern char            Buf3[];
extern char            Buf4[];
extern char            Buf5[];
extern char            Buf6[];

extern char            C1, C2, Y1, Y2, N1, N2;

//////////////////////////////////////////////////////////////////////////////

static std::ifstream infile;
static std::ofstream outfile;

//////////////////////////////////////////////////////////////////////////////

static void ReadInitCond(AQUASYS& system, LAKECOMP* com);
static void ReadInput(AQUASYS& system, LAKECOMP* com);

//////////////////////////////////////////////////////////////////////////////

static PINF* NewPointInf(AQUASYS& system, PINF* oldpinf)
{
   PINF* pinf = new PINF(oldpinf);
   ReadSymbol(pinf);
   CARDINAL numerr;
   CARDINAL n = 0;
   while (1)
   {
      if ( pinf->Depth() != 0 )
      {
         if ( (pinf->Depth())->UnParse(Buf2,BufLen-1) != 0 )
            Buf2[0] = '\0';
      }
      else
      {
         Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(1298)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);               // EnterDepth
      if ( pinf->Depth(Buf1,system.Varlist(),Buf2,numerr) == TRUE )
      {
         break;                                  // Success
      }
      else
      {
         n++;
         if ( n > 2 )
         {
            delete pinf; return 0;
         }
         if ( numerr == 0 )
         {                                       // IllegalDep
             Write("\n   "); Write(scr.T(599)); Write("\n");
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
      if ( pinf->Width() != 0 )
      {
         if ( (pinf->Width())->UnParse(Buf2,BufLen-1) != 0 )
            Buf2[0] = '\0';
      }
      else
      {
         Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(1299)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);               // EnterWidth
      if ( pinf->Width(Buf1,system.Varlist(),Buf2,numerr) == TRUE )
      {
         break;                                  // Success
      }
      else
      {
         n++;
         if ( n > 2 )
         {
            delete pinf; return 0;
         }
         if ( numerr == 0 )
         {                                       // IllegalDep
             Write("\n   "); Write(scr.T(599)); Write("\n");
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
      if ( pinf->WaterFlow() != 0 )
      {
         if ( (pinf->WaterFlow())->UnParse(Buf2,BufLen-1) != 0 )
            Buf2[0] = '\0';
      }
      else
      {
         Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(1279)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);               // EnterWaterFlow
      if ( pinf->WaterFlow(Buf1,system.Varlist(),Buf2,numerr) == TRUE )
      {
         break;                                  // Success
      }
      else
      {
         n++;
         if ( n > 2 )
         {
            delete pinf; return 0;
         }
         if ( numerr == 0 )
         {                                       // IllegalDep
             Write("\n   "); Write(scr.T(599)); Write("\n");
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

   while (1)
   {
      AQVAR* v;
      Write("\n   "); Write(scr.T(1300)); Write(" ");     // Fluxes
      Write(pinf->Symbol()); Write(":\n");
      if ( pinf->NumConc() > 0 )
      {
         Write("\n   "); Write(scr.T(685));              // Index
         Write("   "); Write(scr.T(1301));                // Variable
         Write("  :  "); Write(scr.T(1302));              // Flux
         for ( CARDINAL i=0; i<pinf->NumConc(); i++ )
         {
            Write("\n");
            NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
            Write(' ',strlen(scr.T(685)));
            v = pinf->ConcVar(i); Write(v->Symbol()); Write("  :  ");
            const FORMVAR* cfv = pinf->ConcVal(i);
            cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(1303));             // Select
      Write("\n   1   =   "); Write(scr.T(1304));     // AddFlux
      Write("\n   2   =   "); Write(scr.T(1305));     // EditFlux
      Write("\n   3   =   "); Write(scr.T(1306));     // DeleteFlux
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL numerr;
      CARDINAL index = 1;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(1307));       // EnterAddIndex
            index = pinf->NumConc()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(1308));        // EnterVar
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                           // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1309));       // EnterFlux
            Read(Buf1,BufLen,"0");
            if ( pinf->AddConc(system.Varlist(),v,Buf1,
                               Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1310)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1311)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(1312));       // EnterEditIndex
            Read(index,index); index--;
            if ( index >= pinf->NumConc() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                // IllegalIndex
            }
            Write("\n   "); Write(scr.T(1308));        // EnterVar
            Read(Buf1,BufLen,(pinf->ConcVar(index))->Symbol());
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                           // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1309));        // EnterFlux
            (pinf->ConcVal(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( pinf->ReplaceConc(system.Varlist(),v,Buf1,
                                   Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1313)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1314)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(1315));              // EnterIndex
            Read(index,index); index--;
            if ( pinf->DeleteConc(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1316)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1317)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }
   }
   return pinf;
}


static void AddPointInf(AQUASYS& system, LAKECOMP* lcomp)
{
   PINF* pinf = NewPointInf(system,0);
   if ( lcomp->AddPointInf(pinf) == TRUE )
   {
      Write("\n   "); Write(scr.T(1318)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(1319)); Write("\n");  // Failure
      delete pinf;
   }
   return;
}


static void EditPointInf(AQUASYS& system, LAKECOMP* lcomp)
{
   Write("\n   "); Write(scr.T(1320));   // EnterSymbol
   Read(Buf1,BufLen,"");
   PINF* oldpinf = lcomp->GetPointInf(Buf1);
   if ( oldpinf == 0 )
   {
      Write("\n   "); Write(scr.T(1321)); Write(" "); Write(Buf1);
      Write(" "); Write(scr.T(1322)); Write("\n");  // PInfNotFound
      return;
   }
   PINF* pinf = NewPointInf(system,oldpinf);
   if ( lcomp->ReplacePointInf(oldpinf,pinf) == TRUE )
   {
      Write("\n   "); Write(scr.T(1323)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(1324)); Write("\n");  // Failure
      delete pinf;
   }
   return;
}


static void DelPointInf(LAKECOMP* lcomp)
{
   Write("\n   "); Write(scr.T(1325));  // EnterSymbol
   Read(Buf1,BufLen,"");
   if ( lcomp->DeletePointInf(Buf1) == TRUE )
   {
      Write("\n   "); Write(scr.T(1326)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(1327)); Write("\n");  // Failure
   }
   return;
}


static void ReadInitCond(AQUASYS& system, LAKECOMP* com)
{
   while (1)
   {
      AQVAR* v;
      Write("\n   "); Write(scr.T(1209)); Write(" ");     // InitCond
      Write(com->Symbol()); Write(":\n");
      if ( com->NumInitCond() > 0 )
      {
         Write("\n   "); Write(scr.T(685));              // Index
         Write("   "); Write(scr.T(1210));               // Variable
         Write("("); Write(scr.T(1211)); Write(")");     // Zone
         Write("  :  "); Write(scr.T(1212));             // InitCond
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
      Write("\n   "); Write(scr.T(1213));             // Select
      Write("\n   1   =   "); Write(scr.T(1214));     // AddInitCond
      Write("\n   2   =   "); Write(scr.T(1215));     // EditInitCond
      Write("\n   3   =   "); Write(scr.T(1216));     // DeleteInitCond
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
            Write("\n   "); Write(scr.T(1217));            // EnterAddIndex
            index = com->NumInitCond()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(1218));            // EnterStateVar
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
               Write("\n   "); Write(scr.T(1219));        // SelectZone
               for ( CARDINAL i=0; i<com->NumZones(); i++ )
               {
                  NumToString(i+1,Buf1,BufLen,4);
                  Write("\n"); Write(Buf1); Write("   =   "); 
                  Write(com->ZoneName(i));                // ZoneName
               }
               Write("\n  "); Read(zone,zone+1); zone--;
               if ( zone >= com->NumZones() ) zone = com->NumZones()-1;
            }
            Write("\n   "); Write(scr.T(1220));           // EnterInitCond
            Read(Buf1,BufLen,"");
            
            if ( com->AddInitCond(system.Varlist(),zone,v,Buf1,
                                  Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1221)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1222)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(1223));            // EnterEditIndex
            Read(index,index); index--;
            if ( index >= com->NumInitCond() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                    // IllegalIndex
            }
            Write("\n   "); Write(scr.T(1218));           // EnterSymbol
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
               Write("\n   "); Write(scr.T(1219));        // SelectZone
               for ( CARDINAL i=0; i<com->NumZones(); i++ )
               {
                  NumToString(i+1,Buf1,BufLen,4);
                  Write("\n"); Write(Buf1); Write("   =   "); 
                  Write(com->ZoneName(i));                // ZoneName
               }
               Write("\n  "); Read(zone,com->InitZone(index)+1); zone--;
            }
            Write("\n   "); Write(scr.T(1220));           // EnterInitCond
            (com->InitVal(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( com->ReplaceInitCond(system.Varlist(),zone,v,Buf1,
                                      Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1224)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1225)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(1226));               // EnterDelIndex
            Read(index,index); index--;
            if ( com->DeleteInitCond(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1227)); Write("\n");   // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1228)); Write("\n");   // Failure
            }
            break;
         default:
            break;
      }
   }
   return;
}

//////////////////////////////////////////////////////////////////////////////

static void ReadInput(AQUASYS& system, LAKECOMP* com)
{
   while (1)
   {
      AQVAR* v;
      Write("\n   "); Write(scr.T(1230)); Write(" ");   // InputFluxes
      Write(com->Symbol()); Write(":\n");
      if ( com->NumSurfInput() > 0 )
      {
         Write("\n   "); Write(scr.T(685));             // Index
         Write("   "); Write(scr.T(1231));              // Variable
         Write("  :  "); Write(scr.T(1232));            // Input
         for ( CARDINAL i=0; i<com->NumSurfInput(); i++ )
         {
            Write("\n");
            NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
            Write(' ',strlen(scr.T(685)));
            v = com->SurfInputVar(i); Write(v->Symbol()); Write("  :  ");
            const FORMVAR* cfv = com->SurfInputFlux(i);
            cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(1233));               // Select
      Write("\n   1   =   "); Write(scr.T(1234));       // AddInput
      Write("\n   2   =   "); Write(scr.T(1235));       // EditInput
      Write("\n   3   =   "); Write(scr.T(1236));       // DeleteInput
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL numerr;
      CARDINAL index = 1;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(1237));          // EnterAddIndex
            index = com->NumSurfInput()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(1238));          // EnterSymbol
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1239));       // EnterInputFlux
            Read(Buf1,BufLen,"0");
            if ( com->AddSurfInput(system.Varlist(),v,Buf1,
                                   Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1240)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1241)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(1242));          // EnterEditIndex
            Read(index,index); index--;
            if ( index >= com->NumSurfInput() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                  // IllegalIndex
            }
            Write("\n   "); Write(scr.T(1238));          // EnterSymbol
            Read(Buf1,BufLen,(com->SurfInputVar(index))->Symbol());
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1239));       // EnterInputFlux
            (com->SurfInputFlux(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( com->ReplaceSurfInput(system.Varlist(),v,Buf1,
                                       Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1243)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1244)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(1245));              // EnterDelIndex
            Read(index,index); index--;
            if ( com->DeleteSurfInput(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1246)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1247)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }
   }

   CARDINAL n = 0;
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
      Write("\n   "); Write(scr.T(921)); Write("\n  "); // EnterExpression
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
      Write("\n   "); Write(scr.T(922)); Write(" ");    // InputConc
      Write(com->Symbol()); Write(":\n");
      if ( com->NumLatInput() > 0 )
      {
         Write("\n   "); Write(scr.T(685));             // Index
         Write("   "); Write(scr.T(923));               // Variable
         Write("  :  "); Write(scr.T(924));             // InputConc
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
      Write("\n   "); Write(scr.T(925));                // Select
      Write("\n   1   =   "); Write(scr.T(926));        // AddInput
      Write("\n   2   =   "); Write(scr.T(927));        // EditInput
      Write("\n   3   =   "); Write(scr.T(928));        // DeleteInput
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL numerr;
      CARDINAL index = 1;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(929));          // EnterAddIndex
            index = com->NumLatInput()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(930));          // EnterSymbol
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(931));       // EnterInputConc
            Read(Buf1,BufLen,"0");
            if ( com->AddLatInput(system.Varlist(),v,Buf1,
                                Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(932)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(933)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(934));          // EnterEditIndex
            Read(index,index); index--;
            if ( index >= com->NumLatInput() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                  // IllegalIndex
            }
            Write("\n   "); Write(scr.T(930));          // EnterSymbol
            Read(Buf1,BufLen,(com->LatInputVar(index))->Symbol());
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(491));       // EnterInputConc
            (com->LatInputConc(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( com->ReplaceLatInput(system.Varlist(),v,Buf1,
                                   Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(935)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(936)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(937));              // EnterDelIndex
            Read(index,index); index--;
            if ( com->DeleteLatInput(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(938)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(939)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }
   }

   while (1)
   {
      if ( com->PointInflist()->Size() > 0 )
      {
         Write("\n   "); Write(scr.T(1328)); Write(":");
         CARDINAL i = 0;
         PINF* pinf = com->PointInflist()->First();
         while ( pinf != 0 )
         {
            i++;
            if ( (i-1) % scr.I(1) == 0 ) Write("\n   ");
            Write(pinf->Symbol());
            Write(' ',scr.L(1)-strlen(pinf->Symbol()));
            pinf = pinf->Next();
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(1329));              // EditBif
      Write("\n   1   =   "); Write(scr.T(1330));      // AddBif
      Write("\n   2   =   "); Write(scr.T(1331));      // EditBif
      Write("\n   3   =   "); Write(scr.T(1332));      // DeleteBif
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      switch (Buf1[0])
      {
         case '1':
            AddPointInf(system,com);
            break;
         case '2':
            EditPointInf(system,com);
            break;
         case '3':
            DelPointInf(com);
            break;
         default:
            break;
      }
   }

   while (1)
   {
      AQVAR* v;
      Write("\n   "); Write(scr.T(1280)); Write(" ");   // SedInputs
      Write(com->Symbol()); Write(":\n");
      if ( com->NumSedInput() > 0 )
      {
         Write("\n   "); Write(scr.T(685));             // Index
         Write("   "); Write(scr.T(1281));              // Variable
         Write("  :  "); Write(scr.T(1282));            // InputFlux
         for ( CARDINAL i=0; i<com->NumSedInput(); i++ )
         {
            Write("\n");
            NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
            Write(' ',strlen(scr.T(685)));
            v = com->SedInputVar(i); Write(v->Symbol()); Write("  :  ");
            const FORMVAR* cfv = com->SedInputFlux(i);
            cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(1283));               // Select
      Write("\n   1   =   "); Write(scr.T(1284));       // AddSedInp
      Write("\n   2   =   "); Write(scr.T(1285));       // EditSedInp
      Write("\n   3   =   "); Write(scr.T(1286));       // DeleteSedInp
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL numerr;
      CARDINAL index = 1;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(1287));          // EnterAddIndex
            index = com->NumSedInput()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(1288));          // EnterSymbol
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1289));       // EnterSedInp
            Read(Buf1,BufLen,"0");
            if ( com->AddSedInput(system.Varlist(),v,Buf1,
                                  Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1290)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1291)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(1292));          // EnterEditIndex
            Read(index,index); index--;
            if ( index >= com->NumSedInput() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                  // IllegalIndex
            }
            Write("\n   "); Write(scr.T(1288));         // EnterSymbol
            Read(Buf1,BufLen,(com->SedInputVar(index))->Symbol());
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1289));       // EnterSedInp
            (com->SedInputFlux(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( com->ReplaceSedInput(system.Varlist(),v,Buf1,
                                      Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1293)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1294)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(1295));              // EnterDelIndex
            Read(index,index); index--;
            if ( com->DeleteSedInput(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1296)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1297)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }
   }
   return;
}

//////////////////////////////////////////////////////////////////////////////

LAKECOMP* EditLakeComp(AQUASYS& system, LAKECOMP* lakecomp)
{
   ReadSymbol(lakecomp);

   ReadCompInd(lakecomp);

   while (1)
   {
      AQVAR* v;
      Write("\n   "); Write(scr.T(1259)); Write(":\n"); // DissVars
      if ( lakecomp->NumDissVar() > 0 )
      {
         Write("\n   "); Write(scr.T(685));             // Index
         Write("   "); Write(scr.T(1260));              // Variable
         Write("  :  "); Write(scr.T(1261));            // SedExCoeff
         for ( CARDINAL i=0; i<lakecomp->NumDissVar(); i++ )
         {
            Write("\n");
            NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
            Write(' ',strlen(scr.T(685)));
            v = lakecomp->DissVar(i); Write(v->Symbol()); Write("  :  ");
            const FORMVAR* cfv = lakecomp->Disskdiff(i);
            cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(1262));               // Select
      Write("\n   1   =   "); Write(scr.T(1263));       // AddDissVar
      Write("\n   2   =   "); Write(scr.T(1264));       // EditDissVar
      Write("\n   3   =   "); Write(scr.T(1265));       // DeleteDissVar
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL numerr;
      CARDINAL index = 1;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(1266));         // EnterAddIndex
            index = lakecomp->NumDissVar()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(1267));         // EnterSymbol
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1268));      // EnterExCoeff
            Read(Buf1,BufLen,"0");
            if ( lakecomp->AddDissVar(system.Varlist(),v,Buf1,
                                      Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1269)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1270)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(1271));         // EnterEditIndex
            Read(index,index); index--;
            if ( index >= lakecomp->NumDissVar() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                  // IllegalIndex
            }
            Write("\n   "); Write(scr.T(1267));         // EnterSymbol
            Read(Buf1,BufLen,(lakecomp->DissVar(index))->Symbol());
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1268));       // EnterExCoeff
            (lakecomp->Disskdiff(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( lakecomp->ReplaceDissVar(system.Varlist(),v,Buf1,
                                          Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1272)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1273)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(1274));             // EnterDelIndex
            Read(index,index); index--;
            if ( lakecomp->DeleteDissVar(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1275)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1276)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }
   }

   while (1)
   {
      AQVAR* v;
      Write("\n   "); Write(scr.T(1011)); Write(":\n"); // PartVars
      if ( lakecomp->NumPartVar() > 0 )
      {
         Write("\n   "); Write(scr.T(685));             // Index
         Write("   "); Write(scr.T(1012));              // Variable
         Write("  :  "); Write(scr.T(1277));            // Density
         Write(", "); Write(scr.T(1013));               // SedVeloc
         Write(", "); Write(scr.T(1505));               // Mobility
         for ( CARDINAL i=0; i<lakecomp->NumPartVar(); i++ )
         {
            Write("\n");
            NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
            Write(' ',strlen(scr.T(685)));
            v = lakecomp->PartVar(i); Write(v->Symbol());
            Write("  :  ");
            const FORMVAR* cfv = lakecomp->PartRho(i);
            cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
            Write(", ");
            cfv = lakecomp->PartSedVeloc(i);
            cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
            Write(", ");
            cfv = lakecomp->PartMobility(i);
            cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(1014));               // Select
      Write("\n   1   =   "); Write(scr.T(1015));       // AddPartVar
      Write("\n   2   =   "); Write(scr.T(1016));       // EditPartVar
      Write("\n   3   =   "); Write(scr.T(1017));       // DeletePartVar
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL numerr1, numerr2, numerr3;
      CARDINAL index = 1;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(1018));         // EnterAddIndex
            index = lakecomp->NumPartVar()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(1019));         // EnterSymbol
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1278));      // EnterDensity
            Read(Buf1,BufLen,"1000");
            Write("\n   "); Write(scr.T(1020));      // EnterSedVeloc
            Read(Buf3,BufLen,"0");
            Write("\n   "); Write(scr.T(1524));      // EnterMobility
            Read(Buf5,BufLen,"0");
            if ( lakecomp->AddPartVar(system.Varlist(),v,
				                      Buf1,Buf2,numerr1,
                                      Buf3,Buf4,numerr2,
									  Buf5,Buf6,numerr3,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1021)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1022)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(1023));         // EnterEditIndex
            Read(index,index); index--;
            if ( index >= lakecomp->NumPartVar() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                  // IllegalIndex
            }
            Write("\n   "); Write(scr.T(1019));         // EnterSymbol
            Read(Buf1,BufLen,(lakecomp->PartVar(index))->Symbol());
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1278));       // EnterDensity
            (lakecomp->PartRho(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            Write("\n   "); Write(scr.T(1020));       // EnterSedVeloc
            (lakecomp->PartSedVeloc(index))->UnParse(Buf4,BufLen);
            Read(Buf3,BufLen,Buf4);
            Write("\n   "); Write(scr.T(1524));       // EnterMobility
            (lakecomp->PartMobility(index))->UnParse(Buf6,BufLen);
            Read(Buf5,BufLen,Buf6);
            if ( lakecomp->ReplacePartVar(system.Varlist(),v,
				                          Buf1,Buf2,numerr1,
                                          Buf3,Buf4,numerr2,
										  Buf5,Buf6,numerr3,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1024)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1025)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(1026));             // EnterDelIndex
            Read(index,index); index--;
            if ( lakecomp->DeletePartVar(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1027)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1028)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }
   }

   CARDINAL n = 0; REAL r,s;
   do
   {
      Write("\n   "); Write(scr.T(940));   // EnterStart
      Read(r,lakecomp->ZTop());
      Write("\n   "); Write(scr.T(941));   // EnterEnd
      Read(s,lakecomp->ZBottom());
   } while ( lakecomp->ZTopBottom(r,s)== FALSE );

   do
   {
      Write("\n   "); Write(scr.T(826));   // EnterGravit
      Read(r,lakecomp->Gravit());
   } while ( lakecomp->Gravit(r) == FALSE );

   CARDINAL numerr;
   n = 0;
   while (1)
   {
      if ( lakecomp->CrossSect() != 0 )
      {
         if ( (lakecomp->CrossSect())->UnParse(Buf2,BufLen-1) != 0 )
            Buf2[0] = '\0';
      }
      else
      {
         Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(943)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);               // EnterCrossSect
      if ( lakecomp->CrossSect(Buf1,system.Varlist(),Buf2,numerr)
                                                            == TRUE )
      {
         break;                                  // Success
      }
      else
      {
         n++;
         if ( n > 2 )
         {
            delete lakecomp; return 0;
         }
         if ( numerr == 0 )
         {                                       // IllegalDep
            Write("\n   "); Write(scr.T(942)); Write("\n");
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
      if ( lakecomp->Density() != 0 )
      {
         if ( (lakecomp->Density())->UnParse(Buf2,BufLen-1) != 0 )
            Buf2[0] = '\0';
      }
      else
      {
         Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(1005)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);            // EnterDensity
      if ( lakecomp->Density(Buf1,system.Varlist(),Buf2,numerr)
                                                             == TRUE )
      {
         break;
      }
      else
      {
         n++;
         if ( n > 2 )
         {
            delete lakecomp; return 0;
         }
         if ( numerr == 0 )
         {                                       // IllegalDep
            Write("\n   "); Write(scr.T(942)); Write("\n");
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
      if ( lakecomp->Diffusion() != 0 )
      {
         if ( (lakecomp->Diffusion())->UnParse(Buf2,BufLen-1) != 0 )
            Buf2[0] = '\0';
      }
      else
      {
         Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(944)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);            // EnterDiffusion
      if ( lakecomp->Diffusion(Buf1,system.Varlist(),Buf2,numerr)
                                                             == TRUE )
      {
         break;
      }
      else
      {
         n++;
         if ( n > 2 )
         {
            delete lakecomp; return 0;
         }
         if ( numerr == 0 )
         {                                       // IllegalDep
            Write("\n   "); Write(scr.T(942)); Write("\n");
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
      if ( lakecomp->LightSurface() != 0 )
      {
         if ( (lakecomp->LightSurface())->UnParse(Buf2,BufLen-1) != 0 )
            Buf2[0] = '\0';
      }
      else
      {
         Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(1436)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);            // EnterLightSurf
      if ( lakecomp->LightSurface(Buf1,system.Varlist(),Buf2,numerr)
                                                             == TRUE )
      {
         break;
      }
      else
      {
         n++;
         if ( n > 2 )
         {
            delete lakecomp; return 0;
         }
         if ( numerr == 0 )
         {                                       // IllegalDep
            Write("\n   "); Write(scr.T(942)); Write("\n");
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
      if ( lakecomp->LightExtinct() != 0 )
      {
         if ( (lakecomp->LightExtinct())->UnParse(Buf2,BufLen-1) != 0 )
            Buf2[0] = '\0';
      }
      else
      {
         Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(1437)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);            // EnterLightExtinct
      if ( lakecomp->LightExtinct(Buf1,system.Varlist(),Buf2,numerr)
                                                             == TRUE )
      {
         break;
      }
      else
      {
         n++;
         if ( n > 2 )
         {
            delete lakecomp; return 0;
         }
         if ( numerr == 0 )
         {                                       // IllegalDep
            Write("\n   "); Write(scr.T(942)); Write("\n");
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

   if ( lakecomp->CalcSed() == FALSE ) Buf2[0] = '1';
   else                                Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(1333));          // Select
   Write("\n   1   =   "); Write(scr.T(1334));  // calcSedno
   Write("\n   2   =   "); Write(scr.T(1335));  // calcSedyes
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0]=='1' ) lakecomp->CalcSed(FALSE);
   if ( Buf1[0]=='2' ) lakecomp->CalcSed(TRUE);

   if ( lakecomp->CalcSed() == TRUE )
   {
      n = 0;
      while (1)
      {
         if ( lakecomp->SedFluxVol() != 0 )
         {
            if ( (lakecomp->SedFluxVol())->UnParse(Buf2,BufLen-1) != 0 )
               Buf2[0] = '\0';
         }
         else
         {
            Buf2[0] = '\0';
         }
         Write("\n   "); Write(scr.T(1338)); Write("\n  ");
         Read(Buf1,BufLen,Buf2);            // EnterSedFvol
         if ( lakecomp->SedFluxVol(Buf1,system.Varlist(),Buf2,numerr)
                                                                == TRUE )
         {
            break;
         }
         else
         {
            n++;
            if ( n > 2 )
            {
               delete lakecomp; return 0;
            }
            if ( numerr == 0 )
            {                                       // IllegalDep
               Write("\n   "); Write(scr.T(942)); Write("\n");
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
         if ( lakecomp->SedResusp() != 0 )
         {
            if ( (lakecomp->SedResusp())->UnParse(Buf2,BufLen-1) != 0 )
               Buf2[0] = '\0';
         }
         else
         {
            Buf2[0] = '\0';
         }
         Write("\n   "); Write(scr.T(1337)); Write("\n  ");
         Read(Buf1,BufLen,Buf2);            // EnterSedResusp
         if ( lakecomp->SedResusp(Buf1,system.Varlist(),Buf2,numerr)
                                                                == TRUE )
         {
            break;
         }
         else
         {
            n++;
            if ( n > 2 )
            {
               delete lakecomp; return 0;
            }
            if ( numerr == 0 )
            {                                       // IllegalDep
               Write("\n   "); Write(scr.T(942)); Write("\n");
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

      while (1)
      {
         Write("\n   "); Write(scr.T(1371));            // SedLayers
         Write(":\n");
         if ( lakecomp->NumSedLayers() > 0 )
         {
            Write("\n   "); Write(scr.T(685));          // Index
            Write("   "); Write(scr.T(1372));           // ZoneInd
            Write("  :  "); Write(scr.T(1373));         // Thickness
            Write(", "); Write(scr.T(1374));            // Porosity
            for ( CARDINAL i=0; i<lakecomp->NumSedLayers(); i++ )
            {
               Write("\n");
               NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
               Write(' ',strlen(scr.T(685)));
               NumToString(lakecomp->SedLayer(i)->ZoneIndex(),Buf1,BufLen);
			   Write(Buf1); Write("  :  ");
               const FORMVAR* cfv = lakecomp->SedLayer(i)->Thickness();
               cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
               Write(", ");
               cfv = lakecomp->SedLayer(i)->Porosity();
               cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
            }
            Write("\n");
         }
         Write("\n   "); Write(scr.T(1375));               // Select
         Write("\n   1   =   "); Write(scr.T(1376));       // AddLayer
         Write("\n   2   =   "); Write(scr.T(1377));       // EditLayer
         Write("\n   3   =   "); Write(scr.T(1378));       // DeleteLayer
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
                  LAKESEDLAYER* newlayer = new LAKESEDLAYER;
                  Write("\n   "); Write(scr.T(1379));          // EnterAddIndex
                  index = lakecomp->NumSedLayers()+1; Read(index,index); index--;
                  do
                  {
                     Write("\n   "); Write(scr.T(1380));   // EnterZoneInd
                     Read(c,newlayer->ZoneIndex());
                  } while ( newlayer->ZoneIndex(c) == FALSE );
                  do
                  {
                     Write("\n   "); Write(scr.T(1336));   // EnterThickness
                     (newlayer->Thickness())->UnParse(Buf2,BufLen);
                     Read(Buf1,BufLen,Buf2);
				  } while ( newlayer->Thickness(Buf1,system.Varlist(),Buf2,numerr) == FALSE );
                  do
                  {
                     Write("\n   "); Write(scr.T(1010));   // EnterPorosity
                     (newlayer->Porosity())->UnParse(Buf2,BufLen);
                     Read(Buf1,BufLen,Buf2);
				  } while ( newlayer->Porosity(Buf1,system.Varlist(),Buf2,numerr) == FALSE );
                  if ( lakecomp->AddSedLayer(newlayer,index) == TRUE )
                  {
                     Write("\n   "); Write(scr.T(1381)); Write("\n"); // Success
                  }
                  else
                  {
                     Write("\n   "); Write(scr.T(1382)); Write("\n"); // Failure
                     delete newlayer;
                  }
               }
               break;
            case '2':
               {
                  if ( lakecomp->NumSedLayers() == 0 ) break;
                  do
                  {
                     Write("\n   "); Write(scr.T(1383));          // EnterEditIndex
                     index = 1; Read(index,index); index--;
                  } while ( index >= lakecomp->NumSedLayers() );
                  LAKESEDLAYER* newlayer = new LAKESEDLAYER(lakecomp->SedLayer(index));
                  do
                  {
                     Write("\n   "); Write(scr.T(1380));   // EnterZoneInd
                     Read(c,newlayer->ZoneIndex());
                  } while ( newlayer->ZoneIndex(c) == FALSE );
                  do
                  {
                     Write("\n   "); Write(scr.T(1336));   // EnterThickness
                     (newlayer->Thickness())->UnParse(Buf2,BufLen);
                     Read(Buf1,BufLen,Buf2);
				  } while ( newlayer->Thickness(Buf1,system.Varlist(),Buf2,numerr) == FALSE );
                  do
                  {
                     Write("\n   "); Write(scr.T(1010));   // EnterPorosity
                     (newlayer->Porosity())->UnParse(Buf2,BufLen);
                     Read(Buf1,BufLen,Buf2);
				  } while ( newlayer->Porosity(Buf1,system.Varlist(),Buf2,numerr) == FALSE );
                  if ( lakecomp->ReplaceSedLayer(newlayer,index) == TRUE )
                  {
                     Write("\n   "); Write(scr.T(1384)); Write("\n"); // Success
                  }
                  else
                  {
                     Write("\n   "); Write(scr.T(1385)); Write("\n"); // Failure
                     delete newlayer;
                  }
               }
               break;
            case '3':
               Write("\n   "); Write(scr.T(1386));              // EnterDelIndex
               Read(index,index); index--;
               if ( lakecomp->DeleteSedLayer(index) == TRUE )
               {
                  Write("\n   "); Write(scr.T(1387)); Write("\n"); // Success
               }
               else
               {
                  Write("\n   "); Write(scr.T(1388)); Write("\n"); // Failure
               }
               break;
            default:
               break;
         }
      }
   }

   if ( lakecomp->CalcTKE() == FALSE ) Buf2[0] = '1';
   else                                Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(1049));          // Select
   Write("\n   1   =   "); Write(scr.T(1050));  // calcTKEno
   Write("\n   2   =   "); Write(scr.T(1051));  // calcTKEyes
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0]=='1' ) lakecomp->CalcTKE(FALSE);
   if ( Buf1[0]=='2' ) lakecomp->CalcTKE(TRUE);

   if ( lakecomp->CalcTKE() == TRUE )
   {
      REAL r;
      n = 0;
      while (1)
      {
         if ( lakecomp->Prandtl() != 0 )
         {
            if ( (lakecomp->Prandtl())->UnParse(Buf2,BufLen-1) != 0 )
               Buf2[0] = '\0';
         }
         else
         {
            Buf2[0] = '\0';
         }
         Write("\n   "); Write(scr.T(1048)); Write("\n  ");
         Read(Buf1,BufLen,Buf2);            // EnterPrandtl
         if ( lakecomp->Prandtl(Buf1,system.Varlist(),Buf2,numerr)
                                                             == TRUE )
         {
            break;
         }
         else
         {
            n++;
            if ( n > 2 )
            {
               delete lakecomp; return 0;
            }
            if ( numerr == 0 )
            {                                       // IllegalDep
               Write("\n   "); Write(scr.T(942)); Write("\n");
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

      do
      {
         Write("\n   "); Write(scr.T(1390));   // EnterSigTKE
         Read(r,lakecomp->SigmaTKE());
      } while ( lakecomp->SigmaTKE(r) == FALSE );

      do
      {
         Write("\n   "); Write(scr.T(1391));   // EnterSigEps
         Read(r,lakecomp->SigmaEps());
      } while ( lakecomp->SigmaEps(r) == FALSE );

      do
      {
         Write("\n   "); Write(scr.T(1392));   // EnterKzMin
         Read(r,lakecomp->KzMin());
      } while ( lakecomp->KzMin(r) == FALSE );

      n = 0;
      while (1)
      {
         if ( lakecomp->SurfaceShear() != 0 )
         {
            if ( (lakecomp->SurfaceShear())->UnParse(Buf2,BufLen-1) != 0 )
               Buf2[0] = '\0';
         }
         else
         {
            Buf2[0] = '\0';
         }
         Write("\n   "); Write(scr.T(1006)); Write("\n  ");
         Read(Buf1,BufLen,Buf2);            // EnterSurfShear
         if ( lakecomp->SurfaceShear(Buf1,system.Varlist(),Buf2,numerr)
                                                             == TRUE )
         {
            break;
         }
         else
         {
            n++;
            if ( n > 2 )
            {
               delete lakecomp; return 0;
            }
            if ( numerr == 0 )
            {                                       // IllegalDep
               Write("\n   "); Write(scr.T(942)); Write("\n");
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
         if ( lakecomp->PressureGradient() != 0 )
         {
            if ( (lakecomp->PressureGradient())->UnParse(Buf2,BufLen-1) != 0 )
               Buf2[0] = '\0';
         }
         else
         {
            Buf2[0] = '\0';
         }
         Write("\n   "); Write(scr.T(1389)); Write("\n  ");
         Read(Buf1,BufLen,Buf2);            // EnterPressGrad
         if ( lakecomp->PressureGradient(Buf1,system.Varlist(),Buf2,numerr)
                                                             == TRUE )
         {
            break;
         }
         else
         {
            n++;
            if ( n > 2 )
            {
               delete lakecomp; return 0;
            }
            if ( numerr == 0 )
            {                                       // IllegalDep
               Write("\n   "); Write(scr.T(942)); Write("\n");
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
         if ( lakecomp->SeichePwind() != 0 )
         {
            if ( (lakecomp->SeichePwind())->UnParse(Buf2,BufLen-1) != 0 )
               Buf2[0] = '\0';
         }
         else
         {
            Buf2[0] = '\0';
         }
         Write("\n   "); Write(scr.T(1007)); Write("\n  ");
         Read(Buf1,BufLen,Buf2);            // EnterPwind
         if ( lakecomp->SeichePwind(Buf1,system.Varlist(),Buf2,numerr)
                                                                == TRUE )
         {
            break;
         }
         else
         {
            n++;
            if ( n > 2 )
            {
               delete lakecomp; return 0;
            }
            if ( numerr == 0 )
            {                                       // IllegalDep
               Write("\n   "); Write(scr.T(942)); Write("\n");
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
         if ( lakecomp->SeichePbottom() != 0 )
         {
            if ( (lakecomp->SeichePbottom())->UnParse(Buf2,BufLen-1) != 0 )
               Buf2[0] = '\0';
         }
         else
         {
            Buf2[0] = '\0';
         }
         Write("\n   "); Write(scr.T(1008)); Write("\n  ");
         Read(Buf1,BufLen,Buf2);            // EnterSeichePbottom
         if ( lakecomp->SeichePbottom(Buf1,system.Varlist(),Buf2,numerr)
                                                                == TRUE )
         {
            break;
         }
         else
         {
            n++;
            if ( n > 2 )
            {
               delete lakecomp; return 0;
            }
            if ( numerr == 0 )
            {                                       // IllegalDep
               Write("\n   "); Write(scr.T(942)); Write("\n");
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
         if ( lakecomp->SeichePint() != 0 )
         {
            if ( (lakecomp->SeichePint())->UnParse(Buf2,BufLen-1) != 0 )
               Buf2[0] = '\0';
         }
         else
         {
            Buf2[0] = '\0';
         }
         Write("\n   "); Write(scr.T(1009)); Write("\n  ");
         Read(Buf1,BufLen,Buf2);            // EnterSeicheAtt
         if ( lakecomp->SeichePint(Buf1,system.Varlist(),Buf2,numerr)
                                                             == TRUE )
         {
            break;
         }
         else
         {
            n++;
            if ( n > 2 )
            {
               delete lakecomp; return 0;
            }
            if ( numerr == 0 )
            {                                       // IllegalDep
               Write("\n   "); Write(scr.T(942)); Write("\n");
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
         if ( lakecomp->ProdDiss() != 0 )
         {
            if ( (lakecomp->ProdDiss())->UnParse(Buf2,BufLen-1) != 0 )
               Buf2[0] = '\0';
         }
         else
         {
            Buf2[0] = '\0';
         }
         Write("\n   "); Write(scr.T(1047)); Write("\n  ");
         Read(Buf1,BufLen,Buf2);            // EnterProdDiss
         if ( lakecomp->ProdDiss(Buf1,system.Varlist(),Buf2,numerr)
                                                             == TRUE )
         {
            break;
         }
         else
         {
            n++;
            if ( n > 2 )
            {
               delete lakecomp; return 0;
            }
            if ( numerr == 0 )
            {                                       // IllegalDep
               Write("\n   "); Write(scr.T(942)); Write("\n");
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
   }

   Write("\n   "); Write(scr.T(945));   // EnterNumGrid
   Read(n,lakecomp->NumGridPts()); lakecomp->NumGridPts(n);
   if ( lakecomp->HighRes() == FALSE ) Buf2[0] = '1';
   else                                 Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(946));          // Select
   Write("\n   1   =   "); Write(scr.T(947));  // LowRes
   Write("\n   2   =   "); Write(scr.T(948));  // HighRes
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0]=='1' ) lakecomp->HighRes(FALSE);
   if ( Buf1[0]=='2' ) lakecomp->HighRes(TRUE);

   if ( lakecomp->Active() == TRUE ) Buf2[0] = '1';
   else                              Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(1395));                 // Status
   Write("\n   1   =   "); Write(scr.T(1396));         // Active
   Write("\n   2   =   "); Write(scr.T(1397));         // Inactive
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '2' ) lakecomp->Active(FALSE);
   else                  lakecomp->Active(TRUE);

   ReadActVar(system,lakecomp);
   ReadActProc(system,lakecomp);

   ReadInitCond(system,lakecomp);
   ReadInput(system,lakecomp);

   return lakecomp;
}


//////////////////////////////////////////////////////////////////////////////
