/*************************************/
/* Author: Pavlo Nykolyn             */
/* Last modification date 05/02/2026 */
/*************************************/

#include <stdio.h>
#include "mikro_string.h"
#include "sys_wrapperSocketHead.h"
#include "phy_socketInterface.h"
#include "err_system.h"
#include "mikro_wrapper_utilities.h"

// error indicators for socket descriptor creation and socket interface functions
// PHY_SCK_I_ERRIND    ->  socket interface function;
// PHY_SCK_I_ERRSCKCR  ->  socket descriptor creation;
#if defined _WIN32 || defined _WIN64
   #define PHY_SCK_I_ERRIND    SOCKET_ERROR
   #define PHY_SCK_I_ERRSCKCR  INVALID_SOCKET
#elif defined __unix || defined __unix__
   #define PHY_SCK_I_ERRIND    -1
   #define PHY_SCK_I_ERRSCKCR  PHY_SCK_I_ERRIND
#endif

#define PHY_SCK_I_GENTOUT       3000  // a generic time-out (expressed in milliseconds) used during read/write operations
#ifndef PHY_SCK_I_ASYCONNTOUT
   #define PHY_SCK_I_ASYCONNTOUT  60000  // a time-out (expressed in seconds) used to allow for an asynchronous connection
#endif

// flushes the buffer containing read data. The caller must ensure that phy_sck_aDesc is valid and refers
// to an open socket descriptor;
// returns an error code of the OS API
static int phy_sck_flushRecBuf(const phy_Sck_socketD phy_sck_aDesc);

// checks if a socket descriptor is ready for a read/write operation. If phy_sck_fRW is true, readiness is
// ensured for a read operation. Otherwise, readiness is ensured for a write operation;
// phy_sck_timeout defines the maximum wait time of the check operation;
// the return value is false when the socket descriptor is not ready for the requested operation and true otherwise
// the caller MUST ENSURE THE VALIDITY of phy_sck_pCd;
static bool phy_sck_isSDReady(phy_Sck_socketD phy_sck_aDesc,
                              const bool phy_sck_fRW,
                              const int phy_sck_timeout,
                              int* restrict phy_sck_pCd);

static int phy_sck_flushRecBuf(const phy_Sck_socketD phy_sck_aDesc)
{
   int phy_sck_cd = 0;
   int phy_sck_lenBufB;
   // added robustness when ioctl triggers an EINTR error code
   while (ioctl(phy_sck_aDesc, FIONREAD, &phy_sck_lenBufB) == PHY_SCK_I_ERRIND) {
      phy_sck_cd = err_Sys_getCode();
      if (phy_sck_cd &&
          phy_sck_cd != EINTR) {
         static const char funcName[] = "ioctl";
         err_Sys_printMsg(strlen(funcName), funcName,
                          phy_sck_cd);
         goto PHY_SCK_FLUSHRECBUF_EXIT;
      }
   }
   if (phy_sck_lenBufB > 0) {
      unsigned char phy_sck_tmpBuf[phy_sck_lenBufB];
      size_t phy_sck_numBytesR = 0;
      while (phy_sck_numBytesR < phy_sck_lenBufB) {
         ssize_t phy_sck_retVal = recv(phy_sck_aDesc,
                                       phy_sck_tmpBuf + phy_sck_numBytesR,
                                       phy_sck_lenBufB - phy_sck_numBytesR,
                                       0);
         if (phy_sck_retVal == PHY_SCK_I_ERRIND) {
            phy_sck_cd = err_Sys_getCode();
            if (phy_sck_cd &&
                phy_sck_cd != EINTR) {
               static const char funcName[] = "recv";
               err_Sys_printMsg(strlen(funcName), funcName,
                                phy_sck_cd);
               goto PHY_SCK_FLUSHRECBUF_EXIT;
            }
         }
         else
            phy_sck_numBytesR += phy_sck_retVal;
      }
   }
   PHY_SCK_FLUSHRECBUF_EXIT:
   return phy_sck_cd;
}

