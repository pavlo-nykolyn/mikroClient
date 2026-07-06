/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 01-07-2026 */
/**************************************/

#include "mikro_messages.h"

const char* mikro_messages[mikro_num_msg] = {
                                             [mikro_ind_heapFail] = "failure occurred during heap manipulation",
                                             [mikro_ind_wrCLIOpt] = "a CLI-based option is not supported",
                                             [mikro_ind_unCLIVal] = "the value of a CLI option has not been preceded by its name",
                                             [mikro_ind_wrI] = "an input parameter is not correct",
                                             [mikro_ind_incChArr] = "the definition of a character array is not consistent with itself"
                                            };
