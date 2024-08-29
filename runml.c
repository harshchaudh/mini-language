//  CITS2002 Project 1 2024
//  Student1:   23097928   Harsh Chaudhari
//  Student2:   23087371   Jayden Hondris
//  Platform:   Linux

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE 100  // Maximum number of characters in a line
#define INCLUDE_EXT 1 // Include the extension in the filename
#define EXCLUDE_EXT 0 // Exclude the extension in the filename

/**
 * @brief A structure that defines a variable.
 */
typedef struct variable {
    char *name;
    char *value;
} Variable;

/**
 * @brief An enum that defines all the command types.
 */
typedef enum commandType{
    ASSIGNMENT,
    PRINT,
    //COMMENT,
    //FUNCTION,
    //RETURN
} CommandType;

/**
 * @brief A structure that defines a command.
 */
typedef struct command{
    CommandType type;
    Variable var;
} Command;

/**
 * @brief Generates a filename using the process ID.
 *
 * Format: `ml-<<pid>>.c`
 * @param includeExtension `INCLUDE_EXT` to include the extension, `EXCLUDE_EXT` to exclude the extension.
 * @return The generated filename.
 */
char *getFilename(int includeExtension)
{
    char *newFilename = malloc(12 * sizeof(char)); // ml-XXXXX.c
    if (newFilename == NULL) {
        perror("Unable to allocate memory");
        exit(EXIT_FAILURE);
    }

    __pid_t pid = getpid();
    char pidString[6];             // PID is 5 digits long
    sprintf(pidString, "%d", pid); // Convert PID to string

    strcpy(newFilename, "ml-");
    strcat(newFilename, pidString);

    if (includeExtension) {
        strcat(newFilename, ".c");
    }

    return newFilename;
}

/**
 * @brief Creates an exectuable C file.
 *
 * @param filename The contents of the file to be created, including the extension.
 * @param newFilename The new filename to be created, including the extension.
 * @return `void`
 */
void createFile(const char *filename, const char *newFilename)
{
    FILE *file = fopen(newFilename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to create file `%s`.\n", newFilename);
        exit(EXIT_FAILURE);
    }

    // Temporary: create a simple Hello, World! program
    fprintf(file,
            "#include <stdio.h>\n\n"
            "int main() {\n"
            "    printf(\"Hello, World!\\n\");\n"
            "    return 0;\n"
            "}\n");

    fclose(file);
}

/**
 * @brief Compiles a program using the given filename.
 *
 * cmd: `cc -std=c11 -o <<filename>> <<filename>>.c`
 *
 * @param filename The file without an extension to be compiled
 * @return `void`
 */
void compile(const char *filename)
{
    char command[100];
    sprintf(command, "cc -std=c11 -o %s %s.c", filename, filename);
    system(command);
}

/**
 * @brief Removes a file using the given filename.
 *
 * Command: `rm <<filename>>`
 *
 * @param filename The file to be removed
 * @return `void`
 */
void removeFile(char *filename)
{
    char command[100];
    sprintf(command, "rm %s", filename);
    system(command);
}

/**
 * @brief Removes a comment from a line.
 *
 * @param line The line to be processed.
 * @return `void`
 */
void removeComment(char *line)
{
    char *comment = strchr(line, '#');
    if (comment != NULL) {
        *comment = '\0';
    }
}

/**
 * @brief Reads a file using the given filename.
 *
 * @param filename The file to be read.
 * @return `void`
 */
void readFile(const char *filename)
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

int main(int argc, char *argv[])
{
    if (argc != 1) {
        fprintf(stderr, "Error: Incorrect number of arguments.\n");
        exit(EXIT_FAILURE);
    }

    char *newFilenameWithExt = getFilename(INCLUDE_EXT);
    char *newFilenameWithoutExt = getFilename(EXCLUDE_EXT);

    free(newFilenameWithExt);
    free(newFilenameWithoutExt);

    exit(EXIT_SUCCESS);
}
