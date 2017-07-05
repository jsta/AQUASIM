///////////////////////////////   filmcomp.C   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    08.04.92    Peter Reichert
// revisions:   26.10.92    Peter Reichert    redesign of links
//              23.12.92    Peter Reichert    reorganization of numerics
//              03.05.96    Peter Reichert    particle diffusion added
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
// Interface Substratum-Film: ==================  x[0]                  A[0]
//                             layer 1   V[0]     x[1]
//                            ------------------           z[0]         A[1]
//                             layer 2   V[1]     x[2]
//                            ------------------           z[1]         A[2]
//                                                x[3]
// Film:                      ------------------           z[2]         A[3]
//
//
// 
//                            ------------------           z[n-4]       A[n-3]
//                             layer n-2 V[n-3]   x[n-2]
// Film-Interface Film-Bulk:  ==================  x[n-1] = z[n-3] = LF  A[n-2]
// Bulk-Interface Film-Bulk:  ------------------  x[n]
//
//
// Bulk:                         bulk    V[n-2]   x[n+1]
//
//
//                            ==================
//
// Notation:
//           without part diff:   with part diff:
// cell1:        nv+ne+1            nv+ne+1+nv+1
// cell3:        nv+ne+3            nv+ne+3+nv+1
// cell2:        nv+ne+1            nv+ne+1
// cellb:        nv+ns+ne+1         nv+ns+ne+1
//
// type component                            meaning
// ---------------------------------------------------------------------------
// Biofilm base boundary point:
// alg  y[0]                                  Cv[1] at x[0] matrix
//  .    .                                     .
// alg  y[nv-1]                               Cv[nv] at x[0] matrix
// alg  y[nv]                                 Ce[1] at x[0]
//  .    .                                     .
// alg  y[nv+ne-1]                            Ce[ne] at x[0]
// alg  y[nv+ne]                              epsl at x[0]
// if pore particles:                      
// alg  y[nv+ne+1]                            Cv[1] at x[0] pore
//  .    .                                     .
// alg  y[nv+ne+nv]                           Cv[nv] at x[0] pore
// alg  y[nv+ne+nv+1]                         porosity at x[0]
// ---------------------------------------------------------------------------
// Layer 1:
// diff y[cell1]                              V[0](*eps)*Cv[1] at x[1] matrix
//  .    .                                     .
// diff y[cell1+nv-1]                         V[0](*eps)*Cv[nv] at x[1] matrix
// alg  y[cell1+nv]                           Ce[1] at x[1]
//  .    .                                         .
// alg  y[cell1+nv+ne-1]                      Ce[ne] at x[1]
// alg  y[cell1+nv+ne]                        eps at x[1]
// diff y[cell1+nv+ne+1]                      z[0]
// alg  y[cell1+nv+ne+2]                      uF at z[0]
// if pore particles:                         
// alg  y[cell1+nv+ne+3]                      V[0]*Cv[1] at x[1] pore, or 0
//  .    .                                     .
// alg  y[cell1+nv+ne+nv+2]                   V[0]*Cv[nv] at x[1] pore, or 0
// alg  y[cell1+nv+ne+nv+3]                   porosity at x[1]
// ---------------------------------------------------------------------------
// Layer 2:
// diff y[cell1+cell3]                        V[1](*eps)*Cv[1] at x[2] matrix
//  .    .                                     .
// diff y[cell1+cell3+nv-1]                   V[1](*eps)*Cv[ns] at x[2] matrix
// alg  y[cell1+cell3+nv]                     Ce[1] at x[2]
//  .    .                                     .
// alg  y[cell1+cell3+nv+ne-1]                Ce[ne] at x[2]
// alg  y[cell1+cell3+nv+ne]                  epsFl at x[2]
// diff y[cell1+cell3+nv+ne+1]                z[1]
// alg  y[cell1+cell3+nv+ne+2]                uF at z[1]
// if pore particles:                         
// alg  y[cell1+cell3+nv+ne+3]                V[1]*Cv[1] at x[2] pore, or 0
//  .    .                                     .
// alg  y[cell1+cell3+nv+ne+nv+2]             V[1]*Cv[nv] at x[2] pore, or 0
// alg  y[cell1+cell3+nv+ne+nv+3]             porosity at x[2]
// ---------------------------------------------------------------------------
//  .
//  .
//  .
//
// ---------------------------------------------------------------------------
// Layer n-2:
// diff y[cell1+(n-3)*cell3]                  V[n-3](*eps)*Cv[1] at x[n-2] m.
//  .    .                                     .
// diff y[cell1+(n-3)*cell3+nv-1]             V[n-3](*eps)*Cv[nv] at x[n-2] m.
// alg  y[cell1+(n-3)*cell3+nv]               Ce[1] at x[n-2]
//  .    .                                     .
// alg  y[cell1+(n-3)*cell3+nv+ne-1]          Ce[ne] at x[n-2]
// alg  y[cell1+(n-3)*cell3+nv+ne]            eps at x[n-2]
// diff y[cell1+(n-3)*cell3+nv+ne+1]          z[n-3]
// alg  y[cell1+(n-3)*cell3+nv+ne+2]          uF at z[n-3]
// if pore particles:                         
// alg  y[cell1+(n-3)*cell3+nv+ne+3]          V[n-3]*Cv[1] at x[n-2] p., or 0
//  .    .                                     .
// alg  y[cell1+(n-3)*cell3+nv+ne+nv+2]       V[n-3]*Cv[nv] at x[n-2] p., or 0
// alg  y[cell1+(n-3)*cell3+nv+ne+nv+3]       porosity at x[n-2]
// ---------------------------------------------------------------------------
// Biofilm front boundary point:
// a/d  y[cell1+(n-2)*cell3]                  Cv[1] at x[n-1] matrix
//  .    .                                     .
// a/d  y[cell1+(n-2)*cell3+nv-1]             Cv[nv] at x[n-1] matrix
// alg  y[cell1+(n-2)*cell3+nv]               Ce[1] at x[n-1]
//  .    .                                     .
// alg  y[cell1+(n-2)*cell3+nv+ne-1]          Ce[ne] at x[n-1]
// alg  y[cell1+(n-2)*cell3+nv+ne]            eps at x[n-1]
// alg  y[cell1+(n-2)*cell3+nv+ne+1]          uat
// alg  y[cell1+(n-2)*cell3+nv+ne+2]          ude
// if pore particles:
// alg  y[cell1+(n-2)*cell3+nv+ne+3]          Cv[1] at x[n-1] pore
//  .    .                                     .
// alg  y[cell1+(n-2)*cell3+nv+ne+nv+2]       Cv[nv] at x[n-1] pore
// alg  y[cell1+(n-2)*cell3+nv+ne+nv+3]       porosity at x[n-1]
// ---------------------------------------------------------------------------
// Liquid layer boundary point:
// alg  y[cell1+(n-1)*cell3]                  Cv[1] at x[n]
//  .    .                                     .
// alg  y[cell1+(n-1)*cell3+nv-1]             Cv[nv] at x[n]
// alg  y[cell1+(n-1)*cell3+nv]               Ce[1] at x[n]
//  .    .                                     .
// alg  y[cell1+(n-1)*cell3+nv+ne-1]          Ce[ne] at x[n]
// alg  y[cell1+(n-1)*cell3+nv+ne]            eps at x[n]
// ---------------------------------------------------------------------------
// Bulk volume:
// diff y[cell1+(n-1)*cell3+cell2]            VB(*eps)*Cv[1] at x[n+1]
//  .    .                                     .
// diff y[cell1+(n-1)*cell3+cell2+nv-1]       VB(*eps)*Cv[nv] at x[n+1]
// diff y[cell1+(n-1)*cell3+cell2+nv]         Cs[1] at x[n+1]
//  .    .                                     .
// diff y[cell1+(n-1)*cell3+cell2+nv+ns-1]    Cs[ns] at x[n+1]
// alg  y[cell1+(n-1)*cell3+cell2+nv+ns]      Ce[1] at x[n+1]
//  .    .                                     .
// alg  y[cell1+(n-1)*cell3+cell2+nv+ns+ne-1] Ce[ne] at x[n+1]
// alg  y[cell1+(n-1)*cell3+cell2+nv+ns+ne]   eps at x[n+1]
// ---------------------------------------------------------------------------
// Additional variables:
// alg  y[cell1+(n-1)*cell3+cell2+cellb]      Qin
// alg  y[cell1+(n-1)*cell3+cell2+cellb+1]    Qout
// alg  y[cell1+(n-1)*cell3+cell2+cellb+2]    VB
// ---------------------------------------------------------------------------
//
//////////////////////////////////////////////////////////////////////////////

#include "formvar.h"
#include "othervar.h"
#include "varsys.h"
#include "fortran.h"
#include "filmcomp.h"
#include "aqlink.h"

//////////////////////////////////////////////////////////////////////////////

const CARDINAL BufSize = 1024;

//////////////////////////////////////////////////////////////////////////////

void FILMCOMP::init()
{
   numinit = 0; initzone = 0; initvar = 0; initval = 0;

   inflow = 0;
   numinput = 0; inputvar = 0; inputflux = 0;

   gridpts = 0; SetGridPts(10); SetGridPts(ini.I(2));

   confined     = TRUE;
   V            = ini.R(5);
   VB           = ini.R(6);
   nX           = 0;
   nS           = 0;
   Xvar         = 0;
   Svar         = 0;
   rho          = 0;
   surfkat      = 0;
   surfkde      = 0;
   volkat       = 0;
   volkde       = 0;
   resX         = 0;
   resS         = 0;
   DXpore       = 0;
   DXmatrix     = 0;
   DSpore       = 0;
   porepart     = FALSE;
   matrixdiff   = FALSE;
   detachglobal = TRUE;
   udet         = 0;
   A            = 0;
   reps         = 0;
   part         = 0;
   ind          = 0;
   highres      = FALSE;
   Cbulk        = 0;
   numCbulk     = 0;

   relaccQ   = 1.0e-3;
   absaccQ   = 1.0e-3;
   relaccV   = 1.0e-3;
   absaccV   = 1.0e-3;
   relaccZ   = 1.0e-3;
   absaccZ   = 1.0e-7;
   relaccEPS = 1.0e-3;
   absaccEPS = 1.0e-3;

   numadvinconn = 1;
   numadvoutconn = 1;
   numdiffconn = 2;
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


FILMCOMP::FILMCOMP()
{
   init();
}


FILMCOMP::FILMCOMP(const FILMCOMP& com) : AQCOMP(com)
{
   init();
   CARDINAL i;

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

   relaccQ   = com.relaccQ;
   absaccQ   = com.absaccQ;
   relaccV   = com.relaccV;
   absaccV   = com.absaccV;
   relaccZ   = com.relaccZ;
   absaccZ   = com.absaccZ;
   relaccEPS = com.relaccEPS;
   absaccEPS = com.absaccEPS;

   numgrid = com.numgrid;
   if ( gridpts != 0 ) delete [] gridpts;
   gridpts = new REAL[numgrid+2];
   for ( i=0; i<numgrid+2; i++ ) gridpts[i] = com.gridpts[i];
   confined = com.confined;
   V        = com.V;
   VB       = com.VB;
   nX       = com.nX;
   nS       = com.nS;
   if ( nX > 0 )
   {
      Xvar     = new AQVAR*[nX];
      rho      = new FORMVAR*[nX];
      surfkat  = new FORMVAR*[nX];
      surfkde  = new FORMVAR*[nX];
      resX     = new FORMVAR*[nX];
      DXpore   = new FORMVAR*[nX];
      DXmatrix = new FORMVAR*[nX];
      for ( i=0; i<nX; i++ )
      {
         Xvar[i]     = com.Xvar[i];
         rho[i]      = new FORMVAR(com.rho[i]);
         surfkat[i]  = new FORMVAR(com.surfkat[i]);
         surfkde[i]  = new FORMVAR(com.surfkde[i]);
         volkat[i]   = new FORMVAR(com.volkat[i]);
         volkde[i]   = new FORMVAR(com.volkde[i]);
         resX[i]     = new FORMVAR(com.resX[i]);
         DXpore[i]   = new FORMVAR(com.DXpore[i]);
         DXmatrix[i] = new FORMVAR(com.DXmatrix[i]);
      }
   }
   if ( nS > 0 )
   {
      Svar   = new AQVAR*[nS];
      resS   = new FORMVAR*[nS];
      DSpore = new FORMVAR*[nS];
      for ( CARDINAL i=0; i<nS; i++ )
      {
         Svar[i]   = com.Svar[i];
         resS[i]   = new FORMVAR(com.resS[i]);
         DSpore[i] = new FORMVAR(com.DSpore[i]);
      }
   }
   porepart     = com.porepart;
   matrixdiff   = com.matrixdiff;
   detachglobal = com.detachglobal;
   A            = new FORMVAR(com.A);
   udet         = new FORMVAR(com.udet);
   reps         = new FORMVAR(com.reps);
   highres      = com.highres;
   if ( numvolstatevar > 0 )
   {
      part = new BOOLEAN[numvolstatevar];
      ind  = new INTEGER[numvolstatevar];
      for ( i=0; i<numvolstatevar; i++ )
      {
         part[i] = com.part[i];
         ind[i]  = com.ind[i];
      }
   }
}


FILMCOMP::FILMCOMP(const AQCOMP* com) : AQCOMP(com)
{
   init();
   if ( com != 0 )
   {
      if ( com->Type() == FilmComp )
      {
         CARDINAL i;
         FILMCOMP* fc = (FILMCOMP*)com;

         if ( fc->numinit > 0 )
         {
            numinit = fc->numinit;
            initzone = new CARDINAL[numinit];
            initvar  = new AQVAR*[numinit];
            initval  = new FORMVAR*[numinit];
            for ( CARDINAL i=0; i<numinit; i++ )
            {
               initzone[i] = fc->initzone[i];
               initvar[i]  = fc->initvar[i];
               initval[i]  = new FORMVAR(fc->initval[i]);
            }
         }

         inflow = new FORMVAR(fc->inflow);
         if ( fc->numinput > 0 )
         {
            numinput = fc->numinput;
            inputvar = new AQVAR*[numinput];
            inputflux = new FORMVAR*[numinput];
            for ( CARDINAL i=0; i<numinput; i++ )
            {
               inputvar[i] = fc->inputvar[i];
               inputflux[i] = new FORMVAR(fc->inputflux[i]);
            }
         }

         relaccQ   = fc->relaccQ;
         absaccQ   = fc->absaccQ;
         relaccV   = fc->relaccV;
         absaccV   = fc->absaccV;
         relaccZ   = fc->relaccZ;
         absaccZ   = fc->absaccZ;
         relaccEPS = fc->relaccEPS;
         absaccEPS = fc->absaccEPS;

         numgrid = fc->numgrid;
         if ( gridpts != 0 ) delete [] gridpts;
         gridpts = new REAL[numgrid+2];
         for ( i=0; i<numgrid+2; i++ ) gridpts[i] = fc->gridpts[i];
         confined = fc->confined;
         V        = fc->V;
         VB       = fc->VB;
         nX       = fc->nX;
         nS       = fc->nS;
         if ( nX > 0 )
         {
            Xvar     = new AQVAR*[nX];
            rho      = new FORMVAR*[nX];
            surfkat  = new FORMVAR*[nX];
            surfkde  = new FORMVAR*[nX];
            volkat   = new FORMVAR*[nX];
            volkde   = new FORMVAR*[nX];
            resX     = new FORMVAR*[nX];
            DXpore   = new FORMVAR*[nX];
            DXmatrix = new FORMVAR*[nX];
            for ( i=0; i<nX; i++ )
            {
               Xvar[i]     = fc->Xvar[i];
               rho[i]      = new FORMVAR(fc->rho[i]);
               surfkat[i]  = new FORMVAR(fc->surfkat[i]);
               surfkde[i]  = new FORMVAR(fc->surfkde[i]);
               volkat[i]   = new FORMVAR(fc->volkat[i]);
               volkde[i]   = new FORMVAR(fc->volkde[i]);
               resX[i]     = new FORMVAR(fc->resX[i]);
               DXpore[i]   = new FORMVAR(fc->DXpore[i]);
               DXmatrix[i] = new FORMVAR(fc->DXmatrix[i]);
            }
         }
         if ( nS > 0 )
         {
            Svar   = new AQVAR*[nS];
            resS   = new FORMVAR*[nS];
            DSpore = new FORMVAR*[nS];
            for ( i=0; i<nS; i++ )
            {
               Svar[i]   = fc->Svar[i];
               resS[i]   = new FORMVAR(fc->resS[i]);
               DSpore[i] = new FORMVAR(fc->DSpore[i]);
            }
         }
         porepart     = fc->porepart;
         matrixdiff   = fc->matrixdiff;
         detachglobal = fc->detachglobal;
         A            = new FORMVAR(fc->A);
         udet         = new FORMVAR(fc->udet);
         reps         = new FORMVAR(fc->reps);
         highres      = fc->highres;
         if ( numvolstatevar > 0 )
         {
            part = new BOOLEAN[numvolstatevar];
            ind  = new INTEGER[numvolstatevar];
            for ( i=0; i<numvolstatevar; i++ )
            {
               part[i] = fc->part[i];
               ind[i]  = fc->ind[i];
            }
         }
      }
   }
}


FILMCOMP::~FILMCOMP()
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

   delete [] gridpts; gridpts = 0;

   for ( i=0; i<nX; i++ )
   {
      delete rho[i];
      delete surfkat[i];
      delete surfkde[i];
      delete volkat[i];
      delete volkde[i];
      delete resX[i];
      delete DXpore[i];
      delete DXmatrix[i];
   }
   delete [] Xvar;     Xvar = 0;
   delete [] rho;      rho  = 0;
   delete [] surfkat;  surfkat = 0;
   delete [] surfkde;  surfkde = 0;
   delete [] volkat;   volkat = 0;
   delete [] volkde;   volkde = 0;
   delete [] resX;     resX = 0;
   delete [] DXpore;   DXpore = 0;
   delete [] DXmatrix; DXmatrix = 0;
   for ( i=0; i<nS; i++ )
   {
      delete resS[i];
      delete DSpore[i];
   }
   delete [] Svar;   Svar = 0;
   delete [] resS;   resS = 0;
   delete [] DSpore; DSpore   = 0;
   delete udet; udet = 0;
   delete A;    A    = 0;
   delete reps; reps = 0;
   delete [] part; part = 0;
   delete [] ind;  ind  = 0;
   delete [] Cbulk;
}


const char* FILMCOMP::TypeName() const
{
   return ini.T(57);
}


AQVAR* FILMCOMP::SpaceVar(const VARSYS* varsys) const
{
   return varsys->GetProgVar(SpaceZ);
}


BOOLEAN FILMCOMP::AllowedSpaceVal(REAL z) const
{
   if ( z < 0.0 ) return FALSE;
   return TRUE;
}


BOOLEAN FILMCOMP::AllowedProgVarType(PROGVARTYPE type) const
{
   if ( type == CalcNumber )         return TRUE;
   if ( type == Time )               return TRUE;
   if ( type == CompIndex )          return TRUE;
   if ( type == ZoneIndex )          return TRUE;
   if ( type == Discharge )          return TRUE;
   if ( type == SpaceZ )             return TRUE;
   if ( type == FilmDepth )          return TRUE;
   if ( type == WaterFraction )      return TRUE;
   if ( type == FilmVelocity )       return TRUE;
   if ( type == InterfaceVelocity )  return TRUE;
   if ( type == DetachmentVelocity ) return TRUE;
   if ( type == AttachmentVelocity ) return TRUE;
   if ( type == Volume )             return TRUE;
   if ( type == BulkVolume )         return TRUE;
   return FALSE;
}


BOOLEAN FILMCOMP::AllowedComp() const
{
   if ( nX > 0 )
   {
      if ( rho == 0 )                                         return FALSE;
      if ( surfkat == 0 )                                     return FALSE;
      if ( surfkde == 0 )                                     return FALSE;
      if ( volkat == 0 )                                      return FALSE;
      if ( volkde == 0 )                                      return FALSE;
      if ( resX == 0 )                                        return FALSE;
      if ( DXpore == 0 )                                      return FALSE;
      if ( DXmatrix == 0 )                                    return FALSE;
      for ( CARDINAL i=0; i<nX; i++ )
      {
         if ( rho[i] == 0 )                                   return FALSE;
         if ( rho[i]->ArgVarType(StateVar) == TRUE )          return FALSE;
         if ( rho[i]->ArgProgVarType(Time) == TRUE )          return FALSE;
         if ( rho[i]->ArgProgVarType(Discharge) == TRUE )     return FALSE;
         if ( rho[i]->ArgProgVarType(SpaceZ) == TRUE )        return FALSE;
         if ( rho[i]->ArgProgVarType(FilmDepth) == TRUE )     return FALSE;
         if ( rho[i]->ArgProgVarType(WaterFraction) == TRUE ) return FALSE;
         if ( rho[i]->ArgProgVarType(FilmVelocity) == TRUE )  return FALSE;
         if ( rho[i]->ArgProgVarType(InterfaceVelocity) == TRUE )
                                                              return FALSE;
         if ( rho[i]->ArgProgVarType(DetachmentVelocity) == TRUE )
                                                              return FALSE;
         if ( rho[i]->ArgProgVarType(AttachmentVelocity) == TRUE )
                                                              return FALSE;
         if ( rho[i]->ArgProgVarType(Volume) == TRUE )        return FALSE;
         if ( rho[i]->ArgProgVarType(BulkVolume) == TRUE )    return FALSE;
         if ( surfkat[i] == 0 )                               return FALSE;
         if ( surfkde[i] == 0 )                               return FALSE;
         if ( volkat[i] == 0 )                                return FALSE;
         if ( volkde[i] == 0 )                                return FALSE;
         if ( resX[i] == 0 )                                  return FALSE;
         if ( DXpore[i] == 0 )                                return FALSE;
         if ( DXmatrix[i] == 0 )                              return FALSE;
      }
   }
   if ( nS > 0 )
   {
      if ( resS == 0 )                                        return FALSE;
      if ( DSpore == 0 )                                      return FALSE;
      for ( CARDINAL i=0; i<nS; i++ )
      {
         if ( resS[i] == 0 )                                  return FALSE;
         if ( DSpore[i] == 0 )                                return FALSE;
      }
   }
   if ( A == 0 )                                              return FALSE;
   if ( A->ArgVarType(StateVar) == TRUE )                     return FALSE;
   if ( A->ArgProgVarType(Time) == TRUE )                     return FALSE;
   if ( A->ArgProgVarType(Discharge) == TRUE )                return FALSE;
   if ( A->ArgProgVarType(FilmDepth) == TRUE )                return FALSE;
   if ( A->ArgProgVarType(WaterFraction) == TRUE )            return FALSE;
   if ( A->ArgProgVarType(FilmVelocity) == TRUE )             return FALSE;
   if ( A->ArgProgVarType(InterfaceVelocity) == TRUE )        return FALSE;
   if ( A->ArgProgVarType(DetachmentVelocity) == TRUE )       return FALSE;
   if ( A->ArgProgVarType(AttachmentVelocity) == TRUE )       return FALSE;
   if ( A->ArgProgVarType(Volume) == TRUE )                   return FALSE;
   if ( A->ArgProgVarType(BulkVolume) == TRUE )               return FALSE;
   if ( reps == 0 )                                           return FALSE;
   if ( detachglobal==TRUE && udet==0 )                       return FALSE;
   return TRUE;
}


