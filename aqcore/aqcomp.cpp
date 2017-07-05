////////////////////////////////   aqcomp.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    05.09.91    Peter Reichert
// revisions:   22.10.92    Peter Reichert    redesign of links
//              27.10.92    Peter Reichert    redesign of VarArg(CARDINAL)
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "formvar.h"
#include "othervar.h"
#include "varsys.h"
#include "aqcomp.h"

//////////////////////////////////////////////////////////////////////////////

const CARDINAL BufSize = 1024;

//////////////////////////////////////////////////////////////////////////////

INIT AQCOMP::ini(initfile,"COMPSYS","comp_txt_",10,30,10,400);


CARDINAL AQCOMP::number = 0;


void AQCOMP::init()
{
   numvararg = 0; vararg = 0;
   numgrid = 1;
   numstatevar = 0; numdynstatevar = 0;
   numvolstatevar = 0; numsurfstatevar = 0; numequstatevar = 0;
   statevar = 0;
   numactvar = 0; actvar = 0;
   numactproc = 0; actproc = 0;
   active = TRUE;
   ind_y = 0;
   numadvinconn = 0;
   numadvoutconn = 0;
   numdiffconn = 0;
   advinnum   = 0;
   advinindex = 0;
   advinlink  = 0;
   advoutlink = 0;
   diffnum    = 0;
   diffindex  = 0;
   difflink   = 0;
   cind       = 0;
   number++;
}


AQCOMP::AQCOMP()
{
   init();
   char Buf1[BufSize]; char Buf2[BufSize];
   strcpy(Buf1,ini.T(1));
   NumToString(number,Buf2,BufSize);
   strcat(Buf1,Buf2);
   if ( Symbol(Buf1) == FALSE )
      FatalError("AQCOMP::AQCOMP: illegal symbol");
   return;
}


AQCOMP::AQCOMP(const AQCOMP& com) : SYMBOL(com)
{
   init();
   cind = com.cind;
   numgrid = com.numgrid;
   if ( com.numvararg > 0 )
   {
      numvararg = com.numvararg;
      vararg = new AQVAR*[numvararg];
      for ( CARDINAL i=0; i<numvararg; i++ )
      {
         vararg[i] = com.vararg[i];
      }
   }
   if ( com.numactvar > 0 )
   {
      numactvar = com.numactvar;
      actvar = new AQVAR*[numactvar];
      for ( CARDINAL i=0; i<numactvar; i++ )
      {
         actvar[i] = com.actvar[i];
      }
      CalcStateVar();
   }
   if ( com.numactproc > 0 )
   {
      numactproc = com.numactproc;
      actproc = new AQPROC*[numactproc];
      for ( CARDINAL i=0; i<numactproc; i++ )
      {
         actproc[i] = com.actproc[i];
      }
   }
   active = com.active;
}


AQCOMP::AQCOMP(const AQCOMP* com) : SYMBOL(com)
{
   init();
   if ( com != 0 )
   {
      cind = com->cind;
      numgrid = com->numgrid;
      if ( com->numvararg > 0 )
      {
         numvararg = com->numvararg;
         vararg = new AQVAR*[numvararg];
         for ( CARDINAL i=0; i<numvararg; i++ )
         {
            vararg[i] = com->vararg[i];
         }
      }
      if ( com->numactvar > 0 )
      {
         numactvar = com->numactvar;
         actvar = new AQVAR*[numactvar];
         for ( CARDINAL i=0; i<numactvar; i++ )
         {
            actvar[i] = com->actvar[i];
         }
         CalcStateVar();
      }
      if ( com->numactproc > 0 )
      {
         numactproc = com->numactproc;
         actproc = new AQPROC*[numactproc];
         for ( CARDINAL i=0; i<numactproc; i++ )
         {
            actproc[i] = com->actproc[i];
         }
      }
      active = com->active;
   }
   else
   {
      char Buf1[BufSize]; char Buf2[BufSize];
      strcpy(Buf1,ini.T(1));
      NumToString(number,Buf2,BufSize);
      strcat(Buf1,Buf2);
      if ( Symbol(Buf1) == FALSE )
         FatalError("AQCOMP::AQCOMP: illegal symbol");
   }
}


