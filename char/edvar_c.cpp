///////////////////////////////   edvar_c.C   ////////////////////////////////
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
#include "edopt_c.h"
#include "edvar_c.h"

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

char DatFileName[256] = "";

CARDINAL colx = 1, coly = 2, colsy = 3;
CARDLONG firstline = 4, lastline = 0;

//////////////////////////////////////////////////////////////////////////////

static void ReadUnit(AQVAR* var)
{
   Write("\n   "); Write(scr.T(47));                  // EnterUnit
   Read(Buf1,BufLen,var->Unit()); var->Unit(Buf1);
}


static AQVAR* NewStateVar(AQVAR* oldvar)
{
   STATEVAR* statevar = new STATEVAR(oldvar);
   ReadSymbol(statevar);
   ReadUnit(statevar);
   switch ( statevar->StateVarType() )
   {
      case VolStateVar:  Buf2[0] = '1'; break;
      case SurfStateVar: Buf2[0] = '2'; break;
      case EquStateVar:  Buf2[0] = '3'; break;
      default:    FatalError("aquaedit::NewStateVar: unknown StateVarType");
   }
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(58));                // Select
   Write("\n   1   =   "); Write(scr.T(59));        // VolStateVar
   Write("\n   2   =   "); Write(scr.T(305));       // SurfStateVar
   Write("\n   3   =   "); Write(scr.T(60));        // EquStateVar
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0]=='1' )
      statevar->StateVarType(VolStateVar);
   if ( Buf1[0]=='2' )
      statevar->StateVarType(SurfStateVar);
   if ( Buf1[0]=='3' )
      statevar->StateVarType(EquStateVar);
   Write("\n   "); Write(scr.T(61));                // EnterRelAcc
   REAL x;
   Read(x,statevar->RelAccuracy()); statevar->RelAccuracy(x);
   Write("\n   "); Write(scr.T(62)); Write(" (");   // EnterAbsAcc
   Write(statevar->Unit()); Write(")");
   Read(x,statevar->AbsAccuracy()); statevar->AbsAccuracy(x);
   return statevar;
}


static AQVAR* NewProgVar(AQVAR* oldvar)
{
   PROGVAR* progvar = new PROGVAR(oldvar);
   ReadSymbol(progvar);
   ReadUnit(progvar);
   Write("\n   "); Write(scr.T(63));           // SelectType
   CARDINAL n;
   for ( n=0; n<PROGVAR::NumProgVarTypes(); n++ )
   {
      Write("\n   "); if ( n < 9 ) Write(" ");
      Write(n+1); Write("   =   ");
      Write(PROGVAR::ProgVarTypeName((PROGVARTYPE)n));
   }
   Write("\n  ");
   n = progvar->ProgVarType()+1;
   Read(n,n); n--;
   if ( progvar->ProgVarType((PROGVARTYPE)n) == FALSE )
      FatalError("aquaedit::NewProgVar: Unable to set ProgVarType");
   return progvar;
}


static AQVAR* NewConstVar(AQVAR* oldvar)
{
   CONSTVAR* constvar = new CONSTVAR(oldvar);
   ReadSymbol(constvar);
   ReadUnit(constvar);

   Write("\n   "); Write(scr.T(66)); Write(" (");     // EnterValue
   Write(constvar->Unit()); Write(")");
   REAL x;
   Read(x,constvar->Value());
   if ( constvar->Minimum() > x ) constvar->Minimum(x);
   if ( constvar->Maximum() < x ) constvar->Maximum(x);
   constvar->SetValue(x);

   Write("\n   "); Write(scr.T(446)); Write(" (");    // EnterStDev
   Write(constvar->Unit()); Write(")");
   Read(x,constvar->StDev());
   if ( constvar->StDev(x) == FALSE )
   {
      delete constvar; return 0;
   }

   Write("\n   "); Write(scr.T(67)); Write(" (");     // EnterMinimum
   Write(constvar->Unit()); Write(")");
   Read(x,constvar->Minimum());
   if ( constvar->Minimum(x) == FALSE )
   {
      delete constvar; return 0;
   }

   Write("\n   "); Write(scr.T(68)); Write(" (");     // EnterMaximum
   Write(constvar->Unit()); Write(")");
   Read(x,constvar->Maximum());
   if ( constvar->Maximum(x) == FALSE )
   {
      delete constvar; return 0;
   }

   if ( constvar->SensActive() == TRUE ) Buf2[0] = '1';
   else                                  Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(447));                 // SensStatus
   Write("\n   1   =   "); Write(scr.T(405));         // Active
   Write("\n   2   =   "); Write(scr.T(412));         // Inactive
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) constvar->SensActive(TRUE);
   else                  constvar->SensActive(FALSE);

   if ( constvar->FitActive() == TRUE ) Buf2[0] = '1';
   else                                 Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(404));                 // FitStatus
   Write("\n   1   =   "); Write(scr.T(405));         // Active
   Write("\n   2   =   "); Write(scr.T(412));         // Inactive
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) constvar->FitActive(TRUE);
   else                  constvar->FitActive(FALSE);
   return constvar;
}


