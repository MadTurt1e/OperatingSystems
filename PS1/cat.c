#include <errno.h> //error
#include <fcntl.h> //const def
#include <stdio.h> //tradition
#include <string.h> //triviallizes strings


//apparently this is supposed to be convention
int main(int argc, char* argv[]){
    char buffer[1024]; //buffer for the writing of characters
    int rBytes, wBytes; //counter for number of bytes read and wrote
    int iFile, oFile; //input and output file

    //step 1: check if inputs are valid - the only invalid inputs are if there are no inputs, or if no output file is specified
    if (argc == 0 || (argc == 1 && strcmp(argv[0], "-o"))){
        printf("ERROR - NOT ENOUGH INPUTS SPECIFIED");
        return -1;
    }
}