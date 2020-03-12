#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#define COMMAND_SIZE 8

struct file
{
    int no;
    char *filename;
    char *extension;
    char *content;
};

typedef struct file * repository;

int lookingForParameter(const char*, const char*);

char* readline(FILE* fout, FILE* fp);

void allocationControl(FILE*, void *);

void createFile(FILE*,int*, const char*, repository, repository);

void deleteFile(FILE*,int*, const char*, repository);

void removeText(FILE*,int, const char*, repository);

void appendText(FILE*,int, const char*, repository);

void replaceWord(FILE*,int, const char*, repository);

void print(FILE*, int, const char*, repository);

void extendBuffer(FILE*, int*, char**);

void isEmptyFile(char*);

void takingName(FILE*, int*, int*, int*, const char*, char **);

void takingExtension(FILE*, int*, int*, int*, const char*, char **);

int main(int argc, char **argv)
{
    repository repo;
    int max_file = 10;
    int file_num = 0;
    char **lines;   /* contains command lines */
    FILE* fp, *fout;
    int max_line = 50;
    int num_line = 0;
    char *command;  /* It is for taking command keywords (create, append..) */
    int i = 0;

    repo = (repository) calloc (max_file, sizeof(struct file));
    allocationControl(fout, repo);

    command = (char *) calloc(COMMAND_SIZE, sizeof(char));
    allocationControl(fout, command);

    lines = (char **) calloc (max_line, sizeof(char*));
    allocationControl(fout, lines);

    fp = fopen(argv[1], "r");
    fout = fopen("output.txt", "w");
    if(fp == NULL || fout == NULL)
    {
        fprintf(fout, "Error in opening file!");
        return 0;
    }

    while( (fscanf(fp, "%s", command)) != EOF )
    {
        if((num_line + 2) == 50)	/* Controls number of line for reallocation */
        {
            max_line *= 2;
            lines = (char **) realloc(lines, (2*max_line) * sizeof(char *));
            allocationControl(fout, lines);
        }

        *(lines + num_line) = readline(fout, fp);

        fprintf(fout, "%s %s",command ,lines[num_line]);

        if( !(strcmp("create", command)) )
        {
            createFile(fout, &file_num, lines[num_line], (repo + file_num), repo);
            if( file_num == max_file )	/* File reallocation */
            {
                max_file *= 2;
                repo = (repository) realloc(repo, max_file * sizeof(struct file));
                allocationControl(fout, repo);
            }
        }
        else if( !(strcmp("delete", command)) )
            deleteFile(fout, &file_num, lines[num_line], repo);
        else if( !(strcmp("remove", command)) )
            removeText(fout, file_num, lines[num_line], repo);
        else if( !(strcmp("append", command)) )
            appendText(fout, file_num, lines[num_line], repo);
        else if( !(strcmp("replace", command)) )
            replaceWord(fout, file_num, lines[num_line], repo);
        else if( !(strcmp("print", command)) )
            print(fout, file_num, lines[num_line], repo);
        else{
            fprintf(fout, "Invalid command.\n");
            continue;
        }

        num_line++;
    }

    /* Free files */
    for(i = 0; i < file_num; i++)
    {
        free(repo[i].filename);
        free(repo[i].extension);
        free(repo[i].content);
    }

    /* Deallocations */
    for(i=0; i<num_line; i++)   free(lines[i]);
    free(command);
    free(lines);
    free(repo);
    fclose(fp);
    fclose(fout);
    return 0;
}

/* This function reads one line from file and writes that line to string that is dynamically allocated */
char* readline(FILE* fout, FILE* fp)
{
    char* line;
    int max_char = 200;
    int num_char = 0;
    char temp;

    line = (char*) calloc(max_char, sizeof(char));
    allocationControl(fout, line);

    fgetc(fp);      /* discarding space character at the beginning of line */
    while(1)
    {
        temp = fgetc(fp);
        if(temp == ' ') continue;
        else
        {
            line[num_char] = temp;
            break;
        }
    }
    while(line[num_char] != '\n' && line[num_char] != '\r' && line[num_char] != EOF )
    {
        if((num_char) == (max_char-2))
            extendBuffer(fout, &max_char, &line);
        num_char++;
        line[num_char] = fgetc(fp);
    }
    line = (char*) realloc(line, (num_char + 3) * sizeof(char));
    allocationControl(fout, line);

    line[num_char] = '\n';
    line[num_char+1] = '\0';
    return line;
}

/* This function returns index of parameter's content's index if matches parameter and command line, else returns -1 */
int lookingForParameter(const char* line, const char* parameter)
{
    int index = 0;
    int i = 0;

    while( line[index] != '\0' )
    {
        if( ( index == 0 && line[index] == parameter[0] ) || ( line[index] == ' ' && line[index+1] == parameter[0] ) )
        {
            i = 0;
            if(index != 0)	index++;
            while( i < strlen(parameter) && line[index] != '\0' && line[index] != '\n' && line[index] != '\r' )
            {
                if( line[index] != parameter[i] )	break;
                index++;
                i++;
            }
            if( (i == strlen(parameter)) && ( line[index] == ' ' || line[index] == '\n' || line[index] == '\0' ) )
                return (index + 1);
        }
        index++;
    }
    return -1;
}

