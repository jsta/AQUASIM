///////////////////////////////   lakecomp.C   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    01.03.95    Peter Reichert
// revisions:   13.06.96    Peter Reichert    sediment added, TKE revised
//                                            inflow added
//              15.11.96    Peter Reichert    pressure gradient added
//              08.01.98    Peter Reichert    epsilon equation added
//              21.05.98    Peter Reichert    sediment layers added
//              08.02.99    Peter Reichert    light intensity added
//              17.04.03    Peter Reichert    particle mobility added
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
// with U,e,eps: ioff=5, without U,e,eps: ioff=2
// with sediment: ised=1, without sediment: ised=0
// cell = ioff+(1+ised*numsedlayers)*(nv+ns+ne)
// (in the following comment: nl = numsedlayers)
//
// type component                                meaning
// ---------------------------------------------------------------------------
// alg  y[0]                                     Q at z[0]
// alg  y[1]                                     I at z[0]
// alg  y[2]                                     U at z[0]            (ioff=5)
// alg  y[3]                                     e at z[0]            (ioff=5)
// alg  y[4]                                     eps at z[0]          (ioff=5)
// alg  y[ioff]                                  Cv[1] at z[0]
//  .    .                                        .
// alg  y[ioff+nv-1]                             Cv[nv] at z[0]
// diff y[ioff+nv]                               Cs[1] at z[0]
//  .    .                                        .
// diff y[ioff+nv+ns-1]                          Cs[ns] at z[0]
// alg  y[ioff+nv+ns+ne]                         Ce[1] at z[0]
//  .    .                                        .
// alg  y[ioff+nv+ns+ne-1]                       Ce[ne] at z[0]
// alg  y[ioff+(nv+ns+ne)]                       Cv_s1[1] at z[0]     (ised=1)
//  .    .                                        .                   (ised=1)
// alg  y[ioff+(nv+ns+ne)+nv-1]                  Cv_s1[nv] at z[0]    (ised=1)
// diff y[ioff+(nv+ns+ne)+nv]                    Cs_s1[1] at z[0]     (ised=1)
//  .    .                                        .                   (ised=1)
// diff y[ioff+(nv+ns+ne)+nv+ns-1]               Cs_s1[ns] at z[0]    (ised=1)
// alg  y[ioff+(nv+ns+ne)+nv+ns+ne]              Ce_s1[1] at z[0]     (ised=1)
//  .    .                                        .                   (ised=1)
// alg  y[ioff+(nv+ns+ne)+nv+ns+ne-1]            Ce_s1[ne] at z[0]    (ised=1)
//	.	 .										  .
// alg  y[ioff+nl*(nv+ns+ne)]                    Cv_snl[1] at z[0]    (ised=1)
//  .    .                                        .                   (ised=1)
// alg  y[ioff+nl*(nv+ns+ne)+nv-1]               Cv_snl[nv] at z[0]   (ised=1)
// diff y[ioff+nl*(nv+ns+ne)+nv]                 Cs_snl[1] at z[0]    (ised=1)
//  .    .                                        .                   (ised=1)
// diff y[ioff+nl*(nv+ns+ne)+nv+ns-1]            Cs_snl[ns] at z[0]   (ised=1)
// alg  y[ioff+nl*(nv+ns+ne)+nv+ns+ne]           Ce_snl[1] at z[0]    (ised=1)
//  .    .                                        .                   (ised=1)
// alg  y[ioff+nl*(nv+ns+ne)+nv+ns+ne-1]         Ce_snl[ne] at z[0]   (ised=1)
// ---------------------------------------------------------------------------
// alg  y[cell]                                  Q at z[1]
// alg  y[cell+1]                                I at z[1]
// diff y[cell+2]                                U at z[1]            (ioff=5)
// diff y[cell+3]                                e at z[1]            (ioff=5)
// diff y[cell+4]                                eps at z[1]          (ioff=5)
// diff y[cell+ioff]                             Cv[1] at z[1]
//  .    .                                        .
// diff y[cell+ioff+nv-1]                        Cv[nv] at z[1]
// diff y[cell+ioff+nv]                          Cs[1] at z[1]
//  .    .                                        .
// diff y[cel+ioff+nv+ns-1]                      Cs[ns] at z[1]
// alg  y[cell+ioff+nv+ns]                       Ce[1] at z[1]
//  .    .                                        .
// alg  y[cell+ioff+nv+ns+ne-1]                  Ce[ne] at z[1]
// diff y[cell+ioff+(nv+ns+ne)]                  Cv_s1[1] at z[1]     (ised=1)
//  .    .                                        .                   (ised=1)
// diff y[cell+ioff+(nv+ns+ne)+nv-1]             Cv_s1[nv] at z[1]    (ised=1)
// diff y[cell+ioff+(nv+ns+ne)+nv]               Cs_s1[1] at z[1]     (ised=1)
//  .    .                                        .                   (ised=1)
// diff y[cell+ioff+(nv+ns+ne)+nv+ns-1]          Cs_s1[ns] at z[1]    (ised=1)
// alg  y[cell+ioff+(nv+ns+ne)+nv+ns]            Ce_s1[1] at z[1]     (ised=1)
//  .    .                                        .                   (ised=1)
// alg  y[cell+ioff+(nv+ns+ne)+nv+ns+ne-1]       Ce_s1[ne] at z[1]    (ised=1)
//	.	 .										  .
// diff y[cell+ioff+nl*(nv+ns+ne)]               Cv_snl[1] at z[1]    (ised=1)
//  .    .                                        .                   (ised=1)
// diff y[cell+ioff+nl*(nv+ns+ne)+nv-1]          Cv_snl[nv] at z[1]   (ised=1)
// diff y[cell+ioff+nl*(nv+ns+ne)+nv]            Cs_snl[1] at z[1]    (ised=1)
//  .    .                                        .                   (ised=1)
// diff y[cell+ioff+nl*(nv+ns+ne)+nv+ns-1]       Cs_snl[ns] at z[1]   (ised=1)
// alg  y[cell+ioff+nl*(nv+ns+ne)+nv+ns]         Ce_snl[1] at z[1]    (ised=1)
//  .    .                                        .                   (ised=1)
// alg  y[cell+ioff+nl*(nv+ns+ne)+nv+ns+ne-1]    Ce_snl[ne] at z[1]   (ised=1)
// ---------------------------------------------------------------------------
//  .    .                                        .
//  .    .                                        .
//
//
// ---------------------------------------------------------------------------
// alg  y[(n-1)*cell]                            Q at z[n-1]
// alg  y[(n-1)*cell+1]                          I at z[n-1]
// alg  y[(n-1)*cell+2]                          U at z[n-1]          (ioff=5)
// alg  y[(n-1)*cell+3]                          e at z[n-1]          (ioff=5)
// alg  y[(n-1)*cell+4]                          eps at z[n-1]        (ioff=5)
// diff y[(n-1)*cell+ioff]                       Cv[1] at z[n-1]
//  .    .                                        .
// diff y[(n-1)*cell+ioff+nv-1]                  Cv[nv] at z[n-1]
// diff y[(n-1)*cell+ioff+nv]                    Cs[1] at z[n-1]
//  .    .                                        .
// diff y[(n-1)*cell+ioff+nv+ns-1]               Cs[ns] at z[n-1]
// alg  y[(n-1)*cell+ioff+nv+ns]                 Ce[1] at z[n-1]
//  .    .                                        .
// alg  y[(n-1)*cell+ioff+nv+ns+ne-1]            Ce[ne] at z[n-1]
// diff y[(n-1)*cell+ioff+(nv+ns+ne)]            Cv_s1[1] at z[n-1]   (ised=1)
//  .    .                                        .                   (ised=1)
// diff y[(n-1)*cell+ioff+(nv+ns+ne)+nv-1]       Cv_s1[nv] at z[n-1]  (ised=1)
// diff y[(n-1)*cell+ioff+(nv+ns+ne)+nv]         Cs_s1[1] at z[n-1]   (ised=1)
//  .    .                                        .                   (ised=1)
// diff y[(n-1)*cell+ioff+(nv+ns+ne)+nv+ns-1]    Cs_s1[ns] at z[n-1]  (ised=1)
// alg  y[(n-1)*cell+ioff+(nv+ns+ne)+nv+ns]      Ce_s1[1] at z[n-1]   (ised=1)
//  .    .                                        .                   (ised=1)
// alg  y[(n-1)*cell+ioff+(nv+ns+ne)+nv+ns+ne-1] Ce_s1[ne] at z[n-1]  (ised=1)
//	.	 .										  .
// diff y[(n-1)*cell+ioff+nl*(nv+ns+ne)]         Cv_snl[1] at z[n-1]  (ised=1)
//  .    .                                        .                   (ised=1)
// diff y[(n-1)*cell+ioff+nl*(nv+ns+ne)+nv-1]    Cv_snl[nv] at z[n-1] (ised=1)
// diff y[(n-1)*cell+ioff+nl*(nv+ns+ne)+nv]      Cs_snl[1] at z[n-1]  (ised=1)
//  .    .                                        .                   (ised=1)
// diff y[(n-1)*cell+ioff+nl*(nv+ns+ne)+nv+ns-1] Cs_snl[ns] at z[n-1] (ised=1)
// alg  y[(n-1)*cell+ioff+nl*(nv+ns+ne)+nv+ns]   Ce_snl[1] at z[n-1]  (ised=1)
//  .    .                                        .                   (ised=1)
// alg  y[(n-1)*cell+ioff+nl*(nv+ns+ne)+nv+ns+ne-1] Ce_snl[ne]atz[n-1](ised=1)
// ---------------------------------------------------------------------------
// diff y[n*cell]                                Eseiche              (ioff=5)
//
//////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "fortran.h"
#include "formvar.h"
#include "othervar.h"
#include "varsys.h"
#include "lakecomp.h"
#include "aqlink.h"

//////////////////////////////////////////////////////////////////////////////

const CARDINAL BufSize = 1024;

static char namebuffer[BufSize];

//////////////////////////////////////////////////////////////////////////////

const char* LakeSedLayerIdentifier = "SEDLAY";

void LAKESEDLAYER::init()
{
   char Buffer1[BufSize]; char Buffer2[BufSize];
   zind = 1;
   NumToString(0.1,Buffer1,BufSize);
   thickness = new FORMVAR;
   thickness->Symbol("sedthickness");
   if ( thickness->Parse(Buffer1,0,Buffer2) != 0 )
   {
      FatalError("LAKESEDLAYER::init: Unable to parse thickness");
   }
   NumToString(0.8,Buffer1,BufSize);
   porosity = new FORMVAR;
   porosity->Symbol("sedporosity");
   if ( porosity->Parse(Buffer1,0,Buffer2) != 0 )
   {
      FatalError("LAKESEDLAYER::init: Unable to parse porosity");
   }
   numvararg = 0;
   vararg = 0;
   return;
}


LAKESEDLAYER::LAKESEDLAYER()
{
   init();
}


LAKESEDLAYER::LAKESEDLAYER(const LAKESEDLAYER& layer)
{
   init();
   zind = layer.zind;
   if ( layer.thickness != 0 ) 
   {
      delete thickness;
      thickness = new FORMVAR(layer.thickness);
   }
   if ( layer.porosity != 0 )
   {
      delete porosity;
      porosity = new FORMVAR(layer.porosity);
   }
   CalcVarArg();
}


LAKESEDLAYER::LAKESEDLAYER(const LAKESEDLAYER* layer)
{
   init();
   if ( layer != 0 )
   {
      zind = layer->zind;
      if ( layer->thickness != 0 )
      {
         delete thickness;
         thickness = new FORMVAR(layer->thickness);
      }
      if ( layer->porosity != 0 ) 
      {
         delete porosity;
         porosity = new FORMVAR(layer->porosity);
      }
   }
   CalcVarArg();
}


LAKESEDLAYER::~LAKESEDLAYER()
{
   delete thickness; thickness = 0;
   delete porosity;  porosity = 0;

   delete [] vararg; vararg = 0; numvararg = 0;
}


BOOLEAN LAKESEDLAYER::AllowedProgVarType(PROGVARTYPE type) const
{
   if ( type == CalcNumber )      return TRUE;
   if ( type == Time )            return TRUE;
   if ( type == CompIndex )       return TRUE;
   if ( type == ::ZoneIndex )     return TRUE;
   if ( type == WaterFraction )   return TRUE;
   if ( type == SpaceZ )          return TRUE;
   return FALSE;
}


BOOLEAN LAKESEDLAYER::AllowedVar(const AQVAR* var) const
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


AQVAR* LAKESEDLAYER::VarArg(CARDINAL index) const
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


BOOLEAN LAKESEDLAYER::Arg(const AQVAR* var) const
{
   for ( CARDINAL i=0; i<numvararg; i++ )
   {
      if ( vararg[i] == var )              return TRUE;
      if ( (vararg[i])->Arg(var) == TRUE ) return TRUE;
   }
   return FALSE;
}


BOOLEAN LAKESEDLAYER::ReplaceVar(AQVAR* oldvar, AQVAR* newvar)
{
   if ( thickness != 0 ) thickness->ReplaceVarArg(oldvar,newvar);
   if ( porosity != 0 )  porosity->ReplaceVarArg(oldvar,newvar);
   CalcVarArg();
   return TRUE;
}


BOOLEAN LAKESEDLAYER::ExchangeVar(AQVAR* var1, AQVAR* var2)
{
   AQVAR* v = new CONSTVAR;
   if ( thickness != 0 )
   {
      thickness->ReplaceVarArg(var1,v);
      thickness->ReplaceVarArg(var2,var1);
      thickness->ReplaceVarArg(v,var2);
   }
   if ( porosity != 0 )
   {
      porosity->ReplaceVarArg(var1,v);
      porosity->ReplaceVarArg(var2,var1);
      porosity->ReplaceVarArg(v,var2);
   }
   CalcVarArg();
   return TRUE;
}


BOOLEAN LAKESEDLAYER::ZoneIndex(CARDINAL i)
{
   if ( i == 0 ) return FALSE;
   zind = i;
   return TRUE;
}


BOOLEAN LAKESEDLAYER::Thickness(const char* inpline,
                            const VARLIST* varlist,
                            char* errorline,
                            CARDINAL& numparseerrors)
{
   if ( inpline == 0 )            return FALSE;
   if ( inpline[0] == '\0' )      return FALSE;

   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("sedthickness");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar; return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar; return FALSE;
   }
   delete thickness; thickness = formvar;
   CalcVarArg();
   return TRUE;
}


BOOLEAN LAKESEDLAYER::Porosity(const char* inpline,
                           const VARLIST* varlist,
                           char* errorline,
                           CARDINAL& numparseerrors)
{
   if ( inpline == 0 )            return FALSE;
   if ( inpline[0] == '\0' )      return FALSE;

   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("sedporosity");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar; return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar; return FALSE;
   }
   delete porosity; porosity = formvar;
   CalcVarArg();
   return TRUE;
}


void LAKESEDLAYER::CalcVarArg()
{
   numvararg = 0;
   if ( thickness != 0 ) numvararg += thickness->NumVarArg();
   if ( porosity != 0 )  numvararg += porosity->NumVarArg();

   if ( numvararg > 0 )
   {
      delete [] vararg;
      vararg = new AQVAR*[numvararg];
      CARDINAL k = 0;
      CARDINAL i;
      if ( thickness != 0 )
      {
         for ( i=0; i<thickness->NumVarArg(); i++ )
         {
            vararg[k] = thickness->VarArg(i); k++;
         }
      }
      if ( porosity != 0 )
      {
         for ( i=0; i<porosity->NumVarArg(); i++ )
         {
            vararg[k] = porosity->VarArg(i); k++;
         }
      }
      if ( k != numvararg )
         FatalError("LAKESEDLAYER::CalcVarArg: inconsistent number of arguments");
   }
   return;
}


JOBSTATUS LAKESEDLAYER::Load(std::istream& in, const VARLIST* varlist)
{
   char Buffer1[BufSize]; char Buffer2[BufSize];

   if ( LoadStart(in) == FALSE ) return PROBLEM;

   CARDINAL n;
   if (LoadNumber (in, &n)   == FALSE )                         return PROBLEM;
   if (ZoneIndex (n) == FALSE )                                 return PROBLEM;

   if ( LoadString(in, Buffer1, BufSize) == FALSE )             return PROBLEM;
   if ( Thickness(Buffer1, varlist, Buffer2, n) == FALSE )      return PROBLEM;

   if ( LoadString(in, Buffer1, BufSize) == FALSE )             return PROBLEM;
   if ( Porosity(Buffer1, varlist, Buffer2, n) == FALSE )       return PROBLEM;

   while (LoadStart(in) == TRUE )
   {
       if ( LoadEnd(in) == FALSE ) return PROBLEM;
   }
   return OK;
}


JOBSTATUS LAKESEDLAYER::Save(std::ostream& out)
{
   if ( SaveStart(out) == FALSE ) return PROBLEM;

   char Buffer[BufSize];
   const INIT* ini =  AQCOMP::Ini();                      

   if ( SaveNumber(out, zind)  == FALSE)                        return PROBLEM;

   if ( thickness->UnParse(Buffer,sizeof(Buffer)-1) !=0 )       return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                       return PROBLEM;

   if ( porosity->UnParse(Buffer,sizeof(Buffer)-1) !=0 )        return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                       return PROBLEM;

   if ( SaveEnd(out) == FALSE )                                 return PROBLEM;
   return OK;
}


JOBSTATUS LAKESEDLAYER::Write(std::ostream& out, BOOLEAN)
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

   if ( Print(out,ini->T(255),pos1+2) == FALSE )              return PROBLEM;
   if ( Print(out,":") == FALSE )                             return PROBLEM;
   if ( Print(out,ZoneIndex(),pos2) == FALSE )                return PROBLEM;

   if ( Print(out,ini->T(256),pos1+4) == FALSE )              return PROBLEM;
   if ( Print(out,":") == FALSE )                             return PROBLEM;
   if ( thickness->UnParse(Buffer,sizeof(Buffer)-1) != 0 )    return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                     return PROBLEM;

   if ( Print(out,ini->T(257),pos1+4) == FALSE )              return PROBLEM;
   if ( Print(out,":") == FALSE )                             return PROBLEM;
   if ( porosity->UnParse(Buffer,sizeof(Buffer)-1) != 0 )     return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                     return PROBLEM;

   return OK;
}


//////////////////////////////////////////////////////////////////////////////

const char* PointInfIdentifier = "PINF";

void PINF::init()
{
   depth = 0;
   width  = 0;
   waterflow = 0;
   numconc = 0; concvar = 0; concval = 0;
   numvararg = 0; vararg = 0;
   return;
}


PINF::PINF()
{
   init();
}


PINF::PINF(const PINF& pinf) : SYMBOL(pinf)
{
   init();
   depth = new FORMVAR(pinf.depth);
   width = new FORMVAR(pinf.width);
   waterflow = new FORMVAR(pinf.waterflow);
   if ( pinf.numvararg > 0 )
   {
      numvararg = pinf.numvararg;
      vararg = new AQVAR*[numvararg];
      for ( CARDINAL i=0; i<numvararg; i++ )
      {
         vararg[i] = pinf.vararg[i];
      }
   }
   if ( pinf.numconc > 0 )
   {
      numconc = pinf.numconc;
      concvar = new AQVAR*[numconc];
      concval = new FORMVAR*[numconc];
      for ( CARDINAL i=0; i<numconc; i++ )
      {
         concvar[i] = pinf.concvar[i];
         concval[i] = new FORMVAR(pinf.concval[i]);
      }
   }
}


PINF::PINF(const PINF* pinf) : SYMBOL(pinf)
{
   init();
   if ( pinf != 0 )
   {
      depth = new FORMVAR(pinf->depth);
      width = new FORMVAR(pinf->width);
      waterflow = new FORMVAR(pinf->waterflow);
      if ( pinf->numvararg > 0 )
      {
         numvararg = pinf->numvararg;
         vararg = new AQVAR*[numvararg];
         for ( CARDINAL i=0; i<numvararg; i++ )
         {
            vararg[i] = pinf->vararg[i];
         }
      }
      if ( pinf->numconc > 0 )
      {
         numconc = pinf->numconc;
         concvar = new AQVAR*[numconc];
         concval = new FORMVAR*[numconc];
         for ( CARDINAL i=0; i<numconc; i++ )
         {
            concvar[i]  = pinf->concvar[i];
            concval[i] = new FORMVAR(pinf->concval[i]);
         }
      }
   }
}


PINF::~PINF()
{
   CARDINAL i;
   delete depth; depth = 0;
   delete width; width = 0;
   delete waterflow; waterflow = 0;
   delete concvar; concvar = 0;
   for ( i=0; i<numconc; i++ ) delete concval[i];
   delete concval; concval = 0;
   numconc = 0;
   delete vararg; vararg = 0; numvararg = 0;
}


BOOLEAN PINF::AllowedProgVarType(PROGVARTYPE type) const
{
   if ( type == CalcNumber )       return TRUE;
   if ( type == Time )             return TRUE;
   return FALSE;
}


BOOLEAN PINF::AllowedVar(const AQVAR* var) const
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


BOOLEAN PINF::AllowedReplaceVar(const AQVAR* oldvar,
                                const AQVAR* newvar) const
{
   if ( Arg(oldvar) == TRUE )
   {
      if ( AllowedVar(newvar) == FALSE ) return FALSE;
   }
   return TRUE;
}


BOOLEAN PINF::AllowedExchangeVar(const AQVAR* var1,
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


BOOLEAN PINF::Depth(const char* inpline,
                    const VARLIST* varlist,
                    char* errorline,
                    CARDINAL& numparseerrors)
{
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("Depth");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar; return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar; return FALSE;
   }
   delete depth; depth = formvar;
   CalcVarArg();
   return TRUE;
}


BOOLEAN PINF::Width(const char* inpline,
                    const VARLIST* varlist,
                    char* errorline,
                    CARDINAL& numparseerrors)
{
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("Width");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar; return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar; return FALSE;
   }
   delete width; width = formvar;
   CalcVarArg();
   return TRUE;
}


BOOLEAN PINF::WaterFlow(const char* inpline,
                        const VARLIST* varlist,
                        char* errorline,
                        CARDINAL& numparseerrors)
{
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("WaterFlow");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar; return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar; return FALSE;
   }
   delete waterflow; waterflow = formvar;
   CalcVarArg();
   return TRUE;
}


AQVAR* PINF::ConcVar(CARDINAL index) const
{
   if ( index >= numconc )
      FatalError("PINF::ConcVar: Illegal index");
   return concvar[index];
}


FORMVAR* PINF::ConcVal(CARDINAL index) const
{
   if ( index >= numconc )
      FatalError("PINF::ConcVal: Illegal index");
   return concval[index];
}


BOOLEAN PINF::AddConc(const VARLIST* varlist, AQVAR* var,
                      const char* inpline, char* errorline,
                      CARDINAL& numparseerrors, CARDINAL pos)
{
   CARDINAL i;
   numparseerrors = 0;
   if ( InList() == TRUE )             return FALSE;
   if ( inpline == 0 )                 return FALSE;
   if ( inpline[0] == '\0' )           return FALSE;
   if ( varlist->Exist(var) == FALSE ) return FALSE;
   if ( pos > numconc ) pos = numconc;
   for ( i=0; i<numconc; i++ )
   {
      if ( concvar[i] == var )         return FALSE;
   }

   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("PointInpConc");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                  return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                  return FALSE;
   }
   AQVAR**   newvar = new AQVAR*[numconc+1];
   FORMVAR** newval = new FORMVAR*[numconc+1];
   for ( i=0; i<pos; i++ )
   {
      newvar[i] = concvar[i];
      newval[i] = concval[i];
   }
   newvar[pos] = var;
   newval[pos] = formvar;
   for ( i=pos; i<numconc; i++ )
   {
      newvar[i+1] = concvar[i];
      newval[i+1] = concval[i];
   }
   delete concvar; concvar = newvar;
   delete concval; concval = newval;
   numconc++;
   CalcVarArg();
   return TRUE;
}


BOOLEAN PINF::ReplaceConc(const VARLIST* varlist, AQVAR* var,
                          const char* inpline, char* errorline,
                          CARDINAL& numparseerrors, CARDINAL pos)
{
   CARDINAL i;
   numparseerrors = 0;
   if ( InList() == TRUE )             return FALSE;
   if ( inpline == 0 )                 return FALSE;
   if ( inpline[0] == '\0' )           return FALSE;
   if ( varlist->Exist(var) == FALSE ) return FALSE;
   if ( pos >= numconc )               return FALSE;
   for ( i=0; i<numconc; i++ )
   {
      if ( i!=pos && concvar[i]==var ) return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("PinfConc");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                  return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                  return FALSE;
   }
   delete concval[pos];
   concvar[pos] = var;
   concval[pos] = formvar;
   CalcVarArg();
   return TRUE;
}


BOOLEAN PINF::DeleteConc(CARDINAL pos)
{
   CARDINAL i;
   if ( pos >= numconc ) return FALSE;
   AQVAR**   newvar = new AQVAR*[numconc-1];
   FORMVAR** newval = new FORMVAR*[numconc-1];
   for ( i=0; i<pos; i++ )
   {
      newvar[i] = concvar[i];
      newval[i] = concval[i];
   }
   for ( i=pos+1; i<numconc; i++ )
   {
      newvar[i-1] = concvar[i];
      newval[i-1] = concval[i];
   }
   delete concval[pos];
   delete concvar; concvar = newvar;
   delete concval; concval = newval;
   numconc--;
   CalcVarArg();
   return TRUE;
}


AQVAR* PINF::VarArg(CARDINAL index) const
{
   if ( index >= numvararg )
   {
      FatalError("PINF::VarArg: index too large");
   }
   else
   {
      return vararg[index];
   }
   return 0;
}


BOOLEAN PINF::Arg(const AQVAR* var) const
{
   for ( CARDINAL i=0; i<numvararg; i++ )
   {
      if ( vararg[i] == var )              return TRUE;
      if ( (vararg[i])->Arg(var) == TRUE ) return TRUE;
   }
   return FALSE;
}


void PINF::ReplVar(AQVAR* oldvar, AQVAR* newvar)
{
   CARDINAL i;
   if ( depth != 0 ) depth->ReplaceVarArg(oldvar,newvar);
   if ( width != 0 ) width->ReplaceVarArg(oldvar,newvar);
   if ( waterflow != 0 ) waterflow->ReplaceVarArg(oldvar,newvar);
   {
      for ( i=0; i<numconc; i++ )
      {
         if ( concvar[i] == oldvar ) concvar[i] = newvar;
         concval[i]->ReplaceVarArg(oldvar,newvar);
      }
   }
   CalcVarArg();
}


BOOLEAN PINF::ReplaceVar(AQVAR* oldvar, AQVAR* newvar)
{
   if ( InList() == TRUE ) return FALSE;
   ReplVar(oldvar,newvar); return TRUE;
}


void PINF::ExchVar(AQVAR* var1, AQVAR* var2)
{
   CARDINAL i;
   AQVAR* v = new CONSTVAR;
   if ( depth != 0 )
   {
      depth->ReplaceVarArg(var1,v);
      depth->ReplaceVarArg(var2,var1);
      depth->ReplaceVarArg(v,var2);
   }
   if ( width != 0 )
   {
      width->ReplaceVarArg(var1,v);
      width->ReplaceVarArg(var2,var1);
      width->ReplaceVarArg(v,var2);
   }
   if ( waterflow != 0 )
   {
      waterflow->ReplaceVarArg(var1,v);
      waterflow->ReplaceVarArg(var2,var1);
      waterflow->ReplaceVarArg(v,var2);
   }
   for ( i=0; i<numconc; i++ )
   {
      if ( concvar[i] == var1 )
      {
         concvar[i] = var2;
      }
      else
      {
         if ( concvar[i] == var2 ) concvar[i] = var1;
      }
      concval[i]->ReplaceVarArg(var1,v);
      concval[i]->ReplaceVarArg(var2,var1);
      concval[i]->ReplaceVarArg(v,var2);
   }
   delete v;
   CalcVarArg();
}


BOOLEAN PINF::ExchangeVar(AQVAR* var1, AQVAR* var2)
{
   if ( InList() == TRUE ) return FALSE;
   ExchVar(var1,var2);    return TRUE;
}


void PINF::CalcVarArg()
{
   CARDINAL i,j,k;
   CARDINAL num = 0;
   if ( depth != 0 ) num += depth->NumVarArg();
   if ( width != 0 ) num += width->NumVarArg();
   if ( waterflow != 0 ) num += waterflow->NumVarArg();
   for ( i=0; i<numconc; i++ ) num += 1+concval[i]->NumVarArg();
   AQVAR** newarg = new AQVAR*[num];
   k = 0;
   if ( depth != 0 )
   {
      for ( i=0; i<depth->NumVarArg(); i++ )
      {
         newarg[k] = depth->VarArg(i); k++;
      }
   }
   if ( width != 0 )
   {
      for ( i=0; i<width->NumVarArg(); i++ )
      {
         newarg[k] = width->VarArg(i); k++;
      }
   }
   if ( waterflow != 0 )
   {
      for ( i=0; i<waterflow->NumVarArg(); i++ )
      {
         newarg[k] = waterflow->VarArg(i); k++;
      }
   }
   for ( i=0; i<numconc; i++ )
   {
      newarg[k] = concvar[i]; k++;
      for ( j=0; j<(concval[i]->NumVarArg()); j++ )
      {
          newarg[k] = concval[i]->VarArg(j); k++;
      }
   }
   numvararg = num;
   delete vararg;
   vararg = newarg;
   if ( k != num )
      FatalError("PINF::CalcVarArg: Inconsistent number of arguments");
   return;
}


