/*
 * Group ID - 2
 *  Vansh Patel         - 2023A7PS0629P
 *  Darshil Chauhan     - 2023A7PS0609P
 *  Adityasinh Solanki  - 2023A7PS0615P
 *  Sneh Desai          - 2023A7PS0498P
 *  Gautam Singhvie     - 2023A7PS0612P
 *  Kanav Kapoor        - 2023A7TS0628P
 */
#include "parserDef.h"
bool isTerminal(grammarSymbol g);
void calculateFirstSets(Grammar* G);
void getFirst(grammarSymbol x, Grammar* G);
void calculateFollowSets(Grammar* G);
bool getFollow(grammarSymbol x, Grammar* G);
Grammar* constructGrammar(char* inputFile);
void removeWhiteSpaces(char* buffer);
const char* getTokenString(grammarSymbol sym);
grammarSymbol getTokenEnum(const char *str);
void populateParseTable(Grammar* G);
void push(Stack* stack, Node* curNode);
Node* pop(Stack* stack);
Node* top(Stack* stack);
Stack* initStack();
Node* initNode(char* lexeme, double numValue, grammarSymbol nodeSymbol, grammarSymbol parentSymbol, bool isLeafNode);
Node* constructParseTree(Grammar* G, const char* inputFileName);
void printParseTree(Node* root, Node* parent, FILE* fp);
const char* getTokenString(grammarSymbol sym);