/* This funcion for alerting an error when dynamic allocation went wrong */
void allocationControl(FILE* fout, void *ptr)
{
    if( ptr == NULL )
    {
        fprintf(fout, "Error in allocation memory!\n");
        exit(0);
    }
}

/* This function is called when a buffer will be extended that buffer is str */
void extendBuffer(FILE* fout, int *limit, char** str)
{
    (*limit) *= 2;
    *str = (char *) realloc(*str, (*limit) * sizeof(char));
    allocationControl(fout, str );
}

/* This function decide a file is empty or not. If it is empty writes in that file.
 * Also ignores the spaces in beginning of the file content */
void isEmptyFile(char* str)
{
    int i=0, j=0;

    if( strlen(str) == 0 )
        strcpy(str, "Empty File");

    for(i = 0; str[i]==' '; i++)
    {
        for(j=0; j<strlen(str)-1; j++)
            str[j] = str[j+1];
    }
}

/* This function takes a name from command line and dynamically allocates memory for that name */
void takingName(FILE* fout, int *name_index, int* name_size, int* name_limit, const char* command_line, char** filename)
{
    /* ignoring spaces between parameter and data */
    while( command_line[(*name_index)] == ' ' )
    {
        (*name_index)++;
        continue;
    }

    /* name recording section */
    while( command_line[*name_index] != '.' )
    {
        (*filename)[(*name_size)++] = command_line[(*name_index)++];

        if(*name_size == *name_limit)
            extendBuffer(fout, name_limit, filename);
    }
    (*name_index)++;
    *filename = (char *) realloc(*filename, (*name_limit + 2) * sizeof(char));
    allocationControl(fout,  *filename );
    (*filename)[*name_size] = '\0';
}

/* This function takes a extension from command line and dynamically allocates memory for that extension */
void takingExtension(FILE* fout, int* name_index, int* extension_size, int* extension_limit, const char* command_line, char** extension)
{
    /* extension recording section */
    while( (command_line[*name_index] != ' ') && (command_line[*name_index] != '\n') && (command_line[*name_index] != '\r') )
    {
        (*extension)[(*extension_size)++] = command_line[(*name_index)++];

        if(*extension_size == *extension_limit)
            extendBuffer(fout, extension_limit, extension);
    }
    *extension = (char *) realloc(*extension, (*extension_limit + 2) * sizeof(char));
    allocationControl(fout,  *extension );
    (*extension)[*extension_size] = '\0';
}

