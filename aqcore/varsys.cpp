////////////////////////////////   varsys.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    24.07.91    Peter Reichert
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "all.h"
#include "init.h"
#include "formvar.h"
#include "othervar.h"
#include "varsys.h"

//////////////////////////////////////////////////////////////////////////////

// Identifiers for VARSYS::Save and VARSYS::Load
// (compare this constant with enum AQVARTYPE from aqvar.h)

                                                      //  enum AQVARTYPE:
const char* const VarIdentifier[] = {
                                       "STATEVAR",    //  StateVar
                                       "PROGVAR",     //  ProgVar
                                       "CONSTVAR",    //  ConstVar
                                       "REALLISTVAR", //  RealListVar
                                       "VARLISTVAR",  //  VarListVar
                                       "FORMVAR",     //  FormVar
                                       "PROBEVAR"     //  ProbeVar
                                    };

const CARDINAL NumVar = sizeof(VarIdentifier)/sizeof(char*);

const char* VarsysIdentifier = "VARSYS";

//////////////////////////////////////////////////////////////////////////////

VARSYS::VARSYS()
{
#ifdef HEAPTEST
HeapFileMessage("start VARSYS::VARSYS()");
#endif

   cancelfun = 0;
   numaddr = 0;
   stateprogaddr = 0;

#ifdef HEAPTEST
HeapFileMessage("end   VARSYS::VARSYS()");
#endif
}


VARSYS::~VARSYS()
{
#ifdef HEAPTEST
HeapFileMessage("start VARSYS::~VARSYS()");
#endif

   Delete();

#ifdef HEAPTEST
HeapFileMessage("end   VARSYS::~VARSYS()");
#endif
}


void VARSYS::Reset()
{
   if ( AQVAR::reset == TRUE ) return;
   AQVAR* v = varlist.First();
   while ( v != 0 )
   {
      v->Reset();
      v = v->Next();
   }
   AQVAR::reset = TRUE;
   return;
}


void VARSYS::StoreStateProgVars()
{
   numaddr = varlist.Size();
   stateprogaddr = new REAL*[numaddr];
   CARDINAL i=0;
   AQVAR* v = varlist.First();
   while ( v != 0 )
   {
      switch ( v->Type() )
      {
         case StateVar:
         {
            STATEVAR* sv = (STATEVAR*)v;
            stateprogaddr[i] = (REAL*)sv->GetStateVar();
            break;
         }
         case ProgVar:
         {
            PROGVAR* pv = (PROGVAR*)v;
            stateprogaddr[i] = (REAL*)pv->GetProgVar();
            break;
         }
         default:
            stateprogaddr[i] = 0;
            break;
      }
      i++;
      if ( i > numaddr )
         FatalError("VARSYS::StoreStateProgVars: illegal index");
      v = v->Next();
   }
   return;
}


void VARSYS::RecallStateProgVars()
{
   if ( numaddr == 0 )
      FatalError("VARSYS::RecallStateProgVars: addresses not stored");
   CARDINAL i=0;
   AQVAR* v = varlist.First();
   while ( v != 0 )
   {
      switch ( v->Type() )
      {
         case StateVar:
         {
            STATEVAR* sv = (STATEVAR*)v;
            sv->SetStateVar(stateprogaddr[i]);
            break;
         }
         case ProgVar:
         {
            PROGVAR* pv = (PROGVAR*)v;
            pv->SetProgVar(stateprogaddr[i]);
            break;
         }
         default:
            break;
      }
      i++;
      if ( i > numaddr )
         FatalError("VARSYS::RecallStateProgVars: illegal index");
      v = v->Next();
   }
   delete [] stateprogaddr;
   numaddr = 0;
   Reset();
   return;
}

	  
BOOLEAN VARSYS::Arg(const AQVAR* var) const
{
   AQVAR* v = varlist.First();
   while ( v != 0 )
   {
      if ( v->Arg(var) == TRUE ) return TRUE;
      v = v->Next();
   }
   return FALSE;
}


BOOLEAN VARSYS::Arg(const AQCOMP* com) const
{
   AQVAR* v = varlist.First();
   while ( v != 0 )
   {
      if ( v->Arg(com) == TRUE ) return TRUE;
      v = v->Next();
   }
   return FALSE;
}


BOOLEAN VARSYS::CheckVarTypes(AQVAR* var)
{
   if ( var->AllowedVar() == FALSE ) return FALSE;
   if ( var->Type() == ProgVar )
   {
      PROGVAR* progvar = (PROGVAR*)var;
      AQVAR* v = varlist.First();
      while ( v != 0 )
      {
         if ( v->Type() == ProgVar )
         {
            if ( ((PROGVAR*)v)->ProgVarType()==progvar->ProgVarType() )
                                                         return FALSE;
         }
         v = v->Next();
      }
      return TRUE;
   }
   return TRUE;
}