#if defined _WIN32 || defined _WIN64
int phy_Sck_interfaceInitializer(void)
{
   WSADATA phy_sck_anImplStruct;
   int phy_sck_cd = WSAStartup(WINSOCK_VERSION, &phy_sck_anImplStruct);
   if (phy_sck_cd) {
      static const char funcName[] = "WSAStartup";
      err_Sys_printMsg(strlen(funcName), funcName,
                       phy_sck_cd);
      return phy_sck_cd;
   }
   return 0;
}

int phy_Sck_interfaceCleanup(void)
{
   int phy_sck_cd = WSACleanup();
   if (phy_sck_cd == PHY_SCK_I_ERRIND) {
      static const char funcName[] = "WSACleanup";
      err_Sys_printMsg(strlen(funcName), funcName,
                       phy_sck_cd);
      return phy_sck_cd;
   }
   return 0;
}
#endif

phy_Sck_socketD phy_Sck_openConnection(const char* const phy_sck_strHostName, const char* const phy_sck_strServiceName,
                                       int* restrict phy_sck_pCd)
{
   int phy_sck_cd = 0; // error checking is performed even if IEC62056_sck_pErrCode is invalid
   struct addrinfo* phy_sck_pListAI = 0; // references a linked list of addrinfo structures returned by getaddrinfo
   struct addrinfo* phy_sck_pAI = 0;     // references an addrinfo structure used to traverse the linked list
   // the initialization is not needed but, I'm adding it just to remove a warning produced when debug features are disabled and optimization options are enabled
   phy_Sck_socketD phy_sck_sockFD = PHY_SCK_I_ERRSCKCR; // will hold the socket file descriptor

   struct addrinfo phy_sck_hints;         // contains hints used to generate the Internet addresses for the specified host-name and service name
   memset(&phy_sck_hints, 0, sizeof(struct addrinfo));
   // IPv4 is the address family that is searched for the given host and service names
   phy_sck_hints.ai_family = AF_INET;
   // the host-name must be a numeric address
   phy_sck_hints.ai_flags = AI_NUMERICHOST;
   // TCP is used with any Internet address family
   phy_sck_hints.ai_protocol = IPPROTO_TCP;
   // the opened socket, with addresses returned by this function call, uses TCP in conjunction with IPv4
   phy_sck_hints.ai_socktype = SOCK_STREAM;
   phy_sck_cd = getaddrinfo(phy_sck_strHostName, phy_sck_strServiceName,
                            &phy_sck_hints,
                            &phy_sck_pListAI);
   if (phy_sck_cd) {
      static const char funcName[] = "getaddrinfo";
      #if defined __unix  || defined __unix__
      if (phy_sck_cd != EAI_SYSTEM) {
         mikro_Log_append_withStr(stdout, __LINE__ - 6, "sys_err", funcName, gai_strerror(phy_sck_cd));
         // the goto is necessary in order to retain compatibility with a Windows operating system (32-bit or 64-bit)
         goto PHY_SCK_OPENCLIENT_EXIT;
      }
      #endif
      phy_sck_cd = err_Sys_getCode();
      err_Sys_printMsg(strlen(funcName), funcName,
                       phy_sck_cd);
      err_Sys_resetCode();
      goto PHY_SCK_OPENCLIENT_EXIT;
   }

   for (phy_sck_pAI = phy_sck_pListAI; phy_sck_pAI; phy_sck_pAI = phy_sck_pAI -> ai_next) {
      fprintf(stdout, "sys__op:opening socket with address %s_%s\n", phy_sck_strHostName, phy_sck_strServiceName);
      err_Sys_resetCode();
      #if defined _WIN32 || defined _WIN64
      phy_sck_sockFD = WSASocket(phy_sck_pAI -> ai_family,
                                 phy_sck_pAI -> ai_socktype,
                                 phy_sck_pAI -> ai_protocol,
                                 0,
                                 0,
                                 0);
      #elif defined __unix || defined __unix__
      phy_sck_sockFD = socket(phy_sck_pAI -> ai_family,
                              (phy_sck_pAI -> ai_socktype) | SOCK_NONBLOCK,
                              phy_sck_pAI -> ai_protocol);
      #endif
      if (phy_sck_sockFD == PHY_SCK_I_ERRSCKCR) {
         phy_sck_cd = err_Sys_getCode();
         #if defined _WIN32 || defined _WIN64
            static const char funcName[] = "WSASocket";
         #elif defined __unix || defined __unix__
            static const char funcName[] = "socket";
         #endif
         err_Sys_printMsg(strlen(funcName), funcName,
                          phy_sck_cd);
         goto PHY_SCK_OPENCLIENT_EXIT;
      }
      else {
         fputs("sys__op:socket opened\n", stdout);
         #if defined _WIN32 || defined _WIN64
         // setting the l_onoff member of the internal linger structure to 0, in order to perform a graceful shutdown when required
         // it is explicitly set in order to avoid issues with changes that may affect the LINGER structure maintained for a socket
         int phy_sck_l_onoff = 0;
         // the following calls to setsockopt shall not fail
         setsockopt(phy_sck_sockFD,
                    SOL_SOCKET,
                    SO_DONTLINGER,
                    (const char*) &phy_sck_l_onoff,
                    sizeof(int));
         #endif
         int phy_sck_status = connect(phy_sck_sockFD,
                                      phy_sck_pAI -> ai_addr,
                                      phy_sck_pAI -> ai_addrlen);
         if (phy_sck_status == PHY_SCK_I_ERRIND) {
            // an interrupted connect is dealt with by closing implicitly the socket
            phy_sck_cd = err_Sys_getCode(); // for now it refers to connect
            int phy_sck_newCd = 0; // it always refers to poll, till this scope ceases to be valid
            bool phy_sck_fReady = false; // assuming that the connection attempt will not be successful
            #if defined __unix || defined __unix__
            if (phy_sck_cd == EINPROGRESS) {
               phy_sck_cd = 0;
               // assuming that the time-out does not expire, exceptional conditions do not arise and poll does not fail,
               // the socket shall connect asynchronously;
               // checking the readiness of a socket descriptor, as a side effect, always resets errno
               phy_sck_fReady = phy_sck_isSDReady(phy_sck_sockFD,
                                                  false,
                                                  PHY_SCK_I_ASYCONNTOUT,
                                                  &phy_sck_newCd);
               if (!phy_sck_fReady) {
                  if (phy_sck_newCd) {
                     static const char funcName[] = "poll";
                     err_Sys_printMsg(strlen(funcName), funcName,
                                      phy_sck_newCd);
                     ; // avoids the need to enclose the conditional instruction within a conditional macro
                  }
                  else
                     phy_sck_cd = ETIMEDOUT;
               }
            }
            if (!phy_sck_cd) {
               static socklen_t phy_sck_lenInt = sizeof(int); // is a constant
               // it is expected that the getsockopt function never fails
               getsockopt(phy_sck_sockFD,
                          SOL_SOCKET,
                          SO_ERROR,
                          &phy_sck_cd,
                          &phy_sck_lenInt);
            }
            #endif
            if (phy_sck_cd) {
               static const char funcName[] = "connect";
               err_Sys_printMsg(strlen(funcName), funcName,
                                phy_sck_newCd);
            }
            if (phy_sck_newCd)
               phy_sck_cd = phy_sck_newCd; // now it refers to poll
            if (phy_sck_cd) {
               // making use of the implicit shutdown sequence as no I/O operation has been performed yet
               // the next calls SHALL NOT fail
               #if defined _WIN32 || defined _WIN64
               closesocket(phy_sck_sockFD);
               #elif defined __unix || defined __unix__
               close(phy_sck_sockFD);
               #endif
               goto PHY_SCK_OPENCLIENT_EXIT;
            }
         }
      }
   }
   PHY_SCK_OPENCLIENT_EXIT:
   freeaddrinfo(phy_sck_pListAI);
   if (phy_sck_pCd)
      *phy_sck_pCd = phy_sck_cd;
   return phy_sck_sockFD;
}

