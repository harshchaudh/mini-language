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

#define MAX_LINE 100          // Maximum number of characters in a line
#define MAX_INPUT_LINES 1000  // Maximum number of lines in the input file
#define IDENTIFIER_LENGTH 64  // Maximum length of an identifier
#define EXPRESSION_LENGTH 256 // Maximum length of an expression
#define INCLUDE_EXT 1         // Include the extension in the filename
#define EXCLUDE_EXT 0         // Exclude the extension in the filename
#define RED "\033[31m"
#define WHITE "\033[37m"
#define RESET "\033[0m"

typedef struct variable {
    char name[IDENTIFIER_LENGTH]; // Variable name
    double value;                 // Variable value
} Variable;

typedef struct expression {
    char expression[256];
} Expression;

typedef enum commandType {
    ASSIGNMENT,
    PRINT,
    FUNCTION_CALL,
    FUNCTION_DEFINE,
    FUNCTION_RETURN,
} CommandType;

typedef struct function Function;

typedef struct command {
    CommandType type;
    Variable var;
    Expression exp;
    Function *func;
} Command;

typedef struct function {
    char name[IDENTIFIER_LENGTH];

    Variable *args;
    int argCount;

    char returnVar[IDENTIFIER_LENGTH];

    Command *commands;
    int statementCount;
} Function;

Command parseAssignment(char *line)
{
    Command command;
    command.type = ASSIGNMENT;
    sscanf(line, "%s <- %le", command.var.name, &command.var.value);

    printf(RED "@ Assignment Variable name: %s, Value: %lf\n" RESET, command.var.name, command.var.value);

    return command;
}

Command parseFunctionDefine(char *line)
{
    Command command;
    command.type = FUNCTION_DEFINE;
    char argsBuffer[100]; // Buffer to store the arguments

    command.func = malloc(sizeof(Function));

    sscanf(line, "function %s %[^\n]", command.func->name, argsBuffer);

    char *argsBufferCopy = malloc(strlen(argsBuffer) + 1);
    strcpy(argsBufferCopy, argsBuffer);

    // Tokenize argsBuffer to count the arguments
    char *arg = strtok(argsBuffer, " ");
    command.func->argCount = 0;
    while (arg != NULL) {
        command.func->argCount++;
        arg = strtok(NULL, " ");
    }

    command.func->args = malloc(command.func->argCount * sizeof(Variable));

    arg = strtok(argsBufferCopy, " ");
    int i = 0;
    while (arg != NULL) {
        strcpy(command.func->args[i].name, arg);
        command.func->args[i].value = 0.0;
        arg = strtok(NULL, " ");
        i++;
    }

    free(argsBufferCopy);

    command.func->statementCount = 0;
    command.func->commands = malloc(100 * sizeof(Command));

    return command;
}

Command parseFunctionReturn(char *line, Command command)
{
    command.type = FUNCTION_RETURN;
    sscanf(line, "\treturn %[^\n]", command.func->returnVar);

    return command;
}

