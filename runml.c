//  CITS2002 Project 1 2024
//  Student1:   23097928   Harsh Chaudhari
//  Student2:   23087371   Jayden Hondris

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
    sprintf(command, "cc -o %s %s.c", filename, filename);
    system(command);
}

int main(void)
{
    return 0;
}