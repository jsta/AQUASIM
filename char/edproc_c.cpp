///////////////////////////////   edproc_c.C   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    08.08.91    Peter Reichert
// revisions:   18.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "all.h"
#include "init.h"
#include "scrio_c.h"
#include "edopt_c.h"
#include "edproc_c.h"

//////////////////////////////////////////////////////////////////////////////

// global buffers and response characters (aquasim.C)

extern const CARDINAL  BufLen;
extern char            Buf[];
extern char            Buf1[];
extern char            Buf2[];
extern char            Buf3[];
extern char            Buf4[];

extern char            C1, C2, Y1, Y2, N1, N2;

//////////////////////////////////////////////////////////////////////////////

static std::ifstream infile;
static std::ofstream outfile;

//////////////////////////////////////////////////////////////////////////////

static AQPROC* NewDynProc(AQUASYS& system, AQPROC* oldproc)
{
   DYNPROC* dynproc = new DYNPROC(oldproc);
   ReadSymbol(dynproc);
   CARDINAL n = 0;
   while (1)
   {
      if ( dynproc->RateLaw() == 0 )
      {
         Buf2[0] = '\0';
      }
      else
      {
         if ( (dynproc->RateLaw())->UnParse(Buf2,BufLen-1) != 0 ) 
            Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(161)); Write("\n  "); // EnterRate
      Read(Buf1,BufLen,Buf2);
      CARDINAL numerr;
      if ( dynproc->RateLaw(system.Varlist(),Buf1,Buf2,numerr)
                                                       == TRUE )
      {
         break;
      }
      else
      {
         n++;
         if ( numerr == 0 )
         {
            Write("\n   "); Write(scr.T(159)); Write("\n");
         }
         else
         {
            FORMVAR::GetErrorMarks(Buf2,'^',Buf1);
            Write("\n    "); Write(Buf1);
            CARDINAL len = strlen(Buf1);
            for ( CARDINAL i=0; i<len; i++ )
            {
               if ( Buf2[i] != 0 )
               {
                  FORMVAR::GetErrorMsg(Buf2[i],Buf1);
                  Write("\n    "); Write(' ',i); Write(Buf1);
               }
            }
            Write("\n");
            if ( n>2 )
            {
               delete dynproc; return 0;
            }
         }
      }
   }
   BOOLEAN b = TRUE;
   do
   {
      AQVAR* v; CARDINAL i;
      Write("\n   "); Write(scr.T(162)); Write(" "); // Stoichiometry
      Write(dynproc->Symbol()); Write(":\n");
      if ( dynproc->NumVar() > 0 )
      {
         Write("\n   "); Write(scr.T(685));          // Index
         Write("   "); Write(scr.T(163));            // Variable
         Write("  :  "); Write(scr.T(164));          // StoichCoeff
         for ( i=0; i<dynproc->NumVar(); i++ )
         {
            Write("\n");
            NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
            Write(' ',strlen(scr.T(685)));
            v = dynproc->Variable(i); Write(v->Symbol()); Write("  :  ");
            const FORMVAR* cfv = dynproc->StoichCoeff(i);
            cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(165));          // Select
      Write("\n   1   =   "); Write(scr.T(166));  // AddStoichCoeff
      Write("\n   2   =   "); Write(scr.T(167));  // EditStoichCoeff
      Write("\n   3   =   "); Write(scr.T(168));  // DelStoichCoeff
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL index = 1;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(175));   // EnterAddIndex
            index = dynproc->NumVar()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(169));   // EnterSymbol
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n"); break;               // VarNotFound
            }
            else
            {
               CARDINAL n = 0;
               while (1)
               {
                  Write("\n   "); Write(scr.T(170));
                  Read(Buf1,BufLen,"");
                  CARDINAL numerr;
                  if ( dynproc->AddStoichCoeff(system.Varlist(),v,Buf1,
                                               Buf2,numerr,index) == TRUE )
                  {
                     Write("\n   "); Write(scr.T(171)); Write("\n");
                     break;                      // Success
                  }
                  else
                  {
                     n++;
                     if ( numerr == 0 )
                     {
                        Write("\n   "); Write(scr.T(436)); Write("\n");
                        break;
                     }
                     else
                     {
                        FORMVAR::GetErrorMarks(Buf2,'^',Buf1);
                        Write("\n    "); Write(Buf1);
                        CARDINAL len = strlen(Buf1);
                        for ( CARDINAL i=0; i<len; i++ )
                        {
                           if ( Buf2[i] != 0 )
                           {
                              FORMVAR::GetErrorMsg(Buf2[i],Buf1);
                              Write("\n    "); Write(' ',i);  Write(Buf1);
                           }
                        }
                        Write("\n");
                        if ( n>2 )
                        {
                           Write("\n   "); Write(scr.T(172)); 
                           Write("\n"); break;   // Failure
                        }
                     }
                  }
               }
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(176));   // EnterEditIndex
            Read(index,index); index--;
            if ( index >= dynproc->NumVar() )
            {
                Write("\n   "); Write(scr.T(733)); Write("\n");
                break;                           // IllegalIndex
            }
            Write("\n   "); Write(scr.T(183));   // EnterSymbol
            Read(Buf1,BufLen,(dynproc->Variable(index))->Symbol());
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n"); break;               // VarNotFound
            }
            else
            {
               CARDINAL n = 0;
               while (1)
               {
                  Write("\n   "); Write(scr.T(170));
                  (dynproc->StoichCoeff(index))->UnParse(Buf2,BufLen);
                  Read(Buf1,BufLen,Buf2);
                  CARDINAL numerr;
                  if ( dynproc->ReplaceStoichCoeff(system.Varlist(),v,Buf1,
                                                   Buf2,numerr,index) == TRUE )
                  {
                     Write("\n   "); Write(scr.T(173)); Write("\n");
                     break;                      // Success
                  }
                  else
                  {
                     n++;
                     if ( numerr == 0 )
                     {
                        Write("\n   "); Write(scr.T(436)); Write("\n");
                        break;
                     }
                     else
                     {
                        FORMVAR::GetErrorMarks(Buf2,'^',Buf1);
                        Write("\n    "); Write(Buf1);
                        CARDINAL len = strlen(Buf1);
                        for ( CARDINAL i=0; i<len; i++ )
                        {
                           if ( Buf2[i] != 0 )
                           {
                              FORMVAR::GetErrorMsg(Buf2[i],Buf1);
                              Write("\n    "); Write(' ',i);  Write(Buf1);
                           }
                        }
                        Write("\n");
                        if ( n>2 )
                        {
                           Write("\n   "); Write(scr.T(174)); 
                           Write("\n"); break;   // Failure
                        }
                     }
                  }
               }
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(177)); // EnterDelIndex
            Read(index,index); index--;
            if ( dynproc->DeleteStoichCoeff(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(178)); Write("\n");
            }                                  // Success
            else
            {
               Write("\n   "); Write(scr.T(179)); Write("\n");
            }                                  // Failure
            break;
         default:
            break;
      }
   } while ( b == TRUE );
   return dynproc;
}


