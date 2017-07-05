///////////////////////////////   edsed_c.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    04.02.01    Peter Reichert
// revisions:   18.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "all.h"
#include "init.h"
#include "aquasys.h"
#include "scrio_c.h"
#include "edopt_c.h"
#include "edsed_c.h"

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

static void ReadInitCond(AQUASYS& system, SEDCOMP* com);
static void ReadInput(AQUASYS& system, SEDCOMP* com);

//////////////////////////////////////////////////////////////////////////////

static void ReadInitCond(AQUASYS& system, SEDCOMP* com)
{
   while (1)
   {
      AQVAR* v;
      Write("\n   "); Write(scr.T(1438)); Write(" ");     // InitCond
      Write(com->Symbol()); Write(":\n");
      if ( com->NumInitCond() > 0 )
      {
         Write("\n   "); Write(scr.T(685));              // Index
         Write("   "); Write(scr.T(1439));               // Variable
         Write("("); Write(scr.T(1440)); Write(")");     // Zone
         Write("  :  "); Write(scr.T(1441));             // InitCond
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
      Write("\n   "); Write(scr.T(1442));             // Select
      Write("\n   1   =   "); Write(scr.T(1443));     // AddInitCond
      Write("\n   2   =   "); Write(scr.T(1444));     // EditInitCond
      Write("\n   3   =   "); Write(scr.T(1445));     // DeleteInitCond
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
            Write("\n   "); Write(scr.T(1446));            // EnterAddIndex
            index = com->NumInitCond()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(1447));            // EnterStateVar
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
               Write("\n   "); Write(scr.T(1448));        // SelectZone
               for ( CARDINAL i=0; i<com->NumZones(); i++ )
               {
                  NumToString(i+1,Buf1,BufLen,4);
                  Write("\n"); Write(Buf1); Write("   =   "); 
                  Write(com->ZoneName(i));                // ZoneName
               }
               Write("\n  "); Read(zone,zone+1); zone--;
               if ( zone >= com->NumZones() ) zone = com->NumZones()-1;
            }
            Write("\n   "); Write(scr.T(1449));           // EnterInitCond
            Read(Buf1,BufLen,"");
            
            if ( com->AddInitCond(system.Varlist(),zone,v,Buf1,
                                  Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1450)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1451)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(1452));            // EnterEditIndex
            Read(index,index); index--;
            if ( index >= com->NumInitCond() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                    // IllegalIndex
            }
            Write("\n   "); Write(scr.T(1453));           // EnterSymbol
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
               Write("\n   "); Write(scr.T(1448));        // SelectZone
               for ( CARDINAL i=0; i<com->NumZones(); i++ )
               {
                  NumToString(i+1,Buf1,BufLen,4);
                  Write("\n"); Write(Buf1); Write("   =   "); 
                  Write(com->ZoneName(i));                // ZoneName
               }
               Write("\n  "); Read(zone,com->InitZone(index)+1); zone--;
            }
            Write("\n   "); Write(scr.T(1449));           // EnterInitCond
            (com->InitVal(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( com->ReplaceInitCond(system.Varlist(),zone,v,Buf1,
                                      Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1453)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1454)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(1455));               // EnterDelIndex
            Read(index,index); index--;
            if ( com->DeleteInitCond(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1456)); Write("\n");   // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1457)); Write("\n");   // Failure
            }
            break;
         default:
            break;
      }
   }
   return;
}

//////////////////////////////////////////////////////////////////////////////

