////////////////////////////////   aqlink.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    15.10.92    Peter Reichert
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "aqlink.h"

//////////////////////////////////////////////////////////////////////////////

const CARDINAL BufSize = 1024;

// Identifiers for BIF::Save and BIF::Load
// (compare this constant with enum BIFTYPE from aqlink.h)

                                                      //  enum BIFTYPE:
const char* const BifTypeIdentifier[] = {
                                           "WATER",   //  WithWater
                                           "GIVEN"    //  AsGiven
                                        };

const CARDINAL NumBifType = sizeof(BifTypeIdentifier)/sizeof(char*);

REAL linkindex;

//////////////////////////////////////////////////////////////////////////////

const char* BifIdentifier = "BIF";

void BIF::init()
{
   compartment = 0;
   connection  = 0;
   waterflow = 0;
   biftype = WithWater;
   numflux = 0; fluxvar = 0; fluxval = 0;
   numvararg = 0; vararg = 0;
   return;
}


BIF::BIF()
{
   init();
}


BIF::BIF(const BIF& bif) : SYMBOL(bif)
{
   init();
   compartment = bif.compartment;
   connection  = bif.connection;
   waterflow = new FORMVAR(bif.waterflow);
   biftype = bif.biftype;
   if ( bif.numvararg > 0 )
   {
      numvararg = bif.numvararg;
      vararg = new AQVAR*[numvararg];
      for ( CARDINAL i=0; i<numvararg; i++ )
      {
         vararg[i] = bif.vararg[i];
      }
   }
   if ( bif.numflux > 0 )
   {
      numflux = bif.numflux;
      fluxvar = new AQVAR*[numflux];
      fluxval = new FORMVAR*[numflux];
      for ( CARDINAL i=0; i<numflux; i++ )
      {
         fluxvar[i]  = bif.fluxvar[i];
         fluxval[i] = new FORMVAR(bif.fluxval[i]);
      }
   }
}


BIF::BIF(const BIF* bif) : SYMBOL(bif)
{
   init();
   if ( bif != 0 )
   {
      compartment = bif->compartment;
      connection  = bif->connection;
      waterflow = new FORMVAR(bif->waterflow);
      biftype = bif->biftype;
      if ( bif->numvararg > 0 )
      {
         numvararg = bif->numvararg;
         vararg = new AQVAR*[numvararg];
         for ( CARDINAL i=0; i<numvararg; i++ )
         {
            vararg[i] = bif->vararg[i];
         }
      }
      if ( bif->numflux > 0 )
      {
         numflux = bif->numflux;
         fluxvar = new AQVAR*[numflux];
         fluxval = new FORMVAR*[numflux];
         for ( CARDINAL i=0; i<numflux; i++ )
         {
            fluxvar[i]  = bif->fluxvar[i];
            fluxval[i] = new FORMVAR(bif->fluxval[i]);
         }
      }
   }
}


BIF::~BIF()
{
   CARDINAL i;
   delete waterflow; waterflow = 0;
   delete fluxvar; fluxvar = 0;
   for ( i=0; i<numflux; i++ ) delete fluxval[i];
   delete fluxval; fluxval = 0;
   numflux = 0;
   delete vararg; vararg = 0; numvararg = 0;
}


BOOLEAN BIF::AllowedProgVarType(PROGVARTYPE type) const
{
   if ( type == CalcNumber ) return TRUE;
   if ( type == LinkIndex )  return TRUE;
   if ( type == Time )       return TRUE;
   if ( type == Discharge )  return TRUE;
   return FALSE;
}


BOOLEAN BIF::AllowedVar(const AQVAR* var) const
{
   if ( var->Type() == ProgVar )
   {
      PROGVAR* pv = (PROGVAR*)var;
      if ( AllowedProgVarType(pv->ProgVarType()) == FALSE ) return FALSE;
   }
   for ( CARDINAL i=0; i<var->NumVarArg(); i++ ) // check should go
   {                                             // deeper than one level
      if ( var->Type() != ProbeVar )
      {
         if ( AllowedVar(var->VarArg(i)) == FALSE ) return FALSE;
      }
   }
   return TRUE;
}


BOOLEAN BIF::AllowedReplaceVar(const AQVAR* oldvar,
                               const AQVAR* newvar) const
{
   if ( Arg(oldvar) == TRUE )
   {
      if ( AllowedVar(newvar) == FALSE ) return FALSE;
   }
   return TRUE;
}


