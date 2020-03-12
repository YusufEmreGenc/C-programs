#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define MAX_ITEM 10

typedef enum {ALG, COND, EXPR, OP, PRE_OP, REL_OP, SET_OP, VAR} Symbol;

typedef struct Node3 *ptrNode3;
typedef struct Node3 Node3;
struct Node3{
    int option;     // This stores the information of branching option
    Symbol symbol;
    void **children;
    int* branchingOptionOfChildren;
};

typedef struct Node2 *ptrNode2;
typedef struct Node2 Node2;
struct Node2{
    int option;     // This stores the information of branching option
    Symbol symbol;
    void **children;
    int* branchingOptionOfChildren;
};

typedef struct Node1 *ptrNode1;
typedef struct Node1 Node1;
struct Node1{
    int option;     // This stores the information of branching option
    Symbol symbol;
    void **child;
    int* branchingOptionOfChildren;
};

typedef struct Node0 *ptrNode0;
typedef struct Node0 Node0;
struct Node0{
    Symbol symbol;
    char *data;
};

typedef struct TerminalData* ptrTerminalData;
typedef struct TerminalData TerminalNode;
struct TerminalData{
    char **op, **preOp, **relOp, **setOp, **var;
    int numOfOp, numOfPreOp, numOfRelOp, numOfSetOp, numOfVar;
};

void readFiles(FILE *opFile, FILE *preOpFile, FILE *relOpFile, FILE *setOpFile, FILE *varFile, ptrTerminalData data);
void readFile(FILE *file, char ***buffer, int *numOfItem);
void deallocBuffers(ptrTerminalData data);
void dealloacateTree(void *root, Symbol symbol, int option);
void branching(void *root, Symbol symbol, int option, ptrTerminalData data);
void print(void *root, Symbol symbol, int option);

int main() {
    // Initializing root
    ptrNode3 root = (ptrNode3) malloc(sizeof(Node3));
    root->symbol = COND;
    root->option = rand()%2;
    srand(time(0));

    FILE *opFile = fopen("op", "r+"), *preOpFile = fopen("pre_op", "r+"), *relOpFile = fopen("rel_op", "r+"), *setOpFile = fopen("set_op", "r+"), *varFile = fopen("var", "r+");
    if(!opFile || !preOpFile || !relOpFile || !setOpFile || !varFile){
        fprintf(stderr, "File cannot be opened!\n");
        return 1;
    }
    ptrTerminalData data = (ptrTerminalData) malloc(sizeof(TerminalNode));
    readFiles(opFile, preOpFile, relOpFile, setOpFile, varFile, data);
    branching(root, root->symbol, root->option, data);

    deallocBuffers(data);
    printf("if (");     print(root, root->symbol, root->option);       printf(") {}\n");
    dealloacateTree(root, root->symbol, root->option); free(root);
    fclose(opFile); fclose(preOpFile);  fclose(relOpFile);  fclose(setOpFile);  fclose(varFile);
    return 0;
}

void readFiles(FILE *opFile, FILE *preOpFile, FILE *relOpFile, FILE *setOpFile, FILE *varFile, ptrTerminalData data){
    readFile(opFile, &(data->op), &(data->numOfOp));
    readFile(preOpFile, &(data->preOp), &(data->numOfPreOp));
    readFile(relOpFile, &(data->relOp), &(data->numOfRelOp));
    readFile(setOpFile, &(data->setOp), &(data->numOfSetOp));
    readFile(varFile, &(data->var), &(data->numOfVar));
}

void readFile(FILE *file, char ***buffer, int *numOfItem){
    (*buffer) = (char **) malloc(sizeof(char*) * MAX_ITEM);
    char temp;
    while(!(feof(file))){
        temp = fgetc(file);
        if(temp == '\n' || temp == '\r' || temp == EOF)
            return;
        int numOfChar = 0, limitOfChar = 2;
        (*buffer)[*numOfItem] = (char *) malloc(sizeof(char) * limitOfChar);
        (*buffer)[*numOfItem][numOfChar++] = temp;
        while((temp = fgetc(file)) != '\n' && (temp != '\r') && !(feof(file))){
            if(numOfChar == limitOfChar-1){
                limitOfChar *= 2;
                (*buffer)[*numOfItem] = (char *) realloc((*buffer)[*numOfItem], sizeof(char) * limitOfChar);
            }
            (*buffer)[*numOfItem][numOfChar++] = temp;
        }
        (*buffer)[*numOfItem][numOfChar] = '\0';
        (*numOfItem)++;
    }
}

