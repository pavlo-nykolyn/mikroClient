/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 06-09-2026 */
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
                    mikro_key_help, /**< concatenates the command synopsis to the output stream and exits */
                    mikro_key_aDir, /**< directory containing authentication data used during the TLS handshake */
                    mikro_key_keyF, /**< file name associated with the key used during the TLS handshake */
                    mikro_key_crtF, /**< file name associated with the certificate used during the TLS handshake */
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
                                                         [mikro_key_help] = "help",
                                                         [mikro_key_aDir] = "authentication-data-directory",
                                                         [mikro_key_keyF] = "key-file",
                                                         [mikro_key_crtF] = "crt-file",
                                                         [mikro_key_host] = "host",
                                                         [mikro_key_port] = "port",
                                                         [mikro_key_name] = "name",
                                                         [mikro_key_pass] = "password",
                                                         [mikro_key_comm] = "command",
                                                         [mikro_key_attN] = "attribute-name",
                                                         [mikro_key_attV] = "attribute-value"
                                                        };

static void mikro_help(void)
{
   fputs("\n\
^^^SYNOPSIS^^^\n\
\n\
mikroClient [--help]\n\
--host=<host>\n\
--port=<port>\n\
--name=<username>\n\
--password=<password>\n\
--command=<command>\n\
[--attribute-name=<identifier>{ --attribute-name=<identifier>}]\n\
[--attribute-value=<token>{ --attribute-value=<token>}]\n\
\n\
some important notes:\n\
* any API call SHALL always be authenticated;\n\
* currently, a connection established toward the 8729 TCP port will not yield anything;\n\
* an attribute name cannot be defined without the corresponding attribute value and viceversa. Though, the options themselves may be value-less;\n\
* the first character of <command> shall be a slash; if <command> contains white-space characters, the --command option shall be quoted.\n", stdout);
}

// provides a realloc-like behaviour (only if oldSz is non-zero)
static by_t* mikro_allocByteSeq(const size_t newSz,
                                const size_t oldSz, by_t** pCurrSeq)
{
   by_t* pSeq = calloc(newSz, sizeof(by_t));
   if (!pSeq) {
      mikro_Log_append(stdout, __LINE__ - 2, "progErr", mikro_messages[mikro_ind_heapFail]);
      exit(EXIT_FAILURE);
   }
   if (oldSz) {
      memcpy(pSeq, *pCurrSeq, oldSz);
      freeSeq(pCurrSeq);
   }
   return pSeq;
}


