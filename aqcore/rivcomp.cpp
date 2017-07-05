///////////////////////////////   rivcomp.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    10.12.91    Peter Reichert    kinematic wave, advection
// revisions:   07.04.92    Peter Reichert    dispersion added
//              26.10.92    Peter Reichert    redesign of links
//              20.11.92    Peter Reichert    diffusive wave implemented
//              24.05.94    Peter Reichert    more robust initial condition,
//                                            lateral inflow
//              28.12.98    Peter Reichert    sediment layers added
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
// cellhyd = 3+nv+ns+ne         
// cellsed = 1+nv+ns+ne 
// without sediment:  cell = cellhyd
// with sediment:     cell = cellhyd + nl*cellsed  ,  nl=numsedlayers
// type component                             meaning
// ---------------------------------------------------------------------------
// alg  y[0]                                  Q at x[0]
// alg  y[1]                                  A at x[0]
// alg  y[2]                                  z at x[0]
// alg  y[3]                                  A*Cv[1] at x[0]
//  .    .                                     .
// alg  y[3+nv-1]                             A*Cv[nv] at x[0]
// diff y[3+nv]                               Cs[1] at x[0]
//  .    .                                     .
// diff y[3+nv+ns-1]                          Cs[ns] at x[0]
// alg  y[3+nv+ns]                            Ce[1] at x[0]
//  .    .                                     .
// alg  y[3+nv+ns+ne-1]                       Ce[ne] at x[0]
// in case of sediment layers:
// diff y[cellhyd]                            Aeff at x[0] sed1
// diff y[cellhyd+1]                          Aeff*Cv[1] at x[0] sed1
//  .                                          .
// diff y[cellhyd+1+nv-1]                     Aeff*Cv[nv] at x[0] sed1
// diff y[cellhyd+1+nv]                       Cs[1] at x[0]	sed1
//  .                                          .
// diff y[cellhyd+1+nv+ns-1]                  Cs[nv] at x[0] sed1
// diff y[cellhyd+1+nv+ns]                    Ce[1] at x[0]	sed1
//  .                                          .
// diff y[cellhyd+1+nv+ns+ne-1]               Ce[nv] at x[0] sed1
// diff y[cellhyd+cellsed]                    Aeff*Cv[1] at x[0] sed2
//	.										   .
// diff y[cellhyd+(nl-1)*cellsed+1+nv+ns+ne-1] Ce[nv] at x[0] sednl
// ---------------------------------------------------------------------------
// alg  y[cell]                               Q at x[1]
// diff y[cell+1]                             A at x[1]
// alg  y[cell+2]                             z at x[1]
// diff y[cell+3]                             A*Cv[1] at x[1]
//  .    .                                     .
// diff y[cell+3+nv-1]                        A*Cv[nv] at x[1]
// diff y[cell+3+nv]                          Cs[1] at x[1]
//  .    .                                     .
// diff y[cell+3+nv+ns-1]                     Cs[ns] at x[1]
// alg  y[cell+3+nv+ns]                       Ce[1] at x[1]
//  .    .                                     .
// alg  y[cell+3+nv+ns+ne-1]                  Ce[ne] at x[1]
// in case of sediment layers:
// diff y[cell+cellhyd]                       Aeff at x[1] sed1
// diff y[cell+cellhyd+1]                     Aeff*Cv[1] at x[1] sed1
//  .                                          .
// diff y[cell+cellhyd+1+nv-1]                Aeff*Cv[nv] at x[1] sed1
// diff y[cell+cellhyd+1+nv]                  Cs[1] at x[1]	sed1
//  .                                          .
// diff y[cell+cellhyd+1+nv+ns-1]             Cs[nv] at x[1] sed1
// diff y[cell+cellhyd+1+nv+ns]               Ce[1] at x[1]	sed1
//  .                                          .
// diff y[cell+cellhyd+1+nv+ns+ne-1]          Ce[nv] at x[1] sed1
// diff y[cell+cellhyd+cellsed]               Aeff*Cv[1] at x[1] sed2
//	.										   .
// diff y[cell+cellhyd+(nl-1)*cellsed+1+nv+ns+ne-1] Ce[nv] at x[1] sednl
// ---------------------------------------------------------------------------
//  .    .                                     .
//  .    .                                     .
//
//
// kinematic:
// ---------------------------------------------------------------------------
// alg  y[(n-1)*cell]                         Q at x[n-1]
// diff y[(n-1)*cell+1]                       A at x[n-1]
// alg  y[(n-1)*cell+2]                       z at x[n-1]
// diff y[(n-1)*cell+3]                       A*Cv[1] at x[n-1]
//  .    .                                     .
// diff y[(n-1)*cell+3+nv-1]                  A*Cv[nv] at x[n-1]
// diff y[(n-1)*cell+3+nv]                    Cs[1] at x[n-1]
//  .    .                                     .
// diff y[(n-1)*cell+3+nv+ns-1]               Cs[ns] at x[n-1]
// alg  y[(n-1)*cell+3+nv+ns]                 Ce[1] at x[n-1]
//  .    .                                     .
// alg  y[(n-1)*cell+3+nv+ns+ne-1]            Ce[ne] at x[n-1]
// in case of sediment layers:
// diff y[(n-1)*cell+cellhyd]                 Aeff at x[n-1] sed1
// diff y[(n-1)*cell+cellhyd+1]               Aeff*Cv[1] at x[n-1] sed1
//  .                                          .
// diff y[(n-1)*cell+cellhyd+1+nv-1]          Aeff*Cv[nv] at x[n-1] sed1
// diff y[(n-1)*cell+cellhyd+1+nv]            Cs[1] at x[n-1]	sed1
//  .                                          .
// diff y[(n-1)*cell+cellhyd+1+nv+ns-1]       Cs[nv] at x[n-1] sed1
// diff y[(n-1)*cell+cellhyd+1+nv+ns]         Ce[1] at x[n-1]	sed1
//  .                                         
// diff y[(n-1)*cell+cellhyd+1+nv+ns+ne-1]    Ce[nv] at x[n-1] sed1
// diff y[(n-1)*cell+cellhyd+cellsed]         Aeff*Cv[1] at x[n-1] sed2
//	.										   .
// diff y[(n-1)*cell+cellhyd+(nl-1)*cellsed+1+nv+ns+ne-1] Ce[nv] at x[n-1] sednl
// ---------------------------------------------------------------------------
// alg  y[n*cell]                             dispersion at x[n-1]
//
// diffusive:
// ---------------------------------------------------------------------------
// alg  y[(n-1)*cell]                         Q at x[n-1]
// alg  y[(n-1)*cell+1]                       A at x[n-1]
// alg  y[(n-1)*cell+2]                       z at x[n-1]
// diff y[(n-1)*cell+3]                       A*Cv[1] at x[n-1]
//  .    .                                     .
// diff y[(n-1)*cell+3+nv-1]                  A*Cv[nv] at x[n-1]
// diff y[(n-1)*cell+3+nv]                    Cs[1] at x[n-1]
//  .    .                                     .
// diff y[(n-1)*cell+3+nv+ns-1]               Cs[ns] at x[n-1]
// alg  y[(n-1)*cell+3+nv+ns]                 Ce[1] at x[n-1]
//  .    .                                     .
// alg  y[(n-1)*cell+3+nv+ns+ne-1]            Ce[ne] at x[n-1]
// in case of sediment layers:
// diff y[(n-1)*cell+cellhyd]                 Aeff at x[n-1] sed1
// diff y[(n-1)*cell+cellhyd+1]               Aeff*Cv[1] at x[n-1] sed1
//  .                                          .
// diff y[(n-1)*cell+cellhyd+1+nv-1]          Aeff*Cv[nv] at x[n-1] sed1
// diff y[(n-1)*cell+cellhyd+1+nv]            Cs[1] at x[n-1]	sed1
//  .                                          .
// diff y[(n-1)*cell+cellhyd+1+nv+ns-1]       Cs[nv] at x[n-1] sed1
// diff y[(n-1)*cell+cellhyd+1+nv+ns]         Ce[1] at x[n-1]	sed1
//  .                                         
// diff y[(n-1)*cell+cellhyd+1+nv+ns+ne-1]    Ce[nv] at x[n-1] sed1
// diff y[(n-1)*cell+cellhyd+cellsed]         Aeff*Cv[1] at x[n-1] sed2
//	.										   .
// diff y[(n-1)*cell+cellhyd+(nl-1)*cellsed+1+nv+ns+ne-1] Ce[nv] at x[n-1] sednl
// ---------------------------------------------------------------------------
// alg  y[n*cell]                             dispersion at x[n-1]
// alg  y[n*cell+1]                           znorm at x[n-1]
// alg  y[n*cell+2]                           zcrit at x[n-1]
//
//////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "fortran.h"
#include "formvar.h"
#include "othervar.h"
#include "varsys.h"
#include "rivcomp.h"
#include "aqlink.h"

//////////////////////////////////////////////////////////////////////////////

const CARDINAL BufSize = 1024;

static char namebuffer[BufSize];

//////////////////////////////////////////////////////////////////////////////

const char* RivSedLayerIdentifier = "SEDLAY";

void RIVSEDLAYER::init()
{
   char Buffer1[BufSize]; char Buffer2[BufSize];
   zind = 1;
   NumToString(1,Buffer1,BufSize);
   area = new FORMVAR;
   area->Symbol("sedarea");
   if ( area->Parse(Buffer1,0,Buffer2) != 0 )
   {
      FatalError("RIVSEDLAYER::init: Unable to parse area");
   }
   NumToString(0.8,Buffer1,BufSize);
   porosity = new FORMVAR;
   porosity->Symbol("sedporosity");
   if ( porosity->Parse(Buffer1,0,Buffer2) != 0 )
   {
      FatalError("RIVSEDLAYER::init: Unable to parse porosity");
   }
   NumToString(0,Buffer1,BufSize);
   discharge = new FORMVAR;
   discharge->Symbol("seddischarge");
   if ( discharge->Parse(Buffer1,0,Buffer2) != 0 )
   {
      FatalError("RIVSEDLAYER::init: Unable to parse discharge");
   }
   NumToString(0,Buffer1,BufSize);
   dispersion = new FORMVAR;
   dispersion->Symbol("seddispersion");
   if ( dispersion->Parse(Buffer1,0,Buffer2) != 0 )
   {
      FatalError("RIVSEDLAYER::init: Unable to parse dispersion");
   }
   NumToString(0,Buffer1,BufSize);
   exchcoeff = new FORMVAR;
   exchcoeff->Symbol("sedexchcoeff");
   if ( exchcoeff->Parse(Buffer1,0,Buffer2) != 0 )
   {
      FatalError("RIVSEDLAYER::init: Unable to parse exchange coefficient");
   }
   numvararg = 0;
   vararg = 0;
   return;
}


RIVSEDLAYER::RIVSEDLAYER()
{
   init();
}


RIVSEDLAYER::RIVSEDLAYER(const RIVSEDLAYER& layer)
{
   init();
   zind = layer.zind;
   if ( layer.area != 0 ) 
   {
      delete area;
      area = new FORMVAR(layer.area);
   }
   if ( layer.porosity != 0 )
   {
      delete porosity;
      porosity = new FORMVAR(layer.porosity);
   }
   if ( layer.discharge != 0 )
   {
      delete discharge;
      discharge = new FORMVAR(layer.discharge);
   }
   if ( layer.dispersion != 0 )
   {
      delete dispersion;
      dispersion = new FORMVAR(layer.dispersion);
   }
   if ( layer.exchcoeff != 0 )
   {
      delete exchcoeff;
      exchcoeff = new FORMVAR(layer.exchcoeff);
   }
   CalcVarArg();
}


RIVSEDLAYER::RIVSEDLAYER(const RIVSEDLAYER* layer)
{
   init();
   if ( layer != 0 )
   {
      zind = layer->zind;
      if ( layer->area != 0 )
      {
         delete area;
         area = new FORMVAR(layer->area);
      }
      if ( layer->porosity != 0 ) 
      {
         delete porosity;
         porosity = new FORMVAR(layer->porosity);
      }
      if ( layer->discharge != 0 ) 
      {
         delete discharge;
         discharge = new FORMVAR(layer->discharge);
      }
      if ( layer->dispersion != 0 ) 
      {
         delete dispersion;
         dispersion = new FORMVAR(layer->dispersion);
      }
      if ( layer->exchcoeff != 0 ) 
      {
         delete exchcoeff;
         exchcoeff = new FORMVAR(layer->exchcoeff);
      }
   }
   CalcVarArg();
}


RIVSEDLAYER::~RIVSEDLAYER()
{
   delete area;       area = 0;
   delete porosity;   porosity = 0;
   delete discharge;  discharge = 0;
   delete dispersion; dispersion = 0;
   delete exchcoeff;  exchcoeff = 0;

   delete [] vararg; vararg = 0; numvararg = 0;
}


BOOLEAN RIVSEDLAYER::AllowedProgVarType(PROGVARTYPE type) const
{
   if ( type == CalcNumber )    return TRUE;
   if ( type == Time )          return TRUE;
   if ( type == CompIndex )     return TRUE;
   if ( type == ::ZoneIndex )   return TRUE;
   if ( type == CrossSection )  return TRUE;
   if ( type == WaterFraction ) return TRUE;
   if ( type == SpaceX )        return TRUE;
   return FALSE;
}


BOOLEAN RIVSEDLAYER::AllowedVar(const AQVAR* var) const
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


AQVAR* RIVSEDLAYER::VarArg(CARDINAL index) const
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


BOOLEAN RIVSEDLAYER::Arg(const AQVAR* var) const
{
   for ( CARDINAL i=0; i<numvararg; i++ )
   {
      if ( vararg[i] == var )              return TRUE;
      if ( (vararg[i])->Arg(var) == TRUE ) return TRUE;
   }
   return FALSE;
}


BOOLEAN RIVSEDLAYER::ReplaceVar(AQVAR* oldvar, AQVAR* newvar)
{
   if ( area != 0 )       area->ReplaceVarArg(oldvar,newvar);
   if ( porosity != 0 )   porosity->ReplaceVarArg(oldvar,newvar);
   if ( discharge != 0 )  discharge->ReplaceVarArg(oldvar,newvar);
   if ( dispersion != 0 ) dispersion->ReplaceVarArg(oldvar,newvar);
   if ( exchcoeff != 0 )  exchcoeff->ReplaceVarArg(oldvar,newvar);
   CalcVarArg();
   return TRUE;
}


BOOLEAN RIVSEDLAYER::ExchangeVar(AQVAR* var1, AQVAR* var2)
{
   AQVAR* v = new CONSTVAR;
   if ( area != 0 )
   {
      area->ReplaceVarArg(var1,v);
      area->ReplaceVarArg(var2,var1);
      area->ReplaceVarArg(v,var2);
   }
   if ( porosity != 0 )
   {
      porosity->ReplaceVarArg(var1,v);
      porosity->ReplaceVarArg(var2,var1);
      porosity->ReplaceVarArg(v,var2);
   }
   if ( discharge != 0 )
   {
      discharge->ReplaceVarArg(var1,v);
      discharge->ReplaceVarArg(var2,var1);
      discharge->ReplaceVarArg(v,var2);
   }
   if ( dispersion != 0 )
   {
      dispersion->ReplaceVarArg(var1,v);
      dispersion->ReplaceVarArg(var2,var1);
      dispersion->ReplaceVarArg(v,var2);
   }
   if ( exchcoeff != 0 )
   {
      exchcoeff->ReplaceVarArg(var1,v);
      exchcoeff->ReplaceVarArg(var2,var1);
      exchcoeff->ReplaceVarArg(v,var2);
   }
   CalcVarArg();
   return TRUE;
}


BOOLEAN RIVSEDLAYER::ZoneIndex(CARDINAL i)
{
   if ( i == 0 ) return FALSE;
   zind = i;
   return TRUE;
}


BOOLEAN RIVSEDLAYER::Area(const char* inpline,
                          const VARLIST* varlist,
                          char* errorline,
                          CARDINAL& numparseerrors)
{
   if ( inpline == 0 )            return FALSE;
   if ( inpline[0] == '\0' )      return FALSE;

   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("sedarea");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar; return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar; return FALSE;
   }
   delete area; area = formvar;
   CalcVarArg();
   return TRUE;
}


BOOLEAN RIVSEDLAYER::Porosity(const char* inpline,
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


BOOLEAN RIVSEDLAYER::Discharge(const char* inpline,
                               const VARLIST* varlist,
                               char* errorline,
                               CARDINAL& numparseerrors)
{
   if ( inpline == 0 )            return FALSE;
   if ( inpline[0] == '\0' )      return FALSE;

   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("seddischarge");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar; return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar; return FALSE;
   }
   delete discharge; discharge = formvar;
   CalcVarArg();
   return TRUE;
}


BOOLEAN RIVSEDLAYER::Dispersion(const char* inpline,
                                const VARLIST* varlist,
                                char* errorline,
                                CARDINAL& numparseerrors)
{
   if ( inpline == 0 )            return FALSE;
   if ( inpline[0] == '\0' )      return FALSE;

   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("seddispersion");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar; return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar; return FALSE;
   }
   delete dispersion; dispersion = formvar;
   CalcVarArg();
   return TRUE;
}


BOOLEAN RIVSEDLAYER::ExchCoeff(const char* inpline,
                               const VARLIST* varlist,
                               char* errorline,
                               CARDINAL& numparseerrors)
{
   if ( inpline == 0 )            return FALSE;
   if ( inpline[0] == '\0' )      return FALSE;

   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("sedexchcoeff");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar; return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar; return FALSE;
   }
   delete exchcoeff; exchcoeff = formvar;
   CalcVarArg();
   return TRUE;
}


void RIVSEDLAYER::CalcVarArg()
{
   numvararg = 0;
   if ( area != 0 )       numvararg += area->NumVarArg();
   if ( porosity != 0 )   numvararg += porosity->NumVarArg();
   if ( discharge != 0 )  numvararg += discharge->NumVarArg();
   if ( dispersion != 0 ) numvararg += dispersion->NumVarArg();
   if ( exchcoeff != 0 )  numvararg += exchcoeff->NumVarArg();

   if ( numvararg > 0 )
   {
      delete [] vararg;
      vararg = new AQVAR*[numvararg];
      CARDINAL k = 0;
      CARDINAL i;
      if ( area != 0 )
      {
         for ( i=0; i<area->NumVarArg(); i++ )
         {
            vararg[k] = area->VarArg(i); k++;
         }
      }
      if ( porosity != 0 )
      {
         for ( i=0; i<porosity->NumVarArg(); i++ )
         {
            vararg[k] = porosity->VarArg(i); k++;
         }
      }
      if ( discharge != 0 )
      {
         for ( i=0; i<discharge->NumVarArg(); i++ )
         {
            vararg[k] = discharge->VarArg(i); k++;
         }
      }
      if ( dispersion != 0 )
      {
         for ( i=0; i<dispersion->NumVarArg(); i++ )
         {
            vararg[k] = dispersion->VarArg(i); k++;
         }
      }
      if ( exchcoeff != 0 )
      {
         for ( i=0; i<exchcoeff->NumVarArg(); i++ )
         {
            vararg[k] = exchcoeff->VarArg(i); k++;
         }
      }
      if ( k != numvararg )
         FatalError("RIVSEDLAYER::CalcVarArg: inconsistent number of arguments");
   }
   return;
}


JOBSTATUS RIVSEDLAYER::Load(std::istream& in, const VARLIST* varlist)
{
   char Buffer1[BufSize]; char Buffer2[BufSize];

   if ( LoadStart(in) == FALSE ) return PROBLEM;

   CARDINAL n;
   if (LoadNumber (in, &n)   == FALSE )                         return PROBLEM;
   if (ZoneIndex (n) == FALSE )                                 return PROBLEM;

   if ( LoadString(in, Buffer1, BufSize) == FALSE )             return PROBLEM;
   if ( Area(Buffer1, varlist, Buffer2, n) == FALSE )           return PROBLEM;

   if ( LoadString(in, Buffer1, BufSize) == FALSE )             return PROBLEM;
   if ( Porosity(Buffer1, varlist, Buffer2, n) == FALSE )       return PROBLEM;

   if ( LoadString(in, Buffer1, BufSize) == FALSE )             return PROBLEM;
   if ( Discharge(Buffer1, varlist, Buffer2, n) == FALSE )      return PROBLEM;

   if ( LoadString(in, Buffer1, BufSize) == FALSE )             return PROBLEM;
   if ( Dispersion(Buffer1, varlist, Buffer2, n) == FALSE )     return PROBLEM;

   if ( LoadString(in, Buffer1, BufSize) == FALSE )             return PROBLEM;
   if ( ExchCoeff(Buffer1, varlist, Buffer2, n) == FALSE )      return PROBLEM;

   while (LoadStart(in) == TRUE )
   {
       if ( LoadEnd(in) == FALSE ) return PROBLEM;
   }
   return OK;
}