void deallocBuffers(ptrTerminalData data){
    int i;
    for (i = 0; i < data->numOfOp; i++)
        free((data->op)[i]);
    free(data->op);
    for (i = 0; i < data->numOfPreOp; i++)
        free((data->preOp)[i]);
    free(data->preOp);
    for (i = 0; i < data->numOfRelOp; i++)
        free((data->relOp)[i]);
    free(data->relOp);
    for (i = 0; i < data->numOfSetOp; i++)
        free((data->setOp)[i]);
    free(data->setOp);
    for (i = 0; i < data->numOfVar; i++)
        free((data->var)[i]);
    free(data->var);
    free(data);
}

// option parameter decides which branching option will be carried out, if no option available it will be -1
void branching(void* root, Symbol symbol, int option, ptrTerminalData data){
    if(symbol == COND){
        ptrNode3 node = (ptrNode3)root;
        node->children = (void **) malloc(sizeof(void *) * 3);
        node->branchingOptionOfChildren = (int *) malloc(sizeof(int) * 3);
        if(node->option == 0){
            ((node->children)[0]) = (ptrNode3) malloc(sizeof(Node3));
            ((ptrNode3)((node->children)[0]))->symbol = COND;
            ((ptrNode3)((node->children)[0]))->option = rand()%2;
            (node->branchingOptionOfChildren)[0] = ((ptrNode3)((node->children)[0]))->option;
            branching(((node->children)[0]), ((ptrNode3)((node->children)[0]))->symbol, ((ptrNode3)((node->children)[0]))->option, data);

            ((node->children)[1]) = (ptrNode0) malloc(sizeof(Node0));
            (node->branchingOptionOfChildren)[1] = -1;
            ((ptrNode0)((node->children)[1]))->symbol = SET_OP;
            branching(((node->children)[1]), ((ptrNode0)((node->children)[1]))->symbol, -1, data);

            ((node->children)[2]) = (ptrNode3) malloc(sizeof(Node3));
            ((ptrNode3)((node->children)[2]))->symbol = COND;
            ((ptrNode3)((node->children)[2]))->option = rand()%2;
            (node->branchingOptionOfChildren)[2] = ((ptrNode3)((node->children)[2]))->option;
            branching(((node->children)[2]), ((ptrNode3)((node->children)[2]))->symbol, ((ptrNode3)((node->children)[2]))->option, data);
        } else if (node->option == 1){
            int branchingOption = rand()%3;
            (node->branchingOptionOfChildren)[0] = branchingOption;
            if(branchingOption == 0){
                ((node->children)[0]) = (ptrNode3) malloc(sizeof(Node3));
                ((ptrNode3)((node->children)[0]))->symbol = EXPR;
                ((ptrNode3)((node->children)[0]))->option = branchingOption;
                branching((node->children)[0], ((ptrNode3)((node->children)[0]))->symbol, ((ptrNode3)((node->children)[0]))->option, data);
            } else if (branchingOption == 1){
                ((node->children)[0]) = (ptrNode2) malloc(sizeof(Node2));
                ((ptrNode2)((node->children)[0]))->symbol = EXPR;
                ((ptrNode2)((node->children)[0]))->option = branchingOption;
                branching((node->children)[0], ((ptrNode2)((node->children)[0]))->symbol, ((ptrNode2)((node->children)[0]))->option, data);
            } else if (branchingOption == 2){
                ((node->children)[0]) = (ptrNode1) malloc(sizeof(Node1));
                ((ptrNode1)((node->children)[0]))->symbol = EXPR;
                ((ptrNode1)((node->children)[0]))->option = branchingOption;
                branching((node->children)[0], ((ptrNode1)((node->children)[0]))->symbol, ((ptrNode1)((node->children)[0]))->option, data);
            }

            ((node->children)[1]) = (ptrNode0) malloc(sizeof(Node0));
            ((ptrNode0)((node->children)[1]))->symbol = REL_OP;
            (node->branchingOptionOfChildren)[1] = -1;
            branching(((node->children)[1]), ((ptrNode0)((node->children)[1]))->symbol, -1, data);

            branchingOption = rand()%3;
            (node->branchingOptionOfChildren)[2] = branchingOption;
            if(branchingOption == 0){
                ((node->children)[2]) = (ptrNode3) malloc(sizeof(Node3));
                ((ptrNode3)((node->children)[2]))->symbol = EXPR;
                ((ptrNode3)((node->children)[2]))->option = branchingOption;
                branching((node->children)[2], ((ptrNode3)((node->children)[2]))->symbol, ((ptrNode3)((node->children)[2]))->option, data);
            } else if (branchingOption == 1){
                ((node->children)[2]) = (ptrNode2) malloc(sizeof(Node2));
                ((ptrNode2)((node->children)[2]))->symbol = EXPR;
                ((ptrNode2)((node->children)[2]))->option = branchingOption;
                branching((node->children)[2], ((ptrNode2)((node->children)[2]))->symbol, ((ptrNode2)((node->children)[2]))->option, data);
            } else if (branchingOption == 2){
                ((node->children)[2]) = (ptrNode1) malloc(sizeof(Node1));
                ((ptrNode1)((node->children)[2]))->symbol = EXPR;
                ((ptrNode1)((node->children)[2]))->option = branchingOption;
                branching((node->children)[2], ((ptrNode1)((node->children)[2]))->symbol, ((ptrNode1)((node->children)[2]))->option, data);
            }
        }
    } else if (symbol == EXPR){
        if(option == 0){
            ptrNode3 node = (ptrNode3)root;
            node->children = (void **) malloc(sizeof(void *) * 3);
            node->branchingOptionOfChildren = (int *) malloc(sizeof(int) * 3);

            int branchingOption = rand()%3;
            (node->branchingOptionOfChildren)[0] = branchingOption;
            if(branchingOption == 0){
                ((node->children)[0]) = (ptrNode3) malloc(sizeof(Node3));
                ((ptrNode3)((node->children)[0]))->symbol = EXPR;
                ((ptrNode3)((node->children)[0]))->option = branchingOption;
                branching((node->children)[0], ((ptrNode3)((node->children)[0]))->symbol, ((ptrNode3)((node->children)[0]))->option, data);
            } else if (branchingOption == 1){
                ((node->children)[0]) = (ptrNode2) malloc(sizeof(Node2));
                ((ptrNode2)((node->children)[0]))->symbol = EXPR;
                ((ptrNode2)((node->children)[0]))->option = branchingOption;
                branching((node->children)[0], ((ptrNode2)((node->children)[0]))->symbol, ((ptrNode2)((node->children)[0]))->option, data);
            } else if (branchingOption == 2){
                ((node->children)[0]) = (ptrNode1) malloc(sizeof(Node1));
                ((ptrNode1)((node->children)[0]))->symbol = EXPR;
                ((ptrNode1)((node->children)[0]))->option = branchingOption;
                branching((node->children)[0], ((ptrNode1)((node->children)[0]))->symbol, ((ptrNode1)((node->children)[0]))->option, data);
            }

            ((node->children)[1]) = (ptrNode0) malloc(sizeof(Node0));
            ((ptrNode0)((node->children)[1]))->symbol = OP;
            (node->branchingOptionOfChildren)[1] = -1;
            branching(((node->children)[1]), ((ptrNode0)((node->children)[1]))->symbol, -1, data);

            branchingOption = rand()%3;
            (node->branchingOptionOfChildren)[2] = branchingOption;
            if(branchingOption == 0){
                ((node->children)[2]) = (ptrNode3) malloc(sizeof(Node3));
                ((ptrNode3)((node->children)[2]))->symbol = EXPR;
                ((ptrNode3)((node->children)[2]))->option = branchingOption;
                branching((node->children)[2], ((ptrNode3)((node->children)[2]))->symbol, ((ptrNode3)((node->children)[2]))->option, data);
            } else if (branchingOption == 1){
                ((node->children)[2]) = (ptrNode2) malloc(sizeof(Node2));
                ((ptrNode2)((node->children)[2]))->symbol = EXPR;
                ((ptrNode2)((node->children)[2]))->option = branchingOption;
                branching((node->children)[2], ((ptrNode2)((node->children)[2]))->symbol, ((ptrNode2)((node->children)[2]))->option, data);
            } else if (branchingOption == 2){
                ((node->children)[2]) = (ptrNode1) malloc(sizeof(Node1));
                ((ptrNode1)((node->children)[2]))->symbol = EXPR;
                ((ptrNode1)((node->children)[2]))->option = branchingOption;
                branching((node->children)[2], ((ptrNode1)((node->children)[2]))->symbol, ((ptrNode1)((node->children)[2]))->option, data);
            }
        } else if (option == 1){
            ptrNode2 node = (ptrNode2)root;
            node->children = (void **) malloc(sizeof(void *) * 2);
            node->branchingOptionOfChildren = (int *) malloc(sizeof(int) * 2);

            ((node->children)[0]) = (ptrNode0) malloc(sizeof(Node0));
            ((ptrNode0)((node->children)[0]))->symbol = PRE_OP;
            (node->branchingOptionOfChildren)[0] = -1;
            branching(((node->children)[0]), ((ptrNode0)((node->children)[0]))->symbol, -1, data);

            int branchingOption = rand()%3;
            (node->branchingOptionOfChildren)[1] = branchingOption;
            if(branchingOption == 0){
                ((node->children)[1]) = (ptrNode3) malloc(sizeof(Node3));
                ((ptrNode3)((node->children)[1]))->symbol = EXPR;
                ((ptrNode3)((node->children)[1]))->option = branchingOption;
                branching((node->children)[1], ((ptrNode3)((node->children)[1]))->symbol, ((ptrNode3)((node->children)[1]))->option, data);
            } else if (branchingOption == 1){
                ((node->children)[1]) = (ptrNode2) malloc(sizeof(Node2));
                ((ptrNode2)((node->children)[1]))->symbol = EXPR;
                ((ptrNode2)((node->children)[1]))->option = branchingOption;
                branching((node->children)[1], ((ptrNode2)((node->children)[1]))->symbol, ((ptrNode2)((node->children)[1]))->option, data);
            } else if (branchingOption == 2){
                ((node->children)[1]) = (ptrNode1) malloc(sizeof(Node1));
                ((ptrNode1)((node->children)[1]))->symbol = EXPR;
                ((ptrNode1)((node->children)[1]))->option = branchingOption;
                branching((node->children)[1], ((ptrNode1)((node->children)[1]))->symbol, ((ptrNode1)((node->children)[1]))->option, data);
            }
        } else if (option == 2){
            ptrNode1 node = (ptrNode1)root;
            node->child = (void **) malloc(sizeof(void *));
            node->branchingOptionOfChildren = (int *) malloc(sizeof(int));

            ((node->child)[0]) = (ptrNode0) malloc(sizeof(Node0));
            ((ptrNode0)((node->child)[0]))->symbol = VAR;
            (node->branchingOptionOfChildren)[0] = -1;
            branching(((node->child)[0]), ((ptrNode0)((node->child)[0]))->symbol, -1, data);
        }
    } else{     // Terminal symbols
        ptrNode0 node = (ptrNode0)root;
        if(symbol == OP){
            int index = rand()%(data->numOfOp);
            node->data = (char *) malloc(sizeof(char) * (strlen((data->op)[index])+1));
            strcpy(node->data, (data->op)[index]);
        } else if (symbol == PRE_OP){
            int index = rand()%(data->numOfPreOp);
            node->data = (char *) malloc(sizeof(char) * (strlen((data->preOp)[index])+1));
            strcpy(node->data, (data->preOp)[index]);
        } else if (symbol == REL_OP){
            int index = rand()%(data->numOfRelOp);
            node->data = (char *) malloc(sizeof(char) * (strlen((data->relOp)[index])+1));
            strcpy(node->data, (data->relOp)[index]);
        } else if (symbol == SET_OP){
            int index = rand()%(data->numOfSetOp);
            node->data = (char *) malloc(sizeof(char) * (strlen((data->setOp)[index])+1));
            strcpy(node->data, (data->setOp)[index]);
        } else if (symbol == VAR){
            int index = rand()%(data->numOfVar);
            node->data = (char *) malloc(sizeof(char) * (strlen((data->var)[index])+1));
            strcpy(node->data, (data->var)[index]);
        }
    }
}

