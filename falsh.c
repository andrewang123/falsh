// Andrew Ang
// Lab 3 - The Falcon Shell
// Professor Dingler
// The purpose of this lab is to create our own linux like shell, it is to be similiar to bash
// Built in commands include: exit, pwd, cd, setpath, help
// The shell will also allow redirection with > and other command use if in path
// More detailed descriptions is included in the readme and in the help function below

// LIBRARIES
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// FUNCTION PROTOTYPES
int redirection(char* buffer);
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
		// strcmp returns a 0 if it is the same string, less than 0 if str1 is < 2 and >0 if str1 > str2
		// 1st parameter is the first string and 2nd parameter is the string to be compared
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
		// strdup returns a pointer to a new string which is a duplicate of the parameter passed
		// it is essentially a strcpy and a malloc built into one
		// make a deep copy of the buffer, it is like strcpy but dynamically allocated memory
		char *bufferCpy  = strdup(buffer);

		// strlen is a function that take in a string and return the length of that string
		// strchr searches for the first occurance of the character in the string
		// strchr takes 2 params: 1st is the string to be scanned, 2nd is the char to be searched in the string
		// strchr returns null if the character is not found
		// check if the setpath is just setpath of there is other tokens with it
		if (strlen(bufferCpy) > 7 && strchr(bufferCpy, ' ') != NULL)
		{
			
			setPathStr = (char*)malloc(sizeof(char) * 255); // allocate dynamic memory	
			// Searches for the first occurance of the 2nd parameter	
			char* startOfSecond = strchr(bufferCpy, ' '); // first instance of a space
			size_t lengthOfFirst = startOfSecond - bufferCpy; // the index of the first occurance of space
			// strdup returns a pointer to a new string which is a duplicate of the parameter passed
			// it is essentially a strcpy and a malloc built into one
			// make a deep copy of the buffer, it is like strcpy but dynamically allocated memory
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
		} else if(contains(buffer, '>') == 0) // checks if there is a > in string
		{	
			int success = redirection(buffer);
		} else if (setPathStr != NULL && strchr(buffer, ' ') != NULL) 
		{	// if it is setpath with a space
	
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
			}
		}
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
	buffer[strlen(buffer) - 1] = '\0'; // change the user input so that it gets rid of the \n 
	// strdup returns NULL if insufficient memory was available otherwise it returns a pointer to the duplicated string
	// the parameter it takes is a string that you want to duplicate
	char* backupBuffer = strdup(buffer); //make a copy of the buffer and set it equal to backupBuffer
	char* allArgs[255]; // An array of strings to hold all of the user input
	int start = 1; 
	allArgs[0] = (char*)malloc(sizeof(char) * 255); // dynamically allocate memory
	// strtok takes in a string and a delimeter, the delimeter is what seperates the strings
	// It returns a token, the first param is the string to be broken up into tokens
	// The second parameter is what is seperating each token
	char *userTkn = strtok(backupBuffer, " "); //find the token before the space
	while(userTkn != NULL)
	{
		userTkn = strtok(NULL, " ");
		if(userTkn != NULL)
		{
			allArgs[start] = (char*)malloc(sizeof(char) * 255); // allocate memory on the heap
			// strcpy takes in two parameters the destination to be copied too and the thing that you want to copy
			strcpy(allArgs[start], userTkn); // copies the token into the array of strings
			start++;
		}
	}
	allArgs[start] = '\0'; // make the last index null
	int success = 1;
	// strdup: returns a pointer to a duplicate string in the heap
	// parameters: a string that is to be copied
	// getenv: gets the value of the environment variable
	// parameter: the path in which it should look ex HOME, PATH
	char* pathOfCommand = strdup(getenv("PATH")); // make a copy of your path
	//remove the \n and add the forward slash to the beginning of the command
	char* command = (char*)malloc(sizeof(char) * 255); // allocate on heap
	strcpy(command, "/"); // make the first index a slash
	char* anotherBuffer = (char*)malloc(sizeof(char) * 255); // dynamically allocate memory
	strcpy(anotherBuffer, buffer);
	// strtok takes in a string and a delimeter, the delimeter is what seperates the strings
	// It returns a token, the first param is the string to be broken up into tokens
	// The second parameter is what is seperating each token	
	char *firstToken = strtok(anotherBuffer, " ");	
	if(firstToken == NULL) // mean that it is just a single argument
	{
		// appends the string pointed to by the first parameter, it appends
		// whatever is in the second paramter
		strcat(command, buffer);
	} else { // there is going to be multiple arguments
		// appends the string pointed to by the first parameter, it appends
		// whatever is in the second paramter
		strcat(command, firstToken);
	}

	char * cpyOfCommandPath = strdup(pathOfCommand); // makes a deep copy of path of command so we can use it for last path 
	// strtok takes in a string and a delimeter, the delimeter is what seperates the strings
	// It returns a token, the first param is the string to be broken up into tokens
	// The second parameter is what is seperating each token		
	char* token = strtok(pathOfCommand, ":");
	if (token != NULL)
	{	
		while(token != NULL)
		{
			// allocate memory on the heap
			char* finalPathOfExec = (char *)malloc(sizeof(char) * 255);
			// construct the full path of the executed program ex) /bin/ls
			strcpy(finalPathOfExec, token); // copy the token to the final path
			strcat(finalPathOfExec, command);// append the command to the final path
			strcpy(allArgs[0], finalPathOfExec); // put the final path in the array of strings that will be used in the exec call
			// create a child process
			// returns a -1 if fork failed, 0 if on child process and 1 if on parent process
			// when a child process is created it basically means that it is a copy of its parents
			int rc = fork();
			if (rc < 0)
			{
				printf("An error occurred during fork.\n");
			} else if (rc == 0)
			{
				//printf("final path of exec : %s\n", finalPathOfExec);
				// execvp exeutes a file
				// the first parameter is the file location, the 2nd parameter is a list of strings that refers to the arguments, i.e when you start main you have argv[] which is 
				//the argument the user types in in the command line
				// Return values: -1 if an error occured (couldnt find the file)
				if(execvp(finalPathOfExec, allArgs) == -1) // cannot run
				{
					exit(0); // exit the child process
				} else 
				{
					//success = 0;	doesnt work need to pass as pid
				}
			} else // parent 
			{ 	// if wait is successful than it returns the process if od the child
				// if there is an error than -1 is returned
				wait(NULL); // parent must wait for child process to finish
			} 	
			token = strtok(NULL, ":"); // move onto the next directory (refer up for more details
		}
	} else {
			// the remaininding code is very similair to the ones above, refer above for more detailed comments
			char* finalPathOfExec = (char *)malloc(sizeof(char) * 255); // allocate on heap
			strcpy(finalPathOfExec, pathOfCommand); // copy pathOfCommand into finalPathOfExe
			strcat(finalPathOfExec, command); // appends command to final path
			strcpy(allArgs[0], finalPathOfExec); // copy finalpath of exec to the first index
			// create a child process
			// returns a -1 if fork failed, 0 if on child process and 1 if on parent process
			// when a child process is created it basically means that it is a copy of its parents	
			int rc = fork();
			if (rc < 0)
			{
				printf("An error occurred during fork.\n");
			} else if (rc == 0)
			{
				//printf("final path of exec : %s", finalPathOfExec);
				// execvp exeutes a file
				// the first parameter is the file location, the 2nd parameter is a list of strings that refers to the arguments, i.e when you start main you have argv[] which is 
				//the argument the user types in in the command line
				// Return values: -1 if an error occured (couldnt find the file)
				if(execv(finalPathOfExec, allArgs) == -1) // cannot run
				{
					exit(0); // exits the child process
				} else 
				{
					// Doesnt work because need to pass pid success = 0;	
				}
			} else // parent 
			{ 
				wait(NULL); // parent must wait for child process to finish
			}
			free(finalPathOfExec);

 		}

	// free all of the heap allocated variables
	free(backupBuffer);
	free(userTkn);
	free(pathOfCommand);
	free(command);
	free(anotherBuffer);
	free(cpyOfCommandPath);
	for(int i = 0; i < start; i++)
	{
		free(allArgs[i]);
	}
	//if(success == 1)
		//printf("command not found.\n");		
}

