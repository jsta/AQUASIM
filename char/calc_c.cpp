////////////////////////////////   calc_c.C   ////////////////////////////////
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
#include "calc_c.h"

//////////////////////////////////////////////////////////////////////////////

// global buffers and response characters (aquasim.C)

extern const CARDINAL  BufLen;
extern char            Buf[];
extern char            Buf1[];
extern char            Buf2[];

extern char            AQ_SaveFile[];
extern char            AQ_SensFile[];
extern char            AQ_FitFile[];

extern char            C1, C2, Y1, Y2, N1, N2;

//////////////////////////////////////////////////////////////////////////////

static std::ofstream sensfile;
static std::ofstream fitfile;

//////////////////////////////////////////////////////////////////////////////

static CARDINAL CharCount = 0;
static CARDINAL CalcCount = 0;
static CARDINAL SensCount = 0;
static CARDINAL FitCount  = 0;

BOOLEAN CalcOut(char*,REAL)
{
   if ( CalcCount % 10 == 0 )
   {
      if ( CharCount >= 60 )
      {
         Write("\n   "); CharCount = 0;
      }
      Write("."); WriteBuffer();
      CharCount++;
   }
   CalcCount++;
   return TRUE;
}


void SensOut()
{
   SensCount++;
   Write("\n   "); Write(scr.T(799)); Write(" "); Write(SensCount);
   Write(" "); Write(scr.T(800));
   WriteBuffer();
   return;
}


void FitOut(REAL chi)
{
   FitCount++;
   Write("\n   "); Write(scr.T(438)); Write(" "); Write(FitCount);
   Write(":   "); Write(scr.T(439)); Write(" = "); Write(chi);
   WriteBuffer();
   return;
}

//////////////////////////////////////////////////////////////////////////////

static CALC* NewCalc(CALC* oldcalc)
{
   CALC* calc = new CALC(oldcalc);
   CARDINAL n; REAL r;

   while (1)
   {
      Write("\n   "); Write(scr.T(44));               // EnterSymbol
      Read(Buf1,BufLen,calc->Symbol());
      if ( (FORMVAR::ReservedSymbol(Buf1)==FALSE)
           && (calc->Symbol(Buf1)==TRUE)           ) break;
      Write("\n   "); Write(scr.T(45)); Write("\n");  // IllegalSymbol
   }

   Write("\n   "); Write(scr.T(46));                  // EnterDescription
   Read(Buf1,BufLen,calc->Description()); calc->Description(Buf1);

   Write("\n   "); Write(scr.T(459));                 // EnterCalcNum
   n = calc->CalcNum();
   Read(n,n); calc->CalcNum(n);

   Write("\n   "); Write(scr.T(460));                 // EnterInitTime
   r = calc->InitTime();
   Read(r,r); calc->InitTime(r);

   if ( calc->InitSteady() == FALSE ) Buf2[0] = '1';
   else                               Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(461));                 // SelectInitState
   Write("\n   1   =   "); Write(scr.T(462));         // Given
   Write("\n   2   =   "); Write(scr.T(463));         // SteadyState
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '2' ) calc->InitSteady(TRUE);
   else                  calc->InitSteady(FALSE);

   BOOLEAN b = TRUE;
   do
   {
      if ( calc->NumStepSeries() > 0 )
      {
         Write("\n   "); Write(scr.T(291));           // StepSeries
         Write("\n     "); Write(scr.T(1356));        // Index
         Write(":  "); Write(scr.T(1354));            // StepSize
         Write(", "); Write(scr.T(1355));             // NumSteps
         for ( CARDINAL i=0; i<calc->NumStepSeries(); i++ )
         {
            Write("\n     "); Write(i+1);
            Write(":  "); Write(calc->StepSize(i));
            Write(", "); Write(calc->NumSteps(i));
         }
      }
      else
      {
         Write("\n   "); Write(scr.T(292)); Write("\n");
      }                                          // NoStepSeries
      Write("\n");
      Write("\n   "); Write(scr.T(1357));
      Write("\n   1   =   "); Write(scr.T(1358));  // AddSeries
      Write("\n   2   =   "); Write(scr.T(1359));  // EditSeries
      Write("\n   3   =   "); Write(scr.T(1360));  // DeleteSeries
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') )
         b = FALSE;
      switch (Buf1[0])
      {
         case '1':
         {
            REAL stepsize = 1; CARDINAL numsteps = 1;
            Write("\n   "); Write(scr.T(467));
            Read(stepsize,stepsize);
            Write("\n   "); Write(scr.T(468));
            Read(numsteps,numsteps);
            CARDINAL pos = calc->NumStepSeries()+1;
            Write("\n   "); Write(scr.T(1361));
            Read(pos,pos);
            if ( calc->AddStepSeries(stepsize,numsteps,pos-1) == TRUE )
            {
               Write("\n   "); Write(scr.T(1362)); Write("\n");
            }                                    // Success
            else
            {
               Write("\n   "); Write(scr.T(1363)); Write("\n");
            }                                    // Failure
            break;
         }
         case '2':
         {
            CARDINAL pos = calc->NumStepSeries();
            Write("\n   "); Write(scr.T(1364));
            Read(pos,pos);
            if ( pos==0 || pos>calc->NumStepSeries() )
            {
               Write("\n   "); Write(scr.T(1365));
               break;
            }
            REAL stepsize = calc->StepSize(pos-1); 
            CARDINAL numsteps = calc->NumSteps(pos-1);
            Write("\n   "); Write(scr.T(467));
            Read(stepsize,stepsize);
            Write("\n   "); Write(scr.T(468));
            Read(numsteps,numsteps);
            if ( calc->ReplaceStepSeries(stepsize,numsteps,pos-1) == TRUE )
            {
               Write("\n   "); Write(scr.T(1366)); Write("\n");
            }                                    // Success
            else
            {
               Write("\n   "); Write(scr.T(1367)); Write("\n");
            }                                    // Failure
            break;
         }
         case '3':
         {
            CARDINAL pos = calc->NumStepSeries();
            Write("\n   "); Write(scr.T(1364));
            Read(pos,pos);
            if ( pos==0 || pos>calc->NumStepSeries() )
            {
               Write("\n   "); Write(scr.T(1365));
               break;
            }
            if ( calc->DelStepSeries(pos-1) == TRUE )
            {
               Write("\n   "); Write(scr.T(1369)); Write("\n");
            }                                    // Success
            else
            {
               Write("\n   "); Write(scr.T(1370)); Write("\n");
            }                                    // Failure
            break;
         }
         default:
            break;
      }
   } while ( b == TRUE );

   if ( calc->SimActive() == TRUE ) Buf2[0] = '1';
   else                             Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(1353));                 // SelectSimStat
   Write("\n   1   =   "); Write(scr.T(465));          // Active
   Write("\n   2   =   "); Write(scr.T(466));          // Inactive
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) calc->SimActive(TRUE);
   else                  calc->SimActive(FALSE);

   if ( calc->SensActive() == TRUE ) Buf2[0] = '1';
   else                              Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(464));                  // SelectSensStat
   Write("\n   1   =   "); Write(scr.T(465));          // Active
   Write("\n   2   =   "); Write(scr.T(466));          // Inactive
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) calc->SensActive(TRUE);
   else                  calc->SensActive(FALSE);
   return calc;
}


