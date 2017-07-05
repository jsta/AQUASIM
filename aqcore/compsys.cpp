///////////////////////////////   compsys.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    09.08.91    Peter Reichert
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "varsys.h"
#include "mixcomp.h"
#include "filmcomp.h"
#include "rivcomp.h"
#include "advcomp.h"
#include "lakecomp.h"
#include "soilcomp.h"
#include "sedcomp.h"
#include "compsys.h"

//////////////////////////////////////////////////////////////////////////////

extern std::ostream* ctest;       // test output stream pointer (all.C)

const CARDINAL BufSize = 1024;

//////////////////////////////////////////////////////////////////////////////

// Identifiers for COMPSYS::Save and COMPSYS::Load
// (compare this constant with enum AQCOMPTYPE from comp.h)

                                                         //  enum AQCOMPTYPE:
const char* const CompIdentifier[] = {
                                        "MIXCOMP",       //  MixComp
                                        "FILMCOMP",      //  FilmComp
                                        "RIVCOMP",       //  RivComp
                                        "ADVCOMP",       //  AdvComp
                                        "LAKECOMP",      //  LakeComp
                                        "SOILCOMP",      //  SoilComp
                                        "SEDCOMP"        //  SedComp
                                     };

const CARDINAL NumComp = sizeof(CompIdentifier)/sizeof(char*);

const char* CompsysIdentifier = "COMPSYS";

//////////////////////////////////////////////////////////////////////////////

COMPSYS::COMPSYS()
{
#ifdef HEAPTEST
HeapFileMessage("start COMPSYS::COMPSYS()");
#endif

   cancelfun = 0;

#ifdef HEAPTEST
HeapFileMessage("end   COMPSYS::COMPSYS()");
#endif
}


COMPSYS::~COMPSYS()
{
#ifdef HEAPTEST
HeapFileMessage("start COMPSYS::~COMPSYS()");
#endif

   Delete();

#ifdef HEAPTEST
HeapFileMessage("end   COMPSYS::~COMPSYS()");
#endif
}


BOOLEAN COMPSYS::AllowedReplaceVar(const AQVAR* oldvar,
                                   const AQVAR* newvar) const
{
   AQCOMP* com = complist.First();
   while ( com != 0 )
   {
      if ( com->AllowedReplaceVar(oldvar,newvar) == FALSE ) return FALSE;
      com = com->Next();
   }
   return TRUE;
}


BOOLEAN COMPSYS::AllowedExchangeVar(const AQVAR* var1,
                                    const AQVAR* var2) const
{
   AQCOMP* com = complist.First();
   while ( com != 0 )
   {
      if ( com->AllowedExchangeVar(var1,var2) == FALSE ) return FALSE;
      com = com->Next();
   }
   return TRUE;
}


BOOLEAN COMPSYS::AllowedReplaceProc(const AQPROC* oldproc,
                                    const AQPROC* newproc) const
{
   AQCOMP* com = complist.First();
   while ( com != 0 )
   {
      if ( com->AllowedReplaceProc(oldproc,newproc) == FALSE ) return FALSE;
      com = com->Next();
   }
   return TRUE;
}


BOOLEAN COMPSYS::Add(AQCOMP* com)
{
   if ( com == 0 )                     return FALSE;
   if ( com->InList() == TRUE )        return FALSE;
   if ( com->AllowedComp() == FALSE )  return FALSE;
   if ( Exist(com->Symbol()) == TRUE ) return FALSE;
   if ( complist.Add(com) == FALSE )
      FatalError("COMPSYS::Add: Unable to add compartment");
   CalcIndex();
   return TRUE;
}


