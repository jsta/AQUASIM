////////////////////////////////   linksys.C   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    15.10.92    Peter Reichert
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "linksys.h"
#include "varsys.h"

//////////////////////////////////////////////////////////////////////////////

// Identifiers for LINKSYS::Save and LINKSYS::Load
// (compare this constant with enum AQLINKTYPE from aqlink.h)

                                                       //  enum AQLINKTYPE:
const char* const LinkIdentifier[] = {
                                        "ADVLINK",     //  AdvLink
                                        "DIFFLINK"     //  DiffLink
                                     };

const CARDINAL NumLink = sizeof(LinkIdentifier)/sizeof(char*);

const char* LinksysIdentifier = "LINKSYS";

//////////////////////////////////////////////////////////////////////////////

LINKSYS::LINKSYS()
{
#ifdef HEAPTEST
HeapFileMessage("start LINKSYS::LINKSYS()");
#endif

   cancelfun = 0;

#ifdef HEAPTEST
HeapFileMessage("end LINKSYS::LINKSYS()");
#endif
}


LINKSYS::~LINKSYS()
{
#ifdef HEAPTEST
HeapFileMessage("start LINKSYS::~LINKSYS()");
#endif

   Delete();

#ifdef HEAPTEST
HeapFileMessage("end   LINKSYS::~LINKSYS()");
#endif
}

void LINKSYS::SetLinkIndex(VARSYS* varsys)
{
   varsys->SetProgVar(LinkIndex,&linkindex);
   return;
}


BOOLEAN LINKSYS::AllowedReplaceVar(const AQVAR* oldvar,
                                   const AQVAR* newvar) const
{
   AQLINK* link = linklist.First();
   while ( link != 0 )
   {
      if ( link->AllowedReplaceVar(oldvar,newvar) == FALSE ) return FALSE;
      link = link->Next();
   }
   return TRUE;
}


BOOLEAN LINKSYS::AllowedExchangeVar(const AQVAR* var1,
                                    const AQVAR* var2) const
{
   AQLINK* link = linklist.First();
   while ( link != 0 )
   {
      if ( link->AllowedExchangeVar(var1,var2) == FALSE ) return FALSE;
      link = link->Next();
   }
   return TRUE;
}


BOOLEAN LINKSYS::AllowedReplaceComp(const AQCOMP* oldcomp,
                                    const AQCOMP* newcomp) const
{
   AQLINK* link = linklist.First();
   while ( link != 0 )
   {
      if ( link->AllowedReplaceComp(oldcomp,newcomp) == FALSE ) return FALSE;
      link = link->Next();
   }
   return TRUE;
}


BOOLEAN LINKSYS::Add(AQLINK* link)
{
   if ( link == 0 )                     return FALSE;
   if ( Exist(link->Symbol()) == TRUE ) return FALSE;
   if ( linklist.Add(link) == FALSE )
      FatalError("LINKSYS::Add: cannot add link");
   return TRUE;
}


BOOLEAN LINKSYS::Replace(AQLINK* oldlink, AQLINK* newlink)
{
   if ( newlink == 0 )                                        return FALSE;
   if ( Exist(oldlink) == FALSE )                             return FALSE;
   if ( ( strcmp(oldlink->Symbol(),newlink->Symbol()) != 0 )
        &&             ( Exist(newlink->Symbol()) == TRUE ) ) return FALSE;
   if ( linklist.Remove(oldlink) == FALSE )
      FatalError("LINKSYS::Replace: cannot remove old link");
   if ( linklist.Add(newlink) == FALSE )
      FatalError("LINKSYS::Replace: cannot insert new link");
   return TRUE;
}


BOOLEAN LINKSYS::Delete(AQLINK* link)
{
   if ( link == 0 )  return FALSE;
   BOOLEAN b = linklist.Remove(link);
   if ( b == FALSE ) return FALSE;
   delete link;
   return TRUE;
}


void LINKSYS::Delete()
{
   AQLINK* link = linklist.First();
   while ( link != 0 )
   {
      if ( linklist.Remove(link) == FALSE )
         FatalError("LINKSYS::Delete: unable to remove link");
      delete link;
      link = linklist.First();
   }
   return;
}


void LINKSYS::ReplaceComp(AQCOMP* oldcomp, AQCOMP* newcomp)
{
   AQLINK* l = linklist.First();
   while ( l != 0 )
   {
      l->ReplComp(oldcomp,newcomp);
      l = l->Next();
   }
   return;
}


