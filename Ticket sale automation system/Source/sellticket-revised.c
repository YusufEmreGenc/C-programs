// This program is an automation system of passenger ticket sales
// Author: Yusuf Emre Genç - 21803663
// Hacettepe Computer Science

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum{Business, Economy, Standard} Class;
typedef enum{Normal, Veteran, Diplomat} Priority;

typedef struct {
    Class class;        // -1 -> not sold yet
}Seat;

typedef struct passenger{
    Priority priority;   // 1 -> priority, 0 -> no priority
    Class class;
    Seat seat;
    char *name;
    struct passenger* nextPassenger;
    struct passenger* previousPassenger;
}Passenger;

typedef struct {
    int top;    // It indicates the empty slot end of the stack
    int size;
    Seat *seats;
}Stack;

typedef struct {
    Passenger *front;
    Passenger *rear;
    int size;
}Queue;

typedef struct {
    Queue businessQueue;
    Queue economyQueue;
    Queue standardQueue;
    Stack availableBusinessSeats;
    Stack availableEconomySeats;
    Stack availableStandardSeats;
    int allBusinessSeats;
    int allEconomySeats;
    int allStandardSeats;
    char* flightName;
    Queue businessPassengers;
    Queue economyPassengers;
    Queue standardPassengers;
    int isOpen;     // 0 -> Flight is closed, 1 -> Flight is open, you can sell ticket and add seats
}Flight;

char** readlines(char* filename, int *numOfLines);
char* getWord(char** string);
int getNumOfWords(char* string);
void removeElemOfString(char *string, int index);
void ignoringRedundantSpaces(char** commands, int numOfLines);
int addSeat(Flight** flights, int *numOfFlights, char **commandLine, FILE* outputFile);
int queryFlightName(Flight* flights, int numOfFlights, char* flightName);
int createFlight(Flight **flights, int *numOfFlights, char* flightName);
Class getClass(char *class);
int isValidNum(char *num);
Stack* getSeats(Flight *flight, Class class);
int pushStack(Stack *stack, int numOfSeats, Class class);
int isStackEmpty(Stack *stack);
void deallocateBuffer(FILE **outputFile, Flight **flights, char ***commands, int numOfLines, int numOfFlights);
void deallocateQueueRecursively(Passenger* firstPassenger);
int enqueue(Flight *flights, int numOfFlights, char **commandLine, FILE *outputFile);
void priorityQueueEnqueue(Queue *priorityQueue, Class queueClass, Passenger *passenger);
int sell(Flight *flights, int numOfFlights, char **commandLine, FILE *outputFile);
Seat* pop(Stack* stack);
Passenger* dequeue(Queue *priorityQueue);
void closeFlight(Flight *flights, int numOfFlights, char **commandLine, FILE* outputFile);
void report(Flight *flights, int numOfFlights, char **commandLine, FILE* outputFile);
void info(Flight *flights, int numOfFlights, char **commandLine, FILE* outputFile);
Passenger* getPassenger(Flight *flights, int numOfFlights, char *passengerName, char **flightName);
char* getClassToStr(Class c);
int checkPriority(Priority p, Class class);
int checkNameConflict(Flight *flights, int numOfFlights, char *passengerName);
Flight* getFlight(Flight *flights, int numOfFlights,char **flightName);

int main(int argc, char** argv) {
    int numOfFlights = 0, numOfLines = 0, i;
    Flight *flights;
    char **commands;
    FILE* outputFile;

    if(!(commands = readlines(argv[1], &numOfLines))){
        return 1;
    }
    if(!(outputFile = fopen(argv[2], "w+"))){
        for(i = 0; i < numOfLines; i++)
            free(commands[i]);
        free(commands);
        printf("File cannot be opened\n");
        return 1;
    }

    ignoringRedundantSpaces(commands, numOfLines);

    flights = (Flight *) malloc(sizeof(Flight));
    for(i = 0; i < numOfLines; i++){
        char* commandLine = commands[i];
        char *command = getWord(&commandLine);

        if(!strcmp(command, "addseat")){
            if(!(addSeat(&flights, &numOfFlights, &commandLine, outputFile))){
                free(command);
                deallocateBuffer(&outputFile, &flights, &commands, numOfLines, numOfFlights);
                return 1;
            }
        } else if(!strcmp(command, "enqueue")){
            if(!(enqueue(flights, numOfFlights, &commandLine, outputFile))){
                free(command);
                deallocateBuffer(&outputFile, &flights, &commands, numOfLines, numOfFlights);
                return 1;
            }
        } else if(!strcmp(command, "sell")){
            if(!(sell(flights, numOfFlights, &commandLine, outputFile))){
                free(command);
                deallocateBuffer(&outputFile, &flights, &commands, numOfLines, numOfFlights);
                return 1;
            }
        } else if(!strcmp(command, "close")){
            closeFlight(flights, numOfFlights, &commandLine, outputFile);
        } else if(!strcmp(command, "report")){
            report(flights, numOfFlights, &commandLine, outputFile);
        } else if(!strcmp(command, "info")){
            info(flights, numOfFlights, &commandLine, outputFile);
        } else{
            fprintf(outputFile, "error\n");
        }
        free(command);
    }
    deallocateBuffer(&outputFile, &flights, &commands, numOfLines, numOfFlights);
    return 0;
}