BOOLEAN BIF::AllowedExchangeVar(const AQVAR* var1,
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


BOOLEAN BIF::AllowedReplaceComp(const AQCOMP* oldcomp,
                                const AQCOMP* newcomp) const
{
   if ( oldcomp == 0 ) return TRUE;
   if ( oldcomp == compartment )
   {
      if ( connection >= newcomp->NumAdvInConn() ) return FALSE;
   }
   return TRUE;
}


BOOLEAN BIF::SetBifComp(AQCOMP* com, CARDINAL conn)
{
   if ( InList() == TRUE ) return FALSE;
   if ( com == 0 )
   {
      compartment = 0;
      connection  = 0;
   }
   else
   {
      if ( conn >= com->NumAdvInConn() ) return FALSE;
      compartment = com;
      connection  = conn;
   }
   return TRUE;
}


BOOLEAN BIF::WaterFlow(const char* inpline,
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


AQVAR* BIF::FluxVar(CARDINAL index) const
{
   if ( index >= numflux )
      FatalError("BIF::FluxVar: Illegal index");
   return fluxvar[index];
}


const FORMVAR* BIF::FluxVal(CARDINAL index) const
{
   if ( index >= numflux )
      FatalError("BIF::FluxVal: Illegal index");
   return fluxval[index];
}


BOOLEAN BIF::AddFlux(const VARLIST* varlist, AQVAR* var,
                     const char* inpline, char* errorline,
                     CARDINAL& numparseerrors, CARDINAL pos)
{
   CARDINAL i;
   numparseerrors = 0;
   if ( InList() == TRUE )             return FALSE;
   if ( inpline == 0 )                 return FALSE;
   if ( inpline[0] == '\0' )           return FALSE;
   if ( varlist->Exist(var) == FALSE ) return FALSE;
   if ( pos > numflux ) pos = numflux;
   for ( i=0; i<numflux; i++ )
   {
      if ( fluxvar[i] == var )         return FALSE;
   }

   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("BifFlux");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                  return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                  return FALSE;
   }
   AQVAR**   newvar = new AQVAR*[numflux+1];
   FORMVAR** newval = new FORMVAR*[numflux+1];
   for ( i=0; i<pos; i++ )
   {
      newvar[i] = fluxvar[i];
      newval[i] = fluxval[i];
   }
   newvar[pos] = var;
   newval[pos] = formvar;
   for ( i=pos; i<numflux; i++ )
   {
      newvar[i+1] = fluxvar[i];
      newval[i+1] = fluxval[i];
   }
   delete fluxvar; fluxvar = newvar;
   delete fluxval; fluxval = newval;
   numflux++;
   CalcVarArg();
   return TRUE;
}


BOOLEAN BIF::ReplaceFlux(const VARLIST* varlist, AQVAR* var,
                         const char* inpline, char* errorline,
                         CARDINAL& numparseerrors, CARDINAL pos)
{
   CARDINAL i;
   numparseerrors = 0;
   if ( InList() == TRUE )             return FALSE;
   if ( inpline == 0 )                 return FALSE;
   if ( inpline[0] == '\0' )           return FALSE;
   if ( varlist->Exist(var) == FALSE ) return FALSE;
   if ( pos >= numflux )               return FALSE;
   for ( i=0; i<numflux; i++ )
   {
      if ( i!=pos && fluxvar[i]==var ) return FALSE;
   }
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("BifFlux");
   numparseerrors = formvar->Parse(inpline,varlist,errorline);
   if ( numparseerrors != 0 )
   {
      delete formvar;                  return FALSE;
   }
   if ( AllowedVar(formvar) == FALSE )
   {
      delete formvar;                  return FALSE;
   }
   delete fluxval[pos];
   fluxvar[pos] = var;
   fluxval[pos] = formvar;
   CalcVarArg();
   return TRUE;
}


BOOLEAN BIF::DeleteFlux(CARDINAL pos)
{
   CARDINAL i;
   if ( pos >= numflux ) return FALSE;
   AQVAR**   newvar = new AQVAR*[numflux-1];
   FORMVAR** newval = new FORMVAR*[numflux-1];
   for ( i=0; i<pos; i++ )
   {
      newvar[i] = fluxvar[i];
      newval[i] = fluxval[i];
   }
   for ( i=pos+1; i<numflux; i++ )
   {
      newvar[i-1] = fluxvar[i];
      newval[i-1] = fluxval[i];
   }
   delete fluxval[pos];
   delete fluxvar; fluxvar = newvar;
   delete fluxval; fluxval = newval;
   numflux--;
   CalcVarArg();
   return TRUE;
}


REAL BIF::AdvOutQ()
{
   if ( waterflow == 0 ) return 0.0;
   return waterflow->Value();
}


REAL BIF::AdvOutJ(REAL Q0, REAL J0, const AQVAR* var)
{
   if ( biftype == WithWater )
   {
      if ( Q0 > 0 )
      {
         REAL Q = AdvOutQ();
         return Q/Q0 * J0;
      }
   }
   else
   {
      for ( CARDINAL i=0; i<numflux; i++ )
      {
         if ( var == fluxvar[i] )
         {
            return fluxval[i]->Value();
         }
      }
   }
   return 0.0;
}


AQVAR* BIF::VarArg(CARDINAL index) const
{
   if ( index >= numvararg )
   {
      FatalError("BIF::VarArg: index too large");
   }
   else
   {
      return vararg[index];
   }
   return 0;
}


BOOLEAN BIF::Arg(const AQVAR* var) const
{
   for ( CARDINAL i=0; i<numvararg; i++ )
   {
      if ( vararg[i] == var )              return TRUE;
      if ( (vararg[i])->Arg(var) == TRUE ) return TRUE;
   }
   return FALSE;
}


void BIF::ReplVar(AQVAR* oldvar, AQVAR* newvar)
{
   CARDINAL i;
   if ( waterflow != 0 ) waterflow->ReplaceVarArg(oldvar,newvar);
   {
      for ( i=0; i<numflux; i++ )
      {
         if ( fluxvar[i] == oldvar ) fluxvar[i] = newvar;
         fluxval[i]->ReplaceVarArg(oldvar,newvar);
      }
   }
   CalcVarArg();
}


BOOLEAN BIF::ReplaceVar(AQVAR* oldvar, AQVAR* newvar)
{
   if ( InList() == TRUE ) return FALSE;
   ReplVar(oldvar,newvar); return TRUE;
}


void BIF::ExchVar(AQVAR* var1, AQVAR* var2)
{
   CARDINAL i;
   AQVAR* v = new CONSTVAR;
   if ( waterflow != 0 )
   {
      waterflow->ReplaceVarArg(var1,v);
      waterflow->ReplaceVarArg(var2,var1);
      waterflow->ReplaceVarArg(v,var2);
   }
   for ( i=0; i<numflux; i++ )
   {
      if ( fluxvar[i] == var1 )
      {
         fluxvar[i] = var2;
      }
      else
      {
         if ( fluxvar[i] == var2 ) fluxvar[i] = var1;
      }
      fluxval[i]->ReplaceVarArg(var1,v);
      fluxval[i]->ReplaceVarArg(var2,var1);
      fluxval[i]->ReplaceVarArg(v,var2);
   }
   delete v;
   CalcVarArg();
}


BOOLEAN BIF::ExchangeVar(AQVAR* var1, AQVAR* var2)
{
   if ( InList() == TRUE ) return FALSE;
   ExchVar(var1,var2);    return TRUE;
}


void BIF::ReplComp(AQCOMP* oldcom, AQCOMP* newcom)
{
   if ( compartment == oldcom ) compartment = newcom;
   return;
}


BOOLEAN BIF::ReplaceComp(AQCOMP* oldcom, AQCOMP* newcom)
{
   if ( InList() == TRUE ) return FALSE;
   ReplComp(oldcom,newcom); return TRUE;
}


void BIF::CalcVarArg()
{
   CARDINAL i,j,k;
   CARDINAL num = 0;
   if ( waterflow != 0 ) num += waterflow->NumVarArg();
   for ( i=0; i<numflux; i++ ) num += 1+fluxval[i]->NumVarArg();
   AQVAR** newarg = new AQVAR*[num];
   k = 0;
   if ( waterflow != 0 )
   {
      for ( i=0; i<waterflow->NumVarArg(); i++ )
      {
         newarg[k] = waterflow->VarArg(i); k++;
      }
   }
   for ( i=0; i<numflux; i++ )
   {
      newarg[k] = fluxvar[i]; k++;
      for ( j=0; j<(fluxval[i]->NumVarArg()); j++ )
      {
          newarg[k] = fluxval[i]->VarArg(j); k++;
      }
   }
   numvararg = num;
   delete vararg;
   vararg = newarg;
   if ( k != num )
      FatalError("BIF::CalcVarArg: Inconsistent number of arguments");
   return;
}


JOBSTATUS BIF::Load(std::istream& in,
                    const VARLIST* varlist,
                    const COMPLIST* complist)
{
   CARDINAL numerr = 0;
   char Buf[BufSize]; char Buf1[BufSize];
   if ( InList() == TRUE )                                 return PROBLEM;
   if ( LoadString(in,Buf,sizeof(Buf)) == FALSE )          return PROBLEM;
   if ( Symbol(Buf) == FALSE )                             return PROBLEM;
   if ( LoadString(in,Buf,sizeof(Buf)) == FALSE )          return PROBLEM;
   Description(Buf);

   AQCOMP* com = 0; CARDINAL conn = 0;
   if ( LoadString(in,Buf,sizeof(Buf)) == FALSE )          return PROBLEM;
   com = complist->Get(Buf);
   if ( LoadNumber(in,&conn) == FALSE )                    return PROBLEM;
   if ( SetBifComp(com,conn) == FALSE )                    return PROBLEM;

   if ( LoadString(in,Buf,sizeof(Buf)) == FALSE )          return PROBLEM;
   if ( WaterFlow(Buf,varlist,Buf1,numerr) == FALSE )      return PROBLEM;
   if ( LoadString(in,Buf,sizeof(Buf)) == FALSE )          return PROBLEM;
   CARDINAL type = 0;
   while ( type < NumBifType )
   {
      if ( strcmp(Buf,BifTypeIdentifier[type]) == 0 ) break;
      type ++;
   }
   switch ( type )
   {
      case WithWater:
         BifType(WithWater);
         break;
      case AsGiven:
         BifType(AsGiven);
         break;
      default:
         return PROBLEM;
   }
   if ( LoadStart(in) == FALSE )                           return PROBLEM;
   while ( LoadString(in,Buf,sizeof(Buf)) == TRUE )
   {
      AQVAR* var = varlist->Get(Buf);        if ( var==0 ) return PROBLEM;
      if ( LoadString(in,Buf,sizeof(Buf)) == FALSE )       return PROBLEM;
      if ( AddFlux(varlist,var,Buf,Buf1,numerr) == FALSE ) return PROBLEM;
   }
   Changed();
   return OK;
}


JOBSTATUS BIF::Save(std::ostream& out)
{
   char Buffer[BufSize];
   const INIT* ini = AQLINK::Ini();
   if ( SaveString(out,Symbol()) == FALSE )                  return PROBLEM;
   if ( SaveString(out,Description()) == FALSE )             return PROBLEM;

   if ( compartment == 0 )
   {
      if ( SaveString(out,"") == FALSE )                     return PROBLEM;
   }
   else
   {
      if ( SaveString(out,compartment->Symbol()) == FALSE )  return PROBLEM;
   }
   if ( SaveNumber(out,connection) == FALSE )                return PROBLEM;

   if ( waterflow->UnParse(Buffer,sizeof(Buffer)-1) != 0 )   return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                    return PROBLEM;
   if ( SaveString(out,BifTypeIdentifier[BifType()]) == FALSE )
                                                             return PROBLEM;

   if ( SaveStart(out) == FALSE )                            return PROBLEM;
   for ( CARDINAL i=0; i<numflux; i++ )
   {
      if ( SaveString(out,(fluxvar[i])->Symbol()) == FALSE ) return PROBLEM;
      if ( (fluxval[i])->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                             return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                 return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                              return PROBLEM;

   return FILEIO::Save(out);
}


JOBSTATUS BIF::Write(std::ostream& out, BOOLEAN)
{
   const INIT* ini = AQLINK::Ini();
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
         if ( Print(out,Description()) == FALSE )           return PROBLEM;
      }
   }

   if ( Print(out,ini->T(5),pos4) == FALSE )                return PROBLEM;
   if ( Print(out,":") == FALSE )                           return PROBLEM;
   if ( compartment != 0 )
   {
      if ( Print(out,compartment->Symbol(),pos2) == FALSE ) return PROBLEM;
      if ( Print(out,ini->T(6),pos4) == FALSE )             return PROBLEM;
      if ( Print(out,":") == FALSE )                        return PROBLEM;
      if ( Print(out,compartment->AdvInName(connection),pos2) == FALSE )
                                                            return PROBLEM;
   }

   if ( Print(out,ini->T(19),pos4) == FALSE )               return PROBLEM;
   if ( Print(out,":") == FALSE )                           return PROBLEM;
   if ( waterflow->UnParse(Buffer,sizeof(Buffer)-1) != 0 )  return PROBLEM;
   if ( Print(out,Buffer,pos2) == FALSE )                   return PROBLEM;
   if ( BifType() == AsGiven )
   {
      PrintIndent(pos6-5);
      if ( Print(out,ini->T(20),pos4) == FALSE )            return PROBLEM;
      if ( Print(out,":") == FALSE )                        return PROBLEM;
      if ( Print(out,ini->T(21),pos5) == FALSE )            return PROBLEM;
      if ( Print(out," : ") == FALSE )                      return PROBLEM;
      if ( Print(out,ini->T(22)) == FALSE )                 return PROBLEM;
      for ( CARDINAL i=0; i<NumFlux(); i++ )
      {
         if ( Print(out,(fluxvar[i])->Symbol(),pos5) == FALSE )
                                                            return PROBLEM;
         if ( Print(out," : ") == FALSE )                   return PROBLEM;
         if ( (fluxval[i])->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                            return PROBLEM;
         if ( Print(out,Buffer) == FALSE )                  return PROBLEM;
      }
   }
   return OK;
}


//////////////////////////////////////////////////////////////////////////////

INIT AQLINK::ini(initfile,"LINKSYS","link_txt_",10,10,10,50);


CARDINAL AQLINK::number = 0;


void AQLINK::init()
{
   numvararg  = 0;
   vararg     = 0;
   numcomparg = 0;
   comparg    = 0;
   lind       = 0;
   number++;
}


AQLINK::AQLINK()
{
   init();
   char Buf1[BufSize]; char Buf2[BufSize];
   strcpy(Buf1,ini.T(1));
   NumToString(number,Buf2,BufSize);
   strcat(Buf1,Buf2);
   if ( Symbol(Buf1) == FALSE )
      FatalError("AQLINK::AQLINK: illegal symbol");
   return;
}


AQLINK::AQLINK(const AQLINK& link) : SYMBOL(link)
{
   init();
   lind = link.lind;
   if ( link.numvararg > 0 )
   {
      numvararg = link.numvararg;
      vararg = new AQVAR*[numvararg];
      for ( CARDINAL i=0; i<numvararg; i++ )
      {
         vararg[i] = link.vararg[i];
      }
   }
   if ( link.numcomparg > 0 )
   {
      numcomparg = link.numcomparg;
      comparg = new AQCOMP*[numcomparg];
      for ( CARDINAL i=0; i<numcomparg; i++ )
      {
         comparg[i] = link.comparg[i];
      }
   }
}


AQLINK::AQLINK(const AQLINK* link) : SYMBOL(link)
{
   init();
   if ( link != 0 )
   {
      lind = link->lind;
      if ( link->numvararg > 0 )
      {
         numvararg = link->numvararg;
         vararg = new AQVAR*[numvararg];
         for ( CARDINAL i=0; i<numvararg; i++ )
         {
            vararg[i] = link->vararg[i];
         }
      }
      if ( link->numcomparg > 0 )
      {
         numcomparg = link->numcomparg;
         comparg = new AQCOMP*[numcomparg];
         for ( CARDINAL i=0; i<numcomparg; i++ )
         {
            comparg[i] = link->comparg[i];
         }
      }
   }
   else
   {
      char Buf1[BufSize]; char Buf2[BufSize];
      strcpy(Buf1,ini.T(1));
      NumToString(number,Buf2,BufSize);
      strcat(Buf1,Buf2);
      if ( Symbol(Buf1) == FALSE )
         FatalError("AQLINK::AQLINK: illegal symbol");
   }
}


AQLINK::~AQLINK()
{
   delete vararg;  vararg = 0;  numvararg = 0;
   delete comparg; comparg = 0; numcomparg = 0;
   if ( number == 0 )
      FatalError("AQLINK::~AQLINK: already all links destructed");
   number--;
}


BOOLEAN AQLINK::AllowedVar(const AQVAR* var) const
{
   if ( var->Type() == ProgVar )
   {
      PROGVAR* pv = (PROGVAR*)var;
      if ( AllowedProgVarType(pv->ProgVarType()) == FALSE ) return FALSE;
   }
   for ( CARDINAL i=0; i<var->NumVarArg(); i++ ) // check should go
   {                                             // deeper than one level
      if ( var->Type() != ProbeVar )
      {
         if ( AllowedVar(var->VarArg(i)) == FALSE ) return FALSE;
      }
   }
   return TRUE;
}


BOOLEAN AQLINK::AllowedReplaceVar(const AQVAR* oldvar,
                                  const AQVAR* newvar) const
{
   if ( Arg(oldvar) == TRUE )
   {
      if ( AllowedVar(newvar) == FALSE ) return FALSE;
   }
   return TRUE;
}


BOOLEAN AQLINK::AllowedExchangeVar(const AQVAR* var1,
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


BOOLEAN AQLINK::AllowedReplaceComp(const AQCOMP*, const AQCOMP*) const
{
   return TRUE;
}


AQVAR* AQLINK::VarArg(CARDINAL index) const
{
   if ( index >= numvararg )
   {
      FatalError("AQLINK::VarArg: index too large");
   }
   else
   {
      return vararg[index];
   }
   return 0;
}


BOOLEAN AQLINK::Arg(const AQVAR* var) const
{
   for ( CARDINAL i=0; i<numvararg; i++ )
   {
      if ( vararg[i] == var )              return TRUE;
      if ( (vararg[i])->Arg(var) == TRUE ) return TRUE;
   }
   return FALSE;
}


AQCOMP* AQLINK::CompArg(CARDINAL index) const
{
   if ( index >= numcomparg )
   {
      FatalError("AQLINK::CompArg: index too large");
   }
   else
   {
      return comparg[index];
   }
   return 0;
}
      

BOOLEAN AQLINK::Arg(const AQCOMP* com) const
{
   for ( CARDINAL i=0; i<numcomparg; i++ )
   {
      if ( comparg[i] == com ) return TRUE;
   }
   return FALSE;
}


void AQLINK::ReplVar(AQVAR*, AQVAR*)
{
   return;
}


BOOLEAN AQLINK::ReplaceVar(AQVAR* oldvar, AQVAR* newvar)
{
   if ( InList() == TRUE ) return FALSE;
   ReplVar(oldvar,newvar); return TRUE;
}


void AQLINK::ExchVar(AQVAR*, AQVAR*)
{
   CalcVarArg();
}


BOOLEAN AQLINK::ExchangeVar(AQVAR* var1, AQVAR* var2)
{
   if ( InList() == TRUE ) return FALSE;
   ExchVar(var1,var2);    return TRUE;
}


void AQLINK::ReplComp(AQCOMP*, AQCOMP*)
{
   return;
}


BOOLEAN AQLINK::ReplaceComp(AQCOMP* oldcom, AQCOMP* newcom)
{
   if ( InList() == TRUE ) return FALSE;
   ReplComp(oldcom,newcom); return TRUE;
}


void AQLINK::CalcVarArg()
{
   delete vararg; vararg = 0;
   numvararg = 0;
   return;
}


void AQLINK::CalcCompArg()
{
   delete comparg; comparg = 0;
   numcomparg = 0;
   return;
}


JOBSTATUS AQLINK::Load(std::istream& in, CARDINAL linkvers)
{
   char Buffer[BufSize];
   if ( InList() == TRUE )                                    return PROBLEM;
   switch ( linkvers )
   {
      case 1:
         if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE ) return PROBLEM;
         if ( Symbol(Buffer) == FALSE )                       return PROBLEM;
         if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE ) return PROBLEM;
         Description(Buffer);
         break;
      case 2:
         if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE ) return PROBLEM;
         if ( Symbol(Buffer) == FALSE )                       return PROBLEM;
         if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE ) return PROBLEM;
         if ( LoadNumber(in,&lind) == FALSE )                 return PROBLEM;
         Description(Buffer);
         break;
   }
   Changed();
   return OK;
}


