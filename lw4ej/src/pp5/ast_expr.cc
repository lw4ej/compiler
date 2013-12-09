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

void IntConstant::Emit(){
	Assert(parent!=NULL);
	codegen = parent->GetGenerator();
	MemAddr = codegen->GenLoadConstant(value);
}

DoubleConstant::DoubleConstant(yyltype loc, double val) : Expr(loc) {
    value = val;
}

void DoubleConstant::Emit(){
}

BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    value = val;
}

void BoolConstant::Emit(){
	Assert(parent);
	codegen = parent->GetGenerator();
	int a = (value == true)?1:0;
	MemAddr = codegen->GenLoadConstant(a);
}

StringConstant::StringConstant(yyltype loc, const char *val) : Expr(loc) {
    Assert(val != NULL);
    value = strdup(val);
}

void StringConstant::Emit(){
	Assert(parent);
	codegen = parent->GetGenerator();
	MemAddr = codegen->GenLoadConstant(value);
}

void NullConstant::Emit(){
	Assert(parent);
	codegen = parent->GetGenerator();
	MemAddr = codegen->GenLoadConstant(0);
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

void AssignExpr::FieldAssignment(){
	Assert(parent);
	codegen = parent->GetGenerator();
	
	FieldAccess *l = dynamic_cast<FieldAccess*>(left);

	ClassDecl* cls = FindClass();
	VarDecl * var = NULL;
	if(cls)
		var = dynamic_cast<VarDecl*>(cls->Lookup( l->field,true));

	if(l->base == NULL && var == NULL){//lvalue != member in class
		VarDecl * v = dynamic_cast<VarDecl*>(Lookup(l->field, false));
		left->Emit();
		right->Emit();
		codegen->GenAssign(left->GetAddr(),right->GetAddr());
	}
	
	else if((l->base == NULL|| dynamic_cast<This*>(l->base))&& var != NULL){
		//refer lvalue = member in class
		right->Emit();
		Assert(right->GetAddr());
		codegen->GenStore(CodeGenerator::ThisPtr,right->GetAddr(),var->GetOffset());
	}
	
	else{ //base != NULL && base != This 
		Type * b = l->base->ExprType();
		NamedType* nb = dynamic_cast<NamedType*>(b);
		Decl *ty = Lookup(nb->getid(),false);
		ClassDecl *tyclass = dynamic_cast<ClassDecl*>(ty);
		var = dynamic_cast <VarDecl*>(tyclass->Lookup(l->field,true));
		l->base->Emit();
		right->Emit();
		Assert(right->GetAddr());
		codegen->GenStore(l->base->GetAddr(),right->GetAddr(),var->GetOffset());
	}
}

void AssignExpr::ArrayAssignment(){
	Assert(parent);
	codegen = parent->GetGenerator();
	ArrayAccess *l = dynamic_cast<ArrayAccess*>(left);
	Location * addr = l->TargetElem();
	right->Emit();
	codegen->GenStore(addr, right->GetAddr(), 0);
}

void AssignExpr::Emit(){
	Assert(parent);
	codegen = parent->GetGenerator();
	
	FieldAccess* fie = dynamic_cast<FieldAccess*>(left);
	ArrayAccess* arr  = dynamic_cast<ArrayAccess*>(left);
    /*deal with store for the left value*/	
	if(arr != NULL)
		ArrayAssignment();
	else{
		Assert(fie != NULL);
		FieldAssignment();
	}
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

void ArithmeticExpr::Emit(){
	Assert(parent);
	codegen = parent->GetGenerator();
	char *opName = op->op();
	if(left){
		left->Emit();
		right->Emit();
		MemAddr = codegen->GenBinaryOp(opName,
		               left->GetAddr(),right->GetAddr());
	}
	else{
		Assert(strcmp(opName, "-")==0);
		Location* tp = codegen->GenLoadConstant(0);
		right->Emit();
		MemAddr = codegen->GenBinaryOp("-", 
					   tp, right->GetAddr());
	}

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

void RelationalExpr::Emit(){
	Assert(parent);
	codegen = parent->GetGenerator();
	char* opName = op->op();
	left->Emit();
	right->Emit();
	if(strcmp(opName,"<")==0){
		MemAddr = codegen->GenBinaryOp("<",left->GetAddr(),right->GetAddr());
	}
	if(strcmp(opName,">")==0){
		MemAddr = codegen->GenBinaryOp("<", right->GetAddr(),left->GetAddr());
	}
	if(strcmp(opName,">=")==0){
		Location* tp1 = codegen->GenBinaryOp("<", right->GetAddr(),left->GetAddr());
		Location* tp2 = codegen->GenBinaryOp("==",left->GetAddr(),right->GetAddr());
		MemAddr = codegen->GenBinaryOp("||",tp1, tp2);
	}
	if(strcmp(opName,"<=")==0){
		Location* tp1 = codegen->GenBinaryOp("<", left->GetAddr(),right->GetAddr());
		Location* tp2 = codegen->GenBinaryOp("==",left->GetAddr(),right->GetAddr());
		MemAddr = codegen->GenBinaryOp("||",tp1, tp2);
	}
}

Type* EqualityExpr::ExprType(){
	if( Etype != NULL )
		return Etype;
	
	if((left->ExprType()==Type::errorType) 
		||right->ExprType()==Type::errorType){
		Etype = Type::boolType;
		return Etype;
	}

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

void EqualityExpr::Emit(){
	Assert(parent);
	codegen=parent->GetGenerator();
	char *opName = op->op();
	left->Emit();
	right->Emit();
	
	if(strcmp(opName, "==")==0){
		if(left->ExprType()->IsEquivalentTo(Type::stringType))
			MemAddr = codegen->GenBuiltInCall(StringEqual,
								left->GetAddr(),right->GetAddr());
		else 
			MemAddr = codegen->GenBinaryOp("==",left->GetAddr(),right->GetAddr());
	}
	else {
		Assert(strcmp(opName, "!=")==0);
		Location *tp1 = NULL;
		if(left->ExprType()->IsEquivalentTo(Type::stringType))
			tp1 = codegen->GenBuiltInCall(StringEqual,
								left->GetAddr(),right->GetAddr());
		else
			tp1 = codegen->GenBinaryOp("==", left->GetAddr(),right->GetAddr());
		
		Location *tp2 = codegen->GenLoadConstant(0);
		MemAddr = codegen->GenBinaryOp("==", tp1, tp2);
	}
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

void LogicalExpr::Emit(){
	Assert(parent);
	codegen = parent->GetGenerator();
	char* opName = op->op();
	if(left){
		left->Emit();
		right->Emit();
		MemAddr = codegen->GenBinaryOp(opName,left->GetAddr(),right->GetAddr());
	}
	else{
		right->Emit();
		Assert(strcmp(opName, "!")==0);
		Location * tp=codegen->GenLoadConstant(0);
		MemAddr = codegen->GenBinaryOp("==",tp,right->GetAddr());
	}
	
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

Location* ArrayAccess::TargetElem(){
	codegen = parent->GetGenerator();
	subscript->Emit();
	Location * index = subscript->GetAddr();
	Location * zero = codegen->GenLoadConstant(0);
	Location * l = codegen->GenBinaryOp("<", index, zero);
	base->Emit();
	Location * len = codegen->GenLoad(base->GetAddr(), -4);
	Location * r0 = codegen->GenBinaryOp("<", index, len);
	Location * r = codegen->GenBinaryOp("==", r0, zero);
	Location * test = codegen->GenBinaryOp("||", l, r);
	
	char* label = codegen->NewLabel();
	codegen->GenIfZ(test,label);
	Location * errinfo = codegen->GenLoadConstant(err_arr_out_of_bounds);
	codegen->GenBuiltInCall(PrintString, errinfo, NULL);	
	codegen->GenBuiltInCall(Halt,NULL,NULL);
	codegen->GenLabel(label);
	Location * size = codegen->GenLoadConstant(4);
	Location * offset = codegen->GenBinaryOp("*", size, index);
	Location * addr = codegen->GenBinaryOp("+", base->GetAddr(),offset);
	return addr;
}

void ArrayAccess::Emit(){
	codegen = parent->GetGenerator();
	Location* addr = TargetElem();
	MemAddr = codegen->GenLoad(addr, 0);
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

void This::Emit(){
	MemAddr = CodeGenerator::ThisPtr;
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
			if(dynamic_cast<This*>(base)) 
				return Etype;
			else {
				ClassDecl *t = FindClass();
				if(t!=NULL&& dynamic_cast<ClassDecl*>(t) 
					&& t->getself()->IsCompatTo(tyclass->getself()))
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

void FieldAccess::Emit(){
	Assert(parent);
	codegen = parent->GetGenerator();
	ClassDecl* cls = FindClass();
	VarDecl * var = NULL;
	if(cls)
		var = dynamic_cast<VarDecl*>(cls->Lookup( field,true));

	if(base == NULL && var == NULL){//lvalue != member in class
		VarDecl * v = dynamic_cast<VarDecl*>(Lookup(field, false));
		MemAddr = v->GetAddr();
		Assert(MemAddr!=NULL);
	}
    //base!=NULL and var==NULL is not possible

	else if((base == NULL|| dynamic_cast<This*>(base))&& var != NULL){
		//refer lvalue = member in class
		MemAddr = codegen->GenLoad(CodeGenerator::ThisPtr, var->GetOffset());
		Assert(MemAddr!=NULL);
	}
	
	else{ //base != NULL && base != This
		Type* b = base->ExprType();
		NamedType* nb = dynamic_cast<NamedType*>(b);
		Decl *ty = Lookup(nb->getid(),false);
		ClassDecl *tyclass = dynamic_cast<ClassDecl*>(ty);
		var = dynamic_cast<VarDecl*> (tyclass->Lookup(field,true));
		base->Emit();	
		MemAddr = codegen->GenLoad(base->GetAddr(),var->GetOffset());
		Assert(MemAddr!=NULL);
	}
	Assert(MemAddr!=NULL);
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

void Call::Emit_Actuals(){
	for(int i = 0; i<actuals->NumElements();i++)
			actuals->Nth(i)->Emit();
}

void Call::Push_Actuals(){
	for(int i = actuals->NumElements()-1;i>=0;i--)
			codegen->GenPushParam(actuals->Nth(i)->GetAddr());
}

void Call::Emit(){
	Assert(parent);
	codegen = parent->GetGenerator();

	ClassDecl * cls = FindClass();
	FnDecl* fn = NULL;
	if(cls)
		fn = dynamic_cast<FnDecl*>(cls->Lookup(field,true));
	
	if(base == NULL && fn == NULL){ //fn != method in class 
	 	fn = dynamic_cast<FnDecl*>(Lookup(field, false));	
		bool isreturnType = (fn->GetRtype() == Type::voidType)?false:true;
		Emit_Actuals();
		Push_Actuals();
		if(isreturnType)
			MemAddr = codegen->GenLCall( fn->GetLabel(), isreturnType);
		else codegen->GenLCall(fn->GetLabel(), false);

		codegen->GenPopParams(actuals->NumElements()*4);
	}
	
	else if((base == NULL || dynamic_cast<This*>(base))&& fn!=NULL){
	//fn == mothod in this class	
		bool isreturnType = (fn->GetRtype() == Type::voidType)?false:true;
		Location* tmp0 = codegen->GenLoad(CodeGenerator::ThisPtr,0);//vtable
		Location* f = codegen->GenLoad(tmp0, fn->GetOffset());
		Emit_Actuals();
		Push_Actuals();
		codegen->GenPushParam(CodeGenerator::ThisPtr);

		if(isreturnType)
			MemAddr = codegen->GenACall(f, isreturnType);
		else 
			codegen->GenACall(f, false);
		codegen->GenPopParams(actuals->NumElements()*4 + 4);
	}

	else{
		//array length
		Type * b = base->ExprType();
		ArrayType * arr = dynamic_cast<ArrayType*>(b);

		if(arr != NULL){//(field->getname()=="length")
			base->Emit();
			MemAddr = codegen->GenLoad(base->GetAddr(),0); 
		}
		else{
		//object function
			bool isreturnType = (fn->GetRtype() == Type::voidType)?false:true;
			Emit_Actuals();
			base->Emit();

			Location * tmp0 = codegen->GenLoad(base->GetAddr(),0); //load vtable
			Location * tmp1 = codegen->GenLoad(tmp0, fn->GetOffset());
		 	Push_Actuals();	
			codegen->GenPushParam(base->GetAddr());
			if(isreturnType)
				MemAddr = codegen->GenACall(tmp1, isreturnType);
			else
				codegen->GenACall(tmp1, isreturnType);
			codegen->GenPopParams(actuals->NumElements()*4 + 4);
		}
	}
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

void NewExpr::Emit(){
	Assert(parent);
	codegen = parent->GetGenerator();
	Decl * d = parent->Lookup(cType->getid(),false);
	ClassDecl* c = dynamic_cast<ClassDecl*>(d);
	Assert(c);
	Location* size = codegen->GenLoadConstant(c->getsize());
	MemAddr = codegen->GenBuiltInCall(Alloc,size,NULL);
	Location* vtable = codegen->GenLoadLabel(cType->getkey());
	codegen->GenStore(MemAddr,vtable,0);
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

void NewArrayExpr::Emit(){
	Assert(parent);
	codegen = parent->GetGenerator();
 
	char * tplabel = codegen->NewLabel();
	
	size->Emit();
	Location* tmp0 = codegen->GenLoadConstant(0);
	Location * tmp1 = codegen->GenBinaryOp("<", size->GetAddr(), tmp0);
	codegen->GenIfZ(tmp1, tplabel);
	Location * tmp2 = codegen->GenLoadConstant(err_arr_bad_size);
	codegen->GenBuiltInCall(PrintString, tmp2, NULL);
	codegen->GenBuiltInCall(Halt, NULL, NULL);
	codegen->GenLabel(tplabel);
	
	Location * tmp3 = codegen->GenLoadConstant(1);
	Location * tmp4 = codegen->GenBinaryOp("+", tmp3, size->GetAddr());
	Location * tmp5 = codegen->GenLoadConstant(4);
	Location * tmp6 = codegen->GenBinaryOp("*", tmp5, tmp4);
	Location * tmp7 = codegen->GenBuiltInCall(Alloc, tmp6, NULL);
	codegen->GenStore(tmp7,size->GetAddr(),0);
	MemAddr = codegen->GenBinaryOp("+", tmp7, tmp5);
}

void ReadIntegerExpr::Emit(){
	Assert(parent);
	codegen = parent->GetGenerator();
	MemAddr = codegen->GenBuiltInCall(ReadInteger,NULL,NULL);
}

void ReadLineExpr::Emit(){
	Assert(parent);
	codegen = parent->GetGenerator();
	MemAddr = codegen ->GenBuiltInCall(ReadLine,NULL,NULL);
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

void PostfixExpr::Emit(){
	Assert(parent);
	codegen = parent->GetGenerator();
	Location* one = codegen->GenLoadConstant(1);
	lvalue->Emit();
	Location* tmp = codegen->GenBinaryOp(op->op(),lvalue->GetAddr(), one);
	FieldAccess* fie = dynamic_cast<FieldAccess*>(lvalue);
	ArrayAccess* arr  = dynamic_cast<ArrayAccess*>(lvalue);
    /*deal with store for the left value*/	
	if(arr != NULL)
		ArrayAssignment(tmp);
	else{
		Assert(fie != NULL);
		FieldAssignment(tmp);
	}
	MemAddr = lvalue->GetAddr();
}
void PostfixExpr::FieldAssignment(Location * right){
	Assert(parent);
	codegen = parent->GetGenerator();
	
	FieldAccess *l = dynamic_cast<FieldAccess*>(lvalue);

	ClassDecl* cls = FindClass();
	VarDecl * var = NULL;
	if(cls)
		var = dynamic_cast<VarDecl*>(cls->Lookup( l->field,true));

	if(l->base == NULL && var == NULL){//lvalue != member in class
		VarDecl * v = dynamic_cast<VarDecl*>(Lookup(l->field, false));
		lvalue->Emit();
		codegen->GenAssign(lvalue->GetAddr(),right);
	}
	
	else if((l->base == NULL|| dynamic_cast<This*>(l->base))&& var != NULL){
		//refer lvalue = member in class
		codegen->GenStore(CodeGenerator::ThisPtr,right,var->GetOffset());
	}
	
	else{ //base != NULL && base != This 
		Type * b = l->base->ExprType();
		NamedType* nb = dynamic_cast<NamedType*>(b);
		Decl *ty = Lookup(nb->getid(),false);
		ClassDecl *tyclass = dynamic_cast<ClassDecl*>(ty);
		var = dynamic_cast <VarDecl*>(tyclass->Lookup(l->field,true));
		l->base->Emit();
		codegen->GenStore(l->base->GetAddr(),right,var->GetOffset());
	}
}

void PostfixExpr::ArrayAssignment(Location* right){
	Assert(parent);
	codegen = parent->GetGenerator();
	ArrayAccess *l = dynamic_cast<ArrayAccess*>(lvalue);
	Location * addr = l->TargetElem();
	codegen->GenStore(addr, right, 0);
}


