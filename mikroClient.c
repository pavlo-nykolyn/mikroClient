/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 05-07-2026 */
/**************************************/

#include <stdio.h>
#include <stdlib.h>
#include "mikro_cli_parser.h"
#include "mikro_word.h"
#include "phy_socketInterface.h"
#include "iterators.h"
#include "mikro_wrapper_utilities.h"

#define MIKRO_I_ST_ITM_IDX 5U // the starting index of the first CLI item used for an internal data structure

// a sequence of keys that are to be used by the CLI interface
enum mikro_keyList {
                    mikro_key_host, /**< an IPv4 address */
                    mikro_key_port, /**< TCP port */
                    mikro_key_name, /**< user name */
                    mikro_key_pass, /**< password */
                    mikro_key_comm, /**< command */
                    mikro_key_attN, /**< attribute name */
                    mikro_key_attV, /**< attribute value */
                    mikro_numKeys   /**< number of keys */
                   };

static const char* mikro_expected_keys[mikro_numKeys] = {
                                                         [mikro_key_host] = "host",
                                                         [mikro_key_port] = "port",
                                                         [mikro_key_name] = "name",
                                                         [mikro_key_pass] = "password",
                                                         [mikro_key_comm] = "command",
                                                         [mikro_key_attN] = "attribute-name",
                                                         [mikro_key_attV] = "attribute-value"
                                                        };

static enum mikro_keyList mikro_chkKey(const char* restrict mikro_pChArr)
{
   enum mikro_keyList kInd = mikro_numKeys;
   if (!strcmp(mikro_pChArr, mikro_expected_keys[mikro_key_host]))
      kInd = mikro_key_host;
   else if (!strcmp(mikro_pChArr, mikro_expected_keys[mikro_key_port]))
      kInd = mikro_key_port;
   else if (!strcmp(mikro_pChArr, mikro_expected_keys[mikro_key_name]))
      kInd = mikro_key_name;
   else if (!strcmp(mikro_pChArr, mikro_expected_keys[mikro_key_pass]))
      kInd = mikro_key_pass;
   else if (!strcmp(mikro_pChArr, mikro_expected_keys[mikro_key_comm]))
      kInd = mikro_key_comm;
   else if (!strcmp(mikro_pChArr, mikro_expected_keys[mikro_key_attN]))
      kInd = mikro_key_attN;
   else if (!strcmp(mikro_pChArr, mikro_expected_keys[mikro_key_attV]))
      kInd = mikro_key_attV;
   return kInd;
}

