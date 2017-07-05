////////////////////////////////   view_c.C   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    08.08.91    Peter Reichert
// revisions:   18.03.93    Peter Reichert    Sens. Anal. and Plot added
//              18.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include "all.h"
#include "init.h"
#include "scrio_c.h"
#include "view_c.h"

//////////////////////////////////////////////////////////////////////////////

// global buffers and response characters (aquasim.C)

extern const CARDINAL  BufLen;
extern char            Buf[];
extern char            Buf1[];
extern char            Buf2[];

extern char            AQ_SaveFile[];
extern char            AQ_PlotFile[];
extern char            AQ_ListFile[];

extern char            C1, C2, Y1, Y2, N1, N2;

//////////////////////////////////////////////////////////////////////////////

CURVE* NewCurve(AQUASYS& system, const CURVE* oldcurve)
{
   CARDINAL i; REAL r;
   CURVE* curve = new CURVE(oldcurve);

   i = curve->Type()+1;
   Write("\n   "); Write(scr.T(703));          // SelectCurveType
   Write("\n   1   =   "); Write(scr.T(704));  // Value
   Write("\n   2   =   "); Write(scr.T(705));  // ErrorContrib
   Write("\n   3   =   "); Write(scr.T(706));  // SensAbsAbs
   Write("\n   4   =   "); Write(scr.T(707));  // SensRelAbs
   Write("\n   5   =   "); Write(scr.T(811));  // SensAbsRel
   Write("\n   6   =   "); Write(scr.T(812));  // SensRelRel
   Write("\n  ");
   Read(i,i); i--; if ( i >= 6 ) i = 0;
   curve->Type((CURVETYPE)i);

   Write("\n   "); Write(scr.T(698));    // EnterVar
   if ( curve->Variable() != 0 )
   {
      Read(Buf1,BufLen,(curve->Variable())->Symbol());
   }
   else
   {
      Read(Buf1,BufLen,"");
   }
   curve->Variable(system.GetVar(Buf1));

   if ( curve->Type() != CurveVal )
   {
      Write("\n   "); Write(scr.T(713));    // EnterPar
      if ( curve->Parameter() != 0 )
      {
         Read(Buf1,BufLen,(curve->Parameter())->Symbol());
      }
      else
      {
         Read(Buf1,BufLen,"");
      }
      curve->Parameter(system.GetVar(Buf1));
   }

   Write("\n   "); Write(scr.T(699));    // EnterCalcNum
   Read(i,curve->CalcNum());
   curve->CalcNum(i);

   if ( (system.Complist())->Size() <= 1 )
   {
      curve->Compartment((system.Complist())->First());
   }
   else
   {
      Write("\n   "); Write(scr.T(700)); // EnterComp
      if ( curve->Compartment() != 0 )
      {
         Read(Buf1,BufLen,(curve->Compartment())->Symbol());
      }
      else
      {
         Read(Buf1,BufLen,"");
      }
      curve->Compartment(system.GetComp(Buf1));
   }
   if ( curve->Compartment() == 0 )
   {
      Write("\n   "); Write(scr.T(191)); Write(" ");
      Write(scr.T(192)); Write ("\n");   // CompNotFound
      delete curve; return 0;
   }
   if ( curve->Zone() >= (curve->Compartment())->NumZones() )
      curve->Zone(0);
   if ( (curve->Compartment())->NumZones() > 1 )
   {
      AQCOMP* com = curve->Compartment();
      Write("\n   "); Write(scr.T(846));         // SelectZone
      for ( i=0; i<com->NumZones(); i++ )
      {
         NumToString(i+1,Buf1,BufLen,4);
         Write("\n"); Write(Buf1); Write("   =   "); 
         Write(com->ZoneName(i));                // ZoneName
      }
      Write("\n  "); Read(i,curve->Zone()+1); i--;
      if ( i >= com->NumZones() ) i = com->NumZones()-1;
      curve->Zone(i);
   }

   Write("\n   "); Write(scr.T(701));            // EnterSpaceTime
   Read(r,curve->XorT());
   curve->XorT(r);

   if ( curve->SpaceRel() == FALSE ) Buf2[0] = '1';
   else                              Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(1404));                // SelSpace
   Write("\n   1   =   "); Write(scr.T(1405));        // Abs
   Write("\n   2   =   "); Write(scr.T(1406));        // Rel
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) curve->SpaceRel(FALSE);
   else                  curve->SpaceRel(TRUE);

   Write("\n   "); Write(scr.T(718));            // EnterLegend
   Read(Buf1,BufLen,curve->Legend());
   curve->Legend(Buf1);

   if ( curve->LineActive() == TRUE ) Buf2[0] = '1';
   else                               Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(727));            // LineStatus
   Write("\n   1   =   "); Write(scr.T(729));    // Active
   Write("\n   2   =   "); Write(scr.T(730));    // Inactive
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) curve->LineActive(TRUE);
   else                  curve->LineActive(FALSE);

   if ( curve->LineActive() == TRUE )
   {
      i = curve->LineStyle() + 1;
      Write("\n   "); Write(scr.T(736));        // LineStyle
      Write("\n   1   =   "); Write(scr.T(737));   // solid
      Write("\n   2   =   "); Write(scr.T(805));   // l dashed
      Write("\n   3   =   "); Write(scr.T(738));   // dashed
      Write("\n   4   =   "); Write(scr.T(806));   // s dashed
      Write("\n   5   =   "); Write(scr.T(739));   // dotted
      Write("\n   6   =   "); Write(scr.T(807));   // l dash dot
      Write("\n   7   =   "); Write(scr.T(808));   // s dash dot
      Write("\n  ");
      Read(i,i); i--; if ( i >= 7 ) i = 0;
      curve->LineStyle((LINESTYLE)i);

      Write("\n   "); Write(scr.T(740));        // LineWidth
      Read(i,curve->LineWidth());
      curve->LineWidth(i);

      i = curve->LineColor() + 1;
      Write("\n   "); Write(scr.T(741));        // LineColor
      Write("\n   1   =   "); Write(scr.T(742));   // black
      Write("\n   2   =   "); Write(scr.T(743));   // red
      Write("\n   3   =   "); Write(scr.T(744));   // green
      Write("\n   4   =   "); Write(scr.T(745));   // blue
      Write("\n   5   =   "); Write(scr.T(746));   // cyan
      Write("\n   6   =   "); Write(scr.T(747));   // magenta
      Write("\n   7   =   "); Write(scr.T(748));   // yellow
      Write("\n   8   =   "); Write(scr.T(749));   // white
      Write("\n  ");
      Read(i,i); i--; if ( i >= 8 ) i = 0;
      curve->LineColor((AQUACOLOR)i);
   }

   if ( curve->MarkActive() == TRUE ) Buf2[0] = '1';
   else                               Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(728));           // MarkerStatus
   Write("\n   1   =   "); Write(scr.T(729));   // Active
   Write("\n   2   =   "); Write(scr.T(730));   // Inactive
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) curve->MarkActive(TRUE);
   else                  curve->MarkActive(FALSE);

   if ( curve->MarkActive() == TRUE )
   {
      i = curve->MarkStyle() + 1;
      Write("\n   "); Write(scr.T(750));        // MarkStyle
      Write("\n   1   =   "); Write(scr.T(751));   // circle
      Write("\n   2   =   "); Write(scr.T(752));   // square
      Write("\n   3   =   "); Write(scr.T(753));   // triangle
      Write("\n   4   =   "); Write(scr.T(754));   // diamond
      Write("\n  ");
      Read(i,i); i--; if ( i >= 4 ) i = 0;
      curve->MarkStyle((MARKSTYLE)i);

      Write("\n   "); Write(scr.T(755));        // MarkSize
      Read(i,curve->MarkSize());
      curve->MarkSize(i);

      i = curve->MarkColor() + 1;
      Write("\n   "); Write(scr.T(756));        // MarkColor
      Write("\n   1   =   "); Write(scr.T(742));   // black
      Write("\n   2   =   "); Write(scr.T(743));   // red
      Write("\n   3   =   "); Write(scr.T(744));   // green
      Write("\n   4   =   "); Write(scr.T(745));   // blue
      Write("\n   5   =   "); Write(scr.T(746));   // cyan
      Write("\n   6   =   "); Write(scr.T(747));   // magenta
      Write("\n   7   =   "); Write(scr.T(748));   // yellow
      Write("\n   8   =   "); Write(scr.T(749));   // white
      Write("\n  ");
      Read(i,i); i--; if ( i >= 8 ) i = 0;
      curve->MarkColor((AQUACOLOR)i);
   }

   return curve;
}


