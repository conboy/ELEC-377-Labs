#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <pwd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/stat.h>

//+
// File:	shell.c
//
// Pupose:	This program implements a simple shell program. It does not start
//		processes at this point in time. However, it will change directory
//		and list the contents of the current directory.
//
//		The commands are:
//		   cd name -> change to directory name, print an error if the directory doesn't exist.
//		              If there is no parameter, then change to the home directory.
//		   ls -> list the entries in the current directory.
//			      If no arguments, then ignores entries starting with .
//			      If -a then all entries
//		   pwd -> print the current directory.
//		   exit -> exit the shell (default exit value 0)
//				any argument must be numeric and is the exit value
//
//		if the command is not recognized an error is printed.
//-

#define CMD_BUFFSIZE 1024
#define MAXARGS 10

int splitCommandLine(char * commandBuffer, char* args[], int maxargs);
int doInternalCommand(char * args[], int nargs);
int doProgram(char * args[], int nargs);
char * skipChar(char * charPtr, char skip);

//+
// Function:	main
//
// Purpose:	The main function. Contains the read
//		eval print loop for the shell.
//
// Parameters:	(none)
//
// Returns:	integer (exit status of shell)
//-

int main() {
    // Testing for skipChar
    //char name[22] = {' ', ' ', 't', 'h', 'i', 's', ' ', 'i', 's', ' ', ' ', 'a', ' ', 's', 'e', 'n', 't', 'e', 'n', 'c', 'e', '\0'}; 

    char commandBuffer[CMD_BUFFSIZE];
    // note the plus one, allows for an extra null
    char *args[MAXARGS+1];



    // print prompt.. fflush is needed because
    // stdout is line buffered, and won't
    // write to terminal until newline
    printf("%%> ");
    fflush(stdout);

    while(fgets(commandBuffer,CMD_BUFFSIZE,stdin) != NULL){
        //printf("%s",commandBuffer);

	// remove newline at end of buffer
	int cmdLen = strlen(commandBuffer);
	if (commandBuffer[cmdLen-1] == '\n'){
	    commandBuffer[cmdLen-1] = '\0';
	    cmdLen--;
            //printf("<%s>\n",commandBuffer);
	}

	// split command line into words.(Step 2)
    int nargs = splitCommandLine(commandBuffer, args, MAXARGS+1);
	// TODO

	// add a null to end of array (Step 2)
    args[nargs] = '\0';
	// TODO

	// debugging
	printf("%d\n", nargs);
	int i;
	for (i = 0; i < nargs; i++){
	  printf("%d: '%s'\n",i,args[i]);
	}
	// element just past nargs
	//printf("%d: %x\n",i, args[i]);

    // check if 1 or more args then run command handler
    if (nargs > 0) {
        // if command doesnt exist call doProgram
        if (doInternalCommand(args, nargs) == 0) {
            if(doProgram(args, nargs) == 0) {
                fprintf(stderr, "Command not found\n");
            }
        }
    }

	// print prompt
	printf("%%> ");
	fflush(stdout);
    }
    return 0;
}

////////////////////////////// String Handling (Step 1) ///////////////////////////////////

//+
// Function:	skipChar
//
// Purpose:	To get rid of spaces at the beginning of the commandBuffer
//
// Parameters:
//    charPtr	Pointer to string
//    skip	character to skip
//
// Returns:	Pointer to first character after skipped chars
//		ID function if the string doesn't start with skip,
//		or skip is the null character
//-

char * skipChar(char * charPtr, char skip){
    if (*charPtr == '\0') return charPtr;
    while (*charPtr == ' ') charPtr++;

    return charPtr;
}

//+
// Funtion:	splitCommandLine
//
// Purpose:	TODO: give descritption of function
//
// Parameters:
//	TODO: parametrs and purpose
//
// Returns:	Number of arguments (< maxargs).
//
//-

int splitCommandLine(char * commandBuffer, char* args[], int maxargs){
   // TODO: Contents of function
   int numWords = 0; 
   char * charPtr;


   // Get pointer to first letter of first word in commandBuffer
   charPtr = skipChar(commandBuffer, ' ');
   // Add pointer to args array
   if (*charPtr != '\0') {
    args[numWords] = charPtr;

    // Increment number of words
    numWords++;
   } 
   
   

   while (1) {
    // Go to first Space
    charPtr = strchr(charPtr, ' ');
    if (charPtr == NULL) {
        break;
    }
    // Change value to \0 and go to the next char
    *charPtr = '\0';
    charPtr++;
    charPtr = skipChar(charPtr, ' ');
    // Add pointer to args array
    args[numWords] = charPtr;

    // Increment number of words
    numWords++;
   }

   return numWords;
}

////////////////////////////// External Program  (Note this is step 4, complete doeInternalCommand first!!) ///////////////////////////////////

// list of directorys to check for command.
// terminated by null value
char * path[] = {
    ".",
    "/usr/bin",
    NULL
};

