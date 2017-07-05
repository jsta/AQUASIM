#include <math.h>

#include "all.h"
#include "formvar.h"

const CARDINAL BufSize = 1024;

// Locally Defined Classes
// ==========================================================================   

class NumEndNode: public Fnode               // terminal node for numbers
{
  public:
  
    REAL num;  // "constant"
    
    REAL evaluate() {return num;}
    Fnode* copy();
    void visitnodes();
    NumEndNode() {num=-999;}
    ~NumEndNode() {}
 
};

class ConstNode: public Fnode           // terminal node for "constants"
{ 
  public:
  
    const REAL* num; // constant
    short name;      // index of name of constant
    
    Fnode* copy();
    REAL evaluate() {return *num;}
    void visitnodes();
    ConstNode() {num=0;}
    ~ConstNode() {}
  
};

class VarEndNode: public Fnode           // terminal node for named variables
{ 
  public:
  
    REAL evaluate() { return var->Value(); }
    Fnode* copy();
    void visitnodes();
    VarEndNode(AQVAR* v) { var=v; }
    ~VarEndNode() {}

  protected:

    AQVAR* var;
  
};

class FuncNode: public Fnode               // non-terminal node for functions with 1 argument
{
  public:
  
    REAL (*func) (REAL);
    
    Fnode* arg;            // argument
    
    short name;            // function name index
    
    REAL evaluate() {return func(arg->evaluate());}
    Fnode* copy();
    void visitnodes();
    FuncNode() {arg=0;}
    ~FuncNode() {delete arg;}
    
};

class Func2Node: public Fnode              // non-terminal node for functions with 2 arguments
{
  public:
  
    REAL (*func) (REAL, REAL);
    
    Fnode* arg1;           // argument
    Fnode* arg2;           // argument

    short name;            // function name number
    
    REAL evaluate() {return func(arg1->evaluate(),arg2->evaluate());}
    Fnode* copy();
    void visitnodes();
    Func2Node() {arg1=0; arg2=0;}
    ~Func2Node() {delete arg1; delete arg2;}
    
};

class ExprNode: public Fnode             // non-terminal node for expressions
{
  public:
  
    Fnode* expr;     // expression
    
    REAL evaluate() {return expr->evaluate();}
    Fnode* copy();
    void visitnodes();
    ExprNode() {expr=0;}
    ~ExprNode() {delete expr;}
    
};

class AssNode: public Fnode              // non-terminal node for assignment
{
  public:

    Fnode* expres;     // expression

    char**  varname;

    REAL evaluate() {return expres->evaluate();}
    Fnode* copy();
    void visitnodes();

    AssNode() {expres=0;}
    ~AssNode() {delete expres;}

};

class IfNode: public Fnode                 // non-terminal node for "if then else" function
{
  public:

    Fnode* condition;       // expression compared with false
    Fnode* ifcond0;         // expression evaluated if condition = true
    Fnode* elcondx;         // expression evaluated if condition = false

    REAL evaluate();
    Fnode* copy();
    void visitnodes();
    IfNode() {condition=0; ifcond0=0; elcondx=0;}
    ~IfNode() {delete condition; delete ifcond0; delete elcondx;}
};


class NegNode: public Fnode           // non-terminal node for negative sign 
{
  public:
  
    Fnode* expr;     // operand 
    
    REAL evaluate() {return -expr->evaluate();}
    Fnode* copy();
    void visitnodes();
    NegNode() {expr=0;}
    ~NegNode() {delete expr;}
    
};

class AddNode: public Fnode                // non-terminal node for addition
{
  public:
  
    Fnode* arg0;            // operand
    Fnode* arg1;            // operand
    
    REAL evaluate() {return arg0->evaluate() + arg1->evaluate();}
    Fnode* copy();
    void visitnodes();
    AddNode() { arg0=0;  arg1=0;}
    ~AddNode() {delete arg0; delete arg1;}
};

class SubNode: public Fnode             // non-terminal node for subtraction
{
  public:
  
    Fnode* arg0;            // operand
    Fnode* arg1;            // operand
    
    REAL evaluate() {return arg0->evaluate() - arg1->evaluate();}
    Fnode* copy();
    void visitnodes();
    SubNode() {arg0=0; arg1=0;}
    ~SubNode() {delete arg0; delete arg1;}
};

class MulNode: public Fnode           // non-terminal node for multiplication
{
  public:
  
    Fnode* arg0;            // operand
    Fnode* arg1;            // operand
    
    REAL evaluate() {return arg0->evaluate() * arg1->evaluate();}
    Fnode* copy();
    void visitnodes();
    MulNode() {arg0=0; arg1=0;}
    ~MulNode() {delete arg0; delete arg1;}
};

class DivNode: public Fnode                 // non-terminal node for division
{
  public:
  
    Fnode* arg0;            // operand
    Fnode* arg1;            // operand
  
    REAL evaluate() {return arg0->evaluate() / arg1->evaluate();}
    Fnode* copy();
    void visitnodes();
    DivNode() {arg0=0; arg1=0;}
    ~DivNode() {delete arg0; delete arg1;}
};

class IDIVNode: public Fnode                 // non-terminal node for integer division
{
  public:

    Fnode* arg0;
    Fnode* arg1;

    REAL evaluate() {return floor(arg0->evaluate()/arg1->evaluate());}
    Fnode* copy();
    void visitnodes();
    IDIVNode() {arg0=0; arg1=0;}
    ~IDIVNode() {delete arg0; delete arg1;}
};

class MODNode: public Fnode                 // non-terminal node for modulo operation
{
  public:

    Fnode* arg0;
    Fnode* arg1;

    REAL evaluate();
    Fnode* copy();
    void visitnodes();
    MODNode() {arg0=0; arg1=0;}
    ~MODNode() {delete arg0; delete arg1;}
};

REAL MODNode::evaluate()
{
   REAL Arg1 = fabs(arg1->evaluate());
   REAL Arg0 = arg0->evaluate();
   REAL val = fmod(Arg0,Arg1);
   if ( val < 0 ) val = Arg1 + val;
   return val;
}

class PowNode: public Fnode                  // non-terminal node for powers
{
  public:
  
    Fnode* arg0;            // operand
    Fnode* arg1;            // exponent
  
    REAL evaluate() {return pow(arg0->evaluate() , arg1->evaluate()) ;}
    Fnode* copy();
    void visitnodes();
    PowNode() {arg0=0; arg1=0;}
    ~PowNode() {delete arg0; delete arg1;}
};

class EQNode: public Fnode                 // non-terminal node for EQUAL test
{
  public:

    Fnode* arg0;            // left operand
    Fnode* arg1;            // right operand

    REAL evaluate();
    Fnode* copy();
    void visitnodes();
    EQNode() {arg0=0; arg1=0;}
    ~EQNode() {delete arg0; delete arg1;}
};

