// getStream(), getNextToken(), removeComment()
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "lexer.h"
int lineNo = 1;
bool isRelationalOp(char c){
    return (c == '<' || c == '>' || c == '!' || c == '=');
}

bool isIDAndKeyWord(char c){
    if('b' <= c && c <= 'd')
        return true;
    if(c == '_') return true; if(c == '#')
        return true;
    if('a' <= c && c <= 'z')
        return true;
    
    return false;
}

bool isNumber(char c){
    return ('0' <= c && c <= '9');
}

bool isLogicalOp(char c){
    return (c == '&' || c == '@' || c == '~');
}

bool isDelimeter(char c){
    return (c == '.' || c == ',' || c == ':' || c == ';');
}

bool isComment(char c){
    return (c == '%');
}

bool isBracket(char c){
    return (c == '(' || c == ')' || c == '[' || c == ']');
}

bool isArithmeticOp(char c){
    return (c == '+' || c == '-' || c == '*' || c == '/');
}
bool isWhiteSpace(char c){
    return (c == ' ' || c == '\t' || c == '\n');
}

void handleWhiteSpace(char c){
    if(c == '\n'){
        lineNo++;
    }
}

Token getTokenFromLexeme(const char *lexeme) {

    if (!strcmp(lexeme, "while")) return LEX_TK_WHILE;
    if (!strcmp(lexeme, "return")) return LEX_TK_RETURN;
    if (!strcmp(lexeme, "main")) return LEX_TK_MAIN;
    if (!strcmp(lexeme, "if")) return LEX_TK_IF;
    if (!strcmp(lexeme, "then")) return LEX_TK_THEN;
    if (!strcmp(lexeme, "endif")) return LEX_TK_ENDIF;
    if (!strcmp(lexeme, "read")) return LEX_TK_READ;
    if (!strcmp(lexeme, "write")) return LEX_TK_WRITE;
    if (!strcmp(lexeme, "call")) return LEX_TK_CALL;
    if (!strcmp(lexeme, "record")) return LEX_TK_RECORD;
    if (!strcmp(lexeme, "endrecord")) return LEX_TK_ENDRECORD;
    if (!strcmp(lexeme, "union")) return LEX_TK_UNION;
    if (!strcmp(lexeme, "endunion")) return LEX_TK_ENDUNION;
    if (!strcmp(lexeme, "definetype")) return LEX_TK_DEFINETYPE;
    if (!strcmp(lexeme, "as")) return LEX_TK_AS;
    if (!strcmp(lexeme, "type")) return LEX_TK_TYPE;
    if (!strcmp(lexeme, "global")) return LEX_TK_GLOBAL;
    if (!strcmp(lexeme, "parameter")) return LEX_TK_PARAMETER;
    if (!strcmp(lexeme, "parameters")) return LEX_TK_PARAMETERS;
    if (!strcmp(lexeme, "list")) return LEX_TK_LIST;
    if (!strcmp(lexeme, "input")) return LEX_TK_INPUT;
    if (!strcmp(lexeme, "output")) return LEX_TK_OUTPUT;
    if (!strcmp(lexeme, "int")) return LEX_TK_INT;
    if (!strcmp(lexeme, "real")) return LEX_TK_REAL;
    if (!strcmp(lexeme, "else")) return LEX_TK_ELSE;
    if (!strcmp(lexeme, "with")) return LEX_TK_WITH;
    if (!strcmp(lexeme, "_main")) return LEX_TK_MAIN;
    if (!strcmp(lexeme, "end")) return LEX_TK_END;
    if (!strcmp(lexeme, "endwhile")) return LEX_TK_ENDWHILE;

    return LEX_TK_FIELDID;
}