static void EditCurves(AQUASYS& system, PLOT* plot)
{
   while (1)
   {
      Write("\n   "); Write(scr.T(684)); Write(":\n");  // Curves
      if ( plot->NumCurves() > 0 )
      {
         Write("\n   "); Write(scr.T(685));             // Index
         Write("   "); Write(scr.T(690));               // Type
         Write("  :  "); Write(scr.T(686));             // Variable
         Write(" [");  Write(scr.T(687));               // CalcNum
         Write(", "); Write(scr.T(688));                // Compartment
         Write(", "); Write(scr.T(845));                // Zone
         Write(", "); Write(scr.T(689));                // Space/Time
         
         for ( CARDINAL i=0; i<plot->NumCurves(); i++ )
         {
            const CURVE* curve = plot->Curve(i);
            Write("\n");
            NumToString(i+1,Buf1,BufLen,6); Write(Buf1);
            Write(' ',strlen(scr.T(685)));
            switch ( curve->Type() )
            {
               case CurveVal:
                  Write(scr.T(691));                    // Value
                  break;
               case CurveSensErr:
                  Write(scr.T(692));                    // ErrorContrib
                  break;
               case CurveSensAbsAbs:
                  Write(scr.T(693));                    // SensAbsAbs
                  break;
               case CurveSensRelAbs:
                  Write(scr.T(694));                    // SensRelAbs
                  break;
               case CurveSensAbsRel:
                  Write(scr.T(809));                    // SensAbsRel
                  break;
               case CurveSensRelRel:
                  Write(scr.T(810));                    // SensRelRel
                  break;
               default:
                  FatalError("Illegal curve type");
            }
            Write("  :  ");
            AQVAR* var = curve->Variable(); Write(var->Symbol());
            if ( curve->Type() != CurveVal )
            {
               Write("(");
               if ( curve->Parameter() != 0 )
               {
                  Write((curve->Parameter())->Symbol());
               }
               Write(")");
            }
            Write(" [");
            CARDINAL cn = curve->CalcNum(); Write(cn);
            Write(", ");
            AQCOMP* com = curve->Compartment(); Write(com->Symbol());
            Write(", ");
            Write(com->ZoneName(curve->Zone()));
            Write(", ");
            Write(curve->XorT());
            if ( curve->SpaceRel() == TRUE )
            {
               Write(" rel.space");
            }
            Write("]");
         }
      }
      Write("\n");
      Write("\n   "); Write(scr.T(695)); Write(":");  // Select
      Write("\n   1   =   "); Write(scr.T(696));      // NewCurve
      Write("\n   2   =   "); Write(scr.T(731));      // EditCurve
      Write("\n   3   =   "); Write(scr.T(697));      // DeleteCurve
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf1,BufLen);
      if ( (Buf1[0]==C1) || (Buf1[0]==C2) || (Buf1[0]=='\0') ) break;
      CURVE* curve = 0;
      CARDINAL index = 1;
      switch (Buf1[0])
      {
         case '1':
            Write("\n   "); Write(scr.T(702));                 // EnterAddIndex
            index = plot->NumCurves()+1; Read(index,index); index--;
            curve = NewCurve(system,0);
            if ( plot->AddCurve(curve,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(708)); Write("\n"); // AddSuccess
            }
            else
            {
               Write("\n   "); Write(scr.T(709)); Write("\n"); // AddFailure
            }
            break;
         case '2':
            Write("\n   "); Write(scr.T(732));    // EnterEditIndex
            Read(index,index); index--;
            if ( index >= plot->NumCurves() )
            {
               Write("\n   "); Write(scr.T(733)); Write("\n"); // IllegalIndex
               break;
            }
            curve = NewCurve(system,plot->Curve(index));
            if ( plot->ReplaceCurve(curve,index) == TRUE )
            {
               Write("\n   "); Write(scr.T(735)); Write("\n"); // EditSuccess
            }
            else
            {
               Write("\n   "); Write(scr.T(734)); Write("\n"); // EditFailure
            }
            break;
         case '3':
            Write("\n   "); Write(scr.T(710));                 // EnterIndex
            Read(index,index); index--;
            if ( plot->DeleteCurve(index) == TRUE )
            {
               Write("\n   "); Write(scr.T(711)); Write("\n"); // Success
            }
            else
            {
               Write("\n   "); Write(scr.T(712)); Write("\n"); // Failure
            }
            break;
         default:
            break;
      }
   }
   return;
}


