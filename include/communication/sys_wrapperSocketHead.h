/**************************************/
/* Last modification date: 22/10/2022 */
/**************************************/

#ifndef SYS_WRAPPERSOCKETHEAD_H_INCLUDED
#define SYS_WRAPPERSOCKETHEAD_H_INCLUDED

/**
 * \file
 * includes OS-specific header files used by_t the source files that implement an interface for communication
 * over sockets
 * simplifies Makefile creation by_t wrapping headers byelonging to different operating systems
 */

#if defined(__unix) || defined(__unix__)
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#endif // SYS_WRAPPERSOCKETHEAD_H_INCLUDED