BOOLEAN FILMCOMP::AllowedReplaceVar(const AQVAR* oldvar,
                                    const AQVAR* newvar) const
{
   if ( AQCOMP::AllowedReplaceVar(oldvar,newvar) == FALSE )      return FALSE;

   if ( A->Arg(oldvar) == TRUE )
   {
      if ( newvar->ArgVarType(StateVar) == TRUE )                return FALSE;
      if ( newvar->ArgProgVarType(Time) == TRUE )                return FALSE;
      if ( newvar->ArgProgVarType(Discharge) == TRUE )           return FALSE;
      if ( newvar->ArgProgVarType(FilmDepth) == TRUE )           return FALSE;
      if ( newvar->ArgProgVarType(WaterFraction) == TRUE )       return FALSE;
      if ( newvar->ArgProgVarType(FilmVelocity) == TRUE )        return FALSE;
      if ( newvar->ArgProgVarType(InterfaceVelocity) == TRUE )   return FALSE;
      if ( newvar->ArgProgVarType(DetachmentVelocity) == TRUE )  return FALSE;
      if ( newvar->ArgProgVarType(AttachmentVelocity) == TRUE )  return FALSE;
      if ( newvar->ArgProgVarType(Volume) == TRUE )              return FALSE;
      if ( newvar->ArgProgVarType(BulkVolume) == TRUE )          return FALSE;
   }

   if ( nX > 0 )
   {
      for ( CARDINAL i=0; i<nX; i++ )
      {
         if ( rho[i]->Arg(oldvar) == TRUE )
         {
             if ( newvar->ArgVarType(StateVar) == TRUE )         return FALSE;
             if ( newvar->ArgProgVarType(Time) == TRUE )         return FALSE;
             if ( newvar->ArgProgVarType(Discharge) == TRUE )    return FALSE;
             if ( newvar->ArgProgVarType(SpaceZ) == TRUE )       return FALSE;
             if ( newvar->ArgProgVarType(FilmDepth) == TRUE )    return FALSE;
             if ( newvar->ArgProgVarType(WaterFraction) == TRUE )
                                                                 return FALSE;
             if ( newvar->ArgProgVarType(FilmVelocity) == TRUE ) return FALSE;
             if ( newvar->ArgProgVarType(InterfaceVelocity) == TRUE )
                                                                 return FALSE;
             if ( newvar->ArgProgVarType(DetachmentVelocity) == TRUE )
                                                                 return FALSE;
             if ( newvar->ArgProgVarType(AttachmentVelocity) == TRUE )
                                                                 return FALSE;
             if ( newvar->ArgProgVarType(Volume) == TRUE )       return FALSE;
             if ( newvar->ArgProgVarType(BulkVolume) == TRUE )   return FALSE;
         }
      }
   }

   return TRUE;
}


BOOLEAN FILMCOMP::AllowedExchangeVar(const AQVAR* var1,
                                     const AQVAR* var2) const
{
   if ( AQCOMP::AllowedExchangeVar(var1,var2) == FALSE )      return FALSE;

   if ( (A->Arg(var1)==FALSE) && (A->Arg(var2)==TRUE) )
   {
      if ( var1->ArgVarType(StateVar) == TRUE )               return FALSE;
      if ( var1->ArgProgVarType(Time) == TRUE )               return FALSE;
      if ( var1->ArgProgVarType(Discharge) == TRUE )          return FALSE;
      if ( var1->ArgProgVarType(FilmDepth) == TRUE )          return FALSE;
      if ( var1->ArgProgVarType(WaterFraction) == TRUE )      return FALSE;
      if ( var1->ArgProgVarType(FilmVelocity) == TRUE )       return FALSE;
      if ( var1->ArgProgVarType(InterfaceVelocity) == TRUE )  return FALSE;
      if ( var1->ArgProgVarType(DetachmentVelocity) == TRUE ) return FALSE;
      if ( var1->ArgProgVarType(AttachmentVelocity) == TRUE ) return FALSE;
      if ( var1->ArgProgVarType(Volume) == TRUE )             return FALSE;
      if ( var1->ArgProgVarType(BulkVolume) == TRUE )         return FALSE;
   }
   if ( (A->Arg(var1)==TRUE) && (A->Arg(var2)==FALSE) )
   {
      if ( var2->ArgVarType(StateVar) == TRUE )               return FALSE;
      if ( var2->ArgProgVarType(Time) == TRUE )               return FALSE;
      if ( var2->ArgProgVarType(Discharge) == TRUE )          return FALSE;
      if ( var2->ArgProgVarType(FilmDepth) == TRUE )          return FALSE;
      if ( var2->ArgProgVarType(WaterFraction) == TRUE )      return FALSE;
      if ( var2->ArgProgVarType(FilmVelocity) == TRUE )       return FALSE;
      if ( var2->ArgProgVarType(InterfaceVelocity) == TRUE )  return FALSE;
      if ( var2->ArgProgVarType(DetachmentVelocity) == TRUE ) return FALSE;
      if ( var2->ArgProgVarType(AttachmentVelocity) == TRUE ) return FALSE;
      if ( var2->ArgProgVarType(Volume) == TRUE )             return FALSE;
      if ( var2->ArgProgVarType(BulkVolume) == TRUE )         return FALSE;
   }

   if ( nX > 0 )
   {
      for ( CARDINAL i=0; i<nX; i++ )
      {
         if ( (rho[i]->Arg(var1)==FALSE) && (rho[i]->Arg(var2)==TRUE) )
         {
            if ( var1->ArgVarType(StateVar) == TRUE )         return FALSE;
            if ( var1->ArgProgVarType(Time) == TRUE )         return FALSE;
            if ( var1->ArgProgVarType(Discharge) == TRUE )    return FALSE;
            if ( var1->ArgProgVarType(SpaceZ) == TRUE )       return FALSE;
            if ( var1->ArgProgVarType(FilmDepth) == TRUE )    return FALSE;
            if ( var1->ArgProgVarType(WaterFraction) == TRUE )
                                                              return FALSE;
            if ( var1->ArgProgVarType(FilmVelocity) == TRUE ) return FALSE;
            if ( var1->ArgProgVarType(InterfaceVelocity) == TRUE )
                                                              return FALSE;
            if ( var1->ArgProgVarType(DetachmentVelocity) == TRUE )
                                                              return FALSE;
            if ( var1->ArgProgVarType(AttachmentVelocity) == TRUE )
                                                              return FALSE;
            if ( var1->ArgProgVarType(Volume) == TRUE )       return FALSE;
            if ( var1->ArgProgVarType(BulkVolume) == TRUE )   return FALSE;
         }
         if ( (rho[i]->Arg(var1)==TRUE) && (rho[i]->Arg(var2)==FALSE) )
         {
            if ( var2->ArgVarType(StateVar) == TRUE )         return FALSE;
            if ( var2->ArgProgVarType(Time) == TRUE )         return FALSE;
            if ( var2->ArgProgVarType(Discharge) == TRUE )    return FALSE;
            if ( var2->ArgProgVarType(SpaceZ) == TRUE )       return FALSE;
            if ( var2->ArgProgVarType(FilmDepth) == TRUE )    return FALSE;
            if ( var2->ArgProgVarType(WaterFraction) == TRUE )
                                                              return FALSE;
            if ( var2->ArgProgVarType(FilmVelocity) == TRUE ) return FALSE;
            if ( var2->ArgProgVarType(InterfaceVelocity) == TRUE )
                                                              return FALSE;
            if ( var2->ArgProgVarType(DetachmentVelocity) == TRUE )
                                                              return FALSE;
            if ( var2->ArgProgVarType(AttachmentVelocity) == TRUE )
                                                              return FALSE;
            if ( var2->ArgProgVarType(Volume) == TRUE )       return FALSE;
            if ( var2->ArgProgVarType(BulkVolume) == TRUE )   return FALSE;
         }
      }
   }

   return TRUE;
}


void FILMCOMP::ReplVar(AQVAR* oldvar, AQVAR* newvar)
{
   AQCOMP::ReplVar(oldvar,newvar);
   CARDINAL i;
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
   for ( i=0; i<nX; i++ )
   {
      if ( Xvar[i] == oldvar ) Xvar[i] = newvar;
      rho[i]->ReplaceVarArg(oldvar,newvar);
      surfkat[i]->ReplaceVarArg(oldvar,newvar);
      surfkde[i]->ReplaceVarArg(oldvar,newvar);
      volkat[i]->ReplaceVarArg(oldvar,newvar);
      volkde[i]->ReplaceVarArg(oldvar,newvar);
      resX[i]->ReplaceVarArg(oldvar,newvar);
      DXpore[i]->ReplaceVarArg(oldvar,newvar);
      DXmatrix[i]->ReplaceVarArg(oldvar,newvar);
   }
   for ( i=0; i<nS; i++ )
   {
      if ( Svar[i] == oldvar ) Svar[i] = newvar;
      resS[i]->ReplaceVarArg(oldvar,newvar);
      DSpore[i]->ReplaceVarArg(oldvar,newvar);
   }
   if ( udet != 0 ) udet->ReplaceVarArg(oldvar,newvar);
   if ( A != 0 )    A->ReplaceVarArg(oldvar,newvar);
   if ( reps != 0 ) reps->ReplaceVarArg(oldvar,newvar);
   CalcArg();
   return;
}


void FILMCOMP::ExchVar(AQVAR* var1, AQVAR* var2)
{
   AQCOMP::ExchVar(var1,var2);
   AQVAR* v = new CONSTVAR;
   CARDINAL i;
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
   for ( i=0; i<nX; i++ )
   {
      if ( Xvar[i] == var1 )
      {
         Xvar[i] = var2;
      }
      else
      {
         if ( Xvar[i] == var2 ) Xvar[i] = var1;
      }
      rho[i]->ReplaceVarArg(var1,v);
      rho[i]->ReplaceVarArg(var2,var1);
      rho[i]->ReplaceVarArg(v,var2);
      surfkat[i]->ReplaceVarArg(var1,v);
      surfkat[i]->ReplaceVarArg(var2,var1);
      surfkat[i]->ReplaceVarArg(v,var2);
      surfkde[i]->ReplaceVarArg(var1,v);
      surfkde[i]->ReplaceVarArg(var2,var1);
      surfkde[i]->ReplaceVarArg(v,var2);
      volkat[i]->ReplaceVarArg(var1,v);
      volkat[i]->ReplaceVarArg(var2,var1);
      volkat[i]->ReplaceVarArg(v,var2);
      volkde[i]->ReplaceVarArg(var1,v);
      volkde[i]->ReplaceVarArg(var2,var1);
      volkde[i]->ReplaceVarArg(v,var2);
      resX[i]->ReplaceVarArg(var1,v);
      resX[i]->ReplaceVarArg(var2,var1);
      resX[i]->ReplaceVarArg(v,var2);
      DXpore[i]->ReplaceVarArg(var1,v);
      DXpore[i]->ReplaceVarArg(var2,var1);
      DXpore[i]->ReplaceVarArg(v,var2);
      DXmatrix[i]->ReplaceVarArg(var1,v);
      DXmatrix[i]->ReplaceVarArg(var2,var1);
      DXmatrix[i]->ReplaceVarArg(v,var2);
   }
   for ( i=0; i<nS; i++ )
   {
      if ( Svar[i] == var1 )
      {
         Svar[i] = var2;
      }
      else
      {
         if ( Svar[i] == var2 ) Svar[i] = var1;
      }
      resS[i]->ReplaceVarArg(var1,v);
      resS[i]->ReplaceVarArg(var2,var1);
      resS[i]->ReplaceVarArg(v,var2);
      DSpore[i]->ReplaceVarArg(var1,v);
      DSpore[i]->ReplaceVarArg(var2,var1);
      DSpore[i]->ReplaceVarArg(v,var2);
   }
   if ( udet != 0 )
   {
      udet->ReplaceVarArg(var1,v);
      udet->ReplaceVarArg(var2,var1);
      udet->ReplaceVarArg(v,var2);
   }
   if ( A != 0 )
   {
      A->ReplaceVarArg(var1,v);
      A->ReplaceVarArg(var2,var1);
      A->ReplaceVarArg(v,var2);
   }
   if ( reps != 0 )
   {
      reps->ReplaceVarArg(var1,v);
      reps->ReplaceVarArg(var2,var1);
      reps->ReplaceVarArg(v,var2);
   }
   delete v;
   CalcArg();
   return;
}


BOOLEAN FILMCOMP::AddX(AQVAR* var)
{
   if ( InList() == TRUE )  return FALSE;
   if ( var == 0 )          return FALSE;
   CARDINAL i;
   for ( i=0; i<nX; i++ )
   {
      if ( Xvar[i] == var ) return FALSE;
   }
   for ( i=0; i<nS; i++ )
   {
      if ( Svar[i] == var ) return FALSE;
   }
   char Buf[BufSize];
   char Buf1[BufSize];

   // rho:
   NumToString(ini.R(7),Buf,BufSize);
   FORMVAR* newrhoi = new FORMVAR;
   newrhoi->Symbol("rho");
   if ( newrhoi->Parse(Buf,0,Buf1) != 0 )
   {
      if ( newrhoi->Parse("1000",0,Buf1) != 0 )
         FatalError("FILMCOMP::AddX: Unable to parse rho");
   }

   // surfkat:
   NumToString(ini.R(8),Buf,BufSize);
   FORMVAR* newsurfkati = new FORMVAR;
   newsurfkati->Symbol("surfkat");
   if ( newsurfkati->Parse(Buf,0,Buf1) != 0 )
   {
      if ( newsurfkati->Parse("0",0,Buf1) != 0 )
         FatalError("FILMCOMP::AddX: Unable to parse surfkat");
   }

   // surfkde:
   NumToString(ini.R(9),Buf,BufSize);
   FORMVAR* newsurfkdei = new FORMVAR;
   newsurfkdei->Symbol("surfkde");
   if ( newsurfkdei->Parse(Buf,0,Buf1) != 0 )
   {
      if ( newsurfkdei->Parse("0",0,Buf1) != 0 )
         FatalError("FILMCOMP::AddX: Unable to parse surfkde");
   }

   // volkat:
   NumToString(ini.R(8),Buf,BufSize);
   FORMVAR* newvolkati = new FORMVAR;
   newvolkati->Symbol("volkat");
   if ( newvolkati->Parse(Buf,0,Buf1) != 0 )
   {
      if ( newvolkati->Parse("0",0,Buf1) != 0 )
         FatalError("FILMCOMP::AddX: Unable to parse volkat");
   }

   // volkde:
   NumToString(ini.R(9),Buf,BufSize);
   FORMVAR* newvolkdei = new FORMVAR;
   newvolkdei->Symbol("volkde");
   if ( newvolkdei->Parse(Buf,0,Buf1) != 0 )
   {
      if ( newvolkdei->Parse("0",0,Buf1) != 0 )
         FatalError("FILMCOMP::AddX: Unable to parse volkde");
   }

   // resX:
   NumToString(ini.R(10),Buf,BufSize);
   FORMVAR* newresXi = new FORMVAR;
   newresXi->Symbol("resX");
   if ( newresXi->Parse(Buf,0,Buf1) != 0 )
   {
      if ( newresXi->Parse("0",0,Buf1) != 0 )
         FatalError("FILMCOMP::AddX: Unable to parse resX");
   }

   // DXpore:
   NumToString(ini.R(12),Buf,BufSize);
   FORMVAR* newDXporei = new FORMVAR;
   newDXporei->Symbol("DXpore");
   if ( newDXporei->Parse(Buf,0,Buf1) != 0 )
   {
      if ( newDXporei->Parse("0",0,Buf1) != 0 )
         FatalError("FILMCOMP::AddX: Unable to parse DXpore");
   }

   // DXmatrix:
   NumToString(ini.R(12),Buf,BufSize);
   FORMVAR* newDXmatrixi = new FORMVAR;
   newDXmatrixi->Symbol("DXmatrix");
   if ( newDXmatrixi->Parse(Buf,0,Buf1) != 0 )
   {
      if ( newDXmatrixi->Parse("0",0,Buf1) != 0 )
         FatalError("FILMCOMP::AddX: Unable to parse DXmatrix");
   }

   // allocate new array:
   AQVAR**   newXvar     = new AQVAR*[nX+1];
   FORMVAR** newrho      = new FORMVAR*[nX+1];
   FORMVAR** newsurfkat  = new FORMVAR*[nX+1];
   FORMVAR** newsurfkde  = new FORMVAR*[nX+1];
   FORMVAR** newvolkat   = new FORMVAR*[nX+1];
   FORMVAR** newvolkde   = new FORMVAR*[nX+1];
   FORMVAR** newresX     = new FORMVAR*[nX+1];
   FORMVAR** newDXpore   = new FORMVAR*[nX+1];
   FORMVAR** newDXmatrix = new FORMVAR*[nX+1];
   CARDINAL j;
   for ( j=0; j<nX; j++ )
   {
      newXvar[j]     = Xvar[j];
      newrho[j]      = rho[j];
      newsurfkat[j]  = surfkat[j];
      newsurfkde[j]  = surfkde[j];
      newvolkat[j]   = volkat[j];
      newvolkde[j]   = volkde[j];
      newresX[j]     = resX[j];
      newDXpore[j]   = DXpore[j];
      newDXmatrix[j] = DXmatrix[j];
   }
   newXvar[nX]     = var;
   newrho[nX]      = newrhoi;
   newsurfkat[nX]  = newsurfkati;
   newsurfkde[nX]  = newsurfkdei;
   newvolkat[nX]   = newvolkati;
   newvolkde[nX]   = newvolkdei;
   newresX[nX]     = newresXi;
   newDXpore[nX]   = newDXporei;
   newDXmatrix[nX] = newDXmatrixi;
   delete Xvar;
   delete rho;
   delete surfkat;
   delete surfkde;
   delete volkat;
   delete volkde;
   delete resX;
   delete DXpore;
   delete DXmatrix;
   Xvar     = newXvar;
   rho      = newrho;
   surfkat  = newsurfkat;
   surfkde  = newsurfkde;
   volkat   = newvolkat;
   volkde   = newvolkde;
   resX     = newresX;
   DXpore   = newDXpore;
   DXmatrix = newDXmatrix;
   nX++;
   CalcArg();
   return TRUE;
}


BOOLEAN FILMCOMP::AddS(AQVAR* var)
{
   if ( InList() == TRUE )  return FALSE;
   if ( var == 0 )          return FALSE;
   CARDINAL i;
   for ( i=0; i<nX; i++ )
   {
      if ( Xvar[i] == var ) return FALSE;
   }
   for ( i=0; i<nS; i++ )
   {
      if ( Svar[i] == var ) return FALSE;
   }
   char Buf[BufSize];
   char Buf1[BufSize];

   // resS:
   NumToString(ini.R(11),Buf,BufSize);
   FORMVAR* newresSi = new FORMVAR;
   newresSi->Symbol("resS");
   if ( newresSi->Parse(Buf,0,Buf1) != 0 )
   {
      if ( newresSi->Parse("0",0,Buf1) != 0 )
         FatalError("FILMCOMP::AddS: Unable to parse resS");
   }

   // DSpore:
   NumToString(ini.R(13),Buf,BufSize);
   FORMVAR* newDSporei = new FORMVAR;
   newDSporei->Symbol("DSpore");
   if ( newDSporei->Parse(Buf,0,Buf1) != 0 )
   {
      if ( newDSporei->Parse("0",0,Buf1) != 0 )
         FatalError("FILMCOMP::AddS: Unable to parse DSpore");
   }

   // allocate new array:
   AQVAR**   newSvar   = new AQVAR*[nS+1];
   FORMVAR** newresS   = new FORMVAR*[nS+1];
   FORMVAR** newDSpore = new FORMVAR*[nS+1];
   CARDINAL j;
   for ( j=0; j<nS; j++ )
   {
      newSvar[j]   = Svar[j];
      newresS[j]   = resS[j];
      newDSpore[j] = DSpore[j];
   }
   newSvar[nS]   = var;
   newresS[nS]   = newresSi;
   newDSpore[nS] = newDSporei;
   delete Svar;
   delete resS;
   delete DSpore;
   Svar = newSvar;
   resS = newresS;
   DSpore = newDSpore;
   nS++;
   CalcArg();
   return TRUE;
}


BOOLEAN FILMCOMP::DelX(CARDINAL i)
{
   if ( i >= nX ) return FALSE;
   AQVAR**   newXvar     = 0;
   FORMVAR** newrho      = 0;
   FORMVAR** newsurfkat  = 0;
   FORMVAR** newsurfkde  = 0;
   FORMVAR** newvolkat   = 0;
   FORMVAR** newvolkde   = 0;
   FORMVAR** newresX     = 0;
   FORMVAR** newDXpore   = 0;
   FORMVAR** newDXmatrix = 0;
   if ( nX > 1 )
   {
      newXvar     = new AQVAR*[nX-1];
      newrho      = new FORMVAR*[nX-1];
      newsurfkat  = new FORMVAR*[nX-1];
      newsurfkde  = new FORMVAR*[nX-1];
      newvolkat   = new FORMVAR*[nX-1];
      newvolkde   = new FORMVAR*[nX-1];
      newresX     = new FORMVAR*[nX-1];
      newDXpore   = new FORMVAR*[nX-1];
      newDXmatrix = new FORMVAR*[nX-1];
      CARDINAL j;
      for ( j=0; j<i; j++ )
      {
         newXvar[j]     = Xvar[j];
         newrho[j]      = rho[j];
         newsurfkat[j]  = surfkat[j];
         newsurfkde[j]  = surfkde[j];
         newvolkat[j]   = volkat[j];
         newvolkde[j]   = volkde[j];
         newresX[j]     = resX[j];
         newDXpore[j]   = DXpore[j];
         newDXmatrix[j] = DXmatrix[j];
      }
      for ( j=i+1; j<nX; j++ )
      {
         newXvar[j-1]     = Xvar[j];
         newrho[j-1]      = rho[j];
         newsurfkat[j-1]  = surfkat[j];
         newsurfkde[j-1]  = surfkde[j];
         newvolkat[j-1]   = volkat[j];
         newvolkde[j-1]   = volkde[j];
         newresX[j-1]     = resX[j];
         newDXpore[j-1]   = DXpore[j];
         newDXmatrix[j-1] = DXmatrix[j];
      }
   }
   delete rho[i];
   delete surfkat[i];
   delete surfkde[i];
   delete volkat[i];
   delete volkde[i];
   delete resX[i];
   delete DXpore[i];
   delete DXmatrix[i];
   delete Xvar;
   delete rho;
   delete surfkat;
   delete surfkde;
   delete volkat;
   delete volkde;
   delete resX;
   delete DXpore;
   delete DXmatrix;
   Xvar = newXvar;
   rho  = newrho;
   surfkat  = newsurfkat;
   surfkde  = newsurfkde;
   volkat   = newvolkat;
   volkde   = newvolkde;
   resX     = newresX;
   DXpore   = newDXpore;
   DXmatrix = newDXmatrix;
   nX--;
   CalcArg();
   return TRUE;
}