static void EditScaling(PLOT* plot)
{
   if ( plot->xMinAuto() == TRUE ) Buf2[0] = '1';
   else                            Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(759));           // xMinStatus
   Write("\n   1   =   "); Write(scr.T(757));   // Auto
   Write("\n   2   =   "); Write(scr.T(758));   // Manual
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) plot->xMinAuto(TRUE);
   else                  plot->xMinAuto(FALSE);
   REAL Min = plot->xMin();
   if ( plot->xMinAuto() == FALSE )
   {
      Write("\n   "); Write(scr.T(760));
      Read(Min,Min);
   }

   if ( plot->xMaxAuto() == TRUE ) Buf2[0] = '1';
   else                            Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(761));           // xMaxStatus
   Write("\n   1   =   "); Write(scr.T(757));   // Auto
   Write("\n   2   =   "); Write(scr.T(758));   // Manual
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) plot->xMaxAuto(TRUE);
   else                  plot->xMaxAuto(FALSE);
   REAL Max = plot->xMax();
   if ( plot->xMaxAuto() == FALSE )
   {
      Write("\n   "); Write(scr.T(762));
      Read(Max,Max);
   }
   plot->xMinMax(Min,Max);

   if ( plot->xTickPos() == TRUE ) Buf2[0] = '1';
   else                            Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(763));           // xTickPosStatus
   Write("\n   1   =   "); Write(scr.T(757));   // Auto
   Write("\n   2   =   "); Write(scr.T(758));   // Manual
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) plot->xTickPosAuto(TRUE);
   else                  plot->xTickPosAuto(FALSE);
   REAL Pos = plot->xTickPos();
   if ( plot->xTickPosAuto() == FALSE )
   {
      Write("\n   "); Write(scr.T(764));
      Read(Pos,Pos);
   }

   if ( plot->xTickDist() == TRUE ) Buf2[0] = '1';
   else                             Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(765));           // xTickDistStatus
   Write("\n   1   =   "); Write(scr.T(757));   // Auto
   Write("\n   2   =   "); Write(scr.T(758));   // Manual
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) plot->xTickDistAuto(TRUE);
   else                  plot->xTickDistAuto(FALSE);
   REAL Dist = plot->xTickDist();
   if ( plot->xTickDistAuto() == FALSE )
   {
      Write("\n   "); Write(scr.T(766));
      Read(Dist,Dist);
   }
   plot->xTick(Pos,Dist);

   if ( plot->yMinAuto() == TRUE ) Buf2[0] = '1';
   else                            Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(767));           // yMinStatus
   Write("\n   1   =   "); Write(scr.T(757));   // Auto
   Write("\n   2   =   "); Write(scr.T(758));   // Manual
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) plot->yMinAuto(TRUE);
   else                  plot->yMinAuto(FALSE);
   Min = plot->yMin();
   if ( plot->yMinAuto() == FALSE )
   {
      Write("\n   "); Write(scr.T(768));
      Read(Min,Min);
   }

   if ( plot->yMaxAuto() == TRUE ) Buf2[0] = '1';
   else                            Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(769));           // yMaxStatus
   Write("\n   1   =   "); Write(scr.T(757));   // Auto
   Write("\n   2   =   "); Write(scr.T(758));   // Manual
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) plot->yMaxAuto(TRUE);
   else                  plot->yMaxAuto(FALSE);
   Max = plot->yMax();
   if ( plot->yMaxAuto() == FALSE )
   {
      Write("\n   "); Write(scr.T(770));
      Read(Max,Max);
   }
   plot->yMinMax(Min,Max);

   if ( plot->yTickPos() == TRUE ) Buf2[0] = '1';
   else                            Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(771));           // yTickPosStatus
   Write("\n   1   =   "); Write(scr.T(757));   // Auto
   Write("\n   2   =   "); Write(scr.T(758));   // Manual
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) plot->yTickPosAuto(TRUE);
   else                  plot->yTickPosAuto(FALSE);
   Pos = plot->yTickPos();
   if ( plot->yTickPosAuto() == FALSE )
   {
      Write("\n   "); Write(scr.T(772));
      Read(Pos,Pos);
   }

   if ( plot->yTickDist() == TRUE ) Buf2[0] = '1';
   else                             Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(773));           // yTickDistStatus
   Write("\n   1   =   "); Write(scr.T(757));   // Auto
   Write("\n   2   =   "); Write(scr.T(758));   // Manual
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) plot->yTickDistAuto(TRUE);
   else                  plot->yTickDistAuto(FALSE);
   Dist = plot->yTickDist();
   if ( plot->yTickDistAuto() == FALSE )
   {
      Write("\n   "); Write(scr.T(774));
      Read(Dist,Dist);
   }
   plot->yTick(Pos,Dist);
   return;
}


