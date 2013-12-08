/* File: ast_type.cc
 * -----------------
 * Implementation of type node classes.
 */
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h>
#include "errors.h"
 
/* Class constants
 * ---------------
 * These are public constants for the built-in base types (int, double, etc.)
 * They can be accessed with the syntax Type::intType. This allows you to
 * directly access them and share the built-in types where needed rather that
 * creates lots of copies.
 */

Type *Type::intType    = new Type("int");
Type *Type::doubleType = new Type("double");
Type *Type::voidType   = new Type("void");
Type *Type::boolType   = new Type("bool");
Type *Type::nullType   = new Type("null");
Type *Type::stringType = new Type("string");
Type *Type::errorType  = new Type("error"); 

Type::Type(const char *n) {
    Assert(n);
    typeName = strdup(n);
	IsError = false;
}

bool Type::IsArithType(){
	if( this == intType
		|| this == doubleType)
		return true;
	return false;
}

bool Type::IsCompatTo(Type *other){
	if( this ==Type:: nullType && dynamic_cast<NamedType*>(other))
		return true;
	else return this==other;
}

NamedType::NamedType(Identifier *i) : Type(*i->GetLocation()) {
    Assert(i != NULL);
    (id=i)->SetParent(this);

} 

bool NamedType::IsClassType(){
	Decl * b = parent->Lookup(id, false);
	if(dynamic_cast<ClassDecl*>(b))
		return true;
	else return false;
}

bool NamedType::IsIntfType(){
	Decl * b = parent->Lookup(id, false);
	if(dynamic_cast<InterfaceDecl*>(b))
		return true;
	else return false;
}

bool NamedType::IsEquivalentTo(Type *other) { 
		NamedType *tp;
		if((tp=dynamic_cast<NamedType*>(other))==NULL)
			return false;
		else return !strcmp(getkey(),tp->getkey()); 
}

bool NamedType::IsCompatTo(Type *other){
	//this could assign to other, but not inverse
	NamedType *tp=NULL;
	if((tp=dynamic_cast<NamedType*>(other))==NULL)
		return false;
	if(!strcmp(getkey(),tp->getkey()))
		return true;
	
	Assert(parent!=NULL);
	Decl* t = parent->Lookup(id,false);
	ClassDecl* b = dynamic_cast<ClassDecl*>(t);
	List<NamedType*> *l = NULL;

	if(b!=NULL && (l=b->getCompatList())!=NULL){
		for(int i=0; i<l->NumElements(); i++){
			if((l->Nth(i)->IsCompatTo(other))==true)
				return true;
		}
	}
	return false;
}

void NamedType::Check(){
	Assert(parent!=NULL);
	Decl* t = parent->Lookup(id,false);

	if(t == NULL ||
		!(dynamic_cast<ClassDecl*>(t)||dynamic_cast<InterfaceDecl*>(t))){
			ReportError::IdentifierNotDeclared(id,LookingForType);
			IsError = true;
	}
}

ArrayType::ArrayType(yyltype loc, Type *et) : Type(loc) {
    Assert(et != NULL);
    (elemType=et)->SetParent(this);

}

bool ArrayType::IsEquivalentTo(Type *other) { 
		ArrayType *tp;
		if((tp=dynamic_cast<ArrayType*>(other))==NULL)
			return false;
		else return elemType->IsEquivalentTo(tp->elemType); 
}

bool ArrayType::IsCompatTo(Type* other){ return IsEquivalentTo(other);}

void ArrayType::Check(){
	Assert(parent!=NULL);
	elemType->Check();
	if(elemType->geterror()==true)
		IsError = true;
}
