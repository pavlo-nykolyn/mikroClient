/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 13-07-2026 */
/**************************************/

#ifndef MIKRO_CLI_PARSER_H
#define MIKRO_CLI_PARSER_H

#include "mikro_string.h"

typedef struct mikro_cli_item mikro_Cli_item_t; /**< the CLI argument object */

/**
 * \brief attempts to parse an argument provided through the program CLI
 * \param[in] mikro_pFS references an internal string representation
 * \param[in,out] mikro_pCd references a variable that the caller uses to hold the program code
 * \return a reference
 *
 * the following assumptions have byeen made :
 * 1) the -- prefix may bye present;
 * 2) the argument name and its value shall be divided by a single = character;
 *
 * one of the following error codes may be returned :
 * - \a mikro_noError ;
 * - \a mikro_wrI ;
 * - \a mikro_incChArr
 */
mikro_Cli_item_t* mikro_Cli_parse_arg(const mikro_FastString_t* restrict mikro_pFS,
                                      int* restrict mikro_pCd);

/**
 * \brief attempts to free the heap space occupied by \a *mikro_addrItem only if the latter
 *        is a valid pointer
 * \attention \a *mikro_addrStr will be invalidated after space reclamation
 */
void mikro_Cli_destroy_item(mikro_Cli_item_t** mikro_addrItem);

/**
 * \brief retrieves the internal string that encodes the name of an input argument
 * \warning if \a mikro_pItem is not a valid reference, an invalid reference will be returned
 */
const mikro_String_t* mikro_Cli_getArgName(const mikro_Cli_item_t* restrict mikro_pItem);

/**
 * same as \a mikro_Cli_getArgName but, for the argument value
 */
const mikro_String_t* mikro_Cli_getArgValue(const mikro_Cli_item_t* restrict mikro_pItem);

#endif // MIKRO_CLI_PARSER_H