static void AddCalc(AQUASYS& system)
{
   CALC* calc = NewCalc(0);
   if ( system.AddCalc(calc) == TRUE )
   {
      Write("\n   "); Write(scr.T(469)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(470)); Write("\n");  // Failure
      delete calc;
   }
}


static void EditCalc(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(471));                 // EnterSymbol
   Read(Buf1,BufLen,"");
   CALC* oldcalc = system.GetCalc(Buf1);
   if ( oldcalc == 0 )
   {
      Write("\n   "); Write(scr.T(472)); Write(" "); Write(Buf1);
      Write(" "); Write(scr.T(473)); Write("\n");     // CalcNotFound
      return;
   }
   CALC* calc = NewCalc(oldcalc);
   if ( system.ReplaceCalc(oldcalc,calc) == TRUE )
   {
      Write("\n   "); Write(scr.T(474)); Write("\n"); // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(475)); Write("\n"); // Failure
      delete calc;
   }
}


static void DeleteCalc(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(476));                 // EnterSymbol
   Read(Buf1,BufLen,"");
   if ( system.DeleteCalc(Buf1) == TRUE )
   {
      Write("\n   "); Write(scr.T(477)); Write("\n"); // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(478)); Write("\n"); // Failure
   }
}


static void ActivateCalcSim(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(1347));                    // EnterSymbol
   Read(Buf1,BufLen,"");
   CALC* calc = system.GetCalc(Buf1);
   if ( calc == 0 )
   {
      Write("\n   "); Write(scr.T(1349)); Write("\n");    // Failure
   }
   else
   {
      if ( system.SimActive(calc,TRUE) == TRUE )
      {
         Write("\n   "); Write(scr.T(1348)); Write("\n"); // Success
      }
      else
      {
         Write("\n   "); Write(scr.T(1349)); Write("\n"); // Failure
      }
   }
}


static void InactivateCalcSim(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(1350));                    // EnterSymbol
   Read(Buf1,BufLen,"");
   CALC* calc = system.GetCalc(Buf1);
   if ( calc == 0 )
   {
      Write("\n   "); Write(scr.T(1352)); Write("\n");    // Failure
   }
   else
   {
      if ( system.SimActive(calc,FALSE) == TRUE )
      {
         Write("\n   "); Write(scr.T(1351)); Write("\n"); // Success
      }
      else
      {
         Write("\n   "); Write(scr.T(1352)); Write("\n"); // Failure
      }
   }
}


static void InitSim(AQUASYS& system)
{
   system.SetCalcOut(CalcOut);
   CalcCount = 0; CharCount = strlen(scr.T(295));
   Write("\n   "); Write(scr.T(295)); WriteBuffer();     // Calculating
   JOBSTATUS status = system.InitSim();
   Write("\n");
   if ( status == OK )
   {
      Write("\n   "); Write(scr.T(289)); Write("\n");    // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(290)); Write("\n");    // Problems
   }
   system.SetCalcOut(0);
   return;
}

