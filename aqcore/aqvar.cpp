////////////////////////////////   aqvar.C   /////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    23.07.91    Peter Reichert
// revisions:   17.09.91    Peter Reichert    AQVAR::AQVAR(const AQVAR&)
//                                            constructor added
//              12.01.92    Peter Reichert    storing of values added
//              27.10.92    Peter Reichert    redesign of VarArg(CARDINAL)
//
//////////////////////////////////////////////////////////////////////////////

#include "aqvar.h"

//////////////////////////////////////////////////////////////////////////////

const CARDINAL BufSize = 1024;

const char* timeUnits[]  = {"s","sec","Sec",
                            "m","min","Min",
                            "h","std","Std",
                            "d","day","days","Tag","Tage"};

const REAL timeFactors[] = {1,1,1,
                            60,60,60,
                            3600,3600,3600,
                            86400,86400,86400,86400,86400};

static CARDINAL numTimeUnits   = sizeof(timeUnits)/sizeof(char*);
static CARDINAL numTimeFactors = sizeof(timeFactors)/sizeof(REAL);

const char* spaceUnits[] = {"mm","cm","dm","m","km"};

const REAL spaceFactors[] = {0.001,0.01,0.1,1,1000};

static CARDINAL numSpaceUnits   = sizeof(spaceUnits)/sizeof(char*);
static CARDINAL numSpaceFactors = sizeof(spaceFactors)/sizeof(REAL);

//////////////////////////////////////////////////////////////////////////////

INIT AQVAR::ini(initfile,"VARSYS","var_text_",10,10,10,100);


BOOLEAN AQVAR::reset = FALSE;


CARDINAL AQVAR::number = 0;


void AQVAR::init()
{
   if ( numTimeUnits != numTimeFactors )
      FatalError("AQVAR::init: inconsistent time units");
   if ( numSpaceUnits != numSpaceFactors )
      FatalError("AQVAR::init: inconsistent space units");
   unit      = 0;
   lastvalue = 0.0;
   evaluated = FALSE;
   number++;
}


AQVAR::AQVAR()
{
   init();
   char Buf1[BufSize]; char Buf2[BufSize];
   strcpy(Buf1,ini.T(1));
   NumToString(number,Buf2,BufSize);
   strcat(Buf1,Buf2);
   if ( Symbol(Buf1) == FALSE )
      FatalError("AQVAR::AQVAR: illegal symbol");
   return;
}


AQVAR::AQVAR(const AQVAR& var) : SYMBOL(var)
{
   init();
   Unit(var.unit);
}


AQVAR::AQVAR(const AQVAR* var) : SYMBOL(var)
{
   init();
   if ( var != 0 )
   {
      Unit(var->unit);
   }
   else
   {
      char Buf1[BufSize]; char Buf2[BufSize];
      strcpy(Buf1,ini.T(1));
      NumToString(number,Buf2,BufSize);
      strcat(Buf1,Buf2);
      if ( Symbol(Buf1) == FALSE )
         FatalError("AQVAR::AQVAR: illegal symbol");
   }
}


AQVAR::~AQVAR()
{
   delete unit; unit=0;
   if ( number == 0 )
      FatalError("AQVAR::~AQVAR: already all vars destructed");
   number--;
}


CARDINAL AQVAR::NumTimeUnits()
{
   return numTimeUnits;
}


const char* AQVAR::TimeUnits(CARDINAL index)
{
   if ( index >= numTimeUnits )
      FatalError("AQVAR::TimeUnits: Illegal index");
   return timeUnits[index];
}


BOOLEAN AQVAR::TimeUnit(const char* u)
{
   for ( CARDINAL i=0; i<numTimeUnits; i++ )
   {
      if ( strcmp(u,timeUnits[i]) == 0 ) return TRUE;
   }
   return FALSE;
}


REAL AQVAR::TimeUnitFactor(const char* u)
{
   for ( CARDINAL i=0; i<numTimeUnits; i++ )
   {
      if ( strcmp(u,timeUnits[i]) == 0 ) return timeFactors[i];
   }
   return 1.0;
}


CARDINAL AQVAR::NumSpaceUnits()
{
   return numSpaceUnits;
}


const char* AQVAR::SpaceUnits(CARDINAL index)
{
   if ( index >= numSpaceUnits )
      FatalError("AQVAR::SpaceUnits: Illegal index");
   return spaceUnits[index];
}


BOOLEAN AQVAR::SpaceUnit(const char* u)
{
   for ( CARDINAL i=0; i<numSpaceUnits; i++ )
   {
      if ( strcmp(u,spaceUnits[i]) == 0 ) return TRUE;
   }
   return FALSE;
}


REAL AQVAR::SpaceUnitFactor(const char* u)
{
   for ( CARDINAL i=0; i<numSpaceUnits; i++ )
   {
      if ( strcmp(u,spaceUnits[i]) == 0 ) return spaceFactors[i];
   }
   return 1.0;
}


BOOLEAN AQVAR::AllowedVar()
{
   return TRUE;
}


REAL AQVAR::Value()
{
   if ( InList() == TRUE )      // efficient implementation for variables
   {                            // belonging to the system of variables
      if ( evaluated == FALSE )
      {
         lastvalue = Evaluate();
         evaluated = TRUE;
         reset = FALSE;
      }
      return lastvalue;
   }
   else                         // variables not within the system of vari-
   {                            // ables (directly defined within processes,
      return Evaluate();        // compartments or connections) have to be
   }                            // evaluated, since these are not reset
}


