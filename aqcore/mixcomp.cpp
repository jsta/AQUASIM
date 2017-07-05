///////////////////////////////   mixcomp.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    10.12.91    Peter Reichert
// revisions:   26.10.92    Peter Reichert    redesign of links
//              11.12.92    Peter Reichert    variable volume added
//
//////////////////////////////////////////////////////////////////////////////
//
// nv = number of volume state variables
// ns = number of surface state variables
// ne = number of equilibrium state variables
// Cv = volume state variable
// Cs = survade state variable
// Ce = equilibrium state variable
//
// type component                             meaning
// ---------------------------------------------------------------------------
// diff y[0]                                  V*Cv[1]
//  .    .                                     .
// diff y[nv-1]                               V*Cv[nv]
// diff y[nv]                                 Cs[1]
//  .    .                                     .
// diff y[nv+ns-1]                            Cs[ns]
// alg  y[nv+ns]                              Ce[1]
//  .    .                                     .
// alg  y[nv+ns+ne-1]                         Ce[ne]
// alg  y[nv+ns+ne]                           Qin

// variable volume:
// ---------------------------------------------------------------------------
// alg  y[nv+ns+ne+1]                         Qout
// diff y[nv+ns+ne+2]                         V
//
//////////////////////////////////////////////////////////////////////////////

#include "formvar.h"
#include "othervar.h"
#include "varsys.h"
#include "mixcomp.h"
#include "aqlink.h"

//////////////////////////////////////////////////////////////////////////////

const CARDINAL BufSize = 1024;

//////////////////////////////////////////////////////////////////////////////

void MIXCOMP::init()
{
   numinit = 0; initzone = 0; initvar = 0; initval = 0;

   inflow = 0;
   numinput = 0; inputvar = 0; inputflux = 0;

   relaccQ = 1.0e-3;
   absaccQ = 1.0e-3;
   relaccV = 1.0e-3;
   absaccV = 1.0e-3;

   fixedvol = TRUE;
   vol  = ini.R(1);
   Qout = 0;
   Cout = 0; numCout = 0;
   numadvinconn = 1;
   numadvoutconn = 1;
   numdiffconn = 1;
   if ( NumAdvInConn() == 0 )
   {
      advinnum = 0; advinindex = 0; advinlink = 0;
   }
   else
   {
      advinnum   = new CARDINAL[NumAdvInConn()];
      advinindex = new CARDINAL*[NumAdvInConn()];
      // COMPILER PROBLEM with codewarrior:
      // advinlink  = new ADVLINK**[NumAdvInConn()];
      typedef ADVLINK** ADVLINKpp;
      advinlink  = new ADVLINKpp[NumAdvInConn()];
      for ( CARDINAL i=0; i<NumAdvInConn(); i++ )
      {
         advinnum[i]   = 0;
         advinindex[i] = 0;
         advinlink[i]  = 0;
      }
   }
   if ( NumAdvOutConn() == 0 )
   {
      advoutlink = 0;
   }
   else
   {
      advoutlink = new ADVLINK*[NumAdvOutConn()];
      for ( CARDINAL i=0; i<NumAdvOutConn(); i++ ) advoutlink[i] = 0;
   }
   if ( NumDiffConn() == 0 )
   {
      diffnum = 0; diffindex = 0; difflink = 0;
   }
   else
   {
      diffnum   = new CARDINAL[NumDiffConn()];
      diffindex = new CARDINAL*[NumDiffConn()];
      // COMPILER PROBLEM with codewarrior:
      // difflink  = new DIFFLINK**[NumDiffConn()];
      typedef DIFFLINK** DIFFLINKpp;
      difflink  = new DIFFLINKpp[NumDiffConn()];
      for ( CARDINAL i=0; i<NumDiffConn(); i++ )
      {
         diffnum[i]   = 0;
         diffindex[i] = 0;
         difflink[i]  = 0;
      }
   }
   return;
}


void MIXCOMP::del()
{
   CARDINAL i;

   delete initzone; initzone = 0;
   delete initvar; initvar = 0;
   for ( i=0; i<numinit; i++ ) delete initval[i];
   delete initval; initval = 0; numinit = 0;

   delete inflow; inflow = 0;
   delete inputvar;  inputvar = 0;
   for ( i=0; i<numinput; i++ ) delete inputflux[i];
   delete inputflux; inputflux = 0; numinput = 0;

   delete Qout; Qout = 0;
   delete [] Cout; Cout = 0; numCout = 0;
   return;
}


MIXCOMP::MIXCOMP()
{
   init();
}


MIXCOMP::MIXCOMP(const MIXCOMP& com) : AQCOMP(com)
{
   init();

   if ( com.numinit > 0 )
   {
      numinit = com.numinit;
      initzone = new CARDINAL[numinit];
      initvar  = new AQVAR*[numinit];
      initval  = new FORMVAR*[numinit];
      for ( CARDINAL i=0; i<numinit; i++ )
      {
         initzone[i] = com.initzone[i];
         initvar[i]  = com.initvar[i];
         initval[i]  = new FORMVAR(com.initval[i]);
      }
   }

   inflow = new FORMVAR(com.inflow);
   if ( com.numinput > 0 )
   {
      numinput = com.numinput;
      inputvar = new AQVAR*[numinput];
      inputflux = new FORMVAR*[numinput];
      for ( CARDINAL i=0; i<numinput; i++ )
      {
         inputvar[i] = com.inputvar[i];
         inputflux[i] = new FORMVAR(com.inputflux[i]);
      }
   }

   relaccQ = com.relaccQ;
   absaccQ = com.absaccQ;
   relaccV = com.relaccV;
   absaccV = com.absaccV;

   fixedvol = com.fixedvol;
   vol  = com.vol;
   if ( com.Qout != 0 ) Qout = new FORMVAR(com.Qout);
}


