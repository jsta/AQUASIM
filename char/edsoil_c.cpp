
///////////////////////////////   edsoil_c.C   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    30.06.94    Werner Simon
// revisions:   18.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "all.h"
#include "init.h"
#include "aquasys.h"
#include "scrio_c.h"
#include "edopt_c.h"
#include "edsoil_c.h"

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

static void ReadInitCond(AQUASYS& system, SOILCOMP* com);
static void ReadInput(AQUASYS& system, SOILCOMP* com);

//////////////////////////////////////////////////////////////////////////////

static void ReadInitCond(AQUASYS& system, SOILCOMP* com)
{
   while (1)
   {
      AQVAR* v;
      Write("\n   "); Write(scr.T(1131)); Write(" ");    // InitCond
      Write(com->Symbol()); Write(":\n");
      if ( com->NumInitCond() > 0 )
      {
         Write("\n   "); Write(scr.T(685));              // Index
         Write("   "); Write(scr.T(1132));               // Variable
         Write("("); Write(scr.T(1133)); Write(")");     // Zone
         Write("  :  "); Write(scr.T(1134));              // InitCond
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
      Write("\n   "); Write(scr.T(1135));             // Select
      Write("\n   1   =   "); Write(scr.T(1136));     // AddInitCond
      Write("\n   2   =   "); Write(scr.T(1137));     // EditInitCond
      Write("\n   3   =   "); Write(scr.T(1138));     // DeleteInitCond
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
            Write("\n   "); Write(scr.T(1139));           // EnterAddIndex
            index = com->NumInitCond()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(1140));           // EnterStateVar
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
               Write("\n   "); Write(scr.T(1141));        // SelectZone
               for ( CARDINAL i=0; i<com->NumZones(); i++ )
               {
                  NumToString(i+1,Buf1,BufLen,4);
                  Write("\n"); Write(Buf1); Write("   =   "); 
                  Write(com->ZoneName(i));                // ZoneName
               }
               Write("\n  "); Read(zone,zone+1); zone--;
               if ( zone >= com->NumZones() ) zone = com->NumZones()-1;
            }
            Write("\n   "); Write(scr.T(1142));           // EnterInitCond
            Read(Buf1,BufLen,"");
            
            if ( com->AddInitCond(system.Varlist(),zone,v,Buf1,
                                  Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1143)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1144)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(1145));           // EnterEditIndex
            Read(index,index); index--;
            if ( index >= com->NumInitCond() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                    // IllegalIndex
            }
            Write("\n   "); Write(scr.T(1140));           // Symbol
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
               Write("\n   "); Write(scr.T(1141));        // SelectZone
               for ( CARDINAL i=0; i<com->NumZones(); i++ )
               {
                  NumToString(i+1,Buf1,BufLen,4);
                  Write("\n"); Write(Buf1); Write("   =   "); 
                  Write(com->ZoneName(i));                // ZoneName
               }
               Write("\n  "); Read(zone,com->InitZone(index)+1); zone--;
            }
            Write("\n   "); Write(scr.T(1142));           // EnterInitCond
            (com->InitVal(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( com->ReplaceInitCond(system.Varlist(),zone,v,Buf1,
                                      Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1146)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1147)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(1148));               // EnterDelIndex
            Read(index,index); index--;
            if ( com->DeleteInitCond(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1149)); Write("\n");   // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1150)); Write("\n");   // Failure
            }
            break;
         default:
            break;
      }
   }
   return;
}

//////////////////////////////////////////////////////////////////////////////

