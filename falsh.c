// Andrew Ang
// Lab 3 - The Falcon Shell
// Professor Dingler
// The purpose of this lab is to create our own shell

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int redirection(char* command, char* filename);
void runOtherCommands(char* command);
int contains(char* buffer, int ascii);
void printIntro();
int setPath(char* buffer);
int changeDir(char* buffer);
char* getPWD();
void reprompt(char *buffer, size_t size);
void printUserDescriptions();
int main(int argc, char * argv[])
{
	// check if they are doing falsh -h
	if (argc == 2)
	{ 
		if (strcmp(argv[1], "-h") == 0) 
		{
			printIntro();
			printUserDescriptions();
			exit(0); // exits the program
		} else 
		{
			printf("Not a valid entrance into falsh shell");
			exit(0); // exits the program			
		}
	} else if (argc > 2)
	{
		printf("command not found.\n");
		exit(0); // exits the program
	}

	char *buffer; // stores the user input
	size_t size = 255; // size of input
	// allocate memory, we have to cast it as a char pointer because if we 
	// dont than the compiler has warnings. malloc is a function that creates 
	// memory and parameter it takes is how big the space is to be		
	buffer = (char *) malloc(size * sizeof(char)); 	
	printIntro();
	char* userPath; // variable holding users path
	while(1) // Invoking falsh
	{
		
		reprompt(buffer, size);
		char *setPathStr;
		// make a deep copy of the buffer, it is like strcpy but dynamically allocated memory
		char *bufferCpy  = strdup(buffer);
		if (strlen(bufferCpy) > 7 && strchr(bufferCpy, ' ') != NULL)
		{
			setPathStr = (char*)malloc(sizeof(char) * 255); // allocate dynamic memory	
			// Searches for the first occurance of the 2nd parameter	
			char* startOfSecond = strchr(bufferCpy, ' '); // first instance of a space
			size_t lengthOfFirst = startOfSecond - bufferCpy; // the index of the first occurance of space
			setPathStr = strndup(bufferCpy, lengthOfFirst);
		}
		if (strcmp(buffer, "exit\n") == 0) // exit the program
		{
			printf("Exiting falsh. . .\n");
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
		} else if(contains(buffer, '>') == 0) // go into redirection
		{	
			int success = redirection("", "");
			//int num = contains(buffer, '>');
			
			//printf("This contains a redirection: %s\n", buffer);
		//	close(1); // close the output
		
		} else if (setPathStr != NULL && strchr(buffer, ' ') != NULL) // if it is setpath with a space
		{
			if (strcmp(setPathStr, "setpath") == 0)	
			{
				int success = setPath(buffer);
			} else 
			{
				runOtherCommands(buffer); // run for more than one "argc"

			}
		} else 
		{
			if (strcmp(buffer, "setpath\n") == 0) // if the user just uses setpath
				printf("Setpath must be accompanied by at least one directory.\n");	
			else 
			{
				runOtherCommands(buffer); // run for one "argc"
				//printf("command not found.\n");		
			}
		}
//		free(setPathStr); // deallocate memory
		free(bufferCpy); // deallocate memory
	}
	free(buffer); // deallocate memory
	return 0;
}

