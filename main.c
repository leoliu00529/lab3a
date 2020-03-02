#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ext2.h"

FILE * imfd;
FILE * outfd;

int total_block;
int total_inode;
int block_size;
int inode_size;
int block_per_group;
int inode_per_group;


void sysError(char * message) {
    fprintf(stderr, "%s\n", message);
    exit(1);
}

void superBlock() {
    struct ext2_super_block ext2superblock;
    
    // move current position to 1024
    fseek(imfd, 1024, SEEK_SET);

    // read 1024 bytes
    if (fread(&ext2superblock, 1024, 1024, imfd) < 0) {
        sysError("Failed to read superblock.");
    }

    total_block = ext2superblock.s_blocks_count;
    total_inode = ext2superblock.s_inodes_count;
    block_size = 1024 << ext2superblock.s_log_block_size;
    inode_size = ext2superblock.s_inode_size;
    block_per_group = ext2superblock.s_blocks_per_group;
    inode_per_group = ext2superblock.s_inodes_per_group;
    
    printf("SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", total_block, total_inode, block_size, inode_size, block_per_group, inode_per_group, ext2superblock.s_first_ino);

    return;
}


int main(int argc, const char * argv[]) {
//    if (argc != 2) {
//        sysError("Invalid argument(s).");
//    }
    
    imfd = fopen("EXT2_test.img", "r");
    if (imfd == NULL) {
        sysError("Failed to open the image file.");
    }
    
    outfd = fopen("output.csv", "w+");
    if (outfd == NULL) {
        sysError("Failed to create output csv file.");
    }
    
    superBlock();
    return 0;
}