JOBSTATUS PINF::Load(std::istream& in,
                     const VARLIST* varlist)
{
   CARDINAL numerr = 0;
   char Buf[BufSize]; char Buf1[BufSize];
   if ( InList() == TRUE )                                 return PROBLEM;
   if ( LoadString(in,Buf,sizeof(Buf)) == FALSE )          return PROBLEM;
   if ( Symbol(Buf) == FALSE )                             return PROBLEM;
   if ( LoadString(in,Buf,sizeof(Buf)) == FALSE )          return PROBLEM;
   Description(Buf);

   if ( LoadString(in,Buf,sizeof(Buf)) == FALSE )          return PROBLEM;
   if ( Depth(Buf,varlist,Buf1,numerr) == FALSE )          return PROBLEM;
   if ( LoadString(in,Buf,sizeof(Buf)) == FALSE )          return PROBLEM;
   if ( Width(Buf,varlist,Buf1,numerr) == FALSE )          return PROBLEM;
   if ( LoadString(in,Buf,sizeof(Buf)) == FALSE )          return PROBLEM;
   if ( WaterFlow(Buf,varlist,Buf1,numerr) == FALSE )      return PROBLEM;
   if ( LoadStart(in) == FALSE )                           return PROBLEM;
   while ( LoadString(in,Buf,sizeof(Buf)) == TRUE )
   {
      AQVAR* var = varlist->Get(Buf);        if ( var==0 ) return PROBLEM;
      if ( LoadString(in,Buf,sizeof(Buf)) == FALSE )       return PROBLEM;
      if ( AddConc(varlist,var,Buf,Buf1,numerr) == FALSE ) return PROBLEM;
   }
   Changed();
   return OK;
}


JOBSTATUS PINF::Save(std::ostream& out)
{
   char Buffer[BufSize];
   const INIT* ini = LAKECOMP::Ini();
   if ( SaveString(out,Symbol()) == FALSE )                  return PROBLEM;
   if ( SaveString(out,Description()) == FALSE )             return PROBLEM;

   if ( depth->UnParse(Buffer,sizeof(Buffer)-1) != 0 )       return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                    return PROBLEM;
   if ( width->UnParse(Buffer,sizeof(Buffer)-1) != 0 )       return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                    return PROBLEM;
   if ( waterflow->UnParse(Buffer,sizeof(Buffer)-1) != 0 )   return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                    return PROBLEM;

   if ( SaveStart(out) == FALSE )                            return PROBLEM;
   for ( CARDINAL i=0; i<numconc; i++ )
   {
      if ( SaveString(out,(concvar[i])->Symbol()) == FALSE ) return PROBLEM;
      if ( (concval[i])->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                             return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                 return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                              return PROBLEM;

   return FILEIO::Save(out);
}


JOBSTATUS PINF::Write(std::ostream& out, BOOLEAN)
{
   const INIT* ini = LAKECOMP::Ini();
   CARDINAL pos1 = ini->L(1)+1; 
   CARDINAL pos2 = pos1+ini->L(2);
   CARDINAL pos3 = pos1+2;
   CARDINAL pos4 = pos3+2;
   CARDINAL pos5 = pos4+2;
   CARDINAL pos6 = pos5+2;

   PrintIndent(pos2-1);
   char Buffer[BufSize];

   if ( Print(out,Symbol(),pos3) == FALSE )                 return PROBLEM;
   if ( Print(out,":") == FALSE )                           return PROBLEM;

   if ( Print(out,ini->T(3),pos4) == FALSE )                return PROBLEM;
   if ( Print(out,":") == FALSE )                           return PROBLEM;
   if ( Description() != 0 )
   {
      if ( strlen(Description()) > 0 )
      {
         if ( Print(out,Description(),pos2) == FALSE )      return PROBLEM;
      }
   }

   if ( Print(out,ini->T(197),pos4) == FALSE )              return PROBLEM;
   if ( Print(out,":") == FALSE )                           return PROBLEM;
   if ( depth->UnParse(Buffer,sizeof(Buffer)-1) != 0 )      return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini->T(198),pos4) == FALSE )              return PROBLEM;
   if ( Print(out,":") == FALSE )                           return PROBLEM;
   if ( width->UnParse(Buffer,sizeof(Buffer)-1) != 0 )      return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini->T(199),pos4) == FALSE )              return PROBLEM;
   if ( Print(out,":") == FALSE )                           return PROBLEM;
   if ( waterflow->UnParse(Buffer,sizeof(Buffer)-1) != 0 )  return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;
   if ( NumConc() > 0 )
   {
      PrintIndent(pos6-5);
      if ( Print(out,ini->T(200),pos4) == FALSE )           return PROBLEM;
      if ( Print(out,":") == FALSE )                        return PROBLEM;
      if ( Print(out,ini->T(201),pos5) == FALSE )           return PROBLEM;
      if ( Print(out," : ") == FALSE )                      return PROBLEM;
      if ( Print(out,ini->T(202)) == FALSE )                return PROBLEM;
      for ( CARDINAL i=0; i<NumConc(); i++ )
      {
         if ( Print(out,(concvar[i])->Symbol(),pos5) == FALSE )
                                                            return PROBLEM;
         if ( Print(out," : ") == FALSE )                   return PROBLEM;
         if ( (concval[i])->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                            return PROBLEM;
         if ( Print(out,Buffer) == FALSE )                  return PROBLEM;
      }
   }
   return OK;
}

//////////////////////////////////////////////////////////////////////////////

void LAKECOMP::init()
{
   numinit = 0; initzone = 0; initvar = 0; initval = 0;

   numsurfinput = 0; surfinputvar = 0; surfinputflux = 0;
   latinflow = 0;
   numlatinput = 0; latinputvar = 0; latinputconc = 0;
   numsedinput = 0; sedinputvar = 0; sedinputflux = 0;

   Zt = ini.R(15);
   Zb = ini.R(16);
   numpartvar = 0; partvar = 0; partrho = 0; partsedveloc = 0; partmobility = 0;
   numdissvar = 0; dissvar = 0; disskdiff = 0;
   gridpts = 0; SetGridPts(12); SetGridPts(ini.I(1));
   char buf[10];
   A       = 0;
   Rho     = 0;
   D       = 0;
   Isurf   = new FORMVAR;
   Isurf->Parse("0",0,buf);
   Eta     = new FORMVAR;
   Eta->Parse("0",0,buf);
   calcSed = FALSE;
   ised    = 0;
   Fvol    = new FORMVAR;
   Fvol->Parse("0",0,buf);
   kres    = new FORMVAR;
   kres->Parse("0",0,buf);
   numsedlayers = 0;
   sedlayers = 0;
   calcTKE = FALSE;
   ioff    = 1;
   Pr      = new FORMVAR;
   Pr->Parse("1",0,buf);
   sigmaTKE = 1.0;
   sigmaEps = 1.3;
   kzmin    = 0.0;
   TauSurf = new FORMVAR;
   TauSurf->Parse("0",0,buf);
   PressGrad = new FORMVAR;
   PressGrad->Parse("0",0,buf);
   Pwind   = new FORMVAR;
   Pwind->Parse("0",0,buf);
   Pbottom = new FORMVAR;
   Pbottom->Parse("0",0,buf);
   Pint    = new FORMVAR;
   Pint->Parse("0",0,buf);
   ProdDissip  = new FORMVAR;
   ProdDissip->Parse("0",0,buf);

   highres       = FALSE;
   gravit        = 9.81;
   relaccQ       = 1.0e-4;
   absaccQ       = 1.0e-4;
   relaccI       = 1.0e-4;
   absaccI       = 1.0e-4;
   relaccU       = 1.0e-4;
   absaccU       = 1.0e+3;
   relaccTKE     = 1.0e-4;
   absaccTKE     = 1.0e-2;
   relaccEPS     = 1.0e-4;
   absaccEPS     = 1.0e-2;
   relaccEseiche = 1.0e-3;
   absaccEseiche = 1.0e-3;
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


void LAKECOMP::del()
{
   CARDINAL i;

   delete initzone; initzone = 0;
   delete initvar; initvar = 0;
   for ( i=0; i<numinit; i++ ) delete initval[i];
   delete initval; initval = 0; numinit = 0;

   delete surfinputvar;  surfinputvar = 0;
   for ( i=0; i<numsurfinput; i++ ) delete surfinputflux[i];
   delete surfinputflux; surfinputflux = 0; numsurfinput = 0;
   delete latinflow; latinflow = 0;
   delete latinputvar; latinputvar = 0;
   for ( i=0; i<numlatinput; i++ ) delete latinputconc[i];
   delete latinputconc; latinputconc = 0; numlatinput = 0;
   DeletePointInf();
   delete sedinputvar; sedinputvar = 0;
   for ( i=0; i<numsedinput; i++ ) delete sedinputflux[i];
   delete sedinputflux; sedinputflux = 0; numsedinput = 0;

   delete gridpts; gridpts = 0;
      
   delete dissvar; dissvar = 0;
   for ( i=0; i<numdissvar; i++ ) 
   {
      delete disskdiff[i];
   }
   delete disskdiff; disskdiff = 0; 
   numdissvar = 0;
   delete partvar; partvar = 0;
   for ( i=0; i<numpartvar; i++ ) 
   {
      delete partrho[i];
      delete partsedveloc[i];
      delete partmobility[i];
   }
   delete partrho; partrho = 0; 
   delete partsedveloc; partsedveloc = 0; 
   delete partmobility; partmobility = 0; 
   numpartvar = 0;

   delete A;          A          = 0;
   delete Rho;        Rho        = 0;
   delete D;          D          = 0;
   delete Isurf;      Isurf      = 0;
   delete Eta;        Eta        = 0;
   for ( i=0; i<numsedlayers; i++ )
   {
      delete sedlayers[i];
   }
   delete [] sedlayers; sedlayers = 0; numsedlayers = 0;
   delete Fvol;       Fvol       = 0;
   delete kres;       kres       = 0;
   delete Pr;         Pr         = 0;
   delete TauSurf;    TauSurf    = 0;
   delete PressGrad;  PressGrad  = 0;
   delete Pwind;      Pwind      = 0;
   delete Pbottom;    Pbottom    = 0;
   delete Pint;       Pint       = 0;
   delete ProdDissip; ProdDissip = 0;
   delete [] Cout; Cout = 0;
   return;
}


LAKECOMP::LAKECOMP()
{
   init();
}


LAKECOMP::LAKECOMP(const LAKECOMP& com) : AQCOMP(com)
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

   if ( com.numsurfinput > 0 )
   {
      numsurfinput = com.numsurfinput;
      surfinputvar = new AQVAR*[numsurfinput];
      surfinputflux = new FORMVAR*[numsurfinput];
      for ( CARDINAL i=0; i<numsurfinput; i++ )
      {
         surfinputvar[i] = com.surfinputvar[i];
         surfinputflux[i] = new FORMVAR(com.surfinputflux[i]);
      }
   }
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
   PINF* pinf = (com.pinflist).First();
   while ( pinf != 0 )
   {
      PINF* p = new PINF(pinf);
      if ( AddPointInf(p) == FALSE )
         FatalError("LAKECOMP::LAKECOMP: unable to add point inflow");
      pinf = pinf->Next();
   }
   if ( com.numsedinput > 0 )
   {
      numsedinput = com.numsedinput;
      sedinputvar  = new AQVAR*[numsedinput];
      sedinputflux = new FORMVAR*[numsedinput];
      for ( CARDINAL i=0; i<numsedinput; i++ )
      {
         sedinputvar[i] = com.sedinputvar[i];
         sedinputflux[i] = new FORMVAR(com.sedinputflux[i]);
      }
   }

   Zt = com.Zt;
   Zb = com.Zb;
   numgrid = com.numgrid;
   if ( gridpts != 0 ) delete [] gridpts;
   gridpts = new REAL[numgrid];
   for ( CARDINAL i=0; i<numgrid; i++ ) gridpts[i] = com.gridpts[i];
   gravit        = com.gravit;
   relaccQ       = com.relaccQ;
   absaccQ       = com.absaccQ;
   relaccI       = com.relaccI;
   absaccI       = com.absaccI;
   relaccU       = com.relaccU;
   absaccU       = com.absaccU;
   relaccTKE     = com.relaccTKE;
   absaccTKE     = com.absaccTKE;
   relaccEPS     = com.relaccEPS;
   absaccEPS     = com.absaccEPS;
   relaccEseiche = com.relaccEseiche;
   absaccEseiche = com.absaccEseiche;
   if ( com.numpartvar > 0 )
   {
      numpartvar = com.numpartvar;
      partvar = new AQVAR*[numpartvar];
      partrho = new FORMVAR*[numpartvar];
      partsedveloc = new FORMVAR*[numpartvar];
      partmobility = new FORMVAR*[numpartvar];
      for ( CARDINAL i=0; i<numpartvar; i++ )
      {
         partvar[i] = com.partvar[i];
         partrho[i] = new FORMVAR(com.partrho[i]);
         partsedveloc[i] = new FORMVAR(com.partsedveloc[i]);
         partmobility[i] = new FORMVAR(com.partmobility[i]);
      }
   }
   if ( com.numdissvar > 0 )
   {
      numdissvar = com.numdissvar;
      dissvar = new AQVAR*[numdissvar];
      disskdiff = new FORMVAR*[numdissvar];
      for ( CARDINAL i=0; i<numpartvar; i++ )
      {
         dissvar[i] = com.dissvar[i];
         disskdiff[i] = new FORMVAR(com.disskdiff[i]);
      }
   }
   if ( A != 0 ) delete A;
   A       = new FORMVAR(com.A);
   if ( Rho != 0 ) delete Rho;
   Rho     = new FORMVAR(com.Rho);
   if ( D != 0 ) delete D;
   D       = new FORMVAR(com.D);
   if ( Isurf != 0 ) delete Isurf;
   Isurf   = new FORMVAR(com.Isurf);
   if ( Eta != 0 ) delete Eta;
   Eta     = new FORMVAR(com.Eta);
   calcSed = com.calcSed;
   ised    = com.ised;
   if ( Fvol != 0 ) delete Fvol;
   Fvol    = new FORMVAR(com.Fvol);
   if ( kres != 0 ) delete kres;
   kres    = new FORMVAR(com.kres);
   numsedlayers = com.numsedlayers;
   if ( numsedlayers > 0 )
   {
      sedlayers = new LAKESEDLAYER*[numsedlayers];
      for ( CARDINAL i=0; i<numsedlayers; i++ )
      {
         sedlayers[i] = new LAKESEDLAYER(com.sedlayers[i]);
      }
   }
   calcTKE = com.calcTKE;
   ioff    = com.ioff;
   if ( Pr != 0 ) delete Pr;
   Pr      = new FORMVAR(com.Pr);
   sigmaTKE = com.sigmaTKE;
   sigmaEps = com.sigmaEps;
   kzmin = com.kzmin;
   if ( TauSurf != 0 ) delete TauSurf;
   TauSurf = new FORMVAR(com.TauSurf);
   if ( PressGrad != 0 ) delete PressGrad;
   PressGrad = new FORMVAR(com.PressGrad);
   if ( Pwind != 0 ) delete Pwind;
   Pwind   = new FORMVAR(com.Pwind);
   if ( Pbottom != 0 ) delete Pbottom;
   Pbottom = new FORMVAR(com.Pbottom);
   if ( Pint != 0 ) delete Pint;
   Pint    = new FORMVAR(com.Pint);
   if ( ProdDissip != 0 ) delete ProdDissip;
   ProdDissip  = new FORMVAR(com.ProdDissip);
   highres = com.highres;
}


LAKECOMP::LAKECOMP(const AQCOMP* com) : AQCOMP(com)
{
   init();
   if ( com != 0 )
   {
      if ( com->Type() == LakeComp )
      {
         CARDINAL i;
         LAKECOMP* lc = (LAKECOMP*)com;

         if ( lc->numinit > 0 )
         {
            numinit = lc->numinit;
            initzone = new CARDINAL[numinit];
            initvar  = new AQVAR*[numinit];
            initval  = new FORMVAR*[numinit];
            for ( CARDINAL i=0; i<numinit; i++ )
            {
               initzone[i] = lc->initzone[i];
               initvar[i]  = lc->initvar[i];
               initval[i]  = new FORMVAR(lc->initval[i]);
            }
         }

         if ( lc->numsurfinput > 0 )
         {
            numsurfinput = lc->numsurfinput;
            surfinputvar = new AQVAR*[numsurfinput];
            surfinputflux = new FORMVAR*[numsurfinput];
            for ( CARDINAL i=0; i<numsurfinput; i++ )
            {
               surfinputvar[i] = lc->surfinputvar[i];
               surfinputflux[i] = new FORMVAR(lc->surfinputflux[i]);
            }
         }

         latinflow = new FORMVAR(lc->latinflow);
         if ( lc->numlatinput > 0 )
         {
            numlatinput = lc->numlatinput;
            latinputvar  = new AQVAR*[numlatinput];
            latinputconc = new FORMVAR*[numlatinput];
            for ( CARDINAL i=0; i<numlatinput; i++ )
            {
               latinputvar[i] = lc->latinputvar[i];
               latinputconc[i] = new FORMVAR(lc->latinputconc[i]);
            }
         }
         PINF* pinf = (lc->pinflist).First();
         while ( pinf != 0 )
         {
            PINF* p = new PINF(pinf);
            if ( AddPointInf(p) == FALSE )
               FatalError("LAKECOMP::LAKECOMP: unable to add point inflow");
            pinf = pinf->Next();
         }
         if ( lc->numsedinput > 0 )
         {
            numsedinput = lc->numsedinput;
            sedinputvar  = new AQVAR*[numsedinput];
            sedinputflux = new FORMVAR*[numsedinput];
            for ( i=0; i<numsedinput; i++ )
            {
               sedinputvar[i] = lc->sedinputvar[i];
               sedinputflux[i] = new FORMVAR(lc->sedinputflux[i]);
            }
         }

         Zt = lc->Zt;
         Zb = lc->Zb;
         numgrid = lc->numgrid;
		 if ( gridpts != 0 ) delete [] gridpts;
         gridpts = new REAL[numgrid];
         for ( i=0; i<numgrid; i++ ) gridpts[i] = lc->gridpts[i];
         gravit        = lc->gravit;
         relaccQ       = lc->relaccQ;
         absaccQ       = lc->absaccQ;
         relaccI       = lc->relaccI;
         absaccI       = lc->absaccI;
         relaccU       = lc->relaccU;
         absaccU       = lc->absaccU;
         relaccTKE     = lc->relaccTKE;
         absaccTKE     = lc->absaccTKE;
         relaccEPS     = lc->relaccEPS;
         absaccEPS     = lc->absaccEPS;
         relaccEseiche = lc->relaccEseiche;
         absaccEseiche = lc->absaccEseiche;
         if ( lc->numpartvar > 0 )
         {
            numpartvar = lc->numpartvar;
            partvar = new AQVAR*[numpartvar];
            partrho = new FORMVAR*[numpartvar];
            partsedveloc = new FORMVAR*[numpartvar];
            partmobility = new FORMVAR*[numpartvar];
            for ( CARDINAL i=0; i<numpartvar; i++ )
            {
               partvar[i] = lc->partvar[i];
               partrho[i] = new FORMVAR(lc->partrho[i]);
               partsedveloc[i] = new FORMVAR(lc->partsedveloc[i]);
               partmobility[i] = new FORMVAR(lc->partmobility[i]);
            }
         }
         if ( lc->numdissvar > 0 )
         {
            numdissvar = lc->numdissvar;
            dissvar = new AQVAR*[numdissvar];
            disskdiff = new FORMVAR*[numdissvar];
            for ( i=0; i<numdissvar; i++ )
            {
               dissvar[i] = lc->dissvar[i];
               disskdiff[i] = new FORMVAR(lc->disskdiff[i]);
            }
         }
         if ( A != 0 ) delete A;
         A       = new FORMVAR(lc->A);
         if ( Rho != 0 ) delete Rho;
         Rho     = new FORMVAR(lc->Rho);
         if ( D != 0 ) delete D;
         D       = new FORMVAR(lc->D);
         if ( Isurf != 0 ) delete Isurf;
         Isurf       = new FORMVAR(lc->Isurf);
         if ( Eta != 0 ) delete Eta;
         Eta     = new FORMVAR(lc->Eta);
         calcSed = lc->calcSed;
         ised    = lc->ised;
         if ( Fvol != 0 ) delete Fvol;
         Fvol    = new FORMVAR(lc->Fvol);
         if ( kres != 0 ) delete kres;
         kres    = new FORMVAR(lc->kres);
         numsedlayers = lc->numsedlayers;
         if ( numsedlayers > 0 )
         {
            sedlayers = new LAKESEDLAYER*[numsedlayers];
            for ( i=0; i<numsedlayers; i++ )
            {
               sedlayers[i] = new LAKESEDLAYER(lc->sedlayers[i]);
            }
         }
         calcTKE = lc->calcTKE;
         ioff    = lc->ioff;
         if ( Pr != 0 ) delete Pr;
         Pr      = new FORMVAR(lc->Pr);
         sigmaTKE = lc->sigmaTKE;
         sigmaEps = lc->sigmaEps;
         kzmin = lc->kzmin;
         if ( TauSurf != 0 ) delete TauSurf;
         TauSurf = new FORMVAR(lc->TauSurf);
         if ( PressGrad != 0 ) delete PressGrad;
         PressGrad = new FORMVAR(lc->PressGrad);
         if ( Pwind != 0 ) delete Pwind;
         Pwind   = new FORMVAR(lc->Pwind);
         if ( Pbottom != 0 ) delete Pbottom;
         Pbottom = new FORMVAR(lc->Pbottom);
         if ( Pint != 0 ) delete Pint;
         Pint    = new FORMVAR(lc->Pint);
         if ( ProdDissip != 0 ) delete ProdDissip;
         ProdDissip  = new FORMVAR(lc->ProdDissip);
         highres = lc->highres;
      }
   }
}


LAKECOMP::~LAKECOMP()
{
   del();
}


const char* LAKECOMP::TypeName() const
{
   return ini.T(95);
}


AQVAR* LAKECOMP::SpaceVar(const VARSYS* varsys) const
{
   return varsys->GetProgVar(SpaceZ);
}


BOOLEAN LAKECOMP::AllowedSpaceVal(REAL z) const
{
   if ( (z<gridpts[0]) && (z<gridpts[numgrid-1]) ) return FALSE;
   if ( (z>gridpts[0]) && (z>gridpts[numgrid-1]) ) return FALSE;
   return TRUE;
}


BOOLEAN LAKECOMP::AllowedProgVarType(PROGVARTYPE type) const
{
   if ( type == CalcNumber )      return TRUE;
   if ( type == Time )            return TRUE;
   if ( type == CompIndex )       return TRUE;
   if ( type == ZoneIndex )       return TRUE;
   if ( type == WaterFraction )   return TRUE;
   if ( type == SpaceZ )          return TRUE;
   if ( type == Discharge )       return TRUE;
   if ( type == CrossSection )    return TRUE;
   if ( type == ::Density )       return TRUE;
   if ( type == AreaGradient )    return TRUE;
   if ( type == LightIntensity )  return TRUE;
   if ( type == BruntVaisala )    return TRUE;
   if ( type == HorizVelocity )   return TRUE;
   if ( type == TurbKinEnergy )   return TRUE;
   if ( type == ShearProd )       return TRUE;
   if ( type == BuoyancyProd )    return TRUE;
   if ( type == Dissipation )     return TRUE;
   if ( type == SeicheEnergy )    return TRUE;
   return FALSE;
}


BOOLEAN LAKECOMP::AllowedComp() const
{
   if ( A == 0 )                                     return FALSE;
   if ( A->ArgVarType(StateVar) == TRUE )            return FALSE;
   if ( Rho == 0 )                                   return FALSE;
   if ( D == 0 )                                     return FALSE;
   if ( D->ArgVarType(StateVar) == TRUE )            return FALSE;
   if ( Isurf == 0 )                                 return FALSE;
   if ( Eta == 0 )                                   return FALSE;
   if ( CalcTKE() == TRUE )
   {
      if ( Pr == 0 )                                 return FALSE;
      if ( TauSurf == 0 )                            return FALSE;
      if ( TauSurf->ArgVarType(StateVar) == TRUE )   return FALSE;
      if ( PressGrad == 0 )                          return FALSE;
//      if ( PressGrad->ArgVarType(StateVar) == TRUE ) return FALSE;
      if ( Pwind == 0 )                              return FALSE;
      if ( Pwind->ArgVarType(StateVar) == TRUE )     return FALSE;
      if ( Pbottom == 0 )                            return FALSE;
//      if ( Pbottom->ArgVarType(StateVar) == TRUE )   return FALSE;
      if ( Pint == 0 )                               return FALSE;
      if ( ProdDissip == 0 )                         return FALSE;
//      if ( ProdDissip->ArgVarType(StateVar) == TRUE )return FALSE;
   }
   return TRUE;
}


