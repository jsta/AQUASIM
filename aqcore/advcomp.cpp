///////////////////////////////   advcomp.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    11.07.94    Peter Reichert
// revisions:   16.06.95    Peter Reichert    Q=0 enabled, A,z eliminated
//              19.06.95    Peter Reichert    individual diffusivities added
//              17.05.04    Peter Reichert    converted to ANSI C++
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
// type component (ioff=1)                       meaning
// ---------------------------------------------------------------------------
// alg  y[0]                                     Q at x[0]
// alg  y[ioff]                                  A*Cv[1] at x[0]
//  .    .                                        .
// alg  y[ioff+nv-1]                             A*Cv[nv] at x[0]
// diff y[ioff+nv]                               Cs[1] at x[0]
//  .    .                                        .
// diff y[ioff+nv+ns-1]                          Cs[ns] at x[0]
// alg  y[ioff+nv+ns+ne]                         Ce[1] at x[0]
//  .    .                                        .
// alg  y[ioff+nv+ns+ne-1]                       Ce[ne] at x[0]
// ---------------------------------------------------------------------------
// alg  y[ioff+nv+ns+ne]                         Q at x[1]
// diff y[ioff+nv+ns+ne+ioff]                    A*Cv[1] at x[1]
//  .    .                                        .
// diff y[ioff+nv+ns+ne+ioff+nv-1]               A*Cv[nv] at x[1]
// diff y[ioff+nv+ns+ne+ioff+nv]                 Cs[1] at x[1]
//  .    .                                        .
// diff y[ioff+nv+ns+ne+ioff+nv+ns-1]            Cs[ns] at x[1]
// alg  y[ioff+nv+ns+ne+ioff+nv+ns]              Ce[1] at x[1]
//  .    .                                        .
// alg  y[ioff+nv+ns+ne+ioff+nv+ns+ne-1]         Ce[ne] at x[1]
// ---------------------------------------------------------------------------
//  .    .                                        .
//  .    .                                        .
//
//
// ---------------------------------------------------------------------------
// alg  y[(n-1)*(ioff+nv+ns+ne)]                 Q at x[n-1]
// diff y[(n-1)*(ioff+nv+ns+ne)+ioff]            A*Cv[1] at x[n-1]
//  .    .                                        .
// diff y[(n-1)*(ioff+nv+ns+ne)+ioff+nv-1]       A*Cv[nv] at x[n-1]
// diff y[(n-1)*(ioff+nv+ns+ne)+ioff+nv]         Cs[1] at x[n-1]
//  .    .                                        .
// diff y[(n-1)*(ioff+nv+ns+ne)+ioff+nv+ns-1]    Cs[ns] at x[n-1]
// alg  y[(n-1)*(ioff+nv+ns+ne)+ioff+nv+ns]      Ce[1] at x[n-1]
//  .    .                                        .
// alg  y[(n-1)*(ioff+nv+ns+ne)+ioff+nv+ns+ne-1] Ce[ne] at x[n-1]
// ---------------------------------------------------------------------------
// alg  y[n*(ioff+nv+ns+ne)]                     diffusion of Cv[1] at x[n-1]
//  .    .                                        .
// alg  y[n*(ioff+nv+ns+ne)+nv-1]                diffusion of Cv[nv] at x[n-1]
//
//////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "fortran.h"
#include "formvar.h"
#include "othervar.h"
#include "varsys.h"
#include "advcomp.h"
#include "aqlink.h"

//////////////////////////////////////////////////////////////////////////////

const CARDINAL BufSize = 1024;

//////////////////////////////////////////////////////////////////////////////

CARDINAL ADVCOMP::ioff = 1;


void ADVCOMP::init()
{
   numinit = 0; initzone = 0; initvar = 0; initval = 0;

   inflow = 0;
   numinput = 0; inputvar = 0; inputflux = 0;
   latinflow = 0;
   numlatinput = 0; latinputvar = 0; latinputconc = 0;

   Xs = ini.R(17);
   Xe = ini.R(18);
   gridpts = 0; SetGridPts(12); SetGridPts(ini.I(1));
   relaccQ = 0.001;
   absaccQ = 1.0e-6;
   relaccD = 1.0e-6;
   absaccD = 1.0e-6;

   A = 0;
   withdiffusion = FALSE;
   globdiff = 0;
   numinddiff = 0; inddiffvar = 0; inddiffval = 0;
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


void ADVCOMP::del()
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
   delete globdiff;  globdiff  = 0;
   delete inddiffvar; inddiffvar = 0;
   for ( i=0; i<numinddiff; i++ ) delete inddiffval[i];
   delete inddiffval; inddiffval = 0; numinddiff = 0;
   delete [] Cout; Cout = 0;
   return;
}


ADVCOMP::ADVCOMP()
{
   init();
}


ADVCOMP::ADVCOMP(const ADVCOMP& com) : AQCOMP(com)
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
   withdiffusion = com.withdiffusion;
   if ( com.globdiff != 0 ) globdiff = new FORMVAR(com.globdiff);
   if ( com.numinddiff > 0 )
   {
      numinddiff = com.numinddiff;
      inddiffvar  = new AQVAR*[numinddiff];
      inddiffval = new FORMVAR*[numinddiff];
      for ( CARDINAL i=0; i<numinddiff; i++ )
      {
         inddiffvar[i] = com.inddiffvar[i];
         inddiffval[i] = new FORMVAR(com.inddiffval[i]);
      }
   }
   highres = com.highres;
}


ADVCOMP::ADVCOMP(const AQCOMP* com) : AQCOMP(com)
{
   init();
   if ( com != 0 )
   {
      if ( com->Type() == AdvComp )
      {
         ADVCOMP* ac = (ADVCOMP*)com;

         if ( ac->numinit > 0 )
         {
            numinit = ac->numinit;
            initzone = new CARDINAL[numinit];
            initvar  = new AQVAR*[numinit];
            initval  = new FORMVAR*[numinit];
            for ( CARDINAL i=0; i<numinit; i++ )
            {
               initzone[i] = ac->initzone[i];
               initvar[i]  = ac->initvar[i];
               initval[i]  = new FORMVAR(ac->initval[i]);
            }
         }

         inflow = new FORMVAR(ac->inflow);
         if ( ac->numinput > 0 )
         {
            numinput = ac->numinput;
            inputvar = new AQVAR*[numinput];
            inputflux = new FORMVAR*[numinput];
            for ( CARDINAL i=0; i<numinput; i++ )
            {
               inputvar[i] = ac->inputvar[i];
               inputflux[i] = new FORMVAR(ac->inputflux[i]);
            }
         }

         relaccQ = ac->relaccQ;
         absaccQ = ac->absaccQ;
         relaccD = ac->relaccD;
         absaccD = ac->absaccD;

         Xs = ac->Xs;
         Xe = ac->Xe;
         numgrid = ac->numgrid;
         if ( gridpts != 0 ) delete [] gridpts;
         gridpts = new REAL[numgrid];
         for ( CARDINAL i=0; i<numgrid; i++ ) gridpts[i] = ac->gridpts[i];
         latinflow = new FORMVAR(ac->latinflow);
         if ( ac->numlatinput > 0 )
         {
            numlatinput = ac->numlatinput;
            latinputvar  = new AQVAR*[numlatinput];
            latinputconc = new FORMVAR*[numlatinput];
            for ( CARDINAL i=0; i<numlatinput; i++ )
            {
               latinputvar[i] = ac->latinputvar[i];
               latinputconc[i] = new FORMVAR(ac->latinputconc[i]);
            }
         }
         A  = new FORMVAR(ac->A);
         withdiffusion = ac->withdiffusion;
         if ( ac->globdiff != 0 ) globdiff = new FORMVAR(ac->globdiff);
         if ( ac->numinddiff > 0 )
         {
            numinddiff = ac->numinddiff;
            inddiffvar = new AQVAR*[numinddiff];
            inddiffval = new FORMVAR*[numinddiff];
            for ( CARDINAL i=0; i<numinddiff; i++ )
            {
               inddiffvar[i] = ac->inddiffvar[i];
               inddiffval[i] = new FORMVAR(ac->inddiffval[i]);
            }
         }
         highres = ac->highres;
      }
   }
}