Token tokenizeRelationalOp(char* lexeme, TwinBuffer* tb){
    enum relopStates{
        Less,
        LessEq,
        LessD,
        LessDD,
        LessDDD,
        Great,
        GreatEq,
        Not,
        NotEq,
        Eq,
        EqEq,
    };
    enum relopStates state;
    char c = lexeme[0];
    int lexIdx = 1; // points to the next character to be filled
    if(c == '<'){
        state = Less;
    }else if(c == '>'){
        state = Great;
    }else if(c == '!'){
        state = Not;
    }else if(c == '='){
        state = Eq;
    }else{
        perror("Not a relop");
        exit(1);
    }
    while(true){
        
        c = getNextChar(tb);
        
        if(lexIdx + 1 >= BUFFSIZE){

            perror("lexeme too long in RelationalOp");
            exit(1);
            
        }
        switch(state){
            case Less:
                if(c == '-'){
                    state = LessD;
                }else if(c == '='){
                    state = LessEq;
                }else{
                    return LEX_TK_LT;
                }
                break;
            case LessEq:
                return LEX_TK_LE;
                break;
            case LessD:
                if(c == '-'){
                    state = LessDD;
                }else{
                    return LEX_TK_ERRPATTERN;
                }
                break;
            case LessDD:
                if(c == '-'){
                    state = LessDDD;
                }else{
                    return LEX_TK_ERRPATTERN;
                }
                break;
            case LessDDD:
                return LEX_TK_ASSIGNOP;
                break;
            case Great:
                if(c == '='){
                    state = GreatEq;
                }else{
                    return LEX_TK_GT;
                }
                break;
            case GreatEq:
                return LEX_TK_GE;
                break;
            case Not:
                if(c == '='){
                    state = NotEq;
                }else{
                    return LEX_TK_NOT;
                }
                break;
            case NotEq:
                return LEX_TK_NE;
                break;
            case Eq:
                if(c == '='){
                    state = EqEq;
                }else{
                    return LEX_TK_ERRPATTERN;
                }
                break;
            case EqEq:
                return LEX_TK_EQ;
                break;
            default:
                perror("impossible in Relop");
                exit(1);
        }
        lexeme[lexIdx] = c; // lexeme is filled only if character read is a part of the lexeme
        lexIdx++;
    }

    perror("Outside of while in ReationalOp");
    exit(1);
    return -1;
}

Token tokenizeIDAndKeyword(char* lexeme, TwinBuffer* tb){
    enum IdKwrdStates{
        Hash,
        HashA,
        Uscore,
        UscoreA,
        UscoreAD,
        BD,
        BD27,
        BD2727,
        AminusBD,
    };
    
    enum IdKwrdStates state;
    int lexIdx = 1;
    char c = lexeme[0];
    if(c == '#'){   
        state = Hash;
    }else if(c == '_'){
        state = Uscore;
    }else if('b' <= c && c <= 'd'){
        state = BD;
    }else if('a' <= c && c <= 'z'){
        state = AminusBD;
    }else{
        perror("Not a ID or Keyword");
        exit(1);
    }

    while(true){
        if(lexIdx >= BUFFSIZE){
            perror("Lexeme is too long");
            exit(1);
        }
        c = getNextChar(tb);
        
        switch(state){
            case Hash:
                if('a' <= c && c <= 'z'){
                    state = HashA;
                }else{
                    return LEX_TK_ERRPATTERN;
                }
                break;
            
            case HashA:
                if('a' <= c && c <= 'z'){
                    state = HashA;
                }else{
                    return LEX_TK_RUID;
                }
                break;
                
            case Uscore:
                if(('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z')){
                    state = UscoreA;
                }else{
                    return LEX_TK_ERRPATTERN;
                }
                break;
            
            case UscoreA:
                if(('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z')){
                    state = UscoreA;
                }else if('0' <= c && c <= '9'){
                    state = UscoreAD;
                }else{
                    // check if function is main
                    if(lexIdx == 5 && !strncmp(lexeme, "_main", 5)){
                        return LEX_TK_MAIN;
                    }else{
                        if(lexIdx > 30){
                            // if more than 30 have been written
                            // enforced length 30 for funid.
                            return LEX_TK_ERRLENTHIRTY;
                        }
                        return LEX_TK_FUNID;
                    }
                }
                break;
            
            case UscoreAD:
                if('0' <= c && c <= '9'){
                    state = UscoreAD;
                }else{
                    return LEX_TK_FUNID;
                }
                break;
            
            case BD:
                if('2' <= c && c <= '7'){
                    state = BD27;
                }else if('a' <= c && c <= 'z'){
                    state = AminusBD;
                }else{
                    if(lexIdx > 20){
                        // 20 have already been written
                        return LEX_TK_ERRLENTWENTY;
                    }
                    return getTokenFromLexeme(lexeme);
                }
                break;
                
            case BD27:
                if('2' <= c && c <= '7'){
                    state = BD2727;
                }else if('b' <= c && c <= 'd'){
                    state = BD27;
                }else{
                    if(lexIdx > 20){
                        // 20 have already been written
                        // enforcing length 20 on LEX_TK_ID
                        return LEX_TK_ERRLENTWENTY;
                    }
                    return LEX_TK_ID;
                }
                break;
            
            case BD2727:
                if('2' <= c && c <= '7'){
                    state = BD2727;
                }else{
                    if(lexIdx > 20){
                        // 20 have already been written
                        // enforced the limits for LEX_TK_ID.
                        return LEX_TK_ERRLENTWENTY;
                    }
                    return LEX_TK_ID;
                }
                break;

            case AminusBD:
                if('a' <= c && c <= 'z'){
                    state = AminusBD;
                }else{
                    // assumed that there is no length limit for field id?!
                    return getTokenFromLexeme(lexeme);
                }
                break;

            default:
                perror("Not a ID or Keyword");
                exit(1);
                
        }
        lexeme[lexIdx] = c; // lexeme is filled only if character read is a part of the lexeme
        lexIdx++;    
    }

}

