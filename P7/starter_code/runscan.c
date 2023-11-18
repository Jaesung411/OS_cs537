#include "ext2_fs.h"
#include "read_ext2.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

struct stat sbuf;

void* img_ptr;


struct ext2_super_block super;
struct ext2_group_desc group;

void reconstruct_jpg(int fd, char* output_dir);

int main(int argc, char **argv) 
{
    if (argc != 3) 
    {
        printf("expected usage: ./runscan inputfile outputfile\n");
        exit(0);
    }

    char* output_dir=argv[2];
    DIR* dir = opendir(output_dir);
    if(dir != NULL)
    {
        printf("Already exist directory\n");
        exit(0);
    }

    if(mkdir(output_dir,0777) == -1)
    {
        printf("Error make the output directory\n");
        exit(0);
    }

    /* This is some boilerplate code to help you get started, feel free to modify
       as needed! */

    int fd;
    fd = open(argv[1], O_RDONLY);    /* open disk image */

    //ext2_read_init will set the num_groups value
    ext2_read_init(fd);


    // // example read first the super-block and group-descriptor
    // read_super_block(fd, 0, &super);
    // read_group_desc(fd, 0, &group);

    reconstruct_jpg(fd,output_dir);

    
    return 0;
}

//Part1
//reconstruct all jpg files
//scan all inodes that represent regular files and check if the first data block of the inode contains the jpg magic numbers: 
//FF D8 FF E0 or FF D8 FF E1 or FF D8 FF E8
//Once you identify an inode that represents a jpg file, 
//you should copy the content of that file to an output file 
//(stored in your 'output/' directory), using the inode number as the file name. 
void reconstruct_jpg(int fd, char* output_dir)
{

    off_t loc_inode_table;

    //check all group
    for(unsigned int group_i=0; group_i< num_groups; group_i++)
    {
        printf("group: %d\n",group_i);
        //read nth group's super block and group descriptor
        read_super_block(fd, group_i, &super);
        read_group_desc(fd, group_i, &group);

        //get the location of inode table in the nth group
        loc_inode_table = locate_inode_table(group_i, &group);

        //check all inode in the nth group
        for(unsigned int inode_i= group_i * inodes_per_group + 1; 
            inode_i <= group_i * inodes_per_group + inodes_per_group; inode_i++)
        {
            // printf("inode: %d\n",inode_i);
            //read the nth inode in the nth group
            struct ext2_inode* inode = malloc(sizeof(struct ext2_inode));
            read_inode(fd, loc_inode_table, inode_i, inode, super.s_inode_size);

            if (S_ISDIR(inode->i_mode)) {
                //skip if the inode is directory
                continue;
            }
            else if (S_ISREG(inode->i_mode)) {
                
                //for check magic numbers
                char buffer[1024];
                int is_jpg = 0;
                
                //read the first data block of the file and put it buffer
                lseek(fd,BLOCK_OFFSET(inode->i_block[0]),SEEK_SET);
                read(fd,buffer,1024);

                //identify whether it is a jpg file or not
                if (buffer[0] == (char)0xff &&
                    buffer[1] == (char)0xd8 &&
                    buffer[2] == (char)0xff &&
                    (buffer[3] == (char)0xe0 ||
                    buffer[3] == (char)0xe1 ||
                    buffer[3] == (char)0xe8)) 
                {
                    is_jpg = 1;
                }
                
                if(is_jpg==1)
                {
                    printf("inode: %d\n",inode_i);
                    //make file name
                    char name_file[1024];
                    sprintf(name_file, "%s/file-%d.jpg",output_dir,inode_i);
                    int output_file = open(name_file, O_CREAT | O_WRONLY | O_TRUNC, 0666);

                    //now iterate over the 15 data blocks
                    //i) 12 direct data blocks
                    //ii) indirect data blocks
                    //iii) doubly indirect data block
                    //iv) trebly indirect data block
                    for(int i=0; i<EXT2_N_BLOCKS; i++)
                    {
                        //i)12 direct data blocks
                        if(i < EXT2_NDIR_BLOCKS)
                        {
                            int num_block = inode->i_block[i];
                            if(num_block != 0)
                            {
                                // printf("%s\n", name_file);
                                lseek(fd, BLOCK_OFFSET(num_block), SEEK_SET);
                                read(fd, buffer, 1024);
                                write(output_file, buffer, 1024);
                            }
                        }
                        //ii) indirect data blocks
                        else if(i == EXT2_NDIR_BLOCKS)
                        {

                        }
                        //iii) doubly indirect data block
                        else if(i == EXT2_DIND_BLOCK)
                        {

                        }
                        //iv) trebly indirect data block
                        else
                        {

                        }
                    }
                }
                
            }
            else 
            {
                // this inode represents other file types
                continue;
            }

            free(inode);
            inode = NULL;
        }
    }
    
}

//Part2
//find out the filenames of those inodes that represent the jpg files.
//Note that filenames are not stored in inodes, but in directory data blocks. 
//should scan all directory data blocks to find the corresponding filenames.
void find_file()
{
    // for(int  i=0; i<inodes_per_block;i++)
    // {

    // }
}