// Redirects output of a command to a file
// Parameters: buffer is the users input that they type in
// Returns: 0 if redirection success otherwise 1 if failed
int redirection(char* buffer)
{
	// strdup returns a pointer to a new string which is a duplicate of the parameter passed
	// it is essentially a strcpy and a malloc built into one
	// make a deep copy of the buffer, it is like strcpy but dynamically allocated memory
	char * copyBuffer = strdup(buffer);
	//remove the /n at the end
	copyBuffer[strlen(copyBuffer) - 1] = '\0';
	char* token = strtok(copyBuffer, " "); // should be the first command
	char* command = (char*)malloc(255 * sizeof(char));
	strcpy(command, token);
	if(strcmp(command, ">") == 0) // if the input is only a >
	{
		printf("Invalid redirection call\n");
		return 1;
	} else if(command[strlen(command) - 1] == '>') // if the character is a single token ending in >
	{
		printf("Invalid redirection call must have more arguments\n"); 
		return 1;
	}
	char* fileName = (char*) malloc(sizeof(char) * 255); // allocate on heap
	if(command != NULL) 
	{
		int counter = 1;
		while(token != NULL) // checks if the follows format of command > file
		{
			// strtok takes in a string and a delimeter, the delimeter is what seperates the strings
			// It returns a token, the first param is the string to be broken up into tokens
			// The second parameter is what is seperating each token
			token = strtok(NULL, " ");	
			if(counter > 3)
			{
				printf("Too many arguements\n");
				return 1;
			}	
			if(token == NULL && counter < 3)
			{
				printf("Too little arguements\n");
				return 1;
			}
			if(counter == 1)
			{
				//printf("THE TOKEN IS AT ONE: %s\n", token);
				if(strcmp(token, ">") != 0 )
				{
					printf("Failure, multiple arguments are not allowed\n");
					return 1;
				}
			} else if(counter == 2)
			{
				//printf("THE TOKEN IS AT TWO: %s\n", token);
				// strcpy takes in two parameters the destination to be copied too and the thing that you want to copy
				strcpy(fileName, token); // get the filename
			}
		counter++;	
		}
	}	
	//printf("The command is: %s\n", command);
	//printf("The filename is: %s\n", fileName);	
	// allocate on the heap
	char *fileNameError = (char*) malloc(sizeof(char) * 255);
	char *fileNameOut = (char*) malloc(sizeof(char) * 255);
	// create the filename and append the .err or .out on file
	// appends the string pointed to by the first parameter, it appends
	// whatever is in the second paramter
	strcpy(fileNameError, fileName);
	strcat(fileNameError, ".err");
	strcpy(fileNameOut, fileName);
	strcat(fileNameOut, ".out");
	
	//printf("FileNameOut = %s\n", fileNameOut);
	//printf("FileNameError = %s\n", fileNameError);
	
	// opens a file, if it successfully open/create file than it will return 0 otherwise -1
	// CREATE - creates the file, 0_TRUNC - overrides the file
	// RDWR - open forread and write to file, IRUSR - read permission, 
	// IWUSR - write permission
	int out = open(fileNameOut, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR, 0600);
	if (out == -1) 
	{ 
		perror("opening output file"); 
	}
	// opens a file, if it successfully open/create file than it will return 0 otherwise -1
	// CREATE - creates the file, 0_TRUNC - overrides the file
	// RDWR - open forread and write to file, IRUSR - read permission, 
	// IWUSR - write permission	
	int err = open(fileNameError, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR, 0600);
    	if (err == -1) { 
		perror("opening error file"); 
	}

	// duplicates an open file descriptor
	// takes in one parameter which is the file descriptor that is to be duplicated
	// it saves the duplicate into an integer, > 0 = success < 0 = failure
    	// fileno is a funciton that takes in a FILE* stream, examines the stream and returns a file descriptor
	// if succeed than reutrn integer value of file descriptor, otherwise -1
	// essentially it is to map a stream pointer to file descriptor
	int saveOut = dup(fileno(stdout));
    	int saveErr = dup(fileno(stderr));
	
	// duplicates a open file descriptor
	// returns negative value if fail, otherwise returns positive number
	// If 2nd param is already open, it is first closed. 
	// If 1st param equals 2nd param, then dup 2 returns 2nd param without closing it.
    	// fileno is a funciton that takes in a FILE* stream, examines the stream and returns a file descriptor
	// if succeed than reutrn integer value of file descriptor, otherwise -1
	// essentially it is to map a stream pointer to file descriptor
	if (dup2(out, fileno(stdout)) == -1) 
	{ 
		perror("cannot redirect stdout"); 
	}
    	// duplicates a open file descriptor
	// returns negative value if fail, otherwise returns positive number
	// If 2nd param is already open, it is first closed. 
	// If 1st param equals 2nd param, then dup 2 returns 2nd param without closing it.	
    	// fileno is a funciton that takes in a FILE* stream, examines the stream and returns a file descriptor
	// if succeed than reutrn integer value of file descriptor, otherwise -1
	// essentially it is to map a stream pointer to file descriptor		
	if (dup2(err, fileno(stderr)) == -1) 
	{ 
		perror("cannot redirect stderr"); 
	}
	if(strcmp(command, "help") == 0)
	{	
		printUserDescriptions();		
	} else if(strcmp(command, "pwd") == 0)
	{
		char* cwd = getPWD();
		printf("%s\n", cwd);
		free(cwd); // free up the dynamically created memory
	} else
	{
		// ALL THE CODE BELOW HERE UNTIL NEXT ALL CAPS WILL RUN THE COMMAND TO PRINT OUT
		char* pathOfCommand = strdup(getenv("PATH")); // make a copy of your path
		//printf("THE PATH OF COMMMAND IS: %s\n", pathOfCommand);
		char* pathToken = strtok(pathOfCommand, ":");
		if (pathToken != NULL)
		{	
			while(pathToken != NULL)
			{
				// allocate memory on the heap
				char* finalPathOfExec = (char *)malloc(sizeof(char) * 255);
				// construct the full path of the executed program ex) /bin/ls
				strcpy(finalPathOfExec, pathToken); // copy the token to the final path
				strcat(finalPathOfExec, "/");
				strcat(finalPathOfExec, command);// append the command to the final path
				// create a child process
				// returns a -1 if fork failed, 0 if on child process and 1 if on parent process
				// when a child process is created it basically means that it is a copy of its parents
				int rc = fork();
				if (rc < 0)
				{
					printf("An error occurred during fork.\n");
				} else if (rc == 0)
				{
					//printf("final path of exec : %s\n", finalPathOfExec);
					if(execl(finalPathOfExec, command, NULL) == -1) // cannot run
					{
						exit(0); // exit the child process
					} 
				} else // parent 
				{ 	// if wait is successful than it returns the process if od the child
					// if there is an error than -1 is returned
					wait(NULL); // parent must wait for child process to finish
				} 	
				pathToken = strtok(NULL, ":"); // move onto the next directory (refer up for more details
			}
		}
	}
	// END OF CODE THAT IS USED TO PRINT OUT THE COMMAND OUTPUT
    	// flushes out the output buffer in the stream, this means that it clears the output
	// parameter is a FILE* which refers to the file object in buffered stream
	// returns 0 success otherwise EOF is error 
	fflush(stdout); 
	// closes the file descriptor
	// returns -1 if interrupted, 0 if successful	
	close(out);
    	// flushes out the output buffer in the stream, this means that it clears the output
	// parameter is a FILE* which refers to the file object in buffered stream
	// returns 0 success otherwise EOF is error 
	fflush(stderr); 
	// closes the file descriptor
	// returns -1 if interrupted, 0 if successful		
	close(err);
	// duplicates a open file descriptor
	// returns negative value if fail, otherwise returns positive number
	// If 2nd param is already open, it is first closed. 
	// If 1st param equals 2nd param, then dup 2 returns 2nd param without closing it.
       	// fileno is a funciton that takes in a FILE* stream, examines the stream and returns a file descriptor
	// if succeed than reutrn integer value of file descriptor, otherwise -1
	// essentially it is to map a stream pointer to file descriptor		
	dup2(saveOut, fileno(stdout));
    	dup2(saveErr, fileno(stderr));
	// closes the file descriptor
	// returns -1 if interrupted, 0 if successful
    	close(saveOut);
    	close(saveErr);
    	return 0;
}
// Checks if a string contains a specific char
// Parameters: A string
// Returns: 0 if true, 1 if false
int contains(char* buffer, int ascii)
{
	int success = 1;
	// strlen return a number which is the length of the string
	// it takes in a parameter, which is a string
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
	buffer[strlen(buffer) - 1] = '\0'; // remove the \n from the buffer
	char* totalPath;
	totalPath = (char*)malloc(sizeof(char) * 255); // dynamically allocate memory
	// strdup duplicates the string, in this case it gets the current path an puts it into 
	// oldEnv (after calling getenv)
	// getenv searches for the environement string by the name and returns the value of the string
	char* oldEnv = strdup(getenv("PATH")); // make a copy of your path
	printf("Old Path: %s\n", oldEnv);
	// accounts for multiple directories being added
	// strtok takes in a string and a delimeter, the delimeter is what seperates the strings
	// It returns a token, the first param is the string to be broken up into tokens
	// The second parameter is what is seperating each token	
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
	// returns 0 is successfully changed
	// returns 1 otherwise (it failed)
	// Parameters: 1st environement name that is to be changed, 2nd is the string of the environment
	// values that is passed in, 3rd is whether or not to overwrite. 1 = yes 0 = no
	success = setenv("PATH", totalPath, 1);
	printf("New Path =%s\n", totalPath);
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
		// When HOME is specified as the parameter it gets the home directory
		char *homeDirectPath = getenv("HOME");
		// chkdir returns a 0 if it successfully changed paths and 1 if it failed to change
		// takes in the specific path that you want to change to
		// chdir changes the directory to the string that you passed in
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
	
	// sizeof gets the amount of bytes the parameter is inside. It is done at compile time
	// malloc allocate memory in the heap
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
	printf("Path should only contain /bin\n\n");
	printf("help - list of all built in commands with short descriptions, aka you are here now!\n");
	printf("Usage: help\n\n");
	printf("Redirection\n");
	printf("The falsh will be able to redirect files using the > \n");
	printf("When redirecting files: \n1. a missing argument\n2. multiple arguments\n");
	printf("Will NOT work, it will not run the command, but display a message\n");
	printf("A filename will be specified after the >\n");
	printf("redirection will work for both built in functions and other commands in path\n");
	printf("Usage: command > filename\n\n");
	printf("Other Commands\n");
	printf("Any non built in commands will work if it is in the users path.\n");
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
