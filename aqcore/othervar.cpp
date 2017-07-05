///////////////////////////////   othervar.C   ///////////////////////////////
//
//              date:       person:           comments:
//
// creation:    24.07.91    Peter Reichert
// revisions:   17.09.91    Peter Reichert    ?VAR::?VAR(const ?VAR&)
//                                            constructor added.
//              29.12.91    Peter Reichert    Interpolation methods added
//              27.10.92    Peter Reichert    redesign of VarArg(CARDINAL)
//              21.03.95    Peter Reichert    ProbeVar added
//              26.07.98    Peter Reichert    relative space added to ProbeVar
//              05.11.98    Peter Reichert    CARDLONG introduced in REALLIST
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////

#include <math.h>

#include "all.h"
#include "init.h"
#include "intpol.h"
#include "othervar.h"

#include "varsys.h"
#include "aqcomp.h"

//////////////////////////////////////////////////////////////////////////////

const CARDINAL BufSize = 1024;

//////////////////////////////////////////////////////////////////////////////

VARSYS*     glob_varsys_ptr = 0;
CARDINAL*   glob_calcnum_ptr = 0;
REAL*       glob_t_ptr = 0;
const REAL* glob_Y = 0;

void SetGlobalState(VARSYS* varsys_ptr, CARDINAL* calcnum_ptr,
                    REAL* t_ptr, const REAL* Y)
{
   glob_varsys_ptr = varsys_ptr;
   glob_calcnum_ptr = calcnum_ptr;
   glob_t_ptr = t_ptr;
   glob_Y = Y;
   return;
}

//////////////////////////////////////////////////////////////////////////////

// Identifiers for STATEVAR::Save and STATEVAR::Load
// (compare this constant with enum STATEVARTYPE from othervar.h)

                                                       //  enum STATEVARTYPE:
const char* const StateVarIdentifier[] = {
                                            "VOL",     //  VolStateVar
                                            "SURF",    //  SurfStateVar
                                            "EQU"      //  EquStateVar
                                         };

const CARDINAL NumStateVar = sizeof(StateVarIdentifier)/sizeof(char*);


// Identifier for PROGVAR::Save and PROGVAR::Load
// (compare this constant with enum PROGVARTYPE from aqvar.h)

                                                       //  enum PROGVARTYPE
const char* const ProgVarIdentifier[]  = {
                                            "CALC",    //  CalcNumber
                                            "T",       //  Time
                                            "COMP",    //  CompIndex
                                            "ZONE",    //  ZoneIndex
                                            "LINK",    //  LinkIndex
                                            "Q",       //  Discharge
                                            "EPSL",    //  WaterFraction

                                            "X",       //  SpaceX
                                            "Z",       //  SpaceZ

                                            "VOL",     //  Volume

                                            "VB",      //  BulkVolume
                                            "LF",      //  FilmDepth
                                            "UF",      //  FilmVelocity
                                            "UL",      //  InterfaceVelocity
                                            "Ude",     //  DetachmentVelocity
                                            "Uat",     //  AttachmentVelocity

                                            "Z0",      //  WaterLevel
                                            "A",       //  CrossSection
                                            "P",       //  Perimeter
                                            "W",       //  Width
                                            "Sf",      //  FrictSlope

                                            "RHO",     //  Density
                                            "DADZ",    //  AreaGradient
                                            "I",       //  LightIntensity
                                            "N2",      //  BruntVaisala
                                            "UH",      //  HorizVelocity
                                            "TKE",     //  TurbKinEnergy
                                            "SHEARP",  //  ShearProd
                                            "BUOP",    //  BuoyancyProd
                                            "EPS",     //  Dissipation
                                            "ES"       //  SeichePotEnergy
                                         };

const CARDINAL NumProgVar  = sizeof(ProgVarIdentifier)/sizeof(char*);


const char* IntPolMethIdentifier[]     = {
                                            "LINEAR",  //  linear
                                            "SPLINE",  //  spline
                                            "SMOOTH"   //  smooth
                                         };

const CARDINAL NumIntPolMeth = sizeof(IntPolMethIdentifier)/sizeof(char*);

//////////////////////////////////////////////////////////////////////////////

void STATEVAR::init()
{
   statevartype = VolStateVar;
   statevar     = 0;
   relaccuracy  = ini.R(1);
   absaccuracy  = ini.R(2);
}


STATEVAR::STATEVAR()
{
   init();
}


STATEVAR::STATEVAR(const STATEVAR& var) : AQVAR(var)
{
   statevartype = var.statevartype;
   statevar     = var.statevar;
   relaccuracy  = var.relaccuracy;
   absaccuracy  = var.absaccuracy;
}


STATEVAR::STATEVAR(const AQVAR* var) : AQVAR(var)
{
   if ( var == 0 )
   {
      init();
   }
   else
   {
      if ( var->Type() == StateVar )
      {
         STATEVAR* v = (STATEVAR*)var;
         statevartype = v->statevartype;
         statevar     = v->statevar;
         relaccuracy  = v->relaccuracy;
         absaccuracy  = v->absaccuracy;
      }
      else
      {
         init();
      }
   }
}


STATEVAR::~STATEVAR()
{
}


const char* STATEVAR::TypeName() const
{
   switch ( statevartype )
   {
      case VolStateVar:  return ini.T(19);
      case SurfStateVar: return ini.T(42);
      case EquStateVar:  return ini.T(20);
   }
   return 0;
}


REAL STATEVAR::Evaluate()
{ 
   if ( statevar == 0 ) return 0.0;
   else                 return *statevar;
}


BOOLEAN STATEVAR::StateVarType(STATEVARTYPE type)
{
   if ( InList() == TRUE ) return FALSE;
   Changed();
   statevartype = type;
   return TRUE;
}


BOOLEAN STATEVAR::RelAccuracy(REAL acc)
{
   Changed();
   if ( acc < 0.0 )                        return FALSE;
   if ( (acc==0.0) && (absaccuracy==0.0) ) return FALSE;
   relaccuracy = acc;
   return TRUE;
}


BOOLEAN STATEVAR::AbsAccuracy(REAL acc)
{
   Changed();
   if ( acc < 0.0 )                        return FALSE;
   if ( (acc==0.0) && (relaccuracy==0.0) ) return FALSE;
   absaccuracy = acc;
   return TRUE;
}


BOOLEAN STATEVAR::Accuracy(REAL relacc, REAL absacc)
{
   if ( (relacc<0)  || (absacc<0)  ) return FALSE;
   if ( (relacc==0) && (absacc==0) ) return FALSE;
   relaccuracy = relacc;
   absaccuracy = absacc;
   return TRUE;
}


JOBSTATUS STATEVAR::Load(std::istream& in)
{
   char Buffer[BufSize];
   if ( LoadStart(in) == FALSE )                           return PROBLEM;
   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )                return PROBLEM;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.0
         {
            JOBSTATUS status = AQVAR::Load(in);
                                       if ( status != OK ) return status;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                           return PROBLEM;
            CARDINAL  type = 0;
            while ( type < NumStateVar )
            {
               if ( strcmp(Buffer,StateVarIdentifier[type]) == 0 ) break;
               type++;
            }
            switch (type)
            {
               case VolStateVar:
                  StateVarType(VolStateVar);
                  break;
               case SurfStateVar:
                  StateVarType(SurfStateVar);
                  break;
               case EquStateVar:
                  StateVarType(EquStateVar);
                  break;
               default:
                  return PROBLEM;
            }
            REAL relacc, absacc;
            if ( LoadNumber(in,&relacc) == FALSE )         return PROBLEM;
            if ( LoadNumber(in,&absacc) == FALSE )         return PROBLEM;
            if ( Accuracy(relacc,absacc) == FALSE )        return PROBLEM;
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                             return PROBLEM;
   return OK;
}


JOBSTATUS STATEVAR::Save(std::ostream& out)
{
   if ( SaveStart(out) == FALSE )                           return PROBLEM;
   INTEGER aquavers = 1;
   if ( SaveNumber(out,aquavers) == FALSE )                 return PROBLEM;
   JOBSTATUS status = AQVAR::Save(out); if ( status != OK ) return status;
   if ( StateVarType() >= NumStateVar )
      FatalError("STATEVAR::Save: Unknown StateVar Type");
   if ( SaveString(out,StateVarIdentifier[StateVarType()]) == FALSE )
                                                            return PROBLEM;
   if ( SaveNumber(out,RelAccuracy()) == FALSE )            return PROBLEM;
   if ( SaveNumber(out,AbsAccuracy()) == FALSE )            return PROBLEM;

   if ( SaveEnd(out) == FALSE )                             return PROBLEM;
   return FILEIO::Save(out);
}