// attempts to read a byte sequence
// mikro_sDesc shall be a valid socket descriptor
// if mikro_pCd is not a valid reference to an integer variable, the caller will not obtain information regarding an error condition.
// In this case, the return value will be zero
static size_t mikro_readSeq(const phy_Sck_socketD mikro_sDesc,
                            by_t** mikro_pBuf,
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
   if (!strcmp(mikro_pChArr, mikro_expected_keys[mikro_key_help]))
      kInd = mikro_key_help;
   else if (!strcmp(mikro_pChArr, mikro_expected_keys[mikro_key_keyF]))
      kInd = mikro_key_keyF;
   else if (!strcmp(mikro_pChArr, mikro_expected_keys[mikro_key_crtF]))
      kInd = mikro_key_crtF;
   else if (!strcmp(mikro_pChArr, mikro_expected_keys[mikro_key_aDir]))
      kInd = mikro_key_aDir;
   else if (!strcmp(mikro_pChArr, mikro_expected_keys[mikro_key_host]))
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
   unsigned itemCnt = 0; // item counter. Each time an item gets added to arrItems, the counter will be increased by one
   unsigned attrsCnt = 0; // attribute counter. Any attribute-name occurrence will be added to the counter
   mikro_FastString_t loginCmdFS = {.pChArr = mikro_loginCmd,
                                    .len = strlen(mikro_loginCmd)}; // an external string that references the authentication command
   /* a couple of variables that use heap memory */
   by_t* pBuf = INV_PNT; // the read buffer
   mikro_String_t* loginS = INV_PNT; // the string that encodes the authentication command ...
   mikro_Word_t* loginW = INV_PNT; // ... and the corresponding word
   mikro_Cli_item_t** arrItems = INV_PNT; // an array of CLI-based items
   // COMMAND
   mikro_Word_t* commW = INV_PNT; // the command word
   // ENDPOINT
   const mikro_String_t* host = INV_PNT; // the string containing the host address
   const mikro_String_t* port = INV_PNT; // the string containing the port address
   // AUTHENTICATION DATA
   mikro_Word_t* userW = INV_PNT; // the user name word
   mikro_Word_t* passW = INV_PNT; // the password word
   mikro_String_t* aDir = INV_PNT; // the directory containing authentication data used during the TLS handshake
   mikro_String_t* keyF = INV_PNT; // the file name for the key used during the TLS handshake
   mikro_String_t* crtF = INV_PNT; // the file name for the certificate used during the TLS handshake
   // ATTRIBUTES
   mikro_Word_t** arrAttrWords = INV_PNT; // an array containing attribute words
   /* defining a set of variables used to store information passed down to the RouterOS host */
   loginS = mikro_String_create(&loginCmdFS,
                                &cd);
   if (cd)
      goto MIKRO_CLEANUP;
   loginW = mikro_Word_encode(loginS,
                              INV_PNT,
                              mikro_wt_command,
                              &cd);
   if (cd)
      goto MIKRO_CLEANUP;
   // an array of CLI options that refere to attributes, API attributes and queries 
   arrItems = calloc(argc - 1, sizeof(mikro_Cli_item_t*));
   if (!arrItems) {
      mikro_Log_append(stdout, __LINE__ - 2, "progErr", mikro_messages[mikro_ind_heapFail]);
      exit(EXIT_FAILURE);
   }
   enum mikro_keyList prevKInd = mikro_numKeys; // key indicator of the previously extracted input argument
   /* retrieving the CLI arguments */
   ITER_SGN_I(1, argc) {
      enum mikro_keyList kInd = mikro_numKeys;
      printf("[%d] argv=%s\n", i, (char*) argv[i]);
      mikro_FastString_t concatArg = {.len = strlen(argv[i]),
                                      .pChArr = argv[i]};
      mikro_Cli_item_t* pItem = mikro_Cli_parse_arg(&concatArg,
                                                    &cd);
      if (!pItem)
         return EXIT_FAILURE;
      kInd = mikro_chkKey(mikro_String_getPChArr(mikro_Cli_getArgName(pItem)));
      /* is the invocation a help request? */
      if (kInd == mikro_key_help) {
         mikro_help();
         goto MIKRO_CLEANUP;
      }
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
         itemCnt ++;
      }
      prevKInd = kInd;
   }
   /* creating the various parameters */
   arrAttrWords = calloc(attrsCnt, sizeof(mikro_Word_t*));
   if (!arrAttrWords) {
      mikro_Log_append(stdout, __LINE__ - 2, "progErr", mikro_messages[mikro_ind_heapFail]);
      exit(EXIT_FAILURE);
   }
   /* keeping track of a name-value pair */
   /* ---------- */
   mikro_Cli_item_t* pName = INV_PNT;
   mikro_Cli_item_t* pVal = INV_PNT;
   /* ---------- */
   unsigned attrWCnt = 0; // counter for attribute words
   ITER_UNS_I(1, argc) {
      mikro_Cli_item_t* pItem = *(arrItems + i - 1);
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
         commW = mikro_Word_encode(mikro_Cli_getArgValue(pItem),
                                   INV_PNT,
                                   mikro_wt_command,
                                   &cd);
         if (cd)
            exit(EXIT_FAILURE);
      }
      else if (kInd == mikro_key_aDir)
         aDir = (mikro_String_t*) IDEM_INT(mikro_Cli_getArgValue(pItem));
      else if (kInd == mikro_key_keyF)
         keyF = (mikro_String_t*) IDEM_INT(mikro_Cli_getArgValue(pItem));
      else if (kInd == mikro_key_crtF)
         crtF = (mikro_String_t*) IDEM_INT(mikro_Cli_getArgValue(pItem));
      else if (kInd == mikro_key_host)
         host = mikro_Cli_getArgValue(pItem);
      else if (kInd == mikro_key_port)
         port = mikro_Cli_getArgValue(pItem);
      else if (kInd == mikro_key_name ||
               kInd == mikro_key_pass) {
         mikro_Word_t* tmpW = mikro_Word_encode(mikro_Cli_getArgName(pItem),
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
      attrWCnt ++;
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
   size_t readB = mikro_readSeq(sDesc,
                                &pBuf,
                                MIKRO_R_TOUT,
                                &cd);
   viewBytes(readB, pBuf);
   mikro_Word_decode(readB, pBuf,
                     &replyWType);
   freeSeq(&pBuf); // will be reused for the command reply
   if (replyWType == mikro_rt_done) {
      // the attribute order follows the same one of the command line arguments
      /* dispatching the command sentence */
      phy_Sck_write(sDesc,
                    mikro_Word_getSz(commW), (void*) mikro_Word_getPBuf(commW),
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
   if (loginS)
      mikro_String_destroy(&loginS);
   if (loginW)
      mikro_Word_destroy(&loginW);
   if (commW)
      mikro_Word_destroy(&commW);
   /*if (aDir)
      mikro_String_destroy(&aDir);
   if (keyF)
      mikro_String_destroy(&keyF);
   if (crtF)
      mikro_String_destroy(&crtF);*/
   if (userW)
      mikro_Word_destroy(&userW);
   if (passW)
      mikro_Word_destroy(&passW);
   if (arrAttrWords) {
      ITER_UNS_I(0, attrsCnt)
         mikro_Word_destroy(arrAttrWords + i);
      free(arrAttrWords);
   }
   if (arrItems) {
      ITER_UNS_I(0, itemCnt)
         mikro_Cli_destroy_item(arrItems + i);
      free(arrItems);
   }
   if (pBuf)
      freeSeq(&pBuf);
   return EXIT_SUCCESS;
}
