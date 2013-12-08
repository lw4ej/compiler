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

void Program::Emit() {
    /* pp5: here is where the code generation is kicked off.
     *      The general idea is perform a tree traversal of the
     *      entire program, generating instructions as you go.
     *      Each node can have its own way of translating itself,
     *      which makes for a great use of inheritance and
     *      polymorphism in the node classes.
     */
	 codegen = new CodeGenerator();
	 bool HasMain = false;
	 int globV = 0;
	 for(int i = 0; i < decls->NumElements(); i++){
		VarDecl* d = dynamic_cast<VarDecl*>(decls->Nth(i));
		if(d!=NULL){
			d->SetAddr(codegen->GenGlobalVar(d->getkey()));
		}
		else{
	 		FnDecl* fn = dynamic_cast<FnDecl*>(decls->Nth(i));
			if(fn!=NULL && strcmp(fn->getkey(),"main")==0)
				HasMain = true;
			decls->Nth(i)->Emit();
		}
	 }
	 
	 if(HasMain==false){
		ReportError::NoMainFound();
		return;
	 }
	 
	 codegen->DoFinalCodeGen();
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

void StmtBlock::Emit(){

	Assert(parent!=NULL);
	codegen = parent->GetGenerator();
	Assert(codegen!=NULL);
	
	for(int i=0; i<decls->NumElements();i++){
		Decl * d = decls->Nth(i);
		VarDecl * var = dynamic_cast<VarDecl*>(d);
		Assert(var);
		var->SetAddr(codegen->GenLocalVar(var->getkey()));	
	}

	for(int i=0; i<stmts->NumElements();i++){
		stmts->Nth(i)->Emit();
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

void WhileStmt::Emit(){
	Assert(parent!=NULL);
	codegen = parent->GetGenerator();
	Assert(codegen!=NULL);

	char * tplabel1;
	char * tplabel2;

	tplabel1=codegen->NewLabel();
	codegen->GenLabel(tplabel1);
	test->Emit();
	tplabel2=codegen->NewLabel();
	endLabel=tplabel2; 
	codegen->GenIfZ( test->GetAddr(), tplabel2);
	body->Emit();
	codegen->GenGoto(tplabel1);
	codegen->GenLabel(tplabel2);
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

void ForStmt::Emit(){
	Assert(parent!=NULL);
	codegen = parent->GetGenerator();
	Assert(codegen!=NULL);

	char * tplabel1;
	char * tplabel2;
	tplabel1 = codegen->NewLabel();
	tplabel2 = codegen->NewLabel();
	endLabel = tplabel2;
	
	init->Emit();
	codegen->GenLabel(tplabel1);
	test->Emit();
	codegen->GenIfZ( test->GetAddr(), tplabel2);
	body->Emit();
	step->Emit();
	codegen->GenGoto(tplabel1);
	codegen->GenLabel(tplabel2);
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

void IfStmt::Emit(){
	
	Assert(parent!=NULL);
	codegen = parent->GetGenerator();
	Assert(codegen!=NULL);
	char * tplabel1;
	char * tplabel2;

	tplabel1=codegen->NewLabel();
	test->Emit();
	codegen->GenIfZ(test->GetAddr(),tplabel1);
	body->Emit();

	if(elseBody){
		tplabel2 = codegen->NewLabel();
		codegen->GenGoto(tplabel2);
		codegen->GenLabel(tplabel1);
		elseBody->Emit();
		codegen->GenLabel(tplabel2);
	}
	else{
		codegen->GenLabel(tplabel1);
	}
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

void BreakStmt::Emit(){
	Assert(parent!=NULL);
	codegen = parent->GetGenerator();
	Assert(codegen!=NULL);
	Node* p = parent;
	LoopStmt* loop = NULL;
	while(p!=NULL){
		loop = dynamic_cast<LoopStmt*>(p);
		if(loop!= NULL) break;
		p = p->GetParent();
	}
	Assert(loop->endLabel);
	codegen->GenGoto(loop->endLabel);
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

void ReturnStmt::Emit(){
	Assert(parent!=NULL);
	codegen = parent->GetGenerator();
	Assert(codegen!=NULL);
	Location * ret = NULL;
	if(expr!=NULL){
		expr->Emit();
		ret = expr->GetAddr();
	}
	codegen->GenReturn(ret);
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

void PrintStmt::Emit(){
	Assert(parent!=NULL);
	codegen = parent->GetGenerator();
	Assert(codegen!=NULL);
	for(int i=0; i<args->NumElements();i++){
		Type * t = args->Nth(i)->ExprType();
		if(t->IsEquivalentTo(Type::boolType)){
			args->Nth(i)->Emit();
			Location * param = args->Nth(i)->GetAddr();
			codegen->GenBuiltInCall(PrintBool,param,NULL);
		}
		else if(t->IsEquivalentTo(Type::intType)){
			args->Nth(i)->Emit();
			Location * param = args->Nth(i)->GetAddr();
			codegen->GenBuiltInCall(PrintInt,param,NULL);
		}
		else if(t->IsEquivalentTo(Type::stringType)){
			args->Nth(i)->Emit();
			Location * param = args->Nth(i)->GetAddr();
			codegen->GenBuiltInCall(PrintString,param,NULL);
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