JOBSTATUS AQLINK::Save(std::ostream& out)
{
   if ( SaveString(out,Symbol()) == FALSE )      return PROBLEM;
   if ( SaveString(out,Description()) == FALSE ) return PROBLEM;
   if ( SaveNumber(out,LnkIndex()) == FALSE )    return PROBLEM;
   return FILEIO::Save(out);
}


JOBSTATUS AQLINK::Write(std::ostream& out, BOOLEAN sh)
{
   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   PrintIndent(pos2-1);

   if ( Print(out,Symbol(),1) == FALSE )               return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;
   if ( sh == TRUE ) return OK;
   if ( Description() != 0 )
   {
      if ( strlen(Description()) != 0 )
      {
         if ( Print(out,ini.T(3),pos1) == FALSE )      return PROBLEM;
         if ( Print(out,":") == FALSE )                return PROBLEM;
         if ( Print(out,Description(),pos2) == FALSE ) return PROBLEM;
      }
   }

   if ( Print(out,ini.T(2),pos1) == FALSE )            return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;
   if ( Print(out,TypeName(),pos2) == FALSE )          return PROBLEM;

   if ( Print(out,ini.T(26),pos1) == FALSE )           return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;
   if ( Print(out,LnkIndex(),pos2) == FALSE )          return PROBLEM;

   return OK;
}