static PLOT* NewPlot(AQUASYS& system, PLOT* oldplot)
{
   PLOT* plot = new PLOT(oldplot);

   while (1)
   {
      Write("\n   "); Write(scr.T(44));               // EnterSymbol
      Read(Buf1,BufLen,plot->Symbol());
      if ( plot->Symbol(Buf1)==TRUE ) break;
      Write("\n   "); Write(scr.T(45)); Write("\n");  // IllegalSymbol
   }

   Write("\n   "); Write(scr.T(46));                  // EnterDescription
   Read(Buf1,BufLen,plot->Description()); plot->Description(Buf1);

   Write("\n   "); Write(scr.T(25));                  // EnterTitle
   Read(Buf1,BufLen,plot->Title()); plot->Title(Buf1);

   switch ( plot->AbscType() )
   {
      case AbscTime:  Buf2[0] = '1'; break;
      case AbscSpace: Buf2[0] = '2'; break;
   }
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(281));          // SelectAbscType
   Write("\n   1   =   "); Write(scr.T(282));  // Time
   Write("\n   2   =   "); Write(scr.T(683));  // Space
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0]=='1' )
      plot->AbscType(AbscTime);
   if ( Buf1[0]=='2' )
      plot->AbscType(AbscSpace);

   Write("\n   "); Write(scr.T(26));                  // xTitle
   Read(Buf1,BufLen,plot->xTitle()); plot->xTitle(Buf1);

   Write("\n   "); Write(scr.T(27));                  // yTitle
   Read(Buf1,BufLen,plot->yTitle()); plot->yTitle(Buf1);

   while (1)
   {
      Write("\n   "); Write(scr.T(842)); Write(":");  // Select
      Write("\n   1   =   "); Write(scr.T(843));   // EditCurves
      Write("\n   2   =   "); Write(scr.T(844));   // EditScaling
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf,BufLen);
      if ( (Buf[0]==C1) || (Buf[0]==C2) || (Buf[0]=='\0') ) break;
      switch (Buf[0])
      {
         case '1':
            EditCurves(system,plot);
            break;
         case '2':
            EditScaling(plot);
            break;
         default:
            break;
      }
   }
   return plot;
}


