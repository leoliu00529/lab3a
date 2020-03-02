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

int group_number = 0;
int num_of_free_block;
int num_of_free_inode;
int block_bitmap;
int inode_bitmap;
int first_block;

void sysError(char * message) {
    fprintf(stderr, "%s\n", message);
    exit(1);
}

void superBlock() {
    struct ext2_super_block ext2superblock;
    
    // move current position to 1024
    fseek(imfd, 1024, SEEK_SET);

    // read 1024 bytes
    if (fread(&ext2superblock, 1024, 1, imfd) < 0) {
        sysError("Failed to read superblock.");
    }

    total_block = ext2superblock.s_blocks_count;
    total_inode = ext2superblock.s_inodes_count;
    block_size = 1024 << ext2superblock.s_log_block_size;
    inode_size = ext2superblock.s_inode_size;
    block_per_group = ext2superblock.s_blocks_per_group;
    inode_per_group = ext2superblock.s_inodes_per_group;
    first_block = ext2superblock.s_first_ino;

    printf("SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", total_block, total_inode, block_size, inode_size, block_per_group, inode_per_group, ext2superblock.s_first_ino);

    return;
}

void group_desc()
{
  struct ext2_group_desc group;
  if(fread(&group, 1024, 1, imfd) < 0){
    sysError("Failed to read group descriptor.");
  }

  num_of_free_block = group.bg_free_blocks_count;
  num_of_free_inode = group.bg_free_inodes_count;
  block_bitmap = group.bg_block_bitmap;
  inode_bitmap = group.bg_inode_bitmap;
  
  printf("GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n", group_number, total_block, total_inode, num_of_free_block, num_of_free_inode, block_bitmap, inode_bitmap, first_block);

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
    group_desc();
    fclose(imfd);
    fclose(outfd);
    
    exit(0);
}
