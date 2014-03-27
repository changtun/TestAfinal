/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name : stapi.c
* Description : Includes all other STAPI driver header files.
              	This allows a STAPI application to include only this one header
              	file directly. 
* History :
*	Date               Modification                                Name
*	----------         ------------                                ----------
*	2006/01/24         Created                                     LSZ
******************************************************************************/
/*******************************************************/
/*              Includes				                   */
/*******************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "nporuntime.h"
#include "pvplugin.h"

	
/*******************************************************/
/*               Private Defines and Macros			   */
/*******************************************************/

/*******************************************************/
/*               Private Types						   */
/*******************************************************/

/*******************************************************/
/*               Private Constants                     */
/*******************************************************/

/*******************************************************/
/*               Global Variables					   */
/*******************************************************/

/*******************************************************/
/*               Private Variables (static)			   */
/*******************************************************/

/*******************************************************/
/*               Private Function prototypes		   */
/*******************************************************/

/*******************************************************/
/*               Functions							   */
/*******************************************************/
char *RuntimeNPObject::stringValue(const NPString &s)
{
  NPUTF8 *val = static_cast < NPUTF8 * > (malloc((s.UTF8Length + 1) *sizeof
    (*val)));
  if (val)
  {
    strncpy(val, s.UTF8Characters, s.UTF8Length);
    val[s.UTF8Length] = '\0';
  }
  return val;
}

char *RuntimeNPObject::stringValue(const NPVariant &v)
{
  char *s = NULL;
  if (NPVARIANT_IS_STRING(v))
  {
    return stringValue(NPVARIANT_TO_STRING(v));
  }
  return s;
}

RuntimeNPObject::InvokeResult RuntimeNPObject::getProperty(int, NPVariant &)
{
  /* default behaviour */
  return INVOKERESULT_GENERIC_ERROR;
}

RuntimeNPObject::InvokeResult RuntimeNPObject::setProperty(int, const NPVariant
  &)
{
  /* default behaviour */
  return INVOKERESULT_GENERIC_ERROR;
}

RuntimeNPObject::InvokeResult RuntimeNPObject::removeProperty(int)
{
  /* default behaviour */
  return INVOKERESULT_GENERIC_ERROR;
}

RuntimeNPObject::InvokeResult RuntimeNPObject::invoke(int, const NPVariant *,
  uint32_t, NPVariant &)
{
  /* default beahviour */
  return INVOKERESULT_GENERIC_ERROR;
}

RuntimeNPObject::InvokeResult RuntimeNPObject::invokeDefault(const NPVariant *,
  uint32_t, NPVariant &result)
{
  /* return void */
  VOID_TO_NPVARIANT(result);
  return INVOKERESULT_NO_ERROR;
}

bool RuntimeNPObject::returnInvokeResult(RuntimeNPObject::InvokeResult result)
{
  switch (result)
  {
    case INVOKERESULT_NO_ERROR:
      return true;
    case INVOKERESULT_GENERIC_ERROR:
      break;
    case INVOKERESULT_NO_SUCH_METHOD:
      NPN_SetException(this, "No such method or arguments mismatch");
      break;
    case INVOKERESULT_INVALID_ARGS:
      NPN_SetException(this, "Invalid arguments");
      break;
    case INVOKERESULT_INVALID_VALUE:
      NPN_SetException(this, "Invalid value in assignment");
      break;
    case INVOKERESULT_OUT_OF_MEMORY:
      NPN_SetException(this, "Out of memory");
      break;
  }
  return false;
}

RuntimeNPObject::InvokeResult RuntimeNPObject::invokeResultString(const
  char*psz, NPVariant &result)
{
  if (!psz)
    NULL_TO_NPVARIANT(result);
  else
  {
    size_t len = strlen(psz);
    NPUTF8 *retval = (NPUTF8*)NPN_MemAlloc(len);
    if (!retval)
      return INVOKERESULT_OUT_OF_MEMORY;
    else
    {
      memcpy(retval, psz, len);
      STRINGN_TO_NPVARIANT(retval, len, result);
    }
  }
  return INVOKERESULT_NO_ERROR;
}