class NENode: public Fnode                 // non-terminal node for NOT EQUAL test
{
  public:

    Fnode* arg0;            // left operand
    Fnode* arg1;            // right operand

    REAL evaluate();
    Fnode* copy();
    void visitnodes();
    NENode() {arg0=0; arg1=0;}
    ~NENode() {delete arg0; delete arg1;}
};

class LTNode: public Fnode                 // non-terminal node for LESS THAN test
{
  public:

    Fnode* arg0;            // left operand
    Fnode* arg1;            // right operand

    REAL evaluate();
    Fnode* copy();
    void visitnodes();
    LTNode() {arg0=0; arg1=0;}
    ~LTNode() {delete arg0; delete arg1;}
};

class LENode: public Fnode                 // non-terminal node for LESS EQUAL test
{
  public:

    Fnode* arg0;            // left operand
    Fnode* arg1;            // right operand

    REAL evaluate();
    Fnode* copy();
    void visitnodes();
    LENode() {arg0=0; arg1=0;}
    ~LENode() {delete arg0; delete arg1;}
};

class GTNode: public Fnode                 // non-terminal node for GREATER test
{
  public:

    Fnode* arg0;            // left operand
    Fnode* arg1;            // right operand

    REAL evaluate();
    Fnode* copy();
    void visitnodes();
    GTNode() {arg0=0; arg1=0;}
    ~GTNode() {delete arg0; delete arg1;}
};

class GENode: public Fnode                 // non-terminal node for GREATER EQUAL test
{
  public:

    Fnode* arg0;            // left operand
    Fnode* arg1;            // right operand

    REAL evaluate();
    Fnode* copy();
    void visitnodes();
    GENode() {arg0=0; arg1=0;}
    ~GENode() {delete arg0; delete arg1;}
};

class NOTNode: public Fnode                 // non-terminal node for NOT operation
{
  public:

    Fnode* arg;            // operand

    REAL evaluate();
    Fnode* copy();
    void visitnodes();
    NOTNode() {arg=0;}
    ~NOTNode() {delete arg;}
};

class ANDNode: public Fnode                 // non-terminal node for AND operation
{
  public:

    Fnode* arg0;            // left operand
    Fnode* arg1;            // right operand

    REAL evaluate();
    Fnode* copy();
    void visitnodes();
    ANDNode() {arg0=0; arg1=0;}
    ~ANDNode() {delete arg0; delete arg1;}
};

class ORNode: public Fnode                 // non-terminal node for OR operation
{
  public:

    Fnode* arg0;            // left operand
    Fnode* arg1;            // right operand

    REAL evaluate();
    Fnode* copy();
    void visitnodes();
    ORNode() {arg0=0; arg1=0;}
    ~ORNode() {delete arg0; delete arg1;}
};

// Error messages:
// ==========================================================================   
const CARDINAL errbase  = 50;
      // Calling parserror(errbase + i) refers to error i in list errstr.
      // Calling parserror(i), i< errbase refers to SyDict[abs(i)]

      // GetErrorMsg interprets error numbers < errbase as follows
      // in list SyDict: i < 0 -> unexpected: SyDict[-i]
      //                 0 < i ->   expected: SyDict[ i]

const char* /*const*/ errstr[]=
               { 0                                  , // error number
                "implementation error"              , // 1
                "Variable name UNKNOWN"             , // 2
                " NOT YET IMPLEMENTED "             , // 3
                "expected: + - or"                  , // 4
                "expected: + - * / # div mod and"   , // 5
                "Factor expected"                   , // 6
                "Term expected"                     , // 7
                "Expression expected"               , // 8
                "Empty Expression"                  , // 9
                "expected: = <> <= < >= >"          , // 10
                "cannot assign to this"             , // 11
                 0
               };


// Globals for Parsing and for UnParsing:
// ========================================================================== 
    const CARDINAL maxStrLen   = 80;    
                               // maximum allowed length of strings for
                               // variable identifiers
    const CARDINAL maxNumBuff  = 50;
                               // maximum allowed length of string
                               // representations of numbers (UnParse)
    static int arglen;         // length of fromline; if *errorline has been
                               // dimensioned smaller than *formline, the
                               // program may produce STRANGE ERRORS !!!
                               //   sizefo(*) < sizeof(fromline*);
    const char* l;             // current *fromline or *toline
    static int   i;            // current *fromline or *toline index
    static char  c;            // current char in *fromline

    static int lastitem;       // type of last item defined by get_item()
    static int lastSy;         // pointer into Symbol Dictionary

    const VARLIST* varLIST;    // variable tree

    static AQVAR** ARG;        // pointer to FORMVAR::arg
    static int   VarnodeVisit; // case tag for VarEndNode::visitnodes(),
                               // set by PutArgs()
    static int   VARCOUNT;     // variable counter and index of arg
                               // (see PutArgs, VarEndNode::visitnodes())
    static AQVAR*  OLDVAR;     // see argument: variable replacement
    static AQVAR*  NEWVAR;     // see argument: variable replacement

    static char workstr[maxStrLen];  // buffer for input of strings

    static char* erl;          // pointer to errorline (Parse, parserror)
    static int   errors;       // total number of errors in Parse
    static int   uperrors;     // total number of errors in UnParse


    static char numbuff[maxNumBuff]; // NumEndNode::visitnodes
	static std::ostrstream oss(numbuff,
            sizeof(numbuff),
			std::ios::out|std::ios::trunc); // NumEndNode::visitnodes
    static char* UNPline;      // pointer to toline (UnParse)
    static int  UNPcur;        // index of current character in toline
    static int  UNPmax;        // maxch (Unparse): max. length of toline 



// Dictionary of Syntactic Symbols
// ==========================================================================  
// -> List SyDict must have a leading 0 element
//    followed by MaxRe string elements. The position of each string in the
//    list corresponds to a case label and should not be changed.

const CARDINAL MaxRe = 31;  // reserved symbols checked in FORMVARSymbol
const CARDINAL MaxSy = 26;  // syntactic symbols parsed in itemtype, FindSymbol;

const char* /*const*/ SyDict[]=
                    { 0   ,
                     ""   , ""   , "("  , ")"  , "+"   , // parsed in itemtype
                     "-"  , "*"  , "/"  , "^"  , "="   , // parsed in itemtype
                     "<>" , "<=" , "<"  , ">=" , ">"   , // parsed in itemtype
                     "div"       , "mod"       ,         // parsed in FindSymbol
                     "not"       , "and"       , "or"  , // parsed in FindSymbol
                     "if"        , "then"      ,         // parsed in FindSymbol
                     "else"      , "endif"     ,         // parsed in FindSymbol
                     ""          , ""          ,         // [reserve]
                     "#"  , "**" , ","  , ";"  , ""    , //[parsed in itemtype ]
                      0
                    };  