AQCOMP::~AQCOMP()
{
   numstatevar = 0; numdynstatevar = 0;
   numvolstatevar = 0; numsurfstatevar = 0; numequstatevar = 0;
   delete statevar; statevar = 0;
   delete actvar; actvar = 0; numactvar = 0;
   delete actproc; actproc = 0; numactproc = 0;
   delete vararg; vararg = 0; numvararg = 0;
   if ( number == 0 )
      FatalError("AQCOMP::~AQCOMP: already all comps destructed");
   number--;
   CARDINAL conn;
   for ( conn=0; conn<NumAdvInConn(); conn++ )
   {
      if ( advinnum[conn] > 0 )
      {
         delete [] advinindex[conn];
         delete [] advinlink[conn];
      }
   }
   for ( conn=0; conn<NumDiffConn(); conn++ )
   {
      if ( diffnum[conn] > 0 )
      {
         delete [] diffindex[conn];
         delete [] difflink[conn];
      }
   }
   delete [] advinnum;
   delete [] advinindex;
   delete [] advinlink;
   delete [] advoutlink;
   delete [] diffnum;
   delete [] diffindex;
   delete [] difflink;
}


BOOLEAN AQCOMP::AllowedVar(const AQVAR* var) const
{
   if ( var->Type() == ProgVar )
   {
      PROGVAR* pv = (PROGVAR*)var;
      if ( AllowedProgVarType(pv->ProgVarType()) == FALSE ) return FALSE;
   }
   for ( CARDINAL i=0; i<var->NumVarArg(); i++ )
   {
      // if ( AllowedVar(var->VarArg(i)) == FALSE ) return FALSE;
      AQVAR* actvar = var->VarArg(i);
      if ( actvar != 0 )
      {
	     if ( actvar->Type() == ProbeVar )
         {
			PROBEVAR* actprobevar = (PROBEVAR*)actvar;
            AQCOMP* com = actprobevar->Compartment();
            if ( com != 0 )
            {
               if ( com->AllowedVar(actprobevar->Variable()) == FALSE )
                  return FALSE;
            }
         }
         else
         {
            if ( AllowedVar(actvar) == FALSE ) return FALSE;
         }
      }
   }
   return TRUE;
}


BOOLEAN AQCOMP::AllowedReplaceVar(const AQVAR* oldvar,
                                  const AQVAR* newvar) const
{
   if ( Arg(oldvar) == TRUE )
   {
      if ( AllowedVar(newvar) == FALSE ) return FALSE;
   }
   return TRUE;
}