// This function reads file and stores it in an array of commandLine structure
// If there is an unexpected error, it returns NULL
char** readlines(char* filename, int *numOfLines){
    FILE* commandFile;
    int limitLines = 10;    // This variable manages allocation for command lines
    *numOfLines = 0;        // This variable also manages allocation for command lines
    char **commands = (char **) malloc(limitLines * sizeof(char *));

    if(!commands){
        printf("Allocation failed!\n");
        return NULL;
    }

    if((commandFile = fopen(filename, "r+")) == NULL){
        printf("File cannot be opened!\n");
        free(commands);
        return NULL;
    }

    char c;     // gets characters from file
    while((c = fgetc(commandFile)) != EOF){     // repeates through number of lines
        while(c == ' ')     //ignoring leading spaces in line
            c = fgetc(commandFile);
        if(c == EOF) break;
        if(c != '\n' && c != '\r'){  // It checks for empty lines
            int countChar = 0, limitChar = 30;      // These variables manages allocation for commands
            commands[*numOfLines] = (char*) malloc(limitChar * sizeof(char));
            commands[*numOfLines][countChar] = c;
            countChar++;
            while((c = fgetc(commandFile)) != '\n' && c != '\r' && c != EOF){        // repeates for each command line
                commands[*numOfLines][countChar] = c;
                countChar++;
                if(countChar == (limitChar-1)){     // Reallocation and checking
                    limitChar *= 2;
                    char* temp = (char*) realloc(commands[*numOfLines], limitChar * sizeof(char));
                    if(!temp){
                        printf("Reallocation failed!\n");
                        int i;
                        for(i = 0; i < *numOfLines; i++)    free(commands[i]);
                        free(commands);
                        fclose(commandFile);
                        return NULL;
                    } else{
                        commands[*numOfLines] = temp;
                    }
                }
            }
            commands[*numOfLines][countChar] = '\0';
            (*numOfLines)++;
            if((*numOfLines) == (limitLines-1)){      // Reallocation and checking
                limitLines *= 10;
                char** temp = (char **) realloc(commands, limitLines * sizeof(commands));
                if(!temp){
                    printf("Reallocation failed!\n");
                    int i;
                    for(i = 0; i < *numOfLines; i++)    free(commands[i]);
                    free(commands);
                    fclose(commandFile);
                    return NULL;
                } else{
                    commands = temp;
                }
            }
        }
    }
    fclose(commandFile);
    return commands;
}

// This function gives the word in a string from starting the point that pointer points
// Users must be careful about "string" parameter because pointing place will be modified
char* getWord(char** string){
    int wordSize = 0, wordSizeLimit = 15;
    char *word = (char*) malloc(wordSizeLimit * sizeof(char));
    char c = *((*string)+wordSize);

    while(c != ' ' && c != '\n' && c != '\r' && c != '\0'){
        word[wordSize] = c;
        wordSize++;
        if(wordSize == (wordSizeLimit - 1)){    // Reallocation and checking
            wordSizeLimit *= 2;
            word = (char *) realloc(word, wordSizeLimit * sizeof(char));
        }
        c = *(++(*string));      // modifying the pointer
    }
    if(c == ' ') {    // ignoring spaces between words, so pointer points head of next word
        (*string)++;
    }

    word[wordSize] = '\0';
    return word;
}

