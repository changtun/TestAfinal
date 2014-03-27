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
#include <npapi.h>

#if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
  #include "npupp.h"
#else
  #include <npfunctions.h>

#endif

#include <npruntime.h>
#include "android_npapi.h"
#include "ANPNativeWindow_npapi.h"
#include "ANPSurface_npapi.h"
#include "ANPSystem_npapi.h"





#include <cstring>


#include "../pvshell.h"
#include "log/anpdebug.h"

	
/*******************************************************/
/*               Private Defines and Macros			   */
/*******************************************************/
#ifdef PLUGIN_TRACE
  #include <stdio.h>
  #define PLUGINDEBUGSTR(msg) DebugMessage(msg)
#else
  #define PLUGINDEBUGSTR(msg)
#endif



#define EXPORT __attribute__((visibility("default")))
#define ARRAY_COUNT(array)      (sizeof(array) / sizeof(array[0]))


/*******************************************************/
/*               Private Types						   */
/*******************************************************/

/*******************************************************/
/*               Private Constants                     */
/*******************************************************/

/*******************************************************/
/*               Global Variables					   */
/*******************************************************/
static NPNetscapeFuncs gNetscapeFuncs; /* Netscape Function table */
static const char *gUserAgent; /* User agent string */
JavaVM *gVM;


ANPAudioTrackInterfaceV0 gSoundI;
ANPBitmapInterfaceV0 gBitmapI;
ANPCanvasInterfaceV0 gCanvasI;
ANPEventInterfaceV0 gEventI;
ANPLogInterfaceV0 gLogI;
ANPPaintInterfaceV0 gPaintI;
ANPPathInterfaceV0 gPathI;
ANPSurfaceInterfaceV0 gSurfaceI;
ANPSystemInterfaceV0 gSystemI;
ANPTypefaceInterfaceV0 gTypefaceI;
ANPWindowInterfaceV0 gWindowI;
ANPNativeWindowInterfaceV0  gNativeWindowI;





/*******************************************************/
/*               Private Variables (static)			   */
/*******************************************************/

/*******************************************************/
/*               Private Function prototypes		   */
/*******************************************************/
extern "C"
{
  EXPORT NPError NP_Initialize(NPNetscapeFuncs *nsTable, NPPluginFuncs *pluginFuncs, void *java_env);
  EXPORT NPError NP_GetValue(NPP instance, NPPVariable variable, void *value);
  EXPORT const char *NP_GetMIMEDescription(void);
  EXPORT void NP_Shutdown(void);
};

NPError Private_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char *argn[], char *argv[], NPSavedData *saved);
NPError Private_Destroy(NPP instance, NPSavedData **save);
NPError Private_SetWindow(NPP instance, NPWindow *window);
NPError Private_NewStream(NPP instance, NPMIMEType type, NPStream *stream,NPBool seekable, uint16_t *stype);
int32_t Private_WriteReady(NPP instance, NPStream *stream);
int32_t Private_Write(NPP instance, NPStream *stream, int32_t offset, int32_t len, void *buffer);
void Private_StreamAsFile(NPP instance, NPStream *stream, const char *fname);
NPError Private_DestroyStream(NPP instance, NPStream *stream, NPError reason);
void Private_URLNotify(NPP instance, const char *url, NPReason reason, void *notifyData);
void Private_Print(NPP instance, NPPrint *platformPrint);
NPError Private_GetValue(NPP instance, NPPVariable variable, void *r_value);
NPError Private_SetValue(NPP instance, NPNVariable variable, void *r_value);
NPError Private_HandleEvent(NPP instance, void *event);

#ifdef OJI
  JRIGlobalRef Private_GetJavaClass(void);
#endif



/*******************************************************/
/*               Functions							   */
/*******************************************************/