BOOLEAN VARSYS::Add(AQVAR* var)
{
   if ( var == 0 )                                       return FALSE;
   if ( FORMVAR::ReservedSymbol(var->Symbol()) == TRUE ) return FALSE;
   if ( Exist(var->Symbol()) == TRUE )                   return FALSE;
   for ( CARDINAL i=0; i<var->NumVarArg(); i++ )
   {
      if ( Exist(var->VarArg(i)) == FALSE )              return FALSE;
      if ( var->VarArg(i) == var )                       return FALSE;
   }
   if ( CheckVarTypes(var) == FALSE )                    return FALSE;
   if ( varlist.Add(var) == FALSE )
      FatalError("VARSYS::Add: cannot add variable");
   Reset();
   return TRUE;
}


BOOLEAN VARSYS::Delete(AQVAR* var)
{
   if ( var == 0 )            return FALSE;
   if ( Arg(var) == TRUE ) return FALSE;
   BOOLEAN b = varlist.Remove(var);
   if ( b == FALSE )          return FALSE;
   delete var;
   Reset();
   return TRUE;
}


void VARSYS::Delete()
{
   AQVAR* var = varlist.First();
   while ( var != 0 )
   {
      if ( varlist.Remove(var) == FALSE )
         FatalError("VARSYS::Delete: unable to remove variable");
      delete var;
      var = varlist.First();
   }
}


BOOLEAN VARSYS::Replace(AQVAR* oldvar, AQVAR* newvar)
{
   if ( newvar == 0 )                                        return FALSE;
   if ( FORMVAR::ReservedSymbol(newvar->Symbol()) == TRUE )  return FALSE;
   if ( Exist(oldvar) == FALSE )                             return FALSE;
   if ( ( strcmp(oldvar->Symbol(),newvar->Symbol()) != 0 )
        &&             ( Exist(newvar->Symbol()) == TRUE ) ) return FALSE;
   if ( newvar->Arg(oldvar) == TRUE )                        return FALSE;
   for ( CARDINAL i=0; i<newvar->NumVarArg(); i++ )
   {
      if ( newvar->VarArg(i) == oldvar )                     return FALSE;
      if ( Exist(newvar->VarArg(i)) == FALSE )               return FALSE;
      if ( newvar->VarArg(i) == newvar )                     return FALSE;
   }
   if ( varlist.Remove(oldvar) == FALSE )
      FatalError("VARSYS::Replace: cannot remove old variable");
   if ( CheckVarTypes(newvar) == FALSE )
   {
      if ( varlist.Add(oldvar) == FALSE )
         FatalError("VARSYS::Replace: cannot reinsert old variable");
      return FALSE;
   }
   if ( varlist.Add(newvar) == FALSE )
      FatalError("VARSYS::Replace: cannot insert new variable");
   AQVAR* var = varlist.First();
   while ( var != 0 )
   {
      var->ReplVarArg(oldvar,newvar);
      var = var->Next();
   }
   Reset();
   return TRUE;
}


BOOLEAN VARSYS::Exchange(AQVAR* var1, AQVAR* var2)
{
   if ( var1 == var2 )                                 return FALSE;
   if ( (Exist(var1)==FALSE) || (Exist(var2)==FALSE) ) return FALSE;
   if ( var1->Arg(var2) == TRUE )                      return FALSE;
   if ( var2->Arg(var1) == TRUE )                      return FALSE;
   AQVAR* v = new CONSTVAR;
   AQVAR* var = varlist.First();
   while ( var != 0 )
   {
      var->ReplVarArg(var1,v);
      var->ReplVarArg(var2,var1);
      var->ReplVarArg(v,var2);
      var = var->Next();
   }
   delete v;
   Reset();
   return TRUE;
}


PROGVAR* VARSYS::GetProgVar(PROGVARTYPE type) const
{
   AQVAR* v = varlist.First();
   while ( v != 0 )
   {
      if ( v->Type() == ProgVar )
      {
         PROGVAR* progvar = (PROGVAR*)v;
         if ( progvar->ProgVarType() == type )
         {
            return progvar;
         }
      }
      v = v->Next();
   }
   return 0;
}


BOOLEAN VARSYS::SetProgVar(PROGVARTYPE type, const REAL* var)
{
   Reset();
   AQVAR* v = varlist.First();
   while ( v != 0 )
   {
      if ( v->Type() == ProgVar )
      {
         PROGVAR* progvar = (PROGVAR*)v;
         if ( progvar->ProgVarType() == type )
         {
            progvar->SetProgVar(var);
            return TRUE;
         }
      }
      v = v->Next();
   }
   return FALSE;
}


void VARSYS::ReplaceComp(AQCOMP* oldcomp, AQCOMP* newcomp)
{
   AQVAR* v = varlist.First();
   while ( v != 0 )
   {
      v->ReplCompArg(oldcomp,newcomp);
      v = v->Next();
   }
}


JOBSTATUS VARSYS::Load(std::istream& in)
{
   char Buffer[32];
   // Delete();

// read identifier and global parameters:

   if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE ) return PROBLEM;
   if ( strcmp(Buffer,VarsysIdentifier) != 0 )          return PROBLEM;
   if ( LoadStart(in) == FALSE )                        return PROBLEM;

