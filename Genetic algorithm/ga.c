#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    char data;
}element;

typedef struct Node* ptrNode;
typedef struct Node Node;
struct Node{
    void *data;
    void *next;
};

typedef struct GeneNode *ptrGeneNode;
typedef struct GeneNode GeneNode;
struct GeneNode{
    element data;
};

typedef struct ChromosomeNode ChromosomeNode;
typedef struct ChromosomeNode *ptrChromosomeNode;
struct ChromosomeNode{
    ptrNode data;
    int fitness;
    int ranking;
};

void readPopulation(ptrNode *population, FILE *popFile, int probSize, int popSize, int *bestFitness, char *bestChromosome);
void printPopulation(ptrNode population);
void printChromosome(ptrChromosomeNode chromosomeNode);
void addRear(ptrNode *rear, void *node);
void deallocBuffers(ptrNode *population, int **mutate, int ***xover, int ****selection, char **bestChromosome, int popSize, int maxGene);
void readFiles(int **mutate, int ***xover, int ****selection, FILE *xoFile, FILE *selFile, FILE *mutFile, int popSize, int maxGen);
void splitList(ptrNode head, ptrNode *a, ptrNode *b, int popSize);
ptrNode mergeLists(ptrNode a, ptrNode b);
void mergeSortPopulation(ptrNode *population, int popSize);
void calculateRankings(ptrNode population, int popSize);
void iterateGene(ptrNode *population, int ***selection, int **xover, int *mutate, int popSize, int probSize, int maxGene, int *bestFitness, char *bestChromosome);
void xOver(ptrChromosomeNode chromosome1, ptrChromosomeNode chromosome2, int rangeX, int rangeY);
void mutateChromosome(ptrNode population, int index);
void calculateFitnesses(ptrNode population, int probSize);
void copyChromosomeToStr(char *str, ptrChromosomeNode chromosome);

int main(int argc, char **argv) {
    int probSize = atoi(argv[1]), popSize = atoi(argv[2]), maxGene = atoi(argv[3]), i;
    FILE *popFile = fopen("population", "r+") , *mutFile = fopen("mutate", "r+"), *selFile = fopen("selection", "r+"), *xoFile = fopen("xover", "r+");
    if(!popFile || !mutFile || !selFile || !xoFile){
        fprintf(stderr, "File cannot be opened!\n");
        return 1;
    }
    int *mutate, **xover, ***selection;
    int bestFitness;
    char *bestChromosome = (char*) malloc(sizeof(char) * (probSize * 2));

    ptrNode population = (ptrNode) malloc(sizeof(Node));
    readPopulation(&population, popFile, probSize, popSize, &bestFitness, bestChromosome);
    readFiles(&mutate, &xover, &selection, xoFile, selFile, mutFile, popSize, maxGene);

    iterateGene(&population, selection, xover, mutate, popSize, probSize, maxGene, &bestFitness, bestChromosome);

    deallocBuffers(&population, &mutate, &xover, &selection, &bestChromosome, popSize, maxGene);
    fclose(popFile);    fclose(mutFile);    fclose(selFile);    fclose(xoFile);
    return 0;
}

void readPopulation(ptrNode *population, FILE *popFile, int probSize, int popSize, int *bestFitness, char *bestChromosome){
    int isFirst = 1;    // This variable is for initializing the bestFitness with the first chromosomes fitness value
    char *line = (char *) malloc(sizeof(char) * probSize * 2);
    ptrNode rearPopulation = *population;
    while(fgets(line, probSize*2, popFile)){    // gets chromosome
        int i = probSize - 1;
        char *token = NULL, *current = line;
        ptrChromosomeNode currentChromosome = (ptrChromosomeNode) malloc(sizeof(ChromosomeNode));
        currentChromosome->data = (ptrNode) malloc(sizeof(Node));
        currentChromosome->fitness = 0;
        currentChromosome->ranking = 0;
        ptrNode rearChromosome = currentChromosome->data;
        while((token = strtok_r(current, ":", &current))){      // gets genes
            currentChromosome->fitness += atoi(token) << (i--);
            ptrGeneNode geneNode = (ptrGeneNode) malloc(sizeof(GeneNode));
            geneNode->data.data = token[0];
            addRear(&rearChromosome, (ptrGeneNode)geneNode);
        }
        rearChromosome->next = NULL;
        if(isFirst){
            isFirst = 0;
            *bestFitness = currentChromosome->fitness;
        }

        // Ignoring the dangling head node in chromosome
        ptrNode temp = currentChromosome->data;
        currentChromosome->data = currentChromosome->data->next;
        free(temp);

        addRear(&rearPopulation, (ptrChromosomeNode)currentChromosome);
        fgetc(popFile);
    }
    rearPopulation->next = NULL;

    // Ignoring the dangling head node in population
    ptrNode temp = *population;
    *population = (*population)->next;
    free(temp);

    mergeSortPopulation(population, popSize);
    // updating the best chromosome
    if(*bestFitness >= ((ptrChromosomeNode)(population[0]->data))->fitness){
        *bestFitness = ((ptrChromosomeNode)(population[0]->data))->fitness;
        copyChromosomeToStr(bestChromosome, (ptrChromosomeNode)(population[0]->data));
    }
    printf("GENERATION: 0\n");
    printPopulation(*population);
    printf("Best chromosome found so far: ");
    printf("%s -> %d\n", bestChromosome, *bestFitness);

    free(line);
}

