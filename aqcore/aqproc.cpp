////////////////////////////////   aqproc.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    22.08.91    Peter Reichert
// revisions:   02.12.91    Peter Reichert    ?COMP::?COMP(const ?COMP&)
//                                            constructor added
//              20.01.92    Peter Reichert    members variable and expression
//                                            from DYNPROC to AQPROC moved
//              27.10.92    Peter Reichert    redesign of VarArg(CARDINAL)
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "aqproc.h"

//////////////////////////////////////////////////////////////////////////////

const CARDINAL BufSize = 1024;

//////////////////////////////////////////////////////////////////////////////

INIT AQPROC::ini(initfile,"PROCSYS","proc_txt_",10,10,10,20);


CARDINAL AQPROC::number = 0;


void AQPROC::init()
{
   numvar     = 0;
   variable   = 0;
   expression = 0;
   numvararg  = 0;
   vararg     = 0;
   number++;
}


AQPROC::AQPROC()
{
   init();
   char Buf1[BufSize]; char Buf2[BufSize];
   strcpy(Buf1,ini.T(1));
   NumToString(number,Buf2,BufSize);
   strcat(Buf1,Buf2);
   if ( Symbol(Buf1) == FALSE )
      FatalError("AQPROC::AQPROC: illegal symbol");
   return;
}


AQPROC::AQPROC(const AQPROC& proc) : SYMBOL(proc)
{
   init();
   if ( proc.numvar > 0 )
   {
      numvar     = proc.numvar;
      variable   = new AQVAR*[numvar];
      expression = new FORMVAR*[numvar];
      for ( CARDINAL i=0; i<numvar; i++ )
      {
         variable[i]   = proc.variable[i];
         expression[i] = new FORMVAR(proc.expression[i]);
      }
   }
   if ( proc.numvararg > 0 )
   {
      numvararg = proc.numvararg;
      vararg = new AQVAR*[numvararg];
      for ( CARDINAL i=0; i<numvararg; i++ )
      {
         vararg[i] = proc.vararg[i];
      }
   }
}


