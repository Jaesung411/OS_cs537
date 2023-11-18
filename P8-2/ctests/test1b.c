//Tests getnextpid after child process is killed.

#include "types.h"
#include "user.h"
#include "param.h"

int
main(int argc, char *argv[])
{
  int nextpid1 = getnextpid();
  
  // fork a child process.
  int childpid = fork();
  if(childpid < 0) {
    printf(1, "TEST FAILED: fork failed");
    exit();
  }

  if(childpid > 0) {
    wait();

    // check if nextpid will be same as previous.
    int nextpid2 = getnextpid();
    if (nextpid2 == nextpid1 + 1)
      printf(1, "TEST PASSED");
    else
      printf(1, "TEST FAILED: nextpid returned = %d, expected nextpid = %d", nextpid2, nextpid1 + 1);
  } else {
    sleep(3);
  }

  exit();
}