JOBSTATUS RIVSEDLAYER::Save(std::ostream& out)
{
   if ( SaveStart(out) == FALSE ) return PROBLEM;

   char Buffer[BufSize];
   const INIT* ini =  AQCOMP::Ini();                      

   if ( SaveNumber(out, zind)  == FALSE)                        return PROBLEM;

   if ( area->UnParse(Buffer,sizeof(Buffer)-1) !=0 )            return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                       return PROBLEM;

   if ( porosity->UnParse(Buffer,sizeof(Buffer)-1) !=0 )        return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                       return PROBLEM;

   if ( discharge->UnParse(Buffer,sizeof(Buffer)-1) !=0 )       return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                       return PROBLEM;

   if ( dispersion->UnParse(Buffer,sizeof(Buffer)-1) !=0 )      return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                       return PROBLEM;

   if ( exchcoeff->UnParse(Buffer,sizeof(Buffer)-1) !=0 )       return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                       return PROBLEM;

   if ( SaveEnd(out) == FALSE )                                 return PROBLEM;
   return OK;
}


JOBSTATUS RIVSEDLAYER::Write(std::ostream& out, BOOLEAN)
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

   if ( Print(out,ini->T(259),pos1+2) == FALSE )              return PROBLEM;
   if ( Print(out,":") == FALSE )                             return PROBLEM;
   if ( Print(out,ZoneIndex(),pos2) == FALSE )                return PROBLEM;

   if ( Print(out,ini->T(260),pos1+4) == FALSE )              return PROBLEM;
   if ( Print(out,":") == FALSE )                             return PROBLEM;
   if ( area->UnParse(Buffer,sizeof(Buffer)-1) != 0 )         return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                     return PROBLEM;

   if ( Print(out,ini->T(261),pos1+4) == FALSE )              return PROBLEM;
   if ( Print(out,":") == FALSE )                             return PROBLEM;
   if ( porosity->UnParse(Buffer,sizeof(Buffer)-1) != 0 )     return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                     return PROBLEM;

   if ( Print(out,ini->T(262),pos1+4) == FALSE )              return PROBLEM;
   if ( Print(out,":") == FALSE )                             return PROBLEM;
   if ( discharge->UnParse(Buffer,sizeof(Buffer)-1) != 0 )    return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                     return PROBLEM;

   if ( Print(out,ini->T(266),pos1+4) == FALSE )              return PROBLEM;
   if ( Print(out,":") == FALSE )                             return PROBLEM;
   if ( dispersion->UnParse(Buffer,sizeof(Buffer)-1) != 0 )   return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                     return PROBLEM;

   if ( Print(out,ini->T(263),pos1+4) == FALSE )              return PROBLEM;
   if ( Print(out,":") == FALSE )                             return PROBLEM;
   if ( exchcoeff->UnParse(Buffer,sizeof(Buffer)-1) != 0 )    return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                     return PROBLEM;

   return OK;
}


//////////////////////////////////////////////////////////////////////////////

// Identifiers for RIVCOMP::Save and RIVCOMP::Load
// (compare this constant with enum RIVCOMPLEVEL from aqcomp.h)

                                                        //  enum RIVCOMPLEVEL:
const char* const RivCompLevelIdentifier[] = {
                                                "NORMAL",   //  NormalLevel
                                                "CRITICAL", //  CriticalLevel
                                                "GIVEN"     //  GivenLevel
                                             };

const CARDINAL NumRivLevel = sizeof(RivCompLevelIdentifier)/sizeof(char*);

// Identifiers for RIVCOMP::Save and RIVCOMP::Load
// (compare this constant with enum RIVCOMPMETH from aqcomp.h)

                                                        //  enum RIVCOMPMETH:
const char* const RivCompMethIdentifier[]   = {
                                                 "KIN",     //  KinWave
                                                 "DIFF"     //  DiffWave
                                              };

const CARDINAL NumRivMeth = sizeof(RivCompMethIdentifier)/sizeof(char*);

//////////////////////////////////////////////////////////////////////////////

void RIVCOMP::init()
{
   numinit = 0; initzone = 0; initvar = 0; initval = 0;

   inflow = 0;
   numinput = 0; inputvar = 0; inputflux = 0;
   latinflow = 0;
   numlatinput = 0; latinputvar = 0; latinputconc = 0;
   latinflow = 0;
   numlatinput = 0; latinputvar = 0; latinputconc = 0;

   Xs = ini.R(3);
   Xe = ini.R(4);
   gridpts = 0; SetGridPts(12); SetGridPts(ini.I(1));
   relaccQ = 0.001;
   absaccQ = 0.001;
   relaccA = 0.001;
   absaccA = 0.001;
   relaccZ = 0.0;
   absaccZ = 1.0E-6;
   relaccE = 1.0E-6;
   absaccE = 1.0E-6;

   A    = 0;
   P    = 0;
   W    = 0;
   Sf   = 0;
   E    = 0;

   calcSed = FALSE;
   numsedlayers = 0;
   sedlayers = 0;

   Zend = 0;
   withdispersion = FALSE;
   endleveltype = NormalLevel;
   method   = KinWave;
   highres  = FALSE;
   gravit = 9.81;
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


void RIVCOMP::del()
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
   delete P;  P  = 0;
   delete W;  W  = 0;
   delete Sf; Sf = 0;
   delete E;  E  = 0;

   for ( i=0; i<numsedlayers; i++ )
   {
      delete sedlayers[i];
   }
   delete [] sedlayers; sedlayers = 0; numsedlayers = 0;

   delete Zend; Zend = 0;
   delete [] Cout; Cout = 0;
   return;
}


RIVCOMP::RIVCOMP()
{
   init();
}


RIVCOMP::RIVCOMP(const RIVCOMP& com) : AQCOMP(com)
{
   init();

   CARDINAL i;
   if ( com.numinit > 0 )
   {
      numinit = com.numinit;
      initzone = new CARDINAL[numinit];
      initvar  = new AQVAR*[numinit];
      initval  = new FORMVAR*[numinit];
      for ( i=0; i<numinit; i++ )
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
      for ( i=0; i<numinput; i++ )
      {
         inputvar[i] = com.inputvar[i];
         inputflux[i] = new FORMVAR(com.inputflux[i]);
      }
   }

   relaccQ = com.relaccQ;
   absaccQ = com.absaccQ;
   relaccA = com.relaccA;
   absaccA = com.absaccA;
   relaccZ = com.relaccZ;
   absaccZ = com.absaccZ;
   relaccE = com.relaccE;
   absaccE = com.absaccE;

   Xs = com.Xs;
   Xe = com.Xe;
   if ( gridpts != 0 ) delete [] gridpts;
   numgrid = com.numgrid;
   gridpts = new REAL[numgrid];
   for ( i=0; i<numgrid; i++ ) gridpts[i] = com.gridpts[i];
   latinflow = new FORMVAR(com.latinflow);
   if ( com.numlatinput > 0 )
   {
      numlatinput = com.numlatinput;
      latinputvar  = new AQVAR*[numlatinput];
      latinputconc = new FORMVAR*[numlatinput];
      for ( i=0; i<numlatinput; i++ )
      {
         latinputvar[i] = com.latinputvar[i];
         latinputconc[i] = new FORMVAR(com.latinputconc[i]);
      }
   }
   gravit = com.gravit;
   A  = new FORMVAR(com.A);
   P  = new FORMVAR(com.P);
   W  = new FORMVAR(com.W);
   Sf = new FORMVAR(com.Sf);
   if ( com.E    != 0 ) E    = new FORMVAR(com.E);

   calcSed = com.calcSed;
   numsedlayers = com.numsedlayers;
   if ( numsedlayers > 0 )
   {
      sedlayers = new RIVSEDLAYER*[numsedlayers];
      for ( i=0; i<numsedlayers; i++ )
      {
         sedlayers[i] = new RIVSEDLAYER(com.sedlayers[i]);
      }
   }

   if ( com.Zend != 0 ) Zend = new FORMVAR(com.Zend);
   withdispersion = com.withdispersion;
   endleveltype = com.endleveltype;
   method = com.method;
   highres = com.highres;
}


RIVCOMP::RIVCOMP(const AQCOMP* com) : AQCOMP(com)
{
   init();
   if ( com != 0 )
   {
      if ( com->Type() == RivComp )
      {
         RIVCOMP* rc = (RIVCOMP*)com;

         CARDINAL i;
         if ( rc->numinit > 0 )
         {
            numinit = rc->numinit;
            initzone = new CARDINAL[numinit];
            initvar  = new AQVAR*[numinit];
            initval  = new FORMVAR*[numinit];
            for ( i=0; i<numinit; i++ )
            {
               initzone[i] = rc->initzone[i];
               initvar[i]  = rc->initvar[i];
               initval[i]  = new FORMVAR(rc->initval[i]);
            }
         }

         inflow = new FORMVAR(rc->inflow);
         if ( rc->numinput > 0 )
         {
            numinput = rc->numinput;
            inputvar = new AQVAR*[numinput];
            inputflux = new FORMVAR*[numinput];
            for ( i=0; i<numinput; i++ )
            {
               inputvar[i] = rc->inputvar[i];
               inputflux[i] = new FORMVAR(rc->inputflux[i]);
            }
         }

         relaccQ = rc->relaccQ;
         absaccQ = rc->absaccQ;
         relaccA = rc->relaccA;
         absaccA = rc->absaccA;
         relaccZ = rc->relaccZ;
         absaccZ = rc->absaccZ;
         relaccE = rc->relaccE;
         absaccE = rc->absaccE;

         Xs = rc->Xs;
         Xe = rc->Xe;
         numgrid = rc->numgrid;
         if ( gridpts != 0 ) delete [] gridpts;
         gridpts = new REAL[numgrid];
         for ( i=0; i<numgrid; i++ ) gridpts[i] = rc->gridpts[i];
         latinflow = new FORMVAR(rc->latinflow);
         if ( rc->numlatinput > 0 )
         {
            numlatinput = rc->numlatinput;
            latinputvar  = new AQVAR*[numlatinput];
            latinputconc = new FORMVAR*[numlatinput];
            for ( i=0; i<numlatinput; i++ )
            {
               latinputvar[i] = rc->latinputvar[i];
               latinputconc[i] = new FORMVAR(rc->latinputconc[i]);
            }
         }
         gravit = rc->gravit;
         A  = new FORMVAR(rc->A);
         P  = new FORMVAR(rc->P);
         W  = new FORMVAR(rc->W);
         Sf = new FORMVAR(rc->Sf);
         if ( rc->E    != 0 ) E    = new FORMVAR(rc->E);

         calcSed = rc->calcSed;
         numsedlayers = rc->numsedlayers;
         if ( numsedlayers > 0 )
         {
            sedlayers = new RIVSEDLAYER*[numsedlayers];
            for ( i=0; i<numsedlayers; i++ )
            {
               sedlayers[i] = new RIVSEDLAYER(rc->sedlayers[i]);
            }
         }

         if ( rc->Zend != 0 ) Zend = new FORMVAR(rc->Zend);
         withdispersion = rc->withdispersion;
         endleveltype = rc->endleveltype;
         method = rc->method;
         highres = rc->highres;
      }
   }
}


RIVCOMP::~RIVCOMP()
{
   del();
}


const char* RIVCOMP::TypeName() const
{
   return ini.T(26);
}


AQVAR* RIVCOMP::SpaceVar(const VARSYS* varsys) const
{
   return varsys->GetProgVar(SpaceX);
}


BOOLEAN RIVCOMP::AllowedSpaceVal(REAL x) const
{
   if ( (x<gridpts[0]) && (x<gridpts[numgrid-1]) ) return FALSE;
   if ( (x>gridpts[0]) && (x>gridpts[numgrid-1]) ) return FALSE;
   return TRUE;
}


BOOLEAN RIVCOMP::AllowedProgVarType(PROGVARTYPE type) const
{
   if ( type == CalcNumber )    return TRUE;
   if ( type == Time )          return TRUE;
   if ( type == CompIndex )     return TRUE;
   if ( type == ZoneIndex )     return TRUE;
   if ( type == WaterFraction ) return TRUE;
   if ( type == SpaceX )        return TRUE;
   if ( type == WaterLevel )    return TRUE;
   if ( type == Discharge )     return TRUE;
   if ( type == CrossSection )  return TRUE;
   if ( type == Perimeter )     return TRUE;
   if ( type == Width )         return TRUE;
   if ( type == FrictSlope )    return TRUE;
   return FALSE;
}


BOOLEAN RIVCOMP::AllowedComp() const
{
   if ( (A==0) || (P==0) || (W==0) || (Sf==0) )     return FALSE;

   if ( A->ArgVarType(StateVar) == TRUE )           return FALSE;
   if ( P->ArgVarType(StateVar) == TRUE )           return FALSE;
   if ( W->ArgVarType(StateVar) == TRUE )           return FALSE;
   if ( Sf->ArgVarType(StateVar) == TRUE )          return FALSE;
   if ( E != 0 )
   {
      if ( E->ArgVarType(StateVar) == TRUE )        return FALSE;
   }

   if ( A->ArgProgVarType(WaterLevel) == FALSE )    return FALSE;
   if ( Sf->ArgProgVarType(Discharge) == FALSE )    return FALSE;
   if ( Sf->ArgProgVarType(CrossSection) == FALSE ) return FALSE;

   return TRUE;
}


