///////////////////////////////   soilcomp.C   /////////////////////////////// //
//              date:       person:           comments:
//
// creation:    30.06.94    Werner Simon
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// n  = number of grid points
// nv = number of volume state variables
// ns = number of surface state variables
// ne = number of equilibrium state variables
// NS = nv+ns+ne = number of state variables
// NZ = total number of states (1+sum(all serial zones of all parallel zones))
// Cv = volume state variable
// Cs = surface state variable
// Ce = equilibrium state variable
//
// Upstream End:            ================  x[0]
//                             element 1      x[1]
//                          ----------------
//                             element 2      x[2]
//                          ----------------
//                                            x[3]
//                          ----------------
//
//
// 
//                          ----------------
//                             element n-2    x[n-2]
// Downstream End:          ================  x[n-1]
//
// type component (ioff=1)                    meaning
// ---------------------------------------------------------------------------
// advective zone:
// alg  y[0]                                  Q at x[0]
// alg  y[ioff]                               Cv[1] at x[0]
//  .    .                                     .
// alg  y[ioff+nv-1]                          Cv[nv] at x[0]
// diff y[ioff+nv]                            Cs[1] at x[0]
//  .    .                                     .
// diff y[ioff+nv+ns-1]                       Cs[ns] at x[0]
// alg  y[ioff+NS]                            Ce[1] at x[0]
//  .    .                                     .
// alg  y[ioff+NS-1]                          Ce[ne] at x[0]
//
// stagnant zones:
// alg  y[ioff+NS]                            Cv[1] in (1,1) at x[0] 
//  .    .                                     .
// alg  y[ioff+NS+nv-1]                       Cv[nv] in (1,1) at x[0]
// diff y[ioff+NS+nv]                         Cs[1] in (1,1) at x[0]
//  .    .                                     .
// diff y[ioff+NS+nv+ns-1]                    Cs[ns] in (1,1) at x[0]
// alg  y[ioff+NS+nv+ns]                      Ce[1] in (1,1) at x[0]
//  .    .                                     .
// alg  y[ioff+NS+NS-1]                       Ce[ne] in (1,1) at x[0]
//  .    .                                     .
//  .    .                                     .
//  .    .                                     .
// alg y[ioff+NS*NZ-1]                        Ce[ne] in NZ at x[0]
// ----------------------------------------------------------------------------
// advective zone:
// alg  y[ioff+NS*NZ]                         Q at x[1]
// diff y[ioff+NS*NZ+ioff]                    Cv[1] at x[1]
//  .    .                                     .
// diff y[ioff+NS*NZ+ioff+nv-1]               Cv[nv] at x[1]
// diff y[ioff+NS*NZ+ioff+nv]                 Cs[1] at x[1]
//  .    .                                     .
// diff y[ioff+NS*NZ+ioff+nv+ns-1]            Cs[ns] at x[1]
// alg  y[ioff+NS*NZ+ioff+nv+ns]              Ce[1] at x[1]
//  .    .                                     .
// alg  y[ioff+NS*NZ+ioff+NS-1]               Ce[ne] at x[1]
//
// stagnant zones:
// alg  y[ioff+NS*NZ+ioff+NS]                 Cv[1] in (1,1) at x[1]  
//  .    .                                     .
// alg  y[ioff+NS*NZ+ioff+NS+nv-1]            Cv[nv] in (1,1) at x[1]
// diff y[ioff+NS*NZ+ioff+NS+nv]              Cs[1] in (1,1) at x[1]
//  .    .                                     .
// diff y[ioff+NS*NZ+ioff+NS+nv+ns-1]         Cs[ns] in (1,1) at x[1]
// alg  y[ioff+NS*NZ+ioff+NS+nv+ns]           Ce[1] in (1,1) at x[1]
//  .    .                                     .
// alg  y[ioff+NS*NZ+ioff+NS+NS-1]            Ce[ne] in (1,1) at x[1]
//  .    .                                     .
//  .    .                                     .
//  .    .                                     .
// alg  y[ioff+NS*NZ+ioff+NS*NZ-1]            Ce[ne] in NZ at x[1]
// ---------------------------------------------------------------------------
//  .    .                                     .
//  .    .                                     .
//  .    .                                     .
// ---------------------------------------------------------------------------
// advective zone:
// alg  y[(n-1)*(ioff+NS*NZ)]                 Q at x[n-1]
// diff y[(n-1)*(ioff+NS*NZ)+ioff]            Cv[1] at x[n-1]
//  .    .                                     .
// diff y[(n-1)*(ioff+NS*NZ)+ioff+nv]         Cs[1] at x[n-1]
//  .    .                                     .
// diff y[(n-1)*(ioff+NS*NZ)+ioff+nv+ns-1]    Cs[ns] at x[n-1]
// alg  y[(n-1)*(ioff+NS*NZ)+ioff+nv+ns]      Ce[1] at x[n-1]
//  .    .                                     .
// alg  y[(n-1)*(ioff+NS*NZ)+ioff+NS-1]       Ce[ne] at x[n-1]
//
// stagnant zones:
// diff y[(n-1)*(ioff+NS*NZ)+ioff+NS]         Cv[1] in (1,1) at x[n-1]
//  .    .                                     .
// alg  y[(n-1)*(ioff+NS*NZ)+ioff+NS+nv-1]    Cv[nv] in (1,1) at x[n-1]
// diff y[(n-1)*(ioff+NS*NZ)+ioff+NS+nv]      Cs[1] in (1,1) at x[n-1]
//  .    .                                     .
// diff y[(n-1)*(ioff+NS*NZ)+ioff+NS+nv+ns-1] Cs[ns] in (1,1) at x[n-1]
// alg  y[(n-1)*(ioff+NS*NZ)+ioff+NS+nv+ns]   Ce[1] in (1,1) at x[n-1]
//  .    .                                     .
// alg  y[(n-1)*(ioff+NS*NZ)+ioff+NS+NS-1]    Ce[ne] in (1,1) at x[n-1]
//  .    .                                     .
//  .    .                                     .
//  .    .                                     .
// alg  y[(n-1)*(ioff+NS*NZ)+ioff+NS*NZ-1]    Ce[ne] in NZ at x[n-1]
// ---------------------------------------------------------------------------
// alg  y[n*(ioff+NS*NZ)]                     dispersion at x[n-1]
//
//////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "fortran.h"
#include "formvar.h"
#include "othervar.h"
#include "varsys.h"
#include "soilcomp.h"
#include "aqlink.h"

//////////////////////////////////////////////////////////////////////////////

const CARDINAL BufSize = 1024;

static char namebuffer[BufSize];

//////////////////////////////////////////////////////////////////////////////

void SERZONE::init()
{
   zind = 1;
   volfract = 0;
   numexch = 0;
   exchvar = 0;
   exchval = 0;
   exchfac = 0;
   numvararg = 0;
   vararg = 0;
   return;
}


SERZONE::SERZONE()
{
   init();
}


SERZONE::SERZONE(const SERZONE& zone)
{
   init();
   CARDINAL i;
   zind = zone.zind;
   if ( zone.volfract != 0 ) volfract = new FORMVAR(zone.volfract);
   numexch = zone.numexch;
   if ( numexch > 0 )
   {
      exchvar = new AQVAR*[numexch];
      exchval = new FORMVAR*[numexch];
      exchfac = new FORMVAR*[numexch];
      for ( i=0; i<numexch; i++ )
      {
         exchvar[i] = zone.exchvar[i];
         exchval[i] = new FORMVAR(zone.exchval[i]);
         exchfac[i] = new FORMVAR(zone.exchfac[i]);
      }
   }
   CalcVarArg();
}


SERZONE::SERZONE(const SERZONE* zone)
{
   init();
   CARDINAL i;
   if ( zone != 0 )
   {
      zind = zone->zind;
      if ( zone->volfract != 0 ) volfract = new FORMVAR(zone->volfract);
      numexch = zone->numexch;
      if ( numexch > 0 )
      {
         exchvar = new AQVAR*[numexch];
         exchval = new FORMVAR*[numexch];
         exchfac = new FORMVAR*[numexch];
         for ( i=0; i<numexch; i++ )
         {
            exchvar[i] = zone->exchvar[i];
            exchval[i] = new FORMVAR(zone->exchval[i]);
            exchfac[i] = new FORMVAR(zone->exchfac[i]);
         }
      }
   }
   CalcVarArg();
}


SERZONE::~SERZONE()
{
   for ( CARDINAL i=0; i<numexch; i++ )
   {
      delete exchval[i];
      delete exchfac[i];
   }
   delete volfract;   volfract = 0;
   delete [] exchvar; exchvar = 0;
   delete [] exchval; exchval = 0;
   delete [] exchfac; exchfac = 0;

   delete [] vararg; vararg = 0; numvararg = 0;
}


BOOLEAN SERZONE::AllowedProgVarType(PROGVARTYPE type) const
{
   if ( type == CalcNumber )    return TRUE;
   if ( type == CompIndex )     return TRUE;
   if ( type == ::ZoneIndex )   return TRUE;
   if ( type == Time )          return TRUE;
   if ( type == WaterFraction ) return TRUE;
   if ( type == SpaceX )        return TRUE;
   if ( type == Discharge )     return TRUE;
   if ( type == CrossSection )  return TRUE;
   return FALSE;
}


BOOLEAN SERZONE::AllowedVar(const AQVAR* var) const
{
   if ( var->Type() == ProgVar )
   {
      PROGVAR* pv = (PROGVAR*)var;
      if ( AllowedProgVarType(pv->ProgVarType()) == FALSE ) return FALSE;
   }
   for ( CARDINAL i=0; i<var->NumVarArg(); i++ )
   {
      if ( AllowedVar(var->VarArg(i)) == FALSE ) return FALSE;
   }
   return TRUE;
}


AQVAR* SERZONE::VarArg(CARDINAL index) const
{
   if ( index >= numvararg )
   {
      FatalError("ZONE::VarArg: index too large");
   }
   else
   {
      return vararg[index];
   }
   return 0;
}


BOOLEAN SERZONE::Arg(const AQVAR* var) const
{
   for ( CARDINAL i=0; i<numvararg; i++ )
   {
      if ( vararg[i] == var )              return TRUE;
      if ( (vararg[i])->Arg(var) == TRUE ) return TRUE;
   }
   return FALSE;
}


BOOLEAN SERZONE::ReplaceVar(AQVAR* oldvar, AQVAR* newvar)
{
   if ( volfract != 0 ) volfract->ReplaceVarArg(oldvar,newvar);
   for ( CARDINAL i=0; i<numexch; i++ )
   {
      if ( exchvar[i] == oldvar ) exchvar[i] = newvar;
      exchval[i]->ReplaceVarArg(oldvar,newvar);
      exchfac[i]->ReplaceVarArg(oldvar,newvar);
   }
   CalcVarArg();
   return TRUE;
}


BOOLEAN SERZONE::ExchangeVar(AQVAR* var1, AQVAR* var2)
{
   AQVAR* v = new CONSTVAR;
   if ( volfract != 0 )
   {
      volfract->ReplaceVarArg(var1,v);
      volfract->ReplaceVarArg(var2,var1);
      volfract->ReplaceVarArg(v,var2);
   }
   for ( CARDINAL i=0; i<numexch; i++ )
   {
      if ( exchvar[i] == var1 )
      {
         exchvar[i] = var2;
      }
      else
      {
         if ( exchvar[i] == var2 ) exchvar[i] = var1;
      }
      exchval[i]->ReplaceVarArg(var1,v);
      exchval[i]->ReplaceVarArg(var2,var1);
      exchval[i]->ReplaceVarArg(v,var2);
      exchfac[i]->ReplaceVarArg(var1,v);
      exchfac[i]->ReplaceVarArg(var2,var1);
      exchfac[i]->ReplaceVarArg(v,var2);
   }
   CalcVarArg();
   return TRUE;
}


BOOLEAN SERZONE::ZoneIndex(CARDINAL i)
{
   if ( i == 0 ) return FALSE;
   zind = i;
   return TRUE;
}


BOOLEAN SERZONE::VolumeFract(const char* inpline,
                          const VARLIST* varlist,
                          char* errorline,
                          CARDINAL& numparseerrors)
{
   if ( inpline == 0 )            return FALSE;
   if ( inpline[0] == '\0' )      return FALSE;

   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("VolumeFract");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar; return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar; return FALSE;
   }
   delete volfract; volfract = formvar;
   CalcVarArg();
   return TRUE;
}


const AQVAR* SERZONE::ExchVar(CARDINAL index) const       // 21.10.94 !!!
{
   if ( index >= numexch )
      FatalError("SERZONE::ExchVar: illegal index");
   return exchvar[index];
}


const FORMVAR* SERZONE::ExchVal(CARDINAL index) const
{
   if ( index >= numexch )
      FatalError("SERZONE::ExchVal: illegal index");
   return exchval[index];
}


const FORMVAR* SERZONE::ExchFac(CARDINAL index) const
{
   if ( index >= numexch )
      FatalError("SERZONE::ExchFac: illegal index");
   return exchfac[index];
}


BOOLEAN SERZONE::AddExchCoeff(const VARLIST* varlist,AQVAR* var,
                              const char* inpval, char* errorlineval,
                              CARDINAL& parseerrorsval,
                              const char* inpfac, char* errorlinefac,
                              CARDINAL& parseerrorsfac,
                              CARDINAL pos)
{
   parseerrorsval = 0;
   parseerrorsfac = 0;
   if ( inpval == 0 )                  return FALSE;
   if ( inpfac == 0 )                  return FALSE;
   if ( inpval[0] == '\0' )            return FALSE;
   if ( inpfac[0] == '\0' )            return FALSE;
   if ( varlist->Exist(var) == FALSE ) return FALSE;
   if ( pos > numexch ) pos = numexch;
   CARDINAL i;
   for ( i=0; i<numexch; i++ )
   {
      if ( exchvar[i] == var )        return FALSE;
   }
   FORMVAR* exch = new FORMVAR;
   exch->Symbol("ExchCoeff");
   parseerrorsval = exch->Parse(inpval,varlist,errorlineval);
   if ( parseerrorsval != 0 )
   {
      delete exch;                    return FALSE;
   }
   if ( AllowedVar(exch) == FALSE )
   {
      delete exch;                    return FALSE;
   }
   FORMVAR* fact = new FORMVAR;
   fact->Symbol("ConvFact1");
   parseerrorsfac = fact->Parse(inpfac,varlist,errorlinefac);
   if ( parseerrorsfac != 0 )
   {
      delete exch; delete fact;       return FALSE;
   }
   if ( AllowedVar(fact) == FALSE )
   {
      delete exch; delete fact;       return FALSE;
   }

   AQVAR**   newvar = new AQVAR*[numexch+1];
   FORMVAR** newval = new FORMVAR*[numexch+1];
   FORMVAR** newfac = new FORMVAR*[numexch+1];

   for ( i=0; i<pos; i++ )
   {
      newvar[i] = exchvar[i];
      newval[i] = exchval[i];
      newfac[i] = exchfac[i];
   }

   newvar[pos] = var;
   newval[pos] = exch;
   newfac[pos] = fact;

   for ( i=pos; i<numexch; i++ )
   {
      newvar[i+1] = exchvar[i];
      newval[i+1] = exchval[i];
      newfac[i+1] = exchfac[i];
   }

   delete [] exchvar; exchvar = newvar;
   delete [] exchval; exchval = newval;
   delete [] exchfac; exchfac = newfac;

   numexch++;
   CalcVarArg();
   return TRUE;
}


BOOLEAN SERZONE::ReplaceExchCoeff(const VARLIST* varlist,AQVAR* var,
                                  const char* inpval, char* errorlineval,
                                  CARDINAL& parseerrorsval,
                                  const char* inpfac, char* errorlinefac,
                                  CARDINAL& parseerrorsfac,
                                  CARDINAL pos)
{
   parseerrorsval = 0;
   parseerrorsfac = 0;
   if ( inpval == 0 )                   return FALSE;
   if ( inpfac == 0 )                   return FALSE;
   if ( inpval[0] == '\0' )             return FALSE;
   if ( inpfac[0] == '\0' )             return FALSE;
   if ( varlist->Exist(var) == FALSE )  return FALSE;
   if ( pos > numexch )                return FALSE;
   CARDINAL i;
   for ( i=0; i<numexch; i++ )
   {
      if ( i!=pos && exchvar[i]==var ) return FALSE;
   }
   FORMVAR* exch = new FORMVAR;
   exch->Symbol("ExchCoeff");
   parseerrorsval = exch->Parse(inpval,varlist,errorlineval);
   if ( parseerrorsval != 0 )
   {
      delete exch;                     return FALSE;
   }
   if ( AllowedVar(exch) == FALSE )
   {
      delete exch;                     return FALSE;
   }
   FORMVAR* fact = new FORMVAR;
   fact->Symbol("ConvFact1");
   parseerrorsfac = fact->Parse(inpfac,varlist,errorlinefac);
   if ( parseerrorsfac != 0 )
   {
      delete exch; delete fact;        return FALSE;
   }
   if ( AllowedVar(fact) == FALSE )
   {
      delete exch; delete fact;        return FALSE;
   }
   delete exchval[pos];
   delete exchfac[pos];
   exchvar[pos] = var;
   exchval[pos] = exch;
   exchfac[pos] = fact;
   CalcVarArg();
   return TRUE;
}


BOOLEAN SERZONE::DeleteExchCoeff(CARDINAL pos)
{
   CARDINAL i;
   if ( pos >= numexch ) return FALSE;

   AQVAR**   newvar = new AQVAR*[numexch-1];
   FORMVAR** newval = new FORMVAR*[numexch-1];
   FORMVAR** newfac = new FORMVAR*[numexch-1];

   for ( i=0; i<pos; i++ )
   {
      newvar[i] = exchvar[i];
      newval[i] = exchval[i];
      newfac[i] = exchfac[i];
   }

   for ( i=pos+1; i<numexch; i++ )
   {
      newvar[i-1] = exchvar[i];
      newval[i-1] = exchval[i];
      newfac[i-1] = exchfac[i];
   }

   delete exchval[pos];
   delete exchfac[pos];

   delete [] exchvar; exchvar = newvar;
   delete [] exchval; exchval = newval;
   delete [] exchfac; exchfac = newfac;

   numexch--;
   CalcVarArg();
   return TRUE;
}


void SERZONE::CalcVarArg()
{
   CARDINAL j;
   numvararg = 0;
   if ( volfract != 0 ) numvararg += volfract->NumVarArg();
   numvararg += numexch;
   for ( j=0; j<numexch; j++ )
   {
      numvararg += exchval[j]->NumVarArg();
      numvararg += exchfac[j]->NumVarArg();
   }

   if ( numvararg > 0 )
   {
      delete [] vararg;
      vararg = new AQVAR*[numvararg];
      CARDINAL k = 0;
      CARDINAL i;
      if ( volfract != 0 )
      {
         for ( i=0; i<volfract->NumVarArg(); i++ )
         {
            vararg[k] = volfract->VarArg(i); k++;
         }
      }
      for ( i=0; i<numexch; i++ )
      {
         vararg[k] = exchvar[i]; k++;
         for ( j=0; j<exchval[i]->NumVarArg(); j++ )
         {
            vararg[k] = exchval[i]->VarArg(j); k++;
         }
         for ( j=0; j<exchfac[i]->NumVarArg(); j++ )
         {
            vararg[k] = exchfac[i]->VarArg(j); k++;
         }
      }
      if ( k != numvararg )
         FatalError("SERZONE::CalcVarArg: inconsistent number of arguments");
   }
   return;
}


