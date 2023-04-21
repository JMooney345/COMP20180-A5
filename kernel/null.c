//
// Assignment 5 Task 1
// Anything inputted into this device driver is deleted
// Cannot read from this, returns -1

#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

//
// user write()s to null, writes nothing
//
int
nullwrite(int user_src, uint64 src, int n)
{
  return 0;
}

//
// Cannot read this file
//
int
nullread(int user_dst, uint64 dst, int n)
{
  return -1;
}


int
nullioctl(int user_dst, uint64 dst, int request)
{
  return -1;
}

void
nullinit(void)
{

  // connect read and write system calls
  devsw[NULL_DRIVER].read = nullread;
  devsw[NULL_DRIVER].write = nullwrite;
  devsw[NULL_DRIVER].ioctl = nullioctl;

}