//////////////////////////////////////////////////////////////////////////////

void ADVLINK::init()
{
   compin  = 0;
   connin  = 0;
   compout = 0;
   connout = 0;
   return;
}


ADVLINK::ADVLINK()
{
   init();
}


ADVLINK::ADVLINK(const ADVLINK& link) : AQLINK(link)
{
   init();
   compin  = link.compin;
   connin  = link.connin;
   compout = link.compout;
   connout = link.connout;
   BIF* bif = (link.biflist).First();
   while ( bif != 0 )
   {
      BIF* b = new BIF(bif);
      if ( AddBif(b) == FALSE )
         FatalError("ADVLINK::ADVLINK: unable to add bifurcation");
      bif = bif->Next();
   }
}


ADVLINK::ADVLINK(const AQLINK* link) : AQLINK(link)
{
   init();
   if ( link != 0 )
   {
      if ( link->Type() == AdvLink )
      {
         ADVLINK* al = (ADVLINK*)link;
         compin  = al->compin;
         connin  = al->connin;
         compout = al->compout;
         connout = al->connout;
         BIF* bif = (al->biflist).First();
         while ( bif != 0 )
         {
            BIF* b = new BIF(bif);
            if ( AddBif(b) == FALSE )
               FatalError("ADVLINK::ADVLINK: unable to add bifurcation");
            bif = bif->Next();
         }
      }
   }
}


ADVLINK::~ADVLINK()
{
   DeleteBif();
}


const char* ADVLINK::TypeName() const
{
   return ini.T(11);
}


BOOLEAN ADVLINK::AllowedProgVarType(PROGVARTYPE type) const
{
   if ( type == CalcNumber ) return TRUE;
   if ( type == LinkIndex )  return TRUE;
   if ( type == Time )       return TRUE;
   if ( type == Discharge )  return TRUE;
   return FALSE;
}


BOOLEAN ADVLINK::SetCompIn(AQCOMP* com, CARDINAL conn)
{
   if ( InList() == TRUE ) return FALSE;
   if ( com == 0 )
   {
      compin = 0;
      connin = 0;
   }
   else
   {
      if ( conn >= com->NumAdvOutConn() ) return FALSE;
      compin = com;
      connin = conn;
   }
   CalcCompArg();
   return TRUE;
}


BOOLEAN ADVLINK::SetCompOut(AQCOMP* com, CARDINAL conn)
{
   if ( InList() == TRUE ) return FALSE;
   if ( com == 0 )
   {
      compout = 0;
      connout = 0;
   }
   else
   {
      if ( conn >= com->NumAdvInConn() ) return FALSE;
      compout = com;
      connout = conn;
   }
   CalcCompArg();
   return TRUE;
}


BOOLEAN ADVLINK::AdvInExZ() const
{
   if ( compout == 0 ) return FALSE;
   return compout->AdvInExZ(connout);
}


REAL ADVLINK::AdvInZ(const REAL* Y) const
{
   if ( compout == 0 )
      FatalError("ADVLINK::AdvInZ: Illegal call to this function");
   return compout->AdvInZ(Y,connout);
}


REAL ADVLINK::AdvOutQ(VARSYS* varsys, const REAL* Y, CARDINAL index)
{
   if ( compin == 0 ) return 0.0;
   linkindex = (REAL)lind;
   REAL Q = compin->AdvOutQ(varsys,Y,connin);  // set ProgVar Discharge
   BIF* bif = 0;
   if ( index == 0 )
   {
      bif = biflist.First();
      while ( bif != 0 )
      {
         Q -= bif->AdvOutQ();
         bif = bif->Next();
      }
      if ( Q < 0.0 ) return 0.0;
      return Q;
   }
   else
   {
      bif = biflist.Get(index);
      if ( bif == 0 )
         FatalError("ADVLINK::AdvOutQ: Illegal index");
      return bif->AdvOutQ();
   }
}


