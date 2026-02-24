// contains data definitions for lexer
#include <stdio.h>
#define BUFFSIZE 50
#define TBSIZE 100

typedef struct twinbuffer{

    char buffer[TBSIZE];
    int tbStart; // points to the start of the next half of the buffer
    int basePtr; // points to the start of the current lexeme.
    int forwardPtr; // points to the start of the next lexeme.
    FILE* fp;
}TwinBuffer;
