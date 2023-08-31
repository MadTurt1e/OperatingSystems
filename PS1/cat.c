#include <errno.h> //error
#include <fcntl.h> //const def
#include <stdio.h> //tradition


int main(){

    //I/O recieved from the keyboard
    char* input;
    char* output;

    //part 1: open the file we'd like to read / write
    int inFile = open(input, O_RDONLY);

    printf(inFile);
    return 0;
}