int getFunctionIndex(char *line, Command commandFunctions[], int functionCount)
{
    char functionName[IDENTIFIER_LENGTH];
    sscanf(line, "%[^(]", functionName);

    for (int i = 0; i < functionCount; i++) {
        if (strcmp(commandFunctions[i].func->name, functionName) == 0) {
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

    while (arg != NULL && i < command.func->argCount) {
        command.func->args[i].value = atof(arg);
        arg = strtok(NULL, " ");
        i++;
    }

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
    command.var.value = 0;
    strcpy(command.var.name, "\0");

    sscanf(line, "print %[^\n]", command.exp.expression);

    printf(RED "@ Parsed Expression: %s\n" RESET, command.exp.expression);

    return command;
}

void parseFunctionPrint(char *line, Function *func)
{
    func->commands[func->statementCount].type = PRINT;
    func->commands[func->statementCount].var.value = 0;
    strcpy(func->commands[func->statementCount].var.name, "\0");

    sscanf(line, "\tprint %[^\n]", func->commands[func->statementCount].exp.expression);

    func->statementCount++;
}

/**
 * @brief Creates an exectuable C file.
 *
 * @param newFilenameWithExt The new file to be created, including the extension.
 * @param commands The commands to be written to the file.
 * @param commandCount The number of commands to be written to the file.
 * @return `void`
 */
void createFile(const char *newFilenameWithExt, Command commands[], int commandCount, Command commandFunctions[], int functionCount)
{
    FILE *file = fopen(newFilenameWithExt, "w");
    if (file == NULL) {
        fprintf(stderr, "!Error: Unable to create file `%s`.\n", newFilenameWithExt);
        exit(EXIT_FAILURE);
    }

    fprintf(file, "#include <stdio.h>\n\n");

    for (int i = 0; i < functionCount; i++) {
        fprintf(file, "double %s(", commandFunctions[i].func->name);
        for (int j = 0; j < commandFunctions[i].func->argCount; j++) {
            fprintf(file, "double %s", commandFunctions[i].func->args[j].name);
            if (j < commandFunctions[i].func->argCount - 1) {
                fprintf(file, ", ");
            }
        }
        fprintf(file, ") \n{\n");

        for (int j = 0; j < commandFunctions[i].func->statementCount; j++) {
            switch (commandFunctions[i].func->commands[j].type) {
            case PRINT:
                fprintf(file, "\tprintf(\"%%f\\n\", %s);\n", commandFunctions[i].func->commands[j].exp.expression);
                break;
            default:
                fprintf(stderr, "!Error: Unknown command type %d.\n", commandFunctions[i].func->commands[j].type);
                exit(EXIT_FAILURE);
            }
        }

        if (commandFunctions[i].type == FUNCTION_RETURN) {
            fprintf(file, "\treturn %s;\n", commandFunctions[i].func->returnVar);
        }

        fprintf(file, "}\n\n");
    }

    fprintf(file, "int main() \n{\n");
    for (int i = 0; i < commandCount; i++) {
        switch (commands[i].type) {
        case ASSIGNMENT:
            printf(RED "@ Command type: ASSIGNMENT(%d), Variable name: %s, Value: %f\n" RESET, commands[i].type, commands[i].var.name, commands[i].var.value);
            fprintf(file, "\tdouble %s = %f;\n", commands[i].var.name, commands[i].var.value);
            fflush(stdout);
            break;

        case PRINT:
            printf(RED "@ Command type: PRINT(%d), Expression: %s\n" RESET, commands[i].type, commands[i].exp.expression);
            if (strchr(commands[i].exp.expression, '.') != NULL || strlen(commands[i].exp.expression) == 1) {
                fprintf(file, "\tprintf(\"%%f\\n\", %s);\n", commands[i].exp.expression);
            } else {
                fprintf(file, "\tprintf(\"%%d\\n\", (int)(%s));\n", commands[i].exp.expression);
            }
            fflush(stdout);
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
        printf(RED "@ Comment removed from line\n" RESET);
    }
}

/**
 * @brief Checks if an expression is valid.
 *
 * @param expression The expression to be validated.
 * @return `1` if the expression is valid, `0` otherwise.
 */
int isValidExpression(const char *expression)
{
    int parenthesis_count = 0;
    for (const char *p = expression; *p != '\0'; p++) {
        if (isalnum(*p) || strchr(" +-*/().,", *p)) {
            // Check for parentheses balance
            if (*p == '(') {
                parenthesis_count++;
            } else if (*p == ')') {
                if (parenthesis_count <= 0)
                    return 0; // Unmatched closing parenthesis
                parenthesis_count--;
            }
        } else if (*p == '_') {
            // Allow underscore for function names
            continue;
        } else {
            return 0; // Invalid character
        }
    }
    return parenthesis_count == 0; // Ensure all parentheses are matched
}

void validateSyntax(char *line)
{
    // Check if line is an assignment
    if (strstr(line, "<-") != NULL) {
        // Validate the structure of the assignment
        char identifier[IDENTIFIER_LENGTH];
        char expression[EXPRESSION_LENGTH];
        if (sscanf(line, "%63s <- %[^\n]", identifier, expression) != 2) {
            fprintf(stderr, "!Error: Invalid assignment\n");
        } else if (!isValidExpression(expression)) {
            fprintf(stderr, "!Error: Invalid character in assignment expression\n");
        }
    } else if (strstr(line, "print") != NULL) {
        // Ensure it matches the "print expression" format
        char expression[EXPRESSION_LENGTH];
        if (sscanf(line, "print %[^\n]", expression) != 1) {
            fprintf(stderr, "!Error: Invalid print statement\n");
        } else if (!isValidExpression(expression)) {
            fprintf(stderr, "!Error: Invalid character in print expression\n");
        } else {
            fprintf(stderr, "!Error: Unrecognized statement\n");
        }
        // skip empty lines
    } else if (line[0] == '\0') {
        return;
    }
    // To be continued to work on function and their body
    else {
        return;
        // fprintf(stderr, "!Error: Unrecognized statement\n");
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
    if (file == NULL) {
        fprintf(stderr, "!Error: `%s` not found.\n", filename);
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = '\0'; // Remove trailing newline characters (both \r and \n)

        printf(RED "@ Parsing Line: %s\n" RESET, line);
        removeComment(line);
        if (strstr(line, "<-") != NULL) {
            commands[commandCount++] = parseAssignment(line);
        } else if (strstr(line, "function") != NULL) {
            commandFunctions[functionCount] = parseFunctionDefine(line);
            functionIndex = functionCount;
            functionCount++;
        } else if (strstr(line, "print") != NULL && functionIndex == -1) {
            commands[commandCount++] = parsePrint(line);
        } else if (functionIndex != -1 && strstr(line, "print") != NULL) {
            parseFunctionPrint(line, commandFunctions[functionIndex].func);
            functionIndex = -1;
        } else if (strstr(line, "return") != NULL) {
            commandFunctions[functionIndex] = parseFunctionReturn(line, commandFunctions[functionIndex]);
            functionIndex = -1;
        } else if ((strstr(line, "(") != NULL) && (strstr(line, ")") != NULL)) {
            functionIndex = getFunctionIndex(line, commandFunctions, functionCount);
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
