/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 26-06-2025 */
/**************************************/

#ifndef GENERIC_H
#define GENERIC_H

// idempotence checks
#define INV_PNT  ((void*) 0)
#define IDEM_INT(obj) ((obj) & ~0) // any precision will suffice

#endif // GENERIC_H