static void Simulate(AQUASYS& system)
{
   system.SetCalcOut(CalcOut);
   CalcCount = 0; CharCount = strlen(scr.T(295));
   Write("\n   "); Write(scr.T(295)); WriteBuffer();     // Calculating
   JOBSTATUS status = system.Simulate();
   Write("\n");
   if ( status == OK )
   {
      Write("\n   "); Write(scr.T(293)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(294)); Write("\n");  // Problems
   }
   system.SetCalcOut(0);
   return;
}

static void DefineSim(AQUASYS& system)
{
   while (1)
   {
      Write("\n   "); Write(scr.T(323)); Write(":\n");    // AvailableCalcs
      CARDINAL i = 0;
      CALC* calc = system.Calclist()->First();
      while ( calc != 0 )
      {
         if ( i % scr.I(1) == 0 ) Write("\n   ");
         i++;
         Write(calc->Symbol());
         Write(' ',scr.L(1)-strlen(calc->Symbol()));
         calc = calc->Next();
      }
      Write("\n");
      Write("\n   "); Write(scr.T(288)); Write(":\n");    // ActiveCalcs
      i = 0;
      calc = system.Calclist()->First();
      while ( calc != 0 )
      {
         if ( calc->SimActive() == TRUE )
         {
            if ( i % scr.I(1) == 0 ) Write("\n   ");
            i++;
            Write(calc->Symbol());
            Write(' ',scr.L(1)-strlen(calc->Symbol()));
         }
         calc = calc->Next();
      }
      Write("\n");
      Write("\n   "); Write(scr.T(302));
      Write("\n   1   =   "); Write(scr.T(303));       // AddCalc
      Write("\n   2   =   "); Write(scr.T(304));       // EditCalc
      Write("\n   3   =   "); Write(scr.T(1344));      // DeleteCalc
      Write("\n   4   =   "); Write(scr.T(1345));      // Activate
      Write("\n   5   =   "); Write(scr.T(1346));      // Inactivate
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      switch (Buf1[0])
      {
         case '1':
            AddCalc(system);
            break;
         case '2':
            EditCalc(system);
            break;
         case '3':
            DeleteCalc(system);
            break;
         case '4':
            ActivateCalcSim(system);
            break;
         case '5':
            InactivateCalcSim(system);
            break;
         default:
            break;
      }
   }
   return;
}

static void Simulation(AQUASYS& system)
{
   while ( 1 )
   {
      Write("\n   "); Write(scr.T(266)); Write(":");  // Sim Task
      Write("\n   1   =   "); Write(scr.T(1229));  // Define
      Write("\n   2   =   "); Write(scr.T(452));   // Init
      Write("\n   3   =   "); Write(scr.T(422));   // Start
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf,BufLen);
      if ( (Buf[0]==C1) || (Buf[0]==C2) || (Buf[0]=='\0') ) return;
      switch (Buf[0])
      {
         case '1':
            DefineSim(system);
            break;
         case '2':
            InitSim(system);
            break;
         case '3':
            Simulate(system);
            break;
         default:
            break;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////

static void ActivateSensPar(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(453));                       // EnterSymbol
   Read(Buf1,BufLen,"");
   AQVAR* var = system.GetVar(Buf1);
   if ( var == 0 )
   {
      Write("\n   "); Write(scr.T(454)); Write("\n");       // Failure
   }
   else
   {
      if ( var->SensActive(TRUE) == TRUE )
      {
         Write("\n   "); Write(scr.T(455)); Write("\n"); // Success
         system.Changed();
      }
      else
      {
         Write("\n   "); Write(scr.T(454)); Write("\n"); // Failure
      }
   }
}


static void InactivateSensPar(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(456));                       // EnterSymbol
   Read(Buf1,BufLen,"");
   AQVAR* var = system.GetVar(Buf1);
   if ( var == 0 )
   {
      Write("\n   "); Write(scr.T(458)); Write("\n");       // Failure
   }
   else
   {
      if ( var->SensActive(FALSE) == TRUE )
      {
         Write("\n   "); Write(scr.T(457)); Write("\n"); // Success
         system.Changed();
      }
      else
      {
         Write("\n   "); Write(scr.T(458)); Write("\n"); // Failure
      }
   }
}


static void ActivateCalcSens(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(479));                    // EnterSymbol
   Read(Buf1,BufLen,"");
   CALC* calc = system.GetCalc(Buf1);
   if ( calc == 0 )
   {
      Write("\n   "); Write(scr.T(664)); Write("\n");    // Failure
   }
   else
   {
      if ( system.SensActive(calc,TRUE) == TRUE )
      {
         Write("\n   "); Write(scr.T(663)); Write("\n"); // Success
      }
      else
      {
         Write("\n   "); Write(scr.T(664)); Write("\n"); // Failure
      }
   }
}


static void InactivateCalcSens(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(665));                    // EnterSymbol
   Read(Buf1,BufLen,"");
   CALC* calc = system.GetCalc(Buf1);
   if ( calc == 0 )
   {
      Write("\n   "); Write(scr.T(667)); Write("\n");    // Failure
   }
   else
   {
      if ( system.SensActive(calc,FALSE) == TRUE )
      {
         Write("\n   "); Write(scr.T(666)); Write("\n"); // Success
      }
      else
      {
         Write("\n   "); Write(scr.T(667)); Write("\n"); // Failure
      }
   }
}


