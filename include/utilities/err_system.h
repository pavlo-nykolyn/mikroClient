/**************************************/
/* Author: Pavlo Nykolyn              */
/* Last modification date: 18/10/2022 */
/**************************************/

#ifndef ERR_SYSTEM_H_INCLUDED
#define ERR_SYSTEM_H_INCLUDED

#include "header_wrapper.h"

/**
 * \brief retrieves an error code of a function belonging to an operating system API and resets it afterwards
 * \return error code
 */
int err_Sys_getCode(void);

/**
 * \brief resets an error code of a function belonging to an operating system API
 */
void err_Sys_resetCode(void);

/**
 * \brief prints a generated message of an error triggered by a function of an operating system API
 * \param[in] err_sys_szStrFuncName size of the string containg the name of the function
 * \param[in] err_sys_strFuncName string containg the name of the function
 * \param[in] err_sys_code error code
 * \attention the message is printed on \a stderr
 */
void err_Sys_printMsg(const size_t err_sys_szStrFuncName, const char err_sys_strFuncName[static err_sys_szStrFuncName],
                      const int err_sys_code);

#endif // ERR_SYSTEM_H_INCLUDED
