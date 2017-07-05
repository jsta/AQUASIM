///////////////////////////////   edlink_c.C   ///////////////////////////////
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
#include "edlink_c.h"

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

static BIF* NewBif(AQUASYS& system, BIF* oldbif)
{
   BIF* bif = new BIF(oldbif);
   ReadSymbol(bif);
   AQCOMP* com; CARDINAL conn;
   while ( 1 )
   {
      Write("\n   "); Write(scr.T(631));      // EnterCompOut
      Buf[0] = '\0';
      if ( bif->GetBifComp() != 0 )
      {
         strcpy(Buf,bif->GetBifComp()->Symbol());
      }
      Read(Buf1,BufLen,Buf);
      com = system.GetComp(Buf1);
      if ( com == 0 )
      {
         conn = 0;
      }
      else
      {
         if ( com->NumAdvInConn() <= 1 )
         {
            conn = 0;
         }
            else
         {
            Write("\n   "); Write(scr.T(632)); Write(" ");
            Write(com->Symbol()); Write(":"); // SelConnOut
            CARDINAL i;
            for ( i=0; i<com->NumAdvInConn(); i++ )
            {
               Write("\n   "); Write(i+1); Write("   =   ");
               Write(com->AdvInName(i));
            }
            i = bif->GetBifConn()+1;
            Write("\n  ");
            Read(conn,i); conn--;
         }
      }
      if ( bif->SetBifComp(com,conn) == TRUE ) break;
      Write("\n   "); Write(scr.T(633)); Write("\n"); // FailureConnOut
   }
   CARDINAL numerr;
   CARDINAL n = 0;
   while (1)
   {
      if ( bif->WaterFlow() != 0 )
      {
         if ( (bif->WaterFlow())->UnParse(Buf2,BufLen-1) != 0 )
            Buf2[0] = '\0';
      }
      else
      {
         Buf2[0] = '\0';
      }
      Write("\n   "); Write(scr.T(598)); Write("\n  ");
      Read(Buf1,BufLen,Buf2);               // EnterWaterFlow
      if ( bif->WaterFlow(Buf1,system.Varlist(),Buf2,numerr) == TRUE )
      {
         break;                                  // Success
      }
      else
      {
         n++;
         if ( n > 2 )
         {
            delete bif; return 0;
         }
         if ( numerr == 0 )
         {                                       // IllegalDep
             Write("\n   "); Write(scr.T(599)); Write("\n");
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
         }
      }
   }
   if ( bif->BifType() == WithWater ) Buf2[0] = '1';
   else                               Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(600));             // BifType
   Write("\n   1   =   "); Write(scr.T(601));     // WithWater
   Write("\n   2   =   "); Write(scr.T(602));     // AsGiven
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' )
   {
      bif->BifType(WithWater);
   }
   else
   {
      bif->BifType(AsGiven);
   }
   if ( bif->BifType() == AsGiven )
   {
      while (1)
      {
         AQVAR* v;
         Write("\n   "); Write(scr.T(603)); Write(" ");     // Fluxes
         Write(bif->Symbol()); Write(":\n");
         if ( bif->NumFlux() > 0 )
         {
            Write("\n   "); Write(scr.T(685));              // Index
            Write("   "); Write(scr.T(604));                // Variable
            Write("  :  "); Write(scr.T(605));              // Flux
            for ( CARDINAL i=0; i<bif->NumFlux(); i++ )
            {
               Write("\n");
               NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
               Write(' ',strlen(scr.T(685)));
               v = bif->FluxVar(i); Write(v->Symbol()); Write("  :  ");
               const FORMVAR* cfv = bif->FluxVal(i);
               cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
            }
            Write("\n");
         }
         Write("\n   "); Write(scr.T(606));              // Select
         Write("\n   1   =   "); Write(scr.T(607));      // AddFlux
         Write("\n   2   =   "); Write(scr.T(608));      // EditFlux
         Write("\n   3   =   "); Write(scr.T(609));      // DeleteFlux
         Write("\n");
         Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
         Read(Buf1,BufLen);
         if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
         CARDINAL numerr;
         CARDINAL index = 1;
         switch ( Buf1[0] )
         {
            case '1':
               Write("\n   "); Write(scr.T(232));        // EnterAddIndex
               index = bif->NumFlux()+1; Read(index,index); index--;
               Write("\n   "); Write(scr.T(610));        // EnterVar
               Read(Buf1,BufLen,"");
               v = system.GetVar(Buf1);
               if ( v == 0 )
               {
                  Write("\n   "); Write(scr.T(118)); Write(" ");
                  Write(Buf1); Write(" "); Write(scr.T(126));
                  Write("\n");                           // VarNotFound
                  break;
               }
               Write("\n   "); Write(scr.T(611));        // EnterFlux
               Read(Buf1,BufLen,"0");
               if ( bif->AddFlux(system.Varlist(),v,Buf1,
                                 Buf2,numerr,index) == TRUE )
               {
                  Write("\n   "); Write(scr.T(612)); Write("\n"); // Success
               }
               else
               {
                  Write("\n   "); Write(scr.T(613)); Write("\n"); // Failure
               }
               break;
            case '2':
               Write("\n   "); Write(scr.T(233));        // EnterEditIndex
               Read(index,index); index--;
               if ( index >= bif->NumFlux() )
               {
                   Write("\n   "); Write(scr.T(733)); Write("\n");
                   break;                                // IllegalIndex
               }
               Write("\n   "); Write(scr.T(610));        // EnterVar
               Read(Buf1,BufLen,(bif->FluxVar(index))->Symbol());
               v = system.GetVar(Buf1);
               if ( v == 0 )
               {
                  Write("\n   "); Write(scr.T(118)); Write(" ");
                  Write(Buf1); Write(" "); Write(scr.T(126));
                  Write("\n");                           // VarNotFound
                  break;
               }
               Write("\n   "); Write(scr.T(611));        // EnterFlux
               (bif->FluxVal(index))->UnParse(Buf2,BufLen);
               Read(Buf1,BufLen,Buf2);
               if ( bif->ReplaceFlux(system.Varlist(),v,Buf1,
                                     Buf2,numerr,index) == TRUE )
               {
                  Write("\n   "); Write(scr.T(614)); Write("\n"); // Success
               }
               else
               {
                  Write("\n   "); Write(scr.T(615)); Write("\n"); // Failure
               }
               break;
            case '3':
               Write("\n   "); Write(scr.T(616));              // EnterIndex
               Read(index,index); index--;
               if ( bif->DeleteFlux(index) == TRUE )
               {
                  Write("\n   "); Write(scr.T(617)); Write("\n"); // Success
               }
               else
               {
                  Write("\n   "); Write(scr.T(618)); Write("\n"); // Failure
               }
               break;
            default:
               break;
         }
      }
   }
   return bif;
}


