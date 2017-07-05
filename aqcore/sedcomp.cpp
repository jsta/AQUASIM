///////////////////////////////   sedcomp.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    03.02.01    Peter Reichert
// revisions:   23.01.02    Peter Reichert    Variable sedimentation rate and
//                                            compaction added
//              24.11.03    Peter Reichert    Bottom concentration boundary
//                                            condition added
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// n  = number of grid points
// nv = number of volume state variables
// ns = number of surface state variables
// ne = number of equilibrium state variables
// Cv = volume state variable
// Cs = surface state variable
// Ce = equilibrium state variable
//
// Upstream End:            ================  z[0]
//                             element 1      z[1]
//                          ----------------
//                             element 2      z[2]
//                          ----------------
//                                            z[3]
//                          ----------------
//
//
// 
//                          ----------------
//                             element n-2    z[n-2]
// Downstream End:          ================  z[n-1]
//
// ioff = 2
// cell = ioff + nv+ns+ne
//
// type component                                meaning
// ---------------------------------------------------------------------------
// alg  y[0]                                     Q at z[0]
// alg  y[1]                                     theta at z[0]
// alg  y[ioff]                                  Cv[1] at z[0]
//  .    .                                        .
// alg  y[ioff+nv-1]                             Cv[nv] at z[0]
// diff y[ioff+nv]                               Cs[1] at z[0]
//  .    .                                        .
// diff y[ioff+nv+ns-1]                          Cs[ns] at z[0]
// alg  y[ioff+nv+ns+ne]                         Ce[1] at z[0]
//  .    .                                        .
// alg  y[ioff+nv+ns+ne-1]                       Ce[ne] at z[0]
// ---------------------------------------------------------------------------
// alg  y[cell]                                  Q at z[1]
// alg  y[cell+1]                                theta at z[1]
// diff y[cell+ioff]                             (theta*)Cv[1] at z[1]
//  .    .                                        .
// diff y[cell+ioff+nv-1]                        (theta*)Cv[nv] at z[1]
// diff y[cell+ioff+nv]                          Cs[1] at z[1]
//  .    .                                        .
// diff y[cel+ioff+nv+ns-1]                      Cs[ns] at z[1]
// alg  y[cell+ioff+nv+ns]                       Ce[1] at z[1]
//  .    .                                        .
// alg  y[cell+ioff+nv+ns+ne-1]                  Ce[ne] at z[1]
// ---------------------------------------------------------------------------
//  .    .                                        .
//  .    .                                        .
//
//
// ---------------------------------------------------------------------------
// alg  y[(n-1)*cell]                            Q at z[n-1]
// alg  y[(n-1)*cell+1]                          theta at z[n-1]
// diff y[(n-1)*cell+ioff]                       Cv[1] at z[n-1]
//  .    .                                        .
// diff y[(n-1)*cell+ioff+nv-1]                  Cv[nv] at z[n-1]
// diff y[(n-1)*cell+ioff+nv]                    Cs[1] at z[n-1]
//  .    .                                        .
// diff y[(n-1)*cell+ioff+nv+ns-1]               Cs[ns] at z[n-1]
// alg  y[(n-1)*cell+ioff+nv+ns]                 Ce[1] at z[n-1]
//  .    .                                        .
// alg  y[(n-1)*cell+ioff+nv+ns+ne-1]            Ce[ne] at z[n-1]
//
//////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "fortran.h"
#include "formvar.h"
#include "othervar.h"
#include "varsys.h"
#include "sedcomp.h"
#include "aqlink.h"

//////////////////////////////////////////////////////////////////////////////

const CARDINAL BufSize = 1024;

static char namebuffer[BufSize];

//////////////////////////////////////////////////////////////////////////////

void SEDCOMP::init()
{
   numinit = 0; initzone = 0; initvar = 0; initval = 0;

   numtopbc = 0; topbcvar = 0; topbccorf = 0;

   numbottombc = 0; bottombcvar = 0; bottombcconc = 0;

   Zt = ini.R(23);
   Zb = ini.R(24);
   numpartvar = 0; partvar = 0; partrho = 0; partbioturb = 0;
   numdissvar = 0; dissvar = 0; dissdiff = 0;
   gridpts = 0; SetGridPts(12); SetGridPts(ini.I(1));
   char buf[10];
   SurfPor = new FORMVAR;
   SurfPor->Parse("0.9",0,buf);
   RatePor = new FORMVAR;
   RatePor->Parse("0",0,buf);
   ioff    = 2;

   highres       = FALSE;
   relaccQ       = 1.0e-4;
   absaccQ       = 1.0e-4;
   relaccPor     = 1.0e-4;
   absaccPor     = 1.0e-4;
   numCout = 0;
   Cout    = 0;
   numadvinconn = 0;
   numadvoutconn = 0;
   numdiffconn = 0;
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
}


void SEDCOMP::del()
{
   CARDINAL i;

   delete initzone; initzone = 0;
   delete initvar; initvar = 0;
   for ( i=0; i<numinit; i++ ) delete initval[i];
   delete initval; initval = 0; numinit = 0;

   delete topbcvar;  topbcvar = 0;
   for ( i=0; i<numtopbc; i++ ) delete topbccorf[i];
   delete topbccorf; topbccorf = 0; numtopbc = 0;

   delete bottombcvar;  bottombcvar = 0;
   for ( i=0; i<numbottombc; i++ ) delete bottombcconc[i];
   delete bottombcconc; bottombcconc = 0; numbottombc = 0;

   delete gridpts; gridpts = 0;
      
   delete dissvar; dissvar = 0;
   for ( i=0; i<numdissvar; i++ ) 
   {
      delete dissdiff[i];
   }
   delete dissdiff; dissdiff = 0; 
   numdissvar = 0;
   delete partvar; partvar = 0;
   for ( i=0; i<numpartvar; i++ ) 
   {
      delete partrho[i];
      delete partbioturb[i];
   }
   delete partrho; partrho = 0; 
   delete partbioturb; partbioturb = 0;
   numpartvar = 0;

   delete SurfPor; SurfPor = 0;
   delete RatePor; RatePor = 0;
   delete [] Cout; Cout = 0;
   return;
}


SEDCOMP::SEDCOMP()
{
   init();
}


SEDCOMP::SEDCOMP(const SEDCOMP& com) : AQCOMP(com)
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

   if ( com.numtopbc > 0 )
   {
      numtopbc = com.numtopbc;
      topbcvar = new AQVAR*[numtopbc];
      topbccorf = new FORMVAR*[numtopbc];
      for ( CARDINAL i=0; i<numtopbc; i++ )
      {
         topbcvar[i] = com.topbcvar[i];
         topbccorf[i] = new FORMVAR(com.topbccorf[i]);
      }
   }
   if ( com.numbottombc > 0 )
   {
      numbottombc = com.numbottombc;
      bottombcvar = new AQVAR*[numbottombc];
      bottombcconc = new FORMVAR*[numbottombc];
      for ( CARDINAL i=0; i<numbottombc; i++ )
      {
         bottombcvar[i] = com.bottombcvar[i];
         bottombcconc[i] = new FORMVAR(com.bottombcconc[i]);
      }
   }
   Zt = com.Zt;
   Zb = com.Zb;
   numgrid = com.numgrid;
   if ( gridpts != 0 ) delete [] gridpts;
   gridpts = new REAL[numgrid];
   for ( CARDINAL i=0; i<numgrid; i++ ) gridpts[i] = com.gridpts[i];
   relaccQ       = com.relaccQ;
   absaccQ       = com.absaccQ;
   relaccPor     = com.relaccPor;
   absaccPor     = com.absaccPor;
   if ( com.numpartvar > 0 )
   {
      numpartvar = com.numpartvar;
      partvar = new AQVAR*[numpartvar];
      partrho = new FORMVAR*[numpartvar];
      partbioturb = new FORMVAR*[numpartvar];
      for ( CARDINAL i=0; i<numpartvar; i++ )
      {
         partvar[i] = com.partvar[i];
         partrho[i] = new FORMVAR(com.partrho[i]);
         partbioturb[i] = new FORMVAR(com.partbioturb[i]);
      }
   }
   if ( com.numdissvar > 0 )
   {
      numdissvar = com.numdissvar;
      dissvar = new AQVAR*[numdissvar];
      dissdiff = new FORMVAR*[numdissvar];
      for ( CARDINAL i=0; i<numpartvar; i++ )
      {
         dissvar[i] = com.dissvar[i];
         dissdiff[i] = new FORMVAR(com.dissdiff[i]);
      }
   }
   if ( SurfPor != 0 ) delete SurfPor;
   SurfPor   = new FORMVAR(com.SurfPor);
   if ( RatePor != 0 ) delete RatePor;
   RatePor   = new FORMVAR(com.RatePor);
   ioff = com.ioff;
   highres = com.highres;
}


SEDCOMP::SEDCOMP(const AQCOMP* com) : AQCOMP(com)
{
   init();
   if ( com != 0 )
   {
      if ( com->Type() == SedComp )
      {
         CARDINAL i;
         SEDCOMP* sc = (SEDCOMP*)com;

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

         if ( sc->numtopbc > 0 )
         {
            numtopbc = sc->numtopbc;
            topbcvar = new AQVAR*[numtopbc];
            topbccorf = new FORMVAR*[numtopbc];
            for ( CARDINAL i=0; i<numtopbc; i++ )
            {
               topbcvar[i] = sc->topbcvar[i];
               topbccorf[i] = new FORMVAR(sc->topbccorf[i]);
            }
         }
         if ( sc->numbottombc > 0 )
         {
            numbottombc = sc->numbottombc;
            bottombcvar = new AQVAR*[numbottombc];
            bottombcconc = new FORMVAR*[numbottombc];
            for ( CARDINAL i=0; i<numbottombc; i++ )
            {
               bottombcvar[i] = sc->bottombcvar[i];
               bottombcconc[i] = new FORMVAR(sc->bottombcconc[i]);
            }
         }

         Zt = sc->Zt;
         Zb = sc->Zb;
         numgrid = sc->numgrid;
         if ( gridpts != 0 ) delete [] gridpts;
         gridpts = new REAL[numgrid];
         for ( i=0; i<numgrid; i++ ) gridpts[i] = sc->gridpts[i];
         relaccQ       = sc->relaccQ;
         absaccQ       = sc->absaccQ;
         relaccPor     = sc->relaccPor;
         absaccPor     = sc->absaccPor;
         if ( sc->numpartvar > 0 )
         {
            numpartvar = sc->numpartvar;
            partvar = new AQVAR*[numpartvar];
            partrho = new FORMVAR*[numpartvar];
            partbioturb = new FORMVAR*[numpartvar];
            for ( CARDINAL i=0; i<numpartvar; i++ )
            {
               partvar[i] = sc->partvar[i];
               partrho[i] = new FORMVAR(sc->partrho[i]);
               partbioturb[i] = new FORMVAR(sc->partbioturb[i]);
            }
         }
         if ( sc->numdissvar > 0 )
         {
            numdissvar = sc->numdissvar;
            dissvar = new AQVAR*[numdissvar];
            dissdiff = new FORMVAR*[numdissvar];
            for ( i=0; i<numdissvar; i++ )
            {
               dissvar[i] = sc->dissvar[i];
               dissdiff[i] = new FORMVAR(sc->dissdiff[i]);
            }
         }
         if ( SurfPor != 0 ) delete SurfPor;
         SurfPor = new FORMVAR(sc->SurfPor);
         if ( RatePor != 0 ) delete RatePor;
         RatePor = new FORMVAR(sc->RatePor);
         ioff    = sc->ioff;
         highres = sc->highres;
      }
   }
}


