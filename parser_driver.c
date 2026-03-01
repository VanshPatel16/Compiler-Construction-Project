#include "parser.h"
#include "lexer.h"

void printGrammar(Grammar* grammar, const char* outputFile){
    FILE* fp = fopen(outputFile, "w");
    if(!fp){
        printf("Cannot open output file for writing\n");
        return;
    }

    for(int i = 0; i < NUMRULES ; i++){
        if(grammar->rules[i].rlen > 0){
            // Use the stored LHS from the struct
            fprintf(fp, "%s > ", getTokenString(grammar->rules[i].LHS)); 
            
            for(int j = 0; j < grammar->rules[i].rlen; j++){
                for(int k = 0; k < grammar->rules[i].tlen[j]; k++){
                    fprintf(fp, "%s", getTokenString(grammar->rules[i].RHS[j][k]));
                    
                    // Only print comma if it's not the last token
                    if (k < grammar->rules[i].tlen[j] - 1) fprintf(fp, ", ");
                }
                // Only print the OR pipe if it's not the last alternative
                if (j < grammar->rules[i].rlen - 1) fprintf(fp, " | ");
            }
            fprintf(fp, "\n"); // Add a newline after each rule
        }
    }
    
    fclose(fp); // Save and close the file
}

void printFirstSets(Grammar* G, const char* outputFile){
    FILE* fp = fopen(outputFile, "w");
    if(!fp){
        printf("Cannot open output file for writing\n");
        return;
    }

    for(int symbol = 0; symbol < NUMSYMBOLS; symbol++){
        if(!isTerminal(symbol)){
            fprintf(fp, "FIRST(%s) : ", getTokenString(symbol));
            for(int non_terminal = 0;non_terminal < NUMTOKENS; non_terminal++){
                if(G->first[symbol][non_terminal] && (non_terminal != EPSILON)){
                    fprintf(fp, "%s | ",getTokenString(non_terminal));
                }
            }
            if(G->hasEPSILONFirst[symbol])
                fprintf(fp, "EPSILON");
            fprintf(fp, "\n"); // Add a newline after each rule
        }
    }
    fclose(fp);
}

void printFollowSets(Grammar* G, const char* outputFile){
        FILE* fp = fopen(outputFile, "w");
    if(!fp){
        printf("Cannot open output file for writing\n");
        return;
    }

    for(int symbol = 0; symbol < NUMSYMBOLS; symbol++){
        if(!isTerminal(symbol)){
            fprintf(fp, "FOLLOW(%s) : ", getTokenString(symbol));
            for(int non_terminal = 0;non_terminal < NUMTOKENS; non_terminal++){
                if(G->follow[symbol][non_terminal] && (non_terminal != EPSILON)){
                    fprintf(fp, "%s | ",getTokenString(non_terminal));
                }
            }
            // if(G->hasEPSILONFirst[symbol])
            //     fprintf(fp, "EPSILON");
            fprintf(fp, "\n"); // Add a newline after each rule
        }
    }
    fclose(fp);
}

void printParseTable(Grammar* G, const char* outputFile)
{
    FILE* fp = fopen(outputFile, "w");

    if(!fp)
    {
        printf("Cannot open output file for writing\n");
        return;
    }

    fprintf(fp, "=========== LL(1) PARSE TABLE ===========\n\n");

    for(int nt = 0; nt < NUMSYMBOLS; nt++)
    {
        if(isTerminal(nt)) continue;

        fprintf(fp, "%s :\n", getTokenString(nt));

        for(int terminal = 0; terminal < NUMTOKENS; terminal++)
        {
            fprintf(fp, "  %-20s -> ", getTokenString(terminal));

            if(G->parseTable[nt][terminal][0] == PARSER_ERROR)
            {
                fprintf(fp, "PARSER_ERROR");
            }
            else
            {
                for(int k = 0; k < MAXTLEN; k++)
                {
                    grammarSymbol sym =
                        G->parseTable[nt][terminal][k];

                    if(sym == PARSER_ERROR)
                        break;

                    fprintf(fp, "%s ",
                            getTokenString(sym));
                }
            }

            fprintf(fp, "\n");
        }

        fprintf(fp, "\n");
    }

    fclose(fp);

    printf("Parse table written to %s\n", outputFile);
}

void printParseTree(Node* root, Node* parent, FILE* fp){
    if(!root){
        fprintf(fp, "called by %s\n", getTokenString(parent->nodeSymbol));
        fprintf(fp, "Root is null\n");
        return;
    }
    if(root->isLeafNode){
        if(root->nodeSymbol == EPSILON)
            return;
        if(root->nodeSymbol == TK_NUM || root->nodeSymbol == TK_RNUM){
            fprintf(fp, "%lf\n", root->numValue);
        }else{
            fprintf(fp, "%s\n", root->lexeme);
        }
        return;
    }
    // printParseTree(root->children[0], root, fp);

    // fprintf(fp, "Parent %s | Symbol : %s\n\n", getTokenString(parent->nodeSymbol), getTokenString(root->nodeSymbol));
    for(int i = 0;i < root->numChildren;i++){
        int t = root->lineNo;
        printParseTree(root->children[i], root, fp);
    }
    return;
}

int main(){
    removeComments("Parser Test Cases/t6.txt", "clean_t6.txt");
    printf("Cleaned the comments\n");
    Grammar* grammar = constructGrammar("rules.txt");
    calculateFirstSets(grammar);
    printf("Constructed first sets\n");
    printFirstSets(grammar, "first_output.txt");
    calculateFollowSets(grammar);
    printf("Follow sets computed\n");
    printFollowSets(grammar, "follow_output.txt");
    populateParseTable(grammar);
    printf("Parse table populated successfully\n");
    printParseTable(grammar, "parsetable.txt");
    Node* root = constructParseTree(grammar, "clean_t6.txt");
    printf("Parse tree computed\n");
    // FILE* fp = fopen("parse_tree_output.txt", "w");
    // printParseTree(root, root, fp);
    // Don't forget to free what you malloc!
    free(grammar); 
    return 0;
}