MIXCOMP::MIXCOMP(const AQCOMP* com) : AQCOMP(com)
{
   init();

   if ( com != 0 )
   {
      if ( com->Type() == MixComp )
      {
         MIXCOMP* mc = (MIXCOMP*)com;

         if ( mc->numinit > 0 )
         {
            numinit = mc->numinit;
            initzone = new CARDINAL[numinit];
            initvar  = new AQVAR*[numinit];
            initval  = new FORMVAR*[numinit];
            for ( CARDINAL i=0; i<numinit; i++ )
            {
               initzone[i] = mc->initzone[i];
               initvar[i]  = mc->initvar[i];
               initval[i]  = new FORMVAR(mc->initval[i]);
            }
         }

         inflow = new FORMVAR(mc->inflow);
         if ( mc->numinput > 0 )
         {
            numinput = mc->numinput;
            inputvar = new AQVAR*[numinput];
            inputflux = new FORMVAR*[numinput];
            for ( CARDINAL i=0; i<numinput; i++ )
            {
               inputvar[i] = mc->inputvar[i];
               inputflux[i] = new FORMVAR(mc->inputflux[i]);
            }
         }

         relaccQ = mc->relaccQ;
         absaccQ = mc->absaccQ;
         relaccV = mc->relaccV;
         absaccV = mc->absaccV;

         fixedvol = mc->fixedvol;
         vol  = mc->vol;
         if ( mc->Qout != 0 ) Qout = new FORMVAR(mc->Qout);
      }
   }
}


MIXCOMP::~MIXCOMP()
{
   del();
}


const char* MIXCOMP::TypeName() const
{
   return ini.T(24);
}


BOOLEAN MIXCOMP::AllowedProgVarType(PROGVARTYPE type) const
{
   if ( type == CalcNumber )    return TRUE;
   if ( type == Time )          return TRUE;
   if ( type == CompIndex )     return TRUE;
   if ( type == ZoneIndex )     return TRUE;
   if ( type == Discharge )     return TRUE;
   if ( type == WaterFraction ) return TRUE;
   if ( type == Volume )        return TRUE;
   if ( type == BulkVolume )    return TRUE;
   return FALSE;
}


BOOLEAN MIXCOMP::AllowedComp() const
{
   return TRUE;
}


BOOLEAN MIXCOMP::AllowedReplaceVar(const AQVAR* oldvar,
                                   const AQVAR* newvar) const
{
   if ( AQCOMP::AllowedReplaceVar(oldvar,newvar) == FALSE ) return FALSE;
   return TRUE;
}         


BOOLEAN MIXCOMP::AllowedExchangeVar(const AQVAR* var1,
                                    const AQVAR* var2) const
{
   if ( AQCOMP::AllowedExchangeVar(var1,var2) == FALSE ) return FALSE;
   return TRUE;
}


void MIXCOMP::ReplVar(AQVAR* oldvar, AQVAR* newvar)
{
   CARDINAL i;

   if ( AllowedReplaceVar(oldvar,newvar) == FALSE )
      FatalError("MIXCOMP::ReplVar: Illegal replace");
   AQCOMP::ReplVar(oldvar,newvar);
   for ( i=0; i<numinit; i++ )
   {
      if ( initvar[i] == oldvar ) initvar[i] = newvar;
      initval[i]->ReplaceVarArg(oldvar,newvar);
   }
   if ( inflow != 0 ) inflow->ReplaceVarArg(oldvar,newvar);
   for ( i=0; i<numinput; i++ )
   {
      if ( inputvar[i] == oldvar ) inputvar[i] = newvar;
      inputflux[i]->ReplaceVarArg(oldvar,newvar);
   }
   if ( Qout != 0 )
   {
      Qout->ReplaceVarArg(oldvar,newvar);
   }
   CalcArg();
   return;
}


void MIXCOMP::ExchVar(AQVAR* var1, AQVAR* var2)
{
   CARDINAL i;

   if ( AllowedExchangeVar(var1,var2) == FALSE )
      FatalError("MIXCOMP::ExchVar: Illegal exchange");
   AQCOMP::ExchVar(var1,var2);
   AQVAR* v = new CONSTVAR;
   for ( i=0; i<numinit; i++ )
   {
      if ( initvar[i] == var1 )
      {
         initvar[i] = var2;
      }
      else
      {
         if ( initvar[i] == var2 ) initvar[i] = var1;
      }
      initval[i]->ReplaceVarArg(var1,v);
      initval[i]->ReplaceVarArg(var2,var1);
      initval[i]->ReplaceVarArg(v,var2);
   }
   if ( inflow != 0 )
   {
      inflow->ReplaceVarArg(var1,v);
      inflow->ReplaceVarArg(var2,var1);
      inflow->ReplaceVarArg(v,var2);
   }
   for ( i=0; i<numinput; i++ )
   {
      if ( inputvar[i] == var1 )
      {
         inputvar[i] = var2;
      }
      else
      {
         if ( inputvar[i] == var2 ) inputvar[i] = var1;
      }
      inputflux[i]->ReplaceVarArg(var1,v);
      inputflux[i]->ReplaceVarArg(var2,var1);
      inputflux[i]->ReplaceVarArg(v,var2);
   }
   if ( Qout != 0 )
   {
      Qout->ReplaceVarArg(var1,v);
      Qout->ReplaceVarArg(var2,var1);
      Qout->ReplaceVarArg(v,var2);
   }
   delete v;
   return;
}


BOOLEAN MIXCOMP::SetGridPts(CARDINAL n)
{
   if ( n != 1 ) return FALSE;
   return TRUE;
}


BOOLEAN MIXCOMP::NumGridPts(CARDINAL n)
{
   if ( InList() == TRUE ) return FALSE;
   return SetGridPts(n);
}


REAL MIXCOMP::GridPt(const REAL*, CARDINAL index)
{
   if ( index > 0 ) FatalError("MIXCOMP::GridPt: illegal index");
   return 0.0;
}


CARDINAL MIXCOMP::NumZoneGridPts(CARDINAL zone)
{
   if ( zone != 0 ) FatalError("MIXCOMP::NumZoneGridPts: illegal zone");
   return NumGridPts();
}


REAL MIXCOMP::ZoneGridPt(const REAL* Y, CARDINAL zone, CARDINAL index)
{
   if ( zone != 0 ) FatalError("MIXCOMP::ZoneGridPt: illegal zone");
   return GridPt(Y,index);
}


