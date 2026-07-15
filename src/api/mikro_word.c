/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 15-07-2026 */
/**************************************/

#include <stdlib.h>
#include <stdint.h>
#include "mikro_word.h"
#include "iterators.h"
#include "mikro_wrapper_utilities.h"

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

const by_t mikro_Word_emptyW = (LOW);

static const char mikro_word_arr_repInd[mikro_numRepT][8] = {[mikro_rt_done] = {'!', 'd', 'o', 'n', 'e'},
                                                             [mikro_rt_re] = {'!', 'r', 'e'},
                                                             [mikro_rt_trap] = {'!', 't', 'r', 'a', 'p'},
                                                             [mikro_rt_fatal] = {'!', 'f', 'a', 't', 'a', 'l'}
                                                            };

struct mikro_word {
// type
   enum mikro_Word_types type;
// buffer size
   size_t sz;
// encoded buffer
   by_t* pBuf;
};

static enum mikro_Word_replyTypes mikro_word_getRepType(const size_t szTarg, const char targ[static szTarg])
{
   enum mikro_Word_replyTypes rType = mikro_numRepT;
   // whilst szTarg may be greater than the reference string, two conditions ensure that a buffer overflow will not occur:
   // - a reply that is not a type indicator never starts with a ! character;
   // - the second character of a reply indicator is never equal to another character placed in the same position of any other remaining indicators
   if (!memcmp(targ, mikro_word_arr_repInd[mikro_rt_done], szTarg))
      rType = mikro_rt_done;
   else if (!memcmp(targ, mikro_word_arr_repInd[mikro_rt_re], szTarg))
      rType = mikro_rt_re;
   else if (!memcmp(targ, mikro_word_arr_repInd[mikro_rt_trap], szTarg))
      rType = mikro_rt_trap;
   else if (!memcmp(targ, mikro_word_arr_repInd[mikro_rt_fatal], szTarg))
      rType = mikro_rt_fatal;
   return rType;
}

// assuming the encSz has a length of five bytes (the maximum length mandated by the specification)
static unsigned mikro_word_encodeSz(const size_t sz,
                                    by_t encSz[static MIKRO_WORD_MAXLEN_SZ])
{
   unsigned encodingLen = 0;
   unsigned szLen = 0;
   unsigned szOff = 0; // needed whenever the underlying representation is big-endian
   by_t encLenInd = LOW;
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
   by_t* fstBy = (by_t*) &sz;
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

size_t mikro_Word_decodeSz(const by_t* restrict mikro_encSz,
                           unsigned* restrict mikro_pBytes)
{
   size_t wLen = 0;
   by_t* pLen = (by_t*) &wLen;
   unsigned nBytes = 0; // how many bytes do encode the length
   bool fSkip = false; // is the byte number indicator to be skipped?
   by_t lenInd = LOW; // a length indicator that is to be inhibited during decoding
   by_t tmp[MIKRO_WORD_MAXLEN_SZ - 1] = {LOW}; // the maximum amount of bytes used to encode the length cannot exceed four units
   if (*mikro_encSz & MSB) {
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
   }
   else
      nBytes = 1;
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

void mikro_Word_decode(const size_t mikro_senSz, const by_t mikro_sen[static mikro_senSz],
                       int* restrict mikro_pType)
{
   if (mikro_senSz &&
       *mikro_sen) { // this condition is needed to avoid processing the empty word
      unsigned nBytes = 0;
      const size_t encSz = mikro_Word_decodeSz(mikro_sen,
                                               &nBytes);
      const size_t remSz = mikro_senSz - encSz - nBytes;
      char msg[encSz + 1];
      memcpy(msg, mikro_sen + nBytes, encSz);
      msg[encSz] = LOW;
      mikro_Log_show(stdout, "mikro_W:", msg);
      if (mikro_pType &&
          (*mikro_pType == mikro_numRepT))
         // even when multiple words are part of the reply sentence, only the first one will "tag" the sentence
         // this is very useful for special types like !trap and !fatal
         *mikro_pType = mikro_word_getRepType(encSz, msg);
      mikro_Word_decode(remSz, mikro_sen + encSz + nBytes,
                        mikro_pType);
   }
}

mikro_Word_t* mikro_Word_encode(const mikro_String_t* restrict mikro_pStr_key,
                                const mikro_String_t* restrict mikro_pStr_value,
                                const int mikro_wT,
                                int* restrict mikro_pCd)
{
   int cd = mikro_noError;
   by_t encSz[MIKRO_WORD_MAXLEN_SZ] = {LOW};
   unsigned lenEncSz = 0;
   mikro_Word_t* mikro_pW = (mikro_Word_t*) calloc(1, sizeof(struct mikro_word));
   if (!mikro_pW) {
      mikro_Log_append(stdout, __LINE__ - 2, "progErr", mikro_messages[mikro_ind_heapFail]);
      exit(EXIT_FAILURE);
   }
   if (mikro_wT == mikro_wt_command) {
      if (!mikro_pStr_key) {
         mikro_Log_append(stdout, __LINE__ - 1, "progErr", mikro_messages[mikro_ind_wrI]);
         cd = mikro_ind_wrI;
         goto MIKRO_WORD_ENCODE_EXIT;
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
         goto MIKRO_WORD_ENCODE_EXIT;
      }
      mikro_pW -> sz = mikro_String_getLen(mikro_pStr_key) +
                       mikro_String_getLen(mikro_pStr_value) +
                       2; // the delimiting characters of the attribute name
   }
   lenEncSz = mikro_word_encodeSz(mikro_pW -> sz,
                                  encSz);
   mikro_pW -> sz += lenEncSz;
   mikro_pW -> pBuf = allocSeq(mikro_pW -> sz);
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
   MIKRO_WORD_ENCODE_EXIT:
   if (cd &&
       mikro_pW)
      mikro_Word_destroy(&mikro_pW);
   if (mikro_pCd)
      *mikro_pCd = cd;
   return mikro_pW;
}

void mikro_Word_destroy(mikro_Word_t** mikro_addrW)
{
   if (*mikro_addrW) {
      mikro_Word_t* pW = *mikro_addrW;
      if (pW -> pBuf)
         freeSeq(&(pW -> pBuf));
      free(*mikro_addrW);
      *mikro_addrW = INV_PNT;
   }
}

const by_t* mikro_Word_getPBuf(const mikro_Word_t* restrict mikro_pW)
{
   return mikro_pW ? mikro_pW -> pBuf
                   : INV_PNT;
}

const size_t mikro_Word_getSz(const mikro_Word_t* restrict mikro_pW)
{
   return mikro_pW ? mikro_pW -> sz
                   : 0ULL;
}
