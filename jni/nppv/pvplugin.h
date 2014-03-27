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
#ifndef __PV_PLUGIN_H
#define __PV_PLUGIN_H

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
#include "ANPSurface_npapi.h"
#include "ANPSystem_npapi.h"

#include "control/nporuntime.h"
#include <jni.h>

/*******************************************************/
/*              Exported Defines and Macros            */
/*******************************************************/
#ifndef __MAX
#define __MAX(a, b)	 ( ((a) > (b)) ? (a) : (b) )
#endif
#ifndef __MIN
#define __MIN(a, b)	 ( ((a) < (b)) ? (a) : (b) )
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/*******************************************************/
/*              Exported Types			                */
/*******************************************************/
#if (((NP_VERSION_MAJOR << 8) + NP_VERSION_MINOR) < 20)
typedef uint16 NPuint16_t;
typedef int16 NPint16_t;
typedef int32 NPint32_t;
#else
typedef uint16_t NPuint16_t;
typedef int16_t NPint16_t;
typedef int32_t NPint32_t;
#endif


enum CustomEventTypes
{
    kSurfaceCreated_CustomEvent     = 0,
    kSurfaceChanged_CustomEvent     = 1,
    kSurfaceDestroyed_CustomEvent   = 2,
};
typedef int32_t CustomEventType;

class SubPlugin
{
public:
    SubPlugin(NPP inst) : m_inst(inst) {}
    virtual ~SubPlugin() {}
    virtual int16_t handleEvent(const ANPEvent *evt) = 0;
    virtual bool supportsDrawingModel(ANPDrawingModel) = 0;

    int getPluginWidth();
    int getPluginHeight();

    NPP inst() const
    {
        return m_inst;
    }

private:
    NPP m_inst;
};

class SurfaceSubPlugin : public SubPlugin
{
public:
    SurfaceSubPlugin(NPP inst) : SubPlugin(inst)
    {
        m_context = NULL;
    }
    virtual ~SurfaceSubPlugin();
    virtual jobject getSurface() = 0;
    virtual bool supportsDrawingModel(ANPDrawingModel);

    void setContext(jobject context);

    jobject m_context;
};

enum PluginTypes
{
    kAnimation_PluginType  = 1,
    kAudio_PluginType      = 2,
    kBackground_PluginType = 3,
    kForm_PluginType       = 4,
    kText_PluginType       = 5,
    kPaint_PluginType      = 6,
    kVideo_PluginType      = 7,
    kNavigation_PluginType = 8,
};
typedef uint32_t PluginType;


class PvPluginBase
{
protected:

public:
    PvPluginBase(NPP, NPuint16_t);
    virtual ~PvPluginBase();
    NPError init(int argc, char *const argn[], char *const argv[]);
    NPP getBrowser()
    {
        return p_browser;
    };
    char *getAbsoluteURL(const char *url);
    NPWindow *getWindow()
    {
        return &npwindow;
    };
    virtual void setWindow(const NPWindow &window);

    NPClass *getScriptClass()
    {
        return p_scriptClass;
    };

    NPuint16_t i_npmode; /* either NP_EMBED or NP_FULL */

    /* plugin properties */
    int b_stream;
    char *psz_target;

    PluginType pluginType;
    SubPlugin *activePlugin;



protected:
    NPClass *p_scriptClass;
    NPP p_browser;
    char *psz_baseURL;
    NPWindow npwindow;


};


/*******************************************************/
/*              Exported Variables		                */
/*******************************************************/

/*******************************************************/
/*              External				                   */
/*******************************************************/

/*******************************************************/
/*              Exported Functions		                */
/*******************************************************/

#endif  /* #ifndef __STAPI_H */
/* End of stapi.h  --------------------------------------------------------- */