int phy_Sck_closeConnection(const phy_Sck_socketD phy_sck_sockFD)
{
   int phy_sck_cd = 0;
   // a macro that defines an OS-specific identifier for both read and write operations, used for their termination
   #if defined _WIN32 || defined _WIN64
   #define PHY_SCK_SHUTRW  SD_BOTH
   #elif defined __unix || defined __unix__
   #define PHY_SCK_SHUTRW  SHUT_RDWR
   #endif
   // robustness with respect to EINTR
   do {
      err_Sys_resetCode();
      int phy_sck_status = shutdown(phy_sck_sockFD,
                                    PHY_SCK_SHUTRW);
      if (phy_sck_status == PHY_SCK_I_ERRIND) {
         phy_sck_cd = err_Sys_getCode();
         if (phy_sck_cd != EINTR) {
            static const char funcName[] = "shutdown";
            err_Sys_printMsg(strlen(funcName), funcName,
                             phy_sck_cd);
         }
      }
   } while (phy_sck_cd == EINTR);
   // relying on the implicit shutdown sequence if the explicit one failed
   #if defined _WIN32 || defined _WIN64
   closesocket(phy_sck_sockFD);
   #elif defined __unix || defined __unix__
   close(phy_sck_sockFD); // it is assumed that even if close is interrupted by a signal, the descriptor will not be valid
   #endif
   fputs("sys__op:socket closed\n", stdout);
   return phy_sck_cd;
}