CARDINAL MIXCOMP::NumEq() const
{
   if ( fixedvol == TRUE )
   {
      return numstatevar+1;
   }
   else
   {
      return numstatevar+3;
   }
}


REAL MIXCOMP::RelAcc(CARDINAL i) const
{
   if ( i >= NumEq() ) FatalError("MIXCOMP::RelAcc: Illegal index");
   if ( i == numstatevar )   return relaccQ;  // inflow
   if ( i == numstatevar+1 ) return relaccQ;  // outflow
   if ( i == numstatevar+2 ) return relaccV;  // volume
   return statevar[i]->RelAccuracy();
}


REAL MIXCOMP::AbsAcc(CARDINAL i) const
{
   if ( i >= NumEq() ) FatalError("MIXCOMP::AbsAcc: Illegal index");
   if ( i == numstatevar )   return absaccQ;  // inflow
   if ( i == numstatevar+1 ) return absaccQ;  // outflow
   if ( i == numstatevar+2 ) return absaccV;  // volume
   return statevar[i]->AbsAccuracy();
}


EQTYPE MIXCOMP::EqType(CARDINAL i) const
{
   if ( i >= NumEq() ) FatalError("MIXCOMP::EqType: Illegal index");
   if ( i == numstatevar )   return ALG;   // inflow
   if ( i == numstatevar+1 ) return ALG;   // outflow
   if ( i == numstatevar+2 ) return DIFF;  // volume
   if ( statevar[i]->StateVarType() == EquStateVar ) return ALG;
   return DIFF;
}


BOOLEAN MIXCOMP::InitCond(VARSYS* varsys, REAL* Y, CARDINAL callnum)
{
   if ( Active() != TRUE ) return FALSE;
   if ( callnum > 0 ) return TRUE;
   REAL* y = &Y[ind_y];
   REAL eps  = 1.0; 
   REAL zoneindex = 0.0; 
   REAL compindex = (REAL)cind;
   varsys->SetProgVar(CompIndex,&compindex);
   varsys->SetProgVar(ZoneIndex,&zoneindex);
   varsys->SetProgVar(WaterFraction,&eps);

   CARDINAL j     = 0;
   CARDINAL k     = 0;
   BOOLEAN  found = FALSE;
   PROGVAR* Qvar  = varsys->GetProgVar(Discharge);
   PROGVAR* Vvar  = varsys->GetProgVar(Volume);
   REAL v = vol; if ( Vvar != 0 ) Vvar->SetProgVar(&v);
   y[numstatevar] = 0.0; if ( Qvar != 0 ) Qvar->SetProgVar(&y[numstatevar]);

   // Set volume (discharge is zero!):

   if ( fixedvol == FALSE )
   {
      y[numstatevar+1] = y[numstatevar];
      found = FALSE;
      for ( j=0; j<numinit; j++ )
      {
         if ( initvar[j] == Vvar )
         {
            k = j; found = TRUE; break;
         }
      }
      if ( found == TRUE )
      {
         y[numstatevar+2] = initval[k]->Value();
         if ( initval[k]->Arg(Qvar) )
         {
            *cjob << "\n   MIXCOMP numerical problem:"
                  << " Initial volume may not depend on discharge";
            cjob->flush();
            return FALSE;
         }
         if ( y[numstatevar+2] <= 0.0 )
         {
            *cjob << "\n   MIXCOMP numerical problem:"
                  << " Initial volume is negative";
            cjob->flush();
            return FALSE;
         }
      }
      else
      {
         y[numstatevar+2] = vol;
      }
      v = y[numstatevar+2];
   }

   // set discharge (volume defined):

   found = FALSE;
   for ( j=0; j<numinit; j++ )
   {
      if ( initvar[j] == Qvar )
      {
         k = j; found = TRUE; break;
      }
   }
   if ( found == TRUE )
   {
      y[numstatevar]   = initval[k]->Value();
      if ( y[numstatevar] < 0.0 )
      {
         *cjob << "\n   MIXCOMP numerical problem:"
               << " Initial discharge is negative";
         cjob->flush();
         return FALSE;
      }
   }

   // set state variables (volume and discharge defined):

   CARDINAL i;
   for ( i=0; i<numstatevar; i++ )
   {
      found = FALSE;
      k = 0;
      for ( CARDINAL j=0; j<numinit; j++ )
      {
         if ( initvar[j] == statevar[i] )
         {
            k = j; found = TRUE; break;
         }
      }
      if ( found == TRUE )
      { 
         y[i] = initval[k]->Value();
      }
      else
      {
         y[i] = 0.0;
      }
   }
   for ( i=0; i<numvolstatevar; i++ ) y[i] = v*y[i];

/*
cout << "\n\nInitCond: " << Symbol();
for ( i=0; i<NumEq(); i++ )
{
   cout << "\n";
   if ( i < 10 )  cout << " ";
   if ( i < 100 ) cout << " ";
   cout << i << "  ";
   switch ( EqType(i) )
   {
      case ALG:  cout << "ALG "; break;
      case DIFF: cout << "DIFF"; break;
      default:   cout << "    "; break;
   }
   cout << "  ";
   cout << y[i];
}
cout.flush();
*/

   return TRUE;
}