JOBSTATUS STATEVAR::Write(std::ostream& out, BOOLEAN sh)
{
   JOBSTATUS status = AQVAR::Write(out,sh); 
                                         if ( status != OK ) return status;

   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   if ( sh == TRUE )
   {
      PrintIndent(pos1-1);
      if ( Print(out,TypeName(),pos1) == FALSE )             return PROBLEM;
   }
   else
   {
      PrintIndent(pos2-1);
      if ( Print(out,ini.T(21),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      if ( Print(out,RelAccuracy(),pos2) == FALSE )          return PROBLEM;
      if ( Print(out,ini.T(22),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      if ( Print(out,AbsAccuracy(),pos2) == FALSE )          return PROBLEM;
   }

   return OK;
}


void STATEVAR::Test()
{
   *ctest << "\nclass STATEVAR:   statevartype= ";
   switch (statevartype)
   {
      case VolStateVar:  *ctest << "Vol";         break;
      case SurfStateVar: *ctest << "Surf";        break;
      case EquStateVar:  *ctest << "Equilibrium"; break;
      default:           *ctest << "unknown";     break;
   }
   *ctest << "\n                  relaccuracy = " << relaccuracy;
   *ctest << "\n                  absaccuracy = " << absaccuracy;
   REAL* r = (REAL*)statevar;  // operator << does not understand const pointer
   *ctest << "\n                  statevar     = " << r;
   AQVAR::Test();
}

//////////////////////////////////////////////////////////////////////////////

void PROGVAR::init()
{
   progvartype = Time;
   progvar     = 0;
}


PROGVAR::PROGVAR()
{
   init();
}


PROGVAR::PROGVAR(const PROGVAR& var) : AQVAR(var)
{
   progvartype = var.progvartype;
   progvar     = var.progvar;
}  


PROGVAR::PROGVAR(const AQVAR* var) : AQVAR(var)
{
   if ( var == 0 )
   {
      init();
   }
   else
   {
      if ( var->Type() == ProgVar )
      {
         PROGVAR* v = (PROGVAR*)var;
         progvartype = v->progvartype;
         progvar     = v->progvar;
      }
      else
      {
         init();
      }
   }
}


PROGVAR::~PROGVAR()
{
   progvar = 0;
}


const char* PROGVAR::TypeName() const
{
   return ini.T(23);
}


BOOLEAN PROGVAR::ArgProgVarType(PROGVARTYPE type, const AQVAR* besides) const
{
   if ( this != besides )
   {
      if ( ProgVarType() == type ) return TRUE;
   }
   return FALSE;
}


REAL PROGVAR::Evaluate()
{ 
   if ( progvar == 0 )
   {
	   std::ostrstream buf;
      buf << "PROGVAR::Evaluate: no pointer assigned to program variable "
          << Symbol() << '\0';
      char* string = buf.str();
      FatalError(string);
   }
   return *progvar;
}


CARDINAL PROGVAR::NumProgVarTypes()
{
   return NumProgVar;     // see aqvar.h
}


BOOLEAN PROGVAR::ProgVarType(PROGVARTYPE type)
{
   if ( InList() == TRUE )          return FALSE;
   if ( type >= NumProgVarTypes() ) return FALSE;
   Changed();
   progvartype = type;
   return TRUE;
}


const char* PROGVAR::ProgVarTypeName(PROGVARTYPE type)
{
   switch ( type ) 
   {
      case CalcNumber:
         return ini.T(43);
      case Time:
         return ini.T(24);
      case CompIndex:
         return ini.T(82);
      case ZoneIndex:
         return ini.T(69);
      case LinkIndex:
         return ini.T(83);
      case Discharge:
         return ini.T(27);
      case WaterFraction:
         return ini.T(10);
      case SpaceX:
         return ini.T(25);
      case SpaceZ:
         return ini.T(13);
      case Volume:
         return ini.T(45);
      case BulkVolume:
         return ini.T(4);
      case FilmDepth:
         return ini.T(5);
      case FilmVelocity:
         return ini.T(8);
      case InterfaceVelocity:
         return ini.T(9);
      case DetachmentVelocity:
         return ini.T(11);
      case AttachmentVelocity:
         return ini.T(12);
      case WaterLevel:
         return ini.T(26);
      case CrossSection:
         return ini.T(28);
      case Perimeter:
         return ini.T(29);
      case Width:
         return ini.T(30);
      case FrictSlope:
         return ini.T(31);
      case Density:
         return ini.T(75);
      case AreaGradient:
         return ini.T(78);
      case LightIntensity:
         return ini.T(88);
      case BruntVaisala:
         return ini.T(76);
      case HorizVelocity:
         return ini.T(81);
      case TurbKinEnergy:
         return ini.T(79);
      case ShearProd:
         return ini.T(85);
      case BuoyancyProd:
         return ini.T(86);
      case Dissipation:
         return ini.T(77);
      case SeicheEnergy:
         return ini.T(80);
   }
   FatalError("PROGVAR::ProgVarTypeName: illegal type");
   return 0;
}


JOBSTATUS PROGVAR::Load(std::istream& in)
{
   char Buffer[BufSize];
   if ( LoadStart(in) == FALSE )                           return PROBLEM;
   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )                return PROBLEM;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.0
         {
            JOBSTATUS status = AQVAR::Load(in);
                                       if ( status != OK ) return status;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                           return PROBLEM;
            CARDINAL type = 0;
            while ( type < NumProgVar )
            {
               if ( strcmp(Buffer,ProgVarIdentifier[type]) == 0 ) break;
               type++;
            }
            if ( ProgVarType((PROGVARTYPE)type) == FALSE )
               return PROBLEM;
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                             return PROBLEM;
   return OK;
}


JOBSTATUS PROGVAR::Save(std::ostream& out)
{
   if ( SaveStart(out) == FALSE )                           return PROBLEM;
   INTEGER aquavers = 1;
   if ( SaveNumber(out,aquavers) == FALSE )                 return PROBLEM;
   JOBSTATUS status = AQVAR::Save(out); if ( status != OK ) return status;
   if ( ProgVarType() > NumProgVar )
      FatalError("PROGVAR::Save: Unknown ProgVar Type");
   if ( SaveString(out,ProgVarIdentifier[ProgVarType()]) == FALSE )
                                                            return PROBLEM;

   if ( SaveEnd(out) == FALSE )                             return PROBLEM;
   return FILEIO::Save(out);
}


JOBSTATUS PROGVAR::Write(std::ostream& out, BOOLEAN sh)
{
   JOBSTATUS status = AQVAR::Write(out,sh); 
                                         if ( status != OK ) return status;

   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   CARDINAL post = pos1;
   if ( sh == TRUE )
   {
      PrintIndent(pos1-1);
      post = pos1;
   }
   else
   {
      PrintIndent(pos2-1);
	  post = pos2;
      if ( Print(out,ini.T(66),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
   }

   switch ( ProgVarType() )
   {
      case CalcNumber:
         if ( Print(out,ini.T(43),post) == FALSE )           return PROBLEM;
         break;
      case Time:
         if ( Print(out,ini.T(24),post) == FALSE )           return PROBLEM;
         break;
      case CompIndex:
         if ( Print(out,ini.T(82),post) == FALSE )           return PROBLEM;
         break;
      case ZoneIndex:
         if ( Print(out,ini.T(69),post) == FALSE )           return PROBLEM;
         break;
      case LinkIndex:
         if ( Print(out,ini.T(83),post) == FALSE )           return PROBLEM;
         break;
      case Discharge:
         if ( Print(out,ini.T(27),post) == FALSE )           return PROBLEM;
         break;
      case WaterFraction:
         if ( Print(out,ini.T(10),post) == FALSE )           return PROBLEM;
         break;
      case SpaceX:
         if ( Print(out,ini.T(25),post) == FALSE )           return PROBLEM;
         break;
      case SpaceZ:
         if ( Print(out,ini.T(13),post) == FALSE )           return PROBLEM;
         break;
      case Volume:
         if ( Print(out,ini.T(45),post) == FALSE )           return PROBLEM;
         break;
      case BulkVolume:
         if ( Print(out,ini.T(4),post) == FALSE )            return PROBLEM;
         break;
      case FilmDepth:
         if ( Print(out,ini.T(5),post) == FALSE )            return PROBLEM;
         break;
      case FilmVelocity:
         if ( Print(out,ini.T(8),post) == FALSE )            return PROBLEM;
         break;
      case InterfaceVelocity:
         if ( Print(out,ini.T(9),post) == FALSE )            return PROBLEM;
         break;
      case DetachmentVelocity:
         if ( Print(out,ini.T(11),post) == FALSE )           return PROBLEM;
         break;
      case AttachmentVelocity:
         if ( Print(out,ini.T(12),post) == FALSE )           return PROBLEM;
         break;
      case WaterLevel:
         if ( Print(out,ini.T(26),post) == FALSE )           return PROBLEM;
         break;
      case CrossSection:
         if ( Print(out,ini.T(28),post) == FALSE )           return PROBLEM;
         break;
      case Perimeter:
         if ( Print(out,ini.T(29),post) == FALSE )           return PROBLEM;
         break;
      case Width:
         if ( Print(out,ini.T(30),post) == FALSE )           return PROBLEM;
         break;
      case FrictSlope:
         if ( Print(out,ini.T(31),post) == FALSE )           return PROBLEM;
         break;
      case Density:
         if ( Print(out,ini.T(75),post) == FALSE )           return PROBLEM;
         break;
      case AreaGradient:
         if ( Print(out,ini.T(78),post) == FALSE )           return PROBLEM;
         break;
      case LightIntensity:
         if ( Print(out,ini.T(88),post) == FALSE )           return PROBLEM;
         break;
      case BruntVaisala:
         if ( Print(out,ini.T(76),post) == FALSE )           return PROBLEM;
         break;
      case HorizVelocity:
         if ( Print(out,ini.T(81),post) == FALSE )           return PROBLEM;
         break;
      case TurbKinEnergy:
         if ( Print(out,ini.T(79),post) == FALSE )           return PROBLEM;
         break;
      case ShearProd:
         if ( Print(out,ini.T(85),post) == FALSE )           return PROBLEM;
         break;
      case BuoyancyProd:
         if ( Print(out,ini.T(86),post) == FALSE )           return PROBLEM;
         break;
      case Dissipation:
         if ( Print(out,ini.T(77),post) == FALSE )           return PROBLEM;
         break;
      case SeicheEnergy:
         if ( Print(out,ini.T(80),post) == FALSE )           return PROBLEM;
         break;
      default:
         FatalError("PROGVAR::Write: Unknown ProgVar Type");
         break;
   }

   return OK;
}


void PROGVAR::Test()
{
   *ctest << "\nclass PROGVAR:    progvartype = ";
   switch (progvartype)
   {
      case CalcNumber:         *ctest << "CalcNumber";         break;
      case Time:               *ctest << "Time";               break;
      case CompIndex:          *ctest << "CompIndex";          break;
      case ZoneIndex:          *ctest << "ZoneIndex";          break;
      case LinkIndex:          *ctest << "LinkIndex";          break;
      case Discharge:          *ctest << "Discharge";          break;
      case WaterFraction:      *ctest << "WaterFraction";      break;
      case SpaceX:             *ctest << "SpaceX";             break;
      case SpaceZ:             *ctest << "SpaceZ";             break;
      case Volume:             *ctest << "Volume";             break;
      case BulkVolume:         *ctest << "BulkVolume";         break;
      case FilmDepth:          *ctest << "FilmDepth";          break;
      case FilmVelocity:       *ctest << "FilmVelocity";       break;
      case InterfaceVelocity:  *ctest << "InterfaceVelocity";  break;
      case DetachmentVelocity: *ctest << "DetachmentVelocity"; break;
      case AttachmentVelocity: *ctest << "AttachmentVelocity"; break;
      case WaterLevel:         *ctest << "WaterLevel";         break;
      case CrossSection:       *ctest << "CrossSection";       break;
      case Perimeter:          *ctest << "Perimeter";          break;
      case Width:              *ctest << "Width";              break;
      case FrictSlope:         *ctest << "FrictSlope";         break;
      case Density:            *ctest << "Density";            break;
      case AreaGradient:       *ctest << "AreaGradient";       break;
      case LightIntensity:     *ctest << "LightIntensity";     break;
      case BruntVaisala:       *ctest << "BruntVaisala";       break;
      case HorizVelocity:      *ctest << "HorizVelocity";      break;
      case TurbKinEnergy:      *ctest << "TurbKinEnergy";      break;
      case ShearProd:          *ctest << "ShearProd";          break;
      case BuoyancyProd:       *ctest << "BuoyancyProd";       break;
      case Dissipation:        *ctest << "Dissipation";        break;
      case SeicheEnergy:       *ctest << "SeichePotEnergy";    break;
      default:                 *ctest << "unknown";            break;
   }
   REAL* r = (REAL*)progvar;  // operator << does not understand const pointer
   *ctest << "\n                  progvar     = " << r;
   AQVAR::Test();
}

//////////////////////////////////////////////////////////////////////////////

void CONSTVAR::init()
{
   value      = ini.R(3);
   stdev      = ini.R(6); if ( stdev <= 0.0 )    stdev = 1.0;
   minimum    = ini.R(4); if ( minimum > value ) minimum = value;
   maximum    = ini.R(5); if ( maximum < value ) maximum = value;
   sensactive = FALSE;
   fitactive  = FALSE;
   sensvalue  = FALSE;
}


CONSTVAR::CONSTVAR()
{
   init();
}


CONSTVAR::CONSTVAR(const CONSTVAR& var) : AQVAR(var)
{
   init();
   value      = var.value;
   stdev      = var.stdev;
   minimum    = var.minimum;
   maximum    = var.maximum;
   sensactive = var.sensactive;
   fitactive  = var.fitactive;
}


CONSTVAR::CONSTVAR(AQVAR* var) : AQVAR(var)
{
   init();
   if ( var != 0 )
   {
      if ( var->Type() == ConstVar )
      {
         CONSTVAR* v = (CONSTVAR*)var;
         value       = v->value;
         stdev       = v->stdev;
         minimum     = v->minimum;
         maximum     = v->maximum;
         sensactive  = v->sensactive;
         fitactive   = v->fitactive;
      }
      else
      {
         if ( var->NumVarArg() == 0 )
         {
            if ( (var->Type()!=StateVar) && (var->Type()!=ProgVar) )
            {
               value = var->Value();
            }
         }
      }
   }
}


CONSTVAR::~CONSTVAR()
{
}


const char* CONSTVAR::TypeName() const
{
   return ini.T(32);
}


REAL CONSTVAR::Evaluate()
{
   // usual case:

   if ( sensvalue == FALSE ) return value;

   // different value during sensitivity analysis:

   REAL inc = ini.R(10)*stdev;
   if ( inc <= 0.0 )
      FatalError("CONSTVAR::Evaluate: sensitivity increment is zero");
   if ( value+inc <= maximum ) return value+inc;
   if ( value-inc >= minimum ) return value-inc;
   if ( maximum-value > value-minimum ) return maximum;
   if ( maximum == minimum )
      FatalError("CONSTVAR::Evaluate: illegal sensitivity analysis");
   return minimum;
}


BOOLEAN CONSTVAR::SetValue(REAL val)
{
   sensvalue  = FALSE;
   if ( !aqfinite(val) ) return FALSE;
   if ( val < minimum )  return FALSE;
   if ( val > maximum )  return FALSE;
   Changed();
   value = val;
   return TRUE;
}


BOOLEAN CONSTVAR::StDev(REAL s)
{
   sensvalue  = FALSE;
   if ( !aqfinite(s) ) return FALSE;
   if ( s <= 0.0 )     return FALSE;
   Changed();
   stdev = s;
   return TRUE;
}


BOOLEAN CONSTVAR::Minimum(REAL mini)
{
   sensvalue  = FALSE;
   if ( mini > value ) return FALSE;
   Changed();
   minimum = mini;
   if ( minimum == maximum ) fitactive  = FALSE;
   if ( minimum == maximum ) sensactive = FALSE;
   return TRUE;
}


BOOLEAN CONSTVAR::Maximum(REAL maxi)
{
   sensvalue  = FALSE;
   if ( maxi < value ) return FALSE;
   Changed();
   maximum = maxi;
   if ( minimum == maximum ) fitactive  = FALSE;
   if ( minimum == maximum ) sensactive = FALSE;
   return TRUE;
}


BOOLEAN CONSTVAR::SensActive(BOOLEAN act)
{
   sensvalue  = FALSE;
   if ( act == TRUE )
   {
      if ( minimum == maximum ) return FALSE;
      sensactive = TRUE;
   }
   else
   {
      sensactive = act;
   }
   return TRUE;
}


BOOLEAN CONSTVAR::FitActive(BOOLEAN act)
{
   sensvalue  = FALSE;
   if ( act == TRUE )
   {
      if ( minimum == maximum ) return FALSE;
      fitactive = TRUE;
   }
   else
   {
      fitactive = FALSE;
   }
   return TRUE;
}


BOOLEAN CONSTVAR::Set(REAL val, REAL s, REAL mini, REAL maxi,
                      BOOLEAN sensact, BOOLEAN fitact)
{
   sensvalue  = FALSE;
   if ( (val<mini) || (val>maxi) )                        return FALSE;
   if ( s <= 0.0 )                                        return FALSE;
   if ( (mini==maxi) && (fitact==TRUE || sensact==TRUE) ) return FALSE;
   value      = val;
   stdev      = s;
   minimum    = mini;
   maximum    = maxi;
   sensactive = sensact;
   fitactive  = fitact;
   return TRUE;
}


BOOLEAN CONSTVAR::SetSensValue(BOOLEAN b)
{
   sensvalue = FALSE;
   if ( b==TRUE && minimum==maximum )
      FatalError("CONSTVAR::SensValue: illegal variable for sens. anal.");
   if ( sensactive == FALSE ) return FALSE;
   if ( b == TRUE ) sensvalue = TRUE;
   return TRUE;
}


JOBSTATUS CONSTVAR::Load(std::istream& in)
{
   if ( LoadStart(in) == FALSE )                           return PROBLEM;
   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )                return PROBLEM;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.0
         {
            JOBSTATUS status = AQVAR::Load(in);
                                       if ( status != OK ) return status;
            if ( LoadNumber(in,&value) == FALSE )          return PROBLEM;
            if ( LoadNumber(in,&stdev) == FALSE )          return PROBLEM;
            if ( stdev <= 0.0 )                            return PROBLEM;
            if ( LoadNumber(in,&minimum) == FALSE )        return PROBLEM;
            if ( minimum > value ) minimum = value;
            if ( LoadNumber(in,&maximum) == FALSE )        return PROBLEM;
            if ( maximum < value ) maximum = value;
            if ( LoadBoolean(in,&sensactive) == FALSE )    return PROBLEM;
            if ( LoadBoolean(in,&fitactive) == FALSE )     return PROBLEM;
            if ( minimum == maximum ) fitactive = FALSE;
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                             return PROBLEM;
   return OK;
}


JOBSTATUS CONSTVAR::Save(std::ostream& out)
{
   if ( SaveStart(out) == FALSE )                           return PROBLEM;
   INTEGER aquavers = 1;
   if ( SaveNumber(out,aquavers) == FALSE )                 return PROBLEM;
   JOBSTATUS status = AQVAR::Save(out); if ( status != OK ) return status;
   if ( SaveNumber(out,value) == FALSE )                    return PROBLEM;
   if ( SaveNumber(out,StDev()) == FALSE )                  return PROBLEM;
   if ( SaveNumber(out,Minimum()) == FALSE )                return PROBLEM;
   if ( SaveNumber(out,Maximum()) == FALSE )                return PROBLEM;
   if ( SaveBoolean(out,SensActive()) == FALSE )            return PROBLEM;
   if ( SaveBoolean(out,FitActive()) == FALSE )             return PROBLEM;

   if ( SaveEnd(out) == FALSE )                             return PROBLEM;
   return FILEIO::Save(out);
}


JOBSTATUS CONSTVAR::Write(std::ostream& out, BOOLEAN sh)
{
   JOBSTATUS status = AQVAR::Write(out,sh); 
                                         if ( status != OK ) return status;

   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);

   if ( sh == TRUE )
   {
      PrintIndent(pos1-1);
      if ( Print(out,value,pos1) == FALSE )                  return PROBLEM;
   }
   else
   {
      PrintIndent(pos2-1);
      if ( Print(out,ini.T(70),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      if ( Print(out,value,pos2) == FALSE )                  return PROBLEM;
      if ( Print(out,ini.T(51),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      if ( Print(out,StDev(),pos2) == FALSE )                return PROBLEM;
      if ( Print(out,ini.T(33),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      if ( Print(out,Minimum(),pos2) == FALSE )              return PROBLEM;
      if ( Print(out,ini.T(34),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      if ( Print(out,Maximum(),pos2) == FALSE )              return PROBLEM;
      if ( Print(out,ini.T(54),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      if ( SensActive() == TRUE )
      {
         if ( Print(out,ini.T(49),pos2) == FALSE )           return PROBLEM;
      }
      else
      {
         if ( Print(out,ini.T(50),pos2) == FALSE )           return PROBLEM;
      }
      if ( Print(out,ini.T(48),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      if ( FitActive() == TRUE )
      {
         if ( Print(out,ini.T(49),pos2) == FALSE )           return PROBLEM;
      }
      else
      {
         if ( Print(out,ini.T(50),pos2) == FALSE )           return PROBLEM;
      }
   }

   return OK;
}


void CONSTVAR::Test()
{
   *ctest << "\nclass CONSTVAR:   value       = " << value;
   *ctest << "\n                  minimum     = " << minimum;
   *ctest << "\n                  maximum     = " << maximum;
   AQVAR::Test();
}

//////////////////////////////////////////////////////////////////////////////

void REALLISTVAR::init()
{
   listarg     = 0;
   n           = 0;
   last        = 0;
   xa          = 0;
   ya          = 0;

   stdevglobal = TRUE;
   absstdev    = ini.R(6);
   relstdev    = ini.R(7);
   minimum     = ini.R(8);
   maximum     = ini.R(9);
   sya         = 0;
   sensactive  = FALSE;
   sensvalue   = FALSE;

   method      = linear;
   width       = 1;
   yaxx        = 0;
}


REALLISTVAR::REALLISTVAR()
{
#ifdef HEAPTEST
HeapFileMessage("start REALLISTVAR::REALLISTVAR()");
#endif

   init();

#ifdef HEAPTEST
HeapFileMessage("end   REALLISTVAR::REALLISTVAR()");
#endif
}


REALLISTVAR::REALLISTVAR(REALLISTVAR& var) : AQVAR(var)
{
#ifdef HEAPTEST
HeapFileMessage("start REALLISTVAR::REALLISTVAR(REALLISTVAR& var)");
#endif

   sensvalue   = FALSE;
   SetArg(var.GetArg());
   n           = var.n;
   last        = var.last;
   stdevglobal = var.stdevglobal;
   absstdev    = var.absstdev;
   relstdev    = var.relstdev;
   minimum     = var.minimum;
   maximum     = var.maximum;
   sensactive  = var.sensactive;
   method      = var.method;
   width       = var.width;
   xa = 0; ya = 0; yaxx = 0; sya = 0;
   if ( n > 0 )
   {
      CARDLONG i;
      xa = new REAL[n];
      ya = new REAL[n];
      for ( i=0; i<n; i++ )
      {
         xa[i] = (var.xa)[i];
         ya[i] = (var.ya)[i];
      }
      if ( var.yaxx != 0 )
      {
         yaxx = new REAL[n];
         for ( i=0; i<n; i++ ) yaxx[i] = (var.yaxx)[i];
      }
      if ( var.sya != 0 )
      {
         sya = new REAL[n];
         for ( i=0; i<n; i++ ) sya[i] = (var.sya)[i];
      }
   }

#ifdef HEAPTEST
HeapFileMessage("end   REALLISTVAR::REALLISTVAR(REALLISTVAR& var)");
#endif
}


REALLISTVAR::REALLISTVAR(AQVAR* var) : AQVAR(var)
{
#ifdef HEAPTEST
HeapFileMessage("start REALLISTVAR::REALLISTVAR(AQVAR* var)");
#endif

   sensvalue = FALSE;
   if ( var == 0 )
   {
      init();
   }
   else
   {
      if ( var->Type() == RealListVar )
      {
         REALLISTVAR* v = (REALLISTVAR*)var;
         SetArg(v->GetArg());
         n           = v->n;
         last        = v->last;
         stdevglobal = v->stdevglobal;
         absstdev    = v->absstdev;
         relstdev    = v->relstdev;
         minimum     = v->minimum;
         maximum     = v->maximum;
         sensactive  = v->sensactive;
         method      = v->method;
         width       = v->width;
         xa = 0; ya = 0; yaxx = 0; sya = 0;
         if ( n > 0 )
         {
            CARDLONG i;
            xa = new REAL[n];
            ya = new REAL[n];
            for ( i=0; i<n; i++ )
            {
               xa[i] = (v->xa)[i];
               ya[i] = (v->ya)[i];
            }
            if ( v->yaxx != 0 )
            {
               yaxx = new REAL[n];
               for ( i=0; i<n; i++ ) yaxx[i] = (v->yaxx)[i];
            }
            if ( v->sya != 0 )
            {
               sya = new REAL[n];
               for ( i=0; i<n; i++ ) sya[i] = (v->sya)[i];
            }
         }
      }
      else
      {
         init();
         if ( var->NumVarArg() == 0 )
         {
            if ( (var->Type()!=StateVar) && (var->Type()!=ProgVar) )
            {
               REAL val = var->Value();
               Add(0.0,val);
            }
         }
      }
   }

#ifdef HEAPTEST
HeapFileMessage("end   REALLISTVAR::REALLISTVAR(AQVAR* var)");
#endif
}


REALLISTVAR::~REALLISTVAR()
{
#ifdef HEAPTEST
HeapFileMessage("start REALLISTVAR::~REALLISTVAR()");
#endif

   if ( n > 0 )
   {
      delete [] xa; delete [] ya; delete [] yaxx; delete [] sya;
   }

#ifdef HEAPTEST
HeapFileMessage("end   REALLISTVAR::~REALLISTVAR()");
#endif
}


const char* REALLISTVAR::TypeName() const
{
   return ini.T(35);
}


BOOLEAN REALLISTVAR::AllowedVar()
{
   if ( listarg == 0 ) return FALSE;
   if ( n == 0 )       return FALSE;
   return TRUE;
}


CARDINAL REALLISTVAR::NumVarArg() const
{
   if ( listarg == 0 ) return 0;
   return 1;
}


AQVAR* REALLISTVAR::VarArg(CARDINAL index) const
{
   if ( listarg==0 || index>0 )
      FatalError("REALLISTVAR::VarArg: Illegal index");
   return listarg;
}


CARDLONG REALLISTVAR::ArgIndex(REAL argval)
{
	return Index(xa,n,argval,&last);
}


REAL REALLISTVAR::Interpolate(REAL argval)
{
   REAL y;
   switch ( method )
   {
      case linear:
         y = LinInt(xa,ya,n,argval,&last);
         break;
      case spline:
         if ( yaxx == 0 )
         {
            yaxx = new REAL[n];
            SplineInit(xa,ya,n,yaxx);
         }
         y = Spline(xa,ya,yaxx,n,argval,&last);
         break;
      case smooth:
         y = Smooth(xa,ya,n,argval,width,&last);
         if ( y < minimum ) y = minimum;
         if ( y > maximum ) y = maximum;
         break;
      default:
         FatalError("REALLISTVAR::Interpolate: unknown interpolation method");
   }
   if ( sensvalue == FALSE ) return y;
   REAL inc;
   if ( stdevglobal == TRUE )
   {
      inc = ini.R(10)*sqrt(absstdev*absstdev+relstdev*relstdev*y*y);
   }
   else
   {
      inc = ini.R(10)*LinInt(xa,sya,n,argval,&last);
   }
   if ( inc <= 0.0 )
      FatalError("REALLISTVAR::Interpolate: sensitivity increment is zero");
   if ( y+inc <= maximum ) return y+inc;
   if ( y-inc >= minimum ) return y-inc;
   if ( maximum-y > y-minimum ) return maximum;
   if ( maximum == minimum )
      FatalError("REALLISTVAR::Interpolate: illegal sensitivity analysis");
   return minimum;
}


REAL REALLISTVAR::Evaluate()
{
   if ( listarg == 0 )
      FatalError("REALLISTVAR::Evaluate: argument pointer is zero");
   if ( NumPairs() == 0 )
      FatalError("REALLISTVAR::Evaluate: list is empty");
   REAL x = listarg->Value();
   return Interpolate(x);
}


REAL REALLISTVAR::StDev(REAL argval)
{
   if ( NumPairs() == 0 )
      FatalError("REALLISTVAR::StDev: list is empty");
   if ( stdevglobal == TRUE )
   {
      REAL val = Interpolate(argval);
      return sqrt(absstdev*absstdev+val*val*relstdev*relstdev);
   }
   return LinInt(xa,sya,n,argval,&last);
}


BOOLEAN REALLISTVAR::SetArg(AQVAR* var)
{
   sensvalue = FALSE;
   if ( InList() == TRUE ) return FALSE;
   if ( var == 0 )         return FALSE;
   listarg = var;
   Changed();
   return TRUE;
}


BOOLEAN REALLISTVAR::Add(REAL x, REAL y, REAL sy)
{
   sensvalue = FALSE;
   if ( (stdevglobal==FALSE) && ( !(sy>0.0) ) ) return FALSE;
   if ( (y<minimum) || (y>maximum) )            return FALSE;
   if ( absstdev==0.0 && y==0.0 )
   {
      if ( stdevglobal == TRUE ) return FALSE;
      absstdev = sy;
   }
   CARDLONG pos = Index(xa,n,x,&last);
   if ( n == 0 )
   {
      xa = new REAL[1];
      ya = new REAL[1];
      xa[0] = x;
      ya[0] = y;
      if ( sy > 0.0 )
      {
         sya = new REAL[1];
         sya[0] = sy;
      }
   }
   else
   {
      if ( x == xa[pos] ) return FALSE;
      if ( pos>0 || x>xa[0] ) pos++;
      REAL* xanew  = new REAL[n+1];
      REAL* yanew  = new REAL[n+1];
      REAL* syanew = 0;
      if ( sya != 0 ) syanew = new REAL[n+1];
      CARDLONG i;
      for ( i=0; i<pos; i++ )
      {
         xanew[i] = xa[i];
         yanew[i] = ya[i];
         if ( sya != 0 ) syanew[i] = sya[i];
      }
      xanew[pos] = x;
      yanew[pos] = y;
      if ( sya != 0 )
      {
         if ( sy > 0.0 )
         {
            syanew[pos] = sy;
         }
         else
         {
            syanew[pos] = sqrt(absstdev*absstdev
                               +relstdev*relstdev*ya[pos]*ya[pos]);
         }
      }
      for ( i=pos; i<n; i++ )
      {
         xanew[i+1] = xa[i];
         yanew[i+1] = ya[i];
         if ( sya != 0 ) syanew[i+1] = sya[i];
      }
      delete xa;   xa   = xanew;
      delete ya;   ya   = yanew;
      delete sya;  sya  = syanew;
      delete yaxx; yaxx = 0;
   }
   n++;
   last = pos;
   Changed();
   return TRUE;
}


BOOLEAN REALLISTVAR::Replace(CARDLONG j, REAL x, REAL y, REAL sy)
{
   sensvalue = FALSE;
   if ( j >= n )                            return FALSE;
   if ( stdevglobal==FALSE && (!(sy>0.0)) ) return FALSE;
   if ( absstdev==0.0 && y==0.0 )
   {
      if ( stdevglobal == TRUE ) return FALSE;
      absstdev = sy;
   }
   if ( x == xa[j] )
   {
      ya[j] = y;
      if ( (sya!=0) && (sy>0.0) ) sya[j] = sy;
   }
   else
   {      
      CARDLONG i = 0;
      if ( x < xa[j] ) i = 1;
      if ( Add(x,y,sy) == FALSE )             return FALSE;
      if ( Delete(j+i) == FALSE )
         FatalError("REALLISTVAR::Replace: Unable to delete data pair");
   }
   Changed();
   return TRUE;
}


BOOLEAN REALLISTVAR::Delete(CARDLONG j)
{
   sensvalue = FALSE;
   if ( (InList()==TRUE) && (n<=1) ) return FALSE;
   if ( j >= n )                     return FALSE;
   if ( n == 1 ) return Delete();
   CARDLONG i;
   REAL* xanew  = new REAL[n-1];
   REAL* yanew  = new REAL[n-1];
   REAL* syanew = 0;
   if ( sya != 0 ) syanew = new REAL[n-1];
   for ( i=0; i<j; i++ )
   {
      xanew[i] = xa[i];
      yanew[i] = ya[i];
      if ( sya != 0 ) syanew[i] = sya[i];
   }
   for ( i=j+1; i<n; i++ )
   {
      xanew[i-1] = xa[i];
      yanew[i-1] = ya[i];
      if ( sya != 0 ) syanew[i-1] = sya[i];
   }
   delete xa;   xa   = xanew;
   delete ya;   ya   = yanew;
   delete sya;  sya  = syanew;
   delete yaxx; yaxx = 0;
   n--;
   last = j; if ( last >= n ) last = n-1;
   Changed();
   return TRUE;
}


BOOLEAN REALLISTVAR::Delete()
{
   sensvalue = FALSE;
   if ( InList() == TRUE ) return FALSE;
   delete [] xa;   xa = 0;
   delete [] ya;   ya = 0;
   delete [] yaxx; yaxx = 0;
   n = 0;
   last = 0;
   Changed();
   return TRUE;
}


void REALLISTVAR::StDevGlobal(BOOLEAN global)
{
   sensvalue = FALSE;
   if ( global == FALSE )
   {
      if ( (n>0) && (sya==0) )
      {
         sya = new REAL[n];
         for ( CARDLONG i=0; i<n; i++ )
         {
            sya[i] = sqrt(absstdev*absstdev+relstdev*relstdev*ya[i]*ya[i]);
         }
      }
   }
   stdevglobal = global;
}


BOOLEAN REALLISTVAR::DeleteIndividualStDev()
{
   sensvalue = FALSE;
   if ( stdevglobal == FALSE ) return FALSE;
   delete sya; sya = 0;
   return TRUE;
}


BOOLEAN REALLISTVAR::RelStDev(REAL s)
{
   sensvalue = FALSE;
   if ( s < 0.0 )                 return FALSE;
   if ( (s==0) && (absstdev==0) ) return FALSE;
   Changed();
   relstdev = s;
   return TRUE;
}


BOOLEAN REALLISTVAR::AbsStDev(REAL s)
{
   sensvalue = FALSE;
   if ( s < 0.0 )                 return FALSE;
   if ( (s==0) && (relstdev==0) ) return FALSE;
   if ( s == 0.0 )
   {
      for ( CARDLONG i=0; i<n; i++ )
      {
         if ( ya[i] == 0.0 ) return FALSE;
      }
   }
   Changed();
   absstdev = s;
   return TRUE;
}


BOOLEAN REALLISTVAR::SetStDev(REAL srel, REAL sabs)
{
   sensvalue = FALSE;
   if ( srel<0.0 || sabs<0.0 )   return FALSE;
   if ( srel==0.0 && sabs==0.0 ) return FALSE;
   if ( sabs == 0.0 )
   {
      for ( CARDLONG i=0; i<n; i++ )
      {
         if ( ya[i] == 0.0 ) return FALSE;
      }
   }
   relstdev = srel;
   absstdev = sabs;
   Changed();
   return TRUE;
}


REAL REALLISTVAR::GetStDev(CARDLONG i)
{
   sensvalue = FALSE;
   if ( i >= n ) return absstdev;
   if ( stdevglobal == TRUE )
   {
      return sqrt(absstdev*absstdev+relstdev*relstdev*ya[i]*ya[i]);
   }
   else
   {
      return sya[i];
   }
}


BOOLEAN REALLISTVAR::Minimum(REAL mini)
{
   sensvalue = FALSE;
   if ( mini > maximum ) return FALSE;
   for ( CARDLONG i=0; i<n; i++ )
   {
      if ( ya[i] < mini ) return FALSE;
   }
   minimum = mini; 
   Changed();
   return TRUE;
}


BOOLEAN REALLISTVAR::Maximum(REAL maxi)
{
   sensvalue = FALSE;
   if ( maxi < minimum ) return FALSE;
   for ( CARDLONG i=0; i<n; i++ )
   {
      if ( ya[i] > maxi ) return FALSE;
   }
   maximum = maxi; 
   Changed();
   return TRUE;
}


void REALLISTVAR::Method(INTPOLMETH m)
{
   sensvalue = FALSE;
   if ( m != spline )
   {
      delete yaxx; yaxx = 0;
   }
   method = m;
   return;
}


BOOLEAN REALLISTVAR::SmoothWidth(REAL w)
{
   sensvalue = FALSE;
   if ( w <= 0.0 ) return FALSE;
   width = w;
   return TRUE;
}


BOOLEAN REALLISTVAR::SensActive(BOOLEAN act)
{
   sensvalue = FALSE;
   if ( act == TRUE )
   {
      if ( minimum == maximum ) return FALSE;
   }
   sensactive = act;
   return TRUE;
}


void REALLISTVAR::ReplVarArg(AQVAR* oldvar, AQVAR* newvar)
{
   if ( listarg == oldvar ) listarg = newvar;
   Changed();
   return;
}


BOOLEAN REALLISTVAR::SkipFields(std::istream& in, CARDINAL num)
{
   char c;
   for ( CARDINAL i=0; i<num; i++ )
   {
      CARDINAL phase = 1;
      do
      {
         if ( ! in.get(c) ) return FALSE;
         if ( (c=='\t') || (c==',') ) break;
         switch ( phase )
         {
            case 1:                      // leading blanks
               if ( c=='\n' || c=='\r' )
               {
                  in.putback(c); return FALSE;
               }
               if ( c != ' ' ) phase = 2;
               break;
            case 2:                      // field content
               if ( c == ' ' ) phase = 3;
               if ( c=='\n' || c=='\r' )
               {
                  in.putback(c); phase = 4;
               }
               break;
            case 3:                      // terminating blanks
               if ( c != ' ' )
               {
                  in.putback(c); phase = 4;
               }
               break;
         }
      } while ( phase < 4 );
   }
   return TRUE;
}


BOOLEAN REALLISTVAR::ReadField(std::istream& in, REAL* r)
{
   char     Buf[BufSize];
   char     c;
   CARDINAL index = 0;
   CARDINAL phase = 1;
   do
   {
      if ( ! in.get(c) )
      {
         if ( phase == 1 ) return FALSE;
         else              phase = 4;
      }
      switch ( phase )
      {
         case 1:                         // leading blanks
            switch ( c )
            {
               case '\n': case '\r':
                  in.putback(c);
                  return FALSE;
               case '\t': case ',':
                  return FALSE;
               case ' ':
                  break;
               default:
                  phase = 2;
                  Buf[index] = c; index++;
                  if ( index == BufSize-1 ) phase = 4;
                  break;
            }
            break;
         case 2:                         // field content
            switch ( c )
            {
               case '\n': case '\r':
                  in.putback(c);
                  phase = 4;
                  break;
               case '\t': case ',':
                  phase = 4;
                  break;
               case ' ':
                  phase = 3;
                  break;
               default:
                  Buf[index] = c; index++;
                  if ( index == BufSize-1 ) phase = 4;
                  break;
            }
            break;
         case 3:                         // terminating blanks
            switch ( c )
            {
               case '\t': case ',':
                  phase = 4;
                  break;
               case ' ':
                  break;
               default:
                  in.putback(c);
                  phase = 4;
                  break;
            }
            break;
         default:
            break;
      }
   } while ( phase < 4 );
   Buf[index] = '\0';
   if ( StringToNum(Buf,r) == FALSE ) return FALSE;
   return TRUE;
}


BOOLEAN REALLISTVAR::ReadData(std::istream& in, CARDLONG* numread,
                              CARDINAL colx, CARDINAL coly, CARDINAL colsy,
                              CARDLONG firstline, CARDLONG lastline)
{
   *numread = 0;
   if ( (colx==coly) || (colx==colsy) || (coly==colsy) ) return FALSE;
   if ( (colx<1) || (coly<1) )                           return FALSE;
   if ( firstline < 1 ) firstline = 1;
   if ( (lastline!=0) && (lastline<firstline) )          return FALSE;
   char     c;
   CARDLONG i;
   for ( i=1; i<firstline; i++ )
   {
      do
      {
         if ( !in.get(c) )                               return FALSE;
      } while ( c!='\n' && c!='\r' );
      if ( c == '\r' )
      {
         in.get(c);
         if ( c != '\n' ) in.putback(c);
      }
   }
   CARDLONG line = firstline-1;
   REAL     x,y,sy;
   REAL*    x1;
   REAL*    x2;
   REAL*    x3;
   CARDINAL c1,c2,c3;
   if ( coly < colsy )
   {
      if ( colx < coly )
      {
         c1 = colx; c2 = coly; c3 = colsy;
         x1 = &x;   x2 = &y;   x3 = &sy;
      }
      else
      {
         if ( colx < colsy )
         {
            c1 = coly; c2 = colx; c3 = colsy;
            x1 = &y;   x2 = &x;   x3 = &sy;
         }
         else
         {
            c1 = coly; c2 = colsy; c3 = colx;
            x1 = &y;   x2 = &sy;   x3 = &x;
         }
      }
   }
   else
   {
      if ( colx < colsy )
      {
         c1 = colx; c2 = colsy; c3 = coly;
         x1 = &x;   x2 = &sy;   x3 = &y;
      }
      else
      {
         if ( colx < coly )
         {
            c1 = colsy; c2 = colx; c3 = coly;
            x1 = &sy;   x2 = &x;   x3 = &y;
         }
         else
         {
            c1 = colsy; c2 = coly; c3 = colx;
            x1 = &sy;   x2 = &y;   x3 = &x;
         }
      }
   }
   sy = 0.0;
   BOOLEAN readdev = FALSE; if ( c1 > 0 ) readdev = TRUE;
   while ( 1 )
   {
      BOOLEAN cont = TRUE;
      if ( readdev == TRUE )
      {
         if ( SkipFields(in,c1-1) == TRUE )
         {
            if ( ReadField(in,x1) == FALSE ) cont = FALSE;
         }
         else
         {
            cont = FALSE;
         }
      }
      if ( cont == TRUE )
      {
         if ( SkipFields(in,c2-c1-1) == TRUE )
         {
            if ( ReadField(in,x2) == FALSE ) cont = FALSE;
         }
         else
         {
            cont = FALSE;
         }
      }
      if ( cont == TRUE )
      {
         if ( SkipFields(in,c3-c2-1) == TRUE )
         {
            if ( ReadField(in,x3) == FALSE ) cont = FALSE;
         }
         else
         {
            cont = FALSE;
         }
      }
      if ( cont == TRUE )
      {
         if ( Add(x,y,sy) == FALSE ) return FALSE;
         (*numread)++;
      }
      line++;
      if ( line == lastline ) break;
      do
      {
         if ( ! in.get(c) )
         {
            if ( lastline==0 && (*numread)>0 ) return TRUE;
            else                            return FALSE;
         }
      } while ( c!='\n' && c!='\r' );
      if ( c == '\r' )
      {
         in.get(c);
         if ( c != '\n' ) in.putback(c);
      }
   }
   if ( *numread == 0 ) return FALSE;
   return TRUE;
}


BOOLEAN REALLISTVAR::WriteData(std::ostream& out)
{
   out.precision(ini.I(1));
   PrintWidth(0);

   if ( listarg != 0 )
   {
      if ( Print(out,listarg->Symbol()) == FALSE ) return FALSE;
   }
   if ( Print(out,"\t") == FALSE )                 return FALSE;
   if ( Print(out,Symbol()) == FALSE )             return FALSE;
   if ( sya != 0 )
   {
      if ( Print(out,"\t") == FALSE )              return FALSE;
      if ( Print(out,"+/-") == FALSE )             return FALSE;
   }
   if ( Print(out,"\n") == FALSE )                 return FALSE;
   if ( listarg != 0 )
   {
      if ( Print(out,"[") == FALSE )               return FALSE;
      if ( Print(out,listarg->Unit()) == FALSE )   return FALSE;
      if ( Print(out,"]") == FALSE )               return FALSE;
   }
   if ( Print(out,"\t[") == FALSE )                return FALSE;
   if ( Print(out,Unit()) == FALSE )               return FALSE;
   if ( sya != 0 )
   {
      if ( Print(out,"]\t[") == FALSE )            return FALSE;
      if ( Print(out,Unit()) == FALSE )            return FALSE;
   }
   if ( Print(out,"]\n") == FALSE )                return FALSE;
   for ( CARDLONG i=0; i<n; i++ )
   {
      if ( Print(out,"\n") == FALSE )              return FALSE;
      if ( Print(out,xa[i]) == FALSE )             return FALSE;
      if ( Print(out,"\t") == FALSE )              return FALSE;
      if ( Print(out,ya[i]) == FALSE )             return FALSE;
      if ( sya != 0 )
      {
         if ( Print(out,"\t") == FALSE )           return FALSE;
         if ( Print(out,sya[i]) == FALSE )         return FALSE;
      }
   }
   if ( Print(out,"\n") == FALSE )                 return FALSE;
   return TRUE;
}


BOOLEAN REALLISTVAR::SetSensValue(BOOLEAN b)
{
   sensvalue = FALSE;
   if ( b==TRUE && minimum==maximum )
      FatalError("REALLISTVAR::SensValue: illegal variable for sens. anal.");
   if ( sensactive == FALSE ) return FALSE;
   if ( b == TRUE ) sensvalue = TRUE;
   return TRUE;
}


JOBSTATUS REALLISTVAR::Load(std::istream& in, const VARLIST* varlist)
{
   char Buffer[BufSize];

   if ( LoadStart(in) == FALSE )                         return PROBLEM;
   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )              return PROBLEM;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.0
         {
            JOBSTATUS status = AQVAR::Load(in);
                                     if ( status != OK ) return status;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                         return PROBLEM;
            AQVAR* var = varlist->Get(Buffer);
            if ( var == 0 )                              return PROBLEM;
            if ( SetArg(var) == FALSE )                  return PROBLEM;

            if ( LoadBoolean(in,&stdevglobal) == FALSE ) return PROBLEM;

            REAL r;
            if ( LoadNumber(in,&r) == FALSE )            return PROBLEM;
            if ( RelStDev(r) == FALSE )                  return PROBLEM;

            if ( LoadNumber(in,&r) == FALSE )            return PROBLEM;
            if ( AbsStDev(r) == FALSE )                  return PROBLEM;

            if ( LoadNumber(in,&r) == FALSE )            return PROBLEM;
            if ( Minimum(r) == FALSE )                   return PROBLEM;

            if ( LoadNumber(in,&r) == FALSE )            return PROBLEM;
            if ( Maximum(r) == FALSE )                   return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                         return PROBLEM;   
            CARDINAL type = 0;
            while ( type < NumIntPolMeth )
            {
               if ( strcmp(Buffer,IntPolMethIdentifier[type]) == 0 ) break;
               type++;
            }
            switch (type)
            {
               case linear:
                  Method(linear);
                  break;
               case spline:
                  Method(spline);
                  break;
               case smooth:
                  Method(smooth);
                  break;
               default:
                  return PROBLEM;
            }

            if ( LoadNumber(in,&r) == FALSE )            return PROBLEM;
            if ( SmoothWidth(r) == FALSE )               return PROBLEM;

            if ( LoadBoolean(in,&sensactive) == FALSE )  return PROBLEM;

            BOOLEAN b;
            if ( LoadBoolean(in,&b) == FALSE )           return PROBLEM;
            if ( stdevglobal==FALSE && b==FALSE )        return PROBLEM;
            if ( LoadStart(in) == FALSE )                return PROBLEM;

            const CARDINAL dim = 100;
            CARDINAL i, num;
            REAL x[dim], y[dim], sy[dim];
            BOOLEAN end = FALSE;
            while ( end == FALSE )
            {
               num = 0;
               for ( i=0; i<dim; i++ )
               {
                  if ( LoadNumber(in,&x[i]) == TRUE )
                  {
                     if ( LoadNumber(in,&y[i]) == FALSE ) return PROBLEM;
                     if ( b == TRUE )
                     {
                        if ( LoadNumber(in,&sy[i]) == FALSE ) return PROBLEM;
                     }
                     num = i+1;
                  }
                  else
                  {
                     end = TRUE;
                     break;
                  }
               }
               if ( num > 0 )
               {
                  REAL* xanew = new REAL[n+num];
                  REAL* yanew = new REAL[n+num];
                  for ( i=0; i<n; i++ )
                  {
                     xanew[i] = xa[i];
                     yanew[i] = ya[i];
                  }
                  for ( i=0; i<num; i++ )
                  {
                     xanew[n+i] = x[i];
                     yanew[n+i] = y[i];
                  }
                  delete xa; xa = xanew;
                  delete ya; ya = yanew;
                  if ( b == TRUE )
                  {
                     REAL* syanew = new REAL[n+num];
                     for ( i=0; i<n; i++ ) syanew[i] = sya[i];
                     for ( i=0; i<num; i++ ) syanew[n+i] = sy[i];
                     delete sya; sya = syanew;
                  }
                  n = n+num;
               }
            }
            for ( i=1; i<n; i++ )
            {
               if ( ! (xa[i]>xa[i-1]) )                  return PROBLEM;
            }
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                           return PROBLEM;
   return OK;
}


JOBSTATUS REALLISTVAR::Save(std::ostream& out)
{
   if ( SaveStart(out) == FALSE )                           return PROBLEM;

   INTEGER aquavers = 1;
   if ( SaveNumber(out,aquavers) == FALSE )                 return PROBLEM;
   JOBSTATUS status = AQVAR::Save(out); if ( status != OK ) return status;

   if ( listarg == 0 )
      FatalError("REALLISTVAR::Save: no argument");
   if ( SaveString(out,listarg->Symbol()) == FALSE )        return PROBLEM;

   if ( SaveBoolean(out,StDevGlobal()) == FALSE )           return PROBLEM;

   if ( SaveNumber(out,RelStDev()) == FALSE )               return PROBLEM;

   if ( SaveNumber(out,AbsStDev()) == FALSE )               return PROBLEM;

   if ( SaveNumber(out,Minimum()) == FALSE )                return PROBLEM;

   if ( SaveNumber(out,Maximum()) == FALSE )                return PROBLEM;

   if ( Method() > NumIntPolMeth )
      FatalError("REALLISTVAR::Save: Unknown interpolation method");
   if ( SaveString(out,IntPolMethIdentifier[Method()]) == FALSE )
                                                            return PROBLEM;

   if ( SaveNumber(out,SmoothWidth()) == FALSE )            return PROBLEM;

   if ( SaveBoolean(out,SensActive()) == FALSE )            return PROBLEM;

   BOOLEAN b = TRUE;
   if ( sya == 0 ) b = FALSE;
   if ( SaveBoolean(out,b) == FALSE )                       return PROBLEM;
   if ( SaveStart(out) == FALSE )                           return PROBLEM;
   for ( CARDLONG i=0; i<n; i++ )
   {
      if ( SaveNumber(out,xa[i]) == FALSE )                 return PROBLEM;
      if ( SaveNumber(out,ya[i]) == FALSE )                 return PROBLEM;
      if ( b == TRUE )
      {
         if ( SaveNumber(out,sya[i]) == FALSE )             return PROBLEM;
      }
   }
   if ( SaveEnd(out) == FALSE )                             return PROBLEM;

   if ( SaveEnd(out) == FALSE )                             return PROBLEM;
   return FILEIO::Save(out);
}


JOBSTATUS REALLISTVAR::Write(std::ostream& out, BOOLEAN sh)
{
   JOBSTATUS status = AQVAR::Write(out,sh); 
                                         if ( status != OK ) return status;

   INTEGER lines = ini.I(2);
   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   CARDINAL pos3 = pos1+3;
   CARDINAL pos4 = pos3+abs(out.precision())+8;
   CARDINAL pos5 = pos4+abs(out.precision())+8;

   if ( sh == TRUE )
   {
      PrintIndent(pos1-1);
      if ( Print(out,TypeName(),pos1) == FALSE )             return PROBLEM;
      if ( Print(out," (") == FALSE )                        return PROBLEM;
      if ( Print(out,listarg->Symbol()) == FALSE )           return PROBLEM;
      if ( Print(out,")") == FALSE )                         return PROBLEM;
   }
   else
   {
      PrintIndent(pos2-1);
      if ( listarg == 0 )
         FatalError("REALLISTVAR::Write: no argument");
      if ( Print(out,ini.T(36),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      if ( Print(out,listarg->Symbol(),pos2) == FALSE )      return PROBLEM;
      if ( Print(out,ini.T(56),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      if ( StDevGlobal() == TRUE )
      {
         if ( Print(out,ini.T(64),pos2) == FALSE )           return PROBLEM;
      }
      else
      {
         if ( Print(out,ini.T(65),pos2) == FALSE )           return PROBLEM;
      }
      if ( StDevGlobal() == TRUE )
      {
         if ( Print(out,ini.T(62),pos1) == FALSE )           return PROBLEM;
         if ( Print(out,":") == FALSE )                      return PROBLEM;
         if ( Print(out,RelStDev(),pos2) == FALSE )          return PROBLEM;
         if ( Print(out,ini.T(63),pos1) == FALSE )           return PROBLEM;
         if ( Print(out,":") == FALSE )                      return PROBLEM;
         if ( Print(out,AbsStDev(),pos2) == FALSE )          return PROBLEM;
      }
      if ( Print(out,ini.T(46),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      if ( Print(out,Minimum(),pos2) == FALSE )              return PROBLEM;
      if ( Print(out,ini.T(47),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      if ( Print(out,Maximum(),pos2) == FALSE )              return PROBLEM;
      if ( Print(out,ini.T(57),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      switch ( Method() )
      {
         case linear:
            if ( Print(out,ini.T(58),pos2) == FALSE )        return PROBLEM;
            break;
         case spline:
            if ( Print(out,ini.T(59),pos2) == FALSE )        return PROBLEM;
            break;
         case smooth:
            if ( Print(out,ini.T(60),pos2) == FALSE )        return PROBLEM;
            if ( Print(out,ini.T(61),pos1) == FALSE )        return PROBLEM;
            if ( Print(out,":") == FALSE )                   return PROBLEM;
            if ( Print(out,SmoothWidth(),pos2) == FALSE )    return PROBLEM;
            break;
         default:
            return PROBLEM;
      }
      if ( Print(out,ini.T(55),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      if ( SensActive() == TRUE )
      {
         if ( Print(out,ini.T(52),pos2) == FALSE )           return PROBLEM;
      }
      else
      {
         if ( Print(out,ini.T(53),pos2) == FALSE )           return PROBLEM;
      }
      if ( Print(out,ini.T(37),pos1) == FALSE )              return PROBLEM;
      if ( Print(out," (") == FALSE )                        return PROBLEM;
      if ( Print(out,NumPairs()) == FALSE )                  return PROBLEM;
      if ( Print(out," ") == FALSE )                         return PROBLEM;
      if ( Print(out,ini.T(67)) == FALSE )                   return PROBLEM;
      if ( Print(out,")") == FALSE )                         return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      PrintIndent(pos1+6-1);
      CARDLONG num = n;
      CARDINAL ncrit = 2*lines+5;
      BOOLEAN sp = FALSE;
      for ( CARDLONG i=0; i<num; i++ )
      {
         if ( (i<lines) || (num-i<=lines) || (num<ncrit) )
         {
            if ( Print(out,xa[i],pos3) == FALSE )            return PROBLEM;
            if ( Print(out,ya[i],pos4) == FALSE )            return PROBLEM;
            if ( StDevGlobal() == FALSE )
            {
               if ( Print(out,"+/- ",pos5) == FALSE )        return PROBLEM;
               if ( Print(out,sya[i]) == FALSE )             return PROBLEM;
            }
         }
         else
         {
            if ( sp == FALSE )
            {
               if ( Print(out,".",pos3+3) == FALSE )         return PROBLEM;
               if ( Print(out,".",pos4+3) == FALSE )         return PROBLEM;
               if ( Print(out,".",pos3+3) == FALSE )         return PROBLEM;
               if ( Print(out,".",pos4+3) == FALSE )         return PROBLEM;
               if ( PrintLn(out) == FALSE )                  return PROBLEM;
               sp = TRUE;
            }
         }
      }
   }

   return OK;
}


void REALLISTVAR::Test()
{
   *ctest << "\nclass REALLISTVAR:size        = " << n;
   AQVAR::Test();
}

//////////////////////////////////////////////////////////////////////////////

void VARLISTVAR::init()
{
   listarg     = 0;
   n           = 0;
   last        = 0;
   xa          = 0;
   yvara       = 0;

   method      = linear;
   width       = 1;
}


VARLISTVAR::VARLISTVAR()
{
#ifdef HEAPTEST
HeapFileMessage("start VARLISTVAR::VARLISTVAR()");
#endif

   init();

#ifdef HEAPTEST
HeapFileMessage("end   VARLISTVAR::VARLISTVAR()");
#endif
}


VARLISTVAR::VARLISTVAR(VARLISTVAR& var) : AQVAR(var)
{
#ifdef HEAPTEST
HeapFileMessage("start VARLISTVAR::VARLISTVAR(VARLISTVAR& var)");
#endif

   listarg     = var.GetArg();
   n           = var.n;
   last        = var.last;
   method      = var.method;
   width       = var.width;
   xa = 0; yvara = 0;
   if ( n > 0 )
   {
      CARDINAL i;
      xa    = new REAL[n];
      yvara = new AQVAR*[n];
      for ( i=0; i<n; i++ )
      {
         xa[i]    = (var.xa)[i];
         yvara[i] = (var.yvara)[i];
      }
   }

#ifdef HEAPTEST
HeapFileMessage("end   VARLISTVAR::VARLISTVAR(VARLISTVAR& var)");
#endif
}


VARLISTVAR::VARLISTVAR(AQVAR* var) : AQVAR(var)
{
#ifdef HEAPTEST
HeapFileMessage("start VARLISTVAR::VARLISTVAR(AQVAR* var)");
#endif

   if ( var == 0 )
   {
      init();
   }
   else
   {
      if ( var->Type() == VarListVar )
      {
         VARLISTVAR* v = (VARLISTVAR*)var;
         listarg     = v->GetArg();
         n           = v->n;
         last        = v->last;
         method      = v->method;
         width       = v->width;
         xa = 0; yvara = 0;
         if ( n > 0 )
         {
            CARDINAL i;
            xa    = new REAL[n];
            yvara = new AQVAR*[n];
            for ( i=0; i<n; i++ )
            {
               xa[i]    = (v->xa)[i];
               yvara[i] = (v->yvara)[i];
            }
         }
      }
      else
      {
         init();
      }
   }

#ifdef HEAPTEST
HeapFileMessage("end   VARLISTVAR::VARLISTVAR(AQVAR* var)");
#endif
}


VARLISTVAR::~VARLISTVAR()
{
#ifdef HEAPTEST
HeapFileMessage("start VARLISTVAR::~VARLISTVAR()");
#endif

   delete [] xa; delete [] yvara;

#ifdef HEAPTEST
HeapFileMessage("end   VARLISTVAR::~VARLISTVAR()");
#endif
}


const char* VARLISTVAR::TypeName() const
{
   return ini.T(38);
}


BOOLEAN VARLISTVAR::AllowedVar()
{
   if ( listarg == 0 ) return FALSE;
   if ( n == 0 )       return FALSE;
   return TRUE;
}


CARDINAL VARLISTVAR::NumVarArg() const
{
   CARDINAL k = 0;
   if ( listarg != 0 ) k = 1;
   return n+k;
}


AQVAR* VARLISTVAR::VarArg(CARDINAL index) const
{
   CARDINAL k = 0;
   if ( listarg != 0 )
   {
      if ( index == 0 ) return listarg;
      k = 1;
   }
   if ( index >= n+k )
      FatalError("VARLISTVAR::VarArg: Illegal index");
   return yvara[index-k];
}


CARDINAL VARLISTVAR::ArgIndex(REAL argval)
{
	return Index(xa,n,argval,&last);
}


REAL VARLISTVAR::Evaluate()
{
   if ( listarg == 0 )
      FatalError("VARLISTVAR::Evaluate: argument pointer is zero");
   if ( NumPairs() == 0 )
      FatalError("VARLISTVAR::Evaluate: list is empty");
   REAL x = listarg->Value();
   REAL y;
   switch ( method )
   {
      case linear:
      {
         if ( x <= xa[0] )
         {
            y = yvara[0]->Value();
         }
         else
         {
            if ( x >= xa[n-1] )
            {
               y = yvara[n-1]->Value();
            }
            else
            {
               CARDINAL low = Index(xa,n,x,&last);
               REAL y1 = yvara[low]->Value();
               REAL y2 = yvara[low+1]->Value();
               y = ( (x-xa[low])*y2 + (xa[low+1]-x)*y1 ) / (xa[low+1]-xa[low]);
            }
         }
         break;
      }
      case spline:
      {
         REAL* ya = new REAL[n];
         REAL* yaxx = new REAL[n];
         for ( CARDINAL i=0; i<n; i++ ) ya[i] = yvara[i]->Value();
         SplineInit(xa,ya,n,yaxx);
         y = Spline(xa,ya,yaxx,n,x,&last);
         delete ya;
         delete yaxx;
         break;
      }
      case smooth:
      {
         REAL* ya = new REAL[n];
         for ( CARDINAL i=0; i<n; i++ ) ya[i] = yvara[i]->Value();
         y = Smooth(xa,ya,n,x,width,&last);
         delete ya;
         break;
      }
      default:
         FatalError("VARLISTVAR::Evaluate: unknown interpolation method");
   }
   return y;
}


BOOLEAN VARLISTVAR::SetArg(AQVAR* var)
{
   if ( InList() == TRUE ) return FALSE;
   if ( var == 0 )         return FALSE;
   listarg = var;
   Changed();
   return TRUE;
}


BOOLEAN VARLISTVAR::Add(REAL x, AQVAR* yvar)
{
   if ( yvar == 0 ) return FALSE;
   CARDINAL pos = Index(xa,n,x,&last);
   if ( n == 0 )
   {
      xa    = new REAL[1];
      yvara = new AQVAR*[1];
      xa[0]    = x;
      yvara[0] = yvar;
   }
   else
   {
      if ( x == xa[pos] ) return FALSE;
      if ( pos>0 || x>xa[0] ) pos++;
      REAL* xanew       = new REAL[n+1];
      AQVAR** yvaranew  = new AQVAR*[n+1];
      CARDINAL i;
      for ( i=0; i<pos; i++ )
      {
         xanew[i]    = xa[i];
         yvaranew[i] = yvara[i];
      }
      xanew[pos]    = x;
      yvaranew[pos] = yvar;
      for ( i=pos; i<n; i++ )
      {
         xanew[i+1]    = xa[i];
         yvaranew[i+1] = yvara[i];
      }
      delete xa;    xa    = xanew;
      delete yvara; yvara = yvaranew;
   }
   n++;
   last = pos;
   Changed();
   return TRUE;
}


BOOLEAN VARLISTVAR::Replace(CARDINAL j, REAL x, AQVAR* yvar)
{
   if ( j >= n )                  return FALSE;
   if ( yvar == 0 )               return FALSE;
   if ( x == xa[j] )
   {
      yvara[j] = yvar;
   }
   else
   {
      CARDINAL i = 0;
      if ( x < xa[j] ) i = 1;
      if ( Add(x,yvar) == FALSE ) return FALSE;
      if ( Delete(j+i) == FALSE )
         FatalError("VARLISTVAR::Replace: Unable to delete data pair");
   }
   Changed();
   return TRUE;
}


BOOLEAN VARLISTVAR::Delete(CARDINAL j)
{
   if ( (InList()==TRUE) && (n==1) ) return FALSE;
   if ( j >= n )                     return FALSE;
   if ( n == 1 ) return Delete();
   CARDINAL i;
   REAL* xanew      = new REAL[n-1];
   AQVAR** yvaranew = new AQVAR*[n-1];
   for ( i=0; i<j; i++ )
   {
      xanew[i]    = xa[i];
      yvaranew[i] = yvara[i];
   }
   for ( i=j+1; i<n; i++ )
   {
      xanew[i-1]    = xa[i];
      yvaranew[i-1] = yvara[i];
   }
   delete xa;    xa    = xanew;
   delete yvara; yvara = yvaranew;
   n--;
   last = j; if ( last >= n ) last = n-1;
   Changed();
   return TRUE;
}


BOOLEAN VARLISTVAR::Delete()
{
   if ( InList() == TRUE ) return FALSE;
   delete xa;    xa    = 0;
   delete yvara; yvara = 0;
   n = 0;
   last = 0;
   Changed();
   return TRUE;
}


void VARLISTVAR::ReplVarArg(AQVAR* oldvar, AQVAR* newvar)
{
   if ( listarg == oldvar )
   {
      listarg = newvar;
      Changed();
   }
   for ( CARDINAL i=0; i<n; i++ )
   {
      if ( yvara[i] == oldvar ) 
      {
         yvara[i] = newvar;
         Changed();
      }
   }
   return;
}


void VARLISTVAR::Method(INTPOLMETH m)
{
   method = m;
   return;
}


BOOLEAN VARLISTVAR::SmoothWidth(REAL w)
{
   if ( w <= 0.0 ) return FALSE;
   width = w;
   return TRUE;
}


JOBSTATUS VARLISTVAR::Load(std::istream& in, const VARLIST* varlist)
{
   char Buffer[BufSize];

   if ( LoadStart(in) == FALSE )                           return PROBLEM;
   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )                return PROBLEM;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.0
         {
            JOBSTATUS status = AQVAR::Load(in);
                                       if ( status != OK ) return status;
            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                           return PROBLEM;
            AQVAR* var = varlist->Get(Buffer);
            if ( var == 0 )                                return PROBLEM;
            if ( SetArg(var) == FALSE )                    return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                           return PROBLEM;   
            CARDINAL type = 0;
            while ( type < NumIntPolMeth )
            {
               if ( strcmp(Buffer,IntPolMethIdentifier[type]) == 0 ) break;
               type++;
            }
            switch (type)
            {
               case linear:
                  Method(linear);
                  break;
               case spline:
                  Method(spline);
                  break;
               case smooth:
                  Method(smooth);
                  break;
               default:
                  return PROBLEM;
           }

            REAL r;
            if ( LoadNumber(in,&r) == FALSE )              return PROBLEM;
            if ( SmoothWidth(r) == FALSE )                 return PROBLEM;

            if ( LoadStart(in) == FALSE )                  return PROBLEM;
            REAL x; AQVAR* yvar;
            while ( LoadNumber(in,&x) == TRUE )
            {
               if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                           return PROBLEM;
               yvar = varlist->Get(Buffer);
               if ( Add(x,yvar) == FALSE )                 return PROBLEM;
            }
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                             return PROBLEM;
   return OK;
}


JOBSTATUS VARLISTVAR::Save(std::ostream& out)
{
   if ( SaveStart(out) == FALSE )                             return PROBLEM;

   INTEGER aquavers = 1;
   if ( SaveNumber(out,aquavers) == FALSE )                 return PROBLEM;

   JOBSTATUS status = AQVAR::Save(out); if ( status != OK ) return status;

   if ( listarg == 0 )
      FatalError("VARLISTVAR::Save: no argument");
   if ( SaveString(out,listarg->Symbol()) == FALSE )          return PROBLEM;

   if ( Method() > NumIntPolMeth )
      FatalError("VARLISTVAR::Save: Unknown interpolation method");
   if ( SaveString(out,IntPolMethIdentifier[Method()]) == FALSE )
                                                              return PROBLEM;

   if ( SaveNumber(out,SmoothWidth()) == FALSE )              return PROBLEM;

   if ( SaveStart(out) == FALSE )                             return PROBLEM;
   for ( CARDINAL i=0; i<n; i++ )
   {
      if ( SaveNumber(out,xa[i]) == FALSE )                   return PROBLEM;
      if ( SaveString(out,yvara[i]->Symbol()) == FALSE )      return PROBLEM;
   }
   if ( SaveEnd(out) == FALSE )                               return PROBLEM;

   if ( SaveEnd(out) == FALSE )                               return PROBLEM;
   return FILEIO::Save(out);
}


JOBSTATUS VARLISTVAR::Write(std::ostream& out, BOOLEAN sh)
{
   JOBSTATUS status = AQVAR::Write(out,sh); 
                                          if ( status != OK ) return status;

   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   CARDINAL pos3 = pos1+2;
   CARDINAL pos4 = pos3+abs(out.precision())+8;

   if ( sh == TRUE )
   {
      PrintIndent(pos1-1);
      if ( Print(out,TypeName(),pos1) == FALSE )             return PROBLEM;
      if ( Print(out," (") == FALSE )                        return PROBLEM;
      if ( Print(out,listarg->Symbol()) == FALSE )           return PROBLEM;
      if ( Print(out,")") == FALSE )                         return PROBLEM;
   }
   else
   {
      PrintIndent(pos2-1);
      if ( listarg == 0 ) FatalError("VARLISTVAR::Write: no argument");
      if ( Print(out,ini.T(39),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      if ( Print(out,listarg->Symbol(),pos2) == FALSE )      return PROBLEM;
      if ( Print(out,ini.T(14),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      switch ( Method() )
      {
         case linear:
            if ( Print(out,ini.T(15),pos2) == FALSE )        return PROBLEM;
            break;
         case spline:
            if ( Print(out,ini.T(16),pos2) == FALSE )        return PROBLEM;
            break;
         case smooth:
            if ( Print(out,ini.T(17),pos2) == FALSE )        return PROBLEM;
            if ( Print(out,ini.T(18),pos1) == FALSE )        return PROBLEM;
            if ( Print(out,":") == FALSE )                   return PROBLEM;
            if ( Print(out,SmoothWidth(),pos2) == FALSE )    return PROBLEM;
            break;
         default:
            return PROBLEM;
      }
      if ( Print(out,ini.T(40),pos1) == FALSE )              return PROBLEM;
      if ( Print(out," (") == FALSE )                        return PROBLEM;
      if ( Print(out,NumPairs()) == FALSE )                  return PROBLEM;
      if ( Print(out," ") == FALSE )                         return PROBLEM;
      if ( Print(out,ini.T(67)) == FALSE )                   return PROBLEM;
      if ( Print(out,")") == FALSE )                         return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      PrintIndent(pos1+6-1);
      for ( CARDINAL i=0; i<n; i++ )
      {
         if ( Print(out,xa[i],pos3) == FALSE )               return PROBLEM;
         if ( Print(out,yvara[i]->Symbol(),pos4) == FALSE )  return PROBLEM;
      }
   }

   return OK;
}


void VARLISTVAR::Test()
{
   *ctest << "\nclass VARLISTVAR: size        = " << n;
   AQVAR::Test();
}

//////////////////////////////////////////////////////////////////////////////


void PROBEVAR::init()
{
   variable    = 0;
   compartment = 0;
   compname    = 0;
   zone        = 0;
   space       = 0.0;
   spacerel    = FALSE;
}


PROBEVAR::PROBEVAR()
{
   init();
}


PROBEVAR::PROBEVAR(const PROBEVAR& var) : AQVAR(var)
{
   variable    = var.variable;
   compartment = var.compartment;
   if ( var.compname != 0 )
   {
      compname = new char[strlen(var.compname)+1];
	  strcpy(compname,var.compname);
   }
   else
   {
      compname = 0;
   }
   zone        = var.zone;
   space       = var.space;
   spacerel    = var.spacerel;
}


PROBEVAR::PROBEVAR(const AQVAR* var) : AQVAR(var)
{
   if ( var == 0 )
   {
      init();
   }
   else
   {
      if ( var->Type() == ProbeVar )
      {
         PROBEVAR* v = (PROBEVAR*)var;
         variable    = v->variable;
         compartment = v->compartment;
         if ( v->compname != 0 )
         {
            compname = new char[strlen(v->compname)+1];
            strcpy(compname,v->compname);
         }
         else
         {
            compname = 0;
         }
         zone        = v->zone;
         space       = v->space;
         spacerel    = v->spacerel;
      }
      else
      {
         init();
      }
   }
}


PROBEVAR::~PROBEVAR()
{
   delete compname;
}


const char* PROBEVAR::TypeName() const
{
   return ini.T(71);
}


REAL PROBEVAR::Evaluate()
{
   if ( compartment == 0 )               return 0;
   if ( glob_calcnum_ptr == 0 )          return 0;
   if ( glob_t_ptr == 0 )                return 0;
   if ( compartment->Active() == FALSE ) return 0;
   REAL val = 0;
   glob_varsys_ptr->StoreStateProgVars();
   if ( compartment->SpaceValue(glob_varsys_ptr,glob_calcnum_ptr,glob_t_ptr,
                                glob_Y,zone,space,spacerel,variable,
                                val) == FALSE ) return 0;
   glob_varsys_ptr->RecallStateProgVars();
   return val;
}


CARDINAL PROBEVAR::NumVarArg() const
{
   if ( variable == 0 ) return 0;
   return 1;
}


AQVAR* PROBEVAR::VarArg(CARDINAL index) const
{
   if ( index>0 || variable==0 )
      FatalError("PROBEVAR::VarArg: Illegal index");
   return variable;
}


CARDINAL PROBEVAR::NumCompArg() const
{
   if ( compartment == 0 ) return 0;
   return 1;
}


AQCOMP* PROBEVAR::CompArg(CARDINAL index) const
{
   if ( index>0 || compartment==0 )
      FatalError("PROBEVAR::CompArg: Illegal index");
   return compartment;
}


BOOLEAN PROBEVAR::AllowedVar()
{
   if ( variable == 0 )                 return FALSE;
   if ( compartment==0 && compname==0 ) return FALSE;
   return TRUE;
}


BOOLEAN PROBEVAR::Variable(AQVAR* var)
{
   if ( var == 0 ) return FALSE;
   Changed();
   variable = var;
   return TRUE;
}


BOOLEAN PROBEVAR::Compartment(AQCOMP* com)
{
   if ( com == 0 ) return FALSE;
   Changed();
   compartment = com;
   if ( zone >= com->NumZones() ) zone = 0;
   return TRUE;
}


BOOLEAN PROBEVAR::Zone(CARDINAL z)
{
   if ( compartment == 0 )             return FALSE;
   if ( z >= compartment->NumZones() ) return FALSE;
   zone = z;
   return TRUE;
}

	
void PROBEVAR::Space(REAL x)
{
   Changed();
   space = x;
   return;
}


void PROBEVAR::SpaceRel(BOOLEAN b)
{
   Changed();
   spacerel = b;
   return;
}


BOOLEAN PROBEVAR::UpdateComp(COMPLIST* complist)
{
	compartment = complist->Get(compname);
    if ( compartment == 0 ) return FALSE;
    return TRUE;
}


void PROBEVAR::ReplVarArg(AQVAR* oldvar, AQVAR* newvar)
{
   if ( variable == oldvar ) variable = newvar;
   return;
}


void PROBEVAR::ReplCompArg(AQCOMP* oldcomp, AQCOMP* newcomp)
{
   if ( compartment == oldcomp ) compartment = newcomp;
   if ( compartment == 0 )
   {
      zone = 0;
      return;
   }
   if ( zone >= compartment->NumZones() ) zone = 0;
   return;
}

	  
JOBSTATUS PROBEVAR::Load(std::istream& in, const VARLIST* varlist)
{
   char Buffer[BufSize];
   if ( LoadStart(in) == FALSE )                           return PROBLEM;

   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )                return PROBLEM;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.1
         {
            JOBSTATUS status = AQVAR::Load(in);
                                       if ( status != OK ) return status;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                           return PROBLEM;
            AQVAR* var = varlist->Get(Buffer);
            if ( var == 0 )                                return PROBLEM;
            if ( Variable(var) == FALSE )                  return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                           return PROBLEM;   
            compname = new char[strlen(Buffer)+1];
            strcpy(compname,Buffer);

            if ( LoadNumber(in,&zone) == FALSE )              return PROBLEM;

            REAL r;
            if ( LoadNumber(in,&r) == FALSE )              return PROBLEM;
            Space(r);
         }
         break;
      case 2: // aquavers=2: spacerel added
         {
            JOBSTATUS status = AQVAR::Load(in);
                                       if ( status != OK ) return status;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                           return PROBLEM;
            AQVAR* var = varlist->Get(Buffer);
            if ( var == 0 )                                return PROBLEM;
            if ( Variable(var) == FALSE )                  return PROBLEM;

            if ( LoadString(in,Buffer,sizeof(Buffer)) == FALSE )
                                                           return PROBLEM;   
            compname = new char[strlen(Buffer)+1];
            strcpy(compname,Buffer);

            if ( LoadNumber(in,&zone) == FALSE )           return PROBLEM;

            REAL r;
            if ( LoadNumber(in,&r) == FALSE )              return PROBLEM;
            Space(r);

            BOOLEAN b;
            if ( LoadBoolean(in,&b) == FALSE )             return PROBLEM;
            SpaceRel(b);
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                             return PROBLEM;
   return OK;
}


JOBSTATUS PROBEVAR::Save(std::ostream& out)
{
   if ( SaveStart(out) == FALSE )                           return PROBLEM;

   INTEGER aquavers = 2;
   if ( SaveNumber(out,aquavers) == FALSE )                 return PROBLEM;

   JOBSTATUS status = AQVAR::Save(out); if ( status != OK ) return status;

   if ( variable == 0 )
      FatalError("PROBEVAR::Save: variable not assigned");
   if ( SaveString(out,variable->Symbol()) == FALSE )       return PROBLEM;
   if ( compartment == 0 )
      FatalError("PROBEVAR::Save: compartment not assigned");
   if ( SaveString(out,compartment->Symbol()) == FALSE )    return PROBLEM;
   if ( SaveNumber(out,zone) == FALSE )                     return PROBLEM;
   if ( SaveNumber(out,space) == FALSE )                    return PROBLEM;
   if ( SaveBoolean(out,spacerel) == FALSE )                return PROBLEM;

   if ( SaveEnd(out) == FALSE )                             return PROBLEM;
   return FILEIO::Save(out);
}


JOBSTATUS PROBEVAR::Write(std::ostream& out, BOOLEAN sh)
{
   JOBSTATUS status = AQVAR::Write(out,sh); 
                                         if ( status != OK ) return status;

   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   if ( sh == TRUE )
   {
      PrintIndent(pos1-1);
      if ( Print(out,variable->Symbol(),pos1) == FALSE )     return PROBLEM;
      if ( Print(out,"(") == FALSE )                         return PROBLEM;
	  if ( compartment != 0 )
      {
         if ( Print(out,compartment->Symbol()) == FALSE )    return PROBLEM;
      }
      if ( Print(out,",") == FALSE )                         return PROBLEM;
	  if ( compartment != 0 )
      {
         if ( Print(out,compartment->ZoneName(zone)) == FALSE ) 
                                                             return PROBLEM;
      }
      if ( Print(out,",") == FALSE )                         return PROBLEM;
      if ( Print(out,space) == FALSE )                       return PROBLEM;
      if ( spacerel == TRUE )
      {
         if ( Print(out,",") == FALSE )                      return PROBLEM;
         if ( Print(out,ini.T(87)) == FALSE )                return PROBLEM;
      }
      if ( Print(out,")") == FALSE )                         return PROBLEM;
   }
   else
   {
      PrintIndent(pos2-1);
      if ( Print(out,ini.T(72),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      if ( variable != 0 )
      {
         if ( Print(out,variable->Symbol(),pos2) == FALSE )  return PROBLEM;
      }
      if ( Print(out,ini.T(73),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      if ( compartment != 0 )
      {
         if ( Print(out,compartment->Symbol(),pos2) == FALSE )
			                                                 return PROBLEM;
      }
      if ( Print(out,ini.T(84),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      if ( compartment != 0 )
      {
         if ( Print(out,compartment->ZoneName(zone),pos2) == FALSE )
                                                             return PROBLEM;
      }
      if ( Print(out,ini.T(74),pos1) == FALSE )              return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      if ( Print(out,space,pos2) == FALSE )                  return PROBLEM;
      if ( spacerel == TRUE )
      {
         if ( Print(out,",") == FALSE )                      return PROBLEM;
         if ( Print(out,ini.T(87)) == FALSE )                return PROBLEM;
      }
   }

   return OK;
}


void PROBEVAR::Test()
{
   *ctest << "\nclass PROBEVAR:   variable    = " << variable;
   *ctest << "\n                  compartment = " << compartment;
   *ctest << "\n                  space       = " << space;
   AQVAR::Test();
}

//////////////////////////////////////////////////////////////////////////////


