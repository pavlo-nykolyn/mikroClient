/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 06-09-2026 */
/**************************************/

#include <assert.h>
#include "mikro_word.h"
#include "mikro_logging.h"
#include "generic.h"

// indicators (the most significant bits of the most significant byte) that determine the amount of bytes that encode a word length
#define MIKRO_TEST_LEN_IND_2 (NIB_H_8) // two bytes
#define MIKRO_TEST_LEN_IND_3 (NIB_H_C) // three bytes
#define MIKRO_TEST_LEN_IND_4 (NIB_H_E) // four bytes
#define MIKRO_TEST_LEN_IND_5 (NIB_H_F) // five bytes

/* DECODING */
/* -------------------- */
void mikro_decode_value_from_one_byte(void)
{
   static const by_t encSeq[1] = {((NIB_H_7) | (NIB_L_F))};
   
   size_t val = mikro_Word_decodeSz(encSeq,
                                    INV_PNT);
   assert(val == 127);
   mikro_Log_append_withSz(stdout, __LINE__, "TEST", "[ONE BYTE VALUE] PASSED", val);
}

void mikro_decode_length_from_one_byte(void)
{
   static const by_t encSeq[1] = {((NIB_H_7) | (NIB_L_F))};
   unsigned len = 0;

   const size_t val = mikro_Word_decodeSz(encSeq,
                                          &len);
   assert(len == 1);
   mikro_Log_append_withUns(stdout, __LINE__, "TEST", "[LENGTH] PASSED", len);
}

void mikro_decode_value_from_two_bytes(void)
{
   static const by_t encSeq[2] = {[0] = ((NIB_H_3) | (NIB_L_E)) | MIKRO_TEST_LEN_IND_2,
                                  [1] = ((NIB_H_8))};

   size_t val = mikro_Word_decodeSz(encSeq,
                                    INV_PNT);
   assert(val == 16000);
   mikro_Log_append_withSz(stdout, __LINE__, "TEST", "[TWO BYTES VALUE] PASSED", val);
}

void mikro_decode_length_from_two_bytes(void)
{
   static const by_t encSeq[2] = {[0] = ((NIB_H_3) | (NIB_L_E)) | MIKRO_TEST_LEN_IND_2,
                                  [1] = ((NIB_H_8))};
   unsigned len = 0;

   const size_t val = mikro_Word_decodeSz(encSeq,
                                          &len);
   assert(len == 2);
   mikro_Log_append_withUns(stdout, __LINE__, "TEST", "[LENGTH] PASSED", len);
}

void mikro_decode_value_from_three_bytes(void)
{
   static const by_t encSeq[3] = {[0] = MIKRO_TEST_LEN_IND_3,
                                  [1] = (NIB_H_4),
                                  [2] = (NIB_L_1)};

   size_t val = mikro_Word_decodeSz(encSeq,
                                    INV_PNT);
   assert(val == 16385);
   mikro_Log_append_withSz(stdout, __LINE__, "TEST", "[THREE BYTES VALUE] PASSED", val);
}

void mikro_decode_length_from_three_bytes(void)
{
   static const by_t encSeq[3] = {[0] = MIKRO_TEST_LEN_IND_3,
                                  [1] = (NIB_H_4),
                                  [2] = (NIB_L_1)};
   unsigned len = 0;

   const size_t val = mikro_Word_decodeSz(encSeq,
                                          &len);
   assert(len == 3);
   mikro_Log_append_withUns(stdout, __LINE__, "TEST", "[LENGTH] PASSED", len);
}

void mikro_decode_value_from_four_bytes(void)
{
   static const by_t encSeq[4] = {[0] = MIKRO_TEST_LEN_IND_4,
                                  [1] = ((NIB_H_1) | (NIB_L_F)),
                                  [2] = (LOW),
                                  [3] = ((NIB_H_E) | (NIB_L_8))};

   size_t val = mikro_Word_decodeSz(encSeq,
                                    INV_PNT);
   assert(val == 2031848);
   mikro_Log_append_withSz(stdout, __LINE__, "TEST", "[FOUR BYTES VALUE] PASSED", val);
}

void mikro_decode_length_from_four_bytes(void)
{
   static const by_t encSeq[4] = {[0] = MIKRO_TEST_LEN_IND_4,
                                  [1] = ((NIB_H_1) | (NIB_L_F)),
                                  [2] = (LOW),
                                  [3] = ((NIB_H_E) | (NIB_L_8))};
   unsigned len = 0;

   size_t val = mikro_Word_decodeSz(encSeq,
                                    &len);
   assert(len == 4);
   mikro_Log_append_withUns(stdout, __LINE__, "TEST", "[LENGTH] PASSED", len);
}