static void ReadInput(AQUASYS& system, SEDCOMP* com)
{
   CARDINAL numerr;
   CARDINAL n = 0;
   while (1)
   {
      if ( com->SurfPorosity() != 0 )
      {
         if ( (com->SurfPorosity())->UnParse(Buf2,BufLen-1) != 0 )
            Buf2[0] = '\0';
      }
      else
      {
         Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(1497)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);               // EnterSurfPor
      if ( com->SurfPorosity(Buf1,system.Varlist(),Buf2,numerr)
                                                            == TRUE )
      {
         break;                                  // Success
      }
      else
      {
         n++;
         if ( n > 2 )
         {
            return;
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
      AQVAR* v;
      Write("\n   "); Write(scr.T(1458)); Write(" ");   // TopBCCorFs
      Write(com->Symbol()); Write(":\n");
      if ( com->NumTopBC() > 0 )
      {
         Write("\n   "); Write(scr.T(685));             // Index
         Write("   "); Write(scr.T(1459));              // Variable
         Write("  :  "); Write(scr.T(1460));            // TopBC
         for ( CARDINAL i=0; i<com->NumTopBC(); i++ )
         {
            Write("\n");
            NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
            Write(' ',strlen(scr.T(685)));
            v = com->TopBCVar(i); Write(v->Symbol()); Write("  :  ");
            const FORMVAR* cfv = com->TopBCCorF(i);
            cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(1461));               // Select
      Write("\n   1   =   "); Write(scr.T(1462));       // AddTopBC
      Write("\n   2   =   "); Write(scr.T(1463));       // EditTopBC
      Write("\n   3   =   "); Write(scr.T(1464));       // DeleteTopBC
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL numerr;
      CARDINAL index = 1;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(1465));          // EnterAddIndex
            index = com->NumTopBC()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(1466));          // EnterSymbol
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1467));       // EnterTopBCCorF
            Read(Buf1,BufLen,"0");
            if ( com->AddTopBC(system.Varlist(),v,Buf1,
                                   Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1468)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1469)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(1470));          // EnterEditIndex
            Read(index,index); index--;
            if ( index >= com->NumTopBC() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                  // IllegalIndex
            }
            Write("\n   "); Write(scr.T(1466));          // EnterSymbol
            Read(Buf1,BufLen,(com->TopBCVar(index))->Symbol());
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1467));       // EnterTopBCConc
            (com->TopBCCorF(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( com->ReplaceTopBC(system.Varlist(),v,Buf1,
                                       Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1471)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1472)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(1473));              // EnterDelIndex
            Read(index,index); index--;
            if ( com->DeleteTopBC(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1474)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1475)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }
   }
   return;
}

//////////////////////////////////////////////////////////////////////////////

