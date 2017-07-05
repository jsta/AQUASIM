////////////////////////////////   fitsys.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    17.12.91    Peter Reichert
// revisions:   17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "fitsys.h"

//////////////////////////////////////////////////////////////////////////////

const char* FitsysIdentifier = "FITSYS";
const char* FitIdentifier = "FIT";

const CARDINAL BufSize = 1024;

//////////////////////////////////////////////////////////////////////////////

CARDINAL FIT::number = 0;


INIT FIT::ini(initfile,"FITSYS","fit_txt_",10,10,10,30);


void FIT::init()
{
   calcnumber    = 0;
   inittime      = 0.0;
   initsteady    = FALSE;
   datastarttime = 0.0;
   dataendtime   = 1e9;
   numdata       = 0;
   data          = 0;
   fitactive     = FALSE;
   number++;
}


FIT::FIT()
{
   init();
   char Buf1[BufSize]; char Buf2[BufSize];
   strcpy(Buf1,ini.T(1));
   NumToString(number,Buf2,BufSize);
   strcat(Buf1,Buf2);
   if ( Symbol(Buf1) == FALSE )
      FatalError("FIT::FIT: illegal symbol");
   return;
}


FIT::FIT(const FIT* fit) : SYMBOL(fit)
{
   init();
   if ( fit != 0 )
   {
      CalcNum(fit->CalcNum());
      InitTime(fit->InitTime());
      InitSteady(fit->InitSteady());
      DataStartTime(fit->DataStartTime());
      DataEndTime(fit->DataEndTime());
      if ( fit->NumData() > 0 )
      {
         numdata = fit->NumData();
         data = new FITDATA[numdata];
         for ( CARDINAL i=0; i<numdata; i++ )
         {
            data[i] = fit->Data(i);
         }
      }
      FitActive(fit->FitActive());
   }
   else
   {
      char Buf1[BufSize]; char Buf2[BufSize];
      strcpy(Buf1,ini.T(1));
      NumToString(number,Buf2,BufSize);
      strcat(Buf1,Buf2);
      if ( Symbol(Buf1) == FALSE )
         FatalError("FIT::FIT: illegal symbol");
   }
}


FIT::~FIT()
{
   if ( number == 0 )
      FatalError("FIT::~FIT: already all FITs destructed");
   number--;
   delete data; numdata = 0;
}


BOOLEAN FIT::Arg(const AQVAR* var) const
{
   for ( CARDINAL i=0; i<numdata; i++ )
   {
      if ( data[i].var  == var ) return TRUE;
      if ( data[i].data == var ) return TRUE;
   }
   return FALSE;
}


BOOLEAN FIT::Arg(const AQCOMP* com) const
{
   for ( CARDINAL i=0; i<numdata; i++ )
   {
      if ( data[i].com == com ) return TRUE;
   }
   return FALSE;
}


void FIT::ReplaceVar(AQVAR* oldvar, AQVAR* newvar)
{
   for ( CARDINAL i=0; i<numdata; i++ )
   {
      if ( data[i].var  == oldvar ) data[i].var  = newvar;
      if ( data[i].data == oldvar ) data[i].data = newvar;
   }
   return;
}


void FIT::ExchangeVar(AQVAR* var1, AQVAR* var2)
{
   for ( CARDINAL i=0; i<numdata; i++ )
   {
      if ( data[i].var == var1 )
      {
         data[i].var = var2;
      }
      else
      {
         if ( data[i].var == var2 )
         {
            data[i].var = var1;
         }
      }
      if ( data[i].data == var1 )
      {
         data[i].data = var2;
      }
      else
      {
         if ( data[i].data == var2 )
         {
            data[i].data = var1;
         }
      }
   }
   return;
}


void FIT::ReplaceComp(AQCOMP* oldcomp, AQCOMP* newcomp)
{
   for ( CARDINAL i=0; i<numdata; i++ )
   {
      if ( data[i].com == oldcomp ) data[i].com = newcomp;
   }
   return;
}