static AQVAR* NewRealListVar(AQUASYS& system, AQVAR* oldvar)
{
   REALLISTVAR* rlistvar = new REALLISTVAR(oldvar);
   ReadSymbol(rlistvar);
   ReadUnit(rlistvar);
   if ( rlistvar->GetArg() != 0 )
   {
      strcpy(Buf2,(rlistvar->GetArg())->Symbol());
   }
   else
   {
      Buf2[0] = '\0';
   }
   Write("\n   "); Write(scr.T(69));    // EnterArgument
   Read(Buf1,BufLen,Buf2);
   AQVAR* v = system.GetVar(Buf1);
   if ( v == 0 )
   {
      Write("\n   "); Write(scr.T(70)); Write(" "); Write(Buf1);
      Write("\n");                      // UnknownVar
      delete rlistvar; return 0;
   }
   if ( rlistvar->SetArg(v) == FALSE )
      FatalError("aquaedit::NewRealListVar: Argument cannot be set");

   if ( rlistvar->StDevGlobal() == TRUE ) Buf2[0] = '1';
   else                                   Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(481));             // StDevType
   Write("\n   1   =   "); Write(scr.T(482));     // Global
   Write("\n   2   =   "); Write(scr.T(483));     // Individual
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' )
   {
      if ( rlistvar->StDevGlobal() == FALSE )
      {
         rlistvar->StDevGlobal(TRUE);
         Write("\n   "); Write(scr.T(486)); WriteYesNo();
         Read(Buf1,BufLen,Y1);               // DeleteExistPairs
         if ( (Buf1[0]==Y1) || (Buf1[0]==Y2) )
            rlistvar->DeleteIndividualStDev();
      }
      else
      {
         rlistvar->StDevGlobal(TRUE);
      }
   }
   else
   {
      rlistvar->StDevGlobal(FALSE);
   }

   if ( rlistvar->StDevGlobal() == TRUE )
   {
      REAL r;
      Write("\n   "); Write(scr.T(484));          // RelStDev
      Read(r,rlistvar->RelStDev());
      if ( rlistvar->RelStDev(r) == FALSE )
      {
         delete rlistvar; return 0;
      }
      Write("\n   "); Write(scr.T(485));          // AbsStDev
      Write(" ("); Write(rlistvar->Unit()); Write(")");
      Read(r,rlistvar->AbsStDev());
      if ( rlistvar->AbsStDev(r) == FALSE )
      {
         delete rlistvar; return 0;
      }
   }

   REAL r;
   Write("\n   "); Write(scr.T(352));             // Minimum
   Read(r,rlistvar->Minimum());
   if ( rlistvar->Minimum(r) == FALSE )
   {
      delete rlistvar; return 0;
   }
   Write("\n   "); Write(scr.T(353));             // Maximum
   Read(r,rlistvar->Maximum());
   if ( rlistvar->Maximum(r) == FALSE )
   {
      delete rlistvar; return 0;
   }

   BOOLEAN b = TRUE;
   CARDINAL current = 0;
   do
   {
      if ( rlistvar->NumPairs() > 0 )
      {
         if ( current >= rlistvar->NumPairs() )
            current = rlistvar->NumPairs()-1;
         Write("\n   "); Write(scr.T(71)); Write(" ");// CurrentPair
         Write(current+1); Write(" "); Write(scr.T(72));
         Write(" "); Write(rlistvar->NumPairs()); Write(":\n   ");
         Write((rlistvar->GetArg())->Symbol()); Write(" = ");
         Write(rlistvar->X()[current]); Write(" "); 
         Write((rlistvar->GetArg())->Unit());
         Write(", "); Write(rlistvar->Symbol()); Write(" = ");
         if ( rlistvar->StDevGlobal() == FALSE )
         {
            Write("( ");
         }
         Write(rlistvar->Y()[current]); Write(" "); 
         if ( rlistvar->StDevGlobal() == FALSE )
         {
            Write("+/- "); Write(rlistvar->GetStDev(current));
            Write(" ) ");
         }
         Write(rlistvar->Unit()); Write("\n");
      }
      else
      {
         Write("\n   "); Write(scr.T(73)); Write("\n");
      }                                          // ListIsEmpty
      Write("\n   "); Write(scr.T(74));
      Write("\n   1   =   "); Write(scr.T(75));  // AddDataPair
      Write("\n   2   =   "); Write(scr.T(76));  // ReadPairs
      Write("\n   3   =   "); Write(scr.T(414)); // WritePairs
      Write("\n   4   =   "); Write(scr.T(77));  // DeletePair
      Write("\n");
      Write("\n   5   =   "); Write(scr.T(78));  // GotoFirst
      Write("\n   6   =   "); Write(scr.T(79));  // GotoLast
      Write("\n   7   =   "); Write(scr.T(80));  // GotoPrev
      Write("\n   8   =   "); Write(scr.T(81));  // GotoNext
      Write("\n   9   =   "); Write(scr.T(82));  // GotoPos
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') )
         b = FALSE;
      switch (Buf1[0])
      {
         case '1':
         {
            REAL x = -1.0; REAL y = 0.0; REAL sy = 0;
            if ( rlistvar->NumPairs() > 0 )
            {
               x  = rlistvar->X()[rlistvar->NumPairs()-1];
               y  = rlistvar->Y()[rlistvar->NumPairs()-1];
            }
            x++;
            Write("\n   "); Write(scr.T(83)); Write(" (");
            Write((rlistvar->GetArg())->Unit()); Write(")");
            Read(x,x);                           // EnterIndepVal
            Write("\n   "); Write(scr.T(84)); Write(" (");
            Write(rlistvar->Unit()); Write(")"); // EnterDepVal
            Read(y,y);
            if ( rlistvar->StDevGlobal() == FALSE )
            {
               Write("\n   "); Write(scr.T(487)); Write(" (");
               Write(rlistvar->Unit()); Write(")");
               Read(sy,rlistvar->GetStDev(0));      // EnterStDev
            }
            else
            {
               sy = 0.0;
            }
            if ( rlistvar->Add(x,y,sy) == TRUE )
            {
               Write("\n   "); Write(scr.T(85)); Write("\n");
            }                                    // Success
            else
            {
               Write("\n   "); Write(scr.T(86)); Write("\n");
            }                                    // Failure
            break;
         }
         case '2':
         {
            Write("\n   "); Write(scr.T(87));   // EnterFilename
            Read(Buf1,BufLen,DatFileName);
            infile.open(Buf1,std::ios::in);
            if ( infile.fail() )
            {
               Write("\n   "); Write(scr.T(88)); Write(" ");
               Write(Buf1); Write("\n");        // CannotOpenFile
               break;
            }
            strcpy(DatFileName,Buf1);
            CARDINAL COLSY;
            Write("\n   "); Write(scr.T(89));   // EnterIndepCol
            Read(colx,colx);
            Write("\n   "); Write(scr.T(90));   // EnterDepcol
            Read(coly,coly);
            if ( rlistvar->StDevGlobal() == FALSE )
            {
               Write("\n   "); Write(scr.T(488));// EnterStDevcol
               Read(colsy,colsy);
               COLSY = colsy;
            }
            else
            {
               COLSY = 0;
            }
            Write("\n   "); Write(scr.T(91));   // EnterFirstRow
            Read(firstline,firstline);
            Write("\n   "); Write(scr.T(92));   // EnterLastRow
            Read(lastline,lastline);
            Write("\n   "); Write(scr.T(93)); WriteYesNo();
            Read(Buf1,BufLen,Y1);               // DeleteExistPairs
            if ( (Buf1[0]==Y1) || (Buf1[0]==Y2) )
               rlistvar->Delete();
            CARDLONG numread;
            if ( rlistvar->ReadData(infile,&numread,colx,coly,COLSY,
                                    firstline,lastline) == TRUE )
            {
               Write("\n   "); Write(numread);
               Write(" ");
               Write(scr.T(94)); Write("\n");   // Success
            }
            else
            {
               if ( numread == 0 )
               {
                  Write("\n   "); Write(scr.T(264));
                  Write("\n");                     // Failure
               }
               else
               {
                  Write("\n   "); Write(scr.T(95)); Write(" ");
                  Write(numread); Write(" ");
                  Write(scr.T(96)); Write("\n");   // ProblemAfter
               }
            }
            infile.close();
            break;
         }
         case '3':
         {
            Write("\n   "); Write(scr.T(415));   // EnterFilename
            Read(Buf1,BufLen,"");
            outfile.open(Buf1,std::ios::out|std::ios::trunc);
            if ( outfile.fail() )
            {
               Write("\n   "); Write(scr.T(88)); Write(" ");
               Write(Buf1); Write("\n");        // CannotOpenFile
               break;
            }
            SetFileType(Buf,FTdatW,FCdat);
            if ( rlistvar->WriteData(outfile) == TRUE )
            {                                   // Success
               Write("\n   "); Write(scr.T(416)); Write("\n");
            }
            else
            {                                   // Failure
               Write("\n   "); Write(scr.T(417)); Write("\n");
            }
            outfile.close();
            break;
         }
         case '4':
            rlistvar->Delete(current);
            break;
         case '5':
            current = 0;
            break;
         case '6':
            if ( rlistvar->NumPairs() > 0 )
               current = rlistvar->NumPairs()-1;
            break;
         case '7':
            if ( current > 0 ) current--;
            break;
         case '8':
            current++;
             break;
         case '9':
         {
            Write("\n   "); Write(scr.T(97));  // EnterPos
            Read(current,current+1);
            if ( current > 0 ) current--;
            break;
         }
         default:
            break;
      }
   } while ( b == TRUE );
   if ( rlistvar->NumPairs() == 0 )
   {
      delete rlistvar; return 0;
   }
   CARDINAL n = rlistvar->Method()+1;
   Write("\n   "); Write(scr.T(413));             // SelectMethod
   Write("\n   1   =   "); Write(scr.T(445));     // Linear
   Write("\n   2   =   "); Write(scr.T(448));     // Spline
   Write("\n   3   =   "); Write(scr.T(449));     // Smooth
   Write("\n  ");
   Read(n,n);
   switch ( n )
   {
      case 1:
         rlistvar->Method(linear);
         break;
      case 2:
         rlistvar->Method(spline);
         break;
      case 3:
         rlistvar->Method(smooth);
          break;
      default:
         break;
   }

   if ( rlistvar->Method() == smooth )
   {
      Write("\n   "); Write(scr.T(480));     // SmoothWidth
      REAL width;
      Read(width,rlistvar->SmoothWidth());
      if ( rlistvar->SmoothWidth(width) == FALSE )
      {
         delete rlistvar; return 0;
      }
   }

   if ( rlistvar->SensActive() == TRUE ) Buf2[0] = '1';
   else                                  Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(447));              // SensStatus
   Write("\n   1   =   "); Write(scr.T(405));      // Active
   Write("\n   2   =   "); Write(scr.T(412));      // Inactive
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) rlistvar->SensActive(TRUE);
   else                  rlistvar->SensActive(FALSE);
   return rlistvar;
}