// It is a generic function for linked list. For any type linked list can use this function
void addRear(ptrNode *node, void *data){
    ptrNode newNode = (ptrNode) malloc(sizeof(Node));
    newNode->data = data;

    (*node)->next = newNode;
    *node = (*node)->next;
}

void printPopulation(ptrNode population){
    while(population != NULL){
        printChromosome((ptrChromosomeNode)(population->data));
        population = population->next;
    }
}

void printChromosome(ptrChromosomeNode chromosomeNode){
    ptrNode ptr = chromosomeNode->data;
    while((ptr->next) != NULL){
        printf("%c:", ((ptrGeneNode)(ptr->data))->data.data);
        ptr = ptr->next;
    }
    printf("%c -> %d\n", ((ptrGeneNode)(ptr->data))->data.data, chromosomeNode->fitness);
}

void deallocBuffers(ptrNode *population, int **mutate, int ***xover, int ****selection, char **bestChromosome, int popSize, int maxGene){
    // Deallocation linked lists - population part
    while((*population) != NULL){
        while (((ptrChromosomeNode)((*population)->data))->data != NULL){
            free(((ptrChromosomeNode)((*population)->data))->data->data);
            ptrNode temp = ((ptrChromosomeNode)((*population)->data))->data;
            ((ptrChromosomeNode)((*population)->data))->data = ((ptrChromosomeNode)((*population)->data))->data->next;
            free(temp);
        }

        free((ptrChromosomeNode)((*population)->data));
        ptrNode temp = (*population);
        (*population) = (*population)->next;
        free(temp);
    }

    // Deallocation parts that are read from files
    int i, j;
    free(*mutate);
    for(i = 0; i < maxGene; i++)
        free((*xover)[i]);
    free((*xover));
    for(i = 0; i < maxGene; i++){
        for(j = 0; j < popSize/2; j++)
            free((*selection)[i][j]);
        free((*selection)[i]);
    }
    free((*selection));

    free(*bestChromosome);
}

void readFiles(int **mutate, int ***xover, int ****selection, FILE *xoFile, FILE *selFile, FILE *mutFile, int popSize, int maxGen){
    int i, j;
    // Memory allocation
    (*mutate) = (int*) malloc(sizeof(int) * maxGen);
    (*xover) = (int**) malloc(sizeof(int*) * maxGen);
    for(i = 0; i < maxGen; i++)
        (*xover)[i] = (int*) malloc(sizeof(int) * 2);
    (*selection) = (int***) malloc(sizeof(int**) * maxGen);
    for(i = 0; i < maxGen; i++){
        (*selection)[i] = (int**) malloc(sizeof(int*) * (popSize/2));
        for(j = 0; j < popSize/2; j++)
            (*selection)[i][j] = (int*) malloc(sizeof(int) * 2);
    }
    (*selection)[0][0][0] = 0;
    // Reading files
    for(i = 0; i < maxGen; i++){
        for(j = 0; j < (popSize/2); j++){
            fscanf(selFile, "%d:%d", &(*selection)[i][j][0], &(*selection)[i][j][1]);
        }
    }
    for(i = 0; i < maxGen; i++)
        fscanf(xoFile, "%d:%d", &(*xover)[i][0], &(*xover)[i][1]);
    for(i = 0; i < maxGen; i++)
        fscanf(mutFile, "%d", &(*mutate)[i]);
}

void mergeSortPopulation(ptrNode *population, int popSize){
    ptrNode head = *population;
    ptrNode a, b;

    if(head == NULL || head->next == NULL) return;
    else{
        splitList(head, &a, &b, popSize);

        mergeSortPopulation(&a, popSize/2);
        mergeSortPopulation(&b, (popSize+1)/2);

        *population = mergeLists(a, b);
    }
}

// a points to first half, and b points to second half
void splitList(ptrNode head, ptrNode *a, ptrNode *b, int popSize){
    *a = head;
    int i;
    for (i = 0; i < (popSize/2 - 1); i++)     head = head->next;
    *b = head->next;
    head->next = NULL;
}

ptrNode mergeLists(ptrNode a, ptrNode b){
    ptrNode result = NULL;

    if(a == NULL) return b;
    if(b == NULL) return a;

    if(((ptrChromosomeNode)(a->data))->fitness <= ((ptrChromosomeNode)(b->data))->fitness){
        result = a;
        result->next = mergeLists(a->next, b);
    } else{
        result = b;
        result->next = mergeLists(a, b->next);
    }
    return result;
}

