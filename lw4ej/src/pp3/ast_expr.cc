/* File: ast_expr.cc
 * -----------------
 * Implementation of expression node classes.
 */
#include "ast_expr.h"
#include "ast_type.h"
#include "ast_decl.h"
#include <string.h>



IntConstant::IntConstant(yyltype loc, int val) : Expr(loc) {
    value = val;
	strcpy(nodeName,"IntConstant");
}

DoubleConstant::DoubleConstant(yyltype loc, double val) : Expr(loc) {
    value = val;
	strcpy(nodeName,"DoubleConstant");
}

BoolConstant::BoolConstant(yyltype loc, bool val) : Expr(loc) {
    value = val;
	strcpy(nodeName,"BoolConstant");
}

StringConstant::StringConstant(yyltype loc, const char *val) : Expr(loc) {
    Assert(val != NULL);
    value = strdup(val);
	strcpy(nodeName,"StringConstant");
}

Operator::Operator(yyltype loc, const char *tok) : Node(loc) {
    Assert(tok != NULL);
    strncpy(tokenString, tok, sizeof(tokenString));
	strcpy(nodeName, "Operator");
}
CompoundExpr::CompoundExpr(Expr *l, Operator *o, Expr *r) 
  : Expr(Join(l->GetLocation(), r->GetLocation())) {
    Assert(l != NULL && o != NULL && r != NULL);
    (op=o)->SetParent(this);
	SetChild(op);
    (left=l)->SetParent(this); 
	SetChild(left);
    (right=r)->SetParent(this);
	SetChild(right);
	strcpy(nodeName, "CompoundExpr");
}

CompoundExpr::CompoundExpr(Operator *o, Expr *r) 
  : Expr(Join(o->GetLocation(), r->GetLocation())) {
    Assert(o != NULL && r != NULL);
    left = NULL; 
    (op=o)->SetParent(this);
    SetChild(op);
    (right=r)->SetParent(this);
	SetChild(right);
	strcpy(nodeName, "CompoundExpr");
}
   
  
ArrayAccess::ArrayAccess(yyltype loc, Expr *b, Expr *s) : LValue(loc) {
    (base=b)->SetParent(this); 
	SetChild(base);
    (subscript=s)->SetParent(this);
	SetChild(subscript);
	strcpy(nodeName, "ArrayAcess");
}
     
FieldAccess::FieldAccess(Expr *b, Identifier *f) 
  : LValue(b? Join(b->GetLocation(), f->GetLocation()) : *f->GetLocation()) {
    Assert(f != NULL); // b can be be NULL (just means no explicit base)
    base = b; 
    if (base) {
		base->SetParent(this);
		SetChild(base);
	}
    (field=f)->SetParent(this);
	SetChild(field);
	strcpy(nodeName, "FieldAcess");
}


Call::Call(yyltype loc, Expr *b, Identifier *f, List<Expr*> *a) : Expr(loc)  {
    Assert(f != NULL && a != NULL); // b can be be NULL (just means no explicit base)
    base = b;
    if (base) {
		base->SetParent(this);
		SetChild(base);
	}
    (field=f)->SetParent(this);
	SetChild(field);
    (actuals=a)->SetParentAll(this);
	strcpy(nodeName, "Call");
}
 

NewExpr::NewExpr(yyltype loc, NamedType *c) : Expr(loc) { 
  Assert(c != NULL);
  (cType=c)->SetParent(this);
  SetChild(cType);
  strcpy(nodeName, "Call");
}


NewArrayExpr::NewArrayExpr(yyltype loc, Expr *sz, Type *et) : Expr(loc) {
    Assert(sz != NULL && et != NULL);
    (size=sz)->SetParent(this); 
	SetChild(size);
    (elemType=et)->SetParent(this);
	SetChild(elemType);
	strcpy(nodeName,"NewArrayExpr");
}
PostfixExpr::PostfixExpr(LValue *lv, Operator *o) : Expr(Join(lv->GetLocation(), o->GetLocation())) {
	Assert(lv != NULL && o != NULL);
	(lvalue=lv)->SetParent(this);
	SetChild(lvalue);
	(op=o)->SetParent(this);
	SetChild(op);
	strcpy(nodeName, "PostfixExpr");
}
       
