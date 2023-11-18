#include "ext2_fs.h"
#include "read_ext2.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


int main(int argc, char **argv) 
{
    if (argc != 3) 
    {
        printf("expected usage: ./runscan inputfile outputfile\n");
        exit(0);
    }

    /* This is some boilerplate code to help you get started, feel free to modify
       as needed! */

    int fd;
    fd = open(argv[1], O_RDONLY);    /* open disk image */

    ext2_read_init(fd);

    struct ext2_super_block super;
    read_super_block(fd, &super);

    int num_groups = (super.s_blocks_count + super.s_blocks_per_group - 1) / super.s_blocks_per_group;
    struct ext2_group_desc groups[num_groups];

    read_group_descs(fd, groups, num_groups);
    
    return 0;
}