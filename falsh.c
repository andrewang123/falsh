// Andrew Ang
// Lab 3 - The Falcon Shell
// Professor Dingler
// The purpose of this lab is to create our own shell
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
void printIntro();
int setPath(char* buffer);
int changeDir(char* buffer);
char* getPWD();
void reprompt(char *buffer, size_t size);
void printUserDescriptions();
int main(int argc, char * argv[])
{
	printIntro();
	//char buffer[255]; // stores the user input
	char *buffer; // stores the user input
	//int check; // flag to check for EOF	
	size_t size = 255; // size of input
	// allocate memory, we have to cast it as a char pointer because if we 
	// dont than the compiler has warnings. malloc is a function that creates 
	// memory and parameter it takes is how big the space is to be		
	buffer = (char *) malloc(size * sizeof(char)); 	
	// check if they are doing falsh -h
	if (argc == 2)
	{ 
		if (strcmp(argv[1], "-h") == 0) 
		{
			printUserDescriptions();
			free(buffer); // deallocate memory	
			exit(0); // exits the program
		}
	} else if (argc > 2)
	{
		printf("command not found.\n");
		free(buffer); // deallocate memory
		exit(0); // exits the program
	}		
	while(1) // Invoking falsh
	{
		
		reprompt(buffer, size);
		char *setPathStr;
		if (strlen(buffer) > 7 && strchr(buffer, ' ') != NULL)
		{
			setPathStr = (char*)malloc(sizeof(char) * 255); // allocate dynamic memory	
			// Searches for the first occurance of the 2nd parameter	
			char* startOfSecond = strchr(buffer, ' '); // first instance of a space
			size_t lengthOfFirst = startOfSecond - buffer; // the index of the first occurance of space
			strncpy(setPathStr, buffer, lengthOfFirst); // should just be setpath
		}
		if (strcmp(buffer, "exit\n") == 0) // exit the program
		{
			exit(0);
		} else if (strcmp(buffer, "help\n") == 0)
		{
			printUserDescriptions();
		} else if (strcmp(buffer, "pwd\n") == 0)
		{
			char* cwd = getPWD();
			printf("%s\n", cwd);
			free(cwd); // free up the dynamically created memory
		} else if(buffer[0] == 'c' && buffer[1] == 'd') // change directory
		{	
			int success = changeDir(buffer);	
			if(success != 0) // if the given directory fails
			{
				printf("No such file or directory\n");
			} 
		} else if (setPathStr != NULL && strchr(buffer, ' ') != NULL) // if it is setpath with a space
		{
			if (strcmp(setPathStr, "setpath") == 0)	
			{
				int success = setPath(buffer);		
			}
		} else 
		{
			if (strcmp(buffer, "setpath\n") == 0) // if the user just uses setpath
				printf("setpath must be accompanied by at least one directory\n");	
			else
				printf("command not found.\n");
		}
		free(setPathStr); // deallocate memory

	}
	free(buffer); // deallocate memory
	return 0;
}

// Sets the path of the executables
// Parameters: the entire line of user input
// Returns: 0 if succeed, 1 if fail to set path
int setPath(char* buffer)
{
	int success = 1;
	char totalPath[255];
	char* oldEnv = strdup(getenv("PATH")); // make a copy of your path
	printf("Current Path: %s\n", oldEnv);
	
	//string seperated by :
	char* token = strtok(buffer, " "); 
	token = strtok(NULL, " ");
	strcpy(totalPath, token);
	while(token) { // add it to the string
		token = strtok(NULL, " ");			
		if(token)
		{
			strcat(totalPath, ":");
			strcat(totalPath, token);
		} else
			break;
	}
	
	success = setenv("PATH", totalPath, 1);
	printf("Final Path =%s\n", totalPath);
	
	// system("./yo/hello")
	if (success == 0) 
	{
		//printf("it worked");
	} else 
	{
		//printf("it failed");
	}
	
	return success;
}
// Changes the path of the user
// Parameters: buffer is the line the user typed in
// Returns 0 if succeed, 1 if failed
int changeDir(char* buffer)
{
	int ret = 1;
	if (strcmp(buffer, "cd\n") == 0)
	{
		// getenv returns the value of the path of the designated location
		// parameter that is passed in is the name of the location
		char *homeDirectPath = getenv("HOME");
		// chkdir returns a 0 if it successfully changed paths and 1 if it failed to change
		// takes in the specific path that you want to change to
		ret = chdir(homeDirectPath);
	} else 
	{
		char* finalPath;
		finalPath = malloc(sizeof(char) * 255); // dynamically allocate space
		// str copy copies the 2nd string into the first string
		strcpy(finalPath, buffer + 3);
		// strcspn takes in two strings and returns the number of character in the first parameter
		// that is not in the second string (parameter)
		// gets rid of the \n and replaces it with the null byte character
		finalPath[strcspn(finalPath, "\n")] = '\0';		
		// chkdir returns a 0 if it successfully changed paths and 1 if it failed to change
		// takes in the specific path that you want to change to
		ret = chdir(finalPath);
		free(finalPath); // free the dynamically allocated memory of finalpath
	}
	return ret;
}

// Prints the current directory
// Parameters: N/A
// Returns: the current directory the user is in
char* getPWD() 
{
	char* cwd; // create an array to hold the current working directory
	cwd = (char *)malloc(255 * sizeof(char));
	// first parameter is the name of the buffer that will be used to store the current working directory
	// second parameter is the number of characters in the buffer area
	// the getcwd fuction returns a pointer to the buffer if successful otherwise it returns null if fail
	// getcwd determines the path of the working directory and stores it in an array aka the buffer
	if (getcwd(cwd, sizeof(char) * 255) == NULL)
     		perror("getcwd() error");
	return cwd;
}

// Reprompts the user each time
// Parameters: N/A
// Returns: N/A
void reprompt(char *buffer, size_t size) 
{
	char* currentDirectory = getPWD(); // gets the current directory
	printf("falsh: %s > ", currentDirectory);
	// buffer is the address of the first characer position 
	// size is the address of the cariable that holds the size of the input buffer
	// stdin is the type of the FILE * (usually stdin or the file)	
	getline(&buffer, &size, stdin); // get entire line
}

// Lists all of the commands with short user-friendly descriptions
// Parameters: N/A
// Returns: N/A
void printUserDescriptions() 
{
	printf("########################################################################\n");
	printf("	             FAlcon baSH aka falsh       \n");
	printf("LIST OF COMMANDS:\n");
	printf("exit - exits the program. \n Usage: exit\n\n");
	printf("pwd - print the current working directory.\n Usage: pwd\n\n");
	printf("cd - changes the directory.\n Usage: cd [dir] \n");
	printf("If no arguments are specified than changes to home directory.\n");
	printf("If a directory is provided than it will change to that directory.\n");
	printf("Usage: cd [dir]\n");
	printf("setpath - overwrites the path with whatever arguements the user enters.\n");
	printf("Path should only contain /bin\n");
	printf("#######################################################################\n");

}

// Prints the introduction to let the user know that they have entered falsh
// Parameters: N/A
// Returns: N/A
void printIntro()
{
	printf("****************************************************\n");
	printf("	        WELCOME TO FALSH\n");
	printf("****************************************************\n");
	
}
