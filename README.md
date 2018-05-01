Hell aka falsh
Parameters in [] are optional, parameters in <> are required and the | (pipe) means "or". 

What is falsh?  
falsh is a shell written in c. A shell interpreter is a program that continually loops, accepting user input. The shell interprets the user's input to execute programs, run built in functions and many more things. This falsh shell is going to mimic the bash shell.

Functions
* help  
Prints the help message
* exit  
Exits the session
* pwd  
prints the current working directory
* cd [dir]  
changes the directory to the home directory if there are no arguements, otherwise changes it to the directory if it is a valid directory 
* setpath  
sets the path, user has to provide at least one arguement(directory). The path is where falsh will look for executable programs to run.  

Redirection  
* The falsh shell will allow redirection between programs


\* Note: Any errors will not end the session, instead it will generate a "command not found" error.