Token tokenizeNumber(char* lexeme, TwinBuffer* tb){
    enum numStates{
        Dstar,
        DstarDot,
        DstarDotD,
        DstarDotDD,
        DstarDotDDE,
        DstarDotDDES,
        DstarDotDDESD,  // also DstarDotDDED
        DstarDotDDESDD,
    };
    enum numStates state = Dstar;
    char c = lexeme[0];
    int lexIdx = 1;
    while(true){
        if(lexIdx >= BUFFSIZE){
            perror("lexeme too long in tokenizeNumber");
            exit(1);
        }
        c = getNextChar(tb);
        switch(state){
            case Dstar:
                if('0' <= c && c <= '9'){
                    state = Dstar;
                }else if(c == '.'){
                    state = DstarDot;
                }else{
                    if(lexIdx > 16){
                        return LEX_TK_ERRLENNUM;
                    }
                    return LEX_TK_NUM;
                }
                break;
            case DstarDot:
                if('0' <= c && c <= '9'){
                    state = DstarDotD;
                }else{
                    return LEX_TK_ERRPATTERN;
                }
                break;
            case DstarDotD:
                if('0' <= c && c <= '9'){
                    state = DstarDotDD;
                }else{
                    return LEX_TK_ERRPATTERN;
                }
                break;
            case DstarDotDD:
                if(c == 'E'){
                    state = DstarDotDDE;
                }else{
                    if(lexIdx > 16){
                        return LEX_TK_ERRLENNUM;
                    }
                    return LEX_TK_RNUM;
                }
                break;
            case DstarDotDDE:
                if(c == '+' || c == '-'){
                    state = DstarDotDDES;
                }else if('0' <= c && c <= '9'){
                    state = DstarDotDDESD;
                }else{
                    return LEX_TK_ERRPATTERN;
                }
                break;
            case DstarDotDDES:
                if('0' <= c && c <= '9'){
                    state = DstarDotDDESD;
                }else{
                    return LEX_TK_ERRPATTERN;
                }
                break;
            case DstarDotDDESD:
                if('0'<= c && c <= '9'){
                    state = DstarDotDDESDD;
                }else{
                    return LEX_TK_ERRPATTERN;
                }
                break;  
            case DstarDotDDESDD:
                    if(lexIdx > 16){
                        return LEX_TK_ERRLENNUM;
                    }
                return LEX_TK_RNUM;
                break;
            default:
                perror("impossible in Number");
                exit(1);
                break;
        }
        lexeme[lexIdx] = c;
        lexIdx++;
    }
    
    perror("Outside of While in Number");
    exit(1);
    return -1;
}