void dealloacateTree(void *root, Symbol symbol, int option){
    if(symbol == COND){
        ptrNode3 node = (ptrNode3)root;
        if(node->option == 0){
            dealloacateTree((node->children)[0], COND, (node->branchingOptionOfChildren)[0]);
            dealloacateTree((node->children)[1], SET_OP, (node->branchingOptionOfChildren)[1]);
            dealloacateTree((node->children)[2], COND, (node->branchingOptionOfChildren)[2]);
            free((node->children)[0]);
            free((node->children)[1]);
            free((node->children)[2]);
        } else if(node->option == 1){
            dealloacateTree((node->children)[0], EXPR, (node->branchingOptionOfChildren)[0]);
            dealloacateTree((node->children)[1], REL_OP, (node->branchingOptionOfChildren)[1]);
            dealloacateTree((node->children)[2], EXPR, (node->branchingOptionOfChildren)[2]);
            free((node->children)[0]);
            free((node->children)[1]);
            free((node->children)[2]);
        }
        free(node->children);
        free(node->branchingOptionOfChildren);
    } else if (symbol == EXPR){
        if(option == 0){
            ptrNode3 node = (ptrNode3)root;
            dealloacateTree((node->children)[0], EXPR, (node->branchingOptionOfChildren)[0]);
            dealloacateTree((node->children)[1], REL_OP, (node->branchingOptionOfChildren)[1]);
            dealloacateTree((node->children)[2], EXPR, (node->branchingOptionOfChildren)[2]);
            free((node->children)[0]);
            free((node->children)[1]);
            free((node->children)[2]);
            free(node->children);
            free(node->branchingOptionOfChildren);
        } else if (option == 1){
            ptrNode2 node = (ptrNode2)root;
            dealloacateTree((node->children)[0], PRE_OP, (node->branchingOptionOfChildren)[0]);
            dealloacateTree((node->children)[1], EXPR, (node->branchingOptionOfChildren)[1]);
            free((node->children)[0]);
            free((node->children)[1]);
            free(node->children);
            free(node->branchingOptionOfChildren);
        } else if (option == 2){
            ptrNode1 node = (ptrNode1)root;
            dealloacateTree((node->child)[0], VAR, (node->branchingOptionOfChildren)[0]);
            free((node->child)[0]);
            free(node->child);
            free(node->branchingOptionOfChildren);
        }
    } else if (symbol == OP || symbol == PRE_OP || symbol == REL_OP || symbol == SET_OP || symbol == VAR){
        ptrNode0 node = (ptrNode0)root;
        free(node->data);
    }
}

