#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#define NUMTOKENS 61
#define NUMRULES 114
#define NUMSYMBOLS 114
#define MAXSTACKSIZE 1024
typedef enum {
    TK_ASSIGNOP, 
    TK_COMMENT, 
    TK_FIELDID, 
    TK_ID,
    TK_NUM,
    TK_RNUM,
    TK_FUNID,
    TK_RUID,
    TK_WITH,
    TK_PARAMETERS,
    TK_END,
    TK_WHILE,
    TK_UNION,
    TK_ENDUNION,
    TK_DEFINETYPE,
    TK_AS,
    TK_TYPE,
    TK_MAIN,
    TK_GLOBAL,
    TK_PARAMETER,
    TK_LIST,
    TK_SQL,
    TK_SQR,
    TK_INPUT,
    TK_OUTPUT,
    TK_INT,
    TK_REAL,
    TK_COMMA,
    TK_SEM,
    TK_COLON,
    TK_DOT,
    TK_ENDWHILE,
    TK_OP,
    TK_CL,
    TK_IF,
    TK_THEN,
    TK_ENDIF,
    TK_READ,
    TK_WRITE,
    TK_RETURN,
    TK_PLUS,
    TK_MINUS,
    TK_MUL,
    TK_DIV,
    TK_CALL,
    TK_RECORD,
    TK_ENDRECORD,
    TK_ELSE,
    TK_AND,
    TK_OR,
    TK_NOT,
    TK_LT,
    TK_LE,
    TK_EQ,
    TK_GT,
    TK_GE,
    TK_NE,
    EPSILON,
    DOLLAR,
    PARSER_ERROR,
    SYN,
    NT_PROGRAM,
    NT_OTHER_FUNCTIONS,
    NT_MAIN_FUNCTION,
    NT_FUNCTION,
    NT_INPUT_PAR,
    NT_OUTPUT_PAR,
    NT_PARAMETER_LIST,
    NT_REMAINING_LIST,
    NT_DATA_TYPE,
    NT_PRIMITIVE_DATATYPE,
    NT_CONSTRUCTED_DATATYPE,
    NT_STMTS,
    NT_TYPE_DEFINITIONS,
    NT_ACTUAL_OR_REDEFINED,
    NT_TYPE_DEFINITION,
    NT_FIELD_DEFINITIONS,
    NT_FIELD_DEFINITION,
    NT_FIELD_TYPE,
    NT_MORE_FIELDS,
    NT_DECLARATIONS,
    NT_DECLARATION,
    NT_GLOBAL_OR_NOT,
    NT_OTHER_STMTS,
    NT_STMT,
    NT_ASSIGNMENT_STMT,
    NT_SINGLE_OR_REC_ID,
    NT_OPTION_SINGLE_CONSTRUCTED,
    NT_ONE_EXPANSION,
    NT_MORE_EXPANSIONS,
    NT_FUN_CALL_STMT,
    NT_OUTPUT_PARAMETERS,
    NT_INPUT_PARAMETERS,
    NT_ITERATIVE_STMT,
    NT_CONDITIONAL_STMT,
    NT_ELSE_PART,
    NT_IO_STMT,
    NT_ARITHMETIC_EXPRESSION,
    NT_EXP_PRIME,
    NT_TERM,
    NT_TERM_PRIME,
    NT_FACTOR,
    NT_HIGH_PRECEDENCE_OPERATORS,
    NT_LOW_PRECEDENCE_OPERATORS,
    NT_BOOLEAN_EXPRESSION,
    NT_VAR,
    NT_LOGICAL_OP,
    NT_RELATIONAL_OP,
    NT_RETURN_STMT,
    NT_OPTIONAL_RETURN,
    NT_ID_LIST,
    NT_MORE_IDS,
    NT_DEFINETYPESTMT,
    NT_A,
} grammarSymbol;

typedef struct productionRule{
    #define MAXRLEN 10
    #define MAXTLEN 10
    grammarSymbol RHS[MAXRLEN][MAXTLEN];
    int tlen[MAXRLEN];
    grammarSymbol LHS;
    int rlen;
    bool hasEPSILON;
}productionRule;

typedef struct Grammar{
    productionRule rules[NUMRULES];
    grammarSymbol parseTable[NUMSYMBOLS][NUMTOKENS][MAXTLEN];
    int parseTableRuleLen[NUMSYMBOLS][NUMTOKENS];
    bool first[NUMSYMBOLS][NUMTOKENS];
    bool follow[NUMSYMBOLS][NUMTOKENS];
    bool hasEPSILONFirst[NUMSYMBOLS];
    bool isFirstComputed[NUMSYMBOLS];
}Grammar;

typedef struct Node{
    char* lexeme;
    char* numValue;
    grammarSymbol parentSymbol;
    grammarSymbol nodeSymbol;
    int lineNo;

    struct Node* children[MAXTLEN]; 
    int numChildren;
    bool isLeafNode;

}Node;

typedef struct stack{
    Node* arr[MAXSTACKSIZE];
    int currentSize;
} Stack;