static AQVAR* NewVarListVar(AQUASYS& system, AQVAR* oldvar)
{
   VARLISTVAR* vlistvar = new VARLISTVAR(oldvar);
   ReadSymbol(vlistvar);
   ReadUnit(vlistvar);
   if ( vlistvar->GetArg() != 0 )
   {
      strcpy(Buf2,(vlistvar->GetArg())->Symbol());
   }
   else
   {
      Buf2[0] = '\0';
   }
   Write("\n   "); Write(scr.T(98));  // EnterArgument
   Read(Buf1,BufLen,Buf2);
   AQVAR* v = system.GetVar(Buf1);
   if ( v == 0 )
   {
      Write("\n   "); Write(scr.T(99)); Write(" "); Write(Buf1);
      Write("\n");                    // UnknownVar
      delete vlistvar; return 0;
   }
   if ( vlistvar->SetArg(v) == FALSE )
      FatalError("aquaedit::NewVarListVar: Argument cannot be set");

   BOOLEAN b = TRUE;
   CARDINAL current = 0;
   do
   {
      if ( vlistvar->NumPairs() > 0 )
      {
         if ( current >= vlistvar->NumPairs() )
            current = vlistvar->NumPairs()-1;
         Write("\n   "); Write(scr.T(100)); Write(" "); // CurrPair
         Write(current+1); Write(" "); Write(scr.T(101));
         Write(" "); Write(vlistvar->NumPairs()); Write(":\n   ");
         Write((vlistvar->GetArg())->Symbol()); Write(" = ");
         Write(vlistvar->X()[current]); Write(" ");
         Write((vlistvar->GetArg())->Unit());
         Write(", "); Write(vlistvar->Symbol()); Write(" = ");
         Write((vlistvar->Yvar()[current])->Symbol()); Write("\n");
      }
      else
      {
         Write("\n   "); Write(scr.T(102)); Write("\n");
      }                                           // ListIsEmpty
      Write("\n   "); Write(scr.T(103));          // Select
      Write("\n   1   =   "); Write(scr.T(104));  // AddPair
      Write("\n   2   =   "); Write(scr.T(105));  // DeletePair
      Write("\n");
      Write("\n   3   =   "); Write(scr.T(106));  // GotoFirst
      Write("\n   4   =   "); Write(scr.T(107));  // GotoLast
      Write("\n   5   =   "); Write(scr.T(108));  // GotoPrev
      Write("\n   6   =   "); Write(scr.T(109));  // GotoNext
      Write("\n   7   =   "); Write(scr.T(110));  // GotoPos
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') )
         b = FALSE;
      switch (Buf1[0])
      {
         case '1':
         {
            REAL x = -1.0; AQVAR* yvar;
            if ( vlistvar->NumPairs() > 0 )
            {
               x = vlistvar->X()[vlistvar->NumPairs()-1];
            }
            x++;
            Write("\n   "); Write(scr.T(111)); Write(" (");
            Write((vlistvar->GetArg())->Unit()); Write(")");
            Read(x,x);                          // EnterIndepVal
            Write("\n   "); Write(scr.T(112));  // EnterDepVar
            Read(Buf1,BufLen,"");
            yvar = system.GetVar(Buf1);
            if ( vlistvar->Add(x,yvar) == TRUE )
            {
               Write("\n   "); Write(scr.T(113)); Write("\n");
            }                                   // Success
            else
            {
               Write("\n   "); Write(scr.T(114)); Write("\n");
            }                                   // Failure
            break;
         }
         case '2':
            vlistvar->Delete(current);
            break;
         case '3':
            current = 0;
            break;
         case '4':
            if ( vlistvar->NumPairs() > 0 )
               current = vlistvar->NumPairs()-1;
            break;
         case '5':
            if ( current > 0 ) current--;
            break;
         case '6':
            current++;
            break;
         case '7':
         {
            Write("\n   "); Write(scr.T(115));  // EnterPos
            Read(current,current+1);
            if ( current > 0 ) current--;
            break;
         }
         default:
            break;
      }
   } while ( b == TRUE );
   if ( vlistvar->NumPairs() == 0 )
   {
      delete vlistvar; return 0;
   }

   CARDINAL n = vlistvar->Method()+1;
   Write("\n   "); Write(scr.T(413));             // SelectMethod
   Write("\n   1   =   "); Write(scr.T(445));     // Linear
   Write("\n   2   =   "); Write(scr.T(448));     // Spline
   Write("\n   3   =   "); Write(scr.T(449));     // Smooth
   Write("\n  ");
   Read(n,n);
   switch ( n )
   {
      case 1:
         vlistvar->Method(linear);
         break;
      case 2:
         vlistvar->Method(spline);
         break;
      case 3:
         vlistvar->Method(smooth);
         break;
      default:
         break;
   }

   if ( vlistvar->Method() == smooth )
   {
      Write("\n   "); Write(scr.T(480));     // SmoothWidth
      REAL width;
      Read(width,vlistvar->SmoothWidth());
      if ( vlistvar->SmoothWidth(width) == FALSE )
      {
         delete vlistvar; return 0;
      }
   }
   return vlistvar;
}


