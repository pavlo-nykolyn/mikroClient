/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 03-07-2026 */
/**************************************/

#include <stdlib.h>
#include <stdint.h>
#include "mikro_word.h"
#include "mikro_wrapper_utilities.h"
#include "iterators.h"
#include "generic.h"

// indicators (the most significant bits of the most significant byte) that determine the amount of bytes that encode a word length
#define MIKRO_WORD_LEN_IND_2 (NIB_H_8) // two bytes
#define MIKRO_WORD_LEN_IND_3 (NIB_H_C) // three bytes
#define MIKRO_WORD_LEN_IND_4 (NIB_H_E) // four bytes
#define MIKRO_WORD_LEN_IND_5 (NIB_H_F) // five bytes
// maximum size of the encoded length
#define MIKRO_WORD_MAXLEN_SZ 5U // maximum length of the encoded size
// some special characters
#define MIKRO_WORD_SP    ((NIB_H_2)            )
#define MIKRO_WORD_SLASH ((NIB_H_2) | (NIB_L_F))
#define MIKRO_WORD_EQ    ((NIB_H_3) | (NIB_L_D))

const by mikro_Word_emptyW = (LOW);

struct mikro_word {
// type
   enum mikro_word_types type;
// buffer size
   size_t sz;
// encoded buffer
   by* pBuf;
// decoded string
   mikro_String* pDecStr;
// value offset
   size_t valOff;
};

static by* mikro_word_alloc(const size_t sz)
{
   by* pBuf = calloc(sz, sizeof(by));
   if (!pBuf) {
      mikro_Log_append(stdout, __LINE__ - 2, "progErr", mikro_messages[mikro_ind_heapFail]);
      exit(EXIT_FAILURE);
   }
   return pBuf;
}

// assuming the encSz has a length of five bytes (the maximum length mandated by the specification)
static unsigned mikro_word_encodeSz(const size_t sz,
                                    by encSz[static MIKRO_WORD_MAXLEN_SZ])
{
   unsigned encodingLen = 0;
   unsigned szLen = 0;
   unsigned szOff = 0; // needed whenever the underlying representation is big-endian
   by encLenInd = LOW;
   if (sz <= 127) {
      encodingLen = 1;
      szOff = isBE() ? 7 : 0;
   }
   else if (sz >= 128 && sz <= 16383) {
      encodingLen = 2;
      szOff = isBE() ? 6 : 1;
      encLenInd = MIKRO_WORD_LEN_IND_2;
   }
   else if (sz >= 16384 && sz <= 2097151) {
      encodingLen = 3;
      szOff = isBE() ? 5 : 2;
      encLenInd = MIKRO_WORD_LEN_IND_3;
   }
   else if (sz >= 2097152 && sz <= 268435455) {
      encodingLen = 4;
      szOff = isBE() ? 4 : 3;
      encLenInd = MIKRO_WORD_LEN_IND_4;
   }
   else if (sz >= 268435456 && sz <= UINT32_MAX) {
      encodingLen = 5;
      szOff = isBE() ? 4 : 3;
      encLenInd = MIKRO_WORD_LEN_IND_5;
   }
   by* fstBy = (by*) &sz;
   if (szOff &&
       szOff < 4) {
      memcpy(encSz + 1, fstBy, szOff + 1);
      reverse(MIKRO_WORD_MAXLEN_SZ, encSz);
   }
   else if (!szOff)
      encSz[MIKRO_WORD_MAXLEN_SZ - encodingLen] = IDEM_INT(*fstBy); // a single byte encodes the little-endian representation of the size
   else
      memcpy(encSz + MIKRO_WORD_MAXLEN_SZ - encodingLen, fstBy + szOff, 8 - szOff);
   encSz[MIKRO_WORD_MAXLEN_SZ - encodingLen] |= encLenInd;
   return encodingLen;
}

size_t mikro_Word_decodeSz(const by* restrict mikro_encSz,
                           unsigned* restrict mikro_pBytes)
{
   size_t wLen = 0;
   by* pLen = (by*) &wLen;
   unsigned nBytes = 0; // how many bytes do encode the length
   unsigned lOff = 0; // offset within the context of the byte sequence of wLen
                      // whenever the offset is defined, I'm making the assumption
                      // the the amount of bytes used to define any value of type
                      // size_t is eight
   bool fSkip = false; // is the byte number indicator to be skipped?
   by lenInd = LOW; // a length indicator that is to be inhibited during decoding
   by tmp[MIKRO_WORD_MAXLEN_SZ - 1] = {LOW}; // the maximum amount of bytes used to encode the length cannot exceed four units
   if (!((*mikro_encSz ^ (MIKRO_WORD_LEN_IND_2)) & (MIKRO_WORD_LEN_IND_2))) {
      nBytes = 2;
      lenInd = MIKRO_WORD_LEN_IND_2;
   }
   else if (!((*mikro_encSz ^ (MIKRO_WORD_LEN_IND_3)) & (MIKRO_WORD_LEN_IND_3))) {
      nBytes = 3;
      lenInd = MIKRO_WORD_LEN_IND_3;
   }
   else if (!((*mikro_encSz ^ (MIKRO_WORD_LEN_IND_4)) & (MIKRO_WORD_LEN_IND_4))) {
      nBytes = 4;
      lenInd = MIKRO_WORD_LEN_IND_4;
   }
   else if (!((*mikro_encSz ^ (MIKRO_WORD_LEN_IND_5)) & (MIKRO_WORD_LEN_IND_5))) {
      nBytes = 4;
      fSkip = true;
   }
   else {
      nBytes = 1;
   }
   const size_t idx = sizeof(size_t) - nBytes;
   memcpy(pLen + idx, mikro_encSz + (fSkip ? 1 : 0), nBytes);
   if (lenInd)
      *(pLen + idx) &= ~lenInd;
   if (!isBE())
      reverse(sizeof(size_t), pLen);
   if (mikro_pBytes)
      *mikro_pBytes = nBytes;
   return wLen;
}