// This function assumes that line does not include redundant spaces
int getNumOfWords(char* string){
    int numOfWords = 0;
    int charNum = 0;
    int no_word = 1;

    while(string[charNum] != '\n' &&string[charNum] != '\r' && string[charNum] != EOF && string[charNum] != '\0'){
        no_word = 0;
        if(string[charNum] == ' '){
            numOfWords++;
        }
        charNum++;
    }
    if(no_word) return numOfWords;
    else return numOfWords+1;   // +1 for the first word because we only counted space characters
}

void ignoringRedundantSpaces(char** commands, int numOfLines){
    int i = 0;

    for(; i < numOfLines; i++){
        char *line = commands[i];

        int i;
        //removing leading spaces
        for(i = 0; line[i] == ' ';)
            removeElemOfString(line, i);
        //removing redundant spaces
        for(i = 0; line[i] != '\0'; i++){
            if(line[i] == ' '){
                if(line[i+1] == ' '){       // detects redundant space in this line
                    removeElemOfString(line, i);
                    i--;
                }
            }
        }
        //removing redundant spaces at the end of string
        for(i = strlen(line); line[i - 1] == ' '; i--)
            removeElemOfString(line, i - 1);
    }
}

void removeElemOfString(char *string, int index){
    for(; string[index] != '\0'; index++)
        string[index] = string[index+1];
}

// This function returns index, if the word found, otherwise returns -1
int queryFlightName(Flight* flights, int numOfFlights, char* flightName){
    int i;
    for(i = 0; i < numOfFlights; i++)
        if(!strcmp(flightName, flights[i].flightName)) return i;
    return -1;
}

// If flight cannot be created, it returns 0, otherwise 1
int createFlight(Flight **flights, int *numOfFlights, char* flightName){
    (*flights) = (Flight *) realloc((*flights), sizeof(Flight) * (*numOfFlights + 1));
    if(!(*flights)){
        printf("Reallocation failed!\n");
        return 0;
    }
    (*flights)[*numOfFlights].isOpen = 1;
    (*flights)[*numOfFlights].allStandardSeats = 0;
    (*flights)[*numOfFlights].allEconomySeats = 0;
    (*flights)[*numOfFlights].allBusinessSeats = 0;
    (*flights)[*numOfFlights].flightName = flightName;
    (*flights)[*numOfFlights].businessQueue.size = 0;
    (*flights)[*numOfFlights].businessQueue.front = NULL;
    (*flights)[*numOfFlights].businessQueue.rear = NULL;
    (*flights)[*numOfFlights].economyQueue.size = 0;
    (*flights)[*numOfFlights].economyQueue.front = NULL;
    (*flights)[*numOfFlights].economyQueue.rear = NULL;
    (*flights)[*numOfFlights].standardQueue.size = 0;
    (*flights)[*numOfFlights].standardQueue.front = NULL;
    (*flights)[*numOfFlights].standardQueue.rear = NULL;
    (*flights)[*numOfFlights].businessPassengers.size = 0;
    (*flights)[*numOfFlights].businessPassengers.front = NULL;
    (*flights)[*numOfFlights].businessPassengers.rear = NULL;
    (*flights)[*numOfFlights].economyPassengers.size = 0;
    (*flights)[*numOfFlights].economyPassengers.front = NULL;
    (*flights)[*numOfFlights].economyPassengers.rear = NULL;
    (*flights)[*numOfFlights].standardPassengers.size = 0;
    (*flights)[*numOfFlights].standardPassengers.front = NULL;
    (*flights)[*numOfFlights].standardPassengers.rear = NULL;
    (*flights)[*numOfFlights].availableBusinessSeats.size = 0;
    (*flights)[*numOfFlights].availableBusinessSeats.top = -1;
    (*flights)[*numOfFlights].availableBusinessSeats.seats = (Seat *) malloc(sizeof(Seat));
    (*flights)[*numOfFlights].availableEconomySeats.size = 0;
    (*flights)[*numOfFlights].availableEconomySeats.top = -1;
    (*flights)[*numOfFlights].availableEconomySeats.seats = (Seat *) malloc(sizeof(Seat));
    (*flights)[*numOfFlights].availableStandardSeats.size = 0;
    (*flights)[*numOfFlights].availableStandardSeats.top = -1;
    (*flights)[*numOfFlights].availableStandardSeats.seats = (Seat *) malloc(sizeof(Seat));
    if(!((*flights)[*numOfFlights].availableBusinessSeats.seats && (*flights)[*numOfFlights].availableEconomySeats.seats && (*flights)[*numOfFlights].availableStandardSeats.seats)){
        free((*flights)[*numOfFlights].availableBusinessSeats.seats);
        free((*flights)[*numOfFlights].availableEconomySeats.seats);
        free((*flights)[*numOfFlights].availableStandardSeats.seats);
        printf("Allocation failed!\n");
        return 0;
    }

    (*numOfFlights)++;
    return 1;
}