BOOLEAN MIXCOMP::Delta(const NUMPAR&, VARSYS* varsys,
                       const REAL* Y, const REAL* YT, REAL* DELTA)
{
   if ( Active() != TRUE ) return FALSE;
   const REAL* y  = &Y[ind_y];
   const REAL* yt = &YT[ind_y];
   REAL* delta    = &DELTA[ind_y];
   REAL v = vol;
   if ( fixedvol == FALSE ) v = y[numstatevar+2];
   REAL eps  = 1.0; 
   REAL zoneindex = 0.0; 
   REAL compindex = (REAL)cind;
   varsys->SetProgVar(CompIndex,&compindex);
   varsys->SetProgVar(ZoneIndex,&zoneindex);
   varsys->SetProgVar(WaterFraction,&eps);
   varsys->SetProgVar(Volume,&v);
   varsys->SetProgVar(BulkVolume,&v);
   CARDINAL i;

   // calculate input fluxes:

   REAL Q = 0;
   REAL* flux = new REAL[numvolstatevar];
   REAL* conc = new REAL[numvolstatevar];
   for ( i=0; i<numvolstatevar; i++ )
   {
      flux[i] = 0.0; conc[i] = 0.0;
   }

   const CARDINAL advinconn = 0;                      // input from advinlink
   for ( CARDINAL j=0; j<advinnum[advinconn]; j++ )
   {
      Q += advinlink[advinconn][j]
               ->AdvOutQ(varsys,Y,advinindex[advinconn][j]);
   }
   for ( i=0; i<numvolstatevar; i++ )
   {
      for ( CARDINAL j=0; j<advinnum[advinconn]; j++ )
      {
         flux[i] += advinlink[advinconn][j]
                     ->AdvOutJ(varsys,Y,advinindex[advinconn][j],statevar[i]);
      }
   }

   varsys->SetProgVar(Discharge,&Q);                 // input from AQCOMP
   if ( Q > 0.0 )
   {
      for ( i=0; i<numvolstatevar; i++ ) conc[i] = flux[i]/Q;
   }
   for ( i=0; i<numvolstatevar; i++ )
   {
      statevar[i]->SetStateVar(&conc[i]);
   }
   if ( inflow != 0 ) Q += inflow->Value();
   for ( i=0; i<numvolstatevar; i++ )
   {
      for ( CARDINAL j=0; j<numinput; j++ )
      {
         if ( inputvar[j] == statevar[i] )
         {
            flux[i] += inputflux[j]->Value(); break;
         }
      }
   }

   const CARDINAL diffconn = 0;                      // input from difflink
   for ( i=0; i<numvolstatevar; i++ )
   {
      for ( CARDINAL j=0; j<diffnum[diffconn]; j++ )
      {
         flux[i] += difflink[diffconn][j]
                       ->DiffJ(Y,diffindex[diffconn][j],statevar[i]);
      }
   }

   // calculate rates:

   varsys->Reset();
   for ( i=0; i<numvolstatevar; i++ )
   {
      conc[i] = y[i]/v;
      statevar[i]->SetStateVar(&conc[i]);
   }
   for ( i=numvolstatevar; i<numstatevar; i++ )
   {
      statevar[i]->SetStateVar(&y[i]);
   }
   CalcRates(delta);
   for ( i=0; i<numvolstatevar; i++ ) delta[i] = v*delta[i];

   // formulate differential equations:

   REAL Qef = Q;
   if ( fixedvol == FALSE ) Qef = y[numstatevar+1];
   for ( i=0; i<numvolstatevar; i++ )
   {
      delta[i] += flux[i]-Qef*y[i]/v;
   }
   for ( i=0; i<numdynstatevar; i++ )
   {
      delta[i] -= yt[i];
   }
   delta[numstatevar] = y[numstatevar]-Q;
   if ( fixedvol == FALSE )
   {
      Qef = Qout->Value();
      if ( Qef < 0.0 ) return FALSE;
      delta[numstatevar+1] = y[numstatevar+1]-Qef;
      delta[numstatevar+2] = Q-Qef-yt[numstatevar+2];
   }

   for ( i=0; i<numstatevar; i++ ) statevar[i]->ResetStateVar();
   delete [] flux;
   delete [] conc;

/*
cout << "\n\nDelta: " << Symbol();
for ( i=0; i<NumEq(); i++ )
{
   cout << "\n";
   if ( i < 10 )  cout << " ";
   if ( i < 100 ) cout << " ";
   cout << i << "  ";
   switch ( EqType(i) )
   {
      case ALG:  cout << "ALG "; break;
      case DIFF: cout << "DIFF"; break;
      default:   cout << "    "; break;
   }
   cout << "  y=";
   cout << y[i];
   cout << "  yt=";
   cout << yt[i];
   cout << "  delta=";
   cout << delta[i];
}
cout.flush();
*/
/*
cjob->precision(10);
*cjob << "\n\nDelta: " << Symbol();
for ( i=0; i<NumEq(); i++ )
{
   *cjob << "\n";
   if ( i < 10 )  *cjob << " ";
   if ( i < 100 ) *cjob << " ";
   *cjob << i << "  ";
   switch ( EqType(i) )
   {
      case ALG:  *cjob << "ALG "; break;
      case DIFF: *cjob << "DIFF"; break;
      default:   *cjob << "    "; break;
   }
   *cjob << "  y=";
   *cjob << y[i];
   *cjob << "  yt=";
   *cjob << yt[i];
   *cjob << "  delta=";
   *cjob << delta[i];
}
cjob->flush();
*/

   return TRUE;
}


BOOLEAN MIXCOMP::GridValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                           REAL* t_ptr, const REAL* Y,
                           CARDINAL zone, CARDINAL pt, AQVAR* var,
                           REAL& value)
{
   if ( Active() != TRUE ) return FALSE;
   SetGlobalState(varsys,calcnum_ptr,t_ptr,Y);
   if ( zone != 0 )                   return FALSE;
   if ( pt != 0 )                     return FALSE;
   if ( varsys->Exist(var) == FALSE ) return FALSE;
   if ( AllowedVar(var) == FALSE )    return FALSE;
   if ( Active() == FALSE )           return FALSE;
   const REAL* y = &Y[ind_y];
   REAL v = vol;
   if ( fixedvol == FALSE ) v = y[numstatevar+2];
   REAL cn = *calcnum_ptr; REAL eps = 1.0; 
   REAL zoneindex = 0.0; 
   REAL compindex = (REAL)cind;
   varsys->SetProgVar(CalcNumber,&cn);
   varsys->SetProgVar(Time,t_ptr);
   varsys->SetProgVar(CompIndex,&compindex);
   varsys->SetProgVar(ZoneIndex,&zoneindex);
   varsys->SetProgVar(WaterFraction,&eps);
   varsys->SetProgVar(Volume,&v);
   varsys->SetProgVar(BulkVolume,&v);
   varsys->SetProgVar(Discharge,&y[numstatevar]);
   CARDINAL i;
   REAL* conc = new REAL[numvolstatevar];
   for ( i=0; i<numvolstatevar; i++ )
   {
      conc[i] = y[i]/v;
      statevar[i]->SetStateVar(&conc[i]);
   }
   for ( i=numvolstatevar; i<numstatevar; i++ )
   {
      statevar[i]->SetStateVar(&y[i]);
   }
   varsys->Reset();
   value = var->Value();
   for ( i=0; i<numstatevar; i++ ) statevar[i]->ResetStateVar();
   delete [] conc;
   return TRUE;
}