static AQVAR* NewFormVar(AQUASYS& system, AQVAR* oldvar)
{
   FORMVAR* formvar = new FORMVAR(oldvar);
   ReadSymbol(formvar);
   ReadUnit(formvar);
   CARDINAL n = 0;
   while (1)
   {
      n++;
      if ( formvar->UnParse(Buf2,BufLen-1) != 0 ) Buf2[0] = '\0';
      Write("\n   "); Write(scr.T(57)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);                  // EnterExpression
      if ( formvar->Parse(Buf1,system.Varlist(),Buf2) == 0 )
      {
         return formvar;
      }
      else
      {
         formvar->GetErrorMarks(Buf2,'^',Buf1);
         Write("\n    "); Write(Buf1);
         CARDINAL len = strlen(Buf1);
         for ( CARDINAL i=0; i<len; i++ )
         {
            if ( Buf2[i] != 0 )
            {
               formvar->GetErrorMsg(Buf2[i],Buf1);
               Write("\n    "); Write(' ',i); Write(Buf1);
            }
         }
         Write("\n");
         if ( n>2 )
         {
           delete formvar; return 0;
         }
      }
   }
}


static AQVAR* NewProbeVar(AQUASYS& system, AQVAR* oldvar)
{
   CARDINAL i;

   PROBEVAR* probevar = new PROBEVAR(oldvar);
   ReadSymbol(probevar);
   ReadUnit(probevar);

   Write("\n   "); Write(scr.T(1339));    // EnterVar
   if ( probevar->Variable() != 0 )
   {
      Read(Buf1,BufLen,(probevar->Variable())->Symbol());
   }
   else
   {
      Read(Buf1,BufLen,"");
   }
   probevar->Variable(system.GetVar(Buf1));

   if ( (system.Complist())->Size() <= 1 )
   {
      probevar->Compartment((system.Complist())->First());
   }
   else
   {
      Write("\n   "); Write(scr.T(1340)); // EnterComp
      if ( probevar->Compartment() != 0 )
      {
         Read(Buf1,BufLen,(probevar->Compartment())->Symbol());
      }
      else
      {
         Read(Buf1,BufLen,"");
      }
      probevar->Compartment(system.GetComp(Buf1));
   }
   if ( probevar->Compartment() == 0 )
   {
      Write("\n   "); Write(scr.T(191)); Write(" ");
      Write(scr.T(192)); Write ("\n");   // CompNotFound
      delete probevar; return 0;
   }
   if ( probevar->Zone() >= (probevar->Compartment())->NumZones() )
      probevar->Zone(0);
   if ( (probevar->Compartment())->NumZones() > 1 )
   {
      AQCOMP* com = probevar->Compartment();
      Write("\n   "); Write(scr.T(1341));        // SelectZone
      for ( i=0; i<com->NumZones(); i++ )
      {
         NumToString(i+1,Buf1,BufLen,4);
         Write("\n"); Write(Buf1); Write("   =   "); 
         Write(com->ZoneName(i));                // ZoneName
      }
      Write("\n  "); Read(i,probevar->Zone()+1); i--;
      if ( i >= com->NumZones() ) i = com->NumZones()-1;
      probevar->Zone(i);
   }

   REAL x;
   Write("\n   "); Write(scr.T(1342));           // EnterSpace
   Read(x,probevar->Space());
   probevar->Space(x);

   if ( probevar->SpaceRel() == FALSE ) Buf2[0] = '1';
   else                                 Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(1398));                // SelSpace
   Write("\n   1   =   "); Write(scr.T(1399));        // Abs
   Write("\n   2   =   "); Write(scr.T(1400));        // Rel
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) probevar->SpaceRel(FALSE);
   else                  probevar->SpaceRel(TRUE);

   if ( probevar->AllowedVar() == FALSE )
   {
      delete probevar; return 0;
   }
   return probevar;
}


