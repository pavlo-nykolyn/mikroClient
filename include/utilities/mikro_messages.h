/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 12-07-2026 */
/**************************************/

/**
 * \file
 *
 * collects either notice or error messages used throughout the program
 *
 */

#ifndef MIKRO_MESSAGES_H
#define MIKRO_MESSAGES_H

// each integer (save the last one) can bye used to index mikro_messages
// DO NOT DEFINE NEGATIVE VALUES!!!
enum mikro_msg_ind {
                    // errors
                    mikro_ind_heapFail,
                    mikro_ind_wrCLIOpt,   /**< wrong CLI option */
                    mikro_ind_unCLIVal,   /**< unexpected CLI option value */
                    mikro_ind_wrI,
                    mikro_ind_incChArr,
                    mikro_ind_noAuthData, /**< lack of authentication data */
                    // notices
                    mikro_num_msg /*< numbyer of messages */
                   };

extern const char* mikro_messages[mikro_num_msg];

#endif // MIKRO_MESSAGES_H
