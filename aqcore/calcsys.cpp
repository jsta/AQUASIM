////////////////////////////////   calcsys.C   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    15.03.93    Peter Reichert
// revisions:   20.07.97    Peter Reichert    changed from sens to calc
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "calcsys.h"

//////////////////////////////////////////////////////////////////////////////

const char* CalcsysIdentifier = "CALCSYS";
const char* CalcIdentifier = "CALC";

const CARDINAL BufSize = 1024;

//////////////////////////////////////////////////////////////////////////////

CARDINAL CALC::number = 0;


INIT CALC::ini(initfile,"CALCSYS","calc_txt_",10,10,10,30);


void CALC::init()
{
   calcnum = 0;
   inittime = 0;
   initsteady = FALSE;
   numstepseries = 0;
   stepsize = 0;
   numsteps = 0;
   simactive = TRUE;
   sensactive = FALSE;
   number++;
}


CALC::CALC()
{
   init();
   char Buf1[BufSize]; char Buf2[BufSize];
//   strcpy(Buf1,ini.T(1));
   strcpy(Buf1,"calc");
   NumToString(number,Buf2,BufSize);
   strcat(Buf1,Buf2);
   if ( Symbol(Buf1) == FALSE )
      FatalError("CALC::CALC: illegal symbol");
   return;
}


CALC::CALC(const CALC* calc) : SYMBOL(calc)
{
   init();
   if ( calc != 0 )
   {
      calcnum = calc->calcnum;
      inittime = calc->inittime;
      initsteady = calc->initsteady;
      numstepseries = calc->numstepseries;
      if ( numstepseries == 0 )
      {
         stepsize = 0;
         numsteps = 0;
      }
      else
      {
         stepsize = new REAL[numstepseries];
         numsteps = new CARDINAL[numstepseries];
         for ( CARDINAL i=0; i<numstepseries; i++ )
         {
            stepsize[i] = calc->stepsize[i];
            numsteps[i] = calc->numsteps[i];
         }
      }
      SimActive(calc->SimActive());
      SensActive(calc->SensActive());
   }
   else
   {
      char Buf1[BufSize]; char Buf2[BufSize];
      strcpy(Buf1,ini.T(1));
      NumToString(number,Buf2,BufSize);
      strcat(Buf1,Buf2);
      if ( Symbol(Buf1) == FALSE )
         FatalError("CALC::CALC: illegal symbol");
   }
}


CALC::~CALC()
{
   if ( number == 0 )
      FatalError("CALC::~CALC: already all CALCs destructed");
   number--;
   delete [] stepsize;
   delete [] numsteps;
   numstepseries = 0;
}


void CALC::CalcNum(CARDINAL c)
{
   calcnum = c;
   return;
}


CARDINAL CALC::CalcNum() const
{
   return calcnum;
}


void CALC::InitTime(REAL t)
{
   inittime = t;
   return;
}


REAL CALC::InitTime() const
{
   return inittime;
}


void CALC::InitSteady(BOOLEAN s)
{
   initsteady = s;
   return;
}


BOOLEAN CALC::InitSteady() const
{
   return initsteady;
}


CARDINAL CALC::NumStepSeries() const
{
   return numstepseries;
}


REAL CALC::StepSize(CARDINAL index) const
{
   if ( index >= numstepseries ) 
      FatalError("CALC::StepSize: illegal index");
   return stepsize[index];
}


CARDINAL CALC::NumSteps(CARDINAL index) const
{
   if ( index >= numstepseries ) 
      FatalError("CALC::NumSteps: illegal index");
   return numsteps[index];
}


BOOLEAN CALC::AddStepSeries(REAL s, CARDINAL n, CARDINAL index)
{
   if ( s <= 0.0 )           return FALSE;
   if ( n == 0 )             return FALSE;
   if ( n > CARDINAL_MAX-1 ) return FALSE;
   REAL*     newstepsize = new REAL[numstepseries+1];
   CARDINAL* newnumsteps = new CARDINAL[numstepseries+1];
   CARDINAL i;
   if ( index > numstepseries ) index = numstepseries;
   for ( i=0; i<index; i++ )
   {
      newstepsize[i] = stepsize[i];
      newnumsteps[i] = numsteps[i];
   }
   newstepsize[index] = s;
   newnumsteps[index] = n;
   for ( i=index; i<numstepseries; i++ )
   {
      newstepsize[i+1] = stepsize[i];
      newnumsteps[i+1] = numsteps[i];
   }
   numstepseries++;
   delete [] stepsize; stepsize = newstepsize;
   delete [] numsteps; numsteps = newnumsteps;
   return TRUE;
}