AQPROC::AQPROC(const AQPROC* proc) : SYMBOL(proc)
{
   init();
   if ( proc != 0 )
   {
      if ( proc->numvar > 0 )
      {
         numvar     = proc->numvar;
         variable   = new AQVAR*[numvar];
         expression = new FORMVAR*[numvar];
         for ( CARDINAL i=0; i<numvar; i++ )
         {
            variable[i]   = proc->variable[i];
            expression[i] = new FORMVAR(proc->expression[i]);
         }
      }
      if ( proc->numvararg > 0 )
      {
         numvararg = proc->numvararg;
         vararg = new AQVAR*[numvararg];
         for ( CARDINAL i=0; i<numvararg; i++ )
         {
            vararg[i] = proc->vararg[i];
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
         FatalError("AQPROC::AQPROC: illegal symbol");
   }
}


AQPROC::~AQPROC()
{
   delete variable;   variable = 0;
   for ( CARDINAL i=0; i<numvar; i++ ) delete expression[i];
   delete expression; expression = 0;
   numvar = 0;
   delete [] vararg; vararg = 0;
   numvararg = 0;
   if ( number == 0 )
      FatalError("AQPROC::~AQPROC: already all procs destructed");
   number--;
}


AQVAR* AQPROC::Variable(CARDINAL index) const
{
   if ( index >= numvar )
      FatalError("AQPROC::Variable: Illegal index");
   return variable[index];
}


const FORMVAR* AQPROC::Expression(CARDINAL index) const
{
   if ( index >= numvar )
      FatalError("AQPROC::Expression: Illegal index");
   return expression[index];
}


AQVAR* AQPROC::VarArg(CARDINAL index) const
{
   if ( index >= numvararg )
      FatalError("AQPROC::VarArg: Illegal index");
   return vararg[index];
}


BOOLEAN AQPROC::Arg(const AQVAR* var) const
{
   for ( CARDINAL i=0; i<numvararg; i++ )
   {
      if ( vararg[i] == var )            return TRUE;
      if ( vararg[i]->Arg(var) == TRUE ) return TRUE;
   }
   return FALSE;
}


void AQPROC::ReplVar(AQVAR* oldvar, AQVAR* newvar)
{
   for ( CARDINAL i=0; i<numvar; i++ )
   {
      if ( variable[i] == oldvar ) variable[i] = newvar;
      expression[i]->ReplaceVarArg(oldvar,newvar);
   }
   CalcArg();
}


BOOLEAN AQPROC::ReplaceVar(AQVAR* oldvar, AQVAR* newvar)
{
   if ( InList() == TRUE ) return FALSE;
   ReplVar(oldvar,newvar); return TRUE;
}


void AQPROC::ExchVar(AQVAR* var1, AQVAR* var2)
{
   AQVAR* v = new CONSTVAR;
   for ( CARDINAL i=0; i<numvar; i++ )
   {
      if ( variable[i] == var1 )
      {
         variable[i] = var2;
      }
      else
      {
         if ( variable[i] == var2 ) variable[i] = var1;
      }
      variable[i]->ReplaceVarArg(var1,v);
      variable[i]->ReplaceVarArg(var2,var1);
      variable[i]->ReplaceVarArg(v,var2);
   }
   delete v;
   CalcArg();
}


BOOLEAN AQPROC::ExchangeVar(AQVAR* var1, AQVAR* var2)
{
   if ( InList() == TRUE ) return FALSE;
   ExchVar(var1,var2);    return TRUE;
}


void AQPROC::CalcArg()
{
   CARDINAL i,j,k;
   numvararg = numvar;
   for ( i=0; i<numvar; i++ ) numvararg += expression[i]->NumVarArg();
   delete [] vararg; vararg = new AQVAR*[numvararg];
   k = 0;
   for ( i=0; i<numvar; i++ )
   {
      vararg[k] = variable[i]; k++;
      for ( j=0; j<expression[i]->NumVarArg(); j++ )
      {
         vararg[k] = expression[i]->VarArg(j); k++;
      }
   }
   if ( k != numvararg )
      FatalError("AQPROC::CalcArg: Inconsistent argument number");
   return;
}


JOBSTATUS AQPROC::Load(std::istream& in)
{
   char Buffer[BufSize];
   if ( InList() == TRUE )                              return PROBLEM;
   Changed();
   if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE ) return PROBLEM;
   if ( Symbol(Buffer) == FALSE )                       return PROBLEM;
   if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE ) return PROBLEM;
   Description(Buffer);
   return OK;
}


JOBSTATUS AQPROC::Save(std::ostream& out)
{
   if ( SaveString(out,Symbol()) == FALSE )      return PROBLEM;
   if ( SaveString(out,Description()) == FALSE ) return PROBLEM;
   return FILEIO::Save(out);
}


JOBSTATUS AQPROC::Write(std::ostream& out, BOOLEAN sh)
{
   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   PrintIndent(pos2-1);

   if ( Print(out,Symbol(),1) == FALSE )               return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;
   if ( sh == TRUE ) return OK;
   if ( Print(out,ini.T(3),pos1) == FALSE )            return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;
   if ( Description() != 0 )
   {
      if ( strlen(Description()) != 0 )
      {
         if ( Print(out,Description(),pos2) == FALSE ) return PROBLEM;
      }
   }

   if ( Print(out,ini.T(2),pos1) == FALSE )            return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;
   if ( Print(out,TypeName(),pos2) == FALSE )          return PROBLEM;

   return OK;
}

//////////////////////////////////////////////////////////////////////////////

void DYNPROC::init()
{
   rate = 0;
}


DYNPROC::DYNPROC()
{
   init();
}


DYNPROC::DYNPROC(const DYNPROC& proc) : AQPROC(proc)
{
   rate = new FORMVAR(proc.rate);
}


DYNPROC::DYNPROC(const AQPROC* proc) : AQPROC(proc)
{
   if ( proc == 0 )
   {
      init();
   }
   else
   {
      if ( proc->Type() == DynProc )
      {
         DYNPROC* dp = (DYNPROC*)proc;
         rate = new FORMVAR(dp->rate);
      }
      else
      {
         delete variable;   variable = 0;
         for ( CARDINAL i=0; i<numvar; i++ ) delete expression[i];
         delete expression; expression = 0;
         numvar = 0;
         delete [] vararg; vararg = 0;
         numvararg = 0;
         init();
      }
   }
}