size_t phy_Sck_read(const phy_Sck_socketD phy_sck_iden,
                    const size_t phy_sck_len, void* phy_sck_buf,
                    const unsigned phy_sck_timeout,
                    int* restrict phy_sck_pCd)
{
   int phy_sck_cd = 0; // error checking is performed even if phy_sck_pCd is invalid
   size_t phy_sck_numBytesR = 0;
   if (phy_sck_isSDReady(phy_sck_iden,
                         true,
                         phy_sck_timeout,
                         &phy_sck_cd)) {
      ssize_t phy_sck_retVal = recv(phy_sck_iden,
                                    phy_sck_buf,
                                    phy_sck_len,
                                    0);
      if (phy_sck_retVal == PHY_SCK_I_ERRIND) {
         phy_sck_cd = err_Sys_getCode();
         if (phy_sck_cd != EINTR &&
             phy_sck_cd != EAGAIN &&
             phy_sck_cd != EWOULDBLOCK) {
            static const char funcName[] = "recv";
            err_Sys_printMsg(strlen(funcName), funcName,
                             phy_sck_cd);
            goto PHY_SCK_READ_EXIT;
         }
         // robustness with respect to EINTR
         phy_sck_cd = 0;
      }
      else
         phy_sck_numBytesR += phy_sck_retVal;
   }
   if (phy_sck_cd) {
      #if defined __unix || defined __unix__
         static const char funcName[] = "poll";
      #elif defined _WIN32 || defined _WIN64
         static const char funcName[] = "select";
      #endif
      err_Sys_printMsg(strlen(funcName), funcName,
                       phy_sck_cd);
   }
   PHY_SCK_READ_EXIT:
   if (phy_sck_pCd)
      *phy_sck_pCd = phy_sck_cd;
   return phy_sck_numBytesR;
}

size_t phy_Sck_write(const phy_Sck_socketD phy_sck_iden,
                     const size_t phy_sck_len, const void* phy_sck_buf,
                     int* restrict phy_sck_pCd)
{
   int phy_sck_cd = 0; // error checking is performed even if phy_sck_pCd is invalid
   size_t phy_sck_numBytesW = 0;
   phy_sck_cd = phy_sck_flushRecBuf(phy_sck_iden);
   if (phy_sck_cd) goto PHY_SCK_WRITE_EXIT;
   if (phy_sck_isSDReady(phy_sck_iden,
                         false,
                         PHY_SCK_I_GENTOUT,
                         &phy_sck_cd)) {
      ssize_t phy_sck_retVal = send(phy_sck_iden,
                                    phy_sck_buf,
                                    phy_sck_len,
                                    0);
      if (phy_sck_retVal == PHY_SCK_I_ERRIND) {
         phy_sck_cd = err_Sys_getCode();
         if (phy_sck_cd != EINTR &&
             phy_sck_cd != EAGAIN &&
             phy_sck_cd != EWOULDBLOCK) {
            static const char funcName[] = "send";
            err_Sys_printMsg(strlen(funcName), funcName,
                             phy_sck_cd);
            goto PHY_SCK_WRITE_EXIT;
         }
         // robustness with respect to EINTR
         phy_sck_cd = 0;
      }
      else
         phy_sck_numBytesW += phy_sck_retVal;
   }
   if (phy_sck_cd) {
      #if defined __unix || defined __unix__
         static const char funcName[] = "poll";
      #elif defined _WIN32 || defined _WIN64
         static const char funcName[] = "select";
      #endif
      err_Sys_printMsg(strlen(funcName), funcName,
                       phy_sck_cd);
   }
   PHY_SCK_WRITE_EXIT:
   if (phy_sck_pCd)
      *phy_sck_pCd = phy_sck_cd;
   return phy_sck_numBytesW;
}

