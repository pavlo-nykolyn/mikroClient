/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 26-06-2025 */
/**************************************/

#ifndef MIKRO_ERRORS_H
#define MIKRO_ERRORS_H

enum mikro_error_codes {
                        mikro_heapFail = -1, /*< heap manipulation failure */
                        mikro_noError,       /*< successful action */
                        mikro_wrI,           /*< wrong input parameter */
                        mikro_incChArr,      /*< either the size or the buffer of a character array is not consistent with the other parameter */
                       };

#endif // MIKRO_ERRORS_H