REAL ADVLINK::AdvOutJ(VARSYS* varsys,
                      const REAL* Y,
                      CARDINAL index,
                      const AQVAR* var)
{
   if ( compin == 0 ) return 0.0;
   linkindex = (REAL)lind;
   REAL Q0 = compin->AdvOutQ(varsys,Y,connin);       // set ProgVar Discharge
   REAL J0 = compin->AdvOutJ(varsys,Y,connin,var);   // set ProgVars Conc
   BIF* bif = 0;
   if ( index == 0 )
   {
      REAL J = J0;
      bif = biflist.First();
      while ( bif != 0 )
      {
         J -= bif->AdvOutJ(Q0,J0,var);
         bif = bif->Next();
      }
      return J;
   }
   else
   {
      bif = biflist.Get(index);
      if ( bif == 0 ) FatalError("ADVLINK::AdvOutJ: Illegal index");
      return bif->AdvOutJ(Q0,J0,var);
   }
}


BOOLEAN ADVLINK::AddBif(BIF* bif)
{
   if ( bif == 0 )                        return FALSE;
   if ( ExistBif(bif->Symbol()) == TRUE ) return FALSE;
   if ( biflist.Add(bif) == FALSE )
      FatalError("ADVLINK::AddBif: cannot add bifurcation");
   CalcCompArg();
   return TRUE;
}


BOOLEAN ADVLINK::ReplaceBif(BIF* oldbif, BIF* newbif)
{
   if ( newbif == 0 )                                        return FALSE;
   if ( ExistBif(oldbif) == FALSE )                          return FALSE;
   if ( ( strcmp(oldbif->Symbol(),newbif->Symbol()) != 0 )
        &&          ( ExistBif(newbif->Symbol()) == TRUE ) ) return FALSE;
   if ( biflist.Remove(oldbif) == FALSE )
      FatalError("ADVLINK::Replace: cannot remove old bifurcation");
   if ( biflist.Add(newbif) == FALSE )
      FatalError("ADVLINK::Replace: cannot insert new bifurcation");
   delete oldbif;
   CalcCompArg();
   return TRUE;
}


BOOLEAN ADVLINK::DeleteBif(BIF* bif)
{
   if ( bif == 0 )  return FALSE;
   BOOLEAN b = biflist.Remove(bif);
   if ( b == FALSE ) return FALSE;
   delete bif;
   CalcCompArg();
   return TRUE;
}


void ADVLINK::DeleteBif()
{
   BIF* bif = biflist.First();
   while ( bif != 0 )
   {
      if ( biflist.Remove(bif) == FALSE )
         FatalError("ADVLINKS::Delete: unable to remove bifurcation");
      delete bif;
      bif = biflist.First();
   }
   CalcCompArg();
   return;
}


void ADVLINK::ReplVar(AQVAR* oldvar, AQVAR* newvar)
{
   AQLINK::ReplVar(oldvar,newvar);
   BIF* bif = biflist.First();
   while ( bif != 0 )
   {
      bif->ReplVar(oldvar,newvar);
      bif = bif->Next();
   }
   CalcVarArg();
}


BOOLEAN ADVLINK::ReplaceVar(AQVAR* oldvar, AQVAR* newvar)
{
   if ( InList() == TRUE ) return FALSE;
   ReplVar(oldvar,newvar); return TRUE;
}


void ADVLINK::ExchVar(AQVAR* var1, AQVAR* var2)
{
   AQLINK::ExchVar(var1,var2);
   BIF* bif = biflist.First();
   while ( bif != 0 )
   {
      bif->ExchVar(var1,var2);
      bif = bif->Next();
   }
   CalcVarArg();
}


BOOLEAN ADVLINK::ExchangeVar(AQVAR* var1, AQVAR* var2)
{
   if ( InList() == TRUE ) return FALSE;
   ExchVar(var1,var2); return TRUE;
}


void ADVLINK::ReplComp(AQCOMP* oldcomp, AQCOMP* newcomp)
{
   AQLINK::ReplComp(oldcomp,newcomp);
   if ( compin  == oldcomp ) compin  = newcomp;
   if ( compout == oldcomp ) compout = newcomp;
   BIF* bif = biflist.First();
   while ( bif != 0 )
   {
      bif->ReplComp(oldcomp,newcomp);
      bif = bif->Next();
   }
   CalcCompArg();
   return;
}


void ADVLINK::CalcVarArg()
{
   CARDINAL i,j,k;
   AQLINK::CalcVarArg();
   CARDINAL num = numvararg;
   BIF* bif = biflist.First();
   while ( bif != 0 )
   {
      num += bif->NumVarArg();
      bif = bif->Next();
   }
   AQVAR** newarg = new AQVAR*[num];
   for ( i=0; i<numvararg; i++ ) newarg[i] = vararg[i];
   k = numvararg;
   bif = biflist.First();
   while ( bif != 0 )
   {
      for ( j=0; j<bif->NumVarArg(); j++ )
      {
         newarg[k] = bif->VarArg(j); k++;
      }
      bif = bif->Next();
   }
   numvararg = num;
   delete vararg;
   vararg = newarg;
   if ( k != num )
      FatalError("ADVLINK::CalcVarArg: Inconsistent number of arguments");
   return;
}


void ADVLINK::CalcCompArg()
{
   CARDINAL i,k;
   AQLINK::CalcCompArg();
   CARDINAL num = numcomparg;
   if ( compin != 0 )  num++;
   if ( compout != 0 ) num++;
   BIF* bif = biflist.First();
   while ( bif != 0 )
   {
      if ( bif->GetBifComp() != 0 ) num++;
      bif = bif->Next();
   }
   AQCOMP** newarg = new AQCOMP*[num];
   for ( i=0; i<numcomparg; i++ ) newarg[i] = comparg[i];
   k = numcomparg;
   if ( compin != 0 )
   {
      newarg[k] = compin; k++;
   }
   if ( compout != 0 )
   {
      newarg[k] = compout; k++;
   }
   bif = biflist.First();
   while ( bif != 0 )
   {
      if ( bif->GetBifComp() != 0 )
      {
         newarg[k] = bif->GetBifComp(); k++;
      }
      bif = bif->Next();
   }
   numcomparg = num;
   delete comparg;
   comparg = newarg;
   if ( k != num )
      FatalError("ADVLINK::CalcCompArg: Inconsistent number of arguments");
   return;
}


JOBSTATUS ADVLINK::Load(std::istream& in,
                      const VARLIST* varlist,
                      const COMPLIST* complist)
{
   char Buffer[32];

   if ( LoadStart(in) == FALSE )                               return PROBLEM;
   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )                    return PROBLEM;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.0
         {
            JOBSTATUS status = AQLINK::Load(in,1);
                                           if ( status != OK ) return status;
            AQCOMP* com = 0; CARDINAL conn = 0;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
            com = complist->Get(Buffer);
            if ( LoadNumber(in,&conn) == FALSE )               return PROBLEM;
            if ( SetCompIn(com,conn) == FALSE )                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
            com = complist->Get(Buffer);
            if ( LoadNumber(in,&conn) == FALSE )               return PROBLEM;
            if ( SetCompOut(com,conn) == FALSE )               return PROBLEM;

            if ( LoadStart(in) == FALSE )  return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               if ( strcmp(Buffer,BifIdentifier) != 0 ) return PROBLEM;
               BIF* bif = new BIF;
               if ( bif->Load(in,varlist,complist) == PROBLEM )
               {
                  delete bif; return PROBLEM;
               }
               if ( AddBif(bif) == PROBLEM )
               {
                  delete bif; return PROBLEM;
               }
            }
         }
         break;
      case 2: // aquavers=2: link index added 31.01.96
         {
            JOBSTATUS status = AQLINK::Load(in,2);
                                           if ( status != OK ) return status;
            AQCOMP* com = 0; CARDINAL conn = 0;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
            com = complist->Get(Buffer);
            if ( LoadNumber(in,&conn) == FALSE )               return PROBLEM;
            if ( SetCompIn(com,conn) == FALSE )                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                               return PROBLEM;
            com = complist->Get(Buffer);
            if ( LoadNumber(in,&conn) == FALSE )               return PROBLEM;
            if ( SetCompOut(com,conn) == FALSE )               return PROBLEM;

            if ( LoadStart(in) == FALSE )  return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               if ( strcmp(Buffer,BifIdentifier) != 0 ) return PROBLEM;
               BIF* bif = new BIF;
               if ( bif->Load(in,varlist,complist) == PROBLEM )
               {
                  delete bif; return PROBLEM;
               }
               if ( AddBif(bif) == PROBLEM )
               {
                  delete bif; return PROBLEM;
               }
            }
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                                return PROBLEM;
   return OK;
}


