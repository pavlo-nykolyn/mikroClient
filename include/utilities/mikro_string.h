/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 13-07-2026 */
/**************************************/

/**
 * \file
 *
 * provides an interface for a subscriber in order to define and use C-like strings without
 *
 */

#ifndef MIKRO_STRING_H
#define MIKRO_STRING_H

#include <string.h>
#include "header_wrapper.h"

typedef struct mikro_string mikro_String_t;

// this type leaves the definition of the character array length to a client
typedef struct mikro_FastString {
   const size_t len; // the null character shall not be included (ensured by strlen)
   const char* pChArr;
} mikro_FastString_t;

/**
 * \brief attempts to define a \p mikro_String_t object
 * \param[in] mikro_pFStr references an interface of the internal string object
 * \param[in,out] mikro_pCode references a variable used by the caller to hold the program code
 * \return a reference
 *
 *
 * one of the following error codes may be returned :
 * - \a mikro_noError ;
 * - \a mikro_wrI ;
 * - \a mikro_incChArr ;
 */
mikro_String_t* mikro_String_create(const mikro_FastString_t* const mikro_pFStr,
                                    int* restrict mikro_pCode);

/**
 * \brief attempts to free the heap space occupied by \a *mikro_addrStr only if the latter
 *        is a valid pointer
 * \attention \a *mikro_addrStr will be invalidated after space reclamation
 */
void mikro_String_destroy(mikro_String_t** mikro_addrStr);

/**
 * \brief retrieves the length of the character array (the null character is not included)
 * \warning if \a mikro_pStr is an invalid reference, zero will be returned
 */
const size_t mikro_String_getLen(const mikro_String_t* restrict mikro_pStr);

/**
 * \brief retrieves the reference to the character array
 * \warning if \a mikro_pStr is an invalid reference, an invalid reference will be returned
 */
const char* mikro_String_getPChArr(const mikro_String_t* restrict mikro_pStr);

#endif // MIKRO_STRING_H
