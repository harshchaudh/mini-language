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


/** @brief A structure that defines a variable. */
typedef struct variable {
    char name[64];
    double value;
} Variable;

/** @brief An enum that defines all the command types. */
typedef enum commandType {
    ASSIGNMENT,
    PRINT,
    // COMMENT,
    // FUNCTION,
    // RETURN
} CommandType;

/** @brief A structure that defines a command. */
typedef struct command {
    CommandType type;
    Variable var;
} Command;

Command parseAssignment(char *line)
{
    Command command;
    command.type = ASSIGNMENT;
    sscanf(line, "%s <- %le", command.var.name, &command.var.value);
    
    return command;
}

Command parsePrint(char *line)
{
    Command command;
    command.type = PRINT;
    sscanf(line, "print %s", command.var.name);
    return command;
}

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
 * @param newFilenameWithExt The new file to be created, including the extension.
 * @param commands The commands to be written to the file.
 * @param commandCount The number of commands to be written to the file.
 * @return `void`
 */
void createFile(const char *newFilenameWithExt, Command commands[], int commandCount)
{
    FILE *file = fopen(newFilenameWithExt, "w");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to create file `%s`.\n", newFilenameWithExt);
        exit(EXIT_FAILURE);
    }

    fprintf(file, "#include <stdio.h>\n\n");
    fprintf(file, "int main() {\n");

    for (int i = 0; i < commandCount; i++) {
        if (commands[i].type == ASSIGNMENT) {
            fprintf(file, "    int %s = %f;\n", commands[i].var.name, commands[i].var.value);
        } else if (commands[i].type == PRINT) {
            fprintf(file, "    printf(\"%%d\\n\", %s);\n", commands[i].var.name);
        }
    }

    fprintf(file, "    return 0;\n");
    fprintf(file, "}\n");

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
    int result = system(command);
    
    if(result != 0) {
        fprintf(stderr, "!Error: Compilation failed.\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Runs a program using the given filename.
 *
 * cmd: `./<<filename>>`
 *
 * @param filename The file to be executed
 * @return `void`
 */
void run(const char *filename)
{
    char command[100];
    sprintf(command, "./%s", filename);
    int result = system(command);

    if(result != 0) {
        fprintf(stderr, "!Error: Execution failed.\n");
        exit(EXIT_FAILURE);
    }
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

void generateCode(const char *input)
{
    Command commands[1000];
    int commandCount = 0;

    char *line = strtok((char *)input, "\n"); // Causing segfault: Input is not being broken into separate lines.
    while (line != NULL) {
        if (strstr(line, "<-") != NULL) {
            commands[commandCount++] = parseAssignment(line);
        } else if (strstr(line, "print") != NULL) {
            commands[commandCount++] = parsePrint(line);
        }

        line = strtok(NULL, "\n");
    }

    char *newFilenameWithExt = getFilename(INCLUDE_EXT);
    char *newFilenameWithoutExt = getFilename(EXCLUDE_EXT);

    createFile(newFilenameWithExt, commands, commandCount);

    free(newFilenameWithExt);
    free(newFilenameWithoutExt);
}

int main(int argc, char *argv[])
{
    if (argc != 1) {
        fprintf(stderr, "Error: Incorrect number of arguments.\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
