#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BSIZE (1024)
#define BOOT_BLOCK_NO 0
#define SUPER_BLOCK_NO 1

struct superblock {
  uint magic;        // Must be FSMAGIC
  uint size;         // Size of file system image (blocks)
  uint nblocks;      // Number of data blocks
  uint ninodes;      // Number of inodes.
  uint nlog;         // Number of log blocks
  uint logstart;     // Block number of first log block
  uint inodestart;   // Block number of first inode block
  uint bmapstart;    // Block number of first free map block
};

int
main(int argc, char *argv[])
{
    int fd;
    int res;
    int i;
    int j;
    char c;

    if (argc < 2)
    {
	    printf("Usage: fsmap path-to-fs.img\n");
        exit(0);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr,"Cannot open %s\n",argv[1]);
	    exit(-1);
    }

    // move file read position to BSIZE*blockNo from the start of the file
    lseek(fd, BSIZE * SUPER_BLOCK_NO, SEEK_SET);

    //read the super block
    struct superblock super;
    res = read(fd, &super , sizeof(struct superblock));
    if (res == -1) {
        fprintf(stderr, "Could not read in super block!");
    }

    // printf("%u\n", super.size);
    // printf("%u\n", super.nblocks);
    // printf("%u\n", super.ninodes);
    // printf("%u\n", super.nlog);
    // printf("%u\n", super.logstart);
    // printf("%u\n", super.inodestart);
    // printf("%u\n\n", super.bmapstart);

    // Print summary

    uint datastart = super.size - super.nblocks;
    printf("BS");

    for (i = 0; i < super.nlog; i++) putchar('L');
    for (i = 0; i < super.ninodes; i++) putchar('I');
    for (i = super.bmapstart; i < datastart; i++) putchar('T');
    for (i = 0; i < super.nblocks; i++) putchar('D');

    exit(0);
    //never gets here
}