JOBSTATUS SERZONE::Load(std::istream& in, const VARLIST* varlist)
{
   char Buffer1[BufSize]; char Buffer2[BufSize];
   char Buffer3[BufSize]; char Buffer4[BufSize];

   if ( LoadStart(in) == FALSE ) return PROBLEM;

   CARDINAL n;
   CARDINAL numerrfac, numerrval;
   if (LoadNumber (in, &n)   == FALSE )                         return PROBLEM;
   if (ZoneIndex (n) == FALSE )                                 return PROBLEM;

   if (LoadString(in, Buffer1, BufSize) == FALSE)               return PROBLEM;
   if (VolumeFract(Buffer1, varlist, Buffer2, n) == FALSE )     return PROBLEM;


   if (LoadStart(in) == FALSE)                                  return PROBLEM;

   while (LoadString (in, Buffer1, BufSize) == TRUE )
   {
      AQVAR* var = varlist -> Get(Buffer1);
      if (var == 0)                                             return PROBLEM;
      if (LoadString (in, Buffer1, BufSize)   == FALSE )        return PROBLEM;
      if (LoadString (in, Buffer3, BufSize)   == FALSE )        return PROBLEM;
   
      if ( AddExchCoeff ( varlist,var,Buffer1,Buffer2,numerrval,
                          Buffer3,Buffer4,numerrfac) == FALSE ) return PROBLEM;
   }

   while (LoadStart(in) == TRUE )
   {
       if ( LoadEnd(in) == FALSE ) return PROBLEM;
   }
   return OK;
}


JOBSTATUS SERZONE::Save(std::ostream& out)
{
   if ( SaveStart(out) == FALSE ) return PROBLEM;

   char Buffer[BufSize];
   const INIT* ini =  AQCOMP::Ini();                      

   CARDINAL i;

   if ( SaveNumber(out, zind)  == FALSE)                        return PROBLEM;

   if ( volfract->UnParse(Buffer,sizeof(Buffer)-1) !=0)         return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                       return PROBLEM;

   if ( SaveStart(out) == FALSE )                               return PROBLEM;
   for ( i= 0; i < numexch; i++ )
   { 
     if (SaveString(out,( exchvar[i]) -> Symbol()) == FALSE )   return PROBLEM;
     if ( (exchval[i])->UnParse(Buffer,sizeof(Buffer)-1) != 0 ) return PROBLEM;
     if (SaveString(out, Buffer) == FALSE)                      return PROBLEM; 
     if ( (exchfac[i])->UnParse(Buffer,sizeof(Buffer)-1) != 0 ) return PROBLEM;
     if (SaveString(out, Buffer) == FALSE )                     return PROBLEM;
   }                 
   if ( SaveEnd(out) == FALSE )                                 return PROBLEM;

   if ( SaveEnd(out) == FALSE )                                 return PROBLEM;
   return OK;
}


