/* File: ast_expr.h
 * ----------------
 * The Expr class and its subclasses are used to represent
 * expressions in the parse tree.  For each expression in the
 * language (add, call, New, etc.) there is a corresponding
 * node class for that construct. 
 *
 * pp3: You will need to extend the Expr classes to implement 
 * semantic analysis for rules pertaining to expressions.
 */


#ifndef _H_ast_expr
#define _H_ast_expr

#include "ast.h"
#include "ast_stmt.h"
#include "list.h"
#include "ast_type.h"
#include "ast_decl.h"

//class NamedType; // for new
//class Type; // for NewArray

class Expr : public Stmt 
{
  protected:
  	Type * Etype;
	Location* MemAddr;

  public:
    Expr(yyltype loc) : Stmt(loc) {Etype = NULL; MemAddr = NULL;}
    Expr() : Stmt() {Etype = NULL; MemAddr = NULL;}
	ClassDecl* FindClass();
	virtual Type* ExprType(){return NULL;}
	Location *GetAddr(){return MemAddr;}
};

/* This node type is used for those places where an expression is optional.
 * We could use a NULL pointer, but then it adds a lot of checking for
 * NULL. By using a valid, but no-op, node, we save that trouble */
class EmptyExpr : public Expr
{
  public:
};

class IntConstant : public Expr 
{
  protected:
    int value;
  
  public:
    IntConstant(yyltype loc, int val);
	Type* ExprType(){Etype = Type::intType; return Type::intType;}
	void Emit();
};

class DoubleConstant : public Expr 
{
  protected:
    double value;
    
  public:
    DoubleConstant(yyltype loc, double val);
	Type* ExprType(){Etype = Type::doubleType; return Type::doubleType;}
	void Emit();
};

class BoolConstant : public Expr 
{
  protected:
    bool value;

  public:
    BoolConstant(yyltype loc, bool val);
	Type* ExprType(){Etype = Type::boolType; return Type::boolType;}
	void Emit();
};

class StringConstant : public Expr 
{ 
  protected:
    char *value;
    
  public:
    StringConstant(yyltype loc, const char *val);
	Type* ExprType(){Etype = Type::stringType; return Type::stringType;}
	void Emit();
};

class NullConstant: public Expr 
{
  public: 
    NullConstant(yyltype loc) : Expr(loc) {}
	Type* ExprType(){Etype = Type::nullType; return Type::nullType;}
	void Emit();
};

class Operator : public Node 
{
  protected:
    char tokenString[4];
    
  public:
    Operator(yyltype loc, const char *tok);
    friend std::ostream& operator<<(std::ostream& out, Operator *o) { return out << o->tokenString; }
 	char* op(){return tokenString;}
 };
 
class CompoundExpr : public Expr
{
  protected:
    Operator *op;
    Expr *left, *right; // left will be NULL if unary
    
  public:
    CompoundExpr(Expr *lhs, Operator *op, Expr *rhs); // for binary
    CompoundExpr(Operator *op, Expr *rhs);             // for unary
};

class ArithmeticExpr : public CompoundExpr 
{
  public:
    ArithmeticExpr(Expr *lhs, Operator *op, Expr *rhs) : CompoundExpr(lhs,op,rhs) {}
    ArithmeticExpr(Operator *op, Expr *rhs) : CompoundExpr(op,rhs) {}
	Type* ExprType();
	void Check();
	void Emit();
};

class RelationalExpr : public CompoundExpr 
{
  public:
    RelationalExpr(Expr *lhs, Operator *op, Expr *rhs) : CompoundExpr(lhs,op,rhs) {}
	Type* ExprType();
	void Check();
	void Emit();
};

class EqualityExpr : public CompoundExpr 
{
  public:
    EqualityExpr(Expr *lhs, Operator *op, Expr *rhs) : CompoundExpr(lhs,op,rhs) {}
    const char *GetPrintNameForNode() { return "EqualityExpr"; }
	Type* ExprType();
	void Check();
	void Emit();
};

