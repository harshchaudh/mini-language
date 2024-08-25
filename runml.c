//  CITS2002 Project 1 2024
//  Student1:   23097928   Harsh Chaudhari
//  Student2:   23087371   Jayden Hondris
//  Platform:   Linux

#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 100 // Maximum number of characters in a line


/**
 * @brief Creates a file using the given filename.
 *
 * @param filename The file to be created.
 * @return `void`
 */
void createFile(char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to create file `%s`.\n", filename);
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

/**
 * @brief Reads a file using the given filename.
 *
 * @param filename The file to be read.
 * @return `void`
 */
void readFile(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: `%s` not found.\n", filename);
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    printf("\n");

    fclose(file);
}

/**
 * @brief Compiles a program using the given filename.
 *
 * Command: `cc -std=c11 -o <<filename>> <<filename.c>>`
 *
 * @param filename The file to be compiled
 * @return `void`
 */
void compile(char *filename)
{
    char command[100];
    sprintf(command, "cc -std=c11 -o %s %s.c", filename, filename);
    system(command);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Error: Incorrect number of arguments.\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}