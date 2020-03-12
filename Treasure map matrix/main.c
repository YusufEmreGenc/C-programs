#include <stdio.h>
#include <stdlib.h>
#include "ass1_h.h"

int main(int argc, char *argv[]) {
    int **map;
    int **key;
    int row_map, column_map;
    int key_size = atoi(argv[2]);
    int cursorForMatrix[2] = {key_size/2, key_size/2};
    FILE* outFile;

    if(argc != 6){
        fprintf(outFile, "The number of arguments in command line in terminal is invalid!\n");
        return 1;
    }
    
    outFile = fopen(argv[5], "w+");
    
    if(outFile == NULL){
		printf("%s file cannot be opened!\n", argv[5]);
		return 1;
	}

    getMapSize(argv[1], &row_map, &column_map);
    allocateMemToMatrix(&map, row_map, column_map);
    readFileToMap(argv[3], map, row_map, column_map);
    allocateMemToMatrix(&key, key_size, key_size);
    readFileToMap(argv[4], key, key_size, key_size);
    search(map, key, cursorForMatrix, key_size, outFile);
	
	fclose(outFile);
	int i = 0;
	for(i = 0; i < row_map; i++){
		free(map[i]);
	}
	free(map);
	
	for(i = 0; i < key_size; i++){
		free(key[i]);
	}
	free(key);

	//dont forget deallocation and closing files
    return 0;
}