BOOLEAN RIVCOMP::AllowedReplaceVar(const AQVAR* oldvar,
                                   const AQVAR* newvar) const
{
   if ( AQCOMP::AllowedReplaceVar(oldvar,newvar) == FALSE )   return FALSE;
   if ( A->Arg(oldvar) == TRUE )
   {
      if ( newvar->ArgVarType(StateVar) == TRUE )             return FALSE;
      if ( A->ArgProgVarType(WaterLevel,oldvar) == FALSE )
      {
         if ( newvar->ArgProgVarType(WaterLevel) == FALSE )   return FALSE;
      }
   }
   if ( P->Arg(oldvar) == TRUE )
   {
      if ( newvar->ArgVarType(StateVar) == TRUE )             return FALSE;
   }
   if ( W->Arg(oldvar) == TRUE )
   {
      if ( newvar->ArgVarType(StateVar) == TRUE )             return FALSE;
   }
   if ( Sf->Arg(oldvar) == TRUE )
   {
      if ( newvar->ArgVarType(StateVar) == TRUE )             return FALSE;
      if ( Sf->ArgProgVarType(Discharge,oldvar) == FALSE )
      {
         if ( newvar->ArgProgVarType(Discharge) == FALSE )    return FALSE;
      }
      if ( Sf->ArgProgVarType(CrossSection,oldvar) == FALSE )
      {
         if ( newvar->ArgProgVarType(CrossSection) == FALSE ) return FALSE;
      }
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


BOOLEAN RIVCOMP::AllowedExchangeVar(const AQVAR* var1,
                                    const AQVAR* var2) const
{
   if ( AQCOMP::AllowedExchangeVar(var1,var2) == FALSE )    return FALSE;
   if ( (A->Arg(var1)==TRUE) && (A->Arg(var2)==FALSE) )
   {
      if ( var2->ArgVarType(StateVar) == TRUE )             return FALSE;
      if ( A->ArgProgVarType(WaterLevel,var1) == FALSE )
      {
         if ( var1->ArgProgVarType(WaterLevel) == FALSE )   return FALSE;
      }
   }
   if ( (A->Arg(var1)==FALSE) && (A->Arg(var2)==TRUE) )
   {
      if ( var1->ArgVarType(StateVar) == TRUE )             return FALSE;
      if ( A->ArgProgVarType(WaterLevel,var2) == FALSE )
      {
         if ( var2->ArgProgVarType(WaterLevel) == FALSE )   return FALSE;
      }
   }
   if ( (P->Arg(var1)==TRUE) && (P->Arg(var2)==FALSE) )
   {
      if ( var2->ArgVarType(StateVar) == TRUE )             return FALSE;
   }
   if ( (P->Arg(var1)==FALSE) && (P->Arg(var2)==TRUE) )
   {
      if ( var1->ArgVarType(StateVar) == TRUE )             return FALSE;
   }
   if ( (W->Arg(var1)==TRUE) && (W->Arg(var2)==FALSE) )
   {
      if ( var2->ArgVarType(StateVar) == TRUE )             return FALSE;
   }
   if ( (W->Arg(var1)==FALSE) && (W->Arg(var2)==TRUE) )
   {
      if ( var1->ArgVarType(StateVar) == TRUE )             return FALSE;
   }
   if ( (Sf->Arg(var1)==TRUE) && (Sf->Arg(var2)==FALSE) )
   {
      if ( var2->ArgVarType(StateVar) == TRUE )             return FALSE;
      if ( Sf->ArgProgVarType(Discharge,var1) == FALSE )
      {
         if ( var1->ArgProgVarType(Discharge) == FALSE )    return FALSE;
      }
      if ( Sf->ArgProgVarType(CrossSection,var1) == FALSE )
      {
         if ( var1->ArgProgVarType(CrossSection) == FALSE ) return FALSE;
      }
   }
   if ( (Sf->Arg(var1)==FALSE) && (Sf->Arg(var2)==TRUE) )
   {
      if ( var1->ArgVarType(StateVar) == TRUE )             return FALSE;
      if ( Sf->ArgProgVarType(Discharge,var2) == FALSE )
      {
         if ( var2->ArgProgVarType(Discharge) == FALSE )    return FALSE;
      }
      if ( Sf->ArgProgVarType(CrossSection,var2) == FALSE )
      {
         if ( var2->ArgProgVarType(CrossSection) == FALSE ) return FALSE;
      }
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


void RIVCOMP::ReplVar(AQVAR* oldvar, AQVAR* newvar)
{
   CARDINAL i;

   if ( AllowedReplaceVar(oldvar,newvar) == FALSE )
      FatalError("RIVCOMP::ReplVar: Illegal replace");
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
   P->ReplaceVarArg(oldvar,newvar);
   W->ReplaceVarArg(oldvar,newvar);
   Sf->ReplaceVarArg(oldvar,newvar);
   if ( E    != 0 ) E->ReplaceVarArg(oldvar,newvar);
   for ( i=0; i<numsedlayers; i++ )
   {
      sedlayers[i]->ReplaceVar(oldvar,newvar);
   }
   if ( Zend != 0 ) Zend->ReplaceVarArg(oldvar,newvar);
   CalcArg();
   return;
}


void RIVCOMP::ExchVar(AQVAR* var1, AQVAR* var2)
{
   CARDINAL i;

   if ( AllowedExchangeVar(var1,var2) == FALSE )
      FatalError("RIVCOMP::ExchVar: Illegal exchange");
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
   P->ReplaceVarArg(var1,v);
   P->ReplaceVarArg(var2,var1);
   P->ReplaceVarArg(v,var2);
   W->ReplaceVarArg(var1,v);
   W->ReplaceVarArg(var2,var1);
   W->ReplaceVarArg(v,var2);
   Sf->ReplaceVarArg(var1,v);
   Sf->ReplaceVarArg(var2,var1);
   Sf->ReplaceVarArg(v,var2);
   if ( E != 0 )
   {
      E->ReplaceVarArg(var1,v);
      E->ReplaceVarArg(var2,var1);
      E->ReplaceVarArg(v,var2);
   }
   for ( i=0; i<numsedlayers; i++ )
   {
      sedlayers[i]->ExchangeVar(var1,var2);
   }
   if ( Zend != 0 )
   {
      Zend->ReplaceVarArg(var1,v);
      Zend->ReplaceVarArg(var2,var1);
      Zend->ReplaceVarArg(v,var2);
   }
   delete v;
   CalcArg();
   return;
}


BOOLEAN RIVCOMP::LatInflow(const VARLIST* varlist,
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


AQVAR* RIVCOMP::LatInputVar(CARDINAL index) const
{
   if ( index >= numlatinput )
      FatalError("RIVCOMP::LatInputVar: Illegal index");
   return latinputvar[index];
}


const FORMVAR* RIVCOMP::LatInputConc(CARDINAL index) const
{
   if ( index >= numlatinput )
      FatalError("RIVCOMP::LatInputConc: Illegal index");
   return latinputconc[index];
}


BOOLEAN RIVCOMP::AddLatInput(const VARLIST* varlist, AQVAR* var,
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


BOOLEAN RIVCOMP::ReplaceLatInput(const VARLIST* varlist, AQVAR* var,
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


BOOLEAN RIVCOMP::DeleteLatInput(CARDINAL pos)
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


CARDINAL RIVCOMP::NumEq() const
{
   CARDINAL cellhyd = 3+NumStateVar();
   CARDINAL cellsed = 1+NumStateVar();
   CARDINAL cell = cellhyd;
   if ( calcSed == TRUE ) cell += numsedlayers*cellsed;

   switch ( method )
   {
      case KinWave:
         return NumGridPts()*cell+1;
      case DiffWave:
         return NumGridPts()*cell+3;
   }
   return 0;
}


REAL RIVCOMP::RelAcc(CARDINAL i) const
{
   if ( i >= NumEq() ) FatalError("RIVCOMP::RelAcc: Illegal index");

   CARDINAL cellhyd = 3+NumStateVar();
   CARDINAL cellsed = 1+NumStateVar();
   CARDINAL cell = cellhyd;
   if ( calcSed == TRUE ) cell += numsedlayers*cellsed;

   CARDINAL index = i % cell;
   CARDINAL ind;
   switch ( method )
   {
      case KinWave:
         if ( i == NumEq()-1 ) return relaccE;
         if ( index == 0 ) return relaccQ;
         if ( index == 1 ) return relaccA;
         if ( index == 2 ) return relaccZ;
         if ( index < cellhyd ) return statevar[index-3]->RelAccuracy();
         ind = (index-cellhyd) % cellsed;
         if ( ind == 0 ) return relaccA;
         else            return statevar[ind-1]->RelAccuracy();
      case DiffWave:
         if ( i == NumEq()-3 ) return relaccE;
         if ( i == NumEq()-2 ) return relaccZ;
         if ( i == NumEq()-1 ) return relaccZ;
         if ( index == 0 ) return relaccQ;
         if ( index == 1 ) return relaccA;
         if ( index == 2 ) return relaccZ;
         if ( index < cellhyd ) return statevar[index-3]->RelAccuracy();
         ind = (index-cellhyd) % cellsed;
         if ( ind == 0 ) return relaccA;
         else            return statevar[ind-1]->RelAccuracy();
   }
   return 0.0;
}


REAL RIVCOMP::AbsAcc(CARDINAL i) const
{
   if ( i >= NumEq() ) FatalError("RIVCOMP::AbsAcc: Illegal index");

   CARDINAL cellhyd = 3+NumStateVar();
   CARDINAL cellsed = 1+NumStateVar();
   CARDINAL cell = cellhyd;
   if ( calcSed == TRUE ) cell += numsedlayers*cellsed;

   CARDINAL index = i % cell;
   CARDINAL ind;
   switch ( method )
   {
      case KinWave:
         if ( i == NumEq()-1 ) return absaccE;
         if ( index == 0 ) return absaccQ;
         if ( index == 1 ) return absaccA;
         if ( index == 2 ) return absaccZ;
         if ( index < cellhyd )
         {
            if ( index-3 < numvolstatevar )
            {
               return statevar[index-3]->AbsAccuracy()*1e5*absaccA;
            }
            else
            {
               return statevar[index-3]->AbsAccuracy();
            }
         }
         else
         {
			ind = (index-cellhyd) % cellsed;
            if ( ind == 0 )
            {
               return absaccA;
            }
            else
            {
               if ( index-1 < numvolstatevar )
               {
                  return statevar[ind-1]->AbsAccuracy()*1e5*absaccA;
               }
               else
               {
                  return statevar[ind-1]->AbsAccuracy();
               }
            }
         }
      case DiffWave:
         if ( i == NumEq()-3 ) return absaccE;
         if ( i == NumEq()-2 ) return absaccZ;
         if ( i == NumEq()-1 ) return absaccZ;
         if ( index == 0 ) return absaccQ;
         if ( index == 1 ) return absaccA;
         if ( index == 2 ) return absaccZ;
         if ( index < cellhyd )
         {
            if ( index-3 < numvolstatevar )
            {
               return statevar[index-3]->AbsAccuracy()*1e5*absaccA;
            }
            else
            {
               return statevar[index-3]->AbsAccuracy();
            }
         }
         else
         {
			ind = (index-cellhyd) % cellsed;
            if ( ind == 0 )
            {
               return absaccA;
            }
            else
            {
               if ( index-1 < numvolstatevar )
               {
                  return statevar[ind-1]->AbsAccuracy()*1e5*absaccA;
               }
               else
               {
                  return statevar[ind-1]->AbsAccuracy();
               }
            }
         }
   }
   return 0.0;
}


EQTYPE RIVCOMP::EqType(CARDINAL i) const
{
   if ( i >= NumEq() ) FatalError("RIVCOMP::EqType: Illegal index");

   CARDINAL cellhyd = 3+NumStateVar();
   CARDINAL cellsed = 1+NumStateVar();
   CARDINAL cell = cellhyd;
   if ( calcSed == TRUE ) cell += numsedlayers*cellsed;

   CARDINAL index = i % cell;

   if ( index < cellhyd )
   {
      if ( index >= 3+numdynstatevar ) return ALG;  // EquStatevar
      if ( index >= 3+numvolstatevar ) return DIFF; // SurfStatevar
   }
   else
   {
      return DIFF;                                  // Sediment layers
   }

   switch ( method )
   {
      case KinWave:
         if ( i < 3+numstatevar )                // Upstream boundary cond.:
         {
            return ALG;                          // Q,A,z0,VolStatevar
         }
         if ( i == numgrid*(3+numstatevar) )     // End dispersion:
         {
            return ALG;                          // disp
         }
         if ( i >= (numgrid-1)*(3+numstatevar) ) // Downstream boundary cond.:
         {
            if ( index == 0 ) return ALG;        // Q
            if ( index == 1 ) return DIFF;       // A
            if ( index == 2 ) return ALG;        // z0
            if ( WithDispersion() == TRUE )      // VolStatevar
            {
               return ALG;
            }
            else
            {
               return DIFF;
            }
         }
         else                                    // Equations in cells:
         {
            if ( index == 0 ) return ALG;        // Q
            if ( index == 1 ) return DIFF;       // A
            if ( index == 2 ) return ALG;        // z0
            return DIFF;                         // VolStatevar
         }
      case DiffWave:
         if ( i < 3+numstatevar )                // Upstream boundary cond.:
         {
            return ALG;                          // Q,A,z0,VolStatevar
         }
         if ( i >= numgrid*(3+numstatevar) )     // End dispersion, End levels:
         {
            return ALG;                          // znormal, zcritical, disp
         }
         if ( i >= (numgrid-1)*(3+numstatevar) ) // Downstream boundary cond.:
         {
            if ( index < 3 ) return ALG;         // Q,A,z0
            if ( WithDispersion() == TRUE )      // VolStatevar
            {
               return ALG;
            }
            else
            {
               return DIFF;
            }
         }
         else                                    // Equations in cells:
         {
            if ( index == 0 ) return ALG;        // Q
            if ( index == 1 ) return DIFF;       // A
            if ( index == 2 ) return ALG;        // z0
            return DIFF;                         // VolStatevar
         }
   }
   return DIFF;
}


REAL RIVCOMP::DeltaNorm(VARSYS* varsys, REAL* y, CARDINAL j, REAL z)
{
   REAL xj, zj, Aj, wj, Pj, Qj, Sfj;
   REAL xneigh, zneigh, Aneigh, wneigh;
   REAL delta;

   if ( j >= numgrid ) 
      FatalError("RIVCOMP::DeltaNorm: Illegal index");

   CARDINAL cellhyd = 3+NumStateVar();
   CARDINAL cellsed = 1+NumStateVar();
   CARDINAL cell = cellhyd;
   if ( calcSed == TRUE ) cell += numsedlayers*cellsed;

   varsys->Reset();
   xj  = gridpts[j]; Xvar->SetProgVar(&xj);
   zj  = z; Zvar->SetProgVar(&zj);
   Aj  = A->Value(); Avar->SetProgVar(&Aj);
   wj  = W->Value(); if ( Wvar != 0 ) Wvar->SetProgVar(&wj);
   Pj  = P->Value(); if ( Pvar != 0 ) Pvar->SetProgVar(&Pj);
   Qj  = y[j*cell]; Qvar->SetProgVar(&Qj);
   if ( Aj <= 0.0 ) return -1.0;
   if ( wj <= 0.0 ) return -1.0;            // negative for z too small
   if ( Pj <= 0.0 ) return -1.0;
   Sfj = Sf->Value();

   REAL fact;
   varsys->Reset();
   if ( j == 0 )
   {
      xneigh = gridpts[1];
      zneigh = zj - fabs(gridpts[1]-gridpts[0])*Sfj;
      fact = -1.0;
   }
   else
   {
      xneigh = gridpts[j-1];
      zneigh = zj + fabs(gridpts[j]-gridpts[j-1])*Sfj;
      fact = 1.0;
   }
   Xvar->SetProgVar(&xneigh);
   Zvar->SetProgVar(&zneigh);
   Aneigh = A->Value();
   wneigh = W->Value();
/*
cout << "\n  DeltaNorm (j=" << j << " x=" << xj << ",xn=" << xneigh << ")"
     << "\n    z =" << zj     << " A =" << Aj     << " w="  << wj
     << "\n    zn=" << zneigh << " An=" << Aneigh << " wn=" << wneigh;
*/
   if ( Aneigh <= 0.0 ) return fact;        // positive for z too large
   if ( wneigh <= 0.0 ) return fact;        // negative for z too small

   if ( j == 0 )
   {
      delta =  Aneigh*wj - Aj*wneigh;       // positive for z too large
   }
   else
   {
      delta =  Aj*wneigh - Aneigh*wj;       // positive for z too large
   }
/*
cout << "\n    delta=" << delta;
cout.flush();
*/

   return delta;
}


BOOLEAN RIVCOMP::NormLevel(VARSYS* varsys, REAL* y, CARDINAL j, REAL* z)
{
   const REAL eps = 1e-7;
   REAL z1, z2, z3, delta1, delta2, delta3, direct;
   z1 = *z;
   delta1 = DeltaNorm(varsys,y,j,z1);
   if ( delta1 < 0.0 ) direct =  1.0;
   else                direct = -1.0;
   CARDINAL i   = 0;
   REAL     inc = 1.0;
   while ( 1 )                           // bracketing zero
   {
      z2 = z1 + direct*inc;
      delta2 = DeltaNorm(varsys,y,j,z2);
      if ( delta1*delta2 <= 0.0 ) break;
      if ( i > 100 ) return FALSE;
      i++; inc = ((double)i) * ((double)i) * ((double)i);
   }
/*
cout.precision(10);
cout << "\nNormalLevel (compartment: " << Symbol() << ")"
     << "\n  bracketed level: z1=" << z1 << " (delta1=" << delta1 << ")"
     << "\n                   z2=" << z2 << " (delta2=" << delta2 << ")";
cout.flush();
*/
   while ( 1 )                           // bisection algorithm
   {
      z3 = 0.5*(z1+z2);
      delta3 = DeltaNorm(varsys,y,j,z3);
      if ( delta1*delta3 <= 0 )
      {
         z2 = z3; delta2 = delta3;
      }
      else
      {
         z1 = z3; delta1 = delta3;
      }
/*
cout << "\n  trial:           z3=" << z3 << " (delta3=" << delta3 << ")";
cout.flush();
*/
      if ( fabs(z1-z2) < eps )
      {
         *z = z1;
         break;
      }
   }
/*
cout << "\n  solution:        z =" << z1 << " (delta =" << delta1 << ")";
cout.flush();
*/

   return TRUE;
}


REAL RIVCOMP::DeltaCrit(VARSYS* varsys, REAL* y, CARDINAL j, REAL z)
{
   REAL xj, zj, Aj, wj, Qj;
   REAL delta;

   if ( j >= numgrid ) 
      FatalError("RIVCOMP::DeltaCrit: Illegal index");

   CARDINAL cellhyd = 3+NumStateVar();
   CARDINAL cellsed = 1+NumStateVar();
   CARDINAL cell = cellhyd;
   if ( calcSed == TRUE ) cell += numsedlayers*cellsed;

   varsys->Reset();
   xj  = gridpts[j]; Xvar->SetProgVar(&xj);
   zj  = z;          Zvar->SetProgVar(&zj);
   Aj  = A->Value();
   wj  = W->Value();
   Qj  = y[j*cell];
   if ( Aj <= 0.0 ) return -1.0;              // negative for z too small
   if ( wj <= 0.0 ) return -1.0;

   delta = 1.0 - Qj*Qj*wj/(gravit*Aj*Aj*Aj);  // positive for z too large

   return delta;
}


BOOLEAN RIVCOMP::CritLevel(VARSYS* varsys, REAL* y, CARDINAL j, REAL* z)
{
   const REAL eps = 1e-7;
   REAL z1, z2, z3, delta1, delta2, delta3, direct;
   z1 = *z;
   delta1 = DeltaCrit(varsys,y,j,z1);
   if ( delta1 < 0.0 ) direct =  1.0;
   else                direct = -1.0;
   CARDINAL i   = 0;
   REAL     inc = 1.0;
   while ( 1 )                           // bracketing zero
   {
      z2 = z1 + direct*inc;
      delta2 = DeltaCrit(varsys,y,j,z2);
      if ( delta1*delta2 <= 0.0 ) break;
      if ( i > 100 ) return FALSE;
      i++; inc = ((double)i) * ((double)i) * ((double)i);
   }
   while ( 1 )                           // bisection algorithm
   {
      z3 = 0.5*(z1+z2);
      delta3 = DeltaCrit(varsys,y,j,z3);
      if ( delta1*delta3 <= 0 )
      {
         z2 = z3; delta2 = delta3;
      }
      else
      {
         z1 = z3; delta1 = delta3;
      }
      if ( fabs(z1-z2) < eps )
      {
         *z = z1;
         break;
      }
   }
   return TRUE;
}


REAL RIVCOMP::DeltaDiff(VARSYS* varsys, REAL* y, CARDINAL j, REAL z)
{
   REAL xj, zj, Aj, wj, Pj, Qj, Sfj;
   REAL delta;

   if ( j==0 || j>=numgrid ) 
      FatalError("RIVCOMP::DeltaDiff: Illegal index");

   CARDINAL cellhyd = 3+NumStateVar();
   CARDINAL cellsed = 1+NumStateVar();
   CARDINAL cell = cellhyd;
   if ( calcSed == TRUE ) cell += numsedlayers*cellsed;

   varsys->Reset();
   xj  = gridpts[j-1];             Xvar->SetProgVar(&xj);
   zj  = z;                        Zvar->SetProgVar(&zj);
   Aj  = A->Value();               Avar->SetProgVar(&Aj);
   wj  = W->Value(); if ( Wvar != 0 ) Wvar->SetProgVar(&wj);
   Pj  = P->Value(); if ( Pvar != 0 ) Pvar->SetProgVar(&Pj);
   Qj  = y[(j-1)*cell]; Qvar->SetProgVar(&Qj);
   if ( Aj <= 0.0 ) return -1.0;
   if ( wj <= 0.0 ) return -1.0;           // negative for z too small
   if ( Pj <= 0.0 ) return -1.0;
   Sfj = Sf->Value();

   delta = zj - Sfj*fabs(gridpts[j]-gridpts[j-1]) - y[j*cell+2];
                                           // positive for z too large
   return delta;
}


BOOLEAN RIVCOMP::InitCond(VARSYS* varsys, REAL* Y, CARDINAL callnum)
{
   if ( Active() != TRUE ) return FALSE;

   CARDINAL cellhyd = 3+NumStateVar();
   CARDINAL cellsed = 1+NumStateVar();
   CARDINAL cell = cellhyd;
   if ( calcSed == TRUE ) cell += numsedlayers*cellsed;

   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   REAL zoneindex = 0.0; varsys->SetProgVar(ZoneIndex,&zoneindex);
   REAL eps = 1.0; varsys->SetProgVar(WaterFraction,&eps);
   Xvar = varsys->GetProgVar(SpaceX);
   Zvar = varsys->GetProgVar(WaterLevel);
   Avar = varsys->GetProgVar(CrossSection);
   Wvar = varsys->GetProgVar(Width);
   Pvar = varsys->GetProgVar(Perimeter);
   Qvar = varsys->GetProgVar(Discharge);
   if ( Xvar==0 || Zvar==0 || Avar==0 || Qvar==0 ) return FALSE;

   if ( method==KinWave && callnum>0 ) return TRUE;
   REAL* y = &Y[ind_y];
   CARDINAL i,j,k;

   BOOLEAN found = FALSE;                  // Discharge Q
   for ( j=0; j<numinit; j++ )
   {
      if ( initvar[j] == Qvar )
      {
         found = TRUE; k = j; break;
      }
   }
   if ( found == FALSE )
   {
      *cjob << "\n   RIVCOMP numerical problem:"
            << " Initial discharge not specified";
      cjob->flush();
      return FALSE;
   }
   for ( j=0; j<NumGridPts(); j++ )
   {
      varsys->Reset();
      Xvar->SetProgVar(&gridpts[j]);
      y[j*cell] = initval[k]->Value();
      if ( y[j*cell] <= 0.0 )
      {
         *cjob << "\n   RIVCOMP numerical problem:"
               << " Initial discharge is not positive";
         cjob->flush();
         return FALSE;
      }
   }

   found = FALSE;                          // Water level z0
   for ( j=0; j<numinit; j++ )
   {
      if ( initvar[j] == Zvar )
      {
         found = TRUE; k = j; break;
      }
   }
   for ( j=0; j<NumGridPts(); j++ )
   {
      varsys->Reset();
      Xvar->SetProgVar(&gridpts[j]);
      Qvar->SetProgVar(&y[j*cell]);
      if ( found == TRUE ) y[j*cell+2] = initval[k]->Value();
      else                 y[j*cell+2] = 1.0;
   }

   switch ( method )
   {
      case KinWave:                       // calculate normal levels
         for ( j=0; j<NumGridPts(); j++ )
         {
            if ( NormLevel(varsys,y,j,&y[j*cell+2]) == FALSE )
            {
               *cjob << "\n   RIVCOMP numerical problem:"
                     << " Normal water level not found";
               cjob->flush();
               return FALSE;
            }
         }
         break;
      case DiffWave:                       // integrate levels upstream

         // end level:

         y[NumGridPts()*cell+1] = 1.0;
         y[NumGridPts()*cell+2] = y[(NumGridPts()-1)*cell+2];
         if ( CritLevel(varsys,y,numgrid-1,
                                   &y[NumGridPts()*cell+2]) == FALSE )
         {
            *cjob << "\n   RIVCOMP numerical problem:"
                  << " Critical end level not found";
            cjob->flush();
            return FALSE;
         }
         y[NumGridPts()*cell] = 0.0;  // disp

         switch ( endleveltype )
         {
            case GivenLevel:
               varsys->Reset();
               Xvar->SetProgVar(&gridpts[NumGridPts()-1]);
               y[(NumGridPts()-1)*cell+2] = Zend->Value();
               break;
            case NormalLevel:
               y[NumGridPts()*cell+1]
                               = y[(NumGridPts()-1)*cell+2];
               if ( NormLevel(varsys,y,numgrid-1,
                       &y[NumGridPts()*cell+1]) == FALSE )
               {
                  *cjob << "\n   RIVCOMP numerical problem:"
                        << " Normal end level not found";
                  cjob->flush();
                  return FALSE;
               }
               y[(NumGridPts()-1)*cell+2] = y[NumGridPts()*cell+1];
               break;
            case CriticalLevel:
               y[(NumGridPts()-1)*cell+2] = y[NumGridPts()*cell+2];
               break;
         }

         // at least critical level (no supercrit. flow in diff. approx.):

         if ( y[(NumGridPts()-1)*cell+2] < y[NumGridPts()*cell+2] )
         {
            y[(NumGridPts()-1)*cell+2] = y[NumGridPts()*cell+2];
         }

         // at least downstream level:

         if ( callnum > 0 )
         {
            const CARDINAL advoutconn = 0;
            if ( advoutlink[advoutconn] != 0 )
            {
               if ( advoutlink[advoutconn]->AdvInExZ() == TRUE )
               {
                  if ( advoutlink[advoutconn]->AdvInZ(Y)
                          > y[(NumGridPts()-1)*cell+2] )
                     y[(NumGridPts()-1)*cell+2]
                        = advoutlink[advoutconn]->AdvInZ(Y);
               }
            }
         }

         // fully implicit upstream integration (to guarantee non-oscil-
         // lating solution):

         for ( j=NumGridPts()-1; j>0; j-- )
         {
            const REAL eps = 1e-7;
            REAL z1,z2,z3,delta1,delta2,delta3,direct;
            z1 = y[j*cell+2];
            delta1 = DeltaDiff(varsys,y,j,z1);
            if ( delta1 < 0.0 ) direct =  1.0;
            else                direct = -1.0;
            i = 0;
            REAL inc = 1.0;
            while ( 1 )                  // bracketing zero
            {
               z2 = z1 + direct*inc;
               delta2 = DeltaDiff(varsys,y,j,z2);
               if ( delta1*delta2 <= 0.0 ) break;
               if ( i > 100 )
               {
                  *cjob << "\n   RIVCOMP: numerical problem:"
                        << " calculation of backwater curve failed";
                  return FALSE;
               }
               i++; inc = ((double)i) * ((double)i) * ((double)i);
            }
            while ( 1 )                  // bisection algorithm
            {
               z3 = 0.5*(z1+z2);
               delta3 = DeltaDiff(varsys,y,j,z3);
               if ( delta1*delta3 <= 0.0 )
               {
                  z2 = z3; delta2 = delta3;
               }
               else
               {
                  z1 = z3; delta1 = delta3;
               }
               if ( fabs(z1-z2) < eps ) break;
            }
            y [(j-1)*cell+2] = z1;
         }
   }
   for ( j=0; j<NumGridPts(); j++ )
   {
      varsys->Reset();
      Xvar->SetProgVar(&gridpts[j]);
      Zvar->SetProgVar(&y[j*cell+2]);
      y[j*cell+1] = A->Value();
   }
/*
cout << "\nwater levels:";
for ( j=0; j<NumGridPts(); j++ ) cout << "\n  " << j+1 << ":  "
<< y[j*cell+2];
cout << "\n  dispersion:      " << y[NumGridPts()*cell];
if ( method == DiffWave )
{
cout << "\n  normal level:    " << y[NumGridPts()*cell+1];
cout << "\n  critical level:  " << y[NumGridPts()*cell+2];
}
cout.flush();
*/

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
            Xvar->SetProgVar(&gridpts[j]);
            Qvar->SetProgVar(&y[j*cell]);
            Avar->SetProgVar(&y[j*cell+1]);
            Zvar->SetProgVar(&y[j*cell+2]);
            y[j*cell+3+i] = initval[k]->Value();
         }
         else
         {
            y[j*cell+3+i] = 0.0;
         }
      }
   }

   for ( i=0; i<numvolstatevar; i++ )        // VolStateVar: A*C
   {
      for ( j=0; j<NumGridPts(); j++ )
      {
         y[j*cell+3+i] *= y[j*cell+1];
      }
   }

   if ( withdispersion == TRUE )
   {
      varsys->Reset();
      Xvar->SetProgVar(&gridpts[numgrid-1]);
      Qvar->SetProgVar(&y[(numgrid-1)*cell]);
      Avar->SetProgVar(&y[(numgrid-1)*cell+1]);
      Zvar->SetProgVar(&y[(numgrid-1)*cell+2]);
      REAL w  = W->Value(); if ( Wvar != 0 ) Wvar->SetProgVar(&w);
      REAL p  = P->Value(); if ( Pvar != 0 ) Pvar->SetProgVar(&p);
      REAL sf = Sf->Value(); varsys->SetProgVar(FrictSlope,&sf);
      y[numgrid*cell] = E->Value();
   }
   else
   {
      y[numgrid*cell] = 0.0;
   }

   if ( calcSed==TRUE && numsedlayers>0 )   // Sediment submodel
   {
      CARDINAL layer;
      for ( j=0; j<NumGridPts(); j++ )        // Aeff
      {
         varsys->Reset();
         Xvar->SetProgVar(&gridpts[j]);
         Qvar->SetProgVar(&y[j*cell]);
         Avar->SetProgVar(&y[j*cell+1]);
         Zvar->SetProgVar(&y[j*cell+2]);
         for ( layer=0; layer<numsedlayers; layer++ )
         {
            REAL area = sedlayers[layer]->area->Value();
            REAL porosity = sedlayers[layer]->porosity->Value();
            y[j*cell+cellhyd+layer*cellsed] = area*porosity;
         }
	  }
      for ( layer=0; layer<numsedlayers; layer++ )
      {
         for ( i=0; i<numstatevar; i++ )      // Concentrations C
         {
            BOOLEAN found = FALSE;
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
                  Xvar->SetProgVar(&gridpts[j]);
                  Qvar->SetProgVar(&y[j*cell]);
                  Avar->SetProgVar(&y[j*cell+1]);
                  Zvar->SetProgVar(&y[j*cell+2]);
                  y[j*cell+cellhyd+layer*cellsed+1+i] = initval[k]->Value();
               }
               else
               {
                  y[j*cell+cellhyd+layer*cellsed+1+i] = 0.0;
               }
            }
         }

         for ( i=0; i<numvolstatevar; i++ )        // VolStateVar: Aeff*C
         {
            for ( j=0; j<NumGridPts(); j++ )
            {
               y[j*cell+cellhyd+layer*cellsed+1+i]
                  *= y[j*cell+cellhyd+layer*cellsed];
            }
         }
      }
   }

/*
*cjob << "\n\nInitCond: " << Symbol();
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
}
cjob->flush();
*/

   return TRUE;
}


REAL RIVCOMP::FluxLimit(REAL s1, REAL s2, CARDINAL limiter)
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


BOOLEAN RIVCOMP::Delta(const NUMPAR&, VARSYS* varsys,
                       const REAL* Y, const REAL* YT, REAL* DELTA)
{
   if ( Active() != TRUE ) return FALSE;
   const REAL* y  = &Y[ind_y];
   const REAL* yt = &YT[ind_y];
   REAL* delta = &DELTA[ind_y];
   CARDINAL i,j;
   CARDINAL cellhyd = 3+NumStateVar();
   CARDINAL cellsed = 1+NumStateVar();
   CARDINAL cell = cellhyd;
   CARDINAL layer;
   if ( calcSed == TRUE ) cell += numsedlayers*cellsed;
   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   REAL zoneindex = 0.0; varsys->SetProgVar(ZoneIndex,&zoneindex);
   REAL eps = 1.0;       varsys->SetProgVar(WaterFraction,&eps);
   Xvar = varsys->GetProgVar(SpaceX);
   Zvar = varsys->GetProgVar(WaterLevel);
   Avar = varsys->GetProgVar(CrossSection);
   Wvar = varsys->GetProgVar(Width);
   Pvar = varsys->GetProgVar(Perimeter);
   Qvar = varsys->GetProgVar(Discharge);
   if ( Xvar==0 || Zvar==0 || Avar==0 || Qvar==0 ) return FALSE;
   REAL* conc = new REAL[numstatevar];
   REAL* flux = new REAL[numstatevar];
   REAL* areas  = new REAL[numgrid];
   REAL* widths = new REAL[numgrid];
   REAL* sfs    = new REAL[numgrid];
   REAL* es     = new REAL[numgrid];
   REAL* qlat   = new REAL[numgrid];
   REAL** Ased = 0;
   REAL** pors = 0;
   REAL** Qsed = 0;
   REAL** Esed = 0;
   REAL** qex  = 0;
   REAL** qadv = 0;
   if ( calcSed==TRUE && numsedlayers>0 )
   {
      Ased = new REAL*[numsedlayers];
      pors = new REAL*[numsedlayers];
      Qsed = new REAL*[numsedlayers];
      Esed = new REAL*[numsedlayers];
      qex  = new REAL*[numsedlayers];
      qadv = new REAL*[numsedlayers];
      for ( layer=0; layer<numsedlayers; layer++ )
      {
         Ased[layer] = new REAL[numgrid];         
         pors[layer] = new REAL[numgrid];         
         Qsed[layer] = new REAL[numgrid];         
         Esed[layer] = new REAL[numgrid];         
         qex [layer] = new REAL[numgrid];         
         qadv[layer] = new REAL[numgrid];         
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
   Xvar->SetProgVar(&gridpts[0]);
   Qvar->SetProgVar(&Q);

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
   Xvar->SetProgVar(&gridpts[0]);
   Qvar->SetProgVar(&Q);
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
   if ( inflow != 0 ) Q += inflow->Value();

   // calculate rates:
   // ----------------

   REAL a,w,p,sf;
   Avar->SetProgVar(&a);
   if ( Wvar != 0 ) Wvar->SetProgVar(&w);
   if ( Pvar != 0 ) Pvar->SetProgVar(&p);
   varsys->SetProgVar(FrictSlope,&sf);
   for ( j=0; j<NumGridPts(); j++ )
   {
      Xvar->SetProgVar(&gridpts[j]);
      a = y[j*cell+1];
      Zvar->SetProgVar(&y[j*cell+2]);

      // water column:
      Qvar->SetProgVar(&y[j*cell]);
      zoneindex = 0.0;
      eps = 1.0;
      varsys->Reset();
      areas[j] = A->Value();
      p = P->Value();
      w = W->Value(); widths[j] = w;
      sf = Sf->Value(); sfs[j] = sf;
      if ( latinflow == 0 ) qlat[j] = 0.0;
      else                  qlat[j] = latinflow->Value();
      for ( i=0; i<numvolstatevar; i++ )                 // VolStateVar
      {
         conc[i] = y[j*cell+3+i]/y[j*cell+1];
      }
      for ( i=numvolstatevar; i<numdynstatevar; i++ )    // SurfStateVar
      {
         conc[i] = y[j*cell+3+i];
      }
      for ( i=numdynstatevar; i<numstatevar; i++ )       // EquStateVar
      {
         conc[i] = y[j*cell+3+i];
      }
      CalcRates(&delta[j*cell+3]);
      for ( i=0; i<numvolstatevar; i++ )
      {
         delta[j*cell+3+i] = delta[j*cell+3+i] * y[j*cell+1];
         if ( qlat[j] > 0.0 )
         {
            for ( CARDINAL k=0; k<numlatinput; k++ )
            {
               if ( latinputvar[k] == statevar[i] )
               {
                  // if condition added 26.02.98:
                  if ( j == 1 )
                  {
                     delta[j*cell+3+i] += 0.5*qlat[j]*latinputconc[k]->Value();
                  }
                  else
                  {
                     delta[j*cell+3+i] += qlat[j]*latinputconc[k]->Value();
                  }
               }
            }
         }
         else                                      // qlat<0 !
         {
            // if condition added 26.02.98:
            if ( j == 1 )
            {
               delta[j*cell+3+i] += 0.5*qlat[j]*conc[i];
            }
            else
            {
               delta[j*cell+3+i] += qlat[j]*conc[i];
            }
         }
      }
      if ( withdispersion == TRUE )
      {
         es[j] = E->Value();
         if ( es[j] <= 0.0 )
         {
            *cjob << "\n   RIVCOMP numerical problem:"
                  << " Dispersion is not positive";
            cjob->flush();
            return FALSE;
         }
      }
      else
      {
         es[j] = 0.0;
      }

      // sediment layers:
      if ( calcSed==TRUE && numsedlayers>0 )
      {
         for ( layer=0; layer<numsedlayers; layer++ )
         {
            zoneindex = sedlayers[layer]->ZoneIndex();
            varsys->Reset();
            Ased[layer][j] = sedlayers[layer]->area->Value();
            if ( Ased[layer][j] <= 0.0 )
            {
               *cjob << "\n   RIVCOMP numerical problem:"
                     << " Sediment area is not positive";
               cjob->flush();
               return FALSE;
            }
            a = Ased[layer][i];
            pors[layer][j] = sedlayers[layer]->porosity->Value();
            if ( pors[layer][j] <= 0.0 || pors[layer][j] > 1.0 )
            {
               *cjob << "\n   RIVCOMP numerical problem:"
                     << " Illegal value of sediment porosity";
               cjob->flush();
               return FALSE;
            }
            eps = pors[layer][j];
            Qsed[layer][j] = sedlayers[layer]->discharge->Value();
            Qvar->SetProgVar(&Qsed[layer][j]);
            Esed[layer][j] = sedlayers[layer]->dispersion->Value();
            if ( Esed[layer][j] < 0.0 )
            {
               *cjob << "\n   RIVCOMP numerical problem:"
                     << " Sediment dispersion is negative";
               cjob->flush();
               return FALSE;
            }
            qex[layer][j]  = sedlayers[layer]->exchcoeff->Value();
            if ( qex[layer][j] < 0.0 )
            {
               *cjob << "\n   RIVCOMP numerical problem:"
                     << " Sediment exchange coefficient is negative";
               cjob->flush();
               return FALSE;
            }
            eps = pors[layer][j];
            for ( i=0; i<numvolstatevar; i++ )                 // VolStateVar
            {
               conc[i] = y[j*cell+cellhyd+layer*cellsed+1+i]
				         /y[j*cell+cellhyd+layer*cellsed];
            }
            for ( i=numvolstatevar; i<numdynstatevar; i++ )    // SurfStateVar
            {
               conc[i] = y[j*cell+cellhyd+layer*cellsed+1+i];
            }
            for ( i=numdynstatevar; i<numstatevar; i++ )       // EquStateVar
            {
               conc[i] = y[j*cell+cellhyd+layer*cellsed+1+i];
            }
            CalcRates(&delta[j*cell+cellhyd+layer*cellsed+1]); // C
            for ( i=0; i<numvolstatevar; i++ )
            {
               delta[j*cell+cellhyd+layer*cellsed+1+i]         // Aeff*C
                  = delta[j*cell+cellhyd+layer*cellsed+1+i]
                    * y[j*cell+cellhyd+layer*cellsed];
            }
         }
      }
   }

   // calculate advective exchange coefficients: qadv = sum(dA/dt+dQ/dx)

   REAL dx    = fabs(gridpts[2]-gridpts[1]);  // cells of equal length
   REAL dxinv = 1.0/dx;                       // inverse zell length
   REAL krelax = 0;
   if ( calcSed==TRUE && numsedlayers>0 )
   {
      krelax = 100*fmax(qex[0][0],Q*dxinv)/Ased[0][0];
   }
   if ( calcSed==TRUE && numsedlayers>0 )
   {
      for ( j=0; j<NumGridPts(); j++ )
      {
         for ( CARDINAL complayer=0; complayer<numsedlayers; complayer++ )
         {
            layer = numsedlayers-complayer-1;
            if ( j < 2 )
            {
               if ( j == 0 )
               {
                  qadv[layer][j] = 0;
               }
               else  // j == 1
               {
                  qadv[layer][j]
                     = krelax*(Ased[layer][j]*pors[layer][j]
                       -y[j*cell+cellhyd+layer*cellsed])
                       +(Qsed[layer][1]-0)*dxinv;
               }
            }
            else
            {
               if ( j > numgrid-3 )
               {
                  if ( j == numgrid-1 )
                  {
                     qadv[layer][j] = 0;
                  }
                  else  // j == numgrid-2
                  {
                     qadv[layer][j] 
                        = krelax*(Ased[layer][j]*pors[layer][j]
                          -y[j*cell+cellhyd+layer*cellsed])
                          +(Qsed[layer][numgrid-2]-Qsed[layer][numgrid-3])*dxinv;
                  }
               }
               else
               {
			      qadv[layer][j]
                     = krelax*(Ased[layer][j]*pors[layer][j]
                       -y[j*cell+cellhyd+layer*cellsed])
                        +(Qsed[layer][j]-Qsed[layer][j-1])*dxinv;
               }
            }
            if ( layer < numsedlayers-1 )
            {
               qadv[layer][j] += qadv[layer+1][j];
            }
         }
      }
   }

   zoneindex = 0.0;
   eps = 1.0;
   REAL Q1,Q2,advflux1,advflux2,diffflux;     // fluxes
   REAL Aneigh, wneigh, zneigh;               // geometry of neighbouring
                                              // grid point
   REAL s1, s2;
   REAL phi = 0.0;
   REAL slope;

   // equations at the upstream boundary (grid point 0):
   // --------------------------------------------------

   delta[0] = y[0] - Q;

   switch ( method )                                         // A
   {
      case KinWave:
         varsys->Reset();
         zneigh = y[2] - 0.5*dx*sfs[0];
         Xvar->SetProgVar(&gridpts[1]);
         Zvar->SetProgVar(&zneigh);
         Aneigh = A->Value();
         wneigh = W->Value();
         delta[1] = areas[0]*wneigh - Aneigh*widths[0];
         break;
      case DiffWave:
         slope = - (y[cell+2]-y[2]) * 2.0*dxinv;      // discretization for
                                                      // implicit upstream
                                                      // backwater calculation
         delta[1] = y[1]*(1.0-slope/sfs[0]);
         break;
   }

   delta[2] = y[1]-areas[0];                                 // z0

   for ( i=0; i<numvolstatevar; i++ )                        // VolStateVar
   {
      delta[3+i] = y[3+i] - flux[i]*y[1]/y[0];
      if ( withdispersion == TRUE )       // add diffusive flux
      {
         delta[3+i] += -y[1]*es[0]*(y[cell+3+i]/y[cell+1]-y[3+i]/y[1])
                        *2.0*dxinv
                        *y[1]/y[0];
      }
   }

   for ( i=numvolstatevar; i<numdynstatevar; i++ )           // SurfStateVar
   {
      delta[3+i] -= yt[3+i];
   }
   /* numdynstatevar <= i < numstatevar */                   // EquStateVar

   // sediment submodel:
   if ( calcSed==TRUE && numsedlayers>0 )
   {
    // cross-sectional area:
      for ( layer=0; layer<numsedlayers; layer++ ) // time derivatives
      {
         delta[cellhyd+layer*cellsed]                        // Aeff
            = krelax*(Ased[layer][0]*pors[layer][0]
                      -y[cellhyd+layer*cellsed]);
      }
    // exchange processes:
      // top layer:
      for ( i=0; i<numvolstatevar; i++ )                     // Csed
      {
         delta[cellhyd+1+i]             // exch with water column
            += qex[0][0]
                  *( y[3+i]
                     /y[1]
                    -y[cellhyd+1+i]
                     /y[cellhyd]);
         if ( numsedlayers > 1 )        // exch with lower layer
         {
		    delta[cellhyd+1+i]
               += qex[1][0]
                     *( y[cellhyd+cellsed+1+i]
                        /y[cellhyd+cellsed]
                       -y[cellhyd+1+i]
                        /y[cellhyd]);
         }
      }

      // inner layers:
      if ( numsedlayers > 2 )
      {
         for ( layer=1; layer<numsedlayers-1; layer++ )
         {
            for ( i=0; i<numvolstatevar; i++ )               // Csed
            {
               delta[cellhyd+layer*cellsed+1+i] // exch with upper layer
                  += qex[layer][0]
                        *( y[cellhyd+(layer-1)*cellsed+1+i]
                           /y[cellhyd+(layer-1)*cellsed]
                          -y[cellhyd+layer*cellsed+1+i]
                           /y[cellhyd+layer*cellsed]);
		       delta[cellhyd+layer*cellsed+1+i] // exch with lower layer
                  += qex[layer+1][0]
                        *( y[cellhyd+(layer+1)*cellsed+1+i]
                           /y[cellhyd+(layer+1)*cellsed]
                          -y[cellhyd+layer*cellsed+1+i]
                           /y[cellhyd+layer*cellsed]);
            }
         }
      }

      // bottom layer:
      if ( numsedlayers > 1 )
      {
         for ( i=0; i<numvolstatevar; i++ )                  // Csed
         {
            delta[cellhyd+(numsedlayers-1)*cellsed+1+i] // exch with upper layer
               += qex[numsedlayers-1][0]
			         *( y[cellhyd+(numsedlayers-2)*cellsed+1+i]
                        /y[cellhyd+(numsedlayers-2)*cellsed]
                       -y[cellhyd+(numsedlayers-1)*cellsed+1+i]
                        /y[cellhyd+(numsedlayers-1)*cellsed]);
         }
      }

      for ( layer=0; layer<numsedlayers; layer++ ) // time derivatives
      {
         delta[cellhyd+layer*cellsed]             // Aeff
            -= yt[cellhyd+layer*cellsed];
         for ( i=0; i<numstatevar; i++ )          // Csed
	     {
            delta[cellhyd+layer*cellsed+1+i]
               -= yt[cellhyd+layer*cellsed+1+i];
         }
      }
   }

   // equations in the first cell (gridpoint 1):
   // ------------------------------------------

   // recalculate fluxes with actual values to minimize codiagonals:
   Q = y[0];
   for ( i=0; i<numvolstatevar; i++ )
   {
      flux[i] = y[0]*y[3+i]/y[1];
      if ( withdispersion == TRUE )
      {
         diffflux =  -y[1]*es[0]*(y[cell+3+i]/y[cell+1]-y[3+i]/y[1])
                      *2.0*dxinv;
         flux[i] += diffflux;
      }
   }

   switch ( method )                                         // Q
   {
      case KinWave:
         varsys->Reset();
         zneigh = y[cell+2] + 0.5*dx*sfs[1];
         Xvar->SetProgVar(&gridpts[0]);
         Zvar->SetProgVar(&zneigh);
         Aneigh = A->Value();
         wneigh = W->Value();
         delta[cell] = areas[1]*wneigh - Aneigh*widths[1];
         break;
      case DiffWave:
         slope = - (y[2*cell+2]-y[cell+2]) * dxinv;   // discretization for
                                                      // implicit upstream
                                                      // backwater calculation
         delta[cell] = y[cell]*(1.0-slope/sfs[1]);
         break;
   }

   // flux at the left boundary of the cell:                 // A
   delta[cell+1] = Q*dxinv;
   // calculate flux at the right boundary of the cell:
   Q1 = y[cell];
   Q2 = 0.5*(y[2*cell]+y[cell]);
   s1 = (y[cell+1]-y[1]) / fabs(gridpts[1]-gridpts[0]);
   s2 = (y[2*cell+1]-y[cell+1]) / fabs(gridpts[2]-gridpts[1]);
   if ( method == DiffWave )
   {
      phi = 0.0;
   }
   else
   {
      phi = FluxLimit(s1,s2,ini.I(3));
   }
   Q = (1.0-phi)*Q1 + phi*Q2;
   // subtract flux at the right boundary of the cell:
   delta[cell+1] -= Q*dxinv;
   // add lateral inflow:
   delta[cell+1] += 0.5*qlat[1];
   // add sediment exchange:
   if ( calcSed==TRUE && numsedlayers>0 )
   {
      delta[cell+1] -= qadv[0][1];
   }
   // subtract time derivative:
   delta[cell+1] -= yt[cell+1];

   delta[cell+2] = y[cell+1]-areas[1];                       // z0

   for ( i=0; i<numvolstatevar; i++ )                        // VolStateVar
   {
      // add flux at the left boundary of the cell:
//      delta[cell+3+i] += flux[i]*2.0*dxinv;  correction 16.04.96:
      delta[cell+3+i] += flux[i]*dxinv;
      // calculate flux at the right boundary of the cell:
      advflux1 = y[cell]*y[cell+3+i]/y[cell+1];
      advflux2 = 0.5*(y[cell]*y[cell+3+i]/y[cell+1]
                     +y[2*cell]*y[2*cell+3+i]/y[2*cell+1]);
      s1 = (y[cell+3+i]/y[cell+1]-y[3+i]/y[1])
           / fabs(gridpts[1]-gridpts[0]);
      s2 = (y[2*cell+3+i]/y[2*cell+1]-y[cell+3+i]/y[cell+1])
           / fabs(gridpts[2]-gridpts[1]);
      phi = FluxLimit(s1,s2,ini.I(4));

      if ( fabs(y[3+i]/y[1])
          +fabs(y[cell+3+i]/y[cell+1])
          +fabs(y[2*cell+3+i]/y[2+cell+1]) 
                  < 0.001*statevar[i]->AbsAccuracy() ) phi=0;  // test 2.6.97

      flux[i] = (1.0-phi)*advflux1 + phi*advflux2;
      if ( withdispersion == TRUE )   // add dispersive flux
      {
         diffflux =  -(y[cell+1]+y[2*cell+1])*(es[1]+es[2])*0.25
                      *(y[2*cell+3+i]/y[2*cell+1]
                        -y[cell+3+i]/y[cell+1])
                      *dxinv;
         flux[i] += diffflux;
      }
      // subtract flux at the right boundary of the cell:
//      delta[cell+3+i] -= flux[i]*2.0*dxinv;  correction 16.04.96:
      delta[cell+3+i] -= flux[i]*dxinv;
      // add sediment exchange:
      if ( calcSed==TRUE && numsedlayers>0 )
      {
         delta[cell+3+i]
            -= qex[0][1]*( y[cell+3+i]
                           /y[cell+1]
                          -y[cell+cellhyd+1+i]
                           /y[cell+cellhyd]);
         if ( qadv[0][1] >= 0 )
         {
            delta[cell+3+i]
               -= qadv[0][1]*y[cell+3+i]
                             /y[cell+1];
         }
         else
         {
            delta[cell+3+i]
               -= qadv[0][1]*y[cell+cellhyd+1+i]
                            /y[cell+cellhyd];
         }
      }
      // subtract time derivative:
      delta[cell+3+i] -= yt[cell+3+i];
   }

   for ( i=numvolstatevar; i<numdynstatevar; i++ )           // SurfStateVar
   {
      delta[cell+3+i] -= yt[cell+3+i];
   }

   /* numdynstatevar <= i < numstatevar */                   // EquStateVar

   // sediment submodel:
   if ( calcSed==TRUE && numsedlayers>0 )
   {
    // cross-sectional area:
      for ( layer=0; layer<numsedlayers; layer++ ) // time derivatives
      {
         delta[cell+cellhyd+layer*cellsed]                   // Aeff
            = krelax*(Ased[layer][1]*pors[layer][1]
                      -y[cell+cellhyd+layer*cellsed]);
      }
    // transport processes:
      for ( layer=0; layer<numsedlayers; layer++ )
      {
         // advective transport:
         if ( Qsed[layer][2] < 0 )
         {
            for ( i=0; i<numvolstatevar; i++ )
            {
               delta[cell+cellhyd+layer*cellsed+1+i]
                  -= Qsed[layer][2]
                     *y[2*cell+cellhyd+layer*cellsed+1+i]
                     /y[2*cell+cellhyd+layer*cellsed]
                     *dxinv;
            }
         }
         if ( Qsed[layer][1] > 0 )
         {
            for ( i=0; i<numvolstatevar; i++ )
            {
               delta[cell+cellhyd+layer*cellsed+1+i]
                  -= Qsed[layer][1]
                     *y[cell+cellhyd+layer*cellsed+1+i]
                     /y[cell+cellhyd+layer*cellsed]
                     *dxinv;
            }
         }
         // dispersive transport:
         for ( i=0; i<numvolstatevar; i++ )
         {
            delta[cell+cellhyd+layer*cellsed+1+i]
               += 0.25*(Esed[layer][1]+Esed[layer][2])
                      *(y[cell+cellhyd+layer*cellsed]
                        +y[2*cell+cellhyd+layer*cellsed])
                  *( y[2*cell+cellhyd+layer*cellsed+1+i]
                     /y[2*cell+cellhyd+layer*cellsed]
                    -y[cell+cellhyd+layer*cellsed+1+i]
                     /y[cell+cellhyd+layer*cellsed])
                  *dxinv*dxinv;
         }
      }
    // exchange processes:
      // top layer:
      for ( i=0; i<numvolstatevar; i++ )                     // Csed
      {
         delta[cell+cellhyd+1+i]        // exch with water column
            += qex[0][1]
                  *( y[cell+3+i]
                     /y[cell+1]
                    -y[cell+cellhyd+1+i]
                     /y[cell+cellhyd]);
         if ( qadv[0][1] >= 0 )
         {
            delta[cell+cellhyd+1+i]
               += qadv[0][1]*y[cell+3+i]
                             /y[cell+1];
         }
         else
         {
            delta[cell+cellhyd+1+i]
               += qadv[0][1]*y[cell+cellhyd+1+i]
                            /y[cell+cellhyd];
         }
         if ( numsedlayers > 1 )        // exch with lower layer
         {
		    delta[cell+cellhyd+1+i]
               += qex[1][1]
                     *( y[cell+cellhyd+cellsed+1+i]
                        /y[cell+cellhyd+cellsed]
                       -y[cell+cellhyd+1+i]
                        /y[cell+cellhyd]);
            if ( qadv[1][1] >= 0 )
            {
               delta[cell+cellhyd+1+i]
                  -= qadv[1][1]*y[cell+cellhyd+1+i]
                               /y[cell+cellhyd];
            }
            else
            {
               delta[cell+cellhyd+1+i]
                  -= qadv[1][1]*y[cell+cellhyd+cellsed+1+i]
                               /y[cell+cellhyd+cellsed];
            }
         }
      }

      // inner layers:
      if ( numsedlayers > 2 )
      {
         for ( layer=1; layer<numsedlayers-1; layer++ )
         {
            for ( i=0; i<numvolstatevar; i++ )               // Csed
            {
               delta[cell+cellhyd+layer*cellsed+1+i] // exch with upper layer
                  += qex[layer][1]
                        *( y[cell+cellhyd+(layer-1)*cellsed+1+i]
                           /y[cell+cellhyd+(layer-1)*cellsed]
                          -y[cell+cellhyd+layer*cellsed+1+i]
                           /y[cell+cellhyd+layer*cellsed]);
               if ( qadv[layer][1] >= 0 )
               {
                  delta[cell+cellhyd+layer*cellsed+1+i]
                     += qadv[layer][1]*y[cell+cellhyd+(layer-1)*cellsed+1+i]
                                      /y[cell+cellhyd+(layer-1)*cellsed];
               }
               else
               {
                  delta[cell+cellhyd+layer*cellsed+1+i]
                     += qadv[layer][1]*y[cell+cellhyd+layer*cellsed+1+i]
                                      /y[cell+cellhyd+layer*cellsed];
               }
		       delta[cell+cellhyd+layer*cellsed+1+i] // exch with lower layer
                  += qex[layer+1][1]
                        *( y[cell+cellhyd+(layer+1)*cellsed+1+i]
                           /y[cell+cellhyd+(layer+1)*cellsed]
                          -y[cell+cellhyd+layer*cellsed+1+i]
                           /y[cell+cellhyd+layer*cellsed]);
               if ( qadv[layer+1][1] >= 0 )
               {
                  delta[cell+cellhyd+layer*cellsed+1+i]
                     -= qadv[layer][1]*y[cell+cellhyd+layer*cellsed+1+i]
                                      /y[cell+cellhyd+layer*cellsed];
               }
               else
               {
                  delta[cell+cellhyd+layer*cellsed+1+i]
                     -= qadv[layer][1]*y[cell+cellhyd+(layer+1)*cellsed+1+i]
                                      /y[cell+cellhyd+(layer+1)*cellsed];
               }
            }
         }
      }

      // bottom layer:
      if ( numsedlayers > 1 )
      {
         for ( i=0; i<numvolstatevar; i++ )                  // Csed
         {
            delta[cell+cellhyd+(numsedlayers-1)*cellsed+1+i] // exch with upper layer
               += qex[numsedlayers-1][1]
			         *( y[cell+cellhyd+(numsedlayers-2)*cellsed+1+i]
                        /y[cell+cellhyd+(numsedlayers-2)*cellsed]
                       -y[cell+cellhyd+(numsedlayers-1)*cellsed+1+i]
                        /y[cell+cellhyd+(numsedlayers-1)*cellsed]);
            if ( qadv[numsedlayers-1][1] >= 0 )
            {
               delta[cell+cellhyd+(numsedlayers-1)*cellsed+1+i]
                  += qadv[numsedlayers-1][1]*y[cell+cellhyd+(numsedlayers-2)*cellsed+1+i]
                                            /y[cell+cellhyd+(numsedlayers-2)*cellsed];
            }
            else
            {
               delta[cell+cellhyd+(numsedlayers-1)*cellsed+1+i]
                  += qadv[numsedlayers-1][1]*y[cell+cellhyd+(numsedlayers-1)*cellsed+1+i]
                                            /y[cell+cellhyd+(numsedlayers-1)*cellsed];
            }
         }
      }

      for ( layer=0; layer<numsedlayers; layer++ ) // time derivatives
      {
         delta[cell+cellhyd+layer*cellsed]        // Aeff
            -= yt[cell+cellhyd+layer*cellsed];
         for ( i=0; i<numstatevar; i++ )          // Csed
         {
            delta[cell+cellhyd+layer*cellsed+1+i]
               -= yt[cell+cellhyd+layer*cellsed+1+i];
         }
      }
   }

   // equations in the inner cells (gridpoints 2 to n-3):
   // ---------------------------------------------------

   for ( j=2; j<NumGridPts()-2; j++ )
   {
      switch ( method )                                      // Q
      {
         case KinWave:
            varsys->Reset();
            zneigh = y[j*cell+2] + dx*sfs[j];
            Xvar->SetProgVar(&gridpts[j-1]);
            Zvar->SetProgVar(&zneigh);
            Aneigh = A->Value();
            wneigh = W->Value();
            delta[j*cell] = areas[j]*wneigh - Aneigh*widths[j];
            break;
         case DiffWave:
            slope = - (y[(j+1)*cell+2]-y[j*cell+2] ) * dxinv;
                                                      // discretization for
                                                      // implicit upstream
                                                      // backwater calculation
            delta[j*cell] = y[j*cell]*(1.0-slope/sfs[j]);
            break;
      }

      // flux at the left boundary of the cell:              // A
      delta[j*cell+1] = Q*dxinv;
      // calculate flux at the right boundary of the cell:
      Q1 = y[j*cell];
      Q2 = 0.5*(y[j*cell]+y[(j+1)*cell]);
      s1 = (y[j*cell+1]-y[(j-1)*cell+1]) / fabs(gridpts[j]-gridpts[j-1]);
      s2 = (y[(j+1)*cell+1]-y[j*cell+1]) / fabs(gridpts[j+1]-gridpts[j]);
      if ( method == DiffWave )
      {
         phi = 0.0;
      }
      else
      {
         phi = FluxLimit(s1,s2,ini.I(3));
      }
      Q = (1.0-phi)*Q1 + phi*Q2;
      // subtract flux at the right boundary of the cell:
      delta[j*cell+1] -= Q*dxinv;
      // add lateral inflow:
      delta[j*cell+1] += qlat[j];
      // add sediment exchange:
      if ( calcSed==TRUE && numsedlayers>0 )
      {
         delta[j*cell+1] -= qadv[0][j];
      }
      // subtract time derivative:
      delta[j*cell+1] -= yt[j*cell+1];

      delta[j*cell+2] = y[j*cell+1]-areas[j];                // z0

      for ( i=0; i<numvolstatevar; i++ )                     // VolStateVar
      {
         // add flux at the left boundary of the cell:
         delta[j*cell+3+i] += flux[i]*dxinv;
         // calculate flux at the right boundary of the cell:
         advflux1 = y[j*cell]*y[j*cell+3+i]/y[j*cell+1];
         advflux2 = 0.5*(y[j*cell]*y[j*cell+3+i]/y[j*cell+1]
                        +y[(j+1)*cell]*y[(j+1)*cell+3+i]/y[(j+1)*cell+1]);
         s1 = (y[j*cell+3+i]/y[j*cell+1]-y[(j-1)*cell+3+i]/y[(j-1)*cell+1])
              / fabs(gridpts[j]-gridpts[j-1]);
         s2 = (y[(j+1)*cell+3+i]/y[(j+1)*cell+1]-y[j*cell+3+i]/y[j*cell+1])
              / fabs(gridpts[j+1]-gridpts[j]);
         phi = FluxLimit(s1,s2,ini.I(4));

         if ( fabs(y[(j-1)*cell+3+i]/y[(j-1)*cell+1])
             +fabs(y[j*cell+3+i]/y[j*cell+1])
             +fabs(y[(j+1)*cell+3+i]/y[(j+1)*cell+1]) 
                  < 0.001*statevar[i]->AbsAccuracy() ) phi=0;  // test 2.6.97

         flux[i] = (1.0-phi)*advflux1 + phi*advflux2;
         if ( withdispersion == TRUE )
         {
            diffflux =  -(y[j*cell+1]+y[(j+1)*cell+1])*(es[j]+es[j+1])*0.25
                         *(y[(j+1)*cell+3+i]/y[(j+1)*cell+1]
                           -y[j*cell+3+i]/y[j*cell+1])
                         *dxinv;
            flux[i] += diffflux;
         }
         // subtract flux at the right boundary of the cell:
         delta[j*cell+3+i] -= flux[i]*dxinv;
         // add sediment exchange:
         if ( calcSed==TRUE && numsedlayers>0 )
         {
            delta[j*cell+3+i]
               -= qex[0][j]*( y[j*cell+3+i]
                              /y[j*cell+1]
                             -y[j*cell+cellhyd+1+i]
                              /y[j*cell+cellhyd]);
            if ( qadv[0][j] >= 0 )
            {
               delta[j*cell+3+i]
                  -= qadv[0][j]*y[j*cell+3+i]
                                /y[j*cell+1];
            }
            else
            {
               delta[j*cell+3+i]
                  -= qadv[0][j]*y[j*cell+cellhyd+1+i]
                               /y[j*cell+cellhyd];
            }
         }
         // subtract time derivative:
         delta[j*cell+3+i] -= yt[j*cell+3+i];
      }

      for ( i=numvolstatevar; i<numdynstatevar; i++ )        // SurfStateVar
      {
         delta[j*cell+3+i] -= yt[j*cell+3+i];
      }

      /* numdynstatevar <= i < numstatevar */                // EquStateVar

      // sediment submodel:
      if ( calcSed==TRUE && numsedlayers>0 )
      {
       // cross-sectional area:
         for ( layer=0; layer<numsedlayers; layer++ ) // time derivatives
         {
            delta[j*cell+cellhyd+layer*cellsed]              // Aeff
               = krelax*(Ased[layer][j]*pors[layer][j]
                         -y[j*cell+cellhyd+layer*cellsed]);
         }
       // transport processes:
         for ( layer=0; layer<numsedlayers; layer++ )
         {
            // advective transport:
            if ( Qsed[layer][j-1] > 0 )
            {
               for ( i=0; i<numvolstatevar; i++ )
               {
                  delta[j*cell+cellhyd+layer*cellsed+1+i]
                     += Qsed[layer][j-1]
                        *y[(j-1)*cell+cellhyd+layer*cellsed+1+i]
                        /y[(j-1)*cell+cellhyd+layer*cellsed]
                        *dxinv;
               }
            }
            if ( Qsed[layer][j+1] < 0 )
            {
               for ( i=0; i<numvolstatevar; i++ )
               {
                  delta[j*cell+cellhyd+layer*cellsed+1+i]
                     -= Qsed[layer][j+1]
                        *y[(j+1)*cell+cellhyd+layer*cellsed+1+i]
                        /y[(j+1)*cell+cellhyd+layer*cellsed]
                        *dxinv;
               }
            }
            for ( i=0; i<numvolstatevar; i++ )
            {
               delta[j*cell+cellhyd+layer*cellsed+1+i]
                  -= fabs(Qsed[layer][j])
                     *y[j*cell+cellhyd+layer*cellsed+1+i]
                     /y[j*cell+cellhyd+layer*cellsed]
                     *dxinv;
            }
            // dispersive transport:
            for ( i=0; i<numvolstatevar; i++ )
            {
               delta[j*cell+cellhyd+layer*cellsed+1+i]
                  -= 0.25*(Esed[layer][j-1]+Esed[layer][j])
                         *(y[(j-1)*cell+cellhyd+layer*cellsed]
                           +y[j*cell+cellhyd+layer*cellsed])
                     *( y[j*cell+cellhyd+layer*cellsed+1+i]
                        /y[j*cell+cellhyd+layer*cellsed]
                       -y[(j-1)*cell+cellhyd+layer*cellsed+1+i]
                        /y[(j-1)*cell+cellhyd+layer*cellsed])
                     *dxinv*dxinv;
               delta[j*cell+cellhyd+layer*cellsed+1+i]
                  += 0.25*(Esed[layer][j]+Esed[layer][j+1])
                         *(y[j*cell+cellhyd+layer*cellsed]
                           +y[(j+1)*cell+cellhyd+layer*cellsed])
                     *( y[(j+1)*cell+cellhyd+layer*cellsed+1+i]
                        /y[(j+1)*cell+cellhyd+layer*cellsed]
                       -y[j*cell+cellhyd+layer*cellsed+1+i]
                        /y[j*cell+cellhyd+layer*cellsed])
                     *dxinv*dxinv;
            }
         }
       // exchange processes:
         // top layer:
         for ( i=0; i<numvolstatevar; i++ )                     // Csed
         {
            delta[j*cell+cellhyd+1+i]      // exch with water column
               += qex[0][j]
                     *( y[j*cell+3+i]
                        /y[j*cell+1]
                       -y[j*cell+cellhyd+1+i]
                        /y[j*cell+cellhyd]);
            if ( qadv[0][j] >= 0 )
            {
               delta[j*cell+cellhyd+1+i]
                  += qadv[0][j]*y[j*cell+3+i]
                                /y[j*cell+1];
            }
            else
            {
               delta[j*cell+cellhyd+1+i]
                  += qadv[0][j]*y[j*cell+cellhyd+1+i]
                               /y[j*cell+cellhyd];
            }
            if ( numsedlayers > 1 )        // exch with lower layer
            {
               delta[j*cell+cellhyd+1+i]
                  += qex[1][j]
                        *( y[j*cell+cellhyd+cellsed+1+i]
                           /y[j*cell+cellhyd+cellsed]
                          -y[j*cell+cellhyd+1+i]
                           /y[j*cell+cellhyd]);
               if ( qadv[1][j] >= 0 )
               {
                  delta[j*cell+cellhyd+1+i]
                     -= qadv[1][j]*y[j*cell+cellhyd+1+i]
                                  /y[j*cell+cellhyd];
               }
               else
               {
                  delta[j*cell+cellhyd+1+i]
                     -= qadv[1][j]*y[j*cell+cellhyd+cellsed+1+i]
                                  /y[j*cell+cellhyd+cellsed];
               }
            }
         }

         // inner layers:
         if ( numsedlayers > 2 )
         {
            for ( layer=1; layer<numsedlayers-1; layer++ )
            {
               for ( i=0; i<numvolstatevar; i++ )               // Csed
               {
                  delta[j*cell+cellhyd+layer*cellsed+1+i] // exch with upper layer
                     += qex[layer][j]
                           *( y[j*cell+cellhyd+(layer-1)*cellsed+1+i]
                              /y[j*cell+cellhyd+(layer-1)*cellsed]
                             -y[j*cell+cellhyd+layer*cellsed+1+i]
                              /y[j*cell+cellhyd+layer*cellsed]);
                  if ( qadv[layer][j] >= 0 )
                  {
                     delta[j*cell+cellhyd+layer*cellsed+1+i]
                        += qadv[layer][j]*y[j*cell+cellhyd+(layer-1)*cellsed+1+i]
                                         /y[j*cell+cellhyd+(layer-1)*cellsed];
                  }
                  else
                  {
                     delta[j*cell+cellhyd+layer*cellsed+1+i]
                        += qadv[layer][j]*y[j*cell+cellhyd+layer*cellsed+1+i]
                                         /y[j*cell+cellhyd+layer*cellsed];
                  }
		          delta[j*cell+cellhyd+layer*cellsed+1+i] // exch with lower layer
                     += qex[layer+1][j]
                           *( y[j*cell+cellhyd+(layer+1)*cellsed+1+i]
                              /y[j*cell+cellhyd+(layer+1)*cellsed]
                             -y[j*cell+cellhyd+layer*cellsed+1+i]
                              /y[j*cell+cellhyd+layer*cellsed]);
                  if ( qadv[layer+1][j] >= 0 )
                  {
                     delta[j*cell+cellhyd+layer*cellsed+1+i]
                        -= qadv[layer][j]*y[j*cell+cellhyd+layer*cellsed+1+i]
                                         /y[j*cell+cellhyd+layer*cellsed];
                  }
                  else
                  {
                     delta[j*cell+cellhyd+layer*cellsed+1+i]
                        -= qadv[layer][j]*y[j*cell+cellhyd+(layer+1)*cellsed+1+i]
                                         /y[j*cell+cellhyd+(layer+1)*cellsed];
                  }
               }
            }
         }

         // bottom layer:
         if ( numsedlayers > 1 )
         {
            for ( i=0; i<numvolstatevar; i++ )                  // Csed
            {
               delta[j*cell+cellhyd+(numsedlayers-1)*cellsed+1+i] // exch with upper layer
                  += qex[numsedlayers-1][j]
			            *( y[j*cell+cellhyd+(numsedlayers-2)*cellsed+1+i]
                           /y[j*cell+cellhyd+(numsedlayers-2)*cellsed]
                          -y[j*cell+cellhyd+(numsedlayers-1)*cellsed+1+i]
                           /y[j*cell+cellhyd+(numsedlayers-1)*cellsed]);
               if ( qadv[numsedlayers-1][j] >= 0 )
               {
                  delta[j*cell+cellhyd+(numsedlayers-1)*cellsed+1+i]
                     += qadv[numsedlayers-1][j]*y[j*cell+cellhyd+(numsedlayers-2)*cellsed+1+i]
                                               /y[j*cell+cellhyd+(numsedlayers-2)*cellsed];
               }
               else
               {
                  delta[j*cell+cellhyd+(numsedlayers-1)*cellsed+1+i]
                     += qadv[numsedlayers-1][j]*y[j*cell+cellhyd+(numsedlayers-1)*cellsed+1+i]
                                               /y[j*cell+cellhyd+(numsedlayers-1)*cellsed];
               }
            }
         }
	  
         for ( layer=0; layer<numsedlayers; layer++ ) // time derivatives
         {
            delta[j*cell+cellhyd+layer*cellsed]      // Aeff
               -= yt[j*cell+cellhyd+layer*cellsed];
            for ( i=0; i<numstatevar; i++ )          // Csed
            {
               delta[j*cell+cellhyd+layer*cellsed+1+i]
                  -= yt[j*cell+cellhyd+layer*cellsed+1+i];
            }
         }
      }
   }

   // equations in the last cell (grid point n-2):
   // --------------------------------------------

   j = NumGridPts()-2;

   switch ( method )                                         // Q
   {
      case KinWave:
         varsys->Reset();
         zneigh = y[j*cell+2] + dx*sfs[j];
         Xvar->SetProgVar(&gridpts[j-1]);
         Zvar->SetProgVar(&zneigh);
         Aneigh = A->Value();
         wneigh = W->Value();
         delta[j*cell] = areas[j]*wneigh - Aneigh*widths[j];
         break;
      case DiffWave:
         slope = - (y[(j+1)*cell+2]-y[j*cell+2]) * 2.0*dxinv;
                                                      // discretization for
                                                      // implicit upstream
                                                      // backwater calculation
         delta[j*cell] = y[j*cell]*(1.0-slope/sfs[j]);
         break;
   }

   // flux at the left boundary of the cell:                 // A
   delta[j*cell+1] = Q*dxinv;
   // calculate flux at the right boundary of the cell:
   Q1 = y[j*cell];
   Q2 = y[(j+1)*cell];
   s1 = (y[j*cell+1]-y[(j-1)*cell+1]) / fabs(gridpts[j]-gridpts[j-1]);
   s2 = (y[(j+1)*cell+1]-y[j*cell+1]) / fabs(gridpts[j+1]-gridpts[j]);
   if ( method == DiffWave )
   {
      phi = 0.0;
   }
   else
   {
      phi = FluxLimit(s1,s2,ini.I(3));
   }
   Q = (1.0-phi)*Q1 + phi*Q2;
   // subtract flux at the right boundary of the cell:
   delta[j*cell+1] -= Q*dxinv;
   // add lateral inflow:
   delta[j*cell+1] += qlat[j];
   // add sediment exchange:
   if ( calcSed==TRUE && numsedlayers>0 )
   {
      delta[j*cell+1] -= qadv[0][j];
   }
   // subtract time derivative:
   delta[j*cell+1] -= yt[j*cell+1];

   delta[j*cell+2] = y[j*cell+1]-areas[j];                   // z0

   for ( i=0; i<numvolstatevar; i++ )                        // VolStateVar
   {
      // add flux at the left boundary of the cell:
      delta[j*cell+3+i] += flux[i]*dxinv;
      // calculate flux at the right boundary of the cell:
      advflux1 = y[j*cell]*y[j*cell+3+i]/y[j*cell+1];
      advflux2 = y[(j+1)*cell]*y[(j+1)*cell+3+i]/y[(j+1)*cell+1];
      s1 = (y[j*cell+3+i]/y[j*cell+1]-y[(j-1)*cell+3+i]/y[(j-1)*cell+1])
           / fabs(gridpts[j]-gridpts[j-1]);
      s2 = (y[(j+1)*cell+3+i]/y[(j+1)*cell+1]-y[j*cell+3+i]/y[j*cell+1])
           / fabs(gridpts[j+1]-gridpts[j]);
      phi = FluxLimit(s1,s2,ini.I(4));
      flux[i] = (1.0-phi)*advflux1 + phi*advflux2;

      if ( fabs(y[(j-1)*cell+3+i]/y[(j-1)*cell+1])
          +fabs(y[j*cell+3+i]/y[j*cell+1])
          +fabs(y[(j+1)*cell+3+i]/y[(j+1)*cell+1]) 
               < 0.001*statevar[i]->AbsAccuracy() ) phi=0;  // test 2.6.97

      // subtract flux at the right boundary of the cell:
      delta[j*cell+3+i] -= flux[i]*dxinv;
      // add sediment exchange:
      if ( calcSed==TRUE && numsedlayers>0 )
      {
         delta[j*cell+3+i]
            -= qex[0][j]*( y[j*cell+3+i]
                           /y[j*cell+1]
                          -y[j*cell+cellhyd+1+i]
                           /y[j*cell+cellhyd]);
         if ( qadv[0][j] >= 0 )
         {
            delta[j*cell+3+i]
               -= qadv[0][j]*y[j*cell+3+i]
                             /y[j*cell+1];
         }
         else
         {
            delta[j*cell+3+i]
               -= qadv[0][j]*y[j*cell+cellhyd+1+i]
                            /y[j*cell+cellhyd];
         }
      }
      // subtract time derivative:
      delta[j*cell+3+i] -= yt[j*cell+3+i];
   }

   for ( i=numvolstatevar; i<numdynstatevar; i++ )           // SurfStateVar
   {
      delta[j*cell+3+i] -= yt[j*cell+3+i];
   }

   /* numdynstatevar <= i < numstatevar */                   // EquStateVar

   // sediment submodel:
   if ( calcSed==TRUE && numsedlayers>0 )
   {
    // cross-sectional area:
      for ( layer=0; layer<numsedlayers; layer++ ) // time derivatives
      {
         delta[j*cell+cellhyd+layer*cellsed]                 // Aeff
            = krelax*(Ased[layer][j]*pors[layer][j]
                      -y[j*cell+cellhyd+layer*cellsed]);
      }
    // transport processes:
      for ( layer=0; layer<numsedlayers; layer++ )
      {
         // advective transport:
         if ( Qsed[layer][j-1] > 0 )
         {
            for ( i=0; i<numvolstatevar; i++ )
            {
               delta[j*cell+cellhyd+layer*cellsed+1+i]
                  += Qsed[layer][j-1]
                     *y[(j-1)*cell+cellhyd+layer*cellsed+1+i]
                     /y[(j-1)*cell+cellhyd+layer*cellsed]
                     *dxinv;
            }
         }
         if ( Qsed[layer][j] < 0 )
         {
            for ( i=0; i<numvolstatevar; i++ )
            {
               delta[j*cell+cellhyd+layer*cellsed+1+i]
                  -= fabs(Qsed[layer][j])
                     *y[j*cell+cellhyd+layer*cellsed+1+i]
                     /y[j*cell+cellhyd+layer*cellsed]
                     *dxinv;
            }
         }
         // dispersive transport:
         for ( i=0; i<numvolstatevar; i++ )
         {
            delta[j*cell+cellhyd+layer*cellsed+1+i]
               -= 0.25*(Esed[layer][j-1]+Esed[layer][j])
                      *(y[(j-1)*cell+cellhyd+layer*cellsed]
                        +y[j*cell+cellhyd+layer*cellsed])
                  *( y[j*cell+cellhyd+layer*cellsed+1+i]
                     /y[j*cell+cellhyd+layer*cellsed]
                    -y[(j-1)*cell+cellhyd+layer*cellsed+1+i]
                     /y[(j-1)*cell+cellhyd+layer*cellsed])
                  *dxinv*dxinv;
         }
      }
    // exchange processes:
      // top layer:
      for ( i=0; i<numvolstatevar; i++ )                     // Csed
      {
         delta[j*cell+cellhyd+1+i]      // exch with water column
            += qex[0][j]
                  *( y[j*cell+3+i]
                     /y[j*cell+1]
                    -y[j*cell+cellhyd+1+i]
                     /y[j*cell+cellhyd]);
         if ( qadv[0][j] >= 0 )
         {
            delta[j*cell+cellhyd+1+i]
               += qadv[0][j]*y[j*cell+3+i]
                             /y[j*cell+1];
         }
         else
         {
            delta[j*cell+cellhyd+1+i]
               += qadv[0][j]*y[j*cell+cellhyd+1+i]
                            /y[j*cell+cellhyd];
         }
         if ( numsedlayers > 1 )        // exch with lower layer
         {
            delta[j*cell+cellhyd+1+i]
               += qex[1][j]
                     *( y[j*cell+cellhyd+cellsed+1+i]
                        /y[j*cell+cellhyd+cellsed]
                       -y[j*cell+cellhyd+1+i]
                        /y[j*cell+cellhyd]);
            if ( qadv[1][j] >= 0 )
            {
               delta[j*cell+cellhyd+1+i]
                  -= qadv[1][j]*y[j*cell+cellhyd+1+i]
                               /y[j*cell+cellhyd];
            }
            else
            {
               delta[j*cell+cellhyd+1+i]
                  -= qadv[1][j]*y[j*cell+cellhyd+cellsed+1+i]
                               /y[j*cell+cellhyd+cellsed];
            }
         }
      }

      // inner layers:
      if ( numsedlayers > 2 )
      {
         for ( layer=1; layer<numsedlayers-1; layer++ )
         {
            for ( i=0; i<numvolstatevar; i++ )               // Csed
            {
               delta[j*cell+cellhyd+layer*cellsed+1+i] // exch with upper layer
                  += qex[layer][j]
                        *( y[j*cell+cellhyd+(layer-1)*cellsed+1+i]
                           /y[j*cell+cellhyd+(layer-1)*cellsed]
                          -y[j*cell+cellhyd+layer*cellsed+1+i]
                           /y[j*cell+cellhyd+layer*cellsed]);
               if ( qadv[layer][j] >= 0 )
               {
                  delta[j*cell+cellhyd+layer*cellsed+1+i]
                     += qadv[layer][j]*y[j*cell+cellhyd+(layer-1)*cellsed+1+i]
                                      /y[j*cell+cellhyd+(layer-1)*cellsed];
               }
               else
               {
                  delta[j*cell+cellhyd+layer*cellsed+1+i]
                     += qadv[layer][j]*y[j*cell+cellhyd+layer*cellsed+1+i]
                                      /y[j*cell+cellhyd+layer*cellsed];
               }
               delta[j*cell+cellhyd+layer*cellsed+1+i] // exch with lower layer
                  += qex[layer+1][j]
                        *( y[j*cell+cellhyd+(layer+1)*cellsed+1+i]
                           /y[j*cell+cellhyd+(layer+1)*cellsed]
                          -y[j*cell+cellhyd+layer*cellsed+1+i]
                           /y[j*cell+cellhyd+layer*cellsed]);
               if ( qadv[layer+1][j] >= 0 )
               {
                  delta[j*cell+cellhyd+layer*cellsed+1+i]
                     -= qadv[layer][j]*y[j*cell+cellhyd+layer*cellsed+1+i]
                                      /y[j*cell+cellhyd+layer*cellsed];
               }
               else
               {
                  delta[j*cell+cellhyd+layer*cellsed+1+i]
                     -= qadv[layer][j]*y[j*cell+cellhyd+(layer+1)*cellsed+1+i]
                                      /y[j*cell+cellhyd+(layer+1)*cellsed];
               }
            }
         }
      }

      // bottom layer:
      if ( numsedlayers > 1 )
      {
         for ( i=0; i<numvolstatevar; i++ )                  // Csed
         {
            delta[j*cell+cellhyd+(numsedlayers-1)*cellsed+1+i] // exch with upper layer
               += qex[numsedlayers-1][j]
                     *( y[j*cell+cellhyd+(numsedlayers-2)*cellsed+1+i]
                        /y[j*cell+cellhyd+(numsedlayers-2)*cellsed]
                       -y[j*cell+cellhyd+(numsedlayers-1)*cellsed+1+i]
                        /y[j*cell+cellhyd+(numsedlayers-1)*cellsed]);
            if ( qadv[numsedlayers-1][j] >= 0 )
            {
               delta[j*cell+cellhyd+(numsedlayers-1)*cellsed+1+i]
                  += qadv[numsedlayers-1][j]*y[j*cell+cellhyd+(numsedlayers-2)*cellsed+1+i]
                                            /y[j*cell+cellhyd+(numsedlayers-2)*cellsed];
            }
            else
            {
               delta[j*cell+cellhyd+(numsedlayers-1)*cellsed+1+i]
                  += qadv[numsedlayers-1][j]*y[j*cell+cellhyd+(numsedlayers-1)*cellsed+1+i]
                                            /y[j*cell+cellhyd+(numsedlayers-1)*cellsed];
            }
         }
      }
 
      for ( layer=0; layer<numsedlayers; layer++ ) // time derivatives
      {
         delta[j*cell+cellhyd+layer*cellsed]      // Aeff
            -= yt[j*cell+cellhyd+layer*cellsed];
         for ( i=0; i<numstatevar; i++ )          // Csed
         {
            delta[j*cell+cellhyd+layer*cellsed+1+i]
               -= yt[j*cell+cellhyd+layer*cellsed+1+i];
         }
      }
   }

   // equations at the downstream boundary (grid point n-1):
   // ------------------------------------------------------

   j = NumGridPts()-1;
   switch ( method )
   {
      case KinWave:
         varsys->Reset();                                    // Q
         zneigh = y[j*cell+2] + 0.5*dx*sfs[j];
         Xvar->SetProgVar(&gridpts[j-1]);
         Zvar->SetProgVar(&zneigh);
         Aneigh = A->Value();
         wneigh = W->Value();
         delta[j*cell] = areas[j]*wneigh - Aneigh*widths[j];

         delta[j*cell+1] = - (y[j*cell]-y[(j-1)*cell])       // A
                             * 2.0*dxinv
                           + qlat[j-1]
                           - yt[j*cell+1];

         // add sediment exchange:
         if ( calcSed==TRUE && numsedlayers>0 )
         {
            // delta[j*cell+1] -= qadv[0][j-1];
         }
         // subtract time derivative:
         delta[j*cell+2] = y[j*cell+1]-areas[j];             // z0

         break;
      case DiffWave:
      {
         // delta[j*cell] = y[(j-1)*cell]+0.5*qlat[j-1]-y[j*cell];
         // correction 26.02.98:
         delta[j*cell] = (y[(j-1)*cell]-y[j*cell])*dxinv     // Q
                         +0.5*qlat[j-1];
         // add sediment exchange:
         if ( calcSed==TRUE && numsedlayers>0 )
         {
            // delta[j*cell] -= qadv[0][j-1];
         }

         REAL endlevel;                                      // A,z0
         switch ( endleveltype )
         {
            case NormalLevel:
               endlevel = y[numgrid*cell+1];
               break;
            case CriticalLevel:
               endlevel = y[numgrid*cell+2];
               break;
            case GivenLevel:
               endlevel = Zend->Value();
               break;
         }
         if ( endlevel < y[numgrid*cell+2] ) endlevel = y[numgrid*cell+2];
         const CARDINAL advoutconn = 0;
         if ( advoutlink[advoutconn] != 0 )
         {
            if ( advoutlink[advoutconn]->AdvInExZ() == TRUE )
            {
               if ( advoutlink[advoutconn]->AdvInZ(Y) > endlevel )
                  endlevel = advoutlink[advoutconn]->AdvInZ(Y);
            }
         }
         varsys->Reset();
         Xvar->SetProgVar(&gridpts[numgrid-1]);
         Zvar->SetProgVar(&endlevel);
         delta[j*cell+1] = y[j*cell+1]-A->Value();
         delta[j*cell+2] = y[j*cell+2]-endlevel;

         if ( endleveltype == NormalLevel )
         {
            varsys->Reset();                                 // normal level
            Xvar->SetProgVar(&gridpts[numgrid-1]);
            Zvar->SetProgVar(&y[numgrid*cell+1]);
            Qvar->SetProgVar(&y[j*cell]);
            a = A->Value();
            p = P->Value();
            w = W->Value();
            sf = Sf->Value();
//*cjob << "\na=" << a << "  p=" << p << "  w=" << w
// << "  Q=" << y[j*cell]  << "  sf=" << sf;
            varsys->Reset();
            zneigh = y[numgrid*cell+1]+0.5*dx*sf;
            Xvar->SetProgVar(&gridpts[numgrid-2]);
            Zvar->SetProgVar(&zneigh);
            Aneigh = A->Value();
            wneigh = W->Value();
            delta[numgrid*cell+1] = a*wneigh - Aneigh*w;
//*cjob << "\ny=" << y[numgrid*cell+1] << "  delta=" << delta[numgrid*cell+1];
         }
         else   // calculate normal level only if necessary
         {
            delta[numgrid*cell+1] = 1.0 - y[numgrid*cell+1];
         }

         varsys->Reset();                                    // critical level
         Xvar->SetProgVar(&gridpts[numgrid-1]);
         Zvar->SetProgVar(&y[numgrid*cell+2]);
         Qvar->SetProgVar(&y[j*cell]);
         a = A->Value();
         p = P->Value();
         w = W->Value();
         sf = Sf->Value();
         delta[numgrid*cell+2] = 1 - y[j*cell]*y[j*cell]*w/(gravit*a*a*a);

         break;
      }
   }

   for ( i=0; i<numvolstatevar; i++ )                        // VolStateVar
   {
      if ( withdispersion == TRUE )
      {
//       delta[j*cell+3+i] = y[(j-1)*cell+3+i]/y[(j-1)*cell+1]
//                           - y[j*cell+3+i]/y[j*cell+1];        // dC/dx = 0
         delta[j*cell+3+i] = 2*y[j*cell+3+i]/y[j*cell+1]
                             -3*y[(j-1)*cell+3+i]/y[(j-1)*cell+1]
                             +y[(j-2)*cell+3+i]/y[(j-2)*cell+1]; // d2C/dx2 = 0
      }
      else
      {
         delta[j*cell+3+i] += (y[(j-1)*cell]*y[(j-1)*cell+3+i]/y[(j-1)*cell+1]
                               -y[j*cell]*y[j*cell+3+i]/y[j*cell+1])
                              * 2.0*dxinv;
         // add sediment exchange:
         if ( calcSed==TRUE && numsedlayers>0 )
         {
            delta[j*cell+3+i]
               -= qex[0][j]*( y[j*cell+3+i]
                              /y[j*cell+1]
                             -y[j*cell+cellhyd+1+i]
                              /y[j*cell+cellhyd]);
            if ( qadv[0][j] >= 0 )
            {
               delta[j*cell+3+i]
                  -= qadv[0][j]*y[j*cell+3+i]
                                /y[j*cell+1];
            }
            else
            {
               delta[j*cell+3+i]
                  -= qadv[0][j]*y[j*cell+cellhyd+1+i]
                               /y[j*cell+cellhyd];
            }
         } 
         // subtract time derivative:
         delta[j*cell+3+i] -= yt[j*cell+3+i];
      }
   }

   for ( i=numvolstatevar; i<numdynstatevar; i++ )           // SurfStateVar
   {
      delta[j*cell+3+i] -= yt[j*cell+3+i];
   }
   /* numdynstatevar <= i < numstatevar */                   // EquStateVar

   // sediment submodel:
   if ( calcSed==TRUE && numsedlayers>0 )
   {
    // cross-sectional area:
      for ( layer=0; layer<numsedlayers; layer++ ) // time derivatives
      {
         delta[j*cell+cellhyd+layer*cellsed]                 // Aeff
            = krelax*(Ased[layer][j]*pors[layer][j]
                      -y[j*cell+cellhyd+layer*cellsed]);
      }
    // transport processes:
      // top layer:
      for ( i=0; i<numvolstatevar; i++ )                     // Csed
      {
         delta[j*cell+cellhyd+1+i]      // exch with water column
            += qex[0][j]
                  *( y[j*cell+3+i]
                     /y[j*cell+1]
                    -y[j*cell+cellhyd+1+i]
                     /y[j*cell+cellhyd]);
         if ( numsedlayers > 1 )        // exch with lower layer
         {
            delta[j*cell+cellhyd+1+i]
               += qex[1][j]
                     *( y[j*cell+cellhyd+cellsed+1+i]
                        /y[j*cell+cellhyd+cellsed]
                       -y[j*cell+cellhyd+1+i]
                        /y[j*cell+cellhyd]);
         }
      }

      // inner layers:
      if ( numsedlayers > 2 )
      {
         for ( layer=1; layer<numsedlayers-1; layer++ )
         {
            for ( i=0; i<numvolstatevar; i++ )               // Csed
            {
               delta[j*cell+cellhyd+layer*cellsed+1+i] // exch with upper layer
                  += qex[layer][j]
                        *( y[j*cell+cellhyd+(layer-1)*cellsed+1+i]
                           /y[j*cell+cellhyd+(layer-1)*cellsed]
                          -y[j*cell+cellhyd+layer*cellsed+1+i]
                           /y[j*cell+cellhyd+layer*cellsed]);
               delta[j*cell+cellhyd+layer*cellsed+1+i] // exch with lower layer
                  += qex[layer+1][j]
                        *( y[j*cell+cellhyd+(layer+1)*cellsed+1+i]
                           /y[j*cell+cellhyd+(layer+1)*cellsed]
                          -y[j*cell+cellhyd+layer*cellsed+1+i]
                           /y[j*cell+cellhyd+layer*cellsed]);
            }
         }
      }

      // bottom layer:
      if ( numsedlayers > 1 )
      {
         for ( i=0; i<numvolstatevar; i++ )                  // Csed
         {
            delta[j*cell+cellhyd+(numsedlayers-1)*cellsed+1+i] // exch with upper layer
               += qex[numsedlayers-1][j]
                     *( y[j*cell+cellhyd+(numsedlayers-2)*cellsed+1+i]
                        /y[j*cell+cellhyd+(numsedlayers-2)*cellsed]
                       -y[j*cell+cellhyd+(numsedlayers-1)*cellsed+1+i]
                        /y[j*cell+cellhyd+(numsedlayers-1)*cellsed]);
         }
      }
 
      for ( layer=0; layer<numsedlayers; layer++ ) // time derivatives
      {
         delta[j*cell+cellhyd+layer*cellsed]      // Aeff
            -= yt[j*cell+cellhyd+layer*cellsed];
         for ( i=0; i<numstatevar; i++ )          // Csed
         {
            delta[j*cell+cellhyd+layer*cellsed+1+i]
               -= yt[j*cell+cellhyd+layer*cellsed+1+i];
         }
      }
   }

   delta[numgrid*cell] = y[numgrid*cell]-es[numgrid-1];  // disp

   for ( i=0; i<numstatevar; i++ ) statevar[i]->ResetStateVar();
   delete [] conc;
   delete [] flux;
   delete [] areas;
   delete [] widths;
   delete [] sfs;
   delete [] es;
   delete [] qlat;
   if ( calcSed==TRUE && numsedlayers>0 )
   {
      for ( layer=0; layer<numsedlayers; layer++ )
      {
         delete [] Ased[layer];         
         delete [] pors[layer];         
         delete [] Qsed[layer];         
         delete [] Esed[layer];         
         delete [] qex[layer];         
         delete [] qadv[layer];         
      }
      delete [] Ased; Ased = 0;
      delete [] pors; pors = 0;
      delete [] Qsed; Qsed = 0;
      delete [] Esed; Esed = 0;
      delete [] qex;  qex  = 0;
      delete [] qadv; qadv = 0;
   }

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


BOOLEAN RIVCOMP::GridValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                           REAL* t_ptr, const REAL* Y,
                           CARDINAL zone, CARDINAL pt, AQVAR* var,
                           REAL& value)
{
   if ( Active() != TRUE ) return FALSE;
   SetGlobalState(varsys,calcnum_ptr,t_ptr,Y);
   CARDINAL cellhyd = 3+NumStateVar();
   CARDINAL cellsed = 1+NumStateVar();
   CARDINAL cell = cellhyd;
   if ( calcSed == TRUE ) cell += numsedlayers*cellsed;
   CARDINAL i;
   if ( pt >= NumGridPts() )          return FALSE;
   if ( varsys->Exist(var) == FALSE ) return FALSE;
   if ( AllowedVar(var) == FALSE )    return FALSE;
   if ( Active() == FALSE )           return FALSE;
   const REAL* y = &Y[ind_y];
   REAL cn = *calcnum_ptr; 
   REAL compindex = (REAL)cind;
   if ( zone > numsedlayers )         return FALSE;
   if ( calcSed==FALSE && zone!=0 )   return FALSE;
   REAL zoneindex = 0.0;
   varsys->SetProgVar(CalcNumber,&cn);
   varsys->SetProgVar(CompIndex,&compindex);
   varsys->SetProgVar(ZoneIndex,&zoneindex);
   varsys->SetProgVar(Time,t_ptr);
   REAL eps; varsys->SetProgVar(WaterFraction,&eps);
   Xvar = varsys->GetProgVar(SpaceX);
   Zvar = varsys->GetProgVar(WaterLevel);
   Avar = varsys->GetProgVar(CrossSection);
   Wvar = varsys->GetProgVar(Width);
   Pvar = varsys->GetProgVar(Perimeter);
   Qvar = varsys->GetProgVar(Discharge);
   if ( Xvar==0 || Zvar==0 || Avar==0 || Qvar==0 ) return FALSE;
   REAL Q; if ( Qvar != 0 ) Qvar->SetProgVar(&Q);
   REAL a; if ( Avar != 0 ) Avar->SetProgVar(&a);
   REAL p; if ( Pvar != 0 ) Pvar->SetProgVar(&p);
   REAL w; if ( Wvar != 0 ) Wvar->SetProgVar(&w);
   REAL sf; varsys->SetProgVar(FrictSlope,&sf);
   REAL* conc = new REAL[numstatevar];
   for ( i=0; i<numstatevar; i++ ) statevar[i]->SetStateVar(&conc[i]);
   Xvar->SetProgVar(&gridpts[pt]);
   Zvar->SetProgVar(&y[pt*cell+2]);
   varsys->Reset();
   if ( zone == 0 )
   {
      for ( i=0; i<numvolstatevar; i++ )
      {
         conc[i] = y[pt*cell+3+i]/y[pt*cell+1];
      }
      for ( i=numvolstatevar; i<numdynstatevar; i++ )
      {
         conc[i] = y[pt*cell+3+i];
      }
      for ( i=numdynstatevar; i<numstatevar; i++ )
      {
         conc[i] = y[pt*cell+3+i];
      }
      a = y[pt*cell+1];
      eps = 1;
      Q = y[pt*cell];
      p = P->Value();
      w = W->Value();
      sf = Sf->Value();
   }
   else
   {
      zoneindex = sedlayers[zone-1]->ZoneIndex();
      for ( i=0; i<numvolstatevar; i++ )
      {
         conc[i] = y[pt*cell+cellhyd+(zone-1)*cellsed+1+i]
                  /y[pt*cell+cellhyd+(zone-1)*cellsed];
      }
      for ( i=numvolstatevar; i<numdynstatevar; i++ )
      {
         conc[i] = y[pt*cell+cellhyd+(zone-1)*cellsed+1+i];
      }
      for ( i=numdynstatevar; i<numstatevar; i++ )
      {
         conc[i] = y[pt*cell+cellhyd+(zone-1)*cellsed+1+i];
      }
      a = sedlayers[zone-1]->area->Value();
      eps = sedlayers[zone-1]->porosity->Value();
      Q = sedlayers[zone-1]->discharge->Value();
      p = P->Value();
      w = W->Value();
      sf = 0;
   }
   value = var->Value();

   for ( i=0; i<numstatevar; i++ ) statevar[i]->ResetStateVar();
   delete conc;
   return TRUE;
}


BOOLEAN RIVCOMP::SpaceValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
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
               FatalError("RIVCOMP::Value: index negative");
         }
      }
      else
      {
         while ( x > gridpts[i+1] )
         {
            i++;
            if ( i >= numgrid-1 )
               FatalError("RIVCOMP::Value: index too large");
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
               FatalError("RIVCOMP::Value: index negative");
         }
      }
      else
      {
         while ( x < gridpts[i+1] )
         {
            i++;
            if ( i >= numgrid-1 )
               FatalError("RIVCOMP::Value: index too large");
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


CARDINAL RIVCOMP::InitZone(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("RIVCOMP::InitZone: Illegal index");
   return initzone[index];
}


AQVAR* RIVCOMP::InitVar(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("RIVCOMP::InitVar: Illegal index");
   return initvar[index];
}


const FORMVAR* RIVCOMP::InitVal(CARDINAL index) const
{
   if ( index >= numinit )
      FatalError("RIVCOMP::InitVal: Illegal index");
   return initval[index];
}


BOOLEAN RIVCOMP::AddInitCond(const VARLIST* varlist, CARDINAL zone,
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


BOOLEAN RIVCOMP::ReplaceInitCond(const VARLIST* varlist, CARDINAL zone,
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


BOOLEAN RIVCOMP::DeleteInitCond(CARDINAL pos)
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


BOOLEAN RIVCOMP::Inflow(const VARLIST* varlist,
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


AQVAR* RIVCOMP::InputVar(CARDINAL index) const
{
   if ( index >= numinput )
      FatalError("RIVCOMP::InputVar: Illegal index");
   return inputvar[index];
}


const FORMVAR* RIVCOMP::InputFlux(CARDINAL index) const
{
   if ( index >= numinput )
      FatalError("RIVCOMP::InputFlux: Illegal index");
   return inputflux[index];
}


BOOLEAN RIVCOMP::AddInput(const VARLIST* varlist, AQVAR* var,
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


BOOLEAN RIVCOMP::ReplaceInput(const VARLIST* varlist, AQVAR* var,
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


BOOLEAN RIVCOMP::DeleteInput(CARDINAL pos)
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


RIVSEDLAYER* RIVCOMP::SedLayer(CARDINAL index)
{
   if ( index > numsedlayers )
      FatalError("RIVCOMP::RIVSEDLAYER: illegal index");
   return sedlayers[index];
}


BOOLEAN RIVCOMP::AddSedLayer(RIVSEDLAYER* layer, CARDINAL pos)
{
   CARDINAL i;
   if ( InList() == TRUE )          return FALSE;
   if ( pos > numsedlayers ) pos = numsedlayers;

   RIVSEDLAYER** newsedlayers = new RIVSEDLAYER*[numsedlayers+1];
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


BOOLEAN RIVCOMP::ReplaceSedLayer(RIVSEDLAYER* layer, CARDINAL pos)
{
   if ( InList() == TRUE )    return FALSE;
   if ( pos >= numsedlayers ) return FALSE;
   delete sedlayers[pos];
   sedlayers[pos] = layer;
   CalcArg();
   return TRUE;
}


BOOLEAN RIVCOMP::DeleteSedLayer(CARDINAL pos)
{
   CARDINAL i;
   if ( pos >= numsedlayers ) return FALSE;

   RIVSEDLAYER** newsedlayers = new RIVSEDLAYER*[numsedlayers-1];

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


void RIVCOMP::CalcArg()
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
   if ( P != 0 )    numvararg += P->NumVarArg();
   if ( W != 0 )    numvararg += W->NumVarArg();
   if ( Sf != 0 )   numvararg += Sf->NumVarArg();
   if ( E != 0 )    numvararg += E->NumVarArg();
   for ( i=0; i<numsedlayers; i++ )
   {
      numvararg += sedlayers[i]->NumVarArg();
   }
   if ( Zend != 0 ) numvararg += Zend->NumVarArg();
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
      if ( P != 0 )
      {
         for ( i=0; i<P->NumVarArg(); i++ )
         {
            newarg[k] = P->VarArg(i); k++;
         }
      }
      if ( W != 0 )
      {
         for ( i=0; i<W->NumVarArg(); i++ )
         {
            newarg[k] = W->VarArg(i); k++;
         }
      }
      if ( Sf != 0 )
      {
         for ( i=0; i<Sf->NumVarArg(); i++ )
         {
            newarg[k] = Sf->VarArg(i); k++;
         }
      }
      if ( E != 0 )
      {
         for ( i=0; i<E->NumVarArg(); i++ )
         {
            newarg[k] = E->VarArg(i); k++;
         }
      }
      for ( i=0; i<numsedlayers; i++ )
      {
         for ( j=0; j<sedlayers[i]->NumVarArg(); j++ )
         {
            newarg[k] = sedlayers[i]->VarArg(j); k++;
         }
      }
      if ( Zend != 0 )
      {
         for ( i=0; i<Zend->NumVarArg(); i++ )
         {
            newarg[k] = Zend->VarArg(i); k++;
         }
      }
      if ( k != numvararg )
         FatalError("RIVCOMP::CalcArg: inconsistent number of arguments");
      delete [] vararg; vararg = newarg;
   }
   return;
}


BOOLEAN RIVCOMP::SetGridPts(CARDINAL n)
{
   if ( n < 5 )        return FALSE;
   if ( n == numgrid ) return TRUE;
   numgrid = n;
   delete gridpts; gridpts = new REAL[numgrid];
   XStartEnd(Xs,Xe);
   return TRUE;
}


BOOLEAN RIVCOMP::NumGridPts(CARDINAL n)
{
   if ( InList() == TRUE ) return FALSE;
   return SetGridPts(n);
}


REAL RIVCOMP::GridPt(const REAL*, CARDINAL index)
{
   if ( index >= NumGridPts() )
      FatalError("RIVCOMP::GridPt: illegal index");
   return gridpts[index];
}


CARDINAL RIVCOMP::NumZoneGridPts(CARDINAL zone)
{
   if ( zone > numsedlayers ) FatalError("RIVCOMP::NumZoneGridPts: illegal zone");
   return NumGridPts();
}


REAL RIVCOMP::ZoneGridPt(const REAL* Y, CARDINAL zone,
                         CARDINAL index)
{
   if ( zone > numsedlayers ) FatalError("RIVCOMP::ZoneGridPt: illegal zone");
   return GridPt(Y,index);
}


void RIVCOMP::CalcSed(BOOLEAN b)
{
   if ( b == TRUE )
   {
      calcSed = TRUE;
   }
   else
   {
      calcSed = FALSE;
   }
   return;
}


BOOLEAN RIVCOMP::XStartEnd(REAL xs, REAL xe)
{
   if ( xs == xe ) return FALSE;
   Xs = xs; Xe = xe;
   REAL dx = (Xe-Xs)/(numgrid-2);
   gridpts[0] = Xs; gridpts[numgrid-1] = Xe;
   for ( CARDINAL i=1; i<numgrid-1; i++ ) gridpts[i] = Xs + (i-0.5)*dx;
   return TRUE;
}


BOOLEAN RIVCOMP::Gravit(REAL g)
{
   if ( g <= 0.0 ) return FALSE;
   gravit = g;
   return TRUE;
}


BOOLEAN RIVCOMP::CrossSect(const char* inpline,
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

   // A has to depend on WaterLevel:

   if ( formvar->ArgProgVarType(WaterLevel) == FALSE )
   {
      delete formvar; return FALSE;
   }

   delete A; A = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN RIVCOMP::Peri(const char* inpline,
                      const VARLIST* varlist,
                      char* errorline,
                      CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("P");
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

   delete P; P = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN RIVCOMP::Wi(const char* inpline,
                    const VARLIST* varlist,
                    char* errorline,
                    CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("W");
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

   delete W; W = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN RIVCOMP::FrictionSlope(const char* inpline,
                               const VARLIST* varlist,
                               char* errorline,
                               CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("Sf");
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

   // Sf has to depend on Discharge:

   if ( formvar->ArgProgVarType(Discharge) == FALSE )
   {
      delete formvar; return FALSE;
   }

   // Sf has to depend on CrossSection:

   if ( formvar->ArgProgVarType(CrossSection) == FALSE )
   {
      delete formvar; return FALSE;
   }
   delete Sf; Sf = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN RIVCOMP::Dispersion(const char* inpline,
                            const VARLIST* varlist,
                            char* errorline,
                            CARDINAL& numparseerrors)
{
   numparseerrors = 0;
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


BOOLEAN RIVCOMP::WithDispersion(BOOLEAN b)
{
   if ( b==TRUE && E==0 ) return FALSE;
   withdispersion = b;
   return TRUE;
}


BOOLEAN RIVCOMP::EndLevel(const char* inpline,
                          const VARLIST* varlist,
                          char* errorline,
                          CARDINAL& numparseerrors)
{
   numparseerrors = 0;
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' )
   {
      if ( endleveltype != GivenLevel )
      {
         delete Zend; Zend = 0;
      }
      return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("Zend");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;
      if ( endleveltype != GivenLevel )
      {
         delete Zend; Zend = 0;
      }
      return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;
      if ( endleveltype != GivenLevel )
      {
         delete Zend; Zend = 0;
      }
      return FALSE;
   }

   delete Zend; Zend = formvar;
   CalcArg();
   return TRUE;
}


BOOLEAN RIVCOMP::EndLevelType(RIVCOMPLEVEL type)
{
   if ( type == GivenLevel )
   {
      if ( Zend == 0 ) return FALSE;
   }
   endleveltype = type;
   return TRUE;
}


BOOLEAN RIVCOMP::Method(RIVCOMPMETH type)
{
   if ( InList() == TRUE ) return FALSE;
   method = type;
   return TRUE;
}


CARDINAL RIVCOMP::NumZones() const
{
   if ( calcSed == FALSE ) return 1;
   return 1+numsedlayers;
}


const char* RIVCOMP::ZoneName(CARDINAL index) const
{
   char Buf[BufSize];
   switch ( index )
   {
      case 0:
         return ini.T(8);                // RivZoneWater
      default:
         strcpy(namebuffer,ini.T(265));  // RivZoneSed
         NumToString(index,Buf,BufSize);
         strcat(namebuffer,Buf);
         return namebuffer;
   }
   return 0;
}


const char* RIVCOMP::AdvInName(CARDINAL conn) const
{
   if ( conn > 0 ) FatalError("RIVCOMP::AdvInName: Illegal connection");
   return ini.T(39);
}


const char* RIVCOMP::AdvOutName(CARDINAL conn) const
{
   if ( conn > 0 ) FatalError("RIVCOMP::AdvOutName: Illegal connection");
   return ini.T(40);
}


const char* RIVCOMP::DiffName(CARDINAL) const
{
   FatalError("RIVCOMP::DiffName: Illegal connection");
   return 0;
}


BOOLEAN RIVCOMP::AdvInExZ(CARDINAL conn) const
{
   if ( conn > 0 ) FatalError("RIVCOMP::AdvInExZ: Illegal connection");
   if ( Active() == FALSE ) return FALSE;
   return TRUE;
}


REAL RIVCOMP::AdvInZ(const REAL* Y, CARDINAL conn) const
{
   if ( conn > 0 ) FatalError("RIVCOMP::AdvInZ: Illegal connection");
   if ( Active() == FALSE ) FatalError("RIVCOMP::AdvInZ: Illegal connection");
   const REAL* y = &Y[ind_y];
   return y[2];
}


REAL RIVCOMP::AdvOutQ(VARSYS* varsys, const REAL* Y, CARDINAL conn)
{
   if ( conn > 0 ) FatalError("RIVCOMP::AdvOutQ: Illegal connection");
   if ( Active() == FALSE ) return 0.0;
   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   CARDINAL cellhyd = 3+NumStateVar();
   CARDINAL cellsed = 1+NumStateVar();
   CARDINAL cell = cellhyd;
   if ( calcSed == TRUE ) cell += numsedlayers*cellsed;
   const REAL* y = &Y[ind_y];
   Qout = y[(NumGridPts()-1)*cell];
   varsys->SetProgVar(Discharge,&Qout);
   if ( numCout != numvolstatevar )
   {
      delete [] Cout; Cout = new REAL[numvolstatevar];
      numCout = numvolstatevar;
   }
   CARDINAL i;
   for ( i=0; i<numvolstatevar; i++ )
   {
      Cout[i] = y[(NumGridPts()-1)*cell+3+i]
                / y[(NumGridPts()-1)*cell+1];
      statevar[i]->SetStateVar(&Cout[i]);
   }
   for ( i=numvolstatevar; i<numstatevar; i++ )
   {
      statevar[i]->ResetStateVar();
   }
   return y[(NumGridPts()-1)*cell];
}


REAL RIVCOMP::AdvOutJ(VARSYS* varsys,
                      const REAL* Y,
                      CARDINAL conn,
                      const AQVAR* var)
{
   if ( conn > 0 ) FatalError("RIVCOMP::AdvOutJ: Illegal connection");
   if ( Active() == FALSE ) return 0.0;
   REAL compindex = (REAL)cind; varsys->SetProgVar(CompIndex,&compindex);
   CARDINAL cellhyd = 3+NumStateVar();
   CARDINAL cellsed = 1+NumStateVar();
   CARDINAL cell = cellhyd;
   if ( calcSed == TRUE ) cell += numsedlayers*cellsed;
   const REAL* y = &Y[ind_y];
   CARDINAL i;
   Qout = y[(NumGridPts()-1)*cell];
   varsys->SetProgVar(Discharge,&Qout);
   if ( numCout != numvolstatevar )
   {
      delete [] Cout; Cout = new REAL[numvolstatevar];
      numCout = numvolstatevar;
   }
   for ( i=0; i<numvolstatevar; i++ )
   {
      Cout[i] = y[(NumGridPts()-1)*cell+3+i]
                / y[(NumGridPts()-1)*cell+1];
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
         REAL flux =    y[(NumGridPts()-1)*cell+3+i]
                      / y[(NumGridPts()-1)*cell+1]
                      * y[(NumGridPts()-1)*cell];
         if ( withdispersion == TRUE )
         {
            flux -=   y[(numgrid-1)*cell+1]
                    * y[numgrid*cell]
                    * (  y[(numgrid-1)*cell+3+i]
                        /y[(numgrid-1)*cell+1]
                       - y[(numgrid-2)*cell+3+i]
                        /y[(numgrid-2)*cell+1]  )
                    / fabs(gridpts[numgrid-1]-gridpts[numgrid-2]);
         }
         return flux;
      }
   }
   return 0.0;
}


REAL RIVCOMP::DiffC(const REAL*, CARDINAL, const AQVAR*) const
{
   FatalError("RIVCOMP::DiffC: Illegal connection");
   return 0.0;
}


BOOLEAN RIVCOMP::AccQ(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccQ = relacc;
   absaccQ = absacc;
   return TRUE;
}


BOOLEAN RIVCOMP::AccA(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccA = relacc;
   absaccA = absacc;
   return TRUE;
}


BOOLEAN RIVCOMP::AccZ(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccZ = relacc;
   absaccZ = absacc;
   return TRUE;
}


BOOLEAN RIVCOMP::AccE(REAL relacc, REAL absacc)
{
   if ( relacc < 0 )             return FALSE;
   if ( absacc < 0 )             return FALSE;
   if ( relacc==0 && absacc==0 ) return FALSE;
   relaccE = relacc;
   absaccE = absacc;
   return TRUE;
}


JOBSTATUS RIVCOMP::Load(std::istream& in,
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
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Peri(Buffer,varlist,Buffer1,n) == FALSE )      return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Wi(Buffer,varlist,Buffer1,n) == FALSE )        return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( FrictionSlope(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadBoolean(in,&withdispersion) == FALSE )     return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Dispersion(Buffer,varlist,Buffer1,n) == FALSE )
            {
               if ( withdispersion==TRUE )                      return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            EndLevel(Buffer,varlist,Buffer1,n);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            CARDINAL type = 0;
            while ( type < NumRivLevel )
            {
               if ( strcmp(Buffer,RivCompLevelIdentifier[type]) == 0 ) break;
               type++;
            }
            switch (type)
            {
               case NormalLevel:
                  if ( EndLevelType(NormalLevel) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set NormalLevel");
                  break;
               case CriticalLevel:
                  if ( EndLevelType(CriticalLevel) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set CriticalLevel");
                  break;
               case GivenLevel:
                  if ( EndLevel() == 0 )                        return PROBLEM;
                  if ( EndLevelType(GivenLevel) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set GivenLevel");
                  break;
               default:
                  return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            type = 0;
            while ( type < NumRivLevel )
            {
               if ( strcmp(Buffer,RivCompMethIdentifier[type]) == 0 ) break;
               type++;
            }
            switch (type)
            {
               case KinWave:
                  if ( Method(KinWave) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set KinWave");
                  break;
               case DiffWave:
                  if ( Method(DiffWave) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set DiffWave");
                  break;
               default:
                  return PROBLEM;
            }
         }
         break;
      case 2: // aquavers=2: AQUASIM 1.1 with gravit
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
            REAL xs,xe,g;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( XStartEnd(xs,xe) == FALSE )                    return PROBLEM;
            if ( LoadNumber(in,&g) == FALSE )                   return PROBLEM;
            if ( Gravit(g) == FALSE )                           return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Peri(Buffer,varlist,Buffer1,n) == FALSE )      return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Wi(Buffer,varlist,Buffer1,n) == FALSE )        return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( FrictionSlope(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadBoolean(in,&withdispersion) == FALSE )     return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Dispersion(Buffer,varlist,Buffer1,n) == FALSE )
            {
               if ( withdispersion==TRUE )                      return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            EndLevel(Buffer,varlist,Buffer1,n);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            CARDINAL type = 0;
            while ( type < NumRivLevel )
            {
               if ( strcmp(Buffer,RivCompLevelIdentifier[type]) == 0 ) break;
               type++;
            }
            switch (type)
            {
               case NormalLevel:
                  if ( EndLevelType(NormalLevel) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set NormalLevel");
                  break;
               case CriticalLevel:
                  if ( EndLevelType(CriticalLevel) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set CriticalLevel");
                  break;
               case GivenLevel:
                  if ( EndLevel() == 0 )                        return PROBLEM;
                  if ( EndLevelType(GivenLevel) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set GivenLevel");
                  break;
               default:
                  return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            type = 0;
            while ( type < NumRivLevel )
            {
               if ( strcmp(Buffer,RivCompMethIdentifier[type]) == 0 ) break;
               type++;
            }
            switch (type)
            {
               case KinWave:
                  if ( Method(KinWave) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set KinWave");
                  break;
               case DiffWave:
                  if ( Method(DiffWave) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set DiffWave");
                  break;
               default:
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
            REAL xs,xe,g;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( XStartEnd(xs,xe) == FALSE )                    return PROBLEM;
            if ( LoadNumber(in,&g) == FALSE )                   return PROBLEM;
            if ( Gravit(g) == FALSE )                           return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Peri(Buffer,varlist,Buffer1,n) == FALSE )      return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Wi(Buffer,varlist,Buffer1,n) == FALSE )        return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( FrictionSlope(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadBoolean(in,&withdispersion) == FALSE )     return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Dispersion(Buffer,varlist,Buffer1,n) == FALSE )
            {
               if ( withdispersion==TRUE )                      return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            EndLevel(Buffer,varlist,Buffer1,n);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            CARDINAL type = 0;
            while ( type < NumRivLevel )
            {
               if ( strcmp(Buffer,RivCompLevelIdentifier[type]) == 0 ) break;
               type++;
            }
            switch (type)
            {
               case NormalLevel:
                  if ( EndLevelType(NormalLevel) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set NormalLevel");
                  break;
               case CriticalLevel:
                  if ( EndLevelType(CriticalLevel) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set CriticalLevel");
                  break;
               case GivenLevel:
                  if ( EndLevel() == 0 )                        return PROBLEM;
                  if ( EndLevelType(GivenLevel) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set GivenLevel");
                  break;
               default:
                  return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            type = 0;
            while ( type < NumRivLevel )
            {
               if ( strcmp(Buffer,RivCompMethIdentifier[type]) == 0 ) break;
               type++;
            }
            switch (type)
            {
               case KinWave:
                  if ( Method(KinWave) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set KinWave");
                  break;
               case DiffWave:
                  if ( Method(DiffWave) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set DiffWave");
                  break;
               default:
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
			if ( AccA(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccZ(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccE(relacc,absacc) == FALSE )                 return PROBLEM;
            REAL xs,xe,g;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( XStartEnd(xs,xe) == FALSE )                    return PROBLEM;
            if ( LoadNumber(in,&g) == FALSE )                   return PROBLEM;
            if ( Gravit(g) == FALSE )                           return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Peri(Buffer,varlist,Buffer1,n) == FALSE )      return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Wi(Buffer,varlist,Buffer1,n) == FALSE )        return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( FrictionSlope(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadBoolean(in,&withdispersion) == FALSE )     return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Dispersion(Buffer,varlist,Buffer1,n) == FALSE )
            {
               if ( withdispersion==TRUE )                      return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            EndLevel(Buffer,varlist,Buffer1,n);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            CARDINAL type = 0;
            while ( type < NumRivLevel )
            {
               if ( strcmp(Buffer,RivCompLevelIdentifier[type]) == 0 ) break;
               type++;
            }
            switch (type)
            {
               case NormalLevel:
                  if ( EndLevelType(NormalLevel) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set NormalLevel");
                  break;
               case CriticalLevel:
                  if ( EndLevelType(CriticalLevel) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set CriticalLevel");
                  break;
               case GivenLevel:
                  if ( EndLevel() == 0 )                        return PROBLEM;
                  if ( EndLevelType(GivenLevel) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set GivenLevel");
                  break;
               default:
                  return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            type = 0;
            while ( type < NumRivLevel )
            {
               if ( strcmp(Buffer,RivCompMethIdentifier[type]) == 0 ) break;
               type++;
            }
            switch (type)
            {
               case KinWave:
                  if ( Method(KinWave) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set KinWave");
                  break;
               case DiffWave:
                  if ( Method(DiffWave) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set DiffWave");
                  break;
               default:
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
			if ( AccA(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccZ(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccE(relacc,absacc) == FALSE )                 return PROBLEM;
            REAL xs,xe,g;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( XStartEnd(xs,xe) == FALSE )                    return PROBLEM;
            if ( LoadNumber(in,&g) == FALSE )                   return PROBLEM;
            if ( Gravit(g) == FALSE )                           return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Peri(Buffer,varlist,Buffer1,n) == FALSE )      return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Wi(Buffer,varlist,Buffer1,n) == FALSE )        return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( FrictionSlope(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadBoolean(in,&withdispersion) == FALSE )     return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Dispersion(Buffer,varlist,Buffer1,n) == FALSE )
            {
               if ( withdispersion==TRUE )                      return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            EndLevel(Buffer,varlist,Buffer1,n);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            CARDINAL type = 0;
            while ( type < NumRivLevel )
            {
               if ( strcmp(Buffer,RivCompLevelIdentifier[type]) == 0 ) break;
               type++;
            }
            switch (type)
            {
               case NormalLevel:
                  if ( EndLevelType(NormalLevel) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set NormalLevel");
                  break;
               case CriticalLevel:
                  if ( EndLevelType(CriticalLevel) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set CriticalLevel");
                  break;
               case GivenLevel:
                  if ( EndLevel() == 0 )                        return PROBLEM;
                  if ( EndLevelType(GivenLevel) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set GivenLevel");
                  break;
               default:
                  return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            type = 0;
            while ( type < NumRivLevel )
            {
               if ( strcmp(Buffer,RivCompMethIdentifier[type]) == 0 ) break;
               type++;
            }
            switch (type)
            {
               case KinWave:
                  if ( Method(KinWave) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set KinWave");
                  break;
               case DiffWave:
                  if ( Method(DiffWave) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set DiffWave");
                  break;
               default:
                  return PROBLEM;
            }
         }
         break;
      case 6: // aquavers=6: Dec 1998 sediment layers added
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
			if ( AccA(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccZ(relacc,absacc) == FALSE )                 return PROBLEM;
			if ( LoadNumber(in,&relacc) == FALSE )              return PROBLEM;
			if ( LoadNumber(in,&absacc) == FALSE )              return PROBLEM;
			if ( AccE(relacc,absacc) == FALSE )                 return PROBLEM;
            REAL xs,xe,g;
            if ( LoadNumber(in,&xs) == FALSE )                  return PROBLEM;
            if ( LoadNumber(in,&xe) == FALSE )                  return PROBLEM;
            if ( XStartEnd(xs,xe) == FALSE )                    return PROBLEM;
            if ( LoadNumber(in,&g) == FALSE )                   return PROBLEM;
            if ( Gravit(g) == FALSE )                           return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( CrossSect(Buffer,varlist,Buffer1,n) == FALSE ) return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Peri(Buffer,varlist,Buffer1,n) == FALSE )      return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Wi(Buffer,varlist,Buffer1,n) == FALSE )        return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( FrictionSlope(Buffer,varlist,Buffer1,n) == FALSE )
                                                                return PROBLEM;
            if ( LoadBoolean(in,&withdispersion) == FALSE )     return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            if ( Dispersion(Buffer,varlist,Buffer1,n) == FALSE )
            {
               if ( withdispersion==TRUE )                      return PROBLEM;
            }
            BOOLEAN b;
            if ( LoadBoolean(in,&b) == FALSE )                  return PROBLEM;
            CalcSed(b);
            if ( LoadStart(in) == FALSE )                       return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               if ( strcmp(Buffer,RivSedLayerIdentifier) != 0 ) return PROBLEM;
               RIVSEDLAYER* layer = new RIVSEDLAYER;
               if ( layer->Load(in,varlist) == PROBLEM )
               {
                  delete layer; return PROBLEM;
               }
               if ( AddSedLayer(layer) == PROBLEM )
               {
                  delete layer; return PROBLEM;
               }
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            EndLevel(Buffer,varlist,Buffer1,n);
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            CARDINAL type = 0;
            while ( type < NumRivLevel )
            {
               if ( strcmp(Buffer,RivCompLevelIdentifier[type]) == 0 ) break;
               type++;
            }
            switch (type)
            {
               case NormalLevel:
                  if ( EndLevelType(NormalLevel) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set NormalLevel");
                  break;
               case CriticalLevel:
                  if ( EndLevelType(CriticalLevel) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set CriticalLevel");
                  break;
               case GivenLevel:
                  if ( EndLevel() == 0 )                        return PROBLEM;
                  if ( EndLevelType(GivenLevel) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set GivenLevel");
                  break;
               default:
                  return PROBLEM;
            }
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                                return PROBLEM;
            type = 0;
            while ( type < NumRivLevel )
            {
               if ( strcmp(Buffer,RivCompMethIdentifier[type]) == 0 ) break;
               type++;
            }
            switch (type)
            {
               case KinWave:
                  if ( Method(KinWave) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set KinWave");
                  break;
               case DiffWave:
                  if ( Method(DiffWave) == FALSE )
                     FatalError("RIVCOMP::Load: Unable to set DiffWave");
                  break;
               default:
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


JOBSTATUS RIVCOMP::Save(std::ostream& out)
{
   CARDINAL i;
   char     Buffer[BufSize];

   INTEGER aquavers = 6;

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
   if ( SaveNumber(out,RelAccA()) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,AbsAccA()) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,RelAccZ()) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,AbsAccZ()) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,RelAccE()) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,AbsAccE()) == FALSE )                   return PROBLEM;
   if ( SaveNumber(out,Xs) == FALSE )                          return PROBLEM;
   if ( SaveNumber(out,Xe) == FALSE )                          return PROBLEM;
   if ( SaveNumber(out,gravit) == FALSE )                      return PROBLEM;
   if ( A->UnParse(Buffer,sizeof(Buffer)-1) != 0 )             return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( P->UnParse(Buffer,sizeof(Buffer)-1) != 0 )             return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( W->UnParse(Buffer,sizeof(Buffer)-1) != 0 )             return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( Sf->UnParse(Buffer,sizeof(Buffer)-1) != 0 )            return PROBLEM;
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
   if ( SaveBoolean(out,calcSed) == FALSE )                    return PROBLEM;
   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   for ( i=0; i<numsedlayers; i++ )
   {
      if ( SaveString(out,RivSedLayerIdentifier) == FALSE )    return PROBLEM;
      status = sedlayers[i]->Save(out);    if ( status != OK ) return status;
   }   
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;

   if ( Zend == 0 )
   {
      if ( EndLevelType() == GivenLevel )                      return PROBLEM;
      Buffer[0] = '\0';
   }
   else
   {      
      if ( Zend->UnParse(Buffer,sizeof(Buffer)-1) != 0 )       return PROBLEM;
   }
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;

   if ( EndLevelType() > NumRivLevel )
      FatalError("RIVCOMP::Save: Unknown EndLevel Type");
   if ( SaveString(out,RivCompLevelIdentifier[EndLevelType()]) == FALSE )
                                                               return PROBLEM;
   if ( Method() > NumRivMeth )
      FatalError("RIVCOMP::Save: Unknown Method");
   if ( SaveString(out,RivCompMethIdentifier[Method()]) == FALSE )
                                                               return PROBLEM;

   if ( SaveEnd(out) == FALSE )                                return PROBLEM;
   return OK;
}


JOBSTATUS RIVCOMP::Write(std::ostream& out, BOOLEAN sh)
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

   if ( Print(out,ini.T(174),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( NumInitCond() > 0 )
   {
      PrintIndent(pos4-1);
      if ( Print(out,ini.T(175),pos3) == FALSE )               return PROBLEM;
      if ( Print(out,"(") == FALSE )                           return PROBLEM;
      if ( Print(out,ini.T(176)) == FALSE )                    return PROBLEM;
      if ( Print(out,") : ") == FALSE )                        return PROBLEM;
      if ( Print(out,ini.T(177)) == FALSE )                    return PROBLEM;
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
   if ( Print(out,ini.T(170),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Inflow() != 0 )
   {
      if ( inflow->UnParse(Buffer,sizeof(Buffer)-1) != 0 )     return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;
   }

   if ( Print(out,ini.T(171),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( NumInput() > 0 )
   {
      PrintIndent(pos4-1);
      if ( Print(out,ini.T(172),pos3) == FALSE )               return PROBLEM;
      if ( Print(out," : ") == FALSE )                         return PROBLEM;
      if ( Print(out,ini.T(173)) == FALSE )                    return PROBLEM;
      for ( i=0; i<NumInput(); i++ )
      {
         if ( Print(out,InputVar(i)->Symbol(),pos3) == FALSE ) return PROBLEM;
         if ( Print(out," : ") == FALSE )                      return PROBLEM;
         if ( InputFlux(i)->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
         if ( Print(out,Buffer) == FALSE )                     return PROBLEM;
      }
   }

   if ( Print(out,ini.T(12),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( LatInflow() != 0 )
   {
      if ( latinflow->UnParse(Buffer,sizeof(Buffer)-1) != 0 )  return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;

      if ( NumLatInput() > 0 )
      {
         if ( Print(out,ini.T(13),pos1) == FALSE )             return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( Print(out,ini.T(14),pos3) == FALSE )             return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( Print(out,ini.T(15),pos2) == FALSE )             return PROBLEM;
         for ( CARDINAL i=0; i<NumLatInput(); i++ )
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

   if ( Print(out,ini.T(28),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,XStart(),pos2) == FALSE )                    return PROBLEM;

   if ( Print(out,ini.T(29),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,XEnd(),pos2) == FALSE )                      return PROBLEM;

   if ( Print(out,ini.T(145),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,Gravit(),pos2) == FALSE )                    return PROBLEM;

   if ( Print(out,ini.T(30),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( A->UnParse(Buffer,sizeof(Buffer)-1) != 0 )             return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                      return PROBLEM;

   if ( Print(out,ini.T(31),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( P->UnParse(Buffer,sizeof(Buffer)-1) != 0 )             return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                      return PROBLEM;

   if ( Print(out,ini.T(32),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( W->UnParse(Buffer,sizeof(Buffer)-1) != 0 )             return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                      return PROBLEM;

   if ( Print(out,ini.T(33),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Sf->UnParse(Buffer,sizeof(Buffer)-1) != 0 )            return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                      return PROBLEM;

   if ( WithDispersion() == TRUE )
   {
      if ( Print(out,ini.T(44),pos1) == FALSE )                return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( E->UnParse(Buffer,sizeof(Buffer)-1) != 0 )          return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;
   }

   if ( Print(out,ini.T(75),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   switch ( EndLevelType() )
   {
      case NormalLevel:
         if ( Print(out,ini.T(76),pos2) == FALSE )             return PROBLEM;
         break;
      case CriticalLevel:
         if ( Print(out,ini.T(77),pos2) == FALSE )             return PROBLEM;
         break;
      case GivenLevel:
         if ( Zend->UnParse(Buffer,sizeof(Buffer)-1) != 0 )    return PROBLEM;
         if ( Print(out,Buffer,pos2) == FALSE )                return PROBLEM;
         break;
      default:
         return PROBLEM;
   }

   if ( Print(out,ini.T(56),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   switch ( Method() )
   {
      case KinWave:
         if ( Print(out,ini.T(53),pos2) == FALSE )             return PROBLEM;
         break;
      case DiffWave:
         if ( Print(out,ini.T(54),pos2) == FALSE )             return PROBLEM;
         break;
      default:
         return PROBLEM;
   }

   if ( CalcSed() == TRUE )
   {
      for ( i=0; i<numsedlayers; i++ )
      {
         if ( Print(out,ini.T(264),pos1) == FALSE )            return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         status = sedlayers[i]->Write(out);
                                           if ( status != OK ) return status;
      }
   }

   if ( Print(out,ini.T(27),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,NumGridPts(),pos2) == FALSE )                return PROBLEM;
   if ( Print(out," (") == FALSE )                             return PROBLEM;
   if ( HighRes() == FALSE )
   {
      if ( Print(out,ini.T(38)) == FALSE )                     return PROBLEM;
   }
   else
   {
      if ( Print(out,ini.T(35)) == FALSE )                     return PROBLEM;
   }
   if ( Print(out,")") == FALSE )                              return PROBLEM;
   if ( Print(out,ini.T(235),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( Print(out,ini.T(236),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,RelAccQ(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(237),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,AbsAccQ(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(238),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,RelAccA(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(239),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,AbsAccA(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(240),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,RelAccZ(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(241),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,AbsAccZ(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(242),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,RelAccE(),pos2) == FALSE )                   return PROBLEM;
   if ( Print(out,ini.T(243),pos1+2) == FALSE )                return PROBLEM;
   if ( Print(out,": ") == FALSE )                             return PROBLEM;
   if ( Print(out,AbsAccE(),pos2) == FALSE )                   return PROBLEM;

   return OK;
}

//////////////////////////////////////////////////////////////////////////////
