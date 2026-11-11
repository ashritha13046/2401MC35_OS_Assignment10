#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  printf(1, "==== Q1: MEMORY MANAGEMENT TEST ====\n");
  printf(1, "Initial Memory Statistics:\n");
  memstats();

  printf(1, "[*] Allocating 5 pages (20480 bytes) using sbrk()...\n");
  char *ptr = sbrk(5 * 4096);
  if(ptr == (char*)-1){
    printf(1, "sbrk allocation failed!\n");
    exit();
  }

  printf(1, "Memory Statistics After Allocation:\n");
  memstats();

  printf(1, "[*] Deallocating 5 pages using sbrk(-20480)...\n");
  sbrk(-5 * 4096);

  printf(1, "Memory Statistics After Deallocation:\n");
  memstats();

  printf(1, "==== End of Q1 Test ====\n");
  exit();
}