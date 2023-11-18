// Get proc state for zombie process.
#include "types.h"
#include "user.h"

int
main(void)
{
 	char state[16];
  	int ret = getprocstate(getpid(), state, -10);
	if(ret == -1) {
		printf(1, "TEST PASSED");
	} else {
		printf(1, "TEST FAILED: expected -1 ret value");
	}

	exit();
} 