NPError NP_Initialize(NPNetscapeFuncs *nsTable, NPPluginFuncs *pluginFuncs, void *java_env)
{
  NPError err = NPERR_NO_ERROR;

  DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);


  /* validate input parameters */
  if ((nsTable == NULL) || (pluginFuncs == NULL))
    err = NPERR_INVALID_FUNCTABLE_ERROR;

  /*
   * Check the major version passed in Netscape's function table.
   * We won't load if the major version is newer than what we expect.
   * Also check that the function tables passed in are big enough for
   * all the functions we need (they could be bigger, if Netscape added
   * new APIs, but that's OK with us -- we'll just ignore them).
   *
   */
  if (err == NPERR_NO_ERROR)
  {
    if ((nsTable->version >> 8) > NP_VERSION_MAJOR)
      err = NPERR_INCOMPATIBLE_VERSION_ERROR;
    if (nsTable->size < ((char*) &nsTable->posturlnotify - (char*)nsTable))
      err = NPERR_INVALID_FUNCTABLE_ERROR;
    if (pluginFuncs->size < sizeof(NPPluginFuncs))
      err = NPERR_INVALID_FUNCTABLE_ERROR;
  }

  if (err == NPERR_NO_ERROR)
  {
    /*
     * Copy all the fields of Netscape function table into our
     * copy so we can call back into Netscape later.  Note that
     * we need to copy the fields one by one, rather than assigning
     * the whole structure, because the Netscape function table
     * could actually be bigger than what we expect.
     */
    int minor = nsTable->version &0xFF;

    gNetscapeFuncs.version = nsTable->version;
    gNetscapeFuncs.size = nsTable->size;
    gNetscapeFuncs.posturl = nsTable->posturl;
    gNetscapeFuncs.geturl = nsTable->geturl;
    gNetscapeFuncs.requestread = nsTable->requestread;
    gNetscapeFuncs.newstream = nsTable->newstream;
    gNetscapeFuncs.write = nsTable->write;
    gNetscapeFuncs.destroystream = nsTable->destroystream;
    gNetscapeFuncs.status = nsTable->status;
    gNetscapeFuncs.uagent = nsTable->uagent;
    gNetscapeFuncs.memalloc = nsTable->memalloc;
    gNetscapeFuncs.memfree = nsTable->memfree;
    gNetscapeFuncs.memflush = nsTable->memflush;
    gNetscapeFuncs.reloadplugins = nsTable->reloadplugins;
    #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) >= 20)
      gNetscapeFuncs.pluginthreadasynccall = nsTable->pluginthreadasynccall;
    #endif
    #ifdef OJI
      if (minor >= NPVERS_HAS_LIVECONNECT)
      {
        gNetscapeFuncs.getJavaEnv = nsTable->getJavaEnv;
        gNetscapeFuncs.getJavaPeer = nsTable->getJavaPeer;
      }
    #endif
    gNetscapeFuncs.getvalue = nsTable->getvalue;
    gNetscapeFuncs.setvalue = nsTable->setvalue;

    if (minor >= NPVERS_HAS_NOTIFICATION)
    {
      gNetscapeFuncs.geturlnotify = nsTable->geturlnotify;
      gNetscapeFuncs.posturlnotify = nsTable->posturlnotify;
    }

    if (nsTable->size >= ((char*) &nsTable->setexception - (char*)nsTable))
    {
      gNetscapeFuncs.invalidaterect = nsTable->invalidaterect;
      gNetscapeFuncs.invalidateregion = nsTable->invalidateregion;
      gNetscapeFuncs.forceredraw = nsTable->forceredraw;
      /* npruntime support */
      if (minor >= 14)
      {
        gNetscapeFuncs.getstringidentifier = nsTable->getstringidentifier;
        gNetscapeFuncs.getstringidentifiers = nsTable->getstringidentifiers;
        gNetscapeFuncs.getintidentifier = nsTable->getintidentifier;
        gNetscapeFuncs.identifierisstring = nsTable->identifierisstring;
        gNetscapeFuncs.utf8fromidentifier = nsTable->utf8fromidentifier;
        gNetscapeFuncs.intfromidentifier = nsTable->intfromidentifier;
        gNetscapeFuncs.createobject = nsTable->createobject;
        gNetscapeFuncs.retainobject = nsTable->retainobject;
        gNetscapeFuncs.releaseobject = nsTable->releaseobject;
        gNetscapeFuncs.invoke = nsTable->invoke;
        gNetscapeFuncs.invokeDefault = nsTable->invokeDefault;
        gNetscapeFuncs.evaluate = nsTable->evaluate;
        gNetscapeFuncs.getproperty = nsTable->getproperty;
        gNetscapeFuncs.setproperty = nsTable->setproperty;
        gNetscapeFuncs.removeproperty = nsTable->removeproperty;
        gNetscapeFuncs.hasproperty = nsTable->hasproperty;
        gNetscapeFuncs.hasmethod = nsTable->hasmethod;
        gNetscapeFuncs.releasevariantvalue = nsTable->releasevariantvalue;
        gNetscapeFuncs.setexception = nsTable->setexception;
      }
    }
    else
    {
      gNetscapeFuncs.invalidaterect = NULL;
      gNetscapeFuncs.invalidateregion = NULL;
      gNetscapeFuncs.forceredraw = NULL;
      gNetscapeFuncs.getstringidentifier = NULL;
      gNetscapeFuncs.getstringidentifiers = NULL;
      gNetscapeFuncs.getintidentifier = NULL;
      gNetscapeFuncs.identifierisstring = NULL;
      gNetscapeFuncs.utf8fromidentifier = NULL;
      gNetscapeFuncs.intfromidentifier = NULL;
      gNetscapeFuncs.createobject = NULL;
      gNetscapeFuncs.retainobject = NULL;
      gNetscapeFuncs.releaseobject = NULL;
      gNetscapeFuncs.invoke = NULL;
      gNetscapeFuncs.invokeDefault = NULL;
      gNetscapeFuncs.evaluate = NULL;
      gNetscapeFuncs.getproperty = NULL;
      gNetscapeFuncs.setproperty = NULL;
      gNetscapeFuncs.removeproperty = NULL;
      gNetscapeFuncs.hasproperty = NULL;
      gNetscapeFuncs.releasevariantvalue = NULL;
      gNetscapeFuncs.setexception = NULL;
    }
    if (nsTable->size >= ((char*) &nsTable->poppopupsenabledstate - (char*)
      nsTable))
    {
      gNetscapeFuncs.pushpopupsenabledstate = nsTable->pushpopupsenabledstate;
      gNetscapeFuncs.poppopupsenabledstate = nsTable->poppopupsenabledstate;
    }
    else
    {
      gNetscapeFuncs.pushpopupsenabledstate = NULL;
      gNetscapeFuncs.poppopupsenabledstate = NULL;
    }

    /*
     * Set up the plugin function table that Netscape will use to
     * call us.  Netscape needs to know about our version and size
     * and have a UniversalProcPointer for every function we
     * implement.
     */
    pluginFuncs->version = (NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR;
    pluginFuncs->size = sizeof(NPPluginFuncs);
    #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      pluginFuncs->newp = NewNPP_NewProc(Private_New);
      pluginFuncs->destroy = NewNPP_DestroyProc(Private_Destroy);
      pluginFuncs->setwindow = NewNPP_SetWindowProc(Private_SetWindow);
      pluginFuncs->newstream = NewNPP_NewStreamProc(Private_NewStream);
      pluginFuncs->destroystream = NewNPP_DestroyStreamProc
        (Private_DestroyStream);
      pluginFuncs->asfile = NewNPP_StreamAsFileProc(Private_StreamAsFile);
      pluginFuncs->writeready = NewNPP_WriteReadyProc(Private_WriteReady);
      pluginFuncs->write = NewNPP_WriteProc(Private_Write);
      pluginFuncs->print = NewNPP_PrintProc(Private_Print);
      pluginFuncs->getvalue = NewNPP_GetValueProc(Private_GetValue);
      pluginFuncs->setvalue = NewNPP_SetValueProc(Private_SetValue);
    #else
      pluginFuncs->newp = (NPP_NewProcPtr)(Private_New);
      pluginFuncs->destroy = (NPP_DestroyProcPtr)(Private_Destroy);
      pluginFuncs->setwindow = (NPP_SetWindowProcPtr)(Private_SetWindow);
      pluginFuncs->newstream = (NPP_NewStreamProcPtr)(Private_NewStream);
      pluginFuncs->destroystream = (NPP_DestroyStreamProcPtr)
        (Private_DestroyStream);
      pluginFuncs->asfile = (NPP_StreamAsFileProcPtr)(Private_StreamAsFile);
      pluginFuncs->writeready = (NPP_WriteReadyProcPtr)(Private_WriteReady);
      pluginFuncs->write = (NPP_WriteProcPtr)(Private_Write);
      pluginFuncs->print = (NPP_PrintProcPtr)(Private_Print);
      pluginFuncs->getvalue = (NPP_GetValueProcPtr)(Private_GetValue);
      pluginFuncs->setvalue = (NPP_SetValueProcPtr)(Private_SetValue);
    #endif
    pluginFuncs->event = (NPP_HandleEventProcPtr)(Private_HandleEvent);
    if (minor >= NPVERS_HAS_NOTIFICATION)
    {
      #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
        pluginFuncs->urlnotify = NewNPP_URLNotifyProc(Private_URLNotify);
      #else
        pluginFuncs->urlnotify = (NPP_URLNotifyProcPtr)(Private_URLNotify);
      #endif
    }

    static const struct
    {
      NPNVariable v;
      uint32_t size;
      ANPInterface *i;
    } gPairs[] =
    {
      {
        kAudioTrackInterfaceV0_ANPGetValue, sizeof(gSoundI), &gSoundI
      }
      ,
      {
        kBitmapInterfaceV0_ANPGetValue, sizeof(gBitmapI), &gBitmapI
      }
      ,
      {
        kCanvasInterfaceV0_ANPGetValue, sizeof(gCanvasI), &gCanvasI
      }
      ,
      {
        kEventInterfaceV0_ANPGetValue, sizeof(gEventI), &gEventI
      }
      ,
      {
        kLogInterfaceV0_ANPGetValue, sizeof(gLogI), &gLogI
      }
      ,
      {
        kPaintInterfaceV0_ANPGetValue, sizeof(gPaintI), &gPaintI
      }
      ,
      {
        kPathInterfaceV0_ANPGetValue, sizeof(gPathI), &gPathI
      }
      ,
      {
        kSurfaceInterfaceV0_ANPGetValue, sizeof(gSurfaceI), &gSurfaceI
      }
      ,
      {
        kSystemInterfaceV0_ANPGetValue, sizeof(gSystemI), &gSystemI
      }
      ,
      {
        kTypefaceInterfaceV0_ANPGetValue, sizeof(gTypefaceI), &gTypefaceI
      }
      ,
      {
        kWindowInterfaceV0_ANPGetValue, sizeof(gWindowI), &gWindowI
      }
      ,
	  
      { kNativeWindowInterfaceV0_ANPGetValue, sizeof(gNativeWindowI), &gNativeWindowI },
    };
    for (size_t i = 0; i < ARRAY_COUNT(gPairs); i++)
    {
      gPairs[i].i->inSize = gPairs[i].size;
      NPError err = gNetscapeFuncs.getvalue(NULL, gPairs[i].v, gPairs[i].i);
      if (err)
      {
        return err;
      }
    }


    #ifdef OJI
      if (minor >= NPVERS_HAS_LIVECONNECT)
        pluginFuncs->javaClass = Private_GetJavaClass();
      else
        pluginFuncs->javaClass = NULL;
    #else
      pluginFuncs->javaClass = NULL;
    #endif

    JNIEnv *env = (JNIEnv*)java_env;
    env->GetJavaVM(&gVM);

    err = NPP_Initialize();
  }

  return err;
}


