/* File: ast_stmt.cc
 * -----------------
 * Implementation of statement node classes.
 */
#include "ast_stmt.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_expr.h"


Program::Program(List<Decl*> *d) {
    Assert(d != NULL);
    (decls=d)->SetParentAll(this);
	SetChildren((List<Node*>*)decls);
	strcpy(nodeName, "Program");
}

//#define DeBuG
#ifdef DeBuG
void visitTable(Node*p)
{
	SymbolTable * a;
	if((a =p->getCT()) != NULL){
		printf("%s ", p->GetNameForNode());
		printf ("%d\n", a->getTable()->NumEntries());
	}
}
#endif
void Program:: Constable(SymbolTable *s){

	for(int i =0; i< decls->NumElements(); i++){
		decls->Nth(i)->Constable(s);
	}
}

void Program::Check() {
    /* pp3: here is where the semantic analyzer is kicked off.
     *      The general idea is perform a tree traversal of the
     *      entire program, examining all constructs for compliance
     *      with the semantic rules.  Each node can have its own way of
     *      checking itself, which makes for a great use of inheritance
     *      and polymorphism in the node classes.
     */
	 cur = new SymbolTable;
	 Constable(this->getCT());
#ifdef DeBuG
	 AstWalk(this, visitTable);
#endif
	 cur->SetActScope(new ActScope);
	 cur->EnterScope();
	 for(int i =0; i< decls->NumElements(); i++){
		 Assert(decls->Nth(i)->getCT() && decls->Nth(i)->getCT()->GetActScope());
		decls->Nth(i)->Check();
	 }
	 cur->ExitScope();
}

StmtBlock::StmtBlock(List<VarDecl*> *d, List<Stmt*> *s) {
    Assert(d != NULL && s != NULL);
    (decls=d)->SetParentAll(this);
	SetChildren((List<Node*>*)decls);
    (stmts=s)->SetParentAll(this);
	SetChildren((List<Node*>*)stmts);
	strcpy(nodeName, "StmtBlock");
}

void StmtBlock::Constable(SymbolTable* s) {
	cur = s;
	nest = new SymbolTable;
	for(int i =0; i< decls->NumElements(); i++){
		decls->Nth(i)->Constable(nest);
	}
	for(int i=0; i< stmts->NumElements();i++){
		stmts->Nth(i)->Constable(nest);
	}
}

void StmtBlock::Check() {
	Assert(cur!=NULL&&nest!=NULL);
	nest->SetActScope(cur->GetActScope());
	nest->EnterScope();
	for(int i=0; i< decls->NumElements();i++)
		decls->Nth(i)->Check();
	for(int i=0; i< stmts->NumElements();i++)
		stmts->Nth(i)->Check();
	nest->ExitScope();
}

ConditionalStmt::ConditionalStmt(Expr *t, Stmt *b) { 
    Assert(t != NULL && b != NULL);
    (test=t)->SetParent(this); 
	SetChild(test);
    (body=b)->SetParent(this);
	SetChild(body);
	strcpy(nodeName, "ConditionalStmt");
}

void ConditionalStmt::Constable(SymbolTable* s) {
	cur = s;
	nest = new SymbolTable;
	body->Constable(nest);
}

void ConditionalStmt::Check() {
	Assert(cur!=NULL&&nest!=NULL);
	nest->SetActScope(cur->GetActScope());
	nest->EnterScope();
	body->Check();
	nest->ExitScope();
}

void WhileStmt::Constable(SymbolTable* s) {
	cur = s;
	nest = new SymbolTable;
	body->Constable(nest);
}

void WhileStmt::Check() {
	Assert(cur!=NULL&&nest!=NULL);
	nest->SetActScope(cur->GetActScope());
	nest->EnterScope();
	body->Check();
	nest->ExitScope();
}

ForStmt::ForStmt(Expr *i, Expr *t, Expr *s, Stmt *b): LoopStmt(t, b) { 
    Assert(i != NULL && t != NULL && s != NULL && b != NULL);
    (init=i)->SetParent(this);
	SetChild(init);
    (step=s)->SetParent(this);
	SetChild(step);
	strcpy(nodeName, "ForStmt");
}
void ForStmt::Constable(SymbolTable* s) {
	cur = s;
	nest = new SymbolTable;
	body->Constable(nest);
}

void ForStmt::Check() {
	Assert(cur!=NULL&&nest!=NULL);
	nest->SetActScope(cur->GetActScope());
	nest->EnterScope();
	body->Check();
	nest->ExitScope();
}

IfStmt::IfStmt(Expr *t, Stmt *tb, Stmt *eb): ConditionalStmt(t, tb) { 
    Assert(t != NULL && tb != NULL); // else can be NULL
    elseBody = eb;
    if (elseBody) { 
		elseBody->SetParent(this);
		SetChild(elseBody);
	}
	strcpy(nodeName, "IfStmt");
}

void IfStmt::Constable(SymbolTable* s) {
	cur = s;
	nest = new SymbolTable;
	body->Constable(nest);
	if(elseBody)
		elseBody->Constable(nest);
}

void IfStmt::Check() {
	Assert(cur!=NULL&&nest!=NULL);
	nest->SetActScope(cur->GetActScope());
	nest->EnterScope();
	body->Check();
	if(elseBody) elseBody->Check();
	nest->ExitScope();
}

ReturnStmt::ReturnStmt(yyltype loc, Expr *e) : Stmt(loc) { 
    Assert(e != NULL);
    (expr=e)->SetParent(this);
	SetChild(expr);
	strcpy(nodeName, "ReturnStmt");
}
  
PrintStmt::PrintStmt(List<Expr*> *a) {    
    Assert(a != NULL);
    (args=a)->SetParentAll(this);
	SetChildren((List<Node*>*)args);
	strcpy(nodeName,"PrintStmt");
}

Case::Case(IntConstant *v, List<Stmt*> *s) {
    Assert(s != NULL);
    value = v;
    if (value) {
		value->SetParent(this);
		SetChild(value);
	}
    (stmts=s)->SetParentAll(this);
	SetChildren((List<Node*>*)stmts);
	strcpy(nodeName, "Case");
}

void Case::Constable(SymbolTable* s) {
	cur = s;
	nest = new SymbolTable;
	for(int i =0; i< stmts->NumElements(); i++){
		stmts->Nth(i)->Constable(nest);
	}
}

void Case::Check() {
	Assert(cur!=NULL&&nest!=NULL);
	nest->SetActScope(cur->GetActScope());
	nest->EnterScope();
	for( int i=0; i< stmts->NumElements();i++)
		stmts->Nth(i)->Check();
	nest->ExitScope();
}

SwitchStmt::SwitchStmt(Expr *e, List<Case*> *c) {
	Assert(e != NULL && c != NULL);
    (expr=e)->SetParent(this);
	SetChild(expr);
    (cases=c)->SetParentAll(this);
	SetChildren((List<Node*>*)cases);
	strcpy(nodeName, "SwitchStmt");
}

void SwitchStmt::Constable(SymbolTable* s) {
	cur = s;
	nest = new SymbolTable;
	for(int i =0; i< cases->NumElements(); i++){
		cases->Nth(i)->Constable(nest);
	}
}

void SwitchStmt::Check() {
	Assert(cur!=NULL&&nest!=NULL);
	nest->SetActScope(cur->GetActScope());
	nest->EnterScope();
	for(int i=0; i< cases->NumElements();i++)
		cases->Nth(i)->Check();
	nest->ExitScope();
}