void print(void *root, Symbol symbol, int option){//
    if (symbol == COND){
        ptrNode3 node = (ptrNode3)root;
        printf("(");
        if(option == 0){
            print(((node->children)[0]), COND, (node->branchingOptionOfChildren)[0]);
            print(((node->children)[1]), SET_OP, (node->branchingOptionOfChildren)[1]);
            print(((node->children)[2]), COND, (node->branchingOptionOfChildren)[2]);
        } else if (option == 1){
            print(((node->children)[0]), EXPR, (node->branchingOptionOfChildren)[0]);
            print(((node->children)[1]), REL_OP, (node->branchingOptionOfChildren)[1]);
            print(((node->children)[2]), EXPR, (node->branchingOptionOfChildren)[2]);
        }
        printf(")");
    } else if (symbol == EXPR){
        printf("(");
        if(option == 0){
            ptrNode3 node = (ptrNode3)root;
            print(((node->children)[0]), EXPR, (node->branchingOptionOfChildren)[0]);
            print(((node->children)[1]), OP, (node->branchingOptionOfChildren)[1]);
            print(((node->children)[2]), EXPR, (node->branchingOptionOfChildren)[2]);
        } else if (option == 1){
            ptrNode2 node = (ptrNode2)root;
            print(((node->children)[0]), PRE_OP, (node->branchingOptionOfChildren)[0]);
            print(((node->children)[1]), EXPR, (node->branchingOptionOfChildren)[1]);
        } else if (option == 2){
            ptrNode1 node = (ptrNode1)root;
            print(((node->child)[0]), VAR, (node->branchingOptionOfChildren)[0]);
        }
        printf(")");
    } else if (symbol == OP || symbol == PRE_OP || symbol == REL_OP || symbol == SET_OP || symbol == VAR){
        ptrNode0 node = (ptrNode0)root;
        printf("%s", node->data);
    }
}