static void AddBif(AQUASYS& system, ADVLINK* advlink)
{
   BIF* bif = NewBif(system,0);
   if ( advlink->AddBif(bif) == TRUE )
   {
      Write("\n   "); Write(scr.T(588)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(589)); Write("\n");  // Failure
      delete bif;
   }
   return;
}


static void EditBif(AQUASYS& system, ADVLINK* advlink)
{
   Write("\n   "); Write(scr.T(590));   // EnterSymbol
   Read(Buf1,BufLen,"");
   BIF* oldbif = advlink->GetBif(Buf1);
   if ( oldbif == 0 )
   {
      Write("\n   "); Write(scr.T(591)); Write(" "); Write(Buf1);
      Write(" "); Write(scr.T(592)); Write("\n");  // BifNotFound
      return;
   }
   BIF* bif = NewBif(system,oldbif);
   if ( advlink->ReplaceBif(oldbif,bif) == TRUE )
   {
      Write("\n   "); Write(scr.T(593)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(594)); Write("\n");  // Failure
      delete bif;
   }
   return;
}


static void DelBif(ADVLINK* advlink)
{
   Write("\n   "); Write(scr.T(595));  // EnterSymbol
   Read(Buf1,BufLen,"");
   if ( advlink->DeleteBif(Buf1) == TRUE )
   {
      Write("\n   "); Write(scr.T(596)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(597)); Write("\n");  // Failure
   }
   return;
}