// If no match, return -1
Class getClass(char *class){
    if(!strcmp(class, "business")){
        return Business;
    } else if(!strcmp(class, "economy")){
        return Economy;
    } else if(!strcmp(class, "standard")){
        return Standard;
    }
    return -1;
}

// If no match, return -1
Priority getPriority(char *priority){
    if(!strcmp(priority, "diplomat")){
        return Diplomat;
    } else if(!strcmp(priority, "veteran")) {
        return Veteran;
    }
    return -1;
}

// This function returns true if num can be converted to int, otherwise returns 0
int isValidNum(char *num){
    char c;
    while((c = *num++) != '\0'){
        if(c != '0' && c != '1' && c != '2' && c != '3' && c != '4' && c != '5' && c != '6' && c != '7' && c != '8' && c != '9'){
            return 0;
        }
    }
    return 1;
}

Stack* getSeats(Flight *flight, Class class){
    if (class == Business){
        return &(flight->availableBusinessSeats);
    } else if(class == Economy){
        return &(flight->availableEconomySeats);
    } else if(class == Standard){
        return &(flight->availableStandardSeats);
    }
    return NULL;
}

// It returns 0 if occurs any errors, otherwise 1
int pushStack(Stack *stack, int numOfSeats, Class class){
    if(numOfSeats > 0){     // Adding seats
        Seat *temp = (Seat *) realloc(stack->seats, sizeof(Seat) * (numOfSeats + stack->top + 1));
        if(!temp){
            printf("Allocation failed!\n");
            return 0;
        } else{
            if(isStackEmpty(stack)) stack->top = 0;
            stack->seats = temp;
            stack->size += numOfSeats;
            stack->top += numOfSeats;
            int i;
            for(i = stack->top - numOfSeats; i < stack->top; i++)
                stack->seats[i].class = class;
        }
    }
    return 1;
}

// If Stack empty return 1, else 0
int isStackEmpty(Stack *stack){
    if(stack->top == -1) return 1;
    return 0;
}

void deallocateBuffer(FILE **outputFile, Flight **flights, char ***commands, int numOfLines, int numOfFlights){
    int i;
    fclose(*outputFile);
    for(i = 0; i < numOfFlights; i++){
        free((*flights)[i].flightName);
        free((*flights)[i].availableStandardSeats.seats);
        free((*flights)[i].availableEconomySeats.seats);
        free((*flights)[i].availableBusinessSeats.seats);
        deallocateQueueRecursively((*flights)[i].standardPassengers.front);
        deallocateQueueRecursively((*flights)[i].economyPassengers.front);
        deallocateQueueRecursively((*flights)[i].businessPassengers.front);
        deallocateQueueRecursively((*flights)[i].standardQueue.front);
        deallocateQueueRecursively((*flights)[i].economyQueue.front);
        deallocateQueueRecursively((*flights)[i].businessQueue.front);
    }
    free(*flights);
    for(i = 0; i < numOfLines; i++)
        free((*commands)[i]);
    free((*commands));
}

void deallocateQueueRecursively(Passenger* firstPassenger){
    if(firstPassenger == NULL){
        return;
    } else{
        free(firstPassenger->name);
        Passenger *nextPassenger = firstPassenger->nextPassenger;
        free(firstPassenger);
        deallocateQueueRecursively(nextPassenger);
    }
}

Flight* getFlight(Flight *flights, int numOfFlights,char **flightName){
    int i;
    for(i = 0; i < numOfFlights; i++)
        if(!strcmp(*flightName, flights[i].flightName)){
            free(*flightName);
            return (flights+i);
        }
    free(*flightName);
    return NULL;
}