// Dictionary of Constants
// ==========================================================================   
// -> List CoDict must have a leading 0 element
//    followed by MaxCo string elements.

const CARDINAL MaxCo =  4;
                   // reserved symbols checked in FORMVARSymbol
                   // symbols parsed in FindSymbol;

const char* /*const*/ CoDict[]=
                    { 0   ,
                     "pi",
                     "true", "false",
                     "precision"
                    };

// constant definitions:

    static REAL vT=1;        // TRUE; see SetTrue
    static REAL vF=0;        // FALSE; see SetTrue
    static REAL Precision=6; // number precision in unparsing; SetPrecision


// Dictionary of Mathematical Functions with 2 Arguments
// ==========================================================================   
// -> Lists F2Dict and mFn2 must both have a leading 0 element,
//    followed by MaxF2 string and address elements corresponding in both lists.

const CARDINAL MaxF2 =  2;  
                   // reserved symbols checked in FORMVARSymbol
                   // symbols parsed in FindSymbol;

const char* /*const*/ F2Dict[]=
                    {   0    ,
                     "min"   , "max"
                    };

// function definitions:

static REAL pmin (REAL m1, REAL m2)
   {if (m1 < m2) return m1; else return m2; }
static REAL pmax (REAL m1, REAL m2)
   {if (m1 > m2) return m1; else return m2; }

// function lookup table:

//REAL (*const mFn2[]) (REAL, REAL) = {0, pmin, pmax };
// new version because of problems with gnu C++ compiler: (!!!)

typedef REAL (*FUN2PTR) (REAL,REAL);
FUN2PTR mFn2[] = {0, pmin, pmax };

// Dictionary of Mathematical Functions with 1 Argument
// ==========================================================================   
// -> Lists F1Dict and mFn1 must both have a leading 0 element,
//    followed by MaxF1 elements corresponding in both lists.

const CARDINAL MaxF1 = 20;
                   // reserved symbols checked in FORMVARSymbol
                   // symbols parsed in FindSymbol;

const char* /*const*/ F1Dict[]=
                     { 0     ,
                      "sin"  , "cos" , "tan" ,
                      "asin" , "acos", "atan",
                      "sinh" , "cosh", "tanh",
                      "deg"  , "rad" ,
                      "exp"  , "log" , "ln"  , "log10",
                      "sign" , "abs" , "sqrt",
                      "settrue",
                      "setprecision"
                     };

// function definitions:

static REAL psin  (REAL r)  {return sin(r); }
static REAL pcos  (REAL r)  {return cos(r); }
static REAL ptan  (REAL r)  {return tan(r); }
static REAL pasin (REAL r)  {return asin(r); }
static REAL pacos (REAL r)  {return acos(r); }
static REAL patan (REAL r)  {return atan(r); }
static REAL psinh (REAL r)  {return sinh(r); }
static REAL pcosh (REAL r)  {return cosh(r); }
static REAL ptanh (REAL r)  {return tanh(r); }

static REAL pdeg  (REAL r)  {return 180.0 * r / aq_pi; }
static REAL prad  (REAL r)  {return aq_pi * r / 180.0; }

static REAL pexp  (REAL r)  {return exp(r); }
static REAL plog  (REAL r)  {return log(r); }
static REAL pln   (REAL r)  {return log(r); }
static REAL plog10(REAL r)  {return log10(r); }

static REAL psign (REAL r)  {if (r < 0.0) return -1.0;
                                  if (r > 0.0) return  1.0;
                                  return  0.0; }
static REAL pabs  (REAL r)  {return fabs(r); }
static REAL psqrt (REAL r)  {return sqrt(r); }

static void setTrue(REAL ZeroOrOne);  // forward declaration
static void setPrecision(REAL to);    // forward declaration

static REAL psettrue (REAL r)  {setTrue(r); return 1.0; }
static REAL psetprec (REAL r)  {setPrecision(r); return 1.0; }

// function lookup table:

//REAL (*const mFn1[]) (REAL) = {0,psin,pcos,ptan,
//                               pasin,pacos,patan,
//                               psinh,pcosh,ptanh,
//                               pdeg,prad,
//                               pexp,plog,pln,plog10,
//                               psign,pabs,psqrt,
//                               psettrue,psetprec};
// new version because of problems with gnu C++ compiler: (!!!)

typedef REAL (*FUN1PTR) (REAL);
FUN1PTR mFn1[] = {0,psin,pcos,ptan,
                  pasin,pacos,patan,
                  psinh,pcosh,ptanh,
                  pdeg,prad,
                  pexp,plog,pln,plog10,
                  psign,pabs,psqrt,
                  psettrue,psetprec};


   
// Procedures for Evaluation not defined in formvar.h
// ==========================================================================  

REAL EQNode::evaluate()
{ if (arg0->evaluate() == arg1->evaluate()) return vT; else return vF;
}

REAL NENode::evaluate()
{ if (arg0->evaluate() != arg1->evaluate()) return vT; else return vF;
}

REAL LTNode::evaluate()
{ if (arg1->evaluate() > arg0->evaluate()) return vT; else return vF;
}

REAL LENode::evaluate()
{ if (arg0->evaluate() <= arg1->evaluate())  return vT; else return vF;
}

REAL GTNode::evaluate()
{ if (arg0->evaluate() > arg1->evaluate()) return vT; else return vF;
}

REAL GENode::evaluate()
{ if (arg0->evaluate() >= arg1->evaluate()) return vT; else return vF;
}

REAL NOTNode::evaluate()
{ if (arg->evaluate() == vF)
   return vT; else return vF;
}

REAL ANDNode::evaluate()
{ if (arg0->evaluate() == vT && arg1->evaluate() == vT)
   return vT; else return vF;
}

REAL ORNode::evaluate()
{ if (arg0->evaluate() == vT || arg1->evaluate() == vT)
   return vT; else return vF;
}

REAL IfNode::evaluate()
{ 
//  cout<<" CONDITION: "<<condition->evaluate()<<"\n";
  if (condition->evaluate() != vF)
  return ifcond0->evaluate();
  else return elcondx->evaluate();
}

   
// Procedures for Precision in Unparsing and TRUE/FALSE Definition
// ==========================================================================   

static void setPrecision(REAL to)
{ int p=(int)(to+0.5);
      Precision=p;
// cout<<oss.precision()<<"<-"<<p<<"start INteger Precision\n";
 oss.precision(p);
/*
 cout<<oss.precision()<<"<-"<<p<<"after var p INteger Precision\n" << flush;
 oss.precision(10);
cout<<oss.precision()<<"<-"<<p<<"after const 10 INteger Precision\n";
*/
}
void FORMVAR::SetPrecision(int to)
{ setPrecision(to);
}