BOOLEAN COMPSYS::Replace(AQCOMP* oldcomp, AQCOMP* newcomp)
{
   if ( (oldcomp==0) || (newcomp==0) )        return FALSE;
   if ( newcomp->InList() == TRUE )           return FALSE;
   if ( newcomp->AllowedComp() == FALSE )     return FALSE;
   if ( Exist(oldcomp) == FALSE )             return FALSE;
   if ( strcmp(oldcomp->Symbol(),newcomp->Symbol()) != 0 )
   {
      if ( Exist(newcomp->Symbol()) == TRUE ) return FALSE;
   }
   if ( complist.Remove(oldcomp) == FALSE )
      FatalError("COMPSYS::Replace: Unable to remove old compartment");
   if ( complist.Add(newcomp) == FALSE )
      FatalError("COMPSYS::Replace: Unable to add new compartment");
   CalcIndex();
   return TRUE;
}


BOOLEAN COMPSYS::Delete(AQCOMP* com)
{
   if ( complist.Remove(com) == FALSE ) return FALSE;
   delete com;
   CalcIndex();
   return TRUE;
}


void COMPSYS::ReplaceVar(AQVAR* oldvar, AQVAR* newvar)
{
   AQCOMP* c = complist.First();
   while ( c != 0 )
   {
      c->ReplVar(oldvar,newvar);
      c = c->Next();
   }
   CalcIndex();
}


void COMPSYS::ExchangeVar(AQVAR* var1, AQVAR* var2)
{
   AQCOMP* c = complist.First();
   while ( c != 0 )
   {
      c->ExchVar(var1,var2);
      c = c->Next();
   }
   CalcIndex();
   return;
}


void COMPSYS::ReplaceProc(AQPROC* oldproc, AQPROC* newproc)
{
   AQCOMP* c = complist.First();
   while ( c != 0 )
   {
      c->ReplProc(oldproc,newproc);
      c = c->Next();
   }
   return;
}


void COMPSYS::Delete()
{
   AQCOMP* com = complist.First();
   while ( com != 0 )
   {
      if ( complist.Remove(com) == FALSE )
         FatalError("COMPSYS::Delete: Unable to remove compartment");
      delete com;
      com = complist.First();
   }
   return;
}


BOOLEAN COMPSYS::Arg(const AQVAR* var) const
{
   AQCOMP* c = complist.First();
   while ( c != 0 )
   {
      if ( c->Arg(var) == TRUE ) return TRUE;
      c = c->Next();
   }
   return FALSE;
}


BOOLEAN COMPSYS::Arg(const AQPROC* proc) const
{
   AQCOMP* c = complist.First();
   while ( c != 0 )
   {
      if ( c->Arg(proc) == TRUE ) return TRUE;
      c = c->Next();
   }
   return FALSE;
}


CARDINAL COMPSYS::NumEq()
{
   AQCOMP* com = complist.First();
   CARDINAL numeq = 0;
   while ( com != 0 )
   {
      if ( com->Active() == TRUE )
      {
         numeq += com->NumEq();
      }
      com = com->Next();
   }
   return numeq;
}


BOOLEAN COMPSYS::RelAcc(CARDINAL size, REAL* Y)
{
   if ( size < NumEq() ) return FALSE;
   AQCOMP* com = complist.First();
   if ( com == 0 ) return FALSE;
   while ( com != 0 )
   {
      if ( com->Active() == TRUE )
      {
         for ( CARDINAL i=0; i<com->NumEq(); i++ )
         {
            Y[com->ind_y+i] = com->RelAcc(i);
         }
      }
      com = com->Next();
   }
   return TRUE;
}


BOOLEAN COMPSYS::AbsAcc(CARDINAL size, REAL* Y)
{
   if ( size < NumEq() ) return FALSE;
   AQCOMP* com = complist.First();
   if ( com == 0 ) return FALSE;
   while ( com != 0 )
   {
      if ( com->Active() == TRUE )
      {
         for ( CARDINAL i=0; i<com->NumEq(); i++ )
         {
            Y[com->ind_y+i] = com->AbsAcc(i);
         }
      }
      com = com->Next();
   }
   return TRUE;
}


