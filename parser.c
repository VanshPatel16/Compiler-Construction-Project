/*
 * Group ID - 2
 *  Vansh Patel         - 2023A7PS0629P
 *  Darshil Chauhan     - 2023A7PS0609P
 *  Adityasinh Solanki  - 2023A7PS0615P
 *  Sneh Desai          - 2023A7PS0498P
 *  Gautam Singhvie     - 2023A7PS0612P
 *  Kanav Kapoor        - 2023A7TS0628P
 */
#include "parser.h"
#include "lexer.h"


// Checks if a given grammar symbol is a terminal or not
bool isTerminal(grammarSymbol g){
    return g < NUMTOKENS;
}

// Checks if a given grammar terminal matches with a valid lexer token.
bool isEqual(grammarSymbol g, Token t){
    return (g == t);
}

// Populates grammar with first sets for each non-terminal using the getFirst() function
void calculateFirstSets(Grammar* G){
    for(int symbol = 0; symbol < NUMSYMBOLS; symbol++){
        getFirst((grammarSymbol)symbol, G);
    }
    return;
}

// Return next token keeping into consideration that comment token is not to be parsed
TokenInfo* getParserNextToken(TwinBuffer* tb){
    TokenInfo* result = getNextToken(tb);
    if(!result || result->token == TK_COMMENT){
        result = getParserNextToken(tb);
    }
    return result;
}

// Efficiently calculates union of 2 sets
bool unite(bool* dst, bool* src){
    bool modified = false;
    for(int symbol = 0; symbol < NUMTOKENS; symbol++){
        if(!dst[symbol] && src[symbol]){
            modified = true;
        }
        dst[symbol] |= src[symbol];
    }
    return modified;
}

/* 
Computes first set for a given non terminal 
*/
void getFirst(grammarSymbol X, Grammar* G){
    if(isTerminal(X)){
        // If a grammar symbol is a terminal then its first set contains only itself except if it is epsilon
        if(X != EPSILON){
            G->first[X][X] = true;
        }else{
            G->hasEPSILONFirst[X] = true;
        }
        G->isFirstComputed[X] = true;
        return;
    }
    // Memoization : If first set has already been computed then return
    if(G->isFirstComputed[X]){
        return;
    }
    G->hasEPSILONFirst[X] |= G->rules[X].hasEPSILON; // if A has epsilon transition, then it has epsilon in its first set.
    for(int rule = 0; rule < G->rules[X].rlen; rule++){
        bool allEPS = true; // flag that marks if all the symbols in the RHS have epsilon in their first, because that would lead to epsilon
        // being in X's first set
        for(int symbol = 0;symbol < G->rules[X].tlen[rule]; symbol++){
            grammarSymbol g = G->rules[X].RHS[rule][symbol];
            if(g == EPSILON){
                break;
            }else if(isTerminal(g)){
                G->first[X][g] = true;
                allEPS = false;
                break;
            }else{
                // if the first symbol on the RHS is a Non terminal, we first ask it to compute its first set, and then only we proceed.
                getFirst(g, G);
                unite(G->first[X], G->first[g]);
                G->first[X][EPSILON] = false;
                if(!G->hasEPSILONFirst[g]){
                    allEPS = false;
                    break;
                }
            }
        }
        if(allEPS)
            G->hasEPSILONFirst[X] = true;
    }
    G->isFirstComputed[X] = true; // for memoization purposes.
    return;
}

// Computes follow set for each of the non terminals using the getFollow() function
void calculateFollowSets(Grammar* G){
    G->follow[NT_PROGRAM][DOLLAR] = true;
    bool changed = true;
    while(changed){ // Flag to stop follow set computation if no set changed during last interation.
        changed = false;
        for(int symbol = 0; symbol < NUMSYMBOLS; symbol++){
            changed |= getFollow(symbol, G); // The getFollow() function returns True if the follow of 'symbol' has changed.
        }
    }
}

// Computes follow set for a given non terminal x
bool getFollow(grammarSymbol x, Grammar* G){
    if(isTerminal(x)){ // we do not do follow set computation for terminals.
        return false;
    }
    bool changed = false;
    for(int rule = 0; rule < G->rules[x].rlen; rule++){
        for(int symbol = G->rules[x].tlen[rule] - 1; symbol >= 0;symbol--){
            grammarSymbol B = G->rules[x].RHS[rule][symbol];
            
            if(isTerminal(B)){
                continue;
            }
            bool nexteps = true;
            for(int next = symbol + 1; next < G->rules[x].tlen[rule]; next++){
                grammarSymbol nxt = G->rules[x].RHS[rule][next];
                changed |= unite(G->follow[B], G->first[nxt]);

                if(!G->hasEPSILONFirst[nxt]){
                    // if every grammar symbol 'next' to the right of 'symbol' in this 'rule' has epsilon in its first set, we'll have to unite
                    // the follow set of 'x' to follow set of 'symbol'.
                    nexteps = false;
                    break;
                }
            }
            if(nexteps){
                changed |= unite(G->follow[B], G->follow[x]);
            }
        }
    }
    return changed;
}

// Removes whitespaces from the the given buffer(grammar rule).
void removeWhiteSpaces(char* buffer){
    int cnt = 0;
    for(int i = 0; buffer[i]; i ++){
        if(buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != '\r' && buffer[i] != '\t'){
            buffer[cnt] = buffer[i];
            cnt++;
        }
    }
    buffer[cnt] = '\0';
    return;
}

