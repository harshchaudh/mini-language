//  CITS2002 Project 1 2024
//  Student1:   23097928   Harsh Chaudhari
//  Student2:   23087371   Jayden Hondris
//  Platform:   Linux (Ubuntu 22.04.4 LTS)

// Reference: DoctorWkt. 2019. acwj. https://github.com/DoctorWkt/acwj. (2024).

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE 100         // Maximum number of characters in a line
#define MAX_INPUT_LINES 1000 // Maximum number of lines in the input file
#define INCLUDE_EXT 1        // Include the extension in the filename
#define EXCLUDE_EXT 0        // Exclude the extension in the filename

/** @brief A structure that defines a variable. */
typedef struct variable {
    char name[64];
    double value;
} Variable;

typedef struct expression {
    char expression[256];
} Expression;

/** @brief An enum that defines all the command types. */
typedef enum commandType {
    ASSIGNMENT,
    PRINT,
} CommandType;

/** @brief A structure that defines a command. */
typedef struct command {
    CommandType type;
    Variable var;
    Expression exp;
} Command;

/**
 * @brief Parses an assignment command.
 *
 * @param line The line to be processed.
 * @return The parsed command.
 */
Command parseAssignment(char *line)
{
    Command command;
    command.type = ASSIGNMENT;
    sscanf(line, "%s <- %le", command.var.name, &command.var.value);

    printf("@ Assignment Variable name: %s, Value: %lf\n", command.var.name, command.var.value);

    return command;
}
/**
 * @brief Parses a print command.
 *
 * @param line The line to be processed.
 * @return The parsed command.
 */
Command parsePrint(char *line)
{
    Command command;
    command.type = PRINT;

    sscanf(line, "print %[^\n]", command.exp.expression);

    printf("@ Parsed Expression: %s\n", command.exp.expression);

    return command;
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

        printf("@ Command type: %d, Variable name: %s, Value: %f\n", commands[i].type, commands[i].var.name, commands[i].var.value);

        switch (commands[i].type) {
        case ASSIGNMENT:
            fprintf(file, "\tdouble %s = %f;\n", commands[i].var.name, commands[i].var.value);
            break;

        case PRINT:
            fprintf(file, "\tprintf(\"%%g\\n\", %s);\n", commands[i].exp.expression);
            break;

        default:
            fprintf(stderr, "!Error: Unknown command type %d.\n", commands[i].type);
            exit(EXIT_FAILURE);
        }
    }

    fprintf(file, "\treturn 0;\n}\n");

    fclose(file);
}

/**
 * @brief Removes all files with the prefix `ml-`.
 *
 * Command: `rm ml-*`
 * @param `void`
 * @return `void`
 */
void DEV_TOOL_REMOVE_ML(void)
{
    char command[100];
    sprintf(command, "rm ml-*");
    system(command);
}

/**
 * @brief Generates a filename using the process ID.
 *
 * Format: `ml-<<pid>>.c`
 * @param includeExtension `INCLUDE_EXT` to include the extension, `EXCLUDE_EXT` to exclude the extension.
 * @return The generated filename.
 */
char *createFilename(int includeExtension)
{
    char *newFilename = malloc(12 * sizeof(char)); // ml-XXXXX.c
    if (newFilename == NULL) {
        perror("Unable to allocate memory");
        exit(EXIT_FAILURE);
    }

    // pid_t pid = getpid(); // Get the process ID (Apple, MacOS)
    __pid_t pid = getpid(); // Get the process ID (Linux, Ubuntu)

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
 * @brief Compiles a program using the given filename.
 *
 * cmd: `cc -std=c11 -o <<filename>> <<filename>>.c`
 *
 * @param filenameWithoutExt The file without an extension to be compiled
 * @return `void`
 */
void compile(const char *filenameWithoutExt)
{
    char command[100];
    sprintf(command, "cc -std=c11 -o %s %s.c", filenameWithoutExt, filenameWithoutExt);
    int result = system(command);

    if (result != 0) {
        fprintf(stderr, "!Error: Compilation failed.\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Runs a program using the given filename.
 *
 * cmd: `./<<filename>>`
 *
 * @param filenameWithoutExt The file to be executed
 * @return `void`
 */
void run(const char *filenameWithoutExt)
{
    char command[100];
    sprintf(command, "./%s", filenameWithoutExt);
    int result = system(command);

    if (result != 0) {
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
        printf("@ Comment removed: %s\n", line);
    }
}

/**
 * @brief Generates code from a given file.
 *
 * @param filename The file to be processed.
 * @param newFilenameWithExt The new file to be created, including the extension.
 * @return `void`
 */
void generateCode(const char *filename, const char *newFilenameWithExt)
{
    Command commands[MAX_INPUT_LINES];
    int commandCount = 0;

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: `%s` not found.\n", filename);
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {

        printf("@ Line: %s\n", line);
        removeComment(line);

        if (strstr(line, "<-") != NULL) {
            commands[commandCount++] = parseAssignment(line);
        } else if (strstr(line, "print") != NULL) {
            commands[commandCount++] = parsePrint(line);
        }
    }

    fclose(file);

    createFile(newFilenameWithExt, commands, commandCount);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "!Error: Incorrect number of arguments.\n");
        exit(EXIT_FAILURE);
    }

    char *newFilenameWithExt = createFilename(INCLUDE_EXT);
    char *newFilenameWithoutExt = createFilename(EXCLUDE_EXT);

    DEV_TOOL_REMOVE_ML();

    generateCode(argv[1], newFilenameWithExt);
    compile(newFilenameWithoutExt);
    run(newFilenameWithoutExt);
    // removeFile(newFilenameWithoutExt);
    // removeFile(newFilenameWithExt);

    free(newFilenameWithExt);
    free(newFilenameWithoutExt);

    exit(EXIT_SUCCESS);
}
