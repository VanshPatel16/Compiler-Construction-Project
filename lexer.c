// getStream(), getNextToken(), removeComment()
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "lexer.h"

bool isRelationalOp(c){
    return (c == '<' || c == '>' || c == '!' || c == '=');
}

bool isIDAndKeyWord(c){
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

bool isNumber(c){
    return ('0' <= c && c <= '9');
}

bool isLogicalOp(c){
    return (c == '&' || c == '@' || c == '~');
}

bool isDelimeter(c){
    return (c == '.' || c == ',' || c == ':' || c == ';');
}

bool isComment(c){
    return (c == '%');
}

bool isBracket(c){
    return (c == '(' || c == ')' || c == '[' || c == ']');
}

bool isArithmeticOp(c){
    return (c == '+' || c == '-' || c == '*' || c == '/');
}
bool isWhiteSpace(c){
    return (c == ' ' || c == '\t' || c == '\n');
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
        if(lexIdx == MAXLEX){
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
            case Great:
                if(c == '='){
                    state = GreatEq;
                }else{
                    return TK_ERROR;
                }
                break;
            case GreatEq:
                return TK_GE;
            case Not:
                if(c == '='){
                    state = NotEq;
                }else{
                    return TK_NOT;
                }
                break;
            case NotEq:
                return TK_NE;
            case Eq:
                if(c == '='){
                    state = EqEq;
                }else{
                    return TK_ERROR;
                }
                break;
            case EqEq:
                return TK_EQ;
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
    }else{
        perror("Not a ID or Keyword");
        exit(1);
    }

    while(true){
        if(lexIdx > MAXLEX){
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
                    return TK_ERROR;
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
    int lexIndex = 1;
    char c = lexeme[0];
    if(c == '~'){
        // Logical NOT
        return TK_NOT;
    }else if(c == '&'){
        // Logical AND
        c = getNextChar(tb);
        if(c == '&'){
            lexeme[lexIndex] = c;
            lexIndex++;
            c = getNextChar(tb);
            if(c == '&'){
                lexeme[lexIndex] = c;
                return TK_AND;
            }else{
                return TK_ERROR;
            }
        }else{
            return TK_ERROR;
        }
    }else if(c == '@'){
        // Logical OR
        c = getNextChar(tb);
        if(c == '@'){
            lexeme[lexIndex] = c;
            lexIndex++;
            c = getNextChar(tb);
            if(c == '@'){
                lexeme[lexIndex] = c;
                return TK_OR;
            }else{
                return TK_ERROR;
            }
        }else{
            return TK_ERROR;
        }
    }else{
        perror("Not a Logical Op");
        exit(1);
    }
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
        c = getNextChar(tb);
        if(c == '\n'){
            lineNo++; // !!! Incremented line number on encountering \n;
            return TK_COMMENT;
        }
        lexeme[lexIdx] = c;
        lexIdx++;
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

Token tokenizeWhiteSpace(char* lexeme, TwinBuffer* tb){

}



void init(char* fileName, TwinBuffer* tb){
    memset(tb, 0, sizeof(TwinBuffer));
    tb->fp = fopen(fileName, "r");
    getFileStream(tb);
}

void getFileStream(TwinBuffer* tb){
    fread(tb->buffer + tb->tbStart, sizeof(char), BUFFSIZE, tb->fp);
    tb->tbStart = (tb->tbStart + BUFFSIZE) % TBSIZE;
}
char getNextChar(TwinBuffer* tb){
    char c = tb->buffer[tb->forwardPtr];
    tb->forwardPtr = (tb->forwardPtr + 1) % TBSIZE;
    if(tb->forwardPtr == tb->tbStart)
    {
        getFileStream(tb);
    }
    return c;
}

void shiftPointers(TwinBuffer* tb){
    tb->basePtr = tb->forwardPtr;
}

TokenInfo* createToken(Token token, char* lexeme, int lineNo){
    TokenInfo* tk = (TokenInfo*)malloc(sizeof(TokenInfo));
    tk->token = token;
    strcpy(tk->lexeme, lexeme); // need to check if this works!!!
    tk->lineNo = lineNo;
    return tk;
}

TokenInfo* getNextToken(TwinBuffer* tb){
    Token token;
    char lexeme[MAXLEX];
    memset(lexeme, '\0', sizeof(char) * MAXLEX);
    int lineNo = -1;
    // read the twinBuffer and identify Tokens from the input stream and return the token tuple
    char c = getNextChar(tb);
    lexeme[0] = c;
    if(isRelationalOp(c)){
        token = tokenizeRelationalOp(lexeme, tb);
    }else if(isIDAndKeyWord(c)){
        token = tokenizeIDAndKeyword(lexeme, tb);
    }else if(isNumber(c)){
        token = tokenizeNumber(lexeme, tb);
    }else if(isLogicalOp(c)){
        token = tokenizeLLogicalOp(lexeme, tb);
    }else if(isDelimeter(c)){
        token = tokenizeDelimeter(lexeme, tb);
    }else if(isComment(c)){
        token = tokenizeComment(lexeme, tb);
    }else if(isBracket(c)){
        token = tokenizeBracket(lexeme, tb);
    }else if(isArithmeticOp(c)){
        token = tokenizeArithmeticOp(lexeme, tb);
    }else if(isWhiteSpace(c)){
        token = tokenizeWhiteSpace(lexeme, tb);
    }else{
        // c not in vocabulary. throw lexical error
        perror("c is not in vocabulary!");
        exit(1);
    }



}