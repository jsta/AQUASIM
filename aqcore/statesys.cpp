///////////////////////////////   statesys.C   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    12.12.91    Peter Reichert
// revisions:   17.06.93    Peter Reichert    Sensitivity analysis added
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
//
// File format:
//
//   {STATESYS}
//   {
//      {neq}
//      {calcnum_1}
//      {
//         {t_1}
//         {
//            {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//            {par_1}  {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//            {par_2}  {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//      
//      
//            {par_np} {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//         }
//         {t_2}
//         {
//            {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//            {par_1}  {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//            {par_2}  {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//      
//      
//            {par_np} {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//         }
//      
//      
//         {t_nt}
//         {
//            {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//            {par_1}  {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//            {par_2}  {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//      
//      
//            {par_np} {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//         }
//      }
//      {calcnum_2}
//      {
//         {t_1}
//         {
//            {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//            {par_1}  {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//            {par_2}  {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//      
//      
//            {par_np} {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//         }
//         {t_2}
//         {
//            {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//            {par_1}  {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//            {par_2}  {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//      
//      
//            {par_np} {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//         }
//      
//      
//         {t_nt}
//         {
//            {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//            {par_1}  {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//            {par_2}  {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//      
//      
//            {par_np} {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//         }
//      }
//      
//      
//      {calcnum_nc}
//      {
//         {t_1}
//         {
//            {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//            {par_1}  {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//            {par_2}  {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//      
//      
//            {par_np} {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//         }
//         {t_2}
//         {
//            {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//            {par_1}  {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//            {par_2}  {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//      
//      
//            {par_np} {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//         }
//      
//      
//         {t_nt}
//         {
//            {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//            {par_1}  {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//            {par_2}  {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//      
//      
//            {par_np} {y1}{y2} ... {yneq}{yt1}{yt2} ... {ytneq}
//         }
//      }
//   }
//
//
//////////////////////////////////////////////////////////////////////////////

#include "init.h"
#include "statesys.h"

//////////////////////////////////////////////////////////////////////////////

const char* StatesysIdentifier = "STATESYS";

const CARDINAL BufSize = 1024;

//////////////////////////////////////////////////////////////////////////////

STATE::STATE(CARDINAL neq, REAL t, const REAL* y, const REAL* yt)
{
   s_neq = neq;
   s_t   = t;
   s_y   = new REAL[neq];
   s_yt  = new REAL[neq];
   for ( CARDINAL i=0; i<neq; i++ )
   {
      s_y[i]  = y[i];
      s_yt[i] = yt[i];
   }
}

STATE::~STATE()
{
   s_neq = 0;
   delete s_y;  s_y  = 0;
   delete s_yt; s_yt = 0;
}
   

//////////////////////////////////////////////////////////////////////////////

INIT STATESYS::ini(initfile,"STATESYS","state_txt_",10,10,10,10);


STATESYS::STATESYS()
{
#ifdef HEAPTEST
HeapFileMessage("start STATESYS::STATESYS()");
#endif

   numcalc   = 0;
   calcnums  = 0;
   numstates = 0;
   states    = 0;
   numpars   = 0;
   pars      = 0;
   cancelfun = 0;

#ifdef HEAPTEST
HeapFileMessage("end   STATESYS::STATESYS()");
#endif
}


STATESYS::~STATESYS()
{
#ifdef HEAPTEST
HeapFileMessage("start STATESYS::~STATESYS()");
#endif

   Delete();

#ifdef HEAPTEST
HeapFileMessage("end   STATESYS::~STATESYS()");
#endif
}


CARDINAL STATESYS::NumEq() const
{
   if ( numcalc > 0 ) return states[0][0][0]->NumEq();
   else               return 0;
}


CARDINAL STATESYS::NumStates(CARDINAL calcnum) const
{
   for ( CARDINAL i=0; i<numcalc; i++ )
   {
      if ( calcnums[i] == calcnum ) return numstates[i];
   }
   return 0;
}


CARDINAL STATESYS::CalcNum(CARDINAL index) const
{
   if ( index >= numcalc )
      FatalError("STATESYS::CalcNum: illegal index");
   return calcnums[index];
}


CARDINAL STATESYS::NumPar(CARDINAL calcnum) const
{
   for ( CARDINAL i=0; i<numcalc; i++ )
   {
      if ( calcnums[i] == calcnum ) return numpars[i];
   }
   return 0;
}