static AQPROC* NewEquProc(AQUASYS& system, AQPROC* oldproc)
{
   EQUPROC* equproc = new EQUPROC(oldproc);
   ReadSymbol(equproc);
   CARDINAL n = 0;
   AQVAR* v;
   while (1)
   {
      if ( equproc->Variable() == 0 )
      {
         Buf2[0] = '\0';
      }
      else
      {
         strcpy(Buf2,(equproc->Variable())->Symbol());
      }
      Write("\n   "); Write(scr.T(180));  // EnterEquStateVar
      Read(Buf1,BufLen,Buf2);
      v = system.GetVar(Buf1);
      if ( v == 0 )
      {
         n++;
         Write("\n   "); Write(scr.T(118)); Write(" "); Write(Buf1);
         Write(" "); Write(scr.T(126)); Write("\n"); // VarNotFound
         if ( n > 2 )
         {
            delete equproc; return 0;
         }
      }
      else
      {
         break;
      }
   }
   n = 0;
   while (1)
   {
      if ( equproc->Equation() == 0 )
      {
         Buf2[0] = '\0';
      }
      else
      {
         if ( (equproc->Equation())->UnParse(Buf2,BufLen-1) != 0 ) 
            Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(181)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);              // EnterExpression
      CARDINAL numerr;
      if ( equproc->Equation(v,system.Varlist(),Buf1,Buf2,numerr)
                                                          == TRUE )
      {
         return equproc;
      }
      else
      {
         n++;
         if ( numerr == 0 )
         {
            Write("\n   "); Write(scr.T(437)); Write("\n");
         }
         else
         {
            FORMVAR::GetErrorMarks(Buf2,'^',Buf1);
            Write("\n    "); Write(Buf1);
            CARDINAL len = strlen(Buf1);
            for ( CARDINAL i=0; i<len; i++ )
            {
               if ( Buf2[i] != 0 )
               {
                  FORMVAR::GetErrorMsg(Buf2[i],Buf1);
                  Write("\n    "); Write(' ',i); Write(Buf1);
               }
            }
            Write("\n");
            if ( n>2 )
            {
               delete equproc; return 0;
            }
         }
      }
   }
}