JOBSTATUS SERZONE::Write(std::ostream& out, BOOLEAN)
{
   const INIT* ini = AQCOMP::Ini();

   CARDINAL pos1 = ini->L(1)+1; 
   CARDINAL pos2 = pos1+ini->L(2);
   CARDINAL pos3 = pos1+2;
   CARDINAL pos4 = pos3+2;
   CARDINAL pos5 = pos4+2;
   CARDINAL pos6 = pos5+2;
   CARDINAL pos7 = pos6+2;

   PrintIndent(pos2-1);
   char Buffer[BufSize];
   CARDINAL i;

   if ( Print(out,ini->T(128),pos5) == FALSE )                return PROBLEM;
   if ( Print(out,":") == FALSE )                             return PROBLEM;
   if ( Print(out,ZoneIndex(),pos2) == FALSE )                return PROBLEM;

   if (Print(out,ini->T(127),pos6) == FALSE )                 return PROBLEM;
   if ( Print(out,":") == FALSE )                             return PROBLEM;
   if ( volfract->UnParse(Buffer,sizeof(Buffer)-1) != 0 )     return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                     return PROBLEM;

   if ( Print(out,ini->T(123),pos6) == FALSE )                return PROBLEM;
   if ( Print(out,":") == FALSE )                             return PROBLEM;
   if ( NumExch() > 0 )
   {
      PrintIndent(pos4-1);
      if ( Print(out,ini->T(124),pos7) == FALSE )             return PROBLEM;
      if ( Print(out," : ") == FALSE )                        return PROBLEM;
      if ( Print(out,ini->T(125)) == FALSE )                  return PROBLEM;
      if ( Print(out,", ") == FALSE )                         return PROBLEM;
      if ( Print(out,ini->T(126)) == FALSE )                  return PROBLEM;
      for ( i=0; i<NumExch(); i++ )
      {
         if ( Print(out,(exchvar[i])->Symbol(),pos7) == FALSE )
                                                              return PROBLEM;
         if ( Print(out," : ") == FALSE )                     return PROBLEM;
         if ( (exchval[i])->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                              return PROBLEM;
         if ( Print(out,Buffer) == FALSE )                    return PROBLEM;
         if ( Print(out,", ") == FALSE )                      return PROBLEM;
         if ( (exchfac[i])->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                              return PROBLEM;
         if ( Print(out,Buffer) == FALSE )                    return PROBLEM;
      }
   }

   return OK;
}


//////////////////////////////////////////////////////////////////////////////


void PARZONE::init()

{
   numserialzones = 0;
   zones          = 0;

   numvararg = 0; 
   vararg = 0;

   return;
}


PARZONE::PARZONE()
{
   init();
}


PARZONE::PARZONE(const PARZONE& parallel) : SYMBOL(parallel)
{
   init();

   CARDINAL i;
   numserialzones = parallel.numserialzones;
   if ( numserialzones > 0 )
   {
      zones = new SERZONE*[numserialzones];
      for ( i=0; i<numserialzones; i++ )
      {
         zones[i] = new SERZONE(parallel.zones[i]);
      }
   }
   CalcVarArg();
}


PARZONE::PARZONE(const PARZONE* parallel) : SYMBOL(parallel)
{
   init();

   CARDINAL i;
   if ( parallel != 0 )
   {
      numserialzones = parallel->numserialzones;
      if ( numserialzones > 0 )
      {
         zones = new SERZONE*[numserialzones];
         for ( i=0; i<numserialzones; i++ )
         {
            zones[i] = new SERZONE(parallel->zones[i]);
         }
      }
      CalcVarArg();
   }
}


PARZONE::~PARZONE()
{
   CARDINAL i;

   for ( i=0; i<numserialzones; i++ )
   {
      delete zones[i];
   }
   delete [] zones; zones = 0; numserialzones = 0;

   delete vararg; vararg = 0; numvararg = 0;
}



BOOLEAN PARZONE::AllowedProgVarType(PROGVARTYPE type) const
{
   if ( type == CalcNumber )    return TRUE;
   if ( type == CompIndex )     return TRUE;
   if ( type == ZoneIndex )     return TRUE;
   if ( type == Time )          return TRUE;
   if ( type == WaterFraction ) return TRUE;
   if ( type == SpaceX )        return TRUE;
   if ( type == Discharge )     return TRUE;
   if ( type == CrossSection )  return TRUE;
   return FALSE;
}


BOOLEAN PARZONE::AllowedVar(const AQVAR* var) const
{
   if ( var->Type() == ProgVar )
   {
      PROGVAR* pv = (PROGVAR*)var;
      if ( AllowedProgVarType(pv->ProgVarType()) == FALSE ) return FALSE;
   }
   for ( CARDINAL i=0; i<var->NumVarArg(); i++ )
   {
      if ( AllowedVar(var->VarArg(i)) == FALSE ) return FALSE;
   }
   return TRUE;
}


BOOLEAN PARZONE::AllowedReplaceVar(const AQVAR* oldvar,
                                   const AQVAR* newvar) const
{
   if ( Arg(oldvar) == TRUE )
   {
      if ( AllowedVar(newvar) == FALSE ) return FALSE;
   }
   return TRUE;
}


BOOLEAN PARZONE::AllowedExchangeVar(const AQVAR* var1,
                                    const AQVAR* var2) const
{
   if ( Arg(var1) == TRUE )
   {
      if ( AllowedVar(var2) == FALSE ) return FALSE;
   }
   if ( Arg(var2) == TRUE )
   {
      if ( AllowedVar(var1) == FALSE ) return FALSE;
   }
   return TRUE;
}


SERZONE* PARZONE::SerialZone(CARDINAL index)
{
   if ( index > numserialzones )
      FatalError("PARZONE::SerialZone: illegal index");
   return zones[index];
}


BOOLEAN PARZONE::AddSerial(SERZONE* zone, CARDINAL pos)
{
   CARDINAL i;
   if ( InList() == TRUE )             return FALSE;
   if ( pos > numserialzones ) pos = numserialzones;

   SERZONE** newzones = new SERZONE*[numserialzones+1];
   for ( i=0; i<pos; i++ )
   {
      newzones[i] = zones[i];
   }
   newzones[pos] = zone;
   for ( i=pos; i<numserialzones; i++ )
   {
      newzones[i+1] = zones[i];
   }
   delete zones; zones = newzones;
   numserialzones++;
   CalcVarArg();
   return TRUE;
}


BOOLEAN PARZONE::ReplaceSerial(SERZONE* zone, CARDINAL pos)
{
   if ( InList() == TRUE )      return FALSE;
   if ( pos >= numserialzones ) return FALSE;
   delete zones[pos];
   zones[pos] = zone;
   CalcVarArg();
   return TRUE;
}


BOOLEAN PARZONE::DeleteSerial(CARDINAL pos)
{
   CARDINAL i;
   if ( pos >= numserialzones ) return FALSE;

   SERZONE** newzones = new SERZONE*[numserialzones-1];

   for ( i=0; i<pos; i++ )
   {
      newzones[i] = zones[i];
   }
   for ( i=pos+1; i<numserialzones; i++ )
   {
      newzones[i-1] = zones[i];
   }
   delete zones[pos];
   delete [] zones; zones = newzones;
   numserialzones--;
   CalcVarArg();
   return TRUE;
}


AQVAR* PARZONE::VarArg(CARDINAL index) const
{
   if ( index >= numvararg )
   {
      FatalError("PARZONE::VarArg: index too large");
   }
   else
   {
      return vararg[index];
   }
   return 0;
}


BOOLEAN PARZONE::Arg(const AQVAR* var) const
{
   for ( CARDINAL i=0; i<numvararg; i++ )
   {
      if ( vararg[i] == var )              return TRUE;
      if ( (vararg[i])->Arg(var) == TRUE ) return TRUE;
   }
   return FALSE;
}


void PARZONE::ReplVar(AQVAR* oldvar, AQVAR* newvar)
{
   CARDINAL i;
   for ( i=0; i<numserialzones; i++ )
   {
      zones[i]->ReplaceVar(oldvar,newvar);
   }
   CalcVarArg();
   return;
}


BOOLEAN PARZONE::ReplaceVar(AQVAR* oldvar, AQVAR* newvar)
{
   if ( InList() == TRUE ) return FALSE;
   ReplVar(oldvar,newvar); return TRUE;
}


void PARZONE::ExchVar(AQVAR* var1, AQVAR* var2)
{
   CARDINAL i;
   for ( i=0; i<numserialzones; i++ )
   {
      zones[i]->ExchangeVar(var1,var2);
   }
   CalcVarArg();
   return;
}


BOOLEAN PARZONE::ExchangeVar(AQVAR* var1, AQVAR* var2)
{
   if ( InList() == TRUE ) return FALSE;
   ExchVar(var1,var2);    return TRUE;
}


void PARZONE::CalcVarArg()
{
   CARDINAL j;
   numvararg = 0;
   for ( j=0; j<numserialzones; j++ )
   {
      numvararg += zones[j]->NumVarArg();
   }

   if ( numvararg > 0 )
   {
      delete [] vararg;
      vararg = new AQVAR*[numvararg];
      CARDINAL k = 0;
      CARDINAL i;
      for ( i=0; i<numserialzones; i++ )
      {
         for ( j=0; j<zones[i]->NumVarArg(); j++ )
         {
            vararg[k] = zones[i]->VarArg(j); k++;
         }
      }
      if ( k != numvararg )
         FatalError("PARZONE::CalcVarArg: inconsistent number of arguments");
   }
   return;
}


JOBSTATUS PARZONE::Load(std::istream& in, const VARLIST* varlist)
                    
		    
{

    char Buffer[BufSize];

    if (LoadStart(in) == FALSE)                            return PROBLEM;

    if (LoadString (in, Buffer, sizeof(Buffer)) == FALSE ) return PROBLEM;
    if (Symbol(Buffer) == FALSE )                          return PROBLEM;

    if (LoadString (in, Buffer, sizeof(Buffer)) == FALSE ) return PROBLEM;
    Description (Buffer);

   if (LoadStart(in) == FALSE)                             return PROBLEM;
   while (1) 
   {
      SERZONE* serzone = new SERZONE ;
      if ((serzone->Load(in, varlist) == PROBLEM))
      {
	 delete serzone;
	 break;
      }
      if (AddSerial(serzone) == FALSE)
      {
	 delete serzone;
	 return PROBLEM;
      }
   }

   while (LoadStart(in) == TRUE )
   {
       if ( LoadEnd(in) == FALSE ) return PROBLEM;
   }
   return OK;

}


JOBSTATUS PARZONE::Save(std::ostream& out)
{
   if ( SaveStart(out) == FALSE )                return PROBLEM;

   const INIT* ini = AQCOMP::Ini();
   if ( SaveString(out,Symbol()) == FALSE )      return PROBLEM;
   if ( SaveString(out,Description()) == FALSE ) return PROBLEM;

   if ( SaveStart(out) == FALSE )                return PROBLEM;
   for ( CARDINAL i=0; i<numserialzones; i++ )
   {
      if ( zones[i]->Save(out) == PROBLEM )      return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                  return PROBLEM;

   if ( SaveEnd(out) == FALSE )                  return PROBLEM;
   return FILEIO::Save(out);
}


JOBSTATUS PARZONE::Write(std::ostream& out, BOOLEAN)
{
   const INIT* ini = AQCOMP::Ini();

   CARDINAL pos1 = ini->L(1)+1; 
   CARDINAL pos2 = pos1+ini->L(2);
   CARDINAL pos3 = pos1+2;
   CARDINAL pos4 = pos3+2;
   PrintIndent(pos2-1);

   if ( Print(out,Symbol(),pos3) == FALSE )      return PROBLEM;
   if ( Print(out,":") == FALSE )                return PROBLEM;

   if ( Print(out,Description(),pos2) == FALSE ) return PROBLEM;

   if ( Print(out,ini->T(129),pos4) == FALSE )   return PROBLEM;
   if ( Print(out,":") == FALSE )                return PROBLEM;

   for ( CARDINAL i=0; i<NumSerialZones(); i++ )
   {
      SerialZone(i)->Write(out);
   }

   return OK;
}


////////////////////////////////////////////////////////////////////////////////

CARDINAL SOILCOMP::ioff = 1;

void SOILCOMP::init()
{
   numinit = 0; initzone = 0; initvar = 0; initval = 0;

   inflow = 0;
   numinput = 0; inputvar = 0; inputflux = 0;
   latinflow = 0;
   numlatinput = 0; latinputvar = 0; latinputconc = 0;
   latinflow = 0;
   numlatinput = 0; latinputvar = 0; latinputconc = 0;

   Xs = ini.R(15);
   Xe = ini.R(16);
   gridpts = 0; SetGridPts(12); SetGridPts(ini.I(1));
   A = 0;
   advvolfract = 0;

   numserzones = 0;
   numparzones = 0;
   numzones    = 1;

   relaccQ = 1.0e-4;
   absaccQ = 1.0e-6;
   relaccD = 1.0E-6;
   absaccD = 1.0E-6;

   E = 0;
   withdispersion = FALSE;
   highres  = FALSE;
   g = 9.81;
   numCout = 0;
   Cout    = 0;
   numadvinconn = 1;
   numadvoutconn = 1;
   numdiffconn = 0;
   if ( NumAdvInConn() == 0 )
   {
      advinnum = 0; advinindex = 0; advinlink = 0;
   }
   else
   {
      advinnum   = new CARDINAL[NumAdvInConn()];
      advinindex = new CARDINAL*[NumAdvInConn()];
      advinlink  = new ADVLINK**[NumAdvInConn()];
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
      difflink  = new DIFFLINK**[NumDiffConn()];
      for ( CARDINAL i=0; i<NumDiffConn(); i++ )
      {
         diffnum[i]   = 0;
         diffindex[i] = 0;
         difflink[i]  = 0;
      }
   }
   preendcrosssect = 0.0;
   endcrosssect    = 0.0;
   return;
}


// ============================================================================

void SOILCOMP::del()
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
   delete latinflow; latinflow = 0;
   delete latinputvar; latinputvar = 0;
   for ( i=0; i<numlatinput; i++ ) delete latinputconc[i];
   delete latinputconc; latinputconc = 0; numlatinput = 0;

   delete gridpts; gridpts = 0;
   delete A;  A  = 0;

   delete advvolfract; advvolfract = 0;
   delete E;  E  = 0;
   delete [] Cout; Cout = 0;

   PARZONE* zone = parzonelist.First();
   while ( zone != 0 )
   {
      PARZONE* z = zone;
      zone = zone->Next();
      parzonelist.Remove(z);
      delete z;
   }

   return;
}


// ============================================================================

SOILCOMP::SOILCOMP()
{
   init();
}


// ============================================================================

SOILCOMP::SOILCOMP(const SOILCOMP& com) : AQCOMP(com)  // Copy-Konstructor
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
   relaccD = com.relaccD;
   absaccD = com.absaccD;

   Xs = com.Xs;
   Xe = com.Xe;
   numgrid = com.numgrid;
   if ( gridpts != 0 ) delete [] gridpts;
   gridpts = new REAL[numgrid];
   for ( CARDINAL i=0; i<numgrid; i++ ) gridpts[i] = com.gridpts[i];
   latinflow = new FORMVAR(com.latinflow);
   if ( com.numlatinput > 0 )
   {
      numlatinput = com.numlatinput;
      latinputvar  = new AQVAR*[numlatinput];
      latinputconc = new FORMVAR*[numlatinput];
      for ( CARDINAL i=0; i<numlatinput; i++ )
      {
         latinputvar[i] = com.latinputvar[i];
         latinputconc[i] = new FORMVAR(com.latinputconc[i]);
      }
   }
   A  = new FORMVAR(com.A);

   advvolfract = new FORMVAR(com.advvolfract);
   if ( com.E != 0 ) E = new FORMVAR(com.E);  // noch ueberlegen!
   withdispersion = com.withdispersion;
   highres = com.highres;
   PARZONE* zone = com.parzonelist.First();
   while ( zone != 0 )
   {
      PARZONE* z = new PARZONE(zone);
      parzonelist.Add(z);
      zone = zone->Next();
   }
   Num_of_Zones();
}


// ============================================================================

SOILCOMP::SOILCOMP(const AQCOMP* com) : AQCOMP(com)  // Copy-Constructor
{
   init();
   if ( com != 0 )
   {
      if ( com->Type() == SoilComp )
      {
         SOILCOMP* sc = (SOILCOMP*)com;

         if ( sc->numinit > 0 )
         {
            numinit = sc->numinit;
            initzone = new CARDINAL[numinit];
            initvar  = new AQVAR*[numinit];
            initval  = new FORMVAR*[numinit];
            for ( CARDINAL i=0; i<numinit; i++ )
            {
               initzone[i] = sc->initzone[i];
               initvar[i]  = sc->initvar[i];
               initval[i]  = new FORMVAR(sc->initval[i]);
            }
         }

         inflow = new FORMVAR(sc->inflow);
         if ( sc->numinput > 0 )
         {
            numinput = sc->numinput;
            inputvar = new AQVAR*[numinput];
            inputflux = new FORMVAR*[numinput];
            for ( CARDINAL i=0; i<numinput; i++ )
            {
               inputvar[i] = sc->inputvar[i];
               inputflux[i] = new FORMVAR(sc->inputflux[i]);
            }
         }

         relaccQ = sc->relaccQ;
         absaccQ = sc->absaccQ;
         relaccD = sc->relaccD;
         absaccD = sc->absaccD;

         Xs = sc->Xs;
         Xe = sc->Xe;
         numgrid = sc->numgrid;
         if ( gridpts != 0 ) delete [] gridpts;
         gridpts = new REAL[numgrid];
         for ( CARDINAL i=0; i<numgrid; i++ ) gridpts[i] = sc->gridpts[i];
         latinflow = new FORMVAR(sc->latinflow);
         if ( sc->numlatinput > 0 )
         {
            numlatinput = sc->numlatinput;
            latinputvar  = new AQVAR*[numlatinput];
            latinputconc = new FORMVAR*[numlatinput];
            for ( CARDINAL i=0; i<numlatinput; i++ )
            {
               latinputvar[i] = sc->latinputvar[i];
               latinputconc[i] = new FORMVAR(sc->latinputconc[i]);
            }
         }
         A  = new FORMVAR(sc->A);

	 advvolfract = new FORMVAR(sc->advvolfract);
         if ( sc->E != 0 ) E = new FORMVAR(sc->E);
         withdispersion = sc->withdispersion;
         highres = sc->highres;
         PARZONE* zone = (sc->parzonelist).First();
         while ( zone != 0 )
         {
            PARZONE* z = new PARZONE(zone);
            parzonelist.Add(z);
            zone = zone->Next();
         }
      }
   }
   Num_of_Zones();
}


// ============================================================================


SOILCOMP::~SOILCOMP()
{
   del();
}


// ============================================================================

const char* SOILCOMP::TypeName() const
{
   return ini.T(6);
}

// ============================================================================


AQVAR* SOILCOMP::SpaceVar(const VARSYS* varsys) const
{
   return varsys->GetProgVar(SpaceX);
}


// ============================================================================

BOOLEAN SOILCOMP::AllowedSpaceVal(REAL x) const
{
   if ( (x<gridpts[0]) && (x<gridpts[numgrid-1]) ) return FALSE;
   if ( (x>gridpts[0]) && (x>gridpts[numgrid-1]) ) return FALSE;
   return TRUE;
}


// ============================================================================

BOOLEAN SOILCOMP::AllowedProgVarType(PROGVARTYPE type) const
{
   if ( type == CalcNumber )    return TRUE;
   if ( type == CompIndex )     return TRUE;
   if ( type == ZoneIndex )     return TRUE;
   if ( type == Time )          return TRUE;
   if ( type == WaterFraction ) return TRUE;
   if ( type == SpaceX )        return TRUE;
   if ( type == Discharge )     return TRUE;
   if ( type == CrossSection )  return TRUE;
   return FALSE;
}


BOOLEAN SOILCOMP::AllowedComp() const
{
   if ( A==0 )     return FALSE;       // Evtl. noch fuer Theta

   if ( A->ArgVarType(StateVar) == TRUE )    return FALSE;

   if ( advvolfract==0 )    return FALSE;

   if (advvolfract->ArgVarType(StateVar ) == TRUE )   return FALSE;

   if ( E != 0 )
   {
      if ( E->ArgVarType(StateVar) == TRUE ) return FALSE;
   }

   return TRUE;
}



// ============================================================================

BOOLEAN SOILCOMP::AllowedReplaceVar(const AQVAR* oldvar,
                                    const AQVAR* newvar) const
{
   if ( AQCOMP::AllowedReplaceVar(oldvar,newvar) == FALSE )   return FALSE;
   if ( A->Arg(oldvar) == TRUE )
   { 
      if ( newvar->ArgVarType(StateVar) == TRUE )             return FALSE;
   }

   if (advvolfract->Arg(oldvar) == TRUE )             
   { 
      if ( newvar->ArgVarType(StateVar) == TRUE )              return FALSE;
   }

   if ( E != 0 )
   {
      if ( E->Arg(oldvar) == TRUE )
      {
         if ( newvar->ArgVarType(StateVar) == TRUE )          return FALSE;
      }
   }
   return TRUE;
}


// ============================================================================

BOOLEAN SOILCOMP::AllowedExchangeVar(const AQVAR* var1,
                                     const AQVAR* var2) const
{
   if ( AQCOMP::AllowedExchangeVar(var1,var2) == FALSE )    return FALSE;
   if ( (A->Arg(var1)==TRUE) && (A->Arg(var2)==FALSE) )
   if ( (advvolfract->Arg(var1)==TRUE) && (advvolfract->Arg(var2)==FALSE) )
   {
      if ( var2->ArgVarType(StateVar) == TRUE )             return FALSE;
   }
   if ( (A->Arg(var1)==FALSE) && (A->Arg(var2)==TRUE) )
   if ( (advvolfract->Arg(var1)==FALSE) && (advvolfract->Arg(var2)==TRUE) )
   {
      if ( var1->ArgVarType(StateVar) == TRUE )             return FALSE;
   }

   if ( E != 0 )
   {
      if ( (E->Arg(var1)==TRUE) && (E->Arg(var2)==FALSE) )
      {
         if ( var2->ArgVarType(StateVar) == TRUE )          return FALSE;
      }
      if ( (E->Arg(var1)==FALSE) && (E->Arg(var2)==TRUE) )
      {
         if ( var1->ArgVarType(StateVar) == TRUE )          return FALSE;
      }
   }
   return TRUE;
}


// ============================================================================

void SOILCOMP::ReplVar(AQVAR* oldvar, AQVAR* newvar)
{
   CARDINAL i;

   if ( AllowedReplaceVar(oldvar,newvar) == FALSE )
      FatalError("SOILCOMP::ReplVar: Illegal replace");
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
   if ( latinflow != 0 ) latinflow->ReplaceVarArg(oldvar,newvar);
   for ( i=0; i<numlatinput; i++ )
   {
      if ( latinputvar[i] == oldvar ) latinputvar[i] = newvar;
      latinputconc[i]->ReplaceVarArg(oldvar,newvar);
   }
   A->ReplaceVarArg(oldvar,newvar);

   advvolfract->ReplaceVarArg(oldvar,newvar);

   if ( E != 0 ) E->ReplaceVarArg(oldvar,newvar);

   PARZONE* parzone = parzonelist.First();
   while ( parzone != 0 )
   {
      parzone->ReplVar(oldvar,newvar);
      parzone = parzone->Next();
   }

   CalcArg();
   return;
}



// ============================================================================

void SOILCOMP::ExchVar(AQVAR* var1, AQVAR* var2)
{
   CARDINAL i;

   if ( AllowedExchangeVar(var1,var2) == FALSE )
      FatalError("SOILCOMP::ExchVar: Illegal exchange");
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
   if ( latinflow != 0 )
   {
      latinflow->ReplaceVarArg(var1,v);
      latinflow->ReplaceVarArg(var2,var1);
      latinflow->ReplaceVarArg(v,var2);
   }
   for ( i=0; i<numlatinput; i++ )
   {
      if ( latinputvar[i] == var1 )
      {
         latinputvar[i] = var2;
      }
      else
      {
         if ( latinputvar[i] == var2 ) latinputvar[i] = var1;
      }
      latinputconc[i]->ReplaceVarArg(var1,v);
      latinputconc[i]->ReplaceVarArg(var2,var1);
      latinputconc[i]->ReplaceVarArg(v,var2);
   }
   A->ReplaceVarArg(var1,v);
   A->ReplaceVarArg(var2,var1);
   A->ReplaceVarArg(v,var2);

   advvolfract->ReplaceVarArg(var1,v);
   advvolfract->ReplaceVarArg(var2,var1);
   advvolfract->ReplaceVarArg(v,var2);

   if ( E != 0 )
   {
      E->ReplaceVarArg(var1,v);
      E->ReplaceVarArg(var2,var1);
      E->ReplaceVarArg(v,var2);
   }
   delete v;

   PARZONE* parzone = parzonelist.First();
   while ( parzone != 0 )
   {
      parzone->ExchVar(var1,var2);
      parzone = parzone->Next();
   }

   CalcArg();
   return;
}



// ============================================================================

BOOLEAN SOILCOMP::LatInflow(const VARLIST* varlist,
                           const char* inpline,
                           char* errorline)
{
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("LatInflow");
   if ( formvar->Parse(inpline,varlist,errorline) != 0 )
   {
      delete formvar; return FALSE;
   }
   delete latinflow; latinflow = formvar;
   CalcArg();
   return TRUE;
}


// ============================================================================

AQVAR* SOILCOMP::LatInputVar(CARDINAL index) const
{
   if ( index >= numlatinput )
      FatalError("SOILCOMP::LatInputVar: Illegal index");
   return latinputvar[index];
}


// ============================================================================

const FORMVAR* SOILCOMP::LatInputConc(CARDINAL index) const
{
   if ( index >= numlatinput )
      FatalError("SOILCOMP::LatInputConc: Illegal index");
   return latinputconc[index];
}


// ============================================================================

BOOLEAN SOILCOMP::AddLatInput(const VARLIST* varlist, AQVAR* var,
                             const char* inpline, char* errorline,
                             CARDINAL& numparseerrors, CARDINAL pos)
{
   numparseerrors = 0;
   if ( InList() == TRUE )             return FALSE;
   if ( inpline == 0 )                 return FALSE;
   if ( inpline[0] == '\0' )           return FALSE;
   if ( varlist->Exist(var) == FALSE ) return FALSE;
   if ( pos > numlatinput ) pos = numlatinput;
   CARDINAL i;
   for ( i=0; i<numlatinput; i++ )
   {
      if ( latinputvar[i] == var )     return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("LatInput");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                  return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                  return FALSE;
   }
   AQVAR**   newlatvar  = new AQVAR*[numlatinput+1];
   FORMVAR** newlatconc = new FORMVAR*[numlatinput+1];
   for ( i=0; i<pos; i++ )
   {
      newlatvar[i]  = latinputvar[i];
      newlatconc[i] = latinputconc[i];
   }
   newlatvar[pos]  = var;
   newlatconc[pos] = formvar;
   for ( i=pos; i<numlatinput; i++ )
   {
      newlatvar[i+1]  = latinputvar[i];
      newlatconc[i+1] = latinputconc[i];
   }
   delete latinputvar;  latinputvar  = newlatvar;
   delete latinputconc; latinputconc = newlatconc;
   numlatinput++;
   CalcArg();
   return TRUE;
}


// ============================================================================

BOOLEAN SOILCOMP::ReplaceLatInput(const VARLIST* varlist, AQVAR* var,
                                  const char* inpline, char* errorline,
                                  CARDINAL& numparseerrors, CARDINAL pos)
{
   numparseerrors = 0;
   if ( InList() == TRUE )                 return FALSE;
   if ( inpline == 0 )                     return FALSE;
   if ( inpline[0] == '\0' )               return FALSE;
   if ( varlist->Exist(var) == FALSE )     return FALSE;
   if ( pos >= numlatinput )               return FALSE;
   CARDINAL i;
   for ( i=0; i<numlatinput; i++ )
   {
      if ( i!=pos && latinputvar[i]==var ) return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("LatInput");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                      return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                      return FALSE;
   }
   delete latinputconc[pos];
   latinputvar[pos]  = var;
   latinputconc[pos] = formvar;
   CalcArg();
   return TRUE;
}



// ============================================================================

BOOLEAN SOILCOMP::DeleteLatInput(CARDINAL pos)
{
   if ( pos >= numlatinput ) return FALSE;
   AQVAR**   newlatvar  = new AQVAR*[numlatinput-1];
   FORMVAR** newlatconc = new FORMVAR*[numlatinput-1];
   CARDINAL i;
   for ( i=0; i<pos; i++ )
   {
      newlatvar[i]  = latinputvar[i];
      newlatconc[i] = latinputconc[i];
   }
   for ( i=pos+1; i<numlatinput; i++ )
   {
      newlatvar[i-1]  = latinputvar[i];
      newlatconc[i-1] = latinputconc[i];
   }
   delete latinputconc[pos];
   delete latinputvar;  latinputvar  = newlatvar;
   delete latinputconc; latinputconc = newlatconc;
   numlatinput--;
   CalcArg();
   return TRUE;
}


// --------------------------------------------------------------------

 void SOILCOMP::Num_of_Zones() 
{
   numparzones = parzonelist.Size();
   delete numserzones;
   numserzones = new CARDINAL[numparzones];
   numzones    = 1;

   PARZONE* parzone = parzonelist.First();
   CARDINAL i = 0;

   while (parzone != 0)
   {
      if (i >= numparzones) FatalError("Illegal number of parallel zones!");

      numserzones[i] = parzone -> NumSerialZones();  // i++;
      parzone = parzone -> Next();
      numzones += numserzones[i];

      i++;
   }
}


// ============================================================================

CARDINAL SOILCOMP::NumEq() const
{
   return NumGridPts()*(ioff + numstatevar*numzones) + 1;
}


// ============================================================================

REAL SOILCOMP::RelAcc(CARDINAL i) const
{
   if ( i >= NumEq() ) FatalError("SOILCOMP::RelAcc: Illegal index");
   if ( i == NumEq()-1 ) return relaccD;

   CARDINAL index = i % (ioff+numstatevar*numzones);
     
  
   if ( index == 0 ) return relaccQ;

   CARDINAL ind   = (index-ioff) % numstatevar;

   return statevar[ind]->RelAccuracy();
}



// ============================================================================

REAL SOILCOMP::AbsAcc(CARDINAL i) const
{
   if ( i >= NumEq() ) FatalError("SOILCOMP::AbsAcc: Illegal index");
   if ( i == NumEq()-1 ) return absaccD;

  
   CARDINAL index = i % (ioff+numstatevar*numzones);
   if ( index == 0 ) return absaccQ;

   CARDINAL ind   = (index-ioff) % numstatevar;

   return statevar[ind]->AbsAccuracy();
}



// ============================================================================

EQTYPE SOILCOMP::EqType(CARDINAL i) const
{
   if ( i >= NumEq() ) FatalError("SOILCOMP::EqType: Illegal index");
   if ( i == NumEq()-1 ) return ALG;

   CARDINAL index = i % (ioff+numstatevar*numzones);

   if ( index == 0 ) return ALG;        // Q
         
   CARDINAL ind   = (index-ioff) % numstatevar;

   // EquStatevar and SurfStatevar for all grid points:
   if ( ind   >= numdynstatevar )   return ALG;  // EquStatevar 
   if ( ind   >= numvolstatevar )   return DIFF; // SurfStatevar 

   // VolStateVar:
   if ( i < ioff+numstatevar*numzones )   // first grid point            
   {
      if ( i < ioff+numstatevar )   // advective zone
      {
         return ALG;
      }
      else                          // stagnant zones
      {
         return DIFF;
      }
   }
   else
   {
      if ( i >= (numgrid-1)*(ioff+numstatevar*numzones) ) // last grid point
      {
         if ( WithDispersion() == TRUE )      // VolStatevar
         {
            return ALG;
         }
         else
         {
            return DIFF;
         }
      }
   }

   // other grid points:

   return DIFF;    // VolStatevar
}


// ============================================================================

BOOLEAN SOILCOMP::InitCond(VARSYS* varsys, REAL* Y, CARDINAL callnum)
{
   if ( Active() != TRUE ) return FALSE;
   REAL compindex = (REAL)cind;
   REAL zoneindex = 0;
   varsys->SetProgVar(CompIndex,&compindex);
   varsys->SetProgVar(ZoneIndex,&zoneindex);
   Xvar = varsys->GetProgVar(SpaceX);
   Avar = varsys->GetProgVar(CrossSection);
   Qvar = varsys->GetProgVar(Discharge);

 
// ----------------------------------------------------------------------------
//                          Advektiver Wasserkanal
// ----------------------------------------------------------------------------

   if ( callnum>0 ) return TRUE;
   REAL* areas = new REAL[numgrid];
   CARDINAL cell = ioff+numstatevar*numzones;

   REAL* y = &Y[ind_y];
   CARDINAL i,j,k;

   BOOLEAN found = FALSE;                  // Discharge Q
   for ( j=0; j<numinit; j++ )
   {
      if ( initvar[j] == Qvar && initzone[j] == 0 )
      {
         found = TRUE; k = j; break;
      }
   }
   if ( found == TRUE )
   {
      for ( j=0; j<NumGridPts(); j++ )
      {
         varsys->Reset();
         if ( Xvar != 0 ) Xvar->SetProgVar(&gridpts[j]);
         y[j*cell] = initval[k]->Value();
         if ( y[j*cell] < 0.0 )
         {
            *cjob << "\n   SOILCOMP numeric problem:"
                  << " Initial discharge is negative";
            cjob->flush();
            return FALSE;
         }
      }
   }
   else
   {
      for ( j=0; j<NumGridPts(); j++ )
      {
         y[j*cell] = 0.1;
      }
   }

   REAL** zonecrosssect;
   zonecrosssect = new REAL* [numgrid]; 
   for ( j=0; j<NumGridPts(); j++ )
   {
      zonecrosssect[j] = new REAL [numzones];
      varsys->Reset();
      if ( Xvar != 0 ) Xvar->SetProgVar(&gridpts[j]);
      if ( Qvar != 0 ) Qvar->SetProgVar(&y[j*cell]);
      if ( Avar != 0 ) Avar->SetProgVar(&areas[j]);
      areas[j] = A->Value();
      zonecrosssect[j][0] = advvolfract->Value()*areas[j];
      if ( zonecrosssect[j][0] <= 0.0 )
      {
         *cjob << "\n   SOILCOMP numeric problem:"
               << " Advective zone cross section is not positive";
         cjob->flush();
         return FALSE;
      }

      CARDINAL z=1;
      PARZONE* parzone = parzonelist.First();
      while (parzone != 0) 
      { 
         for ( CARDINAL sz=0; sz<parzone->NumSerialZones(); sz++)
         {
            SERZONE* serial = parzone->SerialZone(sz);
            zonecrosssect[j][z] = areas[j]*(serial->volfract) -> Value();
            if ( zonecrosssect[j][0] <= 0.0 )
            {
               *cjob << "\n   SOILCOMP numeric problem:"
                     << " Advective zone cross section is not positive";
               cjob->flush();
               return FALSE;
            }
            z++;
         }
         parzone = parzone -> Next();
      }
   }
   preendcrosssect = zonecrosssect[NumGridPts()-2][0];
   endcrosssect    = zonecrosssect[NumGridPts()-1][0];

   for ( i=0; i<numstatevar; i++ )            // Concentrations C
   {
      BOOLEAN found = FALSE;
      for ( j=0; j<NumInitCond(); j++ )
      {
         if ( initvar[j]==statevar[i] && initzone[j]==0 )
         {
            k = j; found = TRUE; break;
         }
      }
      for ( j=0; j<NumGridPts(); j++ )
      {
         if ( found == TRUE )
         {
            varsys->Reset();
            if ( Xvar != 0 ) Xvar->SetProgVar(&gridpts[j]);
            if ( Qvar != 0 ) Qvar->SetProgVar(&y[j*cell]);
            if ( Avar != 0 ) Avar->SetProgVar(&areas[j]);

            y[j*cell+ioff+i] = initval[k]->Value();
         }
         else
         {
            y[j*cell+ioff+i] = 0.0;
         }           
      }
   }


// ----------------------------------------------------------------------------
//     Stagnierende Zonen
// ----------------------------------------------------------------------------

  

   PARZONE* parzone = parzonelist.First();

   CARDINAL z=1;  

   while (parzone != 0)
   {
      for ( CARDINAL sz=0; sz<parzone->NumSerialZones(); sz++) 
      {
         zoneindex = (parzone->SerialZone(sz))->ZoneIndex();
         for ( i=0; i<numstatevar; i++ )            // Concentrations C
         {
            BOOLEAN found = FALSE;

            for ( j=0; j<NumInitCond(); j++ )
            {
               if ( initvar[j]==statevar[i] && initzone[j]==z)
               {
                  k = j; found = TRUE; break;
               }
            }

            for ( j=0; j<NumGridPts(); j++ )
            {
               if ( found == TRUE )
               {
                  varsys->Reset();
                  if ( Xvar != 0 ) Xvar->SetProgVar(&gridpts[j]);
                  if ( Qvar != 0 ) Qvar->SetProgVar(&y[j*cell]);
                  if ( Avar != 0 ) Avar->SetProgVar(&areas[j]);

                  y[j*cell+ioff+numstatevar*z+i] = initval[k]->Value();
               }
               else
               {
                  y[j*cell+ioff+numstatevar*z+i] = 0.0;
               }           
            }
         }
         z++;
      }
      parzone = parzone -> Next();
   }

//   for ( i=0; i<numvolstatevar; i++ )        // VolStateVar: A*C
//   {
//      for ( j=0; j<NumGridPts(); j++ )
//      {
//         for ( z=0; z<numzones; z++ )
//         {
//            y[j*cell+ioff+z*numstatevar+i] *= zonecrosssect[j][z];
//         }
//      }
//   }

   for ( j=0; j<numgrid; j++ )
   {
      REAL watercrosssect = zonecrosssect[j][0];
      for ( z=1; z<numzones; z++ )
      {
         watercrosssect += zonecrosssect[j][z];
      }
      if ( watercrosssect > areas[j] )
      {
         *cjob << "\n   SOILCOMP numeric problem:"
               << " Sum of volume fractions exceeds 1";
         cjob->flush();
         return FALSE;
      }
   }

   if ( withdispersion == TRUE )
   {
      y[numgrid*cell] = E->Value();
   }
   else
   {
      y[numgrid*cell] = 0.0;
   }

   for ( j=0; j<numgrid; j++ ) delete [] zonecrosssect[j];
   delete [] zonecrosssect;
   delete [] areas;

/* 
cout << "\nInitCond: " << Symbol();
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

// ----------------------------------------------------------------------------

REAL SOILCOMP::FluxLimit(REAL s1, REAL s2, CARDINAL limiter)
{
  

   // 0 = first order; 1 = second order

   if ( highres == FALSE )   return 0.0;
   if ( s2 == 0.0 )          return 0.0;

   REAL theta = s1/s2;

   switch ( limiter )
   {
      case 1:  // van Leer:
         if ( theta <= 0.0 ) return 0.0;
         return 2.0*theta/(1.0+theta);
      case 2:  // superbee:
         if ( theta <= 0.0 ) return 0.0;
         if ( theta <= 0.5 ) return 2.0*theta;
         if ( theta <= 1.0 ) return 1.0;
         if ( theta <= 2.0 ) return theta;
         return 2.0;
      default:
         break;
   }
   return 0;
}


BOOLEAN SOILCOMP::Delta(const NUMPAR&, VARSYS* varsys,
                        const REAL* Y, const REAL* YT, REAL* DELTA)
{
   if ( Active() != TRUE ) return FALSE;

   const REAL* y  = &Y[ind_y];
   const REAL* yt = &YT[ind_y];
   REAL* delta = &DELTA[ind_y];

   CARDINAL i;
   CARDINAL j;
   CARDINAL cell = ioff+numstatevar*numzones;

   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   REAL zoneindex = 0.0; varsys->SetProgVar(ZoneIndex,&zoneindex);
   REAL eps = 1.0;       varsys->SetProgVar(WaterFraction,&eps);

   Xvar = varsys->GetProgVar(SpaceX);
   Avar = varsys->GetProgVar(CrossSection); 
   Qvar = varsys->GetProgVar(Discharge);

   REAL* conc   = new REAL[numstatevar];
   REAL* flux   = new REAL[numstatevar];
   REAL* Qs     = new REAL[numgrid];
   REAL* areas  = new REAL[numgrid];
   REAL* es     = new REAL[numgrid];
   REAL* qlat   = new REAL[numgrid];

   // -----------------------
   // calculate input fluxes:
   // -----------------------

   REAL Q = 0.0;
   for ( i=0; i<numstatevar; i++ )
   {
      conc[i] = 0.0; flux[i] = 0.0;
   }
   varsys->Reset();
   if ( Xvar != 0 ) Xvar->SetProgVar(&gridpts[0]);
   if ( Qvar != 0 ) Qvar->SetProgVar(&Q);

   const CARDINAL advinconn = 0;              // input from advlink
   for ( j=0; j<advinnum[advinconn]; j++ )
   {
      Q += advinlink[advinconn][j]
               ->AdvOutQ(varsys,Y,advinindex[advinconn][j]);
   }
   for ( i=0; i<numvolstatevar; i++ )
   {
      for ( j=0; j<advinnum[advinconn]; j++ )
      {
         flux[i] += advinlink[advinconn][j]
                    ->AdvOutJ(varsys,Y,advinindex[advinconn][j],statevar[i]);
      }
   }

   if ( Q > 0.0 )                             // input from AQCOMP
   {
      for ( i=0; i<numvolstatevar; i++ ) conc[i] = flux[i]/Q;
   }
   varsys->Reset();
   if ( Xvar != 0 ) Xvar->SetProgVar(&gridpts[0]);
   if ( Qvar != 0 ) Qvar->SetProgVar(&Q);
   for ( i=0; i<numstatevar; i++ ) statevar[i]->SetStateVar(&conc[i]);
   for ( i=0; i<numvolstatevar; i++ )
   {
      for ( j=0; j<numinput; j++ )  // CARDINAL wurde entfernt
      {
         if ( inputvar[j] == statevar[i] )
         {
            flux[i] += inputflux[j]->Value(); break;
         }
      }
   }
   if ( inflow != 0 )
   {
      REAL Qext = inflow->Value();
      if ( Qext < 0.0 )
      {
         *cjob << "\n   SOILCOMP numerical problem:"
               << " Inflow is negative";
         cjob->flush();
         return FALSE;
      }
      Q += Qext;
   }
   if ( Q < 0.0 )
   {
      *cjob << "\n   SOILCOMP numerical problem:"
            << " Inflow is negative";
      cjob->flush();
      return FALSE;
   }
 
   REAL** zonecrosssect;   // advective zone: zonecrosssect[j][0]
                           // serial zones:   zonecrosssect[j][i]
   zonecrosssect = new REAL* [numgrid]; 

   for ( j=0; j<numgrid; j++)
   { 
      zonecrosssect[j] = new REAL [numzones];
      varsys -> Reset();

      if ( Xvar != 0) Xvar -> SetProgVar(&gridpts[j]);
      areas[j] = A -> Value();
      if ( Avar != 0) Avar -> SetProgVar(&areas[j]);

      zonecrosssect [j][0] = areas[j] * advvolfract -> Value();

      CARDINAL z=1;

      PARZONE* parzone = parzonelist.First();
  
      while (parzone != 0) 
      { 
         for ( CARDINAL sz=0; sz<parzone->NumSerialZones(); sz++)
         {
            SERZONE* serial = parzone->SerialZone(sz);
            zonecrosssect[j][z] = areas[j]*(serial->volfract) -> Value();
            z++; 
         }
         parzone = parzone -> Next();
      }
   }
   preendcrosssect = zonecrosssect[NumGridPts()-2][0];
   endcrosssect    = zonecrosssect[NumGridPts()-1][0];


   // ----------------
   // calculate rates:
   // ----------------

   for ( j=0; j<NumGridPts(); j++ )
   {
      if ( y[j*cell] > 0.0 )
      {
         Qs[j] = y[j*cell];
      }
      else
      {
         Qs[j] = 0.0;
      }
      if ( Qvar != 0 ) Qvar->SetProgVar(&Qs[j]);

      // advective zone:
      // ---------------
    
      for ( i=0; i<numvolstatevar; i++ )                 // VolStateVar
      {
//         conc[i] = y[j*cell+ioff+i]/zonecrosssect[j][0];
         conc[i] = y[j*cell+ioff+i];
      }
      for ( i=numvolstatevar; i<numdynstatevar; i++ )    // SurfStateVar
      {
         conc[i] = y[j*cell+ioff+i];
      }
      for ( i=numdynstatevar; i<numstatevar; i++ )       // EquStateVar
      {
         conc[i] = y[j*cell+ioff+i];
      }
      zoneindex = 0;                               // ** correction 22.04.96 **
      varsys->Reset();
      if ( Xvar != 0 ) Xvar->SetProgVar(&gridpts[j]);
      if ( Avar != 0 ) Avar->SetProgVar(&areas[j]);
      if ( latinflow == 0 ) qlat[j] = 0.0;
      else                  qlat[j] = latinflow->Value();
      CalcRates(&delta[j*cell+ioff]);
      for ( i=0; i<numvolstatevar; i++ )
      {
//         delta[j*cell+ioff+i] = delta[j*cell+ioff+i] * zonecrosssect[j][0];
         if ( qlat[j] > 0.0 )
         {
            for ( CARDINAL k=0; k<numlatinput; k++ )
            {
               if ( latinputvar[k] == statevar[i] )
               {
//                  delta[j*cell+ioff+i] += qlat[j]*latinputconc[k]->Value();
                  delta[j*cell+ioff+i] += qlat[j]*latinputconc[k]->Value()
                                          /zonecrosssect[j][0];
               }
            }
         }
         else
         {
//            delta[j*cell+ioff+i] += qlat[j]*conc[i];   // qlat < 0 !
            delta[j*cell+ioff+i] += qlat[j]*conc[i]    // qlat < 0 !
                                    /zonecrosssect[j][0];
         }
      }
      if ( withdispersion == TRUE )
      {
         es[j] = E->Value();
         if ( es[j] <= 0.0 )
         {
            *cjob << "\n   SOILCOMP numerical problem:"
                  << " Dispersion coefficient is not positive";
            cjob->flush();
            return FALSE;
         }
      }
      else
      {
         es[j] = 0.0;
      }

      // serial zones:
      // -------------
  
      PARZONE* parzone = parzonelist.First();
      CARDINAL z=1;
      while (parzone != 0)
      {
         for ( CARDINAL sz=0; sz<parzone->NumSerialZones(); sz++)
         {
            SERZONE* serial = parzone -> SerialZone(sz);
     
            for ( i=0; i<numvolstatevar; i++ )                 // VolStateVar
            {
//               conc[i] = y[j*cell+ioff+z*numstatevar+i]/zonecrosssect[j][z];
               conc[i] = y[j*cell+ioff+z*numstatevar+i];
            }
            for ( i=numvolstatevar; i<numdynstatevar; i++ )    // SurfStateVar
            {
               conc[i] = y[j*cell+ioff+z*numstatevar+i];
            }
            for ( i=numdynstatevar; i<numstatevar; i++ )       // EquStateVar
            {
               conc[i] = y[j*cell+ioff+z*numstatevar+i];
            }
            zoneindex = serial->ZoneIndex();       // ** correction 22.04.96 **
            varsys->Reset();
            CalcRates(&delta[j*cell+ioff+z*numstatevar]);
                                                                                
//            for ( i=0; i<numvolstatevar; i++ )
//            {
//               delta[j*cell+ioff+z*numstatevar+i]
//                  = delta[j*cell+ioff+z*numstatevar+i] * zonecrosssect[j][z];
//            }
            z++;
         }
         parzone = parzone -> Next();
      }
   }

   REAL advflux1,advflux2,diffflux;     // fluxes
   REAL dx    = fabs(gridpts[2]-gridpts[1]);  // cells of equal length
   REAL dxinv = 1.0/dx;                       // inverse zell length
   REAL s1, s2;
   REAL phi = 0.0;

   // --------------------------------------------------
   // equations at the upstream boundary (grid point 0):
   // --------------------------------------------------

   // advective zone:
   // ---------------

   delta[0] = y[0] - Q;                                      // Q

   for ( i=0; i<numvolstatevar; i++ )                        // VolStateVar
   {
//      delta[ioff+i] = y[ioff+i] - flux[i]*zonecrosssect[0][0]/y[0];
      delta[ioff+i] = y[ioff+i] - flux[i]/y[0];

      if ( withdispersion == TRUE )       // add dispersive flux    
      {
//         delta[ioff+i] = Qs[0]/zonecrosssect[0][0]*y[ioff+i] - flux[i]
//                         -zonecrosssect[0][0]*es[0]
//                          *(y[cell+ioff+i]/zonecrosssect[1][0]
//                           -y[ioff+i]/zonecrosssect[0][0])
//                          *2.0*dxinv;
         delta[ioff+i] = Qs[0]*y[ioff+i] - flux[i]
                         -zonecrosssect[0][0]*es[0]
                          *(y[cell+ioff+i]-y[ioff+i])
                          *2.0*dxinv;
      }
      else
      {
         if ( Q > 0.0 ) 
         {
//            delta[ioff+i] = Q/zonecrosssect[0][0]*y[ioff+i] - flux[i];
            delta[ioff+i] = Q*y[ioff+i] - flux[i];
         }
         else // conc. equal to grid point 1 if Q=0 and no dispersion
         {
            delta[ioff+i] = y[cell+ioff+i]-y[ioff+i];
            // old version: conc. zero if Q=0 and no dispersion:
            // delta[ioff+i] = y[ioff+i];
         }
      }
   }

   for ( i=numvolstatevar; i<numdynstatevar; i++ )           // SurfStateVar
   {
      delta[ioff+i] -= yt[ioff+i];
   }
   /* numdynstatevar <= i < numstatevar */                   // EquStateVar

   // serial zones adjacent to the advective zone
   // -------------------------------------------

   if ( Xvar != 0 ) Xvar->SetProgVar(&gridpts[0]);
   if ( Qvar != 0 ) Qvar->SetProgVar(&y[0]);
   if ( Avar != 0 ) Avar->SetProgVar(&areas[0]);

   PARZONE* parzone = parzonelist.First();
   CARDINAL z=1;
   while (parzone != 0)
   {
     
      for ( i=0; i<numvolstatevar; i++ )                  // VolStateVar
      {
//         conc[i] = y[ioff+z*numstatevar+i]/zonecrosssect[0][z];
         conc[i] = y[ioff+z*numstatevar+i];
      }
      for ( i=numvolstatevar; i<numdynstatevar; i++ )    // SurfStateVar
      {
         conc[i] = y[ioff+z*numstatevar+i];
      }
      for ( i=numdynstatevar; i<numstatevar; i++ )       // EquStateVar
      {
         conc[i] = y[ioff+z*numstatevar+i];
      }
     

      SERZONE* serzone = parzone->SerialZone(0);
      zoneindex = serzone->ZoneIndex();            // ** correction 22.04.96 **
      varsys->Reset();
      for ( i=0; i<numvolstatevar; i++ )
      {
         REAL exchrate = 0.0;
         for ( CARDINAL k=0; k<serzone->numexch; k++ )
         {
            if ( statevar[i] == serzone->exchvar[k] )
            {
//               exchrate = (y[ioff+i]/zonecrosssect[0][0]                      
//                          -(serzone->exchfac[k])->Value()
//                          *conc[i])*(serzone->exchval[k])->Value();
               exchrate = (y[ioff+i]                      
                          -(serzone->exchfac[k])->Value()
                          *conc[i])*(serzone->exchval[k])->Value();
               break;
            }
         }
//         delta[ioff+z*numstatevar+i] += exchrate;  // serzone z
         delta[ioff+z*numstatevar+i] += exchrate/zonecrosssect[0][z];  // serzone z
         delta[ioff+z*numstatevar+i] -= yt[ioff+z*numstatevar+i];
         // no exchange term in adv. zone, because alg. equation at grid pt. 0
      }

      for ( i=numvolstatevar; i<numdynstatevar; i++ )
      {
         delta[ioff+z*numstatevar+i] -= yt[ioff+z*numstatevar+i];
      }

      // other serial zones:
      // -------------------

      z++;
      for ( CARDINAL sz=1; sz<parzone->NumSerialZones(); sz++)
      {
         serzone = parzone->SerialZone(sz);
         for ( i=0; i<numvolstatevar; i++ )                  // VolStateVar
         {
//            conc[i] = y[ioff+z*numstatevar+i]/zonecrosssect[0][z];// j=0
            conc[i] = y[ioff+z*numstatevar+i]; // j=0
         }
         for ( i=numvolstatevar; i<numdynstatevar; i++ )    // SurfStateVar
         {
            conc[i] = y[ioff+z*numstatevar+i];
         }
         for ( i=numdynstatevar; i<numstatevar; i++ )       // EquStateVar
         {
            conc[i] = y[ioff+z*numstatevar+i];
         }
     
         zoneindex = serzone->ZoneIndex();         // ** correction 22.04.96 **
         varsys->Reset();

         for ( i=0; i<numvolstatevar;i++)
         {
            REAL exchrate = 0.0;
            for ( CARDINAL k=0; k<serzone->numexch; k++ )
            {
               if ( statevar[i] == serzone->exchvar[k] )
               {
//                  exchrate = (y[ioff+(z-1)*numstatevar+i]
//                             /zonecrosssect[0][z-1]
//                             -(serzone->exchfac[k])->Value()
//                              *conc[i])*(serzone->exchval[k])->Value();
                  exchrate = (y[ioff+(z-1)*numstatevar+i]
                             -(serzone->exchfac[k])->Value()
                              *conc[i])*(serzone->exchval[k])->Value();
                  break;
               }
            }
//            delta[ioff+z*numstatevar+i]     += exchrate; // serzone z
            delta[ioff+z*numstatevar+i]     += exchrate/zonecrosssect[0][z]; // serzone z
            delta[ioff+z*numstatevar+i] -= yt[ioff+z*numstatevar+i];
//            delta[ioff+(z-1)*numstatevar+i] -= exchrate; // serzone z-1
            delta[ioff+(z-1)*numstatevar+i] -= exchrate/zonecrosssect[0][z-1]; // serzone z-1
         }
         for ( i=numvolstatevar; i<numdynstatevar; i++ )
         {
            delta[ioff+z*numstatevar+i] -= yt[ioff+z*numstatevar+i];
         }
         z++;
      }
      parzone = parzone->Next();
   }  


   // ------------------------------------------
   // equations in the first cell (gridpoint 1):
   // ------------------------------------------


   if ( Xvar != 0 ) Xvar->SetProgVar(&gridpts[1]);
   if ( Qvar != 0 ) Qvar->SetProgVar(&y[cell]);
   if ( Avar != 0 ) Avar->SetProgVar(&areas[1]);

   // advective zone:
   // ---------------

   // recalculate fluxes with actual values to minimize codiagonals:
   for ( i=0; i<numvolstatevar; i++ )
   {
//      flux[i] = Q*y[ioff+i]/zonecrosssect[0][0];
      flux[i] = Q*y[ioff+i];
      if ( withdispersion == TRUE )
      {
                                  //y[cell+1]=zone[1][0], y[1]=. [0][0]
//         diffflux=-zonecrosssect[0][0]*es[0]*(y[cell+ioff+i]/zonecrosssect[1][0]
//                  -y[ioff+i]/zonecrosssect[0][0])*2.0*dxinv;
         diffflux=-zonecrosssect[0][0]*es[0]
                   *(y[cell+ioff+i]-y[ioff+i])*2.0*dxinv;
         flux[i] += diffflux;
      }
   }

   delta[cell]   = y[cell] - Qs[0]                           // Q
                   - 0.5*(qlat[0]+qlat[1])
                        *(gridpts[1]-gridpts[0]);

   for ( i=0; i<numvolstatevar; i++ )                        // VolStateVar
   {
      // add flux at the left boundary of the cell:

//      delta[cell+ioff+i] += flux[i]*2.0*dxinv;   correction 16.04.96:
//      delta[cell+ioff+i] += flux[i]*dxinv;
      delta[cell+ioff+i] += flux[i]*dxinv/zonecrosssect[1][0];

      // calculate flux at the right boundary of the cell:

//      advflux1 = Qs[1]*y[cell+ioff+i]/zonecrosssect[1][0];
      advflux1 = Qs[1]*y[cell+ioff+i];
//      advflux2 = 0.5*(Qs[1]*y[cell+ioff+i]/zonecrosssect[1][0]
//                     +Qs[2]*y[2*cell+ioff+i]/zonecrosssect[2][0]);  
      advflux2 = 0.5*(Qs[1]*y[cell+ioff+i]
                     +Qs[2]*y[2*cell+ioff+i]);  
//      s1 = (y[cell+ioff+i]/zonecrosssect[1][0]-y[ioff+i]
//           /zonecrosssect[0][0])        
//           / fabs(gridpts[1]-gridpts[0]);
      s1 = (y[cell+ioff+i]-y[ioff+i])        
           / fabs(gridpts[1]-gridpts[0]);
//      s2 = (y[2*cell+ioff+i]/zonecrosssect[2][0]
//            -y[cell+ioff+i]/zonecrosssect[1][0])// 
//           / fabs(gridpts[2]-gridpts[1]);
      s2 = (y[2*cell+ioff+i]-y[cell+ioff+i])
           / fabs(gridpts[2]-gridpts[1]);
      phi = FluxLimit(s1,s2,ini.I(4));

      if ( fabs(y[ioff+i])
          +fabs(y[cell+ioff+i])
          +fabs(y[2*cell+ioff+i]) 
              < 0.001*statevar[i]->AbsAccuracy() ) phi=0;  // test 2.6.97

      flux[i] = (1.0-phi)*advflux1 + phi*advflux2;

      if ( withdispersion == TRUE )   // add dispersive flux
      {
//         diffflux =  -(zonecrosssect[1][0]+zonecrosssect[2][0])
//                      *(es[1]+es[2])*0.25 
//                      *(y[2*cell+ioff+i]/zonecrosssect[2][0]              
//                        -y[cell+ioff+i]/zonecrosssect[1][0])
//                      *dxinv;
         diffflux =  -(zonecrosssect[1][0]+zonecrosssect[2][0])
                      *(es[1]+es[2])*0.25 
                      *(y[2*cell+ioff+i]-y[cell+ioff+i])
                      *dxinv;
         flux[i] += diffflux;
      }

      // subtract flux at the right boundary of the cell:
//      delta[cell+ioff+i] -= flux[i]*2.0*dxinv;   correction 16.04.96:
//      delta[cell+ioff+i] -= flux[i]*dxinv;
      delta[cell+ioff+i] -= flux[i]*dxinv/zonecrosssect[1][0];

      // subtract time derivative:
      delta[cell+ioff+i] -= yt[cell+ioff+i];
   }

   for ( i=numvolstatevar; i<numdynstatevar; i++ )           // SurfStateVar
   {
      delta[cell+ioff+i] -= yt[cell+ioff+i];
   }

   /* numdynstatevar <= i < numstatevar */                   // EquStateVar


   // serial zone adjacent to advective zone:
   // ---------------------------------------

   parzone = parzonelist.First();
   z=1; 
   while (parzone != 0)
   {
      for ( i=0; i<numvolstatevar; i++ )                 // VolStateVar
      {
//         conc[i] = y[cell+ioff+z*numstatevar+i]/zonecrosssect[1][z];
         conc[i] = y[cell+ioff+z*numstatevar+i];
      }
      for ( i=numvolstatevar; i<numdynstatevar; i++ )    // SurfStateVar
      {
         conc[i] = y[cell+ioff+z*numstatevar+i];
      }
      for ( i=numdynstatevar; i<numstatevar; i++ )       // EquStateVar
      {
         conc[i] = y[cell+ioff+z*numstatevar+i];
      }
     
      SERZONE* serzone = parzone->SerialZone(0);
      zoneindex = serzone->ZoneIndex();            // ** correction 22.04.96 **
      varsys->Reset();
      for ( i=0; i<numvolstatevar; i++ )
      {
         REAL exchrate = 0.0;
         for ( CARDINAL k=0; k<serzone->numexch; k++ )
         {
            if ( statevar[i] == serzone->exchvar[k] )
            {
//               exchrate = (y[cell+ioff+i]/zonecrosssect[1][0]
//                          -(serzone->exchfac[k])->Value()*conc[i])
//                          *(serzone->exchval[k])->Value();
               exchrate = (y[cell+ioff+i]
                          -(serzone->exchfac[k])->Value()*conc[i])
                          *(serzone->exchval[k])->Value();
               break;
            }
         }
//         delta[cell+ioff+z*numstatevar+i] += exchrate; // serzone z
         delta[cell+ioff+z*numstatevar+i] += exchrate/zonecrosssect[1][z]; // serzone z
         delta[cell+ioff+z*numstatevar+i]
            -= yt[cell+ioff+z*numstatevar+i];
//         delta[cell+ioff+i]               -= exchrate; // adv. zone
         delta[cell+ioff+i]               -= exchrate/zonecrosssect[1][0]; // adv. zone
      }
      for ( i=numvolstatevar; i<numdynstatevar; i++ )
      {
         delta[cell+ioff+z*numstatevar+i]
            -= yt[cell+ioff+z*numstatevar+i]; //z*num.
      }

      // other serial zones:
      // -------------------

      z++;
      for ( CARDINAL sz=1; sz<parzone->NumSerialZones(); sz++)
      {
         serzone = parzone->SerialZone(sz);

         for ( i=0; i<numvolstatevar; i++ )                 // VolStateVar
         {
//            conc[i] = y[cell+ioff+z*numstatevar+i]/zonecrosssect[1][z];
            conc[i] = y[cell+ioff+z*numstatevar+i];
         }
         for ( i=numvolstatevar; i<numdynstatevar; i++ )    // SurfStateVar
         {
            conc[i] = y[cell+ioff+z*numstatevar+i];
         }
         for ( i=numdynstatevar; i<numstatevar; i++ )       // EquStateVar
         {
            conc[i] = y[cell+ioff+z*numstatevar+i];
         }
     
         zoneindex = serzone->ZoneIndex();         // ** correction 22.04.96 **
         varsys->Reset();
         for ( i=0; i<numvolstatevar;i++)
         {
            REAL exchrate = 0.0;
            for ( CARDINAL k=0; k<serzone->numexch; k++ )
            {
               if ( statevar[i] == serzone->exchvar[k] )
               {
//                  exchrate = (y[cell+ioff+(z-1)*numstatevar+i]/zonecrosssect[1][z-1]  
//                             -(serzone->exchfac[k])->Value()
//                             *conc[i])*(serzone->exchval[k])->Value();
                  exchrate = (y[cell+ioff+(z-1)*numstatevar+i]  
                             -(serzone->exchfac[k])->Value()
                             *conc[i])*(serzone->exchval[k])->Value();
                  break;
               }
            }
//            delta[cell+ioff+z*numstatevar+i]     += exchrate; // serzone z
            delta[cell+ioff+z*numstatevar+i]     += exchrate/zonecrosssect[1][z]; // serzone z
            delta[cell+ioff+z*numstatevar+i]
               -= yt[cell+ioff+z*numstatevar+i];
//            delta[cell+ioff+(z-1)*numstatevar+i] -= exchrate; // serzone z-1
            delta[cell+ioff+(z-1)*numstatevar+i] -= exchrate/zonecrosssect[1][z-1]; // serzone z-1
         }
         for ( i=numvolstatevar; i<numdynstatevar; i++ )
         {
            delta[cell+ioff+z*numstatevar+i] 
               -= yt[cell+ioff+z*numstatevar+i];
         }
         z++;
      }
      parzone = parzone->Next();
   }  
        

   // ---------------------------------------------------
   // equations in the inner cells (gridpoints 2 to n-3):
   // ---------------------------------------------------

   for ( j=2; j<NumGridPts()-2; j++ )
   {
      if ( Xvar != 0 ) Xvar->SetProgVar(&gridpts[j]);
      if ( Qvar != 0 ) Qvar->SetProgVar(&y[j*cell]);
      if ( Avar != 0 ) Avar->SetProgVar(&areas[j]);

      // advective zone:
      // ---------------

      delta[j*cell]   = y[j*cell] - Qs[j-1]                  // Q
                        - 0.5*(qlat[j]+qlat[j-1])
                             *(gridpts[j]-gridpts[j-1]);

      for ( i=0; i<numvolstatevar; i++ )                     // VolStateVar
      {
         // add flux at the left boundary of the cell:

//         delta[j*cell+ioff+i] += flux[i]*dxinv;
         delta[j*cell+ioff+i] += flux[i]*dxinv/zonecrosssect[j][0];

         // calculate flux at the right boundary of the cell:

//         advflux1 = Qs[j]*y[j*cell+ioff+i]/zonecrosssect[j][0];
         advflux1 = Qs[j]*y[j*cell+ioff+i];
//         advflux2 = 0.5*(Qs[j]*y[j*cell+ioff+i]/zonecrosssect[j][0]
//                    +Qs[j+1]*y[(j+1)*cell+ioff+i]/zonecrosssect[j+1][0]);
         advflux2 = 0.5*(Qs[j]*y[j*cell+ioff+i]
                    +Qs[j+1]*y[(j+1)*cell+ioff+i]);
//         s1 = (y[j*cell+ioff+i]/zonecrosssect[j][0]
//                 -y[(j-1)*cell+ioff+i]/zonecrosssect[j-1][0])
//              / fabs(gridpts[j]-gridpts[j-1]);
         s1 = (y[j*cell+ioff+i]-y[(j-1)*cell+ioff+i])
              / fabs(gridpts[j]-gridpts[j-1]);
//         s2 = (y[(j+1)*cell+ioff+i]/zonecrosssect[j+1][0]
//                 -y[j*cell+ioff+i]/zonecrosssect[j][0])
//              / fabs(gridpts[j+1]-gridpts[j]);
         s2 = (y[(j+1)*cell+ioff+i]-y[j*cell+ioff+i])
              / fabs(gridpts[j+1]-gridpts[j]);
         phi = FluxLimit(s1,s2,ini.I(4));

         if ( fabs(y[(j-1)*cell+ioff+i])
             +fabs(y[j*cell+ioff+i])
             +fabs(y[(j+1)*cell+ioff+i])
                 < 0.001*statevar[i]->AbsAccuracy() ) phi=0;  // test 2.6.97

         flux[i] = (1.0-phi)*advflux1 + phi*advflux2;

         if ( withdispersion == TRUE )
         {
//            diffflux =  -(zonecrosssect[j][0]+zonecrosssect[j+1][0])
//                         *(es[j]+es[j+1])*0.25
//                         *(y[(j+1)*cell+ioff+i]/zonecrosssect[j+1][0]
//                           -y[j*cell+ioff+i]/zonecrosssect[j][0])
//                         *dxinv;
            diffflux =  -(zonecrosssect[j][0]+zonecrosssect[j+1][0])
                         *(es[j]+es[j+1])*0.25
                         *(y[(j+1)*cell+ioff+i]-y[j*cell+ioff+i])
                         *dxinv;
            flux[i] += diffflux;
         }

         // subtract flux at the right boundary of the cell:

//         delta[j*cell+ioff+i] -= flux[i]*dxinv;
         delta[j*cell+ioff+i] -= flux[i]*dxinv/zonecrosssect[j][0];

         // subtract time derivative:

         delta[j*cell+ioff+i] -= yt[j*cell+ioff+i];
      }

      for ( i=numvolstatevar; i<numdynstatevar; i++ )        // SurfStateVar
      {
         delta[j*cell+ioff+i] -= yt[j*cell+ioff+i];
      }

      /* numdynstatevar <= i < numstatevar */                // EquStateVar


      // serial zone adjacent to advective zone:
      // ---------------------------------------

      PARZONE* parzone = parzonelist.First();
      z=1; 
      while (parzone != 0)
      {
         for ( i=0; i<numvolstatevar; i++ )                  // VolStateVar
         {
//            conc[i] = y[j*cell+ioff+z*numstatevar+i]
//                      /zonecrosssect[j][z];
            conc[i] = y[j*cell+ioff+z*numstatevar+i];
         }
         for ( i=numvolstatevar; i<numdynstatevar; i++ )    // SurfStateVar
         {
            conc[i] = y[j*cell+ioff+z*numstatevar+i];
         }
         for ( i=numdynstatevar; i<numstatevar; i++ )       // EquStateVar
         {
            conc[i] = y[j*cell+ioff+z*numstatevar+i];
         }
     
         SERZONE* serzone = parzone->SerialZone(0);
         zoneindex = serzone->ZoneIndex();         // ** correction 22.04.96 **
         varsys->Reset();
         for ( i=0; i<numvolstatevar; i++ )
         {
            REAL exchrate = 0.0;
            for ( CARDINAL k=0; k<serzone->numexch; k++ )
            {
               if ( statevar[i] == serzone->exchvar[k] )
               {
//                  exchrate = (y[j*cell+ioff+i]/zonecrosssect[j][0]                
//                             -(serzone->exchfac[k])->Value()*conc[i])
//                             *(serzone->exchval[k])->Value();
                  exchrate = (y[j*cell+ioff+i]                
                             -(serzone->exchfac[k])->Value()*conc[i])
                             *(serzone->exchval[k])->Value();
                  break;
               }
            }
//            delta[j*cell+ioff+z*numstatevar+i] += exchrate; // serzone z
            delta[j*cell+ioff+z*numstatevar+i] += exchrate/zonecrosssect[j][z]; // serzone z
            delta[j*cell+ioff+z*numstatevar+i]
               -= yt[j*cell+ioff+z*numstatevar+i];
//            delta[j*cell+ioff+i]               -= exchrate; // adv. zone
            delta[j*cell+ioff+i]               -= exchrate/zonecrosssect[j][0]; // adv. zone
         }

         for ( i=numvolstatevar; i<numdynstatevar; i++ )
         {
            delta[j*cell+ioff+z*numstatevar+i]
               -= yt[j*cell+ioff+z*numstatevar+i];
         }

         // other serial zones:
         // -------------------

         z++;
         for ( CARDINAL sz=1; sz<parzone->NumSerialZones(); sz++)
         { 
            serzone = parzone->SerialZone(sz);
            for ( i=0; i<numvolstatevar; i++ )                 // VolStateVar
            {
//               conc[i] = y[j*cell+ioff+z*numstatevar+i]/zonecrosssect[j][z];
               conc[i] = y[j*cell+ioff+z*numstatevar+i];
            }
            for ( i=numvolstatevar; i<numdynstatevar; i++ )    // SurfStateVar
            {
               conc[i] = y[j*cell+ioff+z*numstatevar+i];
            }
            for ( i=numdynstatevar; i<numstatevar; i++ )       // EquStateVar
            {
               conc[i] = y[j*cell+ioff+z*numstatevar+i];
            }

            zoneindex = serzone->ZoneIndex();      // ** correction 22.04.96 **
            varsys->Reset();
            for ( i=0; i<numvolstatevar;i++)
            {
               REAL exchrate = 0.0;
               for ( CARDINAL k=0; k<serzone->numexch; k++ )
               {
                  if ( statevar[i] == serzone->exchvar[k] )
                  {
//                     exchrate = (y[j*cell+ioff+(z-1)*numstatevar+i]
//                                /zonecrosssect[j][z-1]
//                                -(serzone->exchfac[k])->Value()*conc[i])
//                                *(serzone->exchval[k])->Value();
                     exchrate = (y[j*cell+ioff+(z-1)*numstatevar+i]
                                -(serzone->exchfac[k])->Value()*conc[i])
                                *(serzone->exchval[k])->Value();
                     break;
                  }
               }
//               delta[j*cell+ioff+z*numstatevar+i]     += exchrate; // serzone z
               delta[j*cell+ioff+z*numstatevar+i]     += exchrate/zonecrosssect[j][z]; // serzone z
               delta[j*cell+ioff+z*numstatevar+i] -= yt[j*cell+ioff+z*numstatevar+i]; 
//               delta[j*cell+ioff+(z-1)*numstatevar+i] -= exchrate; // serzone z-1
               delta[j*cell+ioff+(z-1)*numstatevar+i] -= exchrate/zonecrosssect[j][z-1]; // serzone z-1
            } 
            for ( i=numvolstatevar; i<numdynstatevar; i++ )
            {
               delta[j*cell+ioff+z*numstatevar+i] -= yt[j*cell+ioff+z*numstatevar+i]; 
            }
            z++;     
         }
         parzone = parzone->Next();
      }  
   }



   // --------------------------------------------
   // equations in the last cell (grid point n-2):
   // --------------------------------------------

   if ( Xvar != 0 ) Xvar->SetProgVar(&gridpts[j]);
   if ( Qvar != 0 ) Qvar->SetProgVar(&y[j*cell]);
   if ( Avar != 0 ) Avar->SetProgVar(&areas[j]);

   // advective zone:
   // ---------------

   j = NumGridPts()-2;

   delta[j*cell]   = y[j*cell] - Qs[j-1]                     // Q
                     - 0.5*(qlat[j]+qlat[j-1])
                          *(gridpts[j]-gridpts[j-1]);

   for ( i=0; i<numvolstatevar; i++ )                        // VolStateVar
   {
      // add flux at the left boundary of the cell:

//      delta[j*cell+ioff+i] += flux[i]*dxinv;
      delta[j*cell+ioff+i] += flux[i]*dxinv/zonecrosssect[j][0];

      // calculate flux at the right boundary of the cell:

//      advflux1 = Qs[j]*y[j*cell+ioff+i]/zonecrosssect[j][0];
      advflux1 = Qs[j]*y[j*cell+ioff+i];
//      advflux2 = Qs[j+1]*y[(j+1)*cell+ioff+i]/zonecrosssect[j+1][0];
      advflux2 = Qs[j+1]*y[(j+1)*cell+ioff+i];
//      s1 = (y[j*cell+ioff+i]/zonecrosssect[j][0]
//              -y[(j-1)*cell+ioff+i]/zonecrosssect[j-1][0])
//           / fabs(gridpts[j]-gridpts[j-1]);
      s1 = (y[j*cell+ioff+i]-y[(j-1)*cell+ioff+i])
           / fabs(gridpts[j]-gridpts[j-1]);
//      s2 = (y[(j+1)*cell+ioff+i]/zonecrosssect[j+1][0]
//              -y[j*cell+ioff+i]/zonecrosssect[j][0])
//           / fabs(gridpts[j+1]-gridpts[j]);
      s2 = (y[(j+1)*cell+ioff+i]-y[j*cell+ioff+i])
           / fabs(gridpts[j+1]-gridpts[j]);
      phi = FluxLimit(s1,s2,ini.I(4));

      if ( fabs(y[(j-1)*cell+ioff+i])
          +fabs(y[j*cell+ioff+i])
          +fabs(y[(j+1)*cell+ioff+i])
              < 0.001*statevar[i]->AbsAccuracy() ) phi=0;  // test 2.6.97

      flux[i] = (1.0-phi)*advflux1 + phi*advflux2;
      if ( withdispersion == TRUE )
      {
//         diffflux =  -zonecrosssect[j+1][0]*es[j+1]
//                      *(y[(j+1)*cell+ioff+i]/zonecrosssect[j+1][0]
//                        -y[j*cell+ioff+i]/zonecrosssect[j][0])
//                      *2.0*dxinv;
         diffflux =  -zonecrosssect[j+1][0]*es[j+1]
                      *(y[(j+1)*cell+ioff+i]-y[j*cell+ioff+i])
                      *2.0*dxinv;
         flux[i] += diffflux;
      }

      // subtract flux at the right boundary of the cell:

//      delta[j*cell+ioff+i] -= flux[i]*dxinv;
      delta[j*cell+ioff+i] -= flux[i]*dxinv/zonecrosssect[j][0];

      // subtract time derivative:

      delta[j*cell+ioff+i] -= yt[j*cell+ioff+i];
   }

   for ( i=numvolstatevar; i<numdynstatevar; i++ )           // SurfStateVar
   {
      delta[j*cell+ioff+i] -= yt[j*cell+ioff+i];
   }

   /* numdynstatevar <= i < numstatevar */                   // EquStateVar


   // serial zones adjacent to advective zone:
   // ----------------------------------------
     
   parzone = parzonelist.First();
   z=1; 
   while (parzone != 0)
   {
      SERZONE* serzone = parzone->SerialZone(0);
      for ( i=0; i<numvolstatevar; i++ )                  // VolStateVar
      {
//         conc[i] = y[j*cell+ioff+z*numstatevar+i]/zonecrosssect[j][z];
         conc[i] = y[j*cell+ioff+z*numstatevar+i];
      }
      for ( i=numvolstatevar; i<numdynstatevar; i++ )    // SurfStateVar
      {
         conc[i] = y[j*cell+ioff+z*numstatevar+i];
      }
      for ( i=numdynstatevar; i<numstatevar; i++ )       // EquStateVar
      {
         conc[i] = y[j*cell+ioff+z*numstatevar+i];
      }
     
      zoneindex = serzone->ZoneIndex();            // ** correction 22.04.96 **
      varsys->Reset();
      for ( i=0; i<numvolstatevar; i++ )
      {
         REAL exchrate = 0.0;
         for ( CARDINAL k=0; k<serzone->numexch; k++ )
         {
            if ( statevar[i] == serzone->exchvar[k] )
            {
//               exchrate = (y[j*cell+ioff+i]/zonecrosssect[j][0]
//                          -(serzone->exchfac[k])->Value()*conc[i])
//                          *(serzone->exchval[k])->Value();
               exchrate = (y[j*cell+ioff+i]
                          -(serzone->exchfac[k])->Value()*conc[i])
                          *(serzone->exchval[k])->Value();
               break;
            }
         }
//         delta[j*cell+ioff+z*numstatevar+i] += exchrate; // serzone z
         delta[j*cell+ioff+z*numstatevar+i] += exchrate/zonecrosssect[j][z]; // serzone z
         delta[j*cell+ioff+z*numstatevar+i]
            -= yt[j*cell+ioff+z*numstatevar+i];
//         delta[j*cell+ioff+i]               -= exchrate; // adv. zone
         delta[j*cell+ioff+i]               -= exchrate/zonecrosssect[j][0]; // adv. zone
      }
      for ( i=numvolstatevar; i<numdynstatevar; i++ )
      {
       delta[j*cell+ioff+z*numstatevar+i] -= yt[j*cell+ioff+z*numstatevar+i];
      }

      // other serial zones:
      // -------------------

      z++;
      for ( CARDINAL sz=1; sz<parzone->NumSerialZones(); sz++)
      {
         serzone = parzone->SerialZone(sz);
         for ( i=0; i<numvolstatevar; i++ )                  // VolStateVar
         {
//            conc[i] = y[j*cell+ioff+z*numstatevar+i]/zonecrosssect[j][z];
            conc[i] = y[j*cell+ioff+z*numstatevar+i];
         }
         for ( i=numvolstatevar; i<numdynstatevar; i++ )    // SurfStateVar
         {
            conc[i] = y[j*cell+ioff+z*numstatevar+i];
         }
         for ( i=numdynstatevar; i<numstatevar; i++ )       // EquStateVar
         {
            conc[i] = y[j*cell+ioff+z*numstatevar+i];
         }
     
         zoneindex = serzone->ZoneIndex();         // ** correction 22.04.96 **
         varsys->Reset();
         for ( i=0; i<numvolstatevar;i++)
         {
            REAL exchrate = 0.0;
            for ( CARDINAL k=0; k<serzone->numexch; k++ )
            {
               if ( statevar[i] == serzone->exchvar[k] )
               {
//                  exchrate = (y[j*cell+ioff+(z-1)*numstatevar+i]
//                             /zonecrosssect[j][z-1]
//                             -(serzone->exchfac[k])->Value()*conc[i])
//                             *(serzone->exchval[k])->Value();
                  exchrate = (y[j*cell+ioff+(z-1)*numstatevar+i]
                             -(serzone->exchfac[k])->Value()*conc[i])
                             *(serzone->exchval[k])->Value();
                  break;
               }
            }
//            delta[j*cell+ioff+z*numstatevar+i]     += exchrate; // serzone z
            delta[j*cell+ioff+z*numstatevar+i]     += exchrate/zonecrosssect[j][z]; // serzone z
            delta[j*cell+ioff+z*numstatevar+i]
               -= yt[j*cell+ioff+z*numstatevar+i]; 
//            delta[j*cell+ioff+(z-1)*numstatevar+i] -= exchrate; // serzone z-1
            delta[j*cell+ioff+(z-1)*numstatevar+i] -= exchrate/zonecrosssect[j][z-1]; // serzone z-1
         } 
         for ( i=numvolstatevar; i<numdynstatevar; i++ )
         {
            delta[j*cell+ioff+z*numstatevar+i]
               -= yt[j*cell+ioff+z*numstatevar+i]; 
         }
         z++;
      }
      parzone = parzone->Next();
   }  
        


   // ------------------------------------------------------
   // equations at the downstream boundary (grid point n-1):
   // ------------------------------------------------------

   if ( Xvar != 0 ) Xvar->SetProgVar(&gridpts[j]);
   if ( Qvar != 0 ) Qvar->SetProgVar(&y[j*cell]);
   if ( Avar != 0 ) Avar->SetProgVar(&areas[j]);

   // advective zone:
   // ---------------

   j = NumGridPts()-1;

   delta[j*cell]   = y[j*cell] - Qs[j-1]                     // Q
                     - 0.5*(qlat[j]+qlat[j-1])
                          *(gridpts[j]-gridpts[j-1]);

   for ( i=0; i<numvolstatevar; i++ )                        // VolStateVar
   {
      if ( withdispersion == TRUE )
      {
         if ( Qs[numgrid-1]/es[numgrid-1]      // provisorische Bedingung !!!
              /fabs(gridpts[numgrid-1]-gridpts[0]) < 1e-8 )
         {
            // dC/dx = 0
//            delta[j*cell+ioff+i] = y[(j-1)*cell+ioff+i]/zonecrosssect[j-1][0]
//                                   - y[j*cell+ioff+i]/zonecrosssect[j][0];
            delta[j*cell+ioff+i] = y[(j-1)*cell+ioff+i]-y[j*cell+ioff+i];
         }
         else
         {
            // d2C/dx2 = 0
//            delta[j*cell+ioff+i] = 2*y[j*cell+ioff+i]/zonecrosssect[j][0]
//                                   -3*y[(j-1)*cell+ioff+i]/zonecrosssect[j-1][0]
//                                   +y[(j-2)*cell+ioff+i]/zonecrosssect[j-2][0];
//            delta[j*cell+ioff+i] = 2*y[j*cell+ioff+i]
//                                   -3*y[(j-1)*cell+ioff+i]
//                                   +y[(j-2)*cell+ioff+i];
            REAL yend = 0.5*(3*y[(j-1)*cell+ioff+i]-y[(j-2)*cell+ioff+i]);
            if ( yend*y[(j-1)*cell+ioff+i] < 0 ) yend = 0;
            delta[j*cell+ioff+i] = yend-y[j*cell+ioff+i];
         }
      }
      else
      {
//         delta[j*cell+ioff+i] += (y[(j-1)*cell]*y[(j-1)*cell+ioff+i]/zonecrosssect[j-1][0]
//                               -y[j*cell]*y[j*cell+ioff+i]/zonecrosssect[j][0])
//                              * 2.0*dxinv;
         delta[j*cell+ioff+i] += (y[(j-1)*cell]*y[(j-1)*cell+ioff+i]
                                 -y[j*cell]*y[j*cell+ioff+i])
                                * 2.0*dxinv/zonecrosssect[j][0];
         delta[j*cell+ioff+i] -= yt[j*cell+ioff+i];
      }
   }

   for ( i=numvolstatevar; i<numdynstatevar; i++ )           // SurfStateVar
   {
      delta[j*cell+ioff+i] -= yt[j*cell+ioff+i];
   }
   /* numdynstatevar <= i < numstatevar */                   // EquStateVar

   // serial zones adjacent to advective zone:
   // ----------------------------------------

   parzone = parzonelist.First();
   z=1; 
   while (parzone != 0)
   {
      SERZONE* serzone = parzone->SerialZone(0);
      for ( i=0; i<numvolstatevar; i++ )                  // VolStateVar
      {
//         conc[i] = y[j*cell+ioff+z*numstatevar+i]/zonecrosssect[j][z];
         conc[i] = y[j*cell+ioff+z*numstatevar+i];
      }
      for ( i=numvolstatevar; i<numdynstatevar; i++ )    // SurfStateVar
      {
         conc[i] = y[j*cell+ioff+z*numstatevar+i];
      }
      for ( i=numdynstatevar; i<numstatevar; i++ )       // EquStateVar
      {
         conc[i] = y[j*cell+ioff+z*numstatevar+i];
      }
     
      zoneindex = serzone->ZoneIndex();            // ** correction 22.04.96 **
      varsys->Reset();
      for ( i=0; i<numvolstatevar; i++ )
      {
         REAL exchrate = 0.0;
         for ( CARDINAL k=0; k<serzone->numexch; k++ )
         {
            if ( statevar[i] == serzone->exchvar[k] )
            {
//               exchrate = (y[j*cell+ioff+i]/zonecrosssect[j][0] 
//                          -(serzone->exchfac[k])->Value()
//                          *conc[i])*(serzone->exchval[k])->Value();
               exchrate = (y[j*cell+ioff+i] 
                          -(serzone->exchfac[k])->Value()
                          *conc[i])*(serzone->exchval[k])->Value();
               break;
            }
         }
//         delta[j*cell+ioff+z*numstatevar+i] += exchrate; // serzone z
         delta[j*cell+ioff+z*numstatevar+i] += exchrate/zonecrosssect[j][z]; // serzone z
         delta[j*cell+ioff+z*numstatevar+i]
            -= yt[j*cell+ioff+z*numstatevar+i];
         if ( withdispersion != TRUE )  // with dispersion alg. equation!
         {
//            delta[j*cell+ioff+i]            -= exchrate; // adv zone
            delta[j*cell+ioff+i]            -= exchrate/zonecrosssect[j][0]; // adv zone
         }
      }

      for ( i=numvolstatevar; i<numdynstatevar; i++ )
      {
         delta[j*cell+ioff+z*numstatevar+i] -= yt[j*cell+ioff+z*numstatevar+i];
      }

      // other serial zones:
      // -------------------

      z++;
      for ( CARDINAL sz=1; sz<parzone->NumSerialZones(); sz++)
      {  
         serzone = parzone->SerialZone(sz);
         for ( i=0; i<numvolstatevar; i++ )                  // VolStateVar
         {
//            conc[i] = y[j*cell+ioff+z*numstatevar+i]/zonecrosssect[j][z];
            conc[i] = y[j*cell+ioff+z*numstatevar+i];
         }
         for ( i=numvolstatevar; i<numdynstatevar; i++ )    // SurfStateVar
         {
            conc[i] = y[j*cell+ioff+z*numstatevar+i];
         }
         for ( i=numdynstatevar; i<numstatevar; i++ )       // EquStateVar
         {
            conc[i] = y[j*cell+ioff+z*numstatevar+i];
         }
     
         zoneindex = serzone->ZoneIndex();         // ** correction 22.04.96 **
         varsys->Reset();
         for ( i=0; i<numvolstatevar;i++)
         {
            REAL exchrate = 0.0;
            for ( CARDINAL k=0; k<serzone->numexch; k++ )
            {
               if ( statevar[i] == serzone->exchvar[k] )
               {
//                  exchrate = (y[j*cell+ioff+(z-1)*numstatevar+i]/zonecrosssect[j][z-1] 
//                             -(serzone->exchfac[k])->Value()
//                             *conc[i])*(serzone->exchval[k])->Value();
                  exchrate = (y[j*cell+ioff+(z-1)*numstatevar+i] 
                             -(serzone->exchfac[k])->Value()
                             *conc[i])*(serzone->exchval[k])->Value();
                  break;
               }
            }
//            delta[j*cell+ioff+z*numstatevar+i]     += exchrate; // serzone z
            delta[j*cell+ioff+z*numstatevar+i]     += exchrate/zonecrosssect[j][z]; // serzone z
            delta[j*cell+ioff+z*numstatevar+i] 
               -= yt[j*cell+ioff+z*numstatevar+i];
//            delta[j*cell+ioff+(z-1)*numstatevar+i] -= exchrate; // serzone z-1
            delta[j*cell+ioff+(z-1)*numstatevar+i] -= exchrate/zonecrosssect[j][z-1]; // serzone z-1
         }
         for ( i=numvolstatevar; i<numdynstatevar; i++ )
         {
            delta[j*cell+ioff+z*numstatevar+i]
               -= yt[j*cell+ioff+z*numstatevar+i]; 
         }
         z++;
      }
      parzone = parzone->Next();
   }
        
   delta[NumGridPts()*cell] = y[NumGridPts()*cell]-es[NumGridPts()-1];  // E

   for ( i=0; i<numstatevar; i++ ) statevar[i]->ResetStateVar();
   delete [] conc;
   delete [] flux;
   delete [] es;
   delete [] Qs;
   delete [] areas;
   delete [] qlat;

   for ( j=0; j<numgrid;j++)
   {
      delete [] zonecrosssect[j];
   }
   delete [] zonecrosssect;

/*
cout << "\nDelta: " << Symbol();
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
   cout << "  y=" << y[i]
        << "  dy/dt=" << yt[i]
        << "  delta=" << delta[i];
}
cout.flush();
*/

   return TRUE;
}


BOOLEAN SOILCOMP::GridValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                           REAL* t_ptr, const REAL* Y,
                           CARDINAL zone, CARDINAL pt, AQVAR* var,
                           REAL& value)
{
   if ( Active() != TRUE ) return FALSE;
   SetGlobalState(varsys,calcnum_ptr,t_ptr,Y);
   CARDINAL i;
   if ( pt >= NumGridPts() )          return FALSE;
   if ( varsys->Exist(var) == FALSE ) return FALSE;
   if ( AllowedVar(var) == FALSE )    return FALSE;
   if ( Active() == FALSE )           return FALSE;
   const REAL* y = &Y[ind_y];
   REAL cn = *calcnum_ptr; 
   REAL compindex = (REAL)cind;
   REAL zoneindex = 0.0;
   varsys->SetProgVar(CalcNumber,&cn);
   varsys->SetProgVar(CompIndex,&compindex);
   varsys->SetProgVar(ZoneIndex,&zoneindex);
   varsys->SetProgVar(Time,t_ptr);
   REAL eps = 1.0; varsys->SetProgVar(WaterFraction,&eps);
   Xvar = varsys->GetProgVar(SpaceX);
   Avar = varsys->GetProgVar(CrossSection);
   Qvar = varsys->GetProgVar(Discharge);
   REAL* conc = new REAL[numstatevar];
   for ( i=0; i<numstatevar; i++) statevar[i]->SetStateVar(&conc[i]);

   CARDINAL cell = ioff+numstatevar*numzones;
   varsys->Reset();
   if ( Xvar != 0 ) Xvar->SetProgVar(&gridpts[pt]);
   if ( Qvar != 0 ) Qvar->SetProgVar(&y[pt*cell]);
   REAL a;
   a = A->Value();
   if ( Avar != 0 ) Avar->SetProgVar(&a);

   if ( zone == 0 )   // advective zone
   {
      zoneindex = 0;

      for ( i=0; i<numvolstatevar; i++ )
      {
         conc[i] = y[pt*cell+ioff+i];
      }
      for ( i=numvolstatevar; i<numdynstatevar; i++ )
      {
          conc[i] = y[pt*cell+ioff+i];
      }
      for ( i=numdynstatevar; i<numstatevar; i++ )
      {
           conc[i] = y[pt*cell+ioff+i];
      }

      value = var->Value();
   } 
   else  // serial zones
   {
      PARZONE* parzone = parzonelist.First();
      CARDINAL sz = zone-1;

      while (parzone != 0)
      {
         if ( sz < parzone->NumSerialZones() )
         {
             zoneindex = (parzone->SerialZone(sz))->zind;
             for ( i=0; i<numvolstatevar; i++ )
             {
                conc[i] = y[pt*cell+ioff+numstatevar*zone+i];
             }
             for ( i=numvolstatevar; i<numdynstatevar; i++ )
             {
                conc[i] = y[pt*cell+ioff+numstatevar*zone+i];
             }
             for ( i=numdynstatevar; i<numstatevar; i++ )
             {
                conc[i] = y[pt*cell+ioff+numstatevar*zone+i];
             }

             value = var->Value();
             break;
         }
         else
         {
             sz -= parzone->NumSerialZones();
         }
         parzone = parzone->Next();
      }
      if ( parzone == 0 ) return FALSE;
   }

   for ( i=0; i<numstatevar; i++ ) statevar[i]->ResetStateVar();
   delete [] conc;
   return TRUE;
}