AQVAR* STATESYS::Par(CARDINAL calcnum, CARDINAL index) const
{
   for ( CARDINAL i=0; i<numcalc; i++ )
   {
      if ( calcnums[i] == calcnum )
      {
         if ( index >= numpars[i] )
            FatalError("STATESYS::Par: illegal index");
         return pars[i][index];
      }
   }
   return 0;
}


BOOLEAN STATESYS::ExistPar(CARDINAL calcnum, AQVAR* par) const
{
   for ( CARDINAL i=0; i<numcalc; i++ )
   {
      if ( calcnums[i] == calcnum )
      {
         for ( CARDINAL j=0; j<numpars[i]; j++ )
         {
            if ( pars[i][j] == par ) return TRUE;
         }
      }
   }
   return FALSE;
}


const STATE* STATESYS::GetState(CARDINAL calcnum, CARDINAL statenum,
                                const AQVAR* par) const
{
   for ( CARDINAL i=0; i<numcalc; i++ )
   {
      if ( calcnums[i] == calcnum )
      {
         if ( statenum < numstates[i] )
         {
            if ( par == 0 )
            {
               return states[i][statenum][0];
            }
            else
            {
               for ( CARDINAL k=0; k<numpars[i]; k++ )
               {
                  if ( par == pars[i][k] )
                     return states[i][statenum][k+1];
               }
               return 0;
            }
         }
         else
         {
            return 0;
         }
      }
   }
   return 0;
}


BOOLEAN STATESYS::AddState(CARDINAL calcnum, CARDINAL neq,
                           REAL t, const REAL* y, const REAL* yt,
                           AQVAR* par)
{
   if ( neq < 1 ) return FALSE;
   if ( NumEq() > 0 )
   {
      if ( NumEq() != neq ) return FALSE;
   }
   CARDINAL i;
   for ( i=0; i<numcalc; i++ )
   {
      if ( calcnums[i] == calcnum )
      {
         // calculations with this calculation number already exist:

         if ( par == 0 )
         {
            // append base calculation:

            if ( states[i][numstates[i]-1][0]->Time() < t )
            {
               // COMPILER PROBLEM with Codewarrior:
               // STATE*** newstates = new STATE**[numstates[i]+1];
               typedef STATE** STATEpp;
               STATE*** newstates = new STATEpp[numstates[i]+1];
               for ( CARDINAL j=0; j<numstates[i]; j++ )
               {
                  newstates[j] = states[i][j];
               }
               newstates[numstates[i]] = new STATE*[numpars[i]+1];
               newstates[numstates[i]][0] = new STATE(neq,t,y,yt);
               for ( CARDINAL k=0; k<numpars[i]; k++ ) 
                  newstates[numstates[i]][k+1] = 0;
               delete states[i]; states[i] = newstates;
               numstates[i]++;
               return TRUE;
            }
            else
            {
               return FALSE;
            }
         }
         else
         {
            for ( CARDINAL k=0; k<numpars[i]; k++ )
            {
               // add calculation for existing parameter:

               if ( par == pars[i][k] )
               {
                  for ( CARDINAL j=0; j<numstates[i]; j++ )
                  {
                     if ( t == states[i][j][0]->Time() )
                     {
                        if ( states[i][j][k+1] != 0 ) delete states[i][j][k+1];
                        states[i][j][k+1] = new STATE(neq,t,y,yt);
                        return TRUE;
                     }
                  }
                  return FALSE;
               }
            }

            // add first calculation for new parameter:

            for ( CARDINAL j=0; j<numstates[i]; j++ )
            {
               if ( t == states[i][j][0]->Time() )
               {
                  AQVAR** newpars = new AQVAR*[numpars[i]+1];
                  for ( CARDINAL k=0; k<numpars[i]; k++ )
                  {
                     newpars[k] = pars[i][k];
                  }
                  newpars[numpars[i]] = par;
                  delete pars[i]; pars[i] = newpars;
                  for ( CARDINAL jj=0; jj<numstates[i]; jj++ )
                  {
                     STATE** newstates = new STATE*[numpars[i]+2];
                     for ( CARDINAL k=0; k<numpars[i]+1; k++ )
                     {
                        newstates[k] = states[i][jj][k];
                     }
                     newstates[numpars[i]+1] = 0;
                     delete states[i][jj]; states[i][jj] = newstates;
                  }
                  states[i][j][numpars[i]+1] = new STATE(neq,t,y,yt);
                  numpars[i]++;
                  return TRUE;
               }
            }
            return FALSE;
         }
      }
   }
   if ( par != 0 ) return FALSE;

   // base calculation for new calculation number:

   CARDINAL* newcalcnums  = new CARDINAL[numcalc+1];
   CARDINAL* newnumstates = new CARDINAL[numcalc+1];
   CARDINAL* newnumpars   = new CARDINAL[numcalc+1];
   // COMPILER PROBLEM with Codewarrior:
   // AQVAR***  newpars      = new AQVAR**[numcalc+1];
   typedef AQVAR** AQVARpp;
   AQVAR***  newpars      = new AQVARpp[numcalc+1];
   // COMPILER PROBLEM with Codewarrior:
   // STATE**** newstates    = new STATE***[numcalc+1];
   typedef STATE*** STATEppp;
   STATE**** newstates    = new STATEppp[numcalc+1];
   for ( i=0; i<numcalc; i++ )
   {
      newcalcnums[i]  = calcnums[i];
      newnumstates[i] = numstates[i];
      newnumpars[i]   = numpars[i];
      newpars[i]      = pars[i];
      newstates[i]    = states[i];
   }
   newcalcnums[numcalc]     = calcnum;
   newnumstates[numcalc]    = 1;
   newnumpars[numcalc]      = 0;
   newpars[numcalc]         = 0;
   // COMPILER PROBLEM with Codewarrior:
   // newstates[numcalc]       = new STATE**[1];
   typedef STATE** STATEpp;
   newstates[numcalc]       = new STATEpp[1];
   newstates[numcalc][0]    = new STATE*[1];
   newstates[numcalc][0][0] = new STATE(neq,t,y,yt);
   delete calcnums;  calcnums  = newcalcnums;
   delete numstates; numstates = newnumstates;
   delete states;    states    = newstates;
   delete numpars;   numpars   = newnumpars;
   delete pars;      pars      = newpars;
   numcalc++;
   return TRUE;
}


