#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;

  // Counters for memory tracking (Question 1)
  uint total_allocated;
  uint total_freed;
} kmem;

// Initialization code; marks all physical memory after
// the end of the kernel as free.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  kmem.total_allocated = 0;
  kmem.total_freed = 0;
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
  char *p;
  p = (char*)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a call to kalloc().
void
kfree(char *v)
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  if(kmem.use_lock)
    acquire(&kmem.lock);
  
  r = (struct run*)v;
  r->next = kmem.freelist;
  kmem.freelist = r;
  
  // Increment freed counter only after initialization lock is active
  if(kmem.use_lock)
    kmem.total_freed++;

  if(kmem.use_lock)
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  
  r = kmem.freelist;
  if(r){
    kmem.freelist = r->next;
    if(kmem.use_lock)
      kmem.total_allocated++;
  }

  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
}

// Question 1: System Call to display memory statistics
int
sys_memstats(void)
{
  if(kmem.use_lock)
    acquire(&kmem.lock);

  uint alloc = kmem.total_allocated;
  uint freed = kmem.total_freed;
  
  // Count free pages in freelist
  uint free_pages = 0;
  struct run *r = kmem.freelist;
  while(r){
    free_pages++;
    r = r->next;
  }

  if(kmem.use_lock)
    release(&kmem.lock);

  uint current_alloc = alloc - freed;
  uint avail_mem_bytes = free_pages * PGSIZE;

  cprintf("\n--- Memory Statistics ---\n");
  cprintf("Total Allocated Pages : %d\n", alloc);
  cprintf("Total Freed Pages     : %d\n", freed);
  cprintf("Currently Allocated   : %d pages\n", current_alloc);
  cprintf("Available Phys Memory : %d pages (%d KB)\n", free_pages, avail_mem_bytes / 1024);
  cprintf("-------------------------\n\n");

  return 0;
}