#include <errno.h>  //error
#include <fcntl.h>  //const def
#include <stdio.h>  //tradition
#include <string.h> //triviallizes strings
#include <unistd.h> //read and write commands

// error handler: call this whenever we know there's an error
int errorHandler(char *problem){
    if (errno = ENOENT){
        fprintf(stderr, "%s is not a valid file: %s\n", problem, strerror(errno));
    }
    else{
        fprintf(stderr, "%s could not be open for reading: %s\n", problem, strerror(errno));
    }
    return -1;
}

int main(int argc, char* argv[]){
    char buf[4096];     // buffer for the writing of characters
    int rBytes, wBytes; // counter for number of bytes read and wrote
    int iFile = 0;
    int oFile = 1; // input and output, set to their default values
    int bytesWritten = 0;
    int linesWritten = 1; //the first line is also a line. 
    int tmp = 0;
    char *iFileName = "<standard input>";
    char *oFileName = "";

    // step 1: Check for output file flag
    if (argc > 1 && strcmp(argv[1], "-o") == 0){
        // error: there's not enough arguments for this
        if (argc == 2){
            fprintf(stderr, "No output file specified.\n");
            return -1;
        }
        oFile = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
        oFileName = argv[2];
        // error: the file is not openable
        if (oFile == -1)
        {
            return errorHandler(oFileName);
        }
    } // at this point, we have an acceptable output file, whether stdout or file.
    // step 2: read all the relevant files
    for (int i = 0; i < argc; ++i){
        //if -o is active, skip the first 3 args (input file, -o, output file name). If we have more than one arg, skip the first arg. 
        if ((oFile != 1 && i < 3) || (i < 1 && argc > 1)){
            continue;
        }
        // we only do this section if there are inputs
        if (argc != 1){
            // case: "-" is an input
            if (strcmp(argv[i], "-") == 0){
                iFile = 0;
                iFileName = "<standard input>";
            }
            // case: we actually want "-" as a filename
            else if (strcmp(argv[i], "/-") == 0){
                iFile = open("-", O_RDONLY);
                iFileName = "-";
            }
            // case: actual normal files
            else{
                iFile = open(argv[i], O_RDONLY);
                iFileName = argv[i];
            }
            // error handler
            if (iFile == -1){
                return errorHandler(argv[i]);
            }
        }
        // read all the 4096 bytes we wanted
        do{
            tmp = read(iFile, buf, 4096);
            if (tmp == -1){
                return errorHandler(argv[i]);
            }
            // check for the number of lines inside
            for (int j = 0; j < tmp; ++j){
                if (buf[j] == '\n')
                {
                    ++linesWritten;
                }
            }
            // write to the the relevant files - only go up to the specified bytes
            tmp = write(oFile, buf, tmp);
            bytesWritten += tmp;
            
            if (tmp == -1){
                return errorHandler(oFileName);
            }
        }while (tmp != 0 && iFile != 0);

        // If we get here, we have done well
        fprintf(stderr, "\nFilename: %s\nBytes written: %d\nLines written:%d\n", iFileName, bytesWritten, linesWritten);
    }
    return 0;   // that should basically be it.
}