BOOLEAN FILMCOMP::DelS(CARDINAL i)
{
   if ( i >= nS ) return FALSE;
   AQVAR**   newSvar   = 0;
   FORMVAR** newresS   = 0;
   FORMVAR** newDSpore = 0;
   if ( nS > 1 )
   {
      newSvar   = new AQVAR*[nS-1];
      newresS   = new FORMVAR*[nS-1];
      newDSpore = new FORMVAR*[nS-1];
      CARDINAL j;
      for ( j=0; j<i; j++ )
      {
         newSvar[j]   = Svar[j];
         newresS[j]   = resS[j];
         newDSpore[j] = DSpore[j];
      }
      for ( j=i+1; j<nS; j++ )
      {
         newSvar[j-1]   = Svar[j];
         newresS[j-1]   = resS[j];
         newDSpore[j-1] = DSpore[j];
      }
   }
   delete resS[i];
   delete DSpore[i];
   delete Svar;
   delete resS;
   delete DSpore;
   Svar   = newSvar;
   resS   = newresS;
   DSpore = newDSpore;
   nS--;
   CalcArg();
   return TRUE;
}


BOOLEAN FILMCOMP::DelX(AQVAR* var)
{
   if ( var == 0 ) return FALSE;
   for ( CARDINAL i=0; i<nX; i++ )
   {
      if ( Xvar[i] == var ) return DelX(i);
   }
   return FALSE;
}


BOOLEAN FILMCOMP::DelS(AQVAR* var)
{
   if ( var == 0 ) return FALSE;
   for ( CARDINAL i=0; i<nS; i++ )
   {
      if ( Svar[i] == var ) return DelS(i);
   }
   return FALSE;
}


AQVAR* FILMCOMP::Part(CARDINAL index) const
{
   if ( index >= nX )
      FatalError("FILMCPMP::Part: Illegal index");
   return Xvar[index];
}


AQVAR* FILMCOMP::Diss(CARDINAL index) const
{
   if ( index >= nS )
      FatalError("FILMCPMP::Diss: Illegal index");
   return Svar[index];
}


const FORMVAR* FILMCOMP::Rho(CARDINAL index) const
{
   if ( index >= nX )
      FatalError("FILMCPMP::Rho: Illegal index");
   return rho[index];
}


BOOLEAN FILMCOMP::Rho(AQVAR* var,
                      const char* inpline,
                      const VARLIST* varlist,
                      char* errorline,
                      CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( var == 0 )           return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   for ( CARDINAL i=0; i<nX; i++ )
   {
      if ( Xvar[i] == var )
      {
         FORMVAR* formvar = new FORMVAR;
         formvar->Symbol("rho");
         numparseerrors = formvar->Parse(inpline,varlist,errorline);
         if ( numparseerrors != 0 )
         {
            delete formvar; return FALSE;
         }
         if ( AllowedVar(formvar) == FALSE )
         {
            delete formvar; return FALSE;
         }

         // no dependence on a State Variable and on a Program Variable
         // with the exception of CalcNum

         if ( formvar->ArgVarType(StateVar) == TRUE )
         {
            delete formvar; return FALSE;
         }
         if ( formvar->ArgProgVarType(Time) == TRUE )
         {
            delete formvar; return FALSE;
         }
         if ( formvar->ArgProgVarType(Discharge) == TRUE )
         {
            delete formvar; return FALSE;
         }
         if ( formvar->ArgProgVarType(SpaceZ) == TRUE )
         {
            delete formvar; return FALSE;
         }
         if ( formvar->ArgProgVarType(FilmDepth) == TRUE )
         {
            delete formvar; return FALSE;
         }
         if ( formvar->ArgProgVarType(WaterFraction) == TRUE )
         {
            delete formvar; return FALSE;
         }
         if ( formvar->ArgProgVarType(FilmVelocity) == TRUE )
         {
            delete formvar; return FALSE;
         }
         if ( formvar->ArgProgVarType(InterfaceVelocity) == TRUE )
         {
            delete formvar; return FALSE;
         }
         if ( formvar->ArgProgVarType(DetachmentVelocity) == TRUE )
         {
            delete formvar; return FALSE;
         }
         if ( formvar->ArgProgVarType(AttachmentVelocity) == TRUE )
         {
            delete formvar; return FALSE;
         }
         if ( formvar->ArgProgVarType(Volume) == TRUE )
         {
            delete formvar; return FALSE;
         }
         if ( formvar->ArgProgVarType(BulkVolume) == TRUE )
         {
            delete formvar; return FALSE;
         }

         delete rho[i]; rho[i] = formvar;
         CalcArg();
         return TRUE;
      }
   }
   return FALSE;
}


const FORMVAR* FILMCOMP::SurfKat(CARDINAL index) const
{
   if ( index >= nX )
      FatalError("FILMCPMP::SurfKat: Illegal index");
   return surfkat[index];
}


BOOLEAN FILMCOMP::SurfKat(AQVAR* var,
                          const char* inpline,
                          const VARLIST* varlist,
                          char* errorline,
                          CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( var == 0 )           return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   for ( CARDINAL i=0; i<nX; i++ )
   {
      if ( Xvar[i] == var )
      {
         FORMVAR* formvar = new FORMVAR;
         formvar->Symbol("surfkat");
         numparseerrors = formvar->Parse(inpline,varlist,errorline);
         if ( numparseerrors != 0 )
         {
            delete formvar; return FALSE;
         }
         if ( AllowedVar(formvar) == FALSE )
         {
            delete formvar; return FALSE;
         }

         // further dependency checks to be implemented !!

         delete surfkat[i]; surfkat[i] = formvar;
         CalcArg();
         return TRUE;
      }
   }
   return FALSE;
}


const FORMVAR* FILMCOMP::SurfKde(CARDINAL index) const
{
   if ( index >= nX )
      FatalError("FILMCPMP::SurfKde: Illegal index");
   return surfkde[index];
}


BOOLEAN FILMCOMP::SurfKde(AQVAR* var,
                          const char* inpline,
                          const VARLIST* varlist,
                          char* errorline,
                          CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( var == 0 )           return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   for ( CARDINAL i=0; i<nX; i++ )
   {
      if ( Xvar[i] == var )
      {
         FORMVAR* formvar = new FORMVAR;
         formvar->Symbol("surfkde");
         numparseerrors = formvar->Parse(inpline,varlist,errorline);
         if ( numparseerrors != 0 )
         {
            delete formvar; return FALSE;
         }
         if ( AllowedVar(formvar) == FALSE )
         {
            delete formvar; return FALSE;
         }

         // further dependency checks to be implemented !!

         delete surfkde[i]; surfkde[i] = formvar;
         CalcArg();
         return TRUE;
      }
   }
   return FALSE;
}


const FORMVAR* FILMCOMP::VolKat(CARDINAL index) const
{
   if ( index >= nX )
      FatalError("FILMCPMP::VolKat: Illegal index");
   return volkat[index];
}


BOOLEAN FILMCOMP::VolKat(AQVAR* var,
                         const char* inpline,
                         const VARLIST* varlist,
                         char* errorline,
                         CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( var == 0 )           return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   for ( CARDINAL i=0; i<nX; i++ )
   {
      if ( Xvar[i] == var )
      {
         FORMVAR* formvar = new FORMVAR;
         formvar->Symbol("volkat");
         numparseerrors = formvar->Parse(inpline,varlist,errorline);
         if ( numparseerrors != 0 )
         {
            delete formvar; return FALSE;
         }
         if ( AllowedVar(formvar) == FALSE )
         {
            delete formvar; return FALSE;
         }

         // further dependency checks to be implemented !!

         delete volkat[i]; volkat[i] = formvar;
         CalcArg();
         return TRUE;
      }
   }
   return FALSE;
}


const FORMVAR* FILMCOMP::VolKde(CARDINAL index) const
{
   if ( index >= nX )
      FatalError("FILMCPMP::VolKde: Illegal index");
   return volkde[index];
}


BOOLEAN FILMCOMP::VolKde(AQVAR* var,
                         const char* inpline,
                         const VARLIST* varlist,
                         char* errorline,
                         CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( var == 0 )           return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   for ( CARDINAL i=0; i<nX; i++ )
   {
      if ( Xvar[i] == var )
      {
         FORMVAR* formvar = new FORMVAR;
         formvar->Symbol("volkde");
         numparseerrors = formvar->Parse(inpline,varlist,errorline);
         if ( numparseerrors != 0 )
         {
            delete formvar; return FALSE;
         }
         if ( AllowedVar(formvar) == FALSE )
         {
            delete formvar; return FALSE;
         }

         // further dependency checks to be implemented !!

         delete volkde[i]; volkde[i] = formvar;
         CalcArg();
         return TRUE;
      }
   }
   return FALSE;
}


const FORMVAR* FILMCOMP::ResX(CARDINAL index) const
{
   if ( index >= nX )
      FatalError("FILMCPMP::ResX: Illegal index");
   return resX[index];
}


BOOLEAN FILMCOMP::ResX(AQVAR* var,
                       const char* inpline,
                       const VARLIST* varlist,
                       char* errorline,
                       CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( var == 0 )           return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   for ( CARDINAL i=0; i<nX; i++ )
   {
      if ( Xvar[i] == var )
      {
         FORMVAR* formvar = new FORMVAR;
         formvar->Symbol("resX");
         numparseerrors = formvar->Parse(inpline,varlist,errorline);
         if ( numparseerrors != 0 )
         {
            delete formvar; return FALSE;
         }
         if ( AllowedVar(formvar) == FALSE )
         {
            delete formvar; return FALSE;
         }

         // further dependency checks to be implemented !!

         delete resX[i]; resX[i] = formvar;
         CalcArg();
         return TRUE;
      }
   }
   return FALSE;
}


const FORMVAR* FILMCOMP::PoreDiffX(CARDINAL index) const
{
   if ( index >= nX )
      FatalError("FILMCPMP::PoreDiffX: Illegal index");
   return DXpore[index];
}


BOOLEAN FILMCOMP::PoreDiffX(AQVAR* var,
                            const char* inpline,
                            const VARLIST* varlist,
                            char* errorline,
                            CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( var == 0 )           return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   for ( CARDINAL i=0; i<nX; i++ )
   {
      if ( Xvar[i] == var )
      {
         FORMVAR* formvar = new FORMVAR;
         formvar->Symbol("DXpore");
         numparseerrors = formvar->Parse(inpline,varlist,errorline);
         if ( numparseerrors != 0 )
         {
            delete formvar; return FALSE;
         }
         if ( AllowedVar(formvar) == FALSE )
         {
            delete formvar; return FALSE;
         }

         // further dependency checks to be implemented !!

         delete DXpore[i]; DXpore[i] = formvar;
         CalcArg();
         return TRUE;
      }
   }
   return FALSE;
}


const FORMVAR* FILMCOMP::MatrixDiffX(CARDINAL index) const
{
   if ( index >= nX )
      FatalError("FILMCPMP::MatrixDiffX: Illegal index");
   return DXmatrix[index];
}


BOOLEAN FILMCOMP::MatrixDiffX(AQVAR* var,
                              const char* inpline,
                              const VARLIST* varlist,
                              char* errorline,
                              CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( var == 0 )           return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   for ( CARDINAL i=0; i<nX; i++ )
   {
      if ( Xvar[i] == var )
      {
         FORMVAR* formvar = new FORMVAR;
         formvar->Symbol("DXmatrix");
         numparseerrors = formvar->Parse(inpline,varlist,errorline);
         if ( numparseerrors != 0 )
         {
            delete formvar; return FALSE;
         }
         if ( AllowedVar(formvar) == FALSE )
         {
            delete formvar; return FALSE;
         }

         // further dependency checks to be implemented !!

         delete DXmatrix[i]; DXmatrix[i] = formvar;
         CalcArg();
         return TRUE;
      }
   }
   return FALSE;
}


const FORMVAR* FILMCOMP::ResS(CARDINAL index) const
{
   if ( index >= nS )
      FatalError("FILMCPMP::ResS: Illegal index");
   return resS[index];
}


BOOLEAN FILMCOMP::ResS(AQVAR* var,
                       const char* inpline,
                       const VARLIST* varlist,
                       char* errorline,
                       CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( var == 0 )           return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   for ( CARDINAL i=0; i<nS; i++ )
   {
      if ( Svar[i] == var )
      {
         FORMVAR* formvar = new FORMVAR;
         formvar->Symbol("resS");
         numparseerrors = formvar->Parse(inpline,varlist,errorline);
         if ( numparseerrors != 0 )
         {
            delete formvar; return FALSE;
         }
         if ( AllowedVar(formvar) == FALSE )
         {
            delete formvar; return FALSE;
         }

         // further dependency checks to be implemented !!

         delete resS[i]; resS[i] = formvar;
         CalcArg();
         return TRUE;
      }
   }
   return FALSE;
}


const FORMVAR* FILMCOMP::PoreDiffS(CARDINAL index) const
{
   if ( index >= nS )
      FatalError("FILMCPMP::DiffS: Illegal index");
   return DSpore[index];
}


BOOLEAN FILMCOMP::PoreDiffS(AQVAR* var,
                            const char* inpline,
                            const VARLIST* varlist,
                            char* errorline,
                            CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( var == 0 )           return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   for ( CARDINAL i=0; i<nS; i++ )
   {
      if ( Svar[i] == var )
      {
         FORMVAR* formvar = new FORMVAR;
         formvar->Symbol("DSpore");
         numparseerrors = formvar->Parse(inpline,varlist,errorline);
         if ( numparseerrors != 0 )
         {
            delete formvar; return FALSE;
         }
         if ( AllowedVar(formvar) == FALSE )
         {
            delete formvar; return FALSE;
         }

         // further dependency checks to be implemented !!

         delete DSpore[i]; DSpore[i] = formvar;
         CalcArg();
         return TRUE;
      }
   }
   return FALSE;
}


BOOLEAN FILMCOMP::ReactVol(REAL v)
{
   if ( ! ( v>0.0 ) ) return FALSE;
   V = v;
   Changed();
   return TRUE;
}


BOOLEAN FILMCOMP::BulkVol(REAL vb)
{
   if ( ! ( vb>0.0 ) ) return FALSE;
   VB = vb;
   Changed();
   return TRUE;
}


void FILMCOMP::PoreParticles(BOOLEAN b)
{
   porepart = b;
   return;
}


BOOLEAN FILMCOMP::MatrixDiff(BOOLEAN b)
{
   if ( b==FALSE && detachglobal==FALSE ) return FALSE;
   matrixdiff = b;
   return TRUE;
}


BOOLEAN FILMCOMP::DetachGlobal(BOOLEAN b)
{
   if ( b==FALSE && matrixdiff==FALSE ) return FALSE;
   detachglobal = b;
   return TRUE;
}


BOOLEAN FILMCOMP::Udet(const char* inpline,
                       const VARLIST* varlist,
                       char* errorline,
                       CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("Ude");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar; return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar; return FALSE;
   }

   // add dependence conditions!!

   delete udet; udet = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN FILMCOMP::Surface(const char* inpline,
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

   // no dependence on a State Variable and on Program Variables with
   // the exception of CalcNum and SpaceZ:

   if ( formvar->ArgVarType(StateVar) == TRUE )
   {
      delete formvar; return FALSE;
   }
   if ( formvar->ArgProgVarType(Time) == TRUE )
   {
      delete formvar; return FALSE;
   }
   if ( formvar->ArgProgVarType(Discharge) == TRUE )
   {
      delete formvar; return FALSE;
   }
   if ( formvar->ArgProgVarType(FilmDepth) == TRUE )
   {
      delete formvar; return FALSE;
   }
   if ( formvar->ArgProgVarType(WaterFraction) == TRUE )
   {
      delete formvar; return FALSE;
   }
   if ( formvar->ArgProgVarType(FilmVelocity) == TRUE )
   {
      delete formvar; return FALSE;
   }
   if ( formvar->ArgProgVarType(InterfaceVelocity) == TRUE )
   {
      delete formvar; return FALSE;
   }
   if ( formvar->ArgProgVarType(DetachmentVelocity) == TRUE )
   {
      delete formvar; return FALSE;
   }
   if ( formvar->ArgProgVarType(AttachmentVelocity) == TRUE )
   {
      delete formvar; return FALSE;
   }
   if ( formvar->ArgProgVarType(Volume) == TRUE )
   {
      delete formvar; return FALSE;
   }
   if ( formvar->ArgProgVarType(BulkVolume) == TRUE )
   {
      delete formvar; return FALSE;
   }

   delete A; A = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN FILMCOMP::RateEps(const char* inpline,
                          const VARLIST* varlist,
                          char* errorline,
                          CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("rate_eps");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar; return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar; return FALSE;
   }

   // add dependence conditions!!

   delete reps; reps = formvar;
   CalcArg();
   return TRUE;
}


CARDINAL FILMCOMP::NumEq() const
{
   CARDINAL n  = numgrid;
   CARDINAL nv = NumVolStateVar();
   CARDINAL ns = NumSurfStateVar();
   CARDINAL ne = NumEquStateVar();
   if ( nv == 0 ) return 0;
   CARDINAL cell1 = nv+ne+1;
   CARDINAL cell2 = nv+ne+1;
   CARDINAL cell3 = nv+ne+3;
   CARDINAL cellb = nv+ne+ns+1;
   if ( porepart == TRUE )
   {
      cell1 += nv+1;
      cell3 += nv+1;
   }
   return cell1 + (n-1)*cell3 + cell2 + cellb + 3;
}


REAL FILMCOMP::RelAcc(CARDINAL index) const
{
   CARDINAL n  = numgrid;
   CARDINAL nv = NumVolStateVar();
   CARDINAL ns = NumSurfStateVar();
   CARDINAL ne = NumEquStateVar();
   CARDINAL cell1 = nv+ne+1;
   CARDINAL cell2 = nv+ne+1;
   CARDINAL cell3 = nv+ne+3;
   CARDINAL cellb = nv+ne+ns+1;
   if ( porepart == TRUE )
   {
      cell1 += nv+1;
      cell3 += nv+1;
   }
   if ( index < cell1 )                                  // Biofilm base
   {
      if ( index < nv )          return statevar[index]->RelAccuracy();
      if ( index < nv+ne)        return statevar[index+ns]->RelAccuracy();
      if ( index == nv+ne )      return relaccEPS;       // eps
      if ( porepart == FALSE ) FatalError("FILMCOMP::RelAcc: illegal index");
      if ( index < nv+ne+1+nv )  return statevar[index-nv-ne-1]->RelAccuracy();
      if ( index == nv+ne+1+nv ) return relaccEPS;       // por
      FatalError("FILMCOMP::RelAcc: illegal index");      
   }
   if ( index < cell1+(n-2)*cell3 )                      // Biofilm layer
   {
      CARDINAL i = (index-cell1) % cell3;
      if ( i < nv )          return statevar[i]->RelAccuracy();
      if ( i < nv+ne )       return statevar[i+ns]->RelAccuracy();
      if ( i == nv+ne )      return relaccEPS;           // eps
      if ( i == nv+ne+1 )    return relaccZ;             // z
      if ( i == nv+ne+2 )    return relaccZ;             // uF
      if ( porepart == FALSE ) FatalError("FILMCOMP::RelAcc: illegal index");
      if ( i < nv+ne+3+nv )  return statevar[i-nv-ne-3]->RelAccuracy();
      if ( i == nv+ne+3+nv ) return relaccEPS;           // por
      FatalError("FILMCOMP::RelAcc: illegal index");      
   }
   if ( index < cell1+(n-1)*cell3 )                      // Biofilm front
   {
      CARDINAL i = index-(cell1+(n-2)*cell3);
      if ( i < nv )          return statevar[i]->RelAccuracy();
      if ( i < nv+ne )       return statevar[i+ns]->RelAccuracy();
      if ( i == nv+ne )      return relaccEPS;           // eps
      if ( i == nv+ne+1 )    return relaccZ;             // uat
      if ( i == nv+ne+2 )    return relaccZ;             // ude
      if ( porepart == FALSE ) FatalError("FILMCOMP::RelAcc: illegal index");
      if ( i < nv+ne+3+nv )  return statevar[i-nv-ne-3]->RelAccuracy();
      if ( i == nv+ne+3+nv ) return relaccEPS;
      FatalError("FILMCOMP::RelAcc: illegal index");      
   }
   if ( index < cell1+(n-1)*cell3+cell2 )                // Liquid layer
   {
      CARDINAL i = index-(cell1+(n-1)*cell3);
      if ( i < nv )     return statevar[i]->RelAccuracy();
      if ( i < nv+ne)   return statevar[i+ns]->RelAccuracy();
      if ( i == nv+ne ) return relaccEPS;
      FatalError("FILMCOMP::RelAcc: illegal index");      
   }
   if ( index < cell1+(n-1)*cell3+cell2+cellb )          // Bulk volume
   {
      CARDINAL i = index-(cell1+(n-1)*cell3+cell2);
      if ( i < nv+ne+ns )  return statevar[i]->RelAccuracy();
      if ( i == nv+ne+ns ) return relaccEPS;
      FatalError("FILMCOMP::RelAcc: illegal index");      
   }
   if ( index == cell1+(n-1)*cell3+cell2+cellb )         // Qin
      return relaccQ;
   if ( index == cell1+(n-1)*cell3+cell2+cellb+1 )       // Qout
      return relaccQ;
   if ( index == cell1+(n-1)*cell3+cell2+cellb+2 )       // VB*eps
      return relaccV;
   FatalError("FILMCOMP::RelAcc: Illegal index");
   return 0.0;
}
   