// read variables:

   while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
   {
      CARDINAL type = 0;
      while ( type < NumVar )
      {
         if ( strcmp(Buffer,VarIdentifier[type]) == 0 ) break;
         type++;
      }
      JOBSTATUS status;
      switch ( type )
      {
         case StateVar:
            {
               STATEVAR* sv = new STATEVAR;
               status = sv->Load(in);
               if ( status != OK )
               {
                  delete sv; return status;
               }
               if ( Add(sv) == FALSE )
               {
                  delete sv; return PROBLEM;
               }
            }
            break;
         case ProgVar:
            {
               PROGVAR* pv = new PROGVAR;
               status = pv->Load(in);
               if ( status != OK )
               {
                  delete pv; return status;
               }
               if ( Add(pv) == FALSE )
               {
                  delete pv; return PROBLEM;
               }
            }
            break;
         case ConstVar:
            {
               CONSTVAR* cv = new CONSTVAR;
               status = cv->Load(in);
               if ( status != OK )
               {
                  delete cv; return status;
               }
               if ( Add(cv) == FALSE )
               {
                  delete cv; return PROBLEM;
               }
            }
            break;
         case RealListVar:
            {
               REALLISTVAR* rl = new REALLISTVAR;
               status = rl->Load(in,Varlist());
               if ( status != OK )
               {
                  delete rl; return status;
               }
               if ( Add(rl) == FALSE )
               {
                  delete rl; return PROBLEM;
               }
            }
            break;
         case VarListVar:
            {
               VARLISTVAR* vl = new VARLISTVAR;
               status = vl->Load(in,Varlist());
               if ( status != OK )
               {
                  delete vl; return status;
               }
               if ( Add(vl) == FALSE )
               {
                  delete vl; return PROBLEM;
               }
            }
            break;
         case FormVar:
            {
               FORMVAR* fo = new FORMVAR;
               status = fo->Load(in,Varlist());
               if ( status != OK )
               {
                  delete fo; return status;
               }
               if ( Add(fo) == FALSE )
               {
                  delete fo; return PROBLEM;
               }
            }
            break;
         case ProbeVar:
            {
               PROBEVAR* pr = new PROBEVAR;
               status = pr->Load(in,Varlist());
               if ( status != OK )
               {
                  delete pr; return status;
               }
               if ( Add(pr) == FALSE )
               {
                  delete pr; return PROBLEM;
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


JOBSTATUS VARSYS::Save(std::ostream& out)
{
   if ( SaveString(out,VarsysIdentifier) == FALSE )          return PROBLEM;
   if ( SaveStart(out) == FALSE )                            return PROBLEM;
   
   AQVAR* v = varlist.First();
   while ( v != 0 )
   {
      v->Changed();
      v = v->Next();
   }
   CARDINAL numsaved = 0;
   for ( CARDINAL i=0; i<varlist.Size(); i++ )
   {
      v = varlist.First();
      while ( (v!=0) && (numsaved<varlist.Size()) )
      {
         if ( (v->VarArgSaved()==TRUE) && (v->Saved()==FALSE) ) 
         {
            if ( v->Type() > NumVar )
               FatalError("VARSYS::Save: Unknown Var Type");
            if ( SaveString(out,VarIdentifier[v->Type()]) == FALSE )
                                                             return PROBLEM;
            switch ( v->Save(out) )
            {
               case OK:
                  numsaved++;
                  break;
               case INTERRUPT:
                  return INTERRUPT;
               case PROBLEM:
                  break;
            }
         }
         if ( cancelfun != 0 )
         {
            if ( cancelfun() == TRUE )                       return INTERRUPT;
         }
         v = v->Next();
      }
   }
   if ( numsaved == varlist.Size() )
   {
      if ( SaveEnd(out) == FALSE )                           return PROBLEM;
      return OK;
   }
   else
   {
      return PROBLEM;
   }
}


JOBSTATUS VARSYS::Write(std::ostream& out, BOOLEAN sh)
{
   if ( varlist.Size() == 0 )                  return OK;

   if ( PrintLn(out) == FALSE )                return PROBLEM;
   if ( PrintLn(out) == FALSE )                return PROBLEM;
   if ( PrintLn(out) == FALSE )                return PROBLEM;
   if ( PrintLn(out) == FALSE )                return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )            return PROBLEM;
   if ( Print(out,AQVAR::ini.T(41)) == FALSE ) return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )            return PROBLEM;

   AQVAR* var = varlist.First();
   while ( var != 0 )
   {
      JOBSTATUS status = var->Write(out,sh);
      if ( status != OK )                      return status;
      if ( cancelfun != 0 )
      {
         if ( cancelfun() == TRUE )            return INTERRUPT;
      }
      var = var->Next();
      if ( sh == FALSE )
      {
         if ( var != 0 ) PrintLn(out,'-');
         else            PrintLn(out,'*');
      }
   }

   if ( PrintLn(out) == FALSE )                return PROBLEM;

   return OK;
}

//////////////////////////////////////////////////////////////////////////////
