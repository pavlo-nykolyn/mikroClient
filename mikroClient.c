/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 13-07-2026 */
/**************************************/

#include <stdio.h>
#include <stdlib.h>
#include "mikro_cli_parser.h"
#include "mikro_word.h"
#include "phy_socketInterface.h"
#include "iterators.h"
#include "mikro_wrapper_utilities.h"

/* macros used during a generic read operation */
#define MIKRO_INIT_R_SZ 1024U // initial size of the read buffer (also used as an increment whenever the current buffer size becomes insufficient)
#define MIKRO_R_TOUT 4000U // read timeout expressed in milliseconds

static const char mikro_loginCmd[] = "/login"; // an implicit command run as the first one

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

// depending on how sophisticated the program will become, I may move these to functions to the byteUtilities module
// just because they belong there
static void mikro_freeByteSeq(by** mikro_addrSeq)
{
   if (*mikro_addrSeq) {
      free(*mikro_addrSeq);
      *mikro_addrSeq = INV_PNT;
   }
}

// 
static by* mikro_allocByteSeq(const size_t newSz,
                              const size_t oldSz, by** pCurrSeq)
{
   by* pSeq = calloc(newSz, sizeof(by));
   if (!pSeq) {
      mikro_Log_append(stdout, __LINE__ - 2, "progErr", mikro_messages[mikro_ind_heapFail]);
      exit(EXIT_FAILURE);
   }
   if (oldSz) {
      memcpy(pSeq, *pCurrSeq, oldSz);
      mikro_freeByteSeq(pCurrSeq);
   }
   return pSeq;
}


// attempts to read a byte sequence
// mikro_sDesc shall be a valid socket descriptor
// if mikro_pCd is not a valid reference to an integer variable, the caller will not obtain information regarding an error condition.
// In this case, the return value will be zero
static size_t mikro_readSeq(const phy_Sck_socketD mikro_sDesc,
                            by** mikro_pBuf,
                            const unsigned mikro_timeout,
                            int* restrict mikro_pCd)
{
   size_t readB = 0;
   size_t buffSz = MIKRO_INIT_R_SZ;
   int cd = mikro_noError;
   do {
      if (!readB ||
          readB == buffSz) {
         buffSz += MIKRO_INIT_R_SZ;
         *mikro_pBuf = mikro_allocByteSeq(buffSz,
                                          readB, mikro_pBuf);
      }
      size_t currReadB = phy_Sck_read(mikro_sDesc,
                                      buffSz - readB, (void*) (*mikro_pBuf + readB),
                                      mikro_timeout,
                                      &cd);
      if (cd) {
         if (readB) {
            memset(*mikro_pBuf, LOW, readB);
            readB = 0;
         }
         break;
      }
      if (currReadB)
         readB += currReadB;
      else {
         mikro_Log_append(stdout, __LINE__ - 2, "rx__err", "TIMEOUT");
         break;
      }
   } while (readB &&
            *(*mikro_pBuf + readB - 1));
   if (mikro_pCd)
      *mikro_pCd = cd;
   return readB;
}

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
   const mikro_FastString loginCmdFS = {.pChArr = mikro_loginCmd,
                                        .len = strlen(mikro_loginCmd)};
   const mikro_String* loginS = mikro_String_create(&loginCmdFS,
                                                    &cd);
   if (cd)
      exit(EXIT_FAILURE);
   const mikro_Word* loginW = mikro_Word_encode(loginS,
                                                INV_PNT,
                                                mikro_wt_command,
                                                &cd);
   if (cd)
      exit(EXIT_FAILURE);
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
   // AUTHENTICATION DATA
   const mikro_Word* userW = INV_PNT;
   const mikro_Word* passW = INV_PNT;
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
      else if (kInd == mikro_key_name ||
               kInd == mikro_key_pass) {
         mikro_Word* tmpW = mikro_Word_encode(mikro_Cli_getArgName(pItem),
                                              mikro_Cli_getArgValue(pItem),
                                              mikro_wt_attribute,
                                              &cd);
         if (kInd == mikro_key_name)
            userW = tmpW;
         else if (kInd == mikro_key_pass)
            passW = tmpW;
      }
   }
   if (pName) {
      *(arrAttrWords + attrWCnt) = mikro_Word_encode(mikro_Cli_getArgValue(pName),
                                                     mikro_Cli_getArgValue(pVal),
                                                     mikro_wt_attribute,
                                                     &cd);
      if (cd)
         exit(EXIT_FAILURE);
   }
   if (!userW ||
       !passW) {
      mikro_Log_append(stdout, __LINE__ - 2, "progErr", mikro_messages[mikro_ind_noAuthData]);
      goto MIKRO_CLEANUP;
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
   /* dispatching the sentence related to authentication (the /login command is implied) */
   phy_Sck_write(sDesc,
                 mikro_Word_getSz(loginW), (void*) mikro_Word_getPBuf(loginW),
                 &cd);
   if (cd)
      goto MIKRO_SOCKET_CLOSE;
   phy_Sck_write(sDesc,
                 mikro_Word_getSz(userW), (void*) mikro_Word_getPBuf(userW),
                 &cd);
   if (cd)
      goto MIKRO_SOCKET_CLOSE;
   phy_Sck_write(sDesc,
                 mikro_Word_getSz(passW), (void*) mikro_Word_getPBuf(passW),
                 &cd);
   if (cd)
      goto MIKRO_SOCKET_CLOSE;
   phy_Sck_write(sDesc,
                 1, (void*) &mikro_Word_emptyW,
                 &cd);
   if (cd)
      goto MIKRO_SOCKET_CLOSE;
   /* obtaining a response */
   int replyWType = mikro_numRepT;
   by* pBuf = INV_PNT; // dynamically allocated during the read operation
   size_t readB = mikro_readSeq(sDesc,
                                &pBuf,
                                MIKRO_R_TOUT,
                                &cd);
   viewBytes(readB, pBuf);
   mikro_Word_decode(readB, pBuf,
                     &replyWType);
   mikro_freeByteSeq(&pBuf); // will be reused for the command reply
   if (replyWType == mikro_rt_done) {
      // the attribute order follows the same one of the command line arguments
      /* dispatching the command sentence */
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
      replyWType = mikro_numRepT;
      size_t readB = mikro_readSeq(sDesc,
                                   &pBuf,
                                   MIKRO_R_TOUT,
                                   &cd);
      viewBytes(readB, pBuf);
      mikro_Word_decode(readB, pBuf,
                        &replyWType);
   }
   /* closing the socket connection */
   MIKRO_SOCKET_CLOSE:
   cd = phy_Sck_closeConnection(sDesc);
   #if defined _WIN32 || defined _WIN64
   cd = phy_Sck_interfaceCleanup();
   #endif
   MIKRO_CLEANUP:
   return EXIT_SUCCESS;
}