static void DefineSens(AQUASYS& system)
{
   while (1)
   {
      Write("\n   "); Write(scr.T(668)); Write(":\n");    // AvailablePars
      CARDINAL i = 0;
      AQVAR* var = system.Varlist()->First();
      while ( var != 0 )
      {
         if ( var->Type() == ConstVar )
         {
            if ( i % scr.I(1) == 0 ) Write("\n   ");
            i++;
            Write(var->Symbol());
            Write(' ',scr.L(1)-strlen(var->Symbol()));
         }
         if ( var->Type() == RealListVar )
         {
            if ( i % scr.I(1) == 0 ) Write("\n   ");
            i++;
            Write(var->Symbol());
            Write(' ',scr.L(1)-strlen(var->Symbol()));
         }
         var = var->Next();
      }
      Write("\n");
      Write("\n   "); Write(scr.T(669)); Write(":\n");    // ActivePars
      i = 0;
      var = system.Varlist()->First();
      while ( var != 0 )
      {
         if ( var->SensActive() == TRUE )
         {
            if ( i % scr.I(1) == 0 ) Write("\n   ");
            i++;
            Write(var->Symbol());
            Write(' ',scr.L(1)-strlen(var->Symbol()));
         }
         var = var->Next();
      }
      Write("\n");
      Write("\n   "); Write(scr.T(670));
      Write("\n   1   =   "); Write(scr.T(671));       // Activate
      Write("\n   2   =   "); Write(scr.T(672));       // Inactivate
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      switch (Buf1[0])
      {
         case '1':
            ActivateSensPar(system);
            break;
         case '2':
            InactivateSensPar(system);
            break;
         default:
            break;
      }
   }
   while (1)
   {
      Write("\n   "); Write(scr.T(673)); Write(":\n");    // AvailableCalcs
      CARDINAL i = 0;
      CALC* calc = system.Calclist()->First();
      while ( calc != 0 )
      {
         if ( i % scr.I(1) == 0 ) Write("\n   ");
         i++;
         Write(calc->Symbol());
         Write(' ',scr.L(1)-strlen(calc->Symbol()));
         calc = calc->Next();
      }
      Write("\n");
      Write("\n   "); Write(scr.T(674)); Write(":\n");    // ActiveCalcs
      i = 0;
      calc = system.Calclist()->First();
      while ( calc != 0 )
      {
         if ( calc->SensActive() == TRUE )
         {
            if ( i % scr.I(1) == 0 ) Write("\n   ");
            i++;
            Write(calc->Symbol());
            Write(' ',scr.L(1)-strlen(calc->Symbol()));
         }
         calc = calc->Next();
      }
      Write("\n");
      Write("\n   "); Write(scr.T(675));
      Write("\n   1   =   "); Write(scr.T(676));       // AddCalc
      Write("\n   2   =   "); Write(scr.T(677));       // EditCalc
      Write("\n   3   =   "); Write(scr.T(678));       // DeleteCalc
      Write("\n   4   =   "); Write(scr.T(679));       // Activate
      Write("\n   5   =   "); Write(scr.T(680));       // Inactivate
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      switch (Buf1[0])
      {
         case '1':
            AddCalc(system);
            break;
         case '2':
            EditCalc(system);
            break;
         case '3':
            DeleteCalc(system);
            break;
         case '4':
            ActivateCalcSens(system);
            break;
         case '5':
            InactivateCalcSens(system);
            break;
         default:
            break;
      }
   }
   return;
}

static void StartSens(AQUASYS& system)
{
   CARDINAL num = 0;
   AQVAR* var = (system.Varlist())->First();
   while ( var != 0 )
   {
      if ( var->SensActive() == TRUE ) num++;
      var = var->Next();
   }
   if ( num == 0 )
   {
      Write("\n   "); Write(scr.T(803)); Write("\n");      // NoActPar
      return;
   }
   num = 0;
   CALC* calc = (system.Calclist())->First();
   while ( calc != 0 )
   {
      if ( calc->SensActive() == TRUE ) num++;
      calc = calc->Next();
   }
   if ( num == 0 )
   {
      Write("\n   "); Write(scr.T(804)); Write("\n");      // NoActCalc
      return;
   }

   if ( strlen(AQ_SensFile)==0 && strlen(AQ_SaveFile)>0 )
   {
      strcpy(AQ_SensFile,AQ_SaveFile);
      char* p = strchr(AQ_SensFile,'.');
      if ( p != 0 ) *p = '\0';
      strcat(AQ_SensFile,FEsen);
   }
   Write("\n   "); Write(scr.T(423));                      // EnterFileName
   Read(Buf,BufLen,AQ_SensFile);
   strcpy(AQ_SensFile,Buf);
   sensfile.open(Buf,std::ios::out|std::ios::trunc);
   SetFileType(Buf,FTsen,FCsen);

   system.SetSensOut(SensOut); SensCount = 0;
   Write("\n   "); Write(scr.T(295)); Write("\n");         // Calculating
   WriteBuffer();
   JOBSTATUS status = system.SensAnal(TRUE);
   if ( status == OK )
   {
      status = system.SensRank(TRUE,&sensfile);
   }
   Write("\n");
   if ( status == OK )
   {
      Write("\n   "); Write(scr.T(681)); Write("\n");      // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(682)); Write("\n");      // Problems
   }
   system.SetSensOut(0);
   sensfile.close();
   return;
}