REAL FILMCOMP::AbsAcc(CARDINAL index) const
{
   CARDINAL n  = numgrid;
   CARDINAL nv = NumVolStateVar();
   CARDINAL ns = NumSurfStateVar();
   CARDINAL ne = NumEquStateVar();
   CARDINAL cell1 = nv+ne+1;
   CARDINAL cell2 = nv+ne+1;
   CARDINAL cell3 = nv+ne+3;
   CARDINAL cellb = nv+ne+ns+1;
   if ( porepart == TRUE )
   {
      cell1 += nv+1;
      cell3 += nv+1;
   }
   if ( index < cell1 )                                  // Biofilm base
   {
      if ( index < nv )          return statevar[index]->AbsAccuracy();
      if ( index < nv+ne)        return statevar[index+ns]->AbsAccuracy();
      if ( index == nv+ne )      return absaccEPS;       // eps
      if ( porepart == FALSE ) FatalError("FILMCOMP::AbsAcc: illegal index");
      if ( index < nv+ne+1+nv )  return statevar[index-nv-ne-1]->AbsAccuracy();
      if ( index == nv+ne+1+nv ) return absaccEPS;       // por
      FatalError("FILMCOMP::AbsAcc: illegal index");      
   }
   if ( index < cell1+(n-2)*cell3 )                      // Biofilm layer
   {
      CARDINAL i = (index-cell1) % cell3;
      if ( i < nv )          return statevar[i]->AbsAccuracy();
      if ( i < nv+ne )       return statevar[i+ns]->AbsAccuracy();
      if ( i == nv+ne )      return absaccEPS;           // eps
      if ( i == nv+ne+1 )    return absaccZ;             // z
      if ( i == nv+ne+2 )    return absaccZ;             // uF
      if ( porepart == FALSE ) FatalError("FILMCOMP::AbsAcc: illegal index");
      if ( i < nv+ne+3+nv )  return statevar[i-nv-ne-3]->AbsAccuracy();
      if ( i == nv+ne+3+nv ) return absaccEPS;           // por
      FatalError("FILMCOMP::AbsAcc: illegal index");      
   }
   if ( index < cell1+(n-1)*cell3 )                      // Biofilm front
   {
      CARDINAL i = index-(cell1+(n-2)*cell3);
      if ( i < nv )          return statevar[i]->AbsAccuracy();
      if ( i < nv+ne )       return statevar[i+ns]->AbsAccuracy();
      if ( i == nv+ne )      return absaccEPS;           // eps
      if ( i == nv+ne+1 )    return absaccZ;             // uat
      if ( i == nv+ne+2 )    return absaccZ;             // ude
      if ( porepart == FALSE ) FatalError("FILMCOMP::AbsAcc: illegal index");
      if ( i < nv+ne+3+nv )  return statevar[i-nv-ne-3]->AbsAccuracy();
      if ( i == nv+ne+3+nv ) return absaccEPS;
      FatalError("FILMCOMP::AbsAcc: illegal index");      
   }
   if ( index < cell1+(n-1)*cell3+cell2 )                // Liquid layer
   {
      CARDINAL i = index-(cell1+(n-1)*cell3);
      if ( i < nv )     return statevar[i]->AbsAccuracy();
      if ( i < nv+ne)   return statevar[i+ns]->AbsAccuracy();
      if ( i == nv+ne ) return absaccEPS;
      FatalError("FILMCOMP::AbsAcc: illegal index");      
   }
   if ( index < cell1+(n-1)*cell3+cell2+cellb )          // Bulk volume
   {
      CARDINAL i = index-(cell1+(n-1)*cell3+cell2);
      if ( i < nv+ne+ns )  return statevar[i]->AbsAccuracy();
      if ( i == nv+ne+ns ) return absaccEPS;
      FatalError("FILMCOMP::AbsAcc: illegal index");      
   }
   if ( index == cell1+(n-1)*cell3+cell2+cellb )         // Qin
      return absaccQ;
   if ( index == cell1+(n-1)*cell3+cell2+cellb+1 )       // Qout
      return absaccQ;
   if ( index == cell1+(n-1)*cell3+cell2+cellb+2 )       // VB*eps
      return absaccV;
   FatalError("FILMCOMP::AbsAcc: Illegal index");
   return 0.0;
}
   

EQTYPE FILMCOMP::EqType(CARDINAL index) const
{
   CARDINAL n  = numgrid;
   CARDINAL nv = NumVolStateVar();
   CARDINAL ns = NumSurfStateVar();
   CARDINAL ne = NumEquStateVar();
   CARDINAL cell1 = nv+ne+1;
   CARDINAL cell2 = nv+ne+1;
   CARDINAL cell3 = nv+ne+3;
   CARDINAL cellb = nv+ne+ns+1;
   if ( porepart == TRUE )
   {
      cell1 += nv+1;
      cell3 += nv+1;
   }
   if ( index < cell1 )                                       // Biofilm base
   {
      return ALG;
   }
   if ( index < cell1+(n-2)*cell3 )                           // Biofilm layer
   {
      CARDINAL i = (index-cell1) % cell3;
      if ( i < nv )          return DIFF;   // vol. statevar matrix
      if ( i < nv+ne)        return ALG;    // eq. statevar
      if ( i == nv+ne )      return ALG;    // eps
      if ( i == nv+ne+3+nv ) return ALG;    // por
      if ( i == nv+ne+1 )    return DIFF;   // z
      if ( i == nv+ne+2 )    return ALG;    // uF
      return DIFF;                          // vol. statevar pore
   }
   if ( index < cell1+(n-1)*cell3 )                          // Biofilm front
   {
      CARDINAL i = (index-cell1) % cell3;
      if ( i < nv )
      {
         if ( matrixdiff==FALSE && part[i]==TRUE ) return DIFF;
      }
      return ALG;
   }
   if ( index < cell1+(n-1)*cell3+cell2 )                   // Liquid layer
   {
      return ALG;
   }
   if ( index < cell1+(n-1)*cell3+cell2+cellb )             // Bulk volume
   {
      CARDINAL i = index-(cell1+(n-1)*cell3+cell2);
      if ( i < nv+ns ) return DIFF;
      return ALG;
   }
   if ( index == cell1+(n-1)*cell3+cell2+cellb )            // Qin
      return ALG;
   if ( index == cell1+(n-1)*cell3+cell2+cellb+1 )          // Qout
      return ALG;
   if ( index == cell1+(n-1)*cell3+cell2+cellb+2 )          // VB*eps
      return ALG;
   FatalError("FILMCOMP::EqType: Illegal index");
   return ALG;
}


void FILMCOMP::CalcGeometry(VARSYS* varsys, const REAL* y,
                            REAL* Area, REAL* Vol)
{
   CARDINAL j;
   CARDINAL n  = numgrid;
   CARDINAL nv = NumVolStateVar();
   CARDINAL ne = NumEquStateVar();
   CARDINAL ns = NumSurfStateVar();
   PROGVAR* Zvar = varsys->GetProgVar(SpaceZ);
   CARDINAL cell1 = nv+ne+1;
   CARDINAL cell2 = nv+ne+1;
   CARDINAL cell3 = nv+ne+3;
   CARDINAL cellb = nv+ne+ns+1;
   if ( porepart == TRUE )
   {
      cell1 += nv+1;
      cell3 += nv+1;
   }

   // calculate grid points:

   REAL* z = new REAL[n-2];
   for ( j=0; j<n-2; j++ ) z[j] = y[cell1+j*cell3+nv+ne+1];
   gridpts[0] = 0.0;                                         // start of film
   gridpts[1] = 0.5*z[0];                                    // first cell
   for ( j=2; j<n-1; j++ ) gridpts[j] = 0.5*(z[j-1]+z[j-2]); // other cells
   gridpts[n-1] = z[n-3];                                    // end  of film
   gridpts[n] = z[n-3];                                      // start of bulk
   gridpts[n+1] = z[n-3];                                    // bulk

   // calculate areas:

   varsys->Reset();
   if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[0]);
   Area[0] = A->Value();                             // start of film
   for ( j=1; j<n-1; j++ )
   {
      varsys->Reset();
      if ( Zvar != 0 ) Zvar->SetProgVar(&z[j-1]);
      Area[j] = A->Value();                          // cell boundaries
   }

   // calculate volumes (exact integration for A linear in z):

   Vol[0] = 0.5*(Area[0]+Area[1])*z[0];
   Vol[n-2] = V - Vol[0];
   for ( j=1; j<n-2; j++ )
   {
      Vol[j] = 0.5*(Area[j]+Area[j+1])*(z[j]-z[j-1]);
      Vol[n-2] -= Vol[j];
   }
   if ( confined == FALSE ) Vol[n-2] = VB;

   delete [] z;
   return;
}


void FILMCOMP::CalcConc(CARDINAL j, BOOLEAN bfmatrix,
                        const REAL* y, const REAL* Vol,
                        REAL* conc, REAL* eps, REAL* por)
{
   CARDINAL n  = numgrid;
   CARDINAL nv = NumVolStateVar();
   CARDINAL ns = NumSurfStateVar();
   CARDINAL ne = NumEquStateVar();
   CARDINAL cell1 = nv+ne+1;
   CARDINAL cell2 = nv+ne+1;
   CARDINAL cell3 = nv+ne+3;
   CARDINAL cellb = nv+ne+ns+1;
   if ( porepart == TRUE )
   {
      cell1 += nv+1;
      cell3 += nv+1;
   }
   CARDINAL i;
   if ( j == 0 )         // Biofilm base boundary point
   {
      *eps = y[nv+ne];
      if ( porepart == TRUE )
      {
         *por = y[nv+ne+1+nv];
      }
      else
      {
         *por = *eps;
      }
      if ( bfmatrix == TRUE )
      {
         for ( i=0; i<nv; i++ ) conc[i]       = y[i];
         for ( i=0; i<ns; i++ ) conc[nv+i]    = 0.0;
         for ( i=0; i<ne; i++ ) conc[nv+ns+i] = y[nv+i];
      }
      else
      {
         for ( i=0; i<nv; i++ )
         {
            if ( part[i] == TRUE )
            {
               if ( porepart == TRUE )
               {
                  conc[i] = y[nv+ne+1+i];
               }
               else
               {
                  conc[i] = 0.0;
               }
            }
            else
            {
               conc[i] = y[i];
            }
         }
         for ( i=0; i<ns; i++ ) conc[nv+i]    = 0.0;
         for ( i=0; i<ne; i++ ) conc[nv+ns+i] = y[nv+i];
      }
      return;
   }
   if ( j <= n-2 )       // Biofilm layers
   {
      *eps = y[cell1+(j-1)*cell3+nv+ne];
      if ( porepart == TRUE )
      {
         *por = y[cell1+(j-1)*cell3+nv+ne+3+nv];
      }
      else
      {
         *por = *eps;
      }
      if ( bfmatrix == TRUE )
      {
         for ( i=0; i<nv; i++ )
         {
            if ( part[i] == TRUE )
            {
               conc[i] = y[cell1+(j-1)*cell3+i]/Vol[j-1];
            }
            else
            {
               conc[i] = y[cell1+(j-1)*cell3+i]/(*eps)/Vol[j-1];
            }
         }
         for ( i=0; i<ns; i++ ) conc[nv+i]    = 0.0;
         for ( i=0; i<ne; i++ )
         {
            conc[nv+ns+i] = y[cell1+(j-1)*cell3+nv+i];
         }
      }
      else
      {
         for ( i=0; i<nv; i++ )
         {
            if ( part[i] == TRUE )
            {
               if ( porepart == TRUE )
               {
                  conc[i] = y[cell1+(j-1)*cell3+nv+ne+3+i]/Vol[j-1];
               }
               else
               {
                  conc[i] = 0.0;
               }
            }
            else
            {
               conc[i] = y[cell1+(j-1)*cell3+i]/(*eps)/Vol[j-1];
            }
         }
         for ( i=0; i<ns; i++ ) conc[nv+i]    = 0.0;
         for ( i=0; i<ne; i++ )
         {
            conc[nv+ns+i] = y[cell1+(j-1)*cell3+nv+i];
         }
      }
      return;
   }
   if ( j == n-1 )       // Biofilm front boundary point
   {
      *eps = y[cell1+(n-2)*cell3+nv+ne];
      if ( porepart == TRUE )
      {
         *por = y[cell1+(n-2)*cell3+nv+ne+3+nv];
      }
      else
      {
         *por = *eps;
      }
      if ( bfmatrix == TRUE )
      {
         for ( i=0; i<nv; i++ )
         {
            conc[i] = y[cell1+(n-2)*cell3+i];
         }
         for ( i=0; i<ns; i++ )
         {
            conc[nv+i] = 0.0;
         }
         for ( i=0; i<ne; i++ )
         {
            conc[nv+ns+i] = y[cell1+(n-2)*cell3+nv+i];
         }
      }
      else
      {
         for ( i=0; i<nv; i++ )
         {
            if ( part[i] == TRUE )
            {
               if ( porepart == TRUE )
               {
                  conc[i] = y[cell1+(n-2)*cell3+nv+ne+3+i];
               }
               else
               {
                  conc[i] = 0.0;
               }
            }
            else
            {
               conc[i] = y[cell1+(n-2)*cell3+i];
            }
         }
         for ( i=0; i<ns; i++ )
         {
            conc[nv+i] = 0.0;
         }
         for ( i=0; i<ne; i++ )
         {
            conc[nv+ns+i] = y[cell1+(n-2)*cell3+nv+i];
         }
      }
      return;
   }
   if ( j == n )      // Liquid layer boundary point
   {
      *eps = y[cell1+(n-1)*cell3+nv+ne];
      *por = 1.0;
      for ( i=0; i<nv; i++ )
      {
         conc[i] = y[cell1+(n-1)*cell3+i];
      }
      for ( i=0; i<ns; i++ )
      {
         conc[nv+i] = 0.0;
      }
      for ( i=0; i<ne; i++ )
      {
         conc[nv+ns+i] = y[cell1+(n-1)*cell3+nv+i];
      }
      return;
   }
   if ( j == n+1 )      // Bulk volume
   {
      *eps = y[cell1+(n-1)*cell3+cell2+nv+ns+ne];
      *por = 1.0;
      for ( i=0; i<nv; i++ )
      {
         if ( part[i] == TRUE )
         {
            conc[i] = y[cell1+(n-1)*cell3+cell2+i]/Vol[n-2];
         }
         else
         {
            conc[i] = y[cell1+(n-1)*cell3+cell2+i]/(*eps)/Vol[n-2];
         }
      }
      for ( i=0; i<ns; i++ )
      {
         conc[nv+i] = y[cell1+(n-1)*cell3+cell2+nv+i];
      }
      for ( i=0; i<ne; i++ )
      {
         conc[nv+ns+i] = y[cell1+(n-1)*cell3+cell2+nv+ns+i];
      }
      return;
   }
   FatalError("FILMCOMP::CalcConc: Illegal index");
   return;
}


BOOLEAN FILMCOMP::InitCond(VARSYS* varsys, REAL* Y, CARDINAL callnum)
{
   if ( Active() != TRUE ) return FALSE;
   if ( callnum > 0 ) return TRUE;
   REAL*    y = &Y[ind_y];
   CARDINAL i,j,k;
   CARDINAL n  = numgrid;
   CARDINAL nv = NumVolStateVar();
   CARDINAL ns = NumSurfStateVar();
   CARDINAL ne = NumEquStateVar();
   CARDINAL cell1 = nv+ne+1;
   CARDINAL cell2 = nv+ne+1;
   CARDINAL cell3 = nv+ne+3;
   CARDINAL cellb = nv+ne+ns+1;
   if ( porepart == TRUE )
   {
      cell1 += nv+1;
      cell3 += nv+1;
   }
   PROGVAR* Zvar   = varsys->GetProgVar(SpaceZ);
   PROGVAR* LFvar  = varsys->GetProgVar(FilmDepth);
   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   REAL zoneindex; varsys->SetProgVar(ZoneIndex,&zoneindex);


   // initialize grid points and film velocity:
   // -----------------------------------------

   zoneindex = 1.0;          // biofilm matrix zone
   BOOLEAN found = FALSE;
   for ( j=0; j<NumInitCond(); j++ )
   {
      if ( initvar[j]==LFvar && initzone[j]==1 )
      {
         k = j; found = TRUE; break;
      }
   }
   if ( found == TRUE )
   {
      varsys->Reset();
      y[cell1+(n-3)*cell3+nv+ne+1] = initval[k]->Value();
   }
   else
   {
      y[cell1+(n-3)*cell3+nv+ne+1] = ini.R(2);       // film depth
   }
   for ( i=0; i<n-2; i++ )
   {
      y[cell1+i*cell3+nv+ne+1]
       = (i+1)*y[cell1+(n-3)*cell3+nv+ne+1]/(n-2);   // z[i]
      y[cell1+i*cell3+nv+ne+2] = 0.0;                // uF[i]
   }
   if ( LFvar != 0 ) LFvar->SetProgVar(&y[cell1+(n-3)*cell3+nv+ne+1]);

   REAL* Area = new REAL[n-1];   
   REAL* Vol  = new REAL[n-1];
   CalcGeometry(varsys,y,Area,Vol);
   y[cell1+(n-2)*cell3+nv+ne+1]       = 0.0;      // uat
   y[cell1+(n-2)*cell3+nv+ne+2]       = 0.0;      // ude
   y[cell1+(n-1)*cell3+cell2+cellb]   = 0.0;      // Qin
   y[cell1+(n-1)*cell3+cell2+cellb+1] = 0.0;      // Qout
   y[cell1+(n-1)*cell3+cell2+cellb+2] = Vol[n-2]; // VB

   // initialize concentrations of dynamic volume state variables:
   // ------------------------------------------------------------

   // initialize concentrations:

   REAL* rh   = new REAL[nv];
   REAL* epsX = new REAL[n+2];
   BOOLEAN foundpart = FALSE;

   for ( i=0; i<nv; i++ )
   {
      y[i] = 0.0;                  // initialize to zero  // Biofilm base
      for ( j=1; j<=n-2; j++ )                            // Biofilm layer
      {
         y[cell1+(j-1)*cell3+i] = 0.0;
      }
      y[cell1+(n-2)*cell3+i] = 0.0;                       // Biofilm front
      y[cell1+(n-1)*cell3+i] = 0.0;                       // Liquid layer
      y[cell1+(n-1)*cell3+cell2+i] = 0.0;                 // Bulk volume

      if ( porepart == TRUE )
      {
         y[nv+ne+1+i] = 0.0;                              // Biofilm base
         for ( j=1; j<=n-2; j++ )
         {
            y[cell1+(j-1)*cell3+nv+ne+3+i] = 0.0;         // Biofilm layer
         }
         y[cell1+(n-2)*cell3+nv+ne+3+i] = 0.0;            // Biofilm front
      }

      if ( part[i] == TRUE )           // determine densities
      {
         rh[i] = rho[ind[i]]->Value();
      }
      else
      {
         rh[i] = 0.0;
      }

      zoneindex = 1.0;   // biofilm matrix zone
      found = FALSE;                  // search for biofilm initial condition
      for ( j=0; j<NumInitCond(); j++ )
      {
         if ( initvar[j]==statevar[i] && initzone[j]==1 )     // Matrix zone
         {
            k = j; found = TRUE; break;
         }
      }
      if ( found == TRUE )            // initialize  concentration
      {
         varsys->Reset();                                     // Biofilm base
         if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[0]);
         y[i] = initval[k]->Value();
         for ( j=1; j<=n-2; j++ )                             // Biofilm layers
         {
            varsys->Reset();
            if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[j]);
            y[cell1+(j-1)*cell3+i] = initval[k]->Value();
         }
         varsys->Reset();                                     // Biofilm front
         if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[n-1]);
         y[cell1+(n-2)*cell3+i] = initval[k]->Value();
         if ( part[i] == FALSE )   // diss: liquid layer conc. = front conc.
         {
            y[cell1+(n-1)*cell3+i] = y[cell1+(n-2)*cell3+i];
         }
         else
         {
            foundpart = TRUE;
         }
      }

      zoneindex = 2.0;   // biofilm pore volume
      found = FALSE;               // search for biofilm initial condition
      for ( j=0; j<NumInitCond(); j++ )
      {
         if ( initvar[j]==statevar[i] && initzone[j]==2 )  // Pore zone
         {
            k = j; found = TRUE; break;
         }
      }
      if ( found == TRUE )            // initialize  concentration
      {
         if ( part[i] == TRUE )
         {
            if ( porepart == TRUE )  // define pore part. conc.
            {
               varsys->Reset();                               // Biofilm base
               if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[0]);
               y[nv+ne+1+i] = initval[k]->Value();
               for ( j=1; j<=n-2; j++ )                       // Biofilm layers
               {
                  varsys->Reset();
                  if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[j]);
                  y[cell1+(j-1)*cell3+nv+ne+3+i] = initval[k]->Value();
               }
               varsys->Reset();                               // Biofilm front
               if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[n-1]);
               y[cell1+(n-2)*cell3+nv+ne+3+i] = initval[k]->Value();
               // liquid layer conc. = front conc.:
               y[cell1+(n-1)*cell3+i] = y[cell1+(n-2)*cell3+nv+ne+3+i];
            }
         }
         else   // redefine dissolved conc. "biofilm matrix"
         {
            varsys->Reset();                               // Biofilm base
            if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[0]);
            y[i] = initval[k]->Value();
            for ( j=1; j<=n-2; j++ )                       // Biofilm layers
            {
               varsys->Reset();
               if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[j]);
               y[cell1+(j-1)*cell3+i] = initval[k]->Value();
            }
            varsys->Reset();                               // Biofilm front
            if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[n-1]);
            y[cell1+(n-2)*cell3+i] = initval[k]->Value();
            // liquid layer conc. = front conc.:
            y[cell1+(n-1)*cell3+i] = y[cell1+(n-2)*cell3+i];
         }
      } 

      zoneindex = 0.0;   // bulk volume zone
      found = FALSE;                  // search for bulk initial condition
      for ( j=0; j<NumInitCond(); j++ )
      {
         if ( initvar[j]==statevar[i] && initzone[j]==0 ) // Bulk volume zone
         {
            k = j; found = TRUE; break;
         }
      }
      if ( found == TRUE )            // initialize concentration
      {
         varsys->Reset();                                     // Bulk volume
         if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[n+1]);
         y[cell1+(n-1)*cell3+cell2+i] = initval[k]->Value();
         if ( part[i] == TRUE )  // part: liquid layer conc. = bulk vol. conc.
         {
            y[cell1+(n-1)*cell3+i] = y[cell1+(n-1)*cell3+cell2+i];
         }
      }
   }
   if ( foundpart == FALSE )
   {
      *cjob << "\n   FILMCOMP numerical problem:"
            << " No part. initial cond. specified in biofilm";
      cjob->flush();
      return FALSE;
   }

   // initialize eps:

   for ( j=0; j<n+2; j++ ) epsX[j] = 0.0;   // calculate epsX
   for ( i=0; i<nv; i++ )
   {
      if ( part[i] == TRUE )
      {
         epsX[0] += y[i]/rh[i];                              // Biofilm base
         if ( y[i] < 0.0 )
         {
            *cjob << "\n   FILMCOMP numerical problem:"
                  << " Initial particulate conc. negative";
            cjob->flush();
            return FALSE;
         }
         if ( porepart == TRUE )
         {
            epsX[0] += y[nv+ne+1+i]/rh[i];
            if ( y[nv+ne+1+i] < 0.0 )
            {
               *cjob << "\n   FILMCOMP numerical problem:"
                     << " Initial particulate conc. negative";
               cjob->flush();
               return FALSE;
            }
         }
         for ( j=1; j<=n-2; j++ )
         {
            epsX[j] += y[cell1+(j-1)*cell3+i]/rh[i];         // Biofilm layers
            if ( y[cell1+(j-1)*cell3+i] < 0.0 )
            {
               *cjob << "\n   FILMCOMP numerical problem:"
                     << " Initial particulate conc. negative";
               cjob->flush();
               return FALSE;
            }
            if ( porepart == TRUE )
            {
               epsX[j] += y[cell1+(j-1)*cell3+nv+ne+3+i]/rh[i];
               if ( y[cell1+(j-1)*cell3+nv+ne+3+i] < 0.0 )
               {
                  *cjob << "\n   FILMCOMP numerical problem:"
                        << " Initial particulate conc. negative";
                  cjob->flush();
                  return FALSE;
               }
            }
         }
         epsX[n-1] += y[cell1+(n-2)*cell3+i]/rh[i];          // Biofilm front
         if ( y[cell1+(n-2)*cell3+i] < 0.0 )
         {
            *cjob << "\n   FILMCOMP numerical problem:"
                  << " Initial particulate conc. negative";
            cjob->flush();
            return FALSE;
         }
         if ( porepart == TRUE )
         {
            epsX[n-1] += y[cell1+(n-2)*cell3+nv+ne+3+i]/rh[i];
            if ( y[cell1+(n-2)*cell3+nv+ne+3+i] < 0.0 )
            {
               *cjob << "\n   FILMCOMP numerical problem:"
                     << " Initial particulate conc. negative";
               cjob->flush();
               return FALSE;
            }
         }
         epsX[n] += y[cell1+(n-1)*cell3+i]/rh[i];            // Liquid layer
         if ( y[cell1+(n-1)*cell3+i] < 0.0 )
         {
            *cjob << "\n   FILMCOMP numerical problem:"
                  << " Initial particulate conc. negative";
            cjob->flush();
            return FALSE;
         }
         epsX[n+1] += y[cell1+(n-1)*cell3+cell2+i]/rh[i];    // Bulk volume
         if ( y[cell1+(n-1)*cell3+cell2+i] < 0.0 )
         {
            *cjob << "\n   FILMCOMP numerical problem:"
                  << " Initial particulate conc. negative";
            cjob->flush();
            return FALSE;
         }
      }
   }
   if ( epsX[0] < 1.0 )                                      // Biofilm base
   {
      if ( epsX[0] <= 0.0 )
      {
         *cjob << "\n   FILMCOMP numerical problem:"
               << " Initial part. volume fraction zero";
         cjob->flush();
         return FALSE;
      }
      y[nv+ne] = 1.0-epsX[0];
   }
   else    // correct too large particulate volume fractions
   {
      REAL eps = 0.1;
      y[nv+ne] = eps;
      for ( i=0; i<nv; i++ )
      {
         y[i] *= (1.0-eps)/epsX[0];
      }
      if ( porepart == TRUE )
      {
         y[nv+ne+1+i] *= (1.0-eps)/epsX[0];
      }
   }
   for ( j=1; j<=n-2; j++ )                                  // Biofilm layers
   {
      if ( epsX[j] <= 0.0 )
      {
         *cjob << "\n   FILMCOMP numerical problem:"
               << " Initial part. volume fraction zero";
         cjob->flush();
         return FALSE;
      }
      if ( epsX[j] < 1.0 )
      {
         y[cell1+(j-1)*cell3+nv+ne] = 1.0-epsX[j];
      }
      else
      {
         REAL eps = 0.1;
         y[cell1+(j-1)*cell3+nv+ne] = eps;
         for ( i=0; i<nv; i++ )
         {
            y[cell1+(j-1)*cell3+i] *= (1.0-eps)/epsX[j];
         }
         if ( porepart == TRUE )
         {
            for ( i=0; i<nv; i++ )
            {
               y[cell1+(j-1)*cell3+nv+ne+3+i] *= (1.0-eps)/epsX[j];
            }
         }
      }
   }
   if ( epsX[n-1] < 1.0 )                                    // Biofilm front
   {
      if ( epsX[n-1] <= 0.0 )
      {
         *cjob << "\n   FILMCOMP numerical problem:"
               << " Initial part. volume fraction zero";
         cjob->flush();
         return FALSE;
      }
      y[cell1+(n-2)*cell3+nv+ne] = 1.0-epsX[n-1];
   }
   else
   {
      REAL eps = 0.1;
      y[cell1+(n-2)*cell3+nv+ne] = eps;
      for ( i=0; i<nv; i++ )
      {
         y[cell1+(n-2)*cell3+i] *= (1.0-eps)/epsX[n-1];
      }
      if ( porepart == TRUE )
      {
         for ( i=0; i<nv; i++ )
         {
            y[cell1+(n-2)*cell3+nv+ne+3+i] *= (1.0-eps)/epsX[n-1];
         }
      }
   }
   if ( epsX[n] < 1.0 )                                       // Liquid layer
   {
      y[cell1+(n-1)*cell3+nv+ne] = 1.0-epsX[n];
   }
   else
   {
      REAL eps = 0.99;
      y[cell1+(n-1)*cell3+nv+ne] = eps;
      for ( i=0; i<nv; i++ )
      {
         y[cell1+(n-1)*cell3+i] *= (1.0-eps)/epsX[n];
      }
   }
   if ( epsX[n+1] < 1.0 )                                    // Bulk volume
   {
      y[cell1+(n-1)*cell3+cell2+nv+ne] = 1.0-epsX[n+1];
   }
   else
   {
      REAL eps = 0.99;
      y[cell1+(n-1)*cell3+cell2+nv+ne] = eps;
      for ( i=0; i<nv; i++ )
      {
         y[cell1+(n-1)*cell3+cell2+i] *= (1.0-eps)/epsX[n+1];
      }
   }

   // initialize porosity:

   if ( porepart == TRUE )
   {
      y[nv+ne+1+nv] = 1.0;                                  // Biofilm base
      for ( i=0; i<nv; i++ )
      {
         if ( part[i] == TRUE )
         {
            y[nv+ne+1+nv] -= y[i]/rh[i];
         }
      }
      for ( j=1; j<=n-2; j++ )                              // Biofilm layers
      {
         y[cell1+(j-1)*cell3+nv+ne+3+nv] = 1.0;
         for ( i=0; i<nv; i++ )
         {
            if ( part[i] == TRUE )
            {
               y[cell1+(j-1)*cell3+nv+ne+3+nv]
                  -= y[cell1+(j-1)*cell3+i]/rh[i];
            }
         }
      }
      y[cell1+(n-2)*cell3+nv+ne+3+nv] = 1.0;                // Biofilm front
      for ( i=0; i<nv; i++ )
      {
         if ( part[i] == TRUE )
         {
            y[cell1+(n-2)*cell3+nv+ne+3+nv]
               -= y[cell1+(n-2)*cell3+i]/rh[i];
         }
      }         
   }

   // multiply with volume:

   for ( i=0; i<nv; i++ )                                   // Biofilm layers
   {
      if ( part[i] == TRUE )
      {
         for ( j=1; j<=n-2; j++ )
         {
            y[cell1+(j-1)*cell3+i] *= Vol[j-1];
         }
      }
      else
      {
         for ( j=1; j<=n-2; j++ )
         {
            y[cell1+(j-1)*cell3+i]
               *= Vol[j-1]*y[cell1+(j-1)*cell3+nv+ne];
         }
      }
      if ( porepart == TRUE )
      {
         if ( part[i] == TRUE )
         {
            for ( j=1; j<=n-2; j++ )
            {
               y[cell1+(j-1)*cell3+nv+ne+3+i] *= Vol[j-1];
            }
         }
      }
   }
   for ( i=0; i<nv; i++ )                                    // Bulk volume
   {
      if ( part[i] == TRUE )
      {
         y[cell1+(n-1)*cell3+cell2+i] *= Vol[n-2];
      }
      else
      {
         y[cell1+(n-1)*cell3+cell2+i]
            *= Vol[n-2]*y[cell1+(n-1)*cell3+cell2+nv+ne];
      }
   }

   // initialize concentrations of equilibrium state variables:
   // ---------------------------------------------------------

   for ( i=0; i<ne; i++ )
   {
      y[nv+i] = 0.0;               // initialize to zero  // Biofilm base
      for ( j=1; j<=n-2; j++ )                            // Biofilm layer
      {
         y[cell1+(j-1)*cell3+nv+i] = 0.0;
      }
      y[cell1+(n-2)*cell3+nv+i] = 0.0;                    // Biofilm front
      y[cell1+(n-1)*cell3+nv+i] = 0.0;                    // Liquid layer
      y[cell1+(n-1)*cell3+cell2+nv+ns+i] = 0.0;           // Bulk volume

      zoneindex = 1.0;     // biofilm matrix zone
      found = FALSE;
      for ( j=0; j<NumInitCond(); j++ )
      {
         if ( initvar[j]==statevar[nv+ns+i] && initzone[j]==1 )
         {
            k = j; found = TRUE; break;
         }
      }
      if ( found == TRUE )
      {
         varsys->Reset();                                     // Biofilm base
         if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[0]);
         y[nv+i] = initval[k]->Value();
         for ( j=1; j<=n-2; j++ )                             // Biofilm layer
         {
            varsys->Reset();
            if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[j]);
            y[cell1+(j-1)*cell3+nv+i] = initval[k]->Value();
         }
         varsys->Reset();                                     // Biofilm front
         if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[n-1]);
         y[cell1+(n-2)*cell3+nv+i] = initval[k]->Value();
         y[cell1+(n-1)*cell3+nv+i] = y[cell1+(n-2)*cell3+i];
      }                                                       // Liquid layer

      zoneindex = 0.0;    // bulk volume zone
      found = FALSE;
      for ( j=0; j<NumInitCond(); j++ )
      {
         if ( initvar[j]==statevar[nv+ns+i] && initzone[j]==0 )
         {
            k = j; found = TRUE; break;
         }
      }
      if ( found == TRUE )
      {
         varsys->Reset();                                     // Bulk vol.
         if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[n+1]);
         y[cell1+(n-1)*cell3+cell2+nv+ns+i] = initval[k]->Value();
      }
   }

   // initialize concentrations of surface state variables:
   // -----------------------------------------------------

   for ( i=0; i<ns; i++ )
   {
      zoneindex = 0.0;     // bulk volume zone
      found = FALSE;
      for ( j=0; j<NumInitCond(); j++ )
      {
         if ( initvar[j]==statevar[nv+i] && initzone[j]==0 )
         {
            k = j; found = TRUE; break;
         }
      }
      if ( found == TRUE )
      {
         varsys->Reset();                                    // Bulk volume
         if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[n+1]);
         y[cell1+(n-1)*cell3+cell2+nv+i] = initval[k]->Value();
      }
      else                              // initialize with zero
      {
         y[cell1+(n-1)*cell3+cell2+nv+i] = 0.0;             // Bulk volume
      }
   }

   delete [] rh;
   delete [] epsX;
   delete [] Area;
   delete [] Vol;

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