static void setTrue(REAL ZeroOrOne)
{ if (ZeroOrOne == 0) { vT=0;          vF=1; }
  else                { vT=1;          vF=0; }
}
void FORMVAR::SetTrue(REAL ZeroOrOne)
{ setTrue(ZeroOrOne);
}

// Procedures for Copying Nodes.
// ==========================================================================
Fnode* NumEndNode::copy()
{ NumEndNode* n = new NumEndNode;
  n->num=num;
  return n;
}

Fnode* ConstNode::copy()
{ ConstNode* n = new ConstNode;
  n->num =num;
  n->name=name;
  return n;
}

Fnode* VarEndNode::copy()
{ VarEndNode* n = new VarEndNode(var);
  return n;
}

Fnode* FuncNode::copy()
{ FuncNode* n = new FuncNode;
  n->func = func;
  n->arg  = arg->copy();
  n->name = name;
  return n;
}

Fnode* Func2Node::copy()
{ Func2Node* n = new Func2Node;
  n->func = func;
  n->arg1 = arg1->copy();
  n->arg2 = arg2->copy();
  n->name = name;
  return n;
}

Fnode* ExprNode::copy()
{ ExprNode* n = new ExprNode;
  n->expr = expr->copy();
  return n;
}

Fnode* AssNode::copy()
{ AssNode* n = new AssNode;
  n->expres = expres->copy();
  n->varname= varname;
  return n;
}

Fnode* IfNode::copy()
{ IfNode* n = new IfNode;
  n->condition = condition->copy();
  n->ifcond0   = ifcond0->copy();
  n->elcondx   = elcondx->copy();
  return n;
}

Fnode* NegNode::copy()
{ NegNode* n = new NegNode;
  n->expr = expr->copy();
  return n;
}

Fnode* AddNode::copy()
{ AddNode* n = new AddNode;
  n->arg0 = arg0->copy();
  n->arg1 = arg1->copy();
  return n;
}

Fnode* SubNode::copy()
{ SubNode* n = new SubNode;
  n->arg0 = arg0->copy();
  n->arg1 = arg1->copy();
  return n;
}

Fnode* MulNode::copy()
{ MulNode* n = new MulNode;
  n->arg0 = arg0->copy();
  n->arg1 = arg1->copy();
  return n;
}

Fnode* DivNode::copy()
{ DivNode* n = new DivNode;
  n->arg0 = arg0->copy();
  n->arg1 = arg1->copy();
  return n;
}

Fnode* IDIVNode::copy()
{ IDIVNode* n = new IDIVNode;
  n->arg0 = arg0->copy();
  n->arg1 = arg1->copy();
  return n;
}

Fnode* MODNode::copy()
{ MODNode* n = new MODNode;
  n->arg0 = arg0->copy();
  n->arg1 = arg1->copy();
  return n;
}

Fnode* PowNode::copy()
{ PowNode* n = new PowNode;
  n->arg0 = arg0->copy();
  n->arg1 = arg1->copy();
  return n;
}

Fnode* EQNode::copy()
{ EQNode* n = new EQNode;
  n->arg0 = arg0->copy();
  n->arg1 = arg1->copy();
  return n;
}

Fnode* NENode::copy()
{ NENode* n = new NENode;
  n->arg0 = arg0->copy();
  n->arg1 = arg1->copy();
  return n;
}

Fnode* LTNode::copy()
{ LTNode* n = new LTNode;
  n->arg0 = arg0->copy();
  n->arg1 = arg1->copy();
  return n;
}

Fnode* LENode::copy()
{ LENode* n = new LENode;
  n->arg0 = arg0->copy();
  n->arg1 = arg1->copy();
  return n;
}

Fnode* GTNode::copy()
{ GTNode* n = new GTNode;
  n->arg0 = arg0->copy();
  n->arg1 = arg1->copy();
  return n;
}

Fnode* GENode::copy()
{ GENode* n = new GENode;
  n->arg0 = arg0->copy();
  n->arg1 = arg1->copy();
  return n;
}

Fnode* NOTNode::copy()
{ NOTNode* n = new NOTNode;
  n->arg = arg->copy();
  return n;
}

Fnode* ANDNode::copy()
{ ANDNode* n = new ANDNode;
  n->arg0 = arg0->copy();
  n->arg1 = arg1->copy();
  return n;
}

Fnode* ORNode::copy()
{ ORNode* n = new ORNode;
  n->arg0 = arg0->copy();
  n->arg1 = arg1->copy();
  return n;
}

// Procedures for Visiting Nodes.
// Special case: VarEndNode::visitnodes(), communicating via global variables
//   - UnParse, SetUnparse(), unparse 
//   - Parse, PutArgs(): Count Vars & Register Vars in arg
//   - argument: replace variable
// ==========================================================================   

void (*visit_action) (const char* );
void (*endvar_action) (const char* );


static void NoAction(const char*)
{
}


// Operator = and Constructors
// ==========================================================================   

void FORMVAR::init()
{
   root = 0;
   vararg = 0; numvararg = 0;
   return;
}


FORMVAR::FORMVAR()
{
   init();
}

FORMVAR::FORMVAR(const FORMVAR& var) : AQVAR(var)
{
  init();
  if (var.root != 0)
  {
    root=(var.root)->copy();
    PutArgs();
  }
}

FORMVAR::FORMVAR(AQVAR* var) : AQVAR(var)
{
  init();
  if (var != 0)
  {
    if (var->Type() == FormVar)
    {
      FORMVAR* f = (FORMVAR*) var;
      if ( f->root != 0 )
      {
        root=(f->root)->copy();
        PutArgs();
      }
    }
    else
    {
      if ( var->Type() == ConstVar )
      {
        REAL val = var->Value();
		std::ostrstream buf;
        buf << val << '\0';
        char* string = buf.str();
        char* errlin = new char[strlen(string)+1];
        Parse(string,0,errlin);
        delete string; delete errlin;
      }
    }
  }
}

FORMVAR::~FORMVAR()
{
   delete root; root = 0;
   delete [] vararg; vararg = 0; numvararg = 0;
}

const char* FORMVAR::TypeName() const
{
   return ini.T(7);
}

BOOLEAN FORMVAR::AllowedVar()
{
   if ( root == 0 ) return FALSE;
   else             return TRUE;
}

AQVAR* FORMVAR::VarArg(CARDINAL index) const
{
   if ( index >= numvararg )
      FatalError("FORMVAR::VarArg: Illegal index");
   return vararg[index];
}

void FORMVAR::ReplVarArg(AQVAR* oldvar, AQVAR* newvar)
{
  if (numvararg)
  { VARCOUNT=0;
    ARG=vararg;
    OLDVAR=oldvar;
    NEWVAR=newvar;
    visit_action=NoAction;
    VarnodeVisit=3;              // replacement of Variable
    root->visitnodes();
  }
  return;
}

