///////////////////////////////   formvar.h   ////////////////////////////////
//
//              date:       person:           comments:
//
// creation:    20.06.91    J. Ruchti         Parser, without Variables
// revisions:   23.07.91    P. Reichert       Idea of Embedding Parser
//                                                into FORMVAR
//              20.08.91    P. Reichert       static declarations of globals
//                                                Load, Save and Write added.
//              27.11.91    J. Ruchti         ?VAR::?VAR(const ?VAR&)
//                                                constructor added
//              17.05.04    Peter Reichert    converted to ANSI C++
//
//////////////////////////////////////////////////////////////////////////////
//
// Class hierarchy
// ===============
//
//   NODE
//     |
//     +------+
//            |
//   FILEIO   |
//     |      |
//     +------+----SYMBOL
//                   |
//                   +----AQVAR
//                          |
//                          +-----FORMVAR
//
//////////////////////////////////////////////////////////////////////////////
//
// Classes
// =======
//
// FORMVAR:     The methods in this class parse strings containing algebraic
// --------     formula into a formula tree. This tree can be evaluated and
//              unparsed.
//              The syntax of formula and the methods are given below.
// 
// Fnode:       The methods of a node in a formula tree.
// ------       Classes derived from Fnode are introduced in formvar.C
//
//////////////////////////////////////////////////////////////////////////////
//
// Formula Syntax
// --------------
// <assignment>         ::= <varident> = <expression>.
// <varident>           ::= <letter> { <letter_or_digit> }.
// <letter>             ::= A | ... | Z | a | ... | z.
// <letter_or_digit>    ::= <letter> | <digit>.
// <digit>              ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9.
//
// <expression>         ::= <simple_expression> |
//                          <simple_expression> <relop> <simple_expression>.
// <relop>              ::= = | # | <> | <= | < | >= | >.
// <simple_expression>  ::= <term> | <sign> <term> |
//                          <simple_expression> <addop> <term>.
// <term>               ::= <factor> | <term> <mulop> <factor>.
// <sign>               ::= + | -.
// <addop>              ::= + | - | or.
// <factor>             ::= <varident> | <unsigned_constant> | 
//                          ( <expression> ) | <function> | not <factor>.
// <unsigned_constant>  ::= <unsigned_number> | <predefined_constant>.
// <unsigned_number>    ::= <unsigned_integer>
//                          [.<unsigned_integer> [E [<sign>]
//                                                       <unsigned_integer>]].
// <predefined_constant>::= pi | true | false | precision.
// <unsigned_integer>   ::= <digit> { <digit> }.
// <mulop>              ::= ^ | * | / | div | mod | and.
// <function>           ::= <arith_func_1_arg> | <arith_func_2_arg> |
//                          <if_function>.
// <arith_func_1_arg>   ::= <ident_func_1_arg> ( <expression> ).
// <ident_func_1_arg>   ::= sin | cos | tan |
//                          asin | acos | atan |
//                          deg  | rad  |
//                          exp  | log  | ln   | log10 |
//                          sign | abs  | sqrt |
//                          settrue     |
//                          setprecision.
// <arith_func_2_arg>   ::= <ident_func_2_arg> ( <expression> , <expression> )
// <ident_func_2_arg>   ::= min | max.
// <if_function>        ::= if <condition> then <expression> else <expression>
//                          endif.
// <condition>          ::= <expression>.
//
//----------------------------------------------------------------------------
//
// Semantics: Hints
// ----------------
//
// Operator Precedence  level 1:        ()   not
//                      level 2:        ^
//                      level 3:        *    /    div  mod  and
//                      level 4:        +    -    or
//                      level 5:        =    <>   <=   <    >=    >
//                      level 6:        = 
//
// <relop>              UnParse translates '#' to '<>'.
//
// <factor>,<addop>,    not, and, or, and the relational operators
// <mulop> ,<relop>     return real values for true or false (see below).
//                      not false returns true,
//                      if (x != false) then not x returns false.
//
// <predefined_constant>  pi   = 3.141....
//                        true = 1 [default]; may be redefined using
//                                 function settrue.
//                                      Programmer may use void SetTrue.
//                        false= 0 [default]; may be redefined using
//                                 functions settrue.
//                                      Programmer may use void SetTrue.
//                        precision = 6 [default]: precision of numbers used
//                                      by UnParse;
//                                      may be redefined using function
//                                      setprecision.
//                                      Programmer may use void SetPrecision.
//
// <ident_func_1_arg>  log = ln = natural logarithm.
//
//                     Special function settrue always returns 1.0; side
//                     effect: settrue(real) defines the value of true;
//                     and false is defined as a function of true:
//                     if (true == 0) false=1; else false=0.
//
//                     Special function setprecision always returns 1.0; side
//                     effect: setprecision defines the precision with which
//                     setprecision defines the precision with which UnParse
//                     UnParse represents numbers.
//
// <if_function>       if condition != false,
//                        then the value of the 'then' expression is returned,
//                        else the value of the 'else' expression is returned.
//
// <condition>         any expression in if_function:
//                     if_function compares it's value with false.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef FORMVAR_H
#define FORMVAR_H

