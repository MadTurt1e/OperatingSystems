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
}
//apparently this is supposed to be convention
int main(int argc, char* argv[]){
    char buffer[4092]; //buffer for the writing of characters
    int rBytes, wBytes; //counter for number of bytes read and wrote
    int iFile, oFile; //input and output file

    //step 1: Check for output file flag
    if (argc > 1 && argv[2] == "-o"){
        oFile = open(argv[3], O_RDWR | O_CREAT | O_TRUNC, 0666);
        //error case: we kill the program
        if (oFile == -1){
            return errorHandler(argv[3]);
        }
    }
    else{

    }
}