NPError NP_GetValue(NPP instance, NPPVariable variable, void *value)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  if (variable == NPPVpluginNameString)
  {
    const char **str = (const char **)value;
    *str = "Pvware plugin";
    return NPERR_NO_ERROR;
  }

  if (variable == NPPVpluginDescriptionString)
  {
    const char **str = (const char **)value;
    *str = "Description of Test Plugin";
    return NPERR_NO_ERROR;
  }

  return NPERR_GENERIC_ERROR;
}

void NP_Shutdown(void)
{
  DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
  NPP_Shutdown();
  return ;
}

const char *NP_GetMIMEDescription(void)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
  return NPP_GetMIMEDescription();
}

void NPN_Version(int *plugin_major, int *plugin_minor, int *netscape_major, int
  *netscape_minor)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
  *plugin_major = NP_VERSION_MAJOR;
  *plugin_minor = NP_VERSION_MINOR;

  *netscape_major = gNetscapeFuncs.version >> 8;
  *netscape_minor = gNetscapeFuncs.version &0xFF;
}


NPError NPN_GetValue(NPP instance, NPNVariable variable, void *r_value)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    return CallNPN_GetValueProc(gNetscapeFuncs.getvalue, instance, variable,
      r_value);
  #else
    return (*gNetscapeFuncs.getvalue)(instance, variable, r_value);
  #endif
}