BOOLEAN STATESYS::DeleteState(CARDINAL calcnum, CARDINAL statenum)
{
   CARDINAL i;
   for ( i=0; i<numcalc; i++ )
   {
      if ( calcnums[i] == calcnum )
      {
         if ( statenum < numstates[i] )
         {
            if ( numstates[i] == 1 )
            {
               return DeleteStates(calcnum);
            }
            else
            {
               // COMPILER PROBLEM with Codewarrior:
               // STATE*** newstates = new STATE**[numstates[i]-1];
               typedef STATE** STATEpp;
               STATE*** newstates = new STATEpp[numstates[i]-1];
               CARDINAL j;
               for ( j=0; j<statenum; j++ )
               {
                  newstates[j] = states[i][j];
               }
               for ( j=statenum+1; j<numstates[i]; j++ )
               {
                  newstates[j-1] = states[i][j];
               }
               delete states[i]; states[i] = newstates;
               numstates[i]--;
               return TRUE;
            }
         }
      }
   }
   return FALSE;
}


BOOLEAN STATESYS::DeleteStates(CARDINAL calcnum)
{
   CARDINAL i;
   for ( i=0; i<numcalc; i++ )
   {
      if ( calcnums[i] == calcnum )
      {
         if ( numcalc == 1 )
         {
            Delete();
            return TRUE;
         }
         CARDINAL* newcalcnums  = new CARDINAL[numcalc-1];
         CARDINAL* newnumstates = new CARDINAL[numcalc-1];
         // COMPILER PROBLEM with Codewarrior:
         // STATE**** newstates    = new STATE***[numcalc-1];
         typedef STATE*** STATEppp;
         STATE**** newstates    = new STATEppp[numcalc-1];
         CARDINAL* newnumpars   = new CARDINAL[numcalc-1];
         // COMPILER PROBLEM with Codewarrior:
         // AQVAR***  newpars      = new AQVAR**[numcalc-1];
         typedef AQVAR** AQVARpp;
         AQVAR***  newpars      = new AQVARpp[numcalc-1];
         CARDINAL j;
         for ( j=0; j<i; j++ )
         {
            newcalcnums[j]  = calcnums[j];
            newnumstates[j] = numstates[j];
            newstates[j]    = states[j];
            newnumpars[j]   = numpars[j];
            newpars[j]      = pars[j];
         }
         for ( j=i+1; j<numcalc; j++ )
         {
            newcalcnums[j-1]  = calcnums[j];
            newnumstates[j-1] = numstates[j];
            newstates[j-1]    = states[j];
            newnumpars[j-1]   = numpars[j];
            newpars[j-1]      = pars[j];
         }
         for ( j=0; j<numstates[i]; j++ )
         {
            for ( CARDINAL k=0; k<numpars[i]+1; k++ )
            {
               if ( states[i][j][k] != 0 ) delete states[i][j][k];
            }
            delete states[i][j];
         }
         delete states[i];
         delete pars[i];
         delete states;    states    = newstates;
         delete numstates; numstates = newnumstates;
         delete calcnums;  calcnums  = newcalcnums;
         delete numpars;   numpars   = newnumpars;
         delete pars;      pars      = newpars;
         numcalc--;
         return TRUE;
      }
   }
   return FALSE;
}


