//  CITS2002 Project 1 2024
//  Student1:   23097928   Harsh Chaudhari
//  Student2:   23087371   Jayden Hondris
//  Platform:   Linux

#include <stdio.h>
#include <stdlib.h>

/*
 * Compiles the given C file.
 *
 * Parameters:
 *   filename - the file to be compiled
 *
 * Returns:
 *   void
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
    } else {
        exit(EXIT_SUCCESS);
    }
    return 0;
}