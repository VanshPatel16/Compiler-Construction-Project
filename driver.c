
// contains main()
#include <stdio.h>
#include "lexer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char* tokenToString(enum token_enum token) {
    switch (token) {
        case LEX_TK_ASSIGNOP:   return "TK_ASSIGNOP";
        case LEX_TK_COMMENT:    return "TK_COMMENT";
        case LEX_TK_FIELDID:    return "TK_FIELDID";
        case LEX_TK_ID:         return "TK_ID";
        case LEX_TK_NUM:        return "TK_NUM";
        case LEX_TK_RNUM:       return "TK_RNUM";
        case LEX_TK_FUNID:      return "TK_FUNID";
        case LEX_TK_RUID:       return "TK_RUID";
        case LEX_TK_WITH:       return "TK_WITH";
        case LEX_TK_PARAMETERS: return "TK_PARAMETERS";
        case LEX_TK_END:        return "TK_END";
        case LEX_TK_WHILE:      return "TK_WHILE";
        case LEX_TK_UNION:      return "TK_UNION";
        case LEX_TK_ENDUNION:   return "TK_ENDUNION";
        case LEX_TK_DEFINETYPE: return "TK_DEFINETYPE";
        case LEX_TK_AS:         return "TK_AS";
        case LEX_TK_TYPE:       return "TK_TYPE";
        case LEX_TK_MAIN:       return "TK_MAIN";
        case LEX_TK_GLOBAL:     return "TK_GLOBAL";
        case LEX_TK_PARAMETER:  return "TK_PARAMETER";
        case LEX_TK_LIST:       return "TK_LIST";
        case LEX_TK_SQL:        return "TK_SQL";
        case LEX_TK_SQR:        return "TK_SQR";
        case LEX_TK_INPUT:      return "TK_INPUT";
        case LEX_TK_OUTPUT:     return "TK_OUTPUT";
        case LEX_TK_INT:        return "TK_INT";
        case LEX_TK_REAL:       return "TK_REAL";
        case LEX_TK_COMMA:      return "TK_COMMA";
        case LEX_TK_SEM:        return "TK_SEM";
        case LEX_TK_COLON:      return "TK_COLON";
        case LEX_TK_DOT:        return "TK_DOT";
        case LEX_TK_ENDWHILE:   return "TK_ENDWHILE";
        case LEX_TK_OP:         return "TK_OP";
        case LEX_TK_CL:         return "TK_CL";
        case LEX_TK_IF:         return "TK_IF";
        case LEX_TK_THEN:       return "TK_THEN";
        case LEX_TK_ENDIF:      return "TK_ENDIF";
        case LEX_TK_READ:       return "TK_READ";
        case LEX_TK_WRITE:      return "TK_WRITE";
        case LEX_TK_RETURN:     return "TK_RETURN";
        case LEX_TK_PLUS:       return "TK_PLUS";
        case LEX_TK_MINUS:      return "TK_MINUS";
        case LEX_TK_MUL:        return "TK_MUL";
        case LEX_TK_DIV:        return "TK_DIV";
        case LEX_TK_CALL:       return "TK_CALL";
        case LEX_TK_RECORD:     return "TK_RECORD";
        case LEX_TK_ENDRECORD:  return "TK_ENDRECORD";
        case LEX_TK_ELSE:       return "TK_ELSE";
        case LEX_TK_AND:        return "TK_AND";
        case LEX_TK_OR:         return "TK_OR";
        case LEX_TK_NOT:        return "TK_NOT";
        case LEX_TK_LT:         return "TK_LT";
        case LEX_TK_LE:         return "TK_LE";
        case LEX_TK_EQ:         return "TK_EQ";
        case LEX_TK_GT:         return "TK_GT";
        case LEX_TK_GE:         return "TK_GE";
        case LEX_TK_NE:         return "TK_NE";
        default:            return "UNKNOWN_TOKEN";
    }
}
void main()
{
    TwinBuffer* tb = (TwinBuffer*) malloc(sizeof(struct twinbuffer));
    init("Lexer Test Cases/t1.txt", tb);
    FILE* fp = fopen("lexer_result.txt", "w");
    while(true){
        TokenInfo* curToken = getNextToken(tb);
        if(curToken == NULL){
            continue;
        }
        if(strcmp(curToken->lexeme, "EOF") == 0){
            break;
        }
        fprintf(fp, "Line no. %d\t Lexeme %s\t\tToken %s\n", curToken->lineNo, curToken->lexeme, tokenToString(curToken->token));
    }

    // removeComments("Lexer Test Cases/t2.txt", "outputtest.txt");
}