#include "ext2_fs.h"
#include "read_ext2.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

void reconstruct_jpg(int fd, char* output_dir);
void write_block(struct ext2_inode* inode, int fd, char* buffer, int output_file);

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

    int fd;
    fd = open(argv[1], O_RDONLY);    /* open disk image */

    ext2_read_init(fd);

    reconstruct_jpg(fd, output_dir);
    
    return 0;
}

void reconstruct_jpg(int fd, char* output_dir)
{

    struct ext2_super_block super;
    struct ext2_group_desc groups[num_groups];

    off_t  addr_table_i;
    read_super_block(fd, &super);
    read_group_descs(fd, groups, num_groups);

    //check all group
    for(unsigned int group_i=0; group_i < num_groups; group_i++)
    {
        // get the location of inode table in the nth group
        addr_table_i = locate_inode_table(group_i, groups);

        unsigned int first_inode_i = inodes_per_group * group_i + 1;
        
        // check all inode in the nth group
        for(unsigned int inode_i = first_inode_i; inode_i <= first_inode_i + inodes_per_group; inode_i++)
        {
            
            struct ext2_inode* inode = malloc(sizeof(struct ext2_inode));
            read_inode(fd, addr_table_i, inode_i, inode, super.s_inode_size);

            char buffer[1024];

            //Part2 start
            //check the inode is directory
            if (S_ISDIR(inode->i_mode)) 
            {
            
                char dir_buffer[1024];
                struct ext2_dir_entry* dir_entry;

                //read the first block
                int num_block = inode->i_block[0];
                lseek(fd, BLOCK_OFFSET(num_block),SEEK_SET);
                read(fd,dir_buffer,1024);

                //read all directoty entry
                int offset = 0;
                while(offset < 1024)
                {
                    
                    dir_entry = (struct ext2_dir_entry*)&dir_buffer[offset];

                    //get the file name    
                    int name_len = dir_entry->name_len & 0xFF;
                    char file_name[EXT2_NAME_LEN];
                    strncpy(file_name, dir_entry->name, name_len);
                    file_name[name_len] = '\0';

                    //read the inode of the file
                    read_inode(fd,  addr_table_i, dir_entry->inode, inode, super.s_inode_size);
                    lseek(fd,BLOCK_OFFSET(inode->i_block[0]),SEEK_SET);
                    read(fd,buffer,1024);

                    //check  if  the  first data block of the inode contains the jpg magic numbers
                    if (buffer[0] == (char)0xff &&
                        buffer[1] == (char)0xd8 &&
                        buffer[2] == (char)0xff &&
                        (buffer[3] == (char)0xe0 ||
                        buffer[3] == (char)0xe1 ||
                        buffer[3] == (char)0xe8)) 
                    {
                        //make the origin file
                        char name_file[1024];
                        sprintf(name_file, "%s/%s",output_dir,file_name);

                        //make the detail file
                        char detail[1000];
                        sprintf(detail,"%d\n%d\n%d",inode->i_links_count, inode->i_size, inode->i_uid);
                        char name_detail[1000];
                        sprintf(name_detail,"%s/file-%d-details.txt",output_dir,dir_entry->inode);

                        int detail_file = open(name_detail, O_CREAT | O_WRONLY | O_TRUNC, 0666);
                        write(detail_file,detail,strlen(detail));
                        
                        int output_file = open(name_file, O_CREAT | O_WRONLY | O_TRUNC, 0666);

                        write_block(inode, fd, buffer, output_file);

                        close(output_file);
                
                    }

                    //move the next directory entry
                    offset+= 8 + name_len;
                    if(name_len % 4 != 0)
                    {
                        offset+= 4 - name_len % 4;
                    }

                }
            }
            //check if the inode is file
            else if (S_ISREG(inode->i_mode)) {
                
                int is_jpg = 0;
                
                //read the first block
                lseek(fd,BLOCK_OFFSET(inode->i_block[0]),SEEK_SET);
                read(fd,buffer,1024);

                //check if the file is a jpg file or not
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
                    //make file name
                    char name_file[1024];
                    sprintf(name_file, "%s/file-%d.jpg",output_dir,inode_i);
                    int output_file = open(name_file, O_CREAT | O_WRONLY | O_TRUNC, 0666);

                    write_block(inode, fd, buffer, output_file);

                    close(output_file);
                
                }

                else 
                {
                    continue;
                }

                free(inode);
                inode = NULL;
            }
        }
    }
}