static void ReadInput(AQUASYS& system, SOILCOMP* com)
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
      Write("\n   "); Write(scr.T(1151)); Write("\n  "); // EnterExpression
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
      Write("\n   "); Write(scr.T(1152)); Write(" ");   // InputFluxes
      Write(com->Symbol()); Write(":\n");
      if ( com->NumInput() > 0 )
      {
         Write("\n   "); Write(scr.T(685));             // Index
         Write("   "); Write(scr.T(1153));              // Variable
         Write("  :  "); Write(scr.T(1154));            // Input
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
      Write("\n   "); Write(scr.T(1155));                // Select
      Write("\n   1   =   "); Write(scr.T(1156));        // AddInput
      Write("\n   2   =   "); Write(scr.T(1157));        // EditInput
      Write("\n   3   =   "); Write(scr.T(1158));        // DeleteInput
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL numerr;
      CARDINAL index = 1;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(1159));          // EnterAddIndex
            index = com->NumInput()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(1160));          // EnterSymbol
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1161));       // EnterInputFlux
            Read(Buf1,BufLen,"0");
            if ( com->AddInput(system.Varlist(),v,Buf1,
                                Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1162)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1163)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(1164));          // EnterEditIndex
            Read(index,index); index--;
            if ( index >= com->NumInput() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                  // IllegalIndex
            }
            Write("\n   "); Write(scr.T(1160));          // EnterSymbol
            Read(Buf1,BufLen,(com->InputVar(index))->Symbol());
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(1161));       // EnterInputFlux
            (com->InputFlux(index))->UnParse(Buf2,BufLen);
            Read(Buf1,BufLen,Buf2);
            if ( com->ReplaceInput(system.Varlist(),v,Buf1,
                                   Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1165)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1166)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(1167));              // EnterDelIndex
            Read(index,index); index--;
            if ( com->DeleteInput(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(1168)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(1169)); Write("\n"); // Failure
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
      Write("\n   "); Write(scr.T(444)); Write("\n  "); // EnterExpression
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
      Write("\n   "); Write(scr.T(489)); Write(" ");    // InputConc
      Write(com->Symbol()); Write(":\n");
      if ( com->NumLatInput() > 0 )
      {
         Write("\n   "); Write(scr.T(685));             // Index
         Write("   "); Write(scr.T(490));               // Variable
         Write("  :  "); Write(scr.T(491));             // InputConc
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
      Write("\n   "); Write(scr.T(539));                // Select
      Write("\n   1   =   "); Write(scr.T(582));        // AddInput
      Write("\n   2   =   "); Write(scr.T(776));        // EditInput
      Write("\n   3   =   "); Write(scr.T(777));        // DeleteInput
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL numerr;
      CARDINAL index = 1;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(778));          // EnterAddIndex
            index = com->NumLatInput()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(866));          // EnterSymbol
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                             // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(867));       // EnterInputConc
            Read(Buf1,BufLen,"0");
            if ( com->AddLatInput(system.Varlist(),v,Buf1,
                                Buf2,numerr,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(868)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(869)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(870));          // EnterEditIndex
            Read(index,index); index--;
            if ( index >= com->NumLatInput() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                  // IllegalIndex
            }
            Write("\n   "); Write(scr.T(866));          // EnterSymbol
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
               Write("\n   "); Write(scr.T(871)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(872)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(873));              // EnterDelIndex
            Read(index,index); index--;
            if ( com->DeleteLatInput(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(874)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(875)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }
   }

   return;
}

//////////////////////////////////////////////////////////////////////////////