BOOLEAN AQCOMP::AllowedExchangeVar(const AQVAR* var1,
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


BOOLEAN AQCOMP::AllowedProc(const AQPROC* proc) const
{
   if ( proc == 0 )                               return FALSE;
   CARDINAL i;

   // check legality of arguments:

   for ( i=0; i<proc->NumVarArg(); i++ )
   {
      if ( AllowedVar(proc->VarArg(i)) == FALSE ) return FALSE;
   }

   // check if first equilibrium process for the specified variable:

   if ( proc->Type() == EquProc )
   {
      for ( i=0; i<numactproc; i++ )
      {
         if ( actproc[i]->Type() == EquProc )
         {
            if ( proc->Variable(0) == actproc[i]->Variable(0) )
                                                  return FALSE;
         }
      }
   }
   return TRUE;
}


BOOLEAN AQCOMP::AllowedReplaceProc(const AQPROC* oldproc,
                                   const AQPROC* newproc) const
{
   if ( newproc == 0 ) return FALSE;
   CARDINAL i,j;

   // if oldproc exists, check legality of arguments:

   for ( i=0; i<NumActProc(); i++ )
   {
      if ( ActProc(i) == oldproc )
      {
         for ( j=0; j<newproc->NumVarArg(); j++ )
         {
            if ( AllowedVar(newproc->VarArg(j)) == FALSE ) return FALSE;
         }

         // check if first equilibrium process for the specified variable
         // or for the same variable as specified in oldproc:

         if ( newproc->Type() == EquProc )
         {
            if ( oldproc->Type() == EquProc )
            {
               if ( newproc->Variable(0) == oldproc->Variable(0) )
                  return TRUE;  // (equproc with same variable)
            }
            for ( j=0; j<numactproc; j++ )
            {
               if ( actproc[j]->Type() == EquProc )
               {
                  if ( newproc->Variable(0) == actproc[j]->Variable(0) )
                                                          return FALSE;
               }
            }
         }
         return TRUE;  // (dynproc, or equproc with new variable)
      }
   }
   return TRUE;   // (oldproc not found)
}


AQVAR* AQCOMP::VarArg(CARDINAL index) const
{
   if ( index >= numvararg )
      FatalError("AQCOMP::VarArg: Illegal index");
   return vararg[index];
}


BOOLEAN AQCOMP::Arg(const AQVAR* var) const
{
   for ( CARDINAL i=0; i<NumVarArg(); i++ )
   {
      if ( VarArg(i) == var )            return TRUE;
      if ( VarArg(i)->Arg(var) == TRUE ) return TRUE;
   }
   return FALSE;
}


AQPROC* AQCOMP::ProcArg(CARDINAL index) const
{
   if ( index>numactproc )
      FatalError("AQCOMP::Arg: Illegal index");
   return actproc[index];
}


BOOLEAN AQCOMP::Arg(const AQPROC* proc) const
{
   for ( CARDINAL i=0; i<NumProcArg(); i++ )
   {
      if ( ProcArg(i) == proc ) return TRUE;
   }
   return FALSE;
}


void AQCOMP::ReplVar(AQVAR* oldvar, AQVAR* newvar)
{
   CARDINAL i;
   for ( i=0; i<numactvar; i++ )
   {
      if ( actvar[i] == oldvar ) actvar[i] = newvar;
   }
   CalcStateVar();
   CalcArg();
   return;
}


BOOLEAN  AQCOMP::ReplaceVar(AQVAR* oldvar, AQVAR* newvar)
{
   if ( InList() == TRUE )                          return FALSE;
   if ( AllowedReplaceVar(oldvar,newvar) == FALSE ) return FALSE;
   ReplVar(oldvar,newvar);
   return TRUE;
}


void AQCOMP::ExchVar(AQVAR* var1, AQVAR* var2)
{
   CARDINAL i;
   AQVAR* v = new CONSTVAR;
   for ( i=0; i<numactvar; i++ )
   {
      if ( actvar[i] == var1 )
      {
         actvar[i] = var2;
      }
      else
      {
         if ( actvar[i] == var2 ) actvar[i] = var1;
      }
   }
   CalcStateVar();
   CalcArg();
   return;
}


BOOLEAN  AQCOMP::ExchangeVar(AQVAR* var1, AQVAR* var2)
{
   if ( InList() == TRUE )                       return FALSE;
   if ( AllowedExchangeVar(var1,var2) == FALSE ) return FALSE;
   ExchVar(var1,var2);
   return TRUE;
}


void AQCOMP::ReplProc(AQPROC* oldproc, AQPROC* newproc)
{
   CARDINAL i;
   for ( i=0; i<numactproc; i++ )
   {
      if ( actproc[i] == oldproc )
      {
         actproc[i] = newproc; CalcArg(); return;
      }
   }
   return;
}


BOOLEAN AQCOMP::ReplaceProc(AQPROC* oldproc, AQPROC* newproc)
{
   if ( InList() == TRUE )                             return FALSE;
   if ( AllowedReplaceProc(oldproc,newproc) == FALSE ) return FALSE;
   ReplProc(oldproc,newproc);
   return TRUE;
}


void AQCOMP::CalcStateVar()
{
   numstatevar = 0; numdynstatevar = 0;
   numvolstatevar = 0; numsurfstatevar = 0; numequstatevar = 0;
   CARDINAL i;
   for ( i=0; i<numactvar; i++ )
   {
      if ( (actvar[i])->Type() == StateVar )
      {
         STATEVAR* sv = (STATEVAR*)actvar[i];
         switch ( sv->StateVarType() )
         {
            case VolStateVar:
               numvolstatevar++;
               numdynstatevar++;
               break;
            case SurfStateVar:
               numsurfstatevar++;
               numdynstatevar++;
               break;
            case EquStateVar:
               numequstatevar++;
               break;
            default:
               FatalError("AQCOMP::CalcStateVar: unknown StateVar type");
         }
         numstatevar++;
      }
   }
   delete statevar; statevar = new STATEVAR*[numstatevar];
   CARDINAL ivol = 0; CARDINAL isurf = 0; CARDINAL iequ = 0;
   for ( i=0; i<numactvar; i++ )
   {
      if ( (actvar[i])->Type() == StateVar )
      {
         STATEVAR* sv = (STATEVAR*)actvar[i];
         switch ( sv->StateVarType() )
         {
            case VolStateVar:
               statevar[ivol] = sv; ivol++;
               break;
            case SurfStateVar:
               statevar[numvolstatevar+isurf] = sv; isurf++;
               break;
            case EquStateVar:
               statevar[numdynstatevar+iequ] = sv; iequ++;
               break;
            default:
               FatalError("AQCOMP::CalcStateVar: unknown StateVar type");
         }
      }
   }
   if ( ivol+isurf+iequ != numstatevar )
      FatalError("AQCOMP::CalcStateVar: inconsistent StateVar types");
   return;
}


AQVAR* AQCOMP::ActVar(CARDINAL index) const
{
   if ( index >= numactvar )
      FatalError("AQCOMP::ActVar: Illegal index");
   return actvar[index];
}


BOOLEAN AQCOMP::AddActVar(AQVAR* var, CARDINAL pos)
{
   CARDINAL i;
   if ( InList() == TRUE )            return FALSE;
   if ( var == 0 )                    return FALSE;
   if ( var->Type() != StateVar )
   {
      if ( AllowedVar(var) == FALSE ) return FALSE;
   }
   for ( i=0; i<numactvar; i++ )
   {
      if ( actvar[i] == var )         return FALSE;
   }

   if ( pos > numactvar ) pos = numactvar;
   AQVAR** newactvar = new AQVAR*[numactvar+1];
   for ( i=0; i<pos; i++ )
   {
      newactvar[i] = actvar[i];
   }
   newactvar[pos] = var;
   for ( i=pos; i<numactvar; i++ )
   {
      newactvar[i+1] = actvar[i];
   }
   delete actvar; actvar = newactvar;
   numactvar++;
   CalcStateVar();
   CalcArg();
   return TRUE;
}


BOOLEAN AQCOMP::RemoveActVar(CARDINAL pos)
{
   CARDINAL i;
   if ( pos >= numactvar ) return FALSE;
   AQVAR** newactvar = new AQVAR*[numactvar-1];
   for ( i=0; i<pos; i++ )
   {
      newactvar[i] = actvar[i];
   }
   for ( i=pos+1; i<numactvar; i++ )
   {
      newactvar[i-1] = actvar[i];
   }
   delete actvar; actvar = newactvar;
   numactvar--;
   CalcStateVar();
   CalcArg();
   return TRUE;
}


BOOLEAN AQCOMP::RemoveActVar(AQVAR* var)
{
   for ( CARDINAL i=0; i<numactvar; i++ )
   {
      if ( var == actvar[i] ) return RemoveActVar(i);
   }
   return FALSE;
}


AQPROC* AQCOMP::ActProc(CARDINAL index) const
{
   if ( index >= numactproc )
      FatalError("AQCOMP::ActProc: Illegal index");
   return actproc[index];
}


BOOLEAN AQCOMP::AddActProc(AQPROC* proc, CARDINAL pos)
{
   CARDINAL i;
   if ( InList() == TRUE )           return FALSE;
   if ( proc == 0 )                  return FALSE;
   if ( AllowedProc(proc) == FALSE ) return FALSE;
   for ( i=0; i<numactproc; i++ )
   {
      if ( actproc[i] == proc )      return FALSE;
   }
   if ( proc->Type() == EquProc )
   {
      for ( i=0; i<numactproc; i++ )
      {
         if ( actproc[i]->Type() == EquProc )
         {
            if ( proc->Variable(0) == actproc[i]->Variable(0) )
               return FALSE;
         }
      }
   }

   if ( pos > numactproc ) pos = numactproc;
   AQPROC** newactproc = new AQPROC*[numactproc+1];
   for ( i=0; i<pos; i++ )
   {
      newactproc[i] = actproc[i];
   }
   newactproc[pos] = proc;
   for ( i=pos; i<numactproc; i++ )
   {
      newactproc[i+1] = actproc[i];
   }
   delete actproc; actproc = newactproc;
   numactproc++;
   CalcArg();
   return TRUE;
}


BOOLEAN AQCOMP::RemoveActProc(CARDINAL pos)
{
   CARDINAL i;
   if ( pos >= numactproc ) return FALSE;
   AQPROC** newactproc = new AQPROC*[numactproc-1];
   for ( i=0; i<pos; i++ )
   {
      newactproc[i] = actproc[i];
   }
   for ( i=pos+1; i<numactproc; i++ )
   {
      newactproc[i-1] = actproc[i];
   }
   delete actproc; actproc = newactproc;
   numactproc--;
   CalcArg();
   return TRUE;
}


BOOLEAN AQCOMP::RemoveActProc(AQPROC* proc)
{
   for ( CARDINAL i=0; i<numactproc; i++ )
   {
      if ( proc == actproc[i] ) return RemoveActProc(i);
   }
   return FALSE;
}


void AQCOMP::CalcRates(REAL* rates)
{
   CARDINAL i;

   for ( i=0; i<numdynstatevar; i++ )               // set defaults
   {
      rates[i] = 0.0;
   }
   for ( i=numdynstatevar; i<numstatevar; i++ )
   {
      rates[i] = (statevar[i])->Value();
   }

   for ( i=0; i<numactproc; i++ )                   // sum rates
   {
      (actproc[i])->SumRates(statevar,numstatevar,rates);
   }
   return;
}


void AQCOMP::CalcArg()
{
   CARDINAL i,j,k;
   numvararg = 0;
   numvararg += numactvar;
   for ( i=0; i<numactvar; i++ ) numvararg += (actvar[i])->NumVarArg();
   for ( i=0; i<numactproc; i++ ) numvararg += (actproc[i])->NumVarArg();
   if ( numvararg > 0 )
   {
      delete [] vararg; vararg = new AQVAR*[numvararg];
      k = 0;
      for ( i=0; i<numactvar; i++ )
      {
         vararg[k] = actvar[i]; k++;
         for ( j=0; j<actvar[i]->NumVarArg(); j++ )
         {
            vararg[k] = actvar[i]->VarArg(j); k++;
         }
      }
      for ( i=0; i<numactproc; i++ )
      {
         for ( j=0; j<actproc[i]->NumVarArg(); j++ )
         {
            vararg[k] = actproc[i]->VarArg(j); k++;
         }
      }
      if ( k != numvararg )
         FatalError("AQCOMP::CalcArg: Inconsistent argument number");
   }
   Changed();
   return;
}


const char* AQCOMP::AdvInName(CARDINAL) const
{
   FatalError("AQCOMP::AdvInName: derived version should be called");
   return 0;
}


const char* AQCOMP::AdvOutName(CARDINAL) const
{
   FatalError("AQCOMP::AdvOutName: derived version should be called");
   return 0;
}


const char* AQCOMP::DiffName(CARDINAL) const
{
   FatalError("AQCOMP::DiffName: derived version should be called");
   return 0;
}


BOOLEAN AQCOMP::AdvInExZ(CARDINAL) const
{
   FatalError("AQCOMP::AdvInExZ: derived version should be called");
   return FALSE;
}


REAL AQCOMP::AdvInZ(const REAL*, CARDINAL) const
{
   FatalError("AQCOMP::AdvInZ: derived version should be called");
   return 0;
}


REAL AQCOMP::AdvOutQ(VARSYS*, const REAL*, CARDINAL)
{
   FatalError("AQCOMP::AdvOutQ: derived version should be called");
   return 0;
}


REAL AQCOMP::AdvOutJ(VARSYS*, const REAL*, CARDINAL, const AQVAR*)
{
   FatalError("AQCOMP::AdvOutJ: derived version should be called");
   return 0;
}


REAL AQCOMP::DiffC(const REAL*, CARDINAL, const AQVAR*) const
{
   FatalError("AQCOMP::DiffC: derived version should be called");
   return 0;
}


BOOLEAN AQCOMP::Active(BOOLEAN act)
{
   if ( InList() == TRUE ) return FALSE;
   active = act;
   return TRUE;
}

   
JOBSTATUS AQCOMP::Load(std::istream& in,
                       const VARLIST* varlist,
                       const PROCLIST* proclist,
                       CARDINAL compvers)
{
   char Buffer[BufSize];

   if ( InList() == TRUE )                                      return PROBLEM;
   Changed();

   switch ( compvers )
   {
      case 1:
         if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )   return PROBLEM;
         if ( Symbol(Buffer) == FALSE )                         return PROBLEM;

         if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )   return PROBLEM;
         Description(Buffer);

         if ( LoadStart(in) == FALSE )                          return PROBLEM;
         while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
         {
            AQVAR* var = varlist->Get(Buffer);    if ( var==0 ) return PROBLEM;
            if ( AddActVar(var) == FALSE )                      return PROBLEM;
         }

         if ( LoadStart(in) == FALSE )                          return PROBLEM;
         while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
         {
            AQPROC* proc = proclist->Get(Buffer); 
                                                 if ( proc==0 ) return PROBLEM;
            if ( AddActProc(proc) == FALSE )                    return PROBLEM;
         }
         break;
      case 2:   // compartment index added
         if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )   return PROBLEM;
         if ( Symbol(Buffer) == FALSE )                         return PROBLEM;

         if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )   return PROBLEM;
         Description(Buffer);

         if ( LoadNumber(in,&cind) == FALSE )                   return PROBLEM;

         if ( LoadStart(in) == FALSE )                          return PROBLEM;
         while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
         {
            AQVAR* var = varlist->Get(Buffer);    if ( var==0 ) return PROBLEM;
            if ( AddActVar(var) == FALSE )                      return PROBLEM;
         }

         if ( LoadStart(in) == FALSE )                          return PROBLEM;
         while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
         {
            AQPROC* proc = proclist->Get(Buffer); 
                                                 if ( proc==0 ) return PROBLEM;
            if ( AddActProc(proc) == FALSE )                    return PROBLEM;
         }
         break;
      case 3:   // BOOLEAN active added
         if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )   return PROBLEM;
         if ( Symbol(Buffer) == FALSE )                         return PROBLEM;

         if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )   return PROBLEM;
         Description(Buffer);

         if ( LoadNumber(in,&cind) == FALSE )                   return PROBLEM;

         if ( LoadStart(in) == FALSE )                          return PROBLEM;
         while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
         {
            AQVAR* var = varlist->Get(Buffer);    if ( var==0 ) return PROBLEM;
            if ( AddActVar(var) == FALSE )                      return PROBLEM;
         }

         if ( LoadStart(in) == FALSE )                          return PROBLEM;
         while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
         {
            AQPROC* proc = proclist->Get(Buffer); 
                                                 if ( proc==0 ) return PROBLEM;
            if ( AddActProc(proc) == FALSE )                    return PROBLEM;
         }
         if ( LoadBoolean(in,&active) == FALSE )                return PROBLEM;
         break;
   }

   return OK;
}


