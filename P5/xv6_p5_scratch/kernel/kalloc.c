// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "spinlock.h"

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;

  /*
  P5 changes
  */
  uint free_pages; //track free pages
  uint ref_cnt[PHYSTOP / PGSIZE]; //track reference count

} kmem;

extern char end[]; // first address after kernel loaded from ELF file

// Initialize free list of physical pages.
void
kinit(void)
{
  char *p;

  initlock(&kmem.lock, "kmem");
  p = (char*)PGROUNDUP((uint)end);
  for(; p + PGSIZE <= (char*)PHYSTOP; p += PGSIZE){
    kmem.ref_cnt[((uint)p) / PGSIZE] = 1; 
    kfree(p);
  }

}

//helper method for increasing or decreasing reference count
//if inc_dec value == 1, increase the reference count
//if inc_dec value == -1, decrease the reference count
void
inc_dec_ReferenceCount(char* addr, int inc_dec)
{
  acquire(&kmem.lock);
  if(inc_dec==1){
    kmem.ref_cnt[((uint)addr) /PGSIZE]++;
  }
  else if(inc_dec==-1){
    kmem.ref_cnt[((uint)addr) / PGSIZE]--;
  }
  release(&kmem.lock);
}

//helper method for changing reference count by the change_value
void
changeReferenceCount(char* addr, int change_value)
{
  acquire(&kmem.lock);
  kmem.ref_cnt[((uint)addr) / PGSIZE]=change_value;  
  release(&kmem.lock);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;
  
  if((uint)v % PGSIZE || v < end || (uint)v >= PHYSTOP) 
    panic("kfree");

  acquire(&kmem.lock);
  //free the page when one process points to the page
  if(kmem.ref_cnt[((uint)v) / PGSIZE] <= 1){
    // Fill with junk to catch dangling refs.
    memset(v, 1, PGSIZE);

    r = (struct run*)v;
    r->next = kmem.freelist;
    kmem.freelist = r;
    kmem.free_pages++;
    release(&kmem.lock);
    //set the reference count to one when a page is freed
    changeReferenceCount((char*)v,1);
    return;
  }
  //just decrease the reference when more than two processes points to the page
  else{
    release(&kmem.lock);
    inc_dec_ReferenceCount((char*)v,-1);
  	return;
  }
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;
  
  acquire(&kmem.lock);
  r = kmem.freelist;

  if(r)
  {
    kmem.freelist = r->next;
    kmem.free_pages--;//decrease freed pages
    release(&kmem.lock);
    //the reference count set to one when a free page is allocated 
    changeReferenceCount((char*)r,1);
  }
  else{
  	release(&kmem.lock);
  }
  return (char*)r;
}

//helper method to get the number of free pages
int
getFreePagesCount(void)
{
  return kmem.free_pages;
}

//helper method to get the number of reference count
uint getRefCount(char* addr)
{
  return kmem.ref_cnt[((uint)addr) / PGSIZE];
}