// Populates production rules for the grammar by reading the 'input file'
Grammar* constructGrammar(char* inputFile){
    Grammar* grammar = (Grammar*) malloc(sizeof(Grammar));
    memset(grammar, 0, sizeof(Grammar));
    // open file
    FILE* fp = fopen(inputFile, "r");
    if(!fp){
        printf("Cannot open grammar file\n");
        exit(1);
    }
    
    char buffer[256];
    memset(buffer, '\0', sizeof(buffer));
    // process each line/rule
    while(fgets(buffer, sizeof(buffer), fp) != NULL){
        // remove white spaces
        removeWhiteSpaces(buffer);
        // skipping empty lines 
        if(buffer[0] == '\0') {
            continue;
        }
        
        // extract LHS
        grammarSymbol LHS;
        char* rhs;
        char* fwd1 = NULL;
        char* fwd2 = NULL;
        char* fwd3 = NULL;
        LHS = getTokenEnum(strtok_r(buffer, ">", &fwd1));
        rhs = strtok_r(NULL, ">", &fwd1);
        grammar->rules[LHS].hasEPSILON = false;
        grammar->rules[LHS].LHS = LHS;

        // extract RHS rules
        int rlenVal = 0;
        char* rhsRule = strtok_r(rhs , "|", &fwd2);
        while(rhsRule != NULL){
            // extract seperate tokens from each RHS rule
            int tlenVal = 0;
            char* rhsRuleToken = strtok_r(rhsRule, ",", &fwd3);
            
            while(rhsRuleToken != NULL){
                grammarSymbol tokenValue = getTokenEnum(rhsRuleToken);
                grammar->rules[LHS].RHS[rlenVal][tlenVal] = tokenValue;
                if(tokenValue == EPSILON){
                    grammar->rules[LHS].hasEPSILON = true;
                }
                rhsRuleToken = strtok_r(NULL, ",", &fwd3);
                tlenVal++;
            }

            grammar->rules[LHS].tlen[rlenVal] = tlenVal; 

            rhsRule = strtok_r(NULL, "|", &fwd2);
            rlenVal++;
        }

        grammar->rules[LHS].rlen = rlenVal;
    }

    fclose(fp);
    return grammar;
}


// Helper function for populate parse table, calculates the first set for a given sentential form
bool* getFirstSentential(grammarSymbol* sententialForm,int tlen,Grammar* G,bool* allEps){
    bool* firstSt = (bool*)calloc(NUMTOKENS, sizeof(bool));
    *allEps = true;
    for(int symbol = 0; symbol < tlen; symbol++){
        grammarSymbol g = sententialForm[symbol];
        unite(firstSt, G->first[g]);
        if(!G->hasEPSILONFirst[g])
        {
            *allEps = false;
            break;
        }
    }
    return firstSt;
}

// Function to add sync tags in the parse table for the given terminals to help improve error handling.
bool heuristicSync(int t){
    return (t == TK_SEM || t == TK_ENDIF || t == TK_ELSE || t == TK_CL || t == TK_SQR || t == TK_ENDRECORD || 
    t == TK_ENDUNION || t == TK_ENDWHILE || t == TK_RETURN || 
    t == TK_END);
}

// Populates the parse table for the given grammar
// indexed by parseTable[nonTerminal][terminal] and it holds the appropriate rule.
void populateParseTable(Grammar* G){
    // initialize the parse table by PARSER_ERROR
    for(int i=0; i<NUMSYMBOLS; i++){
        for(int j=0; j<NUMTOKENS; j++){
            G->parseTableRuleLen[i][j] = 0;
            for(int k=0; k<MAXTLEN; k++){
                G->parseTable[i][j][k] = PARSER_ERROR;
            }
        }
    }
    // We implement the algorithm from the slides for parse table construction
    for(int LHS=0; LHS<NUMSYMBOLS; LHS++){
        if(isTerminal(LHS)){
            continue;
        }

        for(int rule=0; rule<G->rules[LHS].rlen; rule++){
            int curRuleLen = G->rules[LHS].tlen[rule];
            bool allEps = false;
            bool* firstSet = getFirstSentential(G->rules[LHS].RHS[rule],curRuleLen,G,&allEps);
            for(int terminal=0; terminal<NUMTOKENS; terminal++){
                if(firstSet[terminal]){
                    for(int k=0; k<curRuleLen; k++){
                        G->parseTable[LHS][terminal][k] = G->rules[LHS].RHS[rule][k];
                    }
                    G->parseTableRuleLen[LHS][terminal] = curRuleLen;
                }
            }

            if(allEps){
                for(int terminal=0; terminal<NUMTOKENS; terminal++){
                    if(G->follow[LHS][terminal]){
                        for(int k=0; k<curRuleLen; k++){
                            G->parseTable[LHS][terminal][k] = G->rules[LHS].RHS[rule][k];
                        }
                        G->parseTableRuleLen[LHS][terminal] = curRuleLen;
                    }
                }
            }
            free(firstSet);

            for(int terminal=0; terminal<NUMTOKENS; terminal++){
                if(G->parseTable[LHS][terminal][0] == PARSER_ERROR && (G->follow[LHS][terminal] || heuristicSync(terminal))){
                    G->parseTable[LHS][terminal][0] = SYN;
                    G->parseTableRuleLen[LHS][terminal] = 0;
                }
            }
        }
    }
}