static AQVAR* NewVar(AQUASYS& system, AQVAR* oldvar, BOOLEAN seltype=FALSE)
{
   while (1)
   {
      if ( (oldvar==0) || (seltype==TRUE) )
      {
         Write("\n   "); Write(scr.T(48));            // SelectVarType
         Write("\n   1   =   "); Write(scr.T(50));    // StateVar
         Write("\n   2   =   "); Write(scr.T(51));    // ProgVar
         Write("\n   3   =   "); Write(scr.T(52));    // ConstVar
         Write("\n   4   =   "); Write(scr.T(53));    // RealListVar
         Write("\n   5   =   "); Write(scr.T(54));    // VarListVar
         Write("\n   6   =   "); Write(scr.T(49));    // Formula
         Write("\n   7   =   "); Write(scr.T(1343));  // ProbeVar
         Write("\n");
         Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
         Read(Buf1,BufLen);
      }
      else
      {
         Write("\n   "); Write(scr.T(55)); Write(" "); 
         Write(oldvar->Symbol());
         Write(" "); Write(scr.T(56)); Write(" ");    // VarIsOfType:
         switch ( oldvar->Type() )
         {
            case StateVar:
               Buf1[0] = '1'; Write(scr.T(50));       // StateVar
               break;
            case ProgVar:
               Buf1[0] = '2'; Write(scr.T(51));       // ProgVar
               break;
            case ConstVar:
               Buf1[0] = '3'; Write(scr.T(52));       // ConstVar
               break;
            case RealListVar:
               Buf1[0] = '4'; Write(scr.T(53));       // RealListVar
               break;
            case VarListVar:
               Buf1[0] = '5'; Write(scr.T(54));       // VarListVar
               break;
            case FormVar:
               Buf1[0] = '6'; Write(scr.T(49));       // Formula
               break;
            case ProbeVar:
               Buf1[0] = '7'; Write(scr.T(1343));     // ProbeVar
               break;
            default:
               FatalError("aquaedit::NewVar: Unknown var type");
               break;
         }
         Write("\n");
      }
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) return 0;
      switch (Buf1[0])
      {
         case '1':
            return NewStateVar(oldvar);
         case '2':
            return NewProgVar(oldvar);
         case '3':
            return NewConstVar(oldvar);
         case '4':
            return NewRealListVar(system,oldvar);
         case '5':
            return NewVarListVar(system,oldvar);
         case '6':
            return NewFormVar(system,oldvar);
         case '7':
            return NewProbeVar(system,oldvar);
         default:
            break;
      }
   }
}


