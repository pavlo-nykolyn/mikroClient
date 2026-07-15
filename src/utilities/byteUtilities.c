/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 14-07-2026 */
/**************************************/

#include <stdlib.h>
#include "byteUtilities.h"
#include "iterators.h"
#include "mikro_wrapper_utilities.h"

bool isBE(void);

by_t* allocSeq(const size_t sz)
{
   by_t* pBuf = calloc(sz, sizeof(by_t));
   if (!pBuf) {
      mikro_Log_append(stdout, __LINE__ - 2, "progErr", mikro_messages[mikro_ind_heapFail]);
      exit(EXIT_FAILURE);
   }
   return pBuf;
}

void freeSeq(by_t** addrSeq)
{
   if (*addrSeq) {
      free(*addrSeq);
      *addrSeq = INV_PNT;
   }
}

void reverse(const size_t sz, by_t buf[static sz])
{
   unsigned mark = 0;
   unsigned remaining = sz - mark - 1; // inhibits a warning
   while (mark < remaining) {
      by_t tmp = buf[mark];
      buf[mark] = buf[remaining];
      buf[remaining] = tmp;
      remaining -= 1;
      mark ++;
   }
}

void viewBytes(const size_t sz, const by_t buf[static sz])
{
   fprintf(stdout, "encSent:");
   ITER_SIZE_T_I(0, sz)
      fprintf(stdout, "%2.2hhX", buf[i]);
   fprintf(stdout, "\n");
}