static AQPROC* NewProc(AQUASYS& system, AQPROC* oldproc)
{
   while (1)
   {
      if ( oldproc == 0 )
      {
         Write("\n   "); Write(scr.T(154));           // SelectProcType
         Write("\n   1   =   "); Write(scr.T(155));   // DynProc
         Write("\n   2   =   "); Write(scr.T(156));   // EquProc
         Write("\n");
         Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
         Read(Buf1,BufLen);
      }
      else
      {
         Write("\n   "); Write(scr.T(157)); Write(" ");
         Write(oldproc->Symbol()); Write(" "); Write(scr.T(158)); Write(" ");
         switch ( oldproc->Type() )                   // ProcIsOfType
         {
            case DynProc:
               Buf1[0] = '1'; Write(scr.T(155));      // DynProc
               break;
            case EquProc:
               Buf1[0] = '2'; Write(scr.T(156));      // EquProc
               break;
            default:
               FatalError("aquaedit::NewProc: Unknown proc type");
         }
         Write("\n");
      }
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) return 0;
      switch (Buf1[0])
      {
         case '1':
            return NewDynProc(system,oldproc);
         case '2':
            return NewEquProc(system,oldproc);
         default:
            break;
      }
   }
}


static void AddProc(AQUASYS& system)
{
   AQPROC* proc = NewProc(system,0);
   if ( system.AddProc(proc) == TRUE )
   {
      Write("\n   "); Write(scr.T(139)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(140)); Write("\n");  // Failure
      delete proc;
   }
   return;
}


static void CopyProc(AQUASYS& system)
{
   Buf1[0] = '\0';
   AQPROC* oldproc = system.Proclist()->First();
   if ( system.Proclist()->Size() > 1 )
   {
      Write("\n   "); Write(scr.T(433));   // EnterSymbol
      Read(Buf1,BufLen,"");
      oldproc = system.GetProc(Buf1);
   }
   if ( oldproc == 0 )
   {
      Write("\n   "); Write(scr.T(141)); Write(" "); Write(Buf1);
      Write(" "); Write(scr.T(142)); Write("\n");  // ProcNotFound
      return;
   }
   AQPROC* proc = NewProc(system,oldproc);
   if ( system.AddProc(proc) == TRUE )
   {
      Write("\n   "); Write(scr.T(434)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(435)); Write("\n");  // Failure
      delete proc;
   }
   return;
}


static void EditProc(AQUASYS& system)
{
   Buf1[0] = '\0';
   AQPROC* oldproc = system.Proclist()->First();
   if ( system.Proclist()->Size() > 1 )
   {
      Write("\n   "); Write(scr.T(182));   // EnterSymbol
      Read(Buf1,BufLen,"");
      oldproc = system.GetProc(Buf1);
   }
   if ( oldproc == 0 )
   {
      Write("\n   "); Write(scr.T(141)); Write(" "); Write(Buf1);
      Write(" "); Write(scr.T(142)); Write("\n");  // ProcNotFound
      return;
   }
   AQPROC* proc = NewProc(system,oldproc);
   if ( system.ReplaceProc(oldproc,proc) == TRUE )
   {
      Write("\n   "); Write(scr.T(143)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(144)); Write("\n");  // Failure
      delete proc;
   }
   return;
}


static void DeleteProc(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(145));  // EnterSymbol
   Read(Buf1,BufLen,"");
   if ( system.DeleteProc(Buf1) == TRUE )
   {
      Write("\n   "); Write(scr.T(146)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(147)); Write("\n");  // Failure
   }
   return;
}


void EditProcesses(AQUASYS& system)
{
   while (1)
   {
      if ( system.Proclist()->Size() > 0 )
      {
         Write("\n"); Write(scr.T(134)); Write("\n");
         CARDINAL i = 0;
         AQPROC* proc = system.Proclist()->First();
         while ( proc != 0 )
         {
            i++;
            if ( (i-1) % scr.I(1) == 0 ) Write("\n   ");
            Write(proc->Symbol());
            Write(' ',scr.L(1)-strlen(proc->Symbol()));
            proc = proc->Next();
         }
         Write("\n");
      }
      Write("\n"); Write(scr.T(135)); Write("\n");     // EditProcMenu
      Write('-',strlen(scr.T(135)));
      Write("\n");
      Write("\n   1   =   "); Write(scr.T(136));       // AddProc
      Write("\n   2   =   "); Write(scr.T(432));       // CopyProc
      Write("\n   3   =   "); Write(scr.T(137));       // EditProc
      Write("\n   4   =   "); Write(scr.T(138));       // DeleteProc
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) return;
      switch (Buf1[0])
      {
         case '1':
            AddProc(system);
            break;
         case '2':
            CopyProc(system);
            break;
         case '3':
            EditProc(system);
            break;
         case '4':
            DeleteProc(system);
            break;
         default:
            break;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////
