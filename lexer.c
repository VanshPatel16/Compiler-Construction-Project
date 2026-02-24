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
    if(c == '_')
        return true;
    if(c == '#')
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
    // printf("Handled white space\n");
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
        if(lexIdx >= MAXLEX){
            perror("lexeme too long in RelationalOp");
            exit(1);
            
        }
        c = getNextChar(tb);
        
        switch(state){
            case Less:
                if(c == '-'){
                    state = LessD;
                }else if(c == '='){
                    state = LessEq;
                }else{
                    // accept <
                    return TK_LT;
                }
                break;
            case LessEq:
                // accept L<=
                return TK_LE;
                break;
            case LessD:
                if(c == '-'){
                    state = LessDD;
                }else{
                    return TK_ERROR;
                }
                break;
            case LessDD:
                if(c == '-'){
                    state = LessDDD;
                }else{
                    return TK_ERROR;
                }
                break;
            case LessDDD:
                // accept <--
                return TK_ASSIGNOP;
                break;
            case Great:
                if(c == '='){
                    state = GreatEq;
                }else{
                    return TK_GT;
                }
                break;
            case GreatEq:
                return TK_GE;
                break;
            case Not:
                if(c == '='){
                    state = NotEq;
                }else{
                    return TK_NOT;
                }
                break;
            case NotEq:
                return TK_NE;
                break;
            case Eq:
                if(c == '='){
                    state = EqEq;
                }else{
                    return TK_ERROR;
                }
                break;
            case EqEq:
                return TK_EQ;
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
                    return TK_ERROR;
                }
                break;
            
            case HashA:
                if('a' <= c && c <= 'z'){
                    state = HashA;
                }else{
                    return TK_RUID;
                }
                break;
                
            case Uscore:
                if(('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z')){
                    state = UscoreA;
                }else{
                    return TK_ERROR;
                }
                break;
            
            case UscoreA:
                if(('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z')){
                    state = UscoreA;
                }else if('0' <= c && c <= '9'){
                    state = UscoreAD;
                }else{
                    // check if function is main
                    if(lexIdx==5 && lexeme[1]=='m' && lexeme[2]=='a' && lexeme[3]=='i' && lexeme[4]=='n'){
                        return TK_MAIN;
                    }else{
                        if(lexIdx > 30){
                            return TK_ERROR;
                        }
                        return TK_FUNID;
                    }
                }
                break;
            
            case UscoreAD:
                if('0' <= c && c <= '9'){
                    state = UscoreAD;
                }else{
                    return TK_FUNID;
                }
                break;
            
            case BD:
                if('2' <= c && c <= '7'){
                    state = BD27;
                }else if('a' <= c && c <= 'z'){
                    state = AminusBD;
                }else{
                    return TK_FIELDID;
                }
                break;
                
            case BD27:
                if('2' <= c && c <= '7'){
                    state = BD2727;
                }else if('b' <= c && c <= 'd'){
                    state = BD27;
                }else{
                    
                    return TK_ID;
                }
                break;
            
            case BD2727:
                if('2' <= c && c <= '7'){
                    state = BD2727;
                }else{
                    if(lexIdx > 20){
                        return TK_ERROR;
                    }
                    return TK_ID;
                }
                break;

            case AminusBD:
                if('a' <= c && c <= 'z'){
                    state = AminusBD;
                }else{
                    return TK_FIELDID;
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
        if(lexIdx == MAXLEX){
            perror("lexeme too long in tokenizeNumber");
            exit(1);
        }
        c = getNextChar(tb);
        switch(state){
            case Dstar:
                if('0' <= c && c <= '9'){
                    // will stay in Dstar
                    state = Dstar;
                }else if(c == '.'){
                    state = DstarDot;
                }else{
                    return TK_NUM;
                }
                break;
            case DstarDot:
                if('0' <= c && c <= '9'){
                    state = DstarDotD;
                }else{
                    return TK_ERROR;
                }
                break;
            case DstarDotD:
                if('0' <= c && c <= '9'){
                    state = DstarDotDD;
                }else{
                    return TK_ERROR;
                }
                break;
            case DstarDotDD:
                if(c == 'E'){
                    state = DstarDotDDE;
                }else{
                    return TK_RNUM;
                }
                break;
            case DstarDotDDE:
                if(c == '+' || c == '-'){
                    state = DstarDotDDES;
                }else if('0' <= c && c <= '9'){
                    state = DstarDotDDESD;
                }else{
                    return TK_ERROR;
                }
                break;
            case DstarDotDDES:
                if('0' <= c && c <= '9'){
                    state = DstarDotDDESD;
                }else{
                    return TK_ERROR;
                }
                break;
            case DstarDotDDESD:
                if('0'<= c && c <= '9'){
                    state = DstarDotDDESDD;
                }else{
                    return TK_ERROR;
                }
                break;  
            case DstarDotDDESDD:
                return TK_RNUM;
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
    // int lexIndex = 1;
    // char c = lexeme[0];
    // if(c == '~'){
    //     // Logical NOT
    //     return TK_NOT;
    // }else if(c == '&'){
    //     // Logical AND
    //     c = getNextChar(tb);
    //     if(c == '&'){
    //         lexeme[lexIndex] = c;
    //         lexIndex++;
    //         c = getNextChar(tb);
    //         if(c == '&'){
    //             lexeme[lexIndex] = c;
    //             retract(tb);
    //             return TK_AND;
    //         }else{
    //             retract(tb);
    //             return TK_ERROR;
    //         }
    //     }else{
    //         retract(tb);
    //         return TK_ERROR;
    //     }
    // }else if(c == '@'){
    //     // Logical OR
    //     c = getNextChar(tb);
    //     if(c == '@'){
    //         lexeme[lexIndex] = c;
    //         lexIndex++;
    //         c = getNextChar(tb);
    //         if(c == '@'){
    //             lexeme[lexIndex] = c;
    //             retract(tb);
    //             return TK_OR;
    //         }else{
    //             retract(tb);
    //             return TK_ERROR;
    //         }
    //     }else{
    //         retract(tb);
    //         return TK_ERROR;
    //     }
    // }else{
    //     perror("Not a Logical Op");
    //     exit(1);
    // }

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
                    return TK_ERROR;
                }
                break;
            case AA:
                if(c == '&'){
                    state = AAA;
                }else{
                    return TK_ERROR;
                }
                break;
            case AAA:
                return TK_AND;
            case O:
                if(c == '@'){
                    state = OO;
                }else{
                    return TK_ERROR;
                }
                break;
            case OO:
                if(c == '@'){
                    state = OOO;
                }else{
                    return TK_ERROR;
                }
                break;
            case OOO:
                return TK_OR;
                break;
            case NOT:
                return TK_NOT;
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
        return TK_DOT;
    }else if(c == ','){
        return TK_COMMA;
    }else if(c == ':'){
        return TK_COLON;
    }else if(c == ';'){
        return TK_SEM;
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
        // printf("lex : %s\n", lexeme);
        c = getNextChar(tb);
        if(c == '\n'){
            return TK_COMMENT;
        }
        // lexeme[lexIdx] = c;
        // lexIdx++;
    }

    perror("outside While in Comment");
    exit(1);
    return -1;
}

