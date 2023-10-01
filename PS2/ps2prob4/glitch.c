#include <stdio.h>

int main() {
    while(1) {
        FILE *file = fopen("catch-me-if-you-can", "w");
        fclose(file);
        
        remove("catch-me-if-you-can");
    }
    return 0;
}