BOOLEAN FIT::Add(FITDATA d, CARDINAL pos)
{
   if ( InList() == TRUE ) return FALSE;
   if ( d.var == 0 )       return FALSE;
   if ( d.com == 0 )       return FALSE;
   if ( d.data == 0 )      return FALSE;
   if ( pos > numdata ) pos = numdata;
   FITDATA* newdata = new FITDATA[numdata+1];
   CARDINAL i;
   for ( i=0; i<pos; i++ )  newdata[i] = data[i];
   newdata[pos] = d;
   for ( i=pos; i<numdata; i++ ) newdata[i+1] = data[i];
   numdata++;
   delete data; data = newdata;
   return TRUE;
}


BOOLEAN FIT::Replace(FITDATA d, CARDINAL pos)
{
   if ( InList() == TRUE ) return FALSE;
   if ( d.var == 0 )       return FALSE;
   if ( d.com == 0 )       return FALSE;
   if ( d.data == 0 )      return FALSE;
   if ( pos > numdata )    return FALSE;
   data[pos] = d;
   return TRUE;
}


BOOLEAN FIT::Delete(CARDINAL pos)
{
   if ( pos >= numdata )   return FALSE;
   if ( numdata == 1 )
   {
      delete data; data = 0;
      numdata = 0;
      fitactive = FALSE;
      return TRUE;
   }
   FITDATA* newdata = new FITDATA[numdata-1];
   CARDINAL i;
   for ( i=0; i<pos; i++ ) newdata[i] = data[i];
   for ( i=pos+1; i<numdata; i++ ) newdata[i-1] = data[i];
   numdata--;
   delete data; data = newdata;
   return TRUE;
}


void FIT::Delete()
{
   delete data; data = 0;
   numdata = 0;
   fitactive = FALSE;
   return;
}


BOOLEAN FIT::CalcNum(CARDINAL calcnum)
{
   if ( InList() == TRUE ) return FALSE;
   calcnumber = calcnum;
   return TRUE;
}


BOOLEAN FIT::FitActive(BOOLEAN act)
{
   if ( InList() == TRUE ) return FALSE;
   if ( act == TRUE )
   {
      if ( numdata == 0 ) return FALSE;
      fitactive = TRUE;
   }
   else
   {
      fitactive = FALSE;
   }
   return TRUE;
}


CARDINAL FIT::NumVarArg() const
{
   CARDINAL num = 0;
   for ( CARDINAL i=0; i<numdata; i++ )
   {
      if ( data[i].var != 0 )  num++;
      if ( data[i].data != 0 ) num++;
   }
   return num;
}


AQVAR* FIT::VarArg(CARDINAL index) const
{
   CARDINAL num = 0;
   for ( CARDINAL i=0; i<numdata; i++ )
   {
      if ( data[i].var != 0 )
      {
         if ( num == index ) return data[i].var;
         num++;
      }
      if ( data[i].data != 0 )
      {
         if ( num == index ) return data[i].data;
         num++;
      }
   }
   FatalError("FIT::VarArg: illegal index");
   return 0;
}


CARDINAL FIT::NumCompArg() const
{
   CARDINAL num = 0;
   for ( CARDINAL i=0; i<numdata; i++ )
   {
      if ( data[i].com != 0 ) num++;
   }
   return num;
}


AQCOMP* FIT::CompArg(CARDINAL index) const
{
   CARDINAL num = 0;
   for ( CARDINAL i=0; i<numdata; i++ )
   {
      if ( data[i].com != 0 )
      {
         if ( num == index ) return data[i].com;
         num++;
      }
   }
   FatalError("FIT::CompArg: illegal index");
   return 0;
}


const FITDATA FIT::Data(CARDINAL index) const
{
   if ( index >= numdata )
      FatalError("FIT::Data: illegal index");
   return data[index];
}


