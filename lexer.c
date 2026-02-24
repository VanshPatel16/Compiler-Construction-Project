// getStream(), getNextToken(), removeComment()
#include <stdio.h>
#include "lexer.h"


void init(char* fileName, TwinBuffer* tb)
{
    tb->fp = fopen(fileName, "r");
    tb->basePtr = 0;
    tb->forwardPtr = 0;
    loadBuffer(tb);
}

void loadBuffer(TwinBuffer* tb)
{
    fread(tb->buffer + tb->tbStart, sizeof(char), BUFFSIZE, tb->fp);
    tb->tbStart = (tb->tbStart + BUFFSIZE) % TBSIZE;
}
char getNextChar(TwinBuffer* tb)
{
    char c = tb->buffer[tb->forwardPtr];
    tb->forwardPtr = (tb->forwardPtr + 1) % TBSIZE;
    if(tb->forwardPtr == tb->tbStart)
    {
        loadBuffer(tb);
    }
    return c;
}

void shiftPointers(TwinBuffer* tb)
{
    tb->basePtr = tb->forwardPtr;
}
