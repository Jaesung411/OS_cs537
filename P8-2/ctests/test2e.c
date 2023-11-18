// Test getprocstate for run process.
//
#include "types.h"
#include "user.h"

int
main(void)
{
	char state[16];
	int ret = getprocstate(100, state, 16);
        
	if(ret == -1) {
		printf(1, "TEST PASSED");
	} else {
		printf(1, "TEST FAILED getprocstate didn't fail for out of bounds pid");
	}

	exit();
} 