BOOLEAN CALC::ReplaceStepSeries(REAL s, CARDINAL n,  CARDINAL index)
{
   if ( s <= 0.0 )               return FALSE;
   if ( n == 0 )                 return FALSE;
   if ( n > CARDINAL_MAX-1 )     return FALSE;
   if ( index >= numstepseries ) return FALSE;
   stepsize[index] = s;
   numsteps[index] = n;
   return TRUE;
}


BOOLEAN CALC::DelStepSeries(CARDINAL index)
{
   if ( index >= numstepseries ) return FALSE;
   if ( numstepseries == 1 )
   {
      numstepseries = 0;
      delete [] stepsize; stepsize = 0;
      delete [] numsteps; numsteps = 0;
      return TRUE;
   }
   REAL*     newstepsize = new REAL[numstepseries-1];
   CARDINAL* newnumsteps = new CARDINAL[numstepseries-1];
   CARDINAL i;
   for ( i=0; i<index; i++ )
   {
      newstepsize[i] = stepsize[i];
      newnumsteps[i] = numsteps[i];
   }
   for ( i=index+1; i<numstepseries; i++ )
   {
      newstepsize[i-1] = stepsize[i];
      newnumsteps[i-1] = numsteps[i];
   }
   numstepseries--;
   delete [] stepsize; stepsize = newstepsize;
   delete [] numsteps; numsteps = newnumsteps;
   return TRUE;
}


BOOLEAN CALC::SimActive(BOOLEAN act)
{
   if ( InList() == TRUE ) return FALSE;
   simactive = act;
   return TRUE;
}


BOOLEAN CALC::SensActive(BOOLEAN act)
{
   if ( InList() == TRUE ) return FALSE;
   sensactive = act;
   return TRUE;
}


JOBSTATUS CALC::Load(std::istream& in)
{
   char Buffer[BufSize];
   if ( InList() == TRUE )                              return PROBLEM;

   if ( LoadStart(in) == FALSE )                        return PROBLEM;
   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )             return PROBLEM;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.0
         {
            CARDINAL num; REAL step;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )                                                        return PROBLEM;
            if ( Symbol(Buffer) == FALSE )              return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                        return PROBLEM;
            Description(Buffer);
            if ( LoadNumber(in,&calcnum) == FALSE )     return PROBLEM;
            if ( LoadNumber(in,&inittime) == FALSE )    return PROBLEM;
            if ( LoadBoolean(in,&initsteady) == FALSE ) return PROBLEM;
            if ( LoadNumber(in,&step) == FALSE )        return PROBLEM;
            if ( LoadNumber(in,&num) == FALSE )         return PROBLEM;
            if ( AddStepSeries(step,num,0) == FALSE )   return PROBLEM;
            if ( LoadBoolean(in,&sensactive) == FALSE ) return PROBLEM;
		    simactive = FALSE;
         }
         break;
      case 2: // aquavers=2: sens changed to calc
         {
            CARDINAL num; REAL step;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )                                                        return PROBLEM;
            if ( Symbol(Buffer) == FALSE )              return PROBLEM;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                        return PROBLEM;
            Description(Buffer);
            if ( LoadNumber(in,&calcnum) == FALSE )     return PROBLEM;
            if ( LoadNumber(in,&inittime) == FALSE )    return PROBLEM;
            if ( LoadBoolean(in,&initsteady) == FALSE ) return PROBLEM;
            if ( LoadStart(in) == FALSE )               return PROBLEM;
            while ( LoadNumber(in,&step) == TRUE )
            {
               if ( LoadNumber(in,&num) == FALSE )      return PROBLEM;
               if ( AddStepSeries(step,num,NumStepSeries()) == FALSE )
                                                        return PROBLEM;
            }
            if ( LoadBoolean(in,&simactive) == FALSE )  return PROBLEM;
            if ( LoadBoolean(in,&sensactive) == FALSE ) return PROBLEM;
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                          return PROBLEM;
   return OK;
}


