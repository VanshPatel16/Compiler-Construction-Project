
// contains main()
#include <stdio.h>
#include "lexer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char* tokenToString(enum token_enum token) {
    switch (token) {
        case TK_ASSIGNOP:   return "TK_ASSIGNOP";
        case TK_COMMENT:    return "TK_COMMENT";
        case TK_FIELDID:    return "TK_FIELDID";
        case TK_ID:         return "TK_ID";
        case TK_NUM:        return "TK_NUM";
        case TK_RNUM:       return "TK_RNUM";
        case TK_FUNID:      return "TK_FUNID";
        case TK_RUID:       return "TK_RUID";
        case TK_WITH:       return "TK_WITH";
        case TK_PARAMETERS: return "TK_PARAMETERS";
        case TK_END:        return "TK_END";
        case TK_WHILE:      return "TK_WHILE";
        case TK_UNION:      return "TK_UNION";
        case TK_ENDUNION:   return "TK_ENDUNION";
        case TK_DEFINETYPE: return "TK_DEFINETYPE";
        case TK_AS:         return "TK_AS";
        case TK_TYPE:       return "TK_TYPE";
        case TK_MAIN:       return "TK_MAIN";
        case TK_GLOBAL:     return "TK_GLOBAL";
        case TK_PARAMETER:  return "TK_PARAMETER";
        case TK_LIST:       return "TK_LIST";
        case TK_SQL:        return "TK_SQL";
        case TK_SQR:        return "TK_SQR";
        case TK_INPUT:      return "TK_INPUT";
        case TK_OUTPUT:     return "TK_OUTPUT";
        case TK_INT:        return "TK_INT";
        case TK_REAL:       return "TK_REAL";
        case TK_COMMA:      return "TK_COMMA";
        case TK_SEM:        return "TK_SEM";
        case TK_COLON:      return "TK_COLON";
        case TK_DOT:        return "TK_DOT";
        case TK_ENDWHILE:   return "TK_ENDWHILE";
        case TK_OP:         return "TK_OP";
        case TK_CL:         return "TK_CL";
        case TK_IF:         return "TK_IF";
        case TK_THEN:       return "TK_THEN";
        case TK_ENDIF:      return "TK_ENDIF";
        case TK_READ:       return "TK_READ";
        case TK_WRITE:      return "TK_WRITE";
        case TK_RETURN:     return "TK_RETURN";
        case TK_PLUS:       return "TK_PLUS";
        case TK_MINUS:      return "TK_MINUS";
        case TK_MUL:        return "TK_MUL";
        case TK_DIV:        return "TK_DIV";
        case TK_CALL:       return "TK_CALL";
        case TK_RECORD:     return "TK_RECORD";
        case TK_ENDRECORD:  return "TK_ENDRECORD";
        case TK_ELSE:       return "TK_ELSE";
        case TK_AND:        return "TK_AND";
        case TK_OR:         return "TK_OR";
        case TK_NOT:        return "TK_NOT";
        case TK_LT:         return "TK_LT";
        case TK_LE:         return "TK_LE";
        case TK_EQ:         return "TK_EQ";
        case TK_GT:         return "TK_GT";
        case TK_GE:         return "TK_GE";
        case TK_NE:         return "TK_NE";
        default:            return "UNKNOWN_TOKEN";
    }
}
void main()
{
    TwinBuffer* tb = (TwinBuffer*) malloc(sizeof(struct twinbuffer));
    init("Lexer Test Cases/t2.txt", tb);
    while(true){
        TokenInfo* curToken = getNextToken(tb);
        if(curToken == NULL){
            continue;
        }
        if(!strcmp(curToken->lexeme, "EOF")){
            break;
        }
        if(curToken->token == TK_ERRPATTERN){
            printf("Line no %d : Error: Unknown pattern <%s>\n", curToken->lineNo,curToken->lexeme);
            continue;
        }
        if(curToken->token == TK_ERRUNK){
            printf("Line no %d : Error : Unknown Symbol <%s>\n", curToken->lineNo,curToken->lexeme);
            continue;
        }
        if(curToken->token == TK_ERRLENTWENTY){
            printf("Line no %d : Error : Variable Identifier is longer than the prescribed length of 20 characters.\n", curToken->lineNo);
            continue;
        }

        if(curToken->token == TK_ERRLENTHIRTY){
            printf("Line no %d : Error : Function Identifier is longer than the prescribed length of 30 characters.\n", curToken->lineNo);
            continue;
        }
        if(curToken->token == TK_ERRLENNUM){
            printf("Line no %d : Number is longer than the prescribed length of 17 characters.\n", curToken->lineNo);
            continue;
        }
        printf("Line no. %d\t Lexeme %s\t\tToken %s\n", curToken->lineNo, curToken->lexeme, tokenToString(curToken->token));
    }

    removeComments("Lexer Test Cases/t2.txt", "outputtest.txt");
}