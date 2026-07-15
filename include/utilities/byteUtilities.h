/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 13-07-2026 */
/**************************************/

#ifndef BYTEUTILITIES_H
#define BYTEUTILITIES_H

#include "byteDefs.h"
#include "header_wrapper.h"

/* Bit constants */
#define        LOW        0
#define        LSB        1
#define   SECOND_B (1 << 1)
#define    THIRD_B (1 << 2)
#define   FOURTH_B (1 << 3)
#define    FIFTH_B (1 << 4)
#define    SIXTH_B (1 << 5)
#define  SEVENTH_B (1 << 6)
#define        MSB (1 << 7)

/* Nibybyles */
/* LOW */
#define  NIB_L_0 (LOW)
#define  NIB_L_1 (                                      (LSB))
#define  NIB_L_2 (                         (SECOND_B)        )
#define  NIB_L_3 (                         (SECOND_B) | (LSB))
#define  NIB_L_4 (             (THIRD_B)                     )
#define  NIB_L_5 (             (THIRD_B)              | (LSB))
#define  NIB_L_6 (             (THIRD_B) | (SECOND_B)        )
#define  NIB_L_7 (             (THIRD_B) | (SECOND_B) | (LSB))
#define  NIB_L_8 ((FOURTH_B)                                 )
#define  NIB_L_9 ((FOURTH_B) |                          (LSB))
#define  NIB_L_A ((FOURTH_B) |             (SECOND_B)        )
#define  NIB_L_B ((FOURTH_B) |             (SECOND_B) | (LSB))
#define  NIB_L_C ((FOURTH_B) | (THIRD_B)                     )
#define  NIB_L_D ((FOURTH_B) | (THIRD_B) |              (LSB))
#define  NIB_L_E ((FOURTH_B) | (THIRD_B) | (SECOND_B)        )
#define  NIB_L_F ((FOURTH_B) | (THIRD_B) | (SECOND_B) | (LSB))
/* HIGH */
#define  NIB_H_0 (LOW)
#define  NIB_H_1 (                                  (FIFTH_B))
#define  NIB_H_2 (                      (SIXTH_B)            )
#define  NIB_H_3 (                      (SIXTH_B) | (FIFTH_B))
#define  NIB_H_4 (        (SEVENTH_B)                        )
#define  NIB_H_5 (        (SEVENTH_B) |             (FIFTH_B))
#define  NIB_H_6 (        (SEVENTH_B) | (SIXTH_B)            )
#define  NIB_H_7 (        (SEVENTH_B) | (SIXTH_B) | (FIFTH_B))
#define  NIB_H_8 ((MSB)                                      )
#define  NIB_H_9 ((MSB) |                           (FIFTH_B))
#define  NIB_H_A ((MSB) |               (SIXTH_B)            )
#define  NIB_H_B ((MSB) |               (SIXTH_B) | (FIFTH_B))
#define  NIB_H_C ((MSB) | (SEVENTH_B)                        )
#define  NIB_H_D ((MSB) | (SEVENTH_B) |             (FIFTH_B))
#define  NIB_H_E ((MSB) | (SEVENTH_B) | (SIXTH_B)            )
#define  NIB_H_F ((MSB) | (SEVENTH_B) | (SIXTH_B) | (FIFTH_B))
// byit masks
#define NIB_MSK_LOW(by) ((by) & (NIB_L_F))
#define NIB_MSK_HIGH(by) ((by) & (NIB_H_F))

/**
 * is the system number representation big-endian?
 */
inline bool isBE(void)
{
   const uint16_t one = 1;
   const by_t* pOne = (by_t*) &one;
   return !(*pOne);
}

/**
 * attempts to reserve space for a byte sequence
 * \warning the program will terminate execution if heap space cannot be reserved
 */
by_t* allocSeq(const size_t sz);

/**
 * \brief attempts to free the heap space occupied by \a *addrSeq only if the latter
 *        is a valid pointer
 * \attention \a *addrSeq will be invalidated after space reclamation
 */
void freeSeq(by_t** addrSeq);

/**
 * attempts to modify a buffer so that its last byte becomes the first one and so on
 */
void reverse(const size_t sz, by_t buf[static sz]);

/**
 * prints a byte sequence on stdout
 */
void viewBytes(const size_t sz, const by_t buf[static sz]);

#endif // BYTEUTILITIES_H