void AQVAR::Unit(const char* u)
{
   Changed();
   delete unit; unit = 0;
   if ( u == 0 ) return;
   unit = new char[strlen(u)+1];
   strcpy(unit,u);
   for ( CARDINAL i=0; i<strlen(unit); i++ )
   {
      if ( unit[i] == '{' ) unit[i] = '[';
      if ( unit[i] == '}' ) unit[i] = ']';
      if ( unit[i] <  ' ' ) unit[i] = ' ';
   }
}


BOOLEAN AQVAR::SensActive(BOOLEAN act)
{
   if ( act == TRUE ) return FALSE;
   return TRUE;
}


BOOLEAN AQVAR::SetSensValue(BOOLEAN b)
{
   if ( b == TRUE ) return FALSE;
   return TRUE;
}


AQVAR* AQVAR::VarArg(CARDINAL) const
{
   FatalError("AQVAR::VarArg: Illegal index");
   return 0;
}


BOOLEAN AQVAR::Arg(const AQVAR* var) const
{
   for ( CARDINAL i=0; i<NumVarArg(); i++ )
   {
      if ( VarArg(i) == var )            return TRUE;
      if ( VarArg(i)->Arg(var) == TRUE ) return TRUE;
   }
   return FALSE;
}


BOOLEAN AQVAR::Arg(const AQCOMP* com) const
{
   for ( CARDINAL i=0; i<NumCompArg(); i++ )
   {
      if ( CompArg(i) == com ) return TRUE;
   }
   return FALSE;
}


BOOLEAN AQVAR::ArgVarType(AQVARTYPE type) const
{
   if ( Type() == type ) return TRUE;
   for ( CARDINAL i=0; i<NumVarArg(); i++ )
   {
      if ( VarArg(i)->ArgVarType(type) == TRUE ) return TRUE;
   }
   return FALSE;
}


BOOLEAN AQVAR::ArgProgVarType(PROGVARTYPE type, const AQVAR* besides) const
{
   for ( CARDINAL i=0; i<NumVarArg(); i++ )
   {
      if ( VarArg(i) != besides )
      {
         if ( VarArg(i)->ArgProgVarType(type,besides) == TRUE ) return TRUE;
      }
   }
   return FALSE;
}


void AQVAR::ReplVarArg(AQVAR*, AQVAR*)
{
   return;
}


BOOLEAN AQVAR::ReplaceVarArg(AQVAR* oldvar, AQVAR* newvar)
{
   if ( InList() == TRUE ) return FALSE;
   ReplVarArg(oldvar,newvar);
   return TRUE;
}


BOOLEAN AQVAR::VarArgSaved()
{
   for ( CARDINAL i=0; i<NumVarArg(); i++ )
   {
      if ( VarArg(i)->Saved() == FALSE )       return FALSE;
      if ( VarArg(i)->VarArgSaved() == FALSE ) return FALSE;
   }
   return TRUE;
}


AQCOMP* AQVAR::CompArg(CARDINAL) const
{
   FatalError("AQVAR::CompArg: Illegal index");
   return 0;
}


void AQVAR::ReplCompArg(AQCOMP*, AQCOMP*)
{
   return;
}


BOOLEAN AQVAR::ReplaceCompArg(AQCOMP* oldcomp, AQCOMP* newcomp)
{
   if ( InList() == TRUE ) return FALSE;
   ReplCompArg(oldcomp,newcomp);
   return TRUE;
}


JOBSTATUS AQVAR::Load(std::istream& in)
{
   char Buffer[1024];
   if ( InList() == TRUE )                              return PROBLEM;
   Changed();
   if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE ) return PROBLEM;
   if ( Symbol(Buffer) == FALSE )                       return PROBLEM;
   if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE ) return PROBLEM;
   Description(Buffer);
   if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE ) return PROBLEM;
   Unit(Buffer);
   return OK;
}


JOBSTATUS AQVAR::Save(std::ostream& out)
{
   if ( SaveString(out,Symbol()) == FALSE )      return PROBLEM;
   if ( SaveString(out,Description()) == FALSE ) return PROBLEM;
   if ( SaveString(out,Unit()) == FALSE )        return PROBLEM;
   return OK;
}


JOBSTATUS AQVAR::Write(std::ostream& out, BOOLEAN sh)
{
   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   PrintIndent(pos2-1);

   if ( Print(out,Symbol(),1) == FALSE )               return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;
   if ( sh == TRUE ) return OK;
   if ( Print(out,ini.T(44),pos1) == FALSE )           return PROBLEM;
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

   if ( Print(out,ini.T(6),pos1) == FALSE )            return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;
   if ( Unit() != 0 )
   {
      if ( strlen(Unit()) > 0 )
      {
         if ( Print(out,Unit(),pos2) == FALSE )        return PROBLEM;
      }
   }

   return OK;
}


void AQVAR::Test()
{
   *ctest << "\nclass AQVAR:      number      = " << number;
   *ctest << "\n                  unit        = ";
   if ( unit != 0 ) *ctest << unit;
   SYMBOL::Test();
}

//////////////////////////////////////////////////////////////////////////////