SEDCOMP::~SEDCOMP()
{
   del();
}


const char* SEDCOMP::TypeName() const
{
   return ini.T(270);
}


AQVAR* SEDCOMP::SpaceVar(const VARSYS* varsys) const
{
   return varsys->GetProgVar(SpaceZ);
}


BOOLEAN SEDCOMP::AllowedSpaceVal(REAL z) const
{
   if ( (z<gridpts[0]) && (z<gridpts[numgrid-1]) ) return FALSE;
   if ( (z>gridpts[0]) && (z>gridpts[numgrid-1]) ) return FALSE;
   return TRUE;
}


BOOLEAN SEDCOMP::AllowedProgVarType(PROGVARTYPE type) const
{
   if ( type == CalcNumber )      return TRUE;
   if ( type == Time )            return TRUE;
   if ( type == CompIndex )       return TRUE;
   if ( type == ZoneIndex )       return TRUE;
   if ( type == WaterFraction )   return TRUE;
   if ( type == SpaceZ )          return TRUE;
   if ( type == Discharge )       return TRUE;
   return FALSE;
}


BOOLEAN SEDCOMP::AllowedComp() const
{
   if ( SurfPor == 0 )                               return FALSE;
   if ( RatePor == 0 )                               return FALSE;
   return TRUE;
}


BOOLEAN SEDCOMP::AllowedReplaceVar(const AQVAR* oldvar,
                                   const AQVAR* newvar) const
{
   if ( AQCOMP::AllowedReplaceVar(oldvar,newvar) == FALSE )   return FALSE;
   return TRUE;
}


BOOLEAN SEDCOMP::AllowedExchangeVar(const AQVAR* var1,
                                    const AQVAR* var2) const
{
   if ( AQCOMP::AllowedExchangeVar(var1,var2) == FALSE ) return FALSE;
   return TRUE;
}


void SEDCOMP::ReplVar(AQVAR* oldvar, AQVAR* newvar)
{
   CARDINAL i;

   if ( AllowedReplaceVar(oldvar,newvar) == FALSE )
      FatalError("SEDCOMP::ReplVar: Illegal replace");
   AQCOMP::ReplVar(oldvar,newvar);
   for ( i=0; i<numinit; i++ )
   {
      if ( initvar[i] == oldvar ) initvar[i] = newvar;
      initval[i]->ReplaceVarArg(oldvar,newvar);
   }
   for ( i=0; i<numtopbc; i++ )
   {
      if ( topbcvar[i] == oldvar ) topbcvar[i] = newvar;
      topbccorf[i]->ReplaceVarArg(oldvar,newvar);
   }
   for ( i=0; i<numbottombc; i++ )
   {
      if ( bottombcvar[i] == oldvar ) bottombcvar[i] = newvar;
      bottombcconc[i]->ReplaceVarArg(oldvar,newvar);
   }
   for ( i=0; i<numpartvar; i++ )
   {
      if ( partvar[i] == oldvar ) partvar[i] = newvar;
      partrho[i]->ReplaceVarArg(oldvar,newvar);
      partbioturb[i]->ReplaceVarArg(oldvar,newvar);
   }
   for ( i=0; i<numdissvar; i++ )
   {
      if ( dissvar[i] == oldvar ) dissvar[i] = newvar;
      dissdiff[i]->ReplaceVarArg(oldvar,newvar);
   }
   SurfPor->ReplaceVarArg(oldvar,newvar);
   RatePor->ReplaceVarArg(oldvar,newvar);
   CalcArg();
   return;
}


void SEDCOMP::ExchVar(AQVAR* var1, AQVAR* var2)
{
   CARDINAL i;

   if ( AllowedExchangeVar(var1,var2) == FALSE )
      FatalError("SEDCOMP::ExchVar: Illegal exchange");
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
   for ( i=0; i<numtopbc; i++ )
   {
      if ( topbcvar[i] == var1 )
      {
         topbcvar[i] = var2;
      }
      else
      {
         if ( topbcvar[i] == var2 ) topbcvar[i] = var1;
      }
      topbccorf[i]->ReplaceVarArg(var1,v);
      topbccorf[i]->ReplaceVarArg(var2,var1);
      topbccorf[i]->ReplaceVarArg(v,var2);
   }
   for ( i=0; i<numbottombc; i++ )
   {
      if ( bottombcvar[i] == var1 )
      {
         bottombcvar[i] = var2;
      }
      else
      {
         if ( bottombcvar[i] == var2 ) bottombcvar[i] = var1;
      }
      bottombcconc[i]->ReplaceVarArg(var1,v);
      bottombcconc[i]->ReplaceVarArg(var2,var1);
      bottombcconc[i]->ReplaceVarArg(v,var2);
   }
   for ( i=0; i<numpartvar; i++ )
   {
      if ( partvar[i] == var1 )
      {
         partvar[i] = var2;
      }
      else
      {
         if ( partvar[i] == var2 ) partvar[i] = var1;
      }
      partrho[i]->ReplaceVarArg(var1,v);
      partrho[i]->ReplaceVarArg(var2,var1);
      partrho[i]->ReplaceVarArg(v,var2);
      partbioturb[i]->ReplaceVarArg(var1,v);
      partbioturb[i]->ReplaceVarArg(var2,var1);
      partbioturb[i]->ReplaceVarArg(v,var2);
   }
   for ( i=0; i<numdissvar; i++ )
   {
      if ( dissvar[i] == var1 )
      {
         dissvar[i] = var2;
      }
      else
      {
         if ( dissvar[i] == var2 ) dissvar[i] = var1;
      }
      dissdiff[i]->ReplaceVarArg(var1,v);
      dissdiff[i]->ReplaceVarArg(var2,var1);
      dissdiff[i]->ReplaceVarArg(v,var2);
   }
   SurfPor->ReplaceVarArg(var1,v);
   SurfPor->ReplaceVarArg(var2,var1);
   SurfPor->ReplaceVarArg(v,var2);
   RatePor->ReplaceVarArg(var1,v);
   RatePor->ReplaceVarArg(var2,var1);
   RatePor->ReplaceVarArg(v,var2);
   delete v;
   CalcArg();
   return;
}


AQVAR* SEDCOMP::DissVar(CARDINAL index) const
{
   if ( index >= numdissvar )
      FatalError("SEDCOMP::DissVar: Illegal index");
   return dissvar[index];
}


const FORMVAR* SEDCOMP::Dissdiff(CARDINAL index) const
{
   if ( index >= numdissvar )
      FatalError("SEDCOMP::Dissdiff: Illegal index");
   return dissdiff[index];
}


BOOLEAN SEDCOMP::AddDissVar(const VARLIST* varlist, 
                            AQVAR* var,
                            const char* inpline, 
                            char* errorline,
                            CARDINAL& numparseerrors, 
                            CARDINAL pos)
{
   numparseerrors = 0;
   if ( InList() == TRUE )             return FALSE;
   if ( inpline == 0 )                 return FALSE;
   if ( inpline[0] == '\0' )           return FALSE;
   if ( varlist->Exist(var) == FALSE ) return FALSE;
   if ( pos > numdissvar ) pos = numdissvar;
   CARDINAL i;
   for ( i=0; i<numdissvar; i++ )
   {
      if ( dissvar[i] == var )         return FALSE;
   }
   for ( i=0; i<numpartvar; i++ )
   {
      if ( partvar[i] == var )         return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("diff");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                  return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                  return FALSE;
   }
   AQVAR**   newdissvar = new AQVAR*[numdissvar+1];
   FORMVAR** newdissdiff = new FORMVAR*[numdissvar+1];
   for ( i=0; i<pos; i++ )
   {
      newdissvar[i] = dissvar[i];
      newdissdiff[i] = dissdiff[i];
   }
   newdissvar[pos] = var;
   newdissdiff[pos] = formvar;
   for ( i=pos; i<numdissvar; i++ )
   {
      newdissvar[i+1] = dissvar[i];
      newdissdiff[i+1] = dissdiff[i];
   }
   delete dissvar; dissvar = newdissvar;
   delete dissdiff; dissdiff = newdissdiff;
   numdissvar++;
   CalcArg();
   return TRUE;
}