mikro_Word* mikro_Word_encode(const mikro_String* restrict mikro_pStr_key,
                              const mikro_String* restrict mikro_pStr_value,
                              const int mikro_wT,
                              int* restrict mikro_pCd)
{
   int cd = mikro_noError;
   by encSz[MIKRO_WORD_MAXLEN_SZ] = {LOW};
   unsigned lenEncSz = 0;
   mikro_Word* mikro_pW = calloc(1, sizeof(struct mikro_word));
   if (!mikro_pW) {
      mikro_Log_append(stdout, __LINE__ - 2, "progErr", mikro_messages[mikro_ind_heapFail]);
      exit(EXIT_FAILURE);
   }
   if (mikro_wT == mikro_wt_command) {
      if (!mikro_pStr_key) {
         mikro_Log_append(stdout, __LINE__ - 1, "progErr", mikro_messages[mikro_ind_wrI]);
         cd = mikro_ind_wrI;
         goto MIKRO_WORD_CREATE_EXIT;
      }
      mikro_pW -> sz = mikro_String_getLen(mikro_pStr_key);
   }
   else if (mikro_wT == mikro_wt_attribute ||
            mikro_wT == mikro_wt_apiAttribute ||
            mikro_wT == mikro_wt_query) {
      // both the value and the key are to be valid
      if (!mikro_pStr_key) {
         mikro_Log_append(stdout, __LINE__ - 2, "progErr", mikro_messages[mikro_ind_wrI]);
         cd = mikro_ind_wrI;
         goto MIKRO_WORD_CREATE_EXIT;
      }
      mikro_pW -> sz = mikro_String_getLen(mikro_pStr_key) +
                       mikro_String_getLen(mikro_pStr_value) +
                       2; // the delimiting characters of the attribute name
   }
   lenEncSz = mikro_word_encodeSz(mikro_pW -> sz,
                                  encSz);
   mikro_pW -> sz += lenEncSz;
   mikro_pW -> pBuf = calloc(mikro_pW -> sz, sizeof(by));
   if (!(mikro_pW -> pBuf)) {
      mikro_Log_append(stdout, __LINE__ - 1, "progErr", mikro_messages[mikro_ind_heapFail]);
      exit(EXIT_FAILURE);
   }
   memcpy(mikro_pW -> pBuf, encSz + MIKRO_WORD_MAXLEN_SZ - lenEncSz, lenEncSz);
   if (mikro_wT == mikro_wt_command) {
      memcpy(mikro_pW -> pBuf + lenEncSz, mikro_String_getPChArr(mikro_pStr_key), mikro_String_getLen(mikro_pStr_key));
      ITER_UNS_I(lenEncSz, mikro_pW -> sz) {
         // slashes shall delimit command words
         if (*((mikro_pW -> pBuf) + i) == MIKRO_WORD_SP)
            *((mikro_pW -> pBuf) + i) = MIKRO_WORD_SLASH;
      }
   }
   else if (mikro_wT == mikro_wt_attribute ||
            mikro_wT == mikro_wt_apiAttribute ||
            mikro_wT == mikro_wt_query) {
      *((mikro_pW -> pBuf) + lenEncSz) = MIKRO_WORD_EQ;
      const size_t szKey = mikro_String_getLen(mikro_pStr_key);
      memcpy((mikro_pW -> pBuf) + lenEncSz + 1, mikro_String_getPChArr(mikro_pStr_key), szKey);
      *((mikro_pW -> pBuf) + lenEncSz + szKey + 1) = MIKRO_WORD_EQ;
      const char* pChArr = mikro_String_getPChArr(mikro_pStr_value);
      if (pChArr)
         memcpy((mikro_pW -> pBuf) + lenEncSz + szKey + 2, pChArr, mikro_String_getLen(mikro_pStr_value));
   }
   viewBytes(mikro_pW -> sz, mikro_pW -> pBuf);
   MIKRO_WORD_CREATE_EXIT:
   if (mikro_pCd)
      *mikro_pCd = cd;
   return mikro_pW;
}

const by* mikro_Word_getPBuf(const mikro_Word* restrict mikro_pW)
{
   return mikro_pW ? mikro_pW -> pBuf
                   : INV_PNT;
}

const size_t mikro_Word_getSz(const mikro_Word* restrict mikro_pW)
{
   return mikro_pW ? mikro_pW -> sz
                   : 0ULL;
}
