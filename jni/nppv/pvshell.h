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
#ifndef __PV_SHELL_H
#define __PV_SHELL_H

/*******************************************************/
/*              Includes				                   */
/*******************************************************/
#include "pvplugin.h"

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
	NPError NPP_Initialize(void);
	void NPP_Shutdown(void);
	NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, NPint16_t
	argc, char *argn[], char *argv[], NPSavedData *saved);
	NPError NPP_Destroy(NPP instance, NPSavedData **save);
	NPError NPP_GetValue(NPP instance, NPPVariable variable, void *value);
	NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value);
	NPError NPP_SetWindow(NPP instance, NPWindow *window);
	NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream *stream, NPBool
	seekable, NPuint16_t *stype);
	NPError NPP_DestroyStream(NPP instance, NPStream *stream, NPError reason);
	void NPP_StreamAsFile(NPP instance, NPStream *stream, const char *fname);
	int32_t NPP_WriteReady(NPP instance, NPStream *stream);
	int32_t NPP_Write(NPP instance, NPStream *stream, NPint32_t offset, NPint32_t
	len, void *buffer);
	void NPP_URLNotify(NPP instance, const char *url, NPReason reason, void
	*notifyData);
	void NPP_Print(NPP instance, NPPrint *printInfo);
	int16_t NPP_HandleEvent(NPP instance, void *event);


	const char *NPP_GetMIMEDescription(void);


#ifdef OJI
	jref NPP_GetJavaClass(void);
#endif



#endif  /* #ifndef __PVSHELL_H */
/* End of stapi.h  --------------------------------------------------------- */
