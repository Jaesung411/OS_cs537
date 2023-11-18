// Get proc state for zombie process.
#include "types.h"
#include "user.h"

int
main(void)
{
  int nextpid = getnextpid();
  printf(1, "next pid=%d\n", nextpid);
  char state[16];
  int ret = getprocstate(nextpid, state, 2);
	
if (ret == -1) {
	printf(1, "TEST PASSED");
} else {
		printf(1, "TEST FAILED: proc state should fail");	
	}
} 