static SERZONE* NewSerZone(AQUASYS& system, const SERZONE* oldserzone)
{
   SERZONE* serzone = new SERZONE(oldserzone);

   while ( 1 )
   {
      Write("\n   "); Write(scr.T(949));        // EnterZoneIndex
      CARDINAL zoneind=serzone->ZoneIndex();
      Read(zoneind,zoneind); // Default auf 1 erhoeht 27.10.
      if ( serzone->ZoneIndex(zoneind) == TRUE ) break;
      Write("\n   "); Write(scr.T(950));  Write("\n");  // IllegalZoneIndex
   }

   CARDINAL numerr;
   CARDINAL n = 0;
   while (1)
   {
      if ( serzone->VolumeFract() != 0 )
      {
         if ( (serzone->VolumeFract())->UnParse(Buf2,BufLen-1) != 0 )
            Buf2[0] = '\0';
      }
      else
      {
         Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(951)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);               // EnterVolFract
      if ( serzone->VolumeFract(Buf1,system.Varlist(),Buf2,numerr)
                                                            == TRUE )
      {
         break;                                  // Success
      }
      else
      {
         n++;
         if ( n > 2 )
         {
            delete serzone; return 0;
         }
         if ( numerr == 0 )
         {                                       // IllegalDep
            Write("\n   "); Write(scr.T(952)); Write("\n");
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
      Write("\n   "); Write(scr.T(953)); Write(" ");     // ExchCoeffs

                            // !!! evtl. weiter oben!
      if ( serzone->NumExch() > 0 )
      {
         Write("\n   "); Write(scr.T(954));              // Index
         Write("   "); Write(scr.T(955));                // Variable
         Write("  :  "); Write(scr.T(956));               // ExchCoeff
         Write(", "); Write(scr.T(957));               // ConvFact1
         for ( CARDINAL i=0; i<serzone->NumExch(); i++ )
         {
            const AQVAR* v;
            Write("\n");
            NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
            Write(' ',strlen(scr.T(954)));
            v = serzone->ExchVar(i);
            Write(v->Symbol()); Write("  :  ");
            const FORMVAR* cfv = serzone->ExchVal(i);
            cfv->UnParse(Buf2,BufLen-1); Write(Buf2); Write(", ");
            cfv = serzone->ExchFac(i);
            cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(958));              // Select
      Write("\n   1   =   "); Write(scr.T(959));      // AddExch
      Write("\n   2   =   "); Write(scr.T(960));      // EditExch
      Write("\n   3   =   "); Write(scr.T(961));      // DeleteExch
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL errorsexch, errorsfact;
      CARDINAL index = 1;
      AQVAR* v;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(962));        // EnterAddIndex
            index = serzone->NumExch()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(963));        // EnterVar
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(964)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(965));
               Write("\n");                           // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(966));         // EnterExch
            Read(Buf1,BufLen,"");
            Write("\n   "); Write(scr.T(967));         // EnterFact
            Read(Buf3,BufLen,"1");
            if ( serzone->AddExchCoeff(system.Varlist(),v,
                                        Buf1,Buf2,errorsexch,
                                        Buf3,Buf4,errorsfact,
                                        index) == TRUE )
            {
               Write("\n   "); Write(scr.T(968)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(969)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(970));        // EnterEditIndex
            Read(index,index); index--;
            Write("\n   "); Write(scr.T(971));        // EnterVar
            Read(Buf1,BufLen,"");
            if ( index >= serzone->NumExch() )
            {
               Write("\n   "); Write(scr.T(972)); Write("\n");
               break;                                 // IllegalIndex
            }
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(964)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(965));
               Write("\n");                           // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(966));         // EnterExch
            Read(Buf1,BufLen,"");
            Write("\n   "); Write(scr.T(967));         // EnterFact
            Read(Buf3,BufLen,"1");
            if ( serzone->ReplaceExchCoeff(system.Varlist(),v,
                                            Buf1,Buf2,errorsexch,
                                            Buf3,Buf4,errorsfact,
                                            index) == TRUE )
            {
               Write("\n   "); Write(scr.T(968)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(969)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(973));      // EnterIndex
            Read(index,index); index--;
            if ( serzone->DeleteExchCoeff(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(974)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(975)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }

   } 
   return serzone; 
}


static void AddSerial(AQUASYS& system, PARZONE* parallel)
{
   Write("\n   "); Write(scr.T(976));        // EnterAddIndex
   CARDINAL index=1; Read(index,index); index--;

   SERZONE* serzone = NewSerZone(system, 0);

   if ( parallel->AddSerial(serzone, index) == TRUE)
   {
      Write("\n   "); Write(scr.T(977)); Write("\n"); // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(978)); Write("\n"); // Failure
      delete serzone;
   }

   return;
}


static void ReplaceSerial(AQUASYS& system, PARZONE* parallel)
{
   Write("\n   "); Write(scr.T(976));        // EnterEditIndex
   CARDINAL index; Read(index,index); index--;

   if ( index >= parallel->NumSerialZones() )
   {
      Write("\n   "); Write(scr.T(972)); Write("\n");
      return;                                 // IllegalIndex
   }

   SERZONE* serzone = NewSerZone(system, parallel->SerialZone(index));

   if ( parallel->ReplaceSerial(serzone,index)==TRUE)
   {
      Write("\n   "); Write(scr.T(977)); Write("\n"); // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(978)); Write("\n"); // Failure
      delete serzone; 
   }
   return;
}


static void DeleteSerial(PARZONE* parallel)
{
   Write("\n   "); Write(scr.T(979));      // EnterIndex
   CARDINAL index=1; Read(index,index); index--;

   if ( parallel->DeleteSerial(index) == TRUE )
   {
      Write("\n   "); Write(scr.T(980)); Write("\n"); // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(981)); Write("\n"); // Failure
   }
   return;
}


static PARZONE* NewParZone(AQUASYS& system, PARZONE* oldparallel)
{
   PARZONE* parallel = new PARZONE(oldparallel);
   ReadSymbol(parallel);
   CARDINAL n = 0;

   while (1)
   {
      Write("\n   "); Write(scr.T(982)); Write(":\n");

      if ( parallel->NumSerialZones() > 0 )
      {
         Write("\n   "); Write(scr.T(983));              // Index
         Write("   "); Write(scr.T(984));                // ZoneIndex
         Write(", "); Write(scr.T(985));                 // VolFract
         for ( CARDINAL i=0; i<parallel->NumSerialZones(); i++ )
         {
            const SERZONE* serzone = parallel->SerialZone(i);
            Write("\n");
            NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
            Write(' ',strlen("Index"));
            Write(serzone->ZoneIndex()); Write(", ");
            const FORMVAR* cfv = serzone->VolumeFract();
            cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(958));            // Select
      Write("\n   1   =   "); Write(scr.T(986));    // AddSerialZone
      Write("\n   2   =   "); Write(scr.T(987));    // EditSerialZone
      Write("\n   3   =   "); Write(scr.T(988));    // DelSerialZone
      Write("\n");

      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);

      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;

      CARDINAL index = 1;

      switch ( Buf1[0] )
      {
         case '1':
            AddSerial(system, parallel);
            break;
         case '2':
            ReplaceSerial(system, parallel);
            break;
         case '3':
            DeleteSerial(parallel);
            break;
         default:
            break;
      }
  }

  return parallel;
}
         

static void AddParZone(AQUASYS& system, SOILCOMP* soilcomp)
{
   PARZONE* parallel = NewParZone(system,0);
            
   if ( soilcomp->AddParZone(parallel) == TRUE )
   {
      Write("\n   "); Write(scr.T(990)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(991));
      Write("\n");  // Failure
      delete parallel;
   }
   return;
}


static void EditParZone(AQUASYS& system, SOILCOMP* soilcomp)
{

   Write("\n   "); Write(scr.T(992));   // EnterSymbol

   Read(Buf1,BufLen,"");

   PARZONE* oldparallel = soilcomp->GetParZone(Buf1);
   if ( oldparallel == 0 )
   {
      Write("\n   "); Write(scr.T(993)); Write(" "); Write(Buf1);
      Write(" "); Write(scr.T(994)); Write("\n"); 
                                                         // ZoneNotFound
   } 

   PARZONE* parallel = NewParZone(system,oldparallel);
           
   if ( soilcomp->ReplaceParZone(oldparallel,parallel) == TRUE )
   {
      Write("\n   "); Write(scr.T(995)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(996)); Write("\n");  // Failure
      delete parallel;
   }
   return;
}            


static void DelParZone(SOILCOMP* soilcomp)
{
   Write("\n   "); Write(scr.T(997));  // EnterSymbol
   Read(Buf1,BufLen,"");
   if ( soilcomp->DeleteParZone(Buf1) == TRUE )
   {
      Write("\n   "); Write(scr.T(998)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(999)); Write("\n");  // Failure
   }
   return;
}


SOILCOMP* EditSoilComp(AQUASYS& system, SOILCOMP* soilcomp)
{
   ReadSymbol(soilcomp);

   ReadCompInd(soilcomp);

   CARDINAL n = 0; REAL r,s;

   Write("\n   "); Write(scr.T(876));   // EnterStart
   Read(r,soilcomp->XStart());

   Write("\n   "); Write(scr.T(877));   // EnterEnd
   Read(s,soilcomp->XEnd()); soilcomp->XStartEnd(r,s);

   CARDINAL numerr;
   n = 0;
   while (1)
   {
      if ( soilcomp->CrossSect() != 0 )
      {
         if ( (soilcomp->CrossSect())->UnParse(Buf2,BufLen-1) != 0 )
            Buf2[0] = '\0';
      }
      else
      {
         Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(878)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);               // EnterCrossSect
      if ( soilcomp->CrossSect(Buf1,system.Varlist(),Buf2,numerr)
                                                            == TRUE )
      {
         break;                                  // Success
      }
      else
      {
         n++;
         if ( n > 2 )
         {
            delete soilcomp; return 0;
         }
         if ( numerr == 0 )
         {                                       // IllegalDep
            Write("\n   "); Write(scr.T(887)); Write("\n");
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

   if ( soilcomp->WithDispersion() == TRUE ) Buf2[0] = '1';
   else                                      Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(879));          // Select
   Write("\n   1   =   "); Write(scr.T(880));  // WithDispersion
   Write("\n   2   =   "); Write(scr.T(881));  // WithoutDispersion
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0]=='1' )
   {
      n = 0;
      while (1)
      {
         if ( soilcomp->Dispersion() != 0 )
         {
            if ( (soilcomp->Dispersion())->UnParse(Buf2,BufLen-1) != 0 )
               Buf2[0] = '\0';
         }
         else
         {
            Buf2[0] = '\0';
         }
         Write("\n   "); Write(scr.T(882)); Write("\n  ");
         Read(Buf1,BufLen,Buf2);            // EnterDispersion
         if ( soilcomp->Dispersion(Buf1,system.Varlist(),Buf2,numerr)
                                                                == TRUE )
         {
            break;
         }
         else
         {
            n++;
            if ( n > 2 )
            {
               delete soilcomp; return 0;
            }
            if ( numerr == 0 )
            {                                       // IllegalDep
               Write("\n   "); Write(scr.T(887)); Write("\n");
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
      soilcomp->WithDispersion(TRUE);
   }
   else
   {
      soilcomp->WithDispersion(FALSE);
   }
 
   while (1)
   {
      if ( soilcomp->ParZoneList()->Size() > 0 )
      {
         Write("\n   "); Write(scr.T(1000)); Write(":");
         CARDINAL i = 0; 
 //        CARDINAL i = 1;     // Auf Verdacht veraendert!
         PARZONE* parallel = soilcomp->ParZoneList()->First();
         while ( parallel != 0 )
         {
            i++;
            if ( (i-1) % scr.I(1) == 0 ) Write("\n   ");
            Write(parallel->Symbol());
            Write(' ',scr.L(1)-strlen(parallel->Symbol()));
            parallel = parallel->Next();
         }
         Write("\n");
      }

      Write("\n   "); Write(scr.T(1001));               // SelectParZone
      Write("\n   1   =   "); Write(scr.T(1002));       // AddParZone
      Write("\n   2   =   "); Write(scr.T(1003));       // EditParZone
      Write("\n   3   =   "); Write(scr.T(1004));       //DeleteParZone

      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));

      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;

      switch (Buf1[0])
      {
         case '1':
               AddParZone(system, soilcomp);
               break;

         case '2':
               EditParZone(system, soilcomp);    
               break;

         case '3':
               DelParZone(soilcomp);       
               break;
               default:
               break;
      }
   }

   Write("\n   "); Write(scr.T(883));   // EnterNumGrid
   Read(n,soilcomp->NumGridPts()); soilcomp->NumGridPts(n);
   if ( soilcomp->HighRes() == FALSE ) Buf2[0] = '1';
   else                                Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(884));          // Select
   Write("\n   1   =   "); Write(scr.T(885));  // LowRes
   Write("\n   2   =   "); Write(scr.T(886));  // HighRes
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0]=='1' ) soilcomp->HighRes(FALSE);
   if ( Buf1[0]=='2' ) soilcomp->HighRes(TRUE);

   if ( soilcomp->Active() == TRUE ) Buf2[0] = '1';
   else                              Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(1395));                 // Status
   Write("\n   1   =   "); Write(scr.T(1396));         // Active
   Write("\n   2   =   "); Write(scr.T(1397));         // Inactive
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '2' ) soilcomp->Active(FALSE);
   else                  soilcomp->Active(TRUE);

   ReadActVar(system,soilcomp);
   ReadActProc(system,soilcomp);

   ReadInitCond(system,soilcomp);
   ReadInput(system,soilcomp);

   return soilcomp;
}


//////////////////////////////////////////////////////////////////////////////