BOOLEAN LAKECOMP::AddPointInf(PINF* pinf)
{
   if ( pinf == 0 )                             return FALSE;
   if ( ExistPointInf(pinf->Symbol()) == TRUE ) return FALSE;
   if ( pinflist.Add(pinf) == FALSE )
      FatalError("LAKECOMP::AddPointInf: cannot add point inflow");
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::ReplacePointInf(PINF* oldpinf, PINF* newpinf)
{
   if ( newpinf == 0 )                                         return FALSE;
   if ( ExistPointInf(oldpinf) == FALSE )                      return FALSE;
   if ( ( strcmp(oldpinf->Symbol(),newpinf->Symbol()) != 0 )
        &&          ( ExistPointInf(newpinf->Symbol()) == TRUE ) ) return FALSE;
   if ( pinflist.Remove(oldpinf) == FALSE )
      FatalError("LAKECOMP::ReplacePointInf: cannot remove old point inflow");
   if ( pinflist.Add(newpinf) == FALSE )
      FatalError("LAKECOMP::ReplacePointInf: cannot insert new point inflow");
   delete oldpinf;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::DeletePointInf(PINF* pinf)
{
   if ( pinf == 0 )  return FALSE;
   BOOLEAN b = pinflist.Remove(pinf);
   if ( b == FALSE ) return FALSE;
   delete pinf;
   CalcArg();
   return TRUE;
}


void LAKECOMP::DeletePointInf()
{
   PINF* pinf = pinflist.First();
   while ( pinf != 0 )
   {
      if ( pinflist.Remove(pinf) == FALSE )
         FatalError("LAKECOMP::DeletePointInf: unable to remove point inflow");
      delete pinf;
      pinf = pinflist.First();
   }
   CalcArg();
   return;
}


BOOLEAN LAKECOMP::AllowedReplaceVar(const AQVAR* oldvar,
                                    const AQVAR* newvar) const
{
   if ( AQCOMP::AllowedReplaceVar(oldvar,newvar) == FALSE )   return FALSE;
   PINF* pinf = pinflist.First();
   while ( pinf != 0 )
   {
      if ( pinf->AllowedReplaceVar(oldvar,newvar) == FALSE )  return FALSE;
      pinf = pinf->Next();
   }
   if ( A->Arg(oldvar) == TRUE )
   {
      if ( newvar->ArgVarType(StateVar) == TRUE )             return FALSE;
   }
   if ( D->Arg(oldvar) == TRUE )
   {
      if ( newvar->ArgVarType(StateVar) == TRUE )             return FALSE;
   }
   if ( TauSurf->Arg(oldvar) == TRUE )
   {
      if ( newvar->ArgVarType(StateVar) == TRUE )             return FALSE;
   }
/*
   if ( PressGrad->Arg(oldvar) == TRUE )
   {
      if ( newvar->ArgVarType(StateVar) == TRUE )             return FALSE;
   }
*/
   if ( Pwind->Arg(oldvar) == TRUE )
   {
      if ( newvar->ArgVarType(StateVar) == TRUE )             return FALSE;
   }
/*
   if ( Pbottom->Arg(oldvar) == TRUE )
   {
      if ( newvar->ArgVarType(StateVar) == TRUE )             return FALSE;
   }
   if ( ProdDissip->Arg(oldvar) == TRUE )
   {
      if ( newvar->ArgVarType(StateVar) == TRUE )             return FALSE;
   }
*/
   return TRUE;
}


BOOLEAN LAKECOMP::AllowedExchangeVar(const AQVAR* var1,
                                     const AQVAR* var2) const
{
   if ( AQCOMP::AllowedExchangeVar(var1,var2) == FALSE ) return FALSE;
   PINF* pinf = pinflist.First();
   while ( pinf != 0 )
   {
      if ( pinf->AllowedExchangeVar(var1,var2) == FALSE )
		                                                 return FALSE;
      pinf = pinf->Next();
   }
   if ( (A->Arg(var1)==TRUE) && (A->Arg(var2)==FALSE) )
   {
      if ( var2->ArgVarType(StateVar) == TRUE )          return FALSE;
   }
   if ( (A->Arg(var1)==FALSE) && (A->Arg(var2)==TRUE) )
   {
      if ( var1->ArgVarType(StateVar) == TRUE )          return FALSE;
   }
   if ( (TauSurf->Arg(var1)==TRUE) && (TauSurf->Arg(var2)==FALSE) )
   {
      if ( var2->ArgVarType(StateVar) == TRUE )          return FALSE;
   }
   if ( (TauSurf->Arg(var1)==FALSE) && (TauSurf->Arg(var2)==TRUE) )
   {
      if ( var1->ArgVarType(StateVar) == TRUE )          return FALSE;
   }
/*
   if ( (PressGrad->Arg(var1)==TRUE) && (PressGrad->Arg(var2)==FALSE) )
   {
      if ( var2->ArgVarType(StateVar) == TRUE )          return FALSE;
   }
   if ( (PressGrad->Arg(var1)==FALSE) && (PressGrad->Arg(var2)==TRUE) )
   {
      if ( var1->ArgVarType(StateVar) == TRUE )          return FALSE;
   }
*/
   if ( (Pwind->Arg(var1)==TRUE) && (Pwind->Arg(var2)==FALSE) )
   {
      if ( var2->ArgVarType(StateVar) == TRUE )          return FALSE;
   }
   if ( (Pwind->Arg(var1)==FALSE) && (Pwind->Arg(var2)==TRUE) )
   {
      if ( var1->ArgVarType(StateVar) == TRUE )          return FALSE;
   }
/*
   if ( (Pbottom->Arg(var1)==TRUE) && (Pbottom->Arg(var2)==FALSE) )
   {
      if ( var2->ArgVarType(StateVar) == TRUE )          return FALSE;
   }
   if ( (Pbottom->Arg(var1)==FALSE) && (Pbottom->Arg(var2)==TRUE) )
   {
      if ( var1->ArgVarType(StateVar) == TRUE )          return FALSE;
   }
   if ( (ProdDissip->Arg(var1)==TRUE) && (ProdDissip->Arg(var2)==FALSE) )
   {
      if ( var2->ArgVarType(StateVar) == TRUE )          return FALSE;
   }
   if ( (ProdDissip->Arg(var1)==FALSE) && (ProdDissip->Arg(var2)==TRUE) )
   {
      if ( var1->ArgVarType(StateVar) == TRUE )          return FALSE;
   }
*/
   return TRUE;
}


void LAKECOMP::ReplVar(AQVAR* oldvar, AQVAR* newvar)
{
   CARDINAL i;

   if ( AllowedReplaceVar(oldvar,newvar) == FALSE )
      FatalError("LAKECOMP::ReplVar: Illegal replace");
   AQCOMP::ReplVar(oldvar,newvar);
   for ( i=0; i<numinit; i++ )
   {
      if ( initvar[i] == oldvar ) initvar[i] = newvar;
      initval[i]->ReplaceVarArg(oldvar,newvar);
   }
   for ( i=0; i<numsurfinput; i++ )
   {
      if ( surfinputvar[i] == oldvar ) surfinputvar[i] = newvar;
      surfinputflux[i]->ReplaceVarArg(oldvar,newvar);
   }
   if ( latinflow != 0 ) latinflow->ReplaceVarArg(oldvar,newvar);
   for ( i=0; i<numlatinput; i++ )
   {
      if ( latinputvar[i] == oldvar ) latinputvar[i] = newvar;
      latinputconc[i]->ReplaceVarArg(oldvar,newvar);
   }
   PINF* pinf = pinflist.First();
   while ( pinf != 0 )
   {
      pinf->ReplVar(oldvar,newvar);
      pinf = pinf->Next();
   }
   for ( i=0; i<numsedinput; i++ )
   {
      if ( sedinputvar[i] == oldvar ) sedinputvar[i] = newvar;
      sedinputflux[i]->ReplaceVarArg(oldvar,newvar);
   }
   for ( i=0; i<numpartvar; i++ )
   {
      if ( partvar[i] == oldvar ) partvar[i] = newvar;
      partrho[i]->ReplaceVarArg(oldvar,newvar);
      partsedveloc[i]->ReplaceVarArg(oldvar,newvar);
      partmobility[i]->ReplaceVarArg(oldvar,newvar);
   }
   for ( i=0; i<numdissvar; i++ )
   {
      if ( dissvar[i] == oldvar ) dissvar[i] = newvar;
      disskdiff[i]->ReplaceVarArg(oldvar,newvar);
   }
   A->ReplaceVarArg(oldvar,newvar);
   Rho->ReplaceVarArg(oldvar,newvar);
   D->ReplaceVarArg(oldvar,newvar);
   Isurf->ReplaceVarArg(oldvar,newvar);
   Eta->ReplaceVarArg(oldvar,newvar);
   Fvol->ReplaceVarArg(oldvar,newvar);
   kres->ReplaceVarArg(oldvar,newvar);
   for ( i=0; i<numsedlayers; i++ )
   {
      sedlayers[i]->ReplaceVar(oldvar,newvar);
   }
   Pr->ReplaceVarArg(oldvar,newvar);
   TauSurf->ReplaceVarArg(oldvar,newvar);
   PressGrad->ReplaceVarArg(oldvar,newvar);
   Pwind->ReplaceVarArg(oldvar,newvar);
   Pbottom->ReplaceVarArg(oldvar,newvar);
   Pint->ReplaceVarArg(oldvar,newvar);
   ProdDissip->ReplaceVarArg(oldvar,newvar);
   CalcArg();
   return;
}


void LAKECOMP::ExchVar(AQVAR* var1, AQVAR* var2)
{
   CARDINAL i;

   if ( AllowedExchangeVar(var1,var2) == FALSE )
      FatalError("LAKECOMP::ExchVar: Illegal exchange");
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
   for ( i=0; i<numsurfinput; i++ )
   {
      if ( surfinputvar[i] == var1 )
      {
         surfinputvar[i] = var2;
      }
      else
      {
         if ( surfinputvar[i] == var2 ) surfinputvar[i] = var1;
      }
      surfinputflux[i]->ReplaceVarArg(var1,v);
      surfinputflux[i]->ReplaceVarArg(var2,var1);
      surfinputflux[i]->ReplaceVarArg(v,var2);
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
   PINF* pinf = pinflist.First();
   while ( pinf != 0 )
   {
      pinf->ExchVar(var1,var2);
      pinf = pinf->Next();
   }
   for ( i=0; i<numsedinput; i++ )
   {
      if ( sedinputvar[i] == var1 )
      {
         sedinputvar[i] = var2;
      }
      else
      {
         if ( sedinputvar[i] == var2 ) sedinputvar[i] = var1;
      }
      sedinputflux[i]->ReplaceVarArg(var1,v);
      sedinputflux[i]->ReplaceVarArg(var2,var1);
      sedinputflux[i]->ReplaceVarArg(v,var2);
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
      partsedveloc[i]->ReplaceVarArg(var1,v);
      partsedveloc[i]->ReplaceVarArg(var2,var1);
      partsedveloc[i]->ReplaceVarArg(v,var2);
      partmobility[i]->ReplaceVarArg(var1,v);
      partmobility[i]->ReplaceVarArg(var2,var1);
      partmobility[i]->ReplaceVarArg(v,var2);
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
      disskdiff[i]->ReplaceVarArg(var1,v);
      disskdiff[i]->ReplaceVarArg(var2,var1);
      disskdiff[i]->ReplaceVarArg(v,var2);
   }
   A->ReplaceVarArg(var1,v);
   A->ReplaceVarArg(var2,var1);
   A->ReplaceVarArg(v,var2);
   Rho->ReplaceVarArg(var1,v);
   Rho->ReplaceVarArg(var2,var1);
   Rho->ReplaceVarArg(v,var2);
   D->ReplaceVarArg(var1,v);
   D->ReplaceVarArg(var2,var1);
   D->ReplaceVarArg(v,var2);
   Isurf->ReplaceVarArg(var1,v);
   Isurf->ReplaceVarArg(var2,var1);
   Isurf->ReplaceVarArg(v,var2);
   Eta->ReplaceVarArg(var1,v);
   Eta->ReplaceVarArg(var2,var1);
   Eta->ReplaceVarArg(v,var2);
   Fvol->ReplaceVarArg(var1,v);
   Fvol->ReplaceVarArg(var2,var1);
   Fvol->ReplaceVarArg(v,var2);
   kres->ReplaceVarArg(var1,v);
   kres->ReplaceVarArg(var2,var1);
   kres->ReplaceVarArg(v,var2);
   for ( i=0; i<numsedlayers; i++ )
   {
      sedlayers[i]->ExchangeVar(var1,var2);
   }
   Pr->ReplaceVarArg(var1,v);
   Pr->ReplaceVarArg(var2,var1);
   Pr->ReplaceVarArg(v,var2);
   TauSurf->ReplaceVarArg(var1,v);
   TauSurf->ReplaceVarArg(var2,var1);
   TauSurf->ReplaceVarArg(v,var2);
   PressGrad->ReplaceVarArg(var1,v);
   PressGrad->ReplaceVarArg(var2,var1);
   PressGrad->ReplaceVarArg(v,var2);
   Pwind->ReplaceVarArg(var1,v);
   Pwind->ReplaceVarArg(var2,var1);
   Pwind->ReplaceVarArg(v,var2);
   Pbottom->ReplaceVarArg(var1,v);
   Pbottom->ReplaceVarArg(var2,var1);
   Pbottom->ReplaceVarArg(v,var2);
   Pint->ReplaceVarArg(var1,v);
   Pint->ReplaceVarArg(var2,var1);
   Pint->ReplaceVarArg(v,var2);
   ProdDissip->ReplaceVarArg(var1,v);
   ProdDissip->ReplaceVarArg(var2,var1);
   ProdDissip->ReplaceVarArg(v,var2);
   delete v;
   CalcArg();
   return;
}


BOOLEAN LAKECOMP::LatInflow(const VARLIST* varlist,
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


AQVAR* LAKECOMP::LatInputVar(CARDINAL index) const
{
   if ( index >= numlatinput )
      FatalError("LAKECOMP::LatInputVar: Illegal index");
   return latinputvar[index];
}


const FORMVAR* LAKECOMP::LatInputConc(CARDINAL index) const
{
   if ( index >= numlatinput )
      FatalError("LAKECOMP::LatInputConc: Illegal index");
   return latinputconc[index];
}


BOOLEAN LAKECOMP::AddLatInput(const VARLIST* varlist, AQVAR* var,
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


BOOLEAN LAKECOMP::ReplaceLatInput(const VARLIST* varlist, AQVAR* var,
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


BOOLEAN LAKECOMP::DeleteLatInput(CARDINAL pos)
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


AQVAR* LAKECOMP::SedInputVar(CARDINAL index) const
{
   if ( index >= numsedinput )
      FatalError("LAKECOMP::SedInputVar: Illegal index");
   return sedinputvar[index];
}


const FORMVAR* LAKECOMP::SedInputFlux(CARDINAL index) const
{
   if ( index >= numsedinput )
      FatalError("LAKECOMP::SedInputFlux: Illegal index");
   return sedinputflux[index];
}


BOOLEAN LAKECOMP::AddSedInput(const VARLIST* varlist, AQVAR* var,
                              const char* inpline, char* errorline,
                              CARDINAL& numparseerrors, CARDINAL pos)
{
   numparseerrors = 0;
   if ( InList() == TRUE )             return FALSE;
   if ( inpline == 0 )                 return FALSE;
   if ( inpline[0] == '\0' )           return FALSE;
   if ( varlist->Exist(var) == FALSE ) return FALSE;
   if ( pos > numsedinput ) pos = numsedinput;
   CARDINAL i;
   for ( i=0; i<numsedinput; i++ )
   {
      if ( sedinputvar[i] == var )     return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("SedInput");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                  return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                  return FALSE;
   }
   AQVAR**   newsedvar  = new AQVAR*[numsedinput+1];
   FORMVAR** newsedflux = new FORMVAR*[numsedinput+1];
   for ( i=0; i<pos; i++ )
   {
      newsedvar[i]  = sedinputvar[i];
      newsedflux[i] = sedinputflux[i];
   }
   newsedvar[pos]  = var;
   newsedflux[pos] = formvar;
   for ( i=pos; i<numsedinput; i++ )
   {
      newsedvar[i+1]  = sedinputvar[i];
      newsedflux[i+1] = sedinputflux[i];
   }
   delete sedinputvar;  sedinputvar  = newsedvar;
   delete sedinputflux; sedinputflux = newsedflux;
   numsedinput++;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::ReplaceSedInput(const VARLIST* varlist, AQVAR* var,
                                  const char* inpline, char* errorline,
                                  CARDINAL& numparseerrors, CARDINAL pos)
{
   numparseerrors = 0;
   if ( InList() == TRUE )                 return FALSE;
   if ( inpline == 0 )                     return FALSE;
   if ( inpline[0] == '\0' )               return FALSE;
   if ( varlist->Exist(var) == FALSE )     return FALSE;
   if ( pos >= numsedinput )               return FALSE;
   CARDINAL i;
   for ( i=0; i<numsedinput; i++ )
   {
      if ( i!=pos && sedinputvar[i]==var ) return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("SedInput");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                      return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                      return FALSE;
   }
   delete sedinputflux[pos];
   sedinputvar[pos]  = var;
   sedinputflux[pos] = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::DeleteSedInput(CARDINAL pos)
{
   if ( pos >= numsedinput ) return FALSE;
   AQVAR**   newsedvar  = new AQVAR*[numsedinput-1];
   FORMVAR** newsedflux = new FORMVAR*[numsedinput-1];
   CARDINAL i;
   for ( i=0; i<pos; i++ )
   {
      newsedvar[i]  = sedinputvar[i];
      newsedflux[i] = sedinputflux[i];
   }
   for ( i=pos+1; i<numsedinput; i++ )
   {
      newsedvar[i-1]  = sedinputvar[i];
      newsedflux[i-1] = sedinputflux[i];
   }
   delete sedinputflux[pos];
   delete sedinputvar;  sedinputvar  = newsedvar;
   delete sedinputflux; sedinputflux = newsedflux;
   numsedinput--;
   CalcArg();
   return TRUE;
}


AQVAR* LAKECOMP::DissVar(CARDINAL index) const
{
   if ( index >= numdissvar )
      FatalError("LAKECOMP::DissVar: Illegal index");
   return dissvar[index];
}


const FORMVAR* LAKECOMP::Disskdiff(CARDINAL index) const
{
   if ( index >= numdissvar )
      FatalError("LAKECOMP::Disskdiff: Illegal index");
   return disskdiff[index];
}


BOOLEAN LAKECOMP::AddDissVar(const VARLIST* varlist, 
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
   formvar->Symbol("kdiff");
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
   FORMVAR** newdisskdiff = new FORMVAR*[numdissvar+1];
   for ( i=0; i<pos; i++ )
   {
      newdissvar[i] = dissvar[i];
      newdisskdiff[i] = disskdiff[i];
   }
   newdissvar[pos] = var;
   newdisskdiff[pos] = formvar;
   for ( i=pos; i<numdissvar; i++ )
   {
      newdissvar[i+1] = dissvar[i];
      newdisskdiff[i+1] = disskdiff[i];
   }
   delete dissvar; dissvar = newdissvar;
   delete disskdiff; disskdiff = newdisskdiff;
   numdissvar++;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::ReplaceDissVar(const VARLIST* varlist, 
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
   formvar->Symbol("kdiff");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                      return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                      return FALSE;
   }
   delete disskdiff[pos];
   dissvar[pos] = var;
   disskdiff[pos] = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::DeleteDissVar(CARDINAL pos)
{
   if ( pos >= numdissvar ) return FALSE;
   AQVAR**   newdissvar = new AQVAR*[numdissvar-1];
   FORMVAR** newdisskdiff = new FORMVAR*[numdissvar-1];
   CARDINAL i;
   for ( i=0; i<pos; i++ )
   {
      newdissvar[i] = dissvar[i];
      newdisskdiff[i] = disskdiff[i];
   }
   for ( i=pos+1; i<numdissvar; i++ )
   {
      newdissvar[i-1] = dissvar[i];
      newdisskdiff[i-1] = disskdiff[i];
   }
   delete disskdiff[pos];
   delete dissvar; dissvar = newdissvar;
   delete disskdiff; disskdiff = newdisskdiff;
   numdissvar--;
   CalcArg();
   return TRUE;
}


AQVAR* LAKECOMP::PartVar(CARDINAL index) const
{
   if ( index >= numpartvar )
      FatalError("LAKECOMP::PartVar: Illegal index");
   return partvar[index];
}


const FORMVAR* LAKECOMP::PartRho(CARDINAL index) const
{
   if ( index >= numpartvar )
      FatalError("LAKECOMP::PartRho: Illegal index");
   return partrho[index];
}


const FORMVAR* LAKECOMP::PartSedVeloc(CARDINAL index) const
{
   if ( index >= numpartvar )
      FatalError("LAKECOMP::PartSedVeloc: Illegal index");
   return partsedveloc[index];
}


const FORMVAR* LAKECOMP::PartMobility(CARDINAL index) const
{
   if ( index >= numpartvar )
      FatalError("LAKECOMP::PartMobility: Illegal index");
   return partmobility[index];
}


BOOLEAN LAKECOMP::AddPartVar(const VARLIST* varlist, 
                             AQVAR* var,
                             const char* inprho, 
                             char* errorlinerho,
                             CARDINAL& parseerrorsrho, 
                             const char* inpsedvel, 
                             char* errorlinesedvel,
                             CARDINAL& parseerrorssedvel, 
                             const char* inpmobility, 
                             char* errorlinemobility,
                             CARDINAL& parseerrorsmobility, 
                             CARDINAL pos)
{
   parseerrorsrho = 0;
   parseerrorssedvel = 0;
   if ( InList() == TRUE )             return FALSE;
   if ( inprho == 0 )                  return FALSE;
   if ( inprho[0] == '\0' )            return FALSE;
   if ( inpsedvel == 0 )               return FALSE;
   if ( inpsedvel[0] == '\0' )         return FALSE;
   if ( inpmobility == 0 )             return FALSE;
   if ( inpmobility[0] == '\0' )       return FALSE;
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
   FORMVAR* varsedvel = new FORMVAR;
   varsedvel->Symbol("wsed");
   parseerrorssedvel = varsedvel->Parse(inpsedvel,varlist,errorlinesedvel);
   if ( parseerrorssedvel != 0 )
   {
      delete varrho; delete varsedvel; return FALSE;
   }
   if ( AllowedVar(varsedvel) == FALSE )
   {
      delete varrho; delete varsedvel; return FALSE;
   }
   FORMVAR* varmobility = new FORMVAR;
   varmobility->Symbol("mobility");
   parseerrorsmobility = varmobility->Parse(inpmobility,varlist,errorlinemobility);
   if ( parseerrorsmobility != 0 )
   {
      delete varrho; delete varsedvel; delete varmobility; return FALSE;
   }
   if ( AllowedVar(varmobility) == FALSE )
   {
      delete varrho; delete varsedvel; delete varmobility; return FALSE;
   }
   AQVAR**   newpartvar = new AQVAR*[numpartvar+1];
   FORMVAR** newpartrho = new FORMVAR*[numpartvar+1];
   FORMVAR** newpartsedveloc = new FORMVAR*[numpartvar+1];
   FORMVAR** newpartmobility = new FORMVAR*[numpartvar+1];
   for ( i=0; i<pos; i++ )
   {
      newpartvar[i] = partvar[i];
      newpartrho[i] = partrho[i];
      newpartsedveloc[i] = partsedveloc[i];
      newpartmobility[i] = partmobility[i];
   }
   newpartvar[pos] = var;
   newpartrho[pos] = varrho;
   newpartsedveloc[pos] = varsedvel;
   newpartmobility[pos] = varmobility;
   for ( i=pos; i<numpartvar; i++ )
   {
      newpartvar[i+1] = partvar[i];
      newpartrho[i+1] = partrho[i];
      newpartsedveloc[i+1] = partsedveloc[i];
      newpartmobility[i+1] = partmobility[i];
   }
   delete partvar; partvar = newpartvar;
   delete partrho; partrho = newpartrho;
   delete partsedveloc; partsedveloc = newpartsedveloc;
   delete partmobility; partmobility = newpartmobility;
   numpartvar++;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::ReplacePartVar(const VARLIST* varlist, 
                                 AQVAR* var,
                                 const char* inprho, 
                                 char* errorlinerho,
                                 CARDINAL& parseerrorsrho, 
                                 const char* inpsedvel, 
                                 char* errorlinesedvel,
                                 CARDINAL& parseerrorssedvel, 
                                 const char* inpmobility, 
                                 char* errorlinemobility,
                                 CARDINAL& parseerrorsmobility, 
                                 CARDINAL pos)
{
   parseerrorsrho = 0;
   parseerrorssedvel = 0;
   if ( InList() == TRUE )                 return FALSE;
   if ( inprho == 0 )                      return FALSE;
   if ( inprho[0] == '\0' )                return FALSE;
   if ( inpsedvel == 0 )                   return FALSE;
   if ( inpsedvel[0] == '\0' )             return FALSE;
   if ( inpmobility == 0 )                 return FALSE;
   if ( inpmobility[0] == '\0' )           return FALSE;
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
   FORMVAR* varsedvel = new FORMVAR;
   varsedvel->Symbol("wsed");
   parseerrorssedvel = varsedvel->Parse(inpsedvel,varlist,errorlinesedvel);
   if ( parseerrorssedvel != 0 )
   {
      delete varsedvel;                   return FALSE;
   }
   if ( AllowedVar(varsedvel) == FALSE )
   {
      delete varsedvel;                   return FALSE;
   }
   FORMVAR* varmobility = new FORMVAR;
   varmobility->Symbol("mobility");
   parseerrorsmobility = varmobility->Parse(inpmobility,varlist,errorlinemobility);
   if ( parseerrorsmobility != 0 )
   {
      delete varmobility;                 return FALSE;
   }
   if ( AllowedVar(varmobility) == FALSE )
   {
      delete varmobility;                 return FALSE;
   }
   delete partrho[pos];
   delete partsedveloc[pos];
   delete partmobility[pos];
   partvar[pos] = var;
   partrho[pos] = varrho;
   partsedveloc[pos] = varsedvel;
   partmobility[pos] = varmobility;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::DeletePartVar(CARDINAL pos)
{
   if ( pos >= numpartvar ) return FALSE;
   AQVAR**   newpartvar = new AQVAR*[numpartvar-1];
   FORMVAR** newpartrho = new FORMVAR*[numpartvar-1];
   FORMVAR** newpartsedveloc = new FORMVAR*[numpartvar-1];
   FORMVAR** newpartmobility = new FORMVAR*[numpartvar-1];
   CARDINAL i;
   for ( i=0; i<pos; i++ )
   {
      newpartvar[i] = partvar[i];
      newpartrho[i] = partrho[i];
      newpartsedveloc[i] = partsedveloc[i];
      newpartmobility[i] = partmobility[i];
   }
   for ( i=pos+1; i<numpartvar; i++ )
   {
      newpartvar[i-1] = partvar[i];
      newpartrho[i-1] = partrho[i];
      newpartsedveloc[i-1] = partsedveloc[i];
      newpartmobility[i-1] = partmobility[i];
   }
   delete partrho[pos];
   delete partsedveloc[pos];
   delete partmobility[pos];
   delete partvar; partvar = newpartvar;
   delete partrho; partrho = newpartrho;
   delete partsedveloc; partsedveloc = newpartsedveloc;
   delete partmobility; partmobility = newpartmobility;
   numpartvar--;
   CalcArg();
   return TRUE;
}


CARDINAL LAKECOMP::NumEq() const
{
   CARDINAL num;
   CARDINAL cell = ioff+(1+ised*numsedlayers)*NumStateVar();
   if ( CalcTKE() == TRUE )
   {
      num = NumGridPts()*cell+1;
   }
   else
   {
      num = NumGridPts()*cell;
   }
   return num;
}


REAL LAKECOMP::RelAcc(CARDINAL i) const
{
   if ( i >= NumEq() ) FatalError("LAKECOMP::RelAcc: Illegal index");
   CARDINAL cell = ioff+(1+ised*numsedlayers)*NumStateVar();
   if ( CalcTKE() == TRUE )
   {
      if ( i == numgrid*cell ) return relaccEseiche;
   }
   CARDINAL index = i % cell;
   if ( index == 0 ) return relaccQ;
   if ( index == 1 ) return relaccI;
   if ( CalcTKE() == TRUE )
   {
      if ( index == 2 ) return relaccU;
      if ( index == 3 ) return relaccTKE;
      if ( index == 4 ) return relaccEPS;
   }
   CARDINAL ind2 = (index-ioff) % NumStateVar();
   return statevar[ind2]->RelAccuracy();
}


REAL LAKECOMP::AbsAcc(CARDINAL i) const
{
   if ( i >= NumEq() ) FatalError("LAKECOMP::AbsAcc: Illegal index");
   CARDINAL cell = ioff+(1+ised*numsedlayers)*NumStateVar();
   if ( CalcTKE() == TRUE )
   {
      if ( i == numgrid*cell ) return absaccEseiche;
   }
   CARDINAL index = i % cell;
   if ( index == 0 ) return absaccQ;
   if ( index == 1 ) return absaccI;
   if ( CalcTKE() == TRUE )
   {
      if ( index == 2 ) return absaccU;
      if ( index == 3 ) return absaccTKE;
      if ( index == 4 ) return absaccEPS;
   }
   CARDINAL ind2 = (index-ioff) % NumStateVar();
   return statevar[ind2]->AbsAccuracy();
}


EQTYPE LAKECOMP::EqType(CARDINAL i) const
{
   if ( i >= NumEq() ) FatalError("LAKECOMP::EqType: Illegal index");
   CARDINAL cell = ioff+(1+ised*numsedlayers)*NumStateVar();
   if ( CalcTKE() == TRUE )
   {
      if ( i == numgrid*cell ) return DIFF;    // Eseiche
   }
   CARDINAL index = i % cell;

   if ( index >= ioff+NumStateVar() )  return DIFF; // Sediment

   if ( index >= ioff+numdynstatevar ) return ALG;  // EquStatevar
   if ( index >= ioff+numvolstatevar ) return DIFF; // SurfStatevar

   if ( i < ioff+numstatevar )             // Upstream boundary cond.:
   {
      if ( i == 0 ) return ALG;            // Q
      if ( i == 1 ) return ALG;            // I
      if ( CalcTKE() == TRUE )
      {
         if ( i == 2 ) return ALG;         // U
         if ( i == 3 ) return ALG;         // e
         if ( i == 4 ) return ALG;         // eps
      }
      return ALG;                          // VolStatevar
   }

   if ( i >= (numgrid-1)*cell ) // Downstream boundary cond.:
   {
      if ( index == 0 ) return ALG;        // Q
      if ( index == 1 ) return ALG;        // I
      if ( CalcTKE() == TRUE )
      {
         if ( index == 2 ) return ALG;     // U
         if ( index == 3 ) return ALG;     // e
         if ( index == 4 ) return ALG;     // eps
      }
      return ALG;                          // VolStatevar
   }
   else                                    // Equations in cells:
   {
      if ( index == 0 ) return ALG;        // Q
      if ( index == 1 ) return ALG;        // I
      if ( CalcTKE() == TRUE )
      {
         if ( index == 2 ) return DIFF;    // U
         if ( index == 3 ) return DIFF;    // e
         if ( index == 4 ) return DIFF;    // eps
      }
      return DIFF;                         // VolStatevar
   }
}


BOOLEAN LAKECOMP::CalcAreas(VARSYS* varsys,
                            REAL* areas, 
                            REAL* sedareas,
                            REAL* sedareasproj)
{
   // definitions must be compatible with GridValue!!
   CARDINAL j;
   for ( j=0; j<NumGridPts(); j++ )
   {
      if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[j]);
      varsys->Reset();
      areas[j] = A->Value();
      if ( areas[j] <= 0.0 )
      {
         *cjob << "\n   LAKECOMP numerical problem:"
               << " cross-sectional area negative or zero";
         cjob->flush();
         return FALSE;
      }
      if ( j > 0 )
      {
         if ( areas[j] > areas[j-1] )
         {
            *cjob << "\n   LAKECOMP numerical problem:"
                  << " cross-sectional area increases with water depth";
            cjob->flush();
            return FALSE;
         }
      }
   }

   sedareasproj[1] = areas[0] - 0.5*(areas[1]+areas[2]);
   sedareasproj[0] = sedareasproj[1];
   for ( j=2; j<numgrid-2; j++ )
   {
      sedareasproj[j] = 0.5*(areas[j-1]-areas[j+1]);
   }
   sedareasproj[numgrid-2] = 0.5*(areas[numgrid-3]+areas[numgrid-2])
                                                   -areas[numgrid-1];
   sedareasproj[numgrid-1] = areas[numgrid-1];

   REAL dz = gridpts[2]-gridpts[1];
   for ( j=1; j<numgrid-1; j++ )
   {
	   if ( sedareasproj[j] > 2*sqrt(3.14*areas[j])*dz )
       {
          sedareas[j] = sedareasproj[j];
       }
       else
       {
          sedareas[j] = 2*sqrt(3.14*areas[j])*dz;
       }
   }
   sedareas[0] = sedareas[1];
   sedareas[numgrid-1] = sedareasproj[numgrid-1];

   return TRUE;
}


BOOLEAN LAKECOMP::InitCond(VARSYS* varsys, REAL* Y, CARDINAL callnum)
{
   if ( Active() != TRUE ) return FALSE;
   BOOLEAN found;
   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   REAL zoneindex = 0.0; varsys->SetProgVar(ZoneIndex,&zoneindex);
   Zvar = varsys->GetProgVar(SpaceZ);
   Avar = varsys->GetProgVar(CrossSection);
   Qvar = varsys->GetProgVar(Discharge);
   Rhovar = varsys->GetProgVar(::Density);
   dAdzvar = varsys->GetProgVar(AreaGradient);
   Ivar = varsys->GetProgVar(LightIntensity);
   N2var  = varsys->GetProgVar(BruntVaisala);
   ShearPvar = varsys->GetProgVar(ShearProd);
   BuoyancyPvar = varsys->GetProgVar(BuoyancyProd);
   CARDINAL cell = ioff+(1+ised*numsedlayers)*NumStateVar();

   if ( callnum>0 ) return TRUE;
   REAL* y = &Y[ind_y];
   CARDINAL i,j,k;

   REAL* conc         = new REAL[numstatevar];
   REAL* areas        = new REAL[numgrid];
   REAL* sedareas     = new REAL[numgrid];
   REAL* sedareasproj = new REAL[numgrid];
   REAL* Qs           = new REAL[numgrid];
   REAL* rhos         = new REAL[numgrid];
   REAL* dAdzs        = new REAL[numgrid];
   REAL* N2s          = new REAL[numgrid];

   for ( i=0; i<numstatevar; i++ ) statevar[i]->SetStateVar(&conc[i]);

   // initialize concentrations in the water column:
   // ----------------------------------------------

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

   // initialize concentrations in the sediment:
   // ------------------------------------------

   if ( CalcSed()==TRUE && numsedlayers>0 )
   {
      for ( CARDINAL layer=0; layer<numsedlayers; layer++ )
      {
         zoneindex = sedlayers[layer]->ZoneIndex();
         for ( i=0; i<numstatevar; i++ )         // Concentrations C
         {
            found = FALSE;
            for ( j=0; j<NumInitCond(); j++ )
            {
               if ( initvar[j]==statevar[i] && initzone[j]==layer+1 )
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
                  y[j*cell+ioff+(layer+1)*NumStateVar()+i] = initval[k]->Value();
               }
               else
               {
                  y[j*cell+ioff+(layer+1)*NumStateVar()+i] = 0.0;
               }
            }
         }
      }
   }

   // cross-sectional area and density
   // --------------------------------

   if ( CalcAreas(varsys,areas,sedareas,sedareasproj) == FALSE )
      return FALSE;
   for ( j=0; j<NumGridPts(); j++ )
   {
      for ( i=0; i<numstatevar; i++ ) conc[i] = y[j*cell+ioff+i];
      varsys->Reset();
      if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[j]);
      Qs[j]    = 0.0;
      rhos[j] = Rho->Value();   // Rho may depend on concentrations
      if ( rhos[j] <= 0.0 )
      {
         *cjob << "\n   LAKECOMP numerical problem:"
               << " density negative or zero";
         cjob->flush();
         return FALSE;
      }
      y[j*cell]   = 0.0;   // Q    (calculation in Delta)
      y[j*cell+1] = 0.0;   // I    (calculation in Delta)
      if ( CalcTKE() == TRUE )
      {
         y[j*cell+2] = 0.0;   // U
         y[j*cell+3] = 0.0;   // e
         y[j*cell+4] = 0.0;   // eps
      }
   }
   // N2:
   N2s[0] = gravit/rhos[0]*(rhos[1]-rhos[0])/fabs(gridpts[1]-gridpts[0]);
   dAdzs[0] = (areas[1]-areas[0])/(gridpts[1]-gridpts[0]);
   for ( j=1; j<numgrid-1; j++ )
   {
      REAL d1 = fabs(gridpts[j]-gridpts[j-1]);
      REAL d2 = fabs(gridpts[j+1]-gridpts[j]);
      N2s[j] = gravit/rhos[j]
               *(d1*d1*rhos[j+1]-d2*d2*rhos[j-1]
                        -(d1*d1-d2*d2)*rhos[j])
               /(d1*d2*(d1+d2));
      dAdzs[j] = (areas[j+1]-areas[j-1])/(gridpts[j+1]-gridpts[j-1]);
   }
   N2s[numgrid-1] = gravit/rhos[numgrid-1]*(rhos[numgrid-1]-rhos[numgrid-2])
                    /fabs(gridpts[numgrid-1]-gridpts[numgrid-2]);
   dAdzs[numgrid-1] = (areas[numgrid-1]-areas[numgrid-2])
                      /(gridpts[numgrid-1]-gridpts[numgrid-2]);

   // initialize TKE submodel:
   // ------------------------

   zoneindex = 0.0;
   if ( CalcTKE() == TRUE )
   {
      UHvar  = varsys->GetProgVar(HorizVelocity);
      TKEvar = varsys->GetProgVar(TurbKinEnergy);
      EPSvar = varsys->GetProgVar(Dissipation);

      if ( TKEvar != 0 ) // TKE must be before UH because the boundary
      {                  // cond. for UH can depend on TKE (through D)
         found = FALSE;
         for ( j=0; j<numinit; j++ )
         {
            if ( initvar[j]==TKEvar && initzone[j]==0 )
            {
               found = TRUE; k = j; break;
            }
         }
         if ( found == TRUE )
         {
            for ( j=0; j<NumGridPts(); j++ )
            {
               varsys->Reset();
               if ( Zvar != 0 )   Zvar->SetProgVar(&gridpts[j]);
               if ( Avar != 0 )   Avar->SetProgVar(&areas[j]);
               if ( Qvar != 0 )   Qvar->SetProgVar(&Qs[j]);
               if ( Rhovar != 0 ) Rhovar->SetProgVar(&rhos[j]);
               y[j*cell+3] = initval[k]->Value();
            }
         }
      }
      y[3] = y[cell+3];                                // surf. bound. cond.
      y[(numgrid-1)*cell+3] = y[(numgrid-2)*cell+3];   // bottom bound. cond.

      if ( EPSvar != 0 ) // EPS must be before UH because the boundary
      {                  // cond. for UH can depend on EPS (through D)
         found = FALSE;
         for ( j=0; j<numinit; j++ )
         {
            if ( initvar[j]==EPSvar && initzone[j]==0 )
            {
               found = TRUE; k = j; break;
            }
         }
         if ( found == TRUE )
         {
            for ( j=0; j<NumGridPts(); j++ )
            {
               varsys->Reset();
               if ( Zvar != 0 )   Zvar->SetProgVar(&gridpts[j]);
               if ( Avar != 0 )   Avar->SetProgVar(&areas[j]);
               if ( Qvar != 0 )   Qvar->SetProgVar(&Qs[j]);
               if ( Rhovar != 0 ) Rhovar->SetProgVar(&rhos[j]);
               y[j*cell+4] = initval[k]->Value();
            }
         }
      }
      y[4] = y[cell+4];                                // surf. bound. cond.
      y[(numgrid-1)*cell+4] = y[(numgrid-2)*cell+4];   // bottom bound. cond.

      if ( UHvar != 0 )
      {
         found = FALSE;
         for ( j=0; j<numinit; j++ )
         {
            if ( initvar[j]==UHvar && initzone[j]==0 )
            {
               found = TRUE; k = j; break;
            }
         }
         if ( found == TRUE )
         {
            for ( j=0; j<NumGridPts(); j++ )
            {
               varsys->Reset();
               if ( Zvar != 0 )   Zvar->SetProgVar(&gridpts[j]);
               if ( Avar != 0 )   Avar->SetProgVar(&areas[j]);
               if ( Qvar != 0 )   Qvar->SetProgVar(&Qs[j]);
               if ( Rhovar != 0 ) Rhovar->SetProgVar(&rhos[j]);
               y[j*cell+2] = initval[k]->Value();
            }
         }
      }
      varsys->Reset();
      if ( Zvar   != 0 ) Zvar->SetProgVar(&gridpts[0]); // surf. bound. cond.
      if ( Avar   != 0 ) Avar->SetProgVar(&areas[0]);
      if ( Qvar   != 0 ) Qvar->SetProgVar(&Qs[0]);
      if ( dAdzvar != 0 ) dAdzvar->SetProgVar(&dAdzs[0]);
      if ( N2var  != 0 ) N2var->SetProgVar(&N2s[0]);
      if ( TKEvar != 0 ) TKEvar->SetProgVar(&y[2]);
      if ( EPSvar != 0 ) EPSvar->SetProgVar(&y[3]);
      y[2] = y[cell+2]+fabs(gridpts[1]-gridpts[0])*TauSurf->Value()
                       /(rhos[0]*D->Value()*Pr->Value());
      y[(numgrid-1)*cell+2] = 0;                        // bottom bound. cond.

      REAL Eseiche = 0.0;
      Eseichevar = varsys->GetProgVar(SeicheEnergy);
      if ( Eseichevar != 0 )
      {
         for ( j=0; j<numinit; j++ )
         {
            if ( initvar[j]==Eseichevar && initzone[j]==0 )
            {
               varsys->Reset();
               Eseiche = initval[j]->Value(); break;
            }
         }
      }
      y[numgrid*cell]   = Eseiche;         // Eseiche
   }

   delete [] conc;
   delete [] areas;
   delete [] sedareas;
   delete [] sedareasproj;
   delete [] Qs;
   delete [] rhos;
   delete [] dAdzs;
   delete [] N2s;

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


REAL LAKECOMP::FluxLimit(REAL s1, REAL s2, CARDINAL limiter)
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


BOOLEAN LAKECOMP::Delta(const NUMPAR&, VARSYS* varsys,
                        const REAL* Y, const REAL* YT, REAL* DELTA)
{
   if ( Active() != TRUE ) return FALSE;
   const REAL* y  = &Y[ind_y];
   const REAL* yt = &YT[ind_y];
   REAL* delta = &DELTA[ind_y];
   REAL zero = 0.0;
   CARDINAL i,j,k;
   CARDINAL cell = ioff+(1+ised*numsedlayers)*NumStateVar();
   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   REAL zoneindex = 0.0; varsys->SetProgVar(ZoneIndex,&zoneindex);
   REAL eps = 1.0;       varsys->SetProgVar(WaterFraction,&eps);
   Zvar    = varsys->GetProgVar(SpaceZ);
   Avar    = varsys->GetProgVar(CrossSection);
   Qvar    = varsys->GetProgVar(Discharge);
   Rhovar  = varsys->GetProgVar(::Density);
   dAdzvar = varsys->GetProgVar(AreaGradient);
   Ivar    = varsys->GetProgVar(LightIntensity);
   N2var   = varsys->GetProgVar(BruntVaisala);
   UHvar   = varsys->GetProgVar(HorizVelocity);
   TKEvar  = varsys->GetProgVar(TurbKinEnergy);
   ShearPvar = varsys->GetProgVar(ShearProd);
   BuoyancyPvar = varsys->GetProgVar(BuoyancyProd);
   EPSvar  = varsys->GetProgVar(Dissipation);
   Eseichevar = varsys->GetProgVar(SeicheEnergy);
   REAL* conc         = new REAL[numstatevar];
   REAL* flux         = new REAL[numvolstatevar];
   REAL* sedflux      = new REAL[numvolstatevar];
   REAL* areas        = new REAL[numgrid];
   REAL* sedareas     = new REAL[numgrid];
   REAL* sedareasproj = new REAL[numgrid];
   REAL* rhos         = new REAL[numgrid];
   REAL* dAdzs        = new REAL[numgrid];
   REAL* N2s          = new REAL[numgrid];
   REAL* Ds           = new REAL[numgrid];
   REAL* etas         = new REAL[numgrid];
   REAL* Prs          = new REAL[numgrid];
   REAL* ShearPs      = new REAL[numgrid];
   REAL* BuoyancyPs   = new REAL[numgrid];
   REAL* pints        = new REAL[numgrid];
   REAL* Qinterf      = new REAL[numgrid-1];
   REAL* qlat         = new REAL[numgrid];
   REAL* bottominp    = new REAL[numvolstatevar];
   BOOLEAN* part      = new BOOLEAN[numvolstatevar];
   CARDINAL* ind      = new CARDINAL[numvolstatevar];
   REAL** wsed        = new REAL*[numgrid];
   for ( j=0; j<numgrid; j++ ) wsed[j] = new REAL[numvolstatevar];
   REAL* rhopart = new REAL[numvolstatevar];
   REAL** kdiffs  = new REAL*[numgrid];
   for ( j=0; j<numgrid; j++ ) kdiffs[j] = new REAL[numvolstatevar];
   REAL* resvels  = new REAL[numgrid];
   REAL* fvols    = new REAL[numgrid];
   REAL** fvoltot = new REAL*[numgrid];
   for ( j=0; j<numgrid; j++ ) fvoltot[j] = new REAL[numsedlayers+1];
   REAL** hseds = new REAL*[numgrid];
   for ( j=0; j<numgrid; j++ ) hseds[j] = new REAL[numsedlayers];
   REAL** pors  = new REAL*[numgrid];
   for ( j=0; j<numgrid; j++ ) pors[j] = new REAL[numsedlayers];
   REAL uh = 0.0;
   if ( UHvar != 0 )  UHvar->SetProgVar(&uh);
   REAL tke = 0.0;
   if ( TKEvar != 0 ) TKEvar->SetProgVar(&tke);
   REAL epsilon = 0.0;
   if ( EPSvar != 0 ) EPSvar->SetProgVar(&epsilon);
   REAL dz    = fabs(gridpts[2]-gridpts[1]);  // cells of equal length
   REAL dzinv = 1.0/dz;                       // inverse zell length

   // calculate areas:
   // ----------------

   if ( CalcAreas(varsys,areas,sedareas,sedareasproj) == FALSE )
      return FALSE;

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
         found = FALSE;
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
      bottominp[i] = 0;
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

   if ( Eseichevar != 0 )
   {
      if ( CalcTKE() == TRUE )
      {
         Eseichevar->SetProgVar(&y[numgrid*cell]);
	  }
	  else
	  {
         Eseichevar->SetProgVar(&zero);
	  }
   }

   for ( i=0; i<numstatevar; i++ ) statevar[i]->SetStateVar(&conc[i]);

   for ( j=0; j<numgrid-1; j++ ) Qinterf[j] = 0.0;

   // calculate surface input fluxes:
   // -------------------------------

   if ( Zvar != 0 )   Zvar->SetProgVar(&gridpts[0]);
   if ( Qvar != 0 )   Qvar->SetProgVar(&y[0]);
   if ( Avar != 0 )   Avar->SetProgVar(&areas[0]);
   if ( Rhovar != 0 ) Rhovar->SetProgVar(&rhos[0]);
   if ( Ivar != 0 )   Ivar->SetProgVar(&y[1]);
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
   for ( i=0; i<numvolstatevar; i++ )
   {
      flux[i] = 0.0;
      for ( j=0; j<numsurfinput; j++ )
      {
         if ( surfinputvar[j] == statevar[i] )
         {
            flux[i] += surfinputflux[j]->Value()*areas[0]; break;
         }
      }
   }
   REAL pwind     = Pwind->Value();
   REAL pbottom   = Pbottom->Value();
   REAL surfshear = TauSurf->Value();
   REAL isurf     = Isurf->Value();

   // calculate rho, Kz, N2, eta:
   // ---------------------------

   zoneindex = 0.0;
   for ( j=0; j<NumGridPts(); j++ )
   {
      if ( CalcTKE() == TRUE )
      {
         uh = y[j*cell+2];
         tke = y[j*cell+3];
         epsilon = y[j*cell+4];
      }
      zoneindex = 0.0;
      varsys->Reset();
      if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[j]);
      for ( i=0; i<numvolstatevar; i++ )                    // VolStateVar
      {
         conc[i] = y[j*cell+ioff+i];
      }
      for ( i=numvolstatevar; i<numdynstatevar; i++ )       // SurfStateVar
      {
         conc[i] = y[j*cell+ioff+i];
      }
      for ( i=numdynstatevar; i<numstatevar; i++ )          // EquStateVar
      {
         conc[i] = y[j*cell+ioff+i];
      }
      rhos[j] = Rho->Value();
      if ( rhos[j] <= 0.0 )
      {
         *cjob << "\n   LAKECOMP numerical problem:"
               << " density negative or zero";
         cjob->flush();
         return FALSE;
      }
      etas[j] = Eta->Value();
   }
   N2s[0] = gravit/rhos[0]*(rhos[1]-rhos[0])/fabs(gridpts[1]-gridpts[0]);
   dAdzs[0] = (areas[1]-areas[0])/(gridpts[1]-gridpts[0]);
   if ( CalcTKE() == TRUE )
   {
      REAL dUdz = (y[cell+2]-y[2])/(gridpts[1]-gridpts[0]);
      ShearPs[0] = dUdz*dUdz;	    // multiplication with D*Pr below
      BuoyancyPs[0] = -N2s[0];	    // multiplication with D*Pr below
   }
   for ( j=1; j<numgrid-1; j++ )
   {
      REAL d1 = fabs(gridpts[j]-gridpts[j-1]);
      REAL d2 = fabs(gridpts[j+1]-gridpts[j]);
      N2s[j] = gravit/rhos[j]
               *(d1*d1*rhos[j+1]-d2*d2*rhos[j-1]
                        -(d1*d1-d2*d2)*rhos[j])
               /(d1*d2*(d1+d2));
      dAdzs[j] = (areas[j+1]-areas[j-1])/(gridpts[j+1]-gridpts[j-1]);
      if ( CalcTKE() == TRUE )
      {
         REAL dUdz = (y[(j+1)*cell+2]-y[(j-1)*cell+2])
                     /(gridpts[j+1]-gridpts[j-1]);
         ShearPs[j]    = dUdz*dUdz; // multiplication with D*Pr below
         BuoyancyPs[j] = -N2s[j];	// multiplication with D*Pr below
      }
   }
   N2s[numgrid-1] = gravit/rhos[numgrid-1]*(rhos[numgrid-1]-rhos[numgrid-2])
                    /fabs(gridpts[numgrid-1]-gridpts[numgrid-2]);
   dAdzs[numgrid-1] = (areas[numgrid-1]-areas[numgrid-2])
                      /(gridpts[numgrid-1]-gridpts[numgrid-2]);
   if ( CalcTKE() == TRUE )
   {
      REAL dUdz = (y[(numgrid-1)*cell+2]-y[(numgrid-2)*cell+2])
                  /(gridpts[numgrid-1]-gridpts[numgrid-2]);
      ShearPs[numgrid-1]    = dUdz*dUdz;	   // multiplication with D*Pr below
      BuoyancyPs[numgrid-1] = -N2s[numgrid-1]; // multiplication with D*Pr below
   }

   // calculate rates, lateral inflow and sediment input:
   // ---------------------------------------------------

   for ( j=0; j<NumGridPts(); j++ )
   {
      if ( Zvar != 0 )   Zvar->SetProgVar(&gridpts[j]);
      if ( Qvar != 0 )   Qvar->SetProgVar(&y[j*cell]);
      if ( Avar != 0 )   Avar->SetProgVar(&areas[j]);
      if ( Rhovar != 0 ) Rhovar->SetProgVar(&rhos[j]);
      if ( dAdzvar != 0 )  dAdzvar->SetProgVar(&dAdzs[j]);
      if ( Ivar != 0 )   Ivar->SetProgVar(&y[j*cell+1]);
      if ( N2var != 0 )  N2var->SetProgVar(&N2s[j]);
      if ( ShearPvar != 0 )  ShearPvar->SetProgVar(&ShearPs[j]);
      if ( BuoyancyPvar != 0 )  BuoyancyPvar->SetProgVar(&BuoyancyPs[j]);
      if ( CalcTKE() == TRUE )
      {
         uh = y[j*cell+2];
         tke = y[j*cell+3];
         epsilon = y[j*cell+4];
      }

      // water column:

      zoneindex = 0.0;
      varsys->Reset();
      for ( i=0; i<numvolstatevar; i++ )                    // VolStateVar
      {
         conc[i] = y[j*cell+ioff+i];
      }
      for ( i=numvolstatevar; i<numdynstatevar; i++ )       // SurfStateVar
      {
         conc[i] = y[j*cell+ioff+i];
      }
      for ( i=numdynstatevar; i<numstatevar; i++ )          // EquStateVar
      {
         conc[i] = y[j*cell+ioff+i];
      }

      Ds[j] = D->Value();  // calculation of D after N2!
      if ( Ds[j] <= 0.0 )
      {
         *cjob << "\n   LAKECOMP numerical problem:"
               << " Diffusion coefficient is not positive";
         cjob->flush();
         return FALSE;
      }
      if ( CalcTKE() == TRUE )
      {
         Prs[j] = Pr->Value();
         if ( Prs[j] <= 0.0 )
         {
            *cjob << "\n   LAKECOMP numerical problem:"
                  << " Prandtl number is not positive";
            cjob->flush();
            return FALSE;
         }
      }
      ShearPs[j]    = Ds[j]*Prs[j]*ShearPs[j];	  // now D and Pr avail.
      if ( Ds[j] > kzmin )
      {											  // now D and Pr avail.
         BuoyancyPs[j] = (Ds[j]-kzmin)*Prs[j]*BuoyancyPs[j];
      }
      else
      {
         BuoyancyPs[j] = 0.0;
      }
      if ( latinflow == 0 )
      {
         qlat[j] = 0.0;
      }
      else
      {
         qlat[j] = latinflow->Value();
		 if ( j < numgrid-1 )  // do not add interface flow for qlat at lake bottom
         {
		    for ( k=0; k<j; k++ ) Qinterf[k] += qlat[j]*dz;
         }
      }
      CalcRates(&delta[j*cell+ioff]);
      for ( i=0; i<numvolstatevar; i++ )
      {
         if ( qlat[j] > 0.0 )
         {
            for ( CARDINAL k=0; k<numlatinput; k++ )
            {
               if ( latinputvar[k] == statevar[i] )
               {
                  delta[j*cell+ioff+i] += qlat[j]*latinputconc[k]->Value()
                                          /areas[j];
               }
            }
         }
         else
         {
            delta[j*cell+ioff+i] += qlat[j]*conc[i]/areas[j]; // qlat<0 !
         }
         if ( CalcSed()==FALSE || numsedlayers==0 )  // evaluate in water column
         {
            for ( CARDINAL k=0; k<numsedinput; k++ )
            {
               if ( sedinputvar[k] == statevar[i] )
               {
                  if ( j < numgrid-1 )
                  {
                     delta[j*cell+ioff+i]
                        += sedinputflux[k]->Value()*sedareasproj[j]
                           *dzinv/areas[j];
                  }
                  else
                  {
                     bottominp[i] = sedinputflux[k]->Value(); // for use in
                     delta[j*cell+ioff+i]                     // bound. cond.
                        += bottominp[i]*sedareasproj[j]
                           *dzinv/areas[j];
                  }
               }
            }
         }
      }
      for ( i=0; i<numvolstatevar; i++ )
      {
         if ( part[i] == TRUE )
         {
            wsed[j][i] = partsedveloc[ind[i]]->Value();
			kdiffs[j][i] = partmobility[ind[i]]->Value();
         }
         else
         {
            wsed[j][i] = 0.0;
			if ( ind[i] == numvolstatevar )
            {
			   kdiffs[j][i] = 0.0;
            }
            else
            {
			   kdiffs[j][i] = disskdiff[ind[i]]->Value();
            }
         }
      }
	  resvels[j] = kres->Value();
      if ( resvels[j] < 0.0 )
      {
         *cjob << "\n   LAKECOMP numerical problem:"
               << " resuspension velocity is negative";
         cjob->flush();
         return FALSE;
      }
	  fvols[j] = Fvol->Value();
      if ( CalcTKE() == TRUE )
      {
         delta[j*cell+2] = PressGrad->Value();      // U

		 pints[j] = Pint->Value();
         delta[j*cell+3] = -y[j*cell+4]
                           +ShearPs[j]+BuoyancyPs[j]
                           +pbottom*sedareasproj[j]*dzinv/rhos[j]/areas[j]
                           +pints[j];
         REAL proddissip = ProdDissip->Value(); // prod of dissipation
         delta[j*cell+4] = proddissip;       // eps
      }

      // sediment:

      if ( CalcSed()==TRUE && numsedlayers>0 )
      {
         CARDINAL layer;
         if ( Ivar != 0 ) Ivar->SetProgVar(&zero);
         for ( layer=0; layer<numsedlayers; layer++ )
         {
            hseds[j][layer] = sedlayers[layer]->thickness->Value();
            pors[j][layer]  = sedlayers[layer]->porosity->Value();
         }
         fvoltot[j][0] = 0.0;
         for ( i=0; i<numvolstatevar; i++ )
         {
            if ( part[i] == TRUE )
            {
               if ( wsed[j][i] > 0 )   // condition wsed > 0 added 26.6.04
			   {
                  fvoltot[j][0] += conc[i]/rhopart[i]*wsed[j][i];
			   }
            }
         }
         fvoltot[j][0] += fvols[j];
         fvoltot[j][0] -= (1-pors[j][0])*resvels[j];
         for ( layer=0; layer<numsedlayers; layer++ )
         {
            fvoltot[j][layer+1] = fvoltot[j][layer];
            zoneindex = sedlayers[layer]->ZoneIndex();
            varsys->Reset();
            for ( i=0; i<numvolstatevar; i++ )              // VolStateVar
            {
               conc[i] = y[j*cell+ioff+(1+layer)*numstatevar+i];
            }
            for ( i=numvolstatevar; i<numdynstatevar; i++ ) // SurfStateVar
            {
               conc[i] = y[j*cell+ioff+(1+layer)*numstatevar+i];
            }
            for ( i=numdynstatevar; i<numstatevar; i++ )    // EquStateVar
            {
               conc[i] = y[j*cell+ioff+(1+layer)*numstatevar+i];
            }
            CalcRates(&delta[j*cell+ioff+(1+layer)*numstatevar]);
            for ( i=0; i<numvolstatevar; i++ )
            {
               if ( part[i] == TRUE )
               {
                  fvoltot[j][layer+1]
                     += hseds[j][layer]
                        *delta[j*cell+ioff+(layer+1)*numstatevar+i]
                        /rhopart[i];
               }
               else
               {
                  delta[j*cell+ioff+(1+layer)*numstatevar+i]
                     = delta[j*cell+ioff+(1+layer)*numstatevar+i]
                       /pors[j][layer];
               }
            }
         }
         for ( CARDINAL k=0; k<numsedinput; k++ )
         {
            zoneindex = sedlayers[numsedlayers-1]->ZoneIndex();
            varsys->Reset();
            if ( sedinputvar[k] == statevar[i] )
            {
               delta[j*cell+ioff+numsedlayers*numstatevar+i]
                  += sedinputflux[k]->Value()
                     /hseds[j][numsedlayers-1];
            }
         }
         zoneindex = 0;
         varsys->Reset();
      }
   }

   // calculate point inputs:
   // -----------------------

   PINF* pinf = pinflist.First();
   while ( pinf != 0 )
   {
      REAL depth = (pinf->Depth())->Value();
      REAL width = (pinf->Width())->Value();
	  REAL depthmin = depth-0.5*width;
	  REAL depthmax = depth+0.5*width;
      CARDINAL j0 = 0;
      REAL* infract = new REAL[numgrid];
	  infract[0] = 0; infract[numgrid-1] = 0;
      if ( gridpts[0] < gridpts[numgrid-1] )
      {
         if ( depthmin < gridpts[0] )         depthmin = gridpts[0];
         if ( depthmax > gridpts[numgrid-1] ) depthmax = gridpts[numgrid-1];
         REAL posrel = (depth-gridpts[0])/(gridpts[numgrid-1]-gridpts[0]);
         j0 = (CARDINAL) posrel*(numgrid-2)+1;
		 if ( j0 > numgrid-1 ) j0 = numgrid-1;
      }
      else
      {
         if ( depthmin < gridpts[numgrid-1] ) depthmin = gridpts[numgrid-1];
         if ( depthmax > gridpts[0] )         depthmax = gridpts[0];
         REAL posrel = (gridpts[0]-depth)/(gridpts[0]-gridpts[numgrid-1]);
         j0 = (CARDINAL) posrel*(numgrid-2)+1;
		 if ( j0 > numgrid-1 ) j0 = numgrid-1;
      }
      REAL range = depthmax - depthmin;
      for ( j=1; j<numgrid-1; j++ )
      {
         if ( depthmin > gridpts[j]+0.5*dz || depthmax < gridpts[j]-0.5*dz )
         {
            infract[j] = 0;
         }
         else
         {
            if ( depthmin < gridpts[j]-0.5*dz )
            {
               if ( depthmax > gridpts[j]+0.5*dz )
               {
                  infract[j] = dz/range;
               }
               else
               {
                  infract[j] = (depthmax-gridpts[j]+0.5*dz)/range;
               }
            }
            else
            {
               if ( depthmax > gridpts[j]+0.5*dz )
               {
                  infract[j] = (gridpts[j]+0.5*dz-depthmin)/range;
               }
               else
               {
                  infract[j] = 1.0;
               }
            }
         }
      }
      if ( Zvar != 0 )   Zvar->SetProgVar(&gridpts[j0]);
      if ( Qvar != 0 )   Qvar->SetProgVar(&y[j0*cell]);
      if ( Avar != 0 )   Avar->SetProgVar(&areas[j0]);
      if ( Rhovar != 0 ) Rhovar->SetProgVar(&rhos[j0]);
      if ( Ivar != 0 )   Ivar->SetProgVar(&y[j0*cell+1]);
      if ( N2var != 0 )  N2var->SetProgVar(&N2s[j0]);
      zoneindex = 0.0;
      varsys->Reset();
      for ( i=0; i<numvolstatevar; i++ )                    // VolStateVar
      {
         conc[i] = y[j0*cell+ioff+i];
      }
      for ( i=numvolstatevar; i<numdynstatevar; i++ )       // SurfStateVar
      {
         conc[i] = y[j0*cell+ioff+i];
      }
      for ( i=numdynstatevar; i<numstatevar; i++ )          // EquStateVar
      {
         conc[i] = y[j0*cell+ioff+i];
      }
      REAL Qpin = pinf->WaterFlow()->Value();
      for ( j=1; j<numgrid-1; j++ )
      {
         for ( i=0; i<j; i++ )
         {
            Qinterf[i] += Qpin*infract[j];
         }
      }
      for ( i=0; i<numvolstatevar; i++ )
      {
         if ( Qpin > 0 )  // inflow
         {
            for ( k=0; k<pinf->NumConc(); k++ )
            {
               if ( pinf->ConcVar(k) == statevar[i] )
               {
                  REAL c = pinf->ConcVal(k)->Value();
                  for ( j=1; j<numgrid-1; j++ )
                  {
                     delta[j*cell+ioff+i] += c*Qpin*infract[j]
                                             /(areas[j]*dz);
                  }
               }
            }
         }
         else             // outflow
         {
            for ( j=1; j<numgrid-1; j++ )
            {
               delta[j*cell+ioff+i] += y[j*cell+ioff+i]*Qpin*infract[j]
                                       /(areas[j]*dz);
            }
         }
      }
      delete [] infract;
      pinf = pinf->Next();
   }

   // exchange with sediment (sedimentation treated below):
   // -----------------------------------------------------

   if ( CalcSed()==TRUE && numsedlayers>0 )
   {
      for ( j=0; j<NumGridPts(); j++ )
      {
         for ( i=0; i<numvolstatevar; i++ )
         {
            CARDINAL layer = 0;
         	if ( part[i] == TRUE )
            {
               if ( wsed[j][i] > 0 )  // condition wsed > 0 added 26.6.04
			   {
                  delta[j*cell+ioff+numstatevar+i]
                     += y[j*cell+ioff+i]*wsed[j][i]/hseds[j][0];
			   }
               delta[j*cell+ioff+numstatevar+i]
				  -= resvels[j]/hseds[j][0]
                     *y[j*cell+ioff+numstatevar+i];
               if ( fvoltot[j][1] > 0 )
               {
                  delta[j*cell+ioff+numstatevar+i]
                     -= fvoltot[j][1]/(1-pors[j][0])
                        /hseds[j][0]
                        *y[j*cell+ioff+numstatevar+i];
               }
               else
               {
                  if ( numsedlayers > 1 )
                  {
                     delta[j*cell+ioff+numstatevar+i]
                        += fvoltot[j][1]/(1-pors[j][0])
                           /hseds[j][0]
                           *y[j*cell+ioff+2*numstatevar+i];
                  }
                  else
                  {
                     delta[j*cell+ioff+numstatevar+i]
                        += fvoltot[j][1]/(1-pors[j][0])
                           /hseds[j][0]
                           *y[j*cell+ioff+numstatevar+i];
                  }
               }
            }
            else  // dissolved, first layer
            {
               delta[j*cell+ioff+numstatevar+i]
                  += 1/(hseds[j][0]*pors[j][0])
                     *(kdiffs[j][i]*pors[j][0]
                       /(0.5*hseds[j][0])
                       *(y[j*cell+ioff+i]-y[j*cell+ioff+numstatevar+i]));
               if ( numsedlayers > 1 )
               {
                  delta[j*cell+ioff+numstatevar+i]
                     += 1/(hseds[j][0]*pors[j][0])
                        *kdiffs[j][i]*fmin(pors[j][0],pors[j][1])
                          /(0.5*(hseds[j][0]+hseds[j][1]))
                          *(y[j*cell+ioff+2*numstatevar+i]
                            -y[j*cell+ioff+numstatevar+i]);
               }
               // effect on water column (effect of bottom considered as 
               // boundary condition at grid point n-1):
               if ( j>0 && j<numgrid-1 )
               {
                  delta[j*cell+ioff+i]
                     -= sedareas[j]/areas[j]*dzinv
                        *(kdiffs[j][i]*pors[j][0]
                         /(0.5*hseds[j][0])
                         *(y[j*cell+ioff+i]-y[j*cell+ioff+numstatevar+i]));
               }
            }
            for ( layer=1; layer<numsedlayers-1; layer++ )
            {
               if ( part[i] == TRUE )
               {
                  if ( fvoltot[j][layer] > 0 )
                  {
                     delta[j*cell+ioff+(1+layer)*numstatevar+i]
                        += fvoltot[j][layer]/(1-pors[j][layer-1])
                           /hseds[j][layer]
                           *y[j*cell+ioff+(1+layer-1)*numstatevar+i];
                  }
                  else
                  {
                     delta[j*cell+ioff+(1+layer)*numstatevar+i]
                        -= fvoltot[j][layer]/(1-pors[j][layer])
                           /hseds[j][layer]
                           *y[j*cell+ioff+(1+layer)*numstatevar+i];
                  }
                  if ( fvoltot[j][layer+1] > 0 )
                  {
                     delta[j*cell+ioff+(1+layer)*numstatevar+i]
                        -= fvoltot[j][layer+1]/(1-pors[j][layer])
                           /hseds[j][layer]
                           *y[j*cell+ioff+(1+layer)*numstatevar+i];
                  }
                  else
                  {
                     delta[j*cell+ioff+(1+layer)*numstatevar+i]
                        += fvoltot[j][layer+1]/(1-pors[j][layer+1])
                           /hseds[j][layer]
                           *y[j*cell+ioff+(1+layer+1)*numstatevar+i];
                  }
               }
               else  // dissolved, inner layers
               {
                  delta[j*cell+ioff+(1+layer)*numstatevar+i]
                     += 1/(hseds[j][layer]*pors[j][layer])
                        *kdiffs[j][i]*fmin(pors[j][layer-1],pors[j][layer])
                          /(0.5*(hseds[j][layer-1]+hseds[j][layer]))
                          *(y[j*cell+ioff+(1+layer-1)*numstatevar+i]
                            -y[j*cell+ioff+(1+layer)*numstatevar+i]);
                  delta[j*cell+ioff+(1+layer)*numstatevar+i]
                     -= 1/(hseds[j][layer]*pors[j][layer])
                        *kdiffs[j][i]*fmin(pors[j][layer],pors[j][layer+1])
                          /(0.5*(hseds[j][layer]+hseds[j][layer+1]))
                          *(y[j*cell+ioff+(1+layer)*numstatevar+i]
                            -y[j*cell+ioff+(1+layer+1)*numstatevar+i]);
               }
            }
            layer = numsedlayers-1;
            if ( numsedlayers > 1 )
            {
               if ( part[i] == TRUE )
               {
                  if ( fvoltot[j][layer] > 0 )
                  {
                     delta[j*cell+ioff+(1+layer)*numstatevar+i]
                        += fvoltot[j][layer]/(1-pors[j][layer-1])
                           /hseds[j][layer]
                           *y[j*cell+ioff+(1+layer-1)*numstatevar+i];
                  }
                  else
                  {
                     delta[j*cell+ioff+(1+layer)*numstatevar+i]
                        -= fvoltot[j][layer]/(1-pors[j][layer])
                           /hseds[j][layer]
                           *y[j*cell+ioff+(1+layer)*numstatevar+i];
                  }
                  if ( fvoltot[j][layer+1] > 0 )
                  {
                     delta[j*cell+ioff+(1+layer)*numstatevar+i]
                        -= fvoltot[j][layer+1]/(1-pors[j][layer])
                           /hseds[j][layer]
                           *y[j*cell+ioff+(1+layer)*numstatevar+i];
                  }
                  else
                  {
                     delta[j*cell+ioff+(1+layer)*numstatevar+i]
                        += fvoltot[j][layer+1]/(1-pors[j][layer])
                           /hseds[j][layer]
                           *y[j*cell+ioff+(1+layer)*numstatevar+i];
                  }
               }
               else  // dissolved, last layer
               {
                  delta[j*cell+ioff+(1+layer)*numstatevar+i]
                     += 1/(hseds[j][layer]*pors[j][layer])
                        *kdiffs[j][i]*fmin(pors[j][layer-1],pors[j][layer])
                          /(0.5*(hseds[j][layer-1]+hseds[j][layer]))
                          *(y[j*cell+ioff+(1+layer-1)*numstatevar+i]
                            -y[j*cell+ioff+(1+layer)*numstatevar+i]);
               }
            }
         }
      }
   }

   // equations at the lake surface (grid point 0):
   // ---------------------------------------------

   REAL advflux1,advflux2,diffflux;           // fluxes
   REAL s1, s2;
   REAL phi = 0.0;
   REAL krelax = 100.0;

   delta[0] = y[0] - Qinterf[0];                             // Q

   delta[1] = y[1] - isurf;	                                 // I

   REAL uflux, eflux, epsflux;
   if ( CalcTKE() == TRUE )
   {
      uflux = areas[0]*surfshear/rhos[0];        // U: Kz*Pr*dU/dz=tau/rho
      delta[2] = ( (y[2]-y[cell+2])
                  /fabs(gridpts[0]-gridpts[1])
                  *Ds[0]*Prs[0]*areas[0]
                  - uflux   ) / areas[0];

      eflux = 0;                                              // e: de/dz=0
      delta[3] = (y[3]-y[cell+3])/fabs(gridpts[0]-gridpts[1])
                 *Ds[0]*Prs[0]/sigmaTKE
                  - eflux/areas[0];

      epsflux = 0;                                        // eps: deps/dz=0
      delta[4] = (y[4]-y[cell+4])/fabs(gridpts[0]-gridpts[1])
                 *Ds[0]*Prs[0]/sigmaEps
                  - epsflux/areas[0];
   }

   // water column:

   for ( i=0; i<numvolstatevar; i++ )                        // VolStateVar
   {
      delta[ioff+i] = Ds[0]*(y[ioff+i]-y[cell+ioff+i])
                      /fabs(gridpts[0]-gridpts[1])
                      - flux[i]/areas[0];
   }

   for ( i=numvolstatevar; i<numdynstatevar; i++ )           // SurfStateVar
   {
      delta[ioff+i] -= yt[ioff+i];
   }
   /* numdynstatevar <= i < numstatevar */                   // EquStateVar

   // sediment:

   if ( CalcSed()==TRUE && numsedlayers>0 )
   {
      for ( i=0; i<numvolstatevar; i++ )                     // VolStateVar
      {
         // exchange calculated above
         for ( CARDINAL layer=0; layer<numsedlayers; layer++ )
         {
            delta[ioff+(1+layer)*numstatevar+i]
               -= yt[ioff+(1+layer)*numstatevar+i];
		 }
      }

      for ( i=numvolstatevar; i<numdynstatevar; i++ )        // SurfStateVar
      {
         for ( CARDINAL layer=0; layer<numsedlayers; layer++ )
         {
            delta[ioff+(1+layer)*numstatevar+i] 
               -= yt[ioff+(1+layer)*numstatevar+i];
         }
      }

      /* numdynstatevar <= i < numstatevar */                // EquStateVar
   }

   // equations in the first cell (gridpoint 1):
   // ------------------------------------------

   delta[cell] = y[cell] - 0.5*(Qinterf[0]+Qinterf[1]);      // Q

   delta[cell+1] = y[cell+1] - y[1]*exp(-0.5*(etas[0]+etas[1])*0.5*dz); // I

   if ( CalcTKE() == TRUE )
   {
	   // add flux at the upper boundary of the cell:
      delta[cell+2] += uflux*dzinv/areas[1];                 // U
      // advective upward flux leaves the lake:
      if ( Qinterf[0] > 0 )
      {
         delta[cell+2] -= Qinterf[0]*y[cell+2]/areas[1]*dzinv;
      }
      // calculate flux at the lower boundary of the cell:
      // advection
      if ( Qinterf[1] < 0 )  // downwards
      {
         advflux1 = -Qinterf[1]*y[cell+2];
      }
      else                   // upwards
      {
         advflux1 = -Qinterf[1]*y[2*cell+2];
      }
      advflux2 = -Qinterf[1]*0.5*(y[cell+2]+y[2*cell+2]);
      s1 = (y[cell+2]-y[2])
           / fabs(gridpts[1]-gridpts[0]);
      s2 = (y[2*cell+2]-y[cell+2])
           / fabs(gridpts[2]-gridpts[1]);
      phi = FluxLimit(s1,s2,ini.I(4));
      uflux = (1.0-phi)*advflux1 + phi*advflux2;
      // diffusion
      diffflux = -(areas[1]+areas[2])*(Ds[1]*Prs[1]+Ds[2]*Prs[2])*0.25
                  *(y[2*cell+2]-y[cell+2])
                  *dzinv;
      uflux += diffflux;
      // subtract flux at the lower boundary of the cell:
      delta[cell+2] -= uflux*dzinv/areas[1];
      // subtract time derivative:
      delta[cell+2] -= yt[cell+2];

	  // add flux at the upper boundary of the cell:
      delta[cell+3] += eflux*dzinv/areas[1];                 // e
      // advective upward flux leaves the lake:
      if ( Qinterf[0] > 0 )
      {
         delta[cell+3] -= Qinterf[0]*y[cell+3]/areas[1]*dzinv;
      }
      // calculate flux at the lower boundary of the cell:
      // advection
      if ( Qinterf[1] < 0 )  // downwards
      {
         advflux1 = -Qinterf[1]*y[cell+3];
      }
      else                   // upwards
      {
         advflux1 = -Qinterf[1]*y[2*cell+3];
      }
      advflux2 = -Qinterf[1]*0.5*(y[cell+3]+y[2*cell+3]);
      s1 = (y[cell+3]-y[3])
           / fabs(gridpts[1]-gridpts[0]);
      s2 = (y[2*cell+3]-y[cell+3])
           / fabs(gridpts[2]-gridpts[1]);
      phi = FluxLimit(s1,s2,ini.I(4));
      eflux = (1.0-phi)*advflux1 + phi*advflux2;
      // diffusion
      diffflux = -(areas[1]+areas[2])
                  *(Ds[1]*Prs[1]+Ds[2]*Prs[2])/sigmaTKE*0.25
                  *(y[2*cell+3]-y[cell+3])
                  *dzinv;
      eflux += diffflux;
      // subtract flux at the lower boundary of the cell:
      delta[cell+3] -= eflux*dzinv/areas[1];
      // subtract time derivative:
      delta[cell+3] -= yt[cell+3];

	  // add flux at the upper boundary of the cell:
      delta[cell+4] += epsflux*dzinv/areas[1];               // eps
      // advective upward flux leaves the lake:
      if ( Qinterf[0] > 0 )
      {
         delta[cell+4] -= Qinterf[0]*y[cell+4]/areas[1]*dzinv;
      }
      // calculate flux at the lower boundary of the cell:
      // advection
      if ( Qinterf[1] < 0 )  // downwards
      {
         advflux1 = -Qinterf[1]*y[cell+4];
      }
      else                   // upwards
      {
         advflux1 = -Qinterf[1]*y[2*cell+4];
      }
      advflux2 = -Qinterf[1]*0.5*(y[cell+4]+y[2*cell+4]);
      s1 = (y[cell+4]-y[4])
           / fabs(gridpts[1]-gridpts[0]);
      s2 = (y[2*cell+4]-y[cell+4])
           / fabs(gridpts[2]-gridpts[1]);
      phi = FluxLimit(s1,s2,ini.I(4));
      epsflux = (1.0-phi)*advflux1 + phi*advflux2;
      // diffusion
      diffflux = -(areas[1]+areas[2])
                  *(Ds[1]*Prs[1]+Ds[2]*Prs[2])/sigmaEps*0.25
                  *(y[2*cell+4]-y[cell+4])
                  *dzinv;
      epsflux += diffflux;
      // subtract flux at the lower boundary of the cell:
      delta[cell+4] -= epsflux*dzinv/areas[1];
      // subtract time derivative:
      delta[cell+4] -= yt[cell+4];
   }

   // water column:

   for ( i=0; i<numvolstatevar; i++ )                        // VolStateVar
   {
      // add flux at the upper boundary of the cell:
      delta[cell+ioff+i] += flux[i]*dzinv/areas[1];
      // advective upward flux leaves the lake:
      if ( Qinterf[0] > 0 )
      {
         delta[cell+ioff+i] -= Qinterf[0]*y[cell+ioff+i]/areas[1]*dzinv;
      }
      // calculate flux at the lower boundary of the cell:
      // advection
      if ( wsed[1][i] >= 0.0 )
      {
         advflux1 = wsed[1][i]*y[cell+ioff+i]*0.5*(areas[1]+areas[2]);
      }
      else
      {
         if ( wsed[2][i] < 0.0 )
         {
            advflux1 = wsed[2][i]*y[2*cell+ioff+i]*areas[2];
         }
         else
         {
            advflux1 = 0.0;
         }
      }
      if ( Qinterf[1] < 0 )  // downwards
      {
         advflux1 -= Qinterf[1]*y[cell+ioff+i];
      }
      else                   // upwards
      {
         advflux1 -= Qinterf[1]*y[2*cell+ioff+i];
      }
      advflux2 = -Qinterf[1]*0.5*(y[cell+ioff+i]+y[2*cell+ioff+i])
                 +0.5*(wsed[1][i]*y[cell+ioff+i]*areas[1]
                      +wsed[2][i]*y[2*cell+ioff+i]*areas[1]);
      s1 = (y[cell+ioff+i]-y[ioff+i])
           / fabs(gridpts[1]-gridpts[0]);
      s2 = (y[2*cell+ioff+i]-y[cell+ioff+i])
           / fabs(gridpts[2]-gridpts[1]);
      phi = FluxLimit(s1,s2,ini.I(4));
      flux[i] = (1.0-phi)*advflux1 + phi*advflux2;
      // diffusion
      //diffflux =  -(areas[1]+areas[2])*(Ds[1]+Ds[2])*0.25 kdiffs added 17.4.03
      diffflux =  -(areas[1]+areas[2])*(Ds[1]+kdiffs[1][i]+Ds[2]+kdiffs[2][i])*0.25
                   *(y[2*cell+ioff+i]-y[cell+ioff+i])
                   *dzinv;
      flux[i] += diffflux;
      // sedimentation
      if ( wsed[1][i] > 0.0 )
      {
         sedflux[i] = wsed[1][i]*sedareasproj[1]
                      *y[cell+ioff+i];
      }
      else
      {
         sedflux[i] = 0.0;
      }
      // subtract flux+sedflux at the lower boundary of the cell:
      delta[cell+ioff+i] -= (flux[i]+sedflux[i])*dzinv/areas[1];
      // subtract time derivative:
      delta[cell+ioff+i] -= yt[cell+ioff+i];
   }

   for ( i=numvolstatevar; i<numdynstatevar; i++ )           // SurfStateVar
   {
      delta[cell+ioff+i] -= yt[cell+ioff+i];
   }

   /* numdynstatevar <= i < numstatevar */                   // EquStateVar

   // sediment:

   if ( CalcSed()==TRUE && numsedlayers>0 )
   {
      for ( i=0; i<numvolstatevar; i++ )                     // VolStateVar
      {
         // exchange calculated above
         for ( CARDINAL layer=0; layer<numsedlayers; layer++ )
         {
            delta[cell+ioff+(1+layer)*numstatevar+i]
               -= yt[cell+ioff+(1+layer)*numstatevar+i];
         }
      }

      for ( i=numvolstatevar; i<numdynstatevar; i++ )        // SurfStateVar
      {
         for ( CARDINAL layer=0; layer<numsedlayers; layer++ )
         {
            delta[cell+ioff+(1+layer)*numstatevar+i]
               -= yt[cell+ioff+(1+layer)*numstatevar+i];
         }
      }

      /* numdynstatevar <= i < numstatevar */                // EquStateVar
   }

   // equations in the inner cells (gridpoints 2 to n-3):
   // ---------------------------------------------------

   for ( j=2; j<NumGridPts()-2; j++ )
   {
      delta[j*cell] = y[j*cell] - 0.5*(Qinterf[j-1]+Qinterf[j]); // Q

      delta[j*cell+1] = y[j*cell+1]                               // I 
                        - y[(j-1)*cell+1]*exp(-0.5*(etas[j-1]+etas[j])*dz);

      if ( CalcTKE() == TRUE )
      {
		  // add flux at the upper boundary of the cell:
         delta[j*cell+2] += uflux*dzinv/areas[j];            // U
         // calculate flux at the lower boundary of the cell:
         // advection
         if ( Qinterf[j] < 0.0 )   // downwards
         {
            advflux1 = -Qinterf[j]*y[j*cell+2];
         }
         else                      // upwards
         {
            advflux1 = -Qinterf[j]*y[(j+1)*cell+2];
         }
         advflux2 = -Qinterf[j]*0.5*(y[j*cell+2]+y[(j+1)*cell+2]);
         s1 = (y[j*cell+2]-y[(j-1)*cell+2])
              / fabs(gridpts[j]-gridpts[j-1]);
         s2 = (y[(j+1)*cell+2]-y[j*cell+2])
              / fabs(gridpts[j+1]-gridpts[j]);
         phi = FluxLimit(s1,s2,ini.I(4));
         uflux = (1.0-phi)*advflux1 + phi*advflux2;
         // diffusion
         diffflux = -(areas[j]+areas[j+1])
                     *(Ds[j]*Prs[j]+Ds[j+1]*Prs[j+1])*0.25
                     *(y[(j+1)*cell+2]-y[j*cell+2])
                     *dzinv;
         uflux += diffflux;
         // subtract flux at the lower boundary of the cell:
         delta[j*cell+2] -= uflux*dzinv/areas[j];
         // subtract time derivative:
         delta[j*cell+2] -= yt[j*cell+2];

		  // add flux at the upper boundary of the cell:
         delta[j*cell+3] += eflux*dzinv/areas[j];            // e
         // calculate flux at the lower boundary of the cell:
         // advection
         if ( Qinterf[j] < 0.0 )   // downwards
         {
            advflux1 = -Qinterf[j]*y[j*cell+3];
         }
         else                      // upwards
         {
            advflux1 = -Qinterf[j+1]*y[(j+1)*cell+3];
         }
         advflux2 = -Qinterf[j]*0.5*(y[j*cell+3]+y[(j+1)*cell+3]);
         s1 = (y[j*cell+3]-y[(j-1)*cell+3])
              / fabs(gridpts[j]-gridpts[j-1]);
         s2 = (y[(j+1)*cell+3]-y[j*cell+3])
              / fabs(gridpts[j+1]-gridpts[j]);
         phi = FluxLimit(s1,s2,ini.I(4));
         eflux = (1.0-phi)*advflux1 + phi*advflux2;
         // diffusion
         diffflux = -(areas[j]+areas[j+1])
                     *(Ds[j]*Prs[j]+Ds[j+1]*Prs[j+1])/sigmaTKE*0.25
                     *(y[(j+1)*cell+3]-y[j*cell+3])
                     *dzinv;
         eflux += diffflux;
         // subtract flux at the lower boundary of the cell:
         delta[j*cell+3] -= eflux*dzinv/areas[j];
         // subtract time derivative:
         delta[j*cell+3] -= yt[j*cell+3];

		  // add flux at the upper boundary of the cell:
         delta[j*cell+4] += epsflux*dzinv/areas[j];          // eps
         // calculate flux at the lower boundary of the cell:
         // advection
         if ( Qinterf[j] < 0.0 )   // downwards
         {
            advflux1 = -Qinterf[j]*y[j*cell+4];
         }
         else                      // upwards
         {
            advflux1 = -Qinterf[j+1]*y[(j+1)*cell+4];
         }
         advflux2 = -Qinterf[j]*0.5*(y[j*cell+4]+y[(j+1)*cell+4]);
         s1 = (y[j*cell+4]-y[(j-1)*cell+4])
              / fabs(gridpts[j]-gridpts[j-1]);
         s2 = (y[(j+1)*cell+4]-y[j*cell+4])
              / fabs(gridpts[j+1]-gridpts[j]);
         phi = FluxLimit(s1,s2,ini.I(4));
         epsflux = (1.0-phi)*advflux1 + phi*advflux2;
         // diffusion
         diffflux = -(areas[j]+areas[j+1])
                     *(Ds[j]*Prs[j]+Ds[j+1]*Prs[j+1])/sigmaEps*0.25
                     *(y[(j+1)*cell+4]-y[j*cell+4])
                     *dzinv;
         epsflux += diffflux;
         // subtract flux at the lower boundary of the cell:
         delta[j*cell+4] -= epsflux*dzinv/areas[j];
         // subtract time derivative:
         delta[j*cell+4] -= yt[j*cell+4];
      }

      // water column:

      for ( i=0; i<numvolstatevar; i++ )                     // VolStateVar
      {
         // add flux (without sedflux) at the upper boundary of the cell:
         delta[j*cell+ioff+i] += flux[i]*dzinv/areas[j];
         // calculate flux at the lower boundary of the cell:
         // advection
         if ( wsed[j][i] >= 0.0 )
         {
            advflux1 = wsed[j][i]*y[j*cell+ioff+i]*0.5*(areas[j]+areas[j+1]);
         }
         else
         {
            if ( wsed[j+1][i] < 0.0 )
            {
               advflux1 = wsed[j+1][i]*y[(j+1)*cell+ioff+i]*areas[j+1];
            }
            else
            {
               advflux1 = 0.0;
            }
         }
         if ( Qinterf[j] < 0.0 )   // downwards
         {
            advflux1 -= Qinterf[j]*y[j*cell+ioff+i];
         }
         else                      // upwards
         {
            advflux1 -= Qinterf[j]*y[(j+1)*cell+ioff+i];
         }
         advflux2 = -Qinterf[j]*0.5*(y[j*cell+ioff+i]+y[(j+1)*cell+ioff+i])
                    +0.5*(wsed[j][i]*y[j*cell+ioff+i]*areas[j]
                         +wsed[j+1][i]*y[(j+1)*cell+ioff+i]*areas[j+1]);
         s1 = (y[j*cell+ioff+i]-y[(j-1)*cell+ioff+i])
              / fabs(gridpts[j]-gridpts[j-1]);
         s2 = (y[(j+1)*cell+ioff+i]-y[j*cell+ioff+i])
              / fabs(gridpts[j+1]-gridpts[j]);
         phi = FluxLimit(s1,s2,ini.I(4));
         flux[i] = (1.0-phi)*advflux1 + phi*advflux2;
         // diffusion
         //diffflux =  -(areas[j]+areas[j+1])*(Ds[j]+Ds[j+1])*0.25  kdiffs added 17.4.03
         diffflux =  -(areas[j]+areas[j+1])*(Ds[j]+kdiffs[j][i]+Ds[j+1]+kdiffs[j+1][i])*0.25
                      *(y[(j+1)*cell+ioff+i]-y[j*cell+ioff+i])
                      *dzinv;
         flux[i] += diffflux;
         // sedimentation
         if ( wsed[j][i] > 0.0 )
         {
            sedflux[i] = wsed[j][i]*sedareasproj[j]
                         *y[j*cell+ioff+i];
         }
         else
         {
            sedflux[i] = 0.0;
         }
         // subtract flux+sedflux at the lower boundary of the cell:
         delta[j*cell+ioff+i] -= (flux[i]+sedflux[i])*dzinv/areas[j];
         // subtract time derivative:
         delta[j*cell+ioff+i] -= yt[j*cell+ioff+i];
      }

      for ( i=numvolstatevar; i<numdynstatevar; i++ )        // SurfStateVar
      {
         delta[j*cell+ioff+i] -= yt[j*cell+ioff+i];
      }

      /* numdynstatevar <= i < numstatevar */                // EquStateVar

      // sediment:

      if ( CalcSed()==TRUE && numsedlayers>0 )
      {
         for ( i=0; i<numvolstatevar; i++ )                  // VolStateVar
         {
            // exchange calculated above
            for ( CARDINAL layer=0; layer<numsedlayers; layer++ )
            {
               delta[j*cell+ioff+(1+layer)*numstatevar+i]
                  -= yt[j*cell+ioff+(1+layer)*numstatevar+i];
            }
         }

         for ( i=numvolstatevar; i<numdynstatevar; i++ )     // SurfStateVar
         {
            for ( CARDINAL layer=0; layer<numsedlayers; layer++ )
            {
               delta[j*cell+ioff+(1+layer)*numstatevar+i]
                  -= yt[j*cell+ioff+(1+layer)*numstatevar+i];
            }
         }

         /* numdynstatevar <= i < numstatevar */             // EquStateVar
      }
   }

   // equations in the last cell (grid point n-2):
   // --------------------------------------------

   j = NumGridPts()-2;

   delta[j*cell] = y[j*cell] - 0.5*(Qinterf[j-1]+Qinterf[j]); // Q

   delta[j*cell+1] = y[j*cell+1]                              // I 
                        - y[(j-1)*cell+1]*exp(-0.5*(etas[j-1]+etas[j])*dz);

   if ( CalcTKE() == TRUE )
   {
	  // add flux at the upper boundary of the cell:
      delta[j*cell+2] += uflux*dzinv/areas[j];               // U
      // calculate flux at the lower boundary of the cell:
      // advection
      if ( Qinterf[j] < 0.0 )   // downwards
      {
         advflux1 = -Qinterf[j]*y[j*cell+2];
      }
      else                      // upwards
      {
         advflux1 = -Qinterf[j]*y[(j+1)*cell+2];
      }
      advflux2 = -Qinterf[j]*y[(j+1)*cell+2];
      s1 = (y[j*cell+2]-y[(j-1)*cell+2]) 
           / fabs(gridpts[j]-gridpts[j-1]);
      s2 = (y[(j+1)*cell+2]-y[j*cell+2]) 
           / fabs(gridpts[j+1]-gridpts[j]);
      phi = FluxLimit(s1,s2,ini.I(4));
      uflux = (1.0-phi)*advflux1 + phi*advflux2;
      // diffusion
      diffflux = -(areas[j]+areas[j+1])
                  *(Ds[j]*Prs[j]+Ds[j+1]*Prs[j+1])*0.25
                  *(y[(j+1)*cell+2]-y[j*cell+2])
                  *dzinv;
      uflux += diffflux;
      // subtract flux at the lower boundary of the cell:
      delta[j*cell+2] -= uflux*dzinv/areas[j];
      // subtract time derivative:
      delta[j*cell+2] -= yt[j*cell+2];

 	  // add flux at the upper boundary of the cell:
      delta[j*cell+3] += eflux*dzinv/areas[j];               // e
      // calculate flux at the lower boundary of the cell:
      // advection
      if ( Qinterf[j] < 0.0 )   // downwards
      {
         advflux1 = -Qinterf[j]*y[j*cell+3];
      }
      else                      // upwards
      {
         advflux1 = -Qinterf[j]*y[(j+1)*cell+3];
      }
      advflux2 = -Qinterf[j]*y[(j+1)*cell+3];
      s1 = (y[j*cell+3]-y[(j-1)*cell+3]) 
           / fabs(gridpts[j]-gridpts[j-1]);
      s2 = (y[(j+1)*cell+3]-y[j*cell+3]) 
           / fabs(gridpts[j+1]-gridpts[j]);
      phi = FluxLimit(s1,s2,ini.I(4));
      eflux = (1.0-phi)*advflux1 + phi*advflux2;
      // diffusion
      diffflux = -(areas[j]+areas[j+1])
                  *(Ds[j]*Prs[j]+Ds[j+1]*Prs[j+1])/sigmaTKE*0.25
                  *(y[(j+1)*cell+3]-y[j*cell+3])
                  *dzinv;
      eflux += diffflux;
      // subtract flux at the lower boundary of the cell:
      delta[j*cell+3] -= eflux*dzinv/areas[j];
      // subtract time derivative:
      delta[j*cell+3] -= yt[j*cell+3];

 	  // add flux at the upper boundary of the cell:
      delta[j*cell+4] += epsflux*dzinv/areas[j];             // eps
      // calculate flux at the lower boundary of the cell:
      // advection
      if ( Qinterf[j] < 0.0 )   // downwards
      {
         advflux1 = -Qinterf[j]*y[j*cell+4];
      }
      else                      // upwards
      {
         advflux1 = -Qinterf[j]*y[(j+1)*cell+4];
      }
      advflux2 = -Qinterf[j]*y[(j+1)*cell+4];
      s1 = (y[j*cell+4]-y[(j-1)*cell+4]) 
           / fabs(gridpts[j]-gridpts[j-1]);
      s2 = (y[(j+1)*cell+4]-y[j*cell+4]) 
           / fabs(gridpts[j+1]-gridpts[j]);
      phi = FluxLimit(s1,s2,ini.I(4));
      epsflux = (1.0-phi)*advflux1 + phi*advflux2;
      // diffusion
      diffflux = -(areas[j]+areas[j+1])
                  *(Ds[j]*Prs[j]+Ds[j+1]*Prs[j+1])/sigmaEps*0.25
                  *(y[(j+1)*cell+4]-y[j*cell+4])
                  *dzinv;
      epsflux += diffflux;
      // subtract flux at the lower boundary of the cell:
      delta[j*cell+4] -= epsflux*dzinv/areas[j];
      // subtract time derivative:
      delta[j*cell+4] -= yt[j*cell+4];
   }

   // water column:

   for ( i=0; i<numvolstatevar; i++ )                        // VolStateVar
   {
      // add flux (without sedflux) at the upper boundary of the cell:
      delta[j*cell+ioff+i] += flux[i]*dzinv/areas[j];
      // calculate flux at the lower boundary of the cell:
      // advection
      if ( wsed[j][i] >= 0.0 )
      {
         advflux1 = 0.0;
      }
      else
      {
         advflux1 = 0.0;
      }
      if ( Qinterf[j] < 0.0 )   // downwards
      {
         advflux1 -= Qinterf[j]*y[j*cell+ioff+i];
      }
      else                      // upwards
      {
         advflux1 -= Qinterf[j]*y[(j+1)*cell+ioff+i];
      }
      advflux2 = -Qinterf[j]*y[(j+1)*cell+ioff+i]
                 +wsed[j+1][i]*y[(j+1)*cell+ioff+i]*areas[j+1];
      s1 = (y[j*cell+ioff+i]-y[(j-1)*cell+ioff+i]) 
           / fabs(gridpts[j]-gridpts[j-1]);
      s2 = (y[(j+1)*cell+ioff+i]-y[j*cell+ioff+i]) 
           / fabs(gridpts[j+1]-gridpts[j]);
      phi = FluxLimit(s1,s2,ini.I(4));
      flux[i] = (1.0-phi)*advflux1 + phi*advflux2;
      // diffusion
      //diffflux = -areas[j+1]*Ds[j+1]  kdiffs added 17.4.03
      diffflux = -areas[j+1]*(Ds[j+1]+kdiffs[j+1][i])
                  *(y[(j+1)*cell+ioff+i]-y[j*cell+ioff+i])
                  *2.0*dzinv;
      flux[i] += diffflux;
      // sedimentation
      if ( wsed[j][i] > 0.0 )
      {
         sedflux[i] = wsed[j][i]*sedareasproj[j]
                      *y[j*cell+ioff+i];
      }
      else
      {
         sedflux[i] = 0.0;
      }
      // subtract flux+sedflux at the lower boundary of the cell:
      delta[j*cell+ioff+i] -= (flux[i]+sedflux[i])*dzinv/areas[j];
      // subtract time derivative:
      delta[j*cell+ioff+i] -= yt[j*cell+ioff+i];
   }

   for ( i=numvolstatevar; i<numdynstatevar; i++ )           // SurfStateVar
   {
      delta[j*cell+ioff+i] -= yt[j*cell+ioff+i];
   }

   /* numdynstatevar <= i < numstatevar */                   // EquStateVar

   // sediment:

   if ( CalcSed()==TRUE && numsedlayers>0 )
   {
      for ( i=0; i<numvolstatevar; i++ )                     // VolStateVar
      {
         // exchange calculated above
         for ( CARDINAL layer=0; layer<numsedlayers; layer++ )
         {
            delta[j*cell+ioff+(1+layer)*numstatevar+i]
               -= yt[j*cell+ioff+(1+layer)*numstatevar+i];
         }
      }

      for ( i=numvolstatevar; i<numdynstatevar; i++ )        // SurfStateVar
      {
         for ( CARDINAL layer=0; layer<numsedlayers; layer++ )
         {
            delta[j*cell+ioff+(1+layer)*numstatevar+i]
               -= yt[j*cell+ioff+(1+layer)*numstatevar+i];
         }
      }

      /* numdynstatevar <= i < numstatevar */                // EquStateVar
   }

   // equations at the lake bottom (grid point n-1):
   // ----------------------------------------------

   j = NumGridPts()-1;

   delta[j*cell] = y[j*cell] - Qinterf[numgrid-2];           // Q

   delta[j*cell+1] = y[j*cell+1]                             // I 
                     - y[(j-1)*cell+1]*exp(-0.5*(etas[j-1]+etas[j])*0.5*dz);

   if ( CalcTKE() == TRUE )
   {
      delta[j*cell+2] = y[j*cell+2];                // U: U=0

/*
      REAL integral = 0.0;                         // U: int(U*sqrt(A)dz) = 0
      for ( CARDINAL jj=1; jj<NumGridPts()-1; jj++ )
      {
        integral += 0.5*(y[(jj-1)*cell+2]/sqrt(areas[jj-1])
                          +y[jj*cell+2]/sqrt(areas[jj]))
                        *fabs(gridpts[jj]-gridpts[jj-1]);
      }
      delta[j*cell+2] = y[j*cell+2]/areas[j]
                       + (2*integral/fabs(gridpts[j]-gridpts[j-1])
                          +y[(j-1)*cell+2]/sqrt(areas[j-1])         )
                         /sqrt(areas[j]);
*/

      delta[j*cell+3] = y[(j-1)*cell+3]-y[j*cell+3];         // de/dz=0

      delta[j*cell+4] = y[(j-1)*cell+4]-y[j*cell+4];         // deps/dz=0
   }

   // water column:

   for ( i=0; i<numvolstatevar; i++ )                        // VolStateVar
   {
      if ( wsed[j-1][i] > 0.0 )
      {
         sedflux[i] = wsed[j-1][i]*sedareasproj[j]
                      *y[(j-1)*cell+ioff+i];
      }
      else
      {
         sedflux[i] = 0.0;
      }
      // subtract sediment flux from last cell:
      delta[(j-1)*cell+ioff+i] -= sedflux[i]*dzinv/areas[j-1];
      if ( part[i] == TRUE )
      {
         if ( CalcSed()==FALSE || numsedlayers==0 )
         {
            delta[j*cell+ioff+i] = y[(j-1)*cell+ioff+i]-y[j*cell+ioff+i]
                                   //+ bottominp[i]*0.5*dz/Ds[j];  kdiffs added 17.4.03
                                   + bottominp[i]*0.5*dz/(Ds[j]+kdiffs[j][i]);
         }
         else
         {
			delta[j*cell+ioff+i] = y[(j-1)*cell+ioff+i]-y[j*cell+ioff+i]
                                   - resvels[j]*y[j*cell+ioff+numstatevar+i]
                                     //*0.5*dz/Ds[j];  kdiffs added 17.4.03
                                     *0.5*dz/(Ds[j]+kdiffs[j][i]);
         }
      }
      else
	  {
         if ( CalcSed()==FALSE || numsedlayers==0 )
         {
            delta[j*cell+ioff+i] = y[(j-1)*cell+ioff+i]-y[j*cell+ioff+i]
                                   //+ bottominp[i]*0.5*dz/Ds[j];  kdiffs added 17.4.03
                                   + bottominp[i]*0.5*dz/(Ds[j]+kdiffs[j][i]);
         }
         else	// flux into sediment
         {
            delta[j*cell+ioff+i]
               = y[(j-1)*cell+ioff+i]-y[j*cell+ioff+i]
                 //- dz/hseds[j][0]*kdiffs[j][i]/Ds[j]*pors[j][0]  kdiffs added 17.4.03
                 - dz/hseds[j][0]*kdiffs[j][i]/(Ds[j]+kdiffs[j][i])*pors[j][0]
                   *(y[j*cell+ioff+i]-y[j*cell+ioff+numstatevar+i]);
		 }
	  }
   }

   for ( i=numvolstatevar; i<numdynstatevar; i++ )           // SurfStateVar
   {
      delta[j*cell+ioff+i] -= yt[j*cell+ioff+i];
   }
   /* numdynstatevar <= i < numstatevar */                   // EquStateVar

   // sediment:

   if ( CalcSed()==TRUE && numsedlayers>0 )
   {
      for ( i=0; i<numvolstatevar; i++ )                     // VolStateVar
      {
         // exchange calculated above
         for ( CARDINAL layer=0; layer<numsedlayers; layer++ )
         {
            delta[j*cell+ioff+(1+layer)*numstatevar+i]
               -= yt[j*cell+ioff+(1+layer)*numstatevar+i];
         }
      }

      for ( i=numvolstatevar; i<numdynstatevar; i++ )        // SurfStateVar
      {
         for ( CARDINAL layer=0; layer<numsedlayers; layer++ )
         {
            delta[j*cell+ioff+(1+layer)*numstatevar+i]
               -= yt[j*cell+ioff+(1+layer)*numstatevar+i];
         }
      }
      /* numdynstatevar <= i < numstatevar */                // EquStateVar
   }

   // equations for seiche energy:
   // ----------------------------

   if ( CalcTKE() == TRUE )
   {
	  REAL pint = 0.0;
	  for ( j=1; j<numgrid-1; j++ )
	  {
		  pint += areas[j]*pints[j]*rhos[j]*dz;
	  }
	  delta[numgrid*cell] = pwind - areas[0]*pbottom - pint; // Eseiche
      delta[numgrid*cell] -= yt[numgrid*cell];
   }

   for ( i=0; i<numstatevar; i++ ) statevar[i]->ResetStateVar();
   delete [] conc;
   delete [] flux;
   delete [] sedflux;
   delete [] areas;
   delete [] sedareas;
   delete [] sedareasproj;
   delete [] rhos;
   delete [] dAdzs;
   delete [] N2s;
   delete [] Ds;
   delete [] etas;
   delete [] Prs;
   delete [] ShearPs;
   delete [] BuoyancyPs;
   delete [] pints;
   delete [] Qinterf;
   delete [] qlat;
   delete [] bottominp;
   delete [] part;
   delete [] ind;
   for ( j=0; j<numgrid; j++ ) delete [] wsed[j];
   delete [] wsed;
   delete [] rhopart;
   for ( j=0; j<numgrid; j++ ) delete [] kdiffs[j];
   delete [] kdiffs;
   delete resvels;
   delete fvols;
   for ( j=0; j<numgrid; j++ ) delete fvoltot[j];
   delete [] fvoltot;
   for ( j=0; j<numgrid; j++ ) delete hseds[j];
   delete [] hseds;
   for ( j=0; j<numgrid; j++ ) delete pors[j];
   delete [] pors;

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


BOOLEAN LAKECOMP::GridValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                            REAL* t_ptr, const REAL* Y,
                            CARDINAL zone, CARDINAL pt, AQVAR* var,
                            REAL& value)
{
   if ( Active() != TRUE ) return FALSE;
   SetGlobalState(varsys,calcnum_ptr,t_ptr,Y);
   CARDINAL i;
   if ( zone >= NumZones() )          return FALSE;
   if ( pt >= NumGridPts() )          return FALSE;
   if ( varsys->Exist(var) == FALSE ) return FALSE;
   if ( AllowedVar(var) == FALSE )    return FALSE;
   if ( Active() == FALSE )           return FALSE;
   const REAL* y = &Y[ind_y];
   CARDINAL cell = ioff+(1+ised*numsedlayers)*NumStateVar();
   varsys->Reset();
   REAL cn = *calcnum_ptr; 
   REAL compindex = (REAL)cind;
   REAL zoneindex = 0.0;
   varsys->SetProgVar(CalcNumber,&cn);
   varsys->SetProgVar(CompIndex,&compindex);
   varsys->SetProgVar(ZoneIndex,&zoneindex);
   varsys->SetProgVar(Time,t_ptr);
   REAL eps = 1.0; varsys->SetProgVar(WaterFraction,&eps);
   Zvar    = varsys->GetProgVar(SpaceZ);
   Avar    = varsys->GetProgVar(CrossSection);
   Qvar    = varsys->GetProgVar(Discharge);
   Rhovar  = varsys->GetProgVar(::Density);
   dAdzvar = varsys->GetProgVar(AreaGradient);
   REAL lightint = 0; varsys->SetProgVar(LightIntensity,&lightint);
   N2var   = varsys->GetProgVar(BruntVaisala);
   UHvar   = varsys->GetProgVar(HorizVelocity);
   TKEvar  = varsys->GetProgVar(TurbKinEnergy);
   ShearPvar = varsys->GetProgVar(ShearProd);
   BuoyancyPvar = varsys->GetProgVar(BuoyancyProd);
   EPSvar  = varsys->GetProgVar(Dissipation);
   Eseichevar = varsys->GetProgVar(SeicheEnergy);
   REAL* conc = new REAL[numstatevar];
   REAL area, rho, dAdz, N2, shearP, buoyancyP;
   if ( Qvar != 0 )    Qvar->SetProgVar(&y[pt*cell]);
   if ( Avar != 0 )    Avar->SetProgVar(&area);
   if ( Zvar != 0 )    Zvar->SetProgVar(&gridpts[pt]);
   if ( Rhovar != 0 )  Rhovar->SetProgVar(&rho);
   if ( dAdzvar != 0 ) dAdzvar->SetProgVar(&dAdz);
   if ( N2var != 0 )   N2var->SetProgVar(&N2);
   if ( ShearPvar != 0 ) ShearPvar->SetProgVar(&shearP);
   if ( BuoyancyPvar != 0 ) BuoyancyPvar->SetProgVar(&buoyancyP);

   for ( i=0; i<numstatevar; i++ ) statevar[i]->SetStateVar(&conc[i]);
   varsys->Reset();
   area = A->Value();
   REAL uh,tke,shearp,buoyancyp,epsilon;
   REAL zero = 0.0;
   if ( CalcTKE() == TRUE )
   {
      uh = y[pt*cell+2];
      if ( UHvar != 0 )  UHvar->SetProgVar(&uh);
      tke = y[pt*cell+3];
      if ( TKEvar != 0 )  TKEvar->SetProgVar(&tke);
      if ( ShearPvar != 0 ) ShearPvar->SetProgVar(&shearp);
      if ( BuoyancyPvar != 0 ) BuoyancyPvar->SetProgVar(&buoyancyp);
      epsilon = y[pt*cell+4];
      if ( EPSvar != 0 )  EPSvar->SetProgVar(&epsilon);
      if ( Eseichevar != 0 ) Eseichevar->SetProgVar(&y[numgrid*cell]);
   }
   else
   {
      if ( UHvar != 0 )   UHvar->SetProgVar(&zero);
      if ( TKEvar != 0 )  TKEvar->SetProgVar(&zero);
      if ( ShearPvar != 0 )  ShearPvar->SetProgVar(&zero);
      if ( BuoyancyPvar != 0 )  BuoyancyPvar->SetProgVar(&zero);
      if ( EPSvar != 0 )  EPSvar->SetProgVar(&zero);
      if ( Eseichevar != 0 ) Eseichevar->SetProgVar(&zero);
   }
   REAL rhopre   = 0;
   REAL rhopost  = 0;
   REAL areapre  = 0;
   REAL areapost = 0;
   if ( pt > 0 )
   {
      if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[pt-1]);
      for ( i=0; i<numvolstatevar; i++ )
      {
         conc[i] = y[(pt-1)*cell+ioff+i];
      }
      for ( i=numvolstatevar; i<numdynstatevar; i++ )
      {
         conc[i] = y[(pt-1)*cell+ioff+i];
      }
      for ( i=numdynstatevar; i<numstatevar; i++ )
      {
         conc[i] = y[(pt-1)*cell+ioff+i];
      }
      varsys->Reset();
	  areapre = A->Value();
      rhopre = Rho->Value();
   }
   if ( pt < numgrid-1 )
   {
      if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[pt+1]);
      for ( i=0; i<numvolstatevar; i++ )
      {
         conc[i] = y[(pt+1)*cell+ioff+i];
      }
      for ( i=numvolstatevar; i<numdynstatevar; i++ )
      {
         conc[i] = y[(pt+1)*cell+ioff+i];
      }
      for ( i=numdynstatevar; i<numstatevar; i++ )
      {
         conc[i] = y[(pt+1)*cell+ioff+i];
      }
      varsys->Reset();
      areapost = A->Value();
      rhopost = Rho->Value();
   }
   if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[pt]);
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
   varsys->Reset();
   rho = Rho->Value();
   if ( pt == 0 )
   {
      N2 = gravit/rho
           *(rhopost-rho)/fabs(gridpts[1]-gridpts[0]);
      dAdz = (areapost-area)/(gridpts[1]-gridpts[0]);
      if ( CalcTKE() == TRUE )
      {
         REAL dUdz = (y[cell+2]-y[2])/(gridpts[1]-gridpts[0]);
         shearp = D->Value()*Pr->Value()*dUdz*dUdz;
         REAL Kz = D->Value();
         if ( Kz > kzmin )
         {
            buoyancyp = -(Kz-kzmin)*Pr->Value()*N2;
         }
         else
         {
            buoyancyp = 0.0;
         }
      }
   }
   else
   {
      if ( pt == numgrid-1 )
      {
         N2 = gravit/rho
              *(rho-rhopre)
              /fabs(gridpts[numgrid-1]-gridpts[numgrid-2]);
         dAdz = (area-areapre)/(gridpts[numgrid-1]-gridpts[numgrid-2]);
         if ( CalcTKE() == TRUE )
         {
            REAL dUdz = (y[(numgrid-1)*cell+2]-y[(numgrid-2)*cell+2])
                        /(gridpts[numgrid-1]-gridpts[numgrid-2]);
            shearp = D->Value()*Pr->Value()*dUdz*dUdz;
            REAL Kz = D->Value();
            if ( Kz > kzmin )
            {
               buoyancyp = -(Kz-kzmin)*Pr->Value()*N2;
            }
            else
            {
               buoyancyp = 0.0;
            }
         }
      }
      else
      {
         REAL d1 = fabs(gridpts[pt]-gridpts[pt-1]);
         REAL d2 = fabs(gridpts[pt+1]-gridpts[pt]);
         N2 = gravit/rho
              *(d1*d1*rhopost-d2*d2*rhopre
                       -(d1*d1-d2*d2)*rho)
              /(d1*d2*(d1+d2));
         dAdz = (areapost-areapre)/(gridpts[pt+1]-gridpts[pt-1]);
         if ( CalcTKE() == TRUE )
         {
            REAL dUdz = (y[(pt+1)*cell+2]-y[(pt-1)*cell+2])
                        /(gridpts[pt+1]-gridpts[pt-1]);
            shearp = D->Value()*Pr->Value()*dUdz*dUdz;
            REAL Kz = D->Value();
            if ( Kz > kzmin )
            {
               buoyancyp = -(Kz-kzmin)*Pr->Value()*N2;
            }
            else
            {
               buoyancyp = 0.0;
            }
         }
      }
   }

   if ( zone > 0 )  // sediment
   {
      REAL sedarea;  // definition must be compatible with CalcAreas!!
      if ( pt == 0 )
      {
         if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[1]);
         varsys->Reset();
         REAL area1 = A->Value();
         if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[2]);
         varsys->Reset();
         REAL area2 = A->Value();
         sedarea = area - 0.5*(area1+area2);
         if ( sedarea < 2*sqrt(3.14*area1)*(gridpts[2]-gridpts[1]) )
         {
            sedarea = 2*sqrt(3.14*area1)*(gridpts[2]-gridpts[1]);
         }
      }
      else
      {
         if ( pt == 1 )
         {
            if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[0]);
            varsys->Reset();
            REAL area0 = A->Value();
            if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[2]);
            varsys->Reset();
            REAL area2 = A->Value();
            sedarea = area0 - 0.5*(area+area2);
            if ( sedarea < 2*sqrt(3.14*area)*(gridpts[2]-gridpts[1]) )
            {
               sedarea = 2*sqrt(3.14*area)*(gridpts[2]-gridpts[1]);
            }
         }
         else
         {
            if ( pt < numgrid-2 )
            {
               if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[pt-1]);
               varsys->Reset();
               REAL areaminus = A->Value();
               if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[pt+1]);
               varsys->Reset();
               REAL areaplus = A->Value();
               sedarea = 0.5*(areaminus-areaplus);
               if ( sedarea < 2*sqrt(3.14*area)*(gridpts[2]-gridpts[1]) )
               {
                  sedarea = 2*sqrt(3.14*area)*(gridpts[2]-gridpts[1]);
               }
            }
            else
            {
               if ( pt < numgrid-1 )   // pt = numgrid-2
               {
                  if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[numgrid-3]);
                  varsys->Reset();
                  REAL areaminus = A->Value();
                  if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[numgrid-1]);
                  varsys->Reset();
                  REAL areaplus = A->Value();
                  sedarea = 0.5*(areaminus+area)-areaplus;
                  if ( sedarea < 2*sqrt(3.14*area)*(gridpts[2]-gridpts[1]) )
                  {
                     sedarea = 2*sqrt(3.14*area)*(gridpts[2]-gridpts[1]);
                  }
               }
               else     // pt = numgrid-1
               {
                  sedarea = area;
               }
            }
         }
      }
      for ( i=0; i<numvolstatevar; i++ )
      {
         conc[i] = y[pt*cell+ioff+zone*numstatevar+i];
      }
      for ( i=numvolstatevar; i<numdynstatevar; i++ )
      {
         conc[i] = y[pt*cell+ioff+zone*numstatevar+i];
      }
      for ( i=numdynstatevar; i<numstatevar; i++ )
      {
         conc[i] = y[pt*cell+ioff+zone*numstatevar+i];
      }
      if ( Zvar != 0 ) Zvar->SetProgVar(&gridpts[pt]);
      zoneindex = sedlayers[zone-1]->ZoneIndex();
   }
   else	  // water column
   {
      lightint = y[pt*cell+1];
   }

   varsys->Reset();
   value = var->Value();
   for ( i=0; i<numstatevar; i++ ) statevar[i]->ResetStateVar();
   delete [] conc;
   return TRUE;
}


