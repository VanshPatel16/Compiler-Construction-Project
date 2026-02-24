// contains main()
#include <stdio.h>
#include "lexer.h"
#include <stdlib.h>
void main()
{
    TwinBuffer* tb = (TwinBuffer*) malloc(sizeof(struct twinbuffer));
    init("test.txt", tb);
    for(int i = 0;i < 65;i++)
    {
        printf("%c", getNextChar(tb));
    }
}