static AQLINK* NewAdvLink(AQUASYS& system, AQLINK* oldlink)
{
   ADVLINK* advlink = new ADVLINK(oldlink);
   ReadSymbol(advlink);
   ReadLinkInd(advlink);
   AQCOMP* com = 0; CARDINAL conn;
   while ( 1 )
   {
      Write("\n   "); Write(scr.T(625));      // EnterCompIn
      Buf[0] = '\0';
      if ( advlink->GetCompIn() != 0 )
      {
         strcpy(Buf,advlink->GetCompIn()->Symbol());
      }
      Read(Buf1,BufLen,Buf);
      com = system.GetComp(Buf1);
      if ( com == 0 )
      {
         conn = 0;
      }
      else
      {
         if ( com->NumAdvOutConn() <= 1 )
         {
            conn = 0;
         }
         else
         {
            Write("\n   "); Write(scr.T(626)); Write(" ");
            Write(com->Symbol()); Write(":"); // SelConnIn
            CARDINAL i;
            for ( i=0; i<com->NumAdvOutConn(); i++ )
            {
               Write("\n   "); Write(i+1); Write("   =   ");
               Write(com->AdvOutName(i));
            }
            i = advlink->GetConnIn()+1;
            Write("\n  ");
            Read(conn,i); conn--;
         }
      }
      if ( advlink->SetCompIn(com,conn) == TRUE ) break;
      Write("\n   "); Write(scr.T(627)); Write("\n"); // FailureConnIn
   }
   while ( 1 )
   {
      Write("\n   "); Write(scr.T(628));      // EnterCompOut
      Buf[0] = '\0';
      if ( advlink->GetCompOut() != 0 )
      {
         strcpy(Buf,advlink->GetCompOut()->Symbol());
      }
      Read(Buf1,BufLen,Buf);
      com = system.GetComp(Buf1);
      if ( com == 0 )
      {
         conn = 0;
      }
      else
      {
         if ( com->NumAdvInConn() <= 1 )
         {
            conn = 0;
         }
         else
         {
            Write("\n   "); Write(scr.T(629)); Write(" ");
            Write(com->Symbol()); Write(":"); // SelConnOut
            CARDINAL i;
            for ( i=0; i<com->NumAdvInConn(); i++ )
            {
               Write("\n   "); Write(i+1); Write("   =   ");
               Write(com->AdvInName(i));
            }
            i = advlink->GetConnOut()+1;
            Write("\n  ");
            Read(conn,i); conn--;
         }
      }
      if ( advlink->SetCompOut(com,conn) == TRUE ) break;
      Write("\n   "); Write(scr.T(630)); Write("\n"); // FailureConnOut
   }
   while (1)
   {
      if ( advlink->Biflist()->Size() > 0 )
      {
         Write("\n   "); Write(scr.T(583)); Write(":");
         CARDINAL i = 0;
         BIF* bif = advlink->Biflist()->First();
         while ( bif != 0 )
         {
            i++;
            if ( (i-1) % scr.I(1) == 0 ) Write("\n   ");
            Write(bif->Symbol());
            Write(' ',scr.L(1)-strlen(bif->Symbol()));
            bif = bif->Next();
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(584));               // EditBif
      Write("\n   1   =   "); Write(scr.T(585));       // AddBif
      Write("\n   2   =   "); Write(scr.T(586));       // EditBif
      Write("\n   3   =   "); Write(scr.T(587));       // DeleteBif
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      switch (Buf1[0])
      {
         case '1':
            AddBif(system,advlink);
            break;
         case '2':
            EditBif(system,advlink);
            break;
         case '3':
            DelBif(advlink);
            break;
         default:
            break;
      }
   }
   return advlink;
}


static AQLINK* NewDiffLink(AQUASYS& system, AQLINK* oldlink)
{
   DIFFLINK* difflink = new DIFFLINK(oldlink);
   ReadSymbol(difflink);
   ReadLinkInd(difflink);
   AQCOMP* com = 0; CARDINAL conn;
   while ( 1 )
   {
      Write("\n   "); Write(scr.T(619));      // EnterComp1
      Buf[0] = '\0';
      if ( difflink->GetComp1() != 0 )
      {
         strcpy(Buf,difflink->GetComp1()->Symbol());
      }
      Read(Buf1,BufLen,Buf);
      com = system.GetComp(Buf1);
      if ( com == 0 )
      {
         conn = 0;
      }
      else
      {
         if ( com->NumDiffConn() <= 1 )
         {
            conn = 0;
         }
         else
         {
            Write("\n   "); Write(scr.T(620)); Write(" ");
            Write(com->Symbol()); Write(":"); // SelConn1
            CARDINAL i;
            for ( i=0; i<com->NumDiffConn(); i++ )
            {
               Write("\n   "); Write(i+1); Write("   =   ");
               Write(com->DiffName(i));
            }
            i = difflink->GetConn1()+1;
            Write("\n  ");
            Read(conn,i); conn--;
         }
      }
      if ( difflink->SetComp1(com,conn) == TRUE ) break;
      Write("\n   "); Write(scr.T(621)); Write("\n"); // FailureConn1
   }
   while ( 1 )
   {
      Write("\n   "); Write(scr.T(622));      // EnterComp2
      Buf[0] = '\0';
      if ( difflink->GetComp2() != 0 )
      {
         strcpy(Buf,difflink->GetComp2()->Symbol());
      }
      Read(Buf1,BufLen,Buf);
      com = system.GetComp(Buf1);
      if ( com == 0 )
      {
         conn = 0;
      }
      else
      {
         if ( com->NumDiffConn() <= 1 )
         {
            conn = 0;
         }
         else
         {
            Write("\n   "); Write(scr.T(623)); Write(" ");
            Write(com->Symbol()); Write(":"); // SelConn2
            CARDINAL i;
            for ( i=0; i<com->NumDiffConn(); i++ )
            {
               Write("\n   "); Write(i+1); Write("   =   ");
               Write(com->DiffName(i));
            }
            i = difflink->GetConn2()+1;
            Write("\n  ");
            Read(conn,i); conn--;
         }
      }
      if ( difflink->SetComp2(com,conn) == TRUE ) break;
      Write("\n   "); Write(scr.T(624)); Write("\n"); // FailureConn2
   }
   while (1)
   {
      AQVAR* v;
      Write("\n   "); Write(scr.T(564)); Write(" ");     // ExchCoeffs
      Write(difflink->Symbol()); Write(":\n");
      if ( difflink->NumExch() > 0 )
      {
         Write("\n   "); Write(scr.T(685));              // Index
         Write("   "); Write(scr.T(565));                // Variable
         Write("  :  "); Write(scr.T(566))               // ExchCoeff
         ; Write(", "); Write(scr.T(725));               // ConvFact1
         for ( CARDINAL i=0; i<difflink->NumExch(); i++ )
         {
            Write("\n");
            NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
            Write(' ',strlen(scr.T(685)));
            v = difflink->ExchVar(i); Write(v->Symbol()); Write("  :  ");
            const FORMVAR* cfv = difflink->ExchVal(i);
            cfv->UnParse(Buf2,BufLen-1); Write(Buf2); Write(", ");
            cfv = difflink->ExchFac(i);
            cfv->UnParse(Buf2,BufLen-1); Write(Buf2);
         }
         Write("\n");
      }
      Write("\n   "); Write(scr.T(567));              // Select
      Write("\n   1   =   "); Write(scr.T(568));      // AddExch
      Write("\n   2   =   "); Write(scr.T(569));      // EditExch
      Write("\n   3   =   "); Write(scr.T(570));      // DeleteExch
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CARDINAL errorsexch, errorsfact;
      CARDINAL index = 1;
      switch ( Buf1[0] )
      {
         case '1':
            Write("\n   "); Write(scr.T(252));        // EnterAddIndex
            index = difflink->NumExch()+1; Read(index,index); index--;
            Write("\n   "); Write(scr.T(571));        // EnterVar
            Read(Buf1,BufLen,"");
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                           // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(572));         // EnterExch
            Read(Buf1,BufLen,"");
            Write("\n   "); Write(scr.T(726));         // EnterFact
            Read(Buf3,BufLen,"1");
            if ( difflink->AddExchCoeff(system.Varlist(),v,
                                        Buf1,Buf2,errorsexch,
                                        Buf3,Buf4,errorsfact,
                                        index) == TRUE )
            {
               Write("\n   "); Write(scr.T(573)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(574)); Write("\n"); // Failure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(253));        // EnterEditIndex
            Read(index,index); index--;
            Write("\n   "); Write(scr.T(571));        // EnterVar
            Read(Buf1,BufLen,"");
            if ( index >= difflink->NumExch() )
            {
               Write("\n   "); Write(scr.T(733)); Write("\n");
               break;                                 // IllegalIndex
            }
            v = system.GetVar(Buf1);
            if ( v == 0 )
            {
               Write("\n   "); Write(scr.T(118)); Write(" ");
               Write(Buf1); Write(" "); Write(scr.T(126));
               Write("\n");                           // VarNotFound
               break;
            }
            Write("\n   "); Write(scr.T(572));         // EnterExch
            Read(Buf1,BufLen,"");
            Write("\n   "); Write(scr.T(726));         // EnterFact
            Read(Buf3,BufLen,"1");
            if ( difflink->ReplaceExchCoeff(system.Varlist(),v,
                                            Buf1,Buf2,errorsexch,
                                            Buf3,Buf4,errorsfact,
                                            index) == TRUE )
            {
               Write("\n   "); Write(scr.T(575)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(576)); Write("\n"); // Failure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(579));      // EnterIndex
            Read(index,index); index--;
            if ( difflink->DeleteExchCoeff(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(580)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(581)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }
   }
   return difflink;
}


static AQLINK* NewLink(AQUASYS& system, AQLINK* oldlink)
{
   while (1)
   {
      if ( oldlink == 0 )
      {
         Write("\n   "); Write(scr.T(550));           // SelectLinkType
         Write("\n   1   =   "); Write(scr.T(553));   // AdvLink
         Write("\n   2   =   "); Write(scr.T(554));   // DiffLink
         Write("\n");
         Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
         Read(Buf1,BufLen);
      }
      else
      {
         Write("\n   "); Write(scr.T(551)); Write(" ");
         Write(oldlink->Symbol()); Write(" "); Write(scr.T(552)); Write(" ");
         switch ( oldlink->Type() )                   // LinkIsOfType
         {
            case AdvLink:
               Buf1[0] = '1'; Write(scr.T(553));      // AdvLink
               break;
            case DiffLink:
               Buf1[0] = '2'; Write(scr.T(554));      // DiffLink
               break;
            default:
               FatalError("aquaedit::NewLink: Unknown link type");
         }
         Write("\n");
      }
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) return 0;
      switch (Buf1[0])
      {
         case '1':
            return NewAdvLink(system,oldlink);
         case '2':
            return NewDiffLink(system,oldlink);
         default:
            break;
      }
   }
}


static void AddLink(AQUASYS& system)
{
   AQLINK* link = NewLink(system,0);
   if ( system.AddLink(link) == TRUE )
   {
      Write("\n   "); Write(scr.T(548)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(549)); Write("\n");  // Failure
      delete link;
   }
   return;
}


static void CopyLink(AQUASYS& system)
{
   Buf1[0] = '\0';
   AQLINK* oldlink = system.Linklist()->First();
   if ( system.Linklist()->Size() > 1 )
   {
      Write("\n   "); Write(scr.T(545));   // EnterSymbol
      Read(Buf1,BufLen,"");
      oldlink = system.GetLink(Buf1);
   }
   if ( oldlink == 0 )
   {
      Write("\n   "); Write(scr.T(364)); Write(" "); Write(Buf1);
      Write(" "); Write(scr.T(365)); Write("\n");  // LinkNotFound
      return;
   }
   AQLINK* link = NewLink(system,oldlink);
   if ( system.AddLink(link) == TRUE )
   {
      Write("\n   "); Write(scr.T(546)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(547)); Write("\n");  // Failure
      delete link;
   }
   return;
}


static void EditLink(AQUASYS& system)
{
   Buf1[0] = '\0';
   AQLINK* oldlink = system.Linklist()->First();
   if ( system.Linklist()->Size() > 1 )
   {
      Write("\n   "); Write(scr.T(363));   // EnterSymbol
      Read(Buf1,BufLen,"");
      oldlink = system.GetLink(Buf1);
   }
   if ( oldlink == 0 )
   {
      Write("\n   "); Write(scr.T(364)); Write(" "); Write(Buf1);
      Write(" "); Write(scr.T(365)); Write("\n");  // LinkNotFound
      return;
   }
   AQLINK* link = NewLink(system,oldlink);
   if ( system.ReplaceLink(oldlink,link) == TRUE )
   {
      Write("\n   "); Write(scr.T(366)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(367)); Write("\n");  // Failure
      delete link;
   }
   return;
}


static void DeleteLink(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(360));  // EnterSymbol
   Read(Buf1,BufLen,"");
   if ( system.DeleteLink(Buf1) == TRUE )
   {
      Write("\n   "); Write(scr.T(361)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(362)); Write("\n");  // Failure
   }
   return;
}


void EditLinks(AQUASYS& system)
{
   while (1)
   {
      if ( system.Linklist()->Size() > 0 )
      {
         Write("\n"); Write(scr.T(354)); Write("\n");
         CARDINAL i = 0;
         AQLINK* link = system.Linklist()->First();
         while ( link != 0 )
         {
            i++;
            if ( (i-1) % scr.I(1) == 0 ) Write("\n   ");
            Write(link->Symbol());
            Write(' ',scr.L(1)-strlen(link->Symbol()));
            link = link->Next();
         }
         Write("\n");
      }
      Write("\n"); Write(scr.T(355)); Write("\n");     // EditLinkMenu
      Write('-',strlen(scr.T(355)));
      Write("\n");
      Write("\n   1   =   "); Write(scr.T(356));       // AddLink
      Write("\n   2   =   "); Write(scr.T(357));       // CopyLink
      Write("\n   3   =   "); Write(scr.T(358));       // EditLink
      Write("\n   4   =   "); Write(scr.T(359));       // DeleteLink
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) return;
      switch (Buf1[0])
      {
         case '1':
            AddLink(system);
            break;
         case '2':
            CopyLink(system);
            break;
         case '3':
            EditLink(system);
            break;
         case '4':
            DeleteLink(system);
            break;
         default:
            break;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////