BOOLEAN MIXCOMP::SpaceValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                            REAL* t_ptr, const REAL* Y,
                            CARDINAL zone, REAL, BOOLEAN, AQVAR* var,
                            REAL& value)
{
   if ( Active() != TRUE ) return FALSE;
   CARDINAL pt = 0;
   return GridValue(varsys,calcnum_ptr,t_ptr,Y,zone,pt,var,value);
}


CARDINAL MIXCOMP::InitZone(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("MIXCOMP::InitZone: Illegal index");
   return initzone[index];
}


AQVAR* MIXCOMP::InitVar(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("MIXCOMP::InitVar: Illegal index");
   return initvar[index];
}


const FORMVAR* MIXCOMP::InitVal(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("MIXCOMP::InitVal: Illegal index");
   return initval[index];
}


BOOLEAN MIXCOMP::AddInitCond(const VARLIST* varlist, CARDINAL zone,
                             AQVAR* var, const char* inpline, char* errorline,
                             CARDINAL& numparseerrors, CARDINAL pos)
{
   numparseerrors = 0;
   if ( InList() == TRUE )                        return FALSE;
   if ( inpline == 0 )                            return FALSE;
   if ( inpline[0] == '\0' )                      return FALSE;
   if ( varlist->Exist(var) == FALSE )            return FALSE;
   if ( zone>=NumZones() && Type()!=SoilComp )    return FALSE;
   if ( pos > numinit ) pos = numinit;
   CARDINAL i;
   for ( i=0; i<numinit; i++ )
   {
      if ( initzone[i]==zone && initvar[i]==var ) return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("InitCond");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                             return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                             return FALSE;
   }
   CARDINAL* newzone = new CARDINAL[numinit+1];
   AQVAR**   newvar  = new AQVAR*[numinit+1];
   FORMVAR** newval  = new FORMVAR*[numinit+1];
   for ( i=0; i<pos; i++ )
   {
      newzone[i] = initzone[i];
      newvar[i]  = initvar[i];
      newval[i]  = initval[i];
   }
   newzone[pos] = zone;
   newvar[pos]  = var;
   newval[pos]  = formvar;
   for ( i=pos; i<numinit; i++ )
   {
      newzone[i+1] = initzone[i];
      newvar[i+1]  = initvar[i];
      newval[i+1]  = initval[i];
   }
   delete initzone; initzone = newzone;
   delete initvar;  initvar = newvar;
   delete initval;  initval = newval;
   numinit++;
   CalcArg();
   return TRUE;
}