BOOLEAN SEDCOMP::ReplaceDissVar(const VARLIST* varlist, 
                                AQVAR* var,
                                const char* inpline, 
                                char* errorline,
                                CARDINAL& numparseerrors, 
                                CARDINAL pos)
{
   numparseerrors = 0;
   if ( InList() == TRUE )                 return FALSE;
   if ( inpline == 0 )                     return FALSE;
   if ( inpline[0] == '\0' )               return FALSE;
   if ( varlist->Exist(var) == FALSE )     return FALSE;
   if ( pos >= numdissvar )                return FALSE;
   CARDINAL i;
   for ( i=0; i<numdissvar; i++ )
   {
      if ( i!=pos && dissvar[i]==var )     return FALSE;
   }
   for ( i=0; i<numpartvar; i++ )
   {
      if ( partvar[i] == var )             return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("diff");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                      return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                      return FALSE;
   }
   delete dissdiff[pos];
   dissvar[pos] = var;
   dissdiff[pos] = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN SEDCOMP::DeleteDissVar(CARDINAL pos)
{
   if ( pos >= numdissvar ) return FALSE;
   AQVAR**   newdissvar = new AQVAR*[numdissvar-1];
   FORMVAR** newdissdiff = new FORMVAR*[numdissvar-1];
   CARDINAL i;
   for ( i=0; i<pos; i++ )
   {
      newdissvar[i] = dissvar[i];
      newdissdiff[i] = dissdiff[i];
   }
   for ( i=pos+1; i<numdissvar; i++ )
   {
      newdissvar[i-1] = dissvar[i];
      newdissdiff[i-1] = dissdiff[i];
   }
   delete dissdiff[pos];
   delete dissvar; dissvar = newdissvar;
   delete dissdiff; dissdiff = newdissdiff;
   numdissvar--;
   CalcArg();
   return TRUE;
}


AQVAR* SEDCOMP::PartVar(CARDINAL index) const
{
   if ( index >= numpartvar )
      FatalError("SEDCOMP::PartVar: Illegal index");
   return partvar[index];
}


const FORMVAR* SEDCOMP::PartRho(CARDINAL index) const
{
   if ( index >= numpartvar )
      FatalError("SEDCOMP::PartRho: Illegal index");
   return partrho[index];
}


const FORMVAR* SEDCOMP::PartBioturb(CARDINAL index) const
{
   if ( index >= numpartvar )
      FatalError("SEDCOMP::PartBioturb: Illegal index");
   return partbioturb[index];
}


BOOLEAN SEDCOMP::AddPartVar(const VARLIST* varlist, 
                            AQVAR* var,
                            const char* inprho, 
                            char* errorlinerho,
                            CARDINAL& parseerrorsrho, 
                            const char* inpbioturb, 
                            char* errorlinebioturb,
                            CARDINAL& parseerrorsbioturb, 
                            CARDINAL pos)
{
   parseerrorsrho = 0;
   parseerrorsbioturb = 0;
   if ( InList() == TRUE )             return FALSE;
   if ( inprho == 0 )                  return FALSE;
   if ( inprho[0] == '\0' )            return FALSE;
   if ( inpbioturb == 0 )              return FALSE;
   if ( inpbioturb[0] == '\0' )        return FALSE;
   if ( varlist->Exist(var) == FALSE ) return FALSE;
   if ( pos > numpartvar ) pos = numpartvar;
   CARDINAL i;
   for ( i=0; i<numpartvar; i++ )
   {
      if ( partvar[i] == var )         return FALSE;
   }
   for ( i=0; i<numdissvar; i++ )
   {
      if ( dissvar[i] == var )         return FALSE;
   }
   FORMVAR* varrho = new FORMVAR;
   varrho->Symbol("Rho");
   parseerrorsrho = varrho->Parse(inprho,varlist,errorlinerho);
   if ( parseerrorsrho != 0 )
   {
      delete varrho;                   return FALSE;
   }
   if ( AllowedVar(varrho) == FALSE )
   {
      delete varrho;                   return FALSE;
   }
   FORMVAR* varbioturb = new FORMVAR;
   varbioturb->Symbol("bioturb");
   parseerrorsbioturb = varbioturb->Parse(inpbioturb,varlist,errorlinebioturb);
   if ( parseerrorsbioturb != 0 )
   {
      delete varrho; delete varbioturb; return FALSE;
   }
   if ( AllowedVar(varbioturb) == FALSE )
   {
      delete varrho; delete varbioturb; return FALSE;
   }
   AQVAR**   newpartvar = new AQVAR*[numpartvar+1];
   FORMVAR** newpartrho = new FORMVAR*[numpartvar+1];
   FORMVAR** newpartbioturb = new FORMVAR*[numpartvar+1];
   for ( i=0; i<pos; i++ )
   {
      newpartvar[i] = partvar[i];
      newpartrho[i] = partrho[i];
      newpartbioturb[i] = partbioturb[i];
   }
   newpartvar[pos] = var;
   newpartrho[pos] = varrho;
   newpartbioturb[pos] = varbioturb;
   for ( i=pos; i<numpartvar; i++ )
   {
      newpartvar[i+1] = partvar[i];
      newpartrho[i+1] = partrho[i];
      newpartbioturb[i+1] = partbioturb[i];
   }
   delete partvar; partvar = newpartvar;
   delete partrho; partrho = newpartrho;
   delete partbioturb; partbioturb = newpartbioturb;
   numpartvar++;
   CalcArg();
   return TRUE;
}


BOOLEAN SEDCOMP::ReplacePartVar(const VARLIST* varlist, 
                                AQVAR* var,
                                const char* inprho, 
                                char* errorlinerho,
                                CARDINAL& parseerrorsrho, 
                                const char* inpbioturb, 
                                char* errorlinebioturb,
                                CARDINAL& parseerrorsbioturb, 
                                CARDINAL pos)
{
   parseerrorsrho = 0;
   parseerrorsbioturb = 0;
   if ( InList() == TRUE )                 return FALSE;
   if ( inprho == 0 )                      return FALSE;
   if ( inprho[0] == '\0' )                return FALSE;
   if ( inpbioturb == 0 )                  return FALSE;
   if ( inpbioturb[0] == '\0' )            return FALSE;
   if ( varlist->Exist(var) == FALSE )     return FALSE;
   if ( pos >= numpartvar )                return FALSE;
   CARDINAL i;
   for ( i=0; i<numpartvar; i++ )
   {
      if ( i!=pos && partvar[i]==var ) return FALSE;
   }
   for ( i=0; i<numdissvar; i++ )
   {
      if ( dissvar[i] == var )            return FALSE;
   }
   FORMVAR* varrho = new FORMVAR;
   varrho->Symbol("Rho");
   parseerrorsrho = varrho->Parse(inprho,varlist,errorlinerho);
   if ( parseerrorsrho != 0 )
   {
      delete varrho;                      return FALSE;
   }
   if ( AllowedVar(varrho) == FALSE )
   {
      delete varrho;                      return FALSE;
   }
   FORMVAR* varbioturb = new FORMVAR;
   varbioturb->Symbol("bioturb");
   parseerrorsbioturb = varbioturb->Parse(inpbioturb,varlist,errorlinebioturb);
   if ( parseerrorsbioturb != 0 )
   {
      delete varrho; delete varbioturb; return FALSE;
   }
   if ( AllowedVar(varbioturb) == FALSE )
   {
      delete varrho; delete varbioturb; return FALSE;
   }
   delete partrho[pos];
   delete partbioturb[pos];
   partvar[pos] = var;
   partrho[pos] = varrho;
   partbioturb[pos] = varbioturb;
   CalcArg();
   return TRUE;
}


BOOLEAN SEDCOMP::DeletePartVar(CARDINAL pos)
{
   if ( pos >= numpartvar ) return FALSE;
   AQVAR**   newpartvar = new AQVAR*[numpartvar-1];
   FORMVAR** newpartrho = new FORMVAR*[numpartvar-1];
   FORMVAR** newpartbioturb = new FORMVAR*[numpartvar-1];
   CARDINAL i;
   for ( i=0; i<pos; i++ )
   {
      newpartvar[i] = partvar[i];
      newpartrho[i] = partrho[i];
      newpartbioturb[i] = partbioturb[i];
   }
   for ( i=pos+1; i<numpartvar; i++ )
   {
      newpartvar[i-1] = partvar[i];
      newpartrho[i-1] = partrho[i];
      newpartbioturb[i-1] = partbioturb[i];
   }
   delete partrho[pos];
   delete partbioturb[pos];
   delete partvar; partvar = newpartvar;
   delete partrho; partrho = newpartrho;
   delete partbioturb; partbioturb = newpartbioturb;
   numpartvar--;
   CalcArg();
   return TRUE;
}


CARDINAL SEDCOMP::NumEq() const
{
   CARDINAL num;
   CARDINAL cell = ioff + NumStateVar();
   num = NumGridPts()*cell;
   return num;
}


REAL SEDCOMP::RelAcc(CARDINAL i) const
{
   if ( i >= NumEq() ) FatalError("SEDCOMP::RelAcc: Illegal index");
   CARDINAL cell = ioff + NumStateVar();
   CARDINAL index = i % cell;
   if ( index == 0 ) return relaccQ;
   if ( index == 1 ) return relaccPor;
   CARDINAL ind2 = (index-ioff) % NumStateVar();
   return statevar[ind2]->RelAccuracy();
}


REAL SEDCOMP::AbsAcc(CARDINAL i) const
{
   if ( i >= NumEq() ) FatalError("SEDCOMP::AbsAcc: Illegal index");
   CARDINAL cell = ioff + NumStateVar();
   CARDINAL index = i % cell;
   if ( index == 0 ) return absaccQ;
   if ( index == 1 ) return absaccPor;
   CARDINAL ind2 = (index-ioff) % NumStateVar();
   return statevar[ind2]->AbsAccuracy();
}


EQTYPE SEDCOMP::EqType(CARDINAL i) const
{
   if ( i >= NumEq() ) FatalError("SEDCOMP::EqType: Illegal index");
   CARDINAL cell = ioff + NumStateVar();
   CARDINAL index = i % cell;

   if ( index >= ioff+numdynstatevar ) return ALG;  // EquStatevar
   if ( index >= ioff+numvolstatevar ) return DIFF; // SurfStatevar

   if ( i < ioff+numstatevar )             // Upstream boundary cond.:
   {
      if ( i == 0 ) return ALG;            // Q
      if ( i == 1 ) return ALG;            // Por
      return ALG;                          // VolStatevar
   }

   if ( i >= (numgrid-1)*cell ) // Downstream boundary cond.:
   {
      if ( index == 0 ) return ALG;        // Q
      if ( index == 1 ) return ALG;        // Por
      return ALG;                          // VolStatevar
   }
   else                                    // Equations in cells:
   {
      if ( index == 0 ) return ALG;        // Q
      if ( index == 1 ) return ALG;        // Por
      return DIFF;                         // VolStatevar
   }
}


BOOLEAN SEDCOMP::InitCond(VARSYS* varsys, REAL* Y, CARDINAL callnum)
{
   if ( Active() != TRUE ) return FALSE;
   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   REAL zoneindex = 0.0; varsys->SetProgVar(ZoneIndex,&zoneindex);
   Zvar = varsys->GetProgVar(SpaceZ);
   Qvar = varsys->GetProgVar(Discharge);
   epsvar = varsys->GetProgVar(WaterFraction);
   CARDINAL cell = ioff + NumStateVar();

   if ( callnum>0 ) return TRUE;
   REAL* y = &Y[ind_y];
   CARDINAL i,j,k;

   REAL* conc         = new REAL[numstatevar];
   REAL* Qs           = new REAL[numgrid];
   BOOLEAN* part      = new BOOLEAN[numvolstatevar];
   REAL* rhopart      = new REAL[numvolstatevar];
   CARDINAL* ind      = new CARDINAL[numvolstatevar];
   BOOLEAN found;
   for ( i=0; i<numvolstatevar; i++ )
   {
      found = FALSE;
      for ( j=0; j<NumPartVar(); j++ )
      {
         if ( statevar[i] == partvar[j] )
         {
            k = j;
            found = TRUE;
            break;
         }
      }
      if ( found == TRUE )
      {
         part[i] = TRUE;
         ind[i] = k;
      }
      else
      {
         part[i] = FALSE;
         for ( j=0; j<NumDissVar(); j++ )
         {
            if ( statevar[i] == dissvar[j] )
            {
               k = j;
               found = TRUE;
               break;
            }
         }
         if ( found == TRUE )
         {
            ind[i] = k;
         }
         else
         {
            ind[i] = numvolstatevar;
         }
      }
   }

   varsys->Reset();
   for ( i=0; i<numvolstatevar; i++ )
   {
      if ( part[i] == TRUE )
      {
         rhopart[i] = partrho[ind[i]]->Value();
      }
      else
      {
         rhopart[i] = 0.0;
      }
   }

   for ( i=0; i<numstatevar; i++ ) statevar[i]->SetStateVar(&conc[i]);

   // initialize concentrations:
   // --------------------------

   zoneindex = 0.0;
   for ( i=0; i<numstatevar; i++ )            // Concentrations C
   {
      found = FALSE;
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
            if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[j]);
            y[j*cell+ioff+i] = initval[k]->Value();
         }
         else
         {
            y[j*cell+ioff+i] = 0.0;
         }
      }
   }

   // calculate porosity:
   // -------------------

   for ( j=0; j<NumGridPts(); j++ )
   {
      y[j*cell+1] = 0;
      for ( i=0; i<numvolstatevar; i++ )
	  {
	     if ( part[i] == TRUE )
		 {
			 y[j*cell+1] += y[j*cell+ioff+i]/rhopart[i];
		 }
	  }
	  if ( y[j*cell+1] >= 1 )
      {
         REAL por = 0.9;
         for ( i=0; i<numvolstatevar; i++ )
		 {
	        if ( part[i] == TRUE )
			{
			   y[j*cell+ioff+i] = y[j*cell+ioff+i]*(1-por)/y[j*cell+1];
			}
		 }
	  }
      y[j*cell+1] = 1 - y[j*cell+1];
   }

   // multiply dissolved concentrations with porosity
   // -----------------------------------------------

   for ( i=0; i<numvolstatevar; i++ )
   {
      if ( part[i] == FALSE )
      {
         for ( j=1; j<NumGridPts()-1; j++ )
         {
            y[j*cell+ioff+i] = y[j*cell+ioff+i]*y[j*cell+1];
         }
      }
   }

   // other variables
   // ---------------

   for ( j=0; j<NumGridPts(); j++ )
   {
      y[j*cell]   = 0.0;   // Q          (calculation in Delta)
   }

   delete [] conc;
   delete [] Qs;
   delete [] part;
   delete [] rhopart;
   delete [] ind;

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
   cout << "  y=";
   cout << y[i];
}
cout.flush();
*/

   return TRUE;
}