// Stack implementation

// Initializes the stack
Stack* initStack(){
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    memset(stack, 0, sizeof(Stack));
    stack->currentSize = 0;
    return stack;
}

// Push function for stack
void push(Stack* stack, Node* currNode){
    stack->arr[stack->currentSize] = currNode;
    stack->currentSize++;
}

// Pop function for stack
Node* pop(Stack* stack){
    if(!stack->currentSize){
        perror("Popping from empty stack.");
        return NULL;
    }
    return stack->arr[--(stack->currentSize)];
}

// Returns top element of the stack.
Node* top(Stack* stack){
    if(!stack->currentSize){
        perror("Accessing Top element from empty stack.");
        return NULL;
    }
    return stack->arr[(stack->currentSize) - 1];
}

// Checks if the stack is empty.
bool isEmpty(Stack* stack){
    return stack->currentSize <= 0;
}

// Parse Tree implementation

// Function initializes node for the parse tree by assigning dynamic memory safely.
Node* initNode(char* lexeme, double numValue, grammarSymbol nodeSymbol, grammarSymbol parentSymbol, bool isLeafNode){
    Node* curNode = (Node*)malloc(sizeof(Node));
    memset(curNode, 0, sizeof(Node));
    if(lexeme){
        int lexlengthRead = strlen(lexeme);
        curNode->lexeme = (char*)malloc(sizeof(char) * lexlengthRead);
        strcpy(curNode->lexeme, lexeme);
    }
    curNode->numValue = numValue;

    curNode->nodeSymbol = nodeSymbol;
    curNode->parentSymbol = parentSymbol;
    curNode->isLeafNode = isLeafNode;
    curNode->numChildren = 0;
    // handle the children addition in another function. 
    return curNode;
}

// Assigns lexeme data to the node from the given token
void assignLexeme(Node* topNode, TokenInfo* curToken){
    int lexreadLength = strlen(curToken->lexeme) + 1;
    topNode->lexeme = (char*)malloc(sizeof(char) * lexreadLength);
    topNode->numValue = curToken->numValue;
    strcpy(topNode->lexeme, curToken->lexeme);
    topNode->lineNo = curToken->lineNo;
    return;
}

// Helper function for panic mode error recovery
TokenInfo* errorHandler(Grammar* G, Stack* stack, TokenInfo* curToken, TwinBuffer* tb){
    if(isEmpty(stack)){
        return NULL;
    }
    Node* topNode = top(stack);
    if(topNode->isLeafNode){
        // Terminal stack symbol does not match with the input symbol
        printf("Line %d Error : The token %s for lexeme %s does not match with the expected token %s\n",
            curToken->lineNo,
            getTokenString(curToken->token),
            curToken->lexeme,
            getTokenString(topNode->nodeSymbol));
        pop(stack);
    }else{
        grammarSymbol nonTerminal = topNode->nodeSymbol;
        grammarSymbol terminal = curToken->token;
        if(G->parseTable[nonTerminal][terminal][0] == PARSER_ERROR){
            // The non terminal stack top is not compatible with the input symbol
            // We have reached an Error state while parsing
            printf("Line %d Error: Invalid token %s encountered with value %s stack top %s\n",
                curToken->lineNo,
                getTokenString(curToken->token),
                curToken->lexeme,
                getTokenString(topNode->nodeSymbol));
            // We need to skip this input token 
                        curToken = getParserNextToken(tb);
        }else if(G->parseTable[nonTerminal][terminal][0] == SYN){
            // The non terminal present just below the stack top is compatible with the input symbol
            // We have reached a synchronization state while parsing
            printf("Line %d Error: Invalid token %s encountered with value %s stack top %s\n",
                curToken->lineNo,
                getTokenString(curToken->token),
                curToken->lexeme,
                getTokenString(topNode->nodeSymbol));
            // We need to pop the top stack symbol               
            pop(stack);
        }else{
            // Error thrown if an impossible state has been reached while parsing
            perror("Impossible\n");
            exit(1);
            return NULL;
        }
    }
    return curToken;
}

