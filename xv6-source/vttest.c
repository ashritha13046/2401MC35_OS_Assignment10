#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  printf(1, "==== Q2: VIRTUAL TO PHYSICAL TRANSLATION TEST ====\n");
  
  printf(1, "[*] Allocating 1 page using sbrk()...\n");
  char *ptr = sbrk(4096);
  ptr[0] = 'X'; // Force page allocation and write

  int va1 = (int)ptr;
  int va2 = (int)(ptr + 100);

  printf(1, "[1] Testing valid base address 0x%x:\n", va1);
  vtop(va1);

  printf(1, "[2] Testing valid address inside page 0x%x:\n", va2);
  vtop(va2);

  printf(1, "[3] Testing invalid/unmapped address 0x7FFFFFFF:\n");
  vtop(0x7FFFFFFF);

  exit();
}