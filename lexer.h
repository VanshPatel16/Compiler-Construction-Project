/*
 * Group ID - 2
 *  Vansh Patel         - 2023A7PS0629P
 *  Darshil Chauhan     - 2023A7PS0609P
 *  Adityasinh Solanki  - 2023A7PS0615P
 *  Sneh Desai          - 2023A7PS0498P
 *  Gautam Singhvie     - 2023A7PS0612P
 *  Kanav Kapoor        - 2023A7TS0628P
 */
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

