/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 16-07-2025 */
/**************************************/

#ifndef GENERIC_H
#define GENERIC_H

#define INV_PNT  ((void*) 0)
// idempotence (useful for inhibiting const-related compilation system warnings)
#define IDEM_INT(obj) (((long) (obj)) & ~0)

#endif // GENERIC_H
