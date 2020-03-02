#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
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

int inode_table;

void sysError(char * message) {
    fprintf(stderr, "%s\n", message);
    exit(2);
}

void superBlock() {
    struct ext2_super_block ext2superblock;
    
    // move current position to 1024
    fseek(imfd, 1024, SEEK_SET);

    // read 1024 bytes
    fread(&ext2superblock, 1024, 1, imfd);
    if(ferror(imfd)){
        sysError("Failed to read superblock.");
    }

    total_block = ext2superblock.s_blocks_count;
    total_inode = ext2superblock.s_inodes_count;
    block_size = 1024 << ext2superblock.s_log_block_size;
    inode_size = ext2superblock.s_inode_size;
    block_per_group = ext2superblock.s_blocks_per_group;
    inode_per_group = ext2superblock.s_inodes_per_group;
    first_block = ext2superblock.s_first_ino;

    fprintf(outfd, "SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", total_block, total_inode, block_size, inode_size, block_per_group, inode_per_group, ext2superblock.s_first_ino);

    return;
}

void group_desc()
{
  struct ext2_group_desc group;
  fread(&group, 32, 1, imfd);
  if(ferror(imfd)){
    sysError("Failed to read group descriptor.");
  }

  num_of_free_block = group.bg_free_blocks_count;
  num_of_free_inode = group.bg_free_inodes_count;
  block_bitmap = group.bg_block_bitmap;
  inode_bitmap = group.bg_inode_bitmap;
  inode_table = group.bg_inode_table;

  fprintf(outfd, "GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n", group_number, total_block, total_inode, num_of_free_block, num_of_free_inode, block_bitmap, inode_bitmap, first_block);

  return;
}

void bFree()
{
  fseek(imfd, block_bitmap * block_size, SEEK_SET);
  char* bitmap = (char*)malloc(sizeof(char) * block_size);
  if(bitmap == NULL)
    {
      sysError("Failed to allocate memory.");
    }
  fread(bitmap, 1, block_size, imfd);
  if(ferror(imfd)){
    sysError("Failed to read block bitmap.");
  }
  int i, j;
  char temp;
  int count = 1;
  for(i = 0; i < block_size; i++)
    {
      temp = bitmap[i] & 0xFF;
      for(j = 0; j < 8; j++)
	{
	  if(!((temp >> j) & 0x1))
	    {
	      fprintf(outfd, "BFREE, %d\n", count);
	    }
	  count++;
	}
    }
  return;
}

void iFree()
{
  fseek(imfd, inode_bitmap * block_size, SEEK_SET);
  char* bitmap = (char*)malloc(sizeof(char) * block_size);
  if(bitmap == NULL)
    {
      sysError("Failed to allocate memory.");
    }
  fread(bitmap, 1, block_size, imfd);
  if(ferror(imfd)){
    sysError("Failed to read inode bitmap.");
  }
  int i, j;
  char temp;
  int count = 1;
  for(i = 0; i < block_size; i++)
    {
      temp = bitmap[i] & 0xFF;
      for(j = 0; j < 8; j++)
        {
          if(!((temp >> j) & 0x1))
            {
              fprintf(outfd, "IFREE, %d\n", count);
            }
          count++;
        }
    }
  return;
}


int main(int argc, const char * argv[]) {
    if (argc != 2) {
      fprintf(stderr,"Invalid argument(s).");
      exit(1);
    }
    
    imfd = fopen(argv[1], "r");
    if (imfd == NULL) {
        sysError("Failed to open the image file.");
    }
    
    outfd = fopen("output.csv", "w+");
    //
    outfd = stdout;
    //
    if (outfd == NULL) {
        sysError("Failed to create output csv file.");
    }
    
    superBlock();
    group_desc();
    bFree();
    iFree();

    fclose(imfd);
    fclose(outfd);
    
    exit(0);
}