//+
// Funtion:	doProgram
//
// Purpose:	TODO: add description of funciton
//
// Parameters:
//	TODO: add paramters and description
//
// Returns	int
//		1 = found and executed the file
//		0 = could not find and execute the file
//-

int doProgram(char * args[], int nargs){
  // find the executable
  
  // TODO: add body.
  // Note this is step 4, complete doInternalCommand first!!!

  return 1;
}

////////////////////////////// Internal Command Handling (Step 3) ///////////////////////////////////

// define command handling function pointer type
typedef void(*commandFunc)(char * args[], int nargs);

// associate a command name with a command handling function
struct cmdStruct{
   char 	* cmdName;
   commandFunc 	cmdFunc;
};

// prototypes for command handling functions
// TODO: add prototype for each comamand function
void exitFunc (char *args[], int nargs);
void pwdFunc(char* args[], int nargs);
void cdFunc(char* args[], int nargs);
void lsFunc(char* args[], int nargs);

// list commands and functions
// must be terminated by {NULL, NULL} 
// in a real shell, this would be a hashtable.
struct cmdStruct commands[] = {
   // TODO: add entry for each command
   {"exit", exitFunc},
   {"pwd",  pwdFunc},
   {"cd", cdFunc},
   {"ls", lsFunc},
   { NULL, NULL}		// terminator
};

//+
// Function:	doInternalCommand
//
// Purpose:	TODO: add description
//
// Parameters:
//	TODO: add parameter names and descriptions
//
// Returns	int
//		1 = args[0] is an internal command
//		0 = args[0] is not an internal command
//-

int doInternalCommand(char * args[], int nargs){
    int i = 0;
    
    
    while (commands[i].cmdName != NULL) {
        if (strcmp(args[0], commands[i].cmdName) == 0) {
            commands[i].cmdFunc(args, nargs);
		   return 1;
        }
        else{
            
        }
        i++;
    }
    fprintf(stderr, "Unable to find command\n");
    return 0;
}

///////////////////////////////
// comand Handling Functions //
///////////////////////////////

// TODO: a function for each command handling function
// goes here. Also make sure a comment block prefaces
// each of the command handling functions.

//+
// Function: exitFunc
//
// Purpose:	This function exits the program
//
// Parameters:
//	args	command and parameters, an array of pointers to strings
//	nargs	number of entries in the args array
//
// Returns	nothing (void)
//-
void exitFunc(char * args[], int nargs){
	exit(0);
}

//+
// Function: pwdFunc
//
// Purpose:	This function prints the current working directory
//
// Parameters:
//	args	command and parameters, an array of pointers to strings
//	nargs	number of entries in the args array
//
// Returns	nothing (void)
//-
void pwdFunc(char* args[], int nargs){
    char * cwd = getcwd(NULL,0);
    printf("Current directory: %s\n", cwd);
    free(cwd);
}


//+ TODO: Write func desc
// Function: exitFunc
//
// Purpose:	This function exits the program
//
// Parameters:
//	args	command and parameters, an array of pointers to strings
//	nargs	number of entries in the args array
//
// Returns	nothing (void)
//-
void cdFunc(char* args[], int nargs){
    // Get password
    struct passwd *pw = getpwuid(getuid());
    if(pw==NULL) {
        fprintf(stderr, "NULL password\n");
        return;
    }
    char* newDir = NULL;
    // if no args after cd then go to home dir
    if(nargs==1) newDir = pw->pw_dir;
    // if there is an arg after cd then try to change to that directory
    else newDir = args[1];
    // try to change directories print an error if it cannot
    if(chdir(newDir)==-1) fprintf(stderr, "Unable to change directory.\n");
}

//+ TODO: Write func desc
// Function: exitFunc
//
// Purpose:	This function exits the program
//
// Parameters:
//	args	command and parameters, an array of pointers to strings
//	nargs	number of entries in the args array
//
// Returns	nothing (void)
//-
int filter(const struct dirent *d){
    if(d->d_name[0]=='.')
        return 0;
    return 1;
}

//+ TODO: Write func desc
// Function: exitFunc
//
// Purpose:	This function exits the program
//
// Parameters:
//	args	command and parameters, an array of pointers to strings
//	nargs	number of entries in the args array
//
// Returns	nothing (void)
//-
void lsFunc(char* args[], int nargs){
    struct dirent **namelist;
    int (*filterOption)(const struct dirent *d) = NULL;
    if(nargs==1)
        filterOption = filter;
    else if(strcmp(args[1], "-a") == 0){
        filterOption = NULL;
    }
    else{
        fprintf(stderr, "Invalid filter option.\n");
        return;
    }

    int numEnts = scandir(".", &namelist, filterOption, NULL);
    int i;
    for(i = 0; i < numEnts; i++){
        printf("%s\n", namelist[i]->d_name);
    }
}