// contains data definitions for lexer
#include <stdio.h>
#include <stdbool.h>
#define BUFFSIZE 2048
#define TBSIZE 4096
#define MAXLEX 35
enum token_enum {
    LEX_TK_ASSIGNOP, 
    LEX_TK_COMMENT, 
    LEX_TK_FIELDID, 
    LEX_TK_ID,
    LEX_TK_NUM,
    LEX_TK_RNUM,
    LEX_TK_FUNID,
    LEX_TK_RUID,
    LEX_TK_WITH,
    LEX_TK_PARAMETERS,
    LEX_TK_END,
    LEX_TK_WHILE,
    LEX_TK_UNION,
    LEX_TK_ENDUNION,
    LEX_TK_DEFINETYPE,
    LEX_TK_AS,
    LEX_TK_TYPE,
    LEX_TK_MAIN,
    LEX_TK_GLOBAL,
    LEX_TK_PARAMETER,
    LEX_TK_LIST,
    LEX_TK_SQL,
    LEX_TK_SQR,
    LEX_TK_INPUT,
    LEX_TK_OUTPUT,
    LEX_TK_INT,
    LEX_TK_REAL,
    LEX_TK_COMMA,
    LEX_TK_SEM,
    LEX_TK_COLON,
    LEX_TK_DOT,
    LEX_TK_ENDWHILE,
    LEX_TK_OP,
    LEX_TK_CL,
    LEX_TK_IF,
    LEX_TK_THEN,
    LEX_TK_ENDIF,
    LEX_TK_READ,
    LEX_TK_WRITE,
    LEX_TK_RETURN,
    LEX_TK_PLUS,
    LEX_TK_MINUS,
    LEX_TK_MUL,
    LEX_TK_DIV,
    LEX_TK_CALL,
    LEX_TK_RECORD,
    LEX_TK_ENDRECORD,
    LEX_TK_ELSE,
    LEX_TK_AND,
    LEX_TK_OR,
    LEX_TK_NOT,
    LEX_TK_LT,
    LEX_TK_LE,
    LEX_TK_EQ,
    LEX_TK_GT,
    LEX_TK_GE,
    LEX_TK_NE,
    // LEX_TK_ERROR changing ERROR to 
    LEX_TK_ERRPATTERN,
    LEX_TK_ERRUNK,
    LEX_TK_ERRLENTHIRTY,
    LEX_TK_ERRLENTWENTY,
    LEX_TK_ERRLENNUM
};

typedef enum token_enum Token;
typedef struct tokeninfo{
    Token token;
    char* lexeme;
    int lineNo;
    double numValue;
} TokenInfo;

typedef struct twinbuffer{
    char buffer[TBSIZE]; // this constitutes the twin buffer, which holds both of our buffer size by side.
    int tbStart; // points to the start of the next half of the buffer
    int basePtr; // points to the start of the current lexeme.
    int forwardPtr; // points to the start of the next lexeme.
    FILE* fp; // holds the file pointer to the input stream
    bool isLastChunk;
}TwinBuffer;