ADVCOMP::~ADVCOMP()
{
   del();
}


const char* ADVCOMP::TypeName() const
{
   return ini.T(91);
}


AQVAR* ADVCOMP::SpaceVar(const VARSYS* varsys) const
{
   return varsys->GetProgVar(SpaceX);
}


BOOLEAN ADVCOMP::AllowedSpaceVal(REAL x) const
{
   if ( (x<gridpts[0]) && (x<gridpts[numgrid-1]) ) return FALSE;
   if ( (x>gridpts[0]) && (x>gridpts[numgrid-1]) ) return FALSE;
   return TRUE;
}


BOOLEAN ADVCOMP::AllowedProgVarType(PROGVARTYPE type) const
{
   if ( type == CalcNumber )    return TRUE;
   if ( type == Time )          return TRUE;
   if ( type == CompIndex )     return TRUE;
   if ( type == ZoneIndex )     return TRUE;
   if ( type == WaterFraction ) return TRUE;
   if ( type == SpaceX )        return TRUE;
   if ( type == Discharge )     return TRUE;
   if ( type == CrossSection )  return TRUE;
   return FALSE;
}


BOOLEAN ADVCOMP::AllowedComp() const
{
   if ( A==0 )     return FALSE;

   if ( A->ArgVarType(StateVar) == TRUE )                   return FALSE;
   if ( globdiff != 0 )
   {
      if ( globdiff->ArgVarType(StateVar) == TRUE )         return FALSE;
   }
   if ( numinddiff > 0 )
   {
      for ( CARDINAL i=0; i<numinddiff; i++ )
      {
         if ( inddiffval[i]->ArgVarType(StateVar) == TRUE ) return FALSE;
      }
   }

   return TRUE;
}


BOOLEAN ADVCOMP::AllowedReplaceVar(const AQVAR* oldvar,
                                    const AQVAR* newvar) const
{
   if ( AQCOMP::AllowedReplaceVar(oldvar,newvar) == FALSE )   return FALSE;
   if ( A->Arg(oldvar) == TRUE )
   {
      if ( newvar->ArgVarType(StateVar) == TRUE )             return FALSE;
   }
   if ( globdiff != 0 )
   {
      if ( globdiff->Arg(oldvar) == TRUE )
      {
         if ( newvar->ArgVarType(StateVar) == TRUE )          return FALSE;
      }
   }
   if ( numinddiff > 0 )
   {
      for ( CARDINAL i=0; i<numinddiff; i++ )
      {
         if ( inddiffval[i]->Arg(oldvar) == TRUE )
         {
            if ( newvar->ArgVarType(StateVar) == TRUE )       return FALSE;
         }
      }
   }
   return TRUE;
}


BOOLEAN ADVCOMP::AllowedExchangeVar(const AQVAR* var1,
                                     const AQVAR* var2) const
{
   if ( AQCOMP::AllowedExchangeVar(var1,var2) == FALSE )    return FALSE;
   if ( (A->Arg(var1)==TRUE) && (A->Arg(var2)==FALSE) )
   {
      if ( var2->ArgVarType(StateVar) == TRUE )             return FALSE;
   }
   if ( (A->Arg(var1)==FALSE) && (A->Arg(var2)==TRUE) )
   {
      if ( var1->ArgVarType(StateVar) == TRUE )             return FALSE;
   }
   if ( globdiff != 0 )
   {
      if ( (globdiff->Arg(var1)==TRUE) && (globdiff->Arg(var2)==FALSE) )
      {
         if ( var2->ArgVarType(StateVar) == TRUE )          return FALSE;
      }
      if ( (globdiff->Arg(var1)==FALSE) && (globdiff->Arg(var2)==TRUE) )
      {
         if ( var1->ArgVarType(StateVar) == TRUE )          return FALSE;
      }
   }
   if ( numinddiff > 0 )
   {
      for ( CARDINAL i=0; i<numinddiff; i++ )
      {
         if ( (inddiffval[i]->Arg(var1)==TRUE)
                                && (inddiffval[i]->Arg(var2)==FALSE) )
         {
            if ( var2->ArgVarType(StateVar) == TRUE )       return FALSE;
         }
         if ( (inddiffval[i]->Arg(var1)==FALSE)
                                 && (inddiffval[i]->Arg(var2)==TRUE) )
         {
            if ( var1->ArgVarType(StateVar) == TRUE )       return FALSE;
         }
      }
   }
   return TRUE;
}


void ADVCOMP::ReplVar(AQVAR* oldvar, AQVAR* newvar)
{
   CARDINAL i;

   if ( AllowedReplaceVar(oldvar,newvar) == FALSE )
      FatalError("ADVCOMP::ReplVar: Illegal replace");
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
   if ( globdiff != 0 ) globdiff->ReplaceVarArg(oldvar,newvar);
   for ( i=0; i<numinddiff; i++ )
   {
      if ( inddiffvar[i] == oldvar ) inddiffvar[i] = newvar;
      inddiffval[i]->ReplaceVarArg(oldvar,newvar);
   }
   CalcArg();
   return;
}


void ADVCOMP::ExchVar(AQVAR* var1, AQVAR* var2)
{
   CARDINAL i;

   if ( AllowedExchangeVar(var1,var2) == FALSE )
      FatalError("ADVCOMP::ExchVar: Illegal exchange");
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
   if ( globdiff != 0 )
   {
      globdiff->ReplaceVarArg(var1,v);
      globdiff->ReplaceVarArg(var2,var1);
      globdiff->ReplaceVarArg(v,var2);
   }
   for ( i=0; i<numinddiff; i++ )
   {
      if ( inddiffvar[i] == var1 )
      {
         inddiffvar[i] = var2;
      }
      else
      {
         if ( inddiffvar[i] == var2 ) inddiffvar[i] = var1;
      }
      inddiffval[i]->ReplaceVarArg(var1,v);
      inddiffval[i]->ReplaceVarArg(var2,var1);
      inddiffval[i]->ReplaceVarArg(v,var2);
   }
   delete v;
   CalcArg();
   return;
}