// This function returns -1, if there is invalid input. If there is error, 0. Otherwise 1
int addSeat(Flight** flights, int *numOfFlights, char **commandLine, FILE* outputFile){
    int numOfWords = getNumOfWords(*commandLine);
    if(numOfWords != 3){
        fprintf(outputFile, "error\n");
        return -1;
    } else{
        // Taking parameters and checking
        char *flightName = getWord(commandLine);
        char *className = getWord(commandLine);
        char *count = getWord(commandLine);
        Class class = getClass(className);
        free(className);
        if(class == -1 || !(isValidNum(count))){
            fprintf(outputFile, "error\n");
            free(flightName);
            free(count);
            return -1;
        }
        int numOfSeats = atoi(count);
        free(count);
        if(numOfSeats < 0){
            fprintf(outputFile, "error\n");
            free(flightName);
            return -1;
        }

        int i;
        if((i = queryFlightName(*flights, *numOfFlights, flightName)) == -1){    // Flight does not exist, it will be created
            if(!createFlight(flights, numOfFlights, flightName)){
                free(flightName);
                return 0;
            }
            i = (*numOfFlights) - 1;
        } else      free(flightName);
        if((*flights)[i].isOpen){
            Stack* seats = getSeats((*flights)+i, class);
            if(!pushStack(seats, numOfSeats, class)){
                return 0;
            }
            // Logging number of seats
            if(class == Business)   (*flights)[i].allBusinessSeats += numOfSeats;
            else if(class == Economy)   (*flights)[i].allEconomySeats += numOfSeats;
            else if(class == Standard)   (*flights)[i].allStandardSeats += numOfSeats;

            fprintf(outputFile, "addseats %s %d %d %d\n", (*flights)[i].flightName, (*flights)[i].allBusinessSeats, (*flights)[i].allEconomySeats, (*flights)[i].allStandardSeats);
        } else{
            fprintf(outputFile, "error\n");
            return -1;
        }
    }
    return 1;
}

// This function returns -1, if there is invalid input. If there is error, 0. Otherwise 1
int enqueue(Flight *flights, int  numOfFlights, char **commandLine, FILE *outputFile){
    int numOfWords = getNumOfWords(*commandLine);
    if(!(numOfWords == 3 || numOfWords == 4)){
        fprintf(outputFile, "error\n");
        return -1;
    } else{
        // getting command line
        char *flightName = getWord(commandLine);
        Flight *flight = getFlight(flights, numOfFlights, &flightName);
        char *className = getWord(commandLine);
        Class class = getClass(className);
        free(className);
        if(class == -1 || flight == NULL || !(flight->isOpen)){
            fprintf(outputFile, "error\n");
            return -1;
        }

        // Preparing passenger
        char *passengerName = getWord(commandLine);
        if(checkNameConflict(flights, numOfFlights, passengerName)){
            free(passengerName);
            fprintf(outputFile, "error\n");
            return -1;
        }
        Passenger* passenger = (Passenger *) malloc(sizeof(Passenger));
        if(!passenger){
            free(passengerName);
            printf("Allocation failed!\n");
            return 0;
        }
        passenger->name = passengerName;
        if(numOfWords == 4){
            char *priority = getWord(commandLine);
            Priority p = getPriority(priority);
            free(priority);
            if(p == -1 || !(checkPriority(p, class))){
                free(passenger->name);
                free(passenger);
                fprintf(outputFile, "error\n");
                return -1;
            }
            passenger->priority = p;
        } else{
            passenger->priority = Normal;
        }
        passenger->nextPassenger = NULL;
        passenger->previousPassenger = NULL;
        passenger->class = class;
        passenger->seat.class = -1;     // No seat yet

        // Adding passenger
        if(class == Business)
            priorityQueueEnqueue(&(flight->businessQueue), Business, passenger);
        else if(class == Economy)
            priorityQueueEnqueue(&(flight->economyQueue), Economy, passenger);
        else if(class == Standard)
            priorityQueueEnqueue(&(flight->standardQueue), Standard, passenger);

        fprintf(outputFile,"queue %s %s %s %d\n", flight->flightName, passenger->name, (passenger->class == Business) ? "business" : ((passenger->class == Economy) ? "economy" : "standard"), (passenger->class == Business) ? flight->businessQueue.size : ((passenger->class == Economy) ? flight->economyQueue.size : flight->standardQueue.size));
    }
    return 1;
}