static void SensitivityAnalysis(AQUASYS& system)
{
   while (1)
   {
      Write("\n   "); Write(scr.T(827)); Write(":");  // Sens Task
      Write("\n   1   =   "); Write(scr.T(828));   // Def
      Write("\n   2   =   "); Write(scr.T(829));   // Start
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf,BufLen);
      if ( (Buf[0]==C1) || (Buf[0]==C2) || (Buf[0]=='\0') ) return;
      switch (Buf[0])
      {
         case '1':
            DefineSens(system);
            break;
         case '2':
            StartSens(system);
            return;
         default:
            break;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////

static void ActivateFitPar(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(344));                       // EnterSymbol
   Read(Buf1,BufLen,"");
   AQVAR* var = system.GetVar(Buf1);
   if ( var == 0 )
   {
      Write("\n   "); Write(scr.T(346)); Write("\n");       // Failure
   }
   else
   {
      if ( var->Type() == ConstVar )
      {
         CONSTVAR* cv = (CONSTVAR*)var;
         if ( cv->FitActive(TRUE) == TRUE )
         {
            Write("\n   "); Write(scr.T(345)); Write("\n"); // Success
            system.Changed();
         }
         else
         {
            Write("\n   "); Write(scr.T(346)); Write("\n"); // Failure
         }
      }
      else
      {
         Write("\n   "); Write(scr.T(346)); Write("\n");    // Failure
      }
   }
}


static void InactivateFitPar(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(347));                       // EnterSymbol
   Read(Buf1,BufLen,"");
   AQVAR* var = system.GetVar(Buf1);
   if ( var == 0 )
   {
      Write("\n   "); Write(scr.T(349)); Write("\n");       // Failure
   }
   else
   {
      if ( var->Type() == ConstVar )
      {
         CONSTVAR* cv = (CONSTVAR*)var;
         if ( cv->FitActive(FALSE) == TRUE )
         {
            Write("\n   "); Write(scr.T(348)); Write("\n"); // Success
            system.Changed();
         }
         else
         {
            Write("\n   "); Write(scr.T(349)); Write("\n"); // Failure
         }
      }
      else
      {
         Write("\n   "); Write(scr.T(349)); Write("\n");    // Failure
      }
   }
}