static void AddVar(AQUASYS& system)
{
   AQVAR* var = NewVar(system,0);
   if ( system.AddVar(var) == TRUE )
   {
      Write("\n   "); Write(scr.T(116)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(117)); Write("\n");  // Failure
      delete var;
   }
   return;
}


static void CopyVar(AQUASYS& system)
{
   Buf1[0] = '\0';
   AQVAR* oldvar = system.Varlist()->First();
   if ( system.Varlist()->Size() > 1 )
   {
      Write("\n   "); Write(scr.T(429));              // EnterSymbol
      Read(Buf1,BufLen,"");
      oldvar = system.GetVar(Buf1);
   }
   if ( oldvar == 0 )
   {
      Write("\n   "); Write(scr.T(118)); Write(" "); Write(Buf1);
      Write(" "); Write(scr.T(126)); Write("\n");     // VarNotFound
      return;
   }
   AQVAR* var = NewVar(system,oldvar);
   if ( system.AddVar(var) == TRUE )
   {
      Write("\n   "); Write(scr.T(430)); Write("\n"); // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(431)); Write("\n"); // Failure
      delete var;
   }
   return;
}


static void EditVar(AQUASYS& system)
{
   Buf1[0] = '\0';
   AQVAR* oldvar = system.Varlist()->First();
   if ( system.Varlist()->Size() > 1 )
   {
      Write("\n   "); Write(scr.T(121));              // EnterSymbol
      Read(Buf1,BufLen,"");
       oldvar = system.GetVar(Buf1);
   }
   if ( oldvar == 0 )
   {
      Write("\n   "); Write(scr.T(118)); Write(" "); Write(Buf1);
      Write(" "); Write(scr.T(126)); Write("\n");     // VarNotFound
      return;
   }
   AQVAR* var = NewVar(system,oldvar);
   if ( system.ReplaceVar(oldvar,var) == TRUE )
   {
      Write("\n   "); Write(scr.T(122)); Write("\n"); // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(123)); Write("\n"); // Failure
      delete var;
   }
   return;
}


