///////////////////////////////   procsys.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    22.08.91    Peter Reichert
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// Implementation of procsys.h
// ===========================
//
//////////////////////////////////////////////////////////////////////////////

#include "all.h"
#include "init.h"
#include "procsys.h"

//////////////////////////////////////////////////////////////////////////////

// Identifiers for PROCSYS::Save and PROCSYS::Load
// (compare this constant with enum AQPROCTYPE from aqproc.h)

                                                    //  enum AQPROCTYPE
const char* const ProcIdentifier[] = {
                                        "DYNPROC",  //  DynProc
                                        "EQUPROC"   //  EquProc
                                     };

const CARDINAL NumProc = sizeof(ProcIdentifier)/sizeof(char*);

const char* ProcsysIdentifier = "PROCSYS";

//////////////////////////////////////////////////////////////////////////////

PROCSYS::PROCSYS()
{
#ifdef HEAPTEST
HeapFileMessage("start PROCSYS::PROCSYS()");
#endif

   cancelfun = 0;

#ifdef HEAPTEST
HeapFileMessage("end   PROCSYS::PROCSYS()");
#endif
}


PROCSYS::~PROCSYS()
{
#ifdef HEAPTEST
HeapFileMessage("start PROCSYS::~PROCSYS()");
#endif

   Delete();

#ifdef HEAPTEST
HeapFileMessage("end   PROCSYS::~PROCSYS()");
#endif
}


BOOLEAN PROCSYS::Add(AQPROC* proc)
{
   if ( proc == 0 )                     return FALSE;
   if ( proc->InList() == TRUE )        return FALSE;
   if ( Exist(proc->Symbol()) == TRUE ) return FALSE;
   if ( proc->Type() == EquProc )
   {
      if ( proc->NumVar() != 1 )        return FALSE;
   }
   if ( proclist.Add(proc) == FALSE )
      FatalError("PROCSYS::Add: Unable to add process");
   return TRUE;
}


BOOLEAN PROCSYS::Replace(AQPROC* oldproc, AQPROC* newproc)
{
   if ( (oldproc==0) || (newproc==0) )           return FALSE;
   if ( newproc->InList() == TRUE )              return FALSE;
   if ( Exist(oldproc) == FALSE )                return FALSE;
   if ( newproc->Type() == EquProc )
   {
      if ( newproc->NumVar() != 1 )              return FALSE;
   }
   if ( strcmp(oldproc->Symbol(),newproc->Symbol()) != 0 )
   {
      if ( Exist(newproc->Symbol()) == TRUE )    return FALSE;
   }
   if ( proclist.Remove(oldproc) == FALSE )
      FatalError("PROCSYS::Replace: Unable to remove old process");
   if ( proclist.Add(newproc) == FALSE )
      FatalError("PROCSYS::Replace: Unable to insert new process");
   return TRUE;
}


BOOLEAN PROCSYS::Delete(AQPROC* proc)
{
   if ( proclist.Remove(proc) == FALSE ) return FALSE;
   delete proc;
   return TRUE;
}


void PROCSYS::Delete()
{
   AQPROC* proc = proclist.First();
   while ( proc != 0 )
   {
      if ( proclist.Remove(proc) == FALSE )
         FatalError("PROCSYS::Delete: Unable to remove process");
      delete proc;
      proc = proclist.First();
   }
}


void PROCSYS::ReplaceVar(AQVAR* oldvar, AQVAR* newvar)
{
   AQPROC* p = proclist.First();
   while ( p != 0 )
   {
      p->ReplVar(oldvar,newvar);
      p = p->Next();
   }
}


void PROCSYS::ExchangeVar(AQVAR* var1, AQVAR* var2)
{
   AQPROC* p = proclist.First();
   while ( p!=0 )
   {
      p->ExchVar(var1,var2);
      p = p->Next();
   }
}


BOOLEAN PROCSYS::Arg(const AQVAR* var) const
{
   AQPROC* p = proclist.First();
   while ( p!=0 )
   {
      if ( p->Arg(var) == TRUE ) return TRUE;
      p = p->Next();
   }
   return FALSE;
}


JOBSTATUS PROCSYS::Load(std::istream& in, const VARLIST* varlist)
{
   char Buffer[32];
   // Delete();

// read identifier and global parameters:

   if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )  return PROBLEM;
   if ( strcmp(Buffer,ProcsysIdentifier) != 0 )       return PROBLEM;
   if ( LoadStart(in) == FALSE )                         return PROBLEM;

// read processes:

   while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
   {
      CARDINAL type = 0;
      while ( type < NumProc )
      {
         if ( strcmp(Buffer,ProcIdentifier[type]) == 0 ) break;
         type++;
      }
      JOBSTATUS status;
      switch (type)
      {
         case DynProc:
            {
               DYNPROC* dp = new DYNPROC;
               status = dp->Load(in,varlist);
               if ( status != OK )
               {
                  delete dp; return status;
               }
               if ( Add(dp) == FALSE )
               {
                  delete dp; return PROBLEM;
               }
            }
            break;
         case EquProc:
            {
               EQUPROC* ap = new EQUPROC;
               status = ap->Load(in,varlist);
               if ( status != OK )
               {
                  delete ap; return status;
               }
               if ( Add(ap) == FALSE )
               {
                  delete ap; return PROBLEM;
               }
            }
            break;
         default:
            return OK;
      }
      if ( cancelfun != 0 )
      {
         if ( cancelfun() == TRUE )                       return INTERRUPT;
      }
   }
   return OK;
}


JOBSTATUS PROCSYS::Save(std::ostream& out)
{
   if ( SaveString(out,ProcsysIdentifier) == FALSE )    return PROBLEM;
   if ( SaveStart(out) == FALSE )                       return PROBLEM;

   // save processes:

   AQPROC* node = proclist.First();
   while ( node !=0 )
   {
      if ( node->Type() > NumProc )
         FatalError("PROCSYS::Save: Unknown Proc Type");
      if ( SaveString(out,ProcIdentifier[node->Type()]) == FALSE )
                                                        return PROBLEM;
      JOBSTATUS status = node->Save(out);
      if ( status != OK ) return status;
      if ( cancelfun != 0 )
      {
         if ( cancelfun() == TRUE )                     return INTERRUPT;
      }
      node = node->Next();
   }

   if ( SaveEnd(out) == FALSE )                         return PROBLEM;
   return OK;
}


JOBSTATUS PROCSYS::Write(std::ostream& out, BOOLEAN sh)
{
   if ( proclist.Size() == 0 )                 return OK;

   if ( PrintLn(out) == FALSE )                return PROBLEM;
   if ( PrintLn(out) == FALSE )                return PROBLEM;
   if ( PrintLn(out) == FALSE )                return PROBLEM;
   if ( PrintLn(out) == FALSE )                return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )            return PROBLEM;
   if ( Print(out,AQPROC::ini.T(4)) == FALSE ) return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )            return PROBLEM;

   AQPROC* proc = proclist.First();
   while ( proc != 0 )
   {
      JOBSTATUS status = proc->Write(out,sh);
      if ( status != OK )                      return status;
      if ( cancelfun != 0 )
      {
         if ( cancelfun() == TRUE )            return INTERRUPT;
      }
      proc = proc->Next();
	  if ( sh == FALSE )
	  {
         if ( proc != 0 ) PrintLn(out,'-');
         else             PrintLn(out,'*');
	  }
   }

   if ( PrintLn(out) == FALSE )                return PROBLEM;

   return OK;
}

//////////////////////////////////////////////////////////////////////////////