// Constructs parse tree for the given input file
Node* constructParseTree(Grammar* G, const char* inputFileName){
    // create stack and twin buffer
    Stack* stack = initStack();
    TwinBuffer* tb = (TwinBuffer*) malloc(sizeof(struct twinbuffer));
    init(inputFileName, tb);
    // add start symbol
    Node* startNode = initNode(NULL, 0, NT_PROGRAM, -1, false);
    push(stack, startNode);
    // while lexer gives tokens parse them
    TokenInfo* curToken = getParserNextToken(tb);
    bool hasError = false;
    while(true){

        if(curToken == NULL){
            // happens in the case of a whitespace (' ', '\t', '\n', etc.)
            curToken = getParserNextToken(tb);
            continue;
        }
        if(strcmp(curToken->lexeme, "EOF") == 0){
            break;
        }
        if(isEmpty(stack)){
            // required check during erroneous parsing
            break;
        }
        Node* topNode = top(stack);
        if(topNode->isLeafNode){
            if(isEqual(topNode->nodeSymbol, curToken->token)){
                assignLexeme(topNode, curToken);
                pop(stack); // we consume the terminal in input
                curToken = getParserNextToken(tb);
            }else{
                // handle non matching terminal error.
                hasError |= true;
                curToken = errorHandler(G, stack, curToken, tb);
            }
        }else{
            grammarSymbol terminal = curToken->token;
            grammarSymbol nonterminal = topNode->nodeSymbol;
            // get rule from table
            if(G->parseTableRuleLen[nonterminal][terminal] > 0){
                // there is a valid rule to parse this
                // pop the stack
                pop(stack);
                // push the rhs to stack in reverse order
                // also create the nodes for them while doing so
                for(int i = G->parseTableRuleLen[nonterminal][terminal] - 1; i >= 0; i--){
                    grammarSymbol curSymb = G->parseTable[nonterminal][terminal][i];
                    bool isLeaf = (isTerminal(curSymb));
                    Node* curSymbNode = initNode(
                        NULL,
                        0,
                        curSymb,
                        nonterminal,
                        isTerminal(curSymb)
                    );
                    // link them to their parent.
                    topNode->children[i] = curSymbNode;
                    topNode->numChildren++;
                    if(curSymb != EPSILON){
                        push(stack, curSymbNode);
                    }
                }
            }else{
                // top of stack is non terminal who has either ERROR or SYNC
                if(G->parseTable[nonterminal][terminal][0] == PARSER_ERROR){
                    hasError |= true;
                    curToken = errorHandler(G, stack, curToken, tb);
                }else if(G->parseTable[nonterminal][terminal][0] == SYN){
                    hasError |= true;
                    curToken = errorHandler(G, stack, curToken, tb);
                }else{
                    perror("Something other than parser error and syn encountered in parseTable : %d");
                    printf("Found %d : ", G->parseTable[nonterminal][terminal][0]);
                    exit(1);
                }
            }
        }
    }
    free(tb);
    if(hasError){
        return NULL;
    }
    return startNode;
}

// Prints in-order traversal of the given parse tree.
void printParseTree(Node* curNode, Node* parent, FILE* fp){
    if(!curNode){
        return;
    }
    if(!curNode->isLeafNode && curNode->numChildren > 0){
        printParseTree(curNode->children[0], curNode, fp);
    }
    if(curNode->nodeSymbol != EPSILON){
        const char* parSymb = (parent != NULL) ? getTokenString(parent->nodeSymbol) : "ROOT";
        if(curNode->isLeafNode){
            if(curNode->nodeSymbol == TK_NUM){
                fprintf(fp, "%-25s %-10d %-20s %-15d %-25s %-10s %-25s\n", 
                    curNode->lexeme ? curNode->lexeme : "----", curNode->lineNo,getTokenString(curNode->nodeSymbol), (int)curNode->numValue,parSymb, "yes", "----");
            } 
            else if(curNode->nodeSymbol == TK_RNUM){
                fprintf(fp, "%-25s %-10d %-20s %-15.4f %-25s %-10s %-25s\n", curNode->lexeme ? curNode->lexeme : "----", curNode->lineNo,getTokenString(curNode->nodeSymbol), curNode->numValue, parSymb, "yes", "----");
            } 
            else {
                fprintf(fp, "%-25s %-10d %-20s %-15s %-25s %-10s %-25s\n", 
                    curNode->lexeme ? curNode->lexeme : "----", curNode->lineNo, getTokenString(curNode->nodeSymbol), "----", parSymb, "yes", "----");
            }
        } 
        else {
            fprintf(fp, "%-25s %-10s %-20s %-15s %-25s %-10s %-25s\n","----", "----", "----", "----", parSymb, "no", getTokenString(curNode->nodeSymbol));
        }
    }
    if(!curNode->isLeafNode){
        for(int i = 1; i < curNode->numChildren; i++){
            printParseTree(curNode->children[i], curNode, fp);
        }
    }
}