static void AddPlot(AQUASYS& system)
{
   PLOT* plot = NewPlot(system,0);
   if ( system.AddPlot(plot) == TRUE )
   {
      Write("\n   "); Write(scr.T(267)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(268)); Write("\n");  // Failure
      delete plot;
   }
}


static void CopyPlot(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(269));   // EnterSymbol
   Read(Buf1,BufLen,"");
   PLOT* oldplot = system.GetPlot(Buf1);
   if ( oldplot == 0 )
   {
      Write("\n   "); Write(scr.T(270)); Write(" "); Write(Buf1);
      Write(" "); Write(scr.T(271)); Write("\n");  // PlotNotFound
      return;
   }
   PLOT* plot = NewPlot(system,oldplot);
   if ( system.AddPlot(plot) == TRUE )
   {
      Write("\n   "); Write(scr.T(272)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(273)); Write("\n");  // Failure
      delete plot;
   }
}


static void EditPlot(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(274));   // EnterSymbol
   Read(Buf1,BufLen,"");
   PLOT* oldplot = system.GetPlot(Buf1);
   if ( oldplot == 0 )
   {
      Write("\n   "); Write(scr.T(270)); Write(" "); Write(Buf1);
      Write(" "); Write(scr.T(271)); Write("\n");  // PlotNotFound
      return;
   }
   PLOT* plot = NewPlot(system,oldplot);
   if ( system.ReplacePlot(oldplot,plot) == TRUE )
   {
      Write("\n   "); Write(scr.T(275)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(276)); Write("\n");  // Failure
      delete plot;
   }
}