static FIT* NewFit(AQUASYS& system, FIT* oldfit)
{
   FIT* fit = new FIT(oldfit);
   CARDINAL n; REAL r;

   while (1)
   {
      Write("\n   "); Write(scr.T(44));               // EnterSymbol
      Read(Buf1,BufLen,fit->Symbol());
      if ( fit->Symbol(Buf1)==TRUE ) break;
      Write("\n   "); Write(scr.T(45)); Write("\n");  // IllegalSymbol
   }

   Write("\n   "); Write(scr.T(46));                  // EnterDescription
   Read(Buf1,BufLen,fit->Description()); fit->Description(Buf1);

   Write("\n   "); Write(scr.T(387));                 // EnterCalcNum
   n = fit->CalcNum();
   Read(n,n); fit->CalcNum(n);

   Write("\n   "); Write(scr.T(388));                 // EnterInitTime
   r = fit->InitTime();
   Read(r,r); fit->InitTime(r);

   if ( fit->InitSteady() == FALSE ) Buf2[0] = '1';
   else                              Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(394));                 // SelectInitState
   Write("\n   1   =   "); Write(scr.T(406));         // Given
   Write("\n   2   =   "); Write(scr.T(407));         // SteadyState
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '2' ) fit->InitSteady(TRUE);
   else                  fit->InitSteady(FALSE);

   while (1)
   {
      Write("\n   "); Write(scr.T(389)); Write(":\n");  // OptQuant
      if ( fit->NumData() > 0 )
      {
         Write("\n   "); Write(scr.T(685));             // Index
         Write("   "); Write(scr.T(324));               // Data
         Write("  :  "); Write(scr.T(325));             // Variable
         Write(" ("); Write(scr.T(326));                // Compartment
         Write(", "); Write(scr.T(328));                // Zone
         Write(", "); Write(scr.T(327));                // Space/Time
         Write(")");
         for ( CARDINAL i=0; i<fit->NumData(); i++ )
         {
            FITDATA data = fit->Data(i);
            Write("\n");
            NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
            Write(' ',strlen(scr.T(685)));
            Write((data.data)->Symbol());
            Write("  :  "); Write((data.var)->Symbol());
            Write(" ("); Write((data.com)->Symbol());
            Write(", "); Write((data.com)->ZoneName(data.zone));
            Write(", "); Write(data.spacetime); 
            if ( data.spacerel == TRUE )
            {
               Write(" rel.space");
            }
            Write(")");
         }
      }
      Write("\n");
      Write("\n   "); Write(scr.T(391)); Write(":");  // Select
      Write("\n   1   =   "); Write(scr.T(392));      // AddTarget
      Write("\n   2   =   "); Write(scr.T(837));      // EditTarget
      Write("\n   3   =   "); Write(scr.T(393));      // DeleteTarget
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL index = 1; FITDATA data;
      switch (Buf1[0])
      {
         case '1':
            Write("\n   "); Write(scr.T(329));       // EnterIndex
            index = fit->NumData()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(395));       // EnterData
            Read(Buf1,BufLen,"");
            data.data = system.GetVar(Buf1);
            Write("\n   "); Write(scr.T(390));       // EnterVar
            Read(Buf1,BufLen,"");
            data.var = system.GetVar(Buf1);
            data.com = (system.Complist())->First();
            if ( system.Complist()->Size() > 1 )
            {
               Write("\n   "); Write(scr.T(396));    // EnterComp
               Read(Buf1,BufLen,"");
               data.com = system.GetComp(Buf1);
            }
            if ( data.com == 0 )
            {
               Write("\n   "); Write(scr.T(191));    // CompNotFound
               Write(" "); Write(scr.T(192)); Write("\n");
               break;
            }
            data.zone = 0;
            if ( (data.com)->NumZones() > 1 )
            {
               Write("\n   "); Write(scr.T(838));    // SelectZone
               for ( CARDINAL i=0; i<(data.com)->NumZones(); i++ )
               {
                  NumToString(i+1,Buf1,BufLen,4);
                  Write("\n"); Write(Buf1); Write("   =   "); 
                  Write((data.com)->ZoneName(i));    // ZoneName
               }
               Write("\n  "); Read(data.zone,data.zone+1); data.zone--;
               if ( data.zone >= (data.com)->NumZones() )
                  data.zone = (data.com)->NumZones()-1;
            }
            Write("\n   "); Write(scr.T(397));       // EnterSpaceTime
            data.spacetime = 0.0;
            Read(data.spacetime,data.spacetime);
            if ( data.spacerel == FALSE ) Buf2[0] = '1';
            else                          Buf2[0] = '2';
            Buf2[1] = '\0';
            Write("\n   "); Write(scr.T(1401));         // SelSpace
            Write("\n   1   =   "); Write(scr.T(1402)); // Abs
            Write("\n   2   =   "); Write(scr.T(1403)); // Rel
            Write("\n  ");
            Read(Buf1,BufLen,Buf2);
            if ( Buf1[0] == '1' ) data.spacerel = FALSE;
            else                  data.spacerel = TRUE;
            if ( fit->Add(data,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(330)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(331)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(841));       // EnterIndex
            Read(index,index); index--;
            if ( index >= fit->NumData() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                                    // IllegalIndex
            }
            data = fit->Data(index);
            Write("\n   "); Write(scr.T(395));       // EnterData
            Read(Buf1,BufLen,(data.data)->Symbol());
            data.data = system.GetVar(Buf1);
            Write("\n   "); Write(scr.T(390));       // EnterVar
            Read(Buf1,BufLen,(data.var)->Symbol());
            data.var = system.GetVar(Buf1);
            if ( system.Complist()->Size() > 1 )
            {
               Write("\n   "); Write(scr.T(396));    // EnterComp
               Read(Buf1,BufLen,(data.com)->Symbol());
               data.com = system.GetComp(Buf1);
            }
            if ( data.com == 0 )
            {
               Write("\n   "); Write(scr.T(191));    // CompNotFound
               Write(" "); Write(scr.T(192)); Write("\n");
               break;
            }
            if ( data.zone >= (data.com)->NumZones() ) data.zone = 0;
            if ( (data.com)->NumZones() > 1 )
            {
               Write("\n   "); Write(scr.T(838));    // SelectZone
               for ( CARDINAL i=0; i<(data.com)->NumZones(); i++ )
               {
                  NumToString(i+1,Buf1,BufLen,4);
                  Write("\n"); Write(Buf1); Write("   =   "); 
                  Write((data.com)->ZoneName(i));    // ZoneName
               }
               Write("\n  "); Read(data.zone,data.zone+1); data.zone--;
               if ( data.zone >= (data.com)->NumZones() )
                  data.zone = (data.com)->NumZones()-1;
            }
            Write("\n   "); Write(scr.T(397));       // EnterSpaceTime
            Read(data.spacetime,data.spacetime);
            if ( data.spacerel == FALSE ) Buf2[0] = '1';
            else                          Buf2[0] = '2';
            Buf2[1] = '\0';
            Write("\n   "); Write(scr.T(1401));         // SelSpace
            Write("\n   1   =   "); Write(scr.T(1402)); // Abs
            Write("\n   2   =   "); Write(scr.T(1403)); // Rel
            Write("\n  ");
            Read(Buf1,BufLen,Buf2);
            if ( Buf1[0] == '1' ) data.spacerel = FALSE;
            else                  data.spacerel = TRUE;
            if ( fit->Replace(data,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(839)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(840)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(398)); Write(": ");    // EnterIndex
            Read(index,index); index--;
            if ( fit->Delete(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(399)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(400)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }
   }
   if ( fit->FitActive() == TRUE ) Buf2[0] = '1';
   else                            Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(401));                  // SelectStatus
   Write("\n   1   =   "); Write(scr.T(402));          // Active
   Write("\n   2   =   "); Write(scr.T(403));          // Inactive
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) fit->FitActive(TRUE);
   else                  fit->FitActive(FALSE);
   return fit;
}


