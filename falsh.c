// Andrew Ang
// Lab 3 - The Falcon Shell
// Professor Dingler
// The purpose of this lab is to create our own shell
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void reprompt(char *buffer, size_t size);
void printUserDescriptions();
int main(int argc, char * argv[])
{
	char *buffer; // stores the user input
	int check; // flag to check for EOF
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
		if (strcmp(buffer, "exit\n") == 0) // exit the program
		{
			exit(0);
		} else if (strcmp(buffer, "help\n") == 0)
		{
			printUserDescriptions();
		} else if (strcmp(buffer, "pwd\n") == 0)
		{
			char cwd[256]; // create an array to hold the current working directory
			// first parameter is the name of the buffer that will be used to store the current working directory
			// second parameter is the number of characters in the buffer area
			// the getcwd fuction returns a pointer to the buffer if successful otherwise it returns null if fail
			// getcwd determines the path of the working directory and stores it in an array aka the buffer
	  		if (getcwd(cwd, sizeof(cwd)) == NULL)
     				perror("getcwd() error");
    			else
      				printf("Current directory: %s\n", cwd);
  			
		} else
		{
			printf("command not found.\n");
		}
	}
	free(buffer); // deallocate memory
	return 0;
}

// Reprompts the user each time
// Parameters: N/A
// Returns: N/A
void reprompt(char *buffer, size_t size) 
{
	printf("falsh > ");
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
	printf("####################################\n");
	printf("        FAlcon baSH aka falsh       \n");

	printf("LIST OF COMMANDS:\n");
	printf("exit - exits the program. \n Usage: exit\n\n");
	printf("pwd - print the current working directory.\n Usage: pwd\n\n");
	printf("cd - changes the directory.\n Usage: cd [dir] \n");
	printf("If no arguments are specified than changes to home directory.\n");
	printf("If a directory is provided than it will change to that directory.\n");
	printf("Usage: cd [dir]\n");

	printf("####################################\n");

}
