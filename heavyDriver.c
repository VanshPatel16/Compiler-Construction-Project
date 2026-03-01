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
    char * ruleFile = "rules.txt";
    printf("Testcase file : %s\n", argv[1]);
    printf("Output file : %s\n", argv[2]);
    while(true){
        int op;
        scanf("%d", &op);
        if(op==0){
            break;
        }
        else if(op==1){
            printf("Removing comments");
            removeComments(ipFile);
            printf("\n");
        }
        else if(op==2){
            TwinBuffer* tb = (TwinBuffer*) malloc(sizeof(struct twinbuffer));
            init(ipFile, tb);
            while(true){
                TokenInfo* curToken = getNextToken(tb);
                if(curToken == NULL){
                    continue;
                }
                if(strcmp(curToken->lexeme, "EOF") == 0){
                    break;
                }
                printf("Token: %-15s | Lexeme: %-15s | Line: %-5d\n", getTokenString(curToken->token), curToken->lexeme, curToken->lineNo);
            }
            free(tb);
        }
        else if(op==3){
            Grammar* G=constructGrammar("rules.txt");
            calculateFirstSets(G);
            calculateFollowSets(G);
            populateParseTable(G);
            Node* root = constructParseTree(G, ipFile);
            FILE* fp = fopen(opFile, "w");
            if(root!=NULL){
                printf("entered printing parse tree\n");
                printParseTree(root, root, fp);
                fflush(fp);
            }
            
            free(G);
        }
        else if(op==4){
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
        }
    }
}