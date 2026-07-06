/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 03-07-2026 */
/**************************************/

#include <stdlib.h>
#include "mikro_string.h"
#include "mikro_wrapper_utilities.h"
#include "generic.h"

struct mikro_string {
   size_t len;
   char* buf; // expected to live within the boundaries of the heap
};

// mikro_string_sz will be incremented by one, given that the underlying datum is a string
static char* mikro_string_alloc(const size_t mikro_string_sz)
{
   char* pBuf = INV_PNT;
   pBuf = (char*) calloc(mikro_string_sz + 1, sizeof(char));
   if (!pBuf) {
      mikro_Log_append(stdout, __LINE__ - 2, "progErr", mikro_messages[mikro_ind_heapFail]);
      exit(EXIT_FAILURE);
   }
   return pBuf;
}

mikro_String* mikro_String_create(const mikro_FastString* const mikro_pFStr,
                                  int* restrict mikro_pCode)
{
   int mikro_code = mikro_noError;
   mikro_String* mikro_pStr = INV_PNT;
   if (!mikro_pFStr ||
       (mikro_pFStr -> len) == 0 ||
       !(mikro_pFStr -> pChArr)) {
      mikro_Log_append(stdout, __LINE__ - 3, "progErr", mikro_messages[mikro_ind_wrI]);
      mikro_code = mikro_wrI;
      goto MIKRO_STRING_CREATE_EXIT;
   }
   if (((mikro_pFStr -> len) && !(mikro_pFStr -> pChArr)) ||
       (!(mikro_pFStr -> len) && (mikro_pFStr -> pChArr))) {
      mikro_Log_append(stdout, __LINE__ - 2, "progErr", mikro_messages[mikro_ind_incChArr]);
      mikro_code = mikro_incChArr;
      goto MIKRO_STRING_CREATE_EXIT;
   }
   mikro_pStr = (mikro_String*) calloc(1, sizeof(struct mikro_string));
   if (!mikro_pStr) {
      mikro_Log_append(stdout, __LINE__ - 2, "progErr", mikro_messages[mikro_ind_heapFail]);
      exit(EXIT_FAILURE);
   }
   mikro_pStr -> len = mikro_pFStr -> len;
   mikro_pStr -> buf = mikro_string_alloc(mikro_pFStr -> len);
   memcpy(mikro_pStr -> buf, mikro_pFStr -> pChArr, mikro_pFStr -> len);
   MIKRO_STRING_CREATE_EXIT:
   if (mikro_pCode)
      *mikro_pCode = mikro_code;
   return mikro_pStr;
}

void mikro_String_destroy(mikro_String** mikro_addrStr)
{
   if (*mikro_addrStr) {
      mikro_String* pStr = *mikro_addrStr;
      free(pStr -> buf);
      pStr -> buf = INV_PNT;
      free(*mikro_addrStr);
      *mikro_addrStr = INV_PNT;
   }
}

const size_t mikro_String_getLen(const mikro_String* restrict mikro_pStr)
{
   return mikro_pStr ? mikro_pStr -> len
                     : 0ULL;
}

const char* mikro_String_getPChArr(const mikro_String* restrict mikro_pStr)
{
   return mikro_pStr ? mikro_pStr -> buf
                     : INV_PNT;
}