NPError NPN_SetValue(NPP instance, NPPVariable variable, void *value)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    return CallNPN_SetValueProc(gNetscapeFuncs.setvalue, instance, variable,
      value);
  #else
    return (*gNetscapeFuncs.setvalue)(instance, variable, value);
  #endif
}

NPError NPN_GetURL(NPP instance, const char *url, const char *window)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    return CallNPN_GetURLProc(gNetscapeFuncs.geturl, instance, url, window);
  #else
    return (*gNetscapeFuncs.geturl)(instance, url, window);
  #endif
}

NPError NPN_GetURLNotify(NPP instance, const char *url, const char *window,
  void *notifyData)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    return CallNPN_GetURLNotifyProc(gNetscapeFuncs.geturlnotify, instance, url,
      window, notifyData);
  #else
    return (*gNetscapeFuncs.geturlnotify)(instance, url, window, notifyData);
  #endif
}

NPError NPN_PostURL(NPP instance, const char *url, const char *window, uint32_t
  len, const char *buf, NPBool file)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    return CallNPN_PostURLProc(gNetscapeFuncs.posturl, instance, url, window,
      len, buf, file);
  #else
    return (*gNetscapeFuncs.posturl)(instance, url, window, len, buf, file);
  #endif
}

NPError NPN_PostURLNotify(NPP instance, const char *url, const char *window,
  uint32_t len, const char *buf, NPBool file, void *notifyData)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    return CallNPN_PostURLNotifyProc(gNetscapeFuncs.posturlnotify, instance,
      url, window, len, buf, file, notifyData);
  #else
    return (*gNetscapeFuncs.posturlnotify)(instance, url, window, len, buf,
      file, notifyData);

  #endif
}

