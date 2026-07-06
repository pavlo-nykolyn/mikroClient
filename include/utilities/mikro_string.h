/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 03-07-2026 */
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

typedef struct mikro_string mikro_String; 

// this type groups leaves the definition of the string length to the client
typedef struct mikro_FastString {
   const size_t len; // the null character shall not be included (ensured by strlen)
   const char* pChArr;
} mikro_FastString;

/**
 * \brief attempts the definition of \p mikro_String object
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
mikro_String* mikro_String_create(const mikro_FastString* const mikro_pFStr,
                                  int* restrict mikro_pCode);
void mikro_String_destroy(mikro_String** mikro_addrStr);

const size_t mikro_String_getLen(const mikro_String* restrict mikro_pStr);
const char* mikro_String_getPChArr(const mikro_String* restrict mikro_pStr);

#endif // MIKRO_STRING_H