void FORMVAR::PutArgs()
{
  delete [] vararg; vararg = 0; numvararg = 0;
  if ( root == 0 ) return;
  VARCOUNT=0;
  visit_action=NoAction;
  VarnodeVisit=1;                // count Variables referenced
  root->visitnodes();

  numvararg=VARCOUNT;
  if (numvararg)
  { vararg=new AQVAR* [VARCOUNT];
    ARG=vararg;
    VARCOUNT=0;
    VarnodeVisit=2;              // copy VarEndNode.var to arg[]
    root->visitnodes();
  }
  return;
}

static void unparse(const char* add) // work horse for unparsing
{ if (UNPmax <= UNPcur) return; // do not overwrite user supplied buffer
  int i=strlen(add);
  UNPcur+=i;
  if (UNPmax <= UNPcur) i=i-(UNPcur - UNPmax);
  strncat(UNPline,add,i);
}
static void SetUnparse()
{ visit_action=unparse;
  VarnodeVisit=0;
}

void NumEndNode::visitnodes()
{ //{cout<<num;}

#if ( AQUA_OS==AQUA_MAC && AQUA_VERSION!=AQUA_VCC )
  oss.rdbuf()->pubseekoff(0,ios::beg);
#else
	oss.seekp(std::ios::beg);
#endif

// cout<<oss.precision()<<"NumEndNode visitnodes\n";
	oss<<num<<std::ends;
  visit_action(numbuff);
}

void ConstNode::visitnodes()
{ visit_action(CoDict[name]);
}

void VarEndNode::visitnodes()
{ switch(VarnodeVisit)
  { case 0: visit_action(var->Symbol()); // activated by SetUnparse()
      break;
    case 1: VARCOUNT++;                  // activated by PutVars()
      break;
    case 2: ARG[VARCOUNT++]=var;         // activated by PutVars()
      break;
    case 3: if (var == OLDVAR)           // activated by argument(..)
            { var=NEWVAR;
              ARG[VARCOUNT]=var;
            }
            VARCOUNT++;
      break;
  }
}

void NegNode::visitnodes()
{ visit_action("-"); expr->visitnodes();
}

void ExprNode::visitnodes()
{ visit_action("("); expr->visitnodes(); visit_action(")");
}

void AssNode::visitnodes()
{ visit_action(*varname); visit_action("="); expres->visitnodes();
}

void FuncNode::visitnodes()
{ visit_action(F1Dict[name]); visit_action("("); arg->visitnodes(); visit_action(")");
}

void Func2Node::visitnodes()
{ visit_action(F2Dict[name]);
  visit_action("("); arg1->visitnodes();
  visit_action(","); arg2->visitnodes(); visit_action(")");
}

void AddNode::visitnodes()
{ arg0->visitnodes(); visit_action("+"); arg1->visitnodes();
}

void SubNode::visitnodes()
{ arg0->visitnodes(); visit_action("-"); arg1->visitnodes();
}

void MulNode::visitnodes()
{ arg0->visitnodes(); visit_action("*"); arg1->visitnodes();
}

void DivNode::visitnodes()
{ arg0->visitnodes(); visit_action("/"); arg1->visitnodes();
}

void IDIVNode::visitnodes()
{ arg0->visitnodes(); visit_action(" div "); arg1->visitnodes();
}

void MODNode::visitnodes()
{ arg0->visitnodes(); visit_action(" mod "); arg1->visitnodes();
}

void PowNode::visitnodes()
{ arg0->visitnodes(); visit_action("^"); arg1->visitnodes();
}

void EQNode::visitnodes()
{
  arg0->visitnodes(); visit_action("="); arg1->visitnodes();
}

void NENode::visitnodes()
{
  arg0->visitnodes(); visit_action("<>"); arg1->visitnodes();
}

void LTNode::visitnodes()
{ arg0->visitnodes(); visit_action("<"); arg1->visitnodes();
}

void LENode::visitnodes()
{
  arg0->visitnodes(); visit_action("<="); arg1->visitnodes();
}

void GTNode::visitnodes()
{
  arg0->visitnodes(); visit_action(">"); arg1->visitnodes();
}

void GENode::visitnodes()
{
  arg0->visitnodes(); visit_action(">="); arg1->visitnodes();
}

void NOTNode::visitnodes()
{
  visit_action(" not "); arg->visitnodes();
}

void ANDNode::visitnodes()
{
  arg0->visitnodes(); visit_action(" and "); arg1->visitnodes();
}

void ORNode::visitnodes()
{
  arg0->visitnodes(); visit_action(" or "); arg1->visitnodes();
}

void IfNode::visitnodes()
{
  visit_action(" if "); condition->visitnodes();
  visit_action(" then "); ifcond0->visitnodes();
  visit_action(" else "); elcondx->visitnodes();
  visit_action(" endif "); 
}

   
// Parsing procedures
// ==========================================================================   

static void read() {if (c != 0) c=l[i++];}
                      // "read" next char from Parse.fromline

static void parserror(int error)
{ short serror=error;
 int k=2;
  if (lastitem == 99) k--;
  if (i < k) k=0;
  if (erl[i-k] == 0) { erl[i-k]=serror; errors++; }
// cout<<"serror "<<serror<<" at "<<i<< "\n"<<flush;
}
    
static REAL deci()    // parse decimal part of a number
{ REAL r=0.0;
  REAL i=1.0;
  
  while ('0' <= c && c <= '9')
  { i=i/10.0;
    r=r + i * (c - '0');
    read();
  }
  return r;
}

static REAL card()    // parse a cardinal
{ REAL r=0.0;
  while ('0' <= c && c <= '9')
  { r=10 * r + (c - '0');
    read();
  }
  return r;
}

static REAL numb()    // parse real number (incl. E-notation)
{ REAL r;
  REAL e=0.0;
  REAL sign=1.0;
  
  r=card();
  if (c == '.')
  { read(); r=r+deci();
  }
  if (c == 'e' || c == 'E')
  { read();
    if (c == '-')
    { sign=-sign; read();
    } else if (c == '+') read();
    e=pow(10.0,sign * numb());
    r=r * e;
  }
// debug: cout<<"DEBUG factor:numb(): t = "<<r<<"\n";
  return r;
}

static int findNo(const char* element, const char* const in[], int max)
{ while (0 < max && strcmp(in[max],element)) max--; return max;
}

static int FindSymbol()
{
  int e=1;
  workstr[0]=c; read();
  while ( SYMBOL::AllowedSubsequentChar(c) == TRUE )
  { workstr[e++]=c; read();
  } workstr[e]=0;
  lastSy=findNo(workstr,SyDict,MaxSy); if (lastSy) return lastSy;
  lastSy=findNo(workstr,CoDict,MaxCo); if (lastSy) return 203;
  lastSy=findNo(workstr,F2Dict,MaxF2); if (lastSy) return 202;
  lastSy=findNo(workstr,F1Dict,MaxF1); if (lastSy) return 201;
  return 200;
}