BOOLEAN SOILCOMP::SpaceValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                            REAL* t_ptr, const REAL* Y,
                            CARDINAL zone, REAL xx, BOOLEAN xrel,
                            AQVAR* var,
                            REAL& value)
{
   if ( Active() != TRUE ) return FALSE;
   REAL x = xx;
   if ( xrel == TRUE )
   {
      x = gridpts[0] + xx*(gridpts[numgrid-1]-gridpts[0]);
   }
   REAL r = (x-gridpts[0])/(gridpts[numgrid-1]-gridpts[0])*(numgrid-2);
   INTEGER i = (INTEGER) r;
   if ( gridpts[0] < gridpts[numgrid-1] )
   {
      if ( (x<gridpts[0]) || (x>gridpts[numgrid-1]) ) return FALSE;
      if ( x <= gridpts[i] )
      {
         while ( x < gridpts[i] )
         {
            i--;
            if ( i < 0 )
               FatalError("SOILCOMP::Value: index negative");
         }
      }
      else
      {
         while ( x > gridpts[i+1] )
         {
            i++;
            if ( i >= numgrid-1 )
               FatalError("SOILCOMP::Value: index too large");
         }
      }
   }
   else
   {
      if ( (x>gridpts[0]) || (x<gridpts[numgrid-1]) ) return FALSE;
      if ( x >= gridpts[i] )
      {
         while ( x > gridpts[i] )
         {
            i--;
            if ( i < 0 )
               FatalError("SOILCOMP::Value: index negative");
         }
      }
      else
      {
         while ( x < gridpts[i+1] )
         {
            i++;
            if ( i >= numgrid-1 )
               FatalError("SOILCOMP::Value: index too large");
         }
      }
   }
   if ( i == numgrid-1 )
   {
      if ( GridValue(varsys,calcnum_ptr,t_ptr,Y,zone,i,var,value) == FALSE )
                                                                return FALSE;
   }
   else
   {      
      CARDINAL i1, i2; REAL val1, val2;
      i1 = i; i2 = i+1;
      if ( GridValue(varsys,calcnum_ptr,t_ptr,Y,zone,i1,var,val1) == FALSE )
                                                                return FALSE;
      if ( GridValue(varsys,calcnum_ptr,t_ptr,Y,zone,i2,var,val2) == FALSE )
                                                                return FALSE;
      value = ( (x-gridpts[i1])*val2 + (gridpts[i2]-x)*val1 )
              / (gridpts[i2]-gridpts[i1]);
   }
   return TRUE;
}


