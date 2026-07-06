/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 03-07-2026 */
/**************************************/

#ifndef MIKRO_WORD_H
#define MIKRO_WORD_H

#include "mikro_string.h"
#include "byteUtilities.h"

typedef struct mikro_word mikro_Word; /**< a word, as defined by the RouterOS API */

extern const by mikro_Word_emptyW; // a single byte that represents an empty word

// available word types
enum mikro_word_types {
                       mikro_wt_command,      /**< command */
                       mikro_wt_attribute,    /**< attribute */
                       mikro_wt_apiAttribute, /**< API attribute */
                       mikro_wt_query,        /**< query */
                       mikro_wt_reply         /**< reply */
                      };

/**
 * \brief attempts to create a RouterOS API word (internally, this operation encodes a word)
 * \param[in] mikro_pStr_key references an internal string representation. Behaves as a key component within a word
 * \param[in] mikro_pStr_value references an internal string representation. Behaves as a value component within a word
 * \param[in] mikro_wT indicates a word type
 * \param[in,out] mikro_pCd references a variable used by a caller to store the program code
 * \return a reference
 *
 * both the key and the value are used whenever the following word types are to be created:
 * - 
 *
 * one of the following error codes may be returned:
 * - \a mikro_noError ;
 * - \a mikro_wrI ;
 * - \a mikro_incChArr
 */
mikro_Word* mikro_Word_encode(const mikro_String* restrict mikro_pStr_key,
                              const mikro_String* restrict mikro_pStr_value,
                              const int mikro_wT,
                              int* restrict mikro_pCd);
void mikro_Word_destroy(mikro_Word** mikro_addrStr);

const by* mikro_Word_getPBuf(const mikro_Word* restrict mikro_pW);
const size_t mikro_Word_getSz(const mikro_Word* restrict mikro_pW);

/**
 * \brief decodes a given byte sequence that represents the size of a word
 * \param[in] mikro_encSz the encoded size (at most, five bytes will be used during computation)
 * \param[in,out] mikro_pBytes references a variable that may be used by the caller to hold the amount of bytes used to encode the size
 * \return the decode length
 * \warning \a mikro_pBytes may be an invalid pointer
 */
size_t mikro_Word_decodeSz(const by* restrict mikro_encSz,
                           unsigned* restrict mikro_pBytes);

#endif // MIKRO_WORD_H