BOOLEAN FORMVAR::ReservedSymbol(const char* s)
{
  if ( s == 0 ) return FALSE;
  int i;
  i=findNo(s,SyDict,MaxSy) +
    findNo(s,CoDict,MaxCo) +
    findNo(s,F2Dict,MaxF2) +
    findNo(s,F1Dict,MaxF1);
  if (i) return TRUE; else return FALSE;
}


static int itemtype()        // get type of last character read
{ 
  if ('0' <= c && c <= '9')                  return 1;             // Number
  if ( SYMBOL::AllowedFirstChar(c) == TRUE ) return FindSymbol();  // String

  switch (c)                                          // special chars
  { case   0:         return 0;                       // End of Input Line
    case '(': read(); return 3; 
    case ')': read(); return 4; 
    case '+': read(); return 5; 
    case '-': read(); return 6; 
    case '*': read(); if (c == '*') {read(); return 9;} else return 7; 
    case '/': read(); return 8;
    case '^': read(); return 9;
    case '=': read(); return 10;
    case '#': read(); return 11;
    case '<': read(); switch(c)
                      { case '>': read(); return 11;
                        case '=': read(); return 12;
                         default: return 13;
                      }
    case '>': read(); switch(c)
                      { case '=': read(); return 14;
                         default: return 15;
                      }
    case ',': read(); return 29;       // list element separator
    case ';': read(); return 30;       // statement terminator
    case ' ': read(); return 90;       // blanc
    default:          return 99;       // not defined, illegal
  }
}

static AQVAR* findvar(char* varsym, const VARLIST* in)
{
   if ( in == 0 ) return 0;
   return in->Get(varsym);
}

static void get_item()       // skip blancs, get type of next parse element
{
  lastitem=itemtype();
  while (lastitem == 90) lastitem=itemtype(); // skip blancs
} // END get_item

static Fnode* expression();  // parse expression: forward declaration

static void debugnode(Fnode* node)
{ UNPmax=80;UNPcur=0;char u[80];UNPline=&u[0]; u[0]=0; 
  if (node) node->visitnodes();
  else strcpy(UNPline,"empty node, cannot be visitnodesd");
  std::cout<<"Debug node: "<<node<<" "<<UNPline<<"\n"; 
}

static Fnode* factor()       // parse factor
{ Fnode*      node=0;
  NumEndNode* nu=0;
  ConstNode*  co=0;
  AQVAR*      var;
  VarEndNode* va=0;
  FuncNode*   fu=0;
  Func2Node*  f2=0;
  ExprNode*   ex=0;
  NOTNode*    no=0;
  IfNode*     iffi=0;
  Fnode*      thennode=0;
  Fnode*      elsenode=0;

  // cout<<"c: "<<c<<" lastitem:"<<lastitem<<" Enter factor\n";  
  switch(lastitem)
  { case 0: // end of line
            // cout<<" EOL = NULL CHAR ";
     break;
    case 1: // number
            nu=new NumEndNode; node=nu; nu->num=numb();
     break;
    case 200: // Variable name
              var=findvar(workstr,varLIST);
              if (var)
              { va=new VarEndNode(var); node=va;
              }
              else parserror(errbase+2);
     break;
    case 201: // Function with 1 Argument
              fu=new FuncNode;
              fu->name=lastSy;
              fu->func=mFn1[lastSy];
              get_item();
              if (lastitem != 3) parserror(3);     // test (
              get_item();                           // skip (
              node=expression();
              if (lastitem != 4) parserror(4);
              if (errors == 0)
              { fu->arg=node;
                node=fu; // skip ) in get_item() at end of factor!
              } else delete fu;
     break;
    case 202: // Function with 2 Arguments
              f2=new Func2Node;
              f2->name=lastSy;
              f2->func=mFn2[lastSy];
              get_item();
              if (lastitem != 3) parserror(3);     // test (
              else
              { get_item();                         // skip (
                f2->arg1=expression();
                if (lastitem != 29) parserror(29); // test ,
                else
                { get_item();                       // skip ,
                  f2->arg2=expression();
                  if (lastitem != 4) parserror(4);
                }
              }
              if (errors == 0)
              { node=f2; // skip ) in get_item() at end of factor!
              } else delete f2;
     break;
    case 203: // Constant
              co=new ConstNode;
              switch(lastSy)
              { case 1: co->num=&aq_pi; break;
                case 2: co->num=&vT; break;
                case 3: co->num=&vF; break;
                case 4: co->num=&Precision; break;
              }
              co->name=lastSy;
              node=co;
     break;
    case 3: // (
            // cout<<" factor ( ";
            get_item();
            node=expression();
            if (node != 0)
            { ex=new ExprNode;  ex->expr=node; node=ex;
            }
            switch(lastitem)
            { case 4:                                 // )
               break;
              case 22: case 23: case 24: return node; // then else fi
              case 29: return node;                   // comma in Func2Node
              default: parserror(4);
                       delete ex;
                       node=0; // node=ex;
               break;
            }
     break;
    case 4:  // )
      return node;
    case 29: // ,
      return node;
    case 5:  // +
    case 6:  // -
    case 7:  // *
    case 8:  // /
    case 9:  // ^
            parserror(-lastitem);
            read(); get_item(); node=factor();
      break;
    case 10: case 11: case 12: case 13: case 14: case 15: // = <> <= < >= >
            parserror(-lastitem);
            read(); get_item(); node=factor();
//            return node;
      break;
    case 18: // NOT operator
             get_item();
             node=factor();
             if (node != 0)
             { no=new NOTNode;
               no->arg=node;
               node=no;
               return node; // skip get_item() !
             }
      break;
    case 21:  // if condition then expression else expression fi
                  get_item();
                  node=expression(); // now test then:
                  if (lastitem != 22) parserror(22);
                  else
                  if (node != 0)
                  { get_item();
                    thennode=expression(); // now test else:
                    if (lastitem != 23) parserror(23);
                    else 
                    if (thennode != 0)
                    { get_item();
                      elsenode=expression(); // now test fi:
                      if (lastitem != 24) parserror(24);
                    }
                  }
                  if (errors == 0)
                  { iffi=new IfNode; iffi->condition=node;
                                     iffi->ifcond0=thennode;
                                     iffi->elcondx=elsenode;
                    node=iffi;
                  } else
                  { if (node)     delete node;
                    if (elsenode) delete elsenode;
                    if (thennode) delete thennode;
                    node=0;
                  }
   
      break;
   case 90:                  break; // blanc
   default: parserror(errbase+3); // illegal character
            read(); get_item(); node=factor();
      break;
  }
  get_item();
  //*debug*/ cout<<"c:"<<c<<" lastitem:"<<lastitem<<" node: "<<node<<" Exit factor\n"<<flush;
  //*debug*/ debugnode(node);
  return node;
} // END factor