void LINKSYS::ReplaceVar(AQVAR* oldvar, AQVAR* newvar)
{
   AQLINK* l = linklist.First();
   while ( l != 0 )
   {
      l->ReplVar(oldvar,newvar);
      l = l->Next();
   }
   return;
}


void LINKSYS::ExchangeVar(AQVAR* var1, AQVAR* var2)
{
   AQLINK* l = linklist.First();
   while ( l != 0 )
   {
      l->ExchVar(var1,var2);
      l = l->Next();
   }
}


BOOLEAN LINKSYS::Arg(const AQVAR* var) const
{
   AQLINK* l = linklist.First();
   while ( l != 0 )
   {
      if ( l->Arg(var) == TRUE ) return TRUE;
      l = l->Next();
   }
   return FALSE;
}


BOOLEAN LINKSYS::Arg(const AQCOMP* com) const
{
   AQLINK* l = linklist.First();
   while ( l != 0 )
   {
      if ( l->Arg(com) == TRUE ) return TRUE;
      l = l->Next();
   }
   return FALSE;
}


JOBSTATUS LINKSYS::Load(std::istream& in,
                        const VARLIST* varlist,
                        const COMPLIST* complist)
{
   char Buffer[32];
   // Delete();

// read identifier and global parameters:

   if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE ) return PROBLEM;
   if ( strcmp(Buffer,LinksysIdentifier) != 0 )      return PROBLEM;
   if ( LoadStart(in) == FALSE )                        return PROBLEM;

// read links:

   while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
   {
      CARDINAL type = 0;
      while ( type < NumLink )
      {
         if ( strcmp(Buffer,LinkIdentifier[type]) == 0 ) break;
         type++;
      }
      JOBSTATUS status;
      switch ( type )
      {
         case AdvLink:
            {
               ADVLINK* al = new ADVLINK;
               status = al->Load(in,varlist,complist);
               if ( status != OK )
               {
                  delete al; return status;
               }
               if ( Add(al) == FALSE )
               {
                  delete al; return PROBLEM;
               }
            }
            break;
         case DiffLink:
            {
               DIFFLINK* dl = new DIFFLINK;
               status = dl->Load(in,varlist,complist);
               if ( status != OK )
               {
                  delete dl; return status;
               }
               if ( Add(dl) == FALSE )
               {
                  delete dl; return PROBLEM;
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


JOBSTATUS LINKSYS::Save(std::ostream& out)
{
   if ( SaveString(out,LinksysIdentifier) == FALSE )      return PROBLEM;
   if ( SaveStart(out) == FALSE )                         return PROBLEM;
   
   // save links:

   AQLINK* node = linklist.First();
   while ( node !=0 )
   {
      if ( node->Type() > NumLink )
         FatalError("LINKSYS::Save: Unknown Link Type");
      if ( SaveString(out,LinkIdentifier[node->Type()]) == FALSE )
                                                          return PROBLEM;
      JOBSTATUS status = node->Save(out);
      if ( status != OK ) return status;
      if ( cancelfun != 0 )
      {
         if ( cancelfun() == TRUE )                       return INTERRUPT;
      }
      node = node->Next();
   }

   if ( SaveEnd(out) == FALSE )                           return PROBLEM;
   return OK;
}


JOBSTATUS LINKSYS::Write(std::ostream& out, BOOLEAN sh)
{
   if ( linklist.Size() == 0 )                 return OK;

   if ( PrintLn(out) == FALSE )                return PROBLEM;
   if ( PrintLn(out) == FALSE )                return PROBLEM;
   if ( PrintLn(out) == FALSE )                return PROBLEM;
   if ( PrintLn(out) == FALSE )                return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )            return PROBLEM;
   if ( Print(out,AQLINK::ini.T(4)) == FALSE ) return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )            return PROBLEM;

   AQLINK* link = linklist.First();
   while ( link != 0 )
   {
      JOBSTATUS status = link->Write(out, sh); 
	                       if ( status != OK ) return status;
      if ( cancelfun != 0 )
      {
         if ( cancelfun() == TRUE )            return INTERRUPT;
      }
      link = link->Next();
	  if ( sh == FALSE )
	  {
         if ( link != 0 ) PrintLn(out,'-');
         else             PrintLn(out,'*');
	  }
   }

   if ( PrintLn(out) == FALSE )                 return PROBLEM;

   return OK;
}

//////////////////////////////////////////////////////////////////////////////
