// Get proc state for zombie process.
#include "types.h"
#include "user.h"

int
main(void)
{
  int nextpid = getnextpid();
  printf(1, "next pid=%d\n", nextpid);

  int childpid = fork();

  if(childpid > 0) {
	sleep(10);
	  char state[16];
	int ret = getprocstate(childpid, state, 16);

	if(ret == 0 && strcmp(state, "zombie") == 0) {
		printf(1, "TEST PASSED");
	} else {
		printf(1, "TEST FAILED: expected zombie state, got %s", state);
	}

	wait();
  } else {
  }

 exit();
} 
