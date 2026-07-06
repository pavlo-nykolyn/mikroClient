/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 01-07-2026 */
/**************************************/

#include <stdlib.h>
#include "mikro_cli_parser.h"
#include "mikro_wrapper_utilities.h"
#include "generic.h"

struct mikro_cli_item {
/* CLI argument */
   mikro_String* item_name; // will not include the prefix --
   mikro_String* item_value;
};

mikro_Cli_item* mikro_Cli_parse_arg(const mikro_FastString* restrict mikro_pFS,
                                    int* restrict mikro_pCd)
{
   int cd = mikro_noError;
   unsigned off = 0; // initial offset used to skip the -- prefix, if present
   if (*(mikro_pFS -> pChArr) == '-' &&
       *((mikro_pFS -> pChArr) + 1) == '-')
      off = 2;
   size_t kSz = strcspn((mikro_pFS -> pChArr) + off, "="); // key size
   size_t vSz = 0; // assuming an empty value
   mikro_Cli_item* mikro_cli_pItem = (mikro_Cli_item*) calloc(1, sizeof(struct mikro_cli_item));
   if (!mikro_cli_pItem) {
      mikro_Log_append(stdout, __LINE__ - 2, "progErr", mikro_messages[mikro_ind_heapFail]);
      exit(EXIT_FAILURE);
   }
   // allocating space for the name
   mikro_FastString keyFStr = {.len = kSz,
                               .pChArr = (mikro_pFS -> pChArr) + off};
   mikro_cli_pItem -> item_name = mikro_String_create(&keyFStr,
                                                      &cd);
   if (!(mikro_cli_pItem -> item_name))
      goto MIKRO_CLI_PARSE_ARG_EXIT;
   if (kSz < (mikro_pFS -> len) - off) {
      vSz = (mikro_pFS -> len) - off - kSz - 1;
      // allocating space for the value
      mikro_FastString valFStr = {.len = vSz,
                                  .pChArr = (mikro_pFS -> pChArr) + off + kSz + 1};
      mikro_cli_pItem -> item_value = mikro_String_create(&valFStr,
                                                          &cd);
      if (!(mikro_cli_pItem -> item_value))
         goto MIKRO_CLI_PARSE_ARG_EXIT;
   }
   MIKRO_CLI_PARSE_ARG_EXIT:
   if (mikro_pCd)
      *mikro_pCd = cd;
   return mikro_cli_pItem;
}

void mikro_Cli_destroy_item(mikro_Cli_item** mikro_addrItem)
{
   if (mikro_addrItem) {
      mikro_Cli_item* tmp = *mikro_addrItem;
      if (tmp -> item_name) {
         mikro_String_destroy(&(tmp -> item_name));
         tmp -> item_name = INV_PNT;
      }
      if (tmp -> item_value) {
         mikro_String_destroy(&(tmp -> item_value));
         tmp -> item_value = INV_PNT;
      }
      free(*mikro_addrItem);
      *mikro_addrItem = INV_PNT;
   }
}

const mikro_String* mikro_Cli_getArgName(const mikro_Cli_item* restrict mikro_pItem)
{
   return mikro_pItem ? mikro_pItem -> item_name
                      : INV_PNT;
}

const mikro_String* mikro_Cli_getArgValue(const mikro_Cli_item* restrict mikro_pItem)
{
   return mikro_pItem ? mikro_pItem -> item_value
                      : INV_PNT;
}