Token tokenizeLogicalOp(char* lexeme, TwinBuffer* tb){

    enum logicalopStates{
        A,
        AA,
        AAA,
        O,
        OO,
        OOO,
        NOT,
    };
    enum logicalopStates state;
    char c = lexeme[0];
    if(c == '&'){
        state = A;
    }else if(c == '@'){
        state = O;
    }else if(c == '~'){
        state = NOT;
    }else{
        perror("impossible in logicalOp\n");
        exit(1);
    }
    int lexIdx = 1;
    while(true){
        c = getNextChar(tb);
        switch(state){
            case A:
                if(c == '&'){
                    state = AA;
                }else{
                    return LEX_TK_ERRPATTERN;
                }
                break;
            case AA:
                if(c == '&'){
                    state = AAA;
                }else{
                    return LEX_TK_ERRPATTERN;
                }
                break;
            case AAA:
                return LEX_TK_AND;
            case O:
                if(c == '@'){
                    state = OO;
                }else{
                    return LEX_TK_ERRPATTERN;
                }
                break;
            case OO:
                if(c == '@'){
                    state = OOO;
                }else{
                    return LEX_TK_ERRPATTERN;
                }
                break;
            case OOO:
                return LEX_TK_OR;
                break;
            case NOT:
                return LEX_TK_NOT;
                break;
            default:
                perror("Impossible in logicalOp\n");
                exit(1);
        }
        lexeme[lexIdx] = c;
        lexIdx++;
    }
    perror("outside of while in logicalOp\n");
    exit(1);
    return -1;
}

Token tokenizeDelimeter(char* lexeme, TwinBuffer* tb){
    char c = lexeme[0];
    if(c == '.'){
        return LEX_TK_DOT;
    }else if(c == ','){
        return LEX_TK_COMMA;
    }else if(c == ':'){
        return LEX_TK_COLON;
    }else if(c == ';'){
        return LEX_TK_SEM;
    }else{
        perror("Not a Delimeter");
        exit(1);
    }
}

Token tokenizeComment(char* lexeme, TwinBuffer* tb){
    char c = lexeme[0];
    int lexIdx = 1;
    while(true){
        if(lexIdx >= BUFFSIZE){
            perror("comment larger than buff size\n");
            exit(1);
        }
        c = getNextChar(tb);
        if(c == '\n'){
            return LEX_TK_COMMENT;
        }
    }

    perror("outside While in Comment");
    exit(1);
    return -1;
}

Token tokenizeBracket(char* lexeme, TwinBuffer* tb){
    char c = lexeme[0];
    if(c == '('){
        return LEX_TK_OP;
    }else if(c == ')'){
        return LEX_TK_CL;
    }else if(c == '['){
        return LEX_TK_SQL;
    }else if(c == ']'){
        return LEX_TK_SQR;
    }else{
        perror("Not a Bracker");
        exit(1);
    }
}

Token tokenizeArithmeticOp(char* lexeme, TwinBuffer* tb){
    char c = lexeme[0];
    if(c == '+'){
        return LEX_TK_PLUS;
    }else if(c == '-'){
        return LEX_TK_MINUS;
    }else if(c == '*'){
        return LEX_TK_MUL;
    }else if(c == '/'){
        return LEX_TK_DIV;
    }else{
        perror("Not an Arithmetic Op");
        exit(1);
    }
}   

void init(const char* fileName, TwinBuffer* tb){
    memset(tb, 0, sizeof(TwinBuffer));
    tb->fp = fopen(fileName, "r");
    getFileStream(tb);
}

void getFileStream(TwinBuffer* tb){
    if(tb->isLastChunk){
        perror("$ Reached");
        exit(1);
    }
    size_t read = fread(tb->buffer + tb->tbStart, sizeof(char), BUFFSIZE, tb->fp);

    if(read < BUFFSIZE){
        tb->isLastChunk = true;
        tb->buffer[(tb->tbStart + read) % TBSIZE] = EOF;
    }
    tb->tbStart = (tb->tbStart + BUFFSIZE) % TBSIZE;
}

char getNextChar(TwinBuffer* tb){
    char c = tb->buffer[tb->forwardPtr];
    tb->forwardPtr = (tb->forwardPtr + 1) % TBSIZE;
    if(tb->forwardPtr == tb->tbStart){
        getFileStream(tb);
    }
    return c;
}

void shiftPointers(TwinBuffer* tb){
    tb->basePtr = tb->forwardPtr;
}
void retract(TwinBuffer* tb){
    tb->forwardPtr = (tb->forwardPtr - 1 + TBSIZE) % TBSIZE;
}