// Helper function to convert given grammarSymbol string to corresponding enum
grammarSymbol getTokenEnum(const char *str){
    if(strcmp(str, "TK_ASSIGNOP") == 0){
        return TK_ASSIGNOP;
    }
    else if(strcmp(str, "TK_COMMENT") == 0){
        return TK_COMMENT;
    }
    else if(strcmp(str, "TK_FIELDID") == 0){
        return TK_FIELDID;
    }
    else if(strcmp(str, "TK_ID") == 0){
        return TK_ID;
    }
    else if(strcmp(str, "TK_NUM") == 0){
        return TK_NUM;
    }
    else if(strcmp(str, "TK_RNUM") == 0){
        return TK_RNUM;
    }
    else if(strcmp(str, "TK_FUNID") == 0){
        return TK_FUNID;
    }
    else if(strcmp(str, "TK_RUID") == 0){
        return TK_RUID;
    }
    else if(strcmp(str, "TK_WITH") == 0){
        return TK_WITH;
    }
    else if(strcmp(str, "TK_PARAMETERS") == 0){
        return TK_PARAMETERS;
    }
    else if(strcmp(str, "TK_END") == 0){
        return TK_END;
    }
    else if(strcmp(str, "TK_WHILE") == 0){
        return TK_WHILE;
    }
    else if(strcmp(str, "TK_UNION") == 0){
        return TK_UNION;
    }
    else if(strcmp(str, "TK_ENDUNION") == 0){
        return TK_ENDUNION;
    }
    else if(strcmp(str, "TK_DEFINETYPE") == 0){
        return TK_DEFINETYPE;
    }
    else if(strcmp(str, "TK_AS") == 0){
        return TK_AS;
    }
    else if(strcmp(str, "TK_TYPE") == 0){
        return TK_TYPE;
    }
    else if(strcmp(str, "TK_MAIN") == 0){
        return TK_MAIN;
    }
    else if(strcmp(str, "TK_GLOBAL") == 0){
        return TK_GLOBAL;
    }
    else if(strcmp(str, "TK_PARAMETER") == 0){
        return TK_PARAMETER;
    }
    else if(strcmp(str, "TK_LIST") == 0){
        return TK_LIST;
    }
    else if(strcmp(str, "TK_SQL") == 0){
        return TK_SQL;
    }
    else if(strcmp(str, "TK_SQR") == 0){
        return TK_SQR;
    }
    else if(strcmp(str, "TK_INPUT") == 0){
        return TK_INPUT;
    }
    else if(strcmp(str, "TK_OUTPUT") == 0){
        return TK_OUTPUT;
    }
    else if(strcmp(str, "TK_INT") == 0){
        return TK_INT;
    }
    else if(strcmp(str, "TK_REAL") == 0){
        return TK_REAL;
    }
    else if(strcmp(str, "TK_COMMA") == 0){
        return TK_COMMA;
    }
    else if(strcmp(str, "TK_SEM") == 0){
        return TK_SEM;
    }
    else if(strcmp(str, "TK_COLON") == 0){
        return TK_COLON;
    }
    else if(strcmp(str, "TK_DOT") == 0){
        return TK_DOT;
    }
    else if(strcmp(str, "TK_ENDWHILE") == 0){
        return TK_ENDWHILE;
    }
    else if(strcmp(str, "TK_OP") == 0){
        return TK_OP;
    }
    else if(strcmp(str, "TK_CL") == 0){
        return TK_CL;
    }
    else if(strcmp(str, "TK_IF") == 0){
        return TK_IF;
    }
    else if(strcmp(str, "TK_THEN") == 0){
        return TK_THEN;
    }
    else if(strcmp(str, "TK_ENDIF") == 0){
        return TK_ENDIF;
    }
    else if(strcmp(str, "TK_READ") == 0){
        return TK_READ;
    }
    else if(strcmp(str, "TK_WRITE") == 0){
        return TK_WRITE;
    }
    else if(strcmp(str, "TK_RETURN") == 0){
        return TK_RETURN;
    }
    else if(strcmp(str, "TK_PLUS") == 0){
        return TK_PLUS;
    }
    else if(strcmp(str, "TK_MINUS") == 0){
        return TK_MINUS;
    }
    else if(strcmp(str, "TK_MUL") == 0){
        return TK_MUL;
    }
    else if(strcmp(str, "TK_DIV") == 0){
        return TK_DIV;
    }
    else if(strcmp(str, "TK_CALL") == 0){
        return TK_CALL;
    }
    else if(strcmp(str, "TK_RECORD") == 0){
        return TK_RECORD;
    }
    else if(strcmp(str, "TK_ENDRECORD") == 0){
        return TK_ENDRECORD;
    }
    else if(strcmp(str, "TK_ELSE") == 0){
        return TK_ELSE;
    }
    else if(strcmp(str, "TK_AND") == 0){
        return TK_AND;
    }
    else if(strcmp(str, "TK_OR") == 0){
        return TK_OR;
    }
    else if(strcmp(str, "TK_NOT") == 0){
        return TK_NOT;
    }
    else if(strcmp(str, "TK_LT") == 0){
        return TK_LT;
    }
    else if(strcmp(str, "TK_LE") == 0){
        return TK_LE;
    }
    else if(strcmp(str, "TK_EQ") == 0){
        return TK_EQ;
    }
    else if(strcmp(str, "TK_GT") == 0){
        return TK_GT;
    }
    else if(strcmp(str, "TK_GE") == 0){
        return TK_GE;
    }
    else if(strcmp(str, "TK_NE") == 0){
        return TK_NE;
    }

    else if(strcmp(str, "EPSILON") == 0){
        return EPSILON;
    }
    else if(strcmp(str, "DOLLAR") == 0){
        return DOLLAR;
    }

    else if(strcmp(str, "NT_PROGRAM") == 0){
        return NT_PROGRAM;
    }
    else if(strcmp(str, "NT_OTHER_FUNCTIONS") == 0){
        return NT_OTHER_FUNCTIONS;
    }
    else if(strcmp(str, "NT_MAIN_FUNCTION") == 0){
        return NT_MAIN_FUNCTION;
    }
    else if(strcmp(str, "NT_FUNCTION") == 0){
        return NT_FUNCTION;
    }
    else if(strcmp(str, "NT_INPUT_PAR") == 0){
        return NT_INPUT_PAR;
    }
    else if(strcmp(str, "NT_OUTPUT_PAR") == 0){
        return NT_OUTPUT_PAR;
    }
    else if(strcmp(str, "NT_PARAMETER_LIST") == 0){
        return NT_PARAMETER_LIST;
    }
    else if(strcmp(str, "NT_REMAINING_LIST") == 0){
        return NT_REMAINING_LIST;
    }
    else if(strcmp(str, "NT_DATA_TYPE") == 0){
        return NT_DATA_TYPE;
    }
    else if(strcmp(str, "NT_PRIMITIVE_DATATYPE") == 0){
        return NT_PRIMITIVE_DATATYPE;
    }
    else if(strcmp(str, "NT_CONSTRUCTED_DATATYPE") == 0){
        return NT_CONSTRUCTED_DATATYPE;
    }
    else if(strcmp(str, "NT_STMTS") == 0){
        return NT_STMTS;
    }
    else if(strcmp(str, "NT_TYPE_DEFINITIONS") == 0){
        return NT_TYPE_DEFINITIONS;
    }
    else if(strcmp(str, "NT_ACTUAL_OR_REDEFINED") == 0){
        return NT_ACTUAL_OR_REDEFINED;
    }
    else if(strcmp(str, "NT_TYPE_DEFINITION") == 0){
        return NT_TYPE_DEFINITION;
    }
    else if(strcmp(str, "NT_FIELD_DEFINITIONS") == 0){
        return NT_FIELD_DEFINITIONS;
    }
    else if(strcmp(str, "NT_FIELD_DEFINITION") == 0){
        return NT_FIELD_DEFINITION;
    }
    else if(strcmp(str, "NT_FIELD_TYPE") == 0){
        return NT_FIELD_TYPE;
    }
    else if(strcmp(str, "NT_MORE_FIELDS") == 0){
        return NT_MORE_FIELDS;
    }
    else if(strcmp(str, "NT_DECLARATIONS") == 0){
        return NT_DECLARATIONS;
    }
    else if(strcmp(str, "NT_DECLARATION") == 0){
        return NT_DECLARATION;
    }
    else if(strcmp(str, "NT_GLOBAL_OR_NOT") == 0){
        return NT_GLOBAL_OR_NOT;
    }
    else if(strcmp(str, "NT_OTHER_STMTS") == 0){
        return NT_OTHER_STMTS;
    }
    else if(strcmp(str, "NT_STMT") == 0){
        return NT_STMT;
    }
    else if(strcmp(str, "NT_ASSIGNMENT_STMT") == 0){
        return NT_ASSIGNMENT_STMT;
    }
    else if(strcmp(str, "NT_SINGLE_OR_REC_ID") == 0){
        return NT_SINGLE_OR_REC_ID;
    }
    else if(strcmp(str, "NT_OPTION_SINGLE_CONSTRUCTED") == 0){
        return NT_OPTION_SINGLE_CONSTRUCTED;
    }
    else if(strcmp(str, "NT_ONE_EXPANSION") == 0){
        return NT_ONE_EXPANSION;
    }
    else if(strcmp(str, "NT_MORE_EXPANSIONS") == 0){
        return NT_MORE_EXPANSIONS;
    }
    else if(strcmp(str, "NT_FUN_CALL_STMT") == 0){
        return NT_FUN_CALL_STMT;
    }
    else if(strcmp(str, "NT_OUTPUT_PARAMETERS") == 0){
        return NT_OUTPUT_PARAMETERS;
    }
    else if(strcmp(str, "NT_INPUT_PARAMETERS") == 0){
        return NT_INPUT_PARAMETERS;
    }
    else if(strcmp(str, "NT_ITERATIVE_STMT") == 0){
        return NT_ITERATIVE_STMT;
    }
    else if(strcmp(str, "NT_CONDITIONAL_STMT") == 0){
        return NT_CONDITIONAL_STMT;
    }
    else if(strcmp(str, "NT_ELSE_PART") == 0){
        return NT_ELSE_PART;
    }
    else if(strcmp(str, "NT_IO_STMT") == 0){
        return NT_IO_STMT;
    }
    else if(strcmp(str, "NT_ARITHMETIC_EXPRESSION") == 0){
        return NT_ARITHMETIC_EXPRESSION;
    }
    else if(strcmp(str, "NT_EXP_PRIME") == 0){
        return NT_EXP_PRIME;
    }
    else if(strcmp(str, "NT_TERM") == 0){
        return NT_TERM;
    }
    else if(strcmp(str, "NT_TERM_PRIME") == 0){
        return NT_TERM_PRIME;
    }
    else if(strcmp(str, "NT_FACTOR") == 0){
        return NT_FACTOR;
    }
    else if(strcmp(str, "NT_HIGH_PRECEDENCE_OPERATORS") == 0){
        return NT_HIGH_PRECEDENCE_OPERATORS;
    }
    else if(strcmp(str, "NT_LOW_PRECEDENCE_OPERATORS") == 0){
        return NT_LOW_PRECEDENCE_OPERATORS;
    }
    else if(strcmp(str, "NT_BOOLEAN_EXPRESSION") == 0){
        return NT_BOOLEAN_EXPRESSION;
    }
    else if(strcmp(str, "NT_VAR") == 0){
        return NT_VAR;
    }
    else if(strcmp(str, "NT_LOGICAL_OP") == 0){
        return NT_LOGICAL_OP;
    }
    else if(strcmp(str, "NT_RELATIONAL_OP") == 0){
        return NT_RELATIONAL_OP;
    }
    else if(strcmp(str, "NT_RETURN_STMT") == 0){
        return NT_RETURN_STMT;
    }
    else if(strcmp(str, "NT_OPTIONAL_RETURN") == 0){
        return NT_OPTIONAL_RETURN;
    }
    else if(strcmp(str, "NT_ID_LIST") == 0){
        return NT_ID_LIST;
    }
    else if(strcmp(str, "NT_MORE_IDS") == 0){
        return NT_MORE_IDS;
    }
    else if(strcmp(str, "NT_DEFINETYPESTMT") == 0){
        return NT_DEFINETYPESTMT;
    }
    else if(strcmp(str, "NT_A") == 0){
        return NT_A;
    }
    else if(strcmp(str, "PARSER_ERROR")==0){
        return PARSER_ERROR;
    }else if(strcmp(str, "SYN") == 0){
        return SYN;
    }
    printf("ERROR: Unrecognized token '%s'\n", str);
    exit(1);

}

