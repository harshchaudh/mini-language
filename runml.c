//  CITS2002 Project 1 2024
//  Student1:   23097928   Harsh Chaudhari
//  Student2:   23087371   Jayden Hondris
//  Platform:   Linux (Ubuntu 22.04.4 LTS)

// Reference: DoctorWkt. 2019. acwj. https://github.com/DoctorWkt/acwj. (2024).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE 100         // Maximum number of characters in a line
#define MAX_INPUT_LINES 1000 // Maximum number of lines in the input file
#define IDENTIFIER_LENGTH 64 // Maximum length of an identifier
#define INCLUDE_EXT 1        // Include the extension in the filename
#define EXCLUDE_EXT 0        // Exclude the extension in the filename

/** @brief A structure that defines a variable. */
typedef struct variable
{
    char name[IDENTIFIER_LENGTH];
    double value;
} Variable;

/** @brief A structure that defines a function. */
typedef struct function
{
    char name[IDENTIFIER_LENGTH];
    Variable *args;
    int argCount;
    char returnVar[IDENTIFIER_LENGTH];
    int voidStatus;
} Function;

/** @brief An enum that defines all the command types. */
typedef enum commandType
{
    ASSIGNMENT,
    PRINT,
    FUNCTION_CALL,
    FUNCTION_DEFINE,
    FUNCTION_RETURN,
} CommandType;

