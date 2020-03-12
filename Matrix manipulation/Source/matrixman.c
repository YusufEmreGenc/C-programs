#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct{
    int length;
    int *data;
    char* name;
}vector;

typedef struct{
    int row;
    int column;
    int **data;
    char* name;
}matrix;

char** readlines(char* filename, int *numOfLines);
char* getWord(char** string);
int getNumOfWords(char* string);
void ignoringRedundantSpaces(char** commands, int numOfLines);
void removeElemOfString(char *string, int index);
int isValidNum(char *num);
vector* getVector(vector* vectors, int vecNum, char *vecName);
matrix* getMatrix(matrix* matrices, int matNum, char *matName);
char* getObjectNameFromFilename(char *filename);
void veczeros(int numOfWords, vector* vectors, int *vecNum, matrix *matrices, int matNum, char** command_line, FILE* outputFile);
void matzeros(int numOfWords, matrix* matrices, int *matNum, vector* vectors, int vecNum, char** command_line, FILE* outputFile);
void printMatrixToFile(matrix* m, FILE* outputFile);
void printVectorToFile(vector* v, FILE* outputFile);
int checkingDoubleNaming(vector *vectors, int vecNum, matrix *matrices, int matNum, char* name);
int vecread(int numOfWords, char** command_line, vector* vectors, int *vecNum, matrix* matrices, int matNum, FILE* outputFile, char* inputFolderPath);
int matread(int numOfWords, char** command_line, matrix* matrices, int *matNum, vector *vectors, int vecNum, FILE* outputFile, char* inputFolderPath);
void vecstack(int numOfWords, vector* vectors, int vecNum, matrix *matrices, int* matNum, char** command_line, FILE* outputFile);
void matstack(int numOfWords, matrix *matrices, int matNum, char **command_line, FILE* outputFile);
void mvstack(int numOfWords, matrix *matrices, int matNum, vector* vectors, int vecNum, char** command_line, FILE* outputFile);
void pad(int numOfWords, matrix *matrices, int matNum, char** command_line, FILE* outputFile);
void padval(int numOfWords, matrix *matrices, int matNum, char** command_line, FILE* outputFile);
void vecslice(int numOfWords, vector* vectors, int *vecNum, matrix* matrices, int matNum, char** command_line, FILE* outputFile);
void matslicecol(int numOfWords, vector* vectors, int *vecNum, matrix *matrices, int matNum, char** command_line, FILE* outputFile);
void matslicerow(int numOfWords, vector* vectors, int *vecNum, matrix *matrices, int matNum, char** command_line, FILE* outputFile);
void matslice(int numOfWords, matrix *matrices, int *matNum, vector *vectors, int vecNum, char** command_line, FILE* outputFile);
void elementwiseMatrixOperation(int numOfWords, matrix *matrices, int matNum, char operation, char** command_line, FILE* outputFile);
void freeBuffers(matrix **matrices, int matNum, vector **vectors, int vecNum, char ***commands, int numOfLines);

int main(int argc, char** argv){
    int numOfLines;     // number of commands in file
    char **commands;
    FILE* outputFile;
    int vecNum = 0, vecNumLimit = 5;
    int matNum = 0, matNumLimit = 5;

    if((outputFile = fopen(argv[3], "w+")) == NULL){
        printf("File cannot be opened!\n");
        return 1;
    }

    vector *vectors = (vector *) malloc (vecNumLimit * sizeof(vector));
    matrix *matrices = (matrix *) malloc (matNumLimit * sizeof(matrix));
    if(vectors == NULL || matrices == NULL){
        printf("Allocation failed\n");
        fclose(outputFile);
        return 1;
    }

    commands = readlines(argv[2], &numOfLines);
    if(!commands){
        printf("error\n");
        free(vectors);
        free(matrices);
        fclose(outputFile);
        return 1;
    }
    int i, j;

    ignoringRedundantSpaces(commands, numOfLines);

    for(i = 0; i < numOfLines; i++){
        char *command_line = commands[i];
        char *command = getWord(&command_line);
        int numOfWords = getNumOfWords(command_line);

        if(!strcmp(command, "veczeros")){
            veczeros(numOfWords, vectors, &vecNum, matrices, matNum, &command_line, outputFile);
        } else if (!strcmp(command, "matzeros")){
            matzeros(numOfWords, matrices, &matNum, vectors, vecNum, &command_line, outputFile);
        } else if (!strcmp(command, "vecread")){
            if(!vecread(numOfWords, &command_line, vectors, &vecNum, matrices, matNum, outputFile, argv[1])){
                fprintf(outputFile, "error\n");
                free(command);
                fclose(outputFile);
                freeBuffers(&matrices, matNum, &vectors, vecNum, &commands, numOfLines);
                return 1;
            }
        } else if (!strcmp(command, "matread")){
            if(!matread(numOfWords, &command_line, matrices, &matNum, vectors, vecNum, outputFile, argv[1])){
                fprintf(outputFile, "error\n");
                free(command);
                fclose(outputFile);
                freeBuffers(&matrices, matNum, &vectors, vecNum, &commands, numOfLines);
                return 1;
            }
        } else if (!strcmp(command, "vecstack")){
            vecstack(numOfWords, vectors, vecNum, matrices, &matNum, &command_line, outputFile);
        } else if (!strcmp(command, "matstack")){
            matstack(numOfWords, matrices, matNum, &command_line, outputFile);
        } else if (!strcmp(command, "mvstack")){
            mvstack(numOfWords, matrices, matNum, vectors, vecNum, &command_line, outputFile);
        } else if (!strcmp(command, "pad")){
            pad(numOfWords, matrices, matNum, &command_line, outputFile);
        } else if (!strcmp(command, "padval")){
            padval(numOfWords, matrices, matNum, &command_line, outputFile);
        } else if (!strcmp(command, "vecslice")){
            vecslice(numOfWords, vectors, &vecNum, matrices, matNum, &command_line, outputFile);
        } else if (!strcmp(command, "matslicecol")){
            matslicecol(numOfWords, vectors, &vecNum, matrices, matNum, &command_line, outputFile);
        } else if (!strcmp(command, "matslicerow")){
            matslicerow(numOfWords, vectors, &vecNum, matrices, matNum, &command_line, outputFile);
        } else if (!strcmp(command, "matslice")){
            matslice(numOfWords, matrices, &matNum, vectors, vecNum, &command_line, outputFile);
        } else if (!strcmp(command, "add")){
            elementwiseMatrixOperation(numOfWords, matrices, matNum, '+', &command_line, outputFile);
        } else if (!strcmp(command, "multiply")){
            elementwiseMatrixOperation(numOfWords, matrices, matNum, '*', &command_line, outputFile);
        } else if (!strcmp(command, "subtract")){
            elementwiseMatrixOperation(numOfWords, matrices, matNum, '-', &command_line, outputFile);
        } else{
            fprintf(outputFile, "error\n");
        }

        if(vecNum == vecNumLimit-1){
            vecNumLimit *= 2;
            vectors = (vector *) realloc(vectors, vecNumLimit * sizeof(vector));
        }
        if(matNum == matNumLimit-1){
            matNumLimit *= 2;
            matrices = (matrix *) realloc(matrices, matNumLimit * sizeof(matrix));
        }
        free(command);
    }

    freeBuffers(&matrices, matNum, &vectors, vecNum, &commands, numOfLines);
    fclose(outputFile);
    return 0;
}

