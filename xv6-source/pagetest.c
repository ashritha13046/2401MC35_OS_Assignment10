#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  printf(1, "==== Q3: PAGE PERMISSIONS AND PROTECTION TEST ====\n");

  printf(1, "[*] Allocating 1 page...\n");
  char *ptr = sbrk(4096);
  int va = (int)ptr;

  printf(1, "[1] Initial Page Information:\n");
  pageinfo(va);

  printf(1, "[*] Writing to page before setting read-only...\n");
  ptr[0] = 'A';
  printf(1, "Successfully wrote '%c' to address 0x%x\n", ptr[0], va);

  printf(1, "[2] Marking page as READ-ONLY...\n");
  setreadonly(va);

  printf(1, "[3] Page Information after modification:\n");
  pageinfo(va);

  printf(1, "[*] Attempting to write to read-only page (Expect Trap/Page Fault)...\n");
  ptr[0] = 'B'; // This will cause a kernel page fault exception and terminate process

  printf(1, "ERROR: Should not reach here if page protection worked!\n");
  exit();
}