static void DeletePlot(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(277));         // EnterSymbol
   Read(Buf1,BufLen,"");
   if ( system.DeletePlot(Buf1) == TRUE )
   {
      Write("\n   "); Write(scr.T(278)); Write("\n");  // Success
   }
   else
   {
      Write("\n   "); Write(scr.T(279)); Write("\n");  // Failure
   }
}


static void PrintOpt(AQUASYS& system)
{
   PLOTFILOPT* opt = new PLOTFILOPT(system.PlotFilOpt());
   opt->NewPage(TRUE);
   CARDINAL i;

   switch ( opt->PaperSize() )
   {
      case SizeA4:     Buf2[0] = '1'; break;
      case SizeLetter: Buf2[0] = '2'; break;
      case SizeLegal:  Buf2[0] = '3'; break;
   }
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(775));          // SelectSize
   for ( i=0; i<opt->NumPaperSizes(); i++ )
   {
      Write("\n   "); Write(i+1); Write("   =   ");
      Write(opt->PaperSizeName(i));
   }
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   PAPERSIZE size;
   switch ( Buf1[0] )
   {
      case '1': size = SizeA4;     break;
      case '2': size = SizeLetter; break;
      case '3': size = SizeLegal;  break;
   }

   if ( opt->Portrait() == TRUE ) Buf2[0] = '1';
   else                           Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(779));           // Orientation
   Write("\n   1   =   "); Write(scr.T(780));   // Portrait
   Write("\n   2   =   "); Write(scr.T(781));   // Landscape
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) opt->Portrait(TRUE);
   else                  opt->Portrait(FALSE);

   if ( opt->Cm() == TRUE ) Buf2[0] = '1';
   else                     Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(786));           // Unit
   Write("\n   1   =   "); Write(scr.T(787));   // cm
   Write("\n   2   =   "); Write(scr.T(788));   // inch
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) opt->Cm(TRUE);
   else                  opt->Cm(FALSE);

   REAL fac = 1.0; if ( opt->Cm() == FALSE ) fac = opt->Inch();
   REAL left   = opt->BorderLeft() / fac;
   REAL right  = opt->BorderRight() / fac;
   REAL top    = opt->BorderTop() / fac;
   REAL bottom = opt->BorderBottom() / fac;
   Write("\n   "); Write(scr.T(782));          // EnterBorderLeft
   Read(left,left);
   Write("\n   "); Write(scr.T(783));          // EnterBorderLeft
   Read(right,right);
   Write("\n   "); Write(scr.T(784));          // EnterBorderLeft
   Read(top,top);
   Write("\n   "); Write(scr.T(785));          // EnterBorderLeft
   Read(bottom,bottom);

   REAL leg = opt->WidthLegend() / fac;
   Write("\n   "); Write(scr.T(789));          // EnterWidthLegend
   Read(leg,leg);

   CARDINAL cols = opt->NumCol();
   Write("\n   "); Write(scr.T(790));          // EnterNumCol
   Read(cols,cols);

   CARDINAL rows = opt->NumRow();
   Write("\n   "); Write(scr.T(791));          // EnterNumRow
   Read(rows,rows);

   if ( opt->Measures(size,cols,rows,fac*left,fac*right,
                      fac*top,fac*bottom,fac*leg) == FALSE )
   {
      delete opt;
      Write("\n   "); Write(scr.T(185)); Write("\n");
   }

   CARDINAL ftit = opt->FontTitle();
   Write("\n   "); Write(scr.T(792));          // EnterFontTitle
   Read(ftit,ftit);

   CARDINAL fleg = opt->FontLegend();
   Write("\n   "); Write(scr.T(793));          // EnterFontLegend
   Read(fleg,fleg);

   CARDINAL fax = opt->FontAxes();
   Write("\n   "); Write(scr.T(794));          // EnterFontAxes
   Read(fax,fax);

   CARDINAL fnum = opt->FontNumber();
   Write("\n   "); Write(scr.T(795));          // EnterFontNumber
   Read(fnum,fnum);

   if ( opt->Fonts(ftit,fax,fnum,fleg) == FALSE )
   {
      delete opt;
      Write("\n   "); Write(scr.T(185)); Write("\n");
   }

   if ( opt->Color() == TRUE ) Buf2[0] = '1';
   else                        Buf2[0] = '2';
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(796));           // PrinterType
   Write("\n   1   =   "); Write(scr.T(797));   // color
   Write("\n   2   =   "); Write(scr.T(798));   // b/w
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   if ( Buf1[0] == '1' ) opt->Color(TRUE);
   else                  opt->Color(FALSE);

   i = system.PlotFilOpt(opt);
   if ( i == 0 )
   {
      Write("\n   "); Write(scr.T(184)); Write("\n");
   }
   else
   {
      delete opt;
      switch ( i )
      {
         case 1:
            Write("\n   "); Write(scr.T(186)); Write("\n");
            break;
         case 2:
            Write("\n   "); Write(scr.T(187)); Write("\n");
            break;
         case 3:
            Write("\n   "); Write(scr.T(263)); Write("\n");
            break;
         default:
            Write("\n   "); Write(scr.T(185)); Write("\n");
            break;
      }
   }

   return;
}