static Fnode* power()          // parse power
{ Fnode*   node=0; Fnode* fn=0;
  PowNode* p=0;

  node=factor();
  while (lastitem == 9)
  { get_item();
    fn=factor();
    if (fn)
    { p=new PowNode; p->arg0=node; p->arg1=fn; node=p;
    }
  }
  //*debug*/ cout<<"c:"<<c<<" lastitem:"<<lastitem<<" node: "<<node<<" Exit power\n"<<flush;
  //*debug*/ debugnode(node);
  return node;
} // END power

static Fnode* term()          // parse term
{ Fnode*  fn=0; Fnode* node=0; Fnode* rn=0;
//  PowNode* p=0;
  DivNode* d=0;
  IDIVNode* D=0;
  MODNode* M=0;
  MulNode* m=0;
  ANDNode* a=0;
  int local;

//  cout<<"Enter term\n";
  fn=power(); node=fn;
  while (lastitem == 7 || lastitem == 8 ||
         lastitem == 16 || lastitem == 17 || lastitem == 19)
  {  local=lastitem;
     get_item(); rn=power();
     if (rn == 0) { delete node; node=0; }
     else switch(local)
    { case 7:  m=new MulNode; m->arg0=fn;
               m->arg1=rn; node=m;
       break;
      case 8:  d=new DivNode; d->arg0=fn;
               d->arg1=rn; node=d;
       break;
      case 19: a=new ANDNode; a->arg0=fn;
               a->arg1=rn; node=a;
       break;
      case 17: M=new MODNode; M->arg0=fn;
               M->arg1=rn; node=M;
       break;
      case 16: D=new IDIVNode; D->arg0=fn;
               D->arg1=rn; node=D;
       break;
      case 1:
      case 2: parserror(errbase+4);
       break;
      default: parserror(errbase+6);
       break;
    } // switch lastitem
    if (node == 0) parserror(errbase+6);
    fn=node;
  } // while 
  //*debug*/ cout<<"c:"<<c<<" lastitem:"<<lastitem<<" node: "<<node<<" Exit term\n"<<flush;
  //*debug*/ debugnode(node);
  return node;
} // END term

static Fnode* simple_expression()   // parse expression
{ Fnode*   fn=0; Fnode* node=0;  Fnode*  rn=0;
  NegNode* ne=0;
  AddNode*  a=0;
  SubNode*  s=0;
  ORNode*   o=0;
  int local;

  // cout<<"Enter expression\n";
  switch(lastitem)
  { case 5: // +
       get_item(); fn=term();
      break;
    case 6: // -
       ne=new NegNode; fn=ne; get_item(); node=term(); ne->expr=node;
       break;
    case 0:
    case 7: case 8: case 9: case 16: case 17:
    case 90: case 99:
       parserror(errbase+8);
       break;
    default: fn=term();
       break;
  }
  node=fn;
  while (lastitem == 5 || lastitem == 6 || lastitem == 20)
  { local=lastitem;
    get_item(); rn=term();
    if (rn == 0) { delete node; node=0; }
    else switch(local)
    { case 5: a=new AddNode; a->arg0=fn;
              a->arg0=fn;
              a->arg1=rn;
              fn=a; node=a;
        break;
      case 6: s=new SubNode; s->arg0=fn;
              s->arg0=fn;
              s->arg1=rn;
              fn=s; node=s;
        break;
      case 20: o=new ORNode;
               o->arg0=fn;
               o->arg1=rn;
               fn=s; node=o;
        break;
      case 1:
      case 200: case 201: case 202: case 203: parserror(errbase+4);
        break;
      default: parserror(errbase+7);
        break;
    }
    if (rn == 0) parserror(errbase+7);
  }
  //*debug*/ cout<<"c:"<<c<<" lastitem:"<<lastitem<<" node: "<<node<<" Exit simple expression\n"<<flush;
  //*debug*/ debugnode(node);
  return node;
} // END simple_expression

static Fnode* expression()
{ Fnode* node=0;
  Fnode*   fn=0;
  EQNode*  eq=0;
  NENode*  ne=0;
  LENode*  le=0;
  LTNode*  lt=0;
  GENode*  ge=0;
  GTNode*  gt=0;
  int go;

// cout <<"Enter expression\n" <<flush;
  fn=simple_expression();
  node=fn;
  if (10 <=lastitem && lastitem <= 15)
  { go=lastitem;
    get_item();
    fn=simple_expression();
    if (fn != 0)
    switch(go)
    { case 10: 
               eq=new EQNode; eq->arg0=node; eq->arg1=fn; node=eq;
        break;
      case 11: ne=new NENode; ne->arg0=node; ne->arg1=fn; node=ne;
        break;
      case 12: le=new LENode; le->arg0=node; le->arg1=fn; node=le;
        break;
      case 13: lt=new LTNode; lt->arg0=node; lt->arg1=fn; node=lt;
        break;
      case 14: ge=new GENode; ge->arg0=node; ge->arg1=fn; node=ge;
        break;
      case 15: gt=new GTNode; gt->arg0=node; gt->arg1=fn; node=gt;
        break;
    }
  }
  //*debug*/ debugnode(node);
  //*debug*/ cout<<"c:"<<c<<" lastitem:"<<lastitem<<" node: "<<node<<" Exit expression\n"<<flush;
  return node;
} // END expression

static Fnode* assignment(char** resultname)
{ Fnode*   node=0;
  AssNode* ass=0;

  switch(lastitem)
  { case 200:strcpy(*resultname,workstr);
             get_item();
             if (lastitem != 10)
             { parserror(10); break;
             }
             get_item();
             node=expression();
             if (node)
             { ass=new AssNode;
               ass->varname=resultname;
               ass->expres=node;
               node=ass;
             }
      break;
    default: parserror(errbase+11);
     break;
  }
  //*debug*/ cout<<"c:"<<c<<" lastitem:"<<lastitem<<" node: "<<node<<" Exit assignment\n"<<flush;
  //*debug*/ debugnode(node);
  return node;
} //END assignment

// Public Procedures
// ==========================================================================   


int FORMVAR::Parse(const char* fromline, const VARLIST* varlist,
                   char* errorline)
{
  // ad-hoc correction 28.8.98 PR:
  if ( !strcmp(fromline,"-") )
  {
	 errorline[0] = 7;
	 errorline[1] = '\0';
	 return 1;
  }
  // end of ad-hoc correction
  l=fromline;
  arglen=strlen(fromline);
  varLIST=varlist;
  errors=0;
  for (i=0;i<arglen;i++) errorline[i]=0; errorline[i]=0;
  erl=errorline;
  i=0;
  if ( arglen == 0 )
  {
    parserror(errbase+9);
    arglen = 1;
    errorline[1] = '\0';
  }
  else
  {
    c=1; /* reads till c= 0 */ read(); get_item();
    delete root; root=expression();
//*debug*/ cout<<"Stop: ctrl_C ? "; char stop; cin.get(stop); cout<<"\n"<<flush;
//*debug*/ debugnode(root); cout << "EXIT PARSE\n"<<flush;
    if (lastitem != 0)  parserror(errbase+5);
    if (root == 0) parserror(errbase+9);
    if (root != 0 && 0 < errors) { delete root; root=0; }
    if (!errors) PutArgs();
  }
  return errors;
}