SEDCOMP* EditSedComp(AQUASYS& system, SEDCOMP* sedcomp)
{
   ReadSymbol(sedcomp);

   ReadCompInd(sedcomp);

   while (1)
   {
      AQVAR* v;
      Write("\n   "); Write(scr.T(1476)); Write(":\n"); // DissVars
      if ( sedcomp->NumDissVar() > 0 )
      {
         Write("\n   "); Write(scr.T(685));             // Index
         Write("   "); Write(scr.T(1477));              // Variable
         Write("  :  "); Write(scr.T(1478));            // Diffusivity
         for ( CARDINAL i=0; i<sedcomp->NumDissVar(); i++ )
         {
            Write("\n");
            NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
            Write(' ',strlen(scr.T(685)));
            v = sedcomp->DissVar(i); Write(v->Symbol()); Write("  :  ");
            const FORMVAR* cfv = sedcomp->Dissdiff(i);
            cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(1479));               // Select
      Write("\n   1   =   "); Write(scr.T(1480));       // AddDissVar
      Write("\n   2   =   "); Write(scr.T(1481));       // EditDissVar
      Write("\n   3   =   "); Write(scr.T(1482));       // DeleteDissVar
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL numerr;
      CARDINAL index = 1;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(1483));         // EnterAddIndex
            index = sedcomp->NumDissVar()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(1484));         // EnterSymbol
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1485));      // EnterDiff
            Read(Buf1,BufLen,"0");
            if ( sedcomp->AddDissVar(system.Varlist(),v,Buf1,
                                      Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1486)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1487)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(1488));         // EnterEditIndex
            Read(index,index); index--;
            if ( index >= sedcomp->NumDissVar() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                  // IllegalIndex
            }
            Write("\n   "); Write(scr.T(1484));         // EnterSymbol
            Read(Buf1,BufLen,(sedcomp->DissVar(index))->Symbol());
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1485));       // EnterDiff
            (sedcomp->Dissdiff(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( sedcomp->ReplaceDissVar(system.Varlist(),v,Buf1,
                                          Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1489)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1490)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(1491));             // EnterDelIndex
            Read(index,index); index--;
            if ( sedcomp->DeleteDissVar(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1492)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1493)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }
   }

   while (1)
   {
      AQVAR* v;
      Write("\n   "); Write(scr.T(1494)); Write(":\n"); // PartVars
      if ( sedcomp->NumPartVar() > 0 )
      {
         Write("\n   "); Write(scr.T(685));             // Index
         Write("   "); Write(scr.T(1495));              // Variable
         Write("  :  "); Write(scr.T(1496));            // Density
         Write(", "); Write(scr.T(1522));               // Bioturb
         for ( CARDINAL i=0; i<sedcomp->NumPartVar(); i++ )
         {
            Write("\n");
            NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
            Write(' ',strlen(scr.T(685)));
            v = sedcomp->PartVar(i); Write(v->Symbol());
            Write("  :  ");
            const FORMVAR* cfv = sedcomp->PartRho(i);
            cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
            Write(", ");
            cfv = sedcomp->PartBioturb(i);
            cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(1498));               // Select
      Write("\n   1   =   "); Write(scr.T(1499));       // AddPartVar
      Write("\n   2   =   "); Write(scr.T(1500));       // EditPartVar
      Write("\n   3   =   "); Write(scr.T(1501));       // DeletePartVar
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL numerr1, numerr2, numerr3;
      CARDINAL index = 1;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(1502));         // EnterAddIndex
            index = sedcomp->NumPartVar()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(1503));         // EnterSymbol
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1504));      // EnterDensity
            Read(Buf1,BufLen,"1000");
            Write("\n   "); Write(scr.T(1523));      // EnterBioturb
            Read(Buf3,BufLen,"0");
            if ( sedcomp->AddPartVar(system.Varlist(),v,Buf1,Buf2,numerr1,
                                       Buf3,Buf4,numerr3,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1506)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1507)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(1508));         // EnterEditIndex
            Read(index,index); index--;
            if ( index >= sedcomp->NumPartVar() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                  // IllegalIndex
            }
            Write("\n   "); Write(scr.T(1503));         // EnterSymbol
            Read(Buf1,BufLen,(sedcomp->PartVar(index))->Symbol());
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1504));       // EnterDensity
            (sedcomp->PartRho(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            Write("\n   "); Write(scr.T(1523));       // EnterBioturb
            (sedcomp->PartBioturb(index))->UnParse(Buf4,BufLen);
            Read(Buf3,BufLen,Buf4);
            if ( sedcomp->ReplacePartVar(system.Varlist(),v,Buf1,Buf2,numerr1,
                                         Buf3,Buf4,numerr2,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1509)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1510)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(1511));             // EnterDelIndex
            Read(index,index); index--;
            if ( sedcomp->DeletePartVar(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1512)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1513)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }
   }

   CARDINAL n = 0; REAL r,s;
   do
   {
      Write("\n   "); Write(scr.T(1514));   // EnterStart
      Read(r,sedcomp->ZTop());
      Write("\n   "); Write(scr.T(1515));   // EnterEnd
      Read(s,sedcomp->ZBottom());
   } while ( sedcomp->ZTopBottom(r,s)== FALSE );

   CARDINAL numerr;
   n = 0;
   while (1)
   {
      if ( sedcomp->RatePorosity() != 0 )
      {
         if ( (sedcomp->RatePorosity())->UnParse(Buf2,BufLen-1) != 0 )
            Buf2[0] = '\0';
      }
      else
      {
         Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(1517)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);               // EnterRatePor
      if ( sedcomp->RatePorosity(Buf1,system.Varlist(),Buf2,numerr)
                                                            == TRUE )
      {
         break;                                  // Success
      }
      else
      {
         n++;
         if ( n > 2 )
         {
            delete sedcomp; return 0;
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

   Write("\n   "); Write(scr.T(1518));   // EnterNumGrid
   Read(n,sedcomp->NumGridPts()); sedcomp->NumGridPts(n);
   if ( sedcomp->HighRes() == FALSE ) Buf2[0] = '1';
   else                               Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(1519));          // Select
   Write("\n   1   =   "); Write(scr.T(1520));  // LowRes
   Write("\n   2   =   "); Write(scr.T(1521));  // HighRes
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0]=='1' ) sedcomp->HighRes(FALSE);
   if ( Buf1[0]=='2' ) sedcomp->HighRes(TRUE);

   if ( sedcomp->Active() == TRUE ) Buf2[0] = '1';
   else                              Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(1395));                 // Status
   Write("\n   1   =   "); Write(scr.T(1396));         // Active
   Write("\n   2   =   "); Write(scr.T(1397));         // Inactive
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '2' ) sedcomp->Active(FALSE);
   else                  sedcomp->Active(TRUE);

   ReadActVar(system,sedcomp);
   ReadActProc(system,sedcomp);

   ReadInitCond(system,sedcomp);
   ReadInput(system,sedcomp);

   return sedcomp;
}


//////////////////////////////////////////////////////////////////////////////
