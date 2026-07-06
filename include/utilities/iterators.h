/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 01-07-2026 */
/**************************************/

#ifndef ITERATORS_H
#define ITERATORS_H

#include "generic.h"

// unsigned
#define ITER_UNS_I(init, max)   for (unsigned i = IDEM_INT(init); i < (max); i ++)
#define ITER_UNS_II(init, max)  for (unsigned ii = IDEM_INT(init); ii < (max); ii ++)
#define ITER_UNS_J(init, max)   for (unsigned j = IDEM_INT(init); j < (max); j ++)
#define ITER_UNS_K(init, max)   for (unsigned k = IDEM_INT(init); k < (max); k ++)
// signed
#define ITER_SGN_I(init, max)   for (int i = IDEM_INT(init); i < (max); i ++)
#define ITER_SGN_II(init, max)  for (int ii = IDEM_INT(init); ii < (max); ii ++)
#define ITER_SGN_J(init, max)   for (int j = IDEM_INT(init); j < (max); j ++)
#define ITER_SGN_K(init, max)   for (int k = IDEM_INT(init); k < (max); k ++)
// size_t
#define ITER_SIZE_T_I(init, max)   for (size_t i = IDEM_INT(init); i < (max); i ++)
#define ITER_SIZE_T_II(init, max)  for (size_t ii = IDEM_INT(init); ii < (max); ii ++)
#define ITER_SIZE_T_J(init, max)   for (size_t j = IDEM_INT(init); j < (max); j ++)
#define ITER_SIZE_T_K(init, max)   for (size_t k = IDEM_INT(init); k < (max); k ++)

#endif // ITERATORS_H