DYNPROC::~DYNPROC()
{
   delete rate;
}


const char* DYNPROC::TypeName() const
{
   return ini.T(8);
}


void DYNPROC::SumRates(STATEVAR* const* statevar,
                       CARDINAL numstatevar,
                       REAL* rates)
{
   if ( numvar == 0 ) return;
   if ( rate == 0 ) FatalError("DYNPROC::SumRates: rate not defined");
   REAL r = rate->Value();
   for ( CARDINAL i=0; i<numstatevar; i++ )
   {
      for ( CARDINAL j=0; j<numvar; j++ )
      {
         if ( variable[j] == statevar[i] )
         {
            if ( statevar[i]->StateVarType() != EquStateVar )
            {
               rates[i] += expression[j]->Value() * r;
            }
            break;
         }
      }
   }
   return;
}


BOOLEAN DYNPROC::RateLaw(const VARLIST* varlist,
                         const char* inpline,
                         char* parseerrorline,
                         CARDINAL& numparseerrors)
{
   char buffer[1024];
   if ( InList() == TRUE )   return FALSE;
   if ( inpline == 0 )       return FALSE;
   if ( inpline[0] == '\0' ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("ratelaw");
   strcpy(buffer,"rate_");
   strcat(buffer,Symbol());
   formvar->Symbol(buffer);
   numparseerrors = formvar->Parse(inpline,varlist,parseerrorline);
   if ( numparseerrors != 0 )
   {
      delete formvar; return FALSE;
   }
   delete rate; rate = formvar;
   CalcArg();
   return TRUE;
}


const FORMVAR* DYNPROC::StoichCoeff(CARDINAL index) const
{
   if ( index >= numvar )
      FatalError("DYNPROC::StoichCoeff: Illegal index");
   return expression[index];
}


BOOLEAN DYNPROC::AddStoichCoeff(const VARLIST* varlist,
                                AQVAR* var,
                                const char* inpline,
                                char* parseerrorline,
                                CARDINAL& numparseerrors,
                                CARDINAL pos)
{
   char buffer[1024];
   numparseerrors = 0;
   if ( InList() == TRUE )             return FALSE;
   if ( inpline == 0 )                 return FALSE;
   if ( inpline[0] == '\0' )           return FALSE;
   if ( varlist->Exist(var) == FALSE ) return FALSE;
   if ( pos > numvar ) pos = numvar;
   CARDINAL i;
   for ( i=0; i<numvar; i++ )
   {
      if ( variable[i] == var ) return FALSE;
   }
   FORMVAR* coeff = new FORMVAR;
   coeff->Symbol("stoichcoeff");
   strcpy(buffer,"stoich_");
   strcat(buffer,Symbol());
   coeff->Symbol(buffer);
   numparseerrors = coeff->Parse(inpline,varlist,parseerrorline);
   if ( numparseerrors != 0 )
   {
      delete coeff;                    return FALSE;
   }
   AQVAR** newvariable = new AQVAR*[numvar+1];
   FORMVAR** newexpression = new FORMVAR*[numvar+1];
   for ( i=0; i<pos; i++ )
   {
      newvariable[i]   = variable[i];
      newexpression[i] = expression[i];
   }
   newvariable[pos]   = var;
   newexpression[pos] = coeff;
   for ( i=pos; i<numvar; i++ )
   {
      newvariable[i+1]   = variable[i];
      newexpression[i+1] = expression[i];
   }
   delete variable;   variable = newvariable;
   delete expression; expression = newexpression;
   numvar++;
   CalcArg();
   return TRUE;
}


BOOLEAN DYNPROC::ReplaceStoichCoeff(const VARLIST* varlist,
                                    AQVAR* var,
                                    const char* inpline,
                                    char* parseerrorline,
                                    CARDINAL& numparseerrors,
                                    CARDINAL pos)
{
   char buffer[1024];
   numparseerrors = 0;
   if ( InList() == TRUE )              return FALSE;
   if ( inpline == 0 )                  return FALSE;
   if ( inpline[0] == '\0' )            return FALSE;
   if ( varlist->Exist(var) == FALSE )  return FALSE;
   if ( pos > numvar )                  return FALSE;
   CARDINAL i;
   for ( i=0; i<numvar; i++ )
   {
      if ( i!=pos && variable[i]==var ) return FALSE;
   }
   FORMVAR* coeff = new FORMVAR;
   coeff->Symbol("stoichcoeff");
   strcpy(buffer,"stoich_");
   strcat(buffer,Symbol());
   coeff->Symbol(buffer);
   numparseerrors = coeff->Parse(inpline,varlist,parseerrorline);
   if ( numparseerrors != 0 )
   {
      delete coeff;    return FALSE;
   }
   delete expression[pos];
   variable[pos]   = var;
   expression[pos] = coeff;
   CalcArg();
   return TRUE;
}


BOOLEAN DYNPROC::DeleteStoichCoeff(CARDINAL pos)
{
   if ( InList() == TRUE ) return FALSE;
   if ( pos >= numvar )    return FALSE;
   CARDINAL i;
   AQVAR** newvariable  = new AQVAR*[numvar-1];
   FORMVAR** newexpression = new FORMVAR*[numvar-1];
   for ( i=0; i<pos; i++ )
   {
      newvariable[i]   = variable[i];
      newexpression[i] = expression[i];
   }
   for ( i=pos+1; i<numvar; i++ )
   {
      newvariable[i-1]   = variable[i];
      newexpression[i-1] = expression[i];
   }
   delete expression[pos];
   delete variable;   variable   = newvariable;
   delete expression; expression = newexpression;
   numvar--;
   CalcArg();
   return TRUE;
}


void DYNPROC::ReplVar(AQVAR* oldvar, AQVAR* newvar)
{
   AQPROC::ReplVar(oldvar,newvar);
   rate->ReplaceVarArg(oldvar,newvar);
   CalcArg();
}


void DYNPROC::ExchVar(AQVAR* var1, AQVAR* var2)
{
   AQPROC::ExchVar(var1,var2);
   AQVAR* v = new CONSTVAR;
   rate->ReplaceVarArg(var1,v);
   rate->ReplaceVarArg(var2,var1);
   rate->ReplaceVarArg(v,var2);
   delete v;
   CalcArg();
}


void DYNPROC::CalcArg()
{
   AQPROC::CalcArg();
   if ( rate != 0 )
   {
      CARDINAL num = numvararg+rate->NumVarArg();
      if ( num > numvararg )
      {
         AQVAR** newarg = new AQVAR*[num];
         CARDINAL i;
         for ( i=0; i<numvararg; i++ ) newarg[i] = vararg[i];
         for ( i=0; i<rate->NumVarArg(); i++ )
            newarg[numvararg+i] = rate->VarArg(i);
         delete [] vararg; vararg = newarg; numvararg = num;
      }
   }
   return;
}


JOBSTATUS DYNPROC::Load(std::istream& in, const VARLIST* varlist)
{
   char Buffer[BufSize]; char Buffer1[BufSize];
   CARDINAL i;

   if ( LoadStart(in) == FALSE )                              return PROBLEM;
   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )                   return PROBLEM;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.0
         {
            JOBSTATUS status = AQPROC::Load(in);
                                       if ( status != OK )    return status;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                              return PROBLEM;
            if ( RateLaw(varlist,Buffer,Buffer1,i) == FALSE ) return PROBLEM;
            if ( LoadStart(in) == FALSE )                     return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               AQVAR* var = varlist->Get(Buffer);
                                              if ( var == 0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                              return PROBLEM;
               if ( AddStoichCoeff(varlist,var,Buffer,Buffer1,i) == FALSE )
                                                              return PROBLEM;
            }
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                                return PROBLEM;
   return OK;
}


JOBSTATUS DYNPROC::Save(std::ostream& out)
{
   char Buffer[BufSize];
   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   INTEGER aquavers = 1;
   if ( SaveNumber(out,aquavers) == FALSE )                    return PROBLEM;
   JOBSTATUS status = AQPROC::Save(out);   if ( status != OK ) return status;
   if ( rate->UnParse(Buffer,sizeof(Buffer)-1) != 0 )          return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;
   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   for ( CARDINAL i=0; i<numvar; i++ )
   {
      if ( SaveString(out,(variable[i])->Symbol()) == FALSE )  return PROBLEM;
      if ( expression[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                               return PROBLEM;
      if ( SaveString(out,Buffer) == FALSE )                   return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;

   if ( SaveEnd(out) == FALSE )                                return PROBLEM;
   return OK;
}


JOBSTATUS DYNPROC::Write(std::ostream& out, BOOLEAN sh)
{
   JOBSTATUS status = AQPROC::Write(out,sh); 
                                          if ( status != OK ) return status;

   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   CARDINAL pos3 = pos1+2;
   CARDINAL pos4 = pos3+2;
   char Buffer[BufSize];
   if ( sh == TRUE )
   {
      PrintIndent(pos1-1);
      if ( rate->UnParse(Buffer,sizeof(Buffer)) != 0 )        return PROBLEM;
      if ( Print(out,Buffer,pos1) == FALSE )                  return PROBLEM;
      if ( numvar > 0 )
      {
         PrintIndent(pos1+2);
         for ( CARDINAL i=0; i<numvar; i++ )
         {
            if ( Print(out,variable[i]->Symbol(),pos1+3) == FALSE )
		    	                                              return PROBLEM;
            if ( Print(out," : ") == FALSE )                  return PROBLEM;
            if ( expression[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                              return PROBLEM;
            if ( Print(out,Buffer) == FALSE )                 return PROBLEM;
         }
      }
   }
   else
   {
      PrintIndent(pos2-1);
      if ( Print(out,ini.T(9),pos1) == FALSE )                return PROBLEM;
      if ( Print(out,":") == FALSE )                          return PROBLEM;
      if ( rate->UnParse(Buffer,sizeof(Buffer)) != 0 )        return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                  return PROBLEM;
      PrintIndent(pos2-1);
      if ( numvar > 0 )
      {
         PrintIndent(pos4-1);
         if ( Print(out,ini.T(10),pos1) == FALSE )            return PROBLEM;
         if ( Print(out,":") == FALSE )                       return PROBLEM;
         if ( Print(out,ini.T(11),pos3) == FALSE )            return PROBLEM;
         if ( Print(out," : ") == FALSE )                     return PROBLEM;
         if ( Print(out,ini.T(12)) == FALSE )                 return PROBLEM;
         for ( CARDINAL i=0; i<numvar; i++ )
         {
            if ( Print(out,variable[i]->Symbol(),pos3) == FALSE )
		    	                                              return PROBLEM;
            if ( Print(out," : ") == FALSE )                  return PROBLEM;
            if ( expression[i]->UnParse(Buffer,sizeof(Buffer)-1) != 0 )
                                                              return PROBLEM;
            if ( Print(out,Buffer) == FALSE )                 return PROBLEM;
         }
      }
   }

   return OK;
}


//////////////////////////////////////////////////////////////////////////////

void EQUPROC::init()
{
}


EQUPROC::EQUPROC()
{
   init();
}


EQUPROC::EQUPROC(const EQUPROC& proc) : AQPROC(proc)
{
}


EQUPROC::EQUPROC(const AQPROC* proc) : AQPROC(proc)
{
   if ( proc == 0 )
   {
      init();
   }
   else
   {
      if ( proc->Type() == EquProc )
      {
      }
      else
      {
         delete variable;   variable = 0;
         for ( CARDINAL i=0; i<numvar; i++ ) delete expression[i];
         delete expression; expression = 0;
         numvar = 0;
         delete [] vararg; vararg = 0;
         numvararg = 0;
         init();
      }
   }
}


EQUPROC::~EQUPROC()
{
}


const char* EQUPROC::TypeName() const
{
   return ini.T(5);
}


void EQUPROC::SumRates(STATEVAR* const* statevar,
                       CARDINAL numstatevar,
                       REAL* rates)
{
   if ( numvar == 0 ) return;
   if ( numvar > 1 ) FatalError("EQUPROC::SumRates: numvar > 1");
   for ( CARDINAL i=0; i<numstatevar; i++ )
   {
      if ( variable[0] == statevar[i] )
      {
         if ( statevar[i]->StateVarType() == EquStateVar )
         {
            rates[i] = expression[0]->Value();
         }
         return;
      }
   }
   return;
}


const AQVAR* EQUPROC::Variable() const
{
   if ( numvar == 0 ) return 0;
   return variable[0];
}


const FORMVAR* EQUPROC::Equation() const
{
   if ( numvar == 0 ) return 0;
   return expression[0];
}


BOOLEAN EQUPROC::Equation(AQVAR* var,
                          const VARLIST* varlist,
                          const char* inpline,
                          char* parseerrorline,
                          CARDINAL& numparseerrors)
{
   if ( InList() == TRUE )           return FALSE;
   if ( inpline == 0 )               return FALSE;
   if ( inpline[0] == '\0' )         return FALSE;
   if ( varlist->Exist(var)==FALSE ) return FALSE;
   FORMVAR* formvar = new FORMVAR;
   formvar->Symbol("equation");
   numparseerrors = formvar->Parse(inpline,varlist,parseerrorline);
   if ( numparseerrors != 0 )
   {
      delete formvar; return FALSE;
   }
   if ( formvar->Arg(var) == FALSE )
   {
      delete formvar; return FALSE;
   }
   if ( numvar == 0 )
   {
      variable   = new AQVAR*[1];
      expression = new FORMVAR*[1];
      expression[0] = 0;
      numvar = 1;
   }
   if ( numvar != 1 ) FatalError("EQUPROC::Equation: numvar != 1");
   delete expression[0]; expression[0] = formvar;
   variable[0] = var;
   CalcArg();
   return TRUE;
}


JOBSTATUS EQUPROC::Load(std::istream& in, const VARLIST* varlist)
{
   CARDINAL i;
   char Buffer[BufSize]; char Buffer1[BufSize];

   if ( LoadStart(in) == FALSE )                           return PROBLEM;
   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )                return PROBLEM;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.0
         {
            JOBSTATUS status = AQPROC::Load(in);
                                       if ( status != OK ) return status;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                           return PROBLEM;
            AQVAR* var = varlist->Get(Buffer);
            if ( var == 0 )                                return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                           return PROBLEM;
            if ( Equation(var,varlist,Buffer,Buffer1,i) == FALSE )
                                                           return PROBLEM;
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                             return PROBLEM;
   return OK;
}


JOBSTATUS EQUPROC::Save(std::ostream& out)
{
   char Buffer[BufSize];

   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   INTEGER aquavers = 1;
   if ( SaveNumber(out,aquavers) == FALSE )                 return PROBLEM;
   JOBSTATUS status = AQPROC::Save(out);   if ( status != OK ) return status;
   if ( SaveString(out,variable[0]->Symbol()) == FALSE )       return PROBLEM;
   if ( expression[0]->UnParse(Buffer,sizeof(Buffer)-1) != 0 ) return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                      return PROBLEM;

   if ( SaveEnd(out) == FALSE )                                return PROBLEM;
   return OK;
}


JOBSTATUS EQUPROC::Write(std::ostream& out, BOOLEAN sh)
{
   JOBSTATUS status = AQPROC::Write(out,sh); 
                                          if ( status != OK ) return status;

   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   char Buffer[BufSize];
   if ( sh == TRUE )
   {
      PrintIndent(pos1-1);
      if ( Print(out,variable[0]->Symbol(),pos1) == FALSE )   return PROBLEM;
      if ( Print(out," : 0 = ") == FALSE )                    return PROBLEM;
      if ( expression[0]->UnParse(Buffer,sizeof(Buffer)) != 0 )
                                                              return PROBLEM;
      if ( Print(out,Buffer) == FALSE )                       return PROBLEM;
   }
   else
   {
      PrintIndent(pos2-1);
      if ( Print(out,ini.T(6),pos1) == FALSE )                return PROBLEM;
      if ( Print(out,":") == FALSE )                          return PROBLEM;
      if ( Print(out,variable[0]->Symbol()) == FALSE )        return PROBLEM;
      if ( Print(out,ini.T(7),pos1) == FALSE )                return PROBLEM;
      if ( expression[0]->UnParse(Buffer,sizeof(Buffer)) != 0 )
                                                              return PROBLEM;
      if ( Print(out,":") == FALSE )                          return PROBLEM;
      if ( Print(out,"0 = ",pos2) == FALSE )                  return PROBLEM;
      if ( Print(out,Buffer) == FALSE )                       return PROBLEM;
   }

   return OK;
}

//////////////////////////////////////////////////////////////////////////////

