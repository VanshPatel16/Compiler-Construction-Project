// will have function definitions
#include "lexerDef.h"
void init(const char* fileName, TwinBuffer* tb);
char getNextChar(TwinBuffer* tb);
void shiftPointers(TwinBuffer* tb);
TokenInfo* createToken(Token token, char* lexeme, int lineNo, double numValue);
void getFileStream(TwinBuffer* tb);
TokenInfo* getNextToken(TwinBuffer* tb);
void retract(TwinBuffer* tb);
void removeComments(const char* testcaseFile);