void FILMCOMP::Zequilibrium(REAL LF, REAL** C, REAL* grid0)  // not used
{
   // compiler problem with BORLAND 2.0: cannot declare C as const
   CARDINAL i,j;
   CARDINAL n  = numgrid;
   CARDINAL nv = NumVolStateVar();

   const REAL factglob = 10.0;
   const REAL factloc  =  1.5;
   REAL* a    = new REAL[n-1];
   REAL  amax = 0.0;
   REAL* fmax = new REAL[nv];
   for ( i=0; i<nv; i++ )
   {
      fmax[i] = 0.0;
      for ( j=0; j<n+1; j++ )
      {
         if ( fabs(C[j][i]) > fmax[i] ) fmax[i] = fabs(C[j][i]);
      }
   }
   for ( j=0; j<n-1; j++ )
   {
      a[j] = 0.0;
      REAL dl   = gridpts[j+1]-gridpts[j];
      REAL dr   = gridpts[j+2]-gridpts[j+1];
      REAL dtot = gridpts[j+2]-gridpts[j];
      for ( i=0; i<nv; i++ )
      {
         if ( fmax[i] > 0.0 )
         {
            REAL fzz = 2*(dr*C[j][i]-dtot*C[j+1][i]+dl*C[j+2][i])/(dl*dr*dtot);
            fzz = fabs(fzz)/fmax[i];  // second derivative divided by max value
            if ( fzz > a[j] ) a[j] = fzz;
         }
      }
      if ( a[j] > amax ) amax = a[j];
   }
   if ( amax == 0.0 ) amax = 1.0;
   // correct for global factor:
   for ( j=0; j<n-1; j++ )
   {
      if ( factglob*a[j] < amax ) a[j] = amax/factglob;
   }
   // correct for local factor:
   for ( i=1; i<n-1; i++ )
   {
      for ( j=1; j<n-1; j++ )
      {
         if ( factloc*a[j-1] < a[j] )
         {
            a[j-1] = a[j]/factloc;
         }
         else
         {
            if ( factloc*a[j] < a[j-1] ) a[j] = a[j-1]/factloc;
         }
      }
   }
   // width of cell inverse proportional to above measure of curvature:
   REAL sum = 0.0;
   for ( j=0; j<n-1; j++ ) sum = sum + 1.0/a[j];
   grid0[0] = LF/(a[0]*sum);
   for ( j=1; j<n-1; j++ ) grid0[j] = grid0[j-1]+LF/(a[j]*sum);

   delete [] a;
   delete [] fmax;
   return;
}


REAL FILMCOMP::FluxLimit(REAL theta)
{
   // 0 = first order; 1 = second order

   if ( highres == FALSE ) return 0.0;

   REAL th = theta;
   if ( th > 1.0 ) th = 1.0/th;
   if ( th < 0.25 ) return 0.0;
   if ( th > 0.50 ) return 1.0;
   return 4.0*(th-0.25);
}


