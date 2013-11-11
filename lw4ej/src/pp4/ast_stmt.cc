/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"
#include "errors.h"

Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
}

SymbolTable* Program:: ConsTable(){
	nodeTable = new SymbolTable();
	for(int i =0; i< decls->NumElements(); i++)
		nodeTable->Insert(decls->Nth(i));
	return nodeTable;
}

void Program::Check() {
    /* pp3: here is where the semantic analyzer is kicked off.
     *      The general idea is perform a tree traversal of the
     *      entire program, examining all constructs for compliance
     *      with the semantic rules.  Each node can have its own way of
     *      checking itself, which makes for a great use of inheritance
     *      and polymorphism in the node classes.
     */
	 ConsTable();
	 for(int i =0; i< decls->NumElements(); i++){
		decls->Nth(i)->Check();
	 }
}

StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    (decls=d)->SetParentAll(this);
    (stmts=s)->SetParentAll(this);
}

SymbolTable* StmtBlock::ConsTable() {
	if(nodeTable!=NULL) return nodeTable;
	nodeTable = new SymbolTable;
	for(int i =0; i< decls->NumElements(); i++)
		nodeTable->Insert(decls->Nth(i));
	return nodeTable;
}

void StmtBlock::Check() {
	if(nodeTable == NULL)
		ConsTable();
	for(int i=0; i< decls->NumElements();i++)
		decls->Nth(i)->Check();
	for(int i=0; i< stmts->NumElements();i++){
		stmts->Nth(i)->Check();
	}
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) { 
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this); 
    (body=b)->SetParent(this);
}

void ConditionalStmt::Check() {
	test->Check();
	body->Check();
}


void WhileStmt::Check() {
	test->Check();
	Type * t = test->ExprType();
	if(t != Type::boolType)
		ReportError::TestNotBoolean(test);
	body->Check();
}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) { 
    Assert(i != NULL && t != NULL && s != NULL && b != NULL);
    (init=i)->SetParent(this);
    (step=s)->SetParent(this);
}

void ForStmt::Check() {
	init->Check();
	test->Check();
	Type * t = test->ExprType();
	if(t!=Type::boolType)
		ReportError::TestNotBoolean(test);
	step->Check();
	body->Check();
}

IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) { 
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) { 
		elseBody->SetParent(this);
	}
}

void IfStmt::Check() {
	test->Check();
	Type * t = test->ExprType();
	if(t!=Type::boolType)
		ReportError::TestNotBoolean(test);
	body->Check();
	if(elseBody) 
		elseBody->Check();
}

void BreakStmt::Check(){
	Node* p = parent;
	LoopStmt* loop = NULL;
	while(p!=NULL){
		loop = dynamic_cast<LoopStmt*>(p);
		if(loop!= NULL) break;
		p = p->GetParent();
	}
	if(loop==NULL)
		ReportError::BreakOutsideLoop(this);
}

ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    Assert(e != NULL);
    (expr=e)->SetParent(this);
}

void ReturnStmt::Check(){
	expr->Check();

	Node* p = parent;
	FnDecl* f = NULL;
	while(p!=NULL){
		f = dynamic_cast<FnDecl*>(p);
		if(f!= NULL) break;
		p = p->GetParent();
	}
	if(f){
		Type* ret = f->GetRtype();

		if( dynamic_cast<EmptyExpr*>(expr)){
			if((ret->IsEquivalentTo(Type::voidType))==false)
				ReportError::ReturnMismatch(this, Type::voidType, ret);
		}
		else if((expr->ExprType()->IsCompatTo(ret))==false)
			ReportError::ReturnMismatch(this, expr->ExprType(),ret);
	}
}

PrintStmt::PrintStmt(List<Expr*> *a) {    
    Assert(a != NULL);
    (args=a)->SetParentAll(this);
}

void PrintStmt::Check(){
	for(int i = 0; i< args->NumElements();i++){
		args->Nth(i)->Check();
		Type* t = args->Nth(i)->ExprType();
		if(t->IsEquivalentTo(Type::errorType))
			continue;
		if(!(t->IsEquivalentTo(Type::boolType)
			||t->IsEquivalentTo(Type::intType)
			||t->IsEquivalentTo(Type::stringType))
			){
			ReportError::PrintArgMismatch(args->Nth(i), i+1,t);
		}
	}
}

Case::Case(IntConstant *v, List<Stmt*> *s) {
    Assert(s != NULL);
    value = v;
    if (value) {
		value->SetParent(this);
	}
    (stmts=s)->SetParentAll(this);
}

void Case::Check() {
	for( int i=0; i< stmts->NumElements();i++)
		stmts->Nth(i)->Check();
}

SwitchStmt::SwitchStmt(Expr *e, List<Case*> *c) {
	Assert(e != NULL && c != NULL);
    (expr=e)->SetParent(this);
    (cases=c)->SetParentAll(this);
}

void SwitchStmt::Check() {
	expr->Check();
	for(int i=0; i< cases->NumElements();i++)
		cases->Nth(i)->Check();
}

