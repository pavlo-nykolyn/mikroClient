/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 13-07-2026 */
/**************************************/

#include "byteUtilities.h"
#include "iterators.h"

bool isBE(void);

void reverse(const size_t sz, by buf[static sz])
{
   unsigned mark = 0;
   unsigned remaining = sz - mark - 1; // inhibits a warning
   while (mark < remaining) {
      by tmp = buf[mark];
      buf[mark] = buf[remaining];
      buf[remaining] = tmp;
      remaining -= 1;
      mark ++;
   }
}

void viewBytes(const size_t sz, const by buf[static sz])
{
   ITER_SIZE_T_I(0, sz)
      fprintf(stdout, "%2.2hhX", buf[i]);
   fprintf(stdout, "\n");
}