// Runs the other commands if it is in the path
// Parameters: buffer, which is the command the user types in
// Returns: 0 if successfully ran, otherwise returns false
void runOtherCommands(char* buffer) 
{

	char* allArgs[255]; //{finalPathOfExec, NULL};	
	int start = 1;
	allArgs[0] = (char*)malloc(sizeof(char) * 255);
	char *userTkn = strtok(buffer, " ");
	while(userTkn != NULL)
	{
//		printf("%s", userTkn);
		userTkn = strtok(NULL, " ");
		if(userTkn != NULL)
		{
			allArgs[start] = (char*)malloc(sizeof(char) * 255);
			strcpy(allArgs[start], userTkn);
			start++;
		}
	}
	allArgs[start] = '\0'; // make the last index null
	



	int success = 1;
	char* pathOfCommand = strdup(getenv("PATH")); // make a copy of your path
//	pathOfCommand[strlen(pathOfCommand) - 1] = '\0';	
	
	//remove the \n and add the forward slash to the beginning of the command
	char* command = (char*)malloc(sizeof(char) * 255);
	strcpy(command, "/");
	strcat(command, buffer);
//	command[strlen(command) - 1] = '\0';
	command[strlen(command)] = '\0';
	
	char * cpyOfCommandPath = strdup(pathOfCommand); // makes a deep copy of path of command so we can use it for last path 
	// every token in the path
	// parse through the string 
	char* token = strtok(pathOfCommand, ":");
	while(token != NULL)
	{
		char* finalPathOfExec = (char *)malloc(sizeof(char) * 255);
		strcpy(finalPathOfExec, token);
		strcat(finalPathOfExec, command);
		
		finalPathOfExec[strlen(finalPathOfExec) - 1] = '\0';
		strcpy(allArgs[0], finalPathOfExec);
		//printf("%s\n", finalPathOfExec);	
		int rc = fork();
		if (rc < 0)
		{
			printf("An error occurred during fork.\n");
		} else if (rc == 0)
		{
		//	char* test[] = {"/bin/ls", NULL};	
//			printf("final path of exec : %s", finalPathOfExec);
			if(execvp(finalPathOfExec, allArgs) == -1) // cannot run
			{
				exit(0);
			} else 
			{
				success = 0;	
			}
		} else // parent 
		{ 
			wait(NULL); // parent must wait for child process to finish
		} 	
		token = strtok(NULL, ":"); // move onto the next directory
	}
/*	// YOU ARE HERE
	// check the last one
	char * lastOccurance = 	strrchr( cpyOfCommandPath,':');
	printf("%s",lastOccurance);
	int rc = fork();
	if (rc < 0)
	{
		printf("An error occurred during fork.\n");
	} else if (rc == 0)
	{
		char * test[] = {"./ls", NULL};	
		//printf("final path of exec : %s", finalPathOfExec);
		if(execv(finalPathOfExec, test) == -1) // cannot run
		{
			exit(0);
		} else 
		{
			success = 0;	
		}
	} else // parent 
	{ 
		wait(NULL); // parent must wait for child process to finish
	}
 	*/	 
	if(success == 1)
		printf("command not found.\n");		
	
	for(int i = 0; i < start; i++)
	{
//			printf("%s\n", allArgs[i]);
	}

}

// Redirects to a file
// Parameters:
// Returns:
int redirection(char* command, char* filename)
{
/*	
//	if (filename == NULL)
//		return 1;
	int pid, status;
	int newFd; // new file descriptor
	char *cmd[] = { "/bin/ls", NULL };
	int other;
	char * outFile = "testing.out";
	if (newFd = open(outFile, O_CREAT|O_TRUNC|O_WRONLY, 0644) < 0)
	{
		printf("error");
	}
			
	
	int rc = fork();
	if (rc < 0)
	{
		printf("Error Creating Process");		
	} else if (rc == 0) // child
	{
		execvp(cmd[0], cmd);	
	
		dup2(newFd, STDOUT_FILENO); // newFd becomes standard output
		close(newFd);
	} else if (rc > 0) // parent
	{
		while(wait(&status) != -1); // pick up zombie children
	}

	return 0;
*/
}
// Checks if a string contains a specific char
// Parameters: A string
// Returns: 0 if true, 1 if false
int contains(char* buffer, int ascii)
{
	int success = 1;
	for(int i = 0; i < strlen(buffer); i++)
	{
		if(buffer[i] == ascii)
		{
			success = 0;
			return success;
		}
	}

	return success;
}

// Sets the path of the executables
// Parameters: the entire line of user input
// Returns: a string containing all of the setpaths seperated by colons
int setPath(char* buffer)
{
	int success = 1;
	buffer[strlen(buffer) - 1] = '\0'; 
	char* totalPath;
	totalPath = (char*)malloc(sizeof(char) * 255);
	// strdup duplicates the string, in this case it gets the current path an puts it into 
	// oldEnv (after calling getenv)
	// getenv searches for the environement string by the name and returns the value of the string
	char* oldEnv = strdup(getenv("PATH")); // make a copy of your path
	printf("Current Path: %s\n", oldEnv);
	// accounts for multiple directories being added
	char* token = strtok(buffer, " "); // the setpath string, we should ignore that
	token = strtok(NULL, " "); // get the first token
	strcpy(totalPath, token); // add that token to the totalPath
	while(token) { // add it to the string
		token = strtok(NULL, " "); // keep iterating through to the next token
		if(token) // if there is still a token to be found
		{
			strcat(totalPath, ":"); // add a colon to seperate directories
			strcat(totalPath, token); // add the next token to the totalPath
		} else
			break; // there is no more tokens 
	}
	success = setenv("PATH", totalPath, 1);
	printf("Final Path =%s\n", totalPath);
	free(oldEnv); // free up memory created by strdup
	
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
