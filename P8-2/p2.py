#! /bin/env python

import toolspath
from testing import Xv6Build, Xv6Test

class test1a(Xv6Test):
   name = "test1a"
   description = "Tests getnextpid() after one fork call"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 1

class test1b(Xv6Test):
   name = "test1b"
   description = "Tests getnextpid() after a child process exits"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 1

class test1c(Xv6Test):
   name = "test1c"
   description = "Tests getnextpid() after forking N number of child processes"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 1

class test2a(Xv6Test):
   name = "test2a"
   description = "Tests getprocstate() for run process"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 1

class test2b(Xv6Test):
   name = "test2b"
   description = "Tests getprocstate() for runble process"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 1

class test2c(Xv6Test):
   name = "test2c"
   description = "Tests getprocstate() for zombie process"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 1

class test2d(Xv6Test):
   name = "test2d"
   description = "Tests getprocstate() with negative byte size"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 1

class test2e(Xv6Test):
   name = "test2e"
   description = "Tests getprocstate() for out of bounds pid"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 1

class test2f(Xv6Test):
   name = "test2f"
   description = "Tests getprocstate() for next pid"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 1

class test2g(Xv6Test):
   name = "test2g"
   description = "Tests getprocstate() for multiple pid"
   tester = "ctests/" + name + ".c"
   make_qemu_args = "CPUS=1"
   point_value = 1


import toolspath
from testing.runtests import main
main(Xv6Build, all_tests=[test2a,test2b,test2c,test2d, test1a, test1b, test1c, test2e, test2f, test2g])