JOBSTATUS CALC::Save(std::ostream& out)
{
   if ( SaveStart(out) == FALSE )                       return PROBLEM;

   INTEGER aquavers = 2;
   if ( SaveNumber(out,aquavers) == FALSE )             return PROBLEM;
   if ( SaveString(out,Symbol()) == FALSE )             return PROBLEM;
   if ( SaveString(out,Description()) == FALSE )        return PROBLEM;
   if ( SaveNumber(out,CalcNum()) == FALSE )            return PROBLEM;
   if ( SaveNumber(out,InitTime()) == FALSE )           return PROBLEM;
   if ( SaveBoolean(out,InitSteady()) == FALSE )        return PROBLEM;
   if ( SaveStart(out) == FALSE )                       return PROBLEM;
   for ( CARDINAL i=0; i<NumStepSeries(); i++ )
   {
      if ( SaveNumber(out,StepSize(i)) == FALSE )       return PROBLEM;
      if ( SaveNumber(out,NumSteps(i)) == FALSE )       return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                         return PROBLEM;
   if ( SaveBoolean(out,SimActive()) == FALSE )         return PROBLEM;
   if ( SaveBoolean(out,SensActive()) == FALSE )        return PROBLEM;
   if ( SaveEnd(out) == FALSE )                         return PROBLEM;
   return FILEIO::Save(out);
}


JOBSTATUS CALC::Write(std::ostream& out, BOOLEAN)
{
   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   PrintIndent(pos2-1);

   if ( Print(out,Symbol(),1) == FALSE )               return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;

   if ( Print(out,ini.T(2),pos1) == FALSE )            return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;
   if ( Description() != 0 )
   {
      if ( strlen(Description()) != 0 )
      {
         if ( Print(out,Description(),pos2) == FALSE ) return PROBLEM;
      }
   }

   if ( Print(out,ini.T(11),pos1) == FALSE )           return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;
   if ( Print(out,CalcNum(),pos2) == FALSE )           return PROBLEM;

   if ( Print(out,ini.T(12),pos1) == FALSE )           return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;
   if ( Print(out,InitTime(),pos2) == FALSE )          return PROBLEM;

   if ( Print(out,ini.T(13),pos1) == FALSE )           return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;
   if ( InitSteady() == FALSE )
   {
      if ( Print(out,ini.T(9),pos2) == FALSE )         return PROBLEM;
   }
   else
   {
      if ( Print(out,ini.T(10),pos2) == FALSE )        return PROBLEM;
   }

   if ( Print(out,ini.T(5),pos1) == FALSE )            return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;
   if ( NumStepSeries() > 0 )
   {
      if ( Print(out,StepSize(0),pos2) == FALSE )      return PROBLEM;
      for ( CARDINAL i=1; i<NumStepSeries(); i++ )
      {
         if ( Print(out,",") == FALSE )                return PROBLEM;
         if ( Print(out,StepSize(i)) == FALSE )        return PROBLEM;
      }
   }
   if ( Print(out,ini.T(6),pos1) == FALSE )            return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;

   if ( NumStepSeries() > 0 )
   {
      if ( Print(out,NumSteps(0),pos2) == FALSE )      return PROBLEM;
      for ( CARDINAL i=1; i<NumStepSeries(); i++ )
      {
         if ( Print(out,",") == FALSE )                return PROBLEM;
         if ( Print(out,NumSteps(i)) == FALSE )        return PROBLEM;
      }
   }

   if ( Print(out,ini.T(7),pos1) == FALSE )            return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;
   if ( SimActive() == TRUE )
   {
      if ( Print(out,ini.T(14),pos2) == FALSE )        return PROBLEM;
   }
   else
   {
      if ( Print(out,ini.T(15),pos2) == FALSE )        return PROBLEM;
   }
   if ( SensActive() == TRUE )
   {
      if ( Print(out,ini.T(3),pos2) == FALSE )         return PROBLEM;
   }
   else
   {
      if ( Print(out,ini.T(4),pos2) == FALSE )         return PROBLEM;
   }

   return OK;
}

//////////////////////////////////////////////////////////////////////////////

CALCSYS::CALCSYS()
{
#ifdef HEAPTEST
HeapFileMessage("start CALCSYS::CALCSYS()");
#endif

   cancelfun = 0;

#ifdef HEAPTEST
HeapFileMessage("end   CALCSYS::CALCSYS()");
#endif
}


CALCSYS::~CALCSYS()
{
#ifdef HEAPTEST
HeapFileMessage("start CALCSYS::~CALCSYS()");
#endif

   Delete();

#ifdef HEAPTEST
HeapFileMessage("end   CALCSYS::~CALCSYS()");
#endif
}


BOOLEAN CALCSYS::Add(CALC* calc)
{
   if ( calc == 0 )                     return FALSE;
   if ( calc->NumStepSeries() == 0 )    return FALSE;
   if ( Exist(calc->Symbol()) == TRUE ) return FALSE;
   CALC* f = calclist.First();
   if ( calc->SimActive() == TRUE )
   {
      while ( f != 0 )
      {
         if ( f->CalcNum() == calc->CalcNum() )
         {
            if ( f->SimActive() == TRUE ) return FALSE;
         }
         f = f->Next();
      }
   }
   if ( calc->SensActive() == TRUE )
   {
      while ( f != 0 )
      {
         if ( f->CalcNum() == calc->CalcNum() )
         {
            if ( f->SensActive() == TRUE ) return FALSE;
         }
         f = f->Next();
      }
   }
   if ( calclist.Add(calc) == FALSE )
      FatalError("CALCSYS::Add: cannot add calc");
   return TRUE;
}


BOOLEAN CALCSYS::Replace(CALC* oldcalc, CALC* newcalc)
{
   if ( newcalc == 0 )                                        return FALSE;
   if ( newcalc->NumStepSeries() == 0 )                       return FALSE;
   if ( Exist(oldcalc) == FALSE )                             return FALSE;
   if ( ( strcmp(oldcalc->Symbol(),newcalc->Symbol()) != 0 )
        &&             ( Exist(newcalc->Symbol()) == TRUE ) ) return FALSE;
   if ( newcalc->SimActive() == TRUE )
   {
      CALC* f = calclist.First();
      while ( f != 0 )
      {
         if ( f != oldcalc )
         {
            if ( f->CalcNum() == newcalc->CalcNum() )
            {
               if ( f->SimActive() == TRUE ) return FALSE;
            }
         }
         f = f->Next();
      }
   }
   if ( newcalc->SensActive() == TRUE )
   {
      CALC* f = calclist.First();
      while ( f != 0 )
      {
         if ( f != oldcalc )
         {
            if ( f->CalcNum() == newcalc->CalcNum() )
            {
               if ( f->SensActive() == TRUE ) return FALSE;
            }
         }
         f = f->Next();
      }
   }
   if ( calclist.Add(newcalc) == FALSE )
      FatalError("CALCSYS::Replace: cannot insert new calc");
   if ( calclist.Remove(oldcalc) == FALSE )
      FatalError("CALCSYS::Replace: cannot remove old calc");
   return TRUE;
}


BOOLEAN CALCSYS::Delete(CALC* calc)
{
   if ( calc == 0 )   return FALSE;
   BOOLEAN b = calclist.Remove(calc);
   if ( b == FALSE ) return FALSE;
   delete calc;
   return TRUE;
}


void CALCSYS::Delete()
{
   CALC* calc = calclist.First();
   while ( calc != 0 )
   {
      if ( calclist.Remove(calc) == FALSE )
         FatalError("CALCSYS::Delete: unable to remove calc");
      delete calc;
      calc = calclist.First();
   }
}


BOOLEAN CALCSYS::SimActive(CALC* calc, BOOLEAN act)
{
   if ( calc == 0 ) return FALSE;
   if ( act == FALSE )
   {
      calc->simactive = FALSE;
      return TRUE;
   }
   CARDINAL calcnum = calc->CalcNum();
   CALC* f = calclist.First();
   BOOLEAN found = FALSE;
   while ( f != 0 )
   {
      if ( f == calc )
      {
         found = TRUE;
      }
      else
      {
         if ( f->CalcNum() == calcnum )
         {
            if ( f->SimActive() == TRUE ) return FALSE;
         }
      }
      f = f->Next();
   }
   if ( found == FALSE ) return FALSE;
   calc->simactive = TRUE;
   return TRUE;
}


BOOLEAN CALCSYS::SensActive(CALC* calc, BOOLEAN act)
{
   if ( calc == 0 ) return FALSE;
   if ( act == FALSE )
   {
      calc->sensactive = FALSE;
      return TRUE;
   }
   CARDINAL calcnum = calc->CalcNum();
   CALC* f = calclist.First();
   BOOLEAN found = FALSE;
   while ( f != 0 )
   {
      if ( f == calc )
      {
         found = TRUE;
      }
      else
      {
         if ( f->CalcNum() == calcnum )
         {
            if ( f->SensActive() == TRUE ) return FALSE;
         }
      }
      f = f->Next();
   }
   if ( found == FALSE ) return FALSE;
   calc->sensactive = TRUE;
   return TRUE;
}


JOBSTATUS CALCSYS::Load(std::istream& in)
{
   char Buffer[BufSize];
   // Delete();

   // read identifier and global parameters:

   if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE ) return PROBLEM;
   if ( strcmp(Buffer,CalcsysIdentifier) != 0 )
   {
      // compatibility with file versions before 2.0delta:
      if ( strcmp(Buffer,"SENSSYS") != 0 )              return PROBLEM;
   }
   if ( LoadStart(in) == FALSE )                        return PROBLEM;

   // read calc:

   while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
   {
      if ( strcmp(Buffer,CalcIdentifier) != 0 )
      {
         // compatibility with file versions before 2.0delta:
         if ( strcmp(Buffer,"SENS") != 0 )              return PROBLEM;
      }
      CALC* calc = new CALC;
      JOBSTATUS status = calc->Load(in);
      if ( status != OK )
      {
         delete calc; return status;
      }
      if ( Add(calc) == FALSE )
      {
         delete calc; return PROBLEM;
      }
      if ( cancelfun != 0 )
      {
         if ( cancelfun() == TRUE )                     return INTERRUPT;
      }
   }
   return OK;
}