/* This function allocates a file in repository and give necessary datas ,which is taken from command line, to that file */
void createFile(FILE* fout, int *file_num, const char* command_line, repository repo, repository start_repo) {
    int name_size = 0, name_limit = 20;
    int extension_size = 0, extension_limit = 5;
    int content_size = 0, content_limit = 200;
    int name_index = 0;
    int content_index = 0;
    int i = 0, j = 0;
    int dotCount = 0;    /* dotCount variable is used for controlling name format "file.txt". */
    static int file_no = 1;     /* This variable for file number */
    int small_parameter = 0;

    name_index = lookingForParameter(command_line, "-n");
    content_index = lookingForParameter(command_line, "-t");

    /* Controlling invalid command case*/
    small_parameter = name_index < content_index ? name_index : content_index;
    while(j < (small_parameter-3))
    {
        if(command_line[j] != ' ')
        {
            fprintf(fout, "Invalid command format.\n");
            return;
        }
        j++;
    }

    /* Preventing segmentation fault in "create -t abcd -n" case */
    if(name_index >= strlen(command_line))
    {
        fprintf(fout, "The name is not entered.\n");
        return;
    }

    /* Controlling creating file format */
    if (name_index == -1 || content_index == -1) {
        fprintf(fout, "Please be sure that you entered creating command in correct form. There is no name or extension indicator.\n");
        return;
    }

    /* Controlling name format (there should be only one dot character for extension specifier)	*/
    while (command_line[name_index + i] != ' ' && command_line[name_index + i] != '\n') {
        if (command_line[name_index + i] == '.') dotCount++;
        i++;
    }
    if (dotCount != 1) {
        fprintf(fout, "The format of name is invalid. Please check name of file. There is more than one dot or not any. There may not be extension part.\n");
        return;
    }

    repo -> filename = (char *) calloc(name_limit, sizeof(char));
    allocationControl(fout,  repo -> filename );

    repo -> extension = (char *) calloc(extension_limit, sizeof(char));
    allocationControl(fout,  repo -> extension );

    repo -> content = (char *) calloc(content_limit, sizeof(char));
    allocationControl(fout,  repo -> content );


    takingName(fout, &name_index, &name_size, &name_limit, command_line, &(repo -> filename));

    takingExtension(fout, &name_index, &extension_size, &extension_limit, command_line, &(repo -> extension));

    /* Controlling no name file */
    if(strlen(repo -> filename) == 0)
    {
        fprintf(fout, "The name of file is not given\n");
        free(repo -> filename);
        free(repo -> extension);
        free(repo -> content);
        return;
    }

    /* Conrolling repository for preventing recreating */
    for(i = 0; i < (*file_num) ; i++)
        if( !(strcmp(repo -> filename, start_repo[i].filename)) && !(strcmp(repo -> extension, start_repo[i].extension)) )
        {
            fprintf(fout, "The file you have entered is already in repository.\n");
            free(repo -> filename);
            free(repo -> extension);
            free(repo -> content);
            return;
        }

    name_index = lookingForParameter(command_line, "-n");

    *file_num += 1;        /* Updating file number in repository */

    /* Preventing segmentation fault in "create -n abc.txt -t" case */
    if(content_index >= strlen(command_line))
    {
        strcpy(repo -> content, "");
        isEmptyFile(repo -> content);
        repo -> no = file_no;
        file_no++;  // It is updated for next file creating
        return;
    }

    /* ignoring spaces between parameter and data */
    while( command_line[content_index] == ' ' )
    {
        content_index++;
        continue;
    }

    /* content recording section */
    while((content_index < (name_index - 4)) || ( (content_index > name_index) && command_line[content_index] != '\n'  && command_line[content_index] != '\r'))
    {
        repo -> content[content_size++] = command_line[content_index++];

        if(content_size == content_limit)
            extendBuffer(fout, &content_limit, &(repo -> content));
    }
    repo -> content = (char *) realloc(repo -> content, (content_limit + 2) * sizeof(char));
    allocationControl(fout,  repo -> content );
    repo -> content[content_size] = '\0';

    repo -> no = file_no;
    file_no++;  // It is updated for next file creating

    isEmptyFile(repo -> content);
}

/* This function deallocates a file in repository and remove that file from repository */
void deleteFile(FILE* fout, int *file_num, const char* command_line, repository repo)
{
    int name_index = lookingForParameter(command_line, "-n");
    char *name, *extension;
    int name_size = 0, name_limit = 20;
    int extension_size = 0, extension_limit = 5;
    int i = 0, j = 0;
    int dotCount = 0;   /* dotCount variable is used for controlling name format "file.txt". */

    /* Controlling invalid command case*/
    while(j < (name_index-3))
    {
        if(command_line[j] != ' ')
        {
            fprintf(fout, "Invalid command format.\n");
            return;
        }
        j++;
    }

    /* Preventing segmentation fault in taking name case of "delete -n" */
    if(name_index >= strlen(command_line))
    {
        fprintf(fout, "The name is not entered.\n");
        return;
    }

    /* Controlling deleting file format */
    if (name_index == -1) {
        fprintf(fout, "Please be sure that you entered deleting command in correct form. There is no name.\n");
        return;
    }

    /* Controlling name format (there should be only one dot character for extension specifier)	*/
    while (command_line[name_index + i] != ' ' && command_line[name_index + i] != '\n') {
        if (command_line[name_index + i] == '.') dotCount++;
        i++;
    }
    if (dotCount != 1) {
        fprintf(fout, "The format of name is invalid. Please check name of file. There is more than one dot or not any. There may not be extension part.\n");
        return;
    }


    name = (char *) calloc(name_limit, sizeof(char));
    allocationControl(fout,  name );

    extension = (char *) calloc(extension_limit, sizeof(char));
    allocationControl(fout,  extension );

    takingName(fout, &name_index, &name_size, &name_limit, command_line, &name);
    takingExtension(fout, &name_index, &extension_size, &extension_limit, command_line, &extension);

    /* Finding relevant file */
    for(i = 0; i < *file_num; i++)
    {
        if( !(strcmp(name, repo[i].filename)) && !(strcmp(extension, repo[i].extension)) )
        {
            /* Cleaning file */
            repo[i].no = 0;
            free(repo[i].filename);
            free(repo[i].extension);
            free(repo[i].content);

            /* Sliding last of repository */
            for(; i < (*file_num-1); i++)
            {
                repo[i].no = repo[i+1].no;
                repo[i].filename = repo[i+1].filename;
                repo[i].extension = repo[i+1].extension;
                repo[i].content = repo[i+1].content;
            }
            break;
        }
    }
    if( i == *file_num )
    {
        fprintf(fout, "The file you entered is not found. Be sure that name and extension are correct.\n");
        return ;
    }
    *file_num -= 1;

    free(name);
    free(extension);
}