static void ReplaceVar(AQUASYS& system)
{
   Buf1[0] = '\0';
   AQVAR* oldvar = system.Varlist()->First();
   if ( system.Varlist()->Size() > 1 )
   {
      Write("\n   "); Write(scr.T(160));              // EnterSymbol
      Read(Buf1,BufLen,"");
       oldvar = system.GetVar(Buf1);
   }
   if ( oldvar == 0 )
   {
      Write("\n   "); Write(scr.T(118)); Write(" "); Write(Buf1);
      Write(" "); Write(scr.T(126)); Write("\n");     // VarNotFound
      return;
   }
   AQVAR* var = NewVar(system,oldvar,TRUE);
   if ( system.ReplaceVar(oldvar,var) == TRUE )
   {
      Write("\n   "); Write(scr.T(119)); Write("\n"); // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(120)); Write("\n"); // Failure
      delete var;
   }
   return;
}


static void ExchangeVar(AQUASYS& system)
{
   if ( system.Varlist()->Size() < 2 )
   {
      Write("\n   "); Write(scr.T(118));
      Write(" "); Write(scr.T(126)); Write("\n");     // VarNotFound
      return;
   }
   Write("\n   "); Write(scr.T(124));                 // EnterSymbol1
   Read(Buf1,BufLen,"");
   Write("\n   "); Write(scr.T(125));                 // EnterSymbol2
   Read(Buf2,BufLen,"");
   if ( system.ExchangeVar(Buf1,Buf2) == TRUE )
   {
      Write("\n   "); Write(scr.T(128)); Write(" "); Write(Buf1); Write(" ");
      Write(scr.T(127)); Write(" "); Write(Buf2); Write(" ");
      Write(scr.T(129)); Write("\n");                 // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(130)); Write(" "); Write(Buf1); Write(" ");
      Write(scr.T(127)); Write(" "); Write(Buf2); Write(" ");
      Write(scr.T(131)); Write("\n");                 // Failure
   }
   return;
}