void STATESYS::Delete()
{
   for ( CARDINAL i=0; i<numcalc; i++ )
   {
      for ( CARDINAL j=0; j<numstates[i]; j++ )
      {
         for ( CARDINAL k=0; k<numpars[i]+1; k++ )
         {
            if ( states[i][j][k] != 0 ) delete states[i][j][k];
         }
         delete states[i][j];
      }
      delete states[i];
      delete pars[i];
   }
   delete states; states = 0;
   delete numstates; numstates = 0;
   delete calcnums; calcnums = 0;
   delete pars; pars = 0;
   delete numpars; numpars = 0;
   numcalc = 0;
}


JOBSTATUS STATESYS::LoadStates(std::istream& in, const VARLIST* varlist,
                               SAVELOADFUN fun)
{
   // Delete();
   char Buffer[BufSize];

   if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )        return PROBLEM;
   if ( strcmp(Buffer,StatesysIdentifier) != 0 )               return PROBLEM;
   if ( LoadStart(in) == FALSE )                               return PROBLEM;

   CARDINAL calcnum, neq; REAL t;

   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )   /* no states */  return OK;
   if ( fun != 0 )
   {
      if ( fun() == FALSE )                                    return OK;
   }
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.0
         {
            if ( LoadNumber(in,&neq) == FALSE )                return PROBLEM;
            REAL* y  = new REAL[neq];
            REAL* yt = new REAL[neq];
            while ( LoadNumber(in,&calcnum) == TRUE )
            {
               if ( LoadStart(in) == FALSE )                   return PROBLEM;
               while ( LoadNumber(in,&t) == TRUE )
               {
                  if ( LoadStart(in) == FALSE )                return PROBLEM;
                  CARDINAL i;
                  for ( i=0; i<neq; i++ ) LoadNumber(in,&y[i]);
                  for ( i=0; i<neq; i++ ) LoadNumber(in,&yt[i]);
                  if ( AddState(calcnum,neq,t,y,yt) == FALSE ) return PROBLEM;
                  if ( cancelfun != 0 )
                  {
                     if ( cancelfun() == TRUE )
                     {
                        delete y;
                        delete yt;
                        return INTERRUPT;
                     }
                  }
                  while ( LoadString(in,Buffer,BufSize) == TRUE )
                  {
                     AQVAR* par = varlist->Get(Buffer);
                     if ( par == 0 )                           return PROBLEM;
                     for ( i=0; i<neq; i++ ) LoadNumber(in,&y[i]);
                     for ( i=0; i<neq; i++ ) LoadNumber(in,&yt[i]);
                     if ( AddState(calcnum,neq,t,y,yt,par) == FALSE )
                                                               return PROBLEM;
                     if ( cancelfun != 0 )
                     {
                        if ( cancelfun() == TRUE )
                        {
                           delete y;
                           delete yt;
                           return INTERRUPT;
                        }
                     }
                  }
               }
            }
            delete y;
            delete yt;
         }
         break;
      default:
         return PROBLEM;
   }
   return OK;
}


