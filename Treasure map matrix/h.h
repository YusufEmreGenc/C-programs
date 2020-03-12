void getMapSize(char* string, int *row, int *column);

void allocateMemToMatrix(int ***map, int row, int column);

void readFileToMap(char *filename, int **map, int row, int column);

void printMap(int **map, int row, int column);

int dotProduct(int **map, int **key, int cursor[], int keySize);

int getModulo(int total, int mod);

void search(int **map, int **key, int cursor[], int keySize, FILE *filename);
