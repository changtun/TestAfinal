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

#include "npnapi.h"


#include "pvplugin.h"

#include "AnimationPlugin.h"
#include "AudioPlugin.h"
#include "BackgroundPlugin.h"
#include "FormPlugin.h"
#include "NavigationPlugin.h"
#include "PaintPlugin.h"
#include "VideoPlugin.h"


#include "pvshell.h"
#include "log/anpdebug.h"






extern ANPLogInterfaceV0 gLogI;
extern ANPSystemInterfaceV0 gSystemI;




/*******************************************************/
/*               Private Defines and Macros			   */
/*******************************************************/
#define PLUGIN_NAME         	"PvWare Web Plugin"

#define PLUGIN_DESCRIPTION 		\
								"Version %s, copyright 1996-2011 PBI-SoftWare and Authors" \
								"<br />" \
								"<a href=\"http://www.pbi-china.com\">http://www.pbi-china.com</a>"



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
static char mimetype[] =	"application/pvware:Pvware:Pvware plugin mimetype is application/pvware";

/*******************************************************/
/*               Private Function prototypes		   */
/*******************************************************/

/*******************************************************/
/*               Functions							   */
/*******************************************************/
NPError NPP_Initialize(void)
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    return NPERR_NO_ERROR;
}

void NPP_Shutdown(void)
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
}

NPError NPP_New(NPMIMEType, NPP instance, NPuint16_t mode, NPint16_t argc, char
                *argn[], char *argv[], NPSavedData *)
{
    NPError status;
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    if (instance == NULL)
    {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    PvPluginBase *p_plugin = new PvPluginBase(instance, mode);
    if (NULL == p_plugin)
    {
        return NPERR_OUT_OF_MEMORY_ERROR;
    }

    status = p_plugin->init(argc, argn, argv);
    if (NPERR_NO_ERROR == status)
    {
        instance->pdata = reinterpret_cast < void * > (p_plugin);
        p_plugin->pluginType = 0;

        ANPDrawingModel model = kBitmap_ANPDrawingModel;
        for (int i = 0; i < argc; i++)
        {
            if (!strcmp(argn[i], "DrawingModel"))
            {
                if (!strcmp(argv[i], "Bitmap"))
                {
                    model = kBitmap_ANPDrawingModel;
                }
                else if (!strcmp(argv[i], "Surface"))
                {
                    model = kSurface_ANPDrawingModel;
                }
                else if (!strcmp(argv[i], "OpenGL"))
                {
                    model = kOpenGL_ANPDrawingModel;
                }
                gLogI.log(kDebug_ANPLogType, "------ %p DrawingModel is %d", instance, model);
                break;
            }
        }
        NPError err = NPN_SetValue(instance, kRequestDrawingModel_ANPSetValue, reinterpret_cast<void *>(model));
        if (err)
        {
            gLogI.log(kError_ANPLogType, "request model %d err %d", model, err);
            return err;
        }

        const char *path = gSystemI.getApplicationDataDirectory();
        if (path)
        {
            gLogI.log(kDebug_ANPLogType, "Application data dir is %s", path);
        }
        else
        {
            gLogI.log(kError_ANPLogType, "Can't find Application data dir");
        }

        // select the pluginType
        for (int i = 0; i < argc; i++)
        {
            if (!strcmp(argn[i], "PluginType"))
            {
                if (!strcmp(argv[i], "Animation"))
                {
                    p_plugin->pluginType = kAnimation_PluginType;
                    p_plugin->activePlugin = new BallAnimation(instance);
                }
                else if (!strcmp(argv[i], "Audio"))
                {
                    p_plugin->pluginType = kAudio_PluginType;
                    p_plugin->activePlugin = new AudioPlugin(instance);
                }
                else if (!strcmp(argv[i], "Background"))
                {
                    p_plugin->pluginType = kBackground_PluginType;
                    p_plugin->activePlugin = new BackgroundPlugin(instance);
                }
                else if (!strcmp(argv[i], "Form"))
                {
                    p_plugin->pluginType = kForm_PluginType;
                    p_plugin->activePlugin = new FormPlugin(instance);
                }
                else if (!strcmp(argv[i], "Navigation"))
                {
                    p_plugin->pluginType = kNavigation_PluginType;
                    p_plugin->activePlugin = new NavigationPlugin(instance);
                }
                else if (!strcmp(argv[i], "Paint"))
                {
                    p_plugin->pluginType = kPaint_PluginType;
                    p_plugin->activePlugin = new PaintPlugin(instance);
                }
                else if (!strcmp(argv[i], "Video"))
                {
                    p_plugin->pluginType = kVideo_PluginType;
                    p_plugin->activePlugin = new VideoPlugin(instance);
                }
                else
                {
                    gLogI.log(kError_ANPLogType, "PluginType %s unknown!", argv[i]);
                }
                break;
            }
        }

        // if no pluginType is specified then default to Animation
        if (!p_plugin->pluginType)
        {
            gLogI.log(kError_ANPLogType, "------ %p No PluginType attribute was found", instance);
            p_plugin->pluginType = kAnimation_PluginType;
            p_plugin->activePlugin = new BallAnimation(instance);
        }

        gLogI.log(kDebug_ANPLogType, "------ %p PluginType is %d", instance, p_plugin->pluginType);

        // check to ensure the pluginType supports the model
        if (!p_plugin->activePlugin->supportsDrawingModel(model))
        {
            gLogI.log(kError_ANPLogType, "------ %p Unsupported DrawingModel (%d)", instance, model);
            return NPERR_GENERIC_ERROR;
        }

        // if the plugin uses the surface drawing model then set the java context
        if (model == kSurface_ANPDrawingModel || model == kOpenGL_ANPDrawingModel)
        {
            SurfaceSubPlugin *surfacePlugin = static_cast<SurfaceSubPlugin *>(p_plugin->activePlugin);

            jobject context;
            NPError err = NPN_GetValue(instance, kJavaContext_ANPGetValue, static_cast<void *>(&context));
            if (err)
            {
                gLogI.log(kError_ANPLogType, "request context err: %d", err);
                return err;
            }

            surfacePlugin->setContext(context);
        }

    }
    else
    {
        delete p_plugin;
    }

    return status;
}

NPError NPP_Destroy(NPP instance, NPSavedData **save)
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    if (NULL == instance)
        return NPERR_INVALID_INSTANCE_ERROR;

    PvPluginBase *p_plugin = reinterpret_cast < PvPluginBase * > (instance->pdata);
    if (NULL == p_plugin)
        return NPERR_NO_ERROR;

    instance->pdata = NULL;
    delete p_plugin;

    return NPERR_NO_ERROR;
}



