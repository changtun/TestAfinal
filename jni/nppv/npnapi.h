/*****************************************************************************
*	(c)	Copyright Pro. Broadband Inc. PVware
*
* File name  : stapi.h
* Description : Includes all other STAPI driver header files.
              	This allows a STAPI application to include only this one header
              	file directly. 
* Author : LSZ
* History :
*	2006/01/24 : Initial Version
******************************************************************************/
#ifndef __NPN_API_H
#define __NPN_API_H

/*******************************************************/
/*              Includes				                   */
/*******************************************************/

/*******************************************************/
/*              Exported Defines and Macros            */
/*******************************************************/

/*******************************************************/
/*              Exported Types			                */
/*******************************************************/

/*******************************************************/
/*              Exported Variables		                */
/*******************************************************/

/*******************************************************/
/*              External				                   */
/*******************************************************/

/*******************************************************/
/*              Exported Functions		                */
/*******************************************************/
void NPN_Version(int *plugin_major, int *plugin_minor, int *netscape_major, int *netscape_minor);
void NPN_PluginThreadAsyncCall(NPP plugin, void(*func)(void*), void *userData);
NPError NPN_GetValue(NPP instance, NPNVariable variable, void *r_value);
NPError NPN_SetValue(NPP instance, NPPVariable variable, void *value);
NPError NPN_GetURL(NPP instance, const char *url, const char *window);
NPError NPN_GetURLNotify(NPP instance, const char *url, const char *window, void *notifyData);
NPError NPN_PostURL(NPP instance, const char *url, const char *window, uint32_t len, const char *buf, NPBool file);
NPError NPN_PostURLNotify(NPP instance, const char *url, const char *window, uint32_t len, const char *buf, NPBool file, void *notifyData);
NPError NPN_RequestRead(NPStream *stream, NPByteRange *rangeList);
NPError NPN_NewStream(NPP instance, NPMIMEType type, const char *window, NPStream **stream_ptr);
int32_t NPN_Write(NPP instance, NPStream *stream, int32_t len, void *buffer);
NPError NPN_DestroyStream(NPP instance, NPStream *stream, NPError reason);
void NPN_Status(NPP instance, const char *message);
const char *NPN_UserAgent(NPP instance);
void *NPN_MemAlloc(uint32_t size);
void NPN_MemFree(void *ptr);
uint32_t NPN_MemFlush(uint32_t size);
void NPN_ReloadPlugins(NPBool reloadPages);


#ifdef OJI
  JRIEnv *NPN_GetJavaEnv();
  jref NPN_GetJavaPeer(NPP instance);

#endif

void NPN_InvalidateRect(NPP instance, NPRect *invalidRect);
void NPN_InvalidateRegion(NPP instance, NPRegion invalidRegion);
void NPN_ForceRedraw(NPP instance);
void NPN_PushPopupsEnabledState(NPP instance, NPBool enabled);
void NPN_PopPopupsEnabledState(NPP instance);
NPIdentifier NPN_GetStringIdentifier(const NPUTF8 *name);
void NPN_GetStringIdentifiers(const NPUTF8 **names, int32_t nameCount, NPIdentifier *identifiers);
NPIdentifier NPN_GetIntIdentifier(int32_t intid);
bool NPN_IdentifierIsString(NPIdentifier identifier);
NPUTF8 *NPN_UTF8FromIdentifier(NPIdentifier identifier);
int32_t NPN_IntFromIdentifier(NPIdentifier identifier);
NPObject *NPN_CreateObject(NPP npp, NPClass *aClass);
NPObject *NPN_RetainObject(NPObject *obj);
void NPN_ReleaseObject(NPObject *obj);
bool NPN_Invoke(NPP npp, NPObject *obj, NPIdentifier methodName, const NPVariant *args, uint32_t argCount, NPVariant *result);
bool NPN_InvokeDefault(NPP npp, NPObject *obj, const NPVariant *args, uint32_t argCount, NPVariant *result);
bool NPN_Evaluate(NPP npp, NPObject *obj, NPString *script, NPVariant *result);
bool NPN_GetProperty(NPP npp, NPObject *obj, NPIdentifier propertyName, NPVariant *result);
bool NPN_SetProperty(NPP npp, NPObject *obj, NPIdentifier propertyName, const NPVariant *value);
bool NPN_RemoveProperty(NPP npp, NPObject *obj, NPIdentifier propertyName);
bool NPN_HasProperty(NPP npp, NPObject *obj, NPIdentifier propertyName);
bool NPN_HasMethod(NPP npp, NPObject *obj, NPIdentifier methodName);
void NPN_ReleaseVariantValue(NPVariant *variant);
void NPN_SetException(NPObject *obj, const NPUTF8 *message);


uint32_t NPN_ScheduleTimer(NPP instance, uint32_t interval, NPBool repeat,void (*timerFunc)(NPP npp, uint32_t timerID));
void NPN_UnscheduleTimer(NPP instance, uint32_t timerID);



#endif  /* #ifndef __NPNAPI_H */
/* End of npnapi.h  --------------------------------------------------------- */