void removeText(FILE* fout, int file_num, const char* command_line, repository repo)
{
    int i = 0, j = 0, k = 0;
    int name_index = lookingForParameter(command_line, "-n");
    char *name, *extension, *start, *len;
    int name_size = 0, name_limit = 20;
    int extension_size = 0, extension_limit = 5;
    int start_size = 0, start_limit = 5;
    int len_size = 0, len_limit = 5;
    int start_index = lookingForParameter(command_line, "-s");
    int len_index = lookingForParameter(command_line, "-l");
    int start_num = 0, len_num = 0;
    int small_parameter = 0;

    /* Controlling invalid command case*/
    small_parameter = name_index < len_index ? name_index : len_index;
    small_parameter = small_parameter < start_index ? small_parameter : start_index;
    while(j < (small_parameter-3))
    {
        if(command_line[j] != ' ')
        {
            fprintf(fout, "Invalid command format.\n");
            return;
        }
        j++;
    }
    j = 0;

    if(name_index >= strlen(command_line) || start_index >= strlen(command_line) || len_index >= strlen(command_line))
    {
        fprintf(fout, "Missed parameter.\n");
        return;
    }

    name = (char *) calloc(name_limit, sizeof(char));
    allocationControl(fout,  name );

    extension = (char *) calloc(extension_limit, sizeof(char));
    allocationControl(fout,  extension );

    takingName(fout, &name_index, &name_size, &name_limit, command_line, &name);
    takingExtension(fout, &name_index, &extension_size, &extension_limit, command_line, &extension);


    start = (char *) calloc(start_limit, sizeof(char));
    allocationControl(fout,  start );

    len = (char *) calloc(len_limit, sizeof(char));
    allocationControl(fout,  len );

    /* ignoring spaces between parameter and data */
    while( command_line[start_index] == ' ' )
    {
        start_index++;
        continue;
    }

    /* Getting start part */
    while( command_line[start_index] != ' ' && command_line[start_index] != '\n' && command_line[start_index] != '\r' )
    {
        if(command_line[start_index] >= '0' && command_line[start_index] <= '9' )
            start[start_size++] = command_line[start_index++];
        else
            break;

        if(start_size == start_limit)
            extendBuffer(fout, &start_limit, &start);
    }
    /* Controls invalid syntax for -s parameter */
    while(start_index != (lookingForParameter(command_line, "-n") - 4) && start_index != (lookingForParameter(command_line, "-l") - 4) && command_line[start_index] != '\n' && command_line[start_index] != '\r' )
    {
        if( command_line[start_index++] == ' ' ) continue;
        else
        {
            fprintf(fout, "Invalid starting indice or length. There may be non-numeric or negative number or not be any character or gap in the number like \"1 2\".\n");
            free(name);
            free(extension);
            free(start);
            free(len);
            return;
        }
    }
    start = (char *) realloc(start, (start_limit + 2) * sizeof(char));
    allocationControl(fout,  start );
    start[start_size] = '\0';


    /* ignoring spaces between parameter and data */
    while( command_line[len_index] == ' ' )
    {
        len_index++;
        continue;
    }

    /* Getting length part */
    while( command_line[len_index] != ' ' && command_line[len_index] != '\n' && command_line[len_index] != '\r' )
    {
        if(command_line[len_index] >= '0' && command_line[len_index] <= '9' )
            len[len_size++] = command_line[len_index++];
        else
            break;

        if(len_size == len_limit)
            extendBuffer(fout, &len_limit, &len);
    }
    /* Controls invalid syntax for -l parameter */
    while(len_index != (lookingForParameter(command_line, "-n") - 4) && len_index != (lookingForParameter(command_line, "-s") - 4) && command_line[len_index] != '\n' && command_line[len_index] != '\r' )
    {
        if( command_line[len_index++] == ' ' ) continue;
        else
        {
            fprintf(fout, "Invalid starting indice or length. There may be non-numeric or negative number or absent(no char) or gap in the number like \"1 2\".\n");
            free(name);
            free(extension);
            free(start);
            free(len);
            return;
        }
    }
    len = (char *) realloc(len, (len_limit + 2) * sizeof(char));
    allocationControl(fout,  len );
    len[len_size] = '\0';



    /* Converting string to integer */
    start_num = atoi(start);
    len_num = atoi(len);


    /* Finding relevant file */
    for(i = 0; i < file_num; i++)
    {
        if( !(strcmp(name, repo[i].filename)) && !(strcmp(extension, repo[i].extension)) )
        {
            /* Controlling for not extend the size of content */
            if(strlen(repo[i].content) < (start_num + len_num))
            {
                fprintf(fout, "The part which will be removed is extends the size of content.\n");
                free(name);
                free(extension);
                free(start);
                free(len);
                return;
            }

            /* Removing text */
            for(k = len_num; k > 0; k--)
            {
                for(j = 0; repo[i].content[start_num + k + j ] != '\0' ; j++)
                    repo[i].content[start_num + k + j - 1] = repo[i].content[start_num + k + j ];
                repo[i].content[start_num + k + j - 1] = '\0';
            }
            break;
        }
    }
    if( i == file_num )
    {
        fprintf(fout, "The file you entered is not found. Be sure that name and extension are correct.\n");
        return;
    }
    isEmptyFile(repo[i].content);

    free(name);
    free(extension);
    free(start);
    free(len);
}