// Helper function to convert enum to string.
const char* getTokenString(grammarSymbol sym){
    if(sym == TK_ASSIGNOP){
        return "TK_ASSIGNOP";
    }
    else if(sym == TK_COMMENT){
        return "TK_COMMENT";
    }
    else if(sym == TK_FIELDID){
        return "TK_FIELDID";
    }
    else if(sym == TK_ID){
        return "TK_ID";
    }
    else if(sym == TK_NUM){
        return "TK_NUM";
    }
    else if(sym == TK_RNUM){
        return "TK_RNUM";
    }
    else if(sym == TK_FUNID){
        return "TK_FUNID";
    }
    else if(sym == TK_RUID){
        return "TK_RUID";
    }
    else if(sym == TK_WITH){
        return "TK_WITH";
    }
    else if(sym == TK_PARAMETERS){
        return "TK_PARAMETERS";
    }
    else if(sym == PARSER_ERROR){
        return "PARSER_ERROR";
    }
    else if(sym == TK_END){
        return "TK_END";
    }
    else if(sym == TK_WHILE){
        return "TK_WHILE";
    }
    else if(sym == TK_UNION){
        return "TK_UNION";
    }
    else if(sym == TK_ENDUNION){
        return "TK_ENDUNION";
    }
    else if(sym == TK_DEFINETYPE){
        return "TK_DEFINETYPE";
    }
    else if(sym == TK_AS){
        return "TK_AS";
    }
    else if(sym == TK_TYPE){
        return "TK_TYPE";
    }
    else if(sym == TK_MAIN){
        return "TK_MAIN";
    }
    else if(sym == TK_GLOBAL){
        return "TK_GLOBAL";
    }
    else if(sym == TK_PARAMETER){
        return "TK_PARAMETER";
    }
    else if(sym == TK_LIST){
        return "TK_LIST";
    }
    else if(sym == TK_SQL){
        return "TK_SQL";
    }
    else if(sym == TK_SQR){
        return "TK_SQR";
    }
    else if(sym == TK_INPUT){
        return "TK_INPUT";
    }
    else if(sym == TK_OUTPUT){
        return "TK_OUTPUT";
    }
    else if(sym == TK_INT){
        return "TK_INT";
    }
    else if(sym == TK_REAL){
        return "TK_REAL";
    }
    else if(sym == TK_COMMA){
        return "TK_COMMA";
    }
    else if(sym == TK_SEM){
        return "TK_SEM";
    }
    else if(sym == TK_COLON){
        return "TK_COLON";
    }
    else if(sym == TK_DOT){
        return "TK_DOT";
    }
    else if(sym == TK_ENDWHILE){
        return "TK_ENDWHILE";
    }
    else if(sym == TK_OP){
        return "TK_OP";
    }
    else if(sym == TK_CL){
        return "TK_CL";
    }
    else if(sym == TK_IF){
        return "TK_IF";
    }
    else if(sym == TK_THEN){
        return "TK_THEN";
    }
    else if(sym == TK_ENDIF){
        return "TK_ENDIF";
    }
    else if(sym == TK_READ){
        return "TK_READ";
    }
    else if(sym == TK_WRITE){
        return "TK_WRITE";
    }
    else if(sym == TK_RETURN){
        return "TK_RETURN";
    }
    else if(sym == TK_PLUS){
        return "TK_PLUS";
    }
    else if(sym == TK_MINUS){
        return "TK_MINUS";
    }
    else if(sym == TK_MUL){
        return "TK_MUL";
    }
    else if(sym == TK_DIV){
        return "TK_DIV";
    }
    else if(sym == TK_CALL){
        return "TK_CALL";
    }
    else if(sym == TK_RECORD){
        return "TK_RECORD";
    }
    else if(sym == TK_ENDRECORD){
        return "TK_ENDRECORD";
    }
    else if(sym == TK_ELSE){
        return "TK_ELSE";
    }
    else if(sym == TK_AND){
        return "TK_AND";
    }
    else if(sym == TK_OR){
        return "TK_OR";
    }
    else if(sym == TK_NOT){
        return "TK_NOT";
    }
    else if(sym == TK_LT){
        return "TK_LT";
    }
    else if(sym == TK_LE){
        return "TK_LE";
    }
    else if(sym == TK_EQ){
        return "TK_EQ";
    }
    else if(sym == TK_GT){
        return "TK_GT";
    }
    else if(sym == TK_GE){
        return "TK_GE";
    }
    else if(sym == TK_NE){
        return "TK_NE";
    }
    else if(sym == EPSILON){
        return "EPSILON";
    }
    else if(sym == DOLLAR){
        return "DOLLAR";
    }
    else if(sym == NT_PROGRAM){
        return "NT_PROGRAM";
    }
    else if(sym == NT_OTHER_FUNCTIONS){
        return "NT_OTHER_FUNCTIONS";
    }
    else if(sym == NT_MAIN_FUNCTION){
        return "NT_MAIN_FUNCTION";
    }
    else if(sym == NT_FUNCTION){
        return "NT_FUNCTION";
    }
    else if(sym == NT_INPUT_PAR){
        return "NT_INPUT_PAR";
    }
    else if(sym == NT_OUTPUT_PAR){
        return "NT_OUTPUT_PAR";
    }
    else if(sym == NT_PARAMETER_LIST){
        return "NT_PARAMETER_LIST";
    }
    else if(sym == NT_REMAINING_LIST){
        return "NT_REMAINING_LIST";
    }
    else if(sym == NT_DATA_TYPE){
        return "NT_DATA_TYPE";
    }
    else if(sym == NT_PRIMITIVE_DATATYPE){
        return "NT_PRIMITIVE_DATATYPE";
    }
    else if(sym == NT_CONSTRUCTED_DATATYPE){
        return "NT_CONSTRUCTED_DATATYPE";
    }
    else if(sym == NT_STMTS){
        return "NT_STMTS";
    }
    else if(sym == NT_TYPE_DEFINITIONS){
        return "NT_TYPE_DEFINITIONS";
    }
    else if(sym == NT_ACTUAL_OR_REDEFINED){
        return "NT_ACTUAL_OR_REDEFINED";
    }
    else if(sym == NT_TYPE_DEFINITION){
        return "NT_TYPE_DEFINITION";
    }
    else if(sym == NT_FIELD_DEFINITIONS){
        return "NT_FIELD_DEFINITIONS";
    }
    else if(sym == NT_FIELD_DEFINITION){
        return "NT_FIELD_DEFINITION";
    }
    else if(sym == NT_FIELD_TYPE){
        return "NT_FIELD_TYPE";
    }
    else if(sym == NT_MORE_FIELDS){
        return "NT_MORE_FIELDS";
    }
    else if(sym == NT_DECLARATIONS){
        return "NT_DECLARATIONS";
    }
    else if(sym == NT_DECLARATION){
        return "NT_DECLARATION";
    }
    else if(sym == NT_GLOBAL_OR_NOT){
        return "NT_GLOBAL_OR_NOT";
    }
    else if(sym == NT_OTHER_STMTS){
        return "NT_OTHER_STMTS";
    }
    else if(sym == NT_STMT){
        return "NT_STMT";
    }
    else if(sym == NT_ASSIGNMENT_STMT){
        return "NT_ASSIGNMENT_STMT";
    }
    else if(sym == NT_SINGLE_OR_REC_ID){
        return "NT_SINGLE_OR_REC_ID";
    }
    else if(sym == NT_OPTION_SINGLE_CONSTRUCTED){
        return "NT_OPTION_SINGLE_CONSTRUCTED";
    }
    else if(sym == NT_ONE_EXPANSION){
        return "NT_ONE_EXPANSION";
    }
    else if(sym == NT_MORE_EXPANSIONS){
        return "NT_MORE_EXPANSIONS";
    }
    else if(sym == NT_FUN_CALL_STMT){
        return "NT_FUN_CALL_STMT";
    }
    else if(sym == NT_OUTPUT_PARAMETERS){
        return "NT_OUTPUT_PARAMETERS";
    }
    else if(sym == NT_INPUT_PARAMETERS){
        return "NT_INPUT_PARAMETERS";
    }
    else if(sym == NT_ITERATIVE_STMT){
        return "NT_ITERATIVE_STMT";
    }
    else if(sym == NT_CONDITIONAL_STMT){
        return "NT_CONDITIONAL_STMT";
    }
    else if(sym == NT_ELSE_PART){
        return "NT_ELSE_PART";
    }
    else if(sym == NT_IO_STMT){
        return "NT_IO_STMT";
    }
    else if(sym == NT_ARITHMETIC_EXPRESSION){
        return "NT_ARITHMETIC_EXPRESSION";
    }
    else if(sym == NT_EXP_PRIME){
        return "NT_EXP_PRIME";
    }
    else if(sym == NT_TERM){
        return "NT_TERM";
    }
    else if(sym == NT_TERM_PRIME){
        return "NT_TERM_PRIME";
    }
    else if(sym == NT_FACTOR){
        return "NT_FACTOR";
    }
    else if(sym == NT_HIGH_PRECEDENCE_OPERATORS){
        return "NT_HIGH_PRECEDENCE_OPERATORS";
    }
    else if(sym == NT_LOW_PRECEDENCE_OPERATORS){
        return "NT_LOW_PRECEDENCE_OPERATORS";
    }
    else if(sym == NT_BOOLEAN_EXPRESSION){
        return "NT_BOOLEAN_EXPRESSION";
    }
    else if(sym == NT_VAR){
        return "NT_VAR";
    }
    else if(sym == NT_LOGICAL_OP){
        return "NT_LOGICAL_OP";
    }
    else if(sym == NT_RELATIONAL_OP){
        return "NT_RELATIONAL_OP";
    }
    else if(sym == NT_RETURN_STMT){
        return "NT_RETURN_STMT";
    }
    else if(sym == NT_OPTIONAL_RETURN){
        return "NT_OPTIONAL_RETURN";
    }
    else if(sym == NT_ID_LIST){
        return "NT_ID_LIST";
    }
    else if(sym == NT_MORE_IDS){
        return "NT_MORE_IDS";
    }
    else if(sym == NT_DEFINETYPESTMT){
        return "NT_DEFINETYPESTMT";
    }
    else if(sym == NT_A){
        return "NT_A";
    }else if(sym == SYN){
        return "SYN";
    }
}