void priorityQueueEnqueue(Queue *priorityQueue, Class queueClass, Passenger *passenger){
    if(priorityQueue->size == 0){   // The first passenger
        priorityQueue->front = passenger;
        priorityQueue->rear = passenger;
        priorityQueue->size = 1;
    } else{
        if(passenger->priority == Normal || passenger->priority != ((queueClass == Business) ? Diplomat : ((queueClass == Economy) ? Veteran : Normal))){      // If passenger has no priority or in different queue with the passenger's priority
            priorityQueue->rear->nextPassenger = passenger;
            passenger->previousPassenger = priorityQueue->rear;
            priorityQueue->rear = passenger;
            priorityQueue->size += 1;
        } else{     // If passenger has priority
            Passenger *p = priorityQueue->rear;
            while (p != NULL && p->priority != passenger->priority)     // Finding where to insert
                p = p -> previousPassenger;
            if(p == NULL){      // If newcoming passenger will be front of queue
                priorityQueue->front->previousPassenger = passenger;
                passenger->nextPassenger = priorityQueue->front;
                priorityQueue->front = passenger;
                priorityQueue->size += 1;
            } else{             // If newcoming passenger will be inserted to intermediate node
                passenger->nextPassenger = p->nextPassenger;
                p->nextPassenger = passenger;
                passenger->previousPassenger = p;
                if(passenger->nextPassenger != NULL)
                    passenger->nextPassenger->previousPassenger = passenger;
                priorityQueue->size += 1;
            }
        }
    }
}

void queueEnqueue(Queue *priorityQueue, Passenger *passenger){
    if(priorityQueue->size == 0){   // The first passenger
        priorityQueue->front = passenger;
        priorityQueue->rear = passenger;
        priorityQueue->size = 1;
    } else{
        priorityQueue->rear->nextPassenger = passenger;
        passenger->previousPassenger = priorityQueue->rear;
        priorityQueue->rear = passenger;
        priorityQueue->size += 1;
    }
}

// This function returns -1, if there is invalid input. If there is error, 0. Otherwise 1
int sell(Flight *flights, int numOfFlights, char **commandLine, FILE *outputFile){
    int numOfWords = getNumOfWords(*commandLine);
    if(numOfWords != 1){
        fprintf(outputFile, "error\n");
        return -1;
    } else{
        char *flightName = getWord(commandLine);
        Flight *flight = getFlight(flights, numOfFlights, &flightName);
        if(flight == NULL){
            fprintf(outputFile, "error\n");
            return -1;
        }
        if(flight->isOpen){
            Seat *seat;
            // Selling to business queue
            while (flight->businessQueue.size > 0 && (seat = pop(&(flight->availableBusinessSeats))) != NULL){
                queueEnqueue(&(flight->businessPassengers), dequeue(&(flight->businessQueue)));
                flight->businessPassengers.rear->seat.class = seat->class;
            }
            // Selling to economy queue
            while (flight->economyQueue.size > 0 && (seat = pop(&(flight->availableEconomySeats))) != NULL){
                queueEnqueue(&(flight->economyPassengers), dequeue(&(flight->economyQueue)));
                flight->economyPassengers.rear->seat.class = seat->class;
            }
            // Selling to standard queue
            while (flight->standardQueue.size > 0 && (seat = pop(&(flight->availableStandardSeats))) != NULL){
                queueEnqueue(&(flight->standardPassengers), dequeue(&(flight->standardQueue)));
                flight->standardPassengers.rear->seat.class = seat->class;
            }

            // The cases that there is not enough seat
            while(flight->availableStandardSeats.size > 0){
                if(flight->businessQueue.size > 0){
                    seat = pop(&(flight->availableStandardSeats));
                    Passenger *passenger = dequeue(&(flight->businessQueue));
                    if(passenger == NULL) continue;
                    passenger->seat.class = seat->class;
                    queueEnqueue(&(flight->standardPassengers), passenger);
                } else if(flight->economyQueue.size > 0){
                    seat = pop(&(flight->availableStandardSeats));
                    Passenger *passenger = dequeue(&(flight->economyQueue));
                    if(passenger == NULL) continue;
                    passenger->seat.class = seat->class;
                    queueEnqueue(&(flight->standardPassengers), passenger);
                }else{
                    break;
                }
            }
            fprintf(outputFile, "sold %s %d %d %d\n", flight->flightName, flight->businessPassengers.size, flight->economyPassengers.size, flight->standardPassengers.size);
        } else{
            fprintf(outputFile, "error\n");
            return -1;
        }
    }
    return 1;
}

