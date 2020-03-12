#include <stdio.h>
#define NUM_NUCL 30
#define CODON_SIZE 3
#define MESSAGE_SIZE 9

const char proteins[5][9] = {"MVAEGTKRI", "MGEAVRKTI", "MFSYCLPQR", "MFLVPTYDH", "MFSYCLPKR"};

int cmpCodon(const char* c1, const char* c2);

void printProtein(char*);

int isCodonStop(char*);

int cmpProtein(char*, int);

char codeOfCodon(char* c);

void currentCodon(const char*, char* ,int);

int main(int argc, char *argv[])
{
    if(argc == 1)
    {
        printf("No argument!\n");
        return 0;
    }

    // Reading data from file
    int i;
    FILE* fp;
    fp = fopen(argv[1], "r");
    char cp[NUM_NUCL+1];
    for(i = 0; i<NUM_NUCL; i++)
        fscanf(fp, "%c", &cp[i]);
    cp[i] = '\0';

    printf("fname: %s File content: %s\n\n", argv[1], cp);


    char proteinCode[MESSAGE_SIZE+1] = {'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};
    char codon[CODON_SIZE+1];
    codon[CODON_SIZE] = '\0';

    for (i = 0; i < (NUM_NUCL/3)-1; i++)
    {
        currentCodon(cp, codon, i*3);
        proteinCode[i] = codeOfCodon(codon);

        if(isCodonStop(codon))
        {
            printf("It is not a known protein.\n");
            proteinCode[i] = '\0';
            break;
        }

        // checking invalid codon in sequence
        if(proteinCode[i] == '0')
        {
            printf("There is invalid codon, please check input text: %s\n", codon);
            return 0;
        }
    }

    // If It is known protein
    for (i = 0; i < 5; i++)
    {
        if(cmpProtein(proteinCode, i) && ((cp[27] == 'U' && cp[28] == 'A' && cp[29] == 'A') || (cp[27] == 'U' && cp[28] == 'A' && cp[29] == 'G') || (cp[27] == 'U' && cp[28] == 'G' && cp[29] == 'A')))
        {
            if(i == 0)
            {
                printf("MyProteinA is identified in sequence.\nThe amino acids of MyProteinA: ");
                printProtein(proteinCode);
                printf("proteinA: ");
                printProtein(proteinCode);
                return 0;
            }
            else if(i == 1)
            {
                printf("MyProteinB is identified in sequence.\nThe amino acids of MyProteinB: ");
                printProtein(proteinCode);
                printf("proteinB: ");
                printProtein(proteinCode);
                return 0;
            }
            else if(i == 2)
            {
                printf("MyProteinC is identified in sequence.\nThe amino acids of MyProteinC: ");
                printProtein(proteinCode);
                printf("proteinC: ");
                printProtein(proteinCode);
                return 0;
            }
            else if(i == 3)
            {
                printf("MyProteinD is identified in sequence.\nThe amino acids of MyProteinD: ");
                printProtein(proteinCode);
                printf("proteinD: ");
                printProtein(proteinCode);
                return 0;
            }
            else if(i == 4)
            {
                printf("MyProteinE is identified in sequence.\nThe amino acids of MyProteinE: ");
                printProtein(proteinCode);
                printf("proteinE: ");
                printProtein(proteinCode);
                return 0;
            }
        }
    }

    // whether it is a protein, or not?
    if (proteinCode[0] == 'M' && ((cp[27] == 'U' && cp[28] == 'A' && cp[29] == 'A') || (cp[27] == 'U' && cp[28] == 'A' && cp[29] == 'G') || (cp[27] == 'U' && cp[28] == 'G' && cp[29] == 'A')))
    {
        printf("It is probably a new protein. ");
        printProtein(proteinCode);
    }
    else
    {
        if(proteinCode[0] != 'M' && !((cp[27] == 'U' && cp[28] == 'A' && cp[29] == 'A') || (cp[27] == 'U' && cp[28] == 'A' && cp[29] == 'G') || (cp[27] == 'U' && cp[28] == 'G' && cp[29] == 'A')))
        {
            printf("It is not protein! Not start with AUG and No Stop code\n");
            return 0;
        }
        if(proteinCode[0] != 'M')
            printf("Not start with AUG it is not a protein\n");
        else
            printf("It is not a protein! No Stop code\n");

    }

    return 0;
}


int cmpCodon(const char* c1, const char* c2)
{
    int flag = 1;
    while(*c1 && *c2)
    {
        if(*c1 != *c2)
            flag = 0;
        c1++, c2++;
    }
    return flag;
}

char codeOfCodon(char* c)
{
    if(cmpCodon(c, "UUU") || cmpCodon(c, "UUC"))    return 'F';
    else if(cmpCodon(c, "UUA") || cmpCodon(c, "UUG") || cmpCodon(c, "CUU") || cmpCodon(c, "CUC") || cmpCodon(c, "CUA") || cmpCodon(c, "CUG"))   return 'L';
    else if(cmpCodon(c, "AUU") || cmpCodon(c, "AUC") || cmpCodon(c, "AUA"))     return 'I';
    else if(cmpCodon(c, "AUG"))     return 'M';
    else if(cmpCodon(c, "GUU") || cmpCodon(c, "GUC") || cmpCodon(c, "GUA") || cmpCodon(c, "GUG"))   return 'V';
    else if(cmpCodon(c, "UCU") || cmpCodon(c, "UCC") || cmpCodon(c, "UCA") || cmpCodon(c, "UCG"))   return 'S';
    else if(cmpCodon(c, "CCU") || cmpCodon(c, "CCC") || cmpCodon(c, "CCA") || cmpCodon(c, "CCG"))   return 'P';
    else if(cmpCodon(c, "ACU") || cmpCodon(c, "ACC") || cmpCodon(c, "ACA") || cmpCodon(c, "ACG"))   return 'T';
    else if(cmpCodon(c, "GCU") || cmpCodon(c, "GCC") || cmpCodon(c, "GCA") || cmpCodon(c, "GCG"))   return 'A';
    else if(cmpCodon(c, "UAU") || cmpCodon(c, "UAC"))   return 'Y';
    else if(cmpCodon(c, "CAU") || cmpCodon(c, "CAC"))   return 'H';
    else if(cmpCodon(c, "CAA") || cmpCodon(c, "CAG"))   return 'Q';
    else if(cmpCodon(c, "AAU") || cmpCodon(c, "AAC"))   return 'N';
    else if(cmpCodon(c, "AAA") || cmpCodon(c, "AAG"))   return 'K';
    else if(cmpCodon(c, "GAU") || cmpCodon(c, "GAC"))   return 'D';
    else if(cmpCodon(c, "GAA") || cmpCodon(c, "GAG"))   return 'E';
    else if(cmpCodon(c, "UGU") || cmpCodon(c, "UGC"))   return 'C';
    else if(cmpCodon(c, "UGG"))   return 'W';
    else if(cmpCodon(c, "CGU") || cmpCodon(c, "CGC") || cmpCodon(c, "CGA") || cmpCodon(c, "CGG"))   return 'R';
    else if(cmpCodon(c, "AGU") || cmpCodon(c, "AGC"))   return 'S';
    else if(cmpCodon(c, "AGA") || cmpCodon(c, "AGG"))   return 'R';
    else if(cmpCodon(c, "GGU") || cmpCodon(c, "GGC") || cmpCodon(c, "GGA") || cmpCodon(c, "GGG"))   return 'G';
    else return '0';    // to check invalid codon
}

void currentCodon(const char* p, char triple[], int x)
{
    int i;
    for (i = 0; i < CODON_SIZE; i++)
        triple[i] = p[x+i];
    triple[i] = '\0';
}

int isCodonStop(char* codon)
{
    int flag = 0;
    if((codon[0] == 'U' && codon[1] == 'A' && codon[2] == 'A') || (codon[0] == 'U' && codon[1] == 'A' && codon[2] == 'G') || (codon[0] == 'U' && codon[1] == 'G' && codon[2] == 'A'))
        flag = 1;
    return flag;
}

int cmpProtein(char* protein, int i)
{
    int flag = 1;
    int x;
    for (x = 0; x < MESSAGE_SIZE; x++)
        if(protein[x] != proteins[i][x])
            flag = 0;
    return flag;
}

void printProtein(char* proteinCode)
{
    int i;
    for (i = 1; proteinCode[i] != '\0'; i++)
        printf("%c-", proteinCode[i-1]);
    printf("%c\n", proteinCode[i-1]);
}