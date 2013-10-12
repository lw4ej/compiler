
%{
#include <stdio.h>
#include "symtab.h"
static int params = 0;
static int n=0;
static infunc = 0;
%}

%union { int i; node *n; double d;}

%token GO TURN VAR JUMP
%token FOR STEP TO DO
%token COPEN CCLOSE
%token SIN COS SQRT
%token <d> FLOAT
%token <n> ID               
%token <i> NUMBER       
%token SEMICOLON PLUS MINUS TIMES DIV OPEN CLOSE ASSIGN
%token IF EQ NEQ GT GTE LT LTE
%token WHILE
%token PROCEDURE CALL PARAM

%nonassoc IF_PREF
%nonassoc ELSE

%expect 1 
 
//shift/reduce warning for general syntax error discarding in stmt

%%
program: head decllist stmtlist tail;

head: { printf("%%!PS Adobe\n"
               "\n"
	       "newpath 0 0 moveto\n"
	       );
      };

tail: { printf("closepath\nstroke\n"); };

decllist: ;
decllist: decllist decl;
decl: VAR ID SEMICOLON { 
	printf("/tlt%s 0 def\n",$2->symbol);
    if($2->defined == 1)
		fprintf(stderr,"multiple define: %s\n", $2->symbol);
    $2->defined = 1;}
     | error SEMICOLON { yyerror("unrecognized declaration"); };

stmtlist: ;
stmtlist: stmtlist stmt ;
stmt: error {yyerror("unrecognized statement");};

//procedure 
stmt: PROCEDURE ID { 
	printf("/proc%s { \n", $2->symbol); 
    if( $2->defined ==1 )
		fprintf(stderr, "multiple define: %s\n",$2->symbol);
    $2->defined =1;
	$2->params = params;
	infunc++;}  
	  stmt{ printf("} def\n"); 
	  		$2->params = params - $2->params;
	  		params = params - $2->params;
			infunc--;
	  	  };
stmt: CALL ID {n =0;}realparams{if(n!=$2->params) fprintf(stderr,"missing parameters for fun %s\n",$2->symbol); n=0;} 
      SEMICOLON{
	          if($2->defined == 1)
		 		printf(" proc%s\n", $2->symbol);
			  else fprintf(stderr,"undefined function: %s\n", $2->symbol);
			  };
realparams: ;
realparams: realparams atomic{n++;};
//end-proc

stmt: ID ASSIGN expr SEMICOLON {
	              if($1->defined == 1)
	                printf("/tlt%s exch store\n",$1->symbol);
				  else fprintf(stderr,"undefined left value: %s\n",$1->symbol);
				};
stmt: GO expr SEMICOLON {printf("0 rlineto\n");};
stmt: JUMP expr SEMICOLON {printf("0 rmoveto\n");};
stmt: TURN expr SEMICOLON {printf("rotate\n");};
stmt: FOR ID ASSIGN expr 
          STEP expr
	  TO expr
	  DO {
		  if($2->defined == 1)
			  printf("{ /tlt%s exch store\n",$2->symbol);
		  else fprintf(stderr,"undefined left value : %s \n",$2->symbol);
		 } 
	     stmt {printf("} for\n");};

//if-else
stmt: IF OPEN condition CLOSE {printf("{\n");} 
      stmt elseif;
elseif: %prec IF_PREF {printf("} if\n");};
elseif: ELSE {printf("} {\n");}
        stmt {printf("} ifelse \n");};
//end-ifelse

//while-loop
stmt: WHILE {printf("{ ");} OPEN condition CLOSE {printf("{} {exit} ifelse\n");}
      stmt {printf("} loop\n");};
//end-while

stmt: COPEN stmtlist CCLOSE;	 

condition: expr EQ expr {printf("eq \n");};
condition: expr NEQ expr {printf("ne \n");};
condition: expr GT expr {printf("gt \n");};
condition: expr GTE expr {printf("ge \n");};
condition: expr LT expr {printf("lt \n");};
condition: expr LTE expr {printf("le \n");};

expr: expr PLUS term { printf("add ");};
expr: expr MINUS term { printf("sub ");};
expr: term;

term: term TIMES factor { printf("mul ");};
term: term DIV factor { printf("div ");};
term: factor;

factor: MINUS atomic { printf("neg ");};
factor: PLUS atomic;
factor: SIN factor { printf("sin ");};
factor: COS factor { printf("cos ");};
factor: SQRT factor { printf("sqrt ");};
factor: atomic;

atomic: OPEN expr CLOSE;
atomic: NUMBER {printf("%d ",$1);};
atomic: FLOAT {printf("%f ",$1);};
atomic: ID {if($1->defined ==1)
               printf("tlt%s ", $1->symbol);
			 else fprintf(stderr,"undefined variable: %s\n",$1->symbol);
		   };
atomic: PARAM{if(infunc>0)
                 params++; 
			  else
				 fprintf(stderr, "NO PARAM available for globle\n");
			 };

%%
int yyerror(char *msg)
{  
	fprintf(stderr,"Error: %s\n", msg);
   return 0;
}
int main(void)
{   yyparse();
    return 0;
}