int FORMVAR::Parse(char** resultname,
                   const char* fromline, const VARLIST* varlist,
                   char* errorline)
{ l=fromline;
  arglen=strlen(fromline);
  varLIST=varlist;
  errors=0;
  for (i=0;i<arglen;i++) errorline[i]=0; errorline[i]=0;
  erl=errorline;
  i=0;
  if ( arglen == 0 )
  {
    parserror(errbase+9);
    arglen = 1;
    errorline[1] = '\0';
  }
  else
  {
    c=1; /* reads till c= 0 */ read(); get_item();
    delete root; root=assignment(resultname);
//*debug*/ cout<<"Stop: ctrl_C ? "; char stop; cin.get(stop); cout<<"\n"<<flush;
//*debug*/ debugnode(root); cout << "EXIT PARSE\n"<<flush;
    if (lastitem != 0)  parserror(errbase+5);
    if (root == 0) parserror(errbase+9);
    if (root != 0 && 0 < errors) { delete root; root=0; }
    if (!errors) PutArgs();
  }
  return errors;
}

void FORMVAR::GetErrorMarks(char* errorline, char mark, char* markedline)
{ int i;
  for (i=0;i<arglen;i++)
  { if (errorline[i] != 0) markedline[i]=mark; else markedline[i]=' ';
  }
  markedline[i]=0;
}
    
void FORMVAR::GetErrorMsg(short n, char* errmsg)
{ 
 if (errbase < n) strcpy(errmsg,errstr[n-errbase]);
  else
  { strcpy(errmsg,"");
    if (n < 0) { strcat(errmsg,"un"); n=-n; }

    strcat(errmsg,"expected: ");
    strcat(errmsg,SyDict[n]);
  }
}

int FORMVAR::UnParse(char* toline, int maxch) const
{ UNPline=toline;  // global
  UNPcur=0;        // global
  UNPmax=maxch;    // global
  SetUnparse();
  strcpy(toline,"");
  uperrors=0;
  if (root == 0)
  {
    toline[0] = '0';
    toline[1] = '\0';
  }
  else
  {
    root->visitnodes();
  }
  return uperrors;
}


CARDINAL FORMVAR::illegalvalcount = 0;

REAL FORMVAR::Evaluate()
{
   if ( root == 0 ) return 0;
   const CARDINAL maxout = 100;
   REAL val = root->evaluate();
   if ( !aqfinite(val) )
   {
      if ( illegalvalcount <= maxout )
      {
         *cjob << "\n   FORMVAR numerical problem:"
               << " illegal value in variable ";
         *cjob << Symbol();
         cjob->flush();
         if ( illegalvalcount == maxout )
         {
            *cjob << "\n   *** FORMVAR numerical problems are no longer written to the log file ***";
            *cjob << "\n   *** please check the above listed variables for illegal dependences  ***";
            *cjob << "\n   *** (powers of nonpositive numbers, etc.)                            ***";
            cjob->flush();

         }
         illegalvalcount++;
      }
   }
   return val;
}

int FORMVAR::FORMVARSymbol(char* symbol) // returns 0, if symbol is used by FORMVAR
{
  int exists;

  exists=findNo(symbol,SyDict,MaxRe); if (exists) return 0;
  exists=findNo(symbol,CoDict,MaxCo); if (exists) return 0;
  exists=findNo(symbol,F2Dict,MaxF2); if (exists) return 0;
  exists=findNo(symbol,F1Dict,MaxF1); if (exists) return 0;

  return 1;
}


JOBSTATUS FORMVAR::Load(std::istream& in, const VARLIST* varlist)
{
   char Buffer1[BufSize]; char Buffer2[BufSize];

   if ( LoadStart(in) == FALSE )                          return PROBLEM;
   INTEGER aquavers = 0;
   if ( LoadNumber(in,&aquavers) == FALSE )               return PROBLEM;
   switch ( aquavers )
   {
      case 1: // aquavers=1: AQUASIM 1.0
         {
            JOBSTATUS status = AQVAR::Load(in);
                                      if ( status != OK ) return status;
            if ( LoadString(in,Buffer1,sizeof(Buffer1)) == FALSE )
                                                          return PROBLEM;
            if ( Parse(Buffer1,varlist,Buffer2) != 0 )    return PROBLEM;
         }
         break;
      default:
         return PROBLEM;
   }
   if ( LoadEnd(in) == FALSE )                            return PROBLEM;
   return OK;
}


JOBSTATUS FORMVAR::Save(std::ostream& out)
{
   char Buffer[BufSize];

   if ( SaveStart(out) == FALSE )                           return PROBLEM;
   INTEGER aquavers = 1;
   if ( SaveNumber(out,aquavers) == FALSE )                 return PROBLEM;
   JOBSTATUS status = AQVAR::Save(out); if ( status != OK ) return status;
   if ( UnParse(Buffer,sizeof(Buffer)-1) != 0 )             return PROBLEM;
   if ( SaveString(out,Buffer) == FALSE )                   return PROBLEM;

   if ( SaveEnd(out) == FALSE )                             return PROBLEM;
   return FILEIO::Save(out);
}


JOBSTATUS FORMVAR::Write(std::ostream& out, BOOLEAN sh)
{
   JOBSTATUS status = AQVAR::Write(out,sh); 
                                         if ( status != OK ) return status;

   char Buffer[BufSize];
   CARDINAL pos1 = ini.L(1)+1; 
   CARDINAL pos2 = pos1+ini.L(2);
   if ( sh == TRUE )
   {
      PrintIndent(pos1-1);
      if ( UnParse(Buffer,sizeof(Buffer)-1) != 0 )           return PROBLEM;
      if ( Print(out,Buffer,pos1) == FALSE )                 return PROBLEM;
   }
   else
   {
      PrintIndent(pos2-1);
      if ( Print(out,ini.T(3),pos1) == FALSE )               return PROBLEM;
      if ( Print(out,":") == FALSE )                         return PROBLEM;
      if ( UnParse(Buffer,sizeof(Buffer)-1) != 0 )           return PROBLEM;
      if ( Print(out,Buffer,pos2) == FALSE )                 return PROBLEM;
   }

   return OK;
}


void FORMVAR::Test()
{
   *ctest << "\nclass FORMVAR:    not yet implemented";
   AQVAR::Test();
}