BOOLEAN COMPSYS::EqType(CARDINAL size, EQTYPE* Y)
{
   if ( size < NumEq() ) return FALSE;
   AQCOMP* com = complist.First();
   if ( com == 0 ) return FALSE;
   while ( com != 0 )
   {
      if ( com->Active() == TRUE )
      {
         for ( CARDINAL i=0; i<com->NumEq(); i++ )
         {
            Y[com->ind_y+i] = com->EqType(i);
         }
      }
      com = com->Next();
   }
   return TRUE;
}


BOOLEAN COMPSYS::InitCond(VARSYS* varsys, REAL t,
                          CARDINAL size, REAL* Y)
{
   REAL time = t;
   if ( size < NumEq() ) return FALSE;
   varsys->SetProgVar(Time,&time);
   for ( CARDINAL i=0; i<5; i++ )
   {
      AQCOMP* com = complist.First();
      while ( com != 0 )
      {
         if ( com->Active() == TRUE )
         {
            if ( com->InitCond(varsys,Y,i) == FALSE ) return FALSE;
         }
         com = com->Next();
      }
   }
   varsys->SetProgVar(Time,0);
   return TRUE;
}


BOOLEAN COMPSYS::Delta(const NUMPAR& numpar, VARSYS* varsys,
                       const REAL* Y, const REAL* YT, REAL* DELTA)
{
   AQCOMP* com = complist.First();
   while ( com != 0 )
   {
      if ( com->Active() == TRUE )
      {
         if ( com->Delta(numpar,varsys,Y,YT,DELTA) == FALSE )
            return FALSE;
      }
      com = com->Next();
   }
   return TRUE;
}


BOOLEAN COMPSYS::GridValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                           REAL* t_ptr, const REAL* Y, AQCOMP* com,
                           CARDINAL zone, CARDINAL pt, AQVAR* var,
                           REAL& value)
{
   if ( complist.Exist(com) )
   {
      if ( com->Active() == FALSE ) return FALSE; 
      return com->GridValue(varsys,calcnum_ptr,t_ptr,Y,zone,pt,var,value);
   }
   return FALSE;
}


BOOLEAN COMPSYS::SpaceValue(VARSYS* varsys, CARDINAL* calcnum_ptr,
                            REAL* t_ptr, const REAL* Y, AQCOMP* com,
                            CARDINAL zone, REAL x, BOOLEAN xrel,
                            AQVAR* var,
                            REAL& value)
{
   if ( complist.Exist(com) )
   {
      if ( com->Active() == FALSE ) return FALSE; 
      return com->SpaceValue(varsys,calcnum_ptr,t_ptr,Y,zone,x,xrel,var,value);
   }
   return FALSE;
}


void COMPSYS::CalcIndex()
{
   CARDINAL index = 0;
   AQCOMP* com = complist.First();
   while ( com != 0 )
   {
      if ( com->Active() == TRUE )
      {
         com->ind_y = index;
         index += com->NumEq();
      }
      else
      {
         com->ind_y = 0;
      }
      com = com->Next();
   }
   return;
}