class LogicalExpr : public CompoundExpr 
{
  public:
    LogicalExpr(Expr *lhs, Operator *op, Expr *rhs) : CompoundExpr(lhs,op,rhs) {}
    LogicalExpr(Operator *op, Expr *rhs) : CompoundExpr(op,rhs) {}
    const char *GetPrintNameForNode() { return "LogicalExpr"; }
	Type* ExprType();
	void Check();
	void Emit();
};

class AssignExpr : public CompoundExpr 
{
  public:
    AssignExpr(Expr *lhs, Operator *op, Expr *rhs) : CompoundExpr(lhs,op,rhs) {}
    const char *GetPrintNameForNode() { return "AssignExpr"; }
	Type* ExprType();
	void Check();
	void Emit();
	void FieldAssignment();
	void ArrayAssignment();
};

class LValue : public Expr 
{
  public:
    LValue(yyltype loc) : Expr(loc) {}
};

class This : public Expr 
{
  public:
    This(yyltype loc) : Expr(loc) {}
	Type* ExprType();
	void Check();
	void Emit();
};

class ArrayAccess : public LValue 
{
  protected:
    Expr *base, *subscript;
    
  public:
    ArrayAccess(yyltype loc, Expr *base, Expr *subscript);
	Type* ExprType();
	void Check();
	void Emit();
	Location * TargetElem();
};

/* Note that field access is used both for qualified names
 * base.field and just field without qualification. We don't
 * know for sure whether there is an implicit "this." in
 * front until later on, so we use one node type for either
 * and sort it out later. */
class FieldAccess : public LValue 
{
//  protected:
public:
    Expr *base;	// will be NULL if no explicit base
    Identifier *field;
    
//  public:
    FieldAccess(Expr *base, Identifier *field); //ok to pass NULL base
	Type* SearchGlobalId();
	Type* ExprType();
	void Check();
	void Emit();
};

/* Like field access, call is used both for qualified base.field()
 * and unqualified field().  We won't figure out until later
 * whether we need implicit "this." so we use one node type for either
 * and sort it out later. */
class Call : public Expr 
{
  protected:
    Expr *base;	// will be NULL if no explicit base
    Identifier *field;
    List<Expr*> *actuals;
    
  public:
    Call(yyltype loc, Expr *base, Identifier *field, List<Expr*> *args);
	Type* ExprType();
	Type* SearchGlobalFn();
	void Check();
	bool CompareArguments(List<VarDecl*> *f);
	void Emit();
	void Emit_Actuals();
	void Push_Actuals();
};

class NewExpr : public Expr
{
  protected:
    NamedType *cType;
    
  public:
    NewExpr(yyltype loc, NamedType *clsType);
	Type* ExprType();
	void Check();
	void Emit();
};

class NewArrayExpr : public Expr
{
  protected:
    Expr *size;
    Type *elemType;
    
  public:
    NewArrayExpr(yyltype loc, Expr *sizeExpr, Type *elemType);
	Type* ExprType();
	void Check();
	void Emit();
};

class ReadIntegerExpr : public Expr
{
  public:
    ReadIntegerExpr(yyltype loc) : Expr(loc) {}
	Type* ExprType(){Etype=Type::intType;return Etype;}
	void Emit();
};

class ReadLineExpr : public Expr
{
  public:
    ReadLineExpr(yyltype loc) : Expr (loc) {}
	Type* ExprType(){Etype=Type::stringType;return Etype;}
	void Emit();
};

class PostfixExpr : public Expr
{
  protected:
	LValue *lvalue;
	Operator *op;
  public: 
	PostfixExpr(LValue *lv, Operator *op);
	Type* ExprType();
	void Check();
	void Emit();
	void FieldAssignment(Location* );
	void ArrayAssignment(Location* );
};
    
#endif