NPError NPN_RequestRead(NPStream *stream, NPByteRange *rangeList)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    return CallNPN_RequestReadProc(gNetscapeFuncs.requestread, stream,
      rangeList);
  #else
    return (*gNetscapeFuncs.requestread)(stream, rangeList);
  #endif
}

NPError NPN_NewStream(NPP instance, NPMIMEType type, const char *window,
  NPStream **stream_ptr)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    return CallNPN_NewStreamProc(gNetscapeFuncs.newstream, instance, type,
      window, stream_ptr);
  #else
    return (*gNetscapeFuncs.newstream)(instance, type, window, stream_ptr);
  #endif
}

int32_t NPN_Write(NPP instance, NPStream *stream, int32_t len, void *buffer)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    return CallNPN_WriteProc(gNetscapeFuncs.write, instance, stream, len,
      buffer);
  #else
    return (*gNetscapeFuncs.write)(instance, stream, len, buffer);
  #endif
}

NPError NPN_DestroyStream(NPP instance, NPStream *stream, NPError reason)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    return CallNPN_DestroyStreamProc(gNetscapeFuncs.destroystream, instance,
      stream, reason);
  #else
    return (*gNetscapeFuncs.destroystream)(instance, stream, reason);
  #endif
}

void NPN_Status(NPP instance, const char *message)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    CallNPN_StatusProc(gNetscapeFuncs.status, instance, message);
  #else
    (*gNetscapeFuncs.status)(instance, message);
  #endif
}

const char *NPN_UserAgent(NPP instance)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    return CallNPN_UserAgentProc(gNetscapeFuncs.uagent, instance);
  #else
    return (*gNetscapeFuncs.uagent)(instance);
  #endif
}

void *NPN_MemAlloc(uint32_t size)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    return CallNPN_MemAllocProc(gNetscapeFuncs.memalloc, size);
  #else
    return (*gNetscapeFuncs.memalloc)(size);
  #endif
}

void NPN_MemFree(void *ptr)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    CallNPN_MemFreeProc(gNetscapeFuncs.memfree, ptr);
  #else
    (*gNetscapeFuncs.memfree)(ptr);
  #endif
}

uint32_t NPN_MemFlush(uint32_t size)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    return CallNPN_MemFlushProc(gNetscapeFuncs.memflush, size);
  #else
    return (*gNetscapeFuncs.memflush)(size);
  #endif
}

void NPN_ReloadPlugins(NPBool reloadPages)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    CallNPN_ReloadPluginsProc(gNetscapeFuncs.reloadplugins, reloadPages);
  #else
    (*gNetscapeFuncs.reloadplugins)(reloadPages);
  #endif
}

#ifdef OJI
  JRIEnv *NPN_GetJavaEnv()
  {
  DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
    #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      return CallNPN_GetJavaEnvProc(gNetscapeFuncs.getJavaEnv);
    #else
      return (*gNetscapeFuncs.getJavaEnv);
    #endif
  }

  jref NPN_GetJavaPeer(NPP instance)
  {
  DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
    #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      return CallNPN_GetJavaPeerProc(gNetscapeFuncs.getJavaPeer, instance);
    #else
      return (*gNetscapeFuncs.getJavaPeer)(instance);
    #endif
  }