static void ListOpt(AQUASYS& system)
{
   PLOTLISOPT* opt = new PLOTLISOPT(system.PlotLisOpt());

   Write("\n   "); Write(scr.T(309));          // EnterPrec
   CARDINAL prec = opt->Precision();
   Read(prec,prec);
   opt->Precision(prec);

   switch ( opt->Separator() )
   {
      case SepTab:   Buf2[0] = '1'; break;
      case SepComma: Buf2[0] = '2'; break;
      case SepSpace: Buf2[0] = '3'; break;
   }
   Buf2[1] = '\0';
   Write("\n   "); Write(scr.T(321));          // SelectSeparator
   Write("\n   1   =   "); Write(scr.T(651));  // Tab
   Write("\n   2   =   "); Write(scr.T(652));  // Comma
   Write("\n   3   =   "); Write(scr.T(653));  // Space
   Write("\n  ");
   Read(Buf1,BufLen,Buf2);
   switch ( Buf1[0] )
   {
      case '1': opt->Separator(SepTab);   break;
      case '2': opt->Separator(SepComma); break;
      case '3': opt->Separator(SepSpace); break;
   }

   if ( system.PlotLisOpt(opt) == TRUE )
   {
      Write("\n   "); Write(scr.T(64)); Write("\n");
   }
   else
   {
      delete opt;
      Write("\n   "); Write(scr.T(65)); Write("\n");
   }

   return;
}


static void PrintRes(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(721));   // EnterSymbol
   Read(Buf1,BufLen,"");
   PLOT* plot = system.GetPlot(Buf1);
   if ( plot == 0 )
   {
      Write("\n   "); Write(scr.T(270)); Write(" "); Write(Buf1);
      Write(" "); Write(scr.T(271)); Write("\n");  // PlotNotFound
      return;
   }
   if ( strlen(AQ_PlotFile)==0 && strlen(AQ_SaveFile)>0 )
   {
      strcpy(AQ_PlotFile,AQ_SaveFile);
      char* p = strchr(AQ_PlotFile,'.');
      if ( p != 0 ) *p = '\0';
      strcat(AQ_PlotFile,FEpts);
   }
   Write("\n   "); Write(scr.T(722));          // EnterFileName
   Read(Buf,BufLen,AQ_PlotFile);
   if ( strcmp(Buf,AQ_PlotFile) != 0 ) system.PlotFilOpt()->NewPage(TRUE);
   strcpy(AQ_PlotFile,Buf);
   std::ifstream oldfile;
   std::ofstream ptsfile;
   if ( ExistFile(Buf) == TRUE ) oldfile.open(Buf,std::ios::in);
   ptsfile.open(ptstmpfile,std::ios::out|std::ios::trunc);
   SetFileType(ptstmpfile,FTpts,FCpts);
   JOBSTATUS status =system.PlotResults(oldfile,ptsfile,plot,system.PlotFilOpt()->Eps());
   oldfile.close();
   ptsfile.close();
   if ( status == OK )
   {
      if ( ExistFile(Buf) == TRUE ) RemoveFile(Buf);
      RenameFile(ptstmpfile,Buf);
      Write("\n   "); Write(scr.T(723)); Write(" \"");   // Success
      Write(AQ_PlotFile); Write("\"\n");
   }
   else
   {
      RemoveFile(ptstmpfile);
      Write("\n   "); Write(scr.T(724)); Write("\n");    // Failure
   }
   return;
}