CARDINAL SOILCOMP::InitZone(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("SOILCOMP::InitZone: Illegal index");
   return initzone[index];
}


AQVAR* SOILCOMP::InitVar(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("SOILCOMP::InitVar: Illegal index");
   return initvar[index];
}


const FORMVAR* SOILCOMP::InitVal(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("SOILCOMP::InitVal: Illegal index");
   return initval[index];
}


BOOLEAN SOILCOMP::AddInitCond(const VARLIST* varlist, CARDINAL zone,
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


BOOLEAN SOILCOMP::ReplaceInitCond(const VARLIST* varlist, CARDINAL zone,
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


BOOLEAN SOILCOMP::DeleteInitCond(CARDINAL pos)
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


BOOLEAN SOILCOMP::Inflow(const VARLIST* varlist,
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


AQVAR* SOILCOMP::InputVar(CARDINAL index) const
{
   if ( index >= numinput )
      FatalError("SOILCOMP::InputVar: Illegal index");
   return inputvar[index];
}


const FORMVAR* SOILCOMP::InputFlux(CARDINAL index) const
{
   if ( index >= numinput )
      FatalError("SOILCOMP::InputFlux: Illegal index");
   return inputflux[index];
}


BOOLEAN SOILCOMP::AddInput(const VARLIST* varlist, AQVAR* var,
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


BOOLEAN SOILCOMP::ReplaceInput(const VARLIST* varlist, AQVAR* var,
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


BOOLEAN SOILCOMP::DeleteInput(CARDINAL pos)
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


BOOLEAN SOILCOMP::AccQ(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccQ = relacc;
   absaccQ = absacc;
   return TRUE;
}


BOOLEAN SOILCOMP::AccD(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccD = relacc;
   absaccD = absacc;
   return TRUE;
}


void SOILCOMP::CalcArg()
{
   CARDINAL i,j;

   AQCOMP::CalcArg();
   CARDINAL num = numvararg;
   numvararg += numinit;
   for ( i=0; i<numinit; i++ ) numvararg += (initval[i])->NumVarArg();
   if ( inflow != 0 ) numvararg += inflow->NumVarArg();
   numvararg += numinput;
   for ( i=0; i<numinput; i++ ) numvararg += (inputflux[i])->NumVarArg();
   if ( latinflow != 0 ) numvararg += latinflow->NumVarArg();
   numvararg += numlatinput;
   for ( j=0; j<numlatinput; j++ )
      numvararg += latinputconc[j]->NumVarArg();
   if ( A != 0 )    numvararg += A->NumVarArg();
   if ( advvolfract != 0 )   numvararg += advvolfract->NumVarArg();
   if ( E != 0 )    numvararg += E->NumVarArg();
   PARZONE* zone = parzonelist.First();
   while ( zone != 0 )
   {
      numvararg += zone->NumVarArg();
      zone = zone->Next();
   }
   if ( numvararg > 0 )
   {
      AQVAR** newarg = new AQVAR*[numvararg];
      CARDINAL k = 0;
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
      if ( latinflow != 0 )
      {
         for ( i=0; i<latinflow->NumVarArg(); i++ )
         {
            newarg[k] = latinflow->VarArg(i); k++;
         }
      }
      for ( i=0; i<numlatinput; i++ )
      {
         newarg[k] = latinputvar[i]; k++;
         for ( j=0; j<latinputconc[i]->NumVarArg(); j++ )
         {
            newarg[k] = latinputconc[i]->VarArg(j); k++;
         }
      }
      if ( A != 0 )
      {
         for ( i=0; i<A->NumVarArg(); i++ )
         {
            newarg[k] = A->VarArg(i); k++;
         }
      }
      if ( advvolfract != 0)
      {
         for ( i=0; i<advvolfract->NumVarArg(); i++ )
         { 
            newarg[k] = advvolfract->VarArg(i); k++;
         }
      }
      if ( E != 0 )
      {
         for ( i=0; i<E->NumVarArg(); i++ )
         {
            newarg[k] = E->VarArg(i); k++;
         }
      }
      zone = parzonelist.First();
      while ( zone != 0 )
      {
         for ( i=0; i<zone->NumVarArg(); i++ )
         {
            newarg[k] = zone->VarArg(i); k++;
         }
         zone = zone->Next();
      }
      if ( k != numvararg )
         FatalError("SOILCOMP::CalcArg: inconsistent number of arguments");
      delete [] vararg; vararg = newarg;
   }
   return;
}



// ===========================================================================

BOOLEAN SOILCOMP::SetGridPts(CARDINAL n)
{
   if ( n < 5 )        return FALSE;
   if ( n == numgrid ) return TRUE;
   numgrid = n;
   delete gridpts; gridpts = new REAL[numgrid];
   XStartEnd(Xs,Xe);
   return TRUE;
}



// ===========================================================================

BOOLEAN SOILCOMP::NumGridPts(CARDINAL n)
{
   if ( InList() == TRUE ) return FALSE;
   return SetGridPts(n);
}



// ==========================================================================

REAL SOILCOMP::GridPt(const REAL*, CARDINAL index)
{
   if ( index >= NumGridPts() )
      FatalError("SOILCOMP::GridPt: illegal index");
   return gridpts[index];
}



// ==========================================================================

CARDINAL SOILCOMP::NumZoneGridPts(CARDINAL zone)
{
   if ( zone > numzones )
      FatalError("SOILCOMP::NumZoneGridPts: illegal zone");
   return NumGridPts();
}




// ==========================================================================

REAL SOILCOMP::ZoneGridPt(const REAL* Y, CARDINAL zone,
                         CARDINAL index)
{
   if ( zone > numzones )
      FatalError("SOILCOMP::ZoneGridPt: illegal zone");
   return GridPt(Y,index);
}



// ==========================================================================

BOOLEAN SOILCOMP::XStartEnd(REAL xs, REAL xe)
{
   if ( xs == xe ) return FALSE;
   Xs = xs; Xe = xe;
   REAL dx = (Xe-Xs)/(numgrid-2);
   gridpts[0] = Xs; gridpts[numgrid-1] = Xe;
   for ( CARDINAL i=1; i<numgrid-1; i++ ) gridpts[i] = Xs + (i-0.5)*dx;
   return TRUE;
}



// ==========================================================================

BOOLEAN SOILCOMP::CrossSect(const char* inpline,
                            const VARLIST* varlist,
                            char* errorline,
                            CARDINAL& numparseerrors)
{
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("A");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar; return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar; return FALSE;
   }

   // no dependence on a State Variable:

   if ( formvar->ArgVarType(StateVar) == TRUE )
   {
      delete formvar; return FALSE;
   }

   delete A; A = formvar;
   CalcArg();
   return TRUE;
}




// =============================================================================

   
BOOLEAN SOILCOMP::AdvVolFract(const char* inpline,
                            const VARLIST* varlist,
                            char* errorline,
                            CARDINAL& numparseerrors)
{
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("advvolfract");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar; return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar; return FALSE;
   }

   // no dependence on a State Variable:

   if ( formvar->ArgVarType(StateVar) == TRUE )
   {
      delete formvar; return FALSE;
   }

   delete advvolfract; advvolfract = formvar;
   CalcArg();
   return TRUE;
}

// ===========================================================================


BOOLEAN SOILCOMP::Dispersion(const char* inpline,
                            const VARLIST* varlist,
                            char* errorline,
                            CARDINAL& numparseerrors)
{
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' )
   {
      if ( WithDispersion() == FALSE )
      {
         delete E; E = 0;
      }
      return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("E");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;
      if ( WithDispersion() == FALSE )
      {
         delete E; E = 0;
      }
      return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;
      if ( WithDispersion() == FALSE )
      {
         delete E; E = 0;
      }
      return FALSE;
   }

   delete E; E = formvar;
   CalcArg();
   return TRUE;
}



// ===========================================================================

BOOLEAN SOILCOMP::WithDispersion(BOOLEAN b)
{
   if ( b==TRUE && E==0 ) return FALSE;
   withdispersion = b;
   return TRUE;
}



// ===========================================================================

CARDINAL SOILCOMP::NumZones() const
{
   return numzones;
}



// ==========================================================================

const char* SOILCOMP::ZoneName(CARDINAL index) const
{
   if ( index >= numzones )
      FatalError("SOILCOMP::ZoneName: Illegal index");
   if ( index == 0 ) return ini.T(55);
   PARZONE* parzone = parzonelist.First();
   CARDINAL i = 1;
   while ( parzone != 0 )
   {
      CARDINAL nserzones = parzone->NumSerialZones();
      if ( index < i+nserzones )
      {
         CARDINAL num = index-i+1;
         char Buf[BufSize];
         strcpy(namebuffer,parzone->Symbol());
         NumToString(num,Buf,BufSize);
         strcat(namebuffer,Buf);
         return namebuffer;
      }
      else
      {
         i += nserzones;
      }
      parzone = parzone->Next();
   }
   FatalError("SOILCOMP::ZoneName: Illegal index");
   return 0;
}



// ==========================================================================

const char* SOILCOMP::AdvInName(CARDINAL conn) const
{
   if ( conn > 0 ) FatalError("SOILCOMP::AdvInName: Illegal connection");
   return ini.T(78);
}



// ==========================================================================

const char* SOILCOMP::AdvOutName(CARDINAL conn) const
{
   if ( conn > 0 ) FatalError("SOILCOMP::AdvOutName: Illegal connection");
   return ini.T(79);
}



// ==========================================================================

const char* SOILCOMP::DiffName(CARDINAL) const
{
   FatalError("SOILCOMP::DiffName: Illegal connection");
   return 0;
}


BOOLEAN SOILCOMP::AdvInExZ(CARDINAL) const
{
   return FALSE;
}


REAL SOILCOMP::AdvInZ(const REAL*, CARDINAL) const
{
   FatalError("SOILCOMP::AdvInZ: Illegal connection");
   return 0;
}


REAL SOILCOMP::AdvOutQ(VARSYS* varsys, const REAL* Y, CARDINAL conn)
{
   if ( conn > 0 ) FatalError("SOILCOMP::AdvOutQ: Illegal connection");
   if ( Active() == FALSE ) return 0.0;
   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   const REAL* y = &Y[ind_y];
   Qout = y[(NumGridPts()-1)*(ioff+numstatevar*numzones)];
   varsys->SetProgVar(Discharge,&Qout);
   if ( numCout != numvolstatevar )
   {
      delete [] Cout; Cout = new REAL[numvolstatevar];
      numCout = numvolstatevar;
   }
   CARDINAL i;
   for ( i=0; i<numvolstatevar; i++ )
   {
//      Cout[i] = y[(NumGridPts()-1)*(ioff+numstatevar*numzones)+ioff+i]
//                / endcrosssect;
      Cout[i] = y[(NumGridPts()-1)*(ioff+numstatevar*numzones)+ioff+i];
      statevar[i]->SetStateVar(&Cout[i]);
   }
   for ( i=numvolstatevar; i<numstatevar; i++ )
   {
      statevar[i]->ResetStateVar();
   }
   return y[(NumGridPts()-1)*(ioff+numstatevar*numzones)];
}


REAL SOILCOMP::AdvOutJ(VARSYS* varsys,
                      const REAL* Y,
                      CARDINAL conn,
                      const AQVAR* var)
{
   if ( conn > 0 ) FatalError("SOILCOMP::AdvOutJ: Illegal connection");
   if ( Active() == FALSE ) return 0.0;
   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   const REAL* y = &Y[ind_y];
   CARDINAL i;
   Qout = y[(NumGridPts()-1)*(ioff+numstatevar*numzones)];
   varsys->SetProgVar(Discharge,&Qout);
   if ( numCout != numvolstatevar )
   {
      delete [] Cout; Cout = new REAL[numvolstatevar];
      numCout = numvolstatevar;
   }
   for ( i=0; i<numvolstatevar; i++ )
   {
//      Cout[i] = y[(NumGridPts()-1)*(ioff+numstatevar*numzones)+ioff+i]
//                / y[(NumGridPts()-1)*(ioff+numstatevar*numzones)+1];
      Cout[i] = y[(NumGridPts()-1)*(ioff+numstatevar*numzones)+ioff+i];
      statevar[i]->SetStateVar(&Cout[i]);
   }
   for ( i=numvolstatevar; i<numstatevar; i++ )
   {
      statevar[i]->ResetStateVar();
   }
   for ( i=0; i<numvolstatevar; i++ )
   {
      if ( var == statevar[i] )
      {
//         REAL flux =    y[(NumGridPts()-1)*(ioff+numstatevar*numzones)+ioff+i]
//                      / endcrosssect
//                      * y[(NumGridPts()-1)*(ioff+numstatevar*numzones)];
         REAL flux =    y[(NumGridPts()-1)*(ioff+numstatevar*numzones)+ioff+i]
                      * y[(NumGridPts()-1)*(ioff+numstatevar*numzones)];
         if ( withdispersion == TRUE )
         {
//            flux -=   y[(numgrid-1)*(ioff+numstatevar*numzones)+1]
//                    * y[numgrid*(ioff+numstatevar*numzones)]
//                    * (  y[(numgrid-1)*(ioff+numstatevar*numzones)+ioff+i]
//                        /endcrosssect
//                       - y[(numgrid-2)*(ioff+numstatevar*numzones)+ioff+i]
//                        /preendcrosssect  )
//                    / fabs(gridpts[numgrid-1]-gridpts[numgrid-2]);
            flux -=   y[(numgrid-1)*(ioff+numstatevar*numzones)+1]
                    * y[numgrid*(ioff+numstatevar*numzones)]
                    * (  y[(numgrid-1)*(ioff+numstatevar*numzones)+ioff+i]
                       - y[(numgrid-2)*(ioff+numstatevar*numzones)+ioff+i])
                    / fabs(gridpts[numgrid-1]-gridpts[numgrid-2]);
         }
         return flux;
      }
   }
   return 0.0;
}


REAL SOILCOMP::DiffC(const REAL*, CARDINAL, const AQVAR*) const
{
   FatalError("SOILCOMP::DiffC: Illegal connection");
   return 0.0;
}


BOOLEAN SOILCOMP::AddParZone(PARZONE* parallel)
{
   // elementary consistency checks:

   if ( parallel == 0 )                            return FALSE;
   if ( ExistParZone(parallel->Symbol()) == TRUE ) return FALSE;

   // add parallel zone:

   if ( parzonelist.Add(parallel) == FALSE )
      FatalError("SOILCOMP::AddParZone: cannot add parallel zone");

   // update variables:

   CalcArg();
   Num_of_Zones();

   // shift initial conditions:

   CARDINAL i;
   CARDINAL pos = parzonelist.Position(parallel);
   if ( pos == 0 ) return FALSE;
   pos--;                 // indices start at zero
   CARDINAL start = 1;    // start after adv. zone
   for ( i=0; i<pos; i++ ) start += numserzones[i];
   CARDINAL range = numserzones[pos];
   for ( i=0; i<numinit; i++ )
   {
      if ( initzone[i] >= start )
      {
         initzone[i] += range;
      }
   }

   return TRUE;
}


BOOLEAN SOILCOMP::AddParZone_compat(PARZONE* parallel)
// this function is only necessary for compatibility with old 
// files with aquavers = -1
// it can be eliminated later
{
   // add parallel zone:

   if ( parallel == 0 )                            return FALSE;
   if ( ExistParZone(parallel->Symbol()) == TRUE ) return FALSE;
   if ( parzonelist.Add(parallel) == FALSE )
      FatalError("SOILCOMP::AddParZone: cannot add parallel zone");

   // update variables:

   CalcArg();
   Num_of_Zones();
   return TRUE;
}


BOOLEAN SOILCOMP::ReplaceParZone(PARZONE* oldparallel, PARZONE* newparallel)
{
   CARDINAL i;

   // elementary consistency checks:

   if ( newparallel == 0 )                                     return FALSE;
   if ( ExistParZone(oldparallel) == FALSE )                   return FALSE;
   if ( ( strcmp(oldparallel->Symbol(),newparallel->Symbol()) != 0 )
        &&   ( ExistParZone(newparallel->Symbol()) == TRUE ) ) return FALSE;

   // determine old index range for initial conditions:

   CARDINAL pos_old = parzonelist.Position(oldparallel);
   if ( pos_old == 0 ) return FALSE;
   pos_old--;                 // indices start at zero
   CARDINAL start_old = 1;    // start after adv. zone
   for ( i=0; i<pos_old; i++ ) start_old += numserzones[i];
   CARDINAL range_old = numserzones[pos_old];
   CARDINAL workpos = numzones;

   // replace parallel zone:

   if ( parzonelist.Remove(oldparallel) == FALSE )
      FatalError("SOILCOMP::Replace: cannot remove old parallel zone");
   if ( parzonelist.Add(newparallel) == FALSE )
      FatalError("SOILCOMP::Replace: cannot insert new parallel zone");
   delete oldparallel;

   // update variables:

   CalcArg();
   Num_of_Zones();

   // determine new index range for initial conditions:

   CARDINAL pos_new = parzonelist.Position(newparallel);
   if ( pos_new == 0 ) 
      FatalError("SOILCOMP::ReplaceParZone: new zone not found");
   pos_new--;                 // indices start at zero
   CARDINAL start_new = 1;    // start after adv. zone
   for ( i=0; i<pos_new; i++ ) start_new += numserzones[i];
   CARDINAL range_new = numserzones[pos_new];
   if ( workpos <= numzones ) workpos = numzones;

   // shift initial conditions:

   for ( i=0; i<numinit; i++ )  // mark init. cond. of edited zone
   {
      if ( initzone[i]>=start_old && initzone[i]<start_old+range_old )
      {
         initzone[i] = workpos+initzone[i]-start_old;
      }
   }
   for ( i=0; i<numinit; i++ )  // remove space of old init. cond.
   {
      if ( initzone[i]>=start_old+range_old && initzone[i]<workpos )
      {
         initzone[i] -= range_old;
      }
   }
   for ( i=0; i<numinit; i++ )  // make space for new init. cond.
   {
      if ( initzone[i]>=start_new && initzone[i]<workpos )
      {
         initzone[i] += range_new;
      }
   }
   i = numinit;
   while ( i > 0 )             // move init. cond. of edited zone
   {
      if ( initzone[i-1] >= workpos )
      {
         if ( initzone[i-1]-workpos < range_new )
         {
            initzone[i-1] = start_new+initzone[i-1]-workpos;
         }
         else
         {
            DeleteInitCond(i-1);
         }
      }
      i--;
   }

   return TRUE;
}


BOOLEAN SOILCOMP::DeleteParZone(PARZONE* parallel)
{
   CARDINAL i;

   // delete and shift initial conditions:

   CARDINAL pos = parzonelist.Position(parallel);
   if ( pos == 0 ) return FALSE;
   pos--;                 // indices start at zero
   CARDINAL start = 1;    // start after adv. zone
   for ( i=0; i<pos; i++ ) start += numserzones[i];
   CARDINAL range = numserzones[pos];

   i = numinit;
   while ( i > 0 )
   {
      if ( initzone[i-1]>=start && initzone[i-1]<start+range )
      {
         DeleteInitCond(i-1);
      }
      i--;
   }
   for ( i=0; i<numinit; i++ )
   {
      if ( initzone[i] >= start+range )
      {
         initzone[i] -= range;
      }
   }

   // delete parallel zone:

   BOOLEAN b = parzonelist.Remove(parallel);
   if ( b == FALSE )
      FatalError("SOILCOMP::DeleteParZone: Unable to delete zone");
   delete parallel;

   // update variables:

   CalcArg();
   Num_of_Zones();

   return TRUE;
}


// ======================================================================

JOBSTATUS SOILCOMP::Load(std::istream& in,
                        const VARLIST* varlist,const PROCLIST* proclist)
	
			
{
   char Buffer[BufSize]; char Buffer1[BufSize];

   if ( LoadStart(in) == FALSE )                                return PROBLEM;
   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )                     return PROBLEM;
   switch ( aquavers )
   {
      case -1: // aquavers=-1: development version june 1995
         {
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

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( strlen(Buffer) > 0 )
            {
               if ( LatInflow(varlist,Buffer,Buffer1) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddLatInput(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }
            if ( LoadNumber(in,&n) == FALSE )                   return PROBLEM;
            if ( NumGridPts(n) == FALSE )                       return PROBLEM;
            if ( LoadBoolean(in,&highres) == FALSE )            return PROBLEM;
            REAL xs,xe;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( XStartEnd(xs,xe) == FALSE )                    return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;

            if (LoadString(in,Buffer,sizeof(Buffer)) == FALSE)  return PROBLEM;
            if ( AdvVolFract(Buffer,varlist, Buffer1,n) == FALSE )
                                                                return PROBLEM;

            if ( LoadBoolean(in,&withdispersion) == FALSE )     return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Dispersion(Buffer,varlist,Buffer1,n) == FALSE )
            {
               if ( withdispersion==TRUE )                      return PROBLEM;
            }

            if (LoadStart(in) == FALSE)                         return PROBLEM;
            while (1) 
            {
               PARZONE* parallel = new PARZONE ;
               if ((parallel->Load(in, varlist) == PROBLEM))
               {
	          delete parallel;
	          break;
               }
               if (AddParZone_compat(parallel) == FALSE)
               {
         	 delete parallel;
         	 return PROBLEM;
               }
            }
         }
         break;
      case 1: // aquavers=1: AQUASIM 1.1
         {
            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,1);
                                            if ( status != OK ) return status;

            CARDINAL n;
            if ( LoadNumber(in,&n) == FALSE )                   return PROBLEM;
            if ( NumGridPts(n) == FALSE )                       return PROBLEM;
            if ( LoadBoolean(in,&highres) == FALSE )            return PROBLEM;
            REAL xs,xe;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( XStartEnd(xs,xe) == FALSE )                    return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;

            if (LoadString(in,Buffer,sizeof(Buffer)) == FALSE)  return PROBLEM;
            if ( AdvVolFract(Buffer,varlist, Buffer1,n) == FALSE )
                                                                return PROBLEM;

            if ( LoadBoolean(in,&withdispersion) == FALSE )     return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Dispersion(Buffer,varlist,Buffer1,n) == FALSE )
            {
               if ( withdispersion==TRUE )                      return PROBLEM;
            }

            if (LoadStart(in) == FALSE)                         return PROBLEM;
            while (1) 
            {
               PARZONE* parallel = new PARZONE ;
               if ((parallel->Load(in, varlist) == PROBLEM))
               {
	          delete parallel;
	          break;
               }
               if (AddParZone(parallel) == FALSE)
               {
         	 delete parallel;
         	 return PROBLEM;
               }
            }

            CARDINAL zone;
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
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

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( strlen(Buffer) > 0 )
            {
               if ( Inflow(varlist,Buffer,Buffer1) == FALSE )   return PROBLEM;
            }
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddInput(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( strlen(Buffer) > 0 )
            {
               if ( LatInflow(varlist,Buffer,Buffer1) == FALSE )
                                                                return PROBLEM;
            }
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddLatInput(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }
         }
         break;
      case 2: // aquavers=2: compartment index added 31.01.96
         {
            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,2);
                                            if ( status != OK ) return status;

            CARDINAL n;
            if ( LoadNumber(in,&n) == FALSE )                   return PROBLEM;
            if ( NumGridPts(n) == FALSE )                       return PROBLEM;
            if ( LoadBoolean(in,&highres) == FALSE )            return PROBLEM;
            REAL xs,xe;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( XStartEnd(xs,xe) == FALSE )                    return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;

            if (LoadString(in,Buffer,sizeof(Buffer)) == FALSE)  return PROBLEM;
            if ( AdvVolFract(Buffer,varlist, Buffer1,n) == FALSE )
                                                                return PROBLEM;

            if ( LoadBoolean(in,&withdispersion) == FALSE )     return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Dispersion(Buffer,varlist,Buffer1,n) == FALSE )
            {
               if ( withdispersion==TRUE )                      return PROBLEM;
            }

            if (LoadStart(in) == FALSE)                         return PROBLEM;
            while (1) 
            {
               PARZONE* parallel = new PARZONE ;
               if ((parallel->Load(in, varlist) == PROBLEM))
               {
	          delete parallel;
	          break;
               }
               if (AddParZone(parallel) == FALSE)
               {
         	 delete parallel;
         	 return PROBLEM;
               }
            }

            CARDINAL zone;
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
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

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( strlen(Buffer) > 0 )
            {
               if ( Inflow(varlist,Buffer,Buffer1) == FALSE )   return PROBLEM;
            }
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddInput(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( strlen(Buffer) > 0 )
            {
               if ( LatInflow(varlist,Buffer,Buffer1) == FALSE )
                                                                return PROBLEM;
            }
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddLatInput(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }
         }
         break;
      case 3: // aquavers=3: AQUASIM 2.0 gamma may 1997, accuracies introduced
         {
            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,2);
                                            if ( status != OK ) return status;

            CARDINAL n;
            if ( LoadNumber(in,&n) == FALSE )                   return PROBLEM;
            if ( NumGridPts(n) == FALSE )                       return PROBLEM;
            if ( LoadBoolean(in,&highres) == FALSE )            return PROBLEM;
            REAL relacc,absacc;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccQ(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccD(relacc,absacc) == FALSE )                 return PROBLEM;
            REAL xs,xe;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( XStartEnd(xs,xe) == FALSE )                    return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;

            if (LoadString(in,Buffer,sizeof(Buffer)) == FALSE)  return PROBLEM;
            if ( AdvVolFract(Buffer,varlist, Buffer1,n) == FALSE )
                                                                return PROBLEM;

            if ( LoadBoolean(in,&withdispersion) == FALSE )     return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Dispersion(Buffer,varlist,Buffer1,n) == FALSE )
            {
               if ( withdispersion==TRUE )                      return PROBLEM;
            }

            if (LoadStart(in) == FALSE)                         return PROBLEM;
            while (1) 
            {
               PARZONE* parallel = new PARZONE ;
               if ((parallel->Load(in, varlist) == PROBLEM))
               {
	          delete parallel;
	          break;
               }
               if (AddParZone(parallel) == FALSE)
               {
         	 delete parallel;
         	 return PROBLEM;
               }
            }

            CARDINAL zone;
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
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

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( strlen(Buffer) > 0 )
            {
               if ( Inflow(varlist,Buffer,Buffer1) == FALSE )   return PROBLEM;
            }
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddInput(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( strlen(Buffer) > 0 )
            {
               if ( LatInflow(varlist,Buffer,Buffer1) == FALSE )
                                                                return PROBLEM;
            }
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddLatInput(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }
         }
         break;
      case 4: // aquavers=4: May 1998 BOOLEAN active added
         {
            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,3);
                                            if ( status != OK ) return status;

            CARDINAL n;
            if ( LoadNumber(in,&n) == FALSE )                   return PROBLEM;
            if ( NumGridPts(n) == FALSE )                       return PROBLEM;
            if ( LoadBoolean(in,&highres) == FALSE )            return PROBLEM;
            REAL relacc,absacc;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccQ(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccD(relacc,absacc) == FALSE )                 return PROBLEM;
            REAL xs,xe;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( XStartEnd(xs,xe) == FALSE )                    return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;

            if (LoadString(in,Buffer,sizeof(Buffer)) == FALSE)  return PROBLEM;
            if ( AdvVolFract(Buffer,varlist, Buffer1,n) == FALSE )
                                                                return PROBLEM;

            if ( LoadBoolean(in,&withdispersion) == FALSE )     return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Dispersion(Buffer,varlist,Buffer1,n) == FALSE )
            {
               if ( withdispersion==TRUE )                      return PROBLEM;
            }

            if (LoadStart(in) == FALSE)                         return PROBLEM;
            while (1) 
            {
               PARZONE* parallel = new PARZONE ;
               if ((parallel->Load(in, varlist) == PROBLEM))
               {
	          delete parallel;
	          break;
               }
               if (AddParZone(parallel) == FALSE)
               {
         	 delete parallel;
         	 return PROBLEM;
               }
            }

            CARDINAL zone;
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
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

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( strlen(Buffer) > 0 )
            {
               if ( Inflow(varlist,Buffer,Buffer1) == FALSE )   return PROBLEM;
            }
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddInput(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( strlen(Buffer) > 0 )
            {
               if ( LatInflow(varlist,Buffer,Buffer1) == FALSE )
                                                                return PROBLEM;
            }
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddLatInput(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                                  return PROBLEM;
   return OK;
}


// ========================================================================

JOBSTATUS SOILCOMP::Save(std::ostream& out)
{
   CARDINAL i;
   char     Buffer[BufSize];

   INTEGER aquavers = 4;

   if ( SaveStart(out) == FALSE )                              return PROBLEM;

   if ( SaveNumber(out,aquavers) == FALSE )                    return PROBLEM;

   JOBSTATUS status = AQCOMP::Save(out);   if ( status != OK ) return status;

   if ( SaveNumber(out,numgrid) == FALSE )                     return PROBLEM;
   if ( SaveBoolean(out,HighRes()) == FALSE )                  return PROBLEM;
   if ( SaveNumber(out,RelAccQ()) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,AbsAccQ()) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,RelAccD()) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,AbsAccD()) == FALSE )                   return PROBLEM;

   if ( SaveNumber(out,Xs) == FALSE )                          return PROBLEM;
   if ( SaveNumber(out,Xe) == FALSE )                          return PROBLEM;

   if ( A->UnParse(Buffer,sizeof(Buffer)-1) != 0 )             return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;

   if ( advvolfract->UnParse(Buffer,sizeof(Buffer)-1) != 0)    return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
 
   if ( SaveBoolean(out,WithDispersion()) == FALSE )           return PROBLEM;
   if ( E == 0 )
   {
      if ( WithDispersion() == TRUE )                          return PROBLEM;
      Buffer[0] = '\0';
   }
   else
   {
      if ( E->UnParse(Buffer,sizeof(Buffer)-1) != 0 )          return PROBLEM;
   }
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;

   if (SaveStart(out) == FALSE)                                return PROBLEM;
   PARZONE* parallel = parzonelist.First();

   while ( parallel != 0 )          
   {
      status = parallel->Save(out);         if (status !=  OK) return status;
      parallel = parallel->Next();
   }
   if (SaveEnd(out) == FALSE)                                  return PROBLEM;

   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   for ( i=0; i<numinit; i++ )
   {
      if ( SaveNumber(out,InitZone(i)) == FALSE )              return PROBLEM;
      if ( SaveString(out,InitVar(i)->Symbol()) == FALSE )     return PROBLEM;
      if ( InitVal(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 ) return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                   return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;

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
   if ( latinflow == 0 )
   {
      Buffer[0] = '\0';
   }
   else
   {
      if ( latinflow->UnParse(Buffer,sizeof(Buffer)-1) != 0 )  return PROBLEM;
   }
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   for ( i=0; i<numlatinput; i++ )
   {
      if ( SaveString(out,LatInputVar(i)->Symbol()) == FALSE ) return PROBLEM;
      if ( LatInputConc(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                   return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;

   if ( SaveEnd(out) == FALSE )                                return PROBLEM;
   return OK;
}


// =======================================================================

JOBSTATUS SOILCOMP::Write(std::ostream& out, BOOLEAN sh)
{
   JOBSTATUS status = AQCOMP::Write(out,sh);
                                           if ( status != OK ) return status;
   if ( sh == TRUE ) return OK;

   CARDINAL i;
   char Buffer[BufSize];

   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   CARDINAL pos3 = pos1+2;
   CARDINAL pos4 = pos3+2;

   PrintIndent(pos2-1);

   if ( Print(out,ini.T(182),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( NumInitCond() > 0 )
   {
      PrintIndent(pos4-1);
      if ( Print(out,ini.T(183),pos3) == FALSE )               return PROBLEM;
      if ( Print(out,"(") == FALSE )                           return PROBLEM;
      if ( Print(out,ini.T(184)) == FALSE )                    return PROBLEM;
      if ( Print(out,") : ") == FALSE )                        return PROBLEM;
      if ( Print(out,ini.T(185)) == FALSE )                    return PROBLEM;
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
   if ( Print(out,ini.T(178),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Inflow() != 0 )
   {
      if ( inflow->UnParse(Buffer,sizeof(Buffer)-1) != 0 )     return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;
   }

   if ( Print(out,ini.T(179),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( NumInput() > 0 )
   {
      PrintIndent(pos4-1);
      if ( Print(out,ini.T(180),pos3) == FALSE )               return PROBLEM;
      if ( Print(out," : ") == FALSE )                         return PROBLEM;
      if ( Print(out,ini.T(181)) == FALSE )                    return PROBLEM;
      for ( i=0; i<NumInput(); i++ )
      {
         if ( Print(out,InputVar(i)->Symbol(),pos3) == FALSE ) return PROBLEM;
         if ( Print(out," : ") == FALSE )                      return PROBLEM;
         if ( InputFlux(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer) == FALSE )                     return PROBLEM;
      }
   }

   if ( Print(out,ini.T(80),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( LatInflow() != 0 )
   {
      if ( latinflow->UnParse(Buffer,sizeof(Buffer)-1) != 0 )  return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;

      if ( NumLatInput() > 0 )
      {
         if ( Print(out,ini.T(81),pos1) == FALSE )             return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( Print(out,ini.T(82),pos3) == FALSE )             return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( Print(out,ini.T(83),pos2) == FALSE )             return PROBLEM;
         for ( i=0; i<NumLatInput(); i++ )
         {
            if ( Print(out,LatInputVar(i)->Symbol(),pos3) == FALSE )
                                                               return PROBLEM;
            if ( Print(out,":") == FALSE )                     return PROBLEM;
            if ( LatInputConc(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
            if ( Print(out,Buffer,pos2) == FALSE )             return PROBLEM;
         }
      }
   }

   if ( Print(out,ini.T(84),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,XStart(),pos2) == FALSE )                    return PROBLEM;

   if ( Print(out,ini.T(85),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,XEnd(),pos2) == FALSE )                      return PROBLEM;

   if ( Print(out,ini.T(86),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( A->UnParse(Buffer,sizeof(Buffer)-1) != 0 )             return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                      return PROBLEM;

   if (Print(out,ini.T(121),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( advvolfract->UnParse(Buffer,sizeof(Buffer)-1) != 0 )   return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                      return PROBLEM;

   if ( WithDispersion() == TRUE )
   {
      if ( Print(out,ini.T(87),pos1) == FALSE )                return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( E->UnParse(Buffer,sizeof(Buffer)-1) != 0 )          return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;
   }

   if ( Print(out,ini.T(122),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( parzonelist.Size() > 0 )
   {
      PARZONE* parallel = parzonelist.First();
      while ( parallel != 0 )
      {
         status = parallel->Write(out);    if ( status != OK ) return status;
         parallel = parallel->Next();
      }
   }

   if ( Print(out,ini.T(88),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,NumGridPts(),pos2) == FALSE )                return PROBLEM;
   if ( Print(out," (") == FALSE )                             return PROBLEM;

   if ( HighRes() == FALSE )
   {
      if ( Print(out,ini.T(89)) == FALSE )                     return PROBLEM;
   }
   else
   {
      if ( Print(out,ini.T(90)) == FALSE )                     return PROBLEM;
   }
   if ( Print(out,")") == FALSE )                              return PROBLEM;
   if ( Print(out,ini.T(225),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,ini.T(226),pos1+2) == FALSE )                  return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,RelAccQ(),pos2) == FALSE )                        return PROBLEM;
   if ( Print(out,ini.T(227),pos1+2) == FALSE )                  return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,AbsAccQ(),pos2) == FALSE )                        return PROBLEM;
   if ( Print(out,ini.T(228),pos1+2) == FALSE )                  return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,RelAccD(),pos2) == FALSE )                        return PROBLEM;
   if ( Print(out,ini.T(229),pos1+2) == FALSE )                  return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,AbsAccD(),pos2) == FALSE )                        return PROBLEM;

   return OK;
}

//////////////////////////////////////////////////////////////////////////////