JOBSTATUS ADVLINK::Save(std::ostream& out)
{
   if ( SaveStart(out) == FALSE )                             return PROBLEM;
   INTEGER aquavers = 2;
   if ( SaveNumber(out,aquavers) == FALSE )                   return PROBLEM;
   JOBSTATUS status = AQLINK::Save(out);
   if ( status != OK ) return status;

   if ( compin == 0 )
   {
      if ( SaveString(out,"") == FALSE )                      return PROBLEM;
   }
   else
   {
      if ( SaveString(out,compin->Symbol()) == FALSE )        return PROBLEM;
   }
   if ( SaveNumber(out,connin) == FALSE )                     return PROBLEM;
   if ( compout == 0 )
   {
      if ( SaveString(out,"") == FALSE )                      return PROBLEM;
   }
   else
   {
      if ( SaveString(out,compout->Symbol()) == FALSE )       return PROBLEM;
   }
   if ( SaveNumber(out,connout) == FALSE )                    return PROBLEM;

   if ( SaveStart(out) == FALSE )                             return PROBLEM;
   BIF* bif = biflist.First();
   while ( bif != 0 )
   {
      if ( SaveString(out,BifIdentifier) == FALSE )           return PROBLEM;
      status = bif->Save(out); if ( status != OK ) return status;
      bif = bif->Next();
   }
   if ( SaveEnd(out) == FALSE )                               return PROBLEM;

   if ( SaveEnd(out) == FALSE )                               return PROBLEM;
   return OK;
}