/** @brief A structure that defines a command. */
typedef struct command
{
    CommandType type;
    Function func;
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

Command parseFunctionDefine(char *line)
{
    Command command;
    command.type = FUNCTION_DEFINE;
    char argsBuffer[100];

    sscanf(line, "function %s %[^\n]", command.func.name, argsBuffer); // `%[^\n]` reads until the end of the line; line is stored in argsBuffer

    char *argsBufferCopy = malloc(strlen(argsBuffer) + 1);
    strcpy(argsBufferCopy, argsBuffer);

    char *arg = strtok(argsBuffer, " ");
    command.func.argCount = 0;
    while (arg != NULL)
    {
        arg = strtok(NULL, " ");
        command.func.argCount++; // Increment the argument count for that function
    }

    command.func.args = malloc(command.func.argCount * sizeof(Variable));
    arg = strtok(argsBufferCopy, " ");
    int i = 0;
    while (arg != NULL)
    {
        strcpy(command.func.args[i].name, arg);
        command.func.args[i].value = 0.0;
        arg = strtok(NULL, " ");
        i++;
    }

    return command;
}

Command parseFunctionReturn(char *line, Command command)
{
    command.type = FUNCTION_RETURN;
    sscanf(line, "\treturn %[^\n]", command.func.returnVar);

    return command;
}

int getFunctionIndex(char *line, Command commandFunctions[], int functionCount)
{
    char functionName[IDENTIFIER_LENGTH];
    sscanf(line, "%[^(]", functionName);

    for (int i = 0; i < functionCount; i++)
    {
        if (strcmp(commandFunctions[i].func.name, functionName) == 0)
        {
            return i;
        }
    }

    return -1;
}

Command parseFunctionCall(char *line, Command command)
{
    command.type = FUNCTION_CALL;
    char argsBuffer[100];

    sscanf(line, "%*[^ (](%[^)])", argsBuffer); // `%[^)]` reads until `)`; line is stored in argsBuffer

    char *arg = strtok(argsBuffer, ", ");
    int i = 0;

    while (arg != NULL && i < command.func.argCount) {
        printf("Arg: %s\n", arg);
        command.func.args[i].value = atof(arg);
        arg = strtok(NULL, " ");
        i++;
    }

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
    if (newFilename == NULL)
    {
        perror("Unable to allocate memory");
        exit(EXIT_FAILURE);
    }

    pid_t pid = getpid(); // Get the process ID (Apple, MacOS)
    //__pid_t pid = getpid(); // Get the process ID (Linux, Ubuntu)

    char pidString[6];             // PID is 5 digits long
    sprintf(pidString, "%d", pid); // Convert PID to string

    strcpy(newFilename, "ml-");
    strcat(newFilename, pidString);

    if (includeExtension)
    {
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
void createFile(const char *newFilenameWithExt, Command commands[], int commandCount, Command functionCommands[], int functionCount)
{
    FILE *file = fopen(newFilenameWithExt, "w");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Unable to create file `%s`.\n", newFilenameWithExt);
        exit(EXIT_FAILURE);
    }

    fprintf(file, "#include <stdio.h>\n\n");

    for (int i = 0; i < functionCount; i++)
    {
        fprintf(file, "double %s(", functionCommands[i].func.name);
        for (int j = 0; j < functionCommands[i].func.argCount; j++)
        {
            fprintf(file, "double %s", functionCommands[i].func.args[j].name);
            if (j < functionCommands[i].func.argCount - 1)
            {
                fprintf(file, ", ");
            }
        }
        fprintf(file, ") \n{\n");

        if (functionCommands[i].type == FUNCTION_RETURN)
        {
            fprintf(file, "\treturn %s;\n", functionCommands[i].func.returnVar);
        }

        fprintf(file, "}\n\n");
    }

    fprintf(file, "int main() \n{\n");
    for (int i = 0; i < commandCount; i++)
    {
        if (commands[i].type == ASSIGNMENT)
        {
            fprintf(file, "\tdouble %s = %f;\n", commands[i].var.name, commands[i].var.value);
        }
        else if (commands[i].type == PRINT)
        {
            fprintf(file, "\tprintf(\"%%g\\n\", %s);\n", commands[i].var.name);
        }
        else if (commands[i].type == FUNCTION_CALL)
        {

            fprintf(file, "\t%s(", commands[i].func.name);
            if (commands[i].func.argCount > 0)
            {
                for (int j = 0; j < commands[i].func.argCount; j++)
                {
                    fprintf(file, "%g", commands[i].func.args[j].value);
                    if (j < commands[i].func.argCount - 1)
                    {
                        fprintf(file, ", ");
                    }
                }
            }

            fprintf(file, ");\n");
        }
    }

    fprintf(file, "\treturn 0;\n");
    fprintf(file, "}\n");

    fclose(file);
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

    if (result != 0)
    {
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

    if (result != 0)
    {
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
    if (comment != NULL)
    {
        *comment = '\0';
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

    Command commandFunctions[MAX_INPUT_LINES];
    int functionCount = 0;
    int functionIndex = -1;

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error: `%s` not found.\n", filename);
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file))
    {

        removeComment(line);

        if (strstr(line, "<-") != NULL)
        {
            commands[commandCount++] = parseAssignment(line);
        }
        else if (strstr(line, "print") != NULL)
        {
            commands[commandCount++] = parsePrint(line);
        }
        else if (strstr(line, "function") != NULL)
        {
            functionIndex = functionCount;
            commandFunctions[functionCount++] = parseFunctionDefine(line);
        }
        else if (strstr(line, "return") != NULL)
        {
            commandFunctions[functionIndex] = parseFunctionReturn(line, commandFunctions[functionIndex]);

        } else if ((strstr(line, "(") != NULL) && (strstr(line, ")") != NULL))
        {   
            int functionIndex = getFunctionIndex(line, commandFunctions, functionCount);
            commands[commandCount++] = parseFunctionCall(line, commandFunctions[functionIndex]);
        }
    }

    fclose(file);

    createFile(newFilenameWithExt, commands, commandCount, commandFunctions, functionCount);
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

int main(int argc, char *argv[])
{
    if (argc != 1)
    {
        fprintf(stderr, "Error: Incorrect number of arguments.\n");
        exit(EXIT_FAILURE);
    }

    char *newFilenameWithExt = getFilename(INCLUDE_EXT);
    char *newFilenameWithoutExt = getFilename(EXCLUDE_EXT);

    DEV_TOOL_REMOVE_ML();

    generateCode("test.ml", newFilenameWithExt);

    // Sample of how the functions can be used
    /*
    generateCode(argv[1], newFilenameWithExt);
    compile(newFilenameWithoutExt);
    run(newFilenameWithoutExt);
    removeFile(newFilenameWithoutExt);
    removeFile(newFilenameWithExt);
    */

    free(newFilenameWithExt);
    free(newFilenameWithoutExt);

    exit(EXIT_SUCCESS);
}