#endif

void NPN_InvalidateRect(NPP instance, NPRect *invalidRect)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    CallNPN_InvalidateRectProc(gNetscapeFuncs.invalidaterect, instance,
      invalidRect);
  #else
    (*gNetscapeFuncs.invalidaterect)(instance, invalidRect);
  #endif
}

void NPN_InvalidateRegion(NPP instance, NPRegion invalidRegion)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    CallNPN_InvalidateRegionProc(gNetscapeFuncs.invalidateregion, instance,
      invalidRegion);
  #else
    (*gNetscapeFuncs.invalidateregion)(instance, invalidRegion);
  #endif
}

void NPN_ForceRedraw(NPP instance)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    CallNPN_ForceRedrawProc(gNetscapeFuncs.forceredraw, instance);
  #else
    (*gNetscapeFuncs.forceredraw)(instance);
  #endif
}

void NPN_PushPopupsEnabledState(NPP instance, NPBool enabled)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    CallNPN_PushPopupsEnabledStateProc(gNetscapeFuncs.pushpopupsenabledstate,
      instance, enabled);
  #else
    (*gNetscapeFuncs.pushpopupsenabledstate)(instance, enabled);
  #endif
}

void NPN_PopPopupsEnabledState(NPP instance)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
    CallNPN_PopPopupsEnabledStateProc(gNetscapeFuncs.poppopupsenabledstate,
      instance);
  #else
    (*gNetscapeFuncs.poppopupsenabledstate)(instance);
  #endif
}

NPIdentifier NPN_GetStringIdentifier(const NPUTF8 *name)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  int minor = gNetscapeFuncs.version &0xFF;
  if (minor >= 14)
  {
    #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      return CallNPN_GetStringIdentifierProc(gNetscapeFuncs.getstringidentifier,
        name);
    #else
      return (*gNetscapeFuncs.getstringidentifier)(name);
    #endif
  }
  return NULL;
}

void NPN_GetStringIdentifiers(const NPUTF8 **names, int32_t nameCount,
  NPIdentifier *identifiers)
{
	DebugMessage("=========   PXWANG	CALL BY %s			<<8   >>20				%s		%d",__FUNCTION__,names,nameCount);

  int minor = gNetscapeFuncs.version &0xFF;
  if (minor >= 14)
  {
    #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      CallNPN_GetStringIdentifiersProc(gNetscapeFuncs.getstringidentifiers,  names, nameCount, identifiers);
    #else
      (*gNetscapeFuncs.getstringidentifiers)(names, nameCount, identifiers);
    #endif
  }
}

NPIdentifier NPN_GetIntIdentifier(int32_t intid)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  int minor = gNetscapeFuncs.version &0xFF;
  if (minor >= 14)
  {
    #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      return CallNPN_GetIntIdentifierProc(gNetscapeFuncs.getintidentifier,
        intid);
    #else
      return (*gNetscapeFuncs.getintidentifier)(intid);
    #endif
  }
  return NULL;
}

bool NPN_IdentifierIsString(NPIdentifier identifier)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  int minor = gNetscapeFuncs.version &0xFF;
  if (minor >= 14)
  {
    #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      return CallNPN_IdentifierIsStringProc(gNetscapeFuncs.identifierisstring,
        identifier);
    #else
      return (*gNetscapeFuncs.identifierisstring)(identifier);
    #endif
  }
  return false;
}

NPUTF8 *NPN_UTF8FromIdentifier(NPIdentifier identifier)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  int minor = gNetscapeFuncs.version &0xFF;
  if (minor >= 14)
  {
    #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      return CallNPN_UTF8FromIdentifierProc(gNetscapeFuncs.utf8fromidentifier,
        identifier);
    #else
      return (*gNetscapeFuncs.utf8fromidentifier)(identifier);
    #endif
  }
  return NULL;
}

int32_t NPN_IntFromIdentifier(NPIdentifier identifier)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  int minor = gNetscapeFuncs.version &0xFF;
  if (minor >= 14)
  {
    #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      return CallNPN_IntFromIdentifierProc(gNetscapeFuncs.intfromidentifier,
        identifier);
    #else
      return (*gNetscapeFuncs.intfromidentifier)(identifier);
    #endif
  }
  return 0;
}

