/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 04/05/2023 */
/**************************************/

#ifndef PHY_SOCKETINTERFACE_H_INCLUDED
#define PHY_SOCKETINTERFACE_H_INCLUDED

/**
 * \file
 * \author Pavlo Nykolyn
 * provides an interface for I/O operations mediated by a socket
 * This module can be used only on machines that run at least a 32-bit Windows OS or a Unix-based OS
 */

#include <stdint.h>

#if defined(_WIN32) || defined(_WIN64)
typedef unsigned phy_Sck_socketD;
#else
typedef int phy_Sck_socketD;
#endif

#if defined(_WIN32) || defined(_WIN64)
int phy_Sck_interfaceInitializer(void);

int phy_Sck_interfaceCleanup(void);
#endif

phy_Sck_socketD phy_Sck_openConnection(const char* const phy_sck_strHostName,
                                       const char* const phy_sck_strServiceName,
                                       int* restrict phy_sck_pCd);

int phy_Sck_closeConnection(const phy_Sck_socketD phy_sck_aDescriptor);

size_t phy_Sck_read(const phy_Sck_socketD phy_sck_iden,
                    const size_t phy_sck_len, void* phy_sck_buf,
                    const unsigned phy_sck_timeout,
                    int* restrict phy_sck_pCd);

size_t phy_Sck_write(const phy_Sck_socketD phy_sck_iden,
                     const size_t phy_sck_len, const void* phy_sck_buf,
                     int* restrict phy_sck_pCd);

#endif // PHY_SOCKETINTERFACE_H_INCLUDED