NPError NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
    static char psz_name[] = PLUGIN_NAME;
    static char psz_desc[1000];

    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    /* plugin class variables */
    switch (variable)
    {
    case NPPVpluginNameString:
        *((char **)value) = psz_name;
        return NPERR_NO_ERROR;
    case NPPVpluginDescriptionString:
        snprintf(psz_desc, sizeof(psz_desc), PLUGIN_DESCRIPTION, "pvware-1.0.0");
        *((char **)value) = psz_desc;
        return NPERR_NO_ERROR;
    default:
        ;
    }

    if (instance == NULL)
    {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    PvPluginBase *p_plugin = reinterpret_cast < PvPluginBase * > (instance->pdata)
                             ;
    if (NULL == p_plugin)
    {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    switch (variable)
    {
    case NPPVpluginScriptableNPObject:
    {
        NPClass *scriptClass = p_plugin->getScriptClass();
        if (scriptClass)
        {
            *(NPObject **)value = NPN_CreateObject(instance, scriptClass);
            return NPERR_NO_ERROR;
        }
        break;
    }

    case kJavaSurface_ANPGetValue:
    {
        if (p_plugin && p_plugin->activePlugin)
        {
            if(p_plugin->activePlugin->supportsDrawingModel(kSurface_ANPDrawingModel) || p_plugin->activePlugin->supportsDrawingModel(kOpenGL_ANPDrawingModel))
            {
                SurfaceSubPlugin *plugin = static_cast<SurfaceSubPlugin *>(p_plugin->activePlugin);
                jobject *surface = static_cast<jobject *>(value);
                *surface = plugin->getSurface();
                return NPERR_NO_ERROR;
            }
            else
            {
                gLogI.log(kError_ANPLogType, "-- %p Tried to retrieve surface for non-surface plugin", instance);
            }
        }
    }
    default:
        ;
    }
    return NPERR_GENERIC_ERROR;
}

NPError NPP_SetValue(NPP, NPNVariable, void *)
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    return NPERR_GENERIC_ERROR;
}

NPError NPP_SetWindow(NPP instance, NPWindow *window)
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    if (NULL == instance)
    {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    PvPluginBase *p_plugin = reinterpret_cast < PvPluginBase * > (instance->pdata)
                             ;
    if (NULL == p_plugin)
    {
        return NPERR_NO_ERROR;
    }
    else
    {
        p_plugin->setWindow(*window);
        NPN_InvalidateRect(instance, NULL);
        return NPERR_NO_ERROR;
    }

    return NPERR_NO_ERROR;
}


NPError NPP_NewStream(NPP instance, NPMIMEType, NPStream *stream, NPBool,
                      NPuint16_t *stype)
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    *stype = NP_ASFILEONLY;
    return NPERR_NO_ERROR;
}

int32_t NPP_WriteReady(NPP instance, NPStream *stream)
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    return 0;
}

int32_t NPP_Write(NPP instance, NPStream *stream, NPint32_t offset, NPint32_t len,
                  void *buffer)
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    return 0;
}

NPError NPP_DestroyStream(NPP instance, NPStream *stream, NPError reason)
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    return NPERR_NO_ERROR;
}

void NPP_StreamAsFile(NPP instance, NPStream *stream, const char *)
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
}

void NPP_URLNotify(NPP instance, const char *url, NPReason reason, void
                   *notifyData)
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);

}

void NPP_Print(NPP instance, NPPrint *printInfo)
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
}

int16_t NPP_HandleEvent(NPP instance, void *event)
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    PvPluginBase *p_plugin = reinterpret_cast < PvPluginBase * > (instance->pdata);
    const ANPEvent *evt = reinterpret_cast<const ANPEvent *>(event);

    if(!p_plugin->activePlugin)
    {
        gLogI.log(kError_ANPLogType, "the active plugin is null.");
        return 0; // unknown or unhandled event
    }
    else
    {
        return p_plugin->activePlugin->handleEvent(evt);
    }

    return NPERR_GENERIC_ERROR;
}

const char *NPP_GetMIMEDescription(void)
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    return mimetype;
}

#ifdef OJI
jref NPP_GetJavaClass(void)
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    return NULL;
}
#endif