void mikro_decode_value_from_five_bytes(void)
{
   static const by_t encSeq[5] = {[0] = MIKRO_TEST_LEN_IND_5,
                                  [1] = ((NIB_H_1) | (NIB_L_F)),
                                  [2] = ((NIB_H_7) | (NIB_L_4)),
                                  [3] = (LOW),
                                  [4] = ((NIB_H_E) | (NIB_L_8))};

   size_t val = mikro_Word_decodeSz(encSeq,
                                    INV_PNT);
   assert(val == 527696104);
   mikro_Log_append_withSz(stdout, __LINE__, "TEST", "[FIVE BYTES VALUE] PASSED", val);
}

void mikro_decode_length_from_five_bytes(void)
{
   static const by_t encSeq[5] = {[0] = MIKRO_TEST_LEN_IND_5,
                                  [1] = ((NIB_H_1) | (NIB_L_F)),
                                  [2] = ((NIB_H_7) | (NIB_L_4)),
                                  [3] = (LOW),
                                  [4] = ((NIB_H_E) | (NIB_L_8))};
   unsigned len = 0;

   size_t val = mikro_Word_decodeSz(encSeq,
                                    &len);
   assert(len == 4);
   mikro_Log_append_withUns(stdout, __LINE__, "TEST", "[LENGTH] PASSED", len);
}
/* -------------------- */

/* ENCODING */
// I did not define functions that test the length as the encoding procedures already
// produces a value for the number of encoded bytes
/* -------------------- */
/*
void mikro_encode_value_into_one_byte(void)
{
   static const size_t val = 127;
   by_t encSeq[1] = {LOW};

   const unsigned len = mikro_Word_encodeSz(val,
                                            encSeq);

   assert(encSeq[0] == ((NIB_H_7) | (NIB_L_F)));
   mikro_Log_append_withSz(stdout, __LINE__, "TEST", "[ONE BYTE ENCODING] SUCCESSFUL", val);
   assert(len == 1)
   mikro_Log_append_withUns(stdout, __LINE__, "TEST", "[LENGTH] ENCODED BYTES", len);
}

void mikro_encode_value_into_two_bytes(void)
{
   static const size_t val = 16000;
   by_t encSeq[2] = {LOW};

   const unsigned len = mikro_Word_encodeSz(val,
                                            encSeq);

   assert((encSeq[0] == ((NIB_H_3) | (NIB_L_E)) | MIKRO_TEST_LEN_IND_2) &&
          (encSeq[1] == (NIB_H_8)));
   mikro_Log_append_withSz(stdout, __LINE__, "TEST", "[TWO BYTES ENCODING] SUCCESSFUL", val);
   assert(len == 2)
   mikro_Log_append_withUns(stdout, __LINE__, "TEST", "[LENGTH] ENCODED BYTES", len);
}

void mikro_encode_value_into_three_bytes(void)
{
   static const size_t val = 16385;
   by_t encSeq[3] = {LOW};

   const unsigned len = mikro_Word_encodeSz(val,
                                            encSeq);

   assert((encSeq[0] == MIKRO_TEST_LEN_IND_3) &&
          (encSeq[1] == (NIB_H_4)) &&
          (encSeq[2] == (NIB_L_1)));
   mikro_Log_append_withSz(stdout, __LINE__, "TEST", "[THREE BYTES ENCODING] SUCCESSFUL", val);
   assert(len == 3)
   mikro_Log_append_withUns(stdout, __LINE__, "TEST", "[LENGTH] ENCODED BYTES", len);
}

void mikro_encode_value_into_four_bytes(void)
{
   static const size_t val = 2031848;
   by_t encSeq[4] = {LOW};

   const unsigned len = mikro_Word_encodeSz(val,
                                            encSeq);

   assert((encSeq[0] == MIKRO_TEST_LEN_IND_4) &&
          (encSeq[1] == ((NIB_H_1) | (NIB_L_F))) &&
          (encSeq[2] == (LOW)) &&
          (encSeq[3] == ((NIB_H_E) | (NIB_L_8))));
   mikro_Log_append_withSz(stdout, __LINE__, "TEST", "[FOUR BYTES ENCODING] SUCCESSFUL", val);
   assert(len == 4)
   mikro_Log_append_withUns(stdout, __LINE__, "TEST", "[LENGTH] ENCODED BYTES", len);
}

void mikro_encode_value_into_five_bytes(void)
{
   static const size_t val = 527696104;
   by_t encSeq[5] = {LOW};

   const unsigned len = mikro_Word_encodeSz(val,
                                            encSeq);

   assert((encSeq[0] == MIKRO_TEST_LEN_IND_5) &&
          (encSeq[1] == ((NIB_H_1) | (NIB_L_F))) &&
          (encSeq[2] == ((NIB_H_7) | (NIB_L_4))) &&
          (encSeq[3] == (LOW)) &&
          (encSeq[4] == ((NIB_H_E) | (NIB_L_8))));
   mikro_Log_append_withSz(stdout, __LINE__, "TEST", "[FIVE BYTES ENCODING] SUCCESSFUL", val);
   assert(len == 5)
   mikro_Log_append_withUns(stdout, __LINE__, "TEST", "[LENGTH] ENCODED BYTES", len);
}
*/
/* -------------------- */