BOOLEAN MIXCOMP::ReplaceInitCond(const VARLIST* varlist, CARDINAL zone,
                                 AQVAR* var, const char* inpline,
                                 char* errorline, CARDINAL& numparseerrors,
                                 CARDINAL pos)
{
   numparseerrors = 0;
   if ( InList() == TRUE )                                  return FALSE;
   if ( inpline == 0 )                                      return FALSE;
   if ( inpline[0] == '\0' )                                return FALSE;
   if ( varlist->Exist(var) == FALSE )                      return FALSE;
   if ( zone >= NumZones() )                                return FALSE;
   if ( pos >= numinit )                                    return FALSE;
   CARDINAL i;
   for ( i=0; i<numinit; i++ )
   {
      if ( i!=pos && initzone[i]==zone && initvar[i]==var ) return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("InitCond");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                                       return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                                       return FALSE;
   }
   delete initval[pos];
   initzone[pos] = zone;
   initvar[pos]  = var;
   initval[pos]  = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN MIXCOMP::DeleteInitCond(CARDINAL pos)
{
   if ( pos >= numinit ) return FALSE;
   CARDINAL* newzone = new CARDINAL[numinit-1];
   AQVAR**   newvar  = new AQVAR*[numinit-1];
   FORMVAR** newval  = new FORMVAR*[numinit-1];
   CARDINAL i;
   for ( i=0; i<pos; i++ )
   {
      newzone[i] = initzone[i];
      newvar[i]  = initvar[i];
      newval[i]  = initval[i];
   }
   for ( i=pos+1; i<numinit; i++ )
   {
      newzone[i-1] = initzone[i];
      newvar[i-1]  = initvar[i];
      newval[i-1]  = initval[i];
   }
   delete initval[pos];
   delete initzone; initzone = newzone;
   delete initvar; initvar = newvar;
   delete initval; initval = newval;
   numinit--;
   CalcArg();
   return TRUE;
}


BOOLEAN MIXCOMP::Inflow(const VARLIST* varlist,
                       const char* inpline,
                       char* errorline)
{
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("Inflow");
   if ( formvar->Parse(inpline,varlist,errorline) != 0 )
   {
      delete formvar; return FALSE;
   }
   delete inflow; inflow = formvar;
   CalcArg();
   return TRUE;
}


AQVAR* MIXCOMP::InputVar(CARDINAL index) const
{
   if ( index >= numinput )
      FatalError("MIXCOMP::InputVar: Illegal index");
   return inputvar[index];
}


const FORMVAR* MIXCOMP::InputFlux(CARDINAL index) const
{
   if ( index >= numinput )
      FatalError("MIXCOMP::InputFlux: Illegal index");
   return inputflux[index];
}


BOOLEAN MIXCOMP::AddInput(const VARLIST* varlist, AQVAR* var,
                          const char* inpline, char* errorline,
                          CARDINAL& numparseerrors, CARDINAL pos)
{
   numparseerrors = 0;
   if ( InList() == TRUE )             return FALSE;
   if ( inpline == 0 )                 return FALSE;
   if ( inpline[0] == '\0' )           return FALSE;
   if ( varlist->Exist(var) == FALSE ) return FALSE;
   if ( pos > numinput ) pos = numinput;
   CARDINAL i;
   for ( i=0; i<numinput; i++ )
   {
      if ( inputvar[i] == var )        return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("Input");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                  return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                  return FALSE;
   }
   AQVAR**   newvar  = new AQVAR*[numinput+1];
   FORMVAR** newflux = new FORMVAR*[numinput+1];
   for ( i=0; i<pos; i++ )
   {
      newvar[i]  = inputvar[i];
      newflux[i] = inputflux[i];
   }
   newvar[pos]  = var;
   newflux[pos] = formvar;
   for ( i=pos; i<numinput; i++ )
   {
      newvar[i+1]  = inputvar[i];
      newflux[i+1] = inputflux[i];
   }
   delete inputvar;  inputvar  = newvar;
   delete inputflux; inputflux = newflux;
   numinput++;
   CalcArg();
   return TRUE;
}


BOOLEAN MIXCOMP::ReplaceInput(const VARLIST* varlist, AQVAR* var,
                              const char* inpline, char* errorline,
                              CARDINAL& numparseerrors, CARDINAL pos)
{
   numparseerrors = 0;
   if ( InList() == TRUE )              return FALSE;
   if ( inpline == 0 )                  return FALSE;
   if ( inpline[0] == '\0' )            return FALSE;
   if ( varlist->Exist(var) == FALSE )  return FALSE;
   if ( pos >= numinput )               return FALSE;
   CARDINAL i;
   for ( i=0; i<numinput; i++ )
   {
      if ( i!=pos && inputvar[i]==var ) return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("Input");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                   return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                   return FALSE;
   }
   delete inputflux[pos];
   inputvar[pos]  = var;
   inputflux[pos] = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN MIXCOMP::DeleteInput(CARDINAL pos)
{
   if ( pos >= numinput ) return FALSE;
   AQVAR**   newvar  = new AQVAR*[numinput-1];
   FORMVAR** newflux = new FORMVAR*[numinput-1];
   CARDINAL i;
   for ( i=0; i<pos; i++ )
   {
      newvar[i]  = inputvar[i];
      newflux[i] = inputflux[i];
   }
   for ( i=pos+1; i<numinput; i++ )
   {
      newvar[i-1]  = inputvar[i];
      newflux[i-1] = inputflux[i];
   }
   delete inputflux[pos];
   delete inputvar;  inputvar = newvar;
   delete inputflux; inputflux = newflux;
   numinput--;
   CalcArg();
   return TRUE;
}


BOOLEAN MIXCOMP::Vol(REAL v)
{
   if ( v <= 0.0 ) return FALSE;
   vol = v;
   Changed();
   return TRUE;
}


BOOLEAN MIXCOMP::FixedVol(BOOLEAN b)
{
   if ( b==FALSE && Qout==0 ) return FALSE;
   fixedvol = b;
   return TRUE;
}


BOOLEAN MIXCOMP::Outflow(const char* inpline,
                         const VARLIST* varlist,
                         char* errorline,
                         CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' )
   {
      if ( fixedvol == TRUE )
      {
         delete Qout; Qout = 0;
      }
      return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("Qout");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;
      if ( fixedvol == TRUE )
      {
         delete Qout; Qout = 0;
      }
      return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;
      if ( fixedvol == TRUE )
      {
         delete Qout; Qout = 0;
      }
      return FALSE;
   }

   delete Qout; Qout = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN MIXCOMP::AccQ(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccQ = relacc;
   absaccQ = absacc;
   return TRUE;
}


BOOLEAN MIXCOMP::AccV(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccV = relacc;
   absaccV = absacc;
   return TRUE;
}


void MIXCOMP::CalcArg()
{
  CARDINAL i,j;

   AQCOMP::CalcArg();
   CARDINAL num = numvararg;
   numvararg += numinit;
   for ( i=0; i<numinit; i++ ) numvararg += (initval[i])->NumVarArg();
   if ( inflow != 0 ) numvararg += inflow->NumVarArg();
   numvararg += numinput;
   for ( i=0; i<numinput; i++ ) numvararg += (inputflux[i])->NumVarArg();
   if ( Qout != 0 ) numvararg += Qout->NumVarArg();
   if ( numvararg > 0 )
   {
      AQVAR** newarg = new AQVAR*[numvararg];
      CARDINAL k=0;
      CARDINAL i;
      for ( i=0; i<num; i++ )
      {
         newarg[k] = vararg[i]; k++;
      }
      for ( i=0; i<numinit; i++ )
      {
         newarg[k] = initvar[i]; k++;
         for ( j=0; j<initval[i]->NumVarArg(); j++ )
         {
            newarg[k] = initval[i]->VarArg(j); k++;
         }
      }
      if ( inflow != 0 )
      {
         for ( i=0; i<inflow->NumVarArg(); i++ )
         {
            newarg[k] = inflow->VarArg(i); k++;
         }
      }
      for ( i=0; i<numinput; i++ )
      {
         newarg[k] = inputvar[i]; k++;
         for ( j=0; j<inputflux[i]->NumVarArg(); j++ )
         {
            newarg[k] = inputflux[i]->VarArg(j); k++;
         }
      }
      if ( Qout != 0 )
      {
         for ( i=0; i<Qout->NumVarArg(); i++ )
         {
            newarg[k] = Qout->VarArg(i); k++;
         }
      }
      if ( k != numvararg )
         FatalError("MIXCOMP::CalcArg: inconsistent number of arguments");
      delete [] vararg; vararg = newarg;
   }
}


CARDINAL MIXCOMP::NumZones() const
{
   return 1;
}


const char* MIXCOMP::ZoneName(CARDINAL index) const
{
   if ( index > 0 )
      FatalError("MIXCOMP::ZoneName: Illegal index");
   return ini.T(4);
}


const char* MIXCOMP::AdvInName(CARDINAL conn) const
{
   if ( conn > 0 )
      FatalError("MIXCOMP::AdvInName: Illegal connection");
   return ini.T(45);
}


const char* MIXCOMP::AdvOutName(CARDINAL conn) const
{
   if ( conn > 0 )
      FatalError("MIXCOMP::AdvOutName: Illegal connection");
   return ini.T(46);
}


const char* MIXCOMP::DiffName(CARDINAL conn) const
{
   if ( conn > 0 )
      FatalError("MIXCOMP::DiffName: Illegal connection");
   return ini.T(50);
}


BOOLEAN MIXCOMP::AdvInExZ(CARDINAL conn) const
{
   if ( conn > 0 ) FatalError("MIXCOMP::AdvInExZ: Illegal connection");
   return FALSE;
}


REAL MIXCOMP::AdvInZ(const REAL*, CARDINAL) const
{
   FatalError("MIXCOMP::AdvInZ: Illegal call to this function");
   return 0.0;
}


REAL MIXCOMP::AdvOutQ(VARSYS* varsys, const REAL* Y, CARDINAL conn)
{
   if ( conn > 0 ) FatalError("MIXCOMP::AdvOutQ: Illegal connection");
   if ( Active() == FALSE ) return 0.0;
   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   const REAL* y = &Y[ind_y];
   REAL v;
   if ( fixedvol == TRUE )
   {
      v = vol;
      varsys->SetProgVar(Discharge,&y[numstatevar]);
   }
   else
   {
      v = y[numstatevar+2];
      varsys->SetProgVar(Discharge,&y[numstatevar+1]);
   }
   if ( numCout != numvolstatevar )
   {
      delete [] Cout; Cout = new REAL[numvolstatevar];
      numCout = numvolstatevar;
   }
   for ( CARDINAL i=0; i<numvolstatevar; i++ )
   {
      Cout[i] = y[i]/v;
      statevar[i]->SetStateVar(&Cout[i]);
   }
   REAL Qef = y[numstatevar];
   if ( fixedvol == FALSE ) Qef = y[numstatevar+1];
   return Qef;
}


REAL MIXCOMP::AdvOutJ(VARSYS* varsys,
                      const REAL* Y,
                      CARDINAL conn,
                      const AQVAR* var)
{
   if ( conn > 0 ) FatalError("MIXCOMP::AdvOutJ: Illegal connection");
   if ( Active() == FALSE ) return 0.0;
   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   const REAL* y = &Y[ind_y];
   REAL v;
   if ( fixedvol == TRUE )
   {
      v = vol;
      varsys->SetProgVar(Discharge,&y[numstatevar]);
   }
   else
   {
      v = y[numstatevar+2];
      varsys->SetProgVar(Discharge,&y[numstatevar+1]);
   }
   if ( numCout != numvolstatevar )
   {
      delete [] Cout; Cout = new REAL[numvolstatevar];
      numCout = numvolstatevar;
   }
   CARDINAL i;
   for ( i=0; i<numvolstatevar; i++ )
   {
      Cout[i] = y[i]/v;
      statevar[i]->SetStateVar(&Cout[i]);
   }
   for ( i=0; i<numvolstatevar; i++ )
   {
      if ( var == statevar[i] )
      {
         REAL Qef = y[numstatevar];
         if ( fixedvol == FALSE ) Qef = y[numstatevar+1];
         return y[i]/v*Qef;
      }
   }
   return 0.0;
}


REAL MIXCOMP::DiffC(const REAL* Y, CARDINAL conn, const AQVAR* var) const
{
   if ( conn > 0 ) FatalError("MIXCOMP::DiffC: Illegal connection");
   if ( Active() == FALSE ) return 0.0;
   const REAL* y = &Y[ind_y];
   REAL v = vol;
   if ( fixedvol == FALSE ) v = y[numstatevar+2];
   for ( CARDINAL i=0; i<numvolstatevar; i++ )
   {
      if ( var == statevar[i] ) return y[i]/v;
   }
   return 0.0;
}


JOBSTATUS MIXCOMP::Load(std::istream& in,
                        const VARLIST* varlist,
                        const PROCLIST* proclist)
{
   char Buffer[BufSize]; char Buffer1[BufSize];

   if ( LoadStart(in) == FALSE )                                return PROBLEM;
   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )                     return PROBLEM;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.1 converted from 1.0
         {
            REAL r;

            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,1);
                                    if ( status != OK ) return status;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( strlen(Buffer) > 0 )
            {
               if ( Inflow(varlist,Buffer,Buffer1) == FALSE )   return PROBLEM;
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            CARDINAL n;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddInput(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            CARDINAL zone;
            while ( LoadNumber(in,&zone) == TRUE )
            {
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddInitCond(varlist,zone,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadBoolean(in,&fixedvol) == FALSE )           return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( Vol(r) == FALSE )                              return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            Outflow(Buffer,varlist,Buffer1,n);
            if ( fixedvol==FALSE && Qout==0 )                   return PROBLEM;
         }
         break;
      case 2: // aquavers=2: compartment index added 31.01.96
         {
            REAL r;

            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,2);
                                    if ( status != OK ) return status;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( strlen(Buffer) > 0 )
            {
               if ( Inflow(varlist,Buffer,Buffer1) == FALSE )   return PROBLEM;
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            CARDINAL n;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddInput(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            CARDINAL zone;
            while ( LoadNumber(in,&zone) == TRUE )
            {
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddInitCond(varlist,zone,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadBoolean(in,&fixedvol) == FALSE )           return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( Vol(r) == FALSE )                              return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            Outflow(Buffer,varlist,Buffer1,n);
            if ( fixedvol==FALSE && Qout==0 )                   return PROBLEM;
         }
		 break;
      case 3: // aquavers=3: AQUASIM 2.0 gamma may 1997, accuracies introduced
         {
            REAL r;

            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,2);
                                    if ( status != OK ) return status;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( strlen(Buffer) > 0 )
            {
               if ( Inflow(varlist,Buffer,Buffer1) == FALSE )   return PROBLEM;
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            CARDINAL n;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddInput(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            CARDINAL zone;
            while ( LoadNumber(in,&zone) == TRUE )
            {
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddInitCond(varlist,zone,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadBoolean(in,&fixedvol) == FALSE )           return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( Vol(r) == FALSE )                              return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            Outflow(Buffer,varlist,Buffer1,n);
            if ( fixedvol==FALSE && Qout==0 )                   return PROBLEM;
            REAL relacc,absacc;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccQ(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccV(relacc,absacc) == FALSE )                 return PROBLEM;
         }
         break;
      case 4: // aquavers=4: May 1998 BOOLEAN active added
         {
            REAL r;

            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,3);
                                    if ( status != OK ) return status;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( strlen(Buffer) > 0 )
            {
               if ( Inflow(varlist,Buffer,Buffer1) == FALSE )   return PROBLEM;
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            CARDINAL n;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddInput(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            CARDINAL zone;
            while ( LoadNumber(in,&zone) == TRUE )
            {
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddInitCond(varlist,zone,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadBoolean(in,&fixedvol) == FALSE )           return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( Vol(r) == FALSE )                              return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            Outflow(Buffer,varlist,Buffer1,n);
            if ( fixedvol==FALSE && Qout==0 )                   return PROBLEM;
            REAL relacc,absacc;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccQ(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccV(relacc,absacc) == FALSE )                 return PROBLEM;
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                                  return PROBLEM;
   return OK;
}


JOBSTATUS MIXCOMP::Save(std::ostream& out)
{
   CARDINAL i;
   char     Buffer[BufSize];

   INTEGER aquavers = 4;

   if ( SaveStart(out) == FALSE )                              return PROBLEM;

   if ( SaveNumber(out,aquavers) == FALSE )                    return PROBLEM;

   JOBSTATUS status = AQCOMP::Save(out);   if ( status != OK ) return status;

   if ( inflow == 0 )
   {
      if ( SaveString(out,"") == FALSE )                       return PROBLEM;
   }
   else
   {
      if ( inflow->UnParse(Buffer,sizeof(Buffer)-1) != 0 )     return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                   return PROBLEM;
   }

   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   for ( i=0; i<numinput; i++ )
   {
      if ( SaveString(out,InputVar(i)->Symbol()) == FALSE )    return PROBLEM;
      if ( InputFlux(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                   return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;

   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   for ( i=0; i<numinit; i++ )
   {
      if ( SaveNumber(out,InitZone(i)) == FALSE )              return PROBLEM;
      if ( SaveString(out,InitVar(i)->Symbol()) == FALSE )     return PROBLEM;
      if ( InitVal(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 ) return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                   return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;

   if ( SaveBoolean(out,FixedVol()) == FALSE )                 return PROBLEM;
   if ( SaveNumber(out,Vol()) == FALSE )                       return PROBLEM;
   if ( Qout == 0 )
   {
      if ( FixedVol() == FALSE )                               return PROBLEM;
      Buffer[0] = '\0';
   }
   else
   {
      if ( Qout->UnParse(Buffer,sizeof(Buffer)-1) != 0 )       return PROBLEM;
   }
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;

   if ( SaveNumber(out,RelAccQ()) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,AbsAccQ()) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,RelAccV()) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,AbsAccV()) == FALSE )                   return PROBLEM;

   if ( SaveEnd(out) == FALSE )                                return PROBLEM;
   return OK;
}


JOBSTATUS MIXCOMP::Write(std::ostream& out, BOOLEAN sh)
{
   JOBSTATUS status = AQCOMP::Write(out,sh);  
                                           if ( status != OK ) return status;
   if ( sh == TRUE ) return OK;

   CARDINAL i;
   char     Buffer[BufSize];

   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   CARDINAL pos3 = pos1+2;
   CARDINAL pos4 = pos3+2;

   PrintIndent(pos2-1);

   if ( Print(out,ini.T(21),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( NumInitCond() > 0 )
   {
      PrintIndent(pos4-1);
      if ( Print(out,ini.T(22),pos3) == FALSE )                return PROBLEM;
      if ( Print(out,"(") == FALSE )                           return PROBLEM;
      if ( Print(out,ini.T(11)) == FALSE )                     return PROBLEM;
      if ( Print(out,") : ") == FALSE )                        return PROBLEM;
      if ( Print(out,ini.T(23)) == FALSE )                     return PROBLEM;
      for ( i=0; i<NumInitCond(); i++ )
      {
         if ( Print(out,InitVar(i)->Symbol(),pos3) == FALSE )  return PROBLEM;
         if ( Print(out,"(") == FALSE )                        return PROBLEM;
         if ( Print(out,ZoneName(InitZone(i))) == FALSE )      return PROBLEM;
         if ( Print(out,") : ") == FALSE )                     return PROBLEM;
         if ( InitVal(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer) == FALSE )                     return PROBLEM;
      }
   }

   PrintIndent(pos2-1);
   if ( Print(out,ini.T(17),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Inflow() != 0 )
   {
      if ( inflow->UnParse(Buffer,sizeof(Buffer)-1) != 0 )     return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;
   }

   if ( Print(out,ini.T(18),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( NumInput() > 0 )
   {
      PrintIndent(pos4-1);
      if ( Print(out,ini.T(19),pos3) == FALSE )                return PROBLEM;
      if ( Print(out," : ") == FALSE )                         return PROBLEM;
      if ( Print(out,ini.T(20)) == FALSE )                     return PROBLEM;
      for ( i=0; i<NumInput(); i++ )
      {
         if ( Print(out,InputVar(i)->Symbol(),pos3) == FALSE ) return PROBLEM;
         if ( Print(out," : ") == FALSE )                      return PROBLEM;
         if ( InputFlux(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer) == FALSE )                     return PROBLEM;
      }
   }

   if ( FixedVol() == TRUE )
   {
      if ( Print(out,ini.T(25),pos1) == FALSE )                return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Print(out,Vol(),pos2) == FALSE )                    return PROBLEM;
   }
   else
   {
      if ( Print(out,ini.T(49),pos1) == FALSE )                return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Qout->UnParse(Buffer,sizeof(Buffer)-1) != 0 )       return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;
   }

   if ( Print(out,ini.T(220),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,ini.T(221),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,RelAccQ(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(222),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,AbsAccQ(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(223),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,RelAccV(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(224),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,AbsAccV(),pos2) == FALSE )                   return PROBLEM;

   return OK;
}

//////////////////////////////////////////////////////////////////////////////
