/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include "ast_stmt.h"
#include <string.h>
#include "errors.h"

IntConstant::IntConstant(yyltype loc, int val) : Expr(loc) {
    value = val;
}

DoubleConstant::DoubleConstant(yyltype loc, double val) : Expr(loc) {
    value = val;
}

BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    value = val;
}

StringConstant::StringConstant(yyltype loc, const char *val) : Expr(loc) {
    Assert(val != NULL);
    value = strdup(val);
}

Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
}

CompoundExpr::CompoundExpr(Expr *l, Operator *o, Expr *r) 
  : Expr(Join(l->GetLocation(), r->GetLocation())) {
    Assert(l != NULL && o != NULL && r != NULL);
    (op=o)->SetParent(this);
    (left=l)->SetParent(this); 
    (right=r)->SetParent(this);
}

CompoundExpr::CompoundExpr(Operator *o, Expr *r) 
  : Expr(Join(o->GetLocation(), r->GetLocation())) {
    Assert(o != NULL && r != NULL);
    left = NULL; 
    (op=o)->SetParent(this);
    (right=r)->SetParent(this);
}

Type* AssignExpr::ExprType(){
	if( Etype != NULL )
		return Etype;
	
	if(dynamic_cast<LValue*>(left) == NULL
		||left->ExprType()==Type::errorType
		||right->ExprType()==Type::errorType){
		Etype=Type::errorType;
		return Etype;
	}

	if(right->ExprType()->IsCompatTo(left->ExprType())){
		Etype = left->ExprType();
		return Etype;
	}

	ReportError:: IncompatibleOperands(op,left->ExprType(),right->ExprType());
	Etype = Type::errorType;
	return Etype;
}

void AssignExpr::Check(){
	left->Check();
	right->Check();
	ExprType();
}

Type* ArithmeticExpr::ExprType(){
	
	if( Etype != NULL )
		return Etype;

	if(left == NULL){//unary
		
		Type * t = right->ExprType();
		
		if(right->ExprType()==Type::errorType){
			Etype = Type::errorType;
			return Etype;
		}

		if(t->IsArithType()){
			Etype = t;
			return Etype;
		}else{
			ReportError::IncompatibleOperand(op,t);
			Etype = Type::errorType;
			return Etype;
		}
	}
	
	//binary
	if(left->ExprType()==Type::errorType
		||right->ExprType()==Type::errorType){
		Etype = Type::errorType;
		return Etype;
	}

	if(left->ExprType()->IsArithType() 
		&& left->ExprType()->IsEquivalentTo(right->ExprType())){
		Etype = left->ExprType();
		return Etype;
	}

	ReportError::IncompatibleOperands(op, left->ExprType(), right->ExprType());
	Etype = Type::errorType;
	return Etype;
}

void ArithmeticExpr::Check(){
	if(left) left->Check();
	right->Check();
	ExprType();
}

Type* RelationalExpr::ExprType(){
	if( Etype != NULL )
		return Etype;
	
	if((left->ExprType()==Type::errorType) 
		||right->ExprType()==Type::errorType){
		Etype = Type::boolType;
		return Etype;
	}
	
	if(	left->ExprType()->IsArithType()
		&& left->ExprType()->IsEquivalentTo(right->ExprType())){
		Etype = Type::boolType;
		return Etype;
	}

	ReportError::IncompatibleOperands(op, left->ExprType(),right->ExprType());
	Etype = Type::boolType;
	return Etype;
}

void RelationalExpr::Check(){
	left->Check();
	right->Check();
	ExprType();
}

Type* EqualityExpr::ExprType(){
	if( Etype != NULL )
		return Etype;
	
	if((left->ExprType()==Type::errorType) 
		||right->ExprType()==Type::errorType){
		Etype = Type::boolType;
		return Etype;
	}

//equal+ null
/*
	if(left->ExprType()->IsEquivalentTo(right->ExprType())){
		Etype = Type::boolType;
		return Etype;
	}
	if((left->ExprType()==Type::nullType && right->ExprType()==Type::stringType)
		||(left->ExprType()==Type::nullType && dynamic_cast<NamedType*>(right->ExprType()))
		){
		Etype = Type::boolType;
		return Etype;
	}
*/
	if(left->ExprType()->IsCompatTo(right->ExprType())
		||right->ExprType()->IsCompatTo(left->ExprType())){
		Etype = Type::boolType;
		return Etype;
	}

	ReportError::IncompatibleOperands(op, left->ExprType(), right->ExprType());
	Etype = Type::boolType;
	return Etype;
}

void EqualityExpr::Check(){
	left->Check();
	right->Check();
	ExprType();
}

