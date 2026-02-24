// will have function definitions
#include "lexerDef.h"
void init(char* fileName, TwinBuffer* tb);
// void loadBuffer(TwinBuffer* tb, FILE* fp);
char getNextChar(TwinBuffer* tb);
void shiftPointers(TwinBuffer* tb);
TokenInfo* createToken(Token token, char* lexeme, int lineNo);
void getFileStream(TwinBuffer* tb);
extern int lineNo = 0;