Token tokenizeBracket(char* lexeme, TwinBuffer* tb){
    char c = lexeme[0];
    if(c == '('){
        return TK_OP;
    }else if(c == ')'){
        return TK_CL;
    }else if(c == '['){
        return TK_SQL;
    }else if(c == ']'){
        return TK_SQR;
    }else{
        perror("Not a Bracker");
        exit(1);
    }
}

Token tokenizeArithmeticOp(char* lexeme, TwinBuffer* tb){
    char c = lexeme[0];
    if(c == '+'){
        return TK_PLUS;
    }else if(c == '-'){
        return TK_MINUS;
    }else if(c == '*'){
        return TK_MUL;
    }else if(c == '/'){
        return TK_DIV;
    }else{
        perror("Not an Arithmetic Op");
        exit(1);
    }
}   

void init(char* fileName, TwinBuffer* tb){
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
    // printf("Bytes read : %d\n", read);
    // printf("Buffer : %s\n", tb->buffer);

    if(read < BUFFSIZE){
        tb->isLastChunk = true;
        tb->buffer[(tb->tbStart + read) % TBSIZE] = EOF;
    }
    tb->tbStart = (tb->tbStart + BUFFSIZE) % TBSIZE;
}

char getNextChar(TwinBuffer* tb){
    char c = tb->buffer[tb->forwardPtr];
    // printf("fwd : %d c :%c \n", tb->forwardPtr, c);
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

TokenInfo* createToken(Token token, char* lexeme, int lineNo){
    TokenInfo* tk = (TokenInfo*)malloc(sizeof(TokenInfo));
    tk->lexeme = (char*)malloc(MAXLEX * sizeof(char));
    tk->token = token;
    strcpy(tk->lexeme, lexeme); // need to check if this works!!!
    tk->lineNo = lineNo;
    // printf("Created Token %d\n", token);
    fflush(stdout);
    return tk;
}

TokenInfo* getNextToken(TwinBuffer* tb){
    Token token;
    char lexeme[BUFFSIZE];
    memset(lexeme, '\0', sizeof(char) * BUFFSIZE);
    int tkLineNo = -1;
    // read the twinBuffer and identify Tokens from the input stream and return the token tuple
    char c = getNextChar(tb);
    if(c == EOF){
        return createToken(-1, "EOF", lineNo);
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
        // printf("Shifted ptrs\n");
        return NULL; // as we're not tokenizing word boundaries.
    }else{
        // c not in vocabulary. throw lexical error
        token = TK_ERROR;
    }
    shiftPointers(tb);
    if(!isCmt)
        tkLineNo = lineNo;
    return createToken(token, lexeme, tkLineNo);
}