/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 06-09-2026 */
/**************************************/

#ifndef MIKRO_VALIDATION_H
#define MIKRO_VALIDATION_H

/* DECODING */
/* -------------------- */
/* a set of functions that test mikro_Word_decodeSz. I've allocated two function */
/* for each value of the supported number of bytes in order to test separately   */
/* the decoded value and the decoded length                                      */
void mikro_decode_value_from_one_byte(void);
void mikro_decode_length_from_one_byte(void);

void mikro_decode_value_from_two_bytes(void);
void mikro_decode_length_from_two_bytes(void);

void mikro_decode_value_from_three_bytes(void);
void mikro_decode_length_from_three_bytes(void);

void mikro_decode_value_from_four_bytes(void);
void mikro_decode_length_from_four_bytes(void);

void mikro_decode_value_from_five_bytes(void);
void mikro_decode_length_from_five_bytes(void);
/* -------------------- */

/* ENCODING */
/* -------------------- */
/* a set of functions that test mikro_Word_encodeSz                              */
void mikro_encode_value_into_one_byte(void);

void mikro_encode_value_into_two_bytes(void);

void mikro_encode_value_into_three_bytes(void);

void mikro_encode_value_into_four_bytes(void);

void mikro_encode_value_into_five_bytes(void);
/* -------------------- */

#endif // MIKRO_VALIDATION_H