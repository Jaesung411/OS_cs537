// Get proc state for zombie process.
#include "types.h"
#include "user.h"


int createNChildProcessTest(int n) {
   int nextpid1 = getnextpid();

   for(int i = 0; i < n; i++) {
        int childpid = fork();
        if(childpid > 0) {
           wait();
        } else {
          return 0;
        }
   }

   int nextpid2 = getnextpid();
   if (nextpid2 == nextpid1 + n) {
           printf(1, "TEST PASSED");
   } else {
           printf(1, "TEST FAILED: nextpid returned = %d, expected = %d", nextpid2, nextpid1 + n);
   }

   return 0;
}


int
main(void)
{
  createNChildProcessTest(20);
  int nextpid = getnextpid();
  printf(1, "next pid=%d\n", nextpid);
  
  char state[16];
  int ret = getprocstate(nextpid, state, 0);
	
if (ret == -1) {
	printf(1, "TEST PASSED");
} else {
	printf(1, "TEST FAILED: proc state should fail");
	}
} 
