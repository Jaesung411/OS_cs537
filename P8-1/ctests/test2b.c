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
	char state[16];
	int ret = getprocstate(childpid, state, 16);
	
	if (ret == 0 && strcmp(state, "runble") == 0) {
		printf(1, "TEST PASSED");
	} else {
		printf(1, "TEST FAILED: expected proc state runble , got %s", state);	
	}
	wait();
  } else {
	  sleep(10);
  }

 exit();
} 
