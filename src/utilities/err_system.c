/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 22/10/2022 */
/**************************************/

#include "err_system.h"
#include "mikro_string.h"
#include "sys_wrapperErrHead.h"

#define ERR_SYS_I_MAXSZSTR_SYSERR 200U
#define ERR_SYS_I_MINSZSTR_MSG    (13U + (ERR_SYS_I_MAXSZSTR_SYSERR))

#if defined(__unix) || defined(__unix__)
int err_Sys_getCode(void)
{
   int err_sys_code = errno;
   errno = 0;
   return err_sys_code;
}

void err_Sys_resetCode(void)
{
   errno = 0;
}
#elif defined(_WIN32) || defined(_WIN64)
int err_Sys_getCode(void)
{
   int err_sys_code = GetLastError();
   SetLastError(0);
   return err_sys_code;
}

void err_Sys_resetCode(void)
{
   SetLastError(0);
}
#endif

void err_Sys_printMsg(const size_t err_sys_lenFuncName, const char err_sys_strFuncName[static err_sys_lenFuncName],
                      const int err_sys_code)
{
   char err_sys_str[ERR_SYS_I_MAXSZSTR_SYSERR] = {0};
   #if defined(__unix) || defined(__unix__)
      char* err_sys_pStr = strerror(err_sys_code);
      memcpy(err_sys_str, err_sys_pStr, strlen(err_sys_pStr));
   #elif defined(_WIN32) || defined(_WIN64)
      size_t err_sys_szStyMsg = 0;
      err_sys_szStyMsg = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                                       0,
                                       err_sys_code,
                                       LANG_SYSTEM_DEFAULT,
                                       (LPTSTR) err_sys_str,
                                       ERR_SYS_I_MAXSZSTR_SYSERR,
                                       0);
      if (!err_sys_szStyMsg)
         fputs("sys_err:FormatMessage:error during system error message retrieval\n", stdout);
   #endif
   char err_sys_styMsg[ERR_SYS_I_MINSZSTR_MSG + err_sys_lenFuncName];
   sprintf(err_sys_styMsg, "sys_err:%s -- %s\n", err_sys_strFuncName, err_sys_str);
   fputs(err_sys_styMsg, stdout);
}