void calculateRankings(ptrNode population, int popSize){
    int bestFitness = ((ptrChromosomeNode)(population[0].data))->fitness;
    ptrNode ptr = population;
    while (ptr->next != NULL)     ptr = ptr->next;
    int worstFitness = ((ptrChromosomeNode)(ptr->data))->fitness;

    if(bestFitness == worstFitness){
        ptr = population;
        while (ptr != NULL){
            ((ptrChromosomeNode)(ptr->data))->ranking = 0;
            ptr = ptr->next;
        }
    }

    while(population != NULL){
        ((ptrChromosomeNode)(population->data))->ranking = (((ptrChromosomeNode)(population->data))->fitness - bestFitness) / (worstFitness - bestFitness);
        population = population->next;
    }
}

void iterateGene(ptrNode *population, int ***selection, int **xover, int *mutate, int popSize, int probSize, int maxGene, int *bestFitness, char *bestChromosome){
    int i, j;
    for (i = 0;  i < maxGene; i++){
        int mutateIndex = mutate[i];
        int xoverRangeX = xover[i][0], xoverRangeY = xover[i][1];
        for (j = 0; j < (popSize/2); j++){
            int first = selection[i][j][0], second = selection[i][j][1];
            ptrNode ptrNode1, ptrNode2, ptr = *population;

            // Selection
            int bigger = first > second ? first : second, smaller = first < second ? first : second, x;
            for (x = 1; x < bigger; x++){
                if(smaller == x)
                    ptrNode1 = ptr;
                ptr = ptr->next;
            }
            ptrNode2 = ptr;

            // Cross over
            xOver(ptrNode1->data, ptrNode2->data, xoverRangeX, xoverRangeY);
        }
        // mutate
        mutateChromosome(*population, mutateIndex);

        calculateFitnesses(*population, probSize);
        mergeSortPopulation(population, popSize);
        // updating the best chromosome
        if(*bestFitness >= ((ptrChromosomeNode)(population[0]->data))->fitness){
            *bestFitness = ((ptrChromosomeNode)(population[0]->data))->fitness;
            copyChromosomeToStr(bestChromosome, (ptrChromosomeNode)(population[0]->data));
        }
        printf("GENERATION: %d\n", i+1);
        printPopulation(*population);
        printf("Best chromosome found so far: ");
        printf("%s -> %d\n", bestChromosome, *bestFitness);
    }
}

void xOver(ptrChromosomeNode chromosome1, ptrChromosomeNode chromosome2, int rangeX, int rangeY){
    int i;
    ptrNode chromosome1Ptr = chromosome1->data, chromosome2Ptr = chromosome2->data;
    for(i = 1; i < rangeX-1; i++){
        chromosome1Ptr = chromosome1Ptr->next;
        chromosome2Ptr = chromosome2Ptr->next;
    }
    if(rangeX == 1){
        ptrNode temp = chromosome1Ptr;
        chromosome1Ptr = chromosome2Ptr;
        chromosome2Ptr = temp;
    } else{
        ptrNode temp = chromosome1Ptr->next;
        chromosome1Ptr->next = chromosome2Ptr->next;
        chromosome2Ptr->next = temp;
    }
    if(rangeY == 1){
        ptrNode temp = chromosome1Ptr->next;
        chromosome1Ptr->next = chromosome2Ptr->next;
        chromosome2Ptr->next = temp;
        return;
    }
    for(i = rangeX == 1 ? 1 : 0; i <= (rangeY-rangeX); i++){
        chromosome1Ptr = chromosome1Ptr->next;
        chromosome2Ptr = chromosome2Ptr->next;
    }
    ptrNode temp = chromosome1Ptr->next;
    chromosome1Ptr->next = chromosome2Ptr->next;
    chromosome2Ptr->next = temp;
}

void mutateChromosome(ptrNode population, int index){
    int i;
    ptrNode node;
    while (population != NULL){
        node = ((ptrChromosomeNode)(population->data))->data;
        for(i = 0; i < index-1; i++)
            node = node->next;
        char x = ((ptrGeneNode)(node->data))->data.data;
        x = x == '0' ? '1' : '0';
        ((ptrGeneNode)(node->data))->data.data = x;

        population = population->next;
    }
}

void calculateFitnesses(ptrNode population, int probSize){
    while(population != NULL){
        int i = probSize-1;
        ptrNode ptr = ((ptrChromosomeNode)(population->data))->data;
        ((ptrChromosomeNode)(population->data))->fitness = 0;
        while(ptr != NULL){
            ((ptrChromosomeNode)(population->data))->fitness +=  (((ptrGeneNode)(ptr->data))->data.data - '0') << i--;
            ptr = ptr->next;
        }
        population = population->next;
    }
}

void copyChromosomeToStr(char *str, ptrChromosomeNode chromosome){
    ptrNode ptr = chromosome->data;
    if(ptr == NULL) return;
    int i=0;
    while (ptr->next != NULL){
        str[i++] = ((ptrGeneNode)(ptr->data))->data.data;
        str[i++] = ':';
        ptr = ptr->next;
    }
    str[i++] = ((ptrGeneNode)(ptr->data))->data.data;
    str[i] = '\0';
}