static void DeleteVar(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(148));                 // EnterSymbol
   Read(Buf1,BufLen,"");
   if ( system.DeleteVar(Buf1) == TRUE )
   {
      Write("\n   "); Write(scr.T(132)); Write("\n"); // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(133)); Write("\n"); // Failure
   }
   return;
}


void EditVariables(AQUASYS& system)
{
   while (1)
   {
      if ( system.Varlist()->Size() > 0 )
      {
         Write("\n"); Write(scr.T(37)); Write(":\n"); // ActualList
         CARDINAL i = 0;
         AQVAR* var = system.Varlist()->First();
         while ( var != 0 )
         {
            i++;
            if ( (i-1) % scr.I(1) == 0 ) Write("\n   ");
            Write(var->Symbol()); Write(' ',scr.L(1)-strlen(var->Symbol()));
            var = var->Next();
         }
         Write("\n");
      }
      Write("\n"); Write(scr.T(38)); Write("\n");     // EditVarMenu
      Write('-',strlen(scr.T(38)));
      Write("\n");
      Write("\n   1   =   "); Write(scr.T(39));       // AddVar
      Write("\n   2   =   "); Write(scr.T(428));      // CopyVar
      Write("\n   3   =   "); Write(scr.T(41));       // EditVar
      Write("\n   4   =   "); Write(scr.T(40));       // ReplaceVar
      Write("\n   5   =   "); Write(scr.T(42));       // ExchangeVar
      Write("\n   6   =   "); Write(scr.T(43));       // DeleteVar
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) return;
      switch (Buf1[0])
      {
         case '1':
            AddVar(system);
            break;
         case '2':
            CopyVar(system);
            break;
         case '3':
            EditVar(system);
            break;
         case '4':
            ReplaceVar(system);
            break;
         case '5':
            ExchangeVar(system);
            break;
         case '6':
            DeleteVar(system);
            break;
         default:
            break;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////