void appendText(FILE* fout, int file_num, const char* command_line, repository repo)
{
    int name_index = lookingForParameter(command_line, "-n");
    int text_index = lookingForParameter(command_line, "-t");
    char *name, *text, *extension;
    int name_size = 0, name_limit = 20;
    int text_size = 0, text_limit = 50;
    int extension_size = 0, extension_limit = 5;
    int i = 0, j = 0;
    int strlen_content = 0;
    int small_parameter = 0;

    /* Controlling invalid command case*/
    small_parameter = name_index < text_index ? name_index : text_index;
    while(j < (small_parameter-3))
    {
        if(command_line[j] != ' ')
        {
            fprintf(fout, "Invalid command format.\n");
            return;
        }
        j++;
    }
    j = 0;

    /* Controlling missing parameter or invalid parameter form. */
    if(name_index >= strlen(command_line) || text_index >= strlen(command_line))
    {
        fprintf(fout, "Invalid form. Missed parameter or no content for parameter.\n");
        return;
    }

    name = (char *) calloc(name_limit, sizeof(char));
    allocationControl(fout,  name );

    text = (char *) calloc(text_limit, sizeof(char));
    allocationControl(fout,  text );

    extension = (char *) calloc(extension_limit, sizeof(char));
    allocationControl(fout,  extension );

    takingName(fout, &name_index, &name_size, &name_limit, command_line, &name);
    takingExtension(fout, &name_index, &extension_size, &extension_limit, command_line, &extension);


    /* Getting text that will be appended */
    while( text_index != (lookingForParameter(command_line, "-n") - 4) && command_line[text_index] != '\n' && command_line[text_index] != '\r' )
    {
        text[text_size++] = command_line[text_index++];

        if(text_size == text_limit)
            extendBuffer(fout, &text_limit, &text);
    }
    text = (char *) realloc(text, (text_limit + 2) * sizeof(char));
    allocationControl(fout,  text );
    text[text_size] = '\0';


    /* Finding relevant file and appending text */
    for(i = 0; i < file_num; i++)
    {
        if( !(strcmp(name, repo[i].filename)) && !(strcmp(extension, repo[i].extension)) )
        {
            strlen_content = strlen(repo[i].content);

            /* Appending operation */
            repo[i].content = (char *) realloc(repo[i].content, (strlen_content + text_size + 2) * sizeof(char));
            allocationControl(fout, repo[i].content );

            for(j = 0; j < text_size; j++)
                repo[i].content[strlen_content + j ] = text[j];
            repo[i].content[strlen_content + j] = '\0';

            break;
        }
    }
    if( i == file_num )
    {
        fprintf(fout, "The file you entered is not found. Be sure that name and extension are correct.\n");
        return;
    }
    isEmptyFile(repo[i].content);

    free(name);
    free(extension);
    free(text);
}

