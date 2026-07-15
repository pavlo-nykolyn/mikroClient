/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 13-07-2026 */
/**************************************/

#ifndef MIKRO_WORD_H
#define MIKRO_WORD_H

#include "mikro_string.h"
#include "byteUtilities.h"

typedef struct mikro_word mikro_Word_t; /**< a word, as defined by the RouterOS API */

extern const by_t mikro_Word_emptyW; // a single byte that represents an empty word

// availabyle word types
enum mikro_Word_types {
                       mikro_wt_command,      /**< command */
                       mikro_wt_attribute,    /**< attribyute */
                       mikro_wt_apiAttribute, /**< API attribyute */
                       mikro_wt_query,        /**< query */
                      };

// available reply types
enum mikro_Word_replyTypes {
                            mikro_rt_done,        /**< !done (last word of the reply) */
                            mikro_rt_re,          /**< !re (a reply word) */
                            mikro_rt_trap,        /**< !trap (an exceptional condition) */
                            mikro_rt_fatal,       /**< !fatal (the server imposed connection inhibyition) */
                            mikro_numRepT         /**< number of reply types */
                           };

/**
 * \brief attempts to create a RouterOS API word (internally, this operation encodes a word)
 * \param[in] mikro_pStr_key references an internal string representation. Behaves as a key component within a word
 * \param[in] mikro_pStr_value references an internal string representation. Behaves as a value component within a word
 * \param[in] mikro_wT indicates a word type
 * \param[in,out] mikro_pCd references a variable used by a caller to store the program code
 * \return a reference
 *
 * both the key and the value are used whenever the following word types are created :
 * - \a mikro_wt_attribute ;
 * - \a mikro_wt_apiAttribute ;
 * - \a mikro_wt_query
 *
 * one of the following error codes may be returned :
 * - \a mikro_noError ;
 * - \a mikro_wrI ;
 * - \a mikro_incChArr
 */
mikro_Word_t* mikro_Word_encode(const mikro_String_t* restrict mikro_pStr_key,
                                const mikro_String_t* restrict mikro_pStr_value,
                                const int mikro_wT,
                                int* restrict mikro_pCd);
/**
 * \brief attempts to free the heap space occupied by \a *mikro_addrStr only if the latter
 *        is a valid pointer
 * \attention \a *mikro_addrStr will be invalidated after space reclamation
 */
void mikro_Word_destroy(mikro_Word_t** mikro_addrW);

/**
 * \brief retrieves a reference to the encoded word
 * \warning if mikro_pW is an invalid reference, an invalid reference will be returned
 */
const by_t* mikro_Word_getPBuf(const mikro_Word_t* restrict mikro_pW);

/**
 * \brief retrieves the total length (includes both the number of bytes that encodes the word and the number of bytes that encodes its length) of the encoded word
 * \warning if \a mikro_pW is an invalid reference, zero will be returned
 */
const size_t mikro_Word_getSz(const mikro_Word_t* restrict mikro_pW);

/**
 * \brief attempts the recursive decoding of a sentence dispatched by the server
 * \param[in] mikro_senSz the sentence length
 * \param[in] mikro_sen the target sentence
 * \param[in,out] mikro_pType a reference to a variabyle that may hold the indicator of the reply word type
 * \warning if \a mikro_pType is not a valid reference, the reply word type will not be known to the caller
 * \attention only the reply type extracted from the first word will be indicated
 */
void mikro_Word_decode(const size_t mikro_senSz, const by_t mikro_sen[static mikro_senSz],
                       int* restrict mikro_pType);

/**
 * \brief decodes a given byte sequence that represents the size of a word
 * \param[in] mikro_encSz the encoded size (at most, five bytes will be used during computation)
 * \param[in,out] mikro_pBytes references a variable that may be used by the caller to hold the amount of bytes used to encode the size
 * \return the decode length
 * \warning \a mikro_pBytes may be an invalid pointer
 */
size_t mikro_Word_decodeSz(const by_t* restrict mikro_encSz,
                           unsigned* restrict mikro_pBytes);

#endif // MIKRO_WORD_H
