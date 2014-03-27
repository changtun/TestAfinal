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

#include <cctype>
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include <cstring>

#include "pvplugin.h"
#include "control/npolibpv.h"


#include "npnapi.h"
#include "log/anpdebug.h"

#include "npsystem.h"

extern JavaVM *gVM;



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

int SubPlugin::getPluginWidth()
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    PvPluginBase *obj = (PvPluginBase *) inst()->pdata;
    return obj->getWindow()->width;
}

int SubPlugin::getPluginHeight()
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    PvPluginBase *obj = (PvPluginBase *) inst()->pdata;
    return obj->getWindow()->height;
}

SurfaceSubPlugin::~SurfaceSubPlugin()
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    setContext(NULL);
}

bool SurfaceSubPlugin::supportsDrawingModel(ANPDrawingModel model)
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    return (model == kSurface_ANPDrawingModel);
}

void SurfaceSubPlugin::setContext(jobject context)
{
    JNIEnv *env = NULL;
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    if (gVM->GetEnv((void **) &env, JNI_VERSION_1_4) == JNI_OK)
    {

        // if one exists then free its global reference
        if (m_context)
        {
            env->DeleteGlobalRef(m_context);
            m_context = NULL;
        }

        // create a new global ref
        if (context)
        {
            context = env->NewGlobalRef(context);
        }

        // set the value
        m_context = context;
    }
}


PvPluginBase::PvPluginBase(NPP instance, NPuint16_t mode):
    i_npmode(mode),
    b_stream(0),
    psz_target(NULL),
    p_scriptClass(NULL),
    p_browser(instance),
    psz_baseURL(NULL)
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    memset(&npwindow, 0, sizeof(NPWindow));
}

static bool boolValue(const char *value)
{
    return (!strcmp(value, "1") || !strcasecmp(value, "true") || !strcasecmp(value, "yes"));
}

NPError PvPluginBase::init(int argc, char *const argn[], char *const argv[])
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    NpSystem_Init();


    NPObject *plugin = NULL;

    if (NPERR_NO_ERROR == NPN_GetValue(p_browser, NPNVWindowNPObject, &plugin))
    {
        static const char docLocHref[] = "document.location.href";
        NPString script;
        NPVariant result;

        script.UTF8Characters = docLocHref;
        script.UTF8Length = sizeof(docLocHref) - 1;

        if(NPN_Evaluate(p_browser, plugin, &script, &result))
        {
            if(NPVARIANT_IS_STRING(result))
            {
                NPString &location = NPVARIANT_TO_STRING(result);

                psz_baseURL = (char *)malloc(location.UTF8Length + 1);
                if(psz_baseURL)
                {
                    strncpy(psz_baseURL, location.UTF8Characters, location.UTF8Length);
                    psz_baseURL[location.UTF8Length] = '\0';
                }
            }
            NPN_ReleaseVariantValue(&result);
        }
        NPN_ReleaseObject(plugin);
    }

    if(psz_target)
    {
        char *psz_absurl = getAbsoluteURL(psz_target);
        psz_target = psz_absurl ? psz_absurl : strdup(psz_target);
    }

    p_scriptClass = RuntimeNPClass < LibpvRootNPObject > ::getClass();

    return NPERR_NO_ERROR;
}

PvPluginBase::~PvPluginBase()
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    free(psz_baseURL);
    free(psz_target);
    //NpDvb_Uninit();
}


void PvPluginBase::setWindow(const NPWindow &window)
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    npwindow = window;
}




/*****************************************************************************
 * PvPluginBase methods
 *****************************************************************************/

char *PvPluginBase::getAbsoluteURL(const char *url)
{
    DebugMessage("=========   PXWANG	CALL BY %s", __FUNCTION__);
    if (NULL != url)
    {
        // check whether URL is already absolute
        const char *end = strchr(url, ':');
        if ((NULL != end) && (end != url))
        {
            // validate protocol header
            const char *start = url;
            char c =  *start;
            if (isalpha(c))
            {
                ++start;
                while (start != end)
                {
                    c =  *start;
                    if (!(isalnum(c) || ('-' == c) || ('+' == c) || ('.' == c) || ('/' ==
                            c)))
                        /* VLC uses / to allow user to specify a demuxer */
                        // not valid protocol header, assume relative URL
                        goto relativeurl;
                    ++start;
                }
                /* we have a protocol header, therefore URL is absolute */
                return strdup(url);
            }
            // not a valid protocol header, assume relative URL
        }

relativeurl:

        if (psz_baseURL)
        {
            size_t baseLen = strlen(psz_baseURL);
            char *href = (char *)malloc(baseLen + strlen(url) + 1);
            if (href)
            {
                /* prepend base URL */
                memcpy(href, psz_baseURL, baseLen + 1);

                /*
                 ** relative url could be empty,
                 ** in which case return base URL
                 */
                if ('\0' ==  *url)
                    return href;

                /*
                 ** locate pathname part of base URL
                 */

                /* skip over protocol part  */
                char *pathstart = strchr(href, ':');
                char *pathend = href + baseLen;
                if (pathstart)
                {
                    if ('/' == *(++pathstart))
                    {
                        if ('/' == *(++pathstart))
                        {
                            ++pathstart;
                        }
                    }
                    /* skip over host part */
                    pathstart = strchr(pathstart, '/');
                    if (!pathstart)
                    {
                        // no path, add a / past end of url (over '\0')
                        pathstart = pathend;
                        *pathstart = '/';
                    }
                }
                else
                {
                    /* baseURL is just a UNIX path */
                    if ('/' !=  *href)
                    {
                        /* baseURL is not an absolute path */
                        free(href);
                        return NULL;
                    }
                    pathstart = href;
                }

                /* relative URL made of an absolute path ? */
                if ('/' ==  *url)
                {
                    /* replace path completely */
                    strcpy(pathstart, url);
                    return href;
                }

                /* find last path component and replace it */
                while ('/' !=  *pathend)
                    --pathend;

                /*
                 ** if relative url path starts with one or more '../',
                 ** factor them out of href so that we return a
                 ** normalized URL
                 */
                while (pathend != pathstart)
                {
                    const char *p = url;
                    if ('.' !=  *p)
                        break;
                    ++p;
                    if ('\0' ==  *p)
                    {
                        /* relative url is just '.' */
                        url = p;
                        break;
                    }
                    if ('/' ==  *p)
                    {
                        /* relative url starts with './' */
                        url = ++p;
                        continue;
                    }
                    if ('.' !=  *p)
                        break;
                    ++p;
                    if ('\0' ==  *p)
                    {
                        /* relative url is '..' */
                    }
                    else
                    {
                        if ('/' !=  *p)
                            break;
                        /* relative url starts with '../' */
                        ++p;
                    }
                    url = p;
                    do
                    {
                        --pathend;
                    }
                    while ('/' !=  *pathend);
                }
                /* skip over '/' separator */
                ++pathend;
                /* concatenate remaining base URL and relative URL */
                strcpy(pathend, url)
                ;
            }
            return href;
        }
    }
    return NULL;
}

