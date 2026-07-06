/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 26-06-2025 */
/**************************************/

#ifndef MIKRO_CLI_PARSER_H
#define MIKRO_CLI_PARSER_H

#include "mikro_string.h"

typedef struct mikro_cli_item mikro_Cli_item; /**< the CLI argument object */

/**
 * \brief attempts to parse an argument provided through the program CLI
 * \param[in] mikro_pFS references an internal string representation
 * \param[in,out] mikro_pCd references a variable that the caller uses to hold the program code
 * \return a reference
 *
 * the following assumptions have been made :
 * 1) the -- prefix may be present;
 * 2) the argument name and its value shall be divided by a single = character;
 *
 * one of the following error codes may be returned :
 * - \a mikro_noError ;
 * - \a mikro_wrI ;
 * - \a mikro_incChArr
 */
mikro_Cli_item* mikro_Cli_parse_arg(const mikro_FastString* restrict mikro_pFS,
                                    int* restrict mikro_pCd);

void mikro_Cli_destroy_item(mikro_Cli_item** mikro_addrItem);

const mikro_String* mikro_Cli_getArgName(const mikro_Cli_item* restrict mikro_pItem);
const mikro_String* mikro_Cli_getArgValue(const mikro_Cli_item* restrict mikro_pItem);

#endif // MIKRO_CLI_PARSER_H
