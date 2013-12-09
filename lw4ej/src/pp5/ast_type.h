/* File: ast_type.h
 * ----------------
 * In our parse tree, Type nodes are used to represent and
 * store type information. The base Type class is used
 * for built-in types, the NamedType for classes and interfaces,
 * and the ArrayType for arrays of other types.  
 *
 * pp3: You will need to extend the Type classes to implement
 * the type system and rules for type equivalency and compatibility.
 */
 
#ifndef _H_ast_type
#define _H_ast_type

#include "ast.h"
#include "list.h"
#include <iostream>


class Type : public Node 
{
  protected:
    char *typeName;
	bool IsError;
  public :
    static Type *intType, *doubleType, *boolType, *voidType,
                *nullType, *stringType, *errorType;

    Type(yyltype loc) : Node(loc) {IsError=false;}
    Type(const char *str);
    virtual void PrintToStream(std::ostream& out) { out << typeName; }
    friend std::ostream& operator<<(std::ostream& out, Type *t) { t->PrintToStream(out); return out; }
	virtual bool IsEquivalentTo(Type *other) { return this == other; }
	virtual bool IsCompatTo(Type * other);
	void Check(){};
	bool IsArithType();
	bool geterror(){return IsError;}
};

class NamedType : public Type 
{
  protected:
    Identifier *id;
    
  public:
    NamedType(Identifier *i);    
	Identifier *getid() {return id;}
	char* getkey(){return id->getname();}
    void PrintToStream(std::ostream& out) { out << id; }
    bool IsEquivalentTo(Type *other);
	bool IsCompatTo(Type *other);
	bool IsClassType();
	bool IsIntfType();
	void Check();
};

class ArrayType : public Type 
{
  protected:
    Type *elemType;

  public:
    ArrayType(yyltype loc, Type *elemType);
	Type *GetEleType() { return elemType; }  
    void PrintToStream(std::ostream& out) { out << elemType << "[]"; }
    bool IsEquivalentTo(Type *other);
	bool IsCompatTo(Type *other);
	void Check();
};

 
#endif