REAL SEDCOMP::FluxLimit(REAL s1, REAL s2, CARDINAL limiter)
{
   // 0 = first order; 1 = second order

   if ( highres == FALSE )   return 0.0;
   if ( s2 == 0.0 )          return 0.0;

   REAL thet = s1/s2;

   switch ( limiter )
   {
      case 1:  // van Leer:
         if ( thet <= 0.0 ) return 0.0;
         return 2.0*thet/(1.0+thet);
      case 2:  // superbee:
         if ( thet <= 0.0 ) return 0.0;
         if ( thet <= 0.5 ) return 2.0*thet;
         if ( thet <= 1.0 ) return 1.0;
         if ( thet <= 2.0 ) return thet;
         return 2.0;
      default:
         break;
   }
   return 0;
}


BOOLEAN SEDCOMP::Delta(const NUMPAR&, VARSYS* varsys,
                       const REAL* Y, const REAL* YT, REAL* DELTA)
{
   if ( Active() != TRUE ) return FALSE;
   const REAL* y  = &Y[ind_y];
   const REAL* yt = &YT[ind_y];
   REAL* delta = &DELTA[ind_y];
   REAL zero = 0.0;
   CARDINAL i,j,k;
   Zvar = varsys->GetProgVar(SpaceZ);
   Qvar = varsys->GetProgVar(Discharge);
   epsvar = varsys->GetProgVar(WaterFraction);
   CARDINAL cell = ioff + NumStateVar();
   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   REAL zoneindex = 0.0; varsys->SetProgVar(ZoneIndex,&zoneindex);
   REAL eps = 1.0;       varsys->SetProgVar(WaterFraction,&eps);
   REAL* conc               = new REAL[numstatevar];
   REAL* flux               = new REAL[numvolstatevar];
   REAL* inpflux            = new REAL[numstatevar];
   REAL* topconc            = new REAL[numstatevar];
   REAL* bottomconc         = new REAL[numstatevar];
   BOOLEAN* givenbottomconc = new BOOLEAN[numstatevar];
   REAL* rhopart            = new REAL[numstatevar];
   REAL* Qinterf            = new REAL[numgrid-1];
   BOOLEAN* part            = new BOOLEAN[numvolstatevar];
   CARDINAL* ind            = new CARDINAL[numvolstatevar];
   REAL* vsed               = new REAL[numgrid];
   REAL** diffs  = new REAL*[numgrid];
   for ( j=0; j<numgrid; j++ ) diffs[j] = new REAL[numvolstatevar];
   REAL* ratepors     = new REAL[numgrid];
   REAL dz    = fabs(gridpts[2]-gridpts[1]);  // cells of equal length
   REAL dzinv = 1.0/dz;                       // inverse zell length

   // initialize pointers to particulate and dissolved variables:
   // -----------------------------------------------------------

   for ( i=0; i<numvolstatevar; i++ )
   {
      BOOLEAN found = FALSE;
      for ( j=0; j<NumPartVar(); j++ )
      {
         if ( statevar[i] == partvar[j] )
         {
            k = j;
            found = TRUE;
            break;
         }
      }
      if ( found == TRUE )
      {
         part[i] = TRUE;
         ind[i] = k;
      }
      else
      {
         part[i] = FALSE;
         for ( j=0; j<NumDissVar(); j++ )
         {
            if ( statevar[i] == dissvar[j] )
            {
               k = j;
               found = TRUE;
               break;
            }
         }
         if ( found == TRUE )
         {
            ind[i] = k;
         }
         else
         {
            ind[i] = numvolstatevar;
         }
      }
   }

   for ( i=0; i<numvolstatevar; i++ )
   {
      if ( part[i] == TRUE )
      {
         rhopart[i] = partrho[ind[i]]->Value();
      }
      else
      {
         rhopart[i] = 0.0;
      }
   }

   for ( i=0; i<numstatevar; i++ ) statevar[i]->SetStateVar(&conc[i]);

   // calculate top boundary conditions:
   // ----------------------------------

   if ( Zvar != 0 )   Zvar->SetProgVar(&gridpts[0]);
   if ( Qvar != 0 )   Qvar->SetProgVar(&y[0]);
   if ( epsvar != 0 ) epsvar->SetProgVar(&y[1]);
   zoneindex = 0.0;
   varsys->Reset();
   for ( i=0; i<numvolstatevar; i++ )                    // VolStateVar
   {
      conc[i] = y[ioff+i];
   }
   for ( i=numvolstatevar; i<numdynstatevar; i++ )       // SurfStateVar
   {
      conc[i] = y[ioff+i];
   }
   for ( i=numdynstatevar; i<numstatevar; i++ )          // EquStateVar
   {
      conc[i] = y[ioff+i];
   }

   REAL thetasurf = SurfPor->Value();
   vsed[0] = 0.0;
   for ( i=0; i<numvolstatevar; i++ )
   {
      inpflux[i] = 0.0;
	  topconc[i] = 0.0;
      if ( part[i] == TRUE )
	  {
         for ( j=0; j<numtopbc; j++ )
		 {
            if ( topbcvar[j] == statevar[i] )
			{
               inpflux[i] = topbccorf[j]->Value(); break;
			}
		 }
         vsed[0] += inpflux[i]/rhopart[i]/(1-thetasurf);
      }
	  else
	  {
         for ( j=0; j<numtopbc; j++ )
		 {
            if ( topbcvar[j] == statevar[i] )
			{
               topconc[i] = topbccorf[j]->Value(); break;
			}
		 }
	  }
   }
   if ( vsed[0] <= 0.0 )
   {
      *cjob << "\n   SEDCOMP numerical problem:"
            << " surface sediment accumulation velocity must be positive";
      cjob->flush();
      return FALSE;
   }
   for ( i=0; i<numvolstatevar; i++ )
   {
      if ( part[i] == TRUE )
	  {
		 topconc[i] = inpflux[i]/vsed[0];
	  }
   }

   // calculate bottom boundary conditions:
   // -------------------------------------

   if ( Zvar != 0 )   Zvar->SetProgVar(&gridpts[0]);
   if ( Qvar != 0 )   Qvar->SetProgVar(&y[0]);
   if ( epsvar != 0 ) epsvar->SetProgVar(&y[1]);
   zoneindex = 0.0;
   varsys->Reset();
   for ( i=0; i<numvolstatevar; i++ )                    // VolStateVar
   {
      conc[i] = y[(NumGridPts()-1)*cell+ioff+i];
   }
   for ( i=numvolstatevar; i<numdynstatevar; i++ )       // SurfStateVar
   {
      conc[i] = y[(NumGridPts()-1)*cell+ioff+i];
   }
   for ( i=numdynstatevar; i<numstatevar; i++ )          // EquStateVar
   {
      conc[i] = y[(NumGridPts()-1)*cell+ioff+i];
   }

   for ( i=0; i<numvolstatevar; i++ )
   {
	  bottomconc[i] = 0.0;
      givenbottomconc[i] = FALSE;
      if ( part[i] == FALSE )
	  {
         for ( j=0; j<numbottombc; j++ )
		 {
            if ( bottombcvar[j] == statevar[i] )
			{
               bottomconc[i] = bottombcconc[j]->Value();
			   givenbottomconc[i] = TRUE;
			   break;
			}
		 }
	  }
   }

   // calculate RatePor, Rates, Diffusivities, etc.:
   // ----------------------------------------------

   for ( j=0; j<NumGridPts(); j++ )
   {
      if ( Zvar != 0 )   Zvar->SetProgVar(&gridpts[j]);
      if ( Qvar != 0 )   Qvar->SetProgVar(&y[j*cell]);
      if ( epsvar != 0 ) epsvar->SetProgVar(&y[j*cell+1]);
      varsys->Reset();
      if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[j]);
      for ( i=0; i<numvolstatevar; i++ )                    // VolStateVar
      {
         conc[i] = y[j*cell+ioff+i];
         if ( part[i]==FALSE && j>0 && j<numgrid-1 )
         {
            conc[i] = conc[i]/y[j*cell+1];
         }
      }
      for ( i=numvolstatevar; i<numdynstatevar; i++ )       // SurfStateVar
      {
         conc[i] = y[j*cell+ioff+i];
      }
      for ( i=numdynstatevar; i<numstatevar; i++ )          // EquStateVar
      {
         conc[i] = y[j*cell+ioff+i];
      }
      ratepors[j] = RatePor->Value();
      CalcRates(&delta[j*cell+ioff]);
      for ( i=0; i<numvolstatevar; i++ )
      {
         if ( part[i] == TRUE )
         {
            diffs[j][i] = partbioturb[ind[i]]->Value();
         }
         else
         {
            if ( ind[i] == numstatevar )
            {
               diffs[j][i] = 0;
            }
            else
            {
               diffs[j][i] = dissdiff[ind[i]]->Value();
            }
         }
      }
      if ( j > 0 )
	  {
         vsed[j] = vsed[j-1] + ratepors[j]*dz/(1-y[j*cell+1]);
	  }
   }
   for ( j=0; j<numgrid-1; j++ ) Qinterf[j] = 0.0;

   // equations at the top boundary (grid point 0):
   // ---------------------------------------------

   REAL advflux1,advflux2,diffflux;                          // fluxes
   REAL s1, s2;
   REAL phi = 0.0;
   REAL krelax = 100.0;

   delta[0] = y[0] - Qinterf[0];                             // Q

   delta[1] = y[1] - thetasurf;                              // Porosity

   for ( i=0; i<numvolstatevar; i++ )                        // VolStateVar
   {
      delta[ioff+i] = topconc[i] - y[ioff+i];

      // calculate flux over top boundary:
      if ( part[i] == FALSE )
      {
         flux[i] = -Qinterf[1]*y[ioff+i];
         flux[i] += -diffs[0][i]
                     *(y[cell+ioff+i]/y[cell+1]-y[ioff+i])
                     *2*dzinv*y[1];
      }
      else
      {
         flux[i] = inpflux[i];
      }
   }

   for ( i=numvolstatevar; i<numdynstatevar; i++ )           // SurfStateVar
   {
      delta[ioff+i] -= yt[ioff+i];
   }
   /* numdynstatevar <= i < numstatevar */                   // EquStateVar

   // equations in the first cell (gridpoint 1):
   // ------------------------------------------

   j = 1;

   eps = 0;
   for ( i=0; i<numvolstatevar; i++ )
   {
      if ( part[i] == TRUE )
	  {
         eps += y[j*cell+ioff+i]/rhopart[i];
	  }
   }
   eps = 1 - eps;

   delta[cell] = y[cell] - 0.5*(Qinterf[0]+Qinterf[1]);      // Q

   delta[cell+1] = y[cell+1] - eps;                          // Porosity

   for ( i=0; i<numvolstatevar; i++ )                        // VolStateVar
   {
      // add flux at the upper boundary of the cell:
      delta[cell+ioff+i] += flux[i]*dzinv;
      // calculate flux at the lower boundary of the cell:
      if ( part[i] == FALSE )
      {
         // advective upward flux leaves the compartment:
         if ( Qinterf[0] > 0 )
         {
            delta[cell+ioff+i] -= Qinterf[0]*y[cell+ioff+i]/y[cell+1]*dzinv;

		 }
         if ( Qinterf[1] < 0 )  // downwards
         {
            advflux1 = -Qinterf[1]*y[cell+ioff+i]/y[cell+1];
         }
         else                   // upwards
         {
            advflux1 = -Qinterf[1]*y[2*cell+ioff+i]/y[2*cell+1];
         }
         advflux2 = -Qinterf[1]*0.5*(y[cell+ioff+i]/y[cell+1]
                                     +y[2*cell+ioff+i]/y[2*cell+1]);
         s1 = (y[cell+ioff+i]/y[cell+1]-y[ioff+i])
              / fabs(gridpts[1]-gridpts[0]);
         s2 = (y[2*cell+ioff+i]/y[2*cell+1]-y[cell+ioff+i]/y[cell+1])
              / fabs(gridpts[2]-gridpts[1]);
         diffflux = -(diffs[1][i]+diffs[2][i])*0.5
                     *(y[2*cell+ioff+i]/y[2*cell+1]
                       -y[cell+ioff+i]/y[cell+1])
                     *dzinv*0.5*(y[2*cell+1]+y[cell+1]);
      }
      else
      {
         if ( vsed[1] >= 0.0 )
         {
            advflux1 = vsed[1]*y[cell+ioff+i];
         }
         else
         {
            if ( vsed[2] < 0.0 )
            {
               advflux1 = vsed[2]*y[2*cell+ioff+i];
            }
            else
            {
               advflux1 = 0.0;
            }
         }
         advflux2 = 0.5*(vsed[1]*y[cell+ioff+i]
                         +vsed[2]*y[2*cell+ioff+i]);
         s1 = (y[cell+ioff+i]-y[ioff+i])
              / fabs(gridpts[1]-gridpts[0]);
         s2 = (y[2*cell+ioff+i]-y[cell+ioff+i])
              / fabs(gridpts[2]-gridpts[1]);
         diffflux = -(diffs[1][i]+diffs[2][i])*0.5
                     *(y[2*cell+ioff+i]-y[cell+ioff+i])
                     *dzinv;
      }
      phi = FluxLimit(s1,s2,ini.I(4));
      flux[i] = (1.0-phi)*advflux1 + phi*advflux2 + diffflux;
      // subtract flux at the lower boundary of the cell:
      delta[cell+ioff+i] -= flux[i]*dzinv;
      // subtract time derivative:
      delta[cell+ioff+i] -= yt[cell+ioff+i];
   }

   for ( i=numvolstatevar; i<numdynstatevar; i++ )           // SurfStateVar
   {
      delta[cell+ioff+i] -= yt[cell+ioff+i];
   }

   /* numdynstatevar <= i < numstatevar */                   // EquStateVar

   // equations in the inner cells (gridpoints 2 to n-3):
   // ---------------------------------------------------

   for ( j=2; j<NumGridPts()-2; j++ )
   {
      eps = 0;
      for ( i=0; i<numvolstatevar; i++ )
	  {
         if ( part[i] == TRUE )
		 {
            eps += y[j*cell+ioff+i]/rhopart[i];
		 }
	  }
      eps = 1 - eps;

      delta[j*cell] = y[j*cell] - 0.5*(Qinterf[j-1]+Qinterf[j]); // Q

      delta[j*cell+1] = y[j*cell+1] - eps;                   // Porosity 

      for ( i=0; i<numvolstatevar; i++ )                     // VolStateVar
      {
         // add flux at the upper boundary of the cell:
         delta[j*cell+ioff+i] += flux[i]*dzinv;
         // calculate flux at the lower boundary of the cell:
         if ( part[i] == FALSE )
         {
            if ( Qinterf[j] < 0.0 )   // downwards
            {
               advflux1 = -Qinterf[j]*y[j*cell+ioff+i]/y[j*cell+1];
            }
            else                      // upwards
            {
               advflux1 = -Qinterf[j]*y[(j+1)*cell+ioff+i]/y[(j+1)*cell+1];
            }
            advflux2 = -Qinterf[j]*0.5*(y[j*cell+ioff+i]/y[j*cell+1]
                                        +y[(j+1)*cell+ioff+i]/y[(j+1)*cell+1]);
            s1 = (y[j*cell+ioff+i]/y[j+cell+1]-y[(j-1)*cell+ioff+i]/y[(j-1)*cell+1])
                 / fabs(gridpts[j]-gridpts[j-1]);
            s2 = (y[(j+1)*cell+ioff+i]/y[(j+1)*cell+1]-y[j*cell+ioff+i]/y[j*cell+1])
                 / fabs(gridpts[j+1]-gridpts[j]);
            diffflux = -(diffs[j][i]+diffs[j+1][i])*0.5
                        *(y[(j+1)*cell+ioff+i]/y[(j+1)*cell+1]
                          -y[j*cell+ioff+i]/y[j*cell+1])
                        *dzinv*0.5*(y[(j+1)*cell+1]+y[j*cell+1]);
		 }
         else
         {
            if ( vsed[j] >= 0.0 )
            {
               advflux1 = vsed[j]*y[j*cell+ioff+i];
            }
            else
            {
               if ( vsed[j+1] < 0.0 )
               {
                  advflux1 = vsed[j+1]*y[(j+1)*cell+ioff+i];
               }
               else
               {
                  advflux1 = 0.0;
               }
            }
            advflux2 = 0.5*(vsed[j]*y[j*cell+ioff+i]
                            +vsed[j+1]*y[(j+1)*cell+ioff+i]);
            s1 = (y[j*cell+ioff+i]-y[(j-1)*cell+ioff+i])
                 / fabs(gridpts[j]-gridpts[j-1]);
            s2 = (y[(j+1)*cell+ioff+i]-y[j*cell+ioff+i])
                 / fabs(gridpts[j+1]-gridpts[j]);
            diffflux =  -(diffs[j][i]+diffs[j+1][i])*0.5
                         *(y[(j+1)*cell+ioff+i]-y[j*cell+ioff+i])
                         *dzinv;
         }
         phi = FluxLimit(s1,s2,ini.I(4));
         flux[i] = (1.0-phi)*advflux1 + phi*advflux2 + diffflux;
         // subtract flux at the lower boundary of the cell:
         delta[j*cell+ioff+i] -= flux[i]*dzinv;
         // subtract time derivative:
         delta[j*cell+ioff+i] -= yt[j*cell+ioff+i];
      }

      for ( i=numvolstatevar; i<numdynstatevar; i++ )        // SurfStateVar
      {
         delta[j*cell+ioff+i] -= yt[j*cell+ioff+i];
      }

      /* numdynstatevar <= i < numstatevar */                // EquStateVar

   }

   // equations in the last cell (grid point n-2):
   // --------------------------------------------

   j = NumGridPts()-2;

   eps = 0;
   for ( i=0; i<numvolstatevar; i++ )
   {
      if ( part[i] == TRUE )
	  {
         eps += y[j*cell+ioff+i]/rhopart[i];
	  }
   }
   eps = 1 - eps;

   delta[j*cell] = y[j*cell] - 0.5*(Qinterf[j-1]+Qinterf[j]);// Q

   delta[j*cell+1] = y[j*cell+1] - eps;                      // Porosity ;

   for ( i=0; i<numvolstatevar; i++ )                        // VolStateVar
   {
      // add flux at the upper boundary of the cell:
      delta[j*cell+ioff+i] += flux[i]*dzinv;
      // calculate flux at the lower boundary of the cell:
      if ( part[i] == FALSE )
      {
         if ( Qinterf[j] < 0.0 )   // downwards
         {
            advflux1 = -Qinterf[j]*y[j*cell+ioff+i]/y[j*cell+1];
         }
         else                      // upwards
         {
            advflux1 = -Qinterf[j]*y[(j+1)*cell+ioff+i]/y[(j+1)*cell+1];
         }
         advflux2 = -Qinterf[j]*y[(j+1)*cell+ioff+i];
         s1 = (y[j*cell+ioff+i]/y[j*cell+1]-y[(j-1)*cell+ioff+i]/y[(j-1)*cell+1]) 
              / fabs(gridpts[j]-gridpts[j-1]);
         s2 = (y[(j+1)*cell+ioff+i]-y[j*cell+ioff+i]/y[j*cell+1]) 
              / fabs(gridpts[j+1]-gridpts[j]);
         diffflux = -diffs[j+1][i]
                     *(y[(j+1)*cell+ioff+i]-y[j*cell+ioff+i]/y[j*cell+1])
                     *2.0*dzinv*y[(j+1)*cell+1];
      }
      else
      {
         if ( vsed[j] >= 0.0 )
         {
            advflux1 = vsed[j]*y[j*cell+ioff+i];
         }
         else
         {
			 /*
            if ( vsed[j+1] < 0.0 )
            {
               advflux1 = 0.0;
            }
            else
            {
               advflux1 = 0.0;
            }
			*/
			 advflux1 = vsed[j]*y[j*cell+ioff+i];
         }
         advflux2 = vsed[j+1]*y[(j+1)*cell+ioff+i];
         s1 = (y[j*cell+ioff+i]-y[(j-1)*cell+ioff+i]) 
              / fabs(gridpts[j]-gridpts[j-1]);
         s2 = (y[(j+1)*cell+ioff+i]-y[j*cell+ioff+i]) 
              / fabs(gridpts[j+1]-gridpts[j]);
         diffflux = -diffs[j+1][i]
                     *(y[(j+1)*cell+ioff+i]-y[j*cell+ioff+i])
                     *2.0*dzinv;
      }
      phi = FluxLimit(s1,s2,ini.I(4));
      flux[i] = (1.0-phi)*advflux1 + phi*advflux2 + diffflux;
      // subtract flux at the lower boundary of the cell:
      delta[j*cell+ioff+i] -= flux[i]*dzinv;
      // subtract time derivative:
      delta[j*cell+ioff+i] -= yt[j*cell+ioff+i];
   }

   for ( i=numvolstatevar; i<numdynstatevar; i++ )           // SurfStateVar
   {
      delta[j*cell+ioff+i] -= yt[j*cell+ioff+i];
   }

   /* numdynstatevar <= i < numstatevar */                   // EquStateVar

   // equations at the sediment bottom (grid point n-1):
   // --------------------------------------------------

   j = NumGridPts()-1;

   eps = 0;
   for ( i=0; i<numvolstatevar; i++ )
   {
      if ( part[i] == TRUE )
	  {
         eps += y[j*cell+ioff+i]/rhopart[i];
	  }
   }
   eps = 1 - eps;

   delta[j*cell] = y[j*cell] - Qinterf[numgrid-2];           // Q

   delta[j*cell+1] = y[j*cell+1] - eps;                      // Porosity 

   for ( i=0; i<numvolstatevar; i++ )                        // VolStateVar
   {
      if ( part[i] == FALSE )
      {
         if ( givenbottomconc[i] == TRUE )
         {
            delta[j*cell+ioff+i] = bottomconc[i]
                                   - y[j*cell+ioff+i];
         }
         else
         {
            delta[j*cell+ioff+i] = y[(j-1)*cell+ioff+i]/y[(j-1)*cell+1]
                                   - y[j*cell+ioff+i];
         }
	  }
      else
      {
         delta[j*cell+ioff+i] = y[(j-1)*cell+ioff+i]
                                - y[j*cell+ioff+i];
	  }
   }

   for ( i=numvolstatevar; i<numdynstatevar; i++ )           // SurfStateVar
   {
      delta[j*cell+ioff+i] -= yt[j*cell+ioff+i];
   }
   /* numdynstatevar <= i < numstatevar */                   // EquStateVar

   for ( i=0; i<numstatevar; i++ ) statevar[i]->ResetStateVar();
   delete [] conc;
   delete [] flux;
   delete [] inpflux;
   delete [] topconc;
   delete [] bottomconc;
   delete [] givenbottomconc;
   delete [] rhopart;
   delete [] Qinterf;
   delete [] part;
   delete [] ind;
   delete [] vsed;
   for ( j=0; j<numgrid; j++ ) delete [] diffs[j];
   delete [] diffs;
   delete [] ratepors;

