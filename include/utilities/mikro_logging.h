/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 06-07-2025 */
/**************************************/

#ifndef MIKRO_LOGGING_H
#define MIKRO_LOGGING_H

#include "header_wrapper.h"

// the descriptor shall be a valid pointer
// the caller shall compute line from the reference __LINE__
#define mikro_Log_append(desc, line, prf, msg)              fprintf(desc, "%d:%s:%s:%s\n", (line), __func__, (prf), (msg))
#define mikro_Log_append_withInt(desc, line, prf, msg, cd)  fprintf(desc, "%d:%s:%s:%s:%d\n", (line), __func__, (prf), (msg), (cd))
#define mikro_Log_append_withStr(desc, line, prf, msg, str) fprintf(desc, "%d:%s:%s:%s:%s\n", (line), __func__, (prf), (msg), (str))
#define mikro_Log_show(desc, prf, msg)                      fprintf(desc, "%s:%s\n", (prf), (msg))

#endif // MIKRO_LOGGING_H