static void ListRes(AQUASYS& system)
{
   Write("\n   "); Write(scr.T(714));   // EnterSymbol
   Read(Buf1,BufLen,"");
   PLOT* plot = system.GetPlot(Buf1);
   if ( plot == 0 )
   {
      Write("\n   "); Write(scr.T(270)); Write(" "); Write(Buf1);
      Write(" "); Write(scr.T(271)); Write("\n");  // PlotNotFound
      return;
   }
   if ( strlen(AQ_ListFile)==0 && strlen(AQ_SaveFile)>0 )
   {
      strcpy(AQ_ListFile,AQ_SaveFile);
      char* p = strchr(AQ_ListFile,'.');
      if ( p != 0 ) *p = '\0';
      strcat(AQ_ListFile,FElis);
   }
   Write("\n   "); Write(scr.T(715));          // EnterFileName
   Read(Buf,BufLen,AQ_ListFile);
   strcpy(AQ_ListFile,Buf);
   std::ofstream listfile;
   listfile.open(Buf,std::ios::out|std::ios::app);
   SetFileType(Buf,FTlis,FClis);
   if ( system.ListResults(listfile,plot) == OK )
   {
      listfile.close();
      Write("\n   "); Write(scr.T(716)); Write(" \"");   // Success
      Write(AQ_ListFile); Write("\"\n");
   }
   else
   {
      listfile.close();
      Write("\n   "); Write(scr.T(717)); Write("\n");    // Failure
   }
   return;
}

//////////////////////////////////////////////////////////////////////////////

void View(AQUASYS& system)
{
   while (1)
   {
      if ( system.Plotlist()->Size() > 0 )
      {
         Write("\n"); Write(scr.T(322)); Write("\n");
         CARDINAL i = 0;
         PLOT* plot = system.Plotlist()->First();
         while ( plot != 0 )
         {
            i++;
            if ( (i-1) % scr.I(1) == 0 ) Write("\n   ");
            Write(plot->Symbol());
            Write(' ',scr.L(1)-strlen(plot->Symbol()));
            plot = plot->Next();
         }
         Write("\n");
      }
      Write("\n"); Write(scr.T(32)); Write("\n");   // ViewMenu
      Write('-',strlen(scr.T(32)));
      Write("\n");
      Write("\n   1   =   "); Write(scr.T(284));    // New
      Write("\n   2   =   "); Write(scr.T(285));    // Edit
      Write("\n   3   =   "); Write(scr.T(286));    // Dupl
      Write("\n   4   =   "); Write(scr.T(287));    // Del
      Write("\n   5   =   "); Write(scr.T(719));    // PrintOptions
      Write("\n   6   =   "); Write(scr.T(307));    // ListOptions
      Write("\n   7   =   "); Write(scr.T(720));    // PrintRes
      Write("\n   8   =   "); Write(scr.T(280));    // ListRes
      Write("\n");
      Write("\n   "); Write(C1); Write("   =   "); Write(scr.T(7));
      Read(Buf,BufLen);
      if ( (Buf[0]==C1) || (Buf[0]==C2) || (Buf[0]=='\0') ) return;
      switch (Buf[0])
      {
         case '1':
            AddPlot(system);
            break;
         case '2':
            CopyPlot(system);
            break;
         case '3':
            EditPlot(system);
            break;
         case '4':
            DeletePlot(system);
            break;
         case '5':
            PrintOpt(system);
            break;
         case '6':
            ListOpt(system);
            break;
         case '7':
            PrintRes(system);
            break;
         case '8':
            ListRes(system);
            break;
         default:
            break;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////