TokenInfo* createToken(Token token, char* lexeme, int lineNo, double numValue){
    TokenInfo* tk = (TokenInfo*)malloc(sizeof(TokenInfo));
    int lengthRead = strlen(lexeme) + 1;
    tk->lexeme = (char*)malloc(lengthRead * sizeof(char));
    tk->token = token;
    strcpy(tk->lexeme, lexeme); // need to check if this works!!!
    tk->lineNo = lineNo;
    tk->numValue = numValue;
    fflush(stdout);
    return tk;
}

TokenInfo* getNextToken(TwinBuffer* tb){
    Token token;
    char lexeme[BUFFSIZE];
    memset(lexeme, '\0', sizeof(char) * BUFFSIZE);
    int tkLineNo = -1;
    char c = getNextChar(tb);
    double numValue = 0;
    if(c == EOF){
        return createToken(-1, "EOF", lineNo, numValue);
    }
    lexeme[0] = c;
    bool isCmt = false;
    if(isRelationalOp(c)){
        token = tokenizeRelationalOp(lexeme, tb);
        retract(tb);
    }else if(isIDAndKeyWord(c)){
        token = tokenizeIDAndKeyword(lexeme, tb);
        retract(tb);
    }else if(isNumber(c)){
        token = tokenizeNumber(lexeme, tb);
        numValue = strtod(lexeme, NULL);
        retract(tb);
    }else if(isLogicalOp(c)){
        token = tokenizeLogicalOp(lexeme, tb);
        retract(tb);
    }else if(isDelimeter(c)){
        token = tokenizeDelimeter(lexeme, tb);
    }else if(isComment(c)){
        token = tokenizeComment(lexeme, tb);
        tkLineNo = lineNo;
        isCmt = true;
        lineNo++; // !!! Incremented line number on encountering \n;
    }else if(isBracket(c)){
        token = tokenizeBracket(lexeme, tb);
    }else if(isArithmeticOp(c)){
        token = tokenizeArithmeticOp(lexeme, tb);
    }else if(isWhiteSpace(c)){
        handleWhiteSpace(c);
        shiftPointers(tb);
        return NULL; // as we're not tokenizing word boundaries.
    }else{
        // c not in vocabulary. throw lexical error
        token = LEX_TK_ERRUNK;
    }
    shiftPointers(tb);
    if(!isCmt)
        tkLineNo = lineNo;
    
    TokenInfo* curToken = createToken(token, lexeme, tkLineNo, numValue); 
    if(curToken->token == LEX_TK_ERRPATTERN){
        printf("Line no %d : Error: Unknown pattern <%s>\n", curToken->lineNo,curToken->lexeme);
        return getNextToken(tb);
    }
    if(curToken->token == LEX_TK_ERRUNK){
        printf("Line no %d : Error : Unknown Symbol <%s>\n", curToken->lineNo,curToken->lexeme);
        return getNextToken(tb);
    }
    if(curToken->token == LEX_TK_ERRLENTWENTY){
        printf("Line no %d : Error : Variable Identifier is longer than the prescribed length of 20 characters.\n", curToken->lineNo);
        return getNextToken(tb);
    }
    if(curToken->token == LEX_TK_ERRLENTHIRTY){
        printf("Line no %d : Error : Function Identifier is longer than the prescribed length of 30 characters.\n", curToken->lineNo);
        return getNextToken(tb);
    }
    if(curToken->token == LEX_TK_ERRLENNUM){
        printf("Line no %d : Number is longer than the prescribed length of 17 characters.\n", curToken->lineNo);
        return getNextToken(tb);
    }
    return curToken;
}

void removeComments(const char* testcaseFile, const char* cleanFile){
    
    FILE* fpInput = fopen(testcaseFile, "r");
    FILE* fpOutput = fopen(cleanFile, "w");

    int ch;
    bool commentActive = false;
    while((ch = fgetc(fpInput)) != EOF){
        if(ch == '\n'){
            commentActive = false;
        }
        if(ch == (int)('%') || commentActive){
            commentActive = true;
            continue;
        }
        int isEOF = fputc(ch, fpOutput);
        if(isEOF == EOF){
            perror("Detected EOF when writing!\n");
            exit(1);
        }
    }

    fclose(fpInput);
    fclose(fpOutput);

    return;
}