static void AddFit(AQUASYS& system)
{
   FIT* fit = NewFit(system,0);
   if ( system.AddFit(fit) == TRUE )
   {
      Write("\n   "); Write(scr.T(377)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(378)); Write("\n");  // Failure
      delete fit;
   }
}


static void EditFit(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(379));                 // EnterSymbol
   Read(Buf1,BufLen,"");
   FIT* oldfit = system.GetFit(Buf1);
   if ( oldfit == 0 )
   {
      Write("\n   "); Write(scr.T(380)); Write(" "); Write(Buf1);
      Write(" "); Write(scr.T(381)); Write("\n");     // CalcNotFound
      return;
   }
   FIT* fit = NewFit(system,oldfit);
   if ( system.ReplaceFit(oldfit,fit) == TRUE )
   {
      Write("\n   "); Write(scr.T(382)); Write("\n"); // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(383)); Write("\n"); // Failure
      delete fit;
   }
}


static void DeleteFit(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(384));                 // EnterSymbol
   Read(Buf1,BufLen,"");
   if ( system.DeleteFit(Buf1) == TRUE )
   {
      Write("\n   "); Write(scr.T(385)); Write("\n"); // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(386)); Write("\n"); // Failure
   }
}


static void ActivateFit(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(334));                    // EnterSymbol
   Read(Buf1,BufLen,"");
   FIT* fit = system.GetFit(Buf1);
   if ( fit == 0 )
   {
      Write("\n   "); Write(scr.T(336)); Write("\n");    // Failure
   }
   else
   {
      if ( system.FitActive(fit,TRUE) == TRUE )
      {
         Write("\n   "); Write(scr.T(335)); Write("\n"); // Success
      }
      else
      {
         Write("\n   "); Write(scr.T(336)); Write("\n"); // Failure
      }
   }
}


static void InactivateFit(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(337));                    // EnterSymbol
   Read(Buf1,BufLen,"");
   FIT* fit = system.GetFit(Buf1);
   if ( fit == 0 )
   {
      Write("\n   "); Write(scr.T(339)); Write("\n");    // Failure
   }
   else
   {
      if ( system.FitActive(fit,FALSE) == TRUE )
      {
         Write("\n   "); Write(scr.T(338)); Write("\n"); // Success
      }
      else
      {
         Write("\n   "); Write(scr.T(339)); Write("\n"); // Failure
      }
   }
}


static void DefineFit(AQUASYS& system)
{
   while (1)
   {
      Write("\n   "); Write(scr.T(340)); Write(":\n");    // AvailablePars
      CARDINAL i = 0;
      AQVAR* var = system.Varlist()->First();
      while ( var != 0 )
      {
         if ( var->Type() == ConstVar )
         {
            if ( i % scr.I(1) == 0 ) Write("\n   ");
            i++;
            Write(var->Symbol());
            Write(' ',scr.L(1)-strlen(var->Symbol()));
         }
         var = var->Next();
      }
      Write("\n");
      Write("\n   "); Write(scr.T(341)); Write(":\n");    // ActivePars
      i = 0;
      var = system.Varlist()->First();
      while ( var != 0 )
      {
         if ( var->Type() == ConstVar )
         {
            CONSTVAR* cv = (CONSTVAR*)var;
            if ( cv->FitActive() == TRUE )
            {
               if ( i % scr.I(1) == 0 ) Write("\n   ");
               i++;
               Write(var->Symbol());
               Write(' ',scr.L(1)-strlen(var->Symbol()));
            }
         }
         var = var->Next();
      }
      Write("\n");
      Write("\n   "); Write(scr.T(350));
      Write("\n   1   =   "); Write(scr.T(342));       // Activate
      Write("\n   2   =   "); Write(scr.T(343));       // Inactivate
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      switch (Buf1[0])
      {
         case '1':
            ActivateFitPar(system);
            break;
         case '2':
            InactivateFitPar(system);
            break;
         default:
            break;
      }
   }
   while (1)
   {
      Write("\n   "); Write(scr.T(373)); Write(":\n");    // AvailableFits
      CARDINAL i = 0;
      FIT* fit = system.Fitlist()->First();
      while ( fit != 0 )
      {
         if ( i % scr.I(1) == 0 ) Write("\n   ");
         i++;
         Write(fit->Symbol());
         Write(' ',scr.L(1)-strlen(fit->Symbol()));
         fit = fit->Next();
      }
      Write("\n");
      Write("\n   "); Write(scr.T(372)); Write(":\n");    // ActiveFits
      i = 0;
      fit = system.Fitlist()->First();
      while ( fit != 0 )
      {
         if ( fit->FitActive() == TRUE )
         {
            if ( i % scr.I(1) == 0 ) Write("\n   ");
            i++;
            Write(fit->Symbol());
            Write(' ',scr.L(1)-strlen(fit->Symbol()));
         }
         fit = fit->Next();
      }
      Write("\n");
      Write("\n   "); Write(scr.T(351));
      Write("\n   1   =   "); Write(scr.T(374));       // AddFit
      Write("\n   2   =   "); Write(scr.T(375));       // EditFit
      Write("\n   3   =   "); Write(scr.T(376));       // DeleteFit
      Write("\n   4   =   "); Write(scr.T(332));       // Activate
      Write("\n   5   =   "); Write(scr.T(333));       // Inactivate
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      switch (Buf1[0])
      {
         case '1':
            AddFit(system);
            break;
         case '2':
            EditFit(system);
            break;
         case '3':
            DeleteFit(system);
            break;
         case '4':
            ActivateFit(system);
            break;
         case '5':
            InactivateFit(system);
            break;
         default:
            break;
      }
   }

   switch ( system.FitMeth() )
   {
      case Secant:
         Buf2[0] = '1';
         break;
      case Simplex:
         Buf2[0] = '2';
         break;
      default:
         FatalError("Fit: illegal fit method");
   }
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(660));                    // Select
   Write("\n   1   =   "); Write(scr.T(661));            // Secant
   Write("\n   2   =   "); Write(scr.T(662));            // Simplex
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   switch ( Buf1[0] )
   {
      case '1':
         system.FitMeth(Secant);
         break;
      case '2':
         system.FitMeth(Simplex);
         break;
   }
   Write("\n   "); Write(scr.T(424));                      // EnterMaxSteps
   CARDINAL maxfitsteps = system.FitIter();
   Read(maxfitsteps,maxfitsteps);
   system.FitIter(maxfitsteps);

   return;
}

