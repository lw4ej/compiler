
/* Tokens.  */
#define GO 258
#define TURN 259
#define VAR 260
#define JUMP 261
#define FOR 262
#define STEP 263
#define TO 264
#define DO 265
#define COPEN 266
#define CCLOSE 267
#define SIN 268
#define COS 269
#define SQRT 270
#define FLOAT 271
#define ID 272
#define NUMBER 273
#define SEMICOLON 274
#define PLUS 275
#define MINUS 276
#define TIMES 277
#define DIV 278
#define OPEN 279
#define CLOSE 280
#define ASSIGN 281

/*Conditional and while Tokens.*/
#define IF 282
#define ELSE 283
#define THEN 284
#define WHILE 285
#define EQ 286      //Equal to
#define NEQ 287     //Not equal to
#define GT 288      //Greater than
#define GTE 289     //Greater than or equal to
#define LT 290      //Less than
#define LTE 291     //Less than or equal to
#define NOT 292
#define TRUE 293
#define FALSE 294
#define CONTINUE 295
#define BREAK 296

/*Procedure with parameters*/
#define PROCEDURE 297
#define COMMA 298
#define RETURN 299

/*Operational keywords*/
#define UP 300
#define DOWN 301
#define NORTH 302
#define EAST 303
#define WEST 304
#define SOUTH 305





typedef union YYSTYPE
{ int i; node *n; double d;}
        YYSTYPE;
YYSTYPE yylval;