void replaceWord(FILE* fout, int file_num, const char* command_line, repository repo)
{
    int name_index = lookingForParameter(command_line, "-n");
    int oldWord_index = lookingForParameter(command_line, "-ow");
    int newWord_index = lookingForParameter(command_line, "-nw");
    char *name, *extension, *oldWord, *newWord;
    int name_size = 0, name_limit = 20;
    int extension_size = 0, extension_limit = 5;
    int oldWord_size = 0, oldWord_limit = 20;
    int newWord_size = 0, newWord_limit = 20;
    int strlen_content = 0;
    int oldWord_location = -1;
    int i = 0, j = 0, k = 0, m = 0;
    int small_parameter = 0;

    /* Controlling invalid command case*/
    small_parameter = name_index < oldWord_index ? name_index : oldWord_index;
    small_parameter = small_parameter < newWord_index ? small_parameter : newWord_index;
    while(j < (small_parameter-3))
    {
        if(command_line[j] != ' ')
        {
            fprintf(fout, "Invalid command format.\n");
            return;
        }
        j++;
    }
    j = 0;

    /* Controlling missing parameter or invalid parameter form. */
    if(name_index >= strlen(command_line) || oldWord_index >= strlen(command_line) || newWord_index >= strlen(command_line))
    {
        fprintf(fout, "Invalid form. Missed parameter or no content for parameter.\n");
        return;
    }

    name = (char *) calloc(name_limit, sizeof(char));
    allocationControl(fout, name );

    extension = (char *) calloc(extension_limit, sizeof(char));
    allocationControl( fout, extension );

    oldWord = (char *) calloc(oldWord_limit, sizeof(char));
    allocationControl( fout, oldWord );

    newWord = (char *) calloc(newWord_limit, sizeof(char));
    allocationControl( fout, newWord );

    takingName(fout, &name_index, &name_size, &name_limit, command_line, &name);
    takingExtension(fout, &name_index, &extension_size, &extension_limit, command_line, &extension);


    /* Finding old word part */
    while( oldWord_index != (lookingForParameter(command_line, "-n") - 4) && oldWord_index != (lookingForParameter(command_line, "-nw") - 5) && command_line[oldWord_index] != '\n' && command_line[oldWord_index] != '\r' )
    {
        oldWord[oldWord_size++] = command_line[oldWord_index++];

        if(oldWord_size == oldWord_limit)
            extendBuffer(fout, &oldWord_limit, &oldWord);
    }
    oldWord = (char *) realloc(oldWord, (oldWord_limit + 2) * sizeof(char));
    allocationControl( fout, oldWord );
    oldWord[oldWord_size] = '\0';


    /* Finding new word part */
    while( newWord_index != (lookingForParameter(command_line, "-n") - 4) && newWord_index != (lookingForParameter(command_line, "-ow") - 5) && command_line[newWord_index] != '\n' && command_line[newWord_index] != '\r' )
    {
        newWord[newWord_size++] = command_line[newWord_index++];

        if(newWord_size == newWord_limit)
            extendBuffer(fout, &newWord_limit, &newWord);
    }
    newWord = (char *) realloc(newWord, (newWord_limit + 2) * sizeof(char));
    allocationControl( fout, newWord );
    newWord[newWord_size] = '\0';

    /* Finding relevant file */
    for(i = 0; i < file_num; i++)
    {
        if( !(strcmp(name, repo[i].filename)) && !(strcmp(extension, repo[i].extension)) )
        {
            strlen_content = strlen(repo[i].content);

            /* Finding old word location */
            for(j = 0; j < strlen_content; j++)
            {
                if(repo[i].content[j] == oldWord[0])
                {
                    for(k = 0; k < oldWord_size; k++)
                        if(repo[i].content[j + k] != oldWord[k])	break;
                    /* The word to be replaced is founded */
                    if(k == oldWord_size)
                    {
                        oldWord_location = j;

                        /* Replacing words */
                        if( oldWord_size == newWord_size )
                            for(k = 0; k < oldWord_size; k++)
                                repo[i].content[oldWord_location + k] = newWord[k];

                        /* Sliding remaining part and Appending new word */
                        if( oldWord_size < newWord_size )
                        {
                            /* reallocation in case new word is bigger */
                            repo[i].content = (char *) realloc(repo[i].content, (strlen_content + newWord_size - oldWord_size + 3) * sizeof(char));
                            allocationControl( fout, repo[i].content );

                            /* Sliding remaining part */
                            for(k = 0; k < (newWord_size - oldWord_size); k++)
                                for(m = 0; m < (strlen_content - oldWord_location - oldWord_size); m++)
                                    repo[i].content[strlen_content - m + k ] = repo[i].content[strlen_content - m + k - 1];

                            /* Placing new word */
                            for(k = 0; k < newWord_size; k++)
                                repo[i].content[oldWord_location + k] = newWord[k];

                            strlen_content = strlen(repo[i].content);
                            repo[i].content[strlen_content] = '\0';
                        }

                        /* Placing the smaller part of word and Only sliding remaining part */
                        if( newWord_size < oldWord_size )
                        {
                            /* Placing new word */
                            for(k = 0; k < newWord_size; k++)
                                repo[i].content[oldWord_location + k] = newWord[k];

                            /* sliding content */
                            for(m = 0; m < ( oldWord_size - newWord_size ); m++)
                            {
                                for(k = 0; repo[i].content[ oldWord_location + newWord_size + k + 1 ] != '\0'; k++)
                                    repo[i].content[ oldWord_location + newWord_size + k ] = repo[i].content[ oldWord_location + newWord_size + k + 1 ];
                                repo[i].content[ oldWord_location + newWord_size + k ] = '\0';
                            }

                            strlen_content = strlen(repo[i].content);

                            /* reallocation in case old word is bigger */
                            if( newWord_size < oldWord_size )
                                repo[i].content = (char *) realloc(repo[i].content, (strlen_content + newWord_size - oldWord_size + 3) * sizeof(char));
                            allocationControl( fout, repo[i].content );
                        }
                    }
                }
            }
            /* Controlling whether old word is found or not */
            if(oldWord_location == -1)
            {
                fprintf(fout, "The word you specified as old word is not in content of file.\n");
                return;
            }
            break;
        }
    }
    if( i == file_num )
    {
        fprintf(fout, "The file you entered is not found. Be sure that name and extension are correct.\n");
        return;
    }
    isEmptyFile(repo[i].content);

    free(name);
    free(extension);
    free(oldWord);
    free(newWord);
}