//Helper Method 
//write all blocks pointer of inode
void write_block(struct ext2_inode* inode, int fd, char* buffer, int output_file)
{
    // char buffer[1024];

    //calculate the number of pointers
    int count_blocks= inode->i_size/block_size;
    //check the size of the last block 
    int last_size =1024;
    if(inode->i_size%block_size != 0)
    {
        last_size = inode->i_size%block_size;
        count_blocks++;
    }
                    
    int curr_count_block=0; //track the current number pointer

    for(int block_i=0; block_i<EXT2_N_BLOCKS; block_i++)
    {
        int num_block;
                            
        //i) 12 direct data blocks
        if(block_i < EXT2_NDIR_BLOCKS)
        {
            num_block = inode->i_block[block_i];
                                    
            curr_count_block++;
            lseek(fd, BLOCK_OFFSET(num_block), SEEK_SET);
            read(fd, buffer, block_size);
            if(inode->i_size%block_size != 0 && curr_count_block==count_blocks)
            {
                write(output_file, buffer, last_size);
            }
            else
            {
                write(output_file, buffer, block_size);
            }            
                                
        }
        //ii) indirect data blocks
        else if(block_i == EXT2_NDIR_BLOCKS)
        {
            int num_indir_block = inode->i_block[block_i];
                        
            lseek(fd,BLOCK_OFFSET(num_indir_block),SEEK_SET);
            read(fd, buffer, 1024);
            for(int offset = 0; offset < 1024; offset += 4 )
            {
                num_block = *(int*)(buffer+offset);
                if(num_block!=0)
                {           
                    curr_count_block++;
                                            
                    char temp_ind_buffer[1024];
                    lseek(fd, BLOCK_OFFSET(num_block), SEEK_SET);
                    read(fd, temp_ind_buffer, sizeof(temp_ind_buffer));
                                        
                    if(inode->i_size % block_size != 0 && curr_count_block==count_blocks)
                    {
                        write(output_file, temp_ind_buffer, last_size);
                    }
                    else
                    {
                                write(output_file, temp_ind_buffer, block_size);
                    }                 
                }
            }
        }                        
        //iii) doubly indirect data block
        else if(block_i == EXT2_DIND_BLOCK)
        {
            int num_db_block = inode->i_block[block_i];
            if(num_db_block != 0)
            {
                lseek(fd,BLOCK_OFFSET(num_db_block),SEEK_SET);
                read(fd, buffer, block_size);

                for(int offset = 0; offset < 1024; offset += 4 )
                {
                    int num_indir_block = *(int*)(buffer+offset);
                    if(num_indir_block != 0)
                    {
                        char temp_db_buffer[1024];
                        lseek(fd, BLOCK_OFFSET(num_indir_block), SEEK_SET);
                        read(fd, temp_db_buffer, 1024);
                        for(int offset2=0; offset2<1024; offset2+=4)
                        {
                            num_block=*(int*)(temp_db_buffer+offset2);
                            if(num_block!=0)
                            {
                                curr_count_block++;
                                char temp2_db_buffer[1024];
                                lseek(fd, BLOCK_OFFSET(num_block), SEEK_SET);
                                read(fd, temp2_db_buffer, block_size);
                                 if(inode->i_size%block_size != 0 && curr_count_block==count_blocks)
                                {
                                    write(output_file, temp2_db_buffer, last_size);
                                }
                                else
                                {
                                    write(output_file, temp2_db_buffer, block_size);
                                }
                            }
                        }
                    }
                }
            }  
        }
    }
}