BOOLEAN LAKECOMP::SpaceValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
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
               FatalError("LAKECOMP::Value: index negative");
         }
      }
      else
      {
         while ( z > gridpts[i+1] )
         {
            i++;
            if ( i >= numgrid-1 )
               FatalError("LAKECOMP::Value: index too large");
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
               FatalError("LAKECOMP::Value: index negative");
         }
      }
      else
      {
         while ( z < gridpts[i+1] )
         {
            i++;
            if ( i >= numgrid-1 )
               FatalError("LAKECOMP::Value: index too large");
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


CARDINAL LAKECOMP::InitZone(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("LAKECOMP::InitZone: Illegal index");
   return initzone[index];
}


AQVAR* LAKECOMP::InitVar(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("LAKECOMP::InitVar: Illegal index");
   return initvar[index];
}


const FORMVAR* LAKECOMP::InitVal(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("LAKECOMP::InitVal: Illegal index");
   return initval[index];
}


BOOLEAN LAKECOMP::AddInitCond(const VARLIST* varlist, CARDINAL zone,
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


BOOLEAN LAKECOMP::ReplaceInitCond(const VARLIST* varlist, CARDINAL zone,
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


BOOLEAN LAKECOMP::DeleteInitCond(CARDINAL pos)
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


AQVAR* LAKECOMP::SurfInputVar(CARDINAL index) const
{
   if ( index >= numsurfinput )
      FatalError("LAKECOMP::SurfInputVar: Illegal index");
   return surfinputvar[index];
}


const FORMVAR* LAKECOMP::SurfInputFlux(CARDINAL index) const
{
   if ( index >= numsurfinput )
      FatalError("LAKECOMP::SurfInputFlux: Illegal index");
   return surfinputflux[index];
}


BOOLEAN LAKECOMP::AddSurfInput(const VARLIST* varlist, AQVAR* var,
                               const char* inpline, char* errorline,
                               CARDINAL& numparseerrors, CARDINAL pos)
{
   numparseerrors = 0;
   if ( InList() == TRUE )             return FALSE;
   if ( inpline == 0 )                 return FALSE;
   if ( inpline[0] == '\0' )           return FALSE;
   if ( varlist->Exist(var) == FALSE ) return FALSE;
   if ( pos > numsurfinput ) pos = numsurfinput;
   CARDINAL i;
   for ( i=0; i<numsurfinput; i++ )
   {
      if ( surfinputvar[i] == var )        return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("SurfInput");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                  return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                  return FALSE;
   }
   AQVAR**   newsurfvar  = new AQVAR*[numsurfinput+1];
   FORMVAR** newsurfflux = new FORMVAR*[numsurfinput+1];
   for ( i=0; i<pos; i++ )
   {
      newsurfvar[i]  = surfinputvar[i];
      newsurfflux[i] = surfinputflux[i];
   }
   newsurfvar[pos]  = var;
   newsurfflux[pos] = formvar;
   for ( i=pos; i<numsurfinput; i++ )
   {
      newsurfvar[i+1]  = surfinputvar[i];
      newsurfflux[i+1] = surfinputflux[i];
   }
   delete surfinputvar;  surfinputvar  = newsurfvar;
   delete surfinputflux; surfinputflux = newsurfflux;
   numsurfinput++;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::ReplaceSurfInput(const VARLIST* varlist, AQVAR* var,
                                   const char* inpline, char* errorline,
                                   CARDINAL& numparseerrors, CARDINAL pos)
{
   numparseerrors = 0;
   if ( InList() == TRUE )              return FALSE;
   if ( inpline == 0 )                  return FALSE;
   if ( inpline[0] == '\0' )            return FALSE;
   if ( varlist->Exist(var) == FALSE )  return FALSE;
   if ( pos >= numsurfinput )           return FALSE;
   CARDINAL i;
   for ( i=0; i<numsurfinput; i++ )
   {
      if ( i!=pos && surfinputvar[i]==var ) return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("SurfInput");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                   return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                   return FALSE;
   }
   delete surfinputflux[pos];
   surfinputvar[pos]  = var;
   surfinputflux[pos] = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::DeleteSurfInput(CARDINAL pos)
{
   if ( pos >= numsurfinput ) return FALSE;
   AQVAR**   newsurfvar  = new AQVAR*[numsurfinput-1];
   FORMVAR** newsurfflux = new FORMVAR*[numsurfinput-1];
   CARDINAL i;
   for ( i=0; i<pos; i++ )
   {
      newsurfvar[i]  = surfinputvar[i];
      newsurfflux[i] = surfinputflux[i];
   }
   for ( i=pos+1; i<numsurfinput; i++ )
   {
      newsurfvar[i-1]  = surfinputvar[i];
      newsurfflux[i-1] = surfinputflux[i];
   }
   delete surfinputflux[pos];
   delete surfinputvar;  surfinputvar = newsurfvar;
   delete surfinputflux; surfinputflux = newsurfflux;
   numsurfinput--;
   CalcArg();
   return TRUE;
}


LAKESEDLAYER* LAKECOMP::SedLayer(CARDINAL index)
{
   if ( index > numsedlayers )
      FatalError("LAKECOMP::LAKESEDLAYER: illegal index");
   return sedlayers[index];
}


BOOLEAN LAKECOMP::AddSedLayer(LAKESEDLAYER* layer, CARDINAL pos)
{
   CARDINAL i;
   if ( InList() == TRUE )          return FALSE;
   if ( pos > numsedlayers ) pos = numsedlayers;

   LAKESEDLAYER** newsedlayers = new LAKESEDLAYER*[numsedlayers+1];
   for ( i=0; i<pos; i++ )
   {
      newsedlayers[i] = sedlayers[i];
   }
   newsedlayers[pos] = layer;
   for ( i=pos; i<numsedlayers; i++ )
   {
      newsedlayers[i+1] = sedlayers[i];
   }
   delete sedlayers; sedlayers = newsedlayers;
   numsedlayers++;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::ReplaceSedLayer(LAKESEDLAYER* layer, CARDINAL pos)
{
   if ( InList() == TRUE )    return FALSE;
   if ( pos >= numsedlayers ) return FALSE;
   delete sedlayers[pos];
   sedlayers[pos] = layer;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::DeleteSedLayer(CARDINAL pos)
{
   CARDINAL i;
   if ( pos >= numsedlayers ) return FALSE;

   LAKESEDLAYER** newsedlayers = new LAKESEDLAYER*[numsedlayers-1];

   for ( i=0; i<pos; i++ )
   {
      newsedlayers[i] = sedlayers[i];
   }
   for ( i=pos+1; i<numsedlayers; i++ )
   {
      newsedlayers[i-1] = sedlayers[i];
   }
   delete sedlayers[pos];
   delete [] sedlayers; sedlayers = newsedlayers;
   numsedlayers--;
   CalcArg();
   return TRUE;
}


void LAKECOMP::CalcArg()
{
   CARDINAL i,j;

   AQCOMP::CalcArg();
   CARDINAL num = numvararg;
   numvararg += numinit;
   for ( i=0; i<numinit; i++ ) numvararg += (initval[i])->NumVarArg();
   numvararg += numsurfinput;
   for ( i=0; i<numsurfinput; i++ )
      numvararg += (surfinputflux[i])->NumVarArg();
   if ( latinflow != 0 ) numvararg += latinflow->NumVarArg();
   numvararg += numlatinput;
   for ( j=0; j<numlatinput; j++ )
      numvararg += latinputconc[j]->NumVarArg();
   PINF* pinf = pinflist.First();
   while ( pinf != 0 )
   {
      numvararg += pinf->NumVarArg();
      pinf = pinf->Next();
   }
   numvararg += numsedinput;
   for ( j=0; j<numsedinput; j++ )
      numvararg += sedinputflux[j]->NumVarArg();
   numvararg += numdissvar;
   for ( j=0; j<numdissvar; j++ )
   {
      numvararg += disskdiff[j]->NumVarArg();
   }
   numvararg += numpartvar;
   for ( j=0; j<numpartvar; j++ )
   {
      numvararg += partrho[j]->NumVarArg();
      numvararg += partsedveloc[j]->NumVarArg();
      numvararg += partmobility[j]->NumVarArg();
   }
   if ( A != 0 )          numvararg += A->NumVarArg();
   if ( Rho != 0 )        numvararg += Rho->NumVarArg();
   if ( D != 0 )          numvararg += D->NumVarArg();
   if ( Isurf != 0 )      numvararg += Isurf->NumVarArg();
   if ( Eta != 0 )        numvararg += Eta->NumVarArg();
   if ( Fvol != 0 )       numvararg += Fvol->NumVarArg();
   if ( kres != 0 )       numvararg += kres->NumVarArg();
   for ( i=0; i<numsedlayers; i++ )
   {
      numvararg += sedlayers[i]->NumVarArg();
   }
   if ( Pr != 0 )         numvararg += Pr->NumVarArg();
   if ( TauSurf != 0 )    numvararg += TauSurf->NumVarArg();
   if ( PressGrad != 0 )  numvararg += PressGrad->NumVarArg();
   if ( Pwind != 0 )      numvararg += Pwind->NumVarArg();
   if ( Pbottom != 0 )    numvararg += Pbottom->NumVarArg();
   if ( Pint != 0 )       numvararg += Pint->NumVarArg();
   if ( ProdDissip != 0 ) numvararg += ProdDissip->NumVarArg();
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
      for ( i=0; i<numsurfinput; i++ )
      {
         newarg[k] = surfinputvar[i]; k++;
         for ( j=0; j<surfinputflux[i]->NumVarArg(); j++ )
         {
            newarg[k] = surfinputflux[i]->VarArg(j); k++;
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
      pinf = pinflist.First();
      while ( pinf != 0 )
      {
         for ( j=0; j<pinf->NumVarArg(); j++ )
         {
            newarg[k] = pinf->VarArg(j); k++;
         }
         pinf = pinf->Next();
      }
      for ( i=0; i<numsedinput; i++ )
      {
         newarg[k] = sedinputvar[i]; k++;
         for ( j=0; j<sedinputflux[i]->NumVarArg(); j++ )
         {
            newarg[k] = sedinputflux[i]->VarArg(j); k++;
         }
      }
      for ( i=0; i<numdissvar; i++ )
      {
         newarg[k] = dissvar[i]; k++;
         for ( j=0; j<disskdiff[i]->NumVarArg(); j++ )
         {
            newarg[k] = disskdiff[i]->VarArg(j); k++;
         }
      }
      for ( i=0; i<numpartvar; i++ )
      {
         newarg[k] = partvar[i]; k++;
         for ( j=0; j<partrho[i]->NumVarArg(); j++ )
         {
            newarg[k] = partrho[i]->VarArg(j); k++;
         }
         for ( j=0; j<partsedveloc[i]->NumVarArg(); j++ )
         {
            newarg[k] = partsedveloc[i]->VarArg(j); k++;
         }
         for ( j=0; j<partmobility[i]->NumVarArg(); j++ )
         {
            newarg[k] = partmobility[i]->VarArg(j); k++;
         }
      }
      if ( A != 0 )
      {
         for ( i=0; i<A->NumVarArg(); i++ )
         {
            newarg[k] = A->VarArg(i); k++;
         }
      }
      if ( Rho != 0 )
      {
         for ( i=0; i<Rho->NumVarArg(); i++ )
         {
            newarg[k] = Rho->VarArg(i); k++;
         }
      }
      if ( D != 0 )
      {
         for ( i=0; i<D->NumVarArg(); i++ )
         {
            newarg[k] = D->VarArg(i); k++;
         }
      }
      if ( Isurf != 0 )
      {
         for ( i=0; i<Isurf->NumVarArg(); i++ )
         {
            newarg[k] = Isurf->VarArg(i); k++;
         }
      }
      if ( Eta != 0 )
      {
         for ( i=0; i<Eta->NumVarArg(); i++ )
         {
            newarg[k] = Eta->VarArg(i); k++;
         }
      }
      if ( Fvol != 0 )
      {
         for ( i=0; i<Fvol->NumVarArg(); i++ )
         {
            newarg[k] = Fvol->VarArg(i); k++;
         }
      }
      if ( kres != 0 )
      {
         for ( i=0; i<kres->NumVarArg(); i++ )
         {
            newarg[k] = kres->VarArg(i); k++;
         }
      }
      for ( i=0; i<numsedlayers; i++ )
      {
         for ( j=0; j<sedlayers[i]->NumVarArg(); j++ )
         {
            newarg[k] = sedlayers[i]->VarArg(j); k++;
         }
      }
      if ( Pr != 0 )
      {
         for ( i=0; i<Pr->NumVarArg(); i++ )
         {
            newarg[k] = Pr->VarArg(i); k++;
         }
      }
      if ( TauSurf != 0 )
      {
         for ( i=0; i<TauSurf->NumVarArg(); i++ )
         {
            newarg[k] = TauSurf->VarArg(i); k++;
         }
      }
      if ( PressGrad != 0 )
      {
         for ( i=0; i<PressGrad->NumVarArg(); i++ )
         {
            newarg[k] = PressGrad->VarArg(i); k++;
         }
      }
      if ( Pwind != 0 )
      {
         for ( i=0; i<Pwind->NumVarArg(); i++ )
         {
            newarg[k] = Pwind->VarArg(i); k++;
         }
      }
      if ( Pbottom != 0 )
      {
         for ( i=0; i<Pbottom->NumVarArg(); i++ )
         {
            newarg[k] = Pbottom->VarArg(i); k++;
         }
      }
      if ( Pint != 0 )
      {
         for ( i=0; i<Pint->NumVarArg(); i++ )
         {
            newarg[k] = Pint->VarArg(i); k++;
         }
      }
      if ( ProdDissip != 0 )
      {
         for ( i=0; i<ProdDissip->NumVarArg(); i++ )
         {
            newarg[k] = ProdDissip->VarArg(i); k++;
         }
      }
      if ( k != numvararg )
         FatalError("LAKECOMP::CalcArg: inconsistent number of arguments");
      delete [] vararg; vararg = newarg;
   }
   return;
}


BOOLEAN LAKECOMP::SetGridPts(CARDINAL n)
{
   if ( n < 5 )        return FALSE;
   if ( n == numgrid ) return TRUE;
   numgrid = n;
   delete gridpts; gridpts = new REAL[numgrid];
   ZTopBottom(Zt,Zb);
   return TRUE;
}


BOOLEAN LAKECOMP::NumGridPts(CARDINAL n)
{
   if ( InList() == TRUE ) return FALSE;
   return SetGridPts(n);
}


REAL LAKECOMP::GridPt(const REAL*, CARDINAL index)
{
   if ( index >= NumGridPts() )
      FatalError("LAKECOMP::GridPt: illegal index");
   return gridpts[index];
}


CARDINAL LAKECOMP::NumZoneGridPts(CARDINAL zone)
{
   if ( zone > numsedlayers ) FatalError("LAKECOMP::NumZoneGridPts: illegal zone");
   return NumGridPts();
}


REAL LAKECOMP::ZoneGridPt(const REAL* Y, CARDINAL zone,
                          CARDINAL index)
{
   if ( zone > numsedlayers ) FatalError("LAKECOMP::ZoneGridPt: illegal zone");
   return GridPt(Y,index);
}


BOOLEAN LAKECOMP::ZTopBottom(REAL zt, REAL zb)
{
   if ( zt == zb ) return FALSE;
   Zt = zt; Zb = zb;
   REAL dz = (Zb-Zt)/(numgrid-2);
   gridpts[0] = Zt; gridpts[numgrid-1] = Zb;
   for ( CARDINAL i=1; i<numgrid-1; i++ ) gridpts[i] = Zt + (i-0.5)*dz;
   return TRUE;
}


BOOLEAN LAKECOMP::Gravit(REAL g)
{
   if ( g <= 0.0 ) return FALSE;
   gravit = g;
   return TRUE;
}


BOOLEAN LAKECOMP::AccQ(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccQ = relacc;
   absaccQ = absacc;
   return TRUE;
}


BOOLEAN LAKECOMP::AccI(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccI = relacc;
   absaccI = absacc;
   return TRUE;
}


BOOLEAN LAKECOMP::AccU(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccU = relacc;
   absaccU = absacc;
   return TRUE;
}


BOOLEAN LAKECOMP::AccTKE(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccTKE = relacc;
   absaccTKE = absacc;
   return TRUE;
}


BOOLEAN LAKECOMP::AccEPS(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccEPS = relacc;
   absaccEPS = absacc;
   return TRUE;
}


BOOLEAN LAKECOMP::AccEseiche(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccEseiche = relacc;
   absaccEseiche = absacc;
   return TRUE;
}


BOOLEAN LAKECOMP::CrossSect(const char* inpline,
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


BOOLEAN LAKECOMP::Density(const char* inpline,
                          const VARLIST* varlist,
                          char* errorline,
                          CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("Rho");
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

   delete Rho; Rho = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::Diffusion(const char* inpline,
                           const VARLIST* varlist,
                           char* errorline,
                           CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("D");
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

   delete D; D = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::LightSurface(const char* inpline,
                               const VARLIST* varlist,
                               char* errorline,
                               CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("I");
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

   delete Isurf; Isurf = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::LightExtinct(const char* inpline,
                               const VARLIST* varlist,
                               char* errorline,
                               CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("eta");
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

   delete Eta; Eta = formvar;
   CalcArg();
   return TRUE;
}


void LAKECOMP::CalcSed(BOOLEAN b)
{
   if ( b == TRUE )
   {
      calcSed = TRUE;
      ised = 1;
   }
   else
   {
      calcSed = FALSE;
      ised = 0;
   }
   return;
}


BOOLEAN LAKECOMP::SedFluxVol(const char* inpline,
                             const VARLIST* varlist,
                             char* errorline,
                             CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("Fvol");
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

   delete Fvol; Fvol = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::SedResusp(const char* inpline,
                            const VARLIST* varlist,
                            char* errorline,
                            CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("kres");
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

   delete kres; kres = formvar;
   CalcArg();
   return TRUE;
}


void LAKECOMP::CalcTKE(BOOLEAN b)
{
   if ( b == TRUE )
   {
      calcTKE = TRUE;
      ioff = 5;
   }
   else
   {
      calcTKE = FALSE;
      ioff = 2;
   }
   return;
}


BOOLEAN LAKECOMP::Prandtl(const char* inpline,
                          const VARLIST* varlist,
                          char* errorline,
                          CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("Pr");
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

   delete Pr; Pr = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::SigmaTKE(REAL s)
{
   if ( s <= 0.0 ) return FALSE;
   sigmaTKE = s;
   return TRUE;
}


BOOLEAN LAKECOMP::SigmaEps(REAL s)
{
   if ( s <= 0.0 ) return FALSE;
   sigmaEps = s;
   return TRUE;
}


BOOLEAN LAKECOMP::KzMin(REAL r)
{
   if ( r < 0.0 ) return FALSE;
   kzmin = r;
   return TRUE;
}


BOOLEAN LAKECOMP::SurfaceShear(const char* inpline,
                               const VARLIST* varlist,
                               char* errorline,
                               CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("Tau");
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

   delete TauSurf; TauSurf = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::PressureGradient(const char* inpline,
                                   const VARLIST* varlist,
                                   char* errorline,
                                   CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("PressGrad");
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

   delete PressGrad; PressGrad = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::SeichePwind(const char* inpline,
                              const VARLIST* varlist,
                              char* errorline,
                              CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("Pwind");
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

   delete Pwind; Pwind = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::SeichePbottom(const char* inpline,
                                const VARLIST* varlist,
                                char* errorline,
                                CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("Pbottom");
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

   delete Pbottom; Pbottom = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::SeichePint(const char* inpline,
                             const VARLIST* varlist,
                             char* errorline,
                             CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("Pint");
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

   delete Pint; Pint = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN LAKECOMP::ProdDiss(const char* inpline,
                           const VARLIST* varlist,
                           char* errorline,
                           CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("ProdEps");
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

   delete ProdDissip; ProdDissip = formvar;
   CalcArg();
   return TRUE;
}


CARDINAL LAKECOMP::NumZones() const
{
   if ( calcSed == FALSE ) return 1;
   return 1+numsedlayers;
}


const char* LAKECOMP::ZoneName(CARDINAL index) const
{
   char Buf[BufSize];
   switch ( index )
   {
      case 0:
         return ini.T(96);               // LakeZoneWater
      default:
         strcpy(namebuffer,ini.T(207));  // LakeZoneSed
         NumToString(index,Buf,BufSize);
         strcat(namebuffer,Buf);
         return namebuffer;
   }
   return 0;
}


const char* LAKECOMP::AdvInName(CARDINAL conn) const
{
   if ( conn > 0 ) FatalError("LAKECOMP::AdvInName: Illegal connection");
   return ini.T(97);
}


const char* LAKECOMP::AdvOutName(CARDINAL conn) const
{
   if ( conn > 0 ) FatalError("LAKECOMP::AdvOutName: Illegal connection");
   return ini.T(98);
}


const char* LAKECOMP::DiffName(CARDINAL) const
{
   FatalError("LAKECOMP::DiffName: Illegal connection");
   return 0;
}


BOOLEAN LAKECOMP::AdvInExZ(CARDINAL) const
{
   return FALSE;
}


REAL LAKECOMP::AdvInZ(const REAL*, CARDINAL) const
{
   FatalError("LAKECOMP::AdvInZ: Illegal connection");
   return 0;
}


REAL LAKECOMP::AdvOutQ(VARSYS* varsys, const REAL* Y, CARDINAL conn)
{
   FatalError("LAKECOMP::AdvOutQ: Illegal connection");
   if ( Active() == FALSE ) return 0.0;
   return 0.0;
}


REAL LAKECOMP::AdvOutJ(VARSYS* varsys,
                      const REAL* Y,
                      CARDINAL conn,
                      const AQVAR* var)
{
   FatalError("LAKECOMP::AdvOutJ: Illegal connection");
   if ( Active() == FALSE ) return 0.0;
   return 0.0;
}


REAL LAKECOMP::DiffC(const REAL*, CARDINAL, const AQVAR*) const
{
   FatalError("LAKECOMP::DiffC: Illegal connection");
   return 0.0;
}


JOBSTATUS LAKECOMP::Load(std::istream& in,
                        const VARLIST* varlist, const PROCLIST* proclist)
{
   char Buffer[BufSize]; 
   char Buffer1[BufSize]; char Buffer2[BufSize];
   char Buffer3[BufSize]; char Buffer4[BufSize];
   char Buffer5[BufSize]; char Buffer6[BufSize];
   CARDINAL n, n1, n2, n3;

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
            /* ignore surface inflow */

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddSurfInput(varlist,var,Buffer,Buffer1,n) == FALSE )
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
            REAL xs,xe,g;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( ZTopBottom(xs,xe) == FALSE )                   return PROBLEM;
            if ( LoadNumber(in,&g) == FALSE )                   return PROBLEM;
            if ( Gravit(g) == FALSE )                           return PROBLEM;
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer3,sizeof(Buffer3)) == FALSE )
                                                                return PROBLEM;
               if ( AddPartVar(varlist,var,
				               "1000",Buffer2,n1,
				               Buffer3,Buffer4,n2,
							   "0",Buffer6,n3) == FALSE )
                                                                return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
			/* correct surface input fluxes (divide by area): */
			for ( CARDINAL i=0; i<numsurfinput; i++ )
            {
               surfinputflux[i]->UnParse(Buffer1,sizeof(Buffer1)-1);
               strcpy(Buffer2,"(");
			   strcat(Buffer2,Buffer1);
			   strcat(Buffer2,")/(");
			   strcat(Buffer2,Buffer);
			   strcat(Buffer2,")");
			   if ( surfinputflux[i]->Parse(Buffer2,varlist,Buffer3) != 0 )
                                                                return PROBLEM;
            }
			/* end of correction */
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Density(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Diffusion(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SurfaceShear(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            char Buffer2[BufSize]; Buffer2[0]='0'; Buffer2[1]='\0';
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            CalcTKE(TRUE);
         }
         break;
      case -2: // aquavers=-2: development version august 1995 (+Dissipation)
         {
            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,1);
                                            if ( status != OK ) return status;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            /* ignore surface inflow */

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddSurfInput(varlist,var,Buffer,Buffer1,n) == FALSE )
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
               AQVAR* var = varlist->Get(Buffer);
                                                  if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddLatInput(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }
            if ( LoadNumber(in,&n) == FALSE )                   return PROBLEM;
            if ( NumGridPts(n) == FALSE )                       return PROBLEM;
            if ( LoadBoolean(in,&highres) == FALSE )            return PROBLEM;
            REAL xs,xe,g;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( ZTopBottom(xs,xe) == FALSE )                   return PROBLEM;
            if ( LoadNumber(in,&g) == FALSE )                   return PROBLEM;
            if ( Gravit(g) == FALSE )                           return PROBLEM;
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer3,sizeof(Buffer3)) == FALSE )
                                                                return PROBLEM;
               if ( AddPartVar(varlist,var,
				               "1000",Buffer2,n1,
				               Buffer3,Buffer4,n2,
							   "0",Buffer6,n3) == FALSE )
                                                                return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
			/* correct surface input fluxes (divide by area): */
			for ( CARDINAL i=0; i<numsurfinput; i++ )
            {
               surfinputflux[i]->UnParse(Buffer1,sizeof(Buffer1)-1);
               strcpy(Buffer2,"(");
			   strcat(Buffer2,Buffer1);
			   strcat(Buffer2,")/(");
			   strcat(Buffer2,Buffer);
			   strcat(Buffer2,")");
			   if ( surfinputflux[i]->Parse(Buffer2,varlist,Buffer3) != 0 )
                                                                return PROBLEM;
            }
			/* end of correction */
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Density(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Diffusion(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SurfaceShear(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            // omit Dissipation;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            CalcTKE(TRUE);
         }
         break;
      case -3: // aquavers=-3: development version november 1995 (+Prandtl)
         {
            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,1);
                                            if ( status != OK ) return status;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            /* ignore surface inflow */

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddSurfInput(varlist,var,Buffer,Buffer1,n) == FALSE )
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
            REAL xs,xe,g;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( ZTopBottom(xs,xe) == FALSE )                   return PROBLEM;
            if ( LoadNumber(in,&g) == FALSE )                   return PROBLEM;
            if ( Gravit(g) == FALSE )                           return PROBLEM;
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer3,sizeof(Buffer3)) == FALSE )
                                                                return PROBLEM;
               if ( AddPartVar(varlist,var,
				               "1000",Buffer2,n1,
				               Buffer3,Buffer4,n2,
							   "0",Buffer6,n3) == FALSE )
                                                                return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
			/* correct surface input fluxes (divide by area): */
			for ( CARDINAL i=0; i<numsurfinput; i++ )
            {
               surfinputflux[i]->UnParse(Buffer1,sizeof(Buffer1)-1);
               strcpy(Buffer2,"(");
			   strcat(Buffer2,Buffer1);
			   strcat(Buffer2,")/(");
			   strcat(Buffer2,Buffer);
			   strcat(Buffer2,")");
			   if ( surfinputflux[i]->Parse(Buffer2,varlist,Buffer3) != 0 )
                                                                return PROBLEM;
            }
			/* end of correction */
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Density(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Diffusion(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            BOOLEAN b;
            if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcTKE(b);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Prandtl(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SurfaceShear(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            // omit Dissipation;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
         }
         break;
      case -4: // aquavers=-4: development version january 1996 (+compindex)
         {
            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,2);
                                            if ( status != OK ) return status;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            /* ignore surface inflow */

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddSurfInput(varlist,var,Buffer,Buffer1,n) == FALSE )
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
            REAL xs,xe,g;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( ZTopBottom(xs,xe) == FALSE )                   return PROBLEM;
            if ( LoadNumber(in,&g) == FALSE )                   return PROBLEM;
            if ( Gravit(g) == FALSE )                           return PROBLEM;
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer3,sizeof(Buffer3)) == FALSE )
                                                                return PROBLEM;
               if ( AddPartVar(varlist,var,
				               "1000",Buffer2,n1,
				               Buffer3,Buffer4,n2,
							   "0",Buffer6,n3) == FALSE )
                                                                return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
			/* correct surface input fluxes (divide by area): */
			for ( CARDINAL i=0; i<numsurfinput; i++ )
            {
               surfinputflux[i]->UnParse(Buffer1,sizeof(Buffer1)-1);
               strcpy(Buffer2,"(");
			   strcat(Buffer2,Buffer1);
			   strcat(Buffer2,")/(");
			   strcat(Buffer2,Buffer);
			   strcat(Buffer2,")");
			   if ( surfinputflux[i]->Parse(Buffer2,varlist,Buffer3) != 0 )
                                                                return PROBLEM;
            }
			/* end of correction */
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Density(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Diffusion(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            BOOLEAN b;
            if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcTKE(b);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Prandtl(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SurfaceShear(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            // omit Dissipation;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
         }
         break;
      case -5: // aquavers=-5: development version june 1996 (+sediment)
         {
            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,2);
                                            if ( status != OK ) return status;

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
            /* ignore surface inflow */

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddSurfInput(varlist,var,Buffer,Buffer1,n) == FALSE )
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

            if ( LoadStart(in) == FALSE )  return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               if ( strcmp(Buffer,PointInfIdentifier) != 0 ) return PROBLEM;
               PINF* pinf = new PINF;
               if ( pinf->Load(in,varlist) == PROBLEM )
               {
                  delete pinf; return PROBLEM;
               }
               if ( AddPointInf(pinf) == PROBLEM )
               {
                  delete pinf; return PROBLEM;
               }
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddSedInput(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadNumber(in,&n) == FALSE )                   return PROBLEM;
            if ( NumGridPts(n) == FALSE )                       return PROBLEM;
            if ( LoadBoolean(in,&highres) == FALSE )            return PROBLEM;
            REAL xs,xe,r;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( ZTopBottom(xs,xe) == FALSE )                   return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( Gravit(r) == FALSE )                           return PROBLEM;

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
                               Buffer3,Buffer4,n2,
							   "0",Buffer6,n3) == FALSE )   return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
			/* correct surface input fluxes (divide by area): */
			for ( CARDINAL i=0; i<numsurfinput; i++ )
            {
               surfinputflux[i]->UnParse(Buffer1,sizeof(Buffer1)-1);
               strcpy(Buffer2,"(");
			   strcat(Buffer2,Buffer1);
			   strcat(Buffer2,")/(");
			   strcat(Buffer2,Buffer);
			   strcat(Buffer2,")");
			   if ( surfinputflux[i]->Parse(Buffer2,varlist,Buffer3) != 0 )
                                                                return PROBLEM;
            }
			/* end of correction */
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Density(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Diffusion(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            BOOLEAN b;
            if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcSed(b);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SedFluxVol(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SedResusp(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;

			LAKESEDLAYER* l = new LAKESEDLAYER;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
			NumToString(r,Buffer,sizeof(Buffer));
			if ( l->Porosity(Buffer,varlist,Buffer1,n) == FALSE )
				                                                return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
			NumToString(r,Buffer,sizeof(Buffer));
			if ( l->Thickness(Buffer,varlist,Buffer1,n) == FALSE )
				                                                return PROBLEM;
			if ( AddSedLayer(l) == FALSE )                      return PROBLEM;

			if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcTKE(b);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Prandtl(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SurfaceShear(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            // omit Dissipation;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePwind(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePbottom(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePint(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
         }
         break;
      case 1: // aquavers=1: AQUASIM 2.0 beta aug. 1996
         {
            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,2);
                                            if ( status != OK ) return status;

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
            /* ignore surface inflow */
            
			if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddSurfInput(varlist,var,Buffer,Buffer1,n) == FALSE )
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

            if ( LoadStart(in) == FALSE )  return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               if ( strcmp(Buffer,PointInfIdentifier) != 0 ) return PROBLEM;
               PINF* pinf = new PINF;
               if ( pinf->Load(in,varlist) == PROBLEM )
               {
                  delete pinf; return PROBLEM;
               }
               if ( AddPointInf(pinf) == PROBLEM )
               {
                  delete pinf; return PROBLEM;
               }
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddSedInput(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadNumber(in,&n) == FALSE )                   return PROBLEM;
            if ( NumGridPts(n) == FALSE )                       return PROBLEM;
            if ( LoadBoolean(in,&highres) == FALSE )            return PROBLEM;
            REAL xs,xe,r;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( ZTopBottom(xs,xe) == FALSE )                   return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( Gravit(r) == FALSE )                           return PROBLEM;

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
                               Buffer3,Buffer4,n2,
							   "0",Buffer6,n3) == FALSE )   return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
			/* correct surface input fluxes (divide by area): */
			for ( CARDINAL i=0; i<numsurfinput; i++ )
            {
               surfinputflux[i]->UnParse(Buffer1,sizeof(Buffer1)-1);
               strcpy(Buffer2,"(");
			   strcat(Buffer2,Buffer1);
			   strcat(Buffer2,")/(");
			   strcat(Buffer2,Buffer);
			   strcat(Buffer2,")");
			   if ( surfinputflux[i]->Parse(Buffer2,varlist,Buffer3) != 0 )
                                                                return PROBLEM;
            }
			/* end of correction */
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Density(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Diffusion(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            BOOLEAN b;
            if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcSed(b);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SedFluxVol(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SedResusp(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;

			LAKESEDLAYER* l = new LAKESEDLAYER;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
			NumToString(r,Buffer,sizeof(Buffer));
			if ( l->Porosity(Buffer,varlist,Buffer1,n) == FALSE )
				                                                return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
			NumToString(r,Buffer,sizeof(Buffer));
			if ( l->Thickness(Buffer,varlist,Buffer1,n) == FALSE )
				                                                return PROBLEM;
			if ( AddSedLayer(l) == FALSE )                      return PROBLEM;
            
			if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcTKE(b);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Prandtl(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SurfaceShear(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            // omit Dissipation;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePwind(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePbottom(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePint(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
         }
         break;
      case 2: // aquavers=2: AQUASIM 2.0 beta aug. 1996, surface inflow eliminated
         {
            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,2);
                                            if ( status != OK ) return status;

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
               if ( AddSurfInput(varlist,var,Buffer,Buffer1,n) == FALSE )
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

            if ( LoadStart(in) == FALSE )  return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               if ( strcmp(Buffer,PointInfIdentifier) != 0 ) return PROBLEM;
               PINF* pinf = new PINF;
               if ( pinf->Load(in,varlist) == PROBLEM )
               {
                  delete pinf; return PROBLEM;
               }
               if ( AddPointInf(pinf) == PROBLEM )
               {
                  delete pinf; return PROBLEM;
               }
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddSedInput(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadNumber(in,&n) == FALSE )                   return PROBLEM;
            if ( NumGridPts(n) == FALSE )                       return PROBLEM;
            if ( LoadBoolean(in,&highres) == FALSE )            return PROBLEM;
            REAL xs,xe,r;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( ZTopBottom(xs,xe) == FALSE )                   return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( Gravit(r) == FALSE )                           return PROBLEM;

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
                               Buffer3,Buffer4,n2,
							   "0",Buffer6,n3) == FALSE )   return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Density(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Diffusion(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            BOOLEAN b;
            if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcSed(b);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SedFluxVol(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SedResusp(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;

			LAKESEDLAYER* l = new LAKESEDLAYER;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
			NumToString(r,Buffer,sizeof(Buffer));
			if ( l->Porosity(Buffer,varlist,Buffer1,n) == FALSE )
				                                                return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
			NumToString(r,Buffer,sizeof(Buffer));
			if ( l->Thickness(Buffer,varlist,Buffer1,n) == FALSE )
				                                                return PROBLEM;
			if ( AddSedLayer(l) == FALSE )                      return PROBLEM;

			if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcTKE(b);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Prandtl(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SurfaceShear(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            // omit Dissipation;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePwind(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePbottom(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePint(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
         }
         break;
      case 3: // aquavers=3: AQUASIM 2.0 beta nov. 1996, pressure gradient introduced
         {
            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,2);
                                            if ( status != OK ) return status;

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
               if ( AddSurfInput(varlist,var,Buffer,Buffer1,n) == FALSE )
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

            if ( LoadStart(in) == FALSE )  return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               if ( strcmp(Buffer,PointInfIdentifier) != 0 ) return PROBLEM;
               PINF* pinf = new PINF;
               if ( pinf->Load(in,varlist) == PROBLEM )
               {
                  delete pinf; return PROBLEM;
               }
               if ( AddPointInf(pinf) == PROBLEM )
               {
                  delete pinf; return PROBLEM;
               }
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddSedInput(varlist,var,Buffer,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            }

            if ( LoadNumber(in,&n) == FALSE )                   return PROBLEM;
            if ( NumGridPts(n) == FALSE )                       return PROBLEM;
            if ( LoadBoolean(in,&highres) == FALSE )            return PROBLEM;
            REAL xs,xe,r;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( ZTopBottom(xs,xe) == FALSE )                   return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( Gravit(r) == FALSE )                           return PROBLEM;

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
                               Buffer3,Buffer4,n2,
							   "0",Buffer6,n3) == FALSE )   return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Density(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Diffusion(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            BOOLEAN b;
            if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcSed(b);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SedFluxVol(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SedResusp(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;

			LAKESEDLAYER* l = new LAKESEDLAYER;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
			NumToString(r,Buffer,sizeof(Buffer));
			if ( l->Porosity(Buffer,varlist,Buffer1,n) == FALSE )
				                                                return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
			NumToString(r,Buffer,sizeof(Buffer));
			if ( l->Thickness(Buffer,varlist,Buffer1,n) == FALSE )
				                                                return PROBLEM;
			if ( AddSedLayer(l) == FALSE )                      return PROBLEM;
            
			if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcTKE(b);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Prandtl(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SurfaceShear(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( PressureGradient(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            // omit Dissipation;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePwind(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePbottom(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePint(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
         }
         break;
      case 4: // aquavers=4: AQUASIM 2.0 gamma march 1997, accuracies introduced
         {
            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,2);
                                            if ( status != OK ) return status;

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
               if ( AddSurfInput(varlist,var,Buffer,Buffer1,n) == FALSE )
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

            if ( LoadStart(in) == FALSE )  return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               if ( strcmp(Buffer,PointInfIdentifier) != 0 ) return PROBLEM;
               PINF* pinf = new PINF;
               if ( pinf->Load(in,varlist) == PROBLEM )
               {
                  delete pinf; return PROBLEM;
               }
               if ( AddPointInf(pinf) == PROBLEM )
               {
                  delete pinf; return PROBLEM;
               }
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddSedInput(varlist,var,Buffer,Buffer1,n) == FALSE )
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
			// omit accuracies for N2
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccU(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccTKE(relacc,absacc) == FALSE )               return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccEseiche(relacc,absacc) == FALSE )           return PROBLEM;
            REAL xs,xe,r;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( ZTopBottom(xs,xe) == FALSE )                   return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( Gravit(r) == FALSE )                           return PROBLEM;

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
                               Buffer3,Buffer4,n2,
							   "0",Buffer6,n3) == FALSE )   return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Density(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Diffusion(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            BOOLEAN b;
            if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcSed(b);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SedFluxVol(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SedResusp(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;

			LAKESEDLAYER* l = new LAKESEDLAYER;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
			NumToString(r,Buffer,sizeof(Buffer));
			if ( l->Porosity(Buffer,varlist,Buffer1,n) == FALSE )
				                                                return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
			NumToString(r,Buffer,sizeof(Buffer));
			if ( l->Thickness(Buffer,varlist,Buffer1,n) == FALSE )
				                                                return PROBLEM;
			if ( AddSedLayer(l) == FALSE )                      return PROBLEM;

			if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcTKE(b);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Prandtl(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SurfaceShear(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( PressureGradient(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            // omit Dissipation;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePwind(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePbottom(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePint(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
         }
         break;
      case 5: // aquavers=5: AQUASIM 2.0 eps jan 1998, eps equation added
         {
            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,2);
                                            if ( status != OK ) return status;

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
               if ( AddSurfInput(varlist,var,Buffer,Buffer1,n) == FALSE )
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

            if ( LoadStart(in) == FALSE )  return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               if ( strcmp(Buffer,PointInfIdentifier) != 0 ) return PROBLEM;
               PINF* pinf = new PINF;
               if ( pinf->Load(in,varlist) == PROBLEM )
               {
                  delete pinf; return PROBLEM;
               }
               if ( AddPointInf(pinf) == PROBLEM )
               {
                  delete pinf; return PROBLEM;
               }
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddSedInput(varlist,var,Buffer,Buffer1,n) == FALSE )
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
			if ( AccU(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccTKE(relacc,absacc) == FALSE )               return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccEPS(relacc,absacc) == FALSE )               return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccEseiche(relacc,absacc) == FALSE )           return PROBLEM;
            REAL xs,xe,r;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( ZTopBottom(xs,xe) == FALSE )                   return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( Gravit(r) == FALSE )                           return PROBLEM;

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
                               Buffer3,Buffer4,n2,
							   "0",Buffer6,n3) == FALSE )       return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Density(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Diffusion(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            BOOLEAN b;
            if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcSed(b);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SedFluxVol(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SedResusp(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;

			LAKESEDLAYER* l = new LAKESEDLAYER;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
			NumToString(r,Buffer,sizeof(Buffer));
			if ( l->Porosity(Buffer,varlist,Buffer1,n) == FALSE )
				                                                return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
			NumToString(r,Buffer,sizeof(Buffer));
			if ( l->Thickness(Buffer,varlist,Buffer1,n) == FALSE )
				                                                return PROBLEM;
			if ( AddSedLayer(l) == FALSE )                      return PROBLEM;

			if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcTKE(b);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Prandtl(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( SigmaTKE(r) == FALSE )                         return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( SigmaEps(r) == FALSE )                         return PROBLEM;            
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SurfaceShear(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( PressureGradient(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePwind(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePbottom(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePint(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( ProdDiss(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
         }
         break;
      case 6: // aquavers=6: AQUASIM 2.0 delta mai 1998, sediment layers added
              //             Mai 1998, BOOLEAN active added
         {
            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,3);
                                            if ( status != OK ) return status;

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
               if ( AddSurfInput(varlist,var,Buffer,Buffer1,n) == FALSE )
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

            if ( LoadStart(in) == FALSE )  return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               if ( strcmp(Buffer,PointInfIdentifier) != 0 ) return PROBLEM;
               PINF* pinf = new PINF;
               if ( pinf->Load(in,varlist) == PROBLEM )
               {
                  delete pinf; return PROBLEM;
               }
               if ( AddPointInf(pinf) == PROBLEM )
               {
                  delete pinf; return PROBLEM;
               }
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddSedInput(varlist,var,Buffer,Buffer1,n) == FALSE )
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
			if ( AccU(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccTKE(relacc,absacc) == FALSE )               return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccEPS(relacc,absacc) == FALSE )               return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccEseiche(relacc,absacc) == FALSE )           return PROBLEM;
            REAL xs,xe,r;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( ZTopBottom(xs,xe) == FALSE )                   return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( Gravit(r) == FALSE )                           return PROBLEM;

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
                               Buffer3,Buffer4,n2,
							   "0",Buffer6,n3) == FALSE )       return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Density(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Diffusion(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            BOOLEAN b;
            if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcSed(b);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SedFluxVol(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SedResusp(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               if ( strcmp(Buffer,LakeSedLayerIdentifier) != 0 ) return PROBLEM;
               LAKESEDLAYER* layer = new LAKESEDLAYER;
               if ( layer->Load(in,varlist) == PROBLEM )
               {
                  delete layer; return PROBLEM;
               }
               if ( AddSedLayer(layer) == PROBLEM )
               {
                  delete layer; return PROBLEM;
               }
            }

			if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcTKE(b);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Prandtl(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( SigmaTKE(r) == FALSE )                         return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( SigmaEps(r) == FALSE )                         return PROBLEM;            
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( KzMin(r) == FALSE )                            return PROBLEM;            
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SurfaceShear(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( PressureGradient(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePwind(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePbottom(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePint(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( ProdDiss(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
         }
         break;
      case 7: // aquavers=7: AQUASIM 2.1_beta01 Jan 1999, light intensity added
         {
            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,3);
                                            if ( status != OK ) return status;

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
               if ( AddSurfInput(varlist,var,Buffer,Buffer1,n) == FALSE )
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

            if ( LoadStart(in) == FALSE )  return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               if ( strcmp(Buffer,PointInfIdentifier) != 0 ) return PROBLEM;
               PINF* pinf = new PINF;
               if ( pinf->Load(in,varlist) == PROBLEM )
               {
                  delete pinf; return PROBLEM;
               }
               if ( AddPointInf(pinf) == PROBLEM )
               {
                  delete pinf; return PROBLEM;
               }
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddSedInput(varlist,var,Buffer,Buffer1,n) == FALSE )
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
			if ( AccI(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccU(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccTKE(relacc,absacc) == FALSE )               return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccEPS(relacc,absacc) == FALSE )               return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccEseiche(relacc,absacc) == FALSE )           return PROBLEM;
            REAL xs,xe,r;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( ZTopBottom(xs,xe) == FALSE )                   return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( Gravit(r) == FALSE )                           return PROBLEM;

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
                               Buffer3,Buffer4,n2,
							   "0",Buffer6,n3) == FALSE )       return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Density(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Diffusion(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( LightSurface(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( LightExtinct(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            BOOLEAN b;
            if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcSed(b);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SedFluxVol(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SedResusp(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               if ( strcmp(Buffer,LakeSedLayerIdentifier) != 0 ) return PROBLEM;
               LAKESEDLAYER* layer = new LAKESEDLAYER;
               if ( layer->Load(in,varlist) == PROBLEM )
               {
                  delete layer; return PROBLEM;
               }
               if ( AddSedLayer(layer) == PROBLEM )
               {
                  delete layer; return PROBLEM;
               }
            }

			if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcTKE(b);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Prandtl(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( SigmaTKE(r) == FALSE )                         return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( SigmaEps(r) == FALSE )                         return PROBLEM;            
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( KzMin(r) == FALSE )                            return PROBLEM;            
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SurfaceShear(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( PressureGradient(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePwind(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePbottom(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePint(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( ProdDiss(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
         }
         break;
      case 8: // aquavers=8: AQUASIM 2.1e April 2003, particle mobility added
         {
            JOBSTATUS status = AQCOMP::Load(in,varlist,proclist,3);
                                            if ( status != OK ) return status;

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
               if ( AddSurfInput(varlist,var,Buffer,Buffer1,n) == FALSE )
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

            if ( LoadStart(in) == FALSE )  return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               if ( strcmp(Buffer,PointInfIdentifier) != 0 ) return PROBLEM;
               PINF* pinf = new PINF;
               if ( pinf->Load(in,varlist) == PROBLEM )
               {
                  delete pinf; return PROBLEM;
               }
               if ( AddPointInf(pinf) == PROBLEM )
               {
                  delete pinf; return PROBLEM;
               }
            }

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
               if ( AddSedInput(varlist,var,Buffer,Buffer1,n) == FALSE )
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
			if ( AccI(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccU(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccTKE(relacc,absacc) == FALSE )               return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccEPS(relacc,absacc) == FALSE )               return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccEseiche(relacc,absacc) == FALSE )           return PROBLEM;
            REAL xs,xe,r;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( ZTopBottom(xs,xe) == FALSE )                   return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( Gravit(r) == FALSE )                           return PROBLEM;

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
                               Buffer3,Buffer4,n2,
							   Buffer5,Buffer6,n3) == FALSE )   return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Density(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Diffusion(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( LightSurface(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( LightExtinct(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            BOOLEAN b;
            if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcSed(b);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SedFluxVol(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SedResusp(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;

            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               if ( strcmp(Buffer,LakeSedLayerIdentifier) != 0 ) return PROBLEM;
               LAKESEDLAYER* layer = new LAKESEDLAYER;
               if ( layer->Load(in,varlist) == PROBLEM )
               {
                  delete layer; return PROBLEM;
               }
               if ( AddSedLayer(layer) == PROBLEM )
               {
                  delete layer; return PROBLEM;
               }
            }

			if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcTKE(b);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Prandtl(Buffer,varlist,Buffer1,n) == FALSE )   return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( SigmaTKE(r) == FALSE )                         return PROBLEM;
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( SigmaEps(r) == FALSE )                         return PROBLEM;            
            if ( LoadNumber(in,&r) == FALSE )                   return PROBLEM;
            if ( KzMin(r) == FALSE )                            return PROBLEM;            
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SurfaceShear(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( PressureGradient(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePwind(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePbottom(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( SeichePint(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( ProdDiss(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                                  return PROBLEM;
   return OK;
}


JOBSTATUS LAKECOMP::Save(std::ostream& out)
{
   char Buffer[BufSize]; CARDINAL i;

   INTEGER aquavers = 8;

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
   for ( i=0; i<numsurfinput; i++ )
   {
      if ( SaveString(out,SurfInputVar(i)->Symbol()) == FALSE ) 
                                                               return PROBLEM;
      if ( SurfInputFlux(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
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

   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   PINF* pinf = pinflist.First();
   while ( pinf != 0 )
   {
      if ( SaveString(out,PointInfIdentifier) == FALSE )       return PROBLEM;
      status = pinf->Save(out); if ( status != OK ) return status;
      pinf = pinf->Next();
   }
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;

   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   for ( i=0; i<numsedinput; i++ )
   {
      if ( SaveString(out,SedInputVar(i)->Symbol()) == FALSE ) return PROBLEM;
      if ( SedInputFlux(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                   return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;

   if ( SaveNumber(out,numgrid) == FALSE )                     return PROBLEM;
   if ( SaveBoolean(out,HighRes()) == FALSE )                  return PROBLEM;
   if ( SaveNumber(out,RelAccQ()) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,AbsAccQ()) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,RelAccI()) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,AbsAccI()) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,RelAccU()) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,AbsAccU()) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,RelAccTKE()) == FALSE )                 return PROBLEM;
   if ( SaveNumber(out,AbsAccTKE()) == FALSE )                 return PROBLEM;
   if ( SaveNumber(out,RelAccEPS()) == FALSE )                 return PROBLEM;
   if ( SaveNumber(out,AbsAccEPS()) == FALSE )                 return PROBLEM;
   if ( SaveNumber(out,RelAccEseiche()) == FALSE )             return PROBLEM;
   if ( SaveNumber(out,AbsAccEseiche()) == FALSE )             return PROBLEM;
   if ( SaveNumber(out,Zt) == FALSE )                          return PROBLEM;
   if ( SaveNumber(out,Zb) == FALSE )                          return PROBLEM;
   if ( SaveNumber(out,gravit) == FALSE )                      return PROBLEM;
   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   for ( i=0; i<numdissvar; i++ )
   {
      if ( SaveString(out,DissVar(i)->Symbol()) == FALSE )     return PROBLEM;
      if ( Disskdiff(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
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
      if ( PartSedVeloc(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                   return PROBLEM;
      if ( PartMobility(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                   return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;
   if ( A->UnParse(Buffer,sizeof(Buffer)-1) != 0 )             return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( Rho->UnParse(Buffer,sizeof(Buffer)-1) != 0 )           return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( D->UnParse(Buffer,sizeof(Buffer)-1) != 0 )             return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( Isurf->UnParse(Buffer,sizeof(Buffer)-1) != 0 )         return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( Eta->UnParse(Buffer,sizeof(Buffer)-1) != 0 )           return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( SaveBoolean(out,calcSed) == FALSE )                    return PROBLEM;
   if ( Fvol->UnParse(Buffer,sizeof(Buffer)-1) != 0 )          return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( kres->UnParse(Buffer,sizeof(Buffer)-1) != 0 )          return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   for ( i=0; i<numsedlayers; i++ )
   {
      if ( SaveString(out,LakeSedLayerIdentifier) == FALSE )       return PROBLEM;
      status = sedlayers[i]->Save(out);    if ( status != OK ) return status;
   }   
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;
   if ( SaveBoolean(out,calcTKE) == FALSE )                    return PROBLEM;
   if ( Pr->UnParse(Buffer,sizeof(Buffer)-1) != 0 )            return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( SaveNumber(out,sigmaTKE) == FALSE )                    return PROBLEM;
   if ( SaveNumber(out,sigmaEps) == FALSE )                    return PROBLEM;
   if ( SaveNumber(out,kzmin) == FALSE )                       return PROBLEM;
   if ( TauSurf->UnParse(Buffer,sizeof(Buffer)-1) != 0 )       return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( PressGrad->UnParse(Buffer,sizeof(Buffer)-1) != 0 )     return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( Pwind->UnParse(Buffer,sizeof(Buffer)-1) != 0 )         return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( Pbottom->UnParse(Buffer,sizeof(Buffer)-1) != 0 )       return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( Pint->UnParse(Buffer,sizeof(Buffer)-1) != 0 )          return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( ProdDissip->UnParse(Buffer,sizeof(Buffer)-1) != 0 )    return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;

   if ( SaveEnd(out) == FALSE )                                return PROBLEM;
   return OK;
}


JOBSTATUS LAKECOMP::Write(std::ostream& out, BOOLEAN sh)
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
      if ( Print(out,ini.T(151),pos3) == FALSE )               return PROBLEM;
      if ( Print(out,"(") == FALSE )                           return PROBLEM;
      if ( Print(out,ini.T(152)) == FALSE )                    return PROBLEM;
      if ( Print(out,") : ") == FALSE )                        return PROBLEM;
      if ( Print(out,ini.T(153)) == FALSE )                    return PROBLEM;
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
   if ( Print(out,ini.T(147),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( NumSurfInput() > 0 )
   {
      PrintIndent(pos4-1);
      if ( Print(out,ini.T(148),pos3) == FALSE )               return PROBLEM;
      if ( Print(out," : ") == FALSE )                         return PROBLEM;
      if ( Print(out,ini.T(149)) == FALSE )                    return PROBLEM;
      for ( i=0; i<NumSurfInput(); i++ )
      {
         if ( Print(out,SurfInputVar(i)->Symbol(),pos3) == FALSE )
                                                               return PROBLEM;
         if ( Print(out," : ") == FALSE )                      return PROBLEM;
         if ( SurfInputFlux(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer) == FALSE )                     return PROBLEM;
      }
   }

   if ( Print(out,ini.T(110),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( LatInflow() != 0 )
   {
      if ( latinflow->UnParse(Buffer,sizeof(Buffer)-1) != 0 )  return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;

      if ( NumLatInput() > 0 )
      {
         if ( Print(out,ini.T(111),pos1) == FALSE )            return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( Print(out,ini.T(112),pos3) == FALSE )            return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( Print(out,ini.T(113),pos2) == FALSE )            return PROBLEM;
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

   if ( Print(out,ini.T(203),pos1) == FALSE )                 return PROBLEM;
   if ( Print(out,":") == FALSE )                             return PROBLEM;
   if ( pinflist.Size() > 0 )
   {
      PINF* pinf = pinflist.First();
      while ( pinf != 0 )
      {
         status = pinf->Write(out);       if ( status != OK ) return status;
         pinf = pinf->Next();
      }
   }

   if ( Print(out,ini.T(204),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( NumSedInput() > 0 )
   {
      PrintIndent(pos4-1);
      if ( Print(out,ini.T(205),pos3) == FALSE )               return PROBLEM;
      if ( Print(out," : ") == FALSE )                         return PROBLEM;
      if ( Print(out,ini.T(206)) == FALSE )                    return PROBLEM;
      for ( i=0; i<NumSedInput(); i++ )
      {
         if ( Print(out,SedInputVar(i)->Symbol(),pos3) == FALSE )
                                                               return PROBLEM;
         if ( Print(out," : ") == FALSE )                      return PROBLEM;
         if ( SedInputFlux(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer) == FALSE )                     return PROBLEM;
      }
   }

   if ( Print(out,ini.T(114),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,ZTop(),pos2) == FALSE )                      return PROBLEM;

   if ( Print(out,ini.T(115),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,ZBottom(),pos2) == FALSE )                   return PROBLEM;

   if ( Print(out,ini.T(136),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,Gravit(),pos2) == FALSE )                    return PROBLEM;

   if ( NumDissVar() > 0 )
   {
      if ( Print(out,ini.T(194),pos1) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Print(out,ini.T(195),pos3) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Print(out,ini.T(196),pos2) == FALSE )               return PROBLEM;
      for ( i=0; i<NumDissVar(); i++ )
      {
         if ( Print(out,DissVar(i)->Symbol(),pos3) == FALSE )
                                                               return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( Disskdiff(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer,pos2) == FALSE )                return PROBLEM;
      }
   }

   if ( NumPartVar() > 0 )
   {
      if ( Print(out,ini.T(137),pos1) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Print(out,ini.T(138),pos3) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Print(out,ini.T(139),pos2) == FALSE )               return PROBLEM;
      if ( Print(out,", ") == FALSE )                          return PROBLEM;
      if ( Print(out,ini.T(135)) == FALSE )                    return PROBLEM;
      if ( Print(out,", ") == FALSE )                          return PROBLEM;
      if ( Print(out,ini.T(300)) == FALSE )                    return PROBLEM;
      for ( i=0; i<NumPartVar(); i++ )
      {
         if ( Print(out,PartVar(i)->Symbol(),pos3) == FALSE )
                                                               return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( PartRho(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer,pos2) == FALSE )                return PROBLEM;
         if ( Print(out,", ") == FALSE )                       return PROBLEM;
         if ( PartSedVeloc(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer) == FALSE )                     return PROBLEM;
         if ( Print(out,", ") == FALSE )                       return PROBLEM;
         if ( PartMobility(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer) == FALSE )                     return PROBLEM;
      }
   }

   if ( Print(out,ini.T(116),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( A->UnParse(Buffer,sizeof(Buffer)-1) != 0 )             return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                      return PROBLEM;

   if ( Print(out,ini.T(130),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Rho->UnParse(Buffer,sizeof(Buffer)-1) != 0 )           return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                      return PROBLEM;

   if ( Print(out,ini.T(117),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( D->UnParse(Buffer,sizeof(Buffer)-1) != 0 )             return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                      return PROBLEM;

   if ( Print(out,ini.T(266),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Isurf->UnParse(Buffer,sizeof(Buffer)-1) != 0 )         return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                      return PROBLEM;

   if ( Print(out,ini.T(267),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Eta->UnParse(Buffer,sizeof(Buffer)-1) != 0 )           return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                      return PROBLEM;

   if ( CalcSed() == TRUE )
   {
      for ( i=0; i<numsedlayers; i++ )
      {
         if ( Print(out,ini.T(258),pos1) == FALSE )            return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         status = sedlayers[i]->Write(out);
                                           if ( status != OK ) return status;
      }
   }
   
   if ( CalcTKE() == TRUE )
   {
      if ( Print(out,ini.T(144),pos1) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Pr->UnParse(Buffer,sizeof(Buffer)-1) != 0 )         return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;

      if ( Print(out,ini.T(253),pos1) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Print(out,SigmaTKE(),pos2) == FALSE )               return PROBLEM;

      if ( Print(out,ini.T(254),pos1) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Print(out,SigmaEps(),pos2) == FALSE )               return PROBLEM;

      if ( Print(out,ini.T(131),pos1) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( TauSurf->UnParse(Buffer,sizeof(Buffer)-1) != 0 )    return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;

      if ( Print(out,ini.T(208),pos1) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( PressGrad->UnParse(Buffer,sizeof(Buffer)-1) != 0 )  return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;

      if ( Print(out,ini.T(132),pos1) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Pwind->UnParse(Buffer,sizeof(Buffer)-1) != 0 )      return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;

      if ( Print(out,ini.T(133),pos1) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Pbottom->UnParse(Buffer,sizeof(Buffer)-1) != 0 )    return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;

      if ( Print(out,ini.T(134),pos1) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Pint->UnParse(Buffer,sizeof(Buffer)-1) != 0 )       return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;

      if ( Print(out,ini.T(143),pos1) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( ProdDissip->UnParse(Buffer,sizeof(Buffer)-1) != 0 ) return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;
   }

   if ( Print(out,ini.T(118),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,NumGridPts(),pos2) == FALSE )                return PROBLEM;
   if ( Print(out," (") == FALSE )                             return PROBLEM;
   if ( HighRes() == FALSE )
   {
      if ( Print(out,ini.T(119)) == FALSE )                    return PROBLEM;
   }
   else
   {
      if ( Print(out,ini.T(120)) == FALSE )                    return PROBLEM;
   }
   if ( Print(out,")") == FALSE )                              return PROBLEM;
   if ( Print(out,ini.T(209),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,ini.T(210),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,RelAccQ(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(211),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,AbsAccQ(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(268),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,RelAccI(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(269),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,AbsAccI(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(214),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,RelAccU(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(215),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,AbsAccU(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(216),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,RelAccTKE(),pos2) == FALSE )                 return PROBLEM;
   if ( Print(out,ini.T(217),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,AbsAccTKE(),pos2) == FALSE )                 return PROBLEM;
   if ( Print(out,ini.T(212),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,RelAccEPS(),pos2) == FALSE )                 return PROBLEM;
   if ( Print(out,ini.T(213),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,AbsAccEPS(),pos2) == FALSE )                 return PROBLEM;
   if ( Print(out,ini.T(218),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,RelAccEseiche(),pos2) == FALSE )             return PROBLEM;
   if ( Print(out,ini.T(219),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,AbsAccEseiche(),pos2) == FALSE )             return PROBLEM;

   return OK;
}

//////////////////////////////////////////////////////////////////////////////