JOBSTATUS FIT::Load(std::istream& in,
                    const VARLIST* varlist,
                    const COMPLIST* complist)
{
   char Buffer[BufSize];
   if ( InList() == TRUE )                                    return PROBLEM;

   if ( LoadStart(in) == FALSE )                              return PROBLEM;
   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )                   return PROBLEM;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.0
         {
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                              return PROBLEM;
            if ( Symbol(Buffer) == FALSE )                    return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                              return PROBLEM;
            Description(Buffer);

            if ( LoadNumber(in,&calcnumber) == FALSE )        return PROBLEM;

            if ( LoadNumber(in,&inittime) == FALSE )          return PROBLEM;

            datastarttime = inittime;
            
			if ( LoadBoolean(in,&initsteady) == FALSE )       return PROBLEM;

            if ( LoadBoolean(in,&fitactive) == FALSE )        return PROBLEM;

            if ( LoadStart(in) == FALSE )                     return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               FITDATA d;
               d.var = varlist->Get(Buffer);  if ( d.var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                              return PROBLEM;
               d.com = complist->Get(Buffer); if ( d.com==0 ) return PROBLEM;
               if ( LoadNumber(in,&d.zone) == FALSE )         return PROBLEM;
               if ( LoadNumber(in,&d.spacetime) == FALSE )    return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                              return PROBLEM;
               d.data = varlist->Get(Buffer); 
                                             if ( d.data==0 ) return PROBLEM;
               if ( Add(d,numdata) == FALSE )                 return PROBLEM;
            }
            if ( numdata == 0 ) fitactive = FALSE;
         }
         break;
      case 2: // aquavers=2: AQUASIM 2.0: datastarttime, dataendtime added
         {
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                              return PROBLEM;
            if ( Symbol(Buffer) == FALSE )                    return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                              return PROBLEM;
            Description(Buffer);

            if ( LoadNumber(in,&calcnumber) == FALSE )        return PROBLEM;

            if ( LoadNumber(in,&inittime) == FALSE )          return PROBLEM;

            if ( LoadBoolean(in,&initsteady) == FALSE )       return PROBLEM;

            if ( LoadNumber(in,&datastarttime) == FALSE )     return PROBLEM;

            if ( LoadNumber(in,&dataendtime) == FALSE )       return PROBLEM;

            if ( LoadBoolean(in,&fitactive) == FALSE )        return PROBLEM;

            if ( LoadStart(in) == FALSE )                     return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               FITDATA d;
               d.var = varlist->Get(Buffer);  if ( d.var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                              return PROBLEM;
               d.com = complist->Get(Buffer); if ( d.com==0 ) return PROBLEM;
               if ( LoadNumber(in,&d.zone) == FALSE )         return PROBLEM;
               if ( LoadNumber(in,&d.spacetime) == FALSE )    return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                              return PROBLEM;
               d.data = varlist->Get(Buffer); 
                                             if ( d.data==0 ) return PROBLEM;
               if ( Add(d,numdata) == FALSE )                 return PROBLEM;
            }
            if ( numdata == 0 ) fitactive = FALSE;
         }
         break;
      case 3: // aquavers=3: AQUASIM 2.0: spacerel added
         {
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                              return PROBLEM;
            if ( Symbol(Buffer) == FALSE )                    return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                              return PROBLEM;
            Description(Buffer);

            if ( LoadNumber(in,&calcnumber) == FALSE )        return PROBLEM;

            if ( LoadNumber(in,&inittime) == FALSE )          return PROBLEM;

            if ( LoadBoolean(in,&initsteady) == FALSE )       return PROBLEM;

            if ( LoadNumber(in,&datastarttime) == FALSE )     return PROBLEM;

            if ( LoadNumber(in,&dataendtime) == FALSE )       return PROBLEM;

            if ( LoadBoolean(in,&fitactive) == FALSE )        return PROBLEM;

            if ( LoadStart(in) == FALSE )                     return PROBLEM;
            while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
            {
               FITDATA d;
               d.var = varlist->Get(Buffer);  if ( d.var==0 ) return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                              return PROBLEM;
               d.com = complist->Get(Buffer); if ( d.com==0 ) return PROBLEM;
               if ( LoadNumber(in,&d.zone) == FALSE )         return PROBLEM;
               if ( LoadNumber(in,&d.spacetime) == FALSE )    return PROBLEM;
               if ( LoadBoolean(in,&d.spacerel) == FALSE )    return PROBLEM;
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                              return PROBLEM;
               d.data = varlist->Get(Buffer); 
                                             if ( d.data==0 ) return PROBLEM;
               if ( Add(d,numdata) == FALSE )                 return PROBLEM;
            }
            if ( numdata == 0 ) fitactive = FALSE;
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                                return PROBLEM;
   return OK;
}


