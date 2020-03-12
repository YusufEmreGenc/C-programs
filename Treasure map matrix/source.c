#include <stdio.h>
#include <stdlib.h>
#include "ass1_h.h"
#define MOD_NUMBER 5

void getMapSize(char* string, int *row, int *column){
    char *rowStr, *columnStr;
    int i = 0, j = 0;

    rowStr = (char *) malloc(15 * sizeof(char));
    columnStr = (char *) malloc(15 * sizeof(char));

    while(string[i] != 'x'){
        if(string[i] == '\0'){
            printf("Invalid form for map size!\n");
            return;
        }

        rowStr[j] = string[i];
        i++, j++;
    }
    rowStr[++j] = '\0';
    *row = atoi(rowStr);

    i++;
    j = 0;
    while(string[i] != '\0'){
        columnStr[j] = string[i];

        i++, j++;
    }
    columnStr[++j] = '\0';
    *column = atoi(columnStr);
}

void allocateMemToMatrix(int ***map, int row, int column){
    *map = (int **) malloc ( row * sizeof(int *));
    int i;

    for(i = 0; i < row; i++){
        *(*map + i) = (int *) malloc(column * sizeof(int));
    }
}

void readFileToMap(char *filename, int **map, int row, int column){
    FILE* mapMatrix = fopen(filename, "r+");
    int i, j;

    if(mapMatrix == NULL){
        printf("%s file cannot be opened!\n", filename);
        exit(1);
    }

    for(i = 0; i < row; i++){
        for(j = 0; j < column; j++) {
            fscanf(mapMatrix, "%d", map[i] + j);
        }
    }

    fclose(mapMatrix);
}

void printMap(int **map, int row, int column){
    int i, j;

    for(i = 0; i < row; i++){
        for(j = 0; j < column; j++){
            printf("%d ", map[i][j]);
        }
        printf("\n");
    }
}

int dotProduct(int **map, int **key, int cursor[], int keySize){
	int total = 0;
	int i, j;
	
	for(i = 0; i < keySize; i++){
		for(j = 0; j < keySize; j++){
			total += (key[i][j] * map[cursor[0]+i-(keySize/2)][cursor[1]+j-(keySize/2)]);
		}
	}

	return total;
}

int getModulo(int total, int mod){
	int result = total % mod;
	if(result < 0)	result += mod;
	return result;
}

void search(int **map, int **key, int cursor[], int keySize, FILE *outFile){
	int total = 0;
	total = dotProduct(map, key, cursor, keySize);
	int direction = getModulo(total, MOD_NUMBER);
	
	fprintf(outFile, "%d,%d:%d\n", cursor[0], cursor[1], total);	
	
	if(direction == 0){
		fprintf(outFile, "Treasure is found!\n");
		return;
	}
	else {
		int flag = 1, i;

		if(direction == 1){
			for(i = 0; i < keySize; i++){
				if(map[cursor[0] - (keySize / 2)][cursor[1] - (keySize / 2) + i ] != 0)		flag = 0;
			}
			if(flag)	cursor[0] += keySize;
			else		cursor[0] -= keySize;
		}
		else if(direction == 2){
			for(i = 0; i < keySize; i++){
				if(map[cursor[0] + (keySize / 2)][cursor[1] - (keySize / 2) + i ] != 0)		flag = 0;
			}
			if(flag)	cursor[0] -= keySize;
			else		cursor[0] += keySize;
		}
		else if(direction == 3){
			for(i = 0; i < keySize; i++){
				if(map[cursor[0] - (keySize / 2) + i ][cursor[1] + (keySize / 2)] != 0)		flag = 0;
			}
			if(flag)	cursor[1] -= keySize;
			else		cursor[1] += keySize;
		}
		else if(direction == 4){
			for(i = 0; i < keySize; i++){
				if(map[cursor[0] - (keySize / 2) + i ][cursor[1] - (keySize / 2)] != 0)		flag = 0;
			}
			if(flag)	cursor[1] += keySize;
			else		cursor[1] -= keySize;
		}

		search(map, key, cursor, keySize, outFile);
	}
}
