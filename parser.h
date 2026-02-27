#include "parserDef.h"
bool isTerminal(grammarSymbol g);
// bool fillGrammarTable
void calculateFirstSets(Grammar* G);
void getFirst(grammarSymbol x, Grammar* G);
void calculateFollowSets(Grammar* G);
bool getFollow(grammarSymbol x, Grammar* G);
Grammar* constructGrammar(char* inputFile);
void removeWhiteSpaces(char* buffer);
const char* getTokenString(grammarSymbol sym);
grammarSymbol getTokenEnum(const char *str);
void populateParseTable(Grammar* G);
