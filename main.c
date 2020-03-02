#include <stdio.h>
#include <stdlib.h>
#include "ext2.h"

FILE * imfd;
FILE * outfd;


void sysError(char * message) {
    fprintf(stderr, "%s\n", message);
    exit(1);
}

void superBlock() {
    struct ext2_super_block ext2superblock;
    
    // move current position to 1024
    fseek(imfd, 1024, SEEK_SET);
    
    // read 1024 bytes
    if (fread(&ext2superblock, 1024, 1, imfd) != 1) {
        sysError("Failed to read superblock.");
    }
    
    
    
    return;
}


int main(int argc, const char * argv[]) {
    if (argc != 2) {
        sysError("Invalid argument(s).");
    }
    
    if ((imfd = fopen(argv[1], "r")) == NULL) {
        sysError("Failed to open the image file.");
    }
    
    if ((outfd = fopen("output.csv", "w+")) == NULL) {
        sysError("Failed to create output csv file.");
    }
    
    
    return 0;
}
