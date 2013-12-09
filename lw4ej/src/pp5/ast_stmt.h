/* File: ast_stmt.h
 * ----------------
 * The Stmt class and its subclasses are used to represent
 * statements in the parse tree.  For each statment in the
 * language (for, if, return, etc.) there is a corresponding
 * node class for that construct. 
 *
 */

#ifndef _H_ast_stmt
#define _H_ast_stmt

#include "list.h"
#include "ast.h"
#include "symboltable.h"

class Decl;
class VarDecl;
class Expr;
  
class Program : public Node
{
  protected:
     List<Decl*> *decls;
  public:
     Program(List<Decl*> *declList);
	 SymbolTable* ConsTable();
	 void Check();
	 void Emit();
};

class Stmt : public Node
{
  public:
     Stmt() : Node() {}
     Stmt(yyltype loc) : Node(loc) {}
};

class StmtBlock : public Stmt 
{
  protected:
    List<VarDecl*> *decls;
    List<Stmt*> *stmts;
  public:
    StmtBlock(List<VarDecl*> *variableDeclarations, List<Stmt*> *statements);
	SymbolTable* ConsTable();
	void Check();
	void Emit();
};

  
class ConditionalStmt : public Stmt
{
  protected:
    Expr *test;
    Stmt *body;
  
  public:
    ConditionalStmt(Expr *testExpr, Stmt *body);
	void Check();
};

class LoopStmt : public ConditionalStmt 
{
  public:
  	char * endLabel;
    LoopStmt(Expr *testExpr, Stmt *body)
            : ConditionalStmt(testExpr, body) {}

};

class ForStmt : public LoopStmt 
{
  protected:
    Expr *init, *step;
  
  public:
    ForStmt(Expr *init, Expr *test, Expr *step, Stmt *body);
	void Check();
	void Emit();
};

class WhileStmt : public LoopStmt 
{
  public:
    WhileStmt(Expr *test, Stmt *body) : LoopStmt(test, body) {}
	void Check();
	void Emit();
};

class IfStmt : public ConditionalStmt 
{
  protected:
    Stmt *elseBody;
  
  public:
    IfStmt(Expr *test, Stmt *thenBody, Stmt *elseBody);
	void Check();
	void Emit();
};

class BreakStmt : public Stmt 
{
  public:
    BreakStmt(yyltype loc) : Stmt(loc) {}
	void Check();
	void Emit();
};

class ReturnStmt : public Stmt  
{
  protected:
    Expr *expr;
  
  public:
    ReturnStmt(yyltype loc, Expr *expr);
	void Check();
	void Emit();
};


class PrintStmt : public Stmt
{
  protected:
    List<Expr*> *args;
    
  public:
    PrintStmt(List<Expr*> *arguments);
	void Check();
	void Emit();
};

class IntConstant;

class Case : public Node
{
  protected:
    IntConstant *value;
    List<Stmt*> *stmts;

  public:
    Case(IntConstant *v, List<Stmt*> *stmts);
	void Check();
};
 
class SwitchStmt : public Stmt
{
  protected:
    Expr *expr;
    List<Case*> *cases;
    
  public:
    SwitchStmt(Expr *e, List<Case*> *cases);
	void Check();
};
		

#endif