BOOLEAN FILMCOMP::Delta(const NUMPAR& numpar, VARSYS* varsys,
                        const REAL* Y, const REAL* YT, REAL* DELTA)
{
   if ( Active() != TRUE ) return FALSE;
   const REAL* y  = &Y[ind_y];
   const REAL* yt = &YT[ind_y];
   REAL* delta = &DELTA[ind_y];
   CARDINAL n  = numgrid;
   CARDINAL nv = NumVolStateVar();
   CARDINAL ns = NumSurfStateVar();
   CARDINAL ne = NumEquStateVar();
   CARDINAL cell1 = nv+ne+1;
   CARDINAL cell2 = nv+ne+1;
   CARDINAL cell3 = nv+ne+3;
   CARDINAL cellb = nv+ne+ns+1;
   if ( porepart == TRUE )
   {
      cell1 += nv+1;
      cell3 += nv+1;
   }
   CARDINAL i,j;

   // initialize global program variables:
   // ------------------------------------

   REAL Q = y[cell1+(n-1)*cell3+cell2+cellb];
   varsys->SetProgVar(Discharge,&Q);
   REAL LF = y[cell1+(n-3)*cell3+nv+ne+1];
   varsys->SetProgVar(FilmDepth,&LF);
   REAL Uattach = y[cell1+(n-2)*cell3+nv+ne+1];
   varsys->SetProgVar(AttachmentVelocity,&Uattach);
   REAL Udetach = y[cell1+(n-2)*cell3+nv+ne+2];
   varsys->SetProgVar(DetachmentVelocity,&Udetach);
   REAL Uinterf = y[cell1+(n-3)*cell3+nv+ne+2]+Uattach-Udetach;
   varsys->SetProgVar(InterfaceVelocity,&Uinterf);
   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   REAL zoneindex = 1.0; varsys->SetProgVar(ZoneIndex,&zoneindex);

   // allocate memory:
   // ----------------

   REAL* Area        = new REAL[n-1];
   REAL* Vol         = new REAL[n-1];
   REAL** concwat    = new REAL*[n+2];
   for ( j=0; j<n+2; j++ ) concwat[j] = new REAL[nv+ns+ne];
   REAL** concmat    = new REAL*[n];
   for ( j=0; j<n; j++ ) concmat[j] = new REAL[nv+ns+ne];
   REAL** diffwat    = new REAL*[n];
   for ( j=0; j<n; j++ ) diffwat[j] = new REAL[nv];
   REAL** diffmat    = new REAL*[n];
   for ( j=0; j<n; j++ ) diffmat[j] = new REAL[nv];
   REAL*    rh       = new REAL[nv];
   REAL*    rateeps  = new REAL[n-2];
   REAL*    rate     = new REAL[nv+ns+ne];
   REAL*    eps      = new REAL[n+2];
   REAL*    por      = new REAL[n+2];
   REAL*    surfkatt = new REAL[nv];
   REAL*    surfkdet = new REAL[nv];
   REAL**   volkatt  = new REAL*[n];
   for ( j=0; j<n; j++ ) volkatt[j] = new REAL[nv];
   REAL**   volkdet  = new REAL*[n];
   for ( j=0; j<n; j++ ) volkdet[j] = new REAL[nv];
   REAL*    resist   = new REAL[nv];
   REAL*    fluxmat  = new REAL[nv];
   REAL*    fluxwat  = new REAL[nv];
   REAL*    baseflux = new REAL[nv];
   REAL*    bulkflux = new REAL[nv];
   REAL ude = 0.0;
   REAL uat = 0.0;

   // initialize grid points, surface and volumes:
   // --------------------------------------------

   CalcGeometry(varsys,y,Area,Vol);
   REAL v = V;
   if ( confined == FALSE )
   {
      v = Vol[n-2];
      for ( j=0; j<n-2; j++ ) v = v + Vol[j];
   }
   if ( Vol[n-2] <= 0.0 )
   {
      *cjob << "\n   FILMCOMP numerical problem:"
            << " Bulk volume becomes negative";
      cjob->flush();
      return FALSE;
   }
   varsys->SetProgVar(Volume,&v);
   varsys->SetProgVar(BulkVolume,&Vol[n-2]);

   // calculate concentrations, rates, diffusivities, attachment,
   // detachment and resistances:
   // -----------------------------------------------------------

   for ( j=0; j<n; j++ )
   {
      for ( i=0; i<nv; i++ )
      {
         volkdet[j][i] = 0.0;
         volkatt[j][i] = 0.0;
      }
   }
   PROGVAR* EPSvar = varsys->GetProgVar(WaterFraction);
   PROGVAR* Zvar   = varsys->GetProgVar(SpaceZ);
   PROGVAR* UFvar  = varsys->GetProgVar(FilmVelocity);
   REAL uF = 0.0; if ( UFvar != 0 ) UFvar->SetProgVar(&uF);

   for ( i=0; i<nv; i++ )      // determine densities
   {
      if ( part[i] == TRUE )
      {
         rh[i] = rho[ind[i]]->Value();
      }
      else
      {
         rh[i] = 0.0;
      }
   }

   zoneindex = 1.0;   // biofilm matrix zone
   CalcConc(0,TRUE,y,Vol,concmat[0],&eps[0],&por[0]);          // Biofilm base
   CalcConc(0,FALSE,y,Vol,concwat[0],&eps[0],&por[0]);
   if ( Zvar != 0 )   Zvar->SetProgVar(&gridpts[0]);
   if ( EPSvar != 0 ) EPSvar->SetProgVar(&eps[0]);
   for ( i=0; i<nv+ns+ne; i++ ) statevar[i]->SetStateVar(&concmat[0][i]);
   varsys->Reset();
   for ( i=0; i<nv; i++ )  // values of variables that depend on matrix conc.
   {
      if ( part[i] == TRUE )
      {
         if ( matrixdiff == TRUE )
         {
            diffmat[0][i] = DXmatrix[ind[i]]->Value();
            if ( diffmat[0][i] <= 0.0 )
            {
               *cjob << "\n   FILMCOMP numerical problem:"
                     << " Particle matrix diffusivity is not positive";
               cjob->flush();
               return FALSE;
            }
         }
         else
         {
            diffmat[0][i] = 0.0;
         }
         if ( porepart == TRUE )
         {
            volkdet[0][i] = volkde[ind[i]]->Value();
         }
      }
   }
   for ( i=0; i<nv+ns+ne; i++ ) statevar[i]->SetStateVar(&concwat[0][i]);
   varsys->Reset();
   for ( i=0; i<nv; i++ )  // values of variables that depend on pore conc.
   {
      if ( part[i] == TRUE )
      {
         if ( ind[i]>=0 && porepart==TRUE )
         {
            diffwat[0][i] = DXpore[ind[i]]->Value();
            if ( diffwat[0][i] <= 0.0 )
            {
               *cjob << "\n   FILMCOMP numerical problem:"
                     << " Particle pore diffusivity is not positive";
               cjob->flush();
               return FALSE;
            }
         }
         else
         {
            diffwat[0][i] = 0.0;
         }
         if ( porepart == TRUE )
         {
            volkatt[0][i] = volkat[ind[i]]->Value();
         }
      }
      else
      {
         if ( ind[i] >= 0 )
         {
            diffwat[0][i] = DSpore[ind[i]]->Value();
         }
         else
         {
            diffwat[0][i] = 0.0;
         }
         if ( diffwat[0][i] <= 0.0 )
         {
            *cjob << "\n   FILMCOMP numerical problem:"
                  << " Dissolved diffusivity is not positive";
            cjob->flush();
            return FALSE;
         }
      }
   }

   for ( j=1; j<=n-2; j++ )                                   // Biofilm layer
   {
      CalcConc(j,TRUE,y,Vol,concmat[j],&eps[j],&por[j]);
      CalcConc(j,FALSE,y,Vol,concwat[j],&eps[j],&por[j]);
      if ( Zvar != 0 )   Zvar->SetProgVar(&gridpts[j]);
      if ( EPSvar != 0 ) EPSvar->SetProgVar(&eps[j]);
      if ( j == 1 )
      {
         uF = 0.5*y[cell1+nv+ne+2];
      }
      else
      {
         uF = 0.5*(y[cell1+(j-2)*cell3+nv+ne+2]
                  +y[cell1+(j-1)*cell3+nv+ne+2]);
      }
      for ( i=0; i<nv+ns+ne; i++ ) statevar[i]->SetStateVar(&concmat[j][i]);
      varsys->Reset();
      for ( i=0; i<nv; i++ )  // values of variables that depend on matrix conc.
      {
         if ( part[i] == TRUE )
         {
            if ( matrixdiff == TRUE )
            {
               diffmat[j][i] = DXmatrix[ind[i]]->Value();
               if ( diffmat[j][i] <= 0.0 )
               {
                  *cjob << "\n   FILMCOMP numerical problem:"
                        << " Particle matrix diffusivity is not positive";
                  cjob->flush();
                  return FALSE;
               }
            }
            else
            {
               diffmat[j][i] = 0.0;
            }
            if ( porepart == TRUE )
            {
               volkdet[j][i] = volkde[ind[i]]->Value();
               volkatt[j][i] = volkat[ind[i]]->Value();
            }
         }
      }
      CalcRates(rate);
      for ( i=0; i<nv; i++ )
      {
         delta[cell1+(j-1)*cell3+i] = rate[i]*Vol[j-1];
      }
      for ( i=0; i<ne; i++ )
      {
         delta[cell1+(j-1)*cell3+nv+i] = rate[nv+ns+i];
      }
      if ( reps != 0 ) rateeps[j-1] = reps->Value()*Vol[j-1];
      else             rateeps[j-1] = 0.0;
      for ( i=0; i<nv+ns+ne; i++ ) statevar[i]->SetStateVar(&concwat[j][i]);
      varsys->Reset();
      for ( i=0; i<nv; i++ )  // values of variables that depend on pore conc.
      {
         if ( part[i] == TRUE )
         {
            if ( ind[i]>=0 && porepart==TRUE )
            {
               diffwat[j][i] = DXpore[ind[i]]->Value();
               if ( diffwat[j][i] <= 0.0 )
               {
                  *cjob << "\n   FILMCOMP numerical problem:"
                        << " Particle pore diffusivity is not positive";
                  cjob->flush();
                  return FALSE;
               }
            }
            else
            {
               diffwat[j][i] = 0.0;
            }
/* volkatt depends also on film matrix and not on pore water properties
            if ( porepart == TRUE )
            {
               volkatt[j][i] = volkat[ind[i]]->Value();
            }
*/
         }
         else
         {
            if ( ind[i] >= 0 )
            {
               diffwat[j][i] = DSpore[ind[i]]->Value();
               if ( diffwat[j][i] <= 0.0 )
               {
                  *cjob << "\n   FILMCOMP numerical problem:"
                        << " Dissolved diffusivity is not positive";
                  cjob->flush();
                  return FALSE;
               }
            }
            else
            {
               diffwat[j][i] = 0.0;
            }
         }
      }
      if ( porepart == TRUE )
      {
         CalcRates(rate);
         for ( i=0; i<nv; i++ )
         {
            if ( part[i] == TRUE )
            {
               delta[cell1+(j-1)*cell3+nv+ne+3+i] = rate[i]*Vol[j-1];
            }
            else
            {
               delta[cell1+(j-1)*cell3+i] += rate[i]*Vol[j-1];
            }
         }
      }
   }

   CalcConc(n-1,TRUE,y,Vol,concmat[n-1],&eps[n-1],&por[n-1]); // Biofilm front
   CalcConc(n-1,FALSE,y,Vol,concwat[n-1],&eps[n-1],&por[n-1]);
   if ( Zvar != 0 )   Zvar->SetProgVar(&gridpts[n-1]);
   if ( EPSvar != 0 ) EPSvar->SetProgVar(&eps[n-1]);
   uF = y[cell1+(n-3)*cell3+nv+ne+2];
   for ( i=0; i<nv+ns+ne; i++ ) statevar[i]->SetStateVar(&concmat[n-1][i]);
   varsys->Reset();
   for ( i=0; i<nv; i++ )  // values of variables that depend on matrix conc.
   {
      if ( part[i] == TRUE )
      {
         if ( matrixdiff == TRUE )
         {
            diffmat[n-1][i] = DXmatrix[ind[i]]->Value();
            if ( diffmat[n-1][i] <= 0.0 )
            {
               *cjob << "\n   FILMCOMP numerical problem:"
                     << " Particle matrix diffusivity is not positive";
               cjob->flush();
               return FALSE;
            }
         }
         else
         {
            diffmat[n-1][i] = 0.0;
         }
         if ( porepart == TRUE )
         {
            volkdet[j][i] = volkde[ind[i]]->Value();
         }
      }
   }
   if ( detachglobal == TRUE )
   {
      ude = udet->Value();
      if ( ude < 0.0 )
      {
         *cjob << "\n   FILMCOMP numerical problem:"
               << " Detachment velocity is negative";
         cjob->flush();
         return FALSE;
      }
      for ( i=0; i<nv; i++ ) surfkdet[i] = ude;
   }
   else
   {
      for ( i=0; i<nv; i++ )
      {
         if ( part[i] == TRUE )
         {
            surfkdet[i] = surfkde[ind[i]]->Value();
            if ( surfkdet[i] < 0.0 )
            {
               *cjob << "\n   FILMCOMP numerical problem:"
                     << " Detachment coefficient is negative";
               cjob->flush();
               return FALSE;
            }
            ude += surfkdet[i]*concmat[n-1][i]/rh[i];
         }
         else
         {
            surfkdet[i] = 0.0;
         }
      }
      ude = ude/(1.0-eps[n-2]); // eps[n-2] (integral of cell) gives more
                                // stable results than the surface value
   }
   for ( i=0; i<nv+ns+ne; i++ ) statevar[i]->SetStateVar(&concmat[n-1][i]);
   varsys->Reset();
   for ( i=0; i<nv; i++ )  // values of variables that depend on pore conc.
   {
      if ( part[i] == TRUE )
      {
         if ( ind[i]>=0 && porepart==TRUE )
         {
            diffwat[n-1][i] = DXpore[ind[i]]->Value();
            if ( diffwat[n-1][i] <= 0.0 )
            {
               *cjob << "\n   FILMCOMP numerical problem:"
                     << " Particle pore diffusivity is not positive";
               cjob->flush();
               return FALSE;
            }
         }
         else
         {
            diffwat[n-1][i] = 0.0;
         }
         if ( porepart == TRUE )
         {
            volkatt[j][i] = volkat[ind[i]]->Value();
         }
      }
      else
      {
         if ( ind[i] >= 0 )
         {
            diffwat[n-1][i] = DSpore[ind[i]]->Value();
            if ( diffwat[n-1][i] <= 0.0 )
            {
               *cjob << "\n   FILMCOMP numerical problem:"
                     << " Dissolved diffusivity is not positive";
               cjob->flush();
               return FALSE;
            }
         }
         else
         {
            diffwat[n-1][i] = 0.0;
         }
      }
   }

   CalcConc(n,FALSE,y,Vol,concwat[n],&eps[n],&por[n]);         // Liquid layer
   for ( i=0; i<nv+ns+ne; i++ ) statevar[i]->SetStateVar(&concwat[n][i]);
   varsys->Reset();
   for ( i=0; i<nv; i++ )
   {
      if ( part[i] == TRUE )
      {
         surfkatt[i] = surfkat[ind[i]]->Value();
         uat += surfkatt[i]*concwat[n][i]/rh[i];
      }
   }
   uat = uat/(1.0-eps[n-2]);  // eps[n-2] (integral of cell) gives more
                              // stable results than the surface value

   for ( i=0; i<nv; i++ )
   {
      if ( part[i] == TRUE )
      {
         resist[i] = resX[ind[i]]->Value();
      }
      else
      {
         if ( ind[i] >= 0 )
         {
            resist[i] = resS[ind[i]]->Value();
         }
         else
         {
            resist[i] = 0.0;
         }
      }
   }

   zoneindex = 0.0;    // bulk volume zone
   CalcConc(n+1,FALSE,y,Vol,concwat[n+1],&eps[n+1],&por[n+1]);   // Bulk volume
   for ( i=0; i<nv+ns+ne; i++ ) statevar[i]->SetStateVar(&concwat[n+1][i]);
   varsys->Reset();
   if ( Zvar != 0 )   Zvar->SetProgVar(&gridpts[n+1]);
   if ( EPSvar != 0 ) EPSvar->SetProgVar(&eps[n+1]);
   CalcRates(rate);
   for ( i=0; i<nv; i++ )
   {
      delta[cell1+(n-1)*cell3+cell2+i] = rate[i]*Vol[n-2];
   }
   for ( i=0; i<ns; i++ )
   {
      delta[cell1+(n-1)*cell3+cell2+nv+i] = rate[nv+i];
   }
   for ( i=0; i<ne; i++ )
   {
      delta[cell1+(n-1)*cell3+cell2+nv+ns+i] = rate[nv+ns+i];
   }

   // calculate input fluxes:
   // -----------------------

   for ( i=0; i<nv; i++ )
   {
      baseflux[i] = 0;
      bulkflux[i] = 0;
   }

   Q = 0;
   const CARDINAL advinconn = 0;                      // input from advinlink
   for ( j=0; j<advinnum[advinconn]; j++ )
   {
      Q += advinlink[advinconn][j]
               ->AdvOutQ(varsys,Y,advinindex[advinconn][j]);
   }
   for ( i=0; i<nv; i++ )
   {
      for ( j=0; j<advinnum[advinconn]; j++ )
      {
         bulkflux[i] += advinlink[advinconn][j]
                     ->AdvOutJ(varsys,Y,advinindex[advinconn][j],statevar[i]);
      }
   }

   varsys->SetProgVar(Discharge,&Q);                 // input from AQCOMP
   REAL* C = new REAL[nv];
   if ( Q > 0.0 )
   {
      for ( i=0; i<nv; i++ ) C[i] = bulkflux[i]/Q;
   }
   else
   {
      for ( i=0; i<nv; i++ ) C[i] = 0.0;
   }
   for ( i=0; i<nv; i++ )
   {
      statevar[i]->SetStateVar(&C[i]);
   }
   if ( inflow != 0 ) Q += inflow->Value();
   for ( i=0; i<nv; i++ )
   {
      for ( j=0; j<numinput; j++ )
      {
         if ( inputvar[j] == statevar[i] )
         {
            bulkflux[i] += inputflux[j]->Value(); break;
         }
      }
   }
   delete [] C;

   const CARDINAL bulkdiffconn = 0;               // input from bulkdifflink
   for ( i=0; i<nv; i++ )
   {
      for ( j=0; j<diffnum[bulkdiffconn]; j++ )
      {
         bulkflux[i] += difflink[bulkdiffconn][j]
                       ->DiffJ(Y,diffindex[bulkdiffconn][j],statevar[i]);
      }
   }

   const CARDINAL basediffconn = 1;               // input from basedifflink
   for ( i=0; i<nv; i++ )
   {
      for ( j=0; j<diffnum[basediffconn]; j++ )
      {
         baseflux[i] += difflink[basediffconn][j]
                       ->DiffJ(Y,diffindex[basediffconn][j],statevar[i]);
      }
   }

   // biofilm base equations:
   // -----------------------

   // equations for dynamic volume state variables:

   for ( i=0; i<nv; i++ )
   {
      if ( part[i] == TRUE )
      {
         fluxwat[i]  = 0.0;
         fluxmat[i]  = 0.0;
         // boundary value equal to value in first cell:
         // delta[i] = concmat[0][i] - concmat[1][i];
         // boundary value determined by linear extrapolation:
         delta[i] = concmat[0][i] - ( concmat[1][i]*(gridpts[2]-gridpts[0])
                                     +concmat[2][i]*(gridpts[0]-gridpts[1]) )
                                    /(gridpts[2]-gridpts[1]);
      }
      else
      {
         fluxwat[i] = -Area[0]*diffwat[0][i]*eps[0]
                       *(concwat[1][i]-concwat[0][i])/(gridpts[1]-gridpts[0]);
         fluxmat[i] = 0.0;
         if ( diffwat[0][i] > 0.0 )
         {
            delta[i] = fluxwat[i] - baseflux[i];
         }
         else     
         {
            delta[i] = concwat[0][i] - concwat[1][i];
         }
      }
      if ( porepart == TRUE )
      {
         if ( part[i] == TRUE )
         {
             delta[nv+ne+1+i] = concwat[0][i] - concwat[1][i];
         }
         else
         {
             delta[nv+ne+1+i] = y[nv+ne+1+i];   // not used: iterate to zero
         }
      }
   }
   
   // equations for equilibrium state variables:

   ;

   // equation for eps:

   delta[nv+ne]
      = 1.0 - y[nv+ne];
   for ( i=0; i<nv; i++ )
   {
      if ( part[i] == TRUE )
      {
         delta[nv+ne] -= concmat[0][i]/rh[i];
      }
   }
   if ( porepart == TRUE )
   {
      for ( i=0; i<nv; i++ )
      {
         if ( part[i] == TRUE )
         {
            delta[nv+ne] -= concwat[0][i]/rh[i];
         }
      }
   }

   // equation for por:

   if ( porepart == TRUE )
   {
      delta[nv+ne+1+nv] = 1.0 - y[nv+ne+1+nv];
      for ( i=0; i<nv; i++ )
      {
         if ( part[i] == TRUE )
         {
            delta[nv+ne+1+nv] -= concmat[0][i]/rh[i];
         }
      }
   }

   // biofilm layer equations:
   // ------------------------

   for ( j=1; j<=n-2; j++ )
   {
      REAL ugrid = Uinterf*y[cell1+(j-1)*cell3+nv+ne+1]/LF;
      REAL ufilm = y[cell1+(j-1)*cell3+nv+ne+2];
      INTEGER jpart = 0; if (  ufilm-ugrid < 0.0 ) jpart = 1;
      INTEGER jdiss = 0; if ( -ufilm-ugrid < 0.0 ) jdiss = 1;
      REAL wl = (gridpts[j+1]-y[cell1+(j-1)*cell3+nv+ne+1])
                /(gridpts[j+1]-gridpts[j]);
      REAL wr = (y[cell1+(j-1)*cell3+nv+ne+1]-gridpts[j])
                /(gridpts[j+1]-gridpts[j]);

      // equation for grid points:

      delta[cell1+(j-1)*cell3+nv+ne+1]
         = ugrid
           + numpar.GridTimeConst()*0.0            // not yet implemented !!!
           - yt[cell1+(j-1)*cell3+nv+ne+1];

      // equation for uF (before state variables to access rates):

      REAL sum = 0.0;
      for ( i=0; i<nv; i++ )
      {
         if ( part[i] == TRUE )
         {
            sum += delta[cell1+(j-1)*cell3+i]/rh[i];
         }
      }
      sum = sum/(1.0-por[j]);
      if ( reps != 0 ) sum += rateeps[j-1];
      uF = 0.0;
      if ( j > 1 ) uF = y[cell1+(j-2)*cell3+nv+ne+2];
      uF = (uF*Area[j-1]+sum)/Area[j];
      delta[cell1+(j-1)*cell3+nv+ne+2]
         = uF - y[cell1+(j-1)*cell3+nv+ne+2];

      // equations for dynamic volume state variables:

      for ( i=0; i<nv; i++ )
      {
         // volume attachment and detachment:
         if ( porepart == TRUE )
         {
            if ( part[i] == TRUE )
            {
               delta[cell1+(j-1)*cell3+nv+ne+3+i]
                  -= (volkatt[j][i]*concwat[j][i]
                     -volkdet[j][i]*concmat[j][i])*Vol[j-1];
               delta[cell1+(j-1)*cell3+i]
                  += (volkatt[j][i]*concwat[j][i]
                     -volkdet[j][i]*concmat[j][i])*Vol[j-1];
            }
         }

         // add flux at left boundary of cell:
         if ( part[i] == TRUE )
         {
            delta[cell1+(j-1)*cell3+i] += fluxmat[i];
            if ( porepart == TRUE )
            {
               // delta[cell1+(j-1)*cell3+nv+ne+1+i] += fluxwat[i];
               // corrected 8.8.97:
               delta[cell1+(j-1)*cell3+nv+ne+3+i] += fluxwat[i];
            }
         }
         else
         {
            delta[cell1+(j-1)*cell3+i] += fluxwat[i];
         }

         // calculate flux at right boundary of cell:
         if ( j < n-2 )
         {
            REAL diffmatflux, advmatflux1, advmatflux2;
            REAL diffwatflux, advwatflux1, advwatflux2;
            if ( part[i] == TRUE )
            {
               if ( matrixdiff == TRUE )
               {
                  diffmatflux = -(wl*diffmat[j][i]+wr*diffmat[j+1][i])
                                 *(concmat[j+1][i]-concmat[j][i])
                                 /(gridpts[j+1]-gridpts[j]);
               }
               else
               {
                  diffmatflux = 0.0;
               }
               advmatflux1 = (ufilm-ugrid)*concmat[j+jpart][i];
               advmatflux2 = (ufilm-ugrid)
                             *(wl*concmat[j][i]+wr*concmat[j+1][i]);
               if ( porepart == TRUE )
               {
                  // diffwatflux = -(wl*diffwat[j][i]+wr*diffwat[j+1][i])
                  //                *(concwat[j+1][i]-concwat[j][i])
                  //                /(gridpts[j+1]-gridpts[j]);
                  diffwatflux = -(wl*diffwat[j][i]+wr*diffwat[j+1][i])
                                *(wl*por[j]+wr*por[j+1])
                                 *(concwat[j+1][i]/por[j+1]
                                                   -concwat[j][i]/por[j])
                                 /(gridpts[j+1]-gridpts[j]);
                  advwatflux1 = (-ufilm-ugrid)
                                *(1.0-por[j+jdiss])*concwat[j+jdiss][i];
                  advwatflux2 = (-ufilm-ugrid)
                                *(wl*concwat[j][i]*(1.0-por[j])
                                 +wr*concwat[j+1][i]*(1.0-por[j+1]));
               }
            }
            else
            {
               diffwatflux = -(wl*diffwat[j][i]+wr*diffwat[j+1][i])
                              *(wl*eps[j]+wr*eps[j+1])
                              *(concwat[j+1][i]-concwat[j][i])
                              /(gridpts[j+1]-gridpts[j]);
               advwatflux1 = (-ufilm-ugrid)
                             *(1.0-eps[j+jdiss])*concwat[j+jdiss][i];
               advwatflux2 = (-ufilm-ugrid)
                             *(wl*concwat[j][i]*(1.0-eps[j])
                              +wr*concwat[j+1][i]*(1.0-eps[j+1]));
            }
            if ( part[i] == TRUE )
            {
               fluxmat[i] = Area[j]*(diffmatflux
                                    +1.0*advmatflux1+0.0*advmatflux2);
               if ( porepart == TRUE )
               {
                  fluxwat[i] = Area[j]*(diffwatflux
                                       +1.0*advwatflux1+0.0*advwatflux2);
               }
            }
            else
            {
               fluxwat[i] = Area[j]*(diffwatflux
                                    +1.0*advwatflux1+0.0*advwatflux2);
            }
         }
         else          // last cell
         {
            REAL diffmatflux, advmatflux;
            REAL diffwatflux, advwatflux;
            if ( part[i] == TRUE )
            {
               if ( matrixdiff == TRUE )
               {
                  diffmatflux = -diffmat[n-1][i]
                                 *(concmat[n-1][i]-concmat[n-2][i])
                                 /(gridpts[n-1]-gridpts[n-2]);
               }
               else
               {
                  diffmatflux = 0.0;
               }
               if ( uat > ude )
               {
                  advmatflux = -(uat-ude)*concmat[n-1][i]; // use new uat, ude
               }
               else
               {
                  advmatflux = (ufilm-Uinterf)*concmat[n-2][i]; // Uinterf,ufilm
               }                                                // from last step
               if ( porepart == TRUE )
               {
                  // diffwatflux = -diffwat[n-1][i]
                  //                *(concwat[n-1][i]-concwat[n-2][i])
                  //                /(gridpts[n-1]-gridpts[n-2]);
                  diffwatflux = -diffwat[n-1][i]*por[n-1]
                                 *(concwat[n-1][i]/por[n-1]
                                           -concwat[n-2][i]/por[n-2])
                                 /(gridpts[n-1]-gridpts[n-2]);
                  advwatflux  = (-ufilm+por[n-1]*(ufilm-Uinterf))
                                *concwat[n-1][i];
               }
            }
            else
            {
               diffwatflux = -diffwat[n-1][i]*eps[n-1]
                              *(concwat[n-1][i]-concwat[n-2][i])
                              /(gridpts[n-1]-gridpts[n-2]);
               advwatflux  = (-ufilm+eps[n-1]*(ufilm-Uinterf))
                             *concwat[n-1][i];
            }
            if ( part[i] == TRUE )
            {
               fluxmat[i] = Area[j]*(diffmatflux+advmatflux);
               if ( porepart == TRUE )
               {
                  fluxwat[i] = Area[j]*(diffwatflux+advwatflux);
               }
            }
            else
            {
               fluxwat[i] = Area[j]*(diffwatflux+advwatflux);
            }
         }

         // subtract flux at right boundary of cell:
         if ( part[i] == TRUE )
         {
            delta[cell1+(j-1)*cell3+i] -= fluxmat[i];
            if ( porepart == TRUE )
            {
               delta[cell1+(j-1)*cell3+nv+ne+3+i] -= fluxwat[i];
            }
         }
         else
         {
            delta[cell1+(j-1)*cell3+i] -= fluxwat[i];
            if ( porepart == TRUE )
            {
               delta[cell1+(j-1)*cell3+nv+ne+3+i] = 0.0; // not used; rate zero
            }
         }

         // complete differential equation:
         delta[cell1+(j-1)*cell3+i]
            -= yt[cell1+(j-1)*cell3+i];
         if ( porepart == TRUE )
         {
            delta[cell1+(j-1)*cell3+nv+ne+3+i]
               -= yt[cell1+(j-1)*cell3+nv+ne+3+i];
         }
      }

      // equations for equilibrium state variables:

      ;

      // equation for eps:

      delta[cell1+(j-1)*cell3+nv+ne]
         = 1.0 - y[cell1+(j-1)*cell3+nv+ne];
      for ( i=0; i<nv; i++ )
      {
         if ( part[i] == TRUE )
         {
            delta[cell1+(j-1)*cell3+nv+ne] -= concmat[j][i]/rh[i];
         }
      }
      if ( porepart == TRUE )
      {
         for ( i=0; i<nv; i++ )
         {
            if ( part[i] == TRUE )
            {
               delta[cell1+(j-1)*cell3+nv+ne] -= concwat[j][i]/rh[i];
            }
         }
      }

      // equation for por:

      if ( porepart == TRUE )
      {
         delta[cell1+(j-1)*cell3+nv+ne+3+nv]
            = 1.0 - y[cell1+(j-1)*cell3+nv+ne+3+nv];
         for ( i=0; i<nv; i++ )
         {
            if ( part[i] == TRUE )
            {
               delta[cell1+(j-1)*cell3+nv+ne+3+nv] -= concmat[j][i]/rh[i];
            }
         }
      }
   }

   // biofilm front equations:
   // ------------------------

   // equations for dynamic volume state variables:

   // relaxation time for particulate bound. cond.
   REAL krelax = y[cell1+(n-3)*cell3+nv+ne+2]/LF;
   if ( uat/LF > krelax ) krelax = uat/LF;
   if ( ude/LF > krelax ) krelax = ude/LF;
   if ( krelax <= 0.0) krelax = 1.0;
   krelax = 100.0*krelax;

   for ( i=0; i<nv; i++ )
   {
      if ( part[i] == TRUE )
      {
         if ( matrixdiff == TRUE )   // algebraic equation
         {
            delta[cell1+(n-2)*cell3+i]
               = fluxmat[i]
                 - Area[n-2]*(surfkdet[i]*concmat[n-1][i]
                             -surfkatt[i]*concwat[n][i]);
         }
         else                      // relaxation to correct solution
         {
            if ( uat>ude && uat>1.0e-10 )
            {
               delta[cell1+(n-2)*cell3+i]
                  = ( surfkatt[i]/uat*concwat[n][i] - concmat[n-1][i]) * krelax
                    - yt[cell1+(n-2)*cell3+i];
            }
            else
            {
               REAL csurf = ( concmat[n-3][i]*(gridpts[n-2]-gridpts[n-1])
                             +concmat[n-2][i]*(gridpts[n-1]-gridpts[n-3]) )
                            /(gridpts[n-2]-gridpts[n-3]);
               if ( csurf < 0.0 ) csurf = 0.0;
               delta[cell1+(n-2)*cell3+i]
                  = (csurf - concmat[n-1][i]) * krelax 
                    - yt[cell1+(n-2)*cell3+i];

            }
         }
      }
      else
      {
         delta[cell1+(n-2)*cell3+i] = concwat[n-1][i]-concwat[n][i];
      }
      if ( porepart == TRUE )
      {
         if ( part[i] == TRUE )
         {
            delta[cell1+(n-2)*cell3+nv+ne+3+i]
               = concwat[n-1][i]-concwat[n][i]*por[n-1];
         }
         else
         {
            delta[cell1+(n-2)*cell3+nv+ne+3+i]
               = y[cell1+(n-2)*cell3+nv+ne+3+i]; // not used, iterate to zero
         }
      }
   }

   // equations for equilibrium state variables:

   ;

   // equation for eps:

   delta[cell1+(n-2)*cell3+nv+ne]
      = 1.0 - y[cell1+(n-2)*cell3+nv+ne];
   for ( i=0; i<nv; i++ )
   {
      if ( part[i] == TRUE )
      {
         delta[cell1+(n-2)*cell3+nv+ne] -= concmat[n-1][i]/rh[i];
      }
   }
   if ( porepart == TRUE )
   {
      for ( i=0; i<nv; i++ )
      {
         if ( part[i] == TRUE )
         {
            delta[cell1+(n-2)*cell3+nv+ne] -= concwat[n-1][i]/rh[i];
         }
      }
   }

   // equation for por:

   if ( porepart == TRUE )
   {
      delta[cell1+(n-2)*cell3+nv+ne+3+nv]
         = 1.0 - y[cell1+(n-2)*cell3+nv+ne+3+nv];
      for ( i=0; i<nv; i++ )
      {
         if ( part[i] == TRUE )
         {
            delta[cell1+(n-2)*cell3+nv+ne+3+nv] -= concmat[n-1][i]/rh[i];
         }
      }
   }

   // equation for attachment velocity:

   delta[cell1+(n-2)*cell3+nv+ne+1]
      = y[cell1+(n-2)*cell3+nv+ne+1]-uat;

   // equation for detachment velocity:

   delta[cell1+(n-2)*cell3+nv+ne+2]
      = y[cell1+(n-2)*cell3+nv+ne+2]-ude;

   // boundary layer equations:
   // -------------------------

   // equations for dynamic volume state variables:

   for ( i=0; i<nv; i++ )
   {
      delta[cell1+(n-1)*cell3+i]
         = (fluxwat[i]+fluxmat[i])*resist[i]
           - Area[n-2]*(concwat[n][i]-concwat[n+1][i]);
   }

   // equations for equilibrium state variables:

   ;

   // equation for eps:

   delta[cell1+(n-1)*cell3+nv+ne]
      = 1.0 - y[cell1+(n-1)*cell3+nv+ne];
   for ( i=0; i<nv; i++ )
   {
      if ( part[i] == TRUE )
      {
         delta[cell1+(n-1)*cell3+nv+ne] -= concwat[n][i]/rh[i];
      }
   }


   // bulk volume equations:
   // ----------------------

   // equations for dynamic volume state variables:

   for ( i=0; i<nv; i++ )
   {
      if ( part[i] == TRUE )
      {
         delta[cell1+(n-1)*cell3+cell2+i]
            += bulkflux[i]                                     // inflow,diff
               - y[cell1+(n-1)*cell3+cell2+cellb+1] // outflow
                 *concwat[n+1][i]
               + fluxwat[i] + fluxmat[i]                       // biofilm
               - yt[cell1+(n-1)*cell3+cell2+i];
      }
      else
      {
         delta[cell1+(n-1)*cell3+cell2+i]
            += bulkflux[i]                                     // inflow,diff
               - y[cell1+(n-1)*cell3+cell2+cellb+1]            // outflow
                 *eps[n+1]*concwat[n+1][i]
               + fluxwat[i]                                    // biofilm
               - yt[cell1+(n-1)*cell3+cell2+i];
      }
   }

   // equations for dynamic surface state variables:

   for ( i=0; i<ns; i++ )
   {
      delta[cell1+(n-1)*cell3+cell2+nv+i]
         -= yt[cell1+(n-1)*cell3+cell2+nv+i];
   }

   // equations for equilibrium state variables:

   ;

   // equation for eps:

   delta[cell1+(n-1)*cell3+cell2+nv+ne]
      = 1.0 - y[cell1+(n-1)*cell3+cell2+nv+ne];
   for ( i=0; i<nv; i++ )
   {
      if ( part[i] == TRUE )
      {
         delta[cell1+(n-1)*cell3+cell2+nv+ne] -= concwat[n+1][i]/rh[i];
      }
   }

   // additional equations:
   // ---------------------

   // equation for inflow:

   delta[cell1+(n-1)*cell3+cell2+cellb]
      = y[cell1+(n-1)*cell3+cell2+cellb]-Q;

   // equation for outflow:

   delta[cell1+(n-1)*cell3+cell2+cellb+1]
      = y[cell1+(n-1)*cell3+cell2+cellb+1]-Q;
   if ( confined == FALSE )
   {
      REAL Qef = Q - Area[n-2]*Uinterf;
      if ( Qef < 0.0 )
      {
         *cjob << "\n   FILMCOMP numerical problem:"
               << " Effluent discharge becomes negative";
         cjob->flush();
         return FALSE;
      }
      else
      {
         delta[cell1+(n-1)*cell3+cell2+cellb+1]
            = y[cell1+(n-1)*cell3+cell2+cellb+1]-Qef;
      }
   }

   // equation for bulk volume:

   delta[cell1+(n-1)*cell3+cell2+cellb+2]
      = y[cell1+(n-1)*cell3+cell2+cellb+2] - Vol[n-2];

   // deallocate memory:
   // ------------------

   delete [] Area;
   delete [] Vol;
   for ( j=0; j<n+2; j++ ) delete [] concwat[j];
   delete [] concwat;
   for ( j=0; j<n; j++ ) delete [] concmat[j];
   delete [] concmat;
   for ( j=0; j<n; j++ ) delete [] diffwat[j];
   delete [] diffwat;
   for ( j=0; j<n; j++ ) delete [] diffmat[j];
   delete [] diffmat;
   delete [] rh;
   delete [] rateeps;
   delete [] rate;
   delete [] eps;
   delete [] por;
   delete [] surfkatt;
   delete [] surfkdet;
   for ( j=0; j<n; j++ ) delete [] volkatt[j];
   delete [] volkatt;
   for ( j=0; j<n; j++ ) delete [] volkdet[j];
   delete [] volkdet;
   delete [] resist;
   delete [] fluxmat;
   delete [] fluxwat;
   delete [] baseflux;
   delete [] bulkflux;

/*
cjob->precision(14);
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
/*
cout.precision(14);
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


BOOLEAN FILMCOMP::GridValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                            REAL* t_ptr, const REAL* Y,
                            CARDINAL zone, CARDINAL pt, AQVAR* var,
                            REAL& value)
{
   if ( Active() != TRUE ) return FALSE;
   SetGlobalState(varsys,calcnum_ptr,t_ptr,Y);
   if ( pt >= NumGridPts() )          return FALSE;
   if ( varsys->Exist(var) == FALSE ) return FALSE;
   if ( AllowedVar(var) == FALSE )    return FALSE;
   if ( Active() == FALSE )           return FALSE;
   const REAL* y = &Y[ind_y];
   REAL cn = *calcnum_ptr; varsys->SetProgVar(CalcNumber,&cn);
   CARDINAL n  = numgrid;
   CARDINAL nv = NumVolStateVar();
   CARDINAL ns = NumSurfStateVar();
   CARDINAL ne = NumEquStateVar();
   CARDINAL cell1 = nv+ne+1;
   CARDINAL cell2 = nv+ne+1;
   CARDINAL cell3 = nv+ne+3;
   CARDINAL cellb = nv+ne+ns+1;
   if ( porepart == TRUE )
   {
      cell1 += nv+1;
      cell3 += nv+1;
   }
   CARDINAL i;

   // initialize global program variables:
   // ------------------------------------

   varsys->SetProgVar(Time,t_ptr);
   REAL Q = y[cell1+(n-1)*cell3+cell2+cellb];
   varsys->SetProgVar(Discharge,&Q);
   REAL LF = y[cell1+(n-3)*cell3+nv+ne+1];
   varsys->SetProgVar(FilmDepth,&LF);
   REAL Uattach = y[cell1+(n-2)*cell3+nv+ne+1];
   varsys->SetProgVar(AttachmentVelocity,&Uattach);
   REAL Udetach = y[cell1+(n-2)*cell3+nv+ne+2];
   varsys->SetProgVar(DetachmentVelocity,&Udetach);
   REAL Uinterf = y[cell1+(n-3)*cell3+nv+ne+2]+Uattach-Udetach;
   varsys->SetProgVar(InterfaceVelocity,&Uinterf);
   REAL compindex = (REAL)cind;
   varsys->SetProgVar(CompIndex,&compindex);
   REAL zoneindex = 1.0;
   varsys->SetProgVar(ZoneIndex,&zoneindex);

   // initialize grid points, surface and volumes:
   // --------------------------------------------

   REAL* Area = new REAL[n-1];   
   REAL* Vol  = new REAL[n-1];
   CalcGeometry(varsys,y,Area,Vol);
   REAL v = V;
   if ( confined == FALSE )
   {
      v = Vol[n-2];
      for ( i=0; i<n-2; i++ ) v = v + Vol[i];
   }
   varsys->SetProgVar(Volume,&v);
   varsys->SetProgVar(BulkVolume,&Vol[n-2]);

   // initialize concentrations:
   // --------------------------

   CARDINAL lp = pt;
   BOOLEAN bfmatrix = FALSE;
   switch ( zone )
   {
      case 0:              // bulk volume zone
         lp = n+1;
         zoneindex = 0.0;
         break;
      case 1:              // biofilm matrix zone
         if ( lp > NumZoneGridPts(zone)+1 )  // allow for liq. layer and bulk
         {
            delete [] Area;
            delete [] Vol;
            return FALSE;
         }
         zoneindex = 1.0;
         bfmatrix = TRUE;
         break;
      case 2:              // biofilm pore zone
         if ( lp > NumZoneGridPts(zone)+1 )  // allow for liq. layer and bulk
         {
            delete [] Area;
            delete [] Vol;
            return FALSE;
         }
         zoneindex = 2.0;
         break;
      default:
         return FALSE;
   }
   varsys->SetProgVar(SpaceZ,&gridpts[lp]);
   REAL* conc = new REAL[nv+ns+ne];
   REAL eps;
   REAL por;
   CalcConc(lp,bfmatrix,y,Vol,conc,&eps,&por);
   REAL uF = 0.0;
   if ( lp == 1 ) uF = 0.5*y[cell1+nv+ne+2];
   if ( 1<lp && lp<=n-2 )
   {
      uF = 0.5*(y[cell1+(lp-2)*cell3+nv+ne+2]
               +y[cell1+(lp-1)*cell3+nv+ne+2]);
   }
   if ( lp >= n-1 ) uF = y[cell1+(n-3)*cell3+nv+ne+2];

   for ( i=0; i<nv+ns+ne; i++ ) statevar[i]->SetStateVar(&conc[i]);
   varsys->SetProgVar(WaterFraction,&eps);
   varsys->SetProgVar(FilmVelocity,&uF);

   varsys->Reset();
   value = var->Value();

   for ( i=0; i<nv+ns+ne; i++ ) statevar[i]->ResetStateVar();
   delete [] conc;
   delete [] Area;
   delete [] Vol;
   return TRUE;
}


BOOLEAN FILMCOMP::SpaceValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                             REAL* t_ptr, const REAL* Y,
                             CARDINAL zone, REAL xx, BOOLEAN xrel,
                             AQVAR* var,
                             REAL& value)
{
   if ( Active() != TRUE ) return FALSE;
   CARDINAL n    = numgrid;
   const REAL* y = &Y[ind_y];

   REAL x = xx;
   if ( xrel == TRUE )
   {
      CARDINAL n  = numgrid;
      CARDINAL nv = NumVolStateVar();
      CARDINAL ne = NumEquStateVar();
      CARDINAL cell1 = nv+ne+1;
      CARDINAL cell3 = nv+ne+3;
      if ( porepart == TRUE )
      {
         cell1 += nv+1;
         cell3 += nv+1;
      }
      x = xx*y[cell1+(n-3)*cell3+nv+ne+1];
   }

   varsys->SetProgVar(SpaceZ,&x);
   switch ( zone )
   {
      case 0:
         if ( GridValue(varsys,calcnum_ptr,t_ptr,Y,0,0,var,value) == FALSE )
                                                              return FALSE;
         break;
      case 1: case 2:
         {
            REAL* Area = new REAL[n-1];   
            REAL* Vol  = new REAL[n-1];
            CalcGeometry(varsys,y,Area,Vol);   // calculate grid points!
            delete [] Area;
            delete [] Vol;
            if ( x < 0.0 )
            {
               if ( x < -1.5 )   // film surface (z=-1)
               {
                  if ( GridValue(varsys,calcnum_ptr,t_ptr,Y,zone,n-1,var,value)==FALSE )
                                                              return FALSE;
               }
               else              // liquid layer (z=-2)
               {
                  if ( GridValue(varsys,calcnum_ptr,t_ptr,Y,zone,n,var,value)==FALSE )
                                                              return FALSE;
               }
               return TRUE;
            }
            if ( x > gridpts[n-1] )
            {
               if ( GridValue(varsys,calcnum_ptr,t_ptr,Y,0,0,var,value) == FALSE )
                                                              return FALSE;
               return TRUE;   // bulk concentration
            }
            REAL r = x/gridpts[n-1]*(n-2);
            INTEGER i = (INTEGER) r; if ( i<0 ) i=0; if ( i>n-2 ) i=n-2;
            if ( x <= gridpts[i] )
            {
               while ( x < gridpts[i] )
               {
                  i--;
                  if ( i < 0 )
                     FatalError("FILMCOMP::Value: index negative");
               }
            }
            else
            {
               while ( x > gridpts[i+1] )
               {
                  i++;
                  if ( i >= n-1 )
                     FatalError("FILMCOMP::Value: index too large");
               }
            }
            CARDINAL i1, i2; REAL val1, val2;
            i1 = i; i2 = i+1;
            if ( GridValue(varsys,calcnum_ptr,t_ptr,Y,zone,i1,var,val1) == FALSE )
                                                              return FALSE;
            if ( GridValue(varsys,calcnum_ptr,t_ptr,Y,zone,i2,var,val2) == FALSE )
                                                              return FALSE;
            value = ( (x-gridpts[i1])*val2 + (gridpts[i2]-x)*val1 )
                    / (gridpts[i2]-gridpts[i1]);
         }
         break;
      default:
         return FALSE;
   }
   return TRUE;
}


CARDINAL FILMCOMP::InitZone(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("FILMCOMP::InitZone: Illegal index");
   return initzone[index];
}


AQVAR* FILMCOMP::InitVar(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("FILMCOMP::InitVar: Illegal index");
   return initvar[index];
}


const FORMVAR* FILMCOMP::InitVal(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("FILMCOMP::InitVal: Illegal index");
   return initval[index];
}


BOOLEAN FILMCOMP::AddInitCond(const VARLIST* varlist, CARDINAL zone,
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


BOOLEAN FILMCOMP::ReplaceInitCond(const VARLIST* varlist, CARDINAL zone,
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


BOOLEAN FILMCOMP::DeleteInitCond(CARDINAL pos)
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


BOOLEAN FILMCOMP::Inflow(const VARLIST* varlist,
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


AQVAR* FILMCOMP::InputVar(CARDINAL index) const
{
   if ( index >= numinput )
      FatalError("FILMCOMP::InputVar: Illegal index");
   return inputvar[index];
}


const FORMVAR* FILMCOMP::InputFlux(CARDINAL index) const
{
   if ( index >= numinput )
      FatalError("FILMCOMP::InputFlux: Illegal index");
   return inputflux[index];
}


BOOLEAN FILMCOMP::AddInput(const VARLIST* varlist, AQVAR* var,
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


BOOLEAN FILMCOMP::ReplaceInput(const VARLIST* varlist, AQVAR* var,
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


BOOLEAN FILMCOMP::DeleteInput(CARDINAL pos)
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


void FILMCOMP::PartDiss()
{
   delete [] part; part = 0;
   delete [] ind;  ind  = 0;
   if ( numvolstatevar > 0 )
   {
      part = new BOOLEAN[numvolstatevar];
      ind  = new INTEGER[numvolstatevar];
      for ( CARDINAL i=0; i<numvolstatevar; i++ )
      {
         part[i] = FALSE; ind[i] = -1;
         CARDINAL j;
         for ( j=0; j<nX; j++ )
         {
            if ( statevar[i] == Xvar[j] )
            {
               part[i] = TRUE; ind[i] = j;
               break;
            }
         }
         if ( part[i] == FALSE )
         {
            for ( j=0; j<nS; j++ )
            {
               if ( statevar[i] == Svar[j] )
               {
                  ind[i] = j;
                  break;
               }
            }
         }
      }
   }
   return;
}


BOOLEAN FILMCOMP::AccQ(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccQ = relacc;
   absaccQ = absacc;
   return TRUE;
}


BOOLEAN FILMCOMP::AccV(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccV = relacc;
   absaccV = absacc;
   return TRUE;
}


BOOLEAN FILMCOMP::AccZ(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccZ = relacc;
   absaccZ = absacc;
   return TRUE;
}


BOOLEAN FILMCOMP::AccEPS(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccEPS = relacc;
   absaccEPS = absacc;
   return TRUE;
}


void FILMCOMP::CalcArg()
{
   AQCOMP::CalcArg();
   CARDINAL num = numvararg;
   CARDINAL i;
   numvararg += numinit;
   for ( i=0; i<numinit; i++ ) numvararg += (initval[i])->NumVarArg();
   if ( inflow != 0 ) numvararg += inflow->NumVarArg();
   numvararg += numinput;
   for ( i=0; i<numinput; i++ ) numvararg += (inputflux[i])->NumVarArg();
   for ( i=0; i<nX; i++ )
   {
      numvararg += 1;
      numvararg += rho[i]->NumVarArg();
      numvararg += surfkat[i]->NumVarArg();
      numvararg += surfkde[i]->NumVarArg();
      numvararg += volkat[i]->NumVarArg();
      numvararg += volkde[i]->NumVarArg();
      numvararg += resX[i]->NumVarArg();
      numvararg += DXpore[i]->NumVarArg();
      numvararg += DXmatrix[i]->NumVarArg();
   }
   for ( i=0; i<nS; i++ )
   {
      numvararg += 1;
      numvararg += resS[i]->NumVarArg();
      numvararg += DSpore[i]->NumVarArg();
   }
   if ( udet != 0 ) numvararg += udet->NumVarArg();
   if ( A != 0 )    numvararg += A->NumVarArg();
   if ( reps != 0 ) numvararg += reps->NumVarArg();
   if ( numvararg > 0 )
   {
      AQVAR** newarg = new AQVAR*[numvararg];
      CARDINAL k = 0;
      for ( i=0; i<num; i++ )
      {
         newarg[k] = vararg[k]; k++;
      }
      CARDINAL j;
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
      for ( i=0; i<nX; i++ )
      {
         newarg[k] = Xvar[i]; k++;
         for ( j=0; j<rho[i]->NumVarArg(); j++ )
         {
            newarg[k] = rho[i]->VarArg(j); k++;
         }
         for ( j=0; j<surfkat[i]->NumVarArg(); j++ )
         {
            newarg[k] = surfkat[i]->VarArg(j); k++;
         }
         for ( j=0; j<surfkde[i]->NumVarArg(); j++ )
         {
            newarg[k] = surfkde[i]->VarArg(j); k++;
         }
         for ( j=0; j<volkat[i]->NumVarArg(); j++ )
         {
            newarg[k] = volkat[i]->VarArg(j); k++;
         }
         for ( j=0; j<volkde[i]->NumVarArg(); j++ )
         {
            newarg[k] = volkde[i]->VarArg(j); k++;
         }
         for ( j=0; j<resX[i]->NumVarArg(); j++ )
         {
            newarg[k] = resX[i]->VarArg(j); k++;
         }
         for ( j=0; j<DXpore[i]->NumVarArg(); j++ )
         {
            newarg[k] = DXpore[i]->VarArg(j); k++;
         }
         for ( j=0; j<DXmatrix[i]->NumVarArg(); j++ )
         {
            newarg[k] = DXmatrix[i]->VarArg(j); k++;
         }
      }
      for ( i=0; i<nS; i++ )
      {
         newarg[k] = Svar[i]; k++;
         for ( j=0; j<resS[i]->NumVarArg(); j++ )
         {
            newarg[k] = resS[i]->VarArg(j); k++;
         }
         for ( j=0; j<DSpore[i]->NumVarArg(); j++ )
         {
            newarg[k] = DSpore[i]->VarArg(j); k++;
         }
      }
      if ( udet != 0 )
      {
         for ( j=0; j<udet->NumVarArg(); j++ )
         {
            newarg[k] = udet->VarArg(j); k++;
         }
      }
      if ( A != 0 )
      {
         for ( j=0; j<A->NumVarArg(); j++ )
         {
            newarg[k] = A->VarArg(j); k++;
         }
      }
      if ( reps != 0 )
      {
         for ( j=0; j<reps->NumVarArg(); j++ )
         {
            newarg[k] = reps->VarArg(j); k++;
         }
      }
      if ( k != numvararg )
         FatalError("FILMCOMP::CalcArg: inconsistent number of arguments");
      delete vararg; vararg = newarg;
   }
   PartDiss();
   Changed();
   return;
}


BOOLEAN FILMCOMP::SetGridPts(CARDINAL n)
{
   if ( n < 7 )        return FALSE;
   if ( n == numgrid ) return TRUE;
   numgrid = n;
   delete gridpts; gridpts = new REAL[numgrid+2];
   return TRUE;
}


BOOLEAN FILMCOMP::NumGridPts(CARDINAL n)
{
   if ( InList() == TRUE ) return FALSE;
   return SetGridPts(n);
}


REAL FILMCOMP::GridPt(const REAL* Y, CARDINAL i)
{
   if ( i >= NumGridPts() )
      FatalError("FILMCOMP::GridPts: illegal index");
   const REAL* y = &Y[ind_y];
   CARDINAL j;
   CARDINAL n  = numgrid;
   CARDINAL nv = NumVolStateVar();
   CARDINAL ns = NumSurfStateVar();
   CARDINAL ne = NumEquStateVar();
   CARDINAL cell1 = nv+ne+1;
   CARDINAL cell2 = nv+ne+1;
   CARDINAL cell3 = nv+ne+3;
   CARDINAL cellb = nv+ne+ns+1;
   if ( porepart == TRUE )
   {
      cell1 += nv+1;
      cell3 += nv+1;
   }
   REAL* z = new REAL[n-2];
   for ( j=0; j<n-2; j++ ) z[j] = y[cell1+j*cell3+nv+ne+1];
   gridpts[0] = 0.0;                                         // start of film
   gridpts[1] = 0.5*z[0];                                    // first cell
   for ( j=2; j<n-1; j++ ) gridpts[j] = 0.5*(z[j-1]+z[j-2]); // other cells
   gridpts[n-1] = z[n-3];                                    // end  of film
   gridpts[n] = z[n-3];                                      // start of bulk
   gridpts[n+1] = z[n-3];                                    // bulk
   delete [] z;
   return gridpts[i];
}


CARDINAL FILMCOMP::NumZones() const
{
   return 3;
}


const char* FILMCOMP::ZoneName(CARDINAL index) const
{
   switch ( index )
   {
      case 0:  return ini.T(7);
      case 1:  return ini.T(5);
      case 2:  return ini.T(193);
      default: break;
   }

   FatalError("FILMCOMP::ZoneName: Illegal index");
   return 0;
}


CARDINAL FILMCOMP::NumZoneGridPts(CARDINAL zone)
{
   switch ( zone )
   {
      case 0:
         return 1;
      case 1:
         return NumGridPts();
      case 2:
         return NumGridPts();
   }
   FatalError("FILMCOMP::NumZoneGridPts: illegal zone");
   return 0;
}


REAL FILMCOMP::ZoneGridPt(const REAL* Y, CARDINAL zone,
                          CARDINAL index)
{
   switch ( zone )
   {
      case 0:
         return GridPt(Y,NumGridPts()-1);
      case 1:
         return GridPt(Y,index);
      case 2:
         return GridPt(Y,index);
   }
   FatalError("FILMCOMP::ZoneGridPt: illegal zone");
   return 0.0;
}


const char* FILMCOMP::AdvInName(CARDINAL conn) const
{
   if ( conn > 0 )
      FatalError("FILMCOMP::AdvInName: Illegal connection");
   return ini.T(47);
}


const char* FILMCOMP::AdvOutName(CARDINAL conn) const
{
   if ( conn > 0 )
      FatalError("FILMCOMP::AdvOutName: Illegal connection");
   return ini.T(48);
}


const char* FILMCOMP::DiffName(CARDINAL conn) const
{
   switch ( conn )
   {
      case 0:
         return ini.T(51);
      case 1:
         return ini.T(52);
      default:
         FatalError("FILMCOMP::DiffName: Illegal connection");
         break;
   }
   return 0;
}


BOOLEAN FILMCOMP::AdvInExZ(CARDINAL conn) const
{
   if ( conn > 0 ) FatalError("FILMCOMP::AdvInExZ: Illegal connection");
   return FALSE;
}


REAL FILMCOMP::AdvInZ(const REAL*, CARDINAL) const
{
   FatalError("FILMCOMP::AdvInZ: Illegal call to this function");
   return 0.0;
}


REAL FILMCOMP::AdvOutQ(VARSYS* varsys, const REAL* Y, CARDINAL conn)
{
   if ( conn > 0 ) FatalError("FILMCOMP::AdvOutQ: Illegal connection");
   if ( Active() == FALSE ) return 0.0;
   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   const REAL* y = &Y[ind_y];
   CARDINAL n  = numgrid;
   CARDINAL nv = NumVolStateVar();
   CARDINAL ns = NumSurfStateVar();
   CARDINAL ne = NumEquStateVar();
   CARDINAL cell1 = nv+ne+1;
   CARDINAL cell2 = nv+ne+1;
   CARDINAL cell3 = nv+ne+3;
   CARDINAL cellb = nv+ne+ns+1;
   if ( porepart == TRUE )
   {
      cell1 += nv+1;
      cell3 += nv+1;
   }
   varsys->SetProgVar(Discharge,
                      &y[cell1+(n-1)*cell3+cell2+cellb+1]);
   if ( numCbulk != nv )
   {
      delete [] Cbulk; Cbulk = new REAL[nv];
      numCbulk = nv;
   }
   for ( CARDINAL i=0; i<nv; i++ )
   {
      Cbulk[i] = y[cell1+(n-1)*cell3+cell2+i]
                 /y[cell1+(n-1)*cell3+cell2+cellb+2];
      statevar[i]->SetStateVar(&Cbulk[i]);
   }
   return y[cell1+(n-1)*cell3+cell2+cellb+1];
}


REAL FILMCOMP::AdvOutJ(VARSYS* varsys,
                       const REAL* Y,
                       CARDINAL conn,
                       const AQVAR* var)
{
   if ( conn > 0 ) FatalError("FILMCOMP::AdvOutJ: Illegal connection");
   if ( Active() == FALSE ) return 0.0;
   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   const REAL* y = &Y[ind_y];
   CARDINAL n  = numgrid;
   CARDINAL nv = NumVolStateVar();
   CARDINAL ns = NumSurfStateVar();
   CARDINAL ne = NumEquStateVar();
   CARDINAL cell1 = nv+ne+1;
   CARDINAL cell2 = nv+ne+1;
   CARDINAL cell3 = nv+ne+3;
   CARDINAL cellb = nv+ne+ns+1;
   if ( porepart == TRUE )
   {
      cell1 += nv+1;
      cell3 += nv+1;
   }
   CARDINAL i;
   varsys->SetProgVar(Discharge,
                      &y[cell1+(n-1)*cell3+cell2+cellb+1]);
   if ( numCbulk != nv )
   {
      delete [] Cbulk; Cbulk = new REAL[nv];
      numCbulk = nv;
   }
   for ( i=0; i<nv; i++ )
   {
      Cbulk[i] = y[cell1+(n-1)*cell3+cell2+i]
                 /y[cell1+(n-1)*cell3+cell2+cellb+2];
      statevar[i]->SetStateVar(&Cbulk[i]);
   }
   for ( i=0; i<nv; i++ )
   {
      if ( var == statevar[i] )
      {
         if ( part[i] == TRUE )
         {
            return y[cell1+(n-1)*cell3+cell2+cellb+1]
                   *y[cell1+(n-1)*cell3+cell2+i]
                   /y[cell1+(n-1)*cell3+cell2+cellb+2];
         }
         else
         {
            return y[cell1+(n-1)*cell3+cell2+cellb+1]
                   *y[cell1+(n-1)*cell3+cell2+i]
                   /y[cell1+(n-1)*cell3+cell2+cellb+2]
                   /y[cell1+(n-1)*cell3+cell2+nv+ns+ne];
         }
      }
   }
   return 0.0;
}


REAL FILMCOMP::DiffC(const REAL* Y, CARDINAL conn, const AQVAR* var) const
{
   if ( conn > 1 ) FatalError("FILMCOMP::DiffC: Illegal connection");
   if ( Active() == FALSE ) return 0.0;
   const REAL* y = &Y[ind_y];
   CARDINAL n  = numgrid;
   CARDINAL nv = NumVolStateVar();
   CARDINAL ns = NumSurfStateVar();
   CARDINAL ne = NumEquStateVar();
   CARDINAL cell1 = nv+ne+1;
   CARDINAL cell2 = nv+ne+1;
   CARDINAL cell3 = nv+ne+3;
   CARDINAL cellb = nv+ne+ns+1;
   if ( porepart == TRUE )
   {
      cell1 += nv+1;
      cell3 += nv+1;
   }
   for ( CARDINAL i=0; i<nv; i++ )
   {
      if ( var == statevar[i] )
      {
         switch ( conn )
         {
            case 0:
               return y[cell1+(n-1)*cell3+cell2+i]
                      /y[cell1+(n-1)*cell3+cell2+cellb+2];
            case 1:
               return y[i];
            default:
               FatalError("FILMCOMP::DiffC: Illegal connection");
         }
      }
   }
   return 0.0;
}


JOBSTATUS FILMCOMP::Load(std::istream& in,
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

            CARDINAL numerr;
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer);
                                                if ( var == 0 ) return PROBLEM;
               if ( AddX(var) == FALSE )                        return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( Rho(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( SurfKat(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( SurfKde(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( ResX(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( MatrixDiffX(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
            }
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer);
                                                if ( var == 0 ) return PROBLEM;
               if ( AddS(var) == FALSE )                        return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( ResS(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( PoreDiffS(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadBoolean(in,&confined) == FALSE )           return PROBLEM;

            REAL r;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( ReactVol(r) == FALSE )                         return PROBLEM;

            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( BulkVol(r) == FALSE )                          return PROBLEM;

            if ( LoadBoolean(in,&matrixdiff) == FALSE )         return PROBLEM;

            if ( LoadBoolean(in,&detachglobal) == FALSE )       return PROBLEM;
            if ( matrixdiff==FALSE && detachglobal==FALSE )     return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Udet(Buffer,varlist,Buffer1,numerr) == FALSE )
            {
               if ( detachglobal == TRUE )                      return PROBLEM;
            }

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Surface(Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( RateEps(Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;

            if ( LoadNumber(in,&n) == FALSE )                   return PROBLEM;
            if ( NumGridPts(n) == FALSE )                       return PROBLEM;
            if ( LoadBoolean(in,&highres) == FALSE )            return PROBLEM;
         }
         break;
      case 2: // aquavers=2: compartment index added 31.01.96
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

            CARDINAL numerr;
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer);
                                                if ( var == 0 ) return PROBLEM;
               if ( AddX(var) == FALSE )                        return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( Rho(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( SurfKat(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( SurfKde(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( ResX(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( MatrixDiffX(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
            }
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer);
                                                if ( var == 0 ) return PROBLEM;
               if ( AddS(var) == FALSE )                        return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( ResS(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( PoreDiffS(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadBoolean(in,&confined) == FALSE )           return PROBLEM;

            REAL r;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( ReactVol(r) == FALSE )                         return PROBLEM;

            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( BulkVol(r) == FALSE )                          return PROBLEM;

            if ( LoadBoolean(in,&matrixdiff) == FALSE )         return PROBLEM;

            if ( LoadBoolean(in,&detachglobal) == FALSE )       return PROBLEM;
            if ( matrixdiff==FALSE && detachglobal==FALSE )     return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Udet(Buffer,varlist,Buffer1,numerr) == FALSE )
            {
               if ( detachglobal == TRUE )                      return PROBLEM;
            }

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Surface(Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( RateEps(Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;

            if ( LoadNumber(in,&n) == FALSE )                   return PROBLEM;
            if ( NumGridPts(n) == FALSE )                       return PROBLEM;
            if ( LoadBoolean(in,&highres) == FALSE )            return PROBLEM;
         }
         break;
      case 3: // aquavers=3: particle diffusion in pores 25.04.96
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

            CARDINAL numerr;
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer);
                                                if ( var == 0 ) return PROBLEM;
               if ( AddX(var) == FALSE )                        return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( Rho(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( SurfKat(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( SurfKde(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( VolKat(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( VolKde(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( ResX(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( PoreDiffX(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( MatrixDiffX(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
            }
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer);
                                                if ( var == 0 ) return PROBLEM;
               if ( AddS(var) == FALSE )                        return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( ResS(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( PoreDiffS(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadBoolean(in,&confined) == FALSE )           return PROBLEM;

            REAL r;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( ReactVol(r) == FALSE )                         return PROBLEM;

            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( BulkVol(r) == FALSE )                          return PROBLEM;

            if ( LoadBoolean(in,&porepart) == FALSE )           return PROBLEM;

            if ( LoadBoolean(in,&matrixdiff) == FALSE )         return PROBLEM;

            if ( LoadBoolean(in,&detachglobal) == FALSE )       return PROBLEM;
            if ( matrixdiff==FALSE && detachglobal==FALSE )     return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Udet(Buffer,varlist,Buffer1,numerr) == FALSE )
            {
               if ( detachglobal == TRUE )                      return PROBLEM;
            }

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Surface(Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( RateEps(Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;

            if ( LoadNumber(in,&n) == FALSE )                   return PROBLEM;
            if ( NumGridPts(n) == FALSE )                       return PROBLEM;
            if ( LoadBoolean(in,&highres) == FALSE )            return PROBLEM;
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

            CARDINAL numerr;
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer);
                                                if ( var == 0 ) return PROBLEM;
               if ( AddX(var) == FALSE )                        return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( Rho(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( SurfKat(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( SurfKde(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( VolKat(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( VolKde(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( ResX(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( PoreDiffX(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( MatrixDiffX(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
            }
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer);
                                                if ( var == 0 ) return PROBLEM;
               if ( AddS(var) == FALSE )                        return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( ResS(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( PoreDiffS(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadBoolean(in,&confined) == FALSE )           return PROBLEM;

            REAL r;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( ReactVol(r) == FALSE )                         return PROBLEM;

            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( BulkVol(r) == FALSE )                          return PROBLEM;

            if ( LoadBoolean(in,&porepart) == FALSE )           return PROBLEM;

            if ( LoadBoolean(in,&matrixdiff) == FALSE )         return PROBLEM;

            if ( LoadBoolean(in,&detachglobal) == FALSE )       return PROBLEM;
            if ( matrixdiff==FALSE && detachglobal==FALSE )     return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Udet(Buffer,varlist,Buffer1,numerr) == FALSE )
            {
               if ( detachglobal == TRUE )                      return PROBLEM;
            }

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Surface(Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( RateEps(Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;

            if ( LoadNumber(in,&n) == FALSE )                   return PROBLEM;
            if ( NumGridPts(n) == FALSE )                       return PROBLEM;
            if ( LoadBoolean(in,&highres) == FALSE )            return PROBLEM;
            REAL relacc,absacc;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccQ(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccV(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccZ(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccEPS(relacc,absacc) == FALSE )               return PROBLEM;
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

            CARDINAL numerr;
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer);
                                                if ( var == 0 ) return PROBLEM;
               if ( AddX(var) == FALSE )                        return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( Rho(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( SurfKat(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( SurfKde(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( VolKat(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( VolKde(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( ResX(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( PoreDiffX(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( MatrixDiffX(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
            }
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer);
                                                if ( var == 0 ) return PROBLEM;
               if ( AddS(var) == FALSE )                        return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( ResS(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( PoreDiffS(var,Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadBoolean(in,&confined) == FALSE )           return PROBLEM;

            REAL r;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( ReactVol(r) == FALSE )                         return PROBLEM;

            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( BulkVol(r) == FALSE )                          return PROBLEM;

            if ( LoadBoolean(in,&porepart) == FALSE )           return PROBLEM;

            if ( LoadBoolean(in,&matrixdiff) == FALSE )         return PROBLEM;

            if ( LoadBoolean(in,&detachglobal) == FALSE )       return PROBLEM;
            if ( matrixdiff==FALSE && detachglobal==FALSE )     return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Udet(Buffer,varlist,Buffer1,numerr) == FALSE )
            {
               if ( detachglobal == TRUE )                      return PROBLEM;
            }

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Surface(Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( RateEps(Buffer,varlist,Buffer1,numerr) == FALSE )
                                                                return PROBLEM;

            if ( LoadNumber(in,&n) == FALSE )                   return PROBLEM;
            if ( NumGridPts(n) == FALSE )                       return PROBLEM;
            if ( LoadBoolean(in,&highres) == FALSE )            return PROBLEM;
            REAL relacc,absacc;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccQ(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccV(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccZ(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccEPS(relacc,absacc) == FALSE )               return PROBLEM;
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                                  return PROBLEM;
   HighRes(FALSE);  // high resolution not yet implemented
   return OK;
}


JOBSTATUS FILMCOMP::Save(std::ostream& out)
{
   char Buffer[BufSize];
   CARDINAL i;

   INTEGER aquavers = 5;
   HighRes(FALSE);  // high res not yet implemented

   if ( SaveStart(out) == FALSE )                             return PROBLEM;

   if ( SaveNumber(out,aquavers) == FALSE )                   return PROBLEM;

   JOBSTATUS status = AQCOMP::Save(out); if ( status != OK )  return status;

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

   if ( SaveStart(out) == FALSE )                             return PROBLEM;
   for ( i=0; i<nX; i++ )
   {
      if ( SaveString(out,Xvar[i]->Symbol()) == FALSE )       return PROBLEM;

      if ( rho[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 )    return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                  return PROBLEM;

      if ( surfkat[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                              return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                  return PROBLEM;

      if ( surfkde[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                              return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                  return PROBLEM;

      if ( volkat[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 ) return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                  return PROBLEM;

      if ( volkde[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 ) return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                  return PROBLEM;

      if ( resX[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 )   return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                  return PROBLEM;

      if ( DXpore[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 ) return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                  return PROBLEM;

      if ( DXmatrix[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                              return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                  return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                               return PROBLEM;

   if ( SaveStart(out) == FALSE )                             return PROBLEM;
   for ( i=0; i<nS; i++ )
   {
      if ( SaveString(out,Svar[i]->Symbol()) == FALSE )       return PROBLEM;

      if ( resS[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 )   return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                  return PROBLEM;

      if ( DSpore[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 ) return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                  return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                               return PROBLEM;

   if ( SaveBoolean(out,confined) == FALSE )                  return PROBLEM;

   if ( SaveNumber(out,V) == FALSE )                          return PROBLEM;

   if ( SaveNumber(out,VB) == FALSE )                         return PROBLEM;

   if ( SaveBoolean(out,porepart) == FALSE )                  return PROBLEM;

   if ( SaveBoolean(out,matrixdiff) == FALSE )                return PROBLEM;

   if ( SaveBoolean(out,detachglobal) == FALSE )              return PROBLEM;

   if ( udet == 0 )
   {
      if ( detachglobal == TRUE )                             return PROBLEM;
      Buffer[0] = '\0';
   }
   else
   {
      if ( udet->UnParse(Buffer,sizeof(Buffer)-1) != 0 )      return PROBLEM;
   }
   if ( SaveString(out,Buffer) == FALSE )                     return PROBLEM;

   if ( A->UnParse(Buffer,sizeof(Buffer)-1) != 0 )            return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                     return PROBLEM;

   if ( reps->UnParse(Buffer,sizeof(Buffer)-1) != 0 )         return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                     return PROBLEM;

   if ( SaveNumber(out,numgrid) == FALSE )                    return PROBLEM;
   if ( SaveBoolean(out,HighRes()) == FALSE )                 return PROBLEM;
   if ( SaveNumber(out,RelAccQ()) == FALSE )                  return PROBLEM;
   if ( SaveNumber(out,AbsAccQ()) == FALSE )                  return PROBLEM;
   if ( SaveNumber(out,RelAccV()) == FALSE )                  return PROBLEM;
   if ( SaveNumber(out,AbsAccV()) == FALSE )                  return PROBLEM;
   if ( SaveNumber(out,RelAccZ()) == FALSE )                  return PROBLEM;
   if ( SaveNumber(out,AbsAccZ()) == FALSE )                  return PROBLEM;
   if ( SaveNumber(out,RelAccEPS()) == FALSE )                return PROBLEM;
   if ( SaveNumber(out,AbsAccEPS()) == FALSE )                return PROBLEM;

   if ( SaveEnd(out) == FALSE )                               return PROBLEM;
   return OK;
}


JOBSTATUS FILMCOMP::Write(std::ostream& out, BOOLEAN sh)
{
   JOBSTATUS status = AQCOMP::Write(out,sh);  
                                           if ( status != OK ) return status;
   if ( sh == TRUE ) return OK;

   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   CARDINAL pos3 = pos1+2;
   CARDINAL pos4 = pos3+2;

   PrintIndent(pos2-1);
   char Buffer[BufSize];
   CARDINAL i;

   if ( Print(out,ini.T(158),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( NumInitCond() > 0 )
   {
      PrintIndent(pos4-1);
      if ( Print(out,ini.T(159),pos3) == FALSE )               return PROBLEM;
      if ( Print(out,"(") == FALSE )                           return PROBLEM;
      if ( Print(out,ini.T(160)) == FALSE )                    return PROBLEM;
      if ( Print(out,") : ") == FALSE )                        return PROBLEM;
      if ( Print(out,ini.T(161)) == FALSE )                    return PROBLEM;
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
   if ( Print(out,ini.T(154),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Inflow() != 0 )
   {
      if ( inflow->UnParse(Buffer,sizeof(Buffer)-1) != 0 )     return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;
   }

   if ( Print(out,ini.T(155),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( NumInput() > 0 )
   {
      PrintIndent(pos4-1);
      if ( Print(out,ini.T(156),pos3) == FALSE )               return PROBLEM;
      if ( Print(out," : ") == FALSE )                         return PROBLEM;
      if ( Print(out,ini.T(157)) == FALSE )                    return PROBLEM;
      for ( i=0; i<NumInput(); i++ )
      {
         if ( Print(out,InputVar(i)->Symbol(),pos3) == FALSE ) return PROBLEM;
         if ( Print(out," : ") == FALSE )                      return PROBLEM;
         if ( InputFlux(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer) == FALSE )                     return PROBLEM;
      }
   }

   if ( Print(out,ini.T(58),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( nX > 0 )
   {
      for ( i=0; i<nX; i++ )
      {
         if ( Print(out,Xvar[i]->Symbol(),pos3) == FALSE )     return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;

         if ( Print(out,ini.T(59),pos4) == FALSE )             return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( rho[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 )  return PROBLEM;
         if ( Print(out,Buffer,pos2) == FALSE )                return PROBLEM;

         if ( Print(out,ini.T(60),pos4) == FALSE )             return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( surfkat[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer,pos2) == FALSE )                return PROBLEM;

         if ( Print(out,ini.T(61),pos4) == FALSE )             return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( surfkde[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer,pos2) == FALSE )                return PROBLEM;

         if ( Print(out,ini.T(190),pos4) == FALSE )            return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( volkat[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer,pos2) == FALSE )                return PROBLEM;

         if ( Print(out,ini.T(191),pos4) == FALSE )            return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( volkde[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer,pos2) == FALSE )                return PROBLEM;

         if ( Print(out,ini.T(62),pos4) == FALSE )             return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( resX[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 ) return PROBLEM;
         if ( Print(out,Buffer,pos2) == FALSE )                return PROBLEM;

         if ( Print(out,ini.T(192),pos4) == FALSE )            return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( DXpore[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer,pos2) == FALSE )                return PROBLEM;

         if ( Print(out,ini.T(63),pos4) == FALSE )             return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( DXmatrix[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer,pos2) == FALSE )                return PROBLEM;
      }
   }

   if ( Print(out,ini.T(64),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( nS > 0 )
   {
      for ( i=0; i<nS; i++ )
      {
         if ( Print(out,Svar[i]->Symbol(),pos3) == FALSE )     return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;

         if ( Print(out,ini.T(65),pos4) == FALSE )             return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( resS[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 ) return PROBLEM;
         if ( Print(out,Buffer,pos2) == FALSE )                return PROBLEM;

         if ( Print(out,ini.T(66),pos4) == FALSE )             return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( DSpore[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer,pos2) == FALSE )                return PROBLEM;
      }
   }

   PrintIndent(pos2-1);
   if ( Print(out,ini.T(67),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( confined == TRUE )
   {
      if ( Print(out,ini.T(68),pos2) == FALSE )                return PROBLEM;
      if ( Print(out,ini.T(70),pos1) == FALSE )                return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Print(out,V,pos2) == FALSE )                        return PROBLEM;
   }
   else
   {
      if ( Print(out,ini.T(69),pos2) == FALSE )                return PROBLEM;
      if ( Print(out,ini.T(71),pos1) == FALSE )                return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Print(out,VB,pos2) == FALSE )                       return PROBLEM;
   }

   if ( Print(out,ini.T(187),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( PoreParticles() == FALSE )
   {
      if ( Print(out,ini.T(188),pos2) == FALSE )               return PROBLEM;
   }
   else
   {
      if ( Print(out,ini.T(189),pos2) == FALSE )               return PROBLEM;
   }

   if ( Print(out,ini.T(37),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( MatrixDiff() == FALSE )
   {
      if ( Print(out,ini.T(41),pos2) == FALSE )                return PROBLEM;
   }
   else
   {
      if ( Print(out,ini.T(42),pos2) == FALSE )                return PROBLEM;
   }

   if ( DetachGlobal() == TRUE )
   {
      if ( Print(out,ini.T(43),pos1) == FALSE )                return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( udet->UnParse(Buffer,sizeof(Buffer)-1) != 0 )       return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;
   }

   if ( Print(out,ini.T(72),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( A->UnParse(Buffer,sizeof(Buffer)-1) != 0 )             return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                      return PROBLEM;

   if ( Print(out,ini.T(73),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( reps->UnParse(Buffer,sizeof(Buffer)-1) != 0 )          return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                      return PROBLEM;

   if ( Print(out,ini.T(74),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,NumGridPts(),pos2) == FALSE )                return PROBLEM;
   if ( Print(out," (") == FALSE )                             return PROBLEM;
   if ( HighRes() == FALSE )
   {
      if ( Print(out,ini.T(9)) == FALSE )                      return PROBLEM;
   }
   else
   {
      if ( Print(out,ini.T(10)) == FALSE )                     return PROBLEM;
   }
   if ( Print(out,")") == FALSE )                              return PROBLEM;
   if ( Print(out,ini.T(244),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,ini.T(245),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,RelAccQ(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(246),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,AbsAccQ(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(247),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,RelAccV(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(248),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,AbsAccV(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(249),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,RelAccZ(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(250),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,AbsAccZ(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(251),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,RelAccEPS(),pos2) == FALSE )                 return PROBLEM;
   if ( Print(out,ini.T(252),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,AbsAccEPS(),pos2) == FALSE )                 return PROBLEM;

   return OK;
}

//////////////////////////////////////////////////////////////////////////////