JOBSTATUS STATESYS::SaveStates(std::ostream& out, SAVELOADFUN fun)
{
   if ( SaveString(out,StatesysIdentifier) == FALSE )        return PROBLEM;
   if ( SaveStart(out) == FALSE )                            return PROBLEM;

   if ( numcalc > 0 )
   {
      if ( fun != 0 )
      {
         if ( fun() == FALSE )
         {
            if ( SaveEnd(out) == FALSE )                     return PROBLEM;
            return OK;
         }
      }
      CARDINAL neq = states[0][0][0]->NumEq();
      INTEGER aquavers = 1;
      if ( SaveNumber(out,aquavers) == FALSE )               return PROBLEM;
      if ( SaveNumber(out,neq) == FALSE )                    return PROBLEM;
      for ( CARDINAL i=0; i<numcalc; i++ )
      {
         if ( SaveNumber(out,calcnums[i]) == FALSE )         return PROBLEM;
         if ( SaveStart(out) == FALSE )                      return PROBLEM;
         for ( CARDINAL j=0; j<numstates[i]; j++ )
         {
            if ( SaveNumber(out,states[i][j][0]->Time()) == FALSE )
                                                             return PROBLEM;
            if ( SaveStart(out) == FALSE )                   return PROBLEM;
            CARDINAL l;
            const REAL* y = states[i][j][0]->Y();
            for ( l=0; l<neq; l++ ) SaveNumber(out,y[l]);
            const REAL* yt = states[i][j][0]->YT();
            for ( l=0; l<neq; l++ ) SaveNumber(out,yt[l]);
            if ( cancelfun != 0 )
            {
               if ( cancelfun() == TRUE )                    return INTERRUPT;
            }
            for ( CARDINAL k=0; k<numpars[i]; k++ )
            {
               if ( SaveString(out,pars[i][k]->Symbol()) == FALSE )
                                                             return PROBLEM;
               y = states[i][j][k+1]->Y();
               for ( l=0; l<neq; l++ ) SaveNumber(out,y[l]);
               yt = states[i][j][k+1]->YT();
               for ( l=0; l<neq; l++ ) SaveNumber(out,yt[l]);
               if ( cancelfun != 0 )
               {
                  if ( cancelfun() == TRUE )                 return INTERRUPT;
               }
            }
            if ( SaveEnd(out) == FALSE )                     return PROBLEM;
         }
         if ( SaveEnd(out) == FALSE )                        return PROBLEM;
      }
   }
   
   if ( SaveEnd(out) == FALSE )                              return PROBLEM;

   return OK;
}


JOBSTATUS STATESYS::Write(std::ostream& out, BOOLEAN)
{
   if ( NumCalc() == 0 )                                     return OK;
   if ( PrintLn(out) == FALSE )                              return PROBLEM;
   if ( PrintLn(out) == FALSE )                              return PROBLEM;
   if ( PrintLn(out) == FALSE )                              return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )                          return PROBLEM;
   if ( Print(out,ini.T(1)) == FALSE )                       return PROBLEM;
   if ( PrintLn(out,'*') == FALSE )                          return PROBLEM;

   CARDINAL pos1 = 1;
   CARDINAL pos2 = pos1+ini.L(1);
   CARDINAL pos3 = pos2+ini.L(2);
   CARDINAL pos4 = pos3+2;
   PrintIndent(pos4-1);

   if ( Print(out,ini.T(2),pos1) == FALSE )                  return PROBLEM;
   if ( Print(out,ini.T(3),pos2) == FALSE )                  return PROBLEM;
   if ( Print(out,ini.T(4),pos3) == FALSE )                  return PROBLEM;
   for ( CARDINAL i=0; i<NumCalc(); i++ )
   {
      CARDINAL cn = CalcNum(i);
      if ( Print(out,cn,pos1) == FALSE )                     return PROBLEM;
      if ( Print(out,NumStates(cn),pos2) == FALSE )          return PROBLEM;
      if ( Print(out,ini.T(5),pos3) == FALSE )               return PROBLEM;
      if ( Print(out,": ") == FALSE )                        return PROBLEM;
      const STATE* state = GetState(cn,0);
      if ( state == 0 ) FatalError("STATESYS::Write: illegal state");
      if ( Print(out,state->Time()) == FALSE )               return PROBLEM;
      if ( NumStates(cn) > 1 )
      {
         if ( Print(out," - ") == FALSE )                    return PROBLEM;
         state = GetState(cn,NumStates(cn)-1);
         if ( state == 0 ) FatalError("STATESYS::Write: illegal state");
         if ( Print(out,state->Time()) == FALSE )            return PROBLEM;
      }
      if ( NumPar(cn) > 0 )
      {
         if ( Print(out,ini.T(6),pos3) == FALSE )            return PROBLEM;
         if ( Print(out,":") == FALSE )                      return PROBLEM;
         if ( Print(out,Par(cn,0)->Symbol(),pos4) == FALSE ) return PROBLEM;
         for ( CARDINAL j=1; j<NumPar(cn); j++ )
         {
            if ( Print(out,", ") == FALSE )                  return PROBLEM;
            if ( Print(out,Par(cn,j)->Symbol()) == FALSE )   return PROBLEM;
         }
      }
   }

   if ( PrintLn(out,'*') == FALSE )                          return PROBLEM;

   if ( PrintLn(out) == FALSE )                              return PROBLEM;

   return OK;
}

//////////////////////////////////////////////////////////////////////////////

