// contains data definitions for lexer
#include <stdio.h>
#define BUFFSIZE 50
#define TBSIZE 100
#define MAXLEX 35
enum token_enum {
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
    TK_ERROR
};

typedef enum token_enum Token;
typedef struct tokeninfo{
    Token token;
    char* lexeme;
    int lineNo;
} TokenInfo;

typedef struct twinbuffer{
    char buffer[TBSIZE];
    int tbStart; // points to the start of the next half of the buffer
    int basePtr; // points to the start of the current lexeme.
    int forwardPtr; // points to the start of the next lexeme.
    FILE* fp;
}TwinBuffer;