static void StartFit(AQUASYS& system)
{
   CARDINAL num = 0;
   AQVAR* var = (system.Varlist())->First();
   while ( var != 0 )
   {
      if ( var->Type() == ConstVar )
      {
         CONSTVAR* cv = (CONSTVAR*)var;
         if ( cv->FitActive() == TRUE )
         {
            num++;
         }
      }
      var = var->Next();
   }
   if ( num == 0 )
   {
      Write("\n   "); Write(scr.T(801)); Write("\n");      // NoActPar
      return;
   }
   num = 0;
   FIT* fit = (system.Fitlist())->First();
   while ( fit != 0 )
   {
      if ( fit->FitActive() == TRUE ) num++;
      fit = fit->Next();
   }
   if ( num == 0 )
   {
      Write("\n   "); Write(scr.T(802)); Write("\n");      // NoActCalc
      return;
   }

   if ( strlen(AQ_FitFile)==0 && strlen(AQ_SaveFile)>0 )
   {
      strcpy(AQ_FitFile,AQ_SaveFile);
      char* p = strchr(AQ_FitFile,'.');
      if ( p != 0 ) *p = '\0';
      strcat(AQ_FitFile,FEfit);
   }
   Write("\n   "); Write(scr.T(423));                      // EnterFileName
   Read(Buf,BufLen,AQ_FitFile);
   strcpy(AQ_FitFile,Buf);
   fitfile.open(Buf,std::ios::out|std::ios::trunc);
   SetFileType(Buf,FTfit,FCfit);

   CARDINAL maxfitsteps = system.FitIter();
   CARDINAL fitsteps = 0;
   system.SetFitOut(FitOut); FitCount = 0;
   Write("\n   "); Write(scr.T(295)); Write("\n");         // Calculating
   WriteBuffer();
   JOBSTATUS status = system.Fit(fitfile,&fitsteps);
   Write("\n");
   if ( status == OK )
   {
      Write("\n   "); Write(scr.T(425)); Write("\n");      // Success
   }
   else
   {
      if ( fitsteps == maxfitsteps )
      {
         Write("\n   "); Write(fitsteps); Write(" ");      // MaxSteps
         Write(scr.T(426)); Write("\n");
      }
      else
      {
         Write("\n   "); Write(scr.T(427)); Write("\n");   // Problems
      }
   }
   system.SetFitOut(0);
   fitfile.close();
   return;
}

static void ParameterEstimation(AQUASYS& system)
{
   while (1)
   {
      Write("\n   "); Write(scr.T(830)); Write(":");  // Fit Task
      Write("\n   1   =   "); Write(scr.T(831));   // Def
      Write("\n   2   =   "); Write(scr.T(832));   // Start
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf,BufLen);
      if ( (Buf[0]==C1) || (Buf[0]==C2) || (Buf[0]=='\0') ) return;
      switch (Buf[0])
      {
         case '1':
            DefineFit(system);
            break;
         case '2':
            StartFit(system);
            return;
         default:
            break;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////

void Calc(AQUASYS& system)
{
   while (1)
   {
      Write("\n"); Write(scr.T(31)); Write("\n");  // Calc Menu
      Write('-',strlen(scr.T(31)));
      Write("\n");
      Write("\n   1   =   "); Write(scr.T(265));   // Sim
      Write("\n   2   =   "); Write(scr.T(451));   // Sens
      Write("\n   3   =   "); Write(scr.T(371));   // Fit
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf,BufLen);
      if ( (Buf[0]==C1) || (Buf[0]==C2) || (Buf[0]=='\0') ) return;
      switch (Buf[0])
      {
         case '1':
            Simulation(system);
            break;
         case '2':
            SensitivityAnalysis(system);
            break;
         case '3':
            ParameterEstimation(system);
            break;
         default:
            break;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////