JOBSTATUS ADVLINK::Write(std::ostream& out, BOOLEAN sh)
{
   JOBSTATUS status = AQLINK::Write(out,sh); 
                                          if ( status != OK ) return status;

   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   if ( sh == TRUE )
   {
      PrintIndent(pos1-1);
	  if ( compin != 0 )
	  {
         if ( Print(out,compin->Symbol(),pos1) == FALSE )     return PROBLEM;
         if ( Print(out," -> ") == FALSE )                    return PROBLEM;
      }
	  else
      {
         if ( Print(out," -> ",pos1) == FALSE )               return PROBLEM;
      }
	  if ( compout != 0 )
	  {
         if ( Print(out,compout->Symbol()) == FALSE )         return PROBLEM;
	  }
      if ( biflist.Size() > 0 )
      {
         PrintIndent(pos1+2);
         BIF* bif = biflist.First();
         while ( bif != 0 )
         {
            if ( Print(out,bif->Symbol(),pos1+3) == FALSE )   return PROBLEM;
            if ( Print(out,": -> ") == FALSE )               return PROBLEM;
            if ( bif->GetBifComp() != 0 )
			{
               if ( Print(out,bif->GetBifComp()->Symbol()) == FALSE )
				                                              return PROBLEM;
			}
            bif = bif->Next();
         }
      }
   }
   else
   {
      PrintIndent(pos2-1);
      if ( Print(out,ini.T(23),pos1) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                          return PROBLEM;
      if ( compin != 0 )
      {
         if ( Print(out,compin->Symbol(),pos2) == FALSE )     return PROBLEM;
         if ( Print(out,ini.T(24),pos1) == FALSE )            return PROBLEM;
         if ( Print(out,":") == FALSE )                       return PROBLEM;
         if ( Print(out,compin->AdvOutName(connin),pos2) == FALSE )
                                                              return PROBLEM;
      }
      if ( Print(out,ini.T(25),pos1) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                          return PROBLEM;
      if ( compout != 0 )
      {
         if ( Print(out,compout->Symbol(),pos2) == FALSE )    return PROBLEM;
         if ( Print(out,ini.T(7),pos1) == FALSE )             return PROBLEM;
         if ( Print(out,":") == FALSE )                       return PROBLEM;
         if ( Print(out,compout->AdvInName(connout),pos2) == FALSE )
                                                              return PROBLEM;
      }
      if ( Print(out,ini.T(18),pos1) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                          return PROBLEM;
      if ( biflist.Size() > 0 )
      {
         BIF* bif = biflist.First();
         while ( bif != 0 )
         {
            status = bif->Write(out);     if ( status != OK ) return status;
            bif = bif->Next();
         }
      }
   }

   return OK;
}


//////////////////////////////////////////////////////////////////////////////

void DIFFLINK::init()
{
   comp1 = 0;
   conn1 = 0;
   comp2 = 0;
   conn2 = 0;
   numexch = 0; exchvar = 0; exchval = 0; exchfac = 0;
   return;
}


DIFFLINK::DIFFLINK()
{
   init();
}


DIFFLINK::DIFFLINK(const DIFFLINK& link) : AQLINK(link)
{
   init();
   comp1 = link.comp1;
   conn1 = link.conn1;
   comp2 = link.comp2;
   conn2 = link.conn2;
   if ( link.numexch > 0 )
   {
      numexch = link.numexch;
      exchvar = new AQVAR*[numexch];
      exchval = new FORMVAR*[numexch];
      exchfac = new FORMVAR*[numexch];
      for ( CARDINAL i=0; i<numexch; i++ )
      {
         exchvar[i] = link.exchvar[i];
         exchval[i] = new FORMVAR(link.exchval[i]);
         exchfac[i] = new FORMVAR(link.exchfac[i]);
      }
   }
}


DIFFLINK::DIFFLINK(const AQLINK* link) : AQLINK(link)
{
   init();
   if ( link != 0 )
   {
      if ( link->Type() == DiffLink )
      {
         DIFFLINK* dl = (DIFFLINK*)link;
         comp1 = dl->comp1;
         conn1 = dl->conn1;
         comp2 = dl->comp2;
         conn2 = dl->conn2;
         if ( dl->numexch > 0 )
         {
            numexch = dl->numexch;
            exchvar = new AQVAR*[numexch];
            exchval = new FORMVAR*[numexch];
            exchfac = new FORMVAR*[numexch];
            for ( CARDINAL i=0; i<numexch; i++ )
            {
               exchvar[i] = dl->exchvar[i];
               exchval[i] = new FORMVAR(dl->exchval[i]);
               exchfac[i] = new FORMVAR(dl->exchfac[i]);
            }
         }
      }
   }
}


DIFFLINK::~DIFFLINK()
{
   CARDINAL i;
   delete exchvar; exchvar = 0;
   for ( i=0; i<numexch; i++ )
   {
      delete exchval[i];
      delete exchfac[i];
   }
   delete exchval; exchval = 0;
   delete exchfac; exchfac = 0;
   numexch = 0;
}


const char* DIFFLINK::TypeName() const
{
   return ini.T(12);
}


BOOLEAN DIFFLINK::SetComp1(AQCOMP* com, CARDINAL conn)
{
   if ( InList() == TRUE ) return FALSE;
   if ( com == 0 )
   {
      comp1 = 0;
      conn1 = 0;
   }
   else
   {
      if ( conn >= com->NumDiffConn() ) return FALSE;
      comp1 = com;
      conn1 = conn;
   }
   CalcCompArg();
   return TRUE;
}


BOOLEAN DIFFLINK::SetComp2(AQCOMP* com, CARDINAL conn)
{
   if ( InList() == TRUE ) return FALSE;
   if ( com == 0 )
   {
      comp2 = 0;
      conn2 = 0;
   }
   else
   {
      if ( conn >= com->NumDiffConn() ) return FALSE;
      comp2 = com;
      conn2 = conn;
   }
   CalcCompArg();
   return TRUE;
}


BOOLEAN DIFFLINK::AllowedProgVarType(PROGVARTYPE type) const
{
   if ( type == CalcNumber ) return TRUE;
   if ( type == LinkIndex )  return TRUE;
   if ( type == Time )       return TRUE;
   return FALSE;
}


AQVAR* DIFFLINK::ExchVar(CARDINAL index) const
{
   if ( index >= numexch )
      FatalError("DIFFLINK::ExchVar: Illegal index");
   return exchvar[index];
}


const FORMVAR* DIFFLINK::ExchVal(CARDINAL index) const
{
   if ( index >= numexch )
      FatalError("DIFFLINK::ExchVal: Illegal index");
   return exchval[index];
}


const FORMVAR* DIFFLINK::ExchFac(CARDINAL index) const
{
   if ( index >= numexch )
      FatalError("DIFFLINK::ExchFac: Illegal index");
   return exchfac[index];
}


BOOLEAN DIFFLINK::AddExchCoeff(const VARLIST* varlist,AQVAR* var,
                               const char* inpval, char* errorlineval,
                               CARDINAL& parseerrorsval,
                               const char* inpfac, char* errorlinefac,
                               CARDINAL& parseerrorsfac,
                               CARDINAL pos)
{
   parseerrorsval = 0;
   parseerrorsfac = 0;
   if ( InList() == TRUE )             return FALSE;
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
   delete exchvar; exchvar = newvar;
   delete exchval; exchval = newval;
   delete exchfac; exchfac = newfac;
   numexch++;
   CalcVarArg();
   return TRUE;
}


BOOLEAN DIFFLINK::ReplaceExchCoeff(const VARLIST* varlist,AQVAR* var,
                                   const char* inpval, char* errorlineval,
                                   CARDINAL& parseerrorsval,
                                   const char* inpfac, char* errorlinefac,
                                   CARDINAL& parseerrorsfac,
                                   CARDINAL pos)
{
   parseerrorsval = 0;
   parseerrorsfac = 0;
   if ( InList() == TRUE )              return FALSE;
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


BOOLEAN DIFFLINK::DeleteExchCoeff(CARDINAL pos)
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
   delete exchvar; exchvar = newvar;
   delete exchval; exchval = newval;
   delete exchfac; exchfac = newfac;
   numexch--;
   CalcVarArg();
   return TRUE;
}


void DIFFLINK::ReplVar(AQVAR* oldvar, AQVAR* newvar)
{
   CARDINAL i;
   AQLINK::ReplVar(oldvar,newvar);
   {
      for ( i=0; i<numexch; i++ )
      {
         if ( exchvar[i] == oldvar ) exchvar[i] = newvar;
         exchval[i]->ReplaceVarArg(oldvar,newvar);
         exchfac[i]->ReplaceVarArg(oldvar,newvar);
      }
   }
   CalcVarArg();
}


BOOLEAN DIFFLINK::ReplaceVar(AQVAR* oldvar, AQVAR* newvar)
{
   if ( InList() == TRUE ) return FALSE;
   ReplVar(oldvar,newvar); return TRUE;
}


void DIFFLINK::ExchVar(AQVAR* var1, AQVAR* var2)
{
   CARDINAL i;
   AQVAR* v = new CONSTVAR;
   AQLINK::ExchVar(var1,var2);
   for ( i=0; i<numexch; i++ )
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
   delete v;
   CalcVarArg();
}


BOOLEAN DIFFLINK::ExchangeVar(AQVAR* var1, AQVAR* var2)
{
   if ( InList() == TRUE ) return FALSE;
   ExchVar(var1,var2); return TRUE;
}


void DIFFLINK::ReplComp(AQCOMP* oldcomp, AQCOMP* newcomp)
{
   AQLINK::ReplComp(oldcomp,newcomp);
   if ( comp1 == oldcomp ) comp1 = newcomp;
   if ( comp2 == oldcomp ) comp2 = newcomp;
   CalcCompArg();
   return;
}


void DIFFLINK::CalcVarArg()
{
   CARDINAL i,j,k;
   AQLINK::CalcVarArg();
   CARDINAL num = numvararg;
   for ( i=0; i<numexch; i++ )
   {
      num++;
      num += exchval[i]->NumVarArg();
      num += exchfac[i]->NumVarArg();
   }
   AQVAR** newarg = new AQVAR*[num];
   for ( i=0; i<numvararg; i++ ) newarg[i] = vararg[i];
   k = numvararg;
   for ( i=0; i<numexch; i++ )
   {
      newarg[k] = exchvar[i]; k++;
      for ( j=0; j<(exchval[i]->NumVarArg()); j++ )
      {
          newarg[k] = exchval[i]->VarArg(j); k++;
      }
      for ( j=0; j<(exchfac[i]->NumVarArg()); j++ )
      {
          newarg[k] = exchfac[i]->VarArg(j); k++;
      }
   }
   numvararg = num;
   delete vararg;
   vararg = newarg;
   if ( k != num )
      FatalError("DIFFLINK::CalcVarArg: Inconsistent number of arguments");
   return;
}


void DIFFLINK::CalcCompArg()
{
   CARDINAL i,k;
   AQLINK::CalcCompArg();
   CARDINAL num = numcomparg;
   if ( comp1 != 0 ) num++;
   if ( comp2 != 0 ) num++;
   AQCOMP** newarg = new AQCOMP*[num];
   for ( i=0; i<numcomparg; i++ ) newarg[i] = comparg[i];
   k = numcomparg;
   if ( comp1 != 0 )
   {
      newarg[k] = comp1; k++;
   }
   if ( comp2 != 0 )
   {
      newarg[k] = comp2; k++;
   }
   numcomparg = num;
   delete comparg;
   comparg = newarg;
   if ( k != num )
      FatalError("ADVLINK::CalcCompArg: Inconsistent number of arguments");
   return;
}


REAL DIFFLINK::DiffJ(const REAL* Y, CARDINAL index, const AQVAR* var)
{
   linkindex = (REAL)lind;
   if ( index > 1 ) FatalError("DIFFLINK::DiffJ: Illegal index");
   REAL C1 = 0.0;
   if ( comp1 != 0 ) C1 = comp1->DiffC(Y,conn1,var);
   REAL C2 = 0.0;
   if ( comp2 != 0 ) C2 = comp2->DiffC(Y,conn2,var);
   for ( CARDINAL i=0; i<numexch; i++ )
   {
      if ( var == exchvar[i] )
      {
         REAL exch = exchval[i]->Value();
         REAL fact  = exchfac[i]->Value();
         switch ( index )
         {
            case 0:
               return (C2-fact*C1)*exch;
            case 1:
               return (fact*C1-C2)*exch;
            default:
               FatalError("DIFFLINK::DiffJ: Illegal index");
         }
      }
   }
   return 0.0;
}


JOBSTATUS DIFFLINK::Load(std::istream& in,
                         const VARLIST* varlist,
                         const COMPLIST* complist)
{
   CARDINAL numerrval, numerrfac;
   char Buffer1[BufSize]; char Buffer2[BufSize];
   char Buffer3[BufSize]; char Buffer4[BufSize];

   if ( LoadStart(in) == FALSE )                                 return PROBLEM;
   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )                      return PROBLEM;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.0
         {
            JOBSTATUS status = AQLINK::Load(in,1);
                                             if ( status != OK ) return status;

            AQCOMP* com = 0; CARDINAL conn = 0;
            if ( LoadString(in,Buffer1,sizeof(Buffer1)) == FALSE )
                                                                 return PROBLEM;
            com = complist->Get(Buffer1);
            if ( LoadNumber(in,&conn) == FALSE )                 return PROBLEM;
            if ( SetComp1(com,conn) == FALSE )                   return PROBLEM;
            if ( LoadString(in,Buffer1,sizeof(Buffer1)) == FALSE )
                                                                 return PROBLEM;
            com = complist->Get(Buffer1);
            if ( LoadNumber(in,&conn) == FALSE )                 return PROBLEM;
            if ( SetComp2(com,conn) == FALSE )                   return PROBLEM;

            if ( LoadStart(in) == FALSE )                        return PROBLEM;
            while ( LoadString(in,Buffer1,sizeof(Buffer1)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer1); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer1,sizeof(Buffer1)) == FALSE )
                                                                 return PROBLEM;
               if ( LoadString(in,Buffer3,sizeof(Buffer3)) == FALSE )
                                                                 return PROBLEM;
               if ( AddExchCoeff(varlist,var,Buffer1,Buffer2,numerrval,
                           Buffer3,Buffer4,numerrfac) == FALSE ) return PROBLEM;
            }
         }
         break;
      case 2: // link index added 31.01.96
         {
            JOBSTATUS status = AQLINK::Load(in,2);
                                             if ( status != OK ) return status;

            AQCOMP* com = 0; CARDINAL conn = 0;
            if ( LoadString(in,Buffer1,sizeof(Buffer1)) == FALSE )
                                                                 return PROBLEM;
            com = complist->Get(Buffer1);
            if ( LoadNumber(in,&conn) == FALSE )                 return PROBLEM;
            if ( SetComp1(com,conn) == FALSE )                   return PROBLEM;
            if ( LoadString(in,Buffer1,sizeof(Buffer1)) == FALSE )
                                                                 return PROBLEM;
            com = complist->Get(Buffer1);
            if ( LoadNumber(in,&conn) == FALSE )                 return PROBLEM;
            if ( SetComp2(com,conn) == FALSE )                   return PROBLEM;

            if ( LoadStart(in) == FALSE )                        return PROBLEM;
            while ( LoadString(in,Buffer1,sizeof(Buffer1)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer1); if ( var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer1,sizeof(Buffer1)) == FALSE )
                                                                 return PROBLEM;
               if ( LoadString(in,Buffer3,sizeof(Buffer3)) == FALSE )
                                                                 return PROBLEM;
               if ( AddExchCoeff(varlist,var,Buffer1,Buffer2,numerrval,
                           Buffer3,Buffer4,numerrfac) == FALSE ) return PROBLEM;
            }
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                                   return PROBLEM;
   return OK;
}