JOBSTATUS CALCSYS::Save(std::ostream& out)
{
   if ( SaveString(out,CalcsysIdentifier) == FALSE )    return PROBLEM;
   if ( SaveStart(out) == FALSE )                       return PROBLEM;

   CALC* calc = calclist.First();
   while ( calc != 0 )
   {
      if ( SaveString(out,CalcIdentifier) == FALSE )    return PROBLEM;
      JOBSTATUS status = calc->Save(out); if ( status != OK ) return status;
      if ( cancelfun != 0 )
      {
         if ( cancelfun() == TRUE )                     return INTERRUPT;
      }
      calc = calc->Next();
   }

   if ( SaveEnd(out) == FALSE )                         return PROBLEM;
   return OK;
}


JOBSTATUS CALCSYS::Write(std::ostream& out, BOOLEAN)
{
   if ( calclist.Size() == 0 )               return OK;

   if ( PrintLn(out) == FALSE )              return PROBLEM;
   if ( PrintLn(out) == FALSE )              return PROBLEM;
   if ( PrintLn(out) == FALSE )              return PROBLEM;
   if ( PrintLn(out) == FALSE )              return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )          return PROBLEM;
   if ( Print(out,CALC::ini.T(8)) == FALSE ) return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )          return PROBLEM;

   CALC* calc = calclist.First();
   while ( calc != 0 )
   {
      JOBSTATUS status = calc->Write(out);
      if ( status != OK )                    return status;
      if ( cancelfun != 0 )
      {
         if ( cancelfun() == TRUE )          return INTERRUPT;
      }
      calc = calc->Next();
      if ( calc != 0 ) PrintLn(out,'-');
      else             PrintLn(out,'*');
   }

   if ( PrintLn(out) == FALSE )              return PROBLEM;

   return OK;
}

//////////////////////////////////////////////////////////////////////////////