JOBSTATUS FIT::Save(std::ostream& out)
{
   if ( SaveStart(out) == FALSE )                              return PROBLEM;

   INTEGER aquavers = 3;
   if ( SaveNumber(out,aquavers) == FALSE )                    return PROBLEM;

   if ( SaveString(out,Symbol()) == FALSE )                    return PROBLEM;

   if ( SaveString(out,Description()) == FALSE )               return PROBLEM;

   if ( SaveNumber(out,CalcNum()) == FALSE )                   return PROBLEM;

   if ( SaveNumber(out,InitTime()) == FALSE )                  return PROBLEM;

   if ( SaveBoolean(out,InitSteady()) == FALSE )               return PROBLEM;

   if ( SaveNumber(out,DataStartTime()) == FALSE )             return PROBLEM;

   if ( SaveNumber(out,DataEndTime()) == FALSE )               return PROBLEM;

   if ( SaveBoolean(out,FitActive()) == FALSE )                return PROBLEM;

   if ( SaveStart(out) == FALSE )                              return PROBLEM;
   for ( CARDINAL i=0; i<numdata; i++ )
   {
      if ( SaveString(out,(data[i].var)->Symbol()) == FALSE )  return PROBLEM;
      if ( SaveString(out,(data[i].com)->Symbol()) == FALSE )  return PROBLEM;
      if ( SaveNumber(out,data[i].zone) == FALSE )             return PROBLEM;
      if ( SaveNumber(out,data[i].spacetime) == FALSE )        return PROBLEM;
      if ( SaveBoolean(out,data[i].spacerel) == FALSE )        return PROBLEM;
      if ( SaveString(out,(data[i].data)->Symbol()) == FALSE ) return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                                return PROBLEM;

   if ( SaveEnd(out) == FALSE )                                return PROBLEM;
   return FILEIO::Save(out);
}


JOBSTATUS FIT::Write(std::ostream& out, BOOLEAN)
{
   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   CARDINAL pos3 = pos1+2;
   CARDINAL pos4 = pos3+2;
   PrintIndent(pos2-1);

   if ( Print(out,Symbol(),1) == FALSE )               return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;

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
   if ( Print(out,CalcNum(),pos2) == FALSE )           return PROBLEM;

   if ( Print(out,ini.T(12),pos1) == FALSE )           return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;
   if ( Print(out,InitTime(),pos2) == FALSE )          return PROBLEM;

   if ( Print(out,ini.T(13),pos1) == FALSE )           return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;
   if ( InitSteady() == FALSE )
   {
      if ( Print(out,ini.T(14),pos2) == FALSE )        return PROBLEM;
   }
   else
   {
      if ( Print(out,ini.T(15),pos2) == FALSE )        return PROBLEM;
   }

   if ( Print(out,ini.T(17),pos1) == FALSE )           return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;
   if ( FitActive() == TRUE )
   {
      if ( Print(out,ini.T(10),pos2) == FALSE )        return PROBLEM;
   }
   else
   {
      if ( Print(out,ini.T(11),pos2) == FALSE )        return PROBLEM;
   }

   if ( Print(out,ini.T(16),pos1) == FALSE )           return PROBLEM;
   if ( Print(out,":") == FALSE )                      return PROBLEM;
   if ( NumData() > 0 )
   {
      const CARDINAL len = 13;
      PrintIndent(pos4-1);
      if ( Print(out,ini.T(4),pos3) == FALSE )         return PROBLEM;
      if ( Print(out," : ") == FALSE )                 return PROBLEM;
      if ( Print(out,ini.T(5)) == FALSE )              return PROBLEM;
      if ( Print(out," (") == FALSE )                  return PROBLEM;
      if ( Print(out,ini.T(6)) == FALSE )              return PROBLEM;
      if ( Print(out,",") == FALSE )                   return PROBLEM;
      if ( Print(out,ini.T(7)) == FALSE )              return PROBLEM;
      if ( Print(out,",") == FALSE )                   return PROBLEM;
      if ( Print(out,ini.T(8)) == FALSE )              return PROBLEM;
      if ( Print(out,")") == FALSE )                   return PROBLEM;
      for ( CARDINAL i=0; i<NumData(); i++ )
      {
         if ( Print(out,(data[i].data)->Symbol(),pos3) == FALSE )
                                                       return PROBLEM;
         if ( Print(out," : ") == FALSE )              return PROBLEM;
         if ( Print(out,(data[i].var)->Symbol()) == FALSE )
                                                       return PROBLEM;
         if ( Print(out," (") == FALSE )               return PROBLEM;
         AQCOMP* com = data[i].com;
         if ( Print(out,com->Symbol()) == FALSE )     return PROBLEM;
         if ( Print(out,",") == FALSE )               return PROBLEM;
         if ( Print(out,com->ZoneName(data[i].zone)) == FALSE )
                                                      return PROBLEM;
         if ( Print(out,",") == FALSE )               return PROBLEM;
         if ( Print(out,data[i].spacetime) == FALSE ) return PROBLEM;
         if ( data[i].spacerel == TRUE )
         {
            if ( Print(out," rel.space") == FALSE )   return PROBLEM;
         }
         if ( Print(out,")") == FALSE )               return PROBLEM;
      }
   }

   return OK;
}

//////////////////////////////////////////////////////////////////////////////

FITSYS::FITSYS()
{
#ifdef HEAPTEST
HeapFileMessage("start FITSYS::FITSYS()");
#endif

   cancelfun = 0;

#ifdef HEAPTEST
HeapFileMessage("end   FITSYS::FITSYS()");
#endif
}


FITSYS::~FITSYS()
{
#ifdef HEAPTEST
HeapFileMessage("start FITSYS::~FITSYS()");
#endif

   Delete();

#ifdef HEAPTEST
HeapFileMessage("end   FITSYS::~FITSYS()");
#endif
}


BOOLEAN FITSYS::Arg(const AQVAR* var) const
{
   FIT* fit = fitlist.First();
   while ( fit != 0 )
   {
      if ( fit->Arg(var) == TRUE ) return TRUE;
      fit = fit->Next();
   }
   return FALSE;
}


BOOLEAN FITSYS::Arg(const AQCOMP* com) const
{
   FIT* fit = fitlist.First();
   while ( fit != 0 )
   {
      if ( fit->Arg(com) == TRUE ) return TRUE;
      fit = fit->Next();
   }
   return FALSE;
}


void FITSYS::ReplaceVar(AQVAR* oldvar, AQVAR* newvar)
{
   FIT* fit = fitlist.First();
   while ( fit != 0 )
   {
      fit->ReplaceVar(oldvar,newvar);
      fit = fit->Next();
   }
   return;
}


void FITSYS::ExchangeVar(AQVAR* var1, AQVAR* var2)
{
   FIT* fit = fitlist.First();
   while ( fit != 0 )
   {
      fit->ExchangeVar(var1,var2);
      fit = fit->Next();
   }
   return;
}


void FITSYS::ReplaceComp(AQCOMP* oldcomp, AQCOMP* newcomp)
{
   FIT* fit = fitlist.First();
   while ( fit != 0 )
   {
      fit->ReplaceComp(oldcomp,newcomp);
      fit = fit->Next();
   }
   return;
}


BOOLEAN FITSYS::Add(FIT* fit)
{
   if ( fit == 0 )                     return FALSE;
   if ( Exist(fit->Symbol()) == TRUE ) return FALSE;
   FIT* f = fitlist.First();
   if ( fit->FitActive() == TRUE )
   {
      while ( f != 0 )
      {
         if ( f->CalcNum() == fit->CalcNum() )
         {
            if ( f->FitActive() == TRUE ) return FALSE;
         }
         f = f->Next();
      }
   }
   if ( fitlist.Add(fit) == FALSE )
      FatalError("FITSYS::Add: cannot add fit");
   return TRUE;
}


BOOLEAN FITSYS::Replace(FIT* oldfit, FIT* newfit)
{
   if ( newfit == 0 )                                        return FALSE;
   if ( Exist(oldfit) == FALSE )                             return FALSE;
   if ( ( strcmp(oldfit->Symbol(),newfit->Symbol()) != 0 )
        &&             ( Exist(newfit->Symbol()) == TRUE ) ) return FALSE;
   if ( newfit->FitActive() == TRUE )
   {
      FIT* f = fitlist.First();
      while ( f != 0 )
      {
         if ( f != oldfit )
         {
            if ( f->CalcNum() == newfit->CalcNum() )
            {
               if ( f->FitActive() == TRUE ) return FALSE;
            }
         }
         f = f->Next();
      }
   }
   if ( fitlist.Add(newfit) == FALSE )
      FatalError("FITSYS::Replace: cannot insert new fit");
   if ( fitlist.Remove(oldfit) == FALSE )
      FatalError("FITSYS::Replace: cannot remove old fit");
   return TRUE;
}


BOOLEAN FITSYS::Delete(FIT* fit)
{
   if ( fit == 0 )   return FALSE;
   BOOLEAN b = fitlist.Remove(fit);
   if ( b == FALSE ) return FALSE;
   delete fit;
   return TRUE;
}


void FITSYS::Delete()
{
   FIT* fit = fitlist.First();
   while ( fit != 0 )
   {
      if ( fitlist.Remove(fit) == FALSE )
         FatalError("FITSYS::Delete: unable to remove fit");
      delete fit;
      fit = fitlist.First();
   }
}


BOOLEAN FITSYS::FitActive(FIT* fit, BOOLEAN act)
{
   if ( fit == 0 ) return FALSE;
   if ( act == FALSE )
   {
      fit->fitactive = FALSE;
      return TRUE;
   }
   if ( fit->numdata == 0 ) return FALSE;
   CARDINAL calcnum = fit->CalcNum();
   FIT* f = fitlist.First();
   BOOLEAN found = FALSE;
   while ( f != 0 )
   {
      if ( f == fit )
      {
         found = TRUE;
      }
      else
      {
         if ( f->CalcNum() == calcnum )
         {
            if ( f->FitActive() == TRUE ) return FALSE;
         }
      }
      f = f->Next();
   }
   if ( found == FALSE ) return FALSE;
   fit->fitactive = TRUE;
   return TRUE;
}


JOBSTATUS FITSYS::Load(std::istream& in,
                       const VARLIST* varlist,
                       const COMPLIST* complist)
{
   char Buffer[BufSize];
   // Delete();

   // read identifier and global parameters:

   if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE ) return PROBLEM;
   if ( strcmp(Buffer,FitsysIdentifier) != 0 )          return PROBLEM;
   if ( LoadStart(in) == FALSE )                        return PROBLEM;

   // read fit:

   while ( LoadString(in,Buffer,sizeof(Buffer)) == TRUE )
   {
      if ( strcmp(Buffer,FitIdentifier) != 0 )          return PROBLEM;
      FIT* fit = new FIT;
      JOBSTATUS status =  fit->Load(in,varlist,complist);
      if ( status != OK )
      {
         delete fit; return status;
      }
      if ( Add(fit) == FALSE )
      {
         delete fit; return PROBLEM;
      }
      if ( cancelfun != 0 )
      {
         if ( cancelfun() == TRUE )                       return INTERRUPT;
      }
   }
   return OK;
}