NPObject *NPN_CreateObject(NPP npp, NPClass *aClass)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  int minor = gNetscapeFuncs.version &0xFF;
  if (minor >= 14)
  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      return CallNPN_CreateObjectProc(gNetscapeFuncs.createobject, npp, aClass);
  #else
    return (*gNetscapeFuncs.createobject)(npp, aClass);
  #endif
  return NULL;
}

NPObject *NPN_RetainObject(NPObject *obj)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  int minor = gNetscapeFuncs.version &0xFF;
  if (minor >= 14)
  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      return CallNPN_RetainObjectProc(gNetscapeFuncs.retainobject, obj);
  #else
    return (*gNetscapeFuncs.retainobject)(obj);
  #endif
  return NULL;
}

void NPN_ReleaseObject(NPObject *obj)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  int minor = gNetscapeFuncs.version &0xFF;
  if (minor >= 14)
  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      CallNPN_ReleaseObjectProc(gNetscapeFuncs.releaseobject, obj);
  #else
    (*gNetscapeFuncs.releaseobject)(obj);
  #endif
}

bool NPN_Invoke(NPP npp, NPObject *obj, NPIdentifier methodName, const
  NPVariant *args, uint32_t argCount, NPVariant *result)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  int minor = gNetscapeFuncs.version &0xFF;
  if (minor >= 14)
  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      return CallNPN_InvokeProc(gNetscapeFuncs.invoke, npp, obj, methodName,
        args, argCount, result);
  #else
    return (*gNetscapeFuncs.invoke)(npp, obj, methodName, args, argCount,
      result);
  #endif
  return false;
}

bool NPN_InvokeDefault(NPP npp, NPObject *obj, const NPVariant *args, uint32_t
  argCount, NPVariant *result)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  int minor = gNetscapeFuncs.version &0xFF;
  if (minor >= 14)
  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      return CallNPN_InvokeDefaultProc(gNetscapeFuncs.invokeDefault, npp, obj,
        args, argCount, result);
  #else
    return (*gNetscapeFuncs.invokeDefault)(npp, obj, args, argCount, result);
  #endif
  return false;
}

bool NPN_Evaluate(NPP npp, NPObject *obj, NPString *script, NPVariant *result)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  int minor = gNetscapeFuncs.version &0xFF;
  if (minor >= 14)
  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      return CallNPN_EvaluateProc(gNetscapeFuncs.evaluate, npp, obj, script,
        result);
  #else
    return (*gNetscapeFuncs.evaluate)(npp, obj, script, result);
  #endif
  return false;
}

bool NPN_GetProperty(NPP npp, NPObject *obj, NPIdentifier propertyName,
  NPVariant *result)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  int minor = gNetscapeFuncs.version &0xFF;
  if (minor >= 14)
  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      return CallNPN_GetPropertyProc(gNetscapeFuncs.getproperty, npp, obj,
        propertyName, result);
  #else
    return (*gNetscapeFuncs.getproperty)(npp, obj, propertyName, result);
  #endif
  return false;
}

bool NPN_SetProperty(NPP npp, NPObject *obj, NPIdentifier propertyName, const
  NPVariant *value)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  int minor = gNetscapeFuncs.version &0xFF;
  if (minor >= 14)
  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      return CallNPN_SetPropertyProc(gNetscapeFuncs.setproperty, npp, obj,
        propertyName, value);
  #else
    return (*gNetscapeFuncs.setproperty)(npp, obj, propertyName, value);
  #endif
  return false;
}

bool NPN_RemoveProperty(NPP npp, NPObject *obj, NPIdentifier propertyName)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  int minor = gNetscapeFuncs.version &0xFF;
  if (minor >= 14)
  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      return CallNPN_RemovePropertyProc(gNetscapeFuncs.removeproperty, npp, obj,
        propertyName);
  #else
    return (*gNetscapeFuncs.removeproperty)(npp, obj, propertyName);
  #endif
  return false;
}

bool NPN_HasProperty(NPP npp, NPObject *obj, NPIdentifier propertyName)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  int minor = gNetscapeFuncs.version &0xFF;
  if (minor >= 14)
  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      return CallNPN_HasPropertyProc(gNetscapeFuncs.hasproperty, npp, obj,
        propertyName);
  #else
    return (*gNetscapeFuncs.hasproperty)(npp, obj, propertyName);
  #endif
  return false;
}

