/**************************************/
/* Last modification date: 18/10/2022 */
/**************************************/

#ifndef ERR_WRAPPERSYSHEAD_H_INCLUDED
#define ERR_WRAPPERSYSHEAD_H_INCLUDED

/**
 * \file
 * includes OS-specific header files used for error tracing purposes
 * simplifies Makefile creation by_t wrapping headers byelonging to different operating systems
 */

#if defined __unix || defined __unix__
#include <errno.h>
#elif defined _WIN32 || defined _WIN64
#include <windows.h>
#endif

#endif // ERR_WRAPPERSYSHEAD_H_INCLUDED