int main(int argc, void** argv)
{
   int cd = mikro_noError;
   mikro_Cli_item** arrItems = calloc(argc - 1, sizeof(mikro_Cli_item*));
   if (!arrItems) {
      mikro_Log_append(stdout, __LINE__ - 2, "progErr", mikro_messages[mikro_ind_heapFail]);
      exit(EXIT_FAILURE);
   }
   unsigned itemCnt = 0; // item counter. Each time an item gets added to arrItems, the counter will be increased by one
   unsigned attrsCnt = 0; // attribute counter. Any attribute-name occurrence will be added to the counter
   enum mikro_keyList prevKInd = mikro_numKeys; // key indicator of the previously extracted input argument
   /* retrieving the CLI arguments */
   ITER_SGN_I(1, argc) {
      enum mikro_keyList kInd = mikro_numKeys;
      printf("[%d] argv=%s\n", i, (char*) argv[i]);
      mikro_FastString concatArg = {.len = strlen(argv[i]),
                                    .pChArr = argv[i]};
      mikro_Cli_item* pItem = mikro_Cli_parse_arg(&concatArg,
                                                  &cd);
      if (!pItem)
         return EXIT_FAILURE;
      kInd = mikro_chkKey(mikro_String_getPChArr(mikro_Cli_getArgName(pItem)));
      if (kInd == mikro_numKeys) {
         mikro_Log_append_withStr(stdout, __LINE__ - 2, "progErr", mikro_messages[mikro_ind_wrCLIOpt], concatArg.pChArr);
         exit(EXIT_FAILURE);
      }
      if ((kInd == mikro_key_attV &&
           prevKInd != mikro_key_attN) ||
          (kInd == mikro_key_attV &&
           prevKInd == mikro_numKeys)) {
         mikro_Log_append_withStr(stdout, __LINE__ - 4, "progErr", mikro_messages[mikro_ind_unCLIVal], concatArg.pChArr);
         exit(EXIT_FAILURE);
      }
      else {
         if (kInd == mikro_key_attN)
            attrsCnt += 1;
         *(arrItems + i - 1) = pItem;
      }
      prevKInd = kInd;
   }
   /* obtaining the various parameters */
   // COMMAND
   mikro_Word* comm = INV_PNT;
   // ENDPOINT
   const mikro_String* host = INV_PNT;
   const mikro_String* port = INV_PNT;
   // ATTRIBUTES
   mikro_Word** arrAttrWords = calloc(attrsCnt, sizeof(mikro_Word*));
   if (!arrAttrWords) {
      mikro_Log_append(stdout, __LINE__ - 2, "progErr", mikro_messages[mikro_ind_heapFail]);
      exit(EXIT_FAILURE);
   }
   /* keep track of a name-value pair */
   /* ---------- */
   mikro_Cli_item* pName = INV_PNT;
   mikro_Cli_item* pVal = INV_PNT;
   /* ---------- */
   unsigned attrWCnt = 0; // counter for attribute words
   ITER_UNS_I(1, argc) {
      mikro_Cli_item* pItem = *(arrItems + i - 1);
      enum mikro_keyList kInd = mikro_chkKey(mikro_String_getPChArr(mikro_Cli_getArgName(pItem)));
      if (kInd == mikro_key_attN) {
         if (pName) {
            *(arrAttrWords + attrWCnt) = mikro_Word_encode(mikro_Cli_getArgValue(pName),
                                                           mikro_Cli_getArgValue(pVal),
                                                           mikro_wt_attribute,
                                                           &cd);
            if (cd)
               exit(EXIT_FAILURE);
            attrWCnt ++;
            pName = INV_PNT;
            pVal = INV_PNT;
         }
         pName = pItem;
      }
      else if (kInd == mikro_key_attV)
         pVal = pItem;
      else if (kInd == mikro_key_comm) {
         comm = mikro_Word_encode(mikro_Cli_getArgValue(pItem),
                                  INV_PNT,
                                  mikro_wt_command,
                                  &cd);
         if (cd)
            exit(EXIT_FAILURE);
      }
      else if (kInd == mikro_key_host)
         host = mikro_Cli_getArgValue(pItem);
      else if (kInd == mikro_key_port)
         port = mikro_Cli_getArgValue(pItem);
   }
   if (pName) {
      *(arrAttrWords + attrWCnt) = mikro_Word_encode(mikro_Cli_getArgValue(pName),
                                                     mikro_Cli_getArgValue(pVal),
                                                     mikro_wt_attribute,
                                                     &cd);
      if (cd)
         exit(EXIT_FAILURE);
   }
   /* opening the socket connection */
   #if defined _WIN32 || defined _WIN64
   cd = phy_Sck_interfaceInitializer();
   if (cd)
      goto MIKRO_CLEANUP;
   #endif
   phy_Sck_socketD sDesc = phy_Sck_openConnection(mikro_String_getPChArr(host), mikro_String_getPChArr(port),
                                                  &cd);
   if (cd)
      goto MIKRO_SOCKET_CLOSE;
   // the attribute order follows the same one of the command line arguments
   /* dispatching the sentence */
   phy_Sck_write(sDesc,
                 mikro_Word_getSz(comm), (void*) mikro_Word_getPBuf(comm),
                 &cd);
   if (cd)
      goto MIKRO_SOCKET_CLOSE;
   ITER_UNS_I(0, attrWCnt) {
      phy_Sck_write(sDesc,
                    mikro_Word_getSz(*(arrAttrWords + i)), (void*) mikro_Word_getPBuf(*(arrAttrWords + i)),
                    &cd);
      if (cd)
         goto MIKRO_SOCKET_CLOSE;
   }
   phy_Sck_write(sDesc,
                 1, (void*) &mikro_Word_emptyW,
                 &cd);
   if (cd)
      goto MIKRO_SOCKET_CLOSE;
   /* obtaining a response */
   by test[1024] = {LOW}; // just a weird test buffer
   size_t readB = 0;
   do {
      size_t currReadB = phy_Sck_read(sDesc,
                                      1024 - readB, (void*) (test + readB),
                                      4000,
                                      &cd);
      if (cd)
         goto MIKRO_SOCKET_CLOSE;
      if (currReadB)
         readB += currReadB;
      else {
         mikro_Log_append(stdout, __LINE__ - 2, "rx__err", "TIMEOUT");
         break;
      }
   } while (readB < 1024 &&
            *(test + readB - 1));
   viewBytes(readB, test);
   /* closing the socket connection */
   MIKRO_SOCKET_CLOSE:
   cd = phy_Sck_closeConnection(sDesc);
   #if defined _WIN32 || defined _WIN64
   cd = phy_Sck_interfaceCleanup();
   #endif
   MIKRO_CLEANUP:
   return EXIT_SUCCESS;
}
