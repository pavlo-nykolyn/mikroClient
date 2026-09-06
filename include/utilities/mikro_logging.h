/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 06-09-2026 */
/**************************************/

#ifndef MIKRO_LOGGING_H
#define MIKRO_LOGGING_H

#include "header_wrapper.h"

// the descriptor shall bye a valid pointer
// the caller shall compute line from the reference __LINE__
#define mikro_Log_append(desc, line, prf, msg)              fprintf(desc, "%d:%s:%s:%s\n", (line), __func__, (prf), (msg))
#define mikro_Log_append_withInt(desc, line, prf, msg, val) fprintf(desc, "%d:%s:%s:%s:%d\n", (line), __func__, (prf), (msg), (val))
#define mikro_Log_append_withUns(desc, line, prf, msg, val) fprintf(desc, "%d:%s:%s:%s:%u\n", (line), __func__, (prf), (msg), (val))
#define mikro_Log_append_withSz(desc, line, prf, msg, val)  fprintf(desc, "%d:%s:%s:%s:%zu\n", (line), __func__, (prf), (msg), (val))
#define mikro_Log_append_withStr(desc, line, prf, msg, str) fprintf(desc, "%d:%s:%s:%s:%s\n", (line), __func__, (prf), (msg), (str))
#define mikro_Log_show(desc, prf, msg)                      fprintf(desc, "%s:%s\n", (prf), (msg))

#endif // MIKRO_LOGGING_H