#include "all.h"
#include "aqvar.h"

//////////////////////////////////////////////////////////////////////////////

class Fnode            // virtual base class for handling nodes in parse tree
{
  public:
  
    Fnode()   {}
    virtual ~Fnode() {}

    virtual REAL evaluate() { return 0.0; }
    virtual Fnode* copy() { return 0; }
    virtual void visitnodes()=0;
    
};

//////////////////////////////////////////////////////////////////////////////

class FORMVAR : public AQVAR
{
    friend class VARSYS;

  public:
    
    FORMVAR();
    FORMVAR(const FORMVAR& var);
    FORMVAR(AQVAR* var);
    ~FORMVAR();

    AQVARTYPE          Type() const
                          { return FormVar; }
    const char*        TypeName() const;

    BOOLEAN            AllowedVar();

    CARDINAL           NumVarArg() const
                          { return numvararg; }
    AQVAR*             VarArg(CARDINAL i) const;

    static BOOLEAN ReservedSymbol(const char* s);
                                     // returns TRUE, if symbol s reserved by
                                     // FORMVAR
                                     // returns FALSE, if s not reserved by 
                                     // FORMVAR

    static void ResetIllegalValCount()
       { illegalvalcount = 0; }

    int Parse(const char* from_line, // Do parse Expression in from_line,
              const VARLIST* varlist,// accept only variables in varlist.
              char* errorline);      // String errorline should be at least as
                                     // long as from_line; on return it 
                                     // contains zero's or error numbers
                                     // which may be evaluated using
                                     // GetErrorMarks and/or GetErrorMsg.
                                     // Defines, if(!errors), (AQVAR)numarg,
                                     // (AQVAR**)arg.
                                     // Pars returns number of errors

    int Parse(char** resultname,     // calling program specifies resultname 
                                     // as a pointer to pointer to a
                                     // (temporary) array of char buffer.
                                     // Parse will write the left-hand 
                                     // Variable identifier of an
                                     // Assignment to this buffer.
                                     // UnParsing an Assignment will use 
                                     // *resultname.
              const char* from_line, // Do parse Assignment in from_line,
              const VARLIST* varlist,// accept only variables in varlist.
              char* errorline);      // String errorline should be at least as
                                     // long as from_line; on return it 
                                     // contains zero's or error numbers
                                     // which may be evaluated
                                     // using GetErrorMarks and/or 
                                     // GetErrorMsg.
                                     // Defines, if(!errors), (AQVAR)numarg, 
                                     // (AQVAR**)arg.
                                     // Pars returns number of errors

    void SetTrue(REAL ZeroOrReal);   // System default :       true=1
                                     //                        false=0.
                                     // After calling SetTrue:
                                     // if ZeroOrReal = 0 then true=0
                                     //                        false=1
                                     // else                   true=ZeroOrReal
                                     //                        false=0.

    static void GetErrorMarks(char* errorline,
              char mark,             // For each error in errorline a mark 
              char* markedline);     // character is written into markedline. 
                                     // Buffer markedline should be at least
                                     // should be at least as long as 
                                     // errorline.
    
    static void GetErrorMsg(short i, // writes "error message" for error i
                   char* errmsg);    // into user string errmsg; programmer 
                                     // should obtain i from errorline[k]
                                     // and call GetErrorMsg only if
                                     // errorline[k] != 0.
    
    int UnParse(char* to_line,       // UnParse parsed expression to_line,
                int maxch) const;    // to_line holds only maxch characters;
                                     // return 0 or number of problems/errors

    void SetPrecision(int to);       // Set precision of numbers in unparsing
    
    REAL Evaluate();                 // Evaluate parsed expression
    
    void Delete() { delete root; root=0; }
                                     // Delete parsed expression

    int FORMVARSymbol(char* symbol); // returns 0, if symbol is used by 
                                     // FORMVAR

	JOBSTATUS Load(std::istream& in, const VARLIST* varlist); // realization of
    JOBSTATUS Save(std::ostream& out);                        // virtual functions
    JOBSTATUS Write(std::ostream& out, BOOLEAN sh=FALSE);     // of FILEIO

    void    Test();

  protected:
    
    Fnode*              root;        // root of expression parsed by Parse,
                                     // to be unparsed by UnParse(),
                                     // to be evaluated by Evaluate()
                                     // to be deleted by Delete()


    void                ReplVarArg(AQVAR* oldvar, AQVAR* newvar);

  private:

    static CARDINAL     illegalvalcount;
    CARDINAL            numvararg;
    AQVAR**             vararg;
    void                PutArgs();
    void                init();
};

//////////////////////////////////////////////////////////////////////////////

#endif

//////////////////////////////////////////////////////////////////////////////