#if defined __unix || defined __unix__
static bool phy_sck_isSDReady(phy_Sck_socketD phy_sck_aDesc,
                              const bool phy_sck_fRW,
                              const int phy_sck_timeout,
                              int* restrict phy_sck_pCd)
{
   struct pollfd phy_sck_anFDS = {.fd = phy_sck_aDesc,
                                  .events = ((phy_sck_fRW ? POLLIN : POLLOUT) | POLLPRI),
                                  .revents = 0};
   int phy_sck_status = 0;  // assuming that poll will consume the time-out
   // robustness with respect to EINTR
   do {
      *phy_sck_pCd = 0;
      err_Sys_resetCode();
      phy_sck_status = poll(&phy_sck_anFDS, 1, phy_sck_timeout);
      if (phy_sck_status == PHY_SCK_I_ERRIND)
         *phy_sck_pCd = err_Sys_getCode();
   } while (*phy_sck_pCd == EINTR);
   if (phy_sck_status == 1) {
      if (phy_sck_anFDS.revents & (POLLERR |
                                   POLLHUP |
                                   POLLNVAL |
                                   POLLPRI))
         // an exceptional event has been triggered
         return false;
      else if (phy_sck_anFDS.revents & (POLLIN |
                                        POLLOUT))
         // the socket descriptor is ready
         return true;
   }
   // poll either triggered an error or timed-out
   return false;
}
#elif defined _WIN32 || defined _WIN64
static bool phy_sck_isSDReady(phy_Sck_socketD phy_sck_aDesc,
                              const bool phy_sck_fRW,
                              const int phy_sck_timeout,
                              int* restrict phy_sck_pCd)
{
   FD_SET phy_sck_anFDSet;
   FD_ZERO(&phy_sck_anFDSet);
   FD_SET(phy_sck_aDesc,
          &phy_sck_anFDSet);
   FD_SET* phy_sck_pFdSetRead = phy_sck_fRW ? &phy_sck_anFDSet : 0;
   FD_SET* phy_sck_pFdSetWrite = !phy_sck_fRW ? &phy_sck_anFDSet : 0;
   FD_SET* phy_sck_pFdSetErr = &phy_sck_anFDSet;
   const long phy_sck_numSec = phy_sck_timeout / 1000; // number of seconds extracted from phy_sck_timeout
   TIMEVAL mtrRead_ser_aTOut = {.tv_sec = phy_sck_numSec,
                                .tv_usec = (phy_sck_timeout - (phy_sck_numSec * 1000))};
   // an integer flag describing the status of select on return
   err_Sys_resetCode();
   int phy_sck_status = select(0,
                               phy_sck_pFdSetRead,
                               phy_sck_pFdSetWrite,
                               phy_sck_pFdSetErr,
                               &mtrRead_ser_aTOut);
   if (phy_sck_status == PHY_SCK_I_ERRIND)
      *phy_sck_pCd = err_Sys_getCode();
   if (phy_sck_status > 0) {
      if (phy_sck_pFdSetRead &&
          FD_ISSET(phy_sck_aDesc, phy_sck_pFdSetRead))
         return true;
      else if (phy_sck_pFdSetWrite &&
               FD_ISSET(phy_sck_aDesc, phy_sck_pFdSetWrite))
         return true;
      else {
         DWORD phy_sck_cd = 0;
         size_t phy_sck_szDWORD = sizeof(DWORD);
         getsockopt(phy_sck_aDesc,
                    SOL_SOCKET,
                    SO_ERROR,
                    (char*) &phy_sck_cd,
                    (int*) &phy_sck_szDWORD);
         *phy_sck_pCd = phy_sck_cd;
         return false;
      }
   }
   return false;
}
#endif