bool NPN_HasMethod(NPP npp, NPObject *obj, NPIdentifier methodName)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  int minor = gNetscapeFuncs.version &0xFF;
  if (minor >= 14)
  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      return CallNPN_HasMethodProc(gNetscapeFuncs.hasmethod, npp, obj,
        methodName);
  #else
    return (*gNetscapeFuncs.hasmethod)(npp, obj, methodName);
  #endif
  return false;
}

void NPN_ReleaseVariantValue(NPVariant *variant)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  int minor = gNetscapeFuncs.version &0xFF;
  if (minor >= 14)
  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      CallNPN_ReleaseVariantValueProc(gNetscapeFuncs.releasevariantvalue,
        variant);
  #else
    (*gNetscapeFuncs.releasevariantvalue)(variant);
  #endif
}

void NPN_SetException(NPObject *obj, const NPUTF8 *message)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);

  int minor = gNetscapeFuncs.version &0xFF;
  if (minor >= 14)
  #if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
      CallNPN_SetExceptionProc(gNetscapeFuncs.setexception, obj, message);
  #else
    (*gNetscapeFuncs.setexception)(obj, message);
  #endif
}


uint32_t NPN_ScheduleTimer(NPP instance, uint32_t interval, NPBool repeat,
                         void (*timerFunc)(NPP npp, uint32_t timerID))
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
    return (*gNetscapeFuncs.scheduletimer)(instance, interval,repeat != 0, timerFunc);
}

void NPN_UnscheduleTimer(NPP instance, uint32_t timerID)
{
	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
    (*gNetscapeFuncs.unscheduletimer)(instance, timerID);
}





/* function implementations */
NPError Private_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t
  argc, char *argn[], char *argv[], NPSavedData *saved)
{
  NPError ret;
  DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
  gUserAgent = NPN_UserAgent(instance);
  ret = NPP_New(pluginType, instance, mode, argc, argn, argv, saved);
  return ret;
}

NPError Private_Destroy(NPP instance, NPSavedData **save)
{
  DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
  return NPP_Destroy(instance, save);
}

NPError Private_SetWindow(NPP instance, NPWindow *window)
{
  NPError err;
  DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
  err = NPP_SetWindow(instance, window);
  return err;
}

NPError Private_NewStream(NPP instance, NPMIMEType type, NPStream *stream,
  NPBool seekable, uint16_t *stype)
{
  NPError err;
  DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
  err = NPP_NewStream(instance, type, stream, seekable, stype);
  return err;
}

int32_t Private_WriteReady(NPP instance, NPStream *stream)
{
  unsigned int result;
  DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
  result = NPP_WriteReady(instance, stream);
  return result;
}

int32_t Private_Write(NPP instance, NPStream *stream, int32_t offset, int32_t
  len, void *buffer)
{
  unsigned int result;
  DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
  result = NPP_Write(instance, stream, offset, len, buffer);
  return result;
}

void Private_StreamAsFile(NPP instance, NPStream *stream, const char *fname)
{
  DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
  NPP_StreamAsFile(instance, stream, fname);
}


NPError Private_DestroyStream(NPP instance, NPStream *stream, NPError reason)
{
  NPError err;
  DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
  err = NPP_DestroyStream(instance, stream, reason);
  return err;
}

void Private_URLNotify(NPP instance, const char *url, NPReason reason, void
  *notifyData)
{
  DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
  NPP_URLNotify(instance, url, reason, notifyData);
}

void Private_Print(NPP instance, NPPrint *platformPrint)
{
  DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
  NPP_Print(instance, platformPrint);
}

NPError Private_GetValue(NPP instance, NPPVariable variable, void *r_value)
{
  DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
  return NPP_GetValue(instance, variable, r_value);
}

NPError Private_SetValue(NPP instance, NPNVariable variable, void *r_value)
{
  DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
  return NPP_SetValue(instance, variable, r_value);
}

#ifdef OJI
  JRIGlobalRef Private_GetJavaClass(void)
  {
  	DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
    jref clazz = NPP_GetJavaClass();
    if (clazz)
    {
      JRIEnv *env = NPN_GetJavaEnv();
      return JRI_NewGlobalRef(env, clazz);
    }
    return NULL;
  }
#endif

NPError Private_HandleEvent(NPP instance, void *event)
{
 DebugMessage("=========   PXWANG	CALL BY %s",__FUNCTION__);
  return NPP_HandleEvent(instance, event);
}








