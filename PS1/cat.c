#include <errno.h> //error
#include <fcntl.h> //const def
#include <stdio.h> //tradition
#include <string.h> //triviallizes strings

//error handler: call this whenever we know there's an error
int errorHandler(char* problem){
    if (errno = ENOENT){
        fprintf(stderr, "%s is not a valid file: %s\n", problem,strerror(errno));
    }
    else{
        fprintf(stderr, "%s could not be open for reading: %s\n", strerror(errno));
    }
    return -1;
}
//apparently this is supposed to be convention
int main(int argc, char* argv[]){
    char buffer[4092]; //buffer for the writing of characters
    int rBytes, wBytes; //counter for number of bytes read and wrote
    int iFile = 0;
    int oFile = 1; //input and output, set to their default values

    //step 1: Check for output file flag
    if (argc > 1)
        if (argv[1] == "-o"){
            //error: there's not enough arguments for this
            if (argc == 2){
                fprintf(stderr, "No output file specified.\n");
                return -1;
            }
            oFile = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0666);
            //error: the file is not openable
            if (oFile == -1){
                return errorHandler(argv[3]);
            }
        }   //at this point, we have an acceptable output file, whether stdout or file. 

        //step 2: read all the relevant files
        for (int i = 2; i < argc; ++i){
            //case: "-" is an input
            if (argv[i] == "-"){
                iFile = 0;
            }
            //case: we actually want "-" as a filename
            else if (argv[i] == "--"){
                iFile = open("-", O_RDONLY);    
            }
            //case: actual normal files
            else{
                iFile = open(argv[i], O_RDONLY);
            }
            //error handler
            if (iFile == -1){
                return errorHandler(argv[i]);
            }

            //step 3: write to our output
            if (writeToOutput(iFile) == -1){
                return errorHandler(argv[i]);
            }
            //condition when we do good
            else{
                fprintf(stder, "")
            }
        }
    }
}