BOOLEAN ADVCOMP::LatInflow(const VARLIST* varlist,
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


AQVAR* ADVCOMP::LatInputVar(CARDINAL index) const
{
   if ( index >= numlatinput )
      FatalError("ADVCOMP::LatInputVar: Illegal index");
   return latinputvar[index];
}


const FORMVAR* ADVCOMP::LatInputConc(CARDINAL index) const
{
   if ( index >= numlatinput )
      FatalError("ADVCOMP::LatInputConc: Illegal index");
   return latinputconc[index];
}


BOOLEAN ADVCOMP::AddLatInput(const VARLIST* varlist, AQVAR* var,
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


BOOLEAN ADVCOMP::ReplaceLatInput(const VARLIST* varlist, AQVAR* var,
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


BOOLEAN ADVCOMP::DeleteLatInput(CARDINAL pos)
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


CARDINAL ADVCOMP::NumEq() const
{
   return NumGridPts()*(ioff+NumStateVar())+numvolstatevar;
}


REAL ADVCOMP::RelAcc(CARDINAL i) const
{
   if ( i >= NumEq() ) FatalError("ADVCOMP::RelAcc: Illegal index");
   if ( i >= NumEq()-numvolstatevar ) return relaccD;
   CARDINAL index = i % (ioff+NumStateVar());
   if ( index == 0 ) return relaccQ;
   return statevar[index-ioff]->RelAccuracy();
}


REAL ADVCOMP::AbsAcc(CARDINAL i) const
{
   if ( i >= NumEq() ) FatalError("ADVCOMP::AbsAcc: Illegal index");
   if ( i >= NumEq()-numvolstatevar ) return absaccD;
   CARDINAL index = i % (ioff+NumStateVar());
   if ( index == 0 ) return absaccQ;
   return statevar[index-ioff]->AbsAccuracy();
}


EQTYPE ADVCOMP::EqType(CARDINAL i) const
{
   if ( i >= NumEq() ) FatalError("ADVCOMP::EqType: Illegal index");
   if ( i >= NumEq()-numvolstatevar ) return ALG;
   CARDINAL index = i % (ioff+numstatevar);

   if ( index >= ioff+numdynstatevar ) return ALG;  // EquStatevar
   if ( index >= ioff+numvolstatevar ) return DIFF; // SurfStatevar

   if ( i < ioff+numstatevar )                // Upstream boundary cond.:
   {
      return ALG;                             // Q,VolStatevar
   }
   if ( i >= (numgrid-1)*(ioff+numstatevar) ) // Downstream boundary cond.:
   {
      if ( index == 0 ) return ALG;           // Q
      if ( WithDiffusion() == TRUE )         // VolStatevar
      {
         return ALG;
      }
      else
      {
         return DIFF;
      }
   }
   else                                       // Equations in cells:
   {
      if ( index == 0 ) return ALG;           // Q
      return DIFF;                            // VolStatevar
   }
}


BOOLEAN ADVCOMP::InitCond(VARSYS* varsys, REAL* Y, CARDINAL callnum)
{
   if ( Active() != TRUE ) return FALSE;
   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   REAL zoneindex = 0.0; varsys->SetProgVar(ZoneIndex,&zoneindex);
   REAL eps = 1.0; varsys->SetProgVar(WaterFraction,&eps);
   Xvar = varsys->GetProgVar(SpaceX);
   Avar = varsys->GetProgVar(CrossSection);
   Qvar = varsys->GetProgVar(Discharge);

   if ( callnum>0 ) return TRUE;
   REAL* y = &Y[ind_y];
   CARDINAL i,j,k;
   REAL* areas = new REAL[numgrid];

   CARDINAL cell = ioff+numstatevar;
   BOOLEAN found = FALSE;                  // Discharge Q
   for ( j=0; j<numinit; j++ )
   {
      if ( initvar[j] == Qvar )
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
            *cjob << "\n   ADVCOMP numerical problem:"
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

   for ( j=0; j<NumGridPts(); j++ )
   {
      varsys->Reset();
      if ( Xvar != 0 ) Xvar->SetProgVar(&gridpts[j]);
      areas[j] = A->Value();
   }

   for ( i=0; i<numstatevar; i++ )            // Concentrations C
   {
      BOOLEAN found = FALSE;
      for ( j=0; j<NumInitCond(); j++ )
      {
         if ( initvar[j] == statevar[i] )
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

   for ( i=0; i<numvolstatevar; i++ )        // VolStateVar: A*C
   {
      for ( j=0; j<NumGridPts(); j++ )
      {
         y[j*cell+ioff+i] *= areas[j];
      }
   }

   if ( withdiffusion == TRUE )
   {
      varsys->Reset();
      if ( Xvar != 0 ) Xvar->SetProgVar(&gridpts[numgrid-1]);
      if ( Qvar != 0 ) Qvar->SetProgVar(&y[(numgrid-1)*cell]);
      if ( Avar != 0 ) Avar->SetProgVar(&areas[numgrid-1]);
      for ( i=0; i<numvolstatevar; i++ )
      {
         found = FALSE;
         for ( j=0; j<NumIndDiff(); j++ )
         {
            if ( statevar[i] == inddiffvar[j] )
            {
               k = j;
               found = TRUE;
               break;
            }
         }
         if ( found == TRUE )
         {
            y[numgrid*cell+i] = inddiffval[k]->Value();
         }
         else
         {
            y[numgrid*cell+i] = globdiff->Value();
         }
      }
   }
   else
   {
      for ( i=0; i<numvolstatevar; i++ )
      {
         y[numgrid*cell+i] = 0.0;
      }
   }
   delete areas;

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


REAL ADVCOMP::FluxLimit(REAL s1, REAL s2, CARDINAL limiter)
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


BOOLEAN ADVCOMP::Delta(const NUMPAR&, VARSYS* varsys,
                       const REAL* Y, const REAL* YT, REAL* DELTA)
{
   if ( Active() != TRUE ) return FALSE;
   const REAL* y  = &Y[ind_y];
   const REAL* yt = &YT[ind_y];
   REAL* delta = &DELTA[ind_y];
   CARDINAL i,j,k;
   CARDINAL cell = ioff+numstatevar;
   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   REAL zoneindex = 0.0; varsys->SetProgVar(ZoneIndex,&zoneindex);
   REAL eps = 1.0;       varsys->SetProgVar(WaterFraction,&eps);
   Xvar = varsys->GetProgVar(SpaceX);
   Avar = varsys->GetProgVar(CrossSection);
   Qvar = varsys->GetProgVar(Discharge);
   REAL*  conc  = new REAL[numstatevar];
   REAL*  flux  = new REAL[numstatevar];
   REAL*  Qs    = new REAL[numgrid];
   REAL*  areas = new REAL[numgrid];
   REAL*  qlat  = new REAL[numgrid];
   REAL** diffs = new REAL*[numgrid];
   for ( j=0; j<numgrid; j++ ) diffs[j] = new REAL[numvolstatevar];
   FORMVAR** diffvars = new FORMVAR*[numvolstatevar];
   for ( i=0; i<numvolstatevar; i++ )
   {
      BOOLEAN found = FALSE;
      for ( j=0; j<NumIndDiff(); j++ )
      {
         if ( statevar[i] == inddiffvar[j] )
         {
            k = j;
            found = TRUE;
            break;
         }
      }
      if ( found == TRUE )
      {
         diffvars[i] = inddiffval[k];
      }
      else
      {
         diffvars[i] = globdiff;
      }
   }

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
      for ( CARDINAL j=0; j<numinput; j++ )
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
         *cjob << "\n   ADVCOMP numerical problem:"
               << " Inflow is negative";
         cjob->flush();
         return FALSE;
      }
      Q += Qext;
   }

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
      varsys->Reset();
      if ( Xvar != 0 ) Xvar->SetProgVar(&gridpts[j]);
      if ( Qvar != 0 ) Qvar->SetProgVar(&Qs[j]);
      areas[j] = A->Value();
      if ( Avar != 0 ) Avar->SetProgVar(&areas[j]);
      for ( i=0; i<numvolstatevar; i++ )                 // VolStateVar
      {
         conc[i] = y[j*cell+ioff+i]/areas[j];
      }
      for ( i=numvolstatevar; i<numdynstatevar; i++ )    // SurfStateVar
      {
         conc[i] = y[j*cell+ioff+i];
      }
      for ( i=numdynstatevar; i<numstatevar; i++ )       // EquStateVar
      {
         conc[i] = y[j*cell+ioff+i];
      }
      if ( latinflow == 0 ) qlat[j] = 0.0;
      else                  qlat[j] = latinflow->Value();
      CalcRates(&delta[j*cell+ioff]);
      for ( i=0; i<numvolstatevar; i++ )
      {
         delta[j*cell+ioff+i] = delta[j*cell+ioff+i]*areas[j];
         if ( qlat[j] > 0.0 )
         {
            for ( CARDINAL k=0; k<numlatinput; k++ )
            {
               if ( latinputvar[k] == statevar[i] )
               {
                  // if condition added 02.03.01:
                  if ( j == 1 )
                  {
                     delta[j*cell+ioff+i] += 0.5*qlat[j]*latinputconc[k]->Value();
                  }
                  else
                  {
                     delta[j*cell+ioff+i] += qlat[j]*latinputconc[k]->Value();
                  }
               }
            }
         }
         else                                 // qlat<0 !
         {
            // if condition added 02.03.01:
            if ( j == 1 )
            {
               delta[j*cell+ioff+i] += 0.5*qlat[j]*conc[i];
            }
            else
            {
               delta[j*cell+ioff+i] += qlat[j]*conc[i];
            }
         }
      }
      if ( withdiffusion == TRUE )
      {
         for ( i=0; i<numvolstatevar; i++ )
         {
            diffs[j][i] = diffvars[i]->Value();
            if ( diffs[j][i] <= 0.0 )
            {
               *cjob << "\n   ADVCOMP numerical problem:"
                     << " Diffusion coefficient is not positive";
               cjob->flush();
               return FALSE;
            }
         }
      }
      else
      {
         for ( i=0; i<numvolstatevar; i++ )
         {
            diffs[j][i] = 0.0;
         }
      }
   }

   REAL advflux1,advflux2,diffflux;     // fluxes
   REAL dx    = fabs(gridpts[2]-gridpts[1]);  // cells of equal length
   REAL dxinv = 1.0/dx;                       // inverse zell length
   REAL s1, s2;
   REAL phi = 0.0;

   // equations at the upstream boundary (grid point 0):
   // --------------------------------------------------

   delta[0] = y[0] - Q;                                      // Q

   for ( i=0; i<numvolstatevar; i++ )                        // VolStateVar
   {
      if ( withdiffusion == TRUE )
      {
         delta[ioff+i] = Qs[0]/areas[0]*y[ioff+i] - flux[i]
                         -areas[0]*diffs[0][i]*(y[cell+ioff+i]/areas[1]
                                                    -y[ioff+i]/areas[0])
                          *2.0*dxinv;
      }
      else
      {
         if ( Q > 0.0 )       //  if ( y[0] > 0.0 ) changed 10.08.95
         {
            delta[ioff+i] = Q/areas[0]*y[ioff+i] - flux[i];
         }
         else // conc. equal to grid point 1 if Q=0 and no dispersion
         {
            delta[ioff+i] = y[cell+ioff+i]-y[ioff+i];     // 10.08.95
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

   // equations in the first cell (gridpoint 1):
   // ------------------------------------------

   // recalculate fluxes with actual values to minimize codiagonals:
   for ( i=0; i<numvolstatevar; i++ )
   {
      flux[i] = Q*y[ioff+i]/areas[0];
      if ( withdiffusion == TRUE )
      {
         diffflux =  -areas[0]*diffs[0][i]*(y[cell+ioff+i]/areas[1]
                                                -y[ioff+i]/areas[0])
                      *2.0*dxinv;
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
      delta[cell+ioff+i] += flux[i]*dxinv;
      // calculate flux at the right boundary of the cell:
      advflux1 = Qs[1]*y[cell+ioff+i]/areas[1];
      advflux2 = 0.5*(Qs[1]*y[cell+ioff+i]/areas[1]
                     +Qs[2]*y[2*cell+ioff+i]/areas[2]);
      s1 = (y[cell+ioff+i]/areas[1]-y[ioff+i]/areas[0])
           / fabs(gridpts[1]-gridpts[0]);
      s2 = (y[2*cell+ioff+i]/areas[2]-y[cell+ioff+i]/areas[1])
           / fabs(gridpts[2]-gridpts[1]);
      phi = FluxLimit(s1,s2,ini.I(4));

      if ( fabs(y[ioff+i]/areas[0])
          +fabs(y[cell+ioff+i]/areas[1])
          +fabs(y[2*cell+ioff+i]/areas[2]) 
              < 0.001*statevar[i]->AbsAccuracy() ) phi=0;  // test 2.6.97

      flux[i] = (1.0-phi)*advflux1 + phi*advflux2;
      if ( withdiffusion == TRUE )   // add dispersive flux
      {
         diffflux =  -(areas[1]+areas[2])
                      *(diffs[1][i]+diffs[2][i])*0.25
                      *(y[2*cell+ioff+i]/areas[2]
                         -y[cell+ioff+i]/areas[1])
                      *dxinv;
         flux[i] += diffflux;
      }
      // subtract flux at the right boundary of the cell:
//      delta[cell+ioff+i] -= flux[i]*2.0*dxinv;   correction 16.04.96:
      delta[cell+ioff+i] -= flux[i]*dxinv;
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
      delta[j*cell]   = y[j*cell] - Qs[j-1]                  // Q
                        - 0.5*(qlat[j]+qlat[j-1])
                             *(gridpts[j]-gridpts[j-1]);

      for ( i=0; i<numvolstatevar; i++ )                     // VolStateVar
      {
         // add flux at the left boundary of the cell:
         delta[j*cell+ioff+i] += flux[i]*dxinv;
         // calculate flux at the right boundary of the cell:
         advflux1 = Qs[j]*y[j*cell+ioff+i]/areas[j];
         advflux2 = 0.5*(Qs[j]*y[j*cell+ioff+i]/areas[j]
                        +Qs[j+1]*y[(j+1)*cell+ioff+i]/areas[j+1]);
         s1 = (y[j*cell+ioff+i]/areas[j]
               -y[(j-1)*cell+ioff+i]/areas[j-1])
              / fabs(gridpts[j]-gridpts[j-1]);
         s2 = (y[(j+1)*cell+ioff+i]/areas[j+1]
               -y[j*cell+ioff+i]/areas[j])
              / fabs(gridpts[j+1]-gridpts[j]);
         phi = FluxLimit(s1,s2,ini.I(4));

         if ( fabs(y[(j-1)*cell+ioff+i]/areas[j-1])
             +fabs(y[j*cell+ioff+i]/areas[j])
             +fabs(y[(j+1)*cell+ioff+i]/areas[j+1])
                        < 0.001*statevar[i]->AbsAccuracy() ) phi=0;  // test 2.6.97

         flux[i] = (1.0-phi)*advflux1 + phi*advflux2;
         if ( withdiffusion == TRUE )
         {
            diffflux =  -(areas[j]+areas[j+1])
                         *(diffs[j][i]+diffs[j+1][i])*0.25
                         *(y[(j+1)*cell+ioff+i]/areas[j+1]
                           -y[j*cell+ioff+i]/areas[j])
                         *dxinv;
            flux[i] += diffflux;
         }
         // subtract flux at the right boundary of the cell:
         delta[j*cell+ioff+i] -= flux[i]*dxinv;
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

   delta[j*cell]   = y[j*cell] - Qs[j-1]                     // Q
                     - 0.5*(qlat[j]+qlat[j-1])
                          *(gridpts[j]-gridpts[j-1]);

   for ( i=0; i<numvolstatevar; i++ )                        // VolStateVar
   {
      // add flux at the left boundary of the cell:
      delta[j*cell+ioff+i] += flux[i]*dxinv;
      // calculate flux at the right boundary of the cell:
      advflux1 = Qs[j]*y[j*cell+ioff+i]/areas[j];
      advflux2 = Qs[j+1]*y[(j+1)*cell+ioff+i]/areas[j+1];
      s1 = (y[j*cell+ioff+i]/areas[j]
            -y[(j-1)*cell+ioff+i]/areas[j-1])
           / fabs(gridpts[j]-gridpts[j-1]);
      s2 = (y[(j+1)*cell+ioff+i]/areas[j+1]
            -y[j*cell+ioff+i]/areas[j])
           / fabs(gridpts[j+1]-gridpts[j]);
      phi = FluxLimit(s1,s2,ini.I(4));

      if ( fabs(y[(j-1)*cell+ioff+i]/areas[j-1])
          +fabs(y[j*cell+ioff+i]/areas[j])
          +fabs(y[(j+1)*cell+ioff+i]/areas[j+1])
                     < 0.001*statevar[i]->AbsAccuracy() ) phi=0;  // test 2.6.97

      flux[i] = (1.0-phi)*advflux1 + phi*advflux2;
      if ( withdiffusion == TRUE )
      {
         diffflux =  -areas[j+1]*diffs[j+1][i]
                      *(y[(j+1)*cell+ioff+i]/areas[j+1]
                        -y[j*cell+ioff+i]/areas[j])
                      *2.0*dxinv;
         flux[i] += diffflux;
      }
      // subtract flux at the right boundary of the cell:
      delta[j*cell+ioff+i] -= flux[i]*dxinv;
      // subtract time derivative:
      delta[j*cell+ioff+i] -= yt[j*cell+ioff+i];
   }

   for ( i=numvolstatevar; i<numdynstatevar; i++ )           // SurfStateVar
   {
      delta[j*cell+ioff+i] -= yt[j*cell+ioff+i];
   }

   /* numdynstatevar <= i < numstatevar */                   // EquStateVar

   // equations at the downstream boundary (grid point n-1):
   // ------------------------------------------------------

   j = NumGridPts()-1;

   delta[j*cell]   = y[j*cell] - Qs[j-1]                     // Q
                     - 0.5*(qlat[j]+qlat[j-1])
                          *(gridpts[j]-gridpts[j-1]);

   for ( i=0; i<numvolstatevar; i++ )                        // VolStateVar
   {
      if ( withdiffusion == TRUE )
      {
         if ( Qs[numgrid-1]/diffs[numgrid-1][i]
              /fabs(gridpts[numgrid-1]-gridpts[0]) < 1e-8 )
         {
            // dC/dx = 0:
            delta[j*cell+ioff+i] = y[(j-1)*cell+ioff+i]/areas[j-1]
                                   - y[j*cell+ioff+i]/areas[j];
         }
         else
         {
            // d2C/dx2 = 0:
            delta[j*cell+ioff+i] = 2*y[j*cell+ioff+i]/areas[j]
                                   -3*y[(j-1)*cell+ioff+i]/areas[j-1]
                                   +y[(j-2)*cell+ioff+i]/areas[j-2];
         }
      }
      else
      {
         delta[j*cell+ioff+i] += (y[(j-1)*cell]*y[(j-1)*cell+ioff+i]
                                                     /areas[j-1]
                                 -y[j*cell]*y[j*cell+ioff+i]/areas[j])
                                  * 2.0*dxinv;
         delta[j*cell+ioff+i] -= yt[j*cell+ioff+i];
      }
   }

   for ( i=numvolstatevar; i<numdynstatevar; i++ )           // SurfStateVar
   {
      delta[j*cell+ioff+i] -= yt[j*cell+ioff+i];
   }
   /* numdynstatevar <= i < numstatevar */                   // EquStateVar

   for ( i=0; i<numvolstatevar; i++ )                        // Diffusivities
   {
      delta[NumGridPts()*cell+i]
                   = y[NumGridPts()*cell+i]-diffs[NumGridPts()-1][i];
   }

   for ( i=0; i<numstatevar; i++ ) statevar[i]->ResetStateVar();
   delete [] conc;
   delete [] flux;
   delete [] Qs;
   delete [] areas;
   delete [] qlat;
   for ( j=0; j<numgrid; j++ ) delete [] diffs[j];
   delete [] diffs;
   delete [] diffvars;

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

   return TRUE;
}


BOOLEAN ADVCOMP::GridValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                           REAL* t_ptr, const REAL* Y,
                           CARDINAL zone, CARDINAL pt, AQVAR* var,
                           REAL& value)
{
   if ( Active() != TRUE ) return FALSE;
   SetGlobalState(varsys,calcnum_ptr,t_ptr,Y);
   CARDINAL i;
   CARDINAL cell = ioff+numstatevar;
   if ( zone != 0 )                   return FALSE;
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
   varsys->Reset();
   if ( Xvar != 0 ) Xvar->SetProgVar(&gridpts[pt]);
   REAL a = A->Value();
   if ( Avar != 0 ) Avar->SetProgVar(&a);
   if ( Qvar != 0 ) Qvar->SetProgVar(&y[pt*cell]);
   for ( i=0; i<numstatevar; i++ ) statevar[i]->SetStateVar(&conc[i]);
   for ( i=0; i<numvolstatevar; i++ )
   {
      conc[i] = y[pt*cell+ioff+i]/a;
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
   for ( i=0; i<numstatevar; i++ ) statevar[i]->ResetStateVar();
   delete conc;
   return TRUE;
}


BOOLEAN ADVCOMP::SpaceValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
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
               FatalError("ADVCOMP::Value: index negative");
         }
      }
      else
      {
         while ( x > gridpts[i+1] )
         {
            i++;
            if ( i >= numgrid-1 )
               FatalError("ADVCOMP::Value: index too large");
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
               FatalError("ADVCOMP::Value: index negative");
         }
      }
      else
      {
         while ( x < gridpts[i+1] )
         {
            i++;
            if ( i >= numgrid-1 )
               FatalError("ADVCOMP::Value: index too large");
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


CARDINAL ADVCOMP::InitZone(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("ADVCOMP::InitZone: Illegal index");
   return initzone[index];
}


AQVAR* ADVCOMP::InitVar(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("ADVCOMP::InitVar: Illegal index");
   return initvar[index];
}


const FORMVAR* ADVCOMP::InitVal(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("ADVCOMP::InitVal: Illegal index");
   return initval[index];
}


BOOLEAN ADVCOMP::AddInitCond(const VARLIST* varlist, CARDINAL zone,
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


BOOLEAN ADVCOMP::ReplaceInitCond(const VARLIST* varlist, CARDINAL zone,
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


BOOLEAN ADVCOMP::DeleteInitCond(CARDINAL pos)
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


BOOLEAN ADVCOMP::Inflow(const VARLIST* varlist,
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


AQVAR* ADVCOMP::InputVar(CARDINAL index) const
{
   if ( index >= numinput )
      FatalError("ADVCOMP::InputVar: Illegal index");
   return inputvar[index];
}


const FORMVAR* ADVCOMP::InputFlux(CARDINAL index) const
{
   if ( index >= numinput )
      FatalError("ADVCOMP::InputFlux: Illegal index");
   return inputflux[index];
}


BOOLEAN ADVCOMP::AddInput(const VARLIST* varlist, AQVAR* var,
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


BOOLEAN ADVCOMP::ReplaceInput(const VARLIST* varlist, AQVAR* var,
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


BOOLEAN ADVCOMP::DeleteInput(CARDINAL pos)
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


BOOLEAN ADVCOMP::AccQ(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccQ = relacc;
   absaccQ = absacc;
   return TRUE;
}


BOOLEAN ADVCOMP::AccD(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccD = relacc;
   absaccD = absacc;
   return TRUE;
}


void ADVCOMP::CalcArg()
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
   if ( globdiff != 0 )    numvararg += globdiff->NumVarArg();
   numvararg += numinddiff;
   for ( j=0; j<numinddiff; j++ )
      numvararg += inddiffval[j]->NumVarArg();
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
      if ( globdiff != 0 )
      {
         for ( i=0; i<globdiff->NumVarArg(); i++ )
         {
            newarg[k] = globdiff->VarArg(i); k++;
         }
      }
      for ( i=0; i<numinddiff; i++ )
      {
         newarg[k] = inddiffvar[i]; k++;
         for ( j=0; j<inddiffval[i]->NumVarArg(); j++ )
         {
            newarg[k] = inddiffval[i]->VarArg(j); k++;
         }
      }
      if ( k != numvararg )
         FatalError("ADVCOMP::CalcArg: inconsistent number of arguments");
      delete [] vararg; vararg = newarg;
   }
   return;
}


BOOLEAN ADVCOMP::SetGridPts(CARDINAL n)
{
   if ( n < 5 )        return FALSE;
   if ( n == numgrid ) return TRUE;
   numgrid = n;
   delete gridpts; gridpts = new REAL[numgrid];
   XStartEnd(Xs,Xe);
   return TRUE;
}


BOOLEAN ADVCOMP::NumGridPts(CARDINAL n)
{
   if ( InList() == TRUE ) return FALSE;
   return SetGridPts(n);
}


REAL ADVCOMP::GridPt(const REAL*, CARDINAL index)
{
   if ( index >= NumGridPts() )
      FatalError("ADVCOMP::GridPt: illegal index");
   return gridpts[index];
}


CARDINAL ADVCOMP::NumZoneGridPts(CARDINAL zone)
{
   if ( zone != 0 ) FatalError("ADVCOMP::NumZoneGridPts: illegal zone");
   return NumGridPts();
}


REAL ADVCOMP::ZoneGridPt(const REAL* Y, CARDINAL zone,
                         CARDINAL index)
{
   if ( zone != 0 ) FatalError("ADVCOMP::ZoneGridPt: illegal zone");
   return GridPt(Y,index);
}


BOOLEAN ADVCOMP::XStartEnd(REAL xs, REAL xe)
{
   if ( xs == xe ) return FALSE;
   Xs = xs; Xe = xe;
   REAL dx = (Xe-Xs)/(numgrid-2);
   gridpts[0] = Xs; gridpts[numgrid-1] = Xe;
   for ( CARDINAL i=1; i<numgrid-1; i++ ) gridpts[i] = Xs + (i-0.5)*dx;
   return TRUE;
}


BOOLEAN ADVCOMP::CrossSect(const char* inpline,
                            const VARLIST* varlist,
                            char* errorline,
                            CARDINAL& numparseerrors)
{
   numparseerrors = 0;
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


BOOLEAN ADVCOMP::WithDiffusion(BOOLEAN b)
{
   if ( b==TRUE && globdiff==0 ) return FALSE;
   withdiffusion = b;
   return TRUE;
}


BOOLEAN ADVCOMP::GlobDiff(const char* inpline,
                          const VARLIST* varlist,
                          char* errorline,
                          CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' )
   {
      if ( WithDiffusion() == FALSE )
      {
         delete globdiff; globdiff = 0;
      }
      return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("E");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;
      if ( WithDiffusion() == FALSE )
      {
         delete globdiff; globdiff = 0;
      }
      return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;
      if ( WithDiffusion() == FALSE )
      {
         delete globdiff; globdiff = 0;
      }
      return FALSE;
   }

   delete globdiff; globdiff = formvar;
   CalcArg();
   return TRUE;
}


AQVAR* ADVCOMP::IndDiffVar(CARDINAL index) const
{
   if ( index >= numinddiff )
      FatalError("ADVCOMP::IndDiffVar: Illegal index");
   return inddiffvar[index];
}


const FORMVAR* ADVCOMP::IndDiffVal(CARDINAL index) const
{
   if ( index >= numinddiff )
      FatalError("ADVCOMP::IndDiffVal: Illegal index");
   return inddiffval[index];
}


BOOLEAN ADVCOMP::AddIndDiff(const VARLIST* varlist, AQVAR* var,
                            const char* inpline, char* errorline,
                            CARDINAL& numparseerrors, CARDINAL pos)
{
   numparseerrors = 0;
   if ( InList() == TRUE )             return FALSE;
   if ( inpline == 0 )                 return FALSE;
   if ( inpline[0] == '\0' )           return FALSE;
   if ( varlist->Exist(var) == FALSE ) return FALSE;
   if ( pos > numinddiff ) pos = numinddiff;
   CARDINAL i;
   for ( i=0; i<numinddiff; i++ )
   {
      if ( inddiffvar[i] == var )     return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("IndDiff");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                  return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                  return FALSE;
   }
   AQVAR**   newdiffvar = new AQVAR*[numinddiff+1];
   FORMVAR** newdiffval = new FORMVAR*[numinddiff+1];
   for ( i=0; i<pos; i++ )
   {
      newdiffvar[i] = inddiffvar[i];
      newdiffval[i] = inddiffval[i];
   }
   newdiffvar[pos] = var;
   newdiffval[pos] = formvar;
   for ( i=pos; i<numinddiff; i++ )
   {
      newdiffvar[i+1] = inddiffvar[i];
      newdiffval[i+1] = inddiffval[i];
   }
   delete inddiffvar; inddiffvar = newdiffvar;
   delete inddiffval; inddiffval = newdiffval;
   numinddiff++;
   CalcArg();
   return TRUE;
}


BOOLEAN ADVCOMP::ReplaceIndDiff(const VARLIST* varlist, AQVAR* var,
                                const char* inpline, char* errorline,
                                CARDINAL& numparseerrors, CARDINAL pos)
{
   numparseerrors = 0;
   if ( InList() == TRUE )                 return FALSE;
   if ( inpline == 0 )                     return FALSE;
   if ( inpline[0] == '\0' )               return FALSE;
   if ( varlist->Exist(var) == FALSE )     return FALSE;
   if ( pos >= numinddiff )               return FALSE;
   CARDINAL i;
   for ( i=0; i<numinddiff; i++ )
   {
      if ( i!=pos && inddiffvar[i]==var ) return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("IndDiff");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                      return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                      return FALSE;
   }
   delete inddiffval[pos];
   inddiffvar[pos] = var;
   inddiffval[pos] = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN ADVCOMP::DeleteIndDiff(CARDINAL pos)
{
   if ( pos >= numinddiff ) return FALSE;
   AQVAR**   newdiffvar = new AQVAR*[numinddiff-1];
   FORMVAR** newdiffval = new FORMVAR*[numinddiff-1];
   CARDINAL i;
   for ( i=0; i<pos; i++ )
   {
      newdiffvar[i] = inddiffvar[i];
      newdiffval[i] = inddiffval[i];
   }
   for ( i=pos+1; i<numinddiff; i++ )
   {
      newdiffvar[i-1] = inddiffvar[i];
      newdiffval[i-1] = inddiffval[i];
   }
   delete inddiffval[pos];
   delete inddiffvar; inddiffvar = newdiffvar;
   delete inddiffval; inddiffval = newdiffval;
   numinddiff--;
   CalcArg();
   return TRUE;
}


CARDINAL ADVCOMP::NumZones() const
{
   return 1;
}


const char* ADVCOMP::ZoneName(CARDINAL index) const
{
   if ( index > 0 )
      FatalError("ADVCOMP::ZoneName: Illegal index");
   return ini.T(92);
}


const char* ADVCOMP::AdvInName(CARDINAL conn) const
{
   if ( conn > 0 ) FatalError("ADVCOMP::AdvInName: Illegal connection");
   return ini.T(93);
}


const char* ADVCOMP::AdvOutName(CARDINAL conn) const
{
   if ( conn > 0 ) FatalError("ADVCOMP::AdvOutName: Illegal connection");
   return ini.T(94);
}


const char* ADVCOMP::DiffName(CARDINAL) const
{
   FatalError("ADVCOMP::DiffName: Illegal connection");
   return 0;
}


BOOLEAN ADVCOMP::AdvInExZ(CARDINAL) const
{
   return FALSE;
}


REAL ADVCOMP::AdvInZ(const REAL*, CARDINAL) const
{
   FatalError("ADVCOMP::AdvInZ: Illegal connection");
   return 0;
}


REAL ADVCOMP::AdvOutQ(VARSYS* varsys, const REAL* Y, CARDINAL conn)
{
   if ( conn > 0 ) FatalError("ADVCOMP::AdvOutQ: Illegal connection");
   if ( Active() == FALSE ) return 0.0;
   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   const REAL* y = &Y[ind_y];
   Qout = y[(NumGridPts()-1)*(ioff+numstatevar)];
   varsys->SetProgVar(Discharge,&Qout);
   if ( numCout != numvolstatevar )
   {
      delete [] Cout; Cout = new REAL[numvolstatevar];
      numCout = numvolstatevar;
   }
   CARDINAL i;
   varsys->Reset();
   if ( Xvar != 0 ) Xvar->SetProgVar(&gridpts[numgrid-1]);
   REAL a = A->Value();
   for ( i=0; i<numvolstatevar; i++ )
   {
      Cout[i] = y[(NumGridPts()-1)*(ioff+numstatevar)+ioff+i]/a;
      statevar[i]->SetStateVar(&Cout[i]);
   }
   for ( i=numvolstatevar; i<numstatevar; i++ )
   {
      statevar[i]->ResetStateVar();
   }
   return y[(NumGridPts()-1)*(ioff+numstatevar)];
}


REAL ADVCOMP::AdvOutJ(VARSYS* varsys,
                      const REAL* Y,
                      CARDINAL conn,
                      const AQVAR* var)
{
   if ( conn > 0 ) FatalError("ADVCOMP::AdvOutJ: Illegal connection");
   if ( Active() == FALSE ) return 0.0;
   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   const REAL* y = &Y[ind_y];
   CARDINAL i;
   Qout = y[(NumGridPts()-1)*(ioff+numstatevar)];
   varsys->SetProgVar(Discharge,&Qout);
   if ( numCout != numvolstatevar )
   {
      delete [] Cout; Cout = new REAL[numvolstatevar];
      numCout = numvolstatevar;
   }
   varsys->Reset();
   if ( Xvar != 0 ) Xvar->SetProgVar(&gridpts[numgrid-1]);
   REAL a = A->Value();
   varsys->Reset();
   if ( Xvar != 0 ) Xvar->SetProgVar(&gridpts[numgrid-2]);
   REAL aa = A->Value();
   for ( i=0; i<numvolstatevar; i++ )
   {
      Cout[i] = y[(NumGridPts()-1)*(ioff+numstatevar)+ioff+i]/a;
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
         REAL flux = y[(NumGridPts()-1)*(ioff+numstatevar)+ioff+i]
                      /a*y[(NumGridPts()-1)*(ioff+numstatevar)];
         if ( withdiffusion == TRUE )
         {
            flux -= a*y[numgrid*(ioff+numstatevar)+i]
                    * (  y[(numgrid-1)*(ioff+numstatevar)+ioff+i]/a
                       - y[(numgrid-2)*(ioff+numstatevar)+ioff+i]/aa)
                    / fabs(gridpts[numgrid-1]-gridpts[numgrid-2]);
         }
         return flux;
      }
   }
   return 0.0;
}


REAL ADVCOMP::DiffC(const REAL*, CARDINAL, const AQVAR*) const
{
   FatalError("ADVCOMP::DiffC: Illegal connection");
   return 0.0;
}


JOBSTATUS ADVCOMP::Load(std::istream& in,
                        const VARLIST* varlist, const PROCLIST* proclist)
{
   char Buffer[BufSize]; char Buffer1[BufSize];

   if ( LoadStart(in) == FALSE )                                return PROBLEM;
   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )                     return PROBLEM;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.1 converted from 1.0
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
            if ( LoadBoolean(in,&withdiffusion) == FALSE )      return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( GlobDiff(Buffer,varlist,Buffer1,n) == FALSE )
            {
               if ( withdiffusion==TRUE )                       return PROBLEM;
            }
         }
         break;
      case 2: // aquavers=2: individual diffusion coefficients added 19.06.95
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
            if ( LoadBoolean(in,&withdiffusion) == FALSE )      return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( GlobDiff(Buffer,varlist,Buffer1,n) == FALSE )
            {
               if ( withdiffusion==TRUE )                       return PROBLEM;
            }
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddIndDiff(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }
         }
         break;
      case 3: // aquavers=3: compartment index added 31.01.96
         {
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
            if ( LoadBoolean(in,&withdiffusion) == FALSE )      return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( GlobDiff(Buffer,varlist,Buffer1,n) == FALSE )
            {
               if ( withdiffusion==TRUE )                       return PROBLEM;
            }
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddIndDiff(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }
         }
         break;
      case 4: // aquavers=4: AQUASIM 2.0 gamma may 1997, accuracies introduced
         {
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
            if ( LoadBoolean(in,&withdiffusion) == FALSE )      return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( GlobDiff(Buffer,varlist,Buffer1,n) == FALSE )
            {
               if ( withdiffusion==TRUE )                       return PROBLEM;
            }
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddIndDiff(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }
         }
         break;
      case 5: // aquavers=5: May 1998 BOOLEAN active added
         {
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
            if ( LoadBoolean(in,&withdiffusion) == FALSE )      return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( GlobDiff(Buffer,varlist,Buffer1,n) == FALSE )
            {
               if ( withdiffusion==TRUE )                       return PROBLEM;
            }
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddIndDiff(varlist,var,Buffer,Buffer1,n) == FALSE )
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


JOBSTATUS ADVCOMP::Save(std::ostream& out)
{
   CARDINAL i;
   char     Buffer[BufSize];

   INTEGER aquavers = 5;

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
   if ( SaveBoolean(out,WithDiffusion()) == FALSE )            return PROBLEM;
   if ( globdiff == 0 )
   {
      if ( WithDiffusion() == TRUE )                           return PROBLEM;
      Buffer[0] = '\0';
   }
   else
   {
      if ( globdiff->UnParse(Buffer,sizeof(Buffer)-1) != 0 )   return PROBLEM;
   }
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   for ( i=0; i<numinddiff; i++ )
   {
      if ( SaveString(out,IndDiffVar(i)->Symbol()) == FALSE )  return PROBLEM;
      if ( IndDiffVal(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                   return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;

   if ( SaveEnd(out) == FALSE )                                return PROBLEM;
   return OK;
}


JOBSTATUS ADVCOMP::Write(std::ostream& out, BOOLEAN sh)
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

   if ( Print(out,ini.T(166),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( NumInitCond() > 0 )
   {
      PrintIndent(pos4-1);
      if ( Print(out,ini.T(167),pos3) == FALSE )               return PROBLEM;
      if ( Print(out,"(") == FALSE )                           return PROBLEM;
      if ( Print(out,ini.T(168)) == FALSE )                    return PROBLEM;
      if ( Print(out,") : ") == FALSE )                        return PROBLEM;
      if ( Print(out,ini.T(169)) == FALSE )                    return PROBLEM;
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
   if ( Print(out,ini.T(162),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Inflow() != 0 )
   {
      if ( inflow->UnParse(Buffer,sizeof(Buffer)-1) != 0 )     return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;
   }

   if ( Print(out,ini.T(163),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( NumInput() > 0 )
   {
      PrintIndent(pos4-1);
      if ( Print(out,ini.T(164),pos3) == FALSE )               return PROBLEM;
      if ( Print(out," : ") == FALSE )                         return PROBLEM;
      if ( Print(out,ini.T(165)) == FALSE )                    return PROBLEM;
      for ( i=0; i<NumInput(); i++ )
      {
         if ( Print(out,InputVar(i)->Symbol(),pos3) == FALSE ) return PROBLEM;
         if ( Print(out," : ") == FALSE )                      return PROBLEM;
         if ( InputFlux(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer) == FALSE )                     return PROBLEM;
      }
   }

   if ( Print(out,ini.T(99),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                             return PROBLEM;
   if ( LatInflow() != 0 )
   {
      if ( latinflow->UnParse(Buffer,sizeof(Buffer)-1) != 0 ) return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                  return PROBLEM;

      if ( NumLatInput() > 0 )
      {
         if ( Print(out,ini.T(100),pos1) == FALSE )           return PROBLEM;
         if ( Print(out,":") == FALSE )                       return PROBLEM;
         if ( Print(out,ini.T(101),pos3) == FALSE )           return PROBLEM;
         if ( Print(out,":") == FALSE )                       return PROBLEM;
         if ( Print(out,ini.T(102),pos2) == FALSE )           return PROBLEM;
         for ( CARDINAL i=0; i<NumLatInput(); i++ )
         {
            if ( Print(out,LatInputVar(i)->Symbol(),pos3) == FALSE )
                                                              return PROBLEM;
            if ( Print(out,":") == FALSE )                    return PROBLEM;
            if ( LatInputConc(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                              return PROBLEM;
            if ( Print(out,Buffer,pos2) == FALSE )            return PROBLEM;
         }
      }
   }

   if ( Print(out,ini.T(103),pos1) == FALSE )                 return PROBLEM;
   if ( Print(out,":") == FALSE )                             return PROBLEM;
   if ( Print(out,XStart(),pos2) == FALSE )                   return PROBLEM;

   if ( Print(out,ini.T(104),pos1) == FALSE )                 return PROBLEM;
   if ( Print(out,":") == FALSE )                             return PROBLEM;
   if ( Print(out,XEnd(),pos2) == FALSE )                     return PROBLEM;

   if ( Print(out,ini.T(105),pos1) == FALSE )                 return PROBLEM;
   if ( Print(out,":") == FALSE )                             return PROBLEM;
   if ( A->UnParse(Buffer,sizeof(Buffer)-1) != 0 )            return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                     return PROBLEM;

   if ( WithDiffusion() == TRUE )
   {
      if ( Print(out,ini.T(106),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                          return PROBLEM;
      if ( globdiff->UnParse(Buffer,sizeof(Buffer)-1) != 0 )  return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                  return PROBLEM;
      if ( NumIndDiff() > 0 )
      {
         if ( Print(out,ini.T(140),pos1) == FALSE )           return PROBLEM;
         if ( Print(out,":") == FALSE )                       return PROBLEM;
         if ( Print(out,ini.T(141),pos3) == FALSE )           return PROBLEM;
         if ( Print(out,":") == FALSE )                       return PROBLEM;
         if ( Print(out,ini.T(142),pos2) == FALSE )           return PROBLEM;
         for ( CARDINAL i=0; i<NumIndDiff(); i++ )
         {
            if ( Print(out,IndDiffVar(i)->Symbol(),pos3) == FALSE )
                                                              return PROBLEM;
            if ( Print(out,":") == FALSE )                    return PROBLEM;
            if ( IndDiffVal(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                              return PROBLEM;
            if ( Print(out,Buffer,pos2) == FALSE )            return PROBLEM;
         }
      }
   }

   if ( Print(out,ini.T(107),pos1) == FALSE )                 return PROBLEM;
   if ( Print(out,":") == FALSE )                             return PROBLEM;
   if ( Print(out,NumGridPts(),pos2) == FALSE )               return PROBLEM;
   if ( Print(out," (") == FALSE )                            return PROBLEM;
   if ( HighRes() == FALSE )
   {
      if ( Print(out,ini.T(108)) == FALSE )                   return PROBLEM;
   }
   else
   {
      if ( Print(out,ini.T(109)) == FALSE )                   return PROBLEM;
   }
   if ( Print(out,")") == FALSE )                             return PROBLEM;
   if ( Print(out,ini.T(230),pos1) == FALSE )                 return PROBLEM;
   if ( Print(out,":") == FALSE )                             return PROBLEM;
   if ( Print(out,ini.T(231),pos1+2) == FALSE )               return PROBLEM;
   if ( Print(out,": ") == FALSE )                            return PROBLEM;
   if ( Print(out,RelAccQ(),pos2) == FALSE )                  return PROBLEM;
   if ( Print(out,ini.T(232),pos1+2) == FALSE )               return PROBLEM;
   if ( Print(out,": ") == FALSE )                            return PROBLEM;
   if ( Print(out,AbsAccQ(),pos2) == FALSE )                  return PROBLEM;
   if ( Print(out,ini.T(233),pos1+2) == FALSE )               return PROBLEM;
   if ( Print(out,": ") == FALSE )                            return PROBLEM;
   if ( Print(out,RelAccD(),pos2) == FALSE )                  return PROBLEM;
   if ( Print(out,ini.T(234),pos1+2) == FALSE )               return PROBLEM;
   if ( Print(out,": ") == FALSE )                            return PROBLEM;
   if ( Print(out,AbsAccD(),pos2) == FALSE )                  return PROBLEM;

   return OK;
}

//////////////////////////////////////////////////////////////////////////////