JOBSTATUS FITSYS::Save(std::ostream& out)
{
   if ( SaveString(out,FitsysIdentifier) == FALSE )      return PROBLEM;
   if ( SaveStart(out) == FALSE )                        return PROBLEM;

   FIT* fit = fitlist.First();
   while ( fit != 0 )
   {
      if ( SaveString(out,FitIdentifier) == FALSE )      return PROBLEM;
      JOBSTATUS status = fit->Save(out);
      if ( status != OK ) return status;
      if ( cancelfun != 0 )
      {
         if ( cancelfun() == TRUE )                       return INTERRUPT;
      }
      fit = fit->Next();
   }

   if ( SaveEnd(out) == FALSE )                          return PROBLEM;
   return OK;
}


JOBSTATUS FITSYS::Write(std::ostream& out, BOOLEAN)
{
   if ( fitlist.Size() == 0 )               return OK;

   if ( PrintLn(out) == FALSE )             return PROBLEM;
   if ( PrintLn(out) == FALSE )             return PROBLEM;
   if ( PrintLn(out) == FALSE )             return PROBLEM;
   if ( PrintLn(out) == FALSE )             return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )         return PROBLEM;
   if ( Print(out,FIT::ini.T(9)) == FALSE ) return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )         return PROBLEM;

   FIT* fit = fitlist.First();
   while ( fit != 0 )
   {
      JOBSTATUS status = fit->Write(out);
      if ( status != OK ) return status;
      if ( cancelfun != 0 )
      {
         if ( cancelfun() == TRUE )         return INTERRUPT;
      }
      fit = fit->Next();
      if ( fit != 0 ) PrintLn(out,'-');
      else            PrintLn(out,'*');
   }

   if ( PrintLn(out) == FALSE )             return PROBLEM;

   return OK;
}

//////////////////////////////////////////////////////////////////////////////