// if there is confliction in names return 1, otherwise 0
int checkingDoubleNaming(vector *vectors, int vecNum, matrix *matrices, int matNum, char* name){
    int i;
    for(i = 0; i < vecNum; i++)
        if(!strcmp(vectors[i].name, name))
            return 1;
    for(i = 0; i < matNum; i++)
        if(!strcmp(matrices[i].name, name))
            return 1;
    return 0;
}

// This function reads file and stores it in an array of commandLine structure
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
        //ignoring leading spaces in line
        while(c == ' ')
            c = fgetc(commandFile);
        if(c == EOF) break;

        if(c != '\n' && c != '\r'){  // It checks for empty lines
            int countChar = 0, limitChar = 30;      // These variables manages allocation for commands

            commands[*numOfLines] = (char*) malloc(limitChar * sizeof(char));
            commands[*numOfLines][countChar] = c;
            countChar++;
            while((c = fgetc(commandFile)) != '\n' && c != '\r' && c != EOF){        // repeates for each command line
                if(c == EOF){   // When it hits EOF, stop reading
                    (*numOfLines)++;
                    fclose(commandFile);
                    return commands;
                } else{
                    commands[*numOfLines][countChar] = c;
                    countChar++;
                    if(countChar == (limitChar-1)){     // Reallocation and checking
                        limitChar *= 30;
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
        //removing last redundant spaces
        for(i = strlen(line); line[i - 1] == ' '; i--)
            removeElemOfString(line, i - 1);

    }
}

void removeElemOfString(char *string, int index){
    for(; string[index] != '\0'; index++)
        string[index] = string[index+1];
}

// This function returns true if num can be converted to int, otherwise returns 0
int isValidNum(char *num){
    char c;
    if(*num == '-') num++;
    while((c = *num++) != '\0'){
        if(c != '0' && c != '1' && c != '2' && c != '3' && c != '4' && c != '5' && c != '6' && c != '7' && c != '8' && c != '9'){
            return 0;
        }
    }
    return 1;
}

// This function takes the name of new vector and it checks for double naming and misformats,
// then creates vector and fills with 0 value
void veczeros(int numOfWords, vector* vectors, int *vecNum, matrix *matrices, int matNum, char** command_line, FILE* outputFile){
    if(numOfWords != 2){        // Wrong argument
        fprintf(outputFile, "error\n");
        return;
    } else{
        char *name = getWord(command_line);
        if(checkingDoubleNaming(vectors, *vecNum, matrices, matNum, name)){
            fprintf(outputFile, "error\n");
            free(name);
            return;
        }
        vectors[*vecNum].name = name;
        char *lengthStr = getWord(command_line);
        if(isValidNum(lengthStr)){
            int length = atoi(lengthStr);
            if(length > 0){
                vectors[*vecNum].length = length;
            } else{
                fprintf(outputFile, "error\n");
                free(lengthStr);
                return;
            }

        } else{
            free(lengthStr);
            fprintf(outputFile, "error\n");
            return;
        }

        // allocating and filling the new vector
        int x;
        vectors[*vecNum].data = (int *) malloc(vectors[*vecNum].length * sizeof(int));
        for(x = 0; x < vectors[*vecNum].length; x++)
            vectors[*vecNum].data[x] = 0;

        fprintf(outputFile, "created vector %s %d\n", vectors[*vecNum].name, vectors[*vecNum].length);
        printVectorToFile(&vectors[*vecNum], outputFile);

        (*vecNum)++;

        free(lengthStr);
    }
}

// This function takes the name of new matrix and it checks for double naming and wrong argument formats.
// then creates the matrix with given size and fills with zeros
void matzeros(int numOfWords, matrix* matrices, int *matNum, vector* vectors, int vecNum, char** command_line, FILE* outputFile){
    if(numOfWords != 3){    // checks wrong size of arguments
        fprintf(outputFile, "error\n");
        return;
    } else{
        char *name = getWord(command_line);
        if(checkingDoubleNaming(vectors, vecNum, matrices, *matNum, name)){
            fprintf(outputFile, "error\n");
            free(name);
            return;
        }
        matrices[*matNum].name = name;
        char *rows = getWord(command_line);
        char *columns = getWord(command_line);
        if(isValidNum(rows) && isValidNum(columns)){
            int row = atoi(rows), column = atoi(columns);
            if(row > 0 && column > 0){
                matrices[*matNum].row = row;
                matrices[*matNum].column = column;
            } else{
                fprintf(outputFile, "error\n");
                free(rows);
                free(columns);
                return;
            }
        } else{
            fprintf(outputFile, "error\n");
            free(rows);
            free(columns);
            return;
        }

        // allocating and filling the new matrix
        int x, y;
        matrices[*matNum].data = (int **) malloc(matrices[*matNum].row * sizeof(int *));
        for(x = 0; x < matrices[*matNum].row; x++){
            matrices[*matNum].data[x] = (int *) malloc(matrices[*matNum].column * sizeof(int));
        }
        for(x = 0; x < matrices[*matNum].row; x++)
            for(y = 0; y < matrices[*matNum].column; y++)
                matrices[*matNum].data[x][y] = 0;

        fprintf(outputFile, "created matrix %s %d %d\n", matrices[*matNum].name, matrices[*matNum].row, matrices[*matNum].column);
        printMatrixToFile(&matrices[*matNum], outputFile);

        (*matNum)++;

        free(rows);
        free(columns);
    }
}

void printMatrixToFile(matrix* m, FILE* outputFile){
    int x, y;
    for(x = 0; x < m->row; x++){
        for(y = 0; y < m->column; y++){
            fprintf(outputFile, "%d", m->data[x][y]);
            if(y != m->column-1)    fprintf(outputFile, " ");
        }
        fprintf(outputFile, "\n");
    }
}
void printVectorToFile(vector* v, FILE* outputFile){
    int x;
    for(x = 0; x < v->length; x++){
        fprintf(outputFile, "%d", v->data[x]);
        if(x != v->length-1)    fprintf(outputFile, " ");
    }
    fprintf(outputFile, "\n");
}

// This function reads a vector from the file which is in given folder in system arguments
// if function returns 0 program exits, if it returns -1 prints error, if returns 1 success
int vecread(int numOfWords, char** command_line, vector* vectors, int *vecNum, matrix* matrices, int matNum, FILE* outputFile, char* inputFolderPath){
    if(numOfWords != 1){    // Number of arguments are incorrect
        fprintf(outputFile, "error\n");
        return -1;
    } else{
        FILE *vectorFile;

        // Organizing the the file path
        char *filename = getWord(command_line);
        char *filePath = (char *) malloc ((strlen(inputFolderPath) + strlen(filename) + 2) * sizeof(char));
        strcpy(filePath, inputFolderPath);
        strcat(filePath, "/");
        strcat(filePath, filename);

        char *name = getObjectNameFromFilename(filename);
        if(checkingDoubleNaming(vectors, *vecNum, matrices, matNum, name)){
            fprintf(outputFile, "error\n");
            free(name);
            free(filename);
            free(filePath);
            return -1;
        }
        vectors[*vecNum].name = name;

        if((vectorFile = fopen(filePath, "r+")) == NULL){
            fprintf(outputFile, "error\n");
            free(name);
            free(filename);
            free(filePath);
            return -1;
        }
        int numEntry = 0, numEntryLimit = 5;
        vectors[*vecNum].data = (int *) malloc(numEntryLimit * sizeof(int));
        while(!feof(vectorFile)){
            fscanf(vectorFile, "%d", &(vectors[*vecNum].data[numEntry]));
            numEntry++;
            if(numEntry == (numEntryLimit-1)){
                numEntryLimit *= 2;
                vectors[*vecNum].data = (int *) realloc(vectors[*vecNum].data, numEntryLimit * sizeof(int));
            }
        }
        vectors[*vecNum].length = numEntry;

        fprintf(outputFile, "read vector %s %d\n", filename, vectors[*vecNum].length);
        printVectorToFile(&vectors[*vecNum], outputFile);

        (*vecNum)++;

        fclose(vectorFile);
        free(filename);
        free(filePath);
        return 1;
    }
}

// This function reads a matrix from the file which is in given folder in system arguments
// If function returns 0 program exits, if returns -1 only prints error, if returns 1 success
int matread(int numOfWords, char** command_line, matrix* matrices, int *matNum, vector *vectors, int vecNum, FILE* outputFile, char* inputFolderPath){
    if(numOfWords != 1){    // Wrong argument  number
        fprintf(outputFile, "error\n");
        return -1;
    } else{
        // Organizing the file path
        char *filename = getWord(command_line);
        char *filePath = (char *) malloc ((strlen(inputFolderPath) + strlen(filename) + 2) * sizeof(char));
        strcpy(filePath, inputFolderPath);
        strcat(filePath, "/");
        strcat(filePath, filename);

        char *name = getObjectNameFromFilename(filename);
        if(checkingDoubleNaming(vectors, vecNum, matrices, *matNum, name)){
            fprintf(outputFile, "error\n");
            free(name);
            free(filename);
            free(filePath);
            return -1;
        }
        matrices[*matNum].name = name;

        int rowNum, columnNum;
        char **data = readlines(filePath, &rowNum);
        if(!data){
            fprintf(outputFile, "error\n");
            free(name);
            free(filename);
            free(filePath);
            return 0;
        }
        columnNum = getNumOfWords(data[0]);
        int i, j;

        // allocating memory for matrix
        matrices[*matNum].data = (int **) malloc(rowNum * sizeof(int *));
        for(i = 0; i < rowNum; i++){
            matrices[*matNum].data[i] = (int *) malloc(columnNum * sizeof(int));
        }

        // filling matrice with values from file
        for(i = 0; i < rowNum; i++){
            char *line = data[i];
            for(j = 0; j < columnNum; j++){
                char *num = getWord(&line);
                matrices[*matNum].data[i][j] = atoi(num);
                free(num);
            }
        }
        matrices[*matNum].row = rowNum;
        matrices[*matNum].column = columnNum;

        fprintf(outputFile, "read matrix %s %d %d\n", filename, matrices[*matNum].row, matrices[*matNum].column);
        printMatrixToFile(&matrices[*matNum], outputFile);

        (*matNum)++;

        for(i = 0; i <rowNum; i++)
            free(data[i]);
        free(data);
        free(filename);
        free(filePath);

        return 1;
    }
}

char* getObjectNameFromFilename(char *filename){
    int i = strlen(filename)-1;     // points to end of filename
    int extensionSize = 0;
    while(*(filename+i) != '.'){
        extensionSize++;
        i--;
    }
    extensionSize++;
    size_t objectNameSize = strlen(filename) - extensionSize;
    char *objectName = (char *) malloc((objectNameSize + 1) * sizeof(char));
    for(i = 0; i < objectNameSize; i++)
        objectName[i] = filename[i];
    objectName[i] = '\0';
    return objectName;
}

// This function combines two vector and creates a matrix in order to specification in direction parameter.
void vecstack(int numOfWords, vector* vectors, int vecNum, matrix *matrices, int* matNum, char** command_line, FILE* outputFile){
    if(numOfWords != 4){
        fprintf(outputFile, "error\n");
        return;
    } else{
        char* vec1Name = getWord(command_line);
        char* vec2Name = getWord(command_line);
        char *direction = getWord(command_line);
        char *matrixName = getWord(command_line);

        if(checkingDoubleNaming(vectors, vecNum, matrices, *matNum, matrixName)){
            fprintf(outputFile, "error\n");
            free(vec1Name);
            free(vec2Name);
            free(direction);
            free(matrixName);
            return;
        }

        vector* vector1;
        vector* vector2;

        if(!(vector1 = getVector(vectors, vecNum, vec1Name))){
            fprintf(outputFile, "error\n");
            free(vec1Name);
            free(vec2Name);
	    free(direction);
            free(matrixName);
            return;
        }

        if(!(vector2 = getVector(vectors, vecNum, vec2Name))){
            fprintf(outputFile, "error\n");
            free(vec1Name);
            free(vec2Name);
	    free(direction);
            free(matrixName);
            return;
        }
        if(vector1->length != vector2->length){
            fprintf(outputFile, "error\n");
            free(vec1Name);
            free(vec2Name);
	    free(direction);
            free(matrixName);
            return;
        }

        if(!strcmp(direction, "row")){
            matrices[*matNum].data = (int **) malloc(2 * sizeof(int *));
            int i, j;
            for(i = 0; i < 2; i++)
                matrices[*matNum].data[i] = (int *) malloc(vector1->length * sizeof(int));
            matrices[*matNum].name = matrixName;
            matrices[*matNum].row = 2;
            matrices[*matNum].column = vector1->length;

            // filling the matrix with the corresponding values in vectors
            for(i = 0; i < 2; i++){
                vector* v = (i == 0 ? vector1 : vector2);
                for(j = 0; j < vector1->length; j++)
                    matrices[*matNum].data[i][j] = v->data[j];
            }
        } else if(!strcmp(direction, "column")){
            matrices[*matNum].data = (int **) malloc(vector1->length * sizeof(int *));
            int i, j;
            for(i = 0; i < vector1->length; i++)
                matrices[*matNum].data[i] = (int *) malloc(2 * sizeof(int));
            matrices[*matNum].name = matrixName;
            matrices[*matNum].row = vector1->length;
            matrices[*matNum].column = 2;

            // filling the matrix with the corresponding values in vectors
            for(i = 0; i < 2; i++){
                vector* v = (i == 0 ? vector1 : vector2);
                for(j = 0; j < vector1->length; j++)
                    matrices[*matNum].data[j][i] = v->data[j];
            }
        } else{
            free(matrixName);
            free(vec1Name);
            free(vec2Name);
            free(direction);
            fprintf(outputFile, "error\n");
            return;
        }
        fprintf(outputFile, "vectors concatenated %s %d %d\n", matrices[*matNum].name, matrices[*matNum].row, matrices[*matNum].column);
        printMatrixToFile(&matrices[*matNum], outputFile);

        (*matNum)++;
        free(vec1Name);
        free(vec2Name);
        free(direction);
    }
}

vector* getVector(vector* vectors, int vecNum, char *vecName){
    int i;

    for(i = 0; i < vecNum; i++){
        if(!strcmp(vectors[i].name, vecName)){
            return &vectors[i];
        }
    }
    return NULL;
}

matrix* getMatrix(matrix* matrices, int matNum, char *matName){
    int i;

    for(i = 0; i < matNum; i++){
        if(!strcmp(matrices[i].name, matName)){
            return &matrices[i];
        }
    }
    return NULL;
}

// This function combines two matrices in places of the first matrix.
// Command line decides where of first matrix to combine other
void matstack(int numOfWords, matrix *matrices, int matNum, char **command_line, FILE* outputFile){
    if(numOfWords != 3){    // Wrong number of arguments
        fprintf(outputFile, "error\n");
        return;
    } else{
        char* mat1Name = getWord(command_line);
        char* mat2Name = getWord(command_line);
        matrix *matrix1, *matrix2;

        if((matrix1 = getMatrix(matrices, matNum, mat1Name)) == NULL){
            fprintf(outputFile, "error\n");
            free(mat1Name);
            free(mat2Name);
            return;
        }
        if((matrix2 = getMatrix(matrices, matNum, mat2Name)) == NULL){
            fprintf(outputFile, "error\n");
            free(mat1Name);
            free(mat2Name);
            return;
        }
        char *where = getWord(command_line);

        int i, j;
        if(!strcmp(where, "r")){    // to right of matrix case
            if(matrix1->row != matrix2->row){
                free(where);
                free(mat1Name);
                free(mat2Name);
                fprintf(outputFile, "error\n");
                return;
            }

            // Reallocation matrix1
            for(i = 0; i < matrix1->row; i++){
                matrix1->data[i] = (int*) realloc(matrix1->data[i], (matrix1->column + matrix2->column) * sizeof(int));
            }

            // filling with new entries
            for(i = 0; i < (matrix1 -> row); i++)
                for(j = 0; j < (matrix2->column); j++)
                    matrix1->data[i][j + matrix1->column] = matrix2->data[i][j];

            matrix1 -> column = matrix1 -> column + matrix2 -> column;      // updates matrix's column number
        } else if(!strcmp(where, "d")){     // to down of the matrix case
            if(matrix1->column != matrix2->column){
                fprintf(outputFile, "error\n");
                free(where);
                free(mat1Name);
                free(mat2Name);
                return;
            }

            // Reallocation matrix2
            matrix1->data = (int **) realloc(matrix1->data, (matrix1->row + matrix2->row) * sizeof(int*));
            for(i = matrix1->row; i < matrix1->row + matrix2->row; i++){
                matrix1->data[i] = (int*) malloc(matrix1->column * sizeof(int));
            }

            // filling with new entries
            for(i = 0; i < matrix2->row; i++)
                for(j = 0; j < matrix2->column; j++){
                    matrix1->data[matrix1->row + i][j] = matrix2->data[i][j];
                }

            matrix1 -> row = matrix1 -> row + matrix2 -> row;       // updates matrix's column number

        } else{
            fprintf(outputFile, "error\n");
            free(where);
            free(mat1Name);
            free(mat2Name);
            return;
        }

        fprintf(outputFile, "matrices concatenated %s %d %d\n", matrix1->name, matrix1->row, matrix1->column);
        printMatrixToFile(matrix1, outputFile);

        free(mat1Name);
        free(mat2Name);
        free(where);
    }
}

// This function extends the matrix with the vector that is in correct form to combine.
// Otherwise it gives an error. Also it checks the name conflictions.
void mvstack(int numOfWords, matrix *matrices, int matNum, vector* vectors, int vecNum, char** command_line, FILE* outputFile){
    if(numOfWords != 3){    // Wrong argument number
        fprintf(outputFile, "error\n");
        return;
    } else{
        char* matrixName = getWord(command_line);
        char* vectorName = getWord(command_line);
        matrix *matrix;
        vector *vector;

        if((matrix = getMatrix(matrices, matNum, matrixName)) == NULL){
            fprintf(outputFile, "error\n");
            free(matrixName);
            free(vectorName);
            return;
        }
        if((vector = getVector(vectors, vecNum, vectorName)) == NULL){
            fprintf(outputFile, "error\n");
            free(matrixName);
            free(vectorName);
            return;
        }
        char *where = getWord(command_line);

        int i, j;
        if(!strcmp(where, "r")){    // in the case of combining from right of the matrix
            if(matrix->row != vector->length){
                fprintf(outputFile, "error\n");
                free(where);
                free(matrixName);
                free(vectorName);
                return;
            }

            // Reallocation matrix1
            for(i = 0; i < matrix->row; i++){
                matrix->data[i] = (int*) realloc(matrix->data[i], (matrix->column + 1) * sizeof(int));
            }

            // filling with new entries
            for(i = 0; i < (matrix -> row); i++)
                matrix->data[i][matrix->column] = vector->data[i];

            matrix -> column = matrix -> column + 1;      // updates matrix's column number
        } else if(!strcmp(where, "d")){     // in the case of combining from right of the matrix
            if(matrix->column != vector->length){
                fprintf(outputFile, "error\n");
                free(where);
                free(matrixName);
                free(vectorName);
                return;
            }

            // Reallocation matrix2
            matrix->data = (int **) realloc(matrix->data, (matrix->row + 1) * sizeof(int*));
            matrix->data[matrix->row] = (int*) realloc(matrix->data[matrix->row], matrix->column * sizeof(int));

            // filling with new entries
            for(i = 0; i < matrix->column; i++)
                matrix->data[matrix->row][i] = vector->data[i];

            matrix -> row = matrix -> row + 1;       // updates matrix's column number
        } else{
            fprintf(outputFile, "error\n");
            free(where);
            free(matrixName);
            free(vectorName);
            return;
        }

        fprintf(outputFile, "matrix and vector concatenated %s %d %d\n", matrix->name, matrix->row, matrix->column);
        printMatrixToFile(matrix, outputFile);

        free(matrixName);
        free(vectorName);
        free(where);
    }
}

// This function extends matrix and fill with the value of maximum aor minimum values due to argument
void pad(int numOfWords, matrix *matrices, int matNum, char** command_line, FILE* outputFile){
    if(numOfWords != 4){    // checking the number of arguments
        fprintf(outputFile, "error\n");
        return;
    } else{
        char* matrixName = getWord(command_line);
        matrix *matrix;

        if((matrix = getMatrix(matrices, matNum, matrixName)) == NULL){
            fprintf(outputFile, "error\n");
            free(matrixName);
            return;
        }
        char *x = getWord(command_line);
        char *y = getWord(command_line);
        if(!isValidNum(x)){
            free(x);
            free(y);
            free(matrixName);
            fprintf(outputFile, "error\n");
            return;
        }
        if(!isValidNum(y)){
            free(x);
            free(y);
            free(matrixName);
            fprintf(outputFile, "error\n");
            return;
        }
        int row = atoi(x);
        int column = atoi(y);
        free(x);
        free(y);
        if(row < 0 || column < 0){
            free(matrixName);
            fprintf(outputFile, "error\n");
            return;
        }
        char *mode = getWord(command_line);

        int i, j;
        if(!strcmp(mode, "maximum")){
            // Reallocating memory
            matrix->data = (int **) realloc(matrix->data, (matrix->row + row) * sizeof(int*));
            for(j = 0; j < matrix->row; j++){
                matrix->data[j] = (int *)realloc(matrix->data[j], sizeof(int) * (matrix->column + column));
            }
            for(j = 0; j < row; j++){
                matrix->data[j + matrix->row] = (int *)malloc(sizeof(int) * (matrix->column + column));
            }

            //Filling with max value
            int max = matrix->data[0][0];
            for(i = 0; i < matrix->row; i++){
                // getting max value
                int maxOfVec = matrix->data[i][0];
                for(j = 0; j < matrix->column; j++){
                    if(matrix->data[i][j] > maxOfVec){
                        maxOfVec = matrix->data[i][j];
                    }
                }
                if(maxOfVec > max){
                    max = maxOfVec;
                }
                for(; j < (matrix->column)+column; j++){
                    matrix->data[i][j] = maxOfVec;
                }
            }
            for(i = 0; i < matrix->column; i++){
                // getting max value
                int maxOfVec = matrix->data[0][i];
                for(j = 0; j < matrix->row; j++){
                    if(matrix->data[j][i] > maxOfVec){
                        maxOfVec = matrix->data[j][i];
                    }
                }
                if(maxOfVec > max){
                    max = maxOfVec;
                }
                for(; j < (matrix->row)+row; j++){
                    matrix->data[j][i] = maxOfVec;
                }
            }
            for(i = matrix->row; i < (matrix->row)+row; i++){
                for(j = matrix->column; j < (matrix->column)+column; j++)
                    matrix->data[i][j] = max;
            }
        } else if(!strcmp(mode, "minimum")){
            // Reallocating memory
            matrix->data = (int **) realloc(matrix->data, (matrix->row + row) * sizeof(int*));
            for(j = 0; j < matrix->row; j++){
                matrix->data[j] = (int *)realloc(matrix->data[j], sizeof(int) * (matrix->column + column));
            }
            for(j = 0; j < row; j++){
                matrix->data[j + matrix->row] = (int *)malloc(sizeof(int) * (matrix->column + column));
            }

            //Filling with max value
            int min = matrix->data[0][0];
            for(i = 0; i < matrix->row; i++){
                // getting max value
                int minOfVec = matrix->data[i][0];
                for(j = 0; j < matrix->column; j++){
                    if(matrix->data[i][j] < minOfVec){
                        minOfVec = matrix->data[i][j];
                    }
                }
                if(minOfVec < min){
                    min = minOfVec;
                }
                for(; j < (matrix->column)+column; j++){
                    matrix->data[i][j] = minOfVec;
                }
            }
            for(i = 0; i < matrix->column; i++){
                // getting max value
                int minOfVec = matrix->data[0][i];
                for(j = 0; j < matrix->row; j++){
                    if(matrix->data[j][i] < minOfVec){
                        minOfVec = matrix->data[j][i];
                    }
                }
                if(minOfVec < min){
                    min = minOfVec;
                }
                for(; j < (matrix->row)+row; j++){
                    matrix->data[j][i] = minOfVec;
                }
            }
            for(i = matrix->row; i < (matrix->row)+row; i++){
                for(j = matrix->column; j < (matrix->column)+column; j++)
                    matrix->data[i][j] = min;
            }
        } else{
            fprintf(outputFile, "error\n");
            free(matrixName);
            free(mode);
            return;
        }

        matrix->row = matrix->row + row;
        matrix->column = matrix->column + column;

        fprintf(outputFile, "matrix paded %s %d %d\n", matrix->name, matrix->row, matrix->column);
        printMatrixToFile(matrix, outputFile);

        free(matrixName);
        free(mode);
    }
}

// This function extends the matrix with given size and fills with given value.
void padval(int numOfWords, matrix *matrices, int matNum, char** command_line, FILE* outputFile){
    if(numOfWords != 4){    // checking for incorrect number of argument
        fprintf(outputFile, "error\n");
        return;
    } else{
        char* matrixName = getWord(command_line);
        matrix *matrix;

        if((matrix = getMatrix(matrices, matNum, matrixName)) == NULL){
            fprintf(outputFile, "error\n");
            free(matrixName);
            return;
        }
        char *x = getWord(command_line);
        char *y = getWord(command_line);
        char *v = getWord(command_line);
        if(!isValidNum(x)){
            fprintf(outputFile, "error\n");
            free(x);
            free(y);
            free(v);
            free(matrixName);
            return;
        }
        if(!isValidNum(y)){
            fprintf(outputFile, "error\n");
            free(x);
            free(y);
            free(v);
            free(matrixName);
            return;
        }
        if(!isValidNum(v)){
            fprintf(outputFile, "error\n");
            free(x);
            free(y);
            free(v);
            free(matrixName);
            return;
        }
        int row = atoi(x);
        int column = atoi(y);
        int val = atoi(v);

        free(x);
        free(y);
        free(v);

        if(row < 0 || column < 0){  // checks for incorrect size parameter
            fprintf(outputFile, "error\n");
            free(matrixName);
            return;
        }

        int i, j;
        // Reallocating memory
        matrix->data = (int **) realloc(matrix->data, (matrix->row + row) * sizeof(int*));
        for(j = 0; j < matrix->row; j++){
            matrix->data[j] = (int *)realloc(matrix->data[j], sizeof(int) * (matrix->column + column));
        }
        for(j = 0; j < row; j++){
            matrix->data[j + matrix->row] = (int *)malloc(sizeof(int) * (matrix->column + column));
        }

        //Filling with value
        for(i = 0; i < (matrix->row)+row; i++){
            for(j = matrix->column; j < matrix->column+column; j++){
                matrix->data[i][j] = val;
            }
        }
        for(i = 0; i < matrix->column; i++){
            for(j = matrix->row; j < matrix->row+row; j++){
                matrix->data[j][i] = val;
            }
        }

        matrix->row = matrix->row + row;
        matrix->column = matrix->column + column;

        fprintf(outputFile, "matrix paded %s %d %d\n", matrix->name, matrix->row, matrix->column);
        printMatrixToFile(matrix, outputFile);

        free(matrixName);
    }
}

// This function takes a part of vector and creates a new vector
void vecslice(int numOfWords, vector* vectors, int *vecNum, matrix* matrices, int matNum, char** command_line, FILE* outputFile){
    if(numOfWords != 4){    // incorrect number of parameters
        fprintf(outputFile, "error\n");
        return;
    } else{
        char* vec1Name = getWord(command_line);
        char* b = getWord(command_line);
        char* e = getWord(command_line);

        vector* vector1;

        char *name = getWord(command_line);
        if(checkingDoubleNaming(vectors, *vecNum, matrices, matNum, name)){
            fprintf(outputFile, "error\n");
            free(name);
            free(b);
            free(e);
            free(vec1Name);
            return;
        }
        vectors[*vecNum].name = name;

        if(!(vector1 = getVector(vectors, *vecNum, vec1Name))){
            fprintf(outputFile, "error\n");
            free(b);
            free(e);
            free(vec1Name);
            return;
        }
        if(!isValidNum(b)){
            fprintf(outputFile, "error\n");
            free(b);
            free(e);
            free(vec1Name);
            return;
        }
        if(!isValidNum(e)){
            fprintf(outputFile, "error\n");
            free(b);
            free(e);
            free(vec1Name);
            return;
        }
        int start = atoi(b);
        int stop = atoi(e);

        free(b);
        free(e);

        // checking incompatible sizes
        if(start < 0 || stop<0 || stop<start || start > vector1->length || stop > vector1->length){
            fprintf(outputFile, "error\n");
            free(vec1Name);
            return;
        }
        vectors[*vecNum].length = stop-start;
        vectors[*vecNum].data = (int*) malloc(sizeof(int)* vectors[*vecNum].length);

        // filling new vector with corresponding values
        int i;
        for(i = 0; i < vectors[*vecNum].length; i++){
            vectors[*vecNum].data[i] = vector1->data[i + start];
        }

        fprintf(outputFile, "vector sliced %s %d\n", vectors[*vecNum].name, vectors[*vecNum].length);
        printVectorToFile(vectors + (*vecNum), outputFile);

        (*vecNum)++;

        free(vec1Name);
    }
}

// This function takes a slice form a matrix's column and with this values creates a new vector
void matslicecol(int numOfWords, vector* vectors, int *vecNum, matrix *matrices, int matNum, char** command_line, FILE* outputFile){
    if(numOfWords != 5){    // incorrect number of parameters
        fprintf(outputFile, "error\n");
        return;
    } else{
        char* matrixName = getWord(command_line);
        char* in = getWord(command_line);
        char* b = getWord(command_line);
        char* e = getWord(command_line);

        matrix* matrix;
        char *name = getWord(command_line);
        if(checkingDoubleNaming(vectors, *vecNum, matrices, matNum, name)){
            fprintf(outputFile, "error\n");
            free(name);
            free(in);
            free(b);
            free(e);
            free(matrixName);
            return;
        }
        vectors[*vecNum].name = name;

        if(!(matrix = getMatrix(matrices, matNum, matrixName))){
            fprintf(outputFile, "error\n");
            free(in);
            free(b);
            free(e);
            free(matrixName);
            return;
        }
        if(!isValidNum(in)){
            fprintf(outputFile, "error\n");
            free(in);
            free(b);
            free(e);
            free(matrixName);
            return;
        }
        if(!isValidNum(b)){
            fprintf(outputFile, "error\n");
            free(in);
            free(b);
            free(e);
            free(matrixName);
            return;
        }
        if(!isValidNum(e)){
            fprintf(outputFile, "error\n");
            free(in);
            free(b);
            free(e);
            free(matrixName);
            return;
        }
        int index = atoi(in);
        int start = atoi(b);
        int stop = atoi(e);

        free(in);
        free(b);
        free(e);

        if(index >= matrix->column || index < 0 || start < 0 || stop<0 || stop<start || start > matrix->row || stop > matrix->row){   // checking size boundaries
            fprintf(outputFile, "error\n");
            free(matrixName);
            return;
        }

        vectors[*vecNum].length = stop - start;
        vectors[*vecNum].data = (int *) malloc(sizeof(int) * vectors[*vecNum].length);

        int i;
        for(i = 0; i < stop-start; i++)
            vectors[*vecNum].data[i] = matrix->data[start+i][index];

        fprintf(outputFile, "vector sliced %s %d\n", vectors[*vecNum].name, vectors[*vecNum].length);
        printVectorToFile(vectors + (*vecNum), outputFile);

        (*vecNum)++;

        free(matrixName);
    }
}

// This function takes a slice form a matrix's row and with this values creates a new vector
void matslicerow(int numOfWords, vector* vectors, int *vecNum, matrix *matrices, int matNum, char** command_line, FILE* outputFile){
    if(numOfWords != 5){
        fprintf(outputFile, "error\n");
        return;
    } else{
        char* matrixName = getWord(command_line);
        char* in = getWord(command_line);
        char* b = getWord(command_line);
        char* e = getWord(command_line);

        matrix* matrix;
        char *name = getWord(command_line);
        if(checkingDoubleNaming(vectors, *vecNum, matrices, matNum, name)){
            fprintf(outputFile, "error\n");
            free(name);
            free(in);
            free(b);
            free(e);
            free(matrixName);
            return;
        }
        vectors[*vecNum].name = name;

        if(!(matrix = getMatrix(matrices, matNum, matrixName))){
            fprintf(outputFile, "error\n");
	    free(name);
            free(in);
            free(b);
            free(e);
            free(matrixName);
            return;
        }
        if(!isValidNum(in)){
            fprintf(outputFile, "error\n");
	    free(name);
            free(in);
            free(b);
            free(e);
            free(matrixName);
            return;
        }
        if(!isValidNum(b)){
            fprintf(outputFile, "error\n");
	    free(name);
            free(in);
            free(b);
            free(e);
            free(matrixName);
            return;
        }
        if(!isValidNum(e)){
            fprintf(outputFile, "error\n");
	    free(name);
            free(in);
            free(b);
            free(e);
            free(matrixName);
            return;
        }
        int index = atoi(in);
        int start = atoi(b);
        int stop = atoi(e);

        free(in);
        free(b);
        free(e);

        if(index >= matrix->row || index < 0 || start < 0 || stop<0 || stop<start || start > matrix->column || stop > matrix->column){  // checks boundaries
            fprintf(outputFile, "error\n");
	    free(name);
            free(matrixName);
            return;
        }

        vectors[*vecNum].length = stop - start;
        vectors[*vecNum].data = (int *) malloc(sizeof(int) * vectors[*vecNum].length);

        int i;
        for(i = 0; i < stop-start; i++)
            vectors[*vecNum].data[i] = matrix->data[index][start+i];

        fprintf(outputFile, "vector sliced %s %d\n", vectors[*vecNum].name, vectors[*vecNum].length);
        printVectorToFile(vectors + (*vecNum), outputFile);

        (*vecNum)++;

        free(matrixName);
    }
}

// This function extracts a new matrix form the part of another matrix with the given sizes.
void matslice(int numOfWords, matrix *matrices, int *matNum, vector *vectors, int vecNum, char** command_line, FILE* outputFile){
    if(numOfWords != 6){    // Wrong number of arguments
        fprintf(outputFile, "error\n");
        return;
    } else{
        char* matrixName = getWord(command_line);
        char* y1 = getWord(command_line);
        char* y2 = getWord(command_line);
        char* x1 = getWord(command_line);
        char* x2 = getWord(command_line);

        char *name = getWord(command_line);
        if(checkingDoubleNaming(vectors, vecNum, matrices, *matNum, name)){
            fprintf(outputFile, "error\n");
            free(name);
            free(y1);
            free(y2);
            free(x1);
            free(x2);
            free(matrixName);
            return;
        }
        matrices[*matNum].name = name;
        matrix* matrix;

        if(!(matrix = getMatrix(matrices, *matNum, matrixName))){
            fprintf(outputFile, "error\n");
            free(y1);
            free(y2);
            free(x1);
            free(x2);
            free(matrixName);
            return;
        }
        if(!isValidNum(y1)){
            fprintf(outputFile, "error\n");
            free(y1);
            free(y2);
            free(x1);
            free(x2);
            free(matrixName);
            return;
        }
        if(!isValidNum(y2)){
            fprintf(outputFile, "error\n");
            free(y1);
            free(y2);
            free(x1);
            free(x2);
            free(matrixName);
            return;
        }
        if(!isValidNum(x1)){
            fprintf(outputFile, "error\n");
            free(y1);
            free(y2);
            free(x1);
            free(x2);
            free(matrixName);
            return;
        }
        if(!isValidNum(x2)){
            fprintf(outputFile, "error\n");
            free(y1);
            free(y2);
            free(x1);
            free(x2);
            free(matrixName);
            return;
        }
        int startColumn = atoi(y1);
        int stopColumn = atoi(y2);
        int startRow = atoi(x1);
        int stopRow = atoi(x2);

        free(y1);
        free(y2);
        free(x1);
        free(x2);

        matrices[*matNum].row = stopRow - startRow;
        matrices[*matNum].column = stopColumn - startColumn;

        // checking the sizes
        if(startColumn < 0 || stopColumn<0 || startRow < 0 || stopRow<0 || stopColumn < startColumn || stopRow < startRow || startRow > matrix->row || stopRow > matrix->row || startColumn > matrix->column || stopColumn > matrix->column){
            fprintf(outputFile, "error\n");
            free(matrixName);
            return;
        }

        int i, j;
        // Memory allocation for new matrix
        matrices[*matNum].data = (int **) malloc(sizeof(int*) * matrices[*matNum].row);
        for(i = 0; i < matrices[*matNum].row; i++)
            matrices[*matNum].data[i] = (int *) malloc(sizeof(int) * matrices[*matNum].column);

        // Filling with corresponding values
        for(i = 0; i < matrices[*matNum].row; i++)
            for(j = 0; j < matrices[*matNum].column; j++){
                matrices[*matNum].data[i][j] = matrix->data[startRow+i][startColumn+j];
            }

        fprintf(outputFile, "matrix sliced %s %d %d\n", matrices[*matNum].name, matrices[*matNum].row, matrices[*matNum].column);
        printMatrixToFile(matrices + (*matNum), outputFile);

        (*matNum)++;

        free(matrixName);
    }
}

// This function makes element-wise mathematical operations
void elementwiseMatrixOperation(int numOfWords, matrix *matrices, int matNum, char operation, char** command_line, FILE* outputFile){
    if(numOfWords != 2){
        fprintf(outputFile, "error\n");
        return;
    } else{
        char* matrix1Name = getWord(command_line);
        char* matrix2Name = getWord(command_line);
        matrix *matrix1, *matrix2;

        if(!(matrix1 = getMatrix(matrices, matNum, matrix1Name))){
            fprintf(outputFile, "error\n");
            free(matrix1Name);
            free(matrix2Name);
            return;
        }
        if(!(matrix2 = getMatrix(matrices, matNum, matrix2Name))){
            fprintf(outputFile, "error\n");
            free(matrix1Name);
            free(matrix2Name);
            return;
        }

        if(matrix1 -> row != matrix2 -> row){
            fprintf(outputFile, "error\n");
            free(matrix1Name);
            free(matrix2Name);
            return;
        }
        if(matrix1 -> column != matrix2 -> column){
            fprintf(outputFile, "error\n");
            free(matrix1Name);
            free(matrix2Name);
            return;
        }

        int i, j;
        // operations
        if(operation == '+'){
            for(i = 0; i < matrix1 -> row; i++)
                for(j = 0; j < matrix1 -> column; j++)
                    matrix1->data[i][j] = matrix1->data[i][j] + matrix2->data[i][j];

            fprintf(outputFile, "add %s %s\n", matrix1->name, matrix2->name);
            printMatrixToFile(matrix1, outputFile);
        } else if(operation == '-'){
            for(i = 0; i < matrix1 -> row; i++)
                for(j = 0; j < matrix1 -> column; j++)
                    matrix1->data[i][j] = matrix1->data[i][j] - matrix2->data[i][j];

            fprintf(outputFile, "subtract %s %s\n", matrix1->name, matrix2->name);
            printMatrixToFile(matrix1, outputFile);
        } else if(operation == '*'){
            for(i = 0; i < matrix1 -> row; i++)
                for(j = 0; j < matrix1 -> column; j++)
                    matrix1->data[i][j] = matrix1->data[i][j] * matrix2->data[i][j];

            fprintf(outputFile, "multiply %s %s\n", matrix1->name, matrix2->name);
            printMatrixToFile(matrix1, outputFile);
        }

        free(matrix1Name);
        free(matrix2Name);
    }
}

void freeBuffers(matrix **matrices, int matNum, vector **vectors, int vecNum, char ***commands, int numOfLines){
    int i, j;

    for(i = 0; i < matNum; i++){
        for(j = 0; j < (*matrices)[i].row; j++){
            free((*matrices)[i].data[j]);
        }
        free((*matrices)[i].data);
        free((*matrices)[i].name);
    }
    free((*matrices));
    for(i = 0; i < vecNum; i++){
        free((*vectors)[i].data);
        free((*vectors)[i].name);
    }
    free((*vectors));

    for(i = 0; i < numOfLines; i++)
        free((*commands)[i]);
    free(*commands);
}