/*
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


BOOLEAN SEDCOMP::GridValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                            REAL* t_ptr, const REAL* Y,
                            CARDINAL zone, CARDINAL pt, AQVAR* var,
                            REAL& value)
{
   if ( Active() != TRUE ) return FALSE;
   SetGlobalState(varsys,calcnum_ptr,t_ptr,Y);
   CARDINAL i,j,k;
   if ( zone >= NumZones() )          return FALSE;
   if ( pt >= NumGridPts() )          return FALSE;
   if ( varsys->Exist(var) == FALSE ) return FALSE;
   if ( AllowedVar(var) == FALSE )    return FALSE;
   if ( Active() == FALSE )           return FALSE;
   const REAL* y = &Y[ind_y];
   CARDINAL cell = ioff + NumStateVar();
   varsys->Reset();
   REAL cn = *calcnum_ptr; 
   REAL compindex = (REAL)cind;
   REAL zoneindex = 0.0;
   varsys->SetProgVar(CalcNumber,&cn);
   varsys->SetProgVar(CompIndex,&compindex);
   varsys->SetProgVar(ZoneIndex,&zoneindex);
   varsys->SetProgVar(Time,t_ptr);
   Zvar    = varsys->GetProgVar(SpaceZ);
   Qvar    = varsys->GetProgVar(Discharge);
   epsvar  = varsys->GetProgVar(WaterFraction);
   if ( Qvar != 0 )    Qvar->SetProgVar(&y[pt*cell]);
   if ( Zvar != 0 )    Zvar->SetProgVar(&gridpts[pt]);
   if ( epsvar != 0 )  epsvar->SetProgVar(&y[pt*cell+1]);
   REAL* conc = new REAL[numstatevar];
   BOOLEAN* part = new BOOLEAN[numstatevar];
   CARDINAL* ind      = new CARDINAL[numvolstatevar];
   for ( i=0; i<numvolstatevar; i++ )
   {
      BOOLEAN found = FALSE;
      for ( j=0; j<NumPartVar(); j++ )
      {
         if ( statevar[i] == partvar[j] )
         {
            k = j;
            found = TRUE;
            break;
         }
      }
      if ( found == TRUE )
      {
         part[i] = TRUE;
         ind[i] = k;
      }
      else
      {
         part[i] = FALSE;
         for ( j=0; j<NumDissVar(); j++ )
         {
            if ( statevar[i] == dissvar[j] )
            {
               k = j;
               found = TRUE;
               break;
            }
         }
         if ( found == TRUE )
         {
            ind[i] = k;
         }
         else
         {
            ind[i] = numvolstatevar;
         }
      }
   }

   for ( i=0; i<numstatevar; i++ ) statevar[i]->SetStateVar(&conc[i]);
   varsys->Reset();
   REAL zero = 0.0;
   if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[pt]);
   for ( i=0; i<numvolstatevar; i++ )
   {
      conc[i] = y[pt*cell+ioff+i];
      if ( pt>0 && pt<numgrid-1 )
      {
         if ( part[i] == FALSE ) conc[i] = conc[i]/y[pt*cell+1];
      }
   }
   for ( i=numvolstatevar; i<numdynstatevar; i++ )
   {
      conc[i] = y[pt*cell+ioff+i];
   }
   for ( i=numdynstatevar; i<numstatevar; i++ )
   {
      conc[i] = y[pt*cell+ioff+i];
   }
   varsys->Reset();

   value = var->Value();

   for ( i=0; i<numstatevar; i++ ) statevar[i]->ResetStateVar();
   delete [] conc;
   delete [] part;
   delete [] ind;
   return TRUE;
}


BOOLEAN SEDCOMP::SpaceValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                            REAL* t_ptr, const REAL* Y,
                            CARDINAL zone, REAL zz, BOOLEAN zrel, 
                            AQVAR* var,
                            REAL& value)
{
   if ( Active() != TRUE ) return FALSE;
   REAL z = zz;
   if ( zrel == TRUE )
   {
      z = gridpts[0] + zz*(gridpts[numgrid-1]-gridpts[0]);
   }
   REAL r = (z-gridpts[0])/(gridpts[numgrid-1]-gridpts[0])*(numgrid-2);
   INTEGER i = (INTEGER) r;
   if ( gridpts[0] < gridpts[numgrid-1] )
   {
      if ( (z<gridpts[0]) || (z>gridpts[numgrid-1]) ) return FALSE;
      if ( z <= gridpts[i] )
      {
         while ( z < gridpts[i] )
         {
            i--;
            if ( i < 0 )
               FatalError("SEDCOMP::Value: index negative");
         }
      }
      else
      {
         while ( z > gridpts[i+1] )
         {
            i++;
            if ( i >= numgrid-1 )
               FatalError("SEDCOMP::Value: index too large");
         }
      }
   }
   else
   {
      if ( (z>gridpts[0]) || (z<gridpts[numgrid-1]) ) return FALSE;
      if ( z >= gridpts[i] )
      {
         while ( z > gridpts[i] )
         {
            i--;
            if ( i < 0 )
               FatalError("SEDCOMP::Value: index negative");
         }
      }
      else
      {
         while ( z < gridpts[i+1] )
         {
            i++;
            if ( i >= numgrid-1 )
               FatalError("SEDCOMP::Value: index too large");
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
      value = ( (z-gridpts[i1])*val2 + (gridpts[i2]-z)*val1 )
              / (gridpts[i2]-gridpts[i1]);
   }
   return TRUE;
}


CARDINAL SEDCOMP::InitZone(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("SEDCOMP::InitZone: Illegal index");
   return initzone[index];
}


AQVAR* SEDCOMP::InitVar(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("SEDCOMP::InitVar: Illegal index");
   return initvar[index];
}


const FORMVAR* SEDCOMP::InitVal(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("SEDCOMP::InitVal: Illegal index");
   return initval[index];
}


BOOLEAN SEDCOMP::AddInitCond(const VARLIST* varlist, CARDINAL zone,
                             AQVAR* var, const char* inpline, char* errorline,
                             CARDINAL& numparseerrors, CARDINAL pos)
{
   numparseerrors = 0;
   if ( InList() == TRUE )                        return FALSE;
   if ( inpline == 0 )                            return FALSE;
   if ( inpline[0] == '\0' )                      return FALSE;
   if ( varlist->Exist(var) == FALSE )            return FALSE;
   //if ( zone >= NumZones() )                      return FALSE;
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


BOOLEAN SEDCOMP::ReplaceInitCond(const VARLIST* varlist, CARDINAL zone,
                                 AQVAR* var, const char* inpline,
                                 char* errorline, CARDINAL& numparseerrors,
                                 CARDINAL pos)
{
   numparseerrors = 0;
   if ( InList() == TRUE )                                  return FALSE;
   if ( inpline == 0 )                                      return FALSE;
   if ( inpline[0] == '\0' )                                return FALSE;
   if ( varlist->Exist(var) == FALSE )                      return FALSE;
   //if ( zone >= NumZones() )                                return FALSE;
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


BOOLEAN SEDCOMP::DeleteInitCond(CARDINAL pos)
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


AQVAR* SEDCOMP::TopBCVar(CARDINAL index) const
{
   if ( index >= numtopbc )
      FatalError("SEDCOMP::TopBCVar: Illegal index");
   return topbcvar[index];
}


const FORMVAR* SEDCOMP::TopBCCorF(CARDINAL index) const
{
   if ( index >= numtopbc )
      FatalError("SEDCOMP::TopBCCorF: Illegal index");
   return topbccorf[index];
}


BOOLEAN SEDCOMP::AddTopBC(const VARLIST* varlist, AQVAR* var,
                          const char* inpline, char* errorline,
                          CARDINAL& numparseerrors, CARDINAL pos)
{
   numparseerrors = 0;
   if ( InList() == TRUE )             return FALSE;
   if ( inpline == 0 )                 return FALSE;
   if ( inpline[0] == '\0' )           return FALSE;
   if ( varlist->Exist(var) == FALSE ) return FALSE;
   if ( pos > numtopbc ) pos = numtopbc;
   CARDINAL i;
   for ( i=0; i<numtopbc; i++ )
   {
      if ( topbcvar[i] == var )        return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("TopBC");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                  return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                  return FALSE;
   }
   AQVAR**   newvar  = new AQVAR*[numtopbc+1];
   FORMVAR** newconc = new FORMVAR*[numtopbc+1];
   for ( i=0; i<pos; i++ )
   {
      newvar[i]  = topbcvar[i];
      newconc[i] = topbccorf[i];
   }
   newvar[pos]  = var;
   newconc[pos] = formvar;
   for ( i=pos; i<numtopbc; i++ )
   {
      newvar[i+1]  = topbcvar[i];
      newconc[i+1] = topbccorf[i];
   }
   delete topbcvar;  topbcvar  = newvar;
   delete topbccorf; topbccorf = newconc;
   numtopbc++;
   CalcArg();
   return TRUE;
}


BOOLEAN SEDCOMP::ReplaceTopBC(const VARLIST* varlist, AQVAR* var,
                               const char* inpline, char* errorline,
                               CARDINAL& numparseerrors, CARDINAL pos)
{
   numparseerrors = 0;
   if ( InList() == TRUE )              return FALSE;
   if ( inpline == 0 )                  return FALSE;
   if ( inpline[0] == '\0' )            return FALSE;
   if ( varlist->Exist(var) == FALSE )  return FALSE;
   if ( pos >= numtopbc )           return FALSE;
   CARDINAL i;
   for ( i=0; i<numtopbc; i++ )
   {
      if ( i!=pos && topbcvar[i]==var ) return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("TopBC");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                   return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                   return FALSE;
   }
   delete topbccorf[pos];
   topbcvar[pos]  = var;
   topbccorf[pos] = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN SEDCOMP::DeleteTopBC(CARDINAL pos)
{
   if ( pos >= numtopbc ) return FALSE;
   AQVAR**   newvar  = new AQVAR*[numtopbc-1];
   FORMVAR** newconc = new FORMVAR*[numtopbc-1];
   CARDINAL i;
   for ( i=0; i<pos; i++ )
   {
      newvar[i]  = topbcvar[i];
      newconc[i] = topbccorf[i];
   }
   for ( i=pos+1; i<numtopbc; i++ )
   {
      newvar[i-1]  = topbcvar[i];
      newconc[i-1] = topbccorf[i];
   }
   delete topbccorf[pos];
   delete topbcvar;  topbcvar = newvar;
   delete topbccorf; topbccorf = newconc;
   numtopbc--;
   CalcArg();
   return TRUE;
}


AQVAR* SEDCOMP::BottomBCVar(CARDINAL index) const
{
   if ( index >= numbottombc )
      FatalError("SEDCOMP::BottomBCVar: Illegal index");
   return bottombcvar[index];
}


const FORMVAR* SEDCOMP::BottomBCConc(CARDINAL index) const
{
   if ( index >= numbottombc )
      FatalError("SEDCOMP::BottomBCConc: Illegal index");
   return bottombcconc[index];
}


BOOLEAN SEDCOMP::AddBottomBC(const VARLIST* varlist, AQVAR* var,
                          const char* inpline, char* errorline,
                          CARDINAL& numparseerrors, CARDINAL pos)
{
   numparseerrors = 0;
   if ( InList() == TRUE )             return FALSE;
   if ( inpline == 0 )                 return FALSE;
   if ( inpline[0] == '\0' )           return FALSE;
   if ( varlist->Exist(var) == FALSE ) return FALSE;
   if ( pos > numbottombc ) pos = numbottombc;
   CARDINAL i;
   for ( i=0; i<numbottombc; i++ )
   {
      if ( bottombcvar[i] == var )        return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("BottomBC");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                  return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                  return FALSE;
   }
   AQVAR**   newvar  = new AQVAR*[numbottombc+1];
   FORMVAR** newconc = new FORMVAR*[numbottombc+1];
   for ( i=0; i<pos; i++ )
   {
      newvar[i]  = bottombcvar[i];
      newconc[i] = bottombcconc[i];
   }
   newvar[pos]  = var;
   newconc[pos] = formvar;
   for ( i=pos; i<numbottombc; i++ )
   {
      newvar[i+1]  = bottombcvar[i];
      newconc[i+1] = bottombcconc[i];
   }
   delete bottombcvar;  bottombcvar  = newvar;
   delete bottombcconc; bottombcconc = newconc;
   numbottombc++;
   CalcArg();
   return TRUE;
}


BOOLEAN SEDCOMP::ReplaceBottomBC(const VARLIST* varlist, AQVAR* var,
                               const char* inpline, char* errorline,
                               CARDINAL& numparseerrors, CARDINAL pos)
{
   numparseerrors = 0;
   if ( InList() == TRUE )              return FALSE;
   if ( inpline == 0 )                  return FALSE;
   if ( inpline[0] == '\0' )            return FALSE;
   if ( varlist->Exist(var) == FALSE )  return FALSE;
   if ( pos >= numbottombc )           return FALSE;
   CARDINAL i;
   for ( i=0; i<numbottombc; i++ )
   {
      if ( i!=pos && bottombcvar[i]==var ) return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("BottomBC");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                   return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                   return FALSE;
   }
   delete bottombcconc[pos];
   bottombcvar[pos]  = var;
   bottombcconc[pos] = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN SEDCOMP::DeleteBottomBC(CARDINAL pos)
{
   if ( pos >= numbottombc ) return FALSE;
   AQVAR**   newvar  = new AQVAR*[numbottombc-1];
   FORMVAR** newconc = new FORMVAR*[numbottombc-1];
   CARDINAL i;
   for ( i=0; i<pos; i++ )
   {
      newvar[i]  = bottombcvar[i];
      newconc[i] = bottombcconc[i];
   }
   for ( i=pos+1; i<numbottombc; i++ )
   {
      newvar[i-1]  = bottombcvar[i];
      newconc[i-1] = bottombcconc[i];
   }
   delete bottombcconc[pos];
   delete bottombcvar;  bottombcvar = newvar;
   delete bottombcconc; bottombcconc = newconc;
   numbottombc--;
   CalcArg();
   return TRUE;
}


void SEDCOMP::CalcArg()
{
   CARDINAL i,j;

   AQCOMP::CalcArg();
   CARDINAL num = numvararg;
   numvararg += numinit;
   for ( i=0; i<numinit; i++ ) numvararg += (initval[i])->NumVarArg();
   numvararg += numtopbc;
   for ( i=0; i<numtopbc; i++ )
      numvararg += (topbccorf[i])->NumVarArg();
   numvararg += numbottombc;
   for ( i=0; i<numbottombc; i++ )
      numvararg += (bottombcconc[i])->NumVarArg();
   numvararg += numdissvar;
   for ( j=0; j<numdissvar; j++ )
   {
      numvararg += dissdiff[j]->NumVarArg();
   }
   numvararg += numpartvar;
   for ( j=0; j<numpartvar; j++ )
   {
      numvararg += partrho[j]->NumVarArg();
      numvararg += partbioturb[j]->NumVarArg();
   }
   if ( SurfPor != 0 )    numvararg += SurfPor->NumVarArg();
   if ( RatePor != 0 )    numvararg += RatePor->NumVarArg();
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
      for ( i=0; i<numtopbc; i++ )
      {
         newarg[k] = topbcvar[i]; k++;
         for ( j=0; j<topbccorf[i]->NumVarArg(); j++ )
         {
            newarg[k] = topbccorf[i]->VarArg(j); k++;
         }
      }
      for ( i=0; i<numbottombc; i++ )
      {
         newarg[k] = bottombcvar[i]; k++;
         for ( j=0; j<bottombcconc[i]->NumVarArg(); j++ )
         {
            newarg[k] = bottombcconc[i]->VarArg(j); k++;
         }
      }
      for ( i=0; i<numdissvar; i++ )
      {
         newarg[k] = dissvar[i]; k++;
         for ( j=0; j<dissdiff[i]->NumVarArg(); j++ )
         {
            newarg[k] = dissdiff[i]->VarArg(j); k++;
         }
      }
      for ( i=0; i<numpartvar; i++ )
      {
         newarg[k] = partvar[i]; k++;
         for ( j=0; j<partrho[i]->NumVarArg(); j++ )
         {
            newarg[k] = partrho[i]->VarArg(j); k++;
         }
         for ( j=0; j<partbioturb[i]->NumVarArg(); j++ )
         {
            newarg[k] = partbioturb[i]->VarArg(j); k++;
         }
      }
      if ( SurfPor != 0 )
      {
         for ( i=0; i<SurfPor->NumVarArg(); i++ )
         {
            newarg[k] = SurfPor->VarArg(i); k++;
         }
      }
      if ( RatePor != 0 )
      {
         for ( i=0; i<RatePor->NumVarArg(); i++ )
         {
            newarg[k] = RatePor->VarArg(i); k++;
         }
      }
      if ( k != numvararg )
         FatalError("SEDCOMP::CalcArg: inconsistent number of arguments");
      delete [] vararg; vararg = newarg;
   }
   return;
}


BOOLEAN SEDCOMP::SetGridPts(CARDINAL n)
{
   if ( n < 5 )        return FALSE;
   if ( n == numgrid ) return TRUE;
   numgrid = n;
   delete gridpts; gridpts = new REAL[numgrid];
   ZTopBottom(Zt,Zb);
   return TRUE;
}


BOOLEAN SEDCOMP::NumGridPts(CARDINAL n)
{
   if ( InList() == TRUE ) return FALSE;
   return SetGridPts(n);
}


REAL SEDCOMP::GridPt(const REAL*, CARDINAL index)
{
   if ( index >= NumGridPts() )
      FatalError("SEDCOMP::GridPt: illegal index");
   return gridpts[index];
}


CARDINAL SEDCOMP::NumZoneGridPts(CARDINAL zone)
{
   if ( zone > 0 ) FatalError("SEDCOMP::NumZoneGridPts: illegal zone");
   return NumGridPts();
}


REAL SEDCOMP::ZoneGridPt(const REAL* Y, CARDINAL zone,
                         CARDINAL index)
{
   if ( zone > 0 ) FatalError("SEDCOMP::ZoneGridPt: illegal zone");
   return GridPt(Y,index);
}


BOOLEAN SEDCOMP::ZTopBottom(REAL zt, REAL zb)
{
   if ( zt == zb ) return FALSE;
   Zt = zt; Zb = zb;
   REAL dz = (Zb-Zt)/(numgrid-2);
   gridpts[0] = Zt; gridpts[numgrid-1] = Zb;
   for ( CARDINAL i=1; i<numgrid-1; i++ ) gridpts[i] = Zt + (i-0.5)*dz;
   return TRUE;
}


BOOLEAN SEDCOMP::AccQ(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccQ = relacc;
   absaccQ = absacc;
   return TRUE;
}


BOOLEAN SEDCOMP::AccPor(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccPor = relacc;
   absaccPor = absacc;
   return TRUE;
}


BOOLEAN SEDCOMP::SurfPorosity(const char* inpline,
                              const VARLIST* varlist,
                              char* errorline,
                              CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("SurfPor");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;
      return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;
      return FALSE;
   }

   delete SurfPor; SurfPor = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN SEDCOMP::RatePorosity(const char* inpline,
                              const VARLIST* varlist,
                              char* errorline,
                              CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("RatePor");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;
      return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;
      return FALSE;
   }

   delete RatePor; RatePor = formvar;
   CalcArg();
   return TRUE;
}


CARDINAL SEDCOMP::NumZones() const
{
   return 1;
}


const char* SEDCOMP::ZoneName(CARDINAL index) const
{
   return ini.T(271);
}


const char* SEDCOMP::AdvInName(CARDINAL conn) const
{
   if ( conn > 0 ) FatalError("SEDCOMP::AdvInName: Illegal connection");
   return ini.T(272);
}


const char* SEDCOMP::AdvOutName(CARDINAL conn) const
{
   if ( conn > 0 ) FatalError("SEDCOMP::AdvOutName: Illegal connection");
   return ini.T(273);
}


const char* SEDCOMP::DiffName(CARDINAL) const
{
   FatalError("SEDCOMP::DiffName: Illegal connection");
   return 0;
}


BOOLEAN SEDCOMP::AdvInExZ(CARDINAL) const
{
   return FALSE;
}


REAL SEDCOMP::AdvInZ(const REAL*, CARDINAL) const
{
   FatalError("SEDCOMP::AdvInZ: Illegal connection");
   return 0;
}


REAL SEDCOMP::AdvOutQ(VARSYS* varsys, const REAL* Y, CARDINAL conn)
{
   FatalError("SEDCOMP::AdvOutQ: Illegal connection");
   if ( Active() == FALSE ) return 0.0;
   return 0.0;
}


REAL SEDCOMP::AdvOutJ(VARSYS* varsys,
                      const REAL* Y,
                      CARDINAL conn,
                      const AQVAR* var)
{
   FatalError("SEDCOMP::AdvOutJ: Illegal connection");
   if ( Active() == FALSE ) return 0.0;
   return 0.0;
}


REAL SEDCOMP::DiffC(const REAL*, CARDINAL, const AQVAR*) const
{
   FatalError("SEDCOMP::DiffC: Illegal connection");
   return 0.0;
}


JOBSTATUS SEDCOMP::Load(std::istream& in,
                        const VARLIST* varlist, const PROCLIST* proclist)
{
   char Buffer[BufSize]; 
   char Buffer1[BufSize]; char Buffer2[BufSize];
   char Buffer3[BufSize]; char Buffer4[BufSize];
   char Buffer5[BufSize]; char Buffer6[BufSize];

   if ( LoadStart(in) == FALSE )                                return PROBLEM;
   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )                     return PROBLEM;
   switch ( aquavers )
   {
      case -1: // aquavers=-1: development version February 2001
         {
            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,3);
                                            if ( status != OK ) return status;
            CARDINAL n, n1, n3;

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

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddTopBC(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadNumber(in,&n) == FALSE )                   return PROBLEM;
            if ( NumGridPts(n) == FALSE )                       return PROBLEM;
            if ( LoadBoolean(in,&highres) == FALSE )            return PROBLEM;
            REAL relacc,absacc;
            if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
            if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
            if ( AccQ(relacc,absacc) == FALSE )                 return PROBLEM;
            if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
            if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
            if ( AccPor(relacc,absacc) == FALSE )               return PROBLEM;
            REAL xs,xe;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( ZTopBottom(xs,xe) == FALSE )                   return PROBLEM;

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer1,sizeof(Buffer1)) == FALSE )
                                                                return PROBLEM;
               if ( AddDissVar(varlist,var,Buffer1,Buffer2,n1) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer1,sizeof(Buffer1)) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer3,sizeof(Buffer3)) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer5,sizeof(Buffer5)) == FALSE )
                                                                return PROBLEM;

               if ( AddPartVar(varlist,var,
                               Buffer1,Buffer2,n1,
                               Buffer5,Buffer6,n3) == FALSE )   return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( RatePorosity(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
         }
         break;
      case 0: // aquavers=0: first final version January 2002
         {
            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,3);
                                            if ( status != OK ) return status;
            CARDINAL n, n1, n2;

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

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddTopBC(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadNumber(in,&n) == FALSE )                   return PROBLEM;
            if ( NumGridPts(n) == FALSE )                       return PROBLEM;
            if ( LoadBoolean(in,&highres) == FALSE )            return PROBLEM;
            REAL relacc,absacc;
            if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
            if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
            if ( AccQ(relacc,absacc) == FALSE )                 return PROBLEM;
            if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
            if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
            if ( AccPor(relacc,absacc) == FALSE )               return PROBLEM;
            REAL xs,xe;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( ZTopBottom(xs,xe) == FALSE )                   return PROBLEM;

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer1,sizeof(Buffer1)) == FALSE )
                                                                return PROBLEM;
               if ( AddDissVar(varlist,var,Buffer1,Buffer2,n1) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer1,sizeof(Buffer1)) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer3,sizeof(Buffer3)) == FALSE )
                                                                return PROBLEM;
               if ( AddPartVar(varlist,var,
                               Buffer1,Buffer2,n1,
                               Buffer3,Buffer4,n2) == FALSE )   return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SurfPorosity(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( RatePorosity(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
         }
         break;
      case 1: // aquavers=1: Bottom boundary conditions added, November 2003
         {
            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,3);
                                            if ( status != OK ) return status;
            CARDINAL n, n1, n2;

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

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddTopBC(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddBottomBC(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadNumber(in,&n) == FALSE )                   return PROBLEM;
            if ( NumGridPts(n) == FALSE )                       return PROBLEM;
            if ( LoadBoolean(in,&highres) == FALSE )            return PROBLEM;
            REAL relacc,absacc;
            if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
            if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
            if ( AccQ(relacc,absacc) == FALSE )                 return PROBLEM;
            if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
            if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
            if ( AccPor(relacc,absacc) == FALSE )               return PROBLEM;
            REAL xs,xe;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( ZTopBottom(xs,xe) == FALSE )                   return PROBLEM;

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer1,sizeof(Buffer1)) == FALSE )
                                                                return PROBLEM;
               if ( AddDissVar(varlist,var,Buffer1,Buffer2,n1) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer1,sizeof(Buffer1)) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer3,sizeof(Buffer3)) == FALSE )
                                                                return PROBLEM;
               if ( AddPartVar(varlist,var,
                               Buffer1,Buffer2,n1,
                               Buffer3,Buffer4,n2) == FALSE )   return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SurfPorosity(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( RatePorosity(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                                  return PROBLEM;
   return OK;
}


JOBSTATUS SEDCOMP::Save(std::ostream& out)
{
   char Buffer[BufSize]; CARDINAL i;

   INTEGER aquavers = 1;

   if ( SaveStart(out) == FALSE )                              return PROBLEM;

   if ( SaveNumber(out,aquavers) == FALSE )                    return PROBLEM;

   JOBSTATUS status = AQCOMP::Save(out);   if ( status != OK ) return status;

   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   for ( i=0; i<numinit; i++ )
   {
      if ( SaveNumber(out,InitZone(i)) == FALSE )              return PROBLEM;
      if ( SaveString(out,InitVar(i)->Symbol()) == FALSE )     return PROBLEM;
      if ( InitVal(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 ) return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                   return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;

   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   for ( i=0; i<numtopbc; i++ )
   {
      if ( SaveString(out,TopBCVar(i)->Symbol()) == FALSE ) 
                                                               return PROBLEM;
      if ( TopBCCorF(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                   return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;

   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   for ( i=0; i<numbottombc; i++ )
   {
      if ( SaveString(out,BottomBCVar(i)->Symbol()) == FALSE ) 
                                                               return PROBLEM;
      if ( BottomBCConc(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                   return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;

   if ( SaveNumber(out,numgrid) == FALSE )                     return PROBLEM;
   if ( SaveBoolean(out,HighRes()) == FALSE )                  return PROBLEM;
   if ( SaveNumber(out,RelAccQ()) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,AbsAccQ()) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,RelAccPor()) == FALSE )                 return PROBLEM;
   if ( SaveNumber(out,AbsAccPor()) == FALSE )                 return PROBLEM;
   if ( SaveNumber(out,Zt) == FALSE )                          return PROBLEM;
   if ( SaveNumber(out,Zb) == FALSE )                          return PROBLEM;
   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   for ( i=0; i<numdissvar; i++ )
   {
      if ( SaveString(out,DissVar(i)->Symbol()) == FALSE )     return PROBLEM;
      if ( Dissdiff(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                   return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;
   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   for ( i=0; i<numpartvar; i++ )
   {
      if ( SaveString(out,PartVar(i)->Symbol()) == FALSE ) return PROBLEM;
      if ( PartRho(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                   return PROBLEM;
      if ( PartBioturb(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                   return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;
   if ( SurfPor->UnParse(Buffer,sizeof(Buffer)-1) != 0 )       return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( RatePor->UnParse(Buffer,sizeof(Buffer)-1) != 0 )       return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;
   return OK;
}


JOBSTATUS SEDCOMP::Write(std::ostream& out, BOOLEAN sh)
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

   if ( Print(out,ini.T(150),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( NumInitCond() > 0 )
   {
      PrintIndent(pos4-1);
      if ( Print(out,ini.T(275),pos3) == FALSE )               return PROBLEM;
      if ( Print(out,"(") == FALSE )                           return PROBLEM;
      if ( Print(out,ini.T(276)) == FALSE )                    return PROBLEM;
      if ( Print(out,") : ") == FALSE )                        return PROBLEM;
      if ( Print(out,ini.T(277)) == FALSE )                    return PROBLEM;
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
   if ( Print(out,ini.T(278),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( NumTopBC() > 0 )
   {
      PrintIndent(pos4-1);
      if ( Print(out,ini.T(279),pos3) == FALSE )               return PROBLEM;
      if ( Print(out," : ") == FALSE )                         return PROBLEM;
      if ( Print(out,ini.T(280)) == FALSE )                    return PROBLEM;
      for ( i=0; i<NumTopBC(); i++ )
      {
         if ( Print(out,TopBCVar(i)->Symbol(),pos3) == FALSE )
                                                               return PROBLEM;
         if ( Print(out," : ") == FALSE )                      return PROBLEM;
         if ( TopBCCorF(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer) == FALSE )                     return PROBLEM;
      }
   }

   if ( Print(out,ini.T(278),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( NumBottomBC() > 0 )
   {
      PrintIndent(pos4-1);
      if ( Print(out,ini.T(279),pos3) == FALSE )               return PROBLEM;
      if ( Print(out," : ") == FALSE )                         return PROBLEM;
      if ( Print(out,ini.T(280)) == FALSE )                    return PROBLEM;
      for ( i=0; i<NumBottomBC(); i++ )
      {
         if ( Print(out,BottomBCVar(i)->Symbol(),pos3) == FALSE )
                                                               return PROBLEM;
         if ( Print(out," : ") == FALSE )                      return PROBLEM;
         if ( BottomBCConc(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer) == FALSE )                     return PROBLEM;
      }
   }

   if ( Print(out,ini.T(281),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,ZTop(),pos2) == FALSE )                      return PROBLEM;

   if ( Print(out,ini.T(282),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,ZBottom(),pos2) == FALSE )                   return PROBLEM;

   if ( NumDissVar() > 0 )
   {
      if ( Print(out,ini.T(283),pos1) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Print(out,ini.T(284),pos3) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Print(out,ini.T(285),pos2) == FALSE )               return PROBLEM;
      for ( i=0; i<NumDissVar(); i++ )
      {
         if ( Print(out,DissVar(i)->Symbol(),pos3) == FALSE )
                                                               return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( Dissdiff(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer,pos2) == FALSE )                return PROBLEM;
      }
   }

   if ( NumPartVar() > 0 )
   {
      if ( Print(out,ini.T(286),pos1) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Print(out,ini.T(287),pos3) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Print(out,ini.T(288),pos2) == FALSE )               return PROBLEM;
      if ( Print(out,", ") == FALSE )                          return PROBLEM;
      if ( Print(out,ini.T(299)) == FALSE )                    return PROBLEM;
      for ( i=0; i<NumPartVar(); i++ )
      {
         if ( Print(out,PartVar(i)->Symbol(),pos3) == FALSE )
                                                               return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( PartRho(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer,pos2) == FALSE )                return PROBLEM;
         if ( Print(out,", ") == FALSE )                       return PROBLEM;
         if ( PartBioturb(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer) == FALSE )                     return PROBLEM;
      }
   }

   if ( Print(out,ini.T(289),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( SurfPor->UnParse(Buffer,sizeof(Buffer)-1) != 0 )       return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                      return PROBLEM;

   if ( Print(out,ini.T(290),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( RatePor->UnParse(Buffer,sizeof(Buffer)-1) != 0 )       return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                      return PROBLEM;

   if ( Print(out,ini.T(291),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,NumGridPts(),pos2) == FALSE )                return PROBLEM;
   if ( Print(out," (") == FALSE )                             return PROBLEM;
   if ( HighRes() == FALSE )
   {
      if ( Print(out,ini.T(292)) == FALSE )                    return PROBLEM;
   }
   else
   {
      if ( Print(out,ini.T(293)) == FALSE )                    return PROBLEM;
   }
   if ( Print(out,")") == FALSE )                              return PROBLEM;
   if ( Print(out,ini.T(294),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,ini.T(295),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,RelAccQ(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(296),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,AbsAccQ(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(297),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,RelAccPor(),pos2) == FALSE )                 return PROBLEM;
   if ( Print(out,ini.T(298),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,AbsAccPor(),pos2) == FALSE )                 return PROBLEM;

   return OK;
}

//////////////////////////////////////////////////////////////////////////////