void print(FILE* fout, int file_num, const char* command_line, repository repo)
{
    int i = 0, j = 0, t = 0, k = 0;
    char *str, *name, *extension;
    int str_size = 0, str_limit = 15;
    int name_size = 0, name_limit = 15;
    int extension_size = 0, extension_limit = 15;
    int flag = 0;   /* This variable controls if "no such file found" case */
    int dotCount = 0;   /* This variable is for controlling name validity */

    if( lookingForParameter(command_line, "-a") != -1 )
    {
        int parameter_index = lookingForParameter(command_line, "-a");
        /* Controlling invalid command case */
        while(j < (parameter_index - 3))
        {
            if(command_line[j] != ' ')
            {
                fprintf(fout, "Invalid command format.\n");
                return;
            }
            j++;
        }
        j = 0;
        while((parameter_index+j) < strlen(command_line))
        {
            if(command_line[parameter_index+j] != ' ')
            {
                fprintf(fout, "Invalid command format.\n");
                return;
            }
            j++;
        }

        for(i = 0; i < file_num; i++)
            fprintf(fout, "Filename %d: %s.%s\n", repo[i].no, repo[i].filename, repo[i].extension);
        if(file_num == 0)   fprintf(fout, "No file in repository.\n");
        return;
    }
    else if( ( i = lookingForParameter(command_line, "-e")) != -1 )
    {
        int parameter_index = lookingForParameter(command_line, "-e");
        /* Controlling invalid command case*/
        while(j < (parameter_index - 3))
        {
            if(command_line[j] != ' ')
            {
                fprintf(fout, "Invalid command format.\n");
                return;
            }
            j++;
        }
        j = 0;

        str = (char *) calloc(str_limit, sizeof(char));

        /* ignoring spaces between parameter and data */
        while( command_line[ i + str_size ] == ' ' )
        {
            i++;
            continue;
        }
        while( command_line[ i + str_size ] != '\n' && command_line[ i + str_size ] != '\r' && command_line[ i + str_size ] != '\0' && command_line[ i + str_size ] != ' ' )
        {
            str[str_size] = command_line[ i + str_size ];
            str_size++;

            if(str_size == str_limit)
                extendBuffer(fout, &str_limit, &str);
        }
        str = (char *) realloc(str, (str_limit + 2) * sizeof(char));
        allocationControl( fout, str );
        str[str_size] = '\0';

        /* Controlling invalid command case*/
        while((j + parameter_index + strlen(str)) < strlen(command_line))
        {
            if(command_line[j + parameter_index + strlen(str)] != ' ')
            {
                fprintf(fout, "Invalid command format.\n");
                return;
            }
            j++;
        }
        j = 0;

        for(i = 0; i < file_num; i++)
        {
            if( !(strcmp(str, repo[i].extension)) )
            {
                fprintf(fout, "Filename %d: %s\nText: %s\n", repo[i].no, repo[i].filename, repo[i].content);
                flag = 1;
            }

        }
        if(!flag)   fprintf(fout, "There is no file in given extension.\n");

        free(str);
        return;
    }
    else if( lookingForParameter(command_line, "-c") != -1 )
    {
        int parameter_index = lookingForParameter(command_line, "-c");
        /* Controlling invalid command case */
        while(j < (parameter_index - 3))
        {
            if(command_line[j] != ' ')
            {
                fprintf(fout, "Invalid command format.\n");
                return;
            }
            j++;
        }
        j = 0;
        while((parameter_index+j) < strlen(command_line))
        {
            if(command_line[parameter_index+j] != ' ')
            {
                fprintf(fout, "Invalid command format.\n");
                return;
            }
            j++;
        }

        for(i = 0; i < file_num; i++)
            fprintf(fout, "Num: %d\nName: %s\nText: %s\n", repo[i].no, repo[i].filename, repo[i].content);
        return;
    }
        /* The cases that we need name and extension parts in command line */
    else if( lookingForParameter(command_line, "-n") != -1 )
    {
        i = 0;
        int name_index = lookingForParameter(command_line, "-n");
        if(name_index == -1)
        {
            fprintf(fout, "The name command is not given.\n");
            return;
        }

        /* Preventing segmentation fault in "print -t -n" case */
        if(name_index >= strlen(command_line))
        {
            fprintf(fout, "The name is not entered.\n");
            return;
        }

        /* Controlling name format (there should be only one dot character for extension specifier and not contain spaces)	*/
        while (command_line[name_index + i] != ' ' && command_line[name_index + i] != '\n') {
            if (command_line[name_index + i] == '.') dotCount++;
            i++;
        }
        if (dotCount != 1) {
            fprintf(fout, "The format of name is invalid. Please check name of file. There is more than one dot or not any or spaces. There may not be extension part.\n");
            return;
        }

        name = (char *) calloc(name_limit, sizeof(char));
        allocationControl( fout, name );

        extension = (char *) calloc(extension_limit, sizeof(char));
        allocationControl( fout, extension );

        takingName(fout, &name_index, &name_size, &name_limit, command_line, &name);
        takingExtension(fout, &name_index, &extension_size, &extension_limit, command_line, &extension);

        if( lookingForParameter(command_line, "-t") != -1 )
        {
            int small_parameter = 0;
            int temp_name_index = lookingForParameter(command_line, "-n");
            int text_index = lookingForParameter(command_line, "-t");

            /* Controlling invalid command case*/
            small_parameter = temp_name_index < text_index ? temp_name_index : text_index;
            while(j < (small_parameter-3))
            {
                if(command_line[j] != ' ')
                {
                    fprintf(fout, "Invalid command format.\n");
                    return;
                }
                j++;
            }

            for(i = 0; i < file_num; i++)
            {
                if(!(strcmp(name, repo[i].filename)) && !(strcmp(extension, repo[i].extension)))
                {
                    fprintf(fout, "Text: %s\n", repo[i].content);
                    break;
                }
            }
            if( i == file_num )
            {
                fprintf(fout, "The file you entered is not found. Be sure that name and extension are correct.\n");
                return;
            }
            return;
        }
        else if( lookingForParameter(command_line, "-cw") != -1 )
        {
            int word_index = lookingForParameter(command_line, "-cw");
            int small_parameter = 0;
            int temp_name_index = lookingForParameter(command_line, "-n");

            /* Controlling invalid command case*/
            small_parameter = temp_name_index < word_index ? temp_name_index : word_index;
            while(j < (small_parameter-3))
            {
                if(command_line[j] != ' ')
                {
                    fprintf(fout, "Invalid command format.\n");
                    return;
                }
                j++;
            }
            j = 0;

            /* Preventing segmentation fault in "print -n abc.txt -cw" case */
            if(word_index >= strlen(command_line))
            {
                fprintf(fout, "The word is not entered.\n");
                return;
            }

            str = (char *) calloc(str_limit, sizeof(char));


            while( command_line[ word_index ] != '\n' && command_line[ word_index ] != '\r' )
            {
                str[str_size++] = command_line[ word_index++ ];

                if(str_size == str_limit)
                    extendBuffer(fout, &str_limit, &str);
            }
            str = (char *) realloc(str, (str_limit + 2) * sizeof(char));
            allocationControl( fout, str );
            str[str_size] = '\0';


            int word_number = 0;
            for(i = 0; i < file_num; i++)
            {
                if(!(strcmp(name, repo[i].filename)) && !(strcmp(extension, repo[i].extension)))
                {
                    for(j = 0; repo[i].content[j] != '\0' ; j++)
                    {
                        if(repo[i].content[j] == str[0])
                        {
                            t = j;
                            for(k = 0; str[k] != '\0'; k++)
                            {
                                if( repo[i].content[t + k] != str[k] )	break;
                            }
                            if( k == strlen(str) )
                                word_number++;
                        }
                    }
                    break;
                }
            }
            if( i == file_num )
            {
                fprintf(fout, "The file you entered is not found. Be sure that name and extension are correct.\n");
                return;
            }
            fprintf(fout, "Text: %s\nNumber Of Occurrence of \"%s\" : %d\n", repo[i].content, str, word_number);

            free(str);
            return;
        }
        else if( lookingForParameter(command_line, "-cs") != -1 )
        {
            int sentence_index = lookingForParameter(command_line, "-cs");
            int small_parameter = 0;
            int temp_name_index = lookingForParameter(command_line, "-n");

            /* Controlling invalid command case*/
            small_parameter = temp_name_index < sentence_index ? temp_name_index : sentence_index;
            while(j < (small_parameter-3))
            {
                if(command_line[j] != ' ')
                {
                    fprintf(fout, "Invalid command format.\n");
                    return;
                }
                j++;
            }
            j = 0;

            int sentence_number = 0;
            for(i = 0; i < file_num; i++)
            {
                if(!(strcmp(name, repo[i].filename)) && !(strcmp(extension, repo[i].extension)))
                {
                    for(j = 0; repo[i].content[j] != '\0'; j++)
                        if( repo[i].content[j] == '.' || repo[i].content[j] == '?' || repo[i].content[j] == '!' )
                            sentence_number++;
                    break;
                }
            }
            if( i == file_num )
            {
                fprintf(fout, "The file you entered is not found. Be sure that name and extension are correct.\n");
                return;
            }
            fprintf(fout, "Number Of Sentences : %d\n", sentence_number);
        }

        free(name);
        free(extension);
        return;
    }
    else
    {
        fprintf(fout, "The parameter is missing or invalid.\n");
        return;
    }
}
