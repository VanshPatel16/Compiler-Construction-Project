/*
 * Group ID - 2
 *  Vansh Patel         - 2023A7PS0629P
 *  Darshil Chauhan     - 2023A7PS0609P
 *  Adityasinh Solanki  - 2023A7PS0615P
 *  Sneh Desai          - 2023A7PS0498P
 *  Gautam Singhvie     - 2023A7PS0612P
 *  Kanav Kapoor        - 2023A7TS0628P
 */
#include <time.h>
#include "lexer.h"
#include "parser.h"

int main(int argc, char* argv[]){
    if (argc < 3) {
        printf("Usage: %s <testcaseFile> <parsetreeOutFile>\n", argv[0]);
        return 1;
    }
    // Assign arguments to your variables
    char * ipFile = argv[1];
    char * opFile = argv[2];
    char * ruleFile = "grammar.txt";
    printf("Testcase file : %s\n", argv[1]);
    printf("Output file : %s\n", argv[2]);
    printf("Implementation Status : \n");
    printf("\t1.) First and Follow set computation Automated (refer calculateFirstSets() and calculateFollowSets() in parser.c file)\n");
    printf("\t2.) Both Lexical and Syntax Analyzer modules implemented with no segmentation fault (refer lexer.c and parser.c)\n");
    printf("\t3.) Module works with all the provided testcases (from t1 to t6)\n");
    printf("\t4.) Parse Tree computed without errors for error-free code and will be printed to the output file %s, errors will be displayed on the console otherwise\n", argv[2]);
    while(true){
        printf("Interactor\n\t0 : Quit\n\t1 : For removing comments from %s and printing to the console\n\t2 : For printing the lexer output to the console\n\t3 : For parsing the input and printing parse tree to file %s\n\t4 : Measure the execution time for parsing the input and constructing the parse tree\n", argv[1], argv[2]);
        int op;
        printf("Input operation : ");
        scanf("%d", &op);
        
        if(op == 0){
            printf("\n\t\tRegards from Group 2!\n");
            break;
        }
        else if(op == 1){
            removeComments(ipFile);
            printf("\n\t\tRemoved comments\n");
        }
        else if(op == 2){
            TwinBuffer* tb = (TwinBuffer*) malloc(sizeof(struct twinbuffer));
            init(ipFile, tb);
            while(true){
                TokenInfo* curToken = getNextToken(tb);
                if(curToken == NULL){
                    continue;
                }
                if(strcmp(curToken->lexeme, "EOF") == 0){
                    if(curToken->token == LEX_TK_COMMENT){
                        curToken->lexeme = "%";
                        printf("Token: %-15s | Lexeme: %-15s | Line: %-5d\n", getTokenString(curToken->token), curToken->lexeme, curToken->lineNo);
                    }
                    break;
                }
                printf("Token: %-15s | Lexeme: %-15s | Line: %-5d\n", getTokenString(curToken->token), curToken->lexeme, curToken->lineNo);
            }
            free(tb);
            printf("\n\t\tLexer Output Printing Complete\n");
        }
        else if(op == 3){
            Grammar* G=constructGrammar("grammar.txt");
            calculateFirstSets(G);
            calculateFollowSets(G);
            populateParseTable(G);
            Node* root = constructParseTree(G, ipFile);
            FILE* fp = fopen(opFile, "w");
            if(root!=NULL){
                printParseTree(root, root, fp);
                printf("\n\t\tParsed the input using the grammar and printed the parse tree to file %s\n", argv[2]);
                fflush(fp);
            }else{
                printf("\n\t\tParsed the input using the grammar and printed the flagged the errors.\n");
            }
            free(G);
        }
        else if(op == 4){
            clock_t start_time, end_time;
            double total_CPU_time, total_CPU_time_in_seconds;
            start_time = clock();
            Grammar* G = constructGrammar(ruleFile);
            calculateFirstSets(G);
            calculateFollowSets(G);
            populateParseTable(G);
            Node* root = constructParseTree(G, ipFile);
            end_time = clock();
            total_CPU_time = (double) (end_time - start_time);
            total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
            printf("\nTotal CPU time (ticks): %f\n", total_CPU_time);
            printf("Total CPU time (seconds): %f\n", total_CPU_time_in_seconds);
            if(G) free(G);
        }else{
            printf("Invalid input! Please enter only numbers 0 to 4\n");
        }
        printf("-----------------------------------------------------------------------------------------------------------------------\n");
    }
}