Type* LogicalExpr::ExprType(){
	if( Etype != NULL )
		return Etype;
	
	if(( left && left->ExprType()==Type::errorType) 
		||right->ExprType()==Type::errorType){
		Etype = Type::boolType;
		return Etype;
	}

	if(left == NULL){
		if(right->ExprType()->IsEquivalentTo(Type::boolType)){
			Etype = Type::boolType;
			return Etype;
		}
		ReportError::IncompatibleOperand(op,right->ExprType());
		Etype = Type::boolType;
		return Etype;
	}

	if(left->ExprType()->IsEquivalentTo(Type::boolType)
		&& left->ExprType()->IsEquivalentTo(right->ExprType())){
		Etype = Type::boolType;
		return Etype;
	}
		
	ReportError::IncompatibleOperands(op, left->ExprType(), right->ExprType());
	Etype = Type::boolType;
	return Type::boolType;
}

void LogicalExpr::Check(){
	if(left) left->Check();
	right->Check();
	ExprType();
}

ArrayAccess::ArrayAccess(yyltype loc, Expr *b, Expr *s) : LValue(loc) {
    (base=b)->SetParent(this); 
    (subscript=s)->SetParent(this);
}

Type* ArrayAccess::ExprType(){
	if(Etype!=NULL)
		return Etype;

	if(subscript->ExprType()!=Type::intType){
		Etype = Type::errorType;
		ReportError::SubscriptNotInteger(subscript);
	}
	
	Type* t = base->ExprType();
	ArrayType* array= dynamic_cast<ArrayType*>(t);

	if(array==NULL){
		Etype = Type::errorType;
		ReportError::BracketsOnNonArray(base);
	}
	else Etype = array->GetEleType();

	return Etype;
}

void ArrayAccess::Check(){
	base->Check();
	subscript->Check();
	ExprType();
}

void This::Check(){
	ExprType();
}

Type* This::ExprType(){

	if(Etype!=NULL)
		return Etype;
	
	ClassDecl *p = FindClass();

	if(p){
		Assert(dynamic_cast<ClassDecl*>(p)!=NULL);
		Etype = p->getself();
		return Etype;
	}
	else{
		Etype = Type::errorType;
		ReportError::ThisOutsideClassScope(this);
	}

	return Etype;
}

ClassDecl* Expr:: FindClass(){
	Node* p = parent;
	while(p!=NULL){
		ClassDecl* a = dynamic_cast<ClassDecl*>(p);
		if(a) return a;
		else p = p->GetParent();
	}
	return NULL;
}

FieldAccess::FieldAccess(Expr *b, Identifier *f) 
  : LValue(b? Join(b->GetLocation(), f->GetLocation()) : *f->GetLocation()) {
    Assert(f != NULL); // b can be be NULL (just means no explicit base)
    base = b; 
    if (base) {
		base->SetParent(this);
	}
    (field=f)->SetParent(this);
}

Type* FieldAccess::SearchGlobalId(){
	
	Decl* ret = Lookup(field, false);
	VarDecl* var = NULL;

	var = dynamic_cast<VarDecl*>(ret);

	if(var == NULL){
		ReportError::IdentifierNotDeclared(field,LookingForVariable);
		return Type::errorType;
	}
	else 
		return var->GetType();
}


Type* FieldAccess::ExprType(){
	if(Etype != NULL)
		return Etype;

	if(base == NULL){
		Etype = SearchGlobalId();
		return Etype;
	}
	
	Type *b = base->ExprType();

	if(b==Type::errorType){
		Etype = Type::errorType;
		return Etype;
	}
	
	NamedType* nb = dynamic_cast<NamedType*>(b);

	if(nb && nb->IsClassType()){

		Decl *ty = parent->Lookup(nb->getid(),false);
		ClassDecl *tyclass = dynamic_cast<ClassDecl*>(ty);
		VarDecl* var = NULL;

		if(tyclass!=NULL && (var =dynamic_cast<VarDecl*>(tyclass->Lookup(field, true)))!= NULL){
			Etype = var->GetType();
			if(dynamic_cast<This*>(base)) return Etype;
			else {
				ClassDecl *t = FindClass();
				if(t!=NULL&& dynamic_cast<ClassDecl*>(t) && t->getself()->IsCompatTo(tyclass->getself()))
					return Etype;
			}
			ReportError::InaccessibleField(field, b);
			return Etype;
		}
	}

	if(nb==NULL || (nb && nb->geterror()==false))
		ReportError::FieldNotFoundInBase(field,b);
	
	Etype = Type::errorType;
	return Etype;
}

void FieldAccess::Check(){
	if(base!=NULL)
		base->Check();
	ExprType();
}

Call::Call(yyltype loc, Expr *b, Identifier *f, List<Expr*> *a) : Expr(loc)  {
    Assert(f != NULL && a != NULL); // b can be be NULL (just means no explicit base)
    base = b;
    if (base) {
		base->SetParent(this);
	}
    (field=f)->SetParent(this);
    (actuals=a)->SetParentAll(this);
}

void Call::Check(){
	if(base)
		base->Check();
	for(int i=0; i< actuals->NumElements(); i++)
		actuals->Nth(i)->Check();
	ExprType();
}


Type* Call::SearchGlobalFn(){

	Decl *ret  = Lookup(field, false);
	FnDecl *fn = dynamic_cast<FnDecl*>(ret);
	if(fn == NULL){
		ReportError::IdentifierNotDeclared(field,LookingForFunction);
		Etype = Type::errorType;
		return Etype;
	}	
	CompareArguments(fn->GetFormals());
	return fn->GetRtype();
}

Type * Call::ExprType(){
	if(Etype !=NULL)
		return Etype;
	if(base == NULL){
		Etype=SearchGlobalFn();
		return Etype;
	}
	
	Type* b = base->ExprType();

	//check array.length
	ArrayType * arr = dynamic_cast<ArrayType*>(b);

	if(arr !=NULL 
		&& (actuals->NumElements())==0
		&& !strcmp(field->getname(),"length")){
		
		Etype = Type::intType;
		return Etype;
	}	

	NamedType* nb = dynamic_cast<NamedType*>(b);

	if(nb && (nb->IsClassType() || nb->IsIntfType())){
		Decl *ty = parent->Lookup(nb->getid(),false);
		ClassDecl *tyclass = dynamic_cast<ClassDecl*>(ty);
		InterfaceDecl* tyintf = dynamic_cast<InterfaceDecl*>(ty);
		FnDecl* fn = NULL;
		if((tyclass&&(fn=dynamic_cast<FnDecl*>(tyclass->Lookup(field, true)))!= NULL)||
			(tyintf&&(fn=dynamic_cast<FnDecl*>(tyintf->Lookup(field, true)))!= NULL)
			){
			Etype = fn->GetRtype();
			CompareArguments(fn->GetFormals());
			return Etype;
		}
	}

	if(nb==NULL || (nb && nb->geterror()==false)){
		ReportError::FieldNotFoundInBase(field,b);
	}
	
	Etype = Type::errorType;
	return Etype;
}

bool Call::CompareArguments(List<VarDecl*> *formals)
{
	bool ret=true;
	int numExpected=formals->NumElements();
	int numGiven= actuals->NumElements();
	if(numExpected!=numGiven){
		ReportError::NumArgsMismatch(field,numExpected,numGiven);
		ret=false;
	}
	int len = 0;

	if(numExpected>numGiven) len = numGiven;
	else len = numExpected;

	for(int i = 0; i<len; i++){
		Type* a = actuals->Nth(i)->ExprType();
		Type* r = formals->Nth(i)->GetType();
		if(!(a->IsCompatTo(r))){
			ReportError::ArgMismatch(actuals->Nth(i),i+1,a,r);
			ret=false;
		}
	}
	return ret;
}

NewExpr::NewExpr(yyltype loc, NamedType *c) : Expr(loc) { 
  Assert(c != NULL);
  (cType=c)->SetParent(this);
}

Type* NewExpr::ExprType(){
	if(Etype!=NULL)
		return Etype;

	if(cType->IsClassType())
		Etype = cType;
	else{
		Etype = Type::errorType;
		ReportError::IdentifierNotDeclared(cType->getid(),LookingForClass);
	}
	return Etype;
}

void NewExpr::Check(){
	ExprType();
}

NewArrayExpr::NewArrayExpr(yyltype loc, Expr *sz, Type *et) : Expr(loc) {
    Assert(sz != NULL && et != NULL);
    (size=sz)->SetParent(this); 
    (elemType=et)->SetParent(this);
}

Type*NewArrayExpr::ExprType(){
	Type *b = size->ExprType();

	if(b!= Type::intType){
		Etype = Type::errorType;
		ReportError::NewArraySizeNotInteger(size);
	}
	else 
		Etype = new ArrayType( *location, elemType);
	return Etype;
}

void NewArrayExpr::Check(){
	size->Check();
	elemType->Check();
	ExprType();
}

PostfixExpr::PostfixExpr(LValue *lv, Operator *o) : Expr(Join(lv->GetLocation(), o->GetLocation())) {
	Assert(lv != NULL && o != NULL);
	(lvalue=lv)->SetParent(this);
	(op=o)->SetParent(this);
}

Type* PostfixExpr::ExprType(){
	if(Etype!=NULL)
		return Etype;
	if(lvalue->ExprType()!=Type::intType)
		ReportError::IncompatibleOperand(op,lvalue->ExprType());
	Etype = Type::intType;
	return Etype;
}

void PostfixExpr::Check(){
	lvalue->Check();
	ExprType();
}