JOBSTATUS AQCOMP::Save(std::ostream& out)
{
   CARDINAL i;

   if ( SaveString(out,Symbol()) == FALSE )                    return PROBLEM;

   if ( SaveString(out,Description()) == FALSE )               return PROBLEM;

   if ( SaveNumber(out,cind) == FALSE )                        return PROBLEM;

   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   for ( i=0; i<NumActVar(); i++ )
   {
      if ( SaveString(out,ActVar(i)->Symbol()) == FALSE )      return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;

   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   for ( i=0; i<NumActProc(); i++ )
   {
      if ( SaveString(out,ActProc(i)->Symbol()) == FALSE )     return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;

   if ( SaveBoolean(out,active) == FALSE )                     return PROBLEM;

   return FILEIO::Save(out);
}


JOBSTATUS AQCOMP::Write(std::ostream& out, BOOLEAN sh)
{
   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   CARDINAL pos3 = pos1+2;
   CARDINAL pos4 = pos3+2;
   PrintIndent(pos2-1);
   CARDINAL i;

   if ( Print(out,Symbol(),1) == FALSE )                       return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( sh == FALSE )
   {
      if ( Print(out,ini.T(3),pos1) == FALSE )                 return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Description() != 0 )
      {
         if ( strlen(Description()) != 0 )
         {
            if ( Print(out,Description(),pos2) == FALSE )      return PROBLEM;
         }
      }

      if ( Print(out,ini.T(2),pos1) == FALSE )                 return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Print(out,TypeName(),pos2) == FALSE )               return PROBLEM;

      if ( Print(out,ini.T(186),pos1) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                           return PROBLEM;
      if ( Print(out,CmpIndex(),pos2) == FALSE )               return PROBLEM;
   }
   else
   {
      if ( Print(out,TypeName(),pos1) == FALSE )               return PROBLEM;
   }
   if ( Print(out,ini.T(36),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( NumActVar() > 0 )
   {
      if ( Print(out,ActVar(0)->Symbol(),pos2) == FALSE )      return PROBLEM;
      for ( i=1; i<NumActVar(); i++ )
      {
         if ( Print(out,", ") == FALSE )                       return PROBLEM;
         if ( Print(out,ActVar(i)->Symbol()) == FALSE )        return PROBLEM;
      }
   }

   if ( Print(out,ini.T(16),pos1) == FALSE )                   return PROBLEM;
   if ( Print(out,":") == FALSE )                              return PROBLEM;
   if ( NumActProc() > 0 )
   {
      if ( Print(out,ActProc(0)->Symbol(),pos2) == FALSE )     return PROBLEM;
      for ( i=1; i<NumActProc(); i++ )
      {
         if ( Print(out,", ") == FALSE )                       return PROBLEM;
         if ( Print(out,ActProc(i)->Symbol()) == FALSE )       return PROBLEM;
      }
   }

   return OK;
}


//////////////////////////////////////////////////////////////////////////////