// This function returns the top seat in stack or NULL(if stack is empty)
Seat* pop(Stack* stack){
    if(stack->top != -1){       // Stack is not empty
        stack->size -= 1;
        if(stack->size == 0){
            stack->top = -1;
            return &(stack->seats[0]);
        }
        stack->top -= 1;
        return &(stack->seats[stack->top]);
    } else{
        return NULL;
    }
}

// This function returns the passenger in the queue or NULL(if queue is empty)
Passenger* dequeue(Queue *priorityQueue){
    if(priorityQueue->size == 0)        // Queue is empty
        return NULL;
    else{
        Passenger *passenger = priorityQueue->front;
        priorityQueue->size -= 1;
        if(priorityQueue->size == 0){
            priorityQueue->front = NULL;
            priorityQueue->rear = NULL;
            passenger->previousPassenger = NULL;
            passenger->nextPassenger = NULL;
            return passenger;
        }
        priorityQueue->front = priorityQueue->front->nextPassenger;
        priorityQueue->front->previousPassenger = NULL;
        passenger->previousPassenger = NULL;
        passenger->nextPassenger = NULL;
        return passenger;
    }
}

void closeFlight(Flight *flights, int numOfFlights, char **commandLine, FILE* outputFile){
    int numOfWords = getNumOfWords(*commandLine);
    if(numOfWords != 1){
        fprintf(outputFile, "error\n");
        return;
    } else{
        char *flightName = getWord(commandLine);
        Flight *flight = getFlight(flights, numOfFlights, &flightName);
        if(flight == NULL || !(flight->isOpen)){
            fprintf(outputFile, "error\n");
            return;
        }
        flight->isOpen = 0;     // Flight is read only after this point.
        fprintf(outputFile, "closed %s %d %d\n", flight->flightName, flight->standardPassengers.size + flight->economyPassengers.size + flight->businessPassengers.size, flight->standardQueue.size + flight->economyQueue.size + flight->businessQueue.size);

        // Printing business queue
        Passenger *p = flight->businessQueue.front;
        while(p != NULL){
            fprintf(outputFile, "waiting %s\n", p->name);
            p = p->nextPassenger;
        }
        // Printing economy queue
        p = flight->economyQueue.front;
        while(p != NULL){
            fprintf(outputFile, "waiting %s\n", p->name);
            p = p->nextPassenger;
        }
        // Printing standard queue
        p = flight->standardQueue.front;
        while(p != NULL){
            fprintf(outputFile, "waiting %s\n", p->name);
            p = p->nextPassenger;
        }
    }
}

void report(Flight *flights, int numOfFlights, char **commandLine, FILE* outputFile){
    int numOfWords = getNumOfWords(*commandLine);
    if(numOfWords != 1){
        fprintf(outputFile, "error\n");
        return;
    } else {
        char *flightName = getWord(commandLine);
        Flight *flight = getFlight(flights, numOfFlights, &flightName);
        if(flight == NULL){
            fprintf(outputFile, "error\n");
            return;
        }
        fprintf(outputFile, "report %s\n", flight->flightName);

        Passenger *passenger;
        fprintf(outputFile, "business %d\n", flight->businessPassengers.size);
        passenger = flight->businessPassengers.front;
        while (passenger != NULL){
            fprintf(outputFile, "%s\n", passenger->name);
            passenger = passenger->nextPassenger;
        }
        fprintf(outputFile, "economy %d\n", flight->economyPassengers.size);
        passenger = flight->economyPassengers.front;
        while (passenger != NULL){
            fprintf(outputFile, "%s\n", passenger->name);
            passenger = passenger->nextPassenger;
        }
        fprintf(outputFile, "standard %d\n", flight->standardPassengers.size);
        passenger = flight->standardPassengers.front;
        while (passenger != NULL){
            fprintf(outputFile, "%s\n", passenger->name);
            passenger = passenger->nextPassenger;
        }
        fprintf(outputFile, "end of report %s\n", flight->flightName);
    }
}

