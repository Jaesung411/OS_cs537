//Tests getnextpid call after forking N number of child processes

#include "types.h"
#include "user.h"
#include "param.h"

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
main(int argc, char *argv[])
{
 // Test getnextpid() call after creating 25 child processes.
  createNChildProcessTest(25);

  exit();
}