JOBSTATUS DIFFLINK::Save(std::ostream& out)
{
   char Buffer[BufSize];
   CARDINAL i;

   if ( SaveStart(out) == FALSE )                               return PROBLEM;
   INTEGER aquavers = 2;
   if ( SaveNumber(out,aquavers) == FALSE )                     return PROBLEM;

   JOBSTATUS status = AQLINK::Save(out);    if ( status != OK ) return status;

   if ( comp1 == 0 )
   {
      if ( SaveString(out,"") == FALSE )                        return PROBLEM;
   }
   else
   {
      if ( SaveString(out,comp1->Symbol()) == FALSE )           return PROBLEM;
   }
   if ( SaveNumber(out,conn1) == FALSE )                        return PROBLEM;
   if ( comp2 == 0 )
   {
      if ( SaveString(out,"") == FALSE )                        return PROBLEM;
   }
   else
   {
      if ( SaveString(out,comp2->Symbol()) == FALSE )           return PROBLEM;
   }
   if ( SaveNumber(out,conn2) == FALSE )                        return PROBLEM;

   if ( SaveStart(out) == FALSE )                               return PROBLEM;
   for ( i=0; i<numexch; i++ )
   {
      if ( SaveString(out,(exchvar[i])->Symbol()) == FALSE )   return PROBLEM;
      if ( (exchval[i])->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                                return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                    return PROBLEM;
      if ( (exchfac[i])->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                                return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                    return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                                 return PROBLEM;

   if ( SaveEnd(out) == FALSE )                                 return PROBLEM;
   return OK;
}


JOBSTATUS DIFFLINK::Write(std::ostream& out, BOOLEAN sh)
{
   JOBSTATUS status = AQLINK::Write(out,sh); 
                                            if ( status != 0 ) return status;

   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   CARDINAL pos3 = pos1+2;
   CARDINAL pos4 = pos3+2;
   if ( sh == TRUE )
   {
      PrintIndent(pos1-1);
	  if ( comp1 != 0 )
	  {
         if ( Print(out,comp1->Symbol(),pos1) == FALSE )      return PROBLEM;
         if ( Print(out," <-> ") == FALSE )                   return PROBLEM;
      }
	  else
      {
         if ( Print(out," <-> ",pos1) == FALSE )              return PROBLEM;
      }
	  if ( comp2 != 0 )
	  {
         if ( Print(out,comp2->Symbol()) == FALSE )           return PROBLEM;
	  }
   }
   else
   {
      PrintIndent(pos2-1);
      char Buffer[BufSize];
      CARDINAL i;
      if ( Print(out,ini.T(14),pos1) == FALSE )                return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( comp1 != 0 )
      {
         if ( Print(out,comp1->Symbol(),pos2) == FALSE )       return PROBLEM;
         if ( Print(out,ini.T(15),pos1) == FALSE )             return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( Print(out,comp1->DiffName(conn1),pos2) == FALSE )
			                                                   return PROBLEM;
      }
      if ( Print(out,ini.T(16),pos1) == FALSE )                return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( comp2 != 0 )
      {
         if ( Print(out,comp2->Symbol(),pos2) == FALSE )       return PROBLEM;
         if ( Print(out,ini.T(17),pos1) == FALSE )             return PROBLEM;
         if ( Print(out,":") == FALSE )                        return PROBLEM;
         if ( Print(out,comp2->DiffName(conn2),pos2) == FALSE )
			                                                   return PROBLEM;
      }
      if ( Print(out,ini.T(8),pos1) == FALSE )                 return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( NumExch() > 0 )
      {
         PrintIndent(pos4-1);
         if ( Print(out,ini.T(9),pos3) == FALSE )              return PROBLEM;
         if ( Print(out," : ") == FALSE )                      return PROBLEM;
         if ( Print(out,ini.T(10)) == FALSE )                  return PROBLEM;
         if ( Print(out,", ") == FALSE )                       return PROBLEM;
         if ( Print(out,ini.T(13)) == FALSE )                  return PROBLEM;
         for ( i=0; i<NumExch(); i++ )
         {
            if ( Print(out,(exchvar[i])->Symbol(),pos3) == FALSE )
                                                               return PROBLEM;
            if ( Print(out," : ") == FALSE )                   return PROBLEM;
            if ( (exchval[i])->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
            if ( Print(out,Buffer) == FALSE )                  return PROBLEM;
            if ( Print(out,", ") == FALSE )                    return PROBLEM;
            if ( (exchfac[i])->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
            if ( Print(out,Buffer) == FALSE )                  return PROBLEM;
         }
      }
   }


   return OK;
}

//////////////////////////////////////////////////////////////////////////////