JOBSTATUS COMPSYS::Load(std::istream& in,
                        const VARLIST* varlist,
                        const PROCLIST* proclist)
{
   char Buffer[BufSize];
   // Delete();

   // read identifier and global parameters:

   if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )   return PROBLEM;
   if ( strcmp(Buffer,CompsysIdentifier) != 0 )        return PROBLEM;
   if ( LoadStart(in) == FALSE )                          return PROBLEM;

   // read compartments:

   while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
   {
      CARDINAL type = 0;
      while ( type < NumComp )
      {
         if ( strcmp(Buffer,CompIdentifier[type]) == 0 ) break;
         type++;
      }
      JOBSTATUS status;
      switch (type)
      {
         case MixComp:
            {
               MIXCOMP* mc = new MIXCOMP;
               status = mc->Load(in,varlist,proclist);
               if ( status != OK )
               {
                  delete mc; return status;
               }
               if ( Add(mc) == FALSE )
               {
                  delete mc; return PROBLEM;
               }
            }
            break;
         case FilmComp:
            {
               FILMCOMP* fc = new FILMCOMP;
               status = fc->Load(in,varlist,proclist);
               if ( status != OK )
               {
                  delete fc; return status;
               }
               if ( Add(fc) == FALSE )
               {
                  delete fc; return PROBLEM;
               }
            }
            break;
         case RivComp:
            {
               RIVCOMP* rc = new RIVCOMP;
               status = rc->Load(in,varlist,proclist);
               if ( status != OK )
               {
                  delete rc; return status;
               }
               if ( Add(rc) == FALSE )
               {
                  delete rc; return PROBLEM;
               }
            }
            break;
         case AdvComp:
            {
               ADVCOMP* ac = new ADVCOMP;
               status = ac->Load(in,varlist,proclist);
               if ( status != OK )
               {
                  delete ac; return status;
               }
               if ( Add(ac) == FALSE )
               {
                  delete ac; return PROBLEM;
               }
            }
            break;
         case LakeComp:
            {
               LAKECOMP* lc = new LAKECOMP;
               status = lc->Load(in,varlist,proclist);
               if ( status != OK )
               {
                  delete lc; return status;
               }
               if ( Add(lc) == FALSE )
               {
                  delete lc; return PROBLEM;
               }
            }
            break;
         case SoilComp:
            {
               SOILCOMP* sc = new SOILCOMP;
               status = sc->Load(in,varlist,proclist);
               if ( status != OK )
               {
                  delete sc; return status;
               }
               if ( Add(sc) == FALSE )
               {
                  delete sc; return PROBLEM;
               }
            }
            break;
         case SedComp:
            {
               SEDCOMP* sc = new SEDCOMP;
               status = sc->Load(in,varlist,proclist);
               if ( status != OK )
               {
                  delete sc; return status;
               }
               if ( Add(sc) == FALSE )
               {
                  delete sc; return PROBLEM;
               }
            }
            break;
         default:
            return PROBLEM;
      }
      if ( cancelfun != 0 )
      {
         if ( cancelfun() == TRUE )                       return INTERRUPT;
      }
   }
   return OK;
}


JOBSTATUS COMPSYS::Save(std::ostream& out)
{
   if ( SaveString(out,CompsysIdentifier) == FALSE )      return PROBLEM;
   if ( SaveStart(out) == FALSE )                         return PROBLEM;

   // save compartments:

   AQCOMP* com = complist.First();
   while ( com != 0 )
   {
      if ( com->Type() > NumComp )
         FatalError("COMPSYS::Save: Unknown Proc Type");
      if ( SaveString(out,CompIdentifier[com->Type()]) == FALSE )
                                                          return PROBLEM;
      JOBSTATUS status = com->Save(out);
      if ( status != OK ) return status;
      if ( cancelfun != 0 )
      {
         if ( cancelfun() == TRUE )                       return INTERRUPT;
      }
      com = com->Next();
   }

   if ( SaveEnd(out) == FALSE )                           return PROBLEM;

   return OK;
}


JOBSTATUS COMPSYS::Write(std::ostream& out, BOOLEAN sh)
{
   if ( complist.Size() == 0 )                  return OK;

   if ( PrintLn(out) == FALSE )                 return PROBLEM;
   if ( PrintLn(out) == FALSE )                 return PROBLEM;
   if ( PrintLn(out) == FALSE )                 return PROBLEM;
   if ( PrintLn(out) == FALSE )                 return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )             return PROBLEM;
   if ( Print(out,AQCOMP::ini.T(34)) == FALSE ) return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )             return PROBLEM;

   AQCOMP* com = complist.First();
   while ( com != 0 )
   {
      JOBSTATUS status = com->Write(out,sh);
      if ( status != OK )                       return status;
      if ( cancelfun != 0 )
      {
         if ( cancelfun() == TRUE )             return INTERRUPT;
      }
      com = com->Next();
	  if ( sh == FALSE )
	  {
         if ( com != 0 ) PrintLn(out,'-');
         else            PrintLn(out,'*');
	  }
   }

   if ( PrintLn(out) == FALSE )                 return PROBLEM;

   return OK;
}

//////////////////////////////////////////////////////////////////////////////