void info(Flight *flights, int numOfFlights, char **commandLine, FILE* outputFile){
    int numOfWords = getNumOfWords(*commandLine);
    if(numOfWords != 1){
        fprintf(outputFile, "error\n");
        return;
    } else{
        char *passengerName = getWord(commandLine);
        Passenger *passenger;
        char *flightName = NULL;
        if(!(passenger = getPassenger(flights, numOfFlights, passengerName, &flightName))){
            free(passengerName);
            fprintf(outputFile, "error\n");
            return;
        } else{
            free(passengerName);
            fprintf(outputFile, "info %s %s %s %s\n", passenger->name, flightName, getClassToStr(passenger->class), getClassToStr(passenger->seat.class));
        }
    }
}

// It returns NULL, if no passenger in that name. Otherwise returns passenger
// flightName parameter is to be able to know in which flight the passenger is
Passenger* getPassenger(Flight *flights, int numOfFlights, char *passengerName, char **flightName){
    int i;
    for(i = 0; i < numOfFlights; i++){
        Passenger *temp = flights[i].businessPassengers.front;
        while (temp != NULL){
            if(!strcmp(passengerName, temp->name)){
                *flightName = flights[i].flightName;
                return temp;
            }
            temp = temp->nextPassenger;
        }
        temp = flights[i].economyPassengers.front;
        while (temp != NULL){
            if(!strcmp(passengerName, temp->name)){
                *flightName = flights[i].flightName;
                return temp;
            }
            temp = temp->nextPassenger;
        }
        temp = flights[i].standardPassengers.front;
        while (temp != NULL){
            if(!strcmp(passengerName, temp->name)){
                *flightName = flights[i].flightName;
                return temp;
            }
            temp = temp->nextPassenger;
        }
        temp = flights[i].businessQueue.front;
        while (temp != NULL){
            if(!strcmp(passengerName, temp->name)){
                *flightName = flights[i].flightName;
                return temp;
            }
            temp = temp->nextPassenger;
        }
        temp = flights[i].economyQueue.front;
        while (temp != NULL){
            if(!strcmp(passengerName, temp->name)){
                *flightName = flights[i].flightName;
                return temp;
            }
            temp = temp->nextPassenger;
        }
        temp = flights[i].standardQueue.front;
        while (temp != NULL){
            if(!strcmp(passengerName, temp->name)){
                *flightName = flights[i].flightName;
                return temp;
            }
            temp = temp->nextPassenger;
        }
    }
    return NULL;
}

char* getClassToStr(Class c){
    if(c == Business) return "business";
    else if(c == Economy) return "economy";
    else if(c == Standard) return "standard";
    else return "none";
}

// If the priority is valid, function returns 1. Otherwise, it returns 0
int checkPriority(Priority p, Class class){
    if(class == Business){
        if(p == Diplomat) return 1;
        else return 0;
    } else if(class == Economy){
        if(p == Veteran) return 1;
        else return 0;
    } else if(class == Standard){
        return 0;
    } else return 0;
}

// If there is confliction, returns 1. Otherwise, 0.
int checkNameConflict(Flight *flights, int numOfFlights, char *passengerName) {
    int i, j;
    for(i = 0; i < numOfFlights; i++){
        Passenger *temp = flights[i].businessPassengers.front;
        while (temp != NULL){
            if(!strcmp(passengerName, temp->name))
                return 1;
            temp = temp->nextPassenger;
        }
        temp = flights[i].economyPassengers.front;
        while (temp != NULL){
            if(!strcmp(passengerName, temp->name))
                return 1;
            temp = temp->nextPassenger;
        }
        temp = flights[i].standardPassengers.front;
        while (temp != NULL){
            if(!strcmp(passengerName, temp->name))
                return 1;
            temp = temp->nextPassenger;
        }
        temp = flights[i].businessQueue.front;
        while (temp != NULL){
            if(!strcmp(passengerName, temp->name))
                return 1;
            temp = temp->nextPassenger;
        }
        temp = flights[i].economyQueue.front;
        while (temp != NULL){
            if(!strcmp(passengerName, temp->name))
                return 1;
            temp = temp->nextPassenger;
        }
        temp = flights[i].standardQueue.front;
        while (temp != NULL){
            if(!strcmp(passengerName, temp->name))
                return 1;
            temp = temp->nextPassenger